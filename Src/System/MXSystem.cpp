#include "MXSystem.h"
#include "../Strings/MXStringDecoder.h"


namespace mx {

	// == Members.
	// Is the WoW64?
	BOOL CSystem::m_bIsWow = FALSE;

	// System information.
	SYSTEM_INFO CSystem::m_siSystemInfo = { 0 };

	// == Types.
	typedef BOOL (WINAPI * LPFN_ISWOW64PROCESS)( HANDLE, PBOOL );
	typedef VOID (WINAPI * LPFN_GETSYSTEMINFO)( LPSYSTEM_INFO );

	// Initialize the system.
	VOID CSystem::InitSystem() {
		// Is this WoW64?
		{
			CHAR szKernel32[_LEN_204C64E5+1];
			CHAR szIsWow64Process[_LEN_2E50340B+1];
			LPFN_ISWOW64PROCESS pfIsWow64Process = reinterpret_cast<LPFN_ISWOW64PROCESS>(::GetProcAddress( ::GetModuleHandleA(
				_DEC_204C64E5_kernel32(  szKernel32 ) ),
				_DEC_2E50340B_IsWow64Process( szIsWow64Process ) ));
			if ( pfIsWow64Process ) {
				pfIsWow64Process( ::GetCurrentProcess(), &m_bIsWow );
			}
			// Don't leave encrypted strings on the stack.
			::ZeroMemory( szKernel32, sizeof( szKernel32 ) );
			::ZeroMemory( szIsWow64Process, sizeof( szIsWow64Process ) );
			pfIsWow64Process = nullptr;
		}
		
		
		// System information.
		{
			LPFN_GETSYSTEMINFO pfGetSystemInfo = nullptr;
			CHAR szKernel32[_LEN_204C64E5+1];
			CHAR szGetSystemInfo[_T_MAX_LEN];
			if ( IsWow64Process() ) {				
				pfGetSystemInfo = reinterpret_cast<LPFN_GETSYSTEMINFO>(::GetProcAddress( ::GetModuleHandleA(
					_DEC_204C64E5_kernel32(  szKernel32 ) ),
					_DEC_EB64C435_GetNativeSystemInfo( szGetSystemInfo ) ));
			}
			else {
				pfGetSystemInfo = reinterpret_cast<LPFN_GETSYSTEMINFO>(::GetProcAddress( ::GetModuleHandleA(
					_DEC_204C64E5_kernel32(  szKernel32 ) ),
					_DEC_763FADF6_GetSystemInfo( szGetSystemInfo ) ));
			}
			// Don't leave encrypted strings on the stack.
			::ZeroMemory( szKernel32, sizeof( szKernel32 ) );
			::ZeroMemory( szGetSystemInfo, sizeof( szGetSystemInfo ) );
			if ( pfGetSystemInfo ) {
				pfGetSystemInfo( &m_siSystemInfo );
			}
			pfGetSystemInfo = nullptr;
		}
	}

}	// namespace mx
