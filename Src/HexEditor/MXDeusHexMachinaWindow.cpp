#include "MXDeusHexMachinaWindow.h"
#include "../Layouts/MXLayoutManager.h"
#include "../MemHack/MXWindowMemHack.h"
#include "../Utilities/MXUtilities.h"
#include "MXHexEditorFile.h"

#include <Base/LSWWndClassEx.h>
#include <Rebar/LSWRebar.h>
#include <ToolBar/LSWToolBar.h>
#include <TreeListView/LSWTreeListView.h>


namespace mx {

	// == Members.
	// The main window class.
	ATOM CDeusHexMachinaWindow::m_aAtom = 0;

	CDeusHexMachinaWindow::CDeusHexMachinaWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout.ChangeClass( reinterpret_cast<LPCWSTR>(m_aAtom) ), _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pmhMemHack( reinterpret_cast<CWindowMemHack *>(_ui64Data) ) {
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
		for ( auto I = m_vTabs.size(); I--; ) {
			delete m_vTabs[I].pheiInterface;
		}
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

		CHexEditorControl::ChooseDefaultFont( m_fsFixedRowFont, m_wDpiY, Wnd() );
		CHexEditorControl::ComputeFontMetrics( m_fsFixedRowFont, m_wDpiY, Wnd() );
		CHexEditorControl::ChooseDefaultFont( m_fsTextViewFont, m_wDpiY, Wnd() );
		CHexEditorControl::ComputeFontMetrics( m_fsTextViewFont, m_wDpiY, Wnd() );

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
		pwSplitter->ShowCursor( true );

		ForceSizeUpdate();
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CDeusHexMachinaWindow::Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc ) {
		if ( _wId >= Layout::MX_M_SHOW_RECENT_BASE ) {
			_wId -= Layout::MX_M_SHOW_RECENT_BASE;
			if ( _wId < m_pmhMemHack->Options().vProcessHistory.size() ) {
				if ( m_pmhMemHack->Options().vProcessHistory[_wId].size() ) {
					Open( std::filesystem::path( static_cast<const std::wstring &>(m_pmhMemHack->Options().vProcessHistory[_wId]) ) );
				}
			}
			return LSW_H_CONTINUE;
		}
		if ( _wId >= Layout::MX_M_USER_PROGRAMS_BASE && _wId < Layout::MX_M_SHOW_RECENT_BASE ) {
			if ( !m_pmhMemHack ) { return LSW_H_CONTINUE; }
			size_t stIdx = _wId - Layout::MX_M_USER_PROGRAMS_BASE;

			m_pmhMemHack->ExecuteProgramByIdx( stIdx );
			return LSW_H_CONTINUE;
		}

		switch ( _wId ) {
			case Layout::MX_M_FILE_OPENFILE : {
				Open();
				break;
			}
			case Layout::MX_M_FILE_EXIT : {
				Close();
				break;
			}

			case Layout::MX_M_VIEW_FONT_ENLARGE_FONT : {
				EnlargeFont();
				break;
			}
			case Layout::MX_M_VIEW_FONT_SHRINK_FONT : {
				EnsmallFont();
				break;
			}
		}
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from menu.
	CWidget::LSW_HANDLED CDeusHexMachinaWindow::MenuCommand( WORD _wId ) {
		return Command( 0, _wId, NULL );
	}

	// WM_CLOSE.
	CWidget::LSW_HANDLED CDeusHexMachinaWindow::Close() {
		::DestroyWindow( Wnd() );
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

	// Informs tha a child tab closed one of it headers.
	void CDeusHexMachinaWindow::ChildTabClosed( CWidget * _pwChild, INT _iTab ) {
		if ( _pwChild == Tab() ) {
			if ( _iTab < m_vTabs.size() ) {
				delete m_vTabs[_iTab].pheiInterface;
				m_vTabs.erase( m_vTabs.begin() + _iTab );
			}
		}
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

	// Performs a Save As operation.
	void CDeusHexMachinaWindow::SaveAs() {
	}

	// Performs a Save operation.
	void CDeusHexMachinaWindow::Save() {
	}

	// Performs an Open operation.
	void CDeusHexMachinaWindow::Open() {
		if ( m_pmhMemHack ) {
			try {
				OPENFILENAMEW ofnOpenFile = { sizeof( ofnOpenFile ) };
				CSecureWString szFileName;
				szFileName.resize( 0xFFFF + 2 );

				CSecureWString wsFilter = _DEC_WS_6331FDE9_All_Files_______0____0;
				ofnOpenFile.hwndOwner = Wnd();
				ofnOpenFile.lpstrFilter = wsFilter.c_str();
				ofnOpenFile.lpstrFile = szFileName.data();
				ofnOpenFile.nMaxFile = DWORD( szFileName.size() );
				ofnOpenFile.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
				ofnOpenFile.lpstrInitialDir = m_pmhMemHack->Options().heHexEditorOptions.wsLastOpenFileDirectory.c_str();

				if ( ::GetOpenFileNameW( &ofnOpenFile ) ) {
					auto oOptions = m_pmhMemHack->Options();
					oOptions.heHexEditorOptions.wsLastOpenFileDirectory = std::filesystem::path( ofnOpenFile.lpstrFile ).remove_filename().generic_wstring();
						
						
					auto pPath = std::filesystem::path( ofnOpenFile.lpstrFile );
					Open( pPath );
					//std::vector<CFoundAddressBase *> vAdded;
					//auto famMan = m_pmhMemHack->FoundAddressManager();
					//if ( !famMan->LoadSettings( pPath, ::_wcsicmp( pPath.extension().c_str(), L".json" ) == 0, MemHack(), vAdded ) ) {
					//	// Show error.
					//}
					//else {
					//	// Add them to the listview.
					//	for ( size_t I = 0; I < vAdded.size(); ++I ) {
					//		if ( !CUtilities::AddFoundAddressToTreeListView( ptTab, vAdded[I], nullptr ) ) {
					//			// If it couldn't be added to the tree then it can't be addressed.
					//			famMan->Delete( vAdded[I]->Id() );
					//		}
					//	}
					//}
					m_pmhMemHack->SetOptions( oOptions );
				}
			}
			catch ( ... ) {}
		}
	}

	// Performs an Open operation.
	void CDeusHexMachinaWindow::Open( const std::filesystem::path &_pPath ) {
		MX_HEX_TAB htTab;
		try {
			auto ptTab = Tab();
			if ( ptTab ) {
				CHexEditorControl::PrepareControl();
				

				CSecureString sLeftSizeExp;
				mx::CStringDecoder::Decode( _T_2762F803_____P__VCL, _LEN_2762F803, sLeftSizeExp );
				CSecureString sRightSizeExp;
				mx::CStringDecoder::Decode( _T_DD6DC560_____P__VCR, _LEN_DD6DC560, sRightSizeExp );
				CSecureString sTopSizeExp;
				mx::CStringDecoder::Decode( _T_340E6055_____P__VCT, _LEN_340E6055, sTopSizeExp );
				CSecureString sBottomSizeExp;
				mx::CStringDecoder::Decode( _T_C0DAD504_____P__VCB, _LEN_C0DAD504, sBottomSizeExp );
				LSW_WIDGET_LAYOUT wlLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager())->FixLayout( LSW_WIDGET_LAYOUT{
					MX_HEX_CONTROL,									// ltType
					static_cast<WORD>(Layout::MX_W_TAB_START + ptTab->GetItemCount()),	// wId
					nullptr,										// lpwcClass
					TRUE,											// bEnabled
					TRUE,											// bActive
					0,												// iLeft
					0,												// iTop
					0,												// dwWidth
					0,												// dwHeight
					WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP,		// dwStyle
					0,												// dwStyleEx
					nullptr,										// pwcText
					0,												// sTextLen
					static_cast<DWORD>(ptTab->Id()),				// dwParentId

//#if 0
					sLeftSizeExp.c_str(), 0,						// pcLeftSizeExp
					sRightSizeExp.c_str(), 0,						// pcRightSizeExp
					sTopSizeExp.c_str(), 0,							// pcTopSizeExp
					sBottomSizeExp.c_str(), 0,						// pcBottomSizeExp
					nullptr, 0,										// pcWidthSizeExp
					nullptr, 0,										// pcHeightSizeExp
//#endif
				} );
				CHexEditorControl::MX_CREATION_PARMS cpCreation;
				cpCreation.pfsFixedRowFont = &m_fsFixedRowFont;
				cpCreation.pfsDynamicRowFont = &m_fsTextViewFont;
				htTab.phecWidget = static_cast<CHexEditorControl *>(static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager())->CreateWidget( wlLayout, ptTab, TRUE, NULL, reinterpret_cast<uint64_t>(&cpCreation) ));
				if ( !htTab.phecWidget ) {
					//delete tTab.ppoPeObject;
					return;
				}
				htTab.phecWidget->InitControl( htTab.phecWidget->Wnd() );

				htTab.pheiInterface = new( std::nothrow ) CHexEditorFile();
				if ( !htTab.pheiInterface || !static_cast<CHexEditorFile*>(htTab.pheiInterface)->SetFile( _pPath ) ) {
					delete htTab.phecWidget;
					delete htTab.pheiInterface;
					return;
				}

				m_vTabs.insert( m_vTabs.begin(), htTab );

				auto wsTabname = _pPath.filename().generic_wstring();
				TCITEMW tciItem = { 0 };
				tciItem.mask = TCIF_TEXT;
				tciItem.pszText = const_cast<LPWSTR>(wsTabname.c_str());

				

				if ( ptTab->InsertItem( 0, &tciItem, htTab.phecWidget ) == -1 ) {
					delete htTab.phecWidget;
					delete htTab.pheiInterface;
					return;
				}

				htTab.phecWidget->SetStream( htTab.pheiInterface );

				LSW_RECT rInternalSize = ptTab->WindowRect().ScreenToClient( ptTab->Wnd() );
				ptTab->AdjustRect( FALSE, &rInternalSize );
				::MoveWindow( htTab.phecWidget->Wnd(), rInternalSize.left, rInternalSize.top, rInternalSize.Width(), rInternalSize.Height(), TRUE );
			}
		}
		catch ( ... ) {
			delete htTab.phecWidget;
			delete htTab.pheiInterface;
		}
	}

	// Enlarge font.
	void CDeusHexMachinaWindow::EnlargeFont() {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }
		LOGFONTW lfFont;
		phecControl->IncreaseFontSize( lfFont );
	}

	// Ensmall font.
	void CDeusHexMachinaWindow::EnsmallFont() {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }
		LOGFONTW lfFont;
		phecControl->DecreaseFontSize( lfFont );
	}

}	// namespace mx
