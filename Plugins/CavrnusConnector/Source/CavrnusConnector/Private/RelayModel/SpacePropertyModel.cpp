// Copyright(c) Cavrnus. All rights reserved.
#include "RelayModel/SpacePropertyModel.h"
#include <TextureResource.h>

namespace Cavrnus
{
	SpacePropertyModel::SpacePropertyModel() 
	{
		ChatModel = new SpaceChatModel();
	}

	SpacePropertyModel::~SpacePropertyModel()
	{
		for (auto& arrived : LocalUserArrivedCallbacks)
		{
			delete arrived;
		}
		LocalUserArrivedCallbacks.Empty();

		for (auto& added : UserAddedBindings)
		{
			delete added;
		}
		UserAddedBindings.Empty();

		for (auto& removed : UserRemovedBindings)
		{
			delete removed;
		}
		UserRemovedBindings.Empty();
	}

	void SpacePropertyModel::UpdateServerPropVal(FAbsolutePropertyId fullPropertyId, FPropertyValue value)
	{
		if (!CurrServerPropValues.Contains(fullPropertyId))
		{
			CurrServerPropValues.FindOrAdd(fullPropertyId);
		}
		
		CurrServerPropValues[fullPropertyId] = value;
		TryExecPropBindings(fullPropertyId);
	}

	int SpacePropertyModel::SetLocalPropVal(FAbsolutePropertyId fullPropertyId, FPropertyValue value)
	{
		//We treat this like a has, but use a dict cuz stupid string comparison stuff
		if (CurrPropReadonlyMetadata.Contains(fullPropertyId)) 
		{
			return -1;
		}

		if (!CurrLocalPropValues.Contains(fullPropertyId))
		{
			CurrLocalPropValues.FindOrAdd(fullPropertyId);
			LocalPropValidationIds.FindOrAdd(fullPropertyId);
		}

		CurrLocalPropValues[fullPropertyId] = value;
		TryExecPropBindings(fullPropertyId);

		validationIdIncrementer++;
		LocalPropValidationIds[fullPropertyId] = validationIdIncrementer;
		return validationIdIncrementer;
	}

	FPropertyValue SpacePropertyModel::GetCurrentPropValue(FAbsolutePropertyId fullPropertyId)
	{
		//Returns an invalid value
		if (!PropValueExists(fullPropertyId))
			return FPropertyValue();

		if (!CurrServerPropValues.Contains(fullPropertyId))
			return CurrLocalPropValues[fullPropertyId];

		if (!CurrLocalPropValues.Contains(fullPropertyId))
			return CurrServerPropValues[fullPropertyId];

		if (CurrServerPropValues[fullPropertyId].Priority > CurrLocalPropValues[fullPropertyId].Priority)
			return CurrServerPropValues[fullPropertyId];

		return CurrLocalPropValues[fullPropertyId];
	}

	void SpacePropertyModel::InvalidateLocalPropValue(FAbsolutePropertyId fullPropertyId, int propValidationId)
	{
		if (LocalPropValidationIds.Contains(fullPropertyId) && LocalPropValidationIds[fullPropertyId] == propValidationId)
		{
			LocalPropValidationIds.Remove(fullPropertyId);

			const FPropertyValue& oldPropVal = GetCurrentPropValue(fullPropertyId);

			CurrLocalPropValues.Remove(fullPropertyId);
			
			const FPropertyValue& newPropVal = GetCurrentPropValue(fullPropertyId);

			bool changed = !(newPropVal == oldPropVal);
			if (changed)
				TryExecPropBindings(fullPropertyId);
		}
	}

