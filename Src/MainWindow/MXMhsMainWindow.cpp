#include "MXMhsMainWindow.h"
#include "../Layouts/MXConverterLayout.h"
#include "../Layouts/MXExpressionEvaluatorLayout.h"
#include "../Layouts/MXFloatingPointStudioLayout.h"
#include "../Layouts/MXFoundAddressLayout.h"
#include "../Layouts/MXMainWindowLayout.h"
#include "../Layouts/MXNewDataTypeSearchLayout.h"
#include "../Layouts/MXNewExpressionSearchLayout.h"
#include "../Layouts/MXNewPointerSearchLayout.h"
#include "../Layouts/MXNewStringSearchLayout.h"
#include "../Layouts/MXOpenProcessLayout.h"
#include "../Layouts/MXOptionsLayout.h"
#include "../Layouts/MXPeWorksLayout.h"
#include "../Layouts/MXSearchProgressLayout.h"
#include "../Layouts/MXStringTheoryLayout.h"
//#include "../Search/MXNewDataTypeSearchWindow.h"
#include "../System/MXSystem.h"
#include "../Utilities/MXUtilities.h"
#include <Base/LSWBase.h>
#include <MultiSplitter/LSWMultiSplitter.h>
#include <Rebar/LSWRebar.h>
#include <Splitter/LSWSplitter.h>
#include <StatusBar/LSWStatusBar.h>
#include <Threads/LSWThread.h>
#include <ToolBar/LSWToolBar.h>

#include <cctype>
#include <mbctype.h>
#include <mbstring.h>

//#include <MLang.h>

namespace mx {

	/*BOOL CALLBACK LocaleEnumprocex( LPWSTR Arg1, DWORD Arg2, LPARAM Arg3 ) {
		::OutputDebugStringW( Arg1 );
		::OutputDebugStringA( "\r\n" );
		return TRUE;
	}*/
	/*BOOL CALLBACK EnumCodePagesProc( LPWSTR lpCodePageString ) {
		UINT uiPage = _wtoi( lpCodePageString );
		if ( ::IsValidCodePage( uiPage ) ) {
			CPINFOEXW cpInfo = { 0 };
			if ( ::GetCPInfoExW( uiPage, 0, &cpInfo ) ) {
				::OutputDebugStringW( cpInfo.CodePageName );
				::OutputDebugStringA( "\r\n" );
			}
		}
		else {
			return TRUE;
		}
		
		return TRUE;
	}*/


/*	std::string VirtualKeyCodeToString(UCHAR virtualKey)
{
    UINT scanCode = MapVirtualKeyA(virtualKey, MAPVK_VK_TO_VSC);

    CHAR szName[128];
    int result = 0;
    switch (virtualKey)
    {
        case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN:
        case VK_RCONTROL: case VK_RMENU:
        case VK_LWIN: case VK_RWIN: case VK_APPS:
        case VK_PRIOR: case VK_NEXT:
        case VK_END: case VK_HOME:
        case VK_INSERT: case VK_DELETE:
        case VK_DIVIDE:
        case VK_NUMLOCK:
            scanCode |= KF_EXTENDED;
        default:
            result = GetKeyNameTextA(scanCode << 16, szName, 128);
    }
    if(result == 0)
        throw std::system_error(std::error_code(GetLastError(), std::system_category()),
                                "WinAPI Error occured.");
    return szName;
}*/


