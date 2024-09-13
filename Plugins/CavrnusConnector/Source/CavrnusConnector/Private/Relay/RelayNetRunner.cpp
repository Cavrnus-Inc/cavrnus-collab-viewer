// Copyright(c) Cavrnus. All rights reserved.
#include "RelayNetRunner.h"

namespace Cavrnus
{
	static const int PORT_DEFAULT = 36045;
	static const std::string PATH_RELAY_NET_DEFAULT("..\\..\\..\\CavrnusPlugin\\Plugins\\CavrnusConnector\\Source\\CavrnusConnector\\CavrnusRelayNet\\net6.0-windows\\CavrnusRelayNet.exe");

	RelayNetRunner::RelayNetRunner()
	{
		clearMemory();
	}

	RelayNetRunner::~RelayNetRunner()
	{
		stopService();

		if (RelayNetProcessRunner.joinable())
		{
			RelayNetProcessRunner.join();
		}
	}

	void RelayNetRunner::clearMemory()
	{
		ZeroMemory(&RelayNetStartupInfo, sizeof(RelayNetStartupInfo));
		RelayNetStartupInfo.cb = sizeof(RelayNetStartupInfo);
		ZeroMemory(&RelayNetProcess, sizeof(RelayNetProcess));
	}

	bool RelayNetRunner::IsRelayNetProcessRunning() const
	{
		if (RelayNetProcess.dwProcessId != 0)
		{
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, RelayNetProcess.dwProcessId);
			if (hProcess != NULL)
			{
				DWORD exitCode;
				GetExitCodeProcess(hProcess, &exitCode);
				CloseHandle(hProcess);
				return exitCode == STILL_ACTIVE;
			}
		}

		return false;
	}

	bool RelayNetRunner::startService(int Port, bool bSilent, const std::string& ExecutablePath, const std::string& OptionalParameters)
	{
		std::string arg = std::to_string(Port); // Convert integer to wstring
		std::string cmd = ExecutablePath + " " + arg + (OptionalParameters.empty() ? "" : " " + OptionalParameters);

		std::wstring cmd_wchar(cmd.begin(), cmd.end());

		return CreateProcess(NULL, const_cast<wchar_t*>(cmd_wchar.c_str()), NULL, NULL, 0, bSilent ? CREATE_NO_WINDOW : 0, NULL, NULL, &RelayNetStartupInfo, &RelayNetProcess);
	}

	bool RelayNetRunner::stopService()
	{
		if (RelayNetProcess.hProcess != NULL)
		{
			TerminateProcess(RelayNetProcess.hProcess, 0);

			// Close CavrnusRelayNet process and thread handles
			CloseHandle(RelayNetProcess.hProcess);
			CloseHandle(RelayNetProcess.hThread);

			RelayNetProcess.hProcess = nullptr;

			if (RelayNetProcessRunner.joinable())
			{
				RelayNetProcessRunner.join();
			}
			return true;
		}

		return false;
	}

	void RelayNetRunner::WaitForRelayNetProcessToEnd()
	{
		// Wait for the CavrnusRelayNet process to finish
		WaitForSingleObject(RelayNetProcess.hProcess, INFINITE);

		// Close CavrnusRelayNet process and thread handles
		CloseHandle(RelayNetProcess.hProcess);
		CloseHandle(RelayNetProcess.hThread);
	}

	void RelayNetRunner::runAsync()
	{
		RelayNetProcessRunner = std::thread(&RelayNetRunner::run, this);
	}

	void RelayNetRunner::run()
	{
		if (!IsRelayNetProcessRunning())
		{
			//start relay net
			startService(PORT_DEFAULT, false, PATH_RELAY_NET_DEFAULT, "");

			//give it time to start up
			Sleep(100);
		}

		try
		{
			// Code to start the process			
			if (IsRelayNetProcessRunning())
			{
				// Wait for the process to exit
				WaitForSingleObject(RelayNetProcess.hProcess, INFINITE);
			}
		}
		catch (const std::exception& e)
		{
			std::cerr << "RelayNetRunner: Exception caught: " << e.what() << std::endl;
		}

	}

} // namespace Cavrnus