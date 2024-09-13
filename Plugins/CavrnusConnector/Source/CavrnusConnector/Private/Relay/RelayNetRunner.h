// Copyright(c) Cavrnus. All rights reserved.
#pragma once

#include "CavrnusRelayLibrary.h"

namespace Cavrnus
{
	class RelayNetRunner
	{
	public:
		RelayNetRunner();
		virtual ~RelayNetRunner();

		void run();
		void runAsync();
		bool startService(int Port, bool bSilent, const std::string& ExecutablePath, const std::string& OptionalParameters);
		bool stopService();
		bool IsRelayNetProcessRunning() const;
		void WaitForRelayNetProcessToEnd();

	private:
		void clearMemory();

		STARTUPINFO RelayNetStartupInfo;
		PROCESS_INFORMATION RelayNetProcess;
		std::thread RelayNetProcessRunner;
	};
}

