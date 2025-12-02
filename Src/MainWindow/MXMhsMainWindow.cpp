#include "MXMhsMainWindow.h"
#include "../Layouts/MXAddFoundAddressWindowLayout.h"
#include "../Layouts/MXConverterLayout.h"
#include "../Layouts/MXExpressionEvaluatorLayout.h"
#include "../Layouts/MXFloatingPointStudioLayout.h"
#include "../Layouts/MXFoundAddressEditLayout.h"
#include "../Layouts/MXFoundAddressLayout.h"
#include "../Layouts/MXDeuHexMachinaWindowLayout.h"
#include "../Layouts/MXLayoutMacros.h"
#include "../Layouts/MXLayoutManager.h"
#include "../Layouts/MXMoveAddressesWindowLayout.h"
#include "../Layouts/MXNewDataTypeSearchLayout.h"
#include "../Layouts/MXNewExpressionSearchLayout.h"
#include "../Layouts/MXNewPointerSearchLayout.h"
#include "../Layouts/MXNewStringSearchLayout.h"
#include "../Layouts/MXOpenProcessLayout.h"
#include "../Layouts/MXOptionsLayout.h"
#include "../Layouts/MXPeWorksLayout.h"
#include "../Layouts/MXSearchProgressLayout.h"
#include "../Layouts/MXStandardSubsearchLayout.h"
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
#include <commdlg.h>
#include <mbctype.h>
#include <mbstring.h>

//#include <MLang.h>

namespace mx {