	void SpacePropertyModel::UpdatePropMetadata(FAbsolutePropertyId fullPropertyId, bool isReadonly)
	{
		if (!isReadonly && CurrPropReadonlyMetadata.Contains(fullPropertyId)) 
		{
			CurrPropReadonlyMetadata.Remove(fullPropertyId);
		}
		else if (isReadonly && !CurrPropReadonlyMetadata.Contains(fullPropertyId)) 
		{
			CurrPropReadonlyMetadata.Add(fullPropertyId, true);
		}

		if (isReadonly && LocalPropValidationIds.Contains(fullPropertyId)) 
		{
			LocalPropValidationIds.Remove(fullPropertyId);

			const FPropertyValue& oldPropVal = GetCurrentPropValue(fullPropertyId);

			CurrLocalPropValues.Remove(fullPropertyId);

			const FPropertyValue& newPropVal = GetCurrentPropValue(fullPropertyId);

			bool changed = !(newPropVal == oldPropVal);
			if (changed)
				TryExecPropBindings(fullPropertyId);
		}
	}

	void SpacePropertyModel::TryExecPropBindings(FAbsolutePropertyId fullPropertyId)
	{
		const FPropertyValue& activePropVal = GetCurrentPropValue(fullPropertyId);

		//This shouldn't happen, but could potentially when handling oddly ordered stuff.
		//We'll ignore it since it shouldn't be the case for long.
		if (activePropVal.PropType == FPropertyValue::PropertyType::Unset)
			return;

		if (PropBindings.Contains(fullPropertyId))
		{
			auto bindings = PropBindings[fullPropertyId];
			for (int i = 0; i < bindings.Num(); i++)
				(*bindings[i])(activePropVal, fullPropertyId.ContainerName, fullPropertyId.PropValueId);
		}
	}

	bool SpacePropertyModel::PropValueExists(FAbsolutePropertyId fullPropertyId)
	{
		return CurrLocalPropValues.Contains(fullPropertyId) || CurrServerPropValues.Contains(fullPropertyId);
	}

	UCavrnusBinding* SpacePropertyModel::BindProperty(FAbsolutePropertyId fullPropertyId, CavrnusPropertyFunction callback)
	{
		CavrnusPropertyFunction* cb = new CavrnusPropertyFunction(callback);
		PropBindings.FindOrAdd(fullPropertyId);
		PropBindings[fullPropertyId].Add(cb);

		callback(GetCurrentPropValue(fullPropertyId), fullPropertyId.ContainerName, fullPropertyId.PropValueId);

		UCavrnusBinding* binding;
		binding = NewObject<UCavrnusBinding>();
		binding->SetupUnbind([this, fullPropertyId, cb]()
		{
			if (!PropBindings.Contains(fullPropertyId))
				return;
			PropBindings[fullPropertyId].Remove(cb);
			if (PropBindings[fullPropertyId].IsEmpty())
				PropBindings.Remove(fullPropertyId);
		});

		return binding;
	}

	UCavrnusBinding* SpacePropertyModel::BindUserVideoTexture(const FCavrnusUser& User, VideoFrameUpdateFunction Callback)
	{
		FString UserConnectionId = User.UserConnectionId;

		if (CurrSpaceUsersVideoTextures.Contains(UserConnectionId))
			Callback(CurrSpaceUsersVideoTextures[UserConnectionId]);

		VideoFrameUpdateFunction* cb = new VideoFrameUpdateFunction(Callback);
		UserVideoFrameBindings.FindOrAdd(UserConnectionId);
		UserVideoFrameBindings[UserConnectionId].Add(cb);


		UCavrnusBinding* binding;
		binding = NewObject<UCavrnusBinding>();
		binding->SetupUnbind([this, UserConnectionId, cb]()
		{
			if (!UserVideoFrameBindings.Contains(UserConnectionId))
				return;
			UserVideoFrameBindings[UserConnectionId].Remove(cb);
			if (UserVideoFrameBindings[UserConnectionId].IsEmpty())
				UserVideoFrameBindings.Remove(UserConnectionId);
		});

		return binding;
	}
	
	Cavrnus::FPropertyValue SpacePropertyModel::GetPropValue(FAbsolutePropertyId fullPropertyId)
	{
		if (PropValueExists(fullPropertyId))
			return GetCurrentPropValue(fullPropertyId);
		return Cavrnus::FPropertyValue();
	}

