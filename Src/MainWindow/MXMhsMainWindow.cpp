#include "MXMhsMainWindow.h"
#include "../Layouts/MXExpressionEvaluatorLayout.h"
#include "../Layouts/MXFoundAddressLayout.h"
#include "../Layouts/MXMainWindowLayout.h"
#include "../Layouts/MXOpenProcessLayout.h"
#include "../Layouts/MXOptionsLayout.h"
#include "../System/MXSystem.h"
#include "../Utilities/MXUtilities.h"
#include <Base/LSWBase.h>
#include <MultiSplitter/LSWMultiSplitter.h>
#include <Rebar/LSWRebar.h>
#include <Splitter/LSWSplitter.h>
#include <StatusBar/LSWStatusBar.h>
#include <ToolBar/LSWToolBar.h>

namespace mx {

	CMhsMainWindow::CMhsMainWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, CMemHack * _pmhMemHack ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu ),
		m_pfaFoundAddresses( nullptr ),
		m_eeExpEval( nullptr ) {

		m_pmhMemHack = _pmhMemHack;

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
			std::wstring wsTemp = L"Resources/";
			wsTemp += sImages[I].lpwsImageName;
			wsTemp += L".bmp";

			m_bBitmaps[sImages[I].dwConst].LoadFromFile( wsTemp.c_str(), 0, 0, LR_CREATEDIBSECTION );
			m_iImageMap[sImages[I].dwConst] = m_iImages.Add( m_bBitmaps[sImages[I].dwConst].Handle() );
		}

	}

	CMhsMainWindow::~CMhsMainWindow() {
		//delete m_pmhMemHack;
		m_pmhMemHack = nullptr;
	}

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CMhsMainWindow::InitDialog() {
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
				{ 450, FALSE },
				// Current process ID.
				{ 450 + 48, FALSE },

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


		{
			LSW_RECT rRect;
			// Send a fake WM_SIZE message to cause the window to recalculate and redraw its layout.
			::GetClientRect( Wnd(), &rRect );
			::SendMessageW( Wnd(), WM_SIZE, SIZE_RESTORED, MAKELPARAM( rRect.Width(), rRect.Height() ) );
		}

		//ShowFoundAddress();
		ShowExpEval();

		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CMhsMainWindow::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {
		switch ( _Id ) {
			case CMainWindowLayout::MX_MWMI_OPENPROCESS : {
				MX_OPTIONS oOptions = m_pmhMemHack->Options();
				DWORD dwId = COpenProcessLayout::CreateOpenProcessDialog( this, &oOptions );
				if ( dwId != static_cast<DWORD>(-1) ) {
					if ( m_pmhMemHack->OpenProcess( dwId ) ) {
						CStatusBar * psbStatus = StatusBar();
						if ( psbStatus ) {
							std::wstring sTemp = _DEC_WS_9FAAF98F_Opened_process_;
							CUtilities::ToHex( dwId, sTemp, 4 );
							sTemp.append( L" (" );
							CUtilities::ToUnsigned( dwId, sTemp );
							sTemp.append( L")." );
							psbStatus->SetTextW( 0, 0, sTemp.c_str() );
						}
						
					}
				}
				break;
			}
			case CMainWindowLayout::MX_MWMI_OPTIONS : {
				MX_OPTIONS oOptions = m_pmhMemHack->Options();
				if ( COptionsLayout::CreateOptionsDialog( this, &oOptions ) ) {
					m_pmhMemHack->SetOptions( oOptions );
				}
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
			case CMainWindowLayout::MX_MWMI_SHOW_ALL : {
				ShowFoundAddress();
				ShowExpEval();
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
		::PostQuitMessage( 0 );
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

	// Shows the Found Address dockable.
	void CMhsMainWindow::ShowFoundAddress() {
		if ( !m_pfaFoundAddresses ) {
			m_pfaFoundAddresses = static_cast<CFoundAddressesWindow *>(CFoundAddressLayout::CreateFoundAddressesWindow( MultiSplitter() ));
			//m_pfaFoundAddresses = static_cast<CFoundAddressesWindow *>(CFoundAddressLayout::CreateFoundAddressesWindow( this ));
			m_pfaFoundAddresses->AddDockTarget( MultiSplitter() );
		}
		else {
			m_pfaFoundAddresses->SetVisible( TRUE );
		}
		UpdateWindowChecks();
	}

	// Shows the Expression Evaluator dockable.
	void CMhsMainWindow::ShowExpEval() {
		if ( !m_eeExpEval ) {
			m_eeExpEval = static_cast<CExpEvalWindow *>(CExpressionEvaluatorLayout::CreateExpEvalWindow( MultiSplitter() ));
			//m_eeExpEval = static_cast<CExpEvalWindow *>(CExpressionEvaluatorLayout::CreateExpEvalWindow( this ));
			m_eeExpEval->AddDockTarget( MultiSplitter() );
		}
		else {
			m_eeExpEval->SetVisible( TRUE );
		}
		UpdateWindowChecks();
	}

	// Informs that a child was removed from a child control (IE this control's child had a child control removed from it).
	// Is also called on the control from which a child was removed for convenience.
	void CMhsMainWindow::ChildWasRemoved( const CWidget * _pwChild ) {
		if ( m_pfaFoundAddresses && static_cast<CWidget *>(m_pfaFoundAddresses) == _pwChild ) {
			m_pfaFoundAddresses = nullptr;
			UpdateWindowChecks();
		}
		if ( m_eeExpEval && static_cast<CWidget *>(m_eeExpEval) == _pwChild ) {
			m_eeExpEval = nullptr;
			UpdateWindowChecks();
		}
		CMainWindow::ChildWasRemoved( _pwChild );
	}

	// Update the "Window" checks.
	void CMhsMainWindow::UpdateWindowChecks() {
		HMENU hMenu = ::GetMenu( Wnd() );
		if ( hMenu ) {
			//BOOL bCheck = m_pfaFoundAddresses && m_pfaFoundAddresses->
			::CheckMenuItem( hMenu, CMainWindowLayout::MX_MWMI_SHOWFOUNDADDR, m_pfaFoundAddresses ? MF_CHECKED : MF_UNCHECKED );
			::CheckMenuItem( hMenu, CMainWindowLayout::MX_MWMI_SHOW_EXPEVAL, m_eeExpEval ? MF_CHECKED : MF_UNCHECKED );
		}
	}

}	// namespace mx