	CMhsMainWindow::CMhsMainWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout.AddStyle( WS_CLIPCHILDREN ), _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pfaFoundAddresses( nullptr ),
		m_peeExpEval( nullptr ),
		m_ppwPeWorks( nullptr ),
		m_pstwStringTheory( nullptr ),
		m_pfpsFloatingPointStudioWindow( nullptr ),
		m_cwConverter( nullptr ),
		m_stLastSearchType( MX_ST_DATA_TYPE ),
		m_sstLastSubsearchType( MX_SST_STANDARD ),
		m_i32LastOptionsPage( COptionsWindow::MX_P_GENERAL ) {

		m_pmhMemHack = reinterpret_cast<CWindowMemHack *>(_ui64Data);
		

		static const struct {
			LPCWSTR				lpwsImageName;
			DWORD				dwConst;
		} sImages[] = {
			{ L"23", MX_I_OPENPROCESS },
			{ L"101", MX_I_OPENFORDEBUG },

			{ L"1", MX_I_ADDENTRY },
			{ L"52", MX_I_OPENENTRY },
			{ L"22", MX_I_SAVEENTRY },
			{ L"86", MX_I_SAVEENTRYAS },

			{ L"33", MX_I_DELETEENTRY },
			{ L"31", MX_I_DELETEALL },

			{ L"27", MX_I_LOCK },
			{ L"28", MX_I_UNLOCK },
			{ L"127", MX_I_LOCKONCE },

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
	}

	CMhsMainWindow::~CMhsMainWindow() {
		if ( m_uiptrUpdateListTimer ) {
			CSystem::KillTimer( Wnd(), m_uiptrUpdateListTimer );
			m_uiptrUpdateListTimer = 0;
		}
		if ( m_uiptrUpdateLocksTimer ) {
			CSystem::KillTimer( Wnd(), m_uiptrUpdateLocksTimer );
			m_uiptrUpdateLocksTimer = 0;
		}
		

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

		delete m_pdhmwDeusHexMachinaWindow;
		m_pdhmwDeusHexMachinaWindow = nullptr;

		delete m_cwConverter;
		m_cwConverter = nullptr;
	}

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CMhsMainWindow::InitDialog() {
		static CHotkeyManBase::MX_HOTKEY_HANDLER hhHandlers[] = {
		//	uiFuncParm0							pfFunc							hdName											uiId								sParms	hdParms																			hdFormattings
			{ reinterpret_cast<uint64_t>(this),	Hotkey_ShowAddFoundAddress,		{ _T_LEN_2FD68502_Add_Entry },					MX_WH_SHOW_ADD_ENTRY,				0 },
			{ reinterpret_cast<uint64_t>(this),	Hotkey_ShowEdit,				{ _T_LEN_F2BB02E1_Show_Found_Address_Edit },	MX_WH_SHOW_EDIT,					0 },
			{ reinterpret_cast<uint64_t>(this),	Hotkey_OpenOptions,				{ _T_LEN_159BB63C_Show_Options },				MX_WH_SHOW_OPTIONS,					0 },
			{ reinterpret_cast<uint64_t>(this),	Hotkey_ShowFoundAddresses,		{ _T_LEN_A5501E10_Show_Found_Addresses },		MX_WH_SHOW_FOUND_ADDRESSES,			0 },
			{ reinterpret_cast<uint64_t>(this),	Hotkey_ShowExpressionEvaluator,	{ _T_LEN_0661F178_Show_Expression_Evaluator },	MX_WH_SHOW_EXPRESSION_EVALUATOR,	0 },
			{ reinterpret_cast<uint64_t>(this),	Hotkey_ShowConverter,			{ _T_LEN_9118A28E_Show_Converter },				MX_WH_SHOW_CONVERTER,				0 },
			{ reinterpret_cast<uint64_t>(this),	Hotkey_ShowPeWorks,				{ _T_LEN_759D0F03_Show_PE_Works },				MX_WH_SHOW_PE_WORKS,				0 },
			{ reinterpret_cast<uint64_t>(this),	Hotkey_ShowStringTheory,		{ _T_LEN_B7A8A10D_Show_String_Theory },			MX_WH_SHOW_STRING_THEORY,			0 },
			{ reinterpret_cast<uint64_t>(this),	Hotkey_ShowFloatingPointStudio,	{ _T_LEN_155439B4_Show_Floating_Point_Studio },	MX_WH_SHOW_FLOATING_POINT_STUDIO,	0 },
			{ reinterpret_cast<uint64_t>(this),	Hotkey_LockSelected,			{ _T_LEN_14B8120F_Lock_Selected },				MX_WH_LOCK_SELECTED,				0 },
			{ reinterpret_cast<uint64_t>(this),	Hotkey_UnLockSelected,			{ _T_LEN_85399C8F_Unlock_Selected },			MX_WH_UNLOCK_SELECTED,				0 },
			{ reinterpret_cast<uint64_t>(this),	Hotkey_UnLockAll,				{ _T_LEN_EE088C0D_Unlock_All },					MX_WH_UNLOCK_ALL,					0 },
		};
		for ( size_t I = 0; I < MX_ELEMENTS( hhHandlers ); ++I ) {
			CHotkeyManBase::RegisterHotkeyHandler( hhHandlers[I] );
		}

		

		CToolBar * plvToolBar = static_cast<CToolBar *>(FindChild( Layout::MX_MWI_TOOLBAR0 ));
		CRebar * plvRebar = static_cast<CRebar *>(FindChild( Layout::MX_MWI_REBAR0 ));

		// ==== TOOL BAR ==== //
		plvToolBar->SetImageList( 0, m_iImages );
//#define MX_TOOL_STR( TXT )					reinterpret_cast<INT_PTR>(TXT)
#define MX_TOOL_STR( TXT )						0
		const TBBUTTON bButtons[] = {
			// iBitmap							idCommand									fsState				fsStyle			bReserved	dwData	iString
			{ m_iImageMap[MX_I_OPENPROCESS],	Layout::MX_MWMI_OPENPROCESS,				TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Open Process" ) },
			{ m_iImageMap[MX_I_OPENFORDEBUG],	Layout::MX_MWMI_OPENFORDEBUG,				TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Open for Debug") },
			{ -1,								0,											TBSTATE_ENABLED,	BTNS_SEP,		{ 0 },		0,		0 },
			{ m_iImageMap[MX_I_ADDENTRY],		Layout::MX_MWMI_ADDENTRY,					TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Add Entry" ) },
			{ m_iImageMap[MX_I_OPENENTRY],		Layout::MX_MWMI_OPENSAVEFILE,				TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Open Save File" ) },
			{ m_iImageMap[MX_I_SAVEENTRY],		Layout::MX_MWMI_SAVE,						TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Save Entries" ) },
			{ m_iImageMap[MX_I_SAVEENTRYAS],	Layout::MX_MWMI_SAVEAS,						TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Save Entries As" ) },
			{ -1,								0,											TBSTATE_ENABLED,	BTNS_SEP,		{ 0 },		0,		0 },
			{ m_iImageMap[MX_I_DELETEENTRY],	Layout::MX_MWMI_DELETE,						TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Delete Entry" ) },
			{ m_iImageMap[MX_I_DELETEALL],		Layout::MX_MWMI_DELETEALL,					TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Delete All Entries" ) },
			{ -1,								0,											TBSTATE_ENABLED,	BTNS_SEP,		{ 0 },		0,		0 },
			{ m_iImageMap[MX_I_LOCK],			Layout::MX_MWMI_LOCK,						TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Lock Selected" ) },
			{ m_iImageMap[MX_I_UNLOCK],			Layout::MX_MWMI_UNLOCK,						TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Unlock Selected" ) },
			{ m_iImageMap[MX_I_LOCKONCE],		Layout::MX_MWMI_LOCK_ONCE,					TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Write Once" ) },
			{ -1,								0,											TBSTATE_ENABLED,	BTNS_SEP,		{ 0 },		0,		0 },
			{ m_iImageMap[MX_I_EDIT],			Layout::MX_MWMI_EDIT,						TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Edit Selected" ) },
			{ -1,								0,											TBSTATE_ENABLED,	BTNS_SEP,		{ 0 },		0,		0 },
			{ m_iImageMap[MX_I_OPTIONS],		Layout::MX_MWMI_OPTIONS,					TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Options" ) },
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
			riRebarInfo.SetId( Layout::MX_MWI_TOOLBAR0 );
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
		CTreeListView * plvAddressList = MainTreeView();
		if ( plvAddressList ) {
			plvAddressList->DeleteAllColumns();
			const struct {
				const char * _pcText;
				size_t sLen;
				DWORD dwWidth;
			} aTitles[] = {
				{ _T_EB78CFF1_Description, _LEN_EB78CFF1, 350 },
				{ _T_C2F3561D_Address, _LEN_C2F3561D, 100 },
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
			plvAddressList->SetSortMethod( 2, ee::EE_TP_NATURAL_CI );
			plvAddressList->SetSortMethod( 3, ee::EE_TP_NATURAL_CI );
		}

		// TEMP.
		/*CSplitter * pwSplitter = static_cast<CSplitter *>(FindChild( Layout::MX_MWI_SPLITTER ));
		pwSplitter->SetSplitterType( CSplitter::LSW_SS_VERTICAL );
		pwSplitter->Attach( MainListView(), CSplitter::LSW_A_RIGHT );*/

		CMultiSplitter * pwSplitter = MultiSplitter();
		CMultiSplitter::LSW_DT_ATTACH aAttach;
		aAttach.atAttachTo = CMultiSplitter::LSW_AT_RIGHT;
		aAttach.dwId = pwSplitter->RootId();
		aAttach.pwWidget = FindChild( Layout::MX_MWI_TEST0 );
		//pwSplitter->Attach( aAttach );
		aAttach.pwWidget = MainTreeView();
		pwSplitter->Attach( aAttach );

		aAttach.atAttachTo = CMultiSplitter::LSW_AT_TOP;
		aAttach.pwWidget = FindChild( Layout::MX_MWI_TEST1 );
		//pwSplitter->Attach( aAttach );


		ForceSizeUpdate();

		LoadSettings( CSecureWString(), true );

		ShowFoundAddress();
		ShowExpEval();
		ShowConverter();

		reinterpret_cast<CHotkeyPollingMethod *>(m_pmhMemHack->HotkeyManager().GetPollingHotkeyManager())->SetWindow( Wnd() );
		reinterpret_cast<CHotkeyWhKeyboardMethod *>(m_pmhMemHack->HotkeyManager().GetWhKeyboardHotkeyManager())->SetWindow( Wnd() );
		reinterpret_cast<CHotkeyRegisterHotKeyMethod *>(m_pmhMemHack->HotkeyManager().GetRegisteredHotkeyManager())->SetWindow( Wnd() );
		m_pmhMemHack->HotkeyManager().Start();

		std::random_device rdRand;
		std::mt19937 mGen( rdRand() );
		std::uniform_int_distribution<> uidDist( MX_T_UPDATE_LIST, MX_T_UPDATE_LIST + 16 );
		m_uiptrUpdateListTimer = CSystem::SetTimer( Wnd(), uidDist( mGen ), 1000 / m_pmhMemHack->Options().dwMainRefresh, NULL );
		m_uiptrUpdateLocksTimer = CSystem::SetTimer( Wnd(), uidDist( mGen ), 1000 / m_pmhMemHack->Options().dwLockedRefresh, NULL );
		

		UpdateOpenRecent();
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CMhsMainWindow::Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc ) {
		if ( _wId >= Layout::MX_MWMI_SHOW_RECENT_BASE ) {
			_wId -= Layout::MX_MWMI_SHOW_RECENT_BASE;
			if ( _wId < m_pmhMemHack->Options().vProcessHistory.size() ) {
				if ( m_pmhMemHack->Options().vProcessHistory[_wId].size() ) {
					OpenProcessByPath( m_pmhMemHack->Options().vProcessHistory[_wId] );
				}
			}
			return LSW_H_CONTINUE;
		}
		if ( _wId >= Layout::MX_MWMI_USER_PROGRAMS_BASE && _wId < Layout::MX_MWMI_SHOW_RECENT_BASE ) {
			if ( !m_pmhMemHack ) { return LSW_H_CONTINUE; }
			size_t stIdx = _wId - Layout::MX_MWMI_USER_PROGRAMS_BASE;

			m_pmhMemHack->ExecuteProgramByIdx( stIdx );
			return LSW_H_CONTINUE;
		}
		switch ( _wId ) {
			case Layout::MX_MWMI_OPENPROCESS : {
				OpenProcess();
				break;
			}
			case Layout::MX_MWMI_DATATYPE : {
				ShowNewDataTypeSearch();
				break;
			}
			case Layout::MX_MWMI_POINTER : {
				ShowNewPointerSearch();
				break;
			}
			case Layout::MX_MWMI_STRING : {
				ShowNewStringSearch();
				break;
			}
			case Layout::MX_MWMI_SEARCHOPTIONS : {
				ShowOptions( SearchOptionsPageClamp() );
				break;
			}
			case Layout::MX_MWMI_OPTIONS : {
				ShowOptions( LastOptionsShown() );
				break;
			}
			case Layout::MX_MWMI_PEWORKS : {
				ShowPeWorks();
				break;
			}
			case Layout::MX_MWMI_STRINGTHEORY : {
				ShowStringTheory();
				break;
			}
			case Layout::MX_MWMI_FLOATINGPOINTSTUDIO : {
				ShowFloatingPointStudio( std::nan( "" ) );
				break;
			}
			case Layout::MX_MWMI_DEUS_HEX_MACHINA : {
				ShowDeusHexMachina();
				break;
			}
			case Layout::MX_MWMI_EXPRESSION : {
				ShowNewExpressionSearch();
				break;
			}
			case Layout::MX_MWMI_SUB : {
				ShowStandardSubsearch();
				break;
			}
			case Layout::MX_MWMI_SHOWFOUNDADDR : {
				ShowFoundAddress();
				break;
			}
			case Layout::MX_MWMI_SHOW_EXPEVAL : {
				ShowExpEval();
				break;
			}
			case Layout::MX_MWMI_SHOW_CONVERTER : {
				ShowConverter();
				break;
			}
			case Layout::MX_MWMI_SHOW_ALL : {
				ShowFoundAddress();
				ShowExpEval();
				ShowConverter();
				break;
			}
			case Layout::MX_MWMI_ADDENTRY : {
				AddEntry();
				break;
			}
			case Layout::MX_MWMI_OPENSAVEFILE : {
				Open();
				break;
			}
			case Layout::MX_MWMI_SAVE : {
				Save();
				break;
			}
			case Layout::MX_MWMI_SAVEAS : {
				SaveAs();
				break;
			}
			case Layout::MX_MWMI_SAVE_SELECTED_AS : {
				SaveSelectedAs();
				break;
			}
			case Layout::MX_MWMI_DELETE : {
				DeleteSelected();
				break;
			}
			case Layout::MX_MWMI_DELETEALL : {
				DeleteAll();
				break;
			}
			case Layout::MX_MWMI_LOCK : {
				LockSelected();
				break;
			}
			case Layout::MX_MWMI_UNLOCK : {
				UnlockSelected();
				break;
			}
			case Layout::MX_MWMI_UNLOCK_ALL : {
				UnlockAll();
				break;
			}
			case Layout::MX_MWMI_LOCK_ONCE : {
				WriteOnce();
				break;
			}
			case Layout::MX_MWMI_EDIT : {
				ShowEdit();
				break;
			}
			case Layout::MX_MWMI_MOVE_UP : {
				MoveUp();
				break;
			}
			case Layout::MX_MWMI_MOVE_DOWN : {
				MoveDown();
				break;
			}
			case Layout::MX_MWMI_SET_MOVE_ADDRESS_SOURCE : {
				SetMoveAddressSource();
				break;
			}
			case Layout::MX_MWMI_MOVE_ADDRESS : {
				MoveSelectedAddresses();
				break;
			}
			case Layout::MX_MWMI_COPY_ADDRESS : {
				CopySelectedAddresses();
				break;
			}
			case Layout::MX_MWMI_EXIT : {
				Destroy();
				NcDestroy();
				break;
			}
		}
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from menu.
	CWidget::LSW_HANDLED CMhsMainWindow::MenuCommand( WORD _wId ) {
		return Command( 0, _wId, NULL );
	}

	// WM_ERASEBKGND.
	CWidget::LSW_HANDLED CMhsMainWindow::EraseBkgnd( HDC _hDc ) {
		return LSW_H_HANDLED;
	}

	// WM_DESTROY.
	CWidget::LSW_HANDLED CMhsMainWindow::Destroy() {
		if ( m_pmhMemHack ) {
			//m_pmhMemHack->SaveSettings( CSecureWString(), true );
			SaveSettings( CSecureWString(), true );
		}
		return LSW_H_CONTINUE;
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
		return static_cast<CStatusBar *>(FindChild( Layout::MX_MWI_STATUSBAR ));
	}

	// Gets the status bar.
	const CStatusBar * CMhsMainWindow::StatusBar() const {
		return static_cast<const CStatusBar *>(FindChild( Layout::MX_MWI_STATUSBAR ));
	}

	// Gets the list view.
	CTreeListView * CMhsMainWindow::MainTreeView() {
		return static_cast<CTreeListView *>(FindChild( Layout::MX_MWI_STOREDADDRESSES ));
	}

	// Gets the multi-splitter.
	CMultiSplitter * CMhsMainWindow::MultiSplitter() {
		return static_cast<CMultiSplitter *>(FindChild( Layout::MX_MWI_SPLITTER ));
	}

	// Gets the multi-splitter.
	const CMultiSplitter * CMhsMainWindow::MultiSplitter() const {
		return static_cast<const CMultiSplitter *>(FindChild( Layout::MX_MWI_SPLITTER ));
	}

	// Virtual client rectangle.  Can be used for things that need to be adjusted based on whether or not status bars, toolbars, etc. are present.
	const LSW_RECT CMhsMainWindow::VirtualClientRect( const CWidget * pwChild ) const {
		LSW_RECT rTemp = ClientRect( this );
		const CRebar * plvRebar = static_cast<const CRebar *>(FindChild( Layout::MX_MWI_REBAR0 ));
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
			case MX_CM_DETATCHED : {
				m_pmhMemHack->Detach( FALSE );
				break;
			}
		}
		return LSW_H_CONTINUE;
	}

	// Shows the Found Address dockable.
	void CMhsMainWindow::ShowFoundAddress() {
		if ( !m_pfaFoundAddresses ) {
			m_pfaFoundAddresses = static_cast<CFoundAddressesWindow *>(CFoundAddressLayout::CreateFoundAddressesWindow( MultiSplitter(), this ));
			if ( !m_pfaFoundAddresses ) { return; }
			m_pfaFoundAddresses->SetUpdateSpeed( 1000 / max( m_pmhMemHack->Options().dwFoundAddressRefresh, static_cast<DWORD>(1) ) );
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
			m_peeExpEval->AddDockTarget( MultiSplitter() );
			m_peeExpEval->SetAddressHandler( CMemHack::ExpAddressHandler, reinterpret_cast<uintptr_t>(m_pmhMemHack) );
			m_peeExpEval->SetAddressWriteHandler( CMemHack::ExpAddressWriteHandler, reinterpret_cast<uintptr_t>(m_pmhMemHack) );
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

	// Deletes the selected Found Addresses.
	void CMhsMainWindow::DeleteSelected() {
		auto ptlvView = MainTreeView();
		if ( ptlvView ) {
			auto famMan = m_pmhMemHack->FoundAddressManager();
			std::vector<LPARAM> vSelected;
			ptlvView->GatherSelectedLParam( vSelected, true );
			ptlvView->BeginLargeUpdate();
			for ( auto & I : vSelected ) {
				famMan->Delete( I );
				ptlvView->DeleteByLParam( I );
			}
			ptlvView->FinishUpdate();
		}
	}

	// Deletes all Found Addresses.
	void CMhsMainWindow::DeleteAll() {
		auto famMan = m_pmhMemHack->FoundAddressManager();
		famMan->DeleteAll();
		auto ptlvView = MainTreeView();
		if ( ptlvView ) {
			ptlvView->DeleteAll();
		}
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
			m_pfpsFloatingPointStudioWindow = static_cast<CFloatingPointStudioWindow *>(CFloatingPointStudioLayout::CreateFloatingPointStudioWindow( nullptr, m_pmhMemHack ));
			m_pfpsFloatingPointStudioWindow->SetWidgetParent( this );
		}
		if ( m_pfpsFloatingPointStudioWindow ) {

		}
	}

	// Shows the Deus Hex Machina window.
	void CMhsMainWindow::ShowDeusHexMachina() {
		if ( !m_pdhmwDeusHexMachinaWindow ) {
			m_pdhmwDeusHexMachinaWindow = static_cast<CDeusHexMachinaWindow *>(CDeusHexMachinaLayout::CreateDeusHexMachinaWindow( nullptr, m_pmhMemHack ));
			m_pdhmwDeusHexMachinaWindow->SetWidgetParent( this );
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
		return CNewDataTypeSearchLayout::CreateNewDataTypeSearchDialog( this, m_pmhMemHack );
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
		return CNewPointerSearchLayout::CreateNewPointerSearchDialog( this, m_pmhMemHack );
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
		return CNewStringSearchLayout::CreateNewStringSearchDialog( this, m_pmhMemHack );
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
		return CNewExpressionSearchLayout::CreateNewExpressionSearchDialog( this, m_pmhMemHack );
	}

	// Shows the standard subsearch dialog.
	uint32_t CMhsMainWindow::ShowStandardSubsearch() {
		if ( !m_pmhMemHack->Process().ProcIsOpened() ) {
			if ( !CBase::PromptYesNo( Wnd(), _DEC_WS_77E541C6_You_must_open_a_process_before_you_can_perform_a_search__r_nOpen_a_process_now_.c_str(),
				_DEC_WS_8C0AA5EB_No_Process_Opened.c_str() ) ) { return 0; }
			if ( !OpenProcess() ) { return 0; }
		}
		// Should not be possible, but just one last check.
		if ( !m_pmhMemHack->Process().ProcIsOpened() ) { return 0; }

		{
			if ( !MemHack()->Searcher().TotalResultsQuick() ) {
				uint32_t uiRet = ShowLastSearch();
				//if ( !uiRet ) { return 0; }
				if ( !MemHack()->Searcher().TotalResultsQuick() ) {
					CBase::PromptOk( Wnd(), _DEC_WS_C616FA6B_There_are_no_results_through_which_to_search_.c_str(),
						_DEC_WS_D42771B0_No_Results.c_str() );
					return 0;
				}
			}
		}
		m_sstLastSubsearchType = MX_SST_STANDARD;

		switch ( m_pmhMemHack->Searcher().LastSearchParms().stType ) {
			case CUtilities::MX_ST_DATATYPE_SEARCH : {
				CStandardSubsearchLayout::CreateStandardSubsearchDialog( this, m_pmhMemHack );
				break;
			}
			case CUtilities::MX_ST_EXP_SEARCH : {
				if ( m_pmhMemHack->Searcher().LastSearchParms().esqmExpSearchType == CUtilities::MX_EXP_SEARCH_QQ_MEANING::MX_ESQM_VALUE ) {
					CStandardSubsearchLayout::CreateStandardSubsearchDialog( this, m_pmhMemHack );
				}
				break;
			}
		}
		return 0;
	}

	// Shows the Edit dialog.
	uint32_t CMhsMainWindow::ShowEdit() {
		try {
			auto pwTree = MainTreeView();
			if ( pwTree ) {
				auto famMan = m_pmhMemHack->FoundAddressManager();				// Locks the Found Address Manager for modifications.
				std::vector<LPARAM> vSelected;
				if ( pwTree->GatherSelectedLParam( vSelected, true ) >= 1 ) {	// At least one thing is selected.
					CFoundAddressEditLayout::CreateEditDialog( this, MemHack(), vSelected );

					// Update colors.
					for ( auto I = vSelected.size(); I--; ) {
						auto hItem = pwTree->GetByLParam( vSelected[I] );
						if ( hItem ) {
							auto pfabThis = famMan->GetById( size_t( vSelected[I] ) );
							if ( pfabThis ) {
								pwTree->SetItemColor( hItem, pfabThis->Color() );
							}
						}
					}
				}
			}
		}
		catch ( ... ) {}
		return 0;
	}

	// Locks the selected items.
	void CMhsMainWindow::LockSelected() {
		auto pwTree = MainTreeView();
		if ( pwTree ) {
			auto famMan = m_pmhMemHack->FoundAddressManager();					// Locks the Found Address Manager for modifications.
			std::vector<LPARAM> vSelected;
			if ( pwTree->GatherSelectedLParam( vSelected, true ) >= 1 ) {		// At least one thing is selected.
				for ( auto I = vSelected.size(); I--; ) {
					auto hItem = pwTree->GetByLParam( vSelected[I] );
					if ( hItem ) {
						auto pfabThis = famMan->GetById( size_t( vSelected[I] ) );
						if ( pfabThis && pfabThis->Type() == MX_FAT_FOUND_ADDRESS ) {
							reinterpret_cast<CFoundAddress *>(pfabThis)->SetLocked( true );
							pwTree->SetItemColor( hItem, pfabThis->Color() );
						}
					}
				}
			}
		}
	}

	// Unlocks the selected items.
	void CMhsMainWindow::UnlockSelected() {
		auto pwTree = MainTreeView();
		if ( pwTree ) {
			auto famMan = m_pmhMemHack->FoundAddressManager();					// Locks the Found Address Manager for modifications.
			std::vector<LPARAM> vSelected;
			if ( pwTree->GatherSelectedLParam( vSelected, true ) >= 1 ) {		// At least one thing is selected.
				for ( auto I = vSelected.size(); I--; ) {
					auto hItem = pwTree->GetByLParam( vSelected[I] );
					if ( hItem ) {
						auto pfabThis = famMan->GetById( size_t( vSelected[I] ) );
						if ( pfabThis && pfabThis->Type() == MX_FAT_FOUND_ADDRESS ) {
							reinterpret_cast<CFoundAddress *>(pfabThis)->SetLocked( false );
							pwTree->SetItemColor( hItem, pfabThis->Color() );
						}
					}
				}
			}
		}
	}

	// Unlocks all items.
	void CMhsMainWindow::UnlockAll() {
		auto famMan = m_pmhMemHack->FoundAddressManager();						// Locks the Found Address Manager for modifications.
		(*famMan).UnlockAll();

		auto pwTree = MainTreeView();
		if ( pwTree ) {
			auto famMan = m_pmhMemHack->FoundAddressManager();					// Locks the Found Address Manager for modifications.
			std::vector<LPARAM> vSelected;
			if ( pwTree->GatherAllLParam( vSelected, true ) >= 1 ) {			// At least one thing exists.
				for ( auto I = vSelected.size(); I--; ) {
					auto hItem = pwTree->GetByLParam( vSelected[I] );
					if ( hItem ) {
						auto pfabThis = famMan->GetById( size_t( vSelected[I] ) );
						if ( pfabThis && pfabThis->Type() == MX_FAT_FOUND_ADDRESS ) {
							reinterpret_cast<CFoundAddress *>(pfabThis)->SetLocked( false );
							pwTree->SetItemColor( hItem, pfabThis->Color() );
						}
					}
				}
			}
		}
	}

	// Writes the lock value once.
	void CMhsMainWindow::WriteOnce() {
		auto pwTree = MainTreeView();
		if ( pwTree ) {
			auto famMan = m_pmhMemHack->FoundAddressManager();					// Locks the Found Address Manager for modifications.
			std::vector<LPARAM> vSelected;
			if ( pwTree->GatherSelectedLParam( vSelected, true ) >= 1 ) {		// At least one thing is selected.
				for ( auto I = vSelected.size(); I--; ) {
					auto hItem = pwTree->GetByLParam( vSelected[I] );
					if ( hItem ) {
						auto pfabThis = famMan->GetById( size_t( vSelected[I] ) );
						if ( pfabThis && pfabThis->Type() == MX_FAT_FOUND_ADDRESS ) {
							reinterpret_cast<CFoundAddress *>(pfabThis)->ApplyLock( true );
						}
					}
				}
			}
		}
	}

	// Move selected items up.
	void CMhsMainWindow::MoveUp() {
		auto pwTree = MainTreeView();
		if ( pwTree ) {
			std::vector<LPARAM> vSelected;
			if ( pwTree->GatherSelectedLParam( vSelected, true ) >= 1 ) {		// At least one thing is selected.
				pwTree->MoveUp( vSelected );
			}
		}
	}

	// Move selected items down.
	void CMhsMainWindow::MoveDown() {
		auto pwTree = MainTreeView();
		if ( pwTree ) {
			std::vector<LPARAM> vSelected;
			if ( pwTree->GatherSelectedLParam( vSelected, true ) >= 1 ) {		// At least one thing is selected.
				pwTree->MoveDown( vSelected );
			}
		}
	}

	// Sets the source address for the Move Address dialog.
	void CMhsMainWindow::SetMoveAddressSource() {
		auto pwTree = MainTreeView();
		if ( pwTree ) {
			int iCaret = m_iContextHitTest;
			if ( -1 != iCaret ) {
				auto htiItem = pwTree->GetItemByIndex( iCaret );
				if ( htiItem ) {
					LPARAM lpId = pwTree->GetItemLParam( htiItem );
					auto famMan = m_pmhMemHack->FoundAddressManager();			// Locks the Found Address Manager for modifications.
					auto pfabThis = famMan->GetById( size_t( lpId ) );
					if ( pfabThis && pfabThis->Type() == MX_FAT_FOUND_ADDRESS ) {
						m_ui64MoveAddressesSourceAddr = reinterpret_cast<CFoundAddress *>(pfabThis)->FinalAddress();
					}
				}
			}
		}
	}

	// Move selected addresses.
	void CMhsMainWindow::MoveSelectedAddresses() {
		CMoveAddressesWindowLayout::CreateMoveAddressDialog( this, MemHack() );
	}

	// Copy the selected addresses to the clipboard.
	void CMhsMainWindow::CopySelectedAddresses() {
		try {
			auto pwTree = MainTreeView();
			if ( pwTree ) {
				std::vector<LPARAM> vSelected;
				if ( pwTree->GatherSelectedLParam( vSelected, true ) >= 1 ) {		// At least one thing is selected.
					CSecureString ssClipText;
					auto famMan = m_pmhMemHack->FoundAddressManager();				// Locks the Found Address Manager for modifications.
					for ( size_t I = 0; I < vSelected.size(); ++I ) {
						auto pfabThis = famMan->GetById( size_t( vSelected[I] ) );
						if ( pfabThis && pfabThis->Type() == MX_FAT_FOUND_ADDRESS ) {
							if ( ssClipText.size() ) {
								ssClipText += ',';
								ssClipText += ' ';
							}
							CUtilities::PrintAddress( reinterpret_cast<CFoundAddress *>(pfabThis)->FinalAddress(), ssClipText );
						}
					}
					if ( ssClipText.size() ) {
						lsw::LSW_CLIPBOARD cbClibBoard( Wnd(), true );
						cbClibBoard.SetText( ssClipText.c_str(), ssClipText.size() );
					}
				}
			}
		}
		catch ( ... ) {
			// TODO: Display Error.
		}
	}

	// Handles opening a process via the Open Process dialog (returns true if a process was actually opened).
	bool CMhsMainWindow::OpenProcess() {
		MX_OPTIONS oOptions = m_pmhMemHack->Options();
		oOptions.pmhMemHackObj = m_pmhMemHack;
		DWORD dwId = COpenProcessLayout::CreateOpenProcessDialog( this, &oOptions );
		if ( dwId != DWINVALID ) {
			if ( m_pmhMemHack->OpenProcess( dwId ) ) {
				UpdateOpenRecent();
				return true;
			}
		}
		return false;
	}

	// Opens a process given its path.
	bool CMhsMainWindow::OpenProcessByPath( const CSecureWString &_swsPath ) {
		MX_OPTIONS oOptions = m_pmhMemHack->Options();

		/*CUtilities::AddOrMove( oOptions.vProcessHistory, _swsPath, MX_R_RECENT_PROCESSES_SIZE );
		m_pmhMemHack->SetOptions( oOptions );*/

		UpdateOpenRecent();
		return true;
	}

	// Updates the Open Recent menu.
	void CMhsMainWindow::UpdateOpenRecent() {
		HMENU hMenu = ::GetSubMenu( ::GetMenu( Wnd() ), 0 );
		HMENU hListMenu = ::GetSubMenu( hMenu, 1 );
		if ( hListMenu != NULL ) {
			if ( ::DestroyMenu( hListMenu ) ) {
				hListMenu = NULL;
			}
		}

		// Store a local copy so that the main list is able to change while we work (should never happen in practice but let's just code safely).
		std::vector<CSecureWString> vList = m_pmhMemHack->Options().vProcessHistory;
		::DeleteMenu( hMenu, 1, MF_BYPOSITION );
		auto swsOpenRecent = _DEC_WS_EA79B404_Open__Recent;
		if ( vList.size() ) {
			std::vector<LSW_MENU_ITEM> vMenu;
			LSW_MENU_ITEM miItem = { FALSE, 0, FALSE, FALSE, TRUE };
			for ( size_t I = 0; I < vList.size(); ++I ) {
				miItem.dwId = DWORD( Layout::MX_MWMI_SHOW_RECENT_BASE + I );
				miItem.lpwcText = reinterpret_cast<LPCWSTR>(vList[I].c_str());
				vMenu.push_back( miItem );
			}
			LSW_MENU_LAYOUT miMenus = {
				Layout::MX_MWMI_OPENRECENT,
				0,
				0,
				vMenu.size(),
				vMenu.data()
			};
			std::vector<ACCEL> vHotkeys;
			HMENU hThis = lsw::CBase::LayoutManager()->CreateMenu( &miMenus, 1, vHotkeys );
			//if ( !::ModifyMenuW( hMenu, CMainWindowLayout::LSN_MWMI_OPENRECENT, MF_BYCOMMAND | MF_POPUP | MF_STRING,
			//	reinterpret_cast<UINT_PTR>(hThis), LSN_LSTR( LSN_OPEN_REC_ENT ) ) ) {
			if ( ::InsertMenuW( hMenu, 1, MF_BYPOSITION | MF_POPUP | MF_STRING,
				reinterpret_cast<UINT_PTR>(hThis), swsOpenRecent.c_str() ) ) {
				::DestroyMenu( hThis );
			}
			return;
		}
		::InsertMenuW( hMenu, 1, MF_BYPOSITION | MF_GRAYED | MF_STRING,
			static_cast<UINT_PTR>(Layout::MX_MWMI_OPENRECENT), swsOpenRecent.c_str() );
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
			sdpParms.bSubSearch = false;
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
				
			std::sprintf( szBuffer, sMsg.c_str(), ui64TotalFound,
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

	// Starts a subsearch on the current thread (when activated via script or plug-in) or on a new thread (normal).
	bool CMhsMainWindow::Subsearch( CSearcher::MX_SUBSEARCH_PARMS * _pspParms, CWidget * _pwParent, bool _bUseSepThread, CWidget * _pwProgress ) {
		_pspParms->ui64MaxChunkLen = m_pmhMemHack->Options().ui64BufferSize;
		_pspParms->bUseEpsilon = m_pmhMemHack->Options().bUseEpsilon;
		_pspParms->bUseSmartEpsilon = m_pmhMemHack->Options().bSmartEpsilon;
		_pspParms->dEpsilonValue = m_pmhMemHack->Options().dEpsilon;
		_pspParms->iThreadPriority = m_pmhMemHack->Options().iThreadPriority;
		_pspParms->bPause = m_pmhMemHack->Options().bPauseTarget;
		_pspParms->bsByteSwapping = static_cast<CUtilities::MX_BYTESWAP>(m_pmhMemHack->Options().bsByteswap);

		if ( _bUseSepThread ) {
			CSearchProgressLayout::MX_SEARCH_DLG_PARMS sdpParms;
			sdpParms.pmmwWindow = this;
			sdpParms.pspSubsearch = _pspParms;
			sdpParms.bSubSearch = true;
			return CSearchProgressLayout::CreateSearchProgressDialog( _pwParent, &sdpParms );
		}

		LONG lSuspend = STATUS_ABANDONED;
		if ( _pspParms->bPause ) { lSuspend = MemHack()->Process().SuspendProcess(); }
		uint64_t ui64TotalFound = 0;
		bool bRes = m_pmhMemHack->Searcher().Subsearch( (*_pspParms), _pwProgress ? _pwProgress->Wnd() : NULL, &ui64TotalFound );

		if ( !bRes ) {
			//CSystem::MessageBoxError( Wnd(), _T_5868E6EA_An_error_occurred_during_the_search_, _LEN_5868E6EA );
		}
		else if ( ui64TotalFound == 0 ) {
			CSystem::MessageBoxOk( Wnd(), _T_LEN_D5B44DE0_No_results_found__previous_results_restored_, _T_LEN_8080EB34_Found_Nothing );
		}


		else {
			// Call UpdateFoundAddressWindowText() on the main thread.
			::SendNotifyMessageW( Wnd(), MX_CM_UPDATE_FA, 0, 0 );
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

	// Saves all program settings.
	bool CMhsMainWindow::SaveSettings( const std::wstring &_wsPath, bool _bAsJson ) const {
		CSecureWString wsFinalPath = CSystem::GetSelfPathW();
		if ( _wsPath.size() ) {
			wsFinalPath += _wsPath;
		}
		else {
			wsFinalPath += _bAsJson ? _DEC_WS_3F15B0CA_config_json : _DEC_WS_7B969963_app_config;
		}
		std::vector<uint8_t> vBuffer;
		if ( _bAsJson ) {
			lson::CJson::LSON_ELEMENT eRoot;
			lson::CJson::CreateObjectElement( "", eRoot );
			if ( !m_pmhMemHack->SaveSettings( &eRoot, nullptr, m_pmhMemHack->Options() ) ) { return false; }
			if ( !m_pmhMemHack->HotkeyManager().SaveSettings( &eRoot, nullptr ) ) { return false; }
			if ( !lson::CJson::WriteElement( eRoot, vBuffer, 0 ) ) { return false; }
			/*vBuffer.push_back( 0 );
			::OutputDebugStringA( reinterpret_cast<LPCSTR>(vBuffer.data()) );*/
		}
		else {
			
			CStream sStream( vBuffer );
			if ( !m_pmhMemHack->SaveSettings( nullptr, &sStream, m_pmhMemHack->Options() ) ) { return false; }
			if ( !m_pmhMemHack->HotkeyManager().SaveSettings( nullptr, &sStream ) ) { return false; }
		}
		
		CFile fFile;
		if ( !fFile.CreateNewFile( wsFinalPath.c_str(), FALSE ) ) { return false; }
		if ( !fFile.Write( vBuffer.data(), static_cast<DWORD>(vBuffer.size()) ) ) { return false; }
		
		return true;
	}

	// Loads all program settings.
	bool CMhsMainWindow::LoadSettings( const std::wstring &_wsPath, bool _bAsJson ) {
		CSecureWString wsFinalPath = CSystem::GetSelfPathW();
		if ( _wsPath.size() ) {
			wsFinalPath += _wsPath;
		}
		else {
			wsFinalPath += _bAsJson ? _DEC_WS_3F15B0CA_config_json : _DEC_WS_7B969963_app_config;
		}
		std::vector<uint8_t> vBuffer;
		CFile fFile;
		fFile.LoadToMemory( wsFinalPath.c_str(), vBuffer );
		if ( vBuffer.size() == 0 ) { return false; }
		MX_OPTIONS oOptions = m_pmhMemHack->Options();
		if ( _bAsJson ) {
			lson::CJson jSon;
			vBuffer.push_back( 0 );

			if ( !jSon.SetJson( reinterpret_cast<const char *>(vBuffer.data()) ) ) { return false; }
			if ( !m_pmhMemHack->LoadSettings( &jSon, nullptr, oOptions ) ) { return false; }
			if ( !m_pmhMemHack->HotkeyManager().LoadSettings( &jSon, nullptr ) ) { return false; }
		}
		else {
			CStream sStream( vBuffer );
			if ( !m_pmhMemHack->LoadSettings( nullptr, &sStream, oOptions ) ) { return false; }
			if ( !m_pmhMemHack->HotkeyManager().LoadSettings( nullptr, &sStream ) ) { return false; }
		}
		m_pmhMemHack->SetOptions( oOptions );

		return true;
	}

	// Update timer speed.
	void CMhsMainWindow::UpdateTimer() {
		if ( m_uiptrUpdateListTimer ) {
			m_uiptrUpdateListTimer = CSystem::SetTimer( Wnd(), m_uiptrUpdateListTimer, max( 1000 / MemHack()->Options().dwMainRefresh, static_cast<DWORD>(1) ), NULL );
		}
		if ( m_uiptrUpdateLocksTimer ) {
			m_uiptrUpdateLocksTimer = CSystem::SetTimer( Wnd(), m_uiptrUpdateLocksTimer, max( 1000 / MemHack()->Options().dwLockedRefresh, static_cast<DWORD>(1) ), NULL );
		}
	}

	// WM_TIMER.
	CWidget::LSW_HANDLED CMhsMainWindow::Timer( UINT_PTR _uiptrId, TIMERPROC _tpProc ) {
		if ( _uiptrId == m_uiptrUpdateListTimer ) {
			// Update the list.
			auto * plvList = MainTreeView();
			if ( plvList ) {
				::RedrawWindow( plvList->Wnd(), NULL, NULL, RDW_INVALIDATE | /*RDW_NOCHILDREN | */RDW_UPDATENOW | RDW_FRAME );
			}
		}
		if ( _uiptrId == m_uiptrUpdateLocksTimer ) {
			MemHack()->ApplyFoundAddressLocks();
		}
		return LSW_H_CONTINUE;
	}

	// WM_NOTIFY->NM_DBLCLK on the main list to edit the currently selected values.
	CWidget::LSW_HANDLED CMhsMainWindow::DblClk( const LPNMITEMACTIVATE _phHdr ) {
		if ( _phHdr->hdr.idFrom == Layout::MX_MWI_STOREDADDRESSES ) {
			ShowEdit();
			return LSW_H_HANDLED;
		}
		return LSW_H_CONTINUE;
	}

	// WM_NOTIFY->NM_DBLCLK for the owning window if the child either could not be resolved or returned LSW_HANDLED::LSW_H_CONTINUE.
	//CWidget::LSW_HANDLED CMhsMainWindow::DblClk( const LPNMITEMACTIVATE _phHdr, WORD /*_wControlId*/, CWidget * /*_pwWidget*/ ) {
	//	return LSW_H_CONTINUE;
	//}

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
		if ( m_pdhmwDeusHexMachinaWindow && static_cast<CWidget *>(m_pdhmwDeusHexMachinaWindow) == _pwChild ) {
			m_pdhmwDeusHexMachinaWindow = nullptr;
		}
		CMainWindow::ChildWasRemoved( _pwChild );
	}

	// Update the "Window" checks.
	void CMhsMainWindow::UpdateWindowChecks() {
		HMENU hMenu = ::GetMenu( Wnd() );
		if ( hMenu ) {
			//BOOL bCheck = m_pfaFoundAddresses && m_pfaFoundAddresses->
			::CheckMenuItem( hMenu, Layout::MX_MWMI_SHOWFOUNDADDR, m_pfaFoundAddresses ? MF_CHECKED : MF_UNCHECKED );
			::CheckMenuItem( hMenu, Layout::MX_MWMI_SHOW_EXPEVAL, m_peeExpEval ? MF_CHECKED : MF_UNCHECKED );
			::CheckMenuItem( hMenu, Layout::MX_MWMI_SHOW_CONVERTER, m_cwConverter ? MF_CHECKED : MF_UNCHECKED );
		}
	}

	// Updates the Found Address window text.
	void CMhsMainWindow::UpdateFoundAddressWindowText() {
		if ( FoundAddresses() ) {
			CSearcher::MX_SEARCH_LOCK slSearchLock( &MemHack()->Searcher() );
			if ( m_pmhMemHack->Searcher().TotalResults() ) {
				CSearcher & sSearcher = m_pmhMemHack->Searcher();
				char szBuffer[128];
				uint64_t ui64SearchTime = std::max( 1ULL, sSearcher.SearchTime() );
				CSecureString sMsg;
				if ( sSearcher.LastSearchTotalResults() > sSearcher.TotalResults() ) {
					sMsg = mx::CStringDecoder::DecodeToString( _T_LEN_37BD2A11_Found_Addresses___I64u_results_in__f_seconds____I64u_ );
					std::sprintf( szBuffer, sMsg.c_str(), sSearcher.TotalResults(),
						ui64SearchTime / 1000000.0,
						sSearcher.LastSearchTotalResults() - sSearcher.TotalResults() );
				}
				else {
					sMsg = mx::CStringDecoder::DecodeToString( _T_LEN_82CDC580_Found_Addresses___I64u_results_in__f_seconds_ );
					std::sprintf( szBuffer, sMsg.c_str(), sSearcher.TotalResults(),
						ui64SearchTime / 1000000.0 );
				}

				
				
				
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

	// WM_CONTEXTMENU.
	CWidget::LSW_HANDLED CMhsMainWindow::ContextMenu( CWidget * _pwControl, INT _iX, INT _iY ) {
		bool iKeyBoard = -1 == _iX && -1 == _iY;
		switch ( _pwControl->Id() ) {
			case Layout::MX_MWI_MAINWINDOW : { if ( !iKeyBoard ) { break; } }	MX_FALLTHROUGH
			case Layout::MX_MWI_SPLITTER : {
				
				POINT pPoint = { _iX, _iY };
				::ScreenToClient( _pwControl->Wnd(), &pPoint );

				// Hit-test to get the child window under the cursor.
				HWND hChild = ::ChildWindowFromPointEx( _pwControl->Wnd(), pPoint, CWP_SKIPINVISIBLE | CWP_SKIPDISABLED );
				auto pwTree = MainTreeView();
				
				if ( pwTree && (hChild == pwTree->Wnd() || iKeyBoard/*(iKeyBoard && _pwControl->Id() == Layout::MX_MWI_MAINWINDOW))*/) ) {
					int iCaret = -1;
					if ( iKeyBoard ) {
						int iFocused = pwTree->GetCaretIndex();
						if ( iFocused >= 0 ) {
							iCaret = iFocused;
							LSW_RECT rRect{};
							pwTree->GetSubItemRect( iFocused, 0, LVIR_LABEL, &rRect );
							POINT pThis{ rRect.left, rRect.bottom };
							::ClientToScreen( _pwControl->Wnd(), &pThis );
							_iX = static_cast<INT>(pThis.x);
							_iY = static_cast<INT>(pThis.y);
						}
						else {
							auto rRect = pwTree->ClientRect();					// WindowRect() doesn’t work for this.
							rRect = rRect.ClientToScreen( pwTree->Wnd() );
							_iX = static_cast<INT>(rRect.left + (rRect.Width() >> 1));
							_iY = static_cast<INT>(rRect.top + (rRect.Height() >> 1));
						}
					}
					else {
						LVHITTESTINFO htiInfo{};
						POINT pTmp = { _iX, _iY };
						::ScreenToClient( pwTree->Wnd(), &pTmp );
						htiInfo.pt = pTmp;
						const int iHit = pwTree->SubItemHitTest( &htiInfo );
						if ( iHit >= 0 && (htiInfo.flags & (LVHT_ONITEMICON | LVHT_ONITEMLABEL | LVHT_ONITEMSTATEICON)) ) {
							//pwTree->SetItemSelection( iHit, TRUE );
							iCaret = htiInfo.iItem;
							//m_iCtxSub  = htiInfo.iSubItem;
						}
					}
					m_iContextHitTest = iCaret;


					size_t sSelected;
					size_t sTotal = 0;
					
					{
						auto famMan = m_pmhMemHack->FoundAddressManager();						// Locks the Found Address Manager for modifications.
						sTotal = famMan->Total();
						std::vector<LPARAM> vSelected;
						sSelected = pwTree->GatherSelectedLParam( vSelected, true );
					}
					LSW_MENU_ITEM miMenuBar[] = {
						//bIsSeperator	dwId											bCheckable	bChecked	bEnabled			lpwcText, stTextLen																											bSkip
						{ FALSE,		Layout::MX_MWMI_ADDENTRY,						FALSE,		FALSE,		TRUE,				MW_MENU_TXT( _T_68168A4D__Add_Entry, _LEN_68168A4D ),																		FALSE },	// &A
						{ FALSE,		Layout::MX_MWMI_EDIT,							FALSE,		FALSE,		sSelected != 0,		MW_MENU_TXT( _T_3DDC3223__Edit_Selected, _LEN_3DDC3223 ),																	FALSE },	// &E
						{ FALSE,		Layout::MX_MWMI_LOCK,							FALSE,		FALSE,		sSelected != 0,		MW_MENU_TXT( _T_FFD11E24__Lock_Selected, _LEN_FFD11E24 ),																	FALSE },	// &L
						{ FALSE,		Layout::MX_MWMI_UNLOCK,							FALSE,		FALSE,		sSelected != 0,		MW_MENU_TXT( _T_FAFFC6A4__Unlock_Selected, _LEN_FAFFC6A4 ),																	FALSE },	// &U
						{ FALSE,		Layout::MX_MWMI_UNLOCK_ALL,						FALSE,		FALSE,		sTotal != 0,		MW_MENU_TXT( _T_C7B383F0_Unloc_k_All, _LEN_C7B383F0 ),																		FALSE },	// &K
						{ TRUE,			0,												FALSE,		FALSE,		FALSE,				nullptr,  0,																												FALSE },
						{ FALSE,		Layout::MX_MWMI_DELETE,							FALSE,		FALSE,		sSelected != 0,		MW_MENU_TXT( _T_3B6DB074__Delete_Selected, _LEN_3B6DB074 ),																	FALSE },	// &D
						{ FALSE,		Layout::MX_MWMI_DELETEALL,						FALSE,		FALSE,		sTotal != 0,		MW_MENU_TXT( _T_29AE3287_Dele_te_All, _LEN_29AE3287 ),																		FALSE },	// &T
						{ TRUE,			0,												FALSE,		FALSE,		FALSE,				nullptr,  0,																												FALSE },
						{ FALSE,		Layout::MX_MWMI_MOVE_UP,						FALSE,		FALSE,		sSelected != 0,		MW_MENU_TXT( _T_7F7393C5_Move_U_p, _LEN_7F7393C5 ),																			FALSE },	// &P
						{ FALSE,		Layout::MX_MWMI_MOVE_DOWN,						FALSE,		FALSE,		sSelected != 0,		MW_MENU_TXT( _T_4955FC02_Move_Dow_n, _LEN_4955FC02 ),																		FALSE },	// &N
						{ TRUE,			0,												FALSE,		FALSE,		FALSE,				nullptr,  0,																												FALSE },
						{ FALSE,		Layout::MX_MWMI_SET_MOVE_ADDRESS_SOURCE,		FALSE,		FALSE,		iCaret != -1,		MW_MENU_TXT( _T_EA7D6530_Set____Move_Selected_Addresses__________Where_the_Address__Was____Address, _LEN_EA7D6530 ),		FALSE },	// &W
						{ FALSE,		Layout::MX_MWMI_COPY_ADDRESS,					FALSE,		FALSE,		sSelected != 0,		MW_MENU_TXT( _T_B1221868__Copy_Selected_Addresses, _LEN_B1221868 ),															FALSE },	// &C
						{ FALSE,		Layout::MX_MWMI_MOVE_ADDRESS,					FALSE,		FALSE,		sSelected != 0,		MW_MENU_TXT( _T_40D39513_Mo_ve_Selected_Addresses, _LEN_40D39513 ),															FALSE },	// &V
					};

					const LSW_MENU_LAYOUT miMenus[] = {
						{
							MX_M_CONTEXT_MENU,
							0,
							0,
							MX_ELEMENTS( miMenuBar ),
							miMenuBar
						},
					};
					mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
					plmLayout->CreatePopupMenuEx( this, miMenus, MX_ELEMENTS( miMenus ), _iX, _iY );
				}
				break;
			}
		}
		return CWidget::LSW_H_CONTINUE;
	}

	// Performs a Save As operation.
	void CMhsMainWindow::SaveAs() {
		if ( m_pmhMemHack ) {
			auto ptvlTree = MainTreeView();
			if ( ptvlTree ) {
				try {
					OPENFILENAMEW ofnOpenFile = { sizeof( ofnOpenFile ) };
					CSecureWString szFileName;
					szFileName.resize( 0xFFFF + 2 );

					CSecureWString wsFilter = _DEC_WS_CC54D980_Save_Files____json____mhssave__0__json___mhssave_0_0;
					ofnOpenFile.hwndOwner = Wnd();
					ofnOpenFile.lpstrFilter = wsFilter.c_str();
					ofnOpenFile.lpstrFile = szFileName.data();
					ofnOpenFile.nMaxFile = DWORD( szFileName.size() );
					ofnOpenFile.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
					ofnOpenFile.lpstrInitialDir = m_pmhMemHack->Options().wsLastFoundAddressSaveDirectory.c_str();

					if ( ::GetSaveFileNameW( &ofnOpenFile ) ) {
						auto oOptions = m_pmhMemHack->Options();
						oOptions.wsLastFoundAddressSaveDirectory = std::filesystem::path( ofnOpenFile.lpstrFile ).remove_filename().generic_wstring();
						
						
						auto pPath = std::filesystem::path( ofnOpenFile.lpstrFile );
						if ( !pPath.has_extension() ) {
							pPath += ".json";
						}
						std::vector<LPARAM> vAll;
						ptvlTree->GatherAllLParam( vAll, true );
						if ( m_pmhMemHack->FoundAddressManager()->SaveSettings( pPath, ::_wcsicmp( pPath.extension().c_str(), L".json" ) == 0, vAll ) ) {
							oOptions.wsLastFoundAddressSaveFile = pPath.generic_wstring();
						}
						m_pmhMemHack->SetOptions( oOptions );
					}
				}
				catch ( ... ) {}
			}
		}
	}

	// Performs a Save Selected As operation.
	void CMhsMainWindow::SaveSelectedAs() {
		if ( m_pmhMemHack ) {
			auto ptvlTree = MainTreeView();
			if ( ptvlTree ) {
				try {
					OPENFILENAMEW ofnOpenFile = { sizeof( ofnOpenFile ) };
					CSecureWString szFileName;
					szFileName.resize( 0xFFFF + 2 );

					CSecureWString wsFilter = _DEC_WS_CC54D980_Save_Files____json____mhssave__0__json___mhssave_0_0;
					ofnOpenFile.hwndOwner = Wnd();
					ofnOpenFile.lpstrFilter = wsFilter.c_str();
					ofnOpenFile.lpstrFile = szFileName.data();
					ofnOpenFile.nMaxFile = DWORD( szFileName.size() );
					ofnOpenFile.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
					ofnOpenFile.lpstrInitialDir = m_pmhMemHack->Options().wsLastFoundAddressSaveDirectory.c_str();

					if ( ::GetSaveFileNameW( &ofnOpenFile ) ) {
						auto oOptions = m_pmhMemHack->Options();
						oOptions.wsLastFoundAddressSaveDirectory = std::filesystem::path( ofnOpenFile.lpstrFile ).remove_filename().generic_wstring();
						
						
						auto pPath = std::filesystem::path( ofnOpenFile.lpstrFile );
						if ( !pPath.has_extension() ) {
							pPath += ".json";
						}
						std::vector<LPARAM> vAll;
						ptvlTree->GatherSelectedLParam( vAll, true );
						if ( m_pmhMemHack->FoundAddressManager()->SaveSettings( pPath, ::_wcsicmp( pPath.extension().c_str(), L".json" ) == 0, vAll ) ) {
							oOptions.wsLastFoundAddressSaveFile = pPath.generic_wstring();
						}
						m_pmhMemHack->SetOptions( oOptions );
					}
				}
				catch ( ... ) {}
			}
		}
	}

	// Performs a Save operation.
	void CMhsMainWindow::Save() {
		if ( m_pmhMemHack ) {
			if ( !m_pmhMemHack->Options().wsLastFoundAddressSaveFile.size() ) {
				SaveAs();
			}
			else {
				auto ptvlTree = MainTreeView();
				if ( ptvlTree ) {
					std::vector<LPARAM> vAll;
					ptvlTree->GatherAllLParam( vAll, true );
					if ( !m_pmhMemHack->FoundAddressManager()->SaveSettings( m_pmhMemHack->Options().wsLastFoundAddressSaveFile, ::_wcsicmp( std::filesystem::path( m_pmhMemHack->Options().wsLastFoundAddressSaveFile.c_str() ).extension().c_str(), L".json" ) == 0, vAll ) ) {
						SaveAs();
					}
				}
			}
		}
	}

	// Performs an Open operation.
	void CMhsMainWindow::Open() {
		if ( m_pmhMemHack ) {
			auto ptvlTree = MainTreeView();
			if ( ptvlTree ) {
				try {
					OPENFILENAMEW ofnOpenFile = { sizeof( ofnOpenFile ) };
					CSecureWString szFileName;
					szFileName.resize( 0xFFFF + 2 );

					CSecureWString wsFilter = _DEC_WS_CC54D980_Save_Files____json____mhssave__0__json___mhssave_0_0;
					ofnOpenFile.hwndOwner = Wnd();
					ofnOpenFile.lpstrFilter = wsFilter.c_str();
					ofnOpenFile.lpstrFile = szFileName.data();
					ofnOpenFile.nMaxFile = DWORD( szFileName.size() );
					ofnOpenFile.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
					ofnOpenFile.lpstrInitialDir = m_pmhMemHack->Options().wsLastFoundAddressSaveDirectory.c_str();

					if ( ::GetOpenFileNameW( &ofnOpenFile ) ) {
						auto oOptions = m_pmhMemHack->Options();
						oOptions.wsLastFoundAddressSaveDirectory = std::filesystem::path( ofnOpenFile.lpstrFile ).remove_filename().generic_wstring();
						
						
						auto pPath = std::filesystem::path( ofnOpenFile.lpstrFile );
						std::vector<CFoundAddressBase *> vAdded;
						auto famMan = m_pmhMemHack->FoundAddressManager();
						if ( !famMan->LoadSettings( pPath, ::_wcsicmp( pPath.extension().c_str(), L".json" ) == 0, MemHack(), vAdded ) ) {
							// Show error.
						}
						else {
							// Add them to the listview.
							for ( size_t I = 0; I < vAdded.size(); ++I ) {
								if ( !CUtilities::AddFoundAddressToTreeListView( ptvlTree, vAdded[I], nullptr ) ) {
									// If it couldn't be added to the tree then it can't be addressed.
									famMan->Delete( vAdded[I]->Id() );
								}
							}
						}
						m_pmhMemHack->SetOptions( oOptions );
					}
				}
				catch ( ... ) {}
			}
		}
	}

	// Add Entry.
	void CMhsMainWindow::AddEntry() {
		if ( m_pmhMemHack ) {
			CAddFoundAddressWindowLayout::CreateAddEntryDialog( this, m_pmhMemHack );
		}
	}

	/**
	 * Called when a CTreeListView wants text for an item.  Can be used to produce real-time or dynamically changing text for items in the tree.
	 * 
	 * \param _pwSrc A pointer to the widget calling the function.
	 * \param _iItem Index of the item whose text is being drawn.
	 * \param _iSubItem Index of the column for which to retreive text.
	 * \param _lpParam The parameter associated with the item.
	 * \param _wsOptionalBuffer An optional buffer for storing text to make it easier to return a persistent std::wstring pointer.  Not necessary if you already have an std::wstring ready to go.
	 * \return Return a pointer to a wide-string result containing the text to display for the given item.  If it is convenient, _wsOptionalBuffer can be used to store the text and &_wsOptionalBuffer returned, otherwise you can return a pointer to an existing std::wstring.
	 *	Return nullptr to use the item's text set by SetItemText().
	 **/
	std::wstring * CMhsMainWindow::TreeListView_ItemText( CWidget * _pwSrc, int _iItem, int _iSubItem, LPARAM _lpParam, std::wstring &_wsOptionalBuffer ) {
		try {
			auto pwTree = MainTreeView();
			if MX_LIKELY( pwTree ) {
				if MX_LIKELY( _pwSrc->Id() == pwTree->Id() ) {
					if ( _iSubItem == 0 ) {						// Description.
						auto famManager = MemHack()->FoundAddressManager();
						auto pfabItem = famManager->GetById( static_cast<size_t>(_lpParam) );
						if MX_LIKELY( pfabItem ) {
							//_wsOptionalBuffer = reinterpret_cast<const wchar_t *>(pfabItem->Locked() ? u"\uD83D\uDD12 " : u"\uD83D\uDD13 ");
							/*_wsOptionalBuffer = wchar_t( 0xD83D );
							_wsOptionalBuffer += pfabItem->Locked() ? wchar_t( 0xDD12 ) : wchar_t( 0xDD13 );*/
							_wsOptionalBuffer = pfabItem->Locked() ? wchar_t( 0x25A0 ) : wchar_t( 0x25CB );
							_wsOptionalBuffer += L' ';
							_wsOptionalBuffer += pfabItem->DescriptionText();
							return &_wsOptionalBuffer;
						}
					}
					else if ( _iSubItem == 1 ) {				// Address.
						//CFoundAddressManager::MX_LOCK lLock( &MemHack()->FoundAddressManager() );
						auto famManager = MemHack()->FoundAddressManager();
						auto pfabItem = famManager->GetById( static_cast<size_t>(_lpParam) );
						if MX_LIKELY( pfabItem /*&& !pfabItem->SimpleAddress()*/ ) {
							pfabItem->Dirty();
							_wsOptionalBuffer = pfabItem->AddressText();
							return &_wsOptionalBuffer;
						}
					}
					else if ( _iSubItem == 2 ) {				// Current value.
						auto famManager = MemHack()->FoundAddressManager();
						auto pfabItem = famManager->GetById( static_cast<size_t>(_lpParam) );
						if MX_LIKELY( pfabItem ) {
							pfabItem->Dirty();
							_wsOptionalBuffer = pfabItem->ValueText();
							return &_wsOptionalBuffer;
						}
					}
					else if ( _iSubItem == 3 ) {				// Lock value.
						auto famManager = MemHack()->FoundAddressManager();
						auto pfabItem = famManager->GetById( static_cast<size_t>(_lpParam) );
						if MX_LIKELY( pfabItem ) {
							_wsOptionalBuffer = pfabItem->ValueWhenLockedText();
							return &_wsOptionalBuffer;
						}
					}
					else if ( _iSubItem == 4 ) {				// Type.
						auto famManager = MemHack()->FoundAddressManager();
						auto pfabItem = famManager->GetById( static_cast<size_t>(_lpParam) );
						if MX_LIKELY( pfabItem ) {
							pfabItem->Dirty();
							_wsOptionalBuffer = pfabItem->TypeText();
							return &_wsOptionalBuffer;
						}
					}
				}
			}
		}
		catch ( ... ) {}
		return nullptr;
	}

	/**
	 * Hotkey handler for opening the options window.
	 *
	 * \param _uiParm0 CMhsMainWindow * stored as a uint64_t.
	 * \param _uiParm1 Unused.
	 * \param _uiParm2 Unused.
	 * \param _uiParm3 Unused.
	 */
	void __stdcall CMhsMainWindow::Hotkey_OpenOptions( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ ) {
		CMhsMainWindow * pmhThis = reinterpret_cast<CMhsMainWindow *>(_uiParm0);
		pmhThis->ShowOptions( pmhThis->LastOptionsShown() );
	}

	/**
	 * Hotkey handler for showing the Found Addresses window.
	 *
	 * \param _uiParm0 CMhsMainWindow * stored as a uint64_t.
	 * \param _uiParm1 Unused.
	 * \param _uiParm2 Unused.
	 * \param _uiParm3 Unused.
	 */
	void __stdcall CMhsMainWindow::Hotkey_ShowFoundAddresses( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ ) {
		CMhsMainWindow * pmhThis = reinterpret_cast<CMhsMainWindow *>(_uiParm0);
		pmhThis->ShowFoundAddress();
	}

	/**
	 * Hotkey handler for showing the Expression Evaluator window.
	 *
	 * \param _uiParm0 CMhsMainWindow * stored as a uint64_t.
	 * \param _uiParm1 Unused.
	 * \param _uiParm2 Unused.
	 * \param _uiParm3 Unused.
	 */
	void __stdcall CMhsMainWindow::Hotkey_ShowExpressionEvaluator( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ ) {
		CMhsMainWindow * pmhThis = reinterpret_cast<CMhsMainWindow *>(_uiParm0);
		pmhThis->ShowExpEval();
	}

	/**
	 * Hotkey handler for showing the Converter window.
	 *
	 * \param _uiParm0 CMhsMainWindow * stored as a uint64_t.
	 * \param _uiParm1 Unused.
	 * \param _uiParm2 Unused.
	 * \param _uiParm3 Unused.
	 */
	void __stdcall CMhsMainWindow::Hotkey_ShowConverter( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ ) {
		CMhsMainWindow * pmhThis = reinterpret_cast<CMhsMainWindow *>(_uiParm0);
		pmhThis->ShowConverter();
	}

	/**
	 * Hotkey handler for showing the PE Works window.
	 *
	 * \param _uiParm0 CMhsMainWindow * stored as a uint64_t.
	 * \param _uiParm1 Unused.
	 * \param _uiParm2 Unused.
	 * \param _uiParm3 Unused.
	 */
	void __stdcall CMhsMainWindow::Hotkey_ShowPeWorks( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ ) {
		CMhsMainWindow * pmhThis = reinterpret_cast<CMhsMainWindow *>(_uiParm0);
		pmhThis->ShowPeWorks();
	}

	/**
	 * Hotkey handler for showing the String Theory window.
	 *
	 * \param _uiParm0 CMhsMainWindow * stored as a uint64_t.
	 * \param _uiParm1 Unused.
	 * \param _uiParm2 Unused.
	 * \param _uiParm3 Unused.
	 */
	void __stdcall CMhsMainWindow::Hotkey_ShowStringTheory( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ ) {
		CMhsMainWindow * pmhThis = reinterpret_cast<CMhsMainWindow *>(_uiParm0);
		pmhThis->ShowStringTheory();
	}

	/**
	 * Hotkey handler for showing the Floating-Point Studio window.
	 *
	 * \param _uiParm0 CMhsMainWindow * stored as a uint64_t.
	 * \param _uiParm1 Unused.
	 * \param _uiParm2 Unused.
	 * \param _uiParm3 Unused.
	 */
	void __stdcall CMhsMainWindow::Hotkey_ShowFloatingPointStudio( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ ) {
		CMhsMainWindow * pmhThis = reinterpret_cast<CMhsMainWindow *>(_uiParm0);
		pmhThis->ShowFloatingPointStudio( std::nan( "0" ) );
	}

	/**
	 * Hotkey handler for showing the Add Entry dialog.
	 *
	 * \param _uiParm0 CMhsMainWindow * stored as a uint64_t.
	 * \param _uiParm1 Unused.
	 * \param _uiParm2 Unused.
	 * \param _uiParm3 Unused.
	 */
	void __stdcall CMhsMainWindow::Hotkey_ShowAddFoundAddress( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ ) {
		CMhsMainWindow * pmhThis = reinterpret_cast<CMhsMainWindow *>(_uiParm0);
		pmhThis->AddEntry();
	}

	/**
	 * Hotkey handler for showing the Modify Entry dialog.
	 *
	 * \param _uiParm0 CMhsMainWindow * stored as a uint64_t.
	 * \param _uiParm1 Unused.
	 * \param _uiParm2 Unused.
	 * \param _uiParm3 Unused.
	 */
	void __stdcall CMhsMainWindow::Hotkey_ShowEdit( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ ) {
		CMhsMainWindow * pmhThis = reinterpret_cast<CMhsMainWindow *>(_uiParm0);
		pmhThis->ShowEdit();
	}

	/**
	 * Hotkey handler for locking selected entries.
	 *
	 * \param _uiParm0 CMhsMainWindow * stored as a uint64_t.
	 * \param _uiParm1 Unused.
	 * \param _uiParm2 Unused.
	 * \param _uiParm3 Unused.
	 */
	void __stdcall CMhsMainWindow::Hotkey_LockSelected( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ ) {
		CMhsMainWindow * pmhThis = reinterpret_cast<CMhsMainWindow *>(_uiParm0);
		pmhThis->LockSelected();
	}

	/**
	 * Hotkey handler for unlocking selected entries.
	 *
	 * \param _uiParm0 CMhsMainWindow * stored as a uint64_t.
	 * \param _uiParm1 Unused.
	 * \param _uiParm2 Unused.
	 * \param _uiParm3 Unused.
	 */
	void __stdcall CMhsMainWindow::Hotkey_UnLockSelected( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ ) {
		CMhsMainWindow * pmhThis = reinterpret_cast<CMhsMainWindow *>(_uiParm0);
		pmhThis->UnlockSelected();
	}

	/**
	 * Hotkey handler for unlocking all entries.
	 *
	 * \param _uiParm0 CMhsMainWindow * stored as a uint64_t.
	 * \param _uiParm1 Unused.
	 * \param _uiParm2 Unused.
	 * \param _uiParm3 Unused.
	 */
	void __stdcall CMhsMainWindow::Hotkey_UnLockAll( uint64_t _uiParm0, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ ) {
		CMhsMainWindow * pmhThis = reinterpret_cast<CMhsMainWindow *>(_uiParm0);
		pmhThis->UnlockAll();
	}

}	// namespace mx