	CMhsMainWindow::CMhsMainWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pfaFoundAddresses( nullptr ),
		m_peeExpEval( nullptr ),
		m_ppwPeWorks( nullptr ),
		m_pstwStringTheory( nullptr ),
		m_pfpsFloatingPointStudioWindow( nullptr ),
		m_cwConverter( nullptr ),
		m_stLastSearchType( MX_ST_DATA_TYPE ),
		m_i32LastOptionsPage( COptionsWindow::MX_P_GENERAL ) {

		m_pmhMemHack = reinterpret_cast<CMemHack *>(_ui64Data);
		

		static const struct {
			LPCWSTR				lpwsImageName;
			DWORD				dwConst;
		} sImages[] = {
			{ L"23", MX_I_OPENPROCESS },
			{ L"101", MX_I_OPENFORDEBUG },

			{ L"1", MX_I_ADDENTRY },
			{ L"52", MX_I_OPENENTRY },
			{ L"22", MX_I_SAVEENTRY },

			{ L"33", MX_I_DELETEENTRY },
			{ L"31", MX_I_DELETEALL },

			{ L"27", MX_I_LOCK },
			{ L"28", MX_I_UNLOCK },

			{ L"96", MX_I_EDIT },

			{ L"73", MX_I_OPTIONS },
		};
		m_iImages.Create( 24, 24, ILC_COLOR32, MX_I_TOTAL, MX_I_TOTAL );

		for ( size_t I = 0; I < MX_I_TOTAL; ++I ) {
			CSecureWString wsTemp = CSystem::GetResourcesPathW();
			wsTemp += sImages[I].lpwsImageName;
			wsTemp += L".bmp";

			m_bBitmaps[sImages[I].dwConst].LoadFromFile( wsTemp.c_str(), 0, 0, LR_CREATEDIBSECTION );
			m_iImageMap[sImages[I].dwConst] = m_iImages.Add( m_bBitmaps[sImages[I].dwConst].Handle() );
		}

		/*::EnumSystemLocalesEx( LocaleEnumprocex, LOCALE_ALL, 0, NULL );*/
		//::EnumSystemCodePagesW( EnumCodePagesProc, CP_SUPPORTED );
		
		/*CPINFOEXW cpInfo = { 0 };
		for ( uint64_t I = 0; I <= 0x0FFFF; ++I ) {
			if ( ::IsValidCodePage( I ) ) {
				if ( ::GetCPInfoExW( I, 0, &cpInfo ) ) {
					::OutputDebugStringW( cpInfo.CodePageName );
					::OutputDebugStringA( "\r\n" );
				}
			}
		}*/
#if 0
		for ( uint32_t I = 0; I < 256; ++I ) {
			std::wstring wText = CHotkeyManager::VirtualKeyToString( I, false );
			if ( wText.size() ) {
				::OutputDebugStringW( wText.c_str() );
				::OutputDebugStringA( "\r\n" );
			}
			if ( CHotkeyManager::VirtualKeyInfo( I ) & CHotkeyManager::MX_K_HAS_EXTENDED/* &&
				CHotkeyManager::VirtualKeyInfo( I ) & CHotkeyManager::MX_K_PRINTABLE*/ ) {
				wText = CHotkeyManager::VirtualKeyToString( I, true );
				if ( wText.size() ) {
					std::string sTmp;
					::OutputDebugStringA( " -> " );
					CUtilities::BytesToCString( CUtilities::ToUtf8( wText ).data(),
						CUtilities::ToUtf8( wText ).size(), sTmp );
					::OutputDebugStringA( sTmp.c_str() );
					::OutputDebugStringA( "\r\n" );
				}
			}
		}
#endif
#if 0
		for ( uint32_t I = 0; I < 256; ++I ) {
			WCHAR szBufText[45];
			WCHAR szTmp[45];
			::ZeroMemory( szBufText, sizeof( szBufText ) );
			UINT uiKey = ::MapVirtualKeyW( I, MAPVK_VK_TO_VSC_EX );
			UINT uiChar = ::MapVirtualKeyW( I, MAPVK_VK_TO_CHAR );
			if ( ::GetKeyNameTextW( (uiKey << 16), szBufText, 45 ) ) {
				/*::OutputDebugStringW( szBufText );
				::GetKeyNameTextW( ((uiKey | KF_EXTENDED) << 16), szTmp, 45 );
				::OutputDebugStringA( "  " );
				::OutputDebugStringW( szTmp );
				::OutputDebugStringA( "\r\n" );*/
				if ( ::GetKeyNameTextW( ((uiKey | KF_EXTENDED) << 16), szTmp, 45 ) ) {
					if ( std::wcscmp( szTmp, szBufText ) != 0 ) {
						::OutputDebugStringW( szBufText );
						::OutputDebugStringA( "\r\n" );
						/*::OutputDebugStringW( szBufText );
						::OutputDebugStringA( "  " );
						::OutputDebugStringW( szTmp );
						::OutputDebugStringA( "\r\n" );*/
						WCHAR szTmpBUGF[145];
						if ( uiChar ) {
							swprintf( szTmpBUGF, L"%X | KF_EXTENDED,\t/* %s '%c' */ \r\n", uiKey, szTmp, uiChar );
						}
						else {
							swprintf( szTmpBUGF, L"%X | KF_EXTENDED,\t/* %s '??' */ \r\n", uiKey, szTmp );
						}
						::OutputDebugStringW( szTmpBUGF );
					}
					else {
						WCHAR szTmpBUGF[145];
						if ( uiChar ) {
							swprintf( szTmpBUGF, L"%X,\t/* %s '%c' */ \r\n", uiKey, szBufText, uiChar );
						}
						else {
							swprintf( szTmpBUGF, L"%X,\t/* %s '??' */ \r\n", uiKey, szBufText );
						}
						::OutputDebugStringW( szTmpBUGF );
						/*::OutputDebugStringW( szBufText );
						::OutputDebugStringA( "\r\n" );*/
					}
				}
				else {
					WCHAR szTmpBUGF[145];
					if ( uiChar ) {
						swprintf( szTmpBUGF, L"%X,\t/* %s '%c' */ Non Extended\r\n", uiKey, szBufText, uiChar );
					}
					else {
						swprintf( szTmpBUGF, L"%X,\t/* %s '??' */ Non Extended\r\n", uiKey, szBufText );
					}
					::OutputDebugStringW( szTmpBUGF );
				}


				//
				/*::OutputDebugStringA( "  " );
				::OutputDebugStringA( VirtualKeyCodeToString( I ).c_str() );
				::OutputDebugStringA( "\r\n" );*/
			}
		}
#endif 0
		/*LCID luId = ::ConvertDefaultLocale( LOCALE_SYSTEM_DEFAULT );
		int gjhg = 0;*/
#if 0
		const wchar_t * pwcTemp0 = L"À";
		const wchar_t * pwcTemp1 = L"à";
		int iRet = ::CompareStringEx( LOCALE_NAME_INVARIANT,
			NORM_IGNORECASE,
			pwcTemp0, -1,
			pwcTemp1, -1,
			nullptr, NULL, 0 );
		_locale_t lLocale = ::_create_locale( LC_ALL, "en-US" );
		//std::setlocale( LC_ALL, "en_US.utf8" );
		for ( uint32_t I = 0; I < 256; ++I ) {
			char szAsciiBuffer[64];
			char cPrintMe = I >= 0x20 ? I : ' ';
			int J = _mbctolower( I );
			char cPrintMe2 = I >= 0x20 ? J : ' ';
			sprintf( szAsciiBuffer, "0x%.2X /* %c -> %c */, ", J, cPrintMe, cPrintMe2 );
			::OutputDebugStringA( szAsciiBuffer );
			if ( (I + 1) % 16 == 0 ) {
				::OutputDebugStringA( "\r\n" );
			}
		}

		{
			::OutputDebugStringA( "\r\n" );
			::OutputDebugStringA( "\r\n" );
			for ( uint32_t I = 0; I < 256; ++I ) {
				char* utf8 = (char*)&I;
				// convert multibyte UTF-8 to wide string UTF-16
				int length = ::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)utf8, -1, NULL, 0);
				if (length > 0)
				{
					wchar_t* wide = new wchar_t[length];
					MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)utf8, -1, wide, length);

					// convert it to ANSI, use setlocale() to set your locale, if not set
					size_t convertedChars = 0;
					char* ansi = new char[length+1];
					::ZeroMemory( ansi, length+1 );
					//wcstombs_s(&convertedChars, ansi, length, wide, _TRUNCATE);
					::WideCharToMultiByte( CP_UTF7, 0/*WC_COMPOSITECHECK | WC_SEPCHARS*/, wide, 1, ansi, length, nullptr, nullptr );
					//::OutputDebugStringA( ansi );
					//::OutputDebugStringA( "\r\n" );

					delete [] ansi;
					delete [] wide;
				}
			}
		}

		//for ( uint32_t I = 0; I < 0x01000000; ++I ) {
		for ( uint32_t I = 0; I < 0x010000; ++I ) {
			uint32_t ui32Hira = _mbctolower( I );//::towctrans( I, ::wctrans("toupper") ); //_mbctolower
			WORD wType[2];
			::GetStringTypeW( CT_CTYPE3,
				reinterpret_cast<LPCWCH>(&I),
				2,
				wType );

			WORD wType1[2];
			::GetStringTypeW( CT_CTYPE1,
				reinterpret_cast<LPCWCH>(&I),
				2,
				wType1 );
			//if ( ui32Hira != I ) {
			//if ( wType[0] & (C3_SYMBOL) ) {
			if ( /*!iswprint( (*reinterpret_cast<LPCWCH>(&I)) )*/
				//(wType1[0] & C1_CNTRL) || !(wType1[0] & C1_DEFINED)
				wType[0] & (C3_FULLWIDTH)
				) {
				wchar_t szBuffer[128];
				wchar_t szWideAscciiBuffer[128];
				::ZeroMemory( szBuffer, sizeof( szBuffer ) );
				
				char szAsciiBuffer[32];
				::ZeroMemory( szAsciiBuffer, sizeof( szAsciiBuffer ) );
				//::WideCharToMultiByte( CP_UTF8, WC_COMPOSITECHECK | WC_SEPCHARS, reinterpret_cast<LPCWCH>(&I), 1, szAsciiBuffer, 32, nullptr, nullptr );
				int iNorm = ::NormalizeString( NormalizationKD, reinterpret_cast<LPCWCH>(&I), 1, szWideAscciiBuffer, 128 );
				//if ( iNorm <= 0 ) {
					szBuffer[1] = 0;
					::OutputDebugStringW( szBuffer );
					//::OutputDebugStringA( szAsciiBuffer );
					//::OutputDebugStringA( "\r\n" );

					//swprintf( szBuffer, L"%c -> %c\r\n", I, ui32Hira );
					swprintf( szBuffer, L"%c (\\u%.4X) -> %c (\\u%.4X)  %.4X  %d \\u%.4X\r\n", I, I, ui32Hira, ui32Hira,
						(wType1[0] & (C1_CNTRL)),
						::iswprint( (*reinterpret_cast<LPCWCH>(&I)) ), szWideAscciiBuffer[0] );
					//swprintf( szBuffer, L"case 0x%.4X : { return '%c' /* %c */; }	// %.4X  %S\r\n", I, towupper( I ), I, wType[0], szAsciiBuffer );
					::OutputDebugStringW( szBuffer );
				//}
				
				/*
				::MultiByteToWideChar( CP_UTF8, 0, reinterpret_cast<LPCCH>(&I), 4, szBuffer, 128 );
				::OutputDebugStringW( szBuffer );
				::OutputDebugStringW( L" -> " );
				::MultiByteToWideChar( CP_UTF8, 0, reinterpret_cast<LPCCH>(&ui32Hira), 4, szBuffer, 128 );
				::OutputDebugStringW( szBuffer );
				::OutputDebugStringW( L"\r\n" );*/
			}
		}
		::_free_locale( lLocale );
