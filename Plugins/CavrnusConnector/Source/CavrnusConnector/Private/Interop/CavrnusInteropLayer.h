// Copyright(c) Cavrnus. All rights reserved.
#pragma once

#include "Relay/RelayClient.h"
#include "Relay/RelayNetRunner.h"

#include <thread>

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

namespace Cavrnus
{
	class CavrnusInteropLayer
	{
	public:
		CavrnusInteropLayer(const FString& serverIP, int serverPort);
		virtual ~CavrnusInteropLayer();

		void SendMessage(const ServerData::RelayClientMessage& message);

		const TArray<ServerData::RelayRemoteMessage> GetReceivedMessages();

		void DoTick();

		void RunService();

		void LogPerformance();

	private:
		void SendLoop();
		void ReceiveLoop();
		void SendKeepAlive();
		void SetStatus(ConnectionStatus newStatus);
		const FString GetPluginPath();


		std::atomic<bool>	ServiceIsStarted;
		std::atomic<int>	CurrTick;
		std::atomic<int>	LastKeepAliveTick;

		RelayClient						Client_;
		std::atomic<ConnectionStatus>   Status_;

		std::atomic<bool>				Stop_;
		mutable std::mutex				Send_mutex_;
		mutable std::mutex				Receive_mutex_;

		std::thread						SendThread_;
		std::thread						ReceiveThread_;

		Cavrnus::RelayNetRunner			RelayNetRunner_;

		ServerData::RelayClientMessageBatch SendMessageBatch;

		typedef std::queue<std::shared_ptr<ServerData::RelayRemoteMessage> > MessageProcessingQueue;
		MessageProcessingQueue MessageProcessingQueue_;


		//Copied from: https://stackoverflow.com/questions/440133/how-do-i-create-a-random-alpha-numeric-string-in-c
		std::string random_string(size_t length)
		{
			auto randchar = []() -> char
				{
					const char charset[] =
						"0123456789"
						"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
						"abcdefghijklmnopqrstuvwxyz";
					const size_t max_index = (sizeof(charset) - 1);
					return charset[rand() % max_index];
				};
			std::string str(length, 0);
			std::generate_n(str.begin(), length, randchar);
			return str;
		}

		std::map<int, int> PropertiesSentPerSecond;
	};

} // namespace CavrnusRelay