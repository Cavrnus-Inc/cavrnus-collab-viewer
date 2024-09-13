// Copyright(c) Cavrnus. All rights reserved.
#pragma once

#ifdef _MSC_VER
#pragma warning(disable : 4251) // needs to have dll-interface to be used by clients of class
#endif

#pragma warning(disable : 4267) // size_t to int
#pragma warning(disable : 4800)	// int to bool
#define GOOGLE_PROTOBUF_INTERNAL_DONATE_STEAL_INLINE 0

#include "Windows/AllowWindowsPlatformTypes.h"
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>

#include <string>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <queue>
#include <thread>
#include <functional>
#include <thread>
#include <mutex>
#include <winsock2.h>
#include <Ws2tcpip.h>

#include "Comm/relay.pb.h"

#include "ConnectionStatus.h"