	void SpacePropertyModel::SetIsDefined(FAbsolutePropertyId fullPropertyId)
	{
		CurrDefinedProps.Add(fullPropertyId);
	}


	void SpacePropertyModel::AddSpaceUser(const FCavrnusUser& user)
	{
		if (user.IsLocalUser)
		{
			LocalUser = user;
			hasLocalUser = true;

			for (int i = 0; i < LocalUserArrivedCallbacks.Num(); i++)
			{
				(*LocalUserArrivedCallbacks[i])(user);
				delete LocalUserArrivedCallbacks[i];
			}
			LocalUserArrivedCallbacks.Empty();
		}

		CurrSpaceUsers.Add(user.UserConnectionId, user);
		CurrSpaceUsersVideoTextures.Add(user.UserConnectionId, nullptr);
		for (int i = 0; i < UserAddedBindings.Num(); i++)
			(*UserAddedBindings[i])(user);
	}

	void SpacePropertyModel::RemoveSpaceUser(FString userId)
	{
		if (CurrSpaceUsers.Contains(userId))
		{
			FCavrnusUser removedUser = CurrSpaceUsers[userId];
			CurrSpaceUsers.Remove(userId);

			for (int i = 0; i < UserRemovedBindings.Num(); i++)
				(*UserRemovedBindings[i])(removedUser);
		}
	}

	void SpacePropertyModel::UpdateUserVideoTexture(FString userId, int ResX, int ResY, const TArray<uint8>& ByteArray)
	{		
		if (!CurrSpaceUsersVideoTextures.Contains(userId))
			return;

		UTexture2D* Tex = CurrSpaceUsersVideoTextures[userId];

		bool TextureCreated = false;
		if (Tex == nullptr || Tex->GetSizeX() != ResX || Tex->GetSizeY() != ResY)
		{
			FName UniqueName = MakeUniqueObjectName(nullptr, UTexture2D::StaticClass(), "RTCStream");
			Tex = UTexture2D::CreateTransient(ResX, ResY, PF_B8G8R8A8, UniqueName);

			CurrSpaceUsersVideoTextures[userId] = Tex;

			TextureCreated = true;
		}

		void* TextureData = Tex->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(TextureData, ByteArray.GetData(), ByteArray.Num());
		Tex->GetPlatformData()->Mips[0].BulkData.Unlock();
		Tex->UpdateResource();

		if (TextureCreated && UserVideoFrameBindings.Contains(userId))
		{
			for (auto& Binding : UserVideoFrameBindings[userId])
			{
				(*Binding)(Tex);
			}
		}
	}

	void SpacePropertyModel::AwaitLocalUser(CavrnusSpaceUserEvent localUserArrived)
	{
		if (hasLocalUser)
		{
			localUserArrived(LocalUser);
		}
		else 
		{
			CavrnusSpaceUserEvent* callback = new CavrnusSpaceUserEvent(localUserArrived);
			LocalUserArrivedCallbacks.Add(callback);
		}
	}

	UCavrnusBinding* SpacePropertyModel::BindSpaceUsers(CavrnusSpaceUserEvent userAdded, CavrnusSpaceUserEvent userRemoved)
	{
		CavrnusSpaceUserEvent* added = new CavrnusSpaceUserEvent(userAdded);
		UserAddedBindings.Add(added);
		CavrnusSpaceUserEvent* removed = new CavrnusSpaceUserEvent(userRemoved);
		UserRemovedBindings.Add(removed);

		for (auto& UserElem : CurrSpaceUsers)
			userAdded(UserElem.Value);

		UCavrnusBinding* binding;
		binding = NewObject<UCavrnusBinding>();
		binding->SetupUnbind([this, added, removed]()
		{
			UserAddedBindings.Remove(added);
			UserRemovedBindings.Remove(removed);
		});

		return binding;
	}

}