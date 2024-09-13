

#include "Unzipper.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"
#include "Async/Async.h"
#include "Async/TaskGraphInterfaces.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"

THIRD_PARTY_INCLUDES_START
#pragma warning( push )
#pragma warning( disable : 4668)
#	if PLATFORM_WINDOWS
#		include "Windows/AllowWindowsPlatformTypes.h"
#		include "ThirdParty/iowin32.h"
#		include "Windows/HideWindowsPlatformTypes.h"
#	endif
#	include "ThirdParty/unzip.h"
#pragma warning( pop )
THIRD_PARTY_INCLUDES_END

UUnzipper* UUnzipper::CreateUnzipper()
{
	return NewObject<UUnzipper>();
}

UUnzipper::UUnzipper()
	: BufferSize(8192)
{}

void UUnzipper::SetBufferSize(const int64& NewBufferSize)
{
	ensureMsgf(NewBufferSize > 0, TEXT("Buffer Size must be bigger than 0."));
	BufferSize = NewBufferSize;
}

void UUnzipper::SetArchive(FString Path)
{
	ArchivePath = FPaths::ConvertRelativePathToFull(MoveTemp(Path));
}

bool UUnzipper::IsValid() const
{
	return FPaths::FileExists(ArchivePath);
}

bool UUnzipper::UnzipAll(const FString& TargetLocation, const FString& Password, const bool bForce)
{
	const auto Callback = [](void* Data, const int64 FileIndex, const int64 TotalFileCount, const FString& ArchiveLocation, const FString& DiskLocation) -> void
	{
		UUnzipper* const This = (UUnzipper*)Data;
		This->OnFileUnzipCompleted(FileIndex, TotalFileCount, ArchiveLocation, DiskLocation);
	};
	
	const int64 bUnzipSuccess = UnzipAllInternal(TargetLocation, ArchivePath, Password, bForce, BufferSize, Callback, this);

	OnUnzipCompleted(bUnzipSuccess, TargetLocation, (bool)bUnzipSuccess);

	return (bool)bUnzipSuccess;
}

void UUnzipper::UnzipAllAsync(FString TargetLocation, FString Password, const bool bForce)
{
	if (!IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("File \"%s\" doesn't exist."), *ArchivePath);
		OnUnzipCompleted(0, TargetLocation, false);
		return;
	}

	AddToRoot();

	FString Archive = FPaths::ConvertRelativePathToFull(ArchivePath);
	const int64 Buffer = BufferSize;

	const bool bCallFileCallback = OnFileUnzippedDynamicEvent.IsBound() || OnFileUnzippedEvent.IsBound();

	TWeakObjectPtr<UUnzipper> This = TWeakObjectPtr<UUnzipper>(this);
	AsyncTask(ENamedThreads::AnyThread, [This, TargetLocation = MoveTemp(TargetLocation), 
		Archive = MoveTemp(Archive), Password= MoveTemp(Password), bForce, Buffer, bCallFileCallback]() -> void
	{
		auto FileUnzipedCallback = [](void* Data, const int64 FileIndex, const int64 TotalFileCount, const FString& ArchiveLocation, const FString& DiskLocation) -> void
		{
			check(!IsInGameThread());
			check(Data != nullptr);

			TWeakObjectPtr<UUnzipper>& Self = *(TWeakObjectPtr<UUnzipper>*)Data;

			AsyncTask(ENamedThreads::GameThread, [Self, ArchiveLocation, DiskLocation, TotalFileCount, FileIndex]() -> void
			{
				if (Self.IsValid())
				{
					Self->OnFileUnzipCompleted(FileIndex, TotalFileCount, ArchiveLocation, DiskLocation);
				}
			});
		};

		const int64 bUnzipSuccess =
			bCallFileCallback ?
			UnzipAllInternal(TargetLocation, Archive, Password, bForce, Buffer, FileUnzipedCallback, (void*)&This) :
			UnzipAllInternal(TargetLocation, Archive, Password, bForce, Buffer, nullptr, nullptr);
		
		AsyncTask(ENamedThreads::GameThread, [This, bUnzipSuccess, TargetLocation]() -> void
		{
			if (This.IsValid())
			{
				This->OnUnzipCompleted(bUnzipSuccess, TargetLocation, (bool)bUnzipSuccess);
				This->RemoveFromRoot();
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Unzipper got garbage collected while unzipping."));
			}
		});
	});
}

int64 UUnzipper::UnzipAllInternal(const FString& TargetLocation, const FString& ArchivePath, const FString& Password, const bool bForce, const int64 BufferSize, FOnFileUnzippedPtr Callback, void* CallbackData)
{	
#if PLATFORM_WINDOWS
	zlib_filefunc64_def Ffunc;
	fill_win32_filefunc64W(&Ffunc);
	unzFile File = unzOpen2_64(TCHAR_TO_WCHAR(*ArchivePath), &Ffunc);
#elif PLATFORM_ANDROID || PLATFORM_IOS
	unzFile File = unzOpen64(TCHAR_TO_UTF8(*ArchivePath));
	if (!File)
	{
		File = unzOpen64(TCHAR_TO_WCHAR(*ArchivePath));
	}
#else
	unzFile File = unzOpen64(TCHAR_TO_WCHAR(*ArchivePath));
	if (!File)
	{
		File = unzOpen64(TCHAR_TO_UTF8(*ArchivePath));
	}
#endif

	if (File == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to open file \"%s\": unzOpen64 returned NULL. Absolute path is \"%s\". UTF8 path is \"%s\" (an invalid path here means unsupported characters)."), 
			*ArchivePath, *FPaths::ConvertRelativePathToFull(ArchivePath), UTF8_TO_TCHAR(TCHAR_TO_UTF8(*ArchivePath)));
		return 0;
	}

	unz_global_info64 GlobalInfo;

	{
		const int32 Error = unzGetGlobalInfo64(File, &GlobalInfo);

		if (Error != UNZ_OK)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to get file \"%s\" info. Error code: %d."), *ArchivePath, Error);
			return 0;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Archive contains %d file(s)."), GlobalInfo.number_entry);

	const int64 FileCount = (int64)GlobalInfo.number_entry;

	void* Buffer = FMemory::Malloc(BufferSize);
	
	const bool bUnzipSuccess = ([&]() -> bool
	{
		for (int64 i = 0; i < FileCount; ++i)
		{
			if (!UnzipCurrentFile(i + 1, FileCount, TargetLocation, Password, bForce, File, Buffer, BufferSize, Callback, CallbackData))
			{
				return false;
			}

			if (i < FileCount - 1)
			{
				const int32 Error = unzGoToNextFile(File);
				if (Error != UNZ_OK)
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to read archive's next file. Code: %d."), Error);
					return false;
				}
			}
		}

		return true;
	})();

	FMemory::Free(Buffer);

	unzClose(File);

	if (bUnzipSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("Zip file \"%s\" extracted to \"%s\""), *ArchivePath, *TargetLocation);
	}
	else 
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to unzip file \"%s\"."), *ArchivePath);
		return 0;
	}

	return FileCount;
}

bool UUnzipper::UnzipCurrentFile(const int64 FileIndex, const int64 TotalFileCount, const FString& TargetLocation, const FString& ArchivePassword, const bool bForceUnzip, void* const File, void* Buffer, const int64 InBufferSize, FOnFileUnzippedPtr Callback, void* CallbackData)
{
	unz_file_info64 FileInfo;

	FString FileName;

	{
		constexpr int32 FileMaxSize = 512;
		char RawFileName[FileMaxSize];

		const int32 Error = unzGetCurrentFileInfo64(File, &FileInfo, RawFileName, FileMaxSize, nullptr, 0, nullptr, 0);
		if (Error != UNZ_OK)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to get current file info. Code: %s."), Error);
			return false;
		}

		FileName = UTF8_TO_TCHAR(RawFileName);
	}

	const FString CleanFileName = FPaths::GetCleanFilename(FileName);
	
	if (CleanFileName.IsEmpty())
	{
		FPlatformFileManager::Get().GetPlatformFile().CreateDirectory(*(TargetLocation / FileName));
		return true;
	}

	const FString WriteLocation = TargetLocation / FileName;

	{
		const int32 Error = ArchivePassword.IsEmpty() ? unzOpenCurrentFile(File) : unzOpenCurrentFilePassword(File, TCHAR_TO_UTF8(*ArchivePassword));
		if (Error != UNZ_OK)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to open file \"%s\", with password: %d. Code: %d."), *FileName, ArchivePassword.IsEmpty() ? 0 : 1, Error);
			return false;
		}
	}

	if (FPaths::FileExists(WriteLocation))
	{
		if (bForceUnzip)
		{
			FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*WriteLocation);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("File \"%s\" already exists. Skipping extraction. Use bForce = true to overwrite existing files."), *WriteLocation);
			return true;
		}
	}

	int32 Error = 1;
	{
		TUniquePtr<FArchive> Ar = TUniquePtr<FArchive>(IFileManager::Get().CreateFileWriter(*WriteLocation));

		if (!Ar)
		{
			Error = -254;
			UE_LOG(LogTemp, Error, TEXT("Failed to create archive to write to \"%s\""), *WriteLocation);
		}

		while (Error > 0)
		{
			Error = unzReadCurrentFile(File, Buffer, InBufferSize);

			if (Error < 0)
			{
				break;
			}

			if (Error > 0)
			{
				Ar->Serialize(Buffer, Error);
			}
		}

		if (Ar)
		{
			Ar->Close();
		}

		if (Error != UNZ_OK)
		{
			UE_LOG(LogTemp, Warning, TEXT("Error while reading file \"%s\". Code: %d."), *FileName, Error);
		}
	}

	{
		const int32 CloseError = unzCloseCurrentFile(File);
		if (CloseError != UNZ_OK)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to close file \"%s\". Code: %d."), *FileName, CloseError);
		}
	}

	if (Callback)
	{
		Callback(CallbackData, FileIndex, TotalFileCount, FileName, WriteLocation);
	}

	return Error == UNZ_OK;
}

void UUnzipper::OnUnzipCompleted(const int64 TotalFileCount, const FString& DirectoryPath, const bool bSuccess)
{
	check(IsInGameThread());

	if (IsRooted())
	{
		RemoveFromRoot();
	}

	OnFilesUnzippedEvent.Broadcast(bSuccess, DirectoryPath, TotalFileCount);
	OnFilesUnzippedDynamicEvent.Broadcast(bSuccess, DirectoryPath, TotalFileCount);
}

void UUnzipper::OnFileUnzipCompleted(const int64 FileIndex, const int64 TotalFileCount, const FString& ArchiveLocation, const FString& DiskLocation)
{
	check(IsInGameThread());

	OnFileUnzippedEvent.Broadcast(ArchiveLocation, DiskLocation, FileIndex, TotalFileCount);
	OnFileUnzippedDynamicEvent.Broadcast(ArchiveLocation, DiskLocation, FileIndex, TotalFileCount);
}

FOnFileUnzipped& UUnzipper::OnFileUnzipped()
{
	return OnFileUnzippedEvent;
}

FOnFilesUnzipped& UUnzipper::OnFilesUnzipped()
{
	return OnFilesUnzippedEvent;
}