#endif
	}

	CMhsMainWindow::~CMhsMainWindow() {
		//delete m_pmhMemHack;
		m_pmhMemHack = nullptr;

		delete m_pfaFoundAddresses;
		m_pfaFoundAddresses = nullptr;

		delete m_ppwPeWorks;
		m_ppwPeWorks = nullptr;

		delete m_pstwStringTheory;
		m_pstwStringTheory = nullptr;

		delete m_pfpsFloatingPointStudioWindow;
		m_pfpsFloatingPointStudioWindow = nullptr;

		delete m_cwConverter;
		m_cwConverter = nullptr;
	}

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CMhsMainWindow::InitDialog() {
		{
														//	uiFuncParm0							pfFunc				hdName								uiId								sParms	hdParms																			hdFormattings
			CHotkeyManBase::MX_HOTKEY_HANDLER hhHandler = { reinterpret_cast<uint64_t>(this),	Hotkey_OpenOptions,	{ _T_LEN_159BB63C_Show_Options },	MX_WH_SHOW_OPTIONS,					0 };
			CHotkeyManBase::RegisterHotkeyHandler( hhHandler );
		}
		{
														//	uiFuncParm0							pfFunc						hdName										uiId								sParms	hdParms																			hdFormattings
			CHotkeyManBase::MX_HOTKEY_HANDLER hhHandler = { reinterpret_cast<uint64_t>(this),	Hotkey_ShowFoundAddresses,	{ _T_LEN_A5501E10_Show_Found_Addresses },	MX_WH_SHOW_FOUND_ADDRESSES,			0 };
			CHotkeyManBase::RegisterHotkeyHandler( hhHandler );
		}
		{
														//	uiFuncParm0							pfFunc							hdName											uiId										sParms	hdParms																			hdFormattings
			CHotkeyManBase::MX_HOTKEY_HANDLER hhHandler = { reinterpret_cast<uint64_t>(this),	Hotkey_ShowExpressionEvaluator,	{ _T_LEN_0661F178_Show_Expression_Evaluator },	MX_WH_SHOW_EXPRESSION_EVALUATOR,			0 };
			CHotkeyManBase::RegisterHotkeyHandler( hhHandler );
		}
		{
														//	uiFuncParm0							pfFunc							hdName											uiId										sParms	hdParms																			hdFormattings
			CHotkeyManBase::MX_HOTKEY_HANDLER hhHandler = { reinterpret_cast<uint64_t>(this),	Hotkey_ShowConverter,			{ _T_LEN_9118A28E_Show_Converter },				MX_WH_SHOW_CONVERTER,						0 };
			CHotkeyManBase::RegisterHotkeyHandler( hhHandler );
		}

		{
														//	uiFuncParm0							pfFunc							hdName											uiId										sParms	hdParms																			hdFormattings
			CHotkeyManBase::MX_HOTKEY_HANDLER hhHandler = { reinterpret_cast<uint64_t>(this),	Hotkey_ShowPeWorks,				{ _T_LEN_759D0F03_Show_PE_Works },				MX_WH_SHOW_PE_WORKS,						0 };
			CHotkeyManBase::RegisterHotkeyHandler( hhHandler );
		}
		{
														//	uiFuncParm0							pfFunc							hdName											uiId										sParms	hdParms																			hdFormattings
			CHotkeyManBase::MX_HOTKEY_HANDLER hhHandler = { reinterpret_cast<uint64_t>(this),	Hotkey_ShowStringTheory,		{ _T_LEN_B7A8A10D_Show_String_Theory },			MX_WH_SHOW_STRING_THEORY,					0 };
			CHotkeyManBase::RegisterHotkeyHandler( hhHandler );
		}


		CToolBar * plvToolBar = static_cast<CToolBar *>(FindChild( CMainWindowLayout::MX_MWI_TOOLBAR0 ));
		CRebar * plvRebar = static_cast<CRebar *>(FindChild( CMainWindowLayout::MX_MWI_REBAR0 ));

		// ==== TOOL BAR ==== //
		plvToolBar->SetImageList( 0, m_iImages );
//#define MX_TOOL_STR( TXT )					reinterpret_cast<INT_PTR>(TXT)
#define MX_TOOL_STR( TXT )						0
		const TBBUTTON bButtons[] = {
			// iBitmap							idCommand									fsState				fsStyle			bReserved	dwData	iString
			{ m_iImageMap[MX_I_OPENPROCESS],	CMainWindowLayout::MX_MWMI_OPENPROCESS,		TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Open Process" ) },
			{ m_iImageMap[MX_I_OPENFORDEBUG],	CMainWindowLayout::MX_MWMI_OPENFORDEBUG,	TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Open for Debug") },
			{ -1,								0,											TBSTATE_ENABLED,	BTNS_SEP,		{ 0 },		0,		0 },
			{ m_iImageMap[MX_I_ADDENTRY],		CMainWindowLayout::MX_MWMI_ADDENTRY,		TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Add Entry" ) },
			{ m_iImageMap[MX_I_OPENENTRY],		CMainWindowLayout::MX_MWMI_OPENSAVEFILE,	TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Open Save File" ) },
			{ m_iImageMap[MX_I_SAVEENTRY],		CMainWindowLayout::MX_MWMI_SAVE,			TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Save Entries" ) },
			{ -1,								0,											TBSTATE_ENABLED,	BTNS_SEP,		{ 0 },		0,		0 },
			{ m_iImageMap[MX_I_DELETEENTRY],	CMainWindowLayout::MX_MWMI_DELETE,			TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Delete Entry" ) },
			{ m_iImageMap[MX_I_DELETEALL],		CMainWindowLayout::MX_MWMI_DELETEALL,		TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Delete All Entries" ) },
			{ -1,								0,											TBSTATE_ENABLED,	BTNS_SEP,		{ 0 },		0,		0 },
			{ m_iImageMap[MX_I_LOCK],			CMainWindowLayout::MX_MWMI_LOCK,			TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Lock Selected" ) },
			{ m_iImageMap[MX_I_UNLOCK],			CMainWindowLayout::MX_MWMI_UNLOCK,			TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Unlock Selected" ) },
			{ -1,								0,											TBSTATE_ENABLED,	BTNS_SEP,		{ 0 },		0,		0 },
			{ m_iImageMap[MX_I_EDIT],			CMainWindowLayout::MX_MWMI_EDIT,			TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Edit Selected" ) },
			{ -1,								0,											TBSTATE_ENABLED,	BTNS_SEP,		{ 0 },		0,		0 },
			{ m_iImageMap[MX_I_OPTIONS],		CMainWindowLayout::MX_MWMI_OPTIONS,			TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Options" ) },
		};
#undef MX_TOOL_STR

		plvToolBar->AddButtons( bButtons, MX_ELEMENTS( bButtons ) );

		plvRebar->SetImageList( m_iImages );
		{
			LSW_REBARBANDINFO riRebarInfo;
			riRebarInfo.SetColors( ::GetSysColor( COLOR_BTNTEXT ), ::GetSysColor( COLOR_BTNFACE ) );
			riRebarInfo.SetStyle( RBBS_CHILDEDGE |
			  RBBS_FIXEDBMP );
			riRebarInfo.SetChild( plvToolBar->Wnd() );
			riRebarInfo.SetChildSize( plvToolBar->GetMinBoundingRect().Width(), plvToolBar->GetMinBoundingRect().Height() );
			riRebarInfo.SetId( CMainWindowLayout::MX_MWI_TOOLBAR0 );
			plvRebar->InsertBand( -1, riRebarInfo );
		}

	   LSW_RECT rRebarRect = ClientRect( this );
	   ::MoveWindow( plvRebar->Wnd(), 0, 0, rRebarRect.Width(), plvRebar->WindowRect( this ).Height(), FALSE );

		plvRebar->UpdateRects();
		

		// ==== STATUS BAR ==== //
		CStatusBar * psbStatus = StatusBar();
		if ( psbStatus ) {
			const CStatusBar::LSW_STATUS_PART spParts[] = {
				// Last status message.
				//{ 450, TRUE },
				// Current process ID.
				//{ 450 + 48, TRUE },
				{ rRebarRect.Width() - psbStatus->ClientRect( this ).Height() - 48, TRUE },

				{ rRebarRect.Width() - psbStatus->ClientRect( this ).Height(), TRUE },
			};
			psbStatus->SetParts( spParts, MX_ELEMENTS( spParts ) );
		}


		// ==== LIST VIEW ==== //
		CListView * plvAddressList = MainListView();
		if ( plvAddressList ) {
			const struct {
				const char * _pcText;
				size_t sLen;
				DWORD dwWidth;
			} aTitles[] = {
				{ _T_EB78CFF1_Description, _LEN_EB78CFF1, 150 },
				{ _T_C2F3561D_Address, _LEN_C2F3561D, 80 },
				{ _T_31A2F4D5_Current_Value, _LEN_31A2F4D5, 120 },
				{ _T_022E8A69_Value_When_Locked, _LEN_022E8A69, 120 },
				{ _T_2CECF817_Type, _LEN_2CECF817, 100 },
			};
			for ( INT I = 0; I < MX_ELEMENTS( aTitles ); I++ ) {
				INT iCol = plvAddressList->AddColumn( mx::CStringDecoder::DecodeToWString( aTitles[I]._pcText, aTitles[I].sLen ).c_str() );
				BOOL bAdded = plvAddressList->SetColumnWidth( iCol, aTitles[I].dwWidth );
				if ( !bAdded ) { break; }
			}
			//plvAddressList->SetColumnWidth( plvAddressList->GetTotalColumns(), LVSCW_AUTOSIZE_USEHEADER );
		}

		// TEMP.
		/*CSplitter * pwSplitter = static_cast<CSplitter *>(FindChild( CMainWindowLayout::MX_MWI_SPLITTER ));
		pwSplitter->SetSplitterType( CSplitter::LSW_SS_VERTICAL );
		pwSplitter->Attach( MainListView(), CSplitter::LSW_A_RIGHT );*/

		CMultiSplitter * pwSplitter = MultiSplitter();
		CMultiSplitter::LSW_DT_ATTACH aAttach;
		aAttach.atAttachTo = CMultiSplitter::LSW_AT_RIGHT;
		aAttach.dwId = pwSplitter->RootId();
		aAttach.pwWidget = FindChild( CMainWindowLayout::MX_MWI_TEST0 );
		//pwSplitter->Attach( aAttach );
		aAttach.pwWidget = MainListView();
		pwSplitter->Attach( aAttach );

		aAttach.atAttachTo = CMultiSplitter::LSW_AT_TOP;
		aAttach.pwWidget = FindChild( CMainWindowLayout::MX_MWI_TEST1 );
		//pwSplitter->Attach( aAttach );


		ForceSizeUpdate();

		ShowFoundAddress();
		ShowExpEval();
		ShowConverter();

		reinterpret_cast<CHotkeyPollingMethod *>(m_pmhMemHack->HotkeyManager().GetPollingHotkeyManager())->SetWindow( Wnd() );
		reinterpret_cast<CHotkeyWhKeyboardMethod *>(m_pmhMemHack->HotkeyManager().GetWhKeyboardHotkeyManager())->SetWindow( Wnd() );
		reinterpret_cast<CHotkeyRegisterHotKeyMethod *>(m_pmhMemHack->HotkeyManager().GetRegisteredHotkeyManager())->SetWindow( Wnd() );
		m_pmhMemHack->HotkeyManager().Start();

		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CMhsMainWindow::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {
		switch ( _Id ) {
			case CMainWindowLayout::MX_MWMI_OPENPROCESS : {
				OpenProcess();
				break;
			}
			case CMainWindowLayout::MX_MWMI_DATATYPE : {
				ShowNewDataTypeSearch();
				break;
			}
			case CMainWindowLayout::MX_MWMI_POINTER : {
				ShowNewPointerSearch();
				break;
			}
			case CMainWindowLayout::MX_MWMI_STRING : {
				ShowNewStringSearch();
				break;
			}
			case CMainWindowLayout::MX_MWMI_SEARCHOPTIONS : {
				ShowOptions( SearchOptionsPageClamp() );
				break;
			}
			case CMainWindowLayout::MX_MWMI_OPTIONS : {
				ShowOptions( LastOptionsShown() );
				break;
			}
			case CMainWindowLayout::MX_MWMI_PEWORKS : {
				ShowPeWorks();
				break;
			}
			case CMainWindowLayout::MX_MWMI_STRINGTHEORY : {
				ShowStringTheory();
				break;
			}
			case CMainWindowLayout::MX_MWMI_FLOATINGPOINTSTUDIO : {
				ShowFloatingPointStudio( std::nan( "" ) );
				break;
			}
			case CMainWindowLayout::MX_MWMI_EXPRESSION : {
				ShowNewExpressionSearch();
				break;
			}
			case CMainWindowLayout::MX_MWMI_SHOWFOUNDADDR : {
				ShowFoundAddress();
				break;
			}
			case CMainWindowLayout::MX_MWMI_SHOW_EXPEVAL : {
				ShowExpEval();
				break;
			}
			case CMainWindowLayout::MX_MWMI_SHOW_CONVERTER : {
				ShowConverter();
				break;
			}
			case CMainWindowLayout::MX_MWMI_SHOW_ALL : {
				ShowFoundAddress();
				ShowExpEval();
				ShowConverter();
				break;
			}
		}
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from menu.
	CWidget::LSW_HANDLED CMhsMainWindow::MenuCommand( WORD _Id ) {
		return Command( 0, _Id, NULL );
	}

	// WM_ERASEBKGND.
	CWidget::LSW_HANDLED CMhsMainWindow::EraseBkgnd( HDC _hDc ) {
		return LSW_H_HANDLED;
	}

	// WM_NCDESTROY.
	CWidget::LSW_HANDLED CMhsMainWindow::NcDestroy() {
		CPeWorksWindow::ShutDown();
		::PostQuitMessage( 0 );
		return LSW_H_CONTINUE;
	}

	// WM_KEYDOWN
	CWidget::LSW_HANDLED CMhsMainWindow::KeyDown( UINT _uiKeyCode, UINT _uiFlags ) {
		/*UINT uiTmp = ::MapVirtualKeyW( _uiKeyCode, MAPVK_VK_TO_VSC_EX );
		std::wstring wTmp = CHotkeyManager::ScanCodeToString( _uiFlags );
		::OutputDebugStringW( L"Pressing: \"" );
		::OutputDebugStringW( wTmp.c_str() );
		::OutputDebugStringW( L" " );
		wTmp.clear();
		CUtilities::ToHex( _uiFlags, wTmp, 4 );
		::OutputDebugStringW( wTmp.c_str() );
		::OutputDebugStringW( L" " );
		wTmp.clear();
		CUtilities::ToHex( ::MapVirtualKeyW( _uiFlags >> 16, MAPVK_VSC_TO_VK_EX ), wTmp, 4 );
		::OutputDebugStringW( wTmp.c_str() );

		::OutputDebugStringW( L" " );
		wTmp.clear();
		CUtilities::ToHex( ::MapVirtualKeyW( (_uiFlags >> 16), MAPVK_VSC_TO_VK ), wTmp, 4 );
		::OutputDebugStringW( wTmp.c_str() );
		::OutputDebugStringW( L"\".\r\n" );*/
 		return lsw::CMainWindow::KeyDown( _uiKeyCode, _uiFlags );
	}

	// WM_KEYUP
	CWidget::LSW_HANDLED CMhsMainWindow::KeyUp( UINT _uiKeyCode, UINT _uiFlags ) {
		return LSW_H_CONTINUE;
	}

	// WM_SYSKEYDOWN
	CWidget::LSW_HANDLED CMhsMainWindow::SysKeyDown( UINT _uiKeyCode, UINT _uiFlags ) {
		return LSW_H_CONTINUE;
	}

	// WM_SYSKEYUP
	CWidget::LSW_HANDLED CMhsMainWindow::SysKeyUp( UINT _uiKeyCode, UINT _uiFlags ) {
		return LSW_H_CONTINUE;
	}

	// WM_HOTKEY.
	CWidget::LSW_HANDLED CMhsMainWindow::Hotkey( INT _iIdentifier, INT _iVirtualKey, INT _iMod ) {
		//m_hmHotkeys
		//return CMainWindow::Hotkey( _iIdentifier, _iVirtualKey, _iMod );
		m_pmhMemHack->HotkeyManager().HandleWM_HOTKEY( _iIdentifier, _iVirtualKey, _iMod );

		return LSW_H_CONTINUE;
	}

	// Gets the status bar.
	CStatusBar * CMhsMainWindow::StatusBar() {
		return static_cast<CStatusBar *>(FindChild( CMainWindowLayout::MX_MWI_STATUSBAR ));
	}

	// Gets the status bar.
	const CStatusBar * CMhsMainWindow::StatusBar() const {
		return static_cast<const CStatusBar *>(FindChild( CMainWindowLayout::MX_MWI_STATUSBAR ));
	}

	// Gets the list view.
	CListView * CMhsMainWindow::MainListView() {
		return static_cast<CListView *>(FindChild( CMainWindowLayout::MX_MWI_STOREDADDRESSES ));
	}

	// Gets the list view.
	const CListView * CMhsMainWindow::MainListView() const {
		return static_cast<const CListView *>(FindChild( CMainWindowLayout::MX_MWI_STOREDADDRESSES ));
	}

	// Gets the multi-splitter.
	CMultiSplitter * CMhsMainWindow::MultiSplitter() {
		return static_cast<CMultiSplitter *>(FindChild( CMainWindowLayout::MX_MWI_SPLITTER ));
	}

	// Gets the multi-splitter.
	const CMultiSplitter * CMhsMainWindow::MultiSplitter() const {
		return static_cast<const CMultiSplitter *>(FindChild( CMainWindowLayout::MX_MWI_SPLITTER ));
	}

	// Virtual client rectangle.  Can be used for things that need to be adjusted based on whether or not status bars, toolbars, etc. are present.
	const LSW_RECT CMhsMainWindow::VirtualClientRect( const CWidget * pwChild ) const {
		LSW_RECT rTemp = ClientRect( this );
		const CRebar * plvRebar = static_cast<const CRebar *>(FindChild( CMainWindowLayout::MX_MWI_REBAR0 ));
		if ( plvRebar ) {
			LSW_RECT rRebar = plvRebar->ClientRect( this );
			rTemp.top += rRebar.Height();
		}

		const CStatusBar * psbStatus = StatusBar();
		if ( psbStatus ) {
			LSW_RECT rStatus = psbStatus->ClientRect( this );
			rTemp.bottom -= rStatus.Height();
		}
		return rTemp;
	}

	// WM_USER/custom messages.
	CWidget::LSW_HANDLED CMhsMainWindow::CustomPrivateMsg( UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
		switch ( _uMsg ) {
			case MX_CM_UPDATE_FA : {
				UpdateFoundAddressWindowText();
				break;
			}
			case MX_CM_HOTKEY : {
				PollHotkeys( static_cast<INT>(_wParam) );
				break;
			}
			case MX_CM_HOOKKEY : {
				PollHookKeys( static_cast<INT>(_wParam) );
				break;
			}
		}
		return LSW_H_CONTINUE;
	}

	// Shows the Found Address dockable.
	void CMhsMainWindow::ShowFoundAddress() {
		/*ee::CExpEvalContainer::EE_RESULT rRes;
		rRes.ncType = ee::EE_NC_UNSIGNED;
		rRes.u.dVal = -903;
		wchar_t szBuffer[128];
		ee::CExpEvalContainer::PrintF( szBuffer, MX_ELEMENTS( szBuffer ), L"Blah %+20f %X KEYS\r\n", 1, rRes );*/

		if ( !m_pfaFoundAddresses ) {
			m_pfaFoundAddresses = static_cast<CFoundAddressesWindow *>(CFoundAddressLayout::CreateFoundAddressesWindow( MultiSplitter(), this ));
			if ( !m_pfaFoundAddresses ) { return; }
			m_pfaFoundAddresses->SetUpdateSpeed( 1000 / max( m_pmhMemHack->Options().dwFoundAddressRefresh, static_cast<DWORD>(1) ) );
			//m_pfaFoundAddresses = static_cast<CFoundAddressesWindow *>(CFoundAddressLayout::CreateFoundAddressesWindow( this ));
			m_pfaFoundAddresses->AddDockTarget( MultiSplitter() );
			m_pfaFoundAddresses->UpdateHeaders( MemHack()->Searcher().LastSearchType() );
			m_pfaFoundAddresses->AddFoundAddresses();
		}
		else {
			m_pfaFoundAddresses->SetVisible( TRUE );
		}
		UpdateWindowChecks();
		UpdateFoundAddressWindowText();
	}

	// Shows the Expression Evaluator dockable.
	void CMhsMainWindow::ShowExpEval() {
		if ( !m_peeExpEval ) {
			m_peeExpEval = static_cast<CExpEvalWindow *>(CExpressionEvaluatorLayout::CreateExpEvalWindow( MultiSplitter() ));
			m_peeExpEval->SetUpdateSpeed( 1000 / max( m_pmhMemHack->Options().dwExpressionRefresh, static_cast<DWORD>(1) ) );
			//m_peeExpEval = static_cast<CExpEvalWindow *>(CExpressionEvaluatorLayout::CreateExpEvalWindow( this ));
			m_peeExpEval->AddDockTarget( MultiSplitter() );
			m_peeExpEval->SetAddressHandler( CMemHack::ExpAddressHandler, reinterpret_cast<uintptr_t>(m_pmhMemHack) );
		}
		else {
			m_peeExpEval->SetVisible( TRUE );
		}
		UpdateWindowChecks();
	}

	// Shows the Converter dockable.
	void CMhsMainWindow::ShowConverter() {
		if ( !m_cwConverter ) {
			m_cwConverter = static_cast<CConverterWindow *>(CConverterLayout::CreateConverterWindow( MultiSplitter() ));
			//m_cwConverter->SetUpdateSpeed( 1000 / max( m_pmhMemHack->Options().dwExpressionRefresh, static_cast<DWORD>(1) ) );
			//m_cwConverter = static_cast<CExpEvalWindow *>(CExpressionEvaluatorLayout::CreateExpEvalWindow( this ));
			m_cwConverter->AddDockTarget( MultiSplitter() );
			m_cwConverter->SetAddressHandler( CMemHack::ExpAddressHandler, reinterpret_cast<uintptr_t>(m_pmhMemHack) );
		}
		else {
			m_cwConverter->SetVisible( TRUE );
		}
		UpdateWindowChecks();
	}

	// Shows the options dialog with a page to be selected by default.
	void CMhsMainWindow::ShowOptions( int32_t _i32Page ) {
		MX_OPTIONS oOptions = m_pmhMemHack->Options();
		oOptions.pmhMemHackObj = m_pmhMemHack;
		m_pmhMemHack->HotkeyManager().Stop();
		if ( COptionsLayout::CreateOptionsDialog( this, &oOptions, _i32Page ) ) {
			m_pmhMemHack->SetOptions( oOptions );
			m_i32LastOptionsPage = COptionsWindow::LastVisiblePage();
		}
		m_pmhMemHack->HotkeyManager().Start();
	}

	// Shows the PE Works window.  If _pwcFile is not nullptr, it points to a default file to
	//	show in a new tab after opening the window.  If the window already exists, it is brought
	//	to the front and the tab with the existing provided file (if any) is activated, or a new
	//	tab with the given file (if any) is created.
	void CMhsMainWindow::ShowPeWorks( const wchar_t * _pwcFile ) {
		if ( !m_ppwPeWorks ) {
			m_ppwPeWorks = static_cast<CPeWorksWindow *>(CPeWorksLayout::CreatePeWorksWindow( nullptr ));
			m_ppwPeWorks->SetWidgetParent( this );
		}

		if ( m_ppwPeWorks && _pwcFile ) {
			// Load the given file.
			m_ppwPeWorks->LoadFile( _pwcFile );
		}
	}

	// Shows the String Theory window.
	void CMhsMainWindow::ShowStringTheory( const wchar_t * _pwcDefaultText ) {
		if ( !m_pstwStringTheory ) {
			m_pstwStringTheory = static_cast<CStringTheoryWindow *>(CStringTheoryLayout::CreateStringTheoryWindow( nullptr ));
			m_pstwStringTheory->SetWidgetParent( this );
		}

		if ( _pwcDefaultText ) {
			m_pstwStringTheory->SetText( _pwcDefaultText );
		}
	}

	// Shows the Floating-Point Studio window with a given default value or std::nan() for no default value.
	void CMhsMainWindow::ShowFloatingPointStudio( double _dDefault,
		bool _bHasSign, bool _bHasExplicitBit,
		uint16_t _uiExpBits, uint16_t _uiManBits ) {
		if ( !m_pfpsFloatingPointStudioWindow ) {
			m_pfpsFloatingPointStudioWindow = static_cast<CFloatingPointStudioWindow *>(CFloatingPointStudioLayout::CreateFloatingPointStudioWindow( nullptr ));
			m_pfpsFloatingPointStudioWindow->SetWidgetParent( this );
		}
		if ( m_pfpsFloatingPointStudioWindow ) {

		}
	}

	// Shows a new search.
	uint32_t CMhsMainWindow::ShowNewDataTypeSearch() {
		if ( !m_pmhMemHack->Process().ProcIsOpened() ) {
			if ( !CBase::PromptYesNo( Wnd(), _DEC_WS_77E541C6_You_must_open_a_process_before_you_can_perform_a_search__r_nOpen_a_process_now_.c_str(),
				_DEC_WS_8C0AA5EB_No_Process_Opened.c_str() ) ) { return 0; }
			if ( !OpenProcess() ) { return 0; }
		}
		// Should not be possible, but just one last check.
		if ( !m_pmhMemHack->Process().ProcIsOpened() ) { return 0; }
		m_stLastSearchType = MX_ST_DATA_TYPE;

		//MX_OPTIONS oOptions = m_pmhMemHack->Options();
		CNewDataTypeSearchLayout::CreateNewDataTypeSearchDialog( this, m_pmhMemHack );
		// TODO: Come up with a more graceful way to do this.
		// A few options can be modified directly in the dialog.  This is a simple way to add
		//	those to a list to check if options need to be updated
		/*struct {
			const BOOL *			pbSrc;
			BOOL *					pbCopy;
		} aStruct[] = {
			{ &m_pmhMemHack->Options().bAligned, &oOptions.bAligned },
			{ &m_pmhMemHack->Options().bSameAsOriginal, &oOptions.bSameAsOriginal },
		};
		for ( size_t I = MX_ELEMENTS( aStruct ); I--; ) {
			if ( (*aStruct[I].pbSrc) != (*aStruct[I].pbCopy) ) {
				m_pmhMemHack->SetOptions( oOptions );
				break;
			}
		}*/
		return 0;
	}

	// Shows a new pointer search.
	uint32_t CMhsMainWindow::ShowNewPointerSearch() {
		if ( !m_pmhMemHack->Process().ProcIsOpened() ) {
			if ( !CBase::PromptYesNo( Wnd(), _DEC_WS_77E541C6_You_must_open_a_process_before_you_can_perform_a_search__r_nOpen_a_process_now_.c_str(),
				_DEC_WS_8C0AA5EB_No_Process_Opened.c_str() ) ) { return 0; }
			if ( !OpenProcess() ) { return 0; }
		}
		// Should not be possible, but just one last check.
		if ( !m_pmhMemHack->Process().ProcIsOpened() ) { return 0; }
		m_stLastSearchType = MX_ST_POINTER;

		//MX_OPTIONS oOptions = m_pmhMemHack->Options();
		CNewPointerSearchLayout::CreateNewPointerSearchDialog( this, m_pmhMemHack );
		return 0;
	}

	// Shows a new string search.
	uint32_t CMhsMainWindow::ShowNewStringSearch() {
		if ( !m_pmhMemHack->Process().ProcIsOpened() ) {
			if ( !CBase::PromptYesNo( Wnd(), _DEC_WS_77E541C6_You_must_open_a_process_before_you_can_perform_a_search__r_nOpen_a_process_now_.c_str(),
				_DEC_WS_8C0AA5EB_No_Process_Opened.c_str() ) ) { return 0; }
			if ( !OpenProcess() ) { return 0; }
		}
		// Should not be possible, but just one last check.
		if ( !m_pmhMemHack->Process().ProcIsOpened() ) { return 0; }
		m_stLastSearchType = MX_ST_STRING;

		//MX_OPTIONS oOptions = m_pmhMemHack->Options();
		CNewStringSearchLayout::CreateNewStringSearchDialog( this, m_pmhMemHack );
		return 0;
	}

	// Shows a new expression search.
	uint32_t CMhsMainWindow::ShowNewExpressionSearch() {
		if ( !m_pmhMemHack->Process().ProcIsOpened() ) {
			if ( !CBase::PromptYesNo( Wnd(), _DEC_WS_77E541C6_You_must_open_a_process_before_you_can_perform_a_search__r_nOpen_a_process_now_.c_str(),
				_DEC_WS_8C0AA5EB_No_Process_Opened.c_str() ) ) { return 0; }
			if ( !OpenProcess() ) { return 0; }
		}
		// Should not be possible, but just one last check.
		if ( !m_pmhMemHack->Process().ProcIsOpened() ) { return 0; }
		m_stLastSearchType = MX_ST_EXPRESSION;

		//MX_OPTIONS oOptions = m_pmhMemHack->Options();
		CNewExpressionSearchLayout::CreateNewExpressionSearchDialog( this, m_pmhMemHack );
		return 0;
	}

	// Handles opening a process via the Open Process dialog (returns true if a process was actually opened).
	bool CMhsMainWindow::OpenProcess() {
		MX_OPTIONS oOptions = m_pmhMemHack->Options();
		oOptions.pmhMemHackObj = m_pmhMemHack;
		DWORD dwId = COpenProcessLayout::CreateOpenProcessDialog( this, &oOptions );
		if ( dwId != DWINVALID ) {
			if ( m_pmhMemHack->OpenProcess( dwId ) ) {
				return true;
			}
		}
		return false;
	}

	// Starts a search on the current thread (when activated via script or plug-in) or on a new thread (normal).
	bool CMhsMainWindow::Search( CSearcher::MX_SEARCH_PARMS * _pspParms, CWidget * _pwParent, bool _bUseSepThread, CWidget * _pwProgress ) {
		_pspParms->ui64MaxChunkLen = m_pmhMemHack->Options().ui64BufferSize;
		_pspParms->dwRegions = (m_pmhMemHack->Options().bMemImage ? MEM_IMAGE : 0) |
			(m_pmhMemHack->Options().bMemMapped ? MEM_MAPPED : 0) |
			(m_pmhMemHack->Options().bMemPrivate ? MEM_PRIVATE : 0);
		_pspParms->bUseEpsilon = m_pmhMemHack->Options().bUseEpsilon;
		_pspParms->bUseSmartEpsilon = m_pmhMemHack->Options().bSmartEpsilon;
		_pspParms->dEpsilonValue = m_pmhMemHack->Options().dEpsilon;
		_pspParms->iThreadPriority = m_pmhMemHack->Options().iThreadPriority;
		_pspParms->bPause = m_pmhMemHack->Options().bPauseTarget;
		_pspParms->bsByteSwapping = static_cast<CUtilities::MX_BYTESWAP>(m_pmhMemHack->Options().bsByteswap);


		if ( _bUseSepThread ) {
			CSearchProgressLayout::MX_SEARCH_DLG_PARMS sdpParms;
			sdpParms.pmmwWindow = this;
			sdpParms.pspSearch = _pspParms;
			return CSearchProgressLayout::CreateSearchProgressDialog( _pwParent, &sdpParms );
		}


		LONG lSuspend = STATUS_ABANDONED;
		if ( _pspParms->bPause ) { lSuspend = MemHack()->Process().SuspendProcess(); }
		uint64_t ui64TotalFound = 0;
		bool bRes = m_pmhMemHack->Searcher().Search( (*_pspParms), _pwProgress ? _pwProgress->Wnd() : NULL, &ui64TotalFound );
		
		if ( !bRes ) {
			//CSystem::MessageBoxError( Wnd(), _T_5868E6EA_An_error_occurred_during_the_search_, _LEN_5868E6EA );
		}
		else if ( ui64TotalFound == 0 ) {
			CSystem::MessageBoxOk( Wnd(), _T_LEN_1B132271_No_results_found__previous_search_restored_, _T_LEN_8080EB34_Found_Nothing );
		}
		else {
			// Call UpdateFoundAddressWindowText() on the main thread.
			::SendNotifyMessageW( Wnd(), MX_CM_UPDATE_FA, 0, 0 );

			/*CSecureString sMsg = mx::CStringDecoder::DecodeToString( _T_192AD0C2_Found__u_results___f_seconds___f_results_per_second__, _LEN_192AD0C2 );
			CSecureString sTitle = mx::CStringDecoder::DecodeToString( _T_E9D1378C_Found_Some, _LEN_E9D1378C );
			char szBuffer[128];
			uint64_t ui64SearchTime = std::max( 1ULL, m_pmhMemHack->Searcher().SearchTime() );
				
			::sprintf_s( szBuffer, sMsg.c_str(), ui64TotalFound,
				ui64SearchTime / 1000000.0,
				ui64TotalFound * 1000000.0 / ui64SearchTime );
			lsw::CBase::PromptOk( Wnd(), szBuffer, sTitle.c_str(), false );
			::ZeroMemory( szBuffer, sizeof( szBuffer ) );*/

			if ( FoundAddresses() ) {
				FoundAddresses()->UpdateHeaders( MemHack()->Searcher().LastSearchType() );
				FoundAddresses()->AddFoundAddresses();
			}
		}
		if ( lSuspend == STATUS_SUCCESS ) {
			MemHack()->Process().ResumeProcess();
		}
		

		return true;
	}

	// Informs that a child was removed from a child control (IE this control's child had a child control removed from it).
	// Is also called on the control from which a child was removed for convenience.
	void CMhsMainWindow::ChildWasRemoved( const CWidget * _pwChild ) {
		if ( m_pfaFoundAddresses && static_cast<CWidget *>(m_pfaFoundAddresses) == _pwChild ) {
			m_pfaFoundAddresses = nullptr;
			UpdateWindowChecks();
		}
		if ( m_peeExpEval && static_cast<CWidget *>(m_peeExpEval) == _pwChild ) {
			m_peeExpEval = nullptr;
			UpdateWindowChecks();
		}
		if ( m_cwConverter && static_cast<CWidget *>(m_cwConverter) == _pwChild ) {
			m_cwConverter = nullptr;
			UpdateWindowChecks();
		}
		if ( m_ppwPeWorks && static_cast<CWidget *>(m_ppwPeWorks) == _pwChild ) {
			m_ppwPeWorks = nullptr;
		}
		if ( m_pstwStringTheory && static_cast<CWidget *>(m_pstwStringTheory) == _pwChild ) {
			m_pstwStringTheory = nullptr;
		}
		if ( m_pfpsFloatingPointStudioWindow && static_cast<CWidget *>(m_pfpsFloatingPointStudioWindow) == _pwChild ) {
			m_pfpsFloatingPointStudioWindow = nullptr;
		}
		CMainWindow::ChildWasRemoved( _pwChild );
	}

	// Update the "Window" checks.
	void CMhsMainWindow::UpdateWindowChecks() {
		HMENU hMenu = ::GetMenu( Wnd() );
		if ( hMenu ) {
			//BOOL bCheck = m_pfaFoundAddresses && m_pfaFoundAddresses->
			::CheckMenuItem( hMenu, CMainWindowLayout::MX_MWMI_SHOWFOUNDADDR, m_pfaFoundAddresses ? MF_CHECKED : MF_UNCHECKED );
			::CheckMenuItem( hMenu, CMainWindowLayout::MX_MWMI_SHOW_EXPEVAL, m_peeExpEval ? MF_CHECKED : MF_UNCHECKED );
			::CheckMenuItem( hMenu, CMainWindowLayout::MX_MWMI_SHOW_CONVERTER, m_cwConverter ? MF_CHECKED : MF_UNCHECKED );
		}
	}

	// Updates the Found Address window text.
	void CMhsMainWindow::UpdateFoundAddressWindowText() {
		if ( FoundAddresses() ) {
			CSearcher::MX_SEARCH_LOCK slSearchLock( &MemHack()->Searcher() );
			if ( m_pmhMemHack->Searcher().TotalResults() ) {
				CSecureString sMsg = mx::CStringDecoder::DecodeToString( _T_LEN_82CDC580_Found_Addresses___I64u_results_in__f_seconds_ );

				char szBuffer[128];
				uint64_t ui64SearchTime = std::max( 1ULL, m_pmhMemHack->Searcher().SearchTime() );
				
				::sprintf_s( szBuffer, sMsg.c_str(), m_pmhMemHack->Searcher().TotalResults(),
					ui64SearchTime / 1000000.0 );
				FoundAddresses()->SetTextA( szBuffer );
				//::SendMessageA( FoundAddresses()->Wnd(), WM_SETTEXT, 0, reinterpret_cast<LPARAM>(szBuffer) );
				//::SendNotifyMessageA( FoundAddresses()->Wnd(), WM_SETTEXT, 0, reinterpret_cast<LPARAM>(szBuffer) );
			}
			else {
				CSecureWString sMsg = _DEC_WS_2741E247_Found_Addresses;
				FoundAddresses()->SetTextW( sMsg.c_str() );
			}
		}
	}

	// Do polling-based hotkeys.
	void CMhsMainWindow::PollHotkeys( INT _iId ) {
		m_pmhMemHack->HotkeyManager().HandlePollHotkey( _iId );
	}

	// Do hook-based hotkeys.
	void CMhsMainWindow::PollHookKeys( INT _iId ) {
		m_pmhMemHack->HotkeyManager().HandleHookHotkey( _iId );
	}

	// Hotkey handler for opening the options window.
	void __stdcall CMhsMainWindow::Hotkey_OpenOptions( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ ) {
		CMhsMainWindow * pmhThis = reinterpret_cast<CMhsMainWindow *>(_uiParm0);
		pmhThis->ShowOptions( pmhThis->LastOptionsShown() );
	}

	// Hotkey handler for showing the Found Addresses window.
	void __stdcall CMhsMainWindow::Hotkey_ShowFoundAddresses( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ ) {
		CMhsMainWindow * pmhThis = reinterpret_cast<CMhsMainWindow *>(_uiParm0);
		pmhThis->ShowFoundAddress();
	}

	// Hotkey handler for showing the Expression Evaluator window.
	void __stdcall CMhsMainWindow::Hotkey_ShowExpressionEvaluator( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ ) {
		CMhsMainWindow * pmhThis = reinterpret_cast<CMhsMainWindow *>(_uiParm0);
		pmhThis->ShowExpEval();
	}

	// Hotkey handler for showing the Converter window.
	void __stdcall CMhsMainWindow::Hotkey_ShowConverter( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ ) {
		CMhsMainWindow * pmhThis = reinterpret_cast<CMhsMainWindow *>(_uiParm0);
		pmhThis->ShowConverter();
	}

	// Hotkey handler for showing the PE Works window.
	void __stdcall CMhsMainWindow::Hotkey_ShowPeWorks( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ ) {
		CMhsMainWindow * pmhThis = reinterpret_cast<CMhsMainWindow *>(_uiParm0);
		pmhThis->ShowPeWorks();
	}

	// Hotkey handler for showing the String Theory window.
	void __stdcall CMhsMainWindow::Hotkey_ShowStringTheory( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ ) {
		CMhsMainWindow * pmhThis = reinterpret_cast<CMhsMainWindow *>(_uiParm0);
		pmhThis->ShowStringTheory();
	}

}	// namespace mx
