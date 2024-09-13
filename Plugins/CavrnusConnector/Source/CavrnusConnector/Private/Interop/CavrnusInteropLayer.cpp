// Copyright(c) Cavrnus. All rights reserved.
#include "CavrnusInteropLayer.h"
#include "CavrnusConnectorModule.h"
#include "CavrnusConnectorSettings.h"
#include "CavrnusFunctionLibrary.h"
#include <HAL/FileManager.h>
#include <Misc/Paths.h>

#define PERFORMANCE_TRACKING 0

namespace Cavrnus
{

	//===============================================================
	CavrnusInteropLayer::CavrnusInteropLayer(const FString& serverIP, int serverPort)
		: ServiceIsStarted(false)
		, CurrTick(0)
		, LastKeepAliveTick(0)
		, Client_(serverIP, serverPort)
		, Status_(ConnectionStatus::eSTATUS_INIT)
		, Stop_(false)
	{
		//Launch listener thread and have it write to receivedMessages
		if (Client_.CreateListenSocket())
		{
			SetStatus(ConnectionStatus::eSTATUS_CONNECTED);
		}
		else
		{
			SetStatus(ConnectionStatus::eSTATUS_NOT_CONNECTED);
		}

		SendThread_ = std::thread(&CavrnusInteropLayer::SendLoop, this);
		ReceiveThread_ = std::thread(&CavrnusInteropLayer::ReceiveLoop, this);

		//Launch RelayNet.exe process
		RunService();

		if (!Client_.Connect())
		{
			SetStatus(ConnectionStatus::eSTATUS_NOT_CONNECTED);
		}
	}


	//===============================================================
	CavrnusInteropLayer::~CavrnusInteropLayer()
	{
		LogPerformance();

		//Kill RelayNet.exe process
		if (ServiceIsStarted)
		{
			RelayNetRunner_.stopService();
			ServiceIsStarted = false;
		}

		Stop_ = true;

		//Stop listener thread/socket
		SetStatus(ConnectionStatus::eSTATUS_SHUTDOWN);

		// Disconnect from the server
		Client_.Disconnect();

		// Stop the send and receive threads
		if (SendThread_.joinable())
		{
			SendThread_.join();
		}

		if (ReceiveThread_.joinable())
		{
			ReceiveThread_.join();
		}
	}


	//===============================================================
	void CavrnusInteropLayer::SetStatus(ConnectionStatus newStatus)
	{
		Status_ = newStatus;
	}

	//===============================================================
	void CavrnusInteropLayer::LogPerformance()
	{
#if PERFORMANCE_TRACKING
		int totalEntries = 0;
		int totalMessages = 0;
		int maxMessagesPerSecond = 0;
		float avgMessagesPerSecond = 0;

		for (std::map<int, int>::iterator it = PropertiesSentPerSecond.begin(); it != PropertiesSentPerSecond.end(); it++)
		{
			totalEntries++;
			totalMessages += it->second;
			maxMessagesPerSecond = it->second > maxMessagesPerSecond ? it->second : maxMessagesPerSecond;
			avgMessagesPerSecond = (float)(totalMessages) / (float)(totalEntries);
		}
		UE_LOG(LogCavrnusConnector, Log, TEXT("Total number of messages: %d"), totalMessages);
		UE_LOG(LogCavrnusConnector, Log, TEXT("Total number of 1 second time windows: %d"), totalEntries);
		UE_LOG(LogCavrnusConnector, Log, TEXT("Max messages per second: %d"), maxMessagesPerSecond);
		UE_LOG(LogCavrnusConnector, Log, TEXT("Average messages per second: %f"), avgMessagesPerSecond);
#endif
	}

	//===============================================================
	void CavrnusInteropLayer::SendMessage(const ServerData::RelayClientMessage& message)
	{
		//send queue is lock free but only one thread can send message at a time
		std::lock_guard<std::mutex> lock(Send_mutex_);

		ServerData::RelayClientMessage* BatchMessage = SendMessageBatch.add_messages();
		BatchMessage->CopyFrom(message);
	}


	//===============================================================
	const TArray<ServerData::RelayRemoteMessage> CavrnusInteropLayer::GetReceivedMessages()
	{
		std::lock_guard<std::mutex> lock(Receive_mutex_);

		TArray<ServerData::RelayRemoteMessage> currMessages;

		while (!MessageProcessingQueue_.empty())
		{
			std::shared_ptr<ServerData::RelayRemoteMessage> msg = MessageProcessingQueue_.front();
			currMessages.Add(*msg);
			MessageProcessingQueue_.pop();
		}

		return currMessages;
	}

