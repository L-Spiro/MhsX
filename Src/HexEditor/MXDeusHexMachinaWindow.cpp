#include "MXDeusHexMachinaWindow.h"
#include "../Layouts/MXLayoutManager.h"
#include "../Layouts/MXOpenProcessLayout.h"
#include "../MainWindow/MXMhsMainWindow.h"
#include "../MemHack/MXWindowMemHack.h"
#include "../Utilities/MXUtilities.h"
#include "MXHexEditorCurProcess.h"
#include "MXHexEditorFile.h"
#include "MXHexEditorProcess.h"

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


		
		m_hecFgColors.DefaultDarkTheme_Fg();
		m_hecBgColors.DefaultDarkTheme_Bg();

		m_sOptions[CHexEditorControl::MX_EA_TEXT].ftFont = CHexEditorControl::MX_FT_TEXT_VIEW;
		m_sOptions[CHexEditorControl::MX_EA_HEX].ftFont = CHexEditorControl::MX_FT_FIXED_ROW;
		m_sOptions[CHexEditorControl::MX_EA_BINARY].ftFont = CHexEditorControl::MX_FT_FIXED_ROW;
		m_sOptions[CHexEditorControl::MX_EA_SCRIPT].ftFont = CHexEditorControl::MX_FT_TEXT_VIEW;
		m_sOptions[CHexEditorControl::MX_EA_TEMPLATE].ftFont = CHexEditorControl::MX_FT_TEXT_VIEW;
		m_sOptions[CHexEditorControl::MX_EA_EBCDIC].ftFont = CHexEditorControl::MX_FT_TEXT_VIEW;
		m_sOptions[CHexEditorControl::MX_EA_UTF16].ftFont = CHexEditorControl::MX_FT_TEXT_VIEW;
		m_sOptions[CHexEditorControl::MX_EA_UTF8].ftFont = CHexEditorControl::MX_FT_TEXT_VIEW;
		m_sOptions[CHexEditorControl::MX_EA_PROCESS].ftFont = CHexEditorControl::MX_FT_FIXED_ROW;
		m_sOptions[CHexEditorControl::MX_EA_CUR_PROCESS].ftFont = CHexEditorControl::MX_FT_FIXED_ROW;
		m_sOptions[CHexEditorControl::MX_EA_CODE].ftFont = CHexEditorControl::MX_FT_TEXT_VIEW;
		m_sOptions[CHexEditorControl::MX_EA_TAGGED].ftFont = CHexEditorControl::MX_FT_TEXT_VIEW;

		m_sOptions[CHexEditorControl::MX_EA_HEX].dfLeftNumbersFmt = CHexEditorControl::MX_DF_HEX;
		m_sOptions[CHexEditorControl::MX_EA_HEX].dfRightNumbersFmt = CHexEditorControl::MX_DF_CHAR;

		m_sOptions[CHexEditorControl::MX_EA_BINARY].dfLeftNumbersFmt = CHexEditorControl::MX_DF_BIN;
		m_sOptions[CHexEditorControl::MX_EA_BINARY].dfRightNumbersFmt = CHexEditorControl::MX_DF_CHAR;

		m_sOptions[CHexEditorControl::MX_EA_CUR_PROCESS].dfLeftNumbersFmt = CHexEditorControl::MX_DF_HEX;
		m_sOptions[CHexEditorControl::MX_EA_CUR_PROCESS].dfRightNumbersFmt = CHexEditorControl::MX_DF_CHAR;

		m_sOptions[CHexEditorControl::MX_EA_PROCESS].dfLeftNumbersFmt = CHexEditorControl::MX_DF_HEX;
		m_sOptions[CHexEditorControl::MX_EA_PROCESS].dfRightNumbersFmt = CHexEditorControl::MX_DF_CHAR;
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

	/**
	 * Handles WM_COMMAND dispatch.
	 * \brief Routes command notifications from menus, accelerators, or child controls.
	 *
	 * \param _wCtrlCode 0 = from menu, 1 = from accelerator; otherwise a control notification code.
	 * \param _wId The identifier for the command or control.
	 * \param _pwSrc The source control for control notifications; otherwise nullptr.
	 * \return Returns a LSW_HANDLED code.
	 */
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


		auto phecControl = CurrentEditor();

		switch ( _wId ) {
			case Layout::MX_M_FILE_OPENFILE : {
				Open();
				break;
			}
			case Layout::MX_M_FILE_OPENPROCESS : {
				OpenProcess();
				break;
			}
			case Layout::MX_M_FILE_OPENCURPROCESS : {
				OpenCurProcess();
				break;
			}
			case Layout::MX_M_FILE_CLOSE : {
				CloseTab();
				break;
			}
			case Layout::MX_M_FILE_CLOSE_ALL : {
				CloseAllTabs();
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
			case Layout::MX_M_VIEW_FONT_RESET_FONT_SIZE : {
				ResetFontSize();
				break;
			}
			case Layout::MX_M_VIEW_FONT_RESET_FONT : {
				ResetFont();
				break;
			}

			case Layout::MX_M_VIEW_EDIT_AS_TEXT : {
				if ( phecControl ) {
					phecControl->SetViewType( CHexEditorControl::MX_EA_TEXT );
				}
				break;
			}
			case Layout::MX_M_VIEW_EDIT_AS_HEX : {
				if ( phecControl ) {
					phecControl->SetViewType( CHexEditorControl::MX_EA_HEX );
				}
				break;
			}
			case Layout::MX_M_VIEW_EDIT_AS_BINARY : {
				if ( phecControl ) {
					phecControl->SetViewType( CHexEditorControl::MX_EA_BINARY );
				}
				break;
			}
			case Layout::MX_M_VIEW_EDIT_AS_SCRIPT : {
				if ( phecControl ) {
					phecControl->SetViewType( CHexEditorControl::MX_EA_SCRIPT );
				}
				break;
			}
			case Layout::MX_M_VIEW_EDIT_AS_TEMPLATE : {
				if ( phecControl ) {
					phecControl->SetViewType( CHexEditorControl::MX_EA_TEMPLATE );
				}
				break;
			}
			case Layout::MX_M_VIEW_EDIT_AS_EBCDIC : {
				if ( phecControl ) {
					phecControl->SetViewType( CHexEditorControl::MX_EA_EBCDIC );
				}
				break;
			}
			case Layout::MX_M_VIEW_EDIT_AS_UNICODE : {
				if ( phecControl ) {
					phecControl->SetViewType( CHexEditorControl::MX_EA_UTF16 );
				}
				break;
			}
			case Layout::MX_M_VIEW_EDIT_AS_UTF8 : {
				if ( phecControl ) {
					phecControl->SetViewType( CHexEditorControl::MX_EA_UTF8 );
				}
				break;
			}
			case Layout::MX_M_VIEW_EDIT_AS_PROCESS : {
				if ( phecControl ) {
					phecControl->SetViewType( CHexEditorControl::MX_EA_PROCESS );
				}
				break;
			}
			case Layout::MX_M_VIEW_EDIT_AS_CODE : {
				if ( phecControl ) {
					phecControl->SetViewType( CHexEditorControl::MX_EA_CODE );
				}
				break;
			}
			case Layout::MX_M_VIEW_EDIT_AS_TAGGED : {
				if ( phecControl ) {
					phecControl->SetViewType( CHexEditorControl::MX_EA_TAGGED );
				}
				break;
			}
			case Layout::MX_M_VIEW_EDIT_AS_TOGGLE : {
				if ( phecControl ) {
					if ( phecControl->GetViewType() != CHexEditorControl::MX_EA_HEX ) {
						phecControl->SetViewType( CHexEditorControl::MX_EA_HEX );
					}
					else {
						phecControl->SetViewType( CHexEditorControl::MX_EA_TEXT );
					}
				}
				break;
			}

			case Layout::MX_M_VIEW_CHAR_SET_ASCII : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_ASCII );
				}
				break;
			}
			case Layout::MX_M_VIEW_CHAR_SET_ASCII_ANSI : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_ASCII_ANSI );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_CHAR_SET_ASCII_OEM : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_ASCII_OEM );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_CHAR_SET_EBCDIC : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_EBCDIC );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_CHAR_SET_UNICODE : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_UNICODE );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_CHAR_SET_UTF8 : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_UTF8 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_CHAR_SET_MACINTOSH : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_MACINTOSH );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_CHAR_SET_ARABIC_WINDOWS : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_ARABIC_WINDOWS );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_CHAR_SET_ARABIC_ISO : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_ARABIC_ISO );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_CHAR_SET_BALTIC_WINDOWS : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_BALTIC_WINDOWS );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_CHAR_SET_BALTIC_ISO : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_BALTIC_ISO );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_CHAR_SET_CHINESE_SIMP : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_CHINESE_SIMPLIFIED );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_CHAR_SET_CHINESE_TRAD : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_CHINESE_TRADITIONAL );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_CHAR_SET_CYRILLIC_WINDOWS : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_CYRILLIC_WINDOWS );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_CHAR_SET_CYRILLIC_KOI8R : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_CYRILLIC_KOI8_R );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_CHAR_SET_CYRILLIC_KOI8U : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_CYRILLIC_KOI8_U );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_CHAR_SET_CYRILLIC_ISO : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_CYRILLIC_ISO );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_CHAR_SET_EASTERN_EUR_WINDOWS : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_EASTERN_EUROPE_WINDOWS );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_CHAR_SET_EASTERN_EUR_ISO : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_EASTERN_EUROPE_ISO );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_CHAR_SET_GREEK_WINDOWS : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_GREEK_WINDOWS );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_CHAR_SET_GREEK_ISO : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_GREEK_ISO );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_CHAR_SET_HEBREW_WINDOWS : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_HEBREW_WINDOWS );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_CHAR_SET_HEBREW_ISO : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_HEBREW_ISO );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_CHAR_SET_JAPANESE_JIS : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_JAPANESE_JIS );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_CHAR_SET_JAPANESE_EUC : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_JAPANESE_EUC_JP );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_CHAR_SET_KOREAN : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_KOREAN );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_CHAR_SET_THAI : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_THAI );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_CHAR_SET_TURKISH_WINDOWS : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_TURKISH_WINDWOS );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_CHAR_SET_TURKISH_ISO : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_TURKISH_ISO );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_CHAR_SET_VIETNAMESE : {
				if ( phecControl ) {
					phecControl->SetCharacterSet( CCharSets::MX_CS_VIETNAMESE );
					RecalcAllBut( phecControl );
				}
				break;
			}

			case Layout::MX_M_VIEW_ADDRESSES_SHOW_LINE_NUMBERS_ADDRESSES : {
				if ( phecControl ) {
					phecControl->SetShowAddressGutter( !phecControl->GetShowAddressGutter() );
				}
				break;
			}
			case Layout::MX_M_VIEW_ADDRESSES_DISPLAY_FORMATS_BYTE_HEX : {
				if ( phecControl ) {
					phecControl->SetAddressGutterFmt( CHexEditorControl::MX_AF_BYTES_HEX );
				}
				break;
			}
			case Layout::MX_M_VIEW_ADDRESSES_DISPLAY_FORMATS_BYTE_DEC : {
				if ( phecControl ) {
					phecControl->SetAddressGutterFmt( CHexEditorControl::MX_AF_BYTES_DEC );
				}
				break;
			}
			case Layout::MX_M_VIEW_ADDRESSES_DISPLAY_FORMATS_BYTE_OCT : {
				if ( phecControl ) {
					phecControl->SetAddressGutterFmt( CHexEditorControl::MX_AF_BYTES_OCT );
				}
				break;
			}
			case Layout::MX_M_VIEW_ADDRESSES_DISPLAY_FORMATS_LINE_DEC : {
				if ( phecControl ) {
					phecControl->SetAddressGutterFmt( CHexEditorControl::MX_AF_LINE_NUMBER );
				}
				break;
			}
			case Layout::MX_M_VIEW_ADDRESSES_DISPLAY_FORMATS_SHORT_HEX : {
				if ( phecControl ) {
					phecControl->SetAddressGutterFmt( CHexEditorControl::MX_AF_SHORT_HEX );
				}
				break;
			}
			case Layout::MX_M_VIEW_ADDRESSES_DISPLAY_FORMATS_SHORT_DEC : {
				if ( phecControl ) {
					phecControl->SetAddressGutterFmt( CHexEditorControl::MX_AF_SHORT_DEC );
				}
				break;
			}

			case Layout::MX_M_VIEW_LINE_WIDTH_4_BYTES : {
				if ( phecControl ) {
					phecControl->SetLineWidth( 4 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LINE_WIDTH_8_BYTES : {
				if ( phecControl ) {
					phecControl->SetLineWidth( 8 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LINE_WIDTH_12_BYTES : {
				if ( phecControl ) {
					phecControl->SetLineWidth( 12 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LINE_WIDTH_16_BYTES : {
				if ( phecControl ) {
					phecControl->SetLineWidth( 16 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LINE_WIDTH_20_BYTES : {
				if ( phecControl ) {
					phecControl->SetLineWidth( 20 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LINE_WIDTH_40_BYTES : {
				if ( phecControl ) {
					phecControl->SetLineWidth( 40 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LINE_WIDTH_AUTO : {
				if ( phecControl ) {
					phecControl->SetLineWidthdAuto( true );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LINE_WIDTH_CUSTOM : {
				break;
			}

			case Layout::MX_M_VIEW_GROUP_BY_BYTE : {
				if ( phecControl ) {
					phecControl->SetGroupSize( 1 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_GROUP_BY_SHORT : {
				if ( phecControl ) {
					phecControl->SetGroupSize( 2 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_GROUP_BY_INT : {
				if ( phecControl ) {
					phecControl->SetGroupSize( 4 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_GROUP_BY_INT64 : {
				if ( phecControl ) {
					phecControl->SetGroupSize( 8 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_GROUP_BY_CUSTOM : {
				
				break;
			}

			case Layout::MX_M_VIEW_DIVISION_LINES_NONE : {
				if ( phecControl ) {
					phecControl->SetDivionLines( 0 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_DIVISION_LINES_1_BYTE : {
				if ( phecControl ) {
					phecControl->SetDivionLines( 1 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_DIVISION_LINES_2_BYTES : {
				if ( phecControl ) {
					phecControl->SetDivionLines( 2 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_DIVISION_LINES_4_BYTES : {
				if ( phecControl ) {
					phecControl->SetDivionLines( 4 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_DIVISION_LINES_8_BYTES : {
				if ( phecControl ) {
					phecControl->SetDivionLines( 8 );
					RecalcAllBut( phecControl );
				}
				break;
			}

			case Layout::MX_M_VIEW_LEFT_AREA_HEX : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( CHexEditorControl::MX_DF_HEX );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LEFT_AREA_CHAR : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( CHexEditorControl::MX_DF_CHAR );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LEFT_AREA_OCTAL : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( CHexEditorControl::MX_DF_OCT );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LEFT_AREA_BINARY : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( CHexEditorControl::MX_DF_BIN );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LEFT_AREA_DECIMAL : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( CHexEditorControl::MX_DF_DEC );
					RecalcAllBut( phecControl );
				}
				break;
			}

			case Layout::MX_M_VIEW_RIGHT_AREA_HEX : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( CHexEditorControl::MX_DF_HEX );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_CHAR : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( CHexEditorControl::MX_DF_CHAR );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_OCTAL : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( CHexEditorControl::MX_DF_OCT );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_BINARY : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( CHexEditorControl::MX_DF_BIN );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_DECIMAL : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( CHexEditorControl::MX_DF_DEC );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_HIDE : {
				if ( phecControl ) {
					phecControl->HideRightArea();
					RecalcAllBut( phecControl );
				}
				break;
			}

			case Layout::MX_M_VIEW_HIGHLIGHTING_LINEFEED_CHARS : {
				if ( phecControl ) {
					phecControl->SetHighlightingLinefeed( !phecControl->GetHighlightingLinefeed() );
				}
				break;
			}
			case Layout::MX_M_VIEW_HIGHLIGHTING_ALPHA_CHARS : {
				if ( phecControl ) {
					phecControl->SetHighlightingAlphaNumeric( !phecControl->GetHighlightingAlphaNumeric() );
				}
				break;
			}
			case Layout::MX_M_VIEW_HIGHLIGHTING_CONTROL_CHARS : {
				if ( phecControl ) {
					phecControl->SetHighlightingControl( !phecControl->GetHighlightingControl() );
				}
				break;
			}
			case Layout::MX_M_VIEW_HIGHLIGHTING_NON_ASCII : {
				if ( phecControl ) {
					phecControl->SetHighlightingNonAscii( !phecControl->GetHighlightingNonAscii() );
				}
				break;
			}
			case Layout::MX_M_VIEW_HIGHLIGHTING_ZEROES : {
				if ( phecControl ) {
					phecControl->SetHighlightingZeros( !phecControl->GetHighlightingZeros() );
				}
				break;
			}
			case Layout::MX_M_VIEW_HIGHLIGHTING_POINTERS : {
				if ( phecControl ) {
					phecControl->SetHighlightingPointers( !phecControl->GetHighlightingPointers() );
				}
				break;
			}

			case Layout::MX_M_VIEW_RULER_SHOW : {
				if ( phecControl ) {
					phecControl->SetShowRuler( !phecControl->GetShowRuler() );
				}
				break;
			}
			case Layout::MX_M_VIEW_RULER_SHOW_LABELS : {
				if ( phecControl ) {
					phecControl->SetShowRulerLabels( !phecControl->GetShowRulerLabels() );
				}
				break;
			}
			case Layout::MX_M_VIEW_RULER_SHOW_ARROWS : {
				if ( phecControl ) {
					phecControl->SetShowRulerCaret( !phecControl->GetShowRulerCaret() );
				}
				break;
			}
		}
		return LSW_H_CONTINUE;
	}

	/**
	 * Custom user-defined message handler.
	 * \brief Handles WM_USER and other private messages.
	 *
	 * \param _uMsg Message identifier.
	 * \param _wParam WPARAM data.
	 * \param _lParam LPARAM data.
	 * \return Returns a LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CDeusHexMachinaWindow::CustomPrivateMsg( UINT _uMsg, WPARAM _wParam, LPARAM /*_lParam*/ ) {
		switch ( _uMsg ) {
			case CMhsMainWindow::MX_CM_UPDATE_HEX_SIBLINGS : {
				RecalcAllBut( reinterpret_cast<CHexEditorControl *>(_wParam) );
				return LSW_H_HANDLED;
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
		try {
			auto wsTabname = _pPath.filename().generic_wstring();

			CHexEditorInterface * pheiInterface = new( std::nothrow ) CHexEditorFile();
			if ( !pheiInterface || !static_cast<CHexEditorFile*>(pheiInterface)->SetFile( _pPath ) ) {
				delete pheiInterface;
				return;
			}

			AddTab( pheiInterface, wsTabname );
		}
		catch ( ... ) {}
	}

	// Handles opening a process via the Open Process dialog (returns true if a process was actually opened).
	bool CDeusHexMachinaWindow::OpenProcess() {
		MX_OPTIONS oOptions = m_pmhMemHack->Options();
		oOptions.pmhMemHackObj = m_pmhMemHack;
		DWORD dwId = COpenProcessLayout::CreateOpenProcessDialog( this, &oOptions );
		if ( dwId != DWINVALID ) {
			// CProcess::MX_OPM_FIXED
			//
			// PROCESS_CREATE_THREAD: CreateThread()
			// PROCESS_DUP_HANDLE: DuplicateHandle()
			// PROCESS_VM_READ: ReadProcessMemory()
			// PROCESS_VM_WRITE: WriteProcessMemory()
			// PROCESS_VM_OPERATION: VirtualProtectEx(), WriteProcessMemory()
			// PROCESS_QUERY_INFORMATION: OpenProcessToken(), GetProcessMemoryInfo(), GetExitCodeProcess(), GetPriorityClass(), IsProcessInJob(), QueryFullProcessImageNameW()
			// PROCESS_QUERY_LIMITED_INFORMATION: GetProcessMemoryInfo(), GetExitCodeProcess(), GetPriorityClass(), IsProcessInJob(), QueryFullProcessImageNameW()
			// PROCESS_SUSPEND_RESUME: NtSuspendProcess(), NtResumeProcess()
			// PROCESS_TERMINATE: TerminateProcess()
			// SYNCHRONIZE: SignalObjectAndWait(), WaitForSingleObject(), WaitForSingleObjectEx(), WaitForMultipleObjects(), WaitForMultipleObjectsEx(), MsgWaitForMultipleObjects(), MsgWaitForMultipleObjectsEx(), etc.

			try {
				CHexEditorInterface * pheiInterface = new( std::nothrow ) CHexEditorProcess();
				if ( !pheiInterface /*|| !static_cast<CHexEditorProcess*>(pheiInterface)->SetFile( _pPath )*/ ) {
					return false;
				}

				DWORD dwAttempts[] = {
					PROCESS_CREATE_THREAD |											// The full range of what we might want to do.
						PROCESS_VM_READ | PROCESS_VM_WRITE |
						PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION |
						PROCESS_SUSPEND_RESUME,
					PROCESS_CREATE_THREAD |											// Maybe we can live without suspending the process.
						PROCESS_VM_READ | PROCESS_VM_WRITE |
						PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION,

					PROCESS_QUERY_INFORMATION,										// The bare minimum.
					/*PROCESS_CREATE_THREAD |											// Maybe we can live without suspending the process.
						PROCESS_VM_READ | PROCESS_VM_WRITE |
						PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION,*/
				};
				for ( size_t I = 0; I < MX_ELEMENTS( dwAttempts ); ++I ) {
					if ( static_cast<CHexEditorProcess *>(pheiInterface)->SetProcess( dwId, CProcess::MX_OPM_FIXED, dwAttempts[I] ) ) {
						break;
					}
				}
				if ( !static_cast<CHexEditorProcess *>(pheiInterface)->Process().ProcIsOpened() ) {
					delete pheiInterface;
					return false;
				}
				auto swsPath = static_cast<CHexEditorProcess *>(pheiInterface)->Process().QueryProcessImageName();
				auto wsTabname = std::filesystem::path( swsPath.c_str() ).filename().generic_wstring();

				return AddTab( pheiInterface, wsTabname );
			}
			catch ( ... ) {}
		}
		return false;
	}

	// Handles opening the current process (opens the Open Process dialog if necessary) (returns true if a process was actually opened).
	bool CDeusHexMachinaWindow::OpenCurProcess( uint64_t _ui64StartAddress ) {
		if ( !m_pmhMemHack->Process().ProcIsOpened() ) {
			if ( !CBase::PromptYesNo( Wnd(), _DEC_WS_A4FA31FF_No_process_has_been_opened__r_nOpen_a_process_now_.c_str(),
				_DEC_WS_8C0AA5EB_No_Process_Opened.c_str() ) ) { return 0; }
			if ( !m_pmhMemHack->Window()->OpenProcess() ) { return 0; }
		}
		// Should not be possible, but just one last check.
		if ( !m_pmhMemHack->Process().ProcIsOpened() ) { return 0; }

		try {
			CHexEditorInterface * pheiInterface = new( std::nothrow ) CHexEditorCurProcess( m_pmhMemHack->Process() );
			if ( !pheiInterface ) {
				return false;
			}
			if ( !static_cast<CHexEditorCurProcess *>(pheiInterface)->Process().ProcIsOpened() ) {
				delete pheiInterface;
				return false;
			}
			auto swsPath = static_cast<CHexEditorCurProcess *>(pheiInterface)->Process().QueryProcessImageName();
			auto wsTabname = std::filesystem::path( swsPath.c_str() ).filename().generic_wstring();

			return AddTab( pheiInterface, wsTabname );
		}
		catch ( ... ) {}
		return false;
	}

	// Closes the active tab.
	void CDeusHexMachinaWindow::CloseTab() {
		auto ptTab = Tab();
		if ( !ptTab ) { return; }
		size_t sIdx = size_t( ptTab->GetCurSel() );
		if ( sIdx >= m_vTabs.size() ) { return; }
		//m_vTabs[sIdx].phecWidget->
		ptTab->DeleteItem( int( sIdx ) );
		delete m_vTabs[sIdx].pheiInterface;
		m_vTabs.erase( m_vTabs.begin() + sIdx );
	}

	// Close all tabs.
	void CDeusHexMachinaWindow::CloseAllTabs() {
		while ( m_vTabs.size() ) {
			CloseTab();
		}
	}

	// Enlarge font.
	void CDeusHexMachinaWindow::EnlargeFont() {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }
		LOGFONTW lfFont;
		phecControl->IncreaseFontSize( lfFont );

		RecalcAllBut( phecControl );
	}

	// Ensmall font.
	void CDeusHexMachinaWindow::EnsmallFont() {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }
		LOGFONTW lfFont;
		phecControl->DecreaseFontSize( lfFont );

		RecalcAllBut( phecControl );
	}

	// Reset font.
	void CDeusHexMachinaWindow::ResetFont() {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }

		CHexEditorControl::MX_FONT_SET * fsSet = phecControl->Font();
		fsSet->i32PointSize = CHexEditorControl::DefaultPointSize();
		CHexEditorControl::ChooseDefaultFont( (*fsSet), m_wDpiY, Wnd() );
		CHexEditorControl::ComputeFontMetrics( (*fsSet), m_wDpiY, Wnd() );

		RecalcAllBut( phecControl );
	}

	// Sets the font size back to normal.
	void CDeusHexMachinaWindow::ResetFontSize() {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }

		phecControl->SetFontSize( CHexEditorControl::DefaultPointSize() );

		RecalcAllBut( phecControl );
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

	/**
	 * The WM_INITMENUPOPUP handler.
	 *
	 * \param _hMenu A handle to the drop-down menu or submenu.
	 * \param _wPos The zero-based relative position of the menu item that opens the drop-down menu or submenu.
	 * \param _bIsWindowMenu Indicates whether the drop-down menu is the window menu. If the menu is the window menu, this parameter is TRUE; otherwise, it is FALSE.
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CDeusHexMachinaWindow::InitMenuPopup( HMENU _hMenu, WORD /*_wPos*/, BOOL _bIsWindowMenu ) {
		if ( _bIsWindowMenu ) { return LSW_H_CONTINUE; }
		auto ptTab = Tab();
		if ( !ptTab ) { return LSW_H_CONTINUE; }
		size_t sIdx = size_t( ptTab->GetCurSel() );
		auto phecControl = CurrentEditor();
		bool bTabIsOpen = sIdx < m_vTabs.size() && ptTab->GetItemCount() && phecControl;

		for ( int I = ::GetMenuItemCount( _hMenu ); --I >= 0; ) {
			UINT uiId = ::GetMenuItemID( _hMenu, I );
			switch ( uiId ) {
				case Layout::MX_M_FILE_SAVE : {}				MX_FALLTHROUGH
				case Layout::MX_M_FILE_SAVE_AS : {}				MX_FALLTHROUGH
				case Layout::MX_M_FILE_SAVE_A_COPY : {}			MX_FALLTHROUGH
				case Layout::MX_M_FILE_SAVE_ALL : {}			MX_FALLTHROUGH
				case Layout::MX_M_FILE_CLOSE : {}				MX_FALLTHROUGH
				case Layout::MX_M_FILE_CLOSE_ALL : {}			MX_FALLTHROUGH
				case Layout::MX_M_FILE_REVERT_REFRESH : {
					::EnableMenuItem(
						_hMenu, uiId,
						MF_BYCOMMAND | (phecControl ? MF_ENABLED : MF_GRAYED) );
					break;
				}
				case Layout::MX_M_VIEW_FONT_ENLARGE_FONT : {
					::EnableMenuItem(
						_hMenu, uiId,
						MF_BYCOMMAND | ((phecControl && phecControl->Font()->i32PointSize < 72) ? MF_ENABLED : MF_GRAYED) );
					break;
				}
				case Layout::MX_M_VIEW_FONT_SHRINK_FONT : {
					::EnableMenuItem(
						_hMenu, uiId,
						MF_BYCOMMAND | ((phecControl && phecControl->Font()->i32PointSize > 2) ? MF_ENABLED : MF_GRAYED) );
					break;
				}

				case Layout::MX_M_VIEW_EDIT_AS_TEXT : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetViewType() == CHexEditorControl::MX_EA_TEXT) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_EDIT_AS_HEX : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetViewType() == CHexEditorControl::MX_EA_HEX) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_EDIT_AS_BINARY : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetViewType() == CHexEditorControl::MX_EA_BINARY) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_EDIT_AS_SCRIPT : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetViewType() == CHexEditorControl::MX_EA_SCRIPT) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_EDIT_AS_TEMPLATE : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetViewType() == CHexEditorControl::MX_EA_TEMPLATE) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_EDIT_AS_EBCDIC : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetViewType() == CHexEditorControl::MX_EA_EBCDIC) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_EDIT_AS_UNICODE : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetViewType() == CHexEditorControl::MX_EA_UTF16) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_EDIT_AS_UTF8 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetViewType() == CHexEditorControl::MX_EA_UTF8) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_EDIT_AS_PROCESS : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetViewType() == CHexEditorControl::MX_EA_PROCESS) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_EDIT_AS_CODE : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetViewType() == CHexEditorControl::MX_EA_CODE) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_EDIT_AS_TAGGED : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetViewType() == CHexEditorControl::MX_EA_TAGGED) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}

				case Layout::MX_M_VIEW_CHAR_SET_ASCII : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_ASCII) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_CHAR_SET_ASCII_ANSI : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_ASCII_ANSI) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_CHAR_SET_ASCII_OEM : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_ASCII_OEM) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_CHAR_SET_EBCDIC : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_EBCDIC) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_CHAR_SET_UNICODE : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_UNICODE) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_CHAR_SET_UTF8 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_UTF8) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_CHAR_SET_MACINTOSH : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_MACINTOSH) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_CHAR_SET_ARABIC_WINDOWS : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_ARABIC_WINDOWS) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_CHAR_SET_ARABIC_ISO : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_ARABIC_ISO) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_CHAR_SET_BALTIC_WINDOWS : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_BALTIC_WINDOWS) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_CHAR_SET_BALTIC_ISO : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_BALTIC_ISO) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_CHAR_SET_CHINESE_SIMP : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_CHINESE_SIMPLIFIED) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_CHAR_SET_CHINESE_TRAD : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_CHINESE_TRADITIONAL) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_CHAR_SET_CYRILLIC_WINDOWS : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_CYRILLIC_WINDOWS) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_CHAR_SET_CYRILLIC_KOI8R : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_CYRILLIC_KOI8_R) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_CHAR_SET_CYRILLIC_KOI8U : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_CYRILLIC_KOI8_U) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_CHAR_SET_CYRILLIC_ISO : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_CYRILLIC_ISO) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_CHAR_SET_EASTERN_EUR_WINDOWS : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_EASTERN_EUROPE_WINDOWS) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_CHAR_SET_EASTERN_EUR_ISO : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_EASTERN_EUROPE_ISO) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_CHAR_SET_GREEK_WINDOWS : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_GREEK_WINDOWS) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_CHAR_SET_GREEK_ISO : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_GREEK_ISO) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_CHAR_SET_HEBREW_WINDOWS : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_HEBREW_WINDOWS) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_CHAR_SET_HEBREW_ISO : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_HEBREW_ISO) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_CHAR_SET_JAPANESE_JIS : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_JAPANESE_JIS) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_CHAR_SET_JAPANESE_EUC : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_JAPANESE_EUC_JP) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_CHAR_SET_KOREAN : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_KOREAN) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_CHAR_SET_THAI : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_THAI) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_CHAR_SET_TURKISH_WINDOWS : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_TURKISH_WINDWOS) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_CHAR_SET_TURKISH_ISO : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_TURKISH_ISO) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_CHAR_SET_VIETNAMESE : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetCharacterSet() == CCharSets::MX_CS_VIETNAMESE) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}

				case Layout::MX_M_VIEW_ADDRESSES_SHOW_LINE_NUMBERS_ADDRESSES : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetShowAddressGutter()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_ADDRESSES_DISPLAY_FORMATS_BYTE_HEX : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetAddressGutterFmr() == CHexEditorControl::MX_AF_BYTES_HEX) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_ADDRESSES_DISPLAY_FORMATS_BYTE_DEC : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetAddressGutterFmr() == CHexEditorControl::MX_AF_BYTES_DEC) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_ADDRESSES_DISPLAY_FORMATS_BYTE_OCT : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetAddressGutterFmr() == CHexEditorControl::MX_AF_BYTES_OCT) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_ADDRESSES_DISPLAY_FORMATS_LINE_DEC : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetAddressGutterFmr() == CHexEditorControl::MX_AF_LINE_NUMBER) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_ADDRESSES_DISPLAY_FORMATS_SHORT_HEX : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetAddressGutterFmr() == CHexEditorControl::MX_AF_SHORT_HEX) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_ADDRESSES_DISPLAY_FORMATS_SHORT_DEC : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetAddressGutterFmr() == CHexEditorControl::MX_AF_SHORT_DEC) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}

				case Layout::MX_M_VIEW_LINE_WIDTH_4_BYTES : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && !phecControl->GetLineWidthAuto() && phecControl->GetLineWidth() == 4) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LINE_WIDTH_8_BYTES : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && !phecControl->GetLineWidthAuto() && phecControl->GetLineWidth() == 8) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LINE_WIDTH_12_BYTES : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && !phecControl->GetLineWidthAuto() && phecControl->GetLineWidth() == 12) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LINE_WIDTH_16_BYTES : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && !phecControl->GetLineWidthAuto() && phecControl->GetLineWidth() == 16) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LINE_WIDTH_20_BYTES : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && !phecControl->GetLineWidthAuto() && phecControl->GetLineWidth() == 20) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LINE_WIDTH_40_BYTES : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && !phecControl->GetLineWidthAuto() && phecControl->GetLineWidth() == 40) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LINE_WIDTH_AUTO : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLineWidthAuto()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LINE_WIDTH_CUSTOM : {
					bool bIsKnownSize = phecControl && (phecControl->GetLineWidth() == 4 ||
						phecControl->GetLineWidth() == 8 ||
						phecControl->GetLineWidth() == 12 ||
						phecControl->GetLineWidth() == 16 ||
						phecControl->GetLineWidth() == 20 ||
						phecControl->GetLineWidth() == 40);
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && !phecControl->GetLineWidthAuto() && !bIsKnownSize) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}

				case Layout::MX_M_VIEW_GROUP_BY_BYTE : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetGroupSize() == 1) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_GROUP_BY_SHORT : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetGroupSize() == 2) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_GROUP_BY_INT : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetGroupSize() == 4) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_GROUP_BY_INT64 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetGroupSize() == 8) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_GROUP_BY_CUSTOM : {
					bool bIsKnownSize = phecControl && (phecControl->GetGroupSize() == 1 ||
						phecControl->GetGroupSize() == 2 ||
						phecControl->GetGroupSize() == 4 ||
						phecControl->GetGroupSize() == 8);
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( !bIsKnownSize ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}

				case Layout::MX_M_VIEW_DIVISION_LINES_NONE : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetDivisionLines() == 0) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_DIVISION_LINES_1_BYTE : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetDivisionLines() == 1) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_DIVISION_LINES_2_BYTES : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetDivisionLines() == 2) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_DIVISION_LINES_4_BYTES : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetDivisionLines() == 4) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_DIVISION_LINES_8_BYTES : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetDivisionLines() == 8) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}

				case Layout::MX_M_VIEW_LEFT_AREA_HEX : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == CHexEditorControl::MX_DF_HEX) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LEFT_AREA_CHAR : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == CHexEditorControl::MX_DF_CHAR) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LEFT_AREA_OCTAL : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == CHexEditorControl::MX_DF_OCT) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LEFT_AREA_BINARY : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == CHexEditorControl::MX_DF_BIN) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LEFT_AREA_DECIMAL : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == CHexEditorControl::MX_DF_DEC) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}

				case Layout::MX_M_VIEW_RIGHT_AREA_HEX : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == CHexEditorControl::MX_DF_HEX && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_CHAR : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == CHexEditorControl::MX_DF_CHAR && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_OCTAL : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == CHexEditorControl::MX_DF_OCT && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_BINARY : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == CHexEditorControl::MX_DF_BIN && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_DECIMAL : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == CHexEditorControl::MX_DF_DEC && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_HIDE : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}

				case Layout::MX_M_VIEW_HIGHLIGHTING_LINEFEED_CHARS : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetHighlightingLinefeed()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_HIGHLIGHTING_ALPHA_CHARS : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetHighlightingAlphaNumeric()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_HIGHLIGHTING_CONTROL_CHARS : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetHighlightingControl()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_HIGHLIGHTING_NON_ASCII : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetHighlightingNonAscii()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_HIGHLIGHTING_ZEROES : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetHighlightingZeros()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_HIGHLIGHTING_POINTERS : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetHighlightingPointers()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}

				case Layout::MX_M_VIEW_RULER_SHOW : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetShowRuler()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RULER_SHOW_LABELS : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetShowRulerLabels()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RULER_SHOW_ARROWS : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetShowRulerCaret()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
			}
		}

		return LSW_H_HANDLED;
	}

	// Recalc all but the given control.
	void CDeusHexMachinaWindow::RecalcAllBut( const CHexEditorControl * _phecSkipMe ) {
		for ( size_t I = m_vTabs.size(); I--; ) {
			if ( m_vTabs[I].phecWidget != _phecSkipMe ) {
				m_vTabs[I].phecWidget->RecalcAndInvalidate();
			}
		}
	}

	// Adds a tab.
	bool CDeusHexMachinaWindow::AddTab( CHexEditorInterface * _pheiInterface, const std::wstring &_wsName ) {
		MX_HEX_TAB htTab;
		htTab.pheiInterface = _pheiInterface;
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

					sLeftSizeExp.c_str(), 0,						// pcLeftSizeExp
					sRightSizeExp.c_str(), 0,						// pcRightSizeExp
					sTopSizeExp.c_str(), 0,							// pcTopSizeExp
					sBottomSizeExp.c_str(), 0,						// pcBottomSizeExp
					nullptr, 0,										// pcWidthSizeExp
					nullptr, 0,										// pcHeightSizeExp
				} );
				CHexEditorControl::MX_CREATION_PARMS cpCreation;
				cpCreation.psOptions = m_sOptions;
				cpCreation.pfsFixedRowFont = &m_fsFixedRowFont;
				cpCreation.pfsDynamicRowFont = &m_fsTextViewFont;
				cpCreation.phecFg = &m_hecFgColors;
				cpCreation.phecBg = &m_hecBgColors;
				htTab.phecWidget = static_cast<CHexEditorControl *>(static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager())->CreateWidget( wlLayout, ptTab, TRUE, NULL, reinterpret_cast<uint64_t>(&cpCreation) ));
				if ( !htTab.phecWidget ) {
					//delete tTab.ppoPeObject;
					return false;
				}
				htTab.phecWidget->InitControl( htTab.phecWidget->Wnd() );

				m_vTabs.insert( m_vTabs.begin(), htTab );

				
				TCITEMW tciItem = { 0 };
				tciItem.mask = TCIF_TEXT;
				tciItem.pszText = const_cast<LPWSTR>(_wsName.c_str());

				

				if ( ptTab->InsertItem( 0, &tciItem, htTab.phecWidget ) == -1 ) {
					delete htTab.phecWidget;
					delete htTab.pheiInterface;
					return false;
				}

				htTab.phecWidget->SetStream( htTab.pheiInterface );

				LSW_RECT rInternalSize = ptTab->WindowRect().ScreenToClient( ptTab->Wnd() );
				ptTab->AdjustRect( FALSE, &rInternalSize );
				::MoveWindow( htTab.phecWidget->Wnd(), rInternalSize.left, rInternalSize.top, rInternalSize.Width(), rInternalSize.Height(), TRUE );
				return true;
			}
		}
		catch ( ... ) {
			delete htTab.phecWidget;
			delete htTab.pheiInterface;
		}
		return false;
	}

}	// namespace mx
