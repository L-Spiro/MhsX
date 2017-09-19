#include "LSWBase.h"
#include "../Layout/LSWLayoutManager.h"

#include <Strsafe.h>

#pragma comment( lib, "comctl32.lib" )
/*#pragma comment( linker, "\"/manifestdependency:type='win32' \
	name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
	processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"" )*/

namespace lsw {

	// == Members.
	// Registered classes.
	std::map<std::wstring, CBase::WNDCLASSEXW_ATOM>	CBase::m_mClasses;

	// This HINSTANCE.
	HINSTANCE CBase::m_hInstance = NULL;

	// The layout manager.
	CLayoutManager * CBase::m_plmLayoutMan = nullptr;

	// == Functions.
	// Initialize.
	VOID CBase::Initialize( HINSTANCE _hInst, CLayoutManager * _plmLayoutMan ) {
		m_hInstance = _hInst;
		m_plmLayoutMan = _plmLayoutMan;

		INITCOMMONCONTROLSEX iccIn = {
			sizeof( iccIn ),
			ICC_BAR_CLASSES | ICC_LISTVIEW_CLASSES | ICC_PROGRESS_CLASS | ICC_TAB_CLASSES | ICC_TREEVIEW_CLASSES | ICC_USEREX_CLASSES
		};
		::InitCommonControlsEx( &iccIn );
	}

	// Shut down (frees memory).
	VOID CBase::ShutDown() {
		for ( auto it = m_mClasses.begin(); it != m_mClasses.end(); ++it ) {
			::UnregisterClassW( (*it).second.lpszClassName, (*it).second.hInstance );
		}
		m_mClasses.clear();
		m_mClasses = std::map<std::wstring, WNDCLASSEXW_ATOM>();

		delete m_plmLayoutMan;
		m_plmLayoutMan = nullptr;
	}

	// Wrapper for ::RegisterClassEx().
	ATOM CBase::RegisterClassExW( const WNDCLASSEXW &_wceClss ) {
		std::wstring wTemp = _wceClss.lpszClassName;
		ATOM aPrev = GetRegisteredClassAtom( wTemp );
		if ( aPrev ) { return aPrev; }

		WNDCLASSEXW_ATOM wceTemp;
		(*reinterpret_cast<WNDCLASSEXW *>(&wceTemp)) = _wceClss;
		wceTemp.aAtom = ::RegisterClassExW( &_wceClss );
		if ( !wceTemp.aAtom ) { return wceTemp.aAtom; }
		m_mClasses[wTemp] = wceTemp;
		return wceTemp.aAtom;
	}

	// Gets the ATOM associated with a class registered via RegisterClassExW().
	ATOM CBase::GetRegisteredClassAtom( LPCWSTR _lpwcClass ) {
		std::wstring wTemp = _lpwcClass;
		return GetRegisteredClassAtom( wTemp );
	}

	// Gets the ATOM associated with a class registered via RegisterClassExW().
	ATOM CBase::GetRegisteredClassAtom( const std::wstring &_wsClass ) {
		auto aThis = m_mClasses.find( _wsClass );
		if ( aThis != m_mClasses.end() ) {
			return (*aThis).second.aAtom;
		}
		return 0;
	}

	// Wrapper for ::GetModuleHandleW().
	HMODULE CBase::GetModuleHandleW( LPCWSTR _lpModuleName ) {
		return _lpModuleName ? ::GetModuleHandleW( _lpModuleName ) : m_hInstance;
	}

	// Wrapper for ::GetModuleHandleA().
	HMODULE CBase::GetModuleHandleA( LPCSTR _lpModuleName ) {
		return _lpModuleName ? ::GetModuleHandleA( _lpModuleName ) : m_hInstance;
	}

	// Prints the current error (from ::GetLastError()).
	VOID CBase::PrintError( LPCWSTR _pwcText ) {
		LPVOID lpMsgBuf;
		LPVOID lpDisplayBuf;
		DWORD dwError = ::GetLastError(); 

		::FormatMessageW(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPWSTR) &lpMsgBuf,
			0, NULL );

		// Display the error message and exit the process

		lpDisplayBuf = static_cast<LPVOID>(::LocalAlloc( LMEM_ZEROINIT, 
			(lstrlenW( static_cast<LPCWSTR>(lpMsgBuf) ) + lstrlenW( static_cast<LPCWSTR>(_pwcText) ) + 40) * sizeof(WCHAR) ));
		::StringCchPrintfW( static_cast<LPWSTR>(lpDisplayBuf), 
			::LocalSize( lpDisplayBuf ) / sizeof( WCHAR ),
			L"%s failed with error %d: %s",
			_pwcText, dwError, lpMsgBuf );
		::MessageBoxW( NULL, static_cast<LPCWSTR>(lpDisplayBuf), L"Error", MB_OK ); 

		::LocalFree( lpMsgBuf );
		::LocalFree( lpDisplayBuf );

	}

}	// namespace lsw
