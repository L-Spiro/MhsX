#pragma once

#include "../LSWWin.h"
#include <map>
#include <string>

namespace lsw {

	class											CLayoutManager;

	class CBase {
	public :
		// == Functions.
		// =======================================
		// Create/Destroy.
		// =======================================
		// Initialize.
		static VOID									Initialize( HINSTANCE _hInst, CLayoutManager * _plmLayoutMan );

		// Shut down (frees memory).
		static VOID									ShutDown();

		// =======================================
		// Windows/controls.
		// =======================================
		// Wrapper for ::RegisterClassEx().
		static ATOM									RegisterClassExW( const WNDCLASSEXW &_wceClss );

		// Gets the ATOM associated with a class registered via RegisterClassExW().
		static ATOM									GetRegisteredClassAtom( LPCWSTR _lpwcClass );

		// Gets the ATOM associated with a class registered via RegisterClassExW().
		static ATOM									GetRegisteredClassAtom( const std::wstring &_wsClass );

		// =======================================
		// Process functions.
		// =======================================
		// Wrapper for ::GetModuleHandleW().
		static HMODULE								GetModuleHandleW( LPCWSTR _lpModuleName );

		// Wrapper for ::GetModuleHandleA().
		static HMODULE								GetModuleHandleA( LPCSTR _lpModuleName );

		// Gets the module handle for this executable.
		static HMODULE								GetThisHandle() { return m_hInstance; }

		// =======================================
		// Layouts.
		// =======================================
		// Gets the layout manager.
		static CLayoutManager *						LayoutManager() { return m_plmLayoutMan; }

		// =======================================
		// Error-checking.
		// =======================================
		// Prints the current error (from ::GetLastError()).
		static VOID									PrintError( LPCWSTR _pwcText );

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

		// The layout manager.
		static CLayoutManager *						m_plmLayoutMan;
												
	};

}	// namespace lsw
