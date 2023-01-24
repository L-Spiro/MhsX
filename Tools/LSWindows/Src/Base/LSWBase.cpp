#include "LSWBase.h"
#include "../Docking/LSWDockable.h"
#include "../Layout/LSWLayoutManager.h"
#include "../TreeList/LSWTreeList.h"
#include "LSWWndClassEx.h"

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

	// Non-client metrics.
	NONCLIENTMETRICSW CBase::m_ncmNonClientMetrics = { sizeof( m_ncmNonClientMetrics ) };

	// Message-box font.
	HFONT CBase::m_hMessageFont = NULL;

	// Status font.
	HFONT CBase::m_hStatusFont = NULL;

	// The dockable class.
	ATOM CBase::m_aDockable = 0;

	// The splitter class.
	ATOM CBase::m_aSplitter = 0;

	// The multi-splitter class.
	ATOM CBase::m_aMultiSplitter = 0;

	// The tree view class.
	ATOM CBase::m_aTreeView = 0;

	// The tree-list view class.
	ATOM CBase::m_aTreeListView = 0;

	/** The child-window class. */
	ATOM CBase::m_aChildWindow = 0;

	// == Functions.
	// Initialize.
	VOID CBase::Initialize( HINSTANCE _hInst, CLayoutManager * _plmLayoutMan,
		const WCHAR * _pwcDockableClassName,
		const WCHAR * _pwcSplitterClassName,
		const WCHAR * _pwcMultiSplitterClassName,
		const WCHAR * _pwcTreeViewClassName,
		const WCHAR * _pwcTreeListViewClassName,
		const WCHAR * _pwcChildWindowClassName ) {
		m_hInstance = _hInst;
		m_plmLayoutMan = _plmLayoutMan;

		// Non-client metrics.
		::SystemParametersInfoW( SPI_GETNONCLIENTMETRICS,
			sizeof( m_ncmNonClientMetrics ),
			&m_ncmNonClientMetrics,
			0 );

		m_hMessageFont = ::CreateFontIndirectW( &m_ncmNonClientMetrics.lfMessageFont );
		m_hStatusFont = ::CreateFontIndirectW( &m_ncmNonClientMetrics.lfStatusFont );

		INITCOMMONCONTROLSEX iccIn = {
			sizeof( iccIn ),
			ICC_BAR_CLASSES | ICC_LISTVIEW_CLASSES | ICC_PROGRESS_CLASS | ICC_TAB_CLASSES | ICC_TREEVIEW_CLASSES | ICC_USEREX_CLASSES | ICC_COOL_CLASSES | ICC_UPDOWN_CLASS
		};
		BOOL bCommons = ::InitCommonControlsEx( &iccIn );
		if ( !bCommons ) {
			MessageBoxError( NULL, L"Common controls failed to initialize." );
		}

		// Register the dockable class.
		if ( _pwcDockableClassName ) {
			lsw::CWndClassEx wceDock;
			wceDock.SetInstance( GetThisHandle() );
			wceDock.SetClassName( _pwcDockableClassName );
			wceDock.SetStyle( 0 );	// Test this.
			wceDock.SetBackgroundBrush( reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1) );
			wceDock.SetWindPro( CDockable::WindowProc );
			m_aDockable = CBase::RegisterClassExW( wceDock.Obj() );
		}

		// Register the splitter class.
		if ( _pwcSplitterClassName ) {
			lsw::CWndClassEx wceSplitter;
			wceSplitter.SetInstance( GetThisHandle() );
			wceSplitter.SetClassName( _pwcSplitterClassName );
			wceSplitter.SetStyle( 0 );
			wceSplitter.SetBackgroundBrush( reinterpret_cast<HBRUSH>(COLOR_3DFACE + 1) );
			wceSplitter.SetWindPro( CWidget::WindowProc );
			wceSplitter.SetCursor( NULL );
			m_aSplitter = CBase::RegisterClassExW( wceSplitter.Obj() );
		}

		// Register the multi-splitter class.
		if ( _pwcMultiSplitterClassName ) {
			lsw::CWndClassEx wceMulSplitter;
			wceMulSplitter.SetInstance( GetThisHandle() );
			wceMulSplitter.SetClassName( _pwcMultiSplitterClassName );
			wceMulSplitter.SetStyle( 0 );
			wceMulSplitter.SetBackgroundBrush( reinterpret_cast<HBRUSH>(COLOR_3DFACE + 1) );
			wceMulSplitter.SetWindPro( CWidget::WindowProc );
			wceMulSplitter.SetCursor( NULL );
			m_aMultiSplitter = CBase::RegisterClassExW( wceMulSplitter.Obj() );
		}

		// Register the unused tree-view class.
		if ( _pwcTreeViewClassName ) {
			lsw::CWndClassEx wceTreeView;
			wceTreeView.SetInstance( GetThisHandle() );
			wceTreeView.SetClassName( _pwcTreeViewClassName );
			wceTreeView.SetStyle( WS_EX_LEFT | WS_EX_CLIENTEDGE | /*WS_EX_TRANSPARENT | */WS_EX_CONTROLPARENT );
			wceTreeView.SetBackgroundBrush( 0 );
			//wceTreeView.SetWindPro( CTreeList::TreeViewProc );
			wceTreeView.SetWindPro( CWidget::WindowProc );
			wceTreeView.SetCursor( NULL );
			m_aTreeView = CBase::RegisterClassExW( wceTreeView.Obj() );
		}

		// Register the tree-list-view class.
		if ( _pwcTreeListViewClassName ) {
			lsw::CWndClassEx wceTreeListView;
			wceTreeListView.SetInstance( GetThisHandle() );
			wceTreeListView.SetClassName( _pwcTreeListViewClassName );
			wceTreeListView.SetStyle( WS_EX_LEFT | WS_EX_CLIENTEDGE | WS_EX_TRANSPARENT/* | WS_EX_CONTROLPARENT*/ /*| WS_EX_COMPOSITED*/ );
			//wceTreeListView.SetBackgroundBrush( 0 );
			wceTreeListView.SetWindPro( CWidget::WindowProc );
			//wceTreeListView.SetCursor( NULL );
			m_aTreeListView = CBase::RegisterClassExW( wceTreeListView.Obj() );
		}

		// Register the child-window class.
		if ( _pwcChildWindowClassName ) {
			lsw::CWndClassEx wceTreeListView;
			wceTreeListView.SetInstance( GetThisHandle() );
			wceTreeListView.SetClassName( _pwcChildWindowClassName );
			wceTreeListView.SetStyle( WS_CHILDWINDOW | WS_VISIBLE /*| DS_3DLOOK | DS_FIXEDSYS | DS_SETFONT | DS_CONTROL*/ );
			wceTreeListView.SetBackgroundBrush( 0 );
			wceTreeListView.SetWindPro( CWidget::WindowProc );
			//wceTreeListView.SetCursor( NULL );
			m_aChildWindow = CBase::RegisterClassExW( wceTreeListView.Obj() );
		}
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

		::DeleteObject( m_hStatusFont );
		::DeleteObject( m_hMessageFont );
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
	VOID CBase::PrintError( LPCWSTR _pwcText, DWORD _dwError ) {
		std::wstring swText;
		AppendError( _pwcText, swText, _dwError );
		::MessageBoxW( NULL, swText.c_str(), L"Error", MB_OK );
	}

	// Appends error text to a string.
	std::wstring CBase::AppendError( LPCWSTR _pwcText, std::wstring &_wsRet, DWORD _dwError ) {
		DWORD dwError = (_dwError == UINT_MAX) ? ::GetLastError() : _dwError;
		LPVOID lpMsgBuf;
		LPVOID lpDisplayBuf;

		::FormatMessageW(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwError,
			MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
			reinterpret_cast<LPWSTR>(&lpMsgBuf),
			0, NULL );

		LPCWSTR lpwsSrc = _pwcText ? _pwcText : L"";

		lpDisplayBuf = static_cast<LPVOID>(::LocalAlloc( LMEM_ZEROINIT,
			(::lstrlenW( static_cast<LPCWSTR>(lpMsgBuf) ) + ::lstrlenW( static_cast<LPCWSTR>(_pwcText) ) + 40) * sizeof( WCHAR ) ));
		::StringCchPrintfW( static_cast<LPWSTR>(lpDisplayBuf),
			::LocalSize( lpDisplayBuf ) / sizeof( WCHAR ),
			(_pwcText && _pwcText[0] != L'\0') ?
				L"%s (error %d): %s" :
				L"%sError %d: %s",
			lpwsSrc, dwError, lpMsgBuf );
		_wsRet += static_cast<LPCWSTR>(lpDisplayBuf);

		::LocalFree( lpDisplayBuf );
		::LocalFree( lpMsgBuf );
		return _wsRet;
	}

	// Displays a message box with the given title and message.
	VOID CBase::MessageBoxError( HWND _hWnd, LPCWSTR _pwcMsg, LPCWSTR _pwcTitle ) {
		::MessageBoxW( _hWnd, _pwcMsg, _pwcTitle, MB_ICONERROR );
	}

	// Displays a message box with the given title and message.
	VOID CBase::MessageBoxError( HWND _hWnd, LPCSTR _pcMsg, LPCSTR _pcTitle ) {
		::MessageBoxA( _hWnd, _pcMsg, _pcTitle, MB_ICONERROR );
	}

	// Prompts with MB_ICONINFORMATION and IDOK.
	bool CBase::PromptOk( HWND _hWnd, LPCSTR _pcMsg, LPCSTR _pcTitle, bool _bIncludeCancel ) {
		return ::MessageBoxA( _hWnd, _pcMsg, _pcTitle, MB_ICONINFORMATION | (_bIncludeCancel ? MB_OKCANCEL : 0)) == IDOK;
	}

	// Prompts with MB_ICONINFORMATION and IDOK.
	bool CBase::PromptOk( HWND _hWnd, LPCWSTR _pwcMsg, LPCWSTR _pwcTitle, bool _bIncludeCancel ) {
		return ::MessageBoxW( _hWnd, _pwcMsg, _pwcTitle, MB_ICONINFORMATION | (_bIncludeCancel ? MB_OKCANCEL : 0)) == IDOK;
	}

	// Prompts with MB_ICONQUESTION and IDYES.
	bool CBase::PromptYesNo( HWND _hWnd, LPCSTR _pcMsg, LPCSTR _pcTitle ) {
		return ::MessageBoxA( _hWnd, _pcMsg, _pcTitle, MB_ICONQUESTION | MB_YESNO ) == IDYES;
	}

	// Prompts with MB_ICONQUESTION and IDYES.
	bool CBase::PromptYesNo( HWND _hWnd, LPCWSTR _pwcMsg, LPCWSTR _pwcTitle ) {
		return ::MessageBoxW( _hWnd, _pwcMsg, _pwcTitle, MB_ICONQUESTION | MB_YESNO ) == IDYES;
	}

