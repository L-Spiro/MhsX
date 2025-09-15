#include "MXDeusHexMachinaWindow.h"
#include "../Utilities/MXUtilities.h"

#include <Base/LSWWndClassEx.h>
#include <Rebar/LSWRebar.h>
#include <ToolBar/LSWToolBar.h>
#include <TreeListView/LSWTreeListView.h>


namespace mx {

	// == Members.
	// The main window class.
	ATOM CDeusHexMachinaWindow::m_aAtom = 0;

	CDeusHexMachinaWindow::CDeusHexMachinaWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout.ChangeClass( reinterpret_cast<LPCWSTR>(m_aAtom) ), _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
		static const struct {
			LPCWSTR				lpwsImageName;
			DWORD				dwConst;
		} sImages[] = {
			{ L"fn",			MX_I_NEW },
			{ L"fo",			MX_I_OPENFILE },
			{ L"fs",			MX_I_SAVE },
			{ L"fsa",			MX_I_SAVEAS },
			{ L"sa",			MX_I_SAVEALL },
			{ L"fc",			MX_I_CLOSE },

			{ L"cut",			MX_I_CUT },
			{ L"copy",			MX_I_COPY },
			{ L"paste",			MX_I_PASTE },
			{ L"undo",			MX_I_UNDO },
			{ L"redo",			MX_I_REDO },
			
			{ L"find",			MX_I_FIND },
			{ L"goto",			MX_I_GOTO },
			
			{ L"calc",			MX_I_EXP },
			{ L"comp",			MX_I_COMPARE },
			{ L"his",			MX_I_HISTOGRAM },
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
	CDeusHexMachinaWindow::~CDeusHexMachinaWindow() {
	}

	// == Functions.
	// Gets the base tab control.
	CTab * CDeusHexMachinaWindow::GetTab() const {
		return const_cast<CTab *>(static_cast<const CTab *>(FindChild( Layout::MX_W_TABS )));
	}

	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CDeusHexMachinaWindow::InitDialog() {
		CToolBar * plvToolBar = static_cast<CToolBar *>(FindChild( Layout::MX_W_TOOLBAR0 ));
		CRebar * plvRebar = static_cast<CRebar *>(FindChild( Layout::MX_W_REBAR0 ));

		plvToolBar->SetImageList( 0, m_iImages );
		//#define MX_TOOL_STR( TXT )					reinterpret_cast<INT_PTR>(TXT)
#define MX_TOOL_STR( TXT )						0
		const TBBUTTON bButtons[] = {
			// iBitmap							idCommand									fsState				fsStyle			bReserved	dwData	iString
			{ m_iImageMap[MX_I_NEW],			Layout::MX_M_FILE_NEW,						TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"New" ) },
			{ m_iImageMap[MX_I_OPENFILE],		Layout::MX_M_FILE_OPENFILE,					TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Open" ) },
			{ m_iImageMap[MX_I_SAVE],			Layout::MX_M_FILE_SAVE,						TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Save" ) },
			{ m_iImageMap[MX_I_SAVEAS],			Layout::MX_M_FILE_SAVE_AS,					TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Save As" ) },
			{ m_iImageMap[MX_I_SAVEALL],		Layout::MX_M_FILE_SAVE_ALL,					TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Save All" ) },
			{ m_iImageMap[MX_I_CLOSE],			Layout::MX_M_FILE_CLOSE,					TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Close" ) },
			{ -1,								0,											TBSTATE_ENABLED,	BTNS_SEP,		{ 0 },		0,		0 },
			{ m_iImageMap[MX_I_CUT],			Layout::MX_M_EDIT_CUT,						TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Cut" ) },
			{ m_iImageMap[MX_I_COPY],			Layout::MX_M_EDIT_COPY,						TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Copy" ) },
			{ m_iImageMap[MX_I_PASTE],			Layout::MX_M_EDIT_PASTE,					TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Paste" ) },
			{ m_iImageMap[MX_I_UNDO],			Layout::MX_M_EDIT_UNDO,						TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Undo" ) },
			{ m_iImageMap[MX_I_REDO],			Layout::MX_M_EDIT_REDO,						TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Redo" ) },
			{ -1,								0,											TBSTATE_ENABLED,	BTNS_SEP,		{ 0 },		0,		0 },
			{ m_iImageMap[MX_I_FIND],			Layout::MX_M_FIND_FIND,						TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Find" ) },
			{ m_iImageMap[MX_I_GOTO],			Layout::MX_M_FIND_GOTO,						TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Goto" ) },
			{ -1,								0,											TBSTATE_ENABLED,	BTNS_SEP,		{ 0 },		0,		0 },
			{ m_iImageMap[MX_I_EXP],			Layout::MX_M_TOOLS_EXPRESSION_EVALUATOR,	TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Expression Evaluator" ) },
			{ m_iImageMap[MX_I_COMPARE],		Layout::MX_M_TOOLS_COMPARE_FILES,			TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Compare Files" ) },
			{ m_iImageMap[MX_I_HISTOGRAM],		Layout::MX_M_TOOLS_HISTOGRAM,				TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Histogram/Entropy" ) },
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
			riRebarInfo.SetId( Layout::MX_W_TOOLBAR0 );
			plvRebar->InsertBand( -1, riRebarInfo );
		}

	   LSW_RECT rRebarRect = ClientRect( this );
	   ::MoveWindow( plvRebar->Wnd(), 0, 0, rRebarRect.Width(), plvRebar->WindowRect( this ).Height(), FALSE );

		plvRebar->UpdateRects();



		// ==== STATUS BAR ==== //
		CStatusBar * psbStatus = StatusBar();
		if ( psbStatus ) {
			const CStatusBar::LSW_STATUS_PART spParts[] = {
				{ rRebarRect.Width() - psbStatus->ClientRect( this ).Height() - 523, TRUE },
				{ rRebarRect.Width() - psbStatus->ClientRect( this ).Height() - 192, TRUE },
				{ rRebarRect.Width() - psbStatus->ClientRect( this ).Height() - 168, TRUE },
				{ rRebarRect.Width() - psbStatus->ClientRect( this ).Height() - 132, TRUE },
				{ rRebarRect.Width() - psbStatus->ClientRect( this ).Height() - 96, TRUE },
				{ rRebarRect.Width() - psbStatus->ClientRect( this ).Height() - 48, TRUE },

				{ rRebarRect.Width() - psbStatus->ClientRect( this ).Height(), TRUE },
			};
			psbStatus->SetParts( spParts, MX_ELEMENTS( spParts ) );
		}


		// ==== SPLITTER ==== //
		CMultiSplitter * pwSplitter = MultiSplitter();
		CMultiSplitter::LSW_DT_ATTACH aAttach;
		aAttach.atAttachTo = CMultiSplitter::LSW_AT_RIGHT;
		aAttach.dwId = pwSplitter->RootId();
		aAttach.pwWidget = Tab();
		pwSplitter->Attach( aAttach );

		ForceSizeUpdate();
		return LSW_H_CONTINUE;
	}

	// Prepares to create the window.  Creates the atom if necessary.
	void CDeusHexMachinaWindow::PrepareWindow() {
		if ( !m_aAtom ) {
			lsw::CWndClassEx wceEx;
			wceEx.SetInstance( lsw::CBase::GetThisHandle() );
			WCHAR szStr[23];
			mx::CUtilities::RandomString( szStr, MX_ELEMENTS( szStr ) );
			wceEx.SetClassName( szStr );
			wceEx.SetBackgroundBrush( reinterpret_cast<HBRUSH>(CTLCOLOR_DLG + 1) );
			wceEx.SetWindPro( CWidget::WindowProc );
			m_aAtom = lsw::CBase::RegisterClassExW( wceEx.Obj() );
		}
	}

	// WM_CLOSE.
	CWidget::LSW_HANDLED CDeusHexMachinaWindow::Close() {
		
		//::EndDialog( Wnd(), 0 );
		::DestroyWindow( Wnd() );
		//CUtilities::PrintTotalGuiObjects( GR_GDIOBJECTS );
		return LSW_H_HANDLED;
	}

	// WM_ERASEBKGND.
	CWidget::LSW_HANDLED CDeusHexMachinaWindow::EraseBkgnd( HDC _hDc ) {
		return LSW_H_HANDLED;
	}

	// Gets the status bar.
	CStatusBar * CDeusHexMachinaWindow::StatusBar() {
		return static_cast<CStatusBar *>(FindChild( Layout::MX_W_STATUSBAR ));
	}

	// Gets the status bar.
	const CStatusBar * CDeusHexMachinaWindow::StatusBar() const {
		return static_cast<const CStatusBar *>(FindChild( Layout::MX_W_STATUSBAR ));
	}

	// Virtual client rectangle.  Can be used for things that need to be adjusted based on whether or not status bars, toolbars, etc. are present.
	const LSW_RECT CDeusHexMachinaWindow::VirtualClientRect( const CWidget * pwChild ) const {
		LSW_RECT rTemp = ClientRect( this );
		const CRebar * plvRebar = static_cast<const CRebar *>(FindChild( Layout::MX_W_REBAR0 ));
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

}	// namespace mx
