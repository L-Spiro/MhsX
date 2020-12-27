#pragma once

#include <LSWWin.h>
#include <cstdint>

#ifndef PROCESSOR_ARCHITECTURE_ARM64
#define PROCESSOR_ARCHITECTURE_ARM64	12
#endif	// #ifndef PROCESSOR_ARCHITECTURE_ARM64

#ifndef DWINVALID
#define DWINVALID						static_cast<DWORD>(-1)
#endif	// #ifndef DWINVALID

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS					0x00000000
#endif	// #ifndef STATUS_SUCCESS

#ifndef STATUS_ABANDONED
#define STATUS_ABANDONED				0x00000080
#endif	// #ifndef STATUS_ABANDONED
