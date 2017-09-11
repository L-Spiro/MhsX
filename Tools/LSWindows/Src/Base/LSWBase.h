#pragma once

#include "../LSWWin.h"
#include <map>
#include <string>

namespace lsw {

	// I tend not to have more than one class in a file, but oh well.
	class CBase {
	public :
		// == Functions.
		// Initialize.
		static VOID									Initialize( HINSTANCE _hInst );

		// Shut down (frees memory).
		static VOID									ShutDown();

		// Wrapper for ::RegisterClassEx().
		static ATOM									RegisterClassExW( const WNDCLASSEXW &_wceClss );

		// Gets the ATOM associated with a class registered via RegisterClassExW().
		static ATOM									GetRegisteredClassAtom( LPCWSTR _lpwcClass );

		// Gets the ATOM associated with a class registered via RegisterClassExW().
		static ATOM									GetRegisteredClassAtom( const std::wstring &_wsClass );

		// Wrapper for ::GetModuleHandleW().
		static HMODULE								GetModuleHandleW( LPCWSTR _lpModuleName );

		// Wrapper for ::GetModuleHandleA().
		static HMODULE								GetModuleHandleA( LPCSTR _lpModuleName );

		// Gets the module handle for this executable.
		static HMODULE								GetThisHandle() { return m_hInstance; }

	protected :
		// == Types.
		struct WNDCLASSEXW_ATOM : public WNDCLASSEXW {
			ATOM									aAtom;
		};

		// == Members.
		// Registered classes.
		static std::map<std::wstring, WNDCLASSEXW_ATOM>
													m_mClasses;

		// This HINSTANCE.
		static HINSTANCE							m_hInstance;
												
	};

}	// namespace lsw