	//TODO: QUESTION, can't this just be done synchronously in SendMessage?
	void CavrnusInteropLayer::DoTick()
	{
		//for (int i = 0; i < messagesToSend.Num(); i++)
		//{
		//	//TODO: SEND messagesToSend[i] to socket
		//}
		//messagesToSend.Empty();
	}


	//===============================================================
	void CavrnusInteropLayer::SendKeepAlive()
	{
		ServerData::RelayClientMessage msg;
		ServerData::KeepAlive keepAlive;
		msg.mutable_keepalive()->CopyFrom(keepAlive);

		SendMessage(msg);
	}


	//===============================================================
	void CavrnusInteropLayer::SendLoop()
	{
		while (!Stop_)
		{
			CurrTick++;

			if (LastKeepAliveTick + 100 < CurrTick)
			{
				LastKeepAliveTick.store(CurrTick);
				//std::cout << "Sending KeepAlive!!!" << std::endl;
				SendKeepAlive();
			}

			//std::shared_ptr<ServerData::RelayClientMessage> sendMessage;
			if (SendMessageBatch.messages_size() > 0)
			{
				// lock the message queue while we're processing it
				std::lock_guard<std::mutex> lock(Send_mutex_);

#if PERFORMANCE_TRACKING				
				int seconds = (int)(FPlatformTime::Seconds());
				int numMessages = SendMessageBatch.messages_size();
				std::map<int,int>::iterator it = PropertiesSentPerSecond.find(seconds);
				if (it != PropertiesSentPerSecond.end())
				{
					it->second += numMessages;
				}
				else
				{
					PropertiesSentPerSecond[seconds] = numMessages;
				}
#endif

				// Send the message to the server
				Client_.SendMessage(SendMessageBatch);

				SendMessageBatch.clear_messages();
			}

			// Sleep for a short time to avoid busy-waiting
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}


	//===============================================================
	void CavrnusInteropLayer::ReceiveLoop()
	{
		while (!Stop_)
		{
			// Receive a message from the server
			ServerData::RelayRemoteMessageBatch response;
			if (Client_.ReceiveMessage(response))
			{
				std::lock_guard<std::mutex> lock(Receive_mutex_);

				for (int i = 0; i < response.messages_size(); i++)
				{
					MessageProcessingQueue_.push(std::make_shared<ServerData::RelayRemoteMessage>(response.messages(i)));
				}
			}
			else
			{
				// Sleep for a short time to avoid busy-waiting
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
		}
	}

	//===============================================================
	void CavrnusInteropLayer::RunService()
	{
		if (!ServiceIsStarted)
		{
			UE_LOG(LogCavrnusConnector, Log, TEXT("Starting CavrnusRelayNet Service."));

			// Get settings class
			UCavrnusConnectorSettings* settings = GetMutableDefault<UCavrnusConnectorSettings>();
			if (!settings)
			{
				UE_LOG(LogCavrnusConnector, Log, TEXT("[FCavrnusConnectorModule::RunService()] Could not get UCavrnusConnectorSettings class."));
				return;
			}

			// Get path to exe
			// \todo fix for packaged builds
			FString exeLocation = FPaths::Combine(GetPluginPath(), TEXT("Source"), TEXT("CavrnusConnector")) / settings->RelayNetExecutableRelativeLocation;

#if UE_BUILD_SHIPPING
			bool bSilent = true;
#else
			bool bSilent = settings->RelayNetVerboseLogging ? settings->RelayNetSilent : true;
#endif

			if (RelayNetRunner_.startService(Client_.GetServerPort(), bSilent, TCHAR_TO_UTF8(*exeLocation), TCHAR_TO_UTF8(*settings->GetRelayNetOptionalParameters())))
			{
				RelayNetRunner_.runAsync();
				ServiceIsStarted = true;
				UE_LOG(LogCavrnusConnector, Log, TEXT("CavrnusRelayNet Service Started Successfully"));
			}
			else
			{
				UE_LOG(LogCavrnusConnector, Log, TEXT("Create CavrnusRelayNet Process failed"));
			}

		}
	}

	const FString CavrnusInteropLayer::GetPluginPath()
	{
		static FString PluginPath;

		if (PluginPath.IsEmpty())
		{
			FString ProjectPath = FPaths::ProjectPluginsDir() / TEXT("CavrnusConnector");
			FString EnginePath = FPaths::EnginePluginsDir() / TEXT("Marketplace") / TEXT("CavrnusConnector");

			if (IFileManager::Get().DirectoryExists(*ProjectPath))
			{
				PluginPath = ProjectPath;
			}
			else
			{
				PluginPath = EnginePath;
			}
		}

		return PluginPath;
	}
}