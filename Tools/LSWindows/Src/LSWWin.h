#pragma once

//#include <SDKDDKVer.h>

/*
// Just for reference.
#define _WIN32_WINNT_NT4                    0x0400 // Windows NT 4.0  
#define _WIN32_WINNT_WIN2K                  0x0500 // Windows 2000  
#define _WIN32_WINNT_WINXP                  0x0501 // Windows XP  
#define _WIN32_WINNT_WS03                   0x0502 // Windows Server 2003  
#define _WIN32_WINNT_WIN6                   0x0600 // Windows Vista  
#define _WIN32_WINNT_VISTA                  0x0600 // Windows Vista  
#define _WIN32_WINNT_WS08                   0x0600 // Windows Server 2008  
#define _WIN32_WINNT_LONGHORN               0x0600 // Windows Vista  
#define _WIN32_WINNT_WIN7                   0x0601 // Windows 7  
#define _WIN32_WINNT_WIN8                   0x0602 // Windows 8  
#define _WIN32_WINNT_WINBLUE                0x0603 // Windows 8.1  
#define _WIN32_WINNT_WINTHRESHOLD           0x0A00 // Windows 10  
#define _WIN32_WINNT_WIN10                  0x0A00 // Windows 10  
*/

#define _WIN32_IE							0x0601
#define WINVER								0x0601
#define _WIN32_WINNT						0x0601
#define WIN32_LEAN_AND_MEAN
#define OEMRESOURCE							// For OBM_* bitmaps etc.
#define NOMINMAX
#include <Windows.h>

#ifdef WIN32_LEAN_AND_MEAN
typedef double DOUBLE;
#endif	// #ifdef WIN32_LEAN_AND_MEAN

#ifndef MAXUINT64
#define MAXUINT64 static_cast<uint64_t>(~static_cast<uint64_t>(0))
#endif	// #ifndef MAXUINT64

#include <CommCtrl.h>

#ifndef LVS_EX_DOUBLEBUFFER
#define LVS_EX_DOUBLEBUFFER 0
#endif	// #ifndef LVS_EX_DOUBLEBUFFER

#ifndef DC_BUTTONS
#define DC_BUTTONS 0
#endif	// #ifndef DC_BUTTONS

#ifndef DC_GRADIENT
#define DC_GRADIENT 0
#endif	// #ifndef DC_GRADIENT