#ifdef _DEBUG
	VOID CBase::MessageToText( WORD _wMessage, std::string &_sName ) {
		static const struct {
			WORD				wId;
			const CHAR *		pcName;
		} sArray[] = {
			{ 0x0000, "WM_NULL" },
			{ 0x0001, "WM_CREATE" },
			{ 0x0002, "WM_DESTROY" },
			{ 0x0003, "WM_MOVE" },
			{ 0x0005, "WM_SIZE" },
			{ 0x0006, "WM_ACTIVATE" },
			{ 0x0007, "WM_SETFOCUS" },
			{ 0x0008, "WM_KILLFOCUS" },
			{ 0x000a, "WM_ENABLE" },
			{ 0x000b, "WM_SETREDRAW" },
			{ 0x000c, "WM_SETTEXT" },
			{ 0x000d, "WM_GETTEXT" },
			{ 0x000e, "WM_GETTEXTLENGTH" },
			{ 0x000f, "WM_PAINT" },
			{ 0x0010, "WM_CLOSE" },
			{ 0x0011, "WM_QUERYENDSESSION" },
			{ 0x0012, "WM_QUIT" },
			{ 0x0013, "WM_QUERYOPEN" },
			{ 0x0014, "WM_ERASEBKGND" },
			{ 0x0015, "WM_SYSCOLORCHANGE" },
			{ 0x0016, "WM_ENDSESSION" },
			{ 0x0018, "WM_SHOWWINDOW" },
			{ 0x0019, "WM_CTLCOLOR" },
			{ 0x001a, "WM_WININICHANGE" },
			{ 0x001b, "WM_DEVMODECHANGE" },
			{ 0x001c, "WM_ACTIVATEAPP" },
			{ 0x001d, "WM_FONTCHANGE" },
			{ 0x001e, "WM_TIMECHANGE" },
			{ 0x001f, "WM_CANCELMODE" },
			{ 0x0020, "WM_SETCURSOR" },
			{ 0x0021, "WM_MOUSEACTIVATE" },
			{ 0x0022, "WM_CHILDACTIVATE" },
			{ 0x0023, "WM_QUEUESYNC" },
			{ 0x0024, "WM_GETMINMAXINFO" },
			{ 0x0026, "WM_PAINTICON" },
			{ 0x0027, "WM_ICONERASEBKGND" },
			{ 0x0028, "WM_NEXTDLGCTL" },
			{ 0x002a, "WM_SPOOLERSTATUS" },
			{ 0x002b, "WM_DRAWITEM" },
			{ 0x002c, "WM_MEASUREITEM" },
			{ 0x002d, "WM_DELETEITEM" },
			{ 0x002e, "WM_VKEYTOITEM" },
			{ 0x002f, "WM_CHARTOITEM" },
			{ 0x0030, "WM_SETFONT" },
			{ 0x0031, "WM_GETFONT" },
			{ 0x0032, "WM_SETHOTKEY" },
			{ 0x0033, "WM_GETHOTKEY" },
			{ 0x0037, "WM_QUERYDRAGICON" },
			{ 0x0039, "WM_COMPAREITEM" },
			{ 0x003d, "WM_GETOBJECT" },
			{ 0x0041, "WM_COMPACTING" },
			{ 0x0044, "WM_COMMNOTIFY" },
			{ 0x0046, "WM_WINDOWPOSCHANGING" },
			{ 0x0047, "WM_WINDOWPOSCHANGED" },
			{ 0x0048, "WM_POWER" },
			{ 0x0049, "WM_COPYGLOBALDATA" },
			{ 0x004a, "WM_COPYDATA" },
			{ 0x004b, "WM_CANCELJOURNAL" },
			{ 0x004e, "WM_NOTIFY" },
			{ 0x0050, "WM_INPUTLANGCHANGEREQUEST" },
			{ 0x0051, "WM_INPUTLANGCHANGE" },
			{ 0x0052, "WM_TCARD" },
			{ 0x0053, "WM_HELP" },
			{ 0x0054, "WM_USERCHANGED" },
			{ 0x0055, "WM_NOTIFYFORMAT" },
			{ 0x007b, "WM_CONTEXTMENU" },
			{ 0x007c, "WM_STYLECHANGING" },
			{ 0x007d, "WM_STYLECHANGED" },
			{ 0x007e, "WM_DISPLAYCHANGE" },
			{ 0x007f, "WM_GETICON" },
			{ 0x0080, "WM_SETICON" },
			{ 0x0081, "WM_NCCREATE" },
			{ 0x0082, "WM_NCDESTROY" },
			{ 0x0083, "WM_NCCALCSIZE" },
			{ 0x0084, "WM_NCHITTEST" },
			{ 0x0085, "WM_NCPAINT" },
			{ 0x0086, "WM_NCACTIVATE" },
			{ 0x0087, "WM_GETDLGCODE" },
			{ 0x0088, "WM_SYNCPAINT" },
			{ 0x00a0, "WM_NCMOUSEMOVE" },
			{ 0x00a1, "WM_NCLBUTTONDOWN" },
			{ 0x00a2, "WM_NCLBUTTONUP" },
			{ 0x00a3, "WM_NCLBUTTONDBLCLK" },
			{ 0x00a4, "WM_NCRBUTTONDOWN" },
			{ 0x00a5, "WM_NCRBUTTONUP" },
			{ 0x00a6, "WM_NCRBUTTONDBLCLK" },
			{ 0x00a7, "WM_NCMBUTTONDOWN" },
			{ 0x00a8, "WM_NCMBUTTONUP" },
			{ 0x00a9, "WM_NCMBUTTONDBLCLK" },
			{ 0x00ab, "WM_NCXBUTTONDOWN" },
			{ 0x00ac, "WM_NCXBUTTONUP" },
			{ 0x00ad, "WM_NCXBUTTONDBLCLK" },
			{ 0x00b0, "EM_GETSEL" },
			{ 0x00b1, "EM_SETSEL" },
			{ 0x00b2, "EM_GETRECT" },
			{ 0x00b3, "EM_SETRECT" },
			{ 0x00b4, "EM_SETRECTNP" },
			{ 0x00b5, "EM_SCROLL" },
			{ 0x00b6, "EM_LINESCROLL" },
			{ 0x00b7, "EM_SCROLLCARET" },
			{ 0x00b8, "EM_GETMODIFY" },
			{ 0x00b9, "EM_SETMODIFY" },
			{ 0x00ba, "EM_GETLINECOUNT" },
			{ 0x00bb, "EM_LINEINDEX" },
			{ 0x00bc, "EM_SETHANDLE" },
			{ 0x00bd, "EM_GETHANDLE" },
			{ 0x00be, "EM_GETTHUMB" },
			{ 0x00c1, "EM_LINELENGTH" },
			{ 0x00c2, "EM_REPLACESEL" },
			{ 0x00c3, "EM_SETFONT" },
			{ 0x00c4, "EM_GETLINE" },
			{ 0x00c5, "EM_LIMITTEXT" },
			{ 0x00c5, "EM_SETLIMITTEXT" },
			{ 0x00c6, "EM_CANUNDO" },
			{ 0x00c7, "EM_UNDO" },
			{ 0x00c8, "EM_FMTLINES" },
			{ 0x00c9, "EM_LINEFROMCHAR" },
			{ 0x00ca, "EM_SETWORDBREAK" },
			{ 0x00cb, "EM_SETTABSTOPS" },
			{ 0x00cc, "EM_SETPASSWORDCHAR" },
			{ 0x00cd, "EM_EMPTYUNDOBUFFER" },
			{ 0x00ce, "EM_GETFIRSTVISIBLELINE" },
			{ 0x00cf, "EM_SETREADONLY" },
			{ 0x00d0, "EM_SETWORDBREAKPROC" },
			{ 0x00d1, "EM_GETWORDBREAKPROC" },
			{ 0x00d2, "EM_GETPASSWORDCHAR" },
			{ 0x00d3, "EM_SETMARGINS" },
			{ 0x00d4, "EM_GETMARGINS" },
			{ 0x00d5, "EM_GETLIMITTEXT" },
			{ 0x00d6, "EM_POSFROMCHAR" },
			{ 0x00d7, "EM_CHARFROMPOS" },
			{ 0x00d8, "EM_SETIMESTATUS" },
			{ 0x00d9, "EM_GETIMESTATUS" },
			{ 0x00e0, "SBM_SETPOS" },
			{ 0x00e1, "SBM_GETPOS" },
			{ 0x00e2, "SBM_SETRANGE" },
			{ 0x00e3, "SBM_GETRANGE" },
			{ 0x00e4, "SBM_ENABLE_ARROWS" },
			{ 0x00e6, "SBM_SETRANGEREDRAW" },
			{ 0x00e9, "SBM_SETSCROLLINFO" },
			{ 0x00ea, "SBM_GETSCROLLINFO" },
			{ 0x00eb, "SBM_GETSCROLLBARINFO" },
			{ 0x00f0, "BM_GETCHECK" },
			{ 0x00f1, "BM_SETCHECK" },
			{ 0x00f2, "BM_GETSTATE" },
			{ 0x00f3, "BM_SETSTATE" },
			{ 0x00f4, "BM_SETSTYLE" },
			{ 0x00f5, "BM_CLICK" },
			{ 0x00f6, "BM_GETIMAGE" },
			{ 0x00f7, "BM_SETIMAGE" },
			{ 0x00f8, "BM_SETDONTCLICK" },
			{ 0x00ff, "WM_INPUT" },
			{ 0x0100, "WM_KEYDOWN" },
			{ 0x0100, "WM_KEYFIRST" },
			{ 0x0101, "WM_KEYUP" },
			{ 0x0102, "WM_CHAR" },
			{ 0x0103, "WM_DEADCHAR" },
			{ 0x0104, "WM_SYSKEYDOWN" },
			{ 0x0105, "WM_SYSKEYUP" },
			{ 0x0106, "WM_SYSCHAR" },
			{ 0x0107, "WM_SYSDEADCHAR" },
			{ 0x0108, "WM_KEYLAST" },
			{ 0x0109, "WM_UNICHAR" },
			{ 0x0109, "WM_WNT_CONVERTREQUESTEX" },
			{ 0x010a, "WM_CONVERTREQUEST" },
			{ 0x010b, "WM_CONVERTRESULT" },
			{ 0x010c, "WM_INTERIM" },
			{ 0x010d, "WM_IME_STARTCOMPOSITION" },
			{ 0x010e, "WM_IME_ENDCOMPOSITION" },
			{ 0x010f, "WM_IME_COMPOSITION" },
			{ 0x010f, "WM_IME_KEYLAST" },
			{ 0x0110, "WM_INITDIALOG" },
			{ 0x0111, "WM_COMMAND" },
			{ 0x0112, "WM_SYSCOMMAND" },
			{ 0x0113, "WM_TIMER" },
			{ 0x0114, "WM_HSCROLL" },
			{ 0x0115, "WM_VSCROLL" },
			{ 0x0116, "WM_INITMENU" },
			{ 0x0117, "WM_INITMENUPOPUP" },
			{ 0x0118, "WM_SYSTIMER" },
			{ 0x011f, "WM_MENUSELECT" },
			{ 0x0120, "WM_MENUCHAR" },
			{ 0x0121, "WM_ENTERIDLE" },
			{ 0x0122, "WM_MENURBUTTONUP" },
			{ 0x0123, "WM_MENUDRAG" },
			{ 0x0124, "WM_MENUGETOBJECT" },
			{ 0x0125, "WM_UNINITMENUPOPUP" },
			{ 0x0126, "WM_MENUCOMMAND" },
			{ 0x0127, "WM_CHANGEUISTATE" },
			{ 0x0128, "WM_UPDATEUISTATE" },
			{ 0x0129, "WM_QUERYUISTATE" },
			{ 0x0132, "WM_CTLCOLORMSGBOX" },
			{ 0x0133, "WM_CTLCOLOREDIT" },
			{ 0x0134, "WM_CTLCOLORLISTBOX" },
			{ 0x0135, "WM_CTLCOLORBTN" },
			{ 0x0136, "WM_CTLCOLORDLG" },
			{ 0x0137, "WM_CTLCOLORSCROLLBAR" },
			{ 0x0138, "WM_CTLCOLORSTATIC" },
			{ 0x0200, "WM_MOUSEFIRST" },
			{ 0x0200, "WM_MOUSEMOVE" },
			{ 0x0201, "WM_LBUTTONDOWN" },
			{ 0x0202, "WM_LBUTTONUP" },
			{ 0x0203, "WM_LBUTTONDBLCLK" },
			{ 0x0204, "WM_RBUTTONDOWN" },
			{ 0x0205, "WM_RBUTTONUP" },
			{ 0x0206, "WM_RBUTTONDBLCLK" },
			{ 0x0207, "WM_MBUTTONDOWN" },
			{ 0x0208, "WM_MBUTTONUP" },
			{ 0x0209, "WM_MBUTTONDBLCLK" },
			{ 0x0209, "WM_MOUSELAST" },
			{ 0x020a, "WM_MOUSEWHEEL" },
			{ 0x020b, "WM_XBUTTONDOWN" },
			{ 0x020c, "WM_XBUTTONUP" },
			{ 0x020d, "WM_XBUTTONDBLCLK" },
			{ 0x0210, "WM_PARENTNOTIFY" },
			{ 0x0211, "WM_ENTERMENULOOP" },
			{ 0x0212, "WM_EXITMENULOOP" },
			{ 0x0213, "WM_NEXTMENU" },
			{ 0x0214, "WM_SIZING" },
			{ 0x0215, "WM_CAPTURECHANGED" },
			{ 0x0216, "WM_MOVING" },
			{ 0x0218, "WM_POWERBROADCAST" },
			{ 0x0219, "WM_DEVICECHANGE" },
			{ 0x0220, "WM_MDICREATE" },
			{ 0x0221, "WM_MDIDESTROY" },
			{ 0x0222, "WM_MDIACTIVATE" },
			{ 0x0223, "WM_MDIRESTORE" },
			{ 0x0224, "WM_MDINEXT" },
			{ 0x0225, "WM_MDIMAXIMIZE" },
			{ 0x0226, "WM_MDITILE" },
			{ 0x0227, "WM_MDICASCADE" },
			{ 0x0228, "WM_MDIICONARRANGE" },
			{ 0x0229, "WM_MDIGETACTIVE" },
			{ 0x0230, "WM_MDISETMENU" },
			{ 0x0231, "WM_ENTERSIZEMOVE" },
			{ 0x0232, "WM_EXITSIZEMOVE" },
			{ 0x0233, "WM_DROPFILES" },
			{ 0x0234, "WM_MDIREFRESHMENU" },
			{ 0x0280, "WM_IME_REPORT" },
			{ 0x0281, "WM_IME_SETCONTEXT" },
			{ 0x0282, "WM_IME_NOTIFY" },
			{ 0x0283, "WM_IME_CONTROL" },
			{ 0x0284, "WM_IME_COMPOSITIONFULL" },
			{ 0x0285, "WM_IME_SELECT" },
			{ 0x0286, "WM_IME_CHAR" },
			{ 0x0288, "WM_IME_REQUEST" },
			{ 0x0290, "WM_IMEKEYDOWN" },
			{ 0x0290, "WM_IME_KEYDOWN" },
			{ 0x0291, "WM_IMEKEYUP" },
			{ 0x0291, "WM_IME_KEYUP" },
			{ 0x02a0, "WM_NCMOUSEHOVER" },
			{ 0x02a1, "WM_MOUSEHOVER" },
			{ 0x02a2, "WM_NCMOUSELEAVE" },
			{ 0x02a3, "WM_MOUSELEAVE" },
			{ 0x0300, "WM_CUT" },
			{ 0x0301, "WM_COPY" },
			{ 0x0302, "WM_PASTE" },
			{ 0x0303, "WM_CLEAR" },
			{ 0x0304, "WM_UNDO" },
			{ 0x0305, "WM_RENDERFORMAT" },
			{ 0x0306, "WM_RENDERALLFORMATS" },
			{ 0x0307, "WM_DESTROYCLIPBOARD" },
			{ 0x0308, "WM_DRAWCLIPBOARD" },
			{ 0x0309, "WM_PAINTCLIPBOARD" },
			{ 0x030a, "WM_VSCROLLCLIPBOARD" },
			{ 0x030b, "WM_SIZECLIPBOARD" },
			{ 0x030c, "WM_ASKCBFORMATNAME" },
			{ 0x030d, "WM_CHANGECBCHAIN" },
			{ 0x030e, "WM_HSCROLLCLIPBOARD" },
			{ 0x030f, "WM_QUERYNEWPALETTE" },
			{ 0x0310, "WM_PALETTEISCHANGING" },
			{ 0x0311, "WM_PALETTECHANGED" },
			{ 0x0312, "WM_HOTKEY" },
			{ 0x0317, "WM_PRINT" },
			{ 0x0318, "WM_PRINTCLIENT" },
			{ 0x0319, "WM_APPCOMMAND" },
			{ 0x0358, "WM_HANDHELDFIRST" },
			{ 0x035f, "WM_HANDHELDLAST" },
			{ 0x0360, "WM_AFXFIRST" },
			{ 0x037f, "WM_AFXLAST" },
			{ 0x0380, "WM_PENWINFIRST" },
			{ 0x0381, "WM_RCRESULT" },
			{ 0x0382, "WM_HOOKRCRESULT" },
			{ 0x0383, "WM_GLOBALRCCHANGE" },
			{ 0x0383, "WM_PENMISCINFO" },
			{ 0x0384, "WM_SKB" },
			{ 0x0385, "WM_HEDITCTL" },
			{ 0x0385, "WM_PENCTL" },
			{ 0x0386, "WM_PENMISC" },
			{ 0x0387, "WM_CTLINIT" },
			{ 0x0388, "WM_PENEVENT" },
			{ 0x038f, "WM_PENWINLAST" },
			{ 0x0400, "DDM_SETFMT" },
			{ 0x0400, "DM_GETDEFID" },
			{ 0x0400, "NIN_SELECT" },
			{ 0x0400, "TBM_GETPOS" },
			{ 0x0400, "WM_PSD_PAGESETUPDLG" },
			{ 0x0400, "WM_USER" },
			{ 0x0401, "CBEM_INSERTITEMA" },
			{ 0x0401, "DDM_DRAW" },
			{ 0x0401, "DM_SETDEFID" },
			{ 0x0401, "HKM_SETHOTKEY" },
			{ 0x0401, "PBM_SETRANGE" },
			{ 0x0401, "RB_INSERTBANDA" },
			{ 0x0401, "SB_SETTEXTA" },
			{ 0x0401, "TB_ENABLEBUTTON" },
			{ 0x0401, "TBM_GETRANGEMIN" },
			{ 0x0401, "TTM_ACTIVATE" },
			{ 0x0401, "WM_CHOOSEFONT_GETLOGFONT" },
			{ 0x0401, "WM_PSD_FULLPAGERECT" },
			{ 0x0402, "CBEM_SETIMAGELIST" },
			{ 0x0402, "DDM_CLOSE" },
			{ 0x0402, "DM_REPOSITION" },
			{ 0x0402, "HKM_GETHOTKEY" },
			{ 0x0402, "PBM_SETPOS" },
			{ 0x0402, "RB_DELETEBAND" },
			{ 0x0402, "SB_GETTEXTA" },
			{ 0x0402, "TB_CHECKBUTTON" },
			{ 0x0402, "TBM_GETRANGEMAX" },
			{ 0x0402, "WM_PSD_MINMARGINRECT" },
			{ 0x0403, "CBEM_GETIMAGELIST" },
			{ 0x0403, "DDM_BEGIN" },
			{ 0x0403, "HKM_SETRULES" },
			{ 0x0403, "PBM_DELTAPOS" },
			{ 0x0403, "RB_GETBARINFO" },
			{ 0x0403, "SB_GETTEXTLENGTHA" },
			{ 0x0403, "TBM_GETTIC" },
			{ 0x0403, "TB_PRESSBUTTON" },
			{ 0x0403, "TTM_SETDELAYTIME" },
			{ 0x0403, "WM_PSD_MARGINRECT" },
			{ 0x0404, "CBEM_GETITEMA" },
			{ 0x0404, "DDM_END" },
			{ 0x0404, "PBM_SETSTEP" },
			{ 0x0404, "RB_SETBARINFO" },
			{ 0x0404, "SB_SETPARTS" },
			{ 0x0404, "TB_HIDEBUTTON" },
			{ 0x0404, "TBM_SETTIC" },
			{ 0x0404, "TTM_ADDTOOLA" },
			{ 0x0404, "WM_PSD_GREEKTEXTRECT" },
			{ 0x0405, "CBEM_SETITEMA" },
			{ 0x0405, "PBM_STEPIT" },
			{ 0x0405, "TB_INDETERMINATE" },
			{ 0x0405, "TBM_SETPOS" },
			{ 0x0405, "TTM_DELTOOLA" },
			{ 0x0405, "WM_PSD_ENVSTAMPRECT" },
			{ 0x0406, "CBEM_GETCOMBOCONTROL" },
			{ 0x0406, "PBM_SETRANGE32" },
			{ 0x0406, "RB_SETBANDINFOA" },
			{ 0x0406, "SB_GETPARTS" },
			{ 0x0406, "TB_MARKBUTTON" },
			{ 0x0406, "TBM_SETRANGE" },
			{ 0x0406, "TTM_NEWTOOLRECTA" },
			{ 0x0406, "WM_PSD_YAFULLPAGERECT" },
			{ 0x0407, "CBEM_GETEDITCONTROL" },
			{ 0x0407, "PBM_GETRANGE" },
			{ 0x0407, "RB_SETPARENT" },
			{ 0x0407, "SB_GETBORDERS" },
			{ 0x0407, "TBM_SETRANGEMIN" },
			{ 0x0407, "TTM_RELAYEVENT" },
			{ 0x0408, "CBEM_SETEXSTYLE" },
			{ 0x0408, "PBM_GETPOS" },
			{ 0x0408, "RB_HITTEST" },
			{ 0x0408, "SB_SETMINHEIGHT" },
			{ 0x0408, "TBM_SETRANGEMAX" },
			{ 0x0408, "TTM_GETTOOLINFOA" },
			{ 0x0409, "CBEM_GETEXSTYLE" },
			{ 0x0409, "CBEM_GETEXTENDEDSTYLE" },
			{ 0x0409, "PBM_SETBARCOLOR" },
			{ 0x0409, "RB_GETRECT" },
			{ 0x0409, "SB_SIMPLE" },
			{ 0x0409, "TB_ISBUTTONENABLED" },
			{ 0x0409, "TBM_CLEARTICS" },
			{ 0x0409, "TTM_SETTOOLINFOA" },
			{ 0x040a, "CBEM_HASEDITCHANGED" },
			{ 0x040a, "RB_INSERTBANDW" },
			{ 0x040a, "SB_GETRECT" },
			{ 0x040a, "TB_ISBUTTONCHECKED" },
			{ 0x040a, "TBM_SETSEL" },
			{ 0x040a, "TTM_HITTESTA" },
			{ 0x040a, "WIZ_QUERYNUMPAGES" },
			{ 0x040b, "CBEM_INSERTITEMW" },
			{ 0x040b, "RB_SETBANDINFOW" },
			{ 0x040b, "SB_SETTEXTW" },
			{ 0x040b, "TB_ISBUTTONPRESSED" },
			{ 0x040b, "TBM_SETSELSTART" },
			{ 0x040b, "TTM_GETTEXTA" },
			{ 0x040b, "WIZ_NEXT" },
			{ 0x040c, "CBEM_SETITEMW" },
			{ 0x040c, "RB_GETBANDCOUNT" },
			{ 0x040c, "SB_GETTEXTLENGTHW" },
			{ 0x040c, "TB_ISBUTTONHIDDEN" },
			{ 0x040c, "TBM_SETSELEND" },
			{ 0x040c, "TTM_UPDATETIPTEXTA" },
			{ 0x040c, "WIZ_PREV" },
			{ 0x040d, "CBEM_GETITEMW" },
			{ 0x040d, "RB_GETROWCOUNT" },
			{ 0x040d, "SB_GETTEXTW" },
			{ 0x040d, "TB_ISBUTTONINDETERMINATE" },
			{ 0x040d, "TTM_GETTOOLCOUNT" },
			{ 0x040e, "CBEM_SETEXTENDEDSTYLE" },
			{ 0x040e, "RB_GETROWHEIGHT" },
			{ 0x040e, "SB_ISSIMPLE" },
			{ 0x040e, "TB_ISBUTTONHIGHLIGHTED" },
			{ 0x040e, "TBM_GETPTICS" },
			{ 0x040e, "TTM_ENUMTOOLSA" },
			{ 0x040f, "SB_SETICON" },
			{ 0x040f, "TBM_GETTICPOS" },
			{ 0x040f, "TTM_GETCURRENTTOOLA" },
			{ 0x0410, "RB_IDTOINDEX" },
			{ 0x0410, "SB_SETTIPTEXTA" },
			{ 0x0410, "TBM_GETNUMTICS" },
			{ 0x0410, "TTM_WINDOWFROMPOINT" },
			{ 0x0411, "RB_GETTOOLTIPS" },
			{ 0x0411, "SB_SETTIPTEXTW" },
			{ 0x0411, "TBM_GETSELSTART" },
			{ 0x0411, "TB_SETSTATE" },
			{ 0x0411, "TTM_TRACKACTIVATE" },
			{ 0x0412, "RB_SETTOOLTIPS" },
			{ 0x0412, "SB_GETTIPTEXTA" },
			{ 0x0412, "TB_GETSTATE" },
			{ 0x0412, "TBM_GETSELEND" },
			{ 0x0412, "TTM_TRACKPOSITION" },
			{ 0x0413, "RB_SETBKCOLOR" },
			{ 0x0413, "SB_GETTIPTEXTW" },
			{ 0x0413, "TB_ADDBITMAP" },
			{ 0x0413, "TBM_CLEARSEL" },
			{ 0x0413, "TTM_SETTIPBKCOLOR" },
			{ 0x0414, "RB_GETBKCOLOR" },
			{ 0x0414, "SB_GETICON" },
			{ 0x0414, "TB_ADDBUTTONSA" },
			{ 0x0414, "TBM_SETTICFREQ" },
			{ 0x0414, "TTM_SETTIPTEXTCOLOR" },
			{ 0x0415, "RB_SETTEXTCOLOR" },
			{ 0x0415, "TB_INSERTBUTTONA" },
			{ 0x0415, "TBM_SETPAGESIZE" },
			{ 0x0415, "TTM_GETDELAYTIME" },
			{ 0x0416, "RB_GETTEXTCOLOR" },
			{ 0x0416, "TB_DELETEBUTTON" },
			{ 0x0416, "TBM_GETPAGESIZE" },
			{ 0x0416, "TTM_GETTIPBKCOLOR" },
			{ 0x0417, "RB_SIZETORECT" },
			{ 0x0417, "TB_GETBUTTON" },
			{ 0x0417, "TBM_SETLINESIZE" },
			{ 0x0417, "TTM_GETTIPTEXTCOLOR" },
			{ 0x0418, "RB_BEGINDRAG" },
			{ 0x0418, "TB_BUTTONCOUNT" },
			{ 0x0418, "TBM_GETLINESIZE" },
			{ 0x0418, "TTM_SETMAXTIPWIDTH" },
			{ 0x0419, "RB_ENDDRAG" },
			{ 0x0419, "TB_COMMANDTOINDEX" },
			{ 0x0419, "TBM_GETTHUMBRECT" },
			{ 0x0419, "TTM_GETMAXTIPWIDTH" },
			{ 0x041a, "RB_DRAGMOVE" },
			{ 0x041a, "TBM_GETCHANNELRECT" },
			{ 0x041a, "TB_SAVERESTOREA" },
			{ 0x041a, "TTM_SETMARGIN" },
			{ 0x041b, "RB_GETBARHEIGHT" },
			{ 0x041b, "TB_CUSTOMIZE" },
			{ 0x041b, "TBM_SETTHUMBLENGTH" },
			{ 0x041b, "TTM_GETMARGIN" },
			{ 0x041c, "RB_GETBANDINFOW" },
			{ 0x041c, "TB_ADDSTRINGA" },
			{ 0x041c, "TBM_GETTHUMBLENGTH" },
			{ 0x041c, "TTM_POP" },
			{ 0x041d, "RB_GETBANDINFOA" },
			{ 0x041d, "TB_GETITEMRECT" },
			{ 0x041d, "TBM_SETTOOLTIPS" },
			{ 0x041d, "TTM_UPDATE" },
			{ 0x041e, "RB_MINIMIZEBAND" },
			{ 0x041e, "TB_BUTTONSTRUCTSIZE" },
			{ 0x041e, "TBM_GETTOOLTIPS" },
			{ 0x041e, "TTM_GETBUBBLESIZE" },
			{ 0x041f, "RB_MAXIMIZEBAND" },
			{ 0x041f, "TBM_SETTIPSIDE" },
			{ 0x041f, "TB_SETBUTTONSIZE" },
			{ 0x041f, "TTM_ADJUSTRECT" },
			{ 0x0420, "TBM_SETBUDDY" },
			{ 0x0420, "TB_SETBITMAPSIZE" },
			{ 0x0420, "TTM_SETTITLEA" },
			{ 0x0421, "MSG_FTS_JUMP_VA" },
			{ 0x0421, "TB_AUTOSIZE" },
			{ 0x0421, "TBM_GETBUDDY" },
			{ 0x0421, "TTM_SETTITLEW" },
			{ 0x0422, "RB_GETBANDBORDERS" },
			{ 0x0423, "MSG_FTS_JUMP_QWORD" },
			{ 0x0423, "RB_SHOWBAND" },
			{ 0x0423, "TB_GETTOOLTIPS" },
			{ 0x0424, "MSG_REINDEX_REQUEST" },
			{ 0x0424, "TB_SETTOOLTIPS" },
			{ 0x0425, "MSG_FTS_WHERE_IS_IT" },
			{ 0x0425, "RB_SETPALETTE" },
			{ 0x0425, "TB_SETPARENT" },
			{ 0x0426, "RB_GETPALETTE" },
			{ 0x0427, "RB_MOVEBAND" },
			{ 0x0427, "TB_SETROWS" },
			{ 0x0428, "TB_GETROWS" },
			{ 0x0429, "TB_GETBITMAPFLAGS" },
			{ 0x042a, "TB_SETCMDID" },
			{ 0x042b, "RB_PUSHCHEVRON" },
			{ 0x042b, "TB_CHANGEBITMAP" },
			{ 0x042c, "TB_GETBITMAP" },
			{ 0x042d, "MSG_GET_DEFFONT" },
			{ 0x042d, "TB_GETBUTTONTEXTA" },
			{ 0x042e, "TB_REPLACEBITMAP" },
			{ 0x042f, "TB_SETINDENT" },
			{ 0x0430, "TB_SETIMAGELIST" },
			{ 0x0431, "TB_GETIMAGELIST" },
			{ 0x0432, "TB_LOADIMAGES" },
			{ 0x0432, "EM_CANPASTE" },
			{ 0x0432, "TTM_ADDTOOLW" },
			{ 0x0433, "EM_DISPLAYBAND" },
			{ 0x0433, "TB_GETRECT" },
			{ 0x0433, "TTM_DELTOOLW" },
			{ 0x0434, "EM_EXGETSEL" },
			{ 0x0434, "TB_SETHOTIMAGELIST" },
			{ 0x0434, "TTM_NEWTOOLRECTW" },
			{ 0x0435, "EM_EXLIMITTEXT" },
			{ 0x0435, "TB_GETHOTIMAGELIST" },
			{ 0x0435, "TTM_GETTOOLINFOW" },
			{ 0x0436, "EM_EXLINEFROMCHAR" },
			{ 0x0436, "TB_SETDISABLEDIMAGELIST" },
			{ 0x0436, "TTM_SETTOOLINFOW" },
			{ 0x0437, "EM_EXSETSEL" },
			{ 0x0437, "TB_GETDISABLEDIMAGELIST" },
			{ 0x0437, "TTM_HITTESTW" },
			{ 0x0438, "EM_FINDTEXT" },
			{ 0x0438, "TB_SETSTYLE" },
			{ 0x0438, "TTM_GETTEXTW" },
			{ 0x0439, "EM_FORMATRANGE" },
			{ 0x0439, "TB_GETSTYLE" },
			{ 0x0439, "TTM_UPDATETIPTEXTW" },
			{ 0x043a, "EM_GETCHARFORMAT" },
			{ 0x043a, "TB_GETBUTTONSIZE" },
			{ 0x043a, "TTM_ENUMTOOLSW" },
			{ 0x043b, "EM_GETEVENTMASK" },
			{ 0x043b, "TB_SETBUTTONWIDTH" },
			{ 0x043b, "TTM_GETCURRENTTOOLW" },
			{ 0x043c, "EM_GETOLEINTERFACE" },
			{ 0x043c, "TB_SETMAXTEXTROWS" },
			{ 0x043d, "EM_GETPARAFORMAT" },
			{ 0x043d, "TB_GETTEXTROWS" },
			{ 0x043e, "EM_GETSELTEXT" },
			{ 0x043e, "TB_GETOBJECT" },
			{ 0x043f, "EM_HIDESELECTION" },
			{ 0x043f, "TB_GETBUTTONINFOW" },
			{ 0x0440, "EM_PASTESPECIAL" },
			{ 0x0440, "TB_SETBUTTONINFOW" },
			{ 0x0441, "EM_REQUESTRESIZE" },
			{ 0x0441, "TB_GETBUTTONINFOA" },
			{ 0x0442, "EM_SELECTIONTYPE" },
			{ 0x0442, "TB_SETBUTTONINFOA" },
			{ 0x0443, "EM_SETBKGNDCOLOR" },
			{ 0x0443, "TB_INSERTBUTTONW" },
			{ 0x0444, "EM_SETCHARFORMAT" },
			{ 0x0444, "TB_ADDBUTTONSW" },
			{ 0x0445, "EM_SETEVENTMASK" },
			{ 0x0445, "TB_HITTEST" },
			{ 0x0446, "EM_SETOLECALLBACK" },
			{ 0x0446, "TB_SETDRAWTEXTFLAGS" },
			{ 0x0447, "EM_SETPARAFORMAT" },
			{ 0x0447, "TB_GETHOTITEM" },
			{ 0x0448, "EM_SETTARGETDEVICE" },
			{ 0x0448, "TB_SETHOTITEM" },
			{ 0x0449, "EM_STREAMIN" },
			{ 0x0449, "TB_SETANCHORHIGHLIGHT" },
			{ 0x044a, "EM_STREAMOUT" },
			{ 0x044a, "TB_GETANCHORHIGHLIGHT" },
			{ 0x044b, "EM_GETTEXTRANGE" },
			{ 0x044b, "TB_GETBUTTONTEXTW" },
			{ 0x044c, "EM_FINDWORDBREAK" },
			{ 0x044c, "TB_SAVERESTOREW" },
			{ 0x044d, "EM_SETOPTIONS" },
			{ 0x044d, "TB_ADDSTRINGW" },
			{ 0x044e, "EM_GETOPTIONS" },
			{ 0x044e, "TB_MAPACCELERATORA" },
			{ 0x044f, "EM_FINDTEXTEX" },
			{ 0x044f, "TB_GETINSERTMARK" },
			{ 0x0450, "EM_GETWORDBREAKPROCEX" },
			{ 0x0450, "TB_SETINSERTMARK" },
			{ 0x0451, "EM_SETWORDBREAKPROCEX" },
			{ 0x0451, "TB_INSERTMARKHITTEST" },
			{ 0x0452, "EM_SETUNDOLIMIT" },
			{ 0x0452, "TB_MOVEBUTTON" },
			{ 0x0453, "TB_GETMAXSIZE" },
			{ 0x0454, "EM_REDO" },
			{ 0x0454, "TB_SETEXTENDEDSTYLE" },
			{ 0x0455, "EM_CANREDO" },
			{ 0x0455, "TB_GETEXTENDEDSTYLE" },
			{ 0x0456, "EM_GETUNDONAME" },
			{ 0x0456, "TB_GETPADDING" },
			{ 0x0457, "EM_GETREDONAME" },
			{ 0x0457, "TB_SETPADDING" },
			{ 0x0458, "EM_STOPGROUPTYPING" },
			{ 0x0458, "TB_SETINSERTMARKCOLOR" },
			{ 0x0459, "EM_SETTEXTMODE" },
			{ 0x0459, "TB_GETINSERTMARKCOLOR" },
			{ 0x045a, "EM_GETTEXTMODE" },
			{ 0x045a, "TB_MAPACCELERATORW" },
			{ 0x045b, "EM_AUTOURLDETECT" },
			{ 0x045b, "TB_GETSTRINGW" },
			{ 0x045c, "EM_GETAUTOURLDETECT" },
			{ 0x045c, "TB_GETSTRINGA" },
			{ 0x045d, "EM_SETPALETTE" },
			{ 0x045e, "EM_GETTEXTEX" },
			{ 0x045f, "EM_GETTEXTLENGTHEX" },
			{ 0x0460, "EM_SHOWSCROLLBAR" },
			{ 0x0461, "EM_SETTEXTEX" },
			{ 0x0463, "TAPI_REPLY" },
			{ 0x0464, "ACM_OPENA" },
			{ 0x0464, "BFFM_SETSTATUSTEXTA" },
			{ 0x0464, "CDM_FIRST" },
			{ 0x0464, "CDM_GETSPEC" },
			{ 0x0464, "EM_SETPUNCTUATION" },
			{ 0x0464, "IPM_CLEARADDRESS" },
			{ 0x0464, "WM_CAP_UNICODE_START" },
			{ 0x0465, "ACM_PLAY" },
			{ 0x0465, "BFFM_ENABLEOK" },
			{ 0x0465, "CDM_GETFILEPATH" },
			{ 0x0465, "EM_GETPUNCTUATION" },
			{ 0x0465, "IPM_SETADDRESS" },
			{ 0x0465, "PSM_SETCURSEL" },
			{ 0x0465, "UDM_SETRANGE" },
			{ 0x0465, "WM_CHOOSEFONT_SETLOGFONT" },
			{ 0x0466, "ACM_STOP" },
			{ 0x0466, "BFFM_SETSELECTIONA" },
			{ 0x0466, "CDM_GETFOLDERPATH" },
			{ 0x0466, "EM_SETWORDWRAPMODE" },
			{ 0x0466, "IPM_GETADDRESS" },
			{ 0x0466, "PSM_REMOVEPAGE" },
			{ 0x0466, "UDM_GETRANGE" },
			{ 0x0466, "WM_CAP_SET_CALLBACK_ERRORW" },
			{ 0x0466, "WM_CHOOSEFONT_SETFLAGS" },
			{ 0x0467, "ACM_OPENW" },
			{ 0x0467, "BFFM_SETSELECTIONW" },
			{ 0x0467, "CDM_GETFOLDERIDLIST" },
			{ 0x0467, "EM_GETWORDWRAPMODE" },
			{ 0x0467, "IPM_SETRANGE" },
			{ 0x0467, "PSM_ADDPAGE" },
			{ 0x0467, "UDM_SETPOS" },
			{ 0x0467, "WM_CAP_SET_CALLBACK_STATUSW" },
			{ 0x0468, "BFFM_SETSTATUSTEXTW" },
			{ 0x0468, "CDM_SETCONTROLTEXT" },
			{ 0x0468, "EM_SETIMECOLOR" },
			{ 0x0468, "IPM_SETFOCUS" },
			{ 0x0468, "PSM_CHANGED" },
			{ 0x0468, "UDM_GETPOS" },
			{ 0x0469, "CDM_HIDECONTROL" },
			{ 0x0469, "EM_GETIMECOLOR" },
			{ 0x0469, "IPM_ISBLANK" },
			{ 0x0469, "PSM_RESTARTWINDOWS" },
			{ 0x0469, "UDM_SETBUDDY" },
			{ 0x046a, "CDM_SETDEFEXT" },
			{ 0x046a, "EM_SETIMEOPTIONS" },
			{ 0x046a, "PSM_REBOOTSYSTEM" },
			{ 0x046a, "UDM_GETBUDDY" },
			{ 0x046b, "EM_GETIMEOPTIONS" },
			{ 0x046b, "PSM_CANCELTOCLOSE" },
			{ 0x046b, "UDM_SETACCEL" },
			{ 0x046c, "EM_CONVPOSITION" },
			{ 0x046c, "EM_CONVPOSITION" },
			{ 0x046c, "PSM_QUERYSIBLINGS" },
			{ 0x046c, "UDM_GETACCEL" },
			{ 0x046d, "MCIWNDM_GETZOOM" },
			{ 0x046d, "PSM_UNCHANGED" },
			{ 0x046d, "UDM_SETBASE" },
			{ 0x046e, "PSM_APPLY" },
			{ 0x046e, "UDM_GETBASE" },
			{ 0x046f, "PSM_SETTITLEA" },
			{ 0x046f, "UDM_SETRANGE32" },
			{ 0x0470, "PSM_SETWIZBUTTONS" },
			{ 0x0470, "UDM_GETRANGE32" },
			{ 0x0470, "WM_CAP_DRIVER_GET_NAMEW" },
			{ 0x0471, "PSM_PRESSBUTTON" },
			{ 0x0471, "UDM_SETPOS32" },
			{ 0x0471, "WM_CAP_DRIVER_GET_VERSIONW" },
			{ 0x0472, "PSM_SETCURSELID" },
			{ 0x0472, "UDM_GETPOS32" },
			{ 0x0473, "PSM_SETFINISHTEXTA" },
			{ 0x0474, "PSM_GETTABCONTROL" },
			{ 0x0475, "PSM_ISDIALOGMESSAGE" },
			{ 0x0476, "MCIWNDM_REALIZE" },
			{ 0x0476, "PSM_GETCURRENTPAGEHWND" },
			{ 0x0477, "MCIWNDM_SETTIMEFORMATA" },
			{ 0x0477, "PSM_INSERTPAGE" },
			{ 0x0478, "EM_SETLANGOPTIONS" },
			{ 0x0478, "MCIWNDM_GETTIMEFORMATA" },
			{ 0x0478, "PSM_SETTITLEW" },
			{ 0x0478, "WM_CAP_FILE_SET_CAPTURE_FILEW" },
			{ 0x0479, "EM_GETLANGOPTIONS" },
			{ 0x0479, "MCIWNDM_VALIDATEMEDIA" },
			{ 0x0479, "PSM_SETFINISHTEXTW" },
			{ 0x0479, "WM_CAP_FILE_GET_CAPTURE_FILEW" },
			{ 0x047a, "EM_GETIMECOMPMODE" },
			{ 0x047b, "EM_FINDTEXTW" },
			{ 0x047b, "MCIWNDM_PLAYTO" },
			{ 0x047b, "WM_CAP_FILE_SAVEASW" },
			{ 0x047c, "EM_FINDTEXTEXW" },
			{ 0x047c, "MCIWNDM_GETFILENAMEA" },
			{ 0x047d, "EM_RECONVERSION" },
			{ 0x047d, "MCIWNDM_GETDEVICEA" },
			{ 0x047d, "PSM_SETHEADERTITLEA" },
			{ 0x047d, "WM_CAP_FILE_SAVEDIBW" },
			{ 0x047e, "EM_SETIMEMODEBIAS" },
			{ 0x047e, "MCIWNDM_GETPALETTE" },
			{ 0x047e, "PSM_SETHEADERTITLEW" },
			{ 0x047f, "EM_GETIMEMODEBIAS" },
			{ 0x047f, "MCIWNDM_SETPALETTE" },
			{ 0x047f, "PSM_SETHEADERSUBTITLEA" },
			{ 0x0480, "MCIWNDM_GETERRORA" },
			{ 0x0480, "PSM_SETHEADERSUBTITLEW" },
			{ 0x0481, "PSM_HWNDTOINDEX" },
			{ 0x0482, "PSM_INDEXTOHWND" },
			{ 0x0483, "MCIWNDM_SETINACTIVETIMER" },
			{ 0x0483, "PSM_PAGETOINDEX" },
			{ 0x0484, "PSM_INDEXTOPAGE" },
			{ 0x0485, "DL_BEGINDRAG" },
			{ 0x0485, "MCIWNDM_GETINACTIVETIMER" },
			{ 0x0485, "PSM_IDTOINDEX" },
			{ 0x0486, "DL_DRAGGING" },
			{ 0x0486, "PSM_INDEXTOID" },
			{ 0x0487, "DL_DROPPED" },
			{ 0x0487, "PSM_GETRESULT" },
			{ 0x0488, "DL_CANCELDRAG" },
			{ 0x0488, "PSM_RECALCPAGESIZES" },
			{ 0x048c, "MCIWNDM_GET_SOURCE" },
			{ 0x048d, "MCIWNDM_PUT_SOURCE" },
			{ 0x048e, "MCIWNDM_GET_DEST" },
			{ 0x048f, "MCIWNDM_PUT_DEST" },
			{ 0x0490, "MCIWNDM_CAN_PLAY" },
			{ 0x0491, "MCIWNDM_CAN_WINDOW" },
			{ 0x0492, "MCIWNDM_CAN_RECORD" },
			{ 0x0493, "MCIWNDM_CAN_SAVE" },
			{ 0x0494, "MCIWNDM_CAN_EJECT" },
			{ 0x0495, "MCIWNDM_CAN_CONFIG" },
			{ 0x0496, "IE_GETINK" },
			{ 0x0496, "IE_MSGFIRST" },
			{ 0x0496, "MCIWNDM_PALETTEKICK" },
			{ 0x0497, "IE_SETINK" },
			{ 0x0498, "IE_GETPENTIP" },
			{ 0x0499, "IE_SETPENTIP" },
			{ 0x049a, "IE_GETERASERTIP" },
			{ 0x049b, "IE_SETERASERTIP" },
			{ 0x049c, "IE_GETBKGND" },
			{ 0x049d, "IE_SETBKGND" },
			{ 0x049e, "IE_GETGRIDORIGIN" },
			{ 0x049f, "IE_SETGRIDORIGIN" },
			{ 0x04a0, "IE_GETGRIDPEN" },
			{ 0x04a1, "IE_SETGRIDPEN" },
			{ 0x04a2, "IE_GETGRIDSIZE" },
			{ 0x04a3, "IE_SETGRIDSIZE" },
			{ 0x04a4, "IE_GETMODE" },
			{ 0x04a5, "IE_SETMODE" },
			{ 0x04a6, "IE_GETINKRECT" },
			{ 0x04a6, "WM_CAP_SET_MCI_DEVICEW" },
			{ 0x04a7, "WM_CAP_GET_MCI_DEVICEW" },
			{ 0x04b4, "WM_CAP_PAL_OPENW" },
			{ 0x04b5, "WM_CAP_PAL_SAVEW" },
			{ 0x04b8, "IE_GETAPPDATA" },
			{ 0x04b9, "IE_SETAPPDATA" },
			{ 0x04ba, "IE_GETDRAWOPTS" },
			{ 0x04bb, "IE_SETDRAWOPTS" },
			{ 0x04bc, "IE_GETFORMAT" },
			{ 0x04bd, "IE_SETFORMAT" },
			{ 0x04be, "IE_GETINKINPUT" },
			{ 0x04bf, "IE_SETINKINPUT" },
			{ 0x04c0, "IE_GETNOTIFY" },
			{ 0x04c1, "IE_SETNOTIFY" },
			{ 0x04c2, "IE_GETRECOG" },
			{ 0x04c3, "IE_SETRECOG" },
			{ 0x04c4, "IE_GETSECURITY" },
			{ 0x04c5, "IE_SETSECURITY" },
			{ 0x04c6, "IE_GETSEL" },
			{ 0x04c7, "IE_SETSEL" },
			{ 0x04c8, "CDM_LAST" },
			{ 0x04c8, "EM_SETBIDIOPTIONS" },
			{ 0x04c8, "IE_DOCOMMAND" },
			{ 0x04c8, "MCIWNDM_NOTIFYMODE" },
			{ 0x04c9, "EM_GETBIDIOPTIONS" },
			{ 0x04c9, "IE_GETCOMMAND" },
			{ 0x04ca, "EM_SETTYPOGRAPHYOPTIONS" },
			{ 0x04ca, "IE_GETCOUNT" },
			{ 0x04cb, "EM_GETTYPOGRAPHYOPTIONS" },
			{ 0x04cb, "IE_GETGESTURE" },
			{ 0x04cb, "MCIWNDM_NOTIFYMEDIA" },
			{ 0x04cc, "EM_SETEDITSTYLE" },
			{ 0x04cc, "IE_GETMENU" },
			{ 0x04cd, "EM_GETEDITSTYLE" },
			{ 0x04cd, "IE_GETPAINTDC" },
			{ 0x04cd, "MCIWNDM_NOTIFYERROR" },
			{ 0x04ce, "IE_GETPDEVENT" },
			{ 0x04cf, "IE_GETSELCOUNT" },
			{ 0x04d0, "IE_GETSELITEMS" },
			{ 0x04d1, "IE_GETSTYLE" },
			{ 0x04db, "MCIWNDM_SETTIMEFORMATW" },
			{ 0x04dc, "EM_OUTLINE" },
			{ 0x04dc, "EM_OUTLINE" },
			{ 0x04dc, "MCIWNDM_GETTIMEFORMATW" },
			{ 0x04dd, "EM_GETSCROLLPOS" },
			{ 0x04dd, "EM_GETSCROLLPOS" },
			{ 0x04de, "EM_SETSCROLLPOS" },
			{ 0x04de, "EM_SETSCROLLPOS" },
			{ 0x04df, "EM_SETFONTSIZE" },
			{ 0x04df, "EM_SETFONTSIZE" },
			{ 0x04e0, "EM_GETZOOM" },
			{ 0x04e0, "MCIWNDM_GETFILENAMEW" },
			{ 0x04e1, "EM_SETZOOM" },
			{ 0x04e1, "MCIWNDM_GETDEVICEW" },
			{ 0x04e2, "EM_GETVIEWKIND" },
			{ 0x04e3, "EM_SETVIEWKIND" },
			{ 0x04e4, "EM_GETPAGE" },
			{ 0x04e4, "MCIWNDM_GETERRORW" },
			{ 0x04e5, "EM_SETPAGE" },
			{ 0x04e6, "EM_GETHYPHENATEINFO" },
			{ 0x04e7, "EM_SETHYPHENATEINFO" },
			{ 0x04eb, "EM_GETPAGEROTATE" },
			{ 0x04ec, "EM_SETPAGEROTATE" },
			{ 0x04ed, "EM_GETCTFMODEBIAS" },
			{ 0x04ee, "EM_SETCTFMODEBIAS" },
			{ 0x04f0, "EM_GETCTFOPENSTATUS" },
			{ 0x04f1, "EM_SETCTFOPENSTATUS" },
			{ 0x04f2, "EM_GETIMECOMPTEXT" },
			{ 0x04f3, "EM_ISIME" },
			{ 0x04f4, "EM_GETIMEPROPERTY" },
			{ 0x050d, "EM_GETQUERYRTFOBJ" },
			{ 0x050e, "EM_SETQUERYRTFOBJ" },
			{ 0x0600, "FM_GETFOCUS" },
			{ 0x0601, "FM_GETDRIVEINFOA" },
			{ 0x0602, "FM_GETSELCOUNT" },
			{ 0x0603, "FM_GETSELCOUNTLFN" },
			{ 0x0604, "FM_GETFILESELA" },
			{ 0x0605, "FM_GETFILESELLFNA" },
			{ 0x0606, "FM_REFRESH_WINDOWS" },
			{ 0x0607, "FM_RELOAD_EXTENSIONS" },
			{ 0x0611, "FM_GETDRIVEINFOW" },
			{ 0x0614, "FM_GETFILESELW" },
			{ 0x0615, "FM_GETFILESELLFNW" },
			{ 0x0659, "WLX_WM_SAS" },
			{ 0x07e8, "SM_GETSELCOUNT" },
			{ 0x07e8, "UM_GETSELCOUNT" },
			{ 0x07e8, "WM_CPL_LAUNCH" },
			{ 0x07e9, "SM_GETSERVERSELA" },
			{ 0x07e9, "UM_GETUSERSELA" },
			{ 0x07e9, "WM_CPL_LAUNCHED" },
			{ 0x07ea, "SM_GETSERVERSELW" },
			{ 0x07ea, "UM_GETUSERSELW" },
			{ 0x07eb, "SM_GETCURFOCUSA" },
			{ 0x07eb, "UM_GETGROUPSELA" },
			{ 0x07ec, "SM_GETCURFOCUSW" },
			{ 0x07ec, "UM_GETGROUPSELW" },
			{ 0x07ed, "SM_GETOPTIONS" },
			{ 0x07ed, "UM_GETCURFOCUSA" },
			{ 0x07ee, "UM_GETCURFOCUSW" },
			{ 0x07ef, "UM_GETOPTIONS" },
			{ 0x07f0, "UM_GETOPTIONS2" },
			{ 0x1000, "LVM_FIRST" },
			{ 0x1000, "LVM_GETBKCOLOR" },
			{ 0x1001, "LVM_SETBKCOLOR" },
			{ 0x1002, "LVM_GETIMAGELIST" },
			{ 0x1003, "LVM_SETIMAGELIST" },
			{ 0x1004, "LVM_GETITEMCOUNT" },
			{ 0x1005, "LVM_GETITEMA" },
			{ 0x1006, "LVM_SETITEMA" },
			{ 0x1007, "LVM_INSERTITEMA" },
			{ 0x1008, "LVM_DELETEITEM" },
			{ 0x1009, "LVM_DELETEALLITEMS" },
			{ 0x100a, "LVM_GETCALLBACKMASK" },
			{ 0x100b, "LVM_SETCALLBACKMASK" },
			{ 0x100c, "LVM_GETNEXTITEM" },
			{ 0x100d, "LVM_FINDITEMA" },
			{ 0x100e, "LVM_GETITEMRECT" },
			{ 0x100f, "LVM_SETITEMPOSITION" },
			{ 0x1010, "LVM_GETITEMPOSITION" },
			{ 0x1011, "LVM_GETSTRINGWIDTHA" },
			{ 0x1012, "LVM_HITTEST" },
			{ 0x1013, "LVM_ENSUREVISIBLE" },
			{ 0x1014, "LVM_SCROLL" },
			{ 0x1015, "LVM_REDRAWITEMS" },
			{ 0x1016, "LVM_ARRANGE" },
			{ 0x1017, "LVM_EDITLABELA" },
			{ 0x1018, "LVM_GETEDITCONTROL" },
			{ 0x1019, "LVM_GETCOLUMNA" },
			{ 0x101a, "LVM_SETCOLUMNA" },
			{ 0x101b, "LVM_INSERTCOLUMNA" },
			{ 0x101c, "LVM_DELETECOLUMN" },
			{ 0x101d, "LVM_GETCOLUMNWIDTH" },
			{ 0x101e, "LVM_SETCOLUMNWIDTH" },
			{ 0x101f, "LVM_GETHEADER" },
			{ 0x1021, "LVM_CREATEDRAGIMAGE" },
			{ 0x1022, "LVM_GETVIEWRECT" },
			{ 0x1023, "LVM_GETTEXTCOLOR" },
			{ 0x1024, "LVM_SETTEXTCOLOR" },
			{ 0x1025, "LVM_GETTEXTBKCOLOR" },
			{ 0x1026, "LVM_SETTEXTBKCOLOR" },
			{ 0x1027, "LVM_GETTOPINDEX" },
			{ 0x1028, "LVM_GETCOUNTPERPAGE" },
			{ 0x1029, "LVM_GETORIGIN" },
			{ 0x102a, "LVM_UPDATE" },
			{ 0x102b, "LVM_SETITEMSTATE" },
			{ 0x102c, "LVM_GETITEMSTATE" },
			{ 0x102d, "LVM_GETITEMTEXTA" },
			{ 0x102e, "LVM_SETITEMTEXTA" },
			{ 0x102f, "LVM_SETITEMCOUNT" },
			{ 0x1030, "LVM_SORTITEMS" },
			{ 0x1031, "LVM_SETITEMPOSITION32" },
			{ 0x1032, "LVM_GETSELECTEDCOUNT" },
			{ 0x1033, "LVM_GETITEMSPACING" },
			{ 0x1034, "LVM_GETISEARCHSTRINGA" },
			{ 0x1035, "LVM_SETICONSPACING" },
			{ 0x1036, "LVM_SETEXTENDEDLISTVIEWSTYLE" },
			{ 0x1037, "LVM_GETEXTENDEDLISTVIEWSTYLE" },
			{ 0x1038, "LVM_GETSUBITEMRECT" },
			{ 0x1039, "LVM_SUBITEMHITTEST" },
			{ 0x103a, "LVM_SETCOLUMNORDERARRAY" },
			{ 0x103b, "LVM_GETCOLUMNORDERARRAY" },
			{ 0x103c, "LVM_SETHOTITEM" },
			{ 0x103d, "LVM_GETHOTITEM" },
			{ 0x103e, "LVM_SETHOTCURSOR" },
			{ 0x103f, "LVM_GETHOTCURSOR" },
			{ 0x1040, "LVM_APPROXIMATEVIEWRECT" },
			{ 0x1041, "LVM_SETWORKAREAS" },
			{ 0x1042, "LVM_GETSELECTIONMARK" },
			{ 0x1043, "LVM_SETSELECTIONMARK" },
			{ 0x1044, "LVM_SETBKIMAGEA" },
			{ 0x1045, "LVM_GETBKIMAGEA" },
			{ 0x1046, "LVM_GETWORKAREAS" },
			{ 0x1047, "LVM_SETHOVERTIME" },
			{ 0x1048, "LVM_GETHOVERTIME" },
			{ 0x1049, "LVM_GETNUMBEROFWORKAREAS" },
			{ 0x104a, "LVM_SETTOOLTIPS" },
			{ 0x104b, "LVM_GETITEMW" },
			{ 0x104c, "LVM_SETITEMW" },
			{ 0x104d, "LVM_INSERTITEMW" },
			{ 0x104e, "LVM_GETTOOLTIPS" },
			{ 0x1053, "LVM_FINDITEMW" },
			{ 0x1057, "LVM_GETSTRINGWIDTHW" },
			{ 0x105f, "LVM_GETCOLUMNW" },
			{ 0x1060, "LVM_SETCOLUMNW" },
			{ 0x1061, "LVM_INSERTCOLUMNW" },
			{ 0x1073, "LVM_GETITEMTEXTW" },
			{ 0x1074, "LVM_SETITEMTEXTW" },
			{ 0x1075, "LVM_GETISEARCHSTRINGW" },
			{ 0x1076, "LVM_EDITLABELW" },
			{ 0x108b, "LVM_GETBKIMAGEW" },
			{ 0x108c, "LVM_SETSELECTEDCOLUMN" },
			{ 0x108d, "LVM_SETTILEWIDTH" },
			{ 0x108e, "LVM_SETVIEW" },
			{ 0x108f, "LVM_GETVIEW" },
			{ 0x1091, "LVM_INSERTGROUP" },
			{ 0x1093, "LVM_SETGROUPINFO" },
			{ 0x1095, "LVM_GETGROUPINFO" },
			{ 0x1096, "LVM_REMOVEGROUP" },
			{ 0x1097, "LVM_MOVEGROUP" },
			{ 0x109a, "LVM_MOVEITEMTOGROUP" },
			{ 0x109b, "LVM_SETGROUPMETRICS" },
			{ 0x109c, "LVM_GETGROUPMETRICS" },
			{ 0x109d, "LVM_ENABLEGROUPVIEW" },
			{ 0x109e, "LVM_SORTGROUPS" },
			{ 0x109f, "LVM_INSERTGROUPSORTED" },
			{ 0x10a0, "LVM_REMOVEALLGROUPS" },
			{ 0x10a1, "LVM_HASGROUP" },
			{ 0x10a2, "LVM_SETTILEVIEWINFO" },
			{ 0x10a3, "LVM_GETTILEVIEWINFO" },
			{ 0x10a4, "LVM_SETTILEINFO" },
			{ 0x10a5, "LVM_GETTILEINFO" },
			{ 0x10a6, "LVM_SETINSERTMARK" },
			{ 0x10a7, "LVM_GETINSERTMARK" },
			{ 0x10a8, "LVM_INSERTMARKHITTEST" },
			{ 0x10a9, "LVM_GETINSERTMARKRECT" },
			{ 0x10aa, "LVM_SETINSERTMARKCOLOR" },
			{ 0x10ab, "LVM_GETINSERTMARKCOLOR" },
			{ 0x10ad, "LVM_SETINFOTIP" },
			{ 0x10ae, "LVM_GETSELECTEDCOLUMN" },
			{ 0x10af, "LVM_ISGROUPVIEWENABLED" },
			{ 0x10b0, "LVM_GETOUTLINECOLOR" },
			{ 0x10b1, "LVM_SETOUTLINECOLOR" },
			{ 0x10b3, "LVM_CANCELEDITLABEL" },
			{ 0x10b4, "LVM_MAPINDEXTOID" },
			{ 0x10b5, "LVM_MAPIDTOINDEX" },
			{ 0x10b6, "LVM_ISITEMVISIBLE" },
			{ 0x2000, "OCM__BASE" },
			{ 0x2005, "LVM_SETUNICODEFORMAT" },
			{ 0x2006, "LVM_GETUNICODEFORMAT" },
			{ 0x2019, "OCM_CTLCOLOR" },
			{ 0x202b, "OCM_DRAWITEM" },
			{ 0x202c, "OCM_MEASUREITEM" },
			{ 0x202d, "OCM_DELETEITEM" },
			{ 0x202e, "OCM_VKEYTOITEM" },
			{ 0x202f, "OCM_CHARTOITEM" },
			{ 0x2039, "OCM_COMPAREITEM" },
			{ 0x204e, "OCM_NOTIFY" },
			{ 0x2111, "OCM_COMMAND" },
			{ 0x2114, "OCM_HSCROLL" },
			{ 0x2115, "OCM_VSCROLL" },
			{ 0x2132, "OCM_CTLCOLORMSGBOX" },
			{ 0x2133, "OCM_CTLCOLOREDIT" },
			{ 0x2134, "OCM_CTLCOLORLISTBOX" },
			{ 0x2135, "OCM_CTLCOLORBTN" },
			{ 0x2136, "OCM_CTLCOLORDLG" },
			{ 0x2137, "OCM_CTLCOLORSCROLLBAR" },
			{ 0x2138, "OCM_CTLCOLORSTATIC" },
			{ 0x2210, "OCM_PARENTNOTIFY" },
			{ 0x8000, "WM_APP" },
			{ 0xcccd, "WM_RASDIALEVENT" },
		};

		for ( size_t I = 0; I < LSW_ELEMENTS( sArray ); ++I ) {
			if ( sArray[I].wId == _wMessage ) {
				if ( _sName.size() ) {
					_sName += ", ";
				}
				_sName += sArray[I].pcName;
			}
		}
	}
#else
	VOID CBase::MessageToText( WORD /*_wMessage*/, std::string &/*_sName*/ ) { return; }
#endif	// #ifdef _DEBUG

}	// namespace lsw
