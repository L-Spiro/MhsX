#include "MXDeusHexMachinaWindow.h"
#include "../Layouts/MXLayoutMacros.h"
#include "../Layouts/MXLayoutManager.h"
#include "../Layouts/MXExportHexLayout.h"
#include "../Layouts/MXExpressionEvaluatorLayout.h"
#include "../Layouts/MXFontDialogLayout.h"
#include "../Layouts/MXNumberInputLayout.h"
#include "../Layouts/MXOpenProcessLayout.h"
#include "../Layouts/MXMiniMapOptionsLayout.h"
#include "../Layouts/MXSelectRangeLayout.h"
#include "../MainWindow/MXMhsMainWindow.h"
#include "../MemHack/MXWindowMemHack.h"
#include "../NumberInputWindow/MXNumberInputDialog.h"
#include "../Utilities/MXUtilities.h"
#include "MXCopyFormatter.h"
#include "MXCopyPasteManager.h"
#include "MXFindProgressDialog.h"
#include "MXHexEditorCurProcess.h"
#include "MXHexEditorFile.h"
#include "MXHexEditorProcess.h"
#include "MXHexImport.h"

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
			{ L"cp",			MX_I_CURPROC },
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

		m_sOptions[CHexEditorControl::MX_EA_HEX].dfLeftNumbersFmt = MX_DF_HEX;
		m_sOptions[CHexEditorControl::MX_EA_HEX].dfRightNumbersFmt = MX_DF_CHAR;

		m_sOptions[CHexEditorControl::MX_EA_BINARY].dfLeftNumbersFmt = MX_DF_BIN;
		m_sOptions[CHexEditorControl::MX_EA_BINARY].dfRightNumbersFmt = MX_DF_CHAR;

		m_sOptions[CHexEditorControl::MX_EA_CUR_PROCESS].dfLeftNumbersFmt = MX_DF_HEX;
		m_sOptions[CHexEditorControl::MX_EA_CUR_PROCESS].dfRightNumbersFmt = MX_DF_CHAR;

		m_sOptions[CHexEditorControl::MX_EA_PROCESS].dfLeftNumbersFmt = MX_DF_HEX;
		m_sOptions[CHexEditorControl::MX_EA_PROCESS].dfRightNumbersFmt = MX_DF_CHAR;
	}
	CDeusHexMachinaWindow::~CDeusHexMachinaWindow() {
		/*for ( auto I = m_vTabs.size(); I--; ) {
			delete m_vTabs[I].pheiInterface;
		}*/
		// Clipboards must not keep the files open after the window is gone.
		for ( auto I = m_vTabs.size(); I--; ) {
			CCopyPasteManager::ReleaseInterface( m_vTabs[I].pheiInterface.get() );
			if ( m_vTabs[I].phecWidget ) {
				m_vTabs[I].phecWidget->ReleaseStream();
			}
		}
	}

	// == Functions.
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
			{ m_iImageMap[MX_I_NEW],			Layout::MX_M_FILE_NEW,						TBSTATE_ENABLED,	BTNS_AUTOSIZE | BTNS_DROPDOWN,	{ 0 },		0,		MX_TOOL_STR( L"New" ) },
			{ m_iImageMap[MX_I_OPENFILE],		Layout::MX_M_FILE_OPENFILE,					TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Open" ) },
			{ m_iImageMap[MX_I_CURPROC],		Layout::MX_M_FILE_OPENCURPROCESS,			TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Current Process" ) },
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

		plvToolBar->AddButtons( bButtons, std::size( bButtons ) );
		// The arrow beside New lists the kinds of files that can be created.
		plvToolBar->SetDropDownMenu( Layout::MX_M_FILE_NEW, Layout::FileNewDropDown(), 1, this );

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
				{ rRebarRect.Width() - psbStatus->ClientRect( this ).Height() - 523, TRUE },			// MX_SB_MESSAGE
				{ rRebarRect.Width() - psbStatus->ClientRect( this ).Height() - 192-48-48, TRUE },		// MX_SB_POSITION_START
				{ rRebarRect.Width() - psbStatus->ClientRect( this ).Height() - 192-48, TRUE },			// MX_SB_VALUE_SEL
				{ rRebarRect.Width() - psbStatus->ClientRect( this ).Height() - 192, TRUE },			// MX_SB_FILE_SIZE
				{ rRebarRect.Width() - psbStatus->ClientRect( this ).Height() - 168, TRUE },			// MX_SB_VIEW_TYPE
				{ rRebarRect.Width() - psbStatus->ClientRect( this ).Height() - 132, TRUE },			// MX_SB_CHAR_SET

				{ rRebarRect.Width() - psbStatus->ClientRect( this ).Height() - 96, TRUE },				// MX_SB_ENDIAN
				{ rRebarRect.Width() - psbStatus->ClientRect( this ).Height() - 48, TRUE },				// MX_SB_CLIPBOARD
				{ rRebarRect.Width() - psbStatus->ClientRect( this ).Height(), TRUE },					// MX_SB_INSERT_OVERWRITE
				{ rRebarRect.Width() - psbStatus->ClientRect( this ).Height(), TRUE },
			};
			psbStatus->SetParts( spParts, std::size( spParts ) );

			UpdateStatusBar_Clipboard();
		}
		UpdateStatusBar_InsertOverwrite();
		UpdateToolBar();


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
		// Anything done from the menu or an accelerator comes after the real-time edits typed so far, which become one Undo item.
		if ( phecControl ) { phecControl->FlushRealTimeUndo(); }

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
			case Layout::MX_M_FILE_NEW : {
				NewFile( m_nfLastNew );
				break;
			}
			case Layout::MX_M_FILE_NEW_TEXT_FILE : {
				NewFile( MX_NF_TEXT );
				break;
			}
			case Layout::MX_M_FILE_NEW_HEX_FILE : {
				NewFile( MX_NF_HEX );
				break;
			}
			case Layout::MX_M_FILE_NEW_UNICODE_FILE : {
				NewFile( MX_NF_UNICODE );
				break;
			}
			case Layout::MX_M_FILE_NEW_UTF8_FILE : {
				NewFile( MX_NF_UTF8 );
				break;
			}
			case Layout::MX_M_FILE_NEW_C_FILE : {
				NewFile( MX_NF_C );
				break;
			}
			case Layout::MX_M_FILE_NEW_HTML_FILE : {
				NewFile( MX_NF_HTML );
				break;
			}
			case Layout::MX_M_FILE_NEW_PHP_FILE : {
				NewFile( MX_NF_PHP );
				break;
			}
			case Layout::MX_M_FILE_NEW_XML_FILE : {
				NewFile( MX_NF_XML );
				break;
			}
			// Accelerators bypass the greyed-out menu items, so each of these checks again whether it can run.
			case Layout::MX_M_FILE_SAVE : {
				Save();
				break;
			}
			case Layout::MX_M_FILE_SAVE_AS : {
				SaveAs();
				break;
			}
			case Layout::MX_M_FILE_SAVE_A_COPY : {
				SaveACopy();
				break;
			}
			case Layout::MX_M_FILE_SAVE_SELECTION : {
				SaveSelection();
				break;
			}
			case Layout::MX_M_FILE_SAVE_ALL : {
				SaveAll();
				break;
			}
			case Layout::MX_M_FILE_REVERT_REFRESH : {
				RevertRefresh();
				break;
			}
			case Layout::MX_M_FILE_RENAME_FILE : {
				RenameFile();
				break;
			}
			case Layout::MX_M_FILE_DELETE_FILE : {
				DeleteCurrentFile();
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
				// The window may be gone.
				return LSW_H_CONTINUE;
			}
			
			case Layout::MX_M_EDIT_UNDO : {
				Undo();
				break;
			}
			case Layout::MX_M_EDIT_REDO : {
				Redo();
				break;
			}

			case Layout::MX_M_EDIT_CUT : {
				Cut();
				break;
			}
			case Layout::MX_M_EDIT_COPY : {
				Copy();
				break;
			}
			case Layout::MX_M_EDIT_PASTE : {
				Paste();
				break;
			}
			case Layout::MX_M_EDIT_CLIPBOARD_0 : {}						MX_FALLTHROUGH
			case Layout::MX_M_EDIT_CLIPBOARD_1 : {}						MX_FALLTHROUGH
			case Layout::MX_M_EDIT_CLIPBOARD_2 : {}						MX_FALLTHROUGH
			case Layout::MX_M_EDIT_CLIPBOARD_3 : {}						MX_FALLTHROUGH
			case Layout::MX_M_EDIT_CLIPBOARD_4 : {}						MX_FALLTHROUGH
			case Layout::MX_M_EDIT_CLIPBOARD_5 : {}						MX_FALLTHROUGH
			case Layout::MX_M_EDIT_CLIPBOARD_6 : {}						MX_FALLTHROUGH
			case Layout::MX_M_EDIT_CLIPBOARD_7 : {}						MX_FALLTHROUGH
			case Layout::MX_M_EDIT_CLIPBOARD_8 : {}						MX_FALLTHROUGH
			case Layout::MX_M_EDIT_CLIPBOARD_9 : {
				SelectClipboard( size_t( _wId - Layout::MX_M_EDIT_CLIPBOARD_0 ) );
				break;
			}
			case Layout::MX_M_EDIT_CLEAR_CLIPBOARDS : {
				ClearClipboards();
				break;
			}
			case Layout::MX_M_EDIT_REAL_TIME_FILE_EDITING : {
				SetRealTimeFileEditing( !RealTimeFileEditing() );
				break;
			}
			case Layout::MX_M_EDIT_REAL_TIME_RAM_TEXT_EDITING : {
				SetRealTimeRamTextEditing( !RealTimeRamTextEditing() );
				break;
			}
			case Layout::MX_M_EDIT_INSERT_MODE : {
				// Process memory cannot grow, so it is always in Overwrite mode.
				if ( IsOverwriteMode() && !(phecControl && phecControl->IsProcess()) ) { ToggleOverwriteInsert(); }
				break;
			}
			case Layout::MX_M_EDIT_OVERWRITE_MODE : {
				if ( !IsOverwriteMode() ) { ToggleOverwriteInsert(); }
				break;
			}
			case Layout::MX_M_EDIT_CLEAR_ALL_CHANGES : {
				if ( phecControl ) { phecControl->ClearAllChanges(); }
				break;
			}
			case Layout::MX_M_EDIT_CLEAR_CHANGES : {
				if ( phecControl ) { phecControl->ClearSelectedChanges(); }
				break;
			}
			case Layout::MX_M_EDIT_CLEAR_CLICKED_CHANGE : {
				if ( phecControl ) { phecControl->ClearClickedChange(); }
				break;
			}
			case Layout::MX_M_EDIT_READ_ONLY : {
				ToggleReadOnly();
				break;
			}
			case Layout::MX_M_EDIT_KEEP_FILE_TIME : {
				ToggleKeepFileTime();
				break;
			}
			case Layout::MX_M_EDIT_PROPERTIES : {
				ShowProperties();
				break;
			}
			case Layout::MX_M_SELECT_SELECT_RANGE : {
				ShowSelectRange();
				break;
			}
			case Layout::MX_M_VIEW_FONT_CHANGE_FONT : {
				ShowChangeFont();
				break;
			}
			case Layout::MX_M_VIEW_ADDRESSES_SET_STARTING_ADDRESS : {
				ShowSetStartingAddress();
				break;
			}
			case Layout::MX_M_FILE_EXPORT_HEX : {
				ExportHex();
				break;
			}
			case Layout::MX_M_FILE_IMPORT_HEX : {
				ImportHex();
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_HEX : {
				Copy( MX_CA_HEX_TEXT );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_DECIMAL : {
				Copy( MX_CA_DECIMAL_TEXT );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_BINARY : {
				Copy( MX_CA_BINARY_TEXT );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_C : {
				Copy( MX_CA_C_CODE );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_JAVA : {
				Copy( MX_CA_JAVA_CODE );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_PYTHON : {
				Copy( MX_CA_PYTHON_CODE );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_C_STRING : {
				Copy( MX_CA_C_STRING );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_CS_STRING : {
				Copy( MX_CA_CS_STRING );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_JAVA_STRING : {
				Copy( MX_CA_JAVA_STRING );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_JS_STRING : {
				Copy( MX_CA_JS_STRING );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_PYTHON_STRING : {
				Copy( MX_CA_PYTHON_STRING );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_8_BIT_HEX : {
				Copy( MX_CA_INTEL_8 );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_16_BIT_HEX : {
				Copy( MX_CA_INTEL_16 );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_32_BIT_HEX : {
				Copy( MX_CA_INTEL_32 );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_MOTOROLA_S19 : {
				Copy( MX_CA_MOTOROLA_S19 );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_MOTOROLA_S28 : {
				Copy( MX_CA_MOTOROLA_S28 );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_MOTOROLA_S37 : {
				Copy( MX_CA_MOTOROLA_S37 );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_HEX_EDITOR_AREAS : {
				Copy( MX_CA_HEX_EDITOR_AREAS );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_WEB_PAGE : {
				Copy( MX_CA_WEB_PAGE );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_RICH_TEXT : {
				Copy( MX_CA_RICH_TEXT );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_BASE64 : {
				Copy( MX_CA_BASE64 );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_UUENCODING : {
				Copy( MX_CA_UUENCODING );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_UINT8 : {
				Copy( MX_CA_UINT8 );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_INT8 : {
				Copy( MX_CA_INT8 );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_UINT16 : {
				Copy( MX_CA_UINT16 );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_INT16 : {
				Copy( MX_CA_INT16 );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_UINT32 : {
				Copy( MX_CA_UINT32 );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_INT32 : {
				Copy( MX_CA_INT32 );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_UINT64 : {
				Copy( MX_CA_UINT64 );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_INT64 : {
				Copy( MX_CA_INT64 );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_P16 : {
				Copy( MX_CA_P16 );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_P32 : {
				Copy( MX_CA_P32 );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_P64 : {
				Copy( MX_CA_P64 );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_FLOAT10 : {
				Copy( MX_CA_FLOAT10 );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_FLOAT11 : {
				Copy( MX_CA_FLOAT11 );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_BFLOAT : {
				Copy( MX_CA_BFLOAT );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_FLOAT16 : {
				Copy( MX_CA_FLOAT16 );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_FLOAT32 : {
				Copy( MX_CA_FLOAT32 );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_FLOAT64 : {
				Copy( MX_CA_FLOAT64 );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_PCM8 : {
				Copy( MX_CA_PCM8 );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_PCM16 : {
				Copy( MX_CA_PCM16 );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_PCM24 : {
				Copy( MX_CA_PCM24 );
				break;
			}
			case Layout::MX_M_EDIT_COPY_AS_PCM32 : {
				Copy( MX_CA_PCM32 );
				break;
			}

			case Layout::MX_M_EDIT_DELETE : {
				DeleteSelectedOrCaret();
				break;
			}

			case Layout::MX_M_SELECT_SELECT_ALL : {
				SelectAll();
				break;
			}
			case Layout::MX_M_SELECT_WORD : {
				SelectWord();
				break;
			}
			case Layout::MX_M_SELECT_LINE : {
				SelectLine();
				break;
			}
			case Layout::MX_M_SELECT_REVSERSE_SELECTION : {
				ReverseSelection();
				break;
			}
			case Layout::MX_M_SELECT_MARK_SELECTION_START : {
				MarkSelectionStart();
				break;
			}
			case Layout::MX_M_SELECT_MARK_SELECTION_END : {
				MarkSelectionEnd();
				break;
			}
			case Layout::MX_M_SELECT_COLUMN_MODE : {
				ToggleColumnSelection();
				break;
			}
			case Layout::MX_M_SELECT_MOVE_SELECTION_BACK : {
				MoveSelectionBack();
				break;
			}
			case Layout::MX_M_SELECT_MOVE_SELECTION_FORWARD : {
				MoveSelectionForward();
				break;
			}
			
			case Layout::MX_M_FIND_GOTO : {
				ShowGoTo();
				break;
			}
			case Layout::MX_M_FIND_FIND : {
				ShowFind();
				break;
			}
			case Layout::MX_M_TOOLS_EXPRESSION_EVALUATOR : {
				ShowExpEval();
				break;
			}
			case Layout::MX_M_TOOLS_WINDOWS_CALCULATOR : {
				::ShellExecuteW( Wnd(), nullptr, _DEC_WS_9A9A2865_Calc_exe.c_str(), nullptr, nullptr, SW_SHOWNORMAL );
				break;
			}
			case Layout::MX_M_TOOLS_WINDOWS_NOTEPAD : {
				::ShellExecuteW( Wnd(), nullptr, _DEC_WS_6DAC8A48_Notepad_exe.c_str(), nullptr, nullptr, SW_SHOWNORMAL );
				break;
			}
			case Layout::MX_M_FIND_FIND_NEXT : {
				FindAgain( false );
				break;
			}
			case Layout::MX_M_FIND_FIND_PREV : {
				FindAgain( true );
				break;
			}


			case Layout::MX_M_VIEW_EDIT_AS_TEXT : {
				if ( phecControl ) {
					phecControl->SetViewType( CHexEditorControl::MX_EA_TEXT );
					UpdateStatusBar_ViewType();
				}
				break;
			}
			case Layout::MX_M_VIEW_EDIT_AS_HEX : {
				if ( phecControl ) {
					phecControl->SetViewType( CHexEditorControl::MX_EA_HEX );
					UpdateStatusBar_ViewType();
				}
				break;
			}
			case Layout::MX_M_VIEW_EDIT_AS_BINARY : {
				if ( phecControl ) {
					phecControl->SetViewType( CHexEditorControl::MX_EA_BINARY );
					UpdateStatusBar_ViewType();
				}
				break;
			}
			case Layout::MX_M_VIEW_EDIT_AS_SCRIPT : {
				if ( phecControl ) {
					phecControl->SetViewType( CHexEditorControl::MX_EA_SCRIPT );
					UpdateStatusBar_ViewType();
				}
				break;
			}
			case Layout::MX_M_VIEW_EDIT_AS_TEMPLATE : {
				if ( phecControl ) {
					phecControl->SetViewType( CHexEditorControl::MX_EA_TEMPLATE );
					UpdateStatusBar_ViewType();
				}
				break;
			}
			case Layout::MX_M_VIEW_EDIT_AS_EBCDIC : {
				if ( phecControl ) {
					phecControl->SetViewType( CHexEditorControl::MX_EA_EBCDIC );
					UpdateStatusBar_ViewType();
				}
				break;
			}
			case Layout::MX_M_VIEW_EDIT_AS_UNICODE : {
				if ( phecControl ) {
					phecControl->SetViewType( CHexEditorControl::MX_EA_UTF16 );
					UpdateStatusBar_ViewType();
				}
				break;
			}
			case Layout::MX_M_VIEW_EDIT_AS_UTF8 : {
				if ( phecControl ) {
					phecControl->SetViewType( CHexEditorControl::MX_EA_UTF8 );
					UpdateStatusBar_ViewType();
				}
				break;
			}
			case Layout::MX_M_VIEW_EDIT_AS_PROCESS : {
				if ( phecControl ) {
					phecControl->SetViewType( CHexEditorControl::MX_EA_PROCESS );
					UpdateStatusBar_ViewType();
				}
				break;
			}
			case Layout::MX_M_VIEW_EDIT_AS_CODE : {
				if ( phecControl ) {
					phecControl->SetViewType( CHexEditorControl::MX_EA_CODE );
					UpdateStatusBar_ViewType();
				}
				break;
			}
			case Layout::MX_M_VIEW_EDIT_AS_TAGGED : {
				if ( phecControl ) {
					phecControl->SetViewType( CHexEditorControl::MX_EA_TAGGED );
					UpdateStatusBar_ViewType();
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

			case Layout::MX_M_VIEW_FONT_LINE_SPACING : {
				ShowSetLineSpacing();
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
				ShowSetCustomLineWidth();
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
				ShowSetGroupWidth();
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
			case Layout::MX_M_VIEW_DIVISION_LINES_CUSTOM : {
				ShowSetDivisionLines();
				break;
			}
			case Layout::MX_M_VIEW_DIVISION_LINES_SET_STARTING : {
				
				break;
			}

			case Layout::MX_M_VIEW_LEFT_AREA_HEX : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( MX_DF_HEX );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LEFT_AREA_CHAR : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( MX_DF_CHAR );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LEFT_AREA_OCTAL : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( MX_DF_OCT );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LEFT_AREA_BINARY : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( MX_DF_BIN );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LEFT_AREA_DECIMAL : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( MX_DF_DEC );
					RecalcAllBut( phecControl );
				}
				break;
			}

			case Layout::MX_M_VIEW_LEFT_AREA_UINT8 : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( MX_DF_UINT8 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LEFT_AREA_INT8 : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( MX_DF_INT8 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LEFT_AREA_UINT16 : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( MX_DF_UINT16 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LEFT_AREA_INT16 : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( MX_DF_INT16 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LEFT_AREA_UINT32 : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( MX_DF_UINT32 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LEFT_AREA_INT32 : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( MX_DF_INT32 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LEFT_AREA_UINT64 : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( MX_DF_UINT64 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LEFT_AREA_INT64 : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( MX_DF_INT64 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LEFT_AREA_POINTER16 : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( MX_DF_POINTER16 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LEFT_AREA_POINTER32 : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( MX_DF_POINTER32 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LEFT_AREA_POINTER64 : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( MX_DF_POINTER64 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LEFT_AREA_FLOAT10 : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( MX_DF_FLOAT10 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LEFT_AREA_FLOAT11 : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( MX_DF_FLOAT11 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LEFT_AREA_BFLOAT16 : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( MX_DF_BFLOAT16 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LEFT_AREA_FLOAT16 : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( MX_DF_FLOAT16 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LEFT_AREA_FLOAT32 : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( MX_DF_FLOAT32 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LEFT_AREA_FLOAT64 : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( MX_DF_FLOAT64 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LEFT_AREA_PCM8 : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( MX_DF_PCM8 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LEFT_AREA_PCM16 : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( MX_DF_PCM16 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LEFT_AREA_PCM24 : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( MX_DF_PCM24 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_LEFT_AREA_PCM32 : {
				if ( phecControl ) {
					phecControl->SetLeftAreaFormat( MX_DF_PCM32 );
					RecalcAllBut( phecControl );
				}
				break;
			}


			case Layout::MX_M_VIEW_RIGHT_AREA_HEX : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( MX_DF_HEX );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_CHAR : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( MX_DF_CHAR );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_OCTAL : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( MX_DF_OCT );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_BINARY : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( MX_DF_BIN );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_DECIMAL : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( MX_DF_DEC );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_UINT8 : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( MX_DF_UINT8 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_INT8 : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( MX_DF_INT8 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_UINT16 : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( MX_DF_UINT16 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_INT16 : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( MX_DF_INT16 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_UINT32 : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( MX_DF_UINT32 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_INT32 : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( MX_DF_INT32 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_UINT64 : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( MX_DF_UINT64 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_INT64 : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( MX_DF_INT64 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_POINTER16 : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( MX_DF_POINTER16 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_POINTER32 : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( MX_DF_POINTER32 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_POINTER64 : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( MX_DF_POINTER64 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_FLOAT10 : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( MX_DF_FLOAT10 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_FLOAT11 : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( MX_DF_FLOAT11 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_BFLOAT16 : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( MX_DF_BFLOAT16 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_FLOAT16 : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( MX_DF_FLOAT16 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_FLOAT32 : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( MX_DF_FLOAT32 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_FLOAT64 : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( MX_DF_FLOAT64 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_PCM8 : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( MX_DF_PCM8 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_PCM16 : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( MX_DF_PCM16 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_PCM24 : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( MX_DF_PCM24 );
					RecalcAllBut( phecControl );
				}
				break;
			}
			case Layout::MX_M_VIEW_RIGHT_AREA_PCM32 : {
				if ( phecControl ) {
					phecControl->SetRightAreaFormat( MX_DF_PCM32 );
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
			case Layout::MX_M_VIEW_RULER_DISPLAY_FMT_HEX : {
				if ( phecControl ) {
					phecControl->SetRulerFormat( CHexEditorControl::MX_RF_HEX );
				}
				break;
			}
			case Layout::MX_M_VIEW_RULER_DISPLAY_FMT_DEC : {
				if ( phecControl ) {
					phecControl->SetRulerFormat( CHexEditorControl::MX_RF_DEC );
				}
				break;
			}
			case Layout::MX_M_VIEW_MINI_MAP_SHOW : {
				if ( phecControl ) {
					phecControl->SetShowMiniMap( !phecControl->GetShowMiniMap() );
				}
				break;
			}
			case Layout::MX_M_VIEW_MINI_MAP_OPTIONS : {
				ShowMiniMapOptions();
				break;
			}
		}
		// Any command can change what the toolbar buttons can do (Undo/Redo, clipboards, tabs, etc.)
		UpdateToolBar();
		return LSW_H_CONTINUE;
	}

	/**
	 * Handles TBN_GETINFOTIPW: supplies the tooltips of the toolbar buttons.  The text is decrypted only when it is needed.
	 *
	 * \param _pgitTip The notification data.  iItem is the command ID of the button.
	 * \return Returns LSW_H_HANDLED if text was supplied.
	 */
	CWidget::LSW_HANDLED CDeusHexMachinaWindow::TbnGetInfoTip( LPNMTBGETINFOTIPW _pgitTip ) {
		if ( !_pgitTip || !_pgitTip->pszText || _pgitTip->cchTextMax <= 0 ) { return LSW_H_CONTINUE; }
		try {
			CSecureWString swsTip;
			switch ( _pgitTip->iItem ) {
				case Layout::MX_M_FILE_NEW : { swsTip = _DEC_WS_53AE02A5_New; break; }
				case Layout::MX_M_FILE_OPENFILE : { swsTip = _DEC_WS_6C2A2DB6_Open_File; break; }
				case Layout::MX_M_FILE_OPENCURPROCESS : { swsTip = _DEC_WS_D1788D14_Open_Current_Process; break; }
				case Layout::MX_M_FILE_SAVE : { swsTip = _DEC_WS_F55495E0_Save; break; }
				case Layout::MX_M_FILE_SAVE_AS : { swsTip = _DEC_WS_2EB0FAD7_Save_As; break; }
				case Layout::MX_M_FILE_SAVE_ALL : { swsTip = _DEC_WS_43C53BED_Save_All; break; }
				case Layout::MX_M_FILE_CLOSE : { swsTip = _DEC_WS_D2C0AEC0_Close; break; }
				case Layout::MX_M_EDIT_CUT : { swsTip = _DEC_WS_88BDD21D_Cut; break; }
				case Layout::MX_M_EDIT_COPY : { swsTip = _DEC_WS_ED8814BC_Copy; break; }
				case Layout::MX_M_EDIT_PASTE : { swsTip = _DEC_WS_5D97579C_Paste; break; }
				case Layout::MX_M_EDIT_UNDO : { swsTip = _DEC_WS_434214CC_Undo; break; }
				case Layout::MX_M_EDIT_REDO : { swsTip = _DEC_WS_D2C0C394_Redo; break; }
				case Layout::MX_M_FIND_FIND : { swsTip = _DEC_WS_699CCB3A_Find; break; }
				case Layout::MX_M_FIND_GOTO : { swsTip = _DEC_WS_74233668_Go_To; break; }
				case Layout::MX_M_TOOLS_EXPRESSION_EVALUATOR : { swsTip = _DEC_WS_54B37832_Expression_Evaluator; break; }
				case Layout::MX_M_TOOLS_COMPARE_FILES : { swsTip = _DEC_WS_4265EB07_Compare_Files; break; }
				case Layout::MX_M_TOOLS_HISTOGRAM : { swsTip = _DEC_WS_BEFDEA03_Histogram_Entropy; break; }
				default : { return LSW_H_CONTINUE; }
			}
			::wcsncpy_s( _pgitTip->pszText, static_cast<size_t>(_pgitTip->cchTextMax), swsTip.c_str(), _TRUNCATE );
			return LSW_H_HANDLED;
		}
		catch ( ... ) { return LSW_H_CONTINUE; }
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

	/**
	 * Handles WM_CONTEXTMENU.  Right-clicking the hex editor shows its context menu.
	 *
	 * \param _pwControl The control that was right-clicked.
	 * \param _iX The horizontal position of the cursor in screen coordinates, or -1 if the menu was opened with the keyboard.
	 * \param _iY The vertical position of the cursor in screen coordinates, or -1 if the menu was opened with the keyboard.
	 * \return Returns a LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CDeusHexMachinaWindow::ContextMenu( CWidget * _pwControl, INT _iX, INT _iY ) {
		if ( ShowHexContextMenu( _pwControl, _iX, _iY ) ) { return LSW_H_HANDLED; }
		return lsw::CMainWindow::ContextMenu( _pwControl, _iX, _iY );
	}

	/**
	 * Shows the hex editor's context menu.  Called by the hex control itself when it gets WM_CONTEXTMENU.
	 *
	 * \param _pwControl The hex control.
	 * \param _iX The horizontal position of the cursor in screen coordinates, or -1 if the menu was opened with the keyboard.
	 * \param _iY The vertical position of the cursor in screen coordinates, or -1 if the menu was opened with the keyboard.
	 * \return Returns true if a menu was shown.
	 **/
	bool CDeusHexMachinaWindow::ShowHexContextMenu( lsw::CWidget * _pwControl, INT _iX, INT _iY ) {
		auto phecControl = CurrentEditor();
		if ( phecControl && _pwControl == phecControl ) {
			// Right-clicking the Mini Map shows its options.
			if ( !(_iX == -1 && _iY == -1) ) {
				POINT ptClient = { _iX, _iY };
				if ( ::ScreenToClient( phecControl->Wnd(), &ptClient ) && phecControl->PointIsOverMiniMap( ptClient ) ) {
					ShowMiniMapOptions();
					return true;
				}
			}
			// A menu opened with the keyboard applies to the caret rather than to the last address right-clicked.
			if ( _iX == -1 && _iY == -1 ) { phecControl->ForgetContextAddress(); }

			// The items are enabled and checked by InitMenuPopup(), and their commands go to Command(), as for the Edit menu.  The
			//	shortcuts are only shown (they belong to the main menu).
			LSW_MENU_ITEM miMenuBar[] = {
				//bIsSeperator	dwId											bCheckable	bChecked	bEnabled	lpwcText, stTextLen																bSkip	aHotkey
				{ FALSE,		Layout::MX_M_EDIT_CUT,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_6E69B475_Cu_t, _LEN_6E69B475 ),									FALSE,	{ static_cast<BYTE>(FVIRTKEY | FCONTROL), 'X', Layout::MX_M_EDIT_CUT } },
				{ FALSE,		Layout::MX_M_EDIT_COPY,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_446FE619__Copy, _LEN_446FE619 ),								FALSE,	{ static_cast<BYTE>(FVIRTKEY | FCONTROL), 'C', Layout::MX_M_EDIT_COPY } },
				{ FALSE,		Layout::MX_M_EDIT_PASTE,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_FB82E709__Paste, _LEN_FB82E709 ),								FALSE,	{ static_cast<BYTE>(FVIRTKEY | FCONTROL), 'V', Layout::MX_M_EDIT_PASTE } },
				{ FALSE,		Layout::MX_M_EDIT_DELETE,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_3DBE79B1_Delete, _LEN_3DBE79B1 ),								FALSE },
				{ TRUE,			0,												FALSE,		FALSE,		FALSE,		nullptr,  0,																	FALSE },
				{ FALSE,		Layout::MX_M_SELECT_SELECT_ALL,					FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_0D2831F4_Select__All, _LEN_0D2831F4 ),							FALSE,	{ static_cast<BYTE>(FVIRTKEY | FCONTROL), 'A', Layout::MX_M_SELECT_SELECT_ALL } },
				{ FALSE,		MX_M_CONTEXT_SELECT_ITEM,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_CC9D9D8D__Select, _LEN_CC9D9D8D ),								FALSE },
				{ TRUE,			0,												FALSE,		FALSE,		FALSE,		nullptr,  0,																	FALSE },
				{ FALSE,		Layout::MX_M_EDIT_CLEAR_CLICKED_CHANGE,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_F9891940_Clear_Change_Highlight_at_Clic_ked_Address, _LEN_F9891940 ),	FALSE },
				{ FALSE,		Layout::MX_M_EDIT_CLEAR_CHANGES,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_84FED5C2_Clear__Selected_Change_Highlights, _LEN_84FED5C2 ),	FALSE },
				{ FALSE,		Layout::MX_M_EDIT_CLEAR_ALL_CHANGES,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_A91F09F9_Clear_All_C_hange_Highlights, _LEN_A91F09F9 ),			FALSE },
				{ TRUE,			0,												FALSE,		FALSE,		FALSE,		nullptr,  0,																	FALSE },
				{ FALSE,		Layout::MX_M_EDIT_READ_ONLY,					TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_03A5AEDB_Read__Only, _LEN_03A5AEDB ),							FALSE },
				{ FALSE,		Layout::MX_M_EDIT_KEEP_FILE_TIME,				TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_381FB8E7__Keep_File_Time, _LEN_381FB8E7 ),						FALSE },
				{ TRUE,			0,												FALSE,		FALSE,		FALSE,		nullptr,  0,																	FALSE },
				{ FALSE,		Layout::MX_M_EDIT_PROPERTIES,					FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_CB94BF68_Prop_erties, _LEN_CB94BF68 ),							FALSE,	{ static_cast<BYTE>(FVIRTKEY | FALT), VK_RETURN, Layout::MX_M_EDIT_PROPERTIES } },
			};
			LSW_MENU_ITEM miSelect[] = {
				//bIsSeperator	dwId											bCheckable	bChecked	bEnabled	lpwcText, stTextLen																bSkip	aHotkey
				{ FALSE,		Layout::MX_M_SELECT_SELECT_RANGE,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_2ED4F2F2__Select_Range___, _LEN_2ED4F2F2 ),						FALSE,	{ static_cast<BYTE>(FVIRTKEY | FCONTROL | FALT), 'A', Layout::MX_M_SELECT_SELECT_RANGE } },
				{ TRUE,			0,												FALSE,		FALSE,		FALSE,		nullptr,  0,																	FALSE },
				{ FALSE,		Layout::MX_M_SELECT_MARK_SELECTION_START,		FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_3AD86671__Mark_Selection_Start, _LEN_3AD86671 ),				FALSE,	{ static_cast<BYTE>(FVIRTKEY | FCONTROL | FALT), VK_OEM_COMMA, Layout::MX_M_SELECT_MARK_SELECTION_START } },
				{ FALSE,		Layout::MX_M_SELECT_MARK_SELECTION_END,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_235B2698_Mark_Selection__End, _LEN_235B2698 ),					FALSE,	{ static_cast<BYTE>(FVIRTKEY | FCONTROL | FALT), VK_OEM_PERIOD, Layout::MX_M_SELECT_MARK_SELECTION_END } },
				{ TRUE,			0,												FALSE,		FALSE,		FALSE,		nullptr,  0,																	FALSE },
				{ FALSE,		Layout::MX_M_SELECT_EXPAND_SELECTION,			FALSE,		FALSE,		FALSE,		MW_MENU_TXT( _T_48180F4F_E_xpand_Selection, _LEN_48180F4F ),					FALSE,	{ static_cast<BYTE>(FVIRTKEY | FCONTROL), 'J', Layout::MX_M_SELECT_EXPAND_SELECTION } },
				{ TRUE,			0,												FALSE,		FALSE,		FALSE,		nullptr,  0,																	FALSE },
				{ FALSE,		Layout::MX_M_SELECT_REVSERSE_SELECTION,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_D0352186__Reverse_Selection, _LEN_D0352186 ),					FALSE,	{ static_cast<BYTE>(FVIRTKEY | FCONTROL), VK_OEM_7, Layout::MX_M_SELECT_REVSERSE_SELECTION } },
				{ TRUE,			0,												FALSE,		FALSE,		FALSE,		nullptr,  0,																	FALSE },
				{ FALSE,		Layout::MX_M_FILE_SAVE_SELECTION,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_B78BF6D5_Save_Selec_tion___, _LEN_B78BF6D5 ),					FALSE },
			};

			const LSW_MENU_LAYOUT miMenus[] = {
				{
					MX_M_CONTEXT_MENU,
					0,
					0,
					std::size( miMenuBar ),
					miMenuBar
				},
				{
					MX_M_CONTEXT_SELECT,
					MX_M_CONTEXT_MENU,
					MX_M_CONTEXT_SELECT_ITEM,
					std::size( miSelect ),
					miSelect
				},
			};
			mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
			return plmLayout->CreatePopupMenuEx( this, miMenus, std::size( miMenus ), _iX, _iY );
		}
		return false;
	}

	// WM_CLOSE.
	CWidget::LSW_HANDLED CDeusHexMachinaWindow::Close() {
		// Modified files are offered to be saved first; cancelling keeps the window open.
		if ( !QueryCloseAll() ) { return LSW_H_HANDLED; }
		::DestroyWindow( Wnd() );
		return LSW_H_HANDLED;
	}

	// WM_ERASEBKGND.
	CWidget::LSW_HANDLED CDeusHexMachinaWindow::EraseBkgnd( HDC _hDc ) {
		return LSW_H_HANDLED;
	}

	/**
	 * Handles WM_SETFOCUS.
	 * \brief Notified when the window receives keyboard focus.
	 *
	 * \param _hPrevFocus The window that previously had focus.
	 * \return Returns a LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CDeusHexMachinaWindow::SetFocus( HWND _hPrevFocus ) {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return LSW_H_CONTINUE; }
		phecControl->CHexEditorControl::SetFocus( _hPrevFocus );
		return LSW_H_CONTINUE;
	}

	/**
	 * Handles WM_KILLFOCUS.
	 * \brief Notified when the window loses keyboard focus.
	 *
	 * \param _hNewFocus The window receiving focus (may be NULL).
	 * \return Returns a LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CDeusHexMachinaWindow::KillFocus( HWND _hNewFocus ) {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return LSW_H_CONTINUE; }
		phecControl->CHexEditorControl::KillFocus( _hNewFocus );
		return LSW_H_CONTINUE;
	}

	/**
	 * Notification for when a child tab changes its selection.
	 * 
	 * \param param _pwTab Pointer to the tab control.
	 * \param _iTab Zero-based tab index that was selected.
	 * \param _lpnHdr Contains information about the notification message.
	 */
	void CDeusHexMachinaWindow::TabSelChanged( CWidget * _pwTab, int _iTab, LPNMHDR _lpnHdr ) {
		static_cast<void>(_pwTab);
		static_cast<void>(_iTab);
		static_cast<void>(_lpnHdr);
		LSW_SETREDRAW srRedraw( StatusBar() );
		SetStatusBarText( L"", false, 0 );
		UpdateStatusBar_Endian();
		UpdateStatusBar_CharSet();
		UpdateStatusBar_ViewType();
		UpdateStatusBar_Size();
		UpdateStatusBar_PosValue_StartSize();
		UpdateStatusBar_InsertOverwrite();
		UpdateToolBar();
	}

	/**
	 * The WM_KEYDOWN handler.
	 *
	 * \param _uiKeyCode The virtual-key code of the nonsystem key.
	 * \param _uiFlags The repeat count, scan code, extended-key flag, context code, previous key-state flag, and transition-state flag.
	 * \return Returns an LSW_HANDLED code.
	 */
	CWidget::LSW_HANDLED CDeusHexMachinaWindow::KeyDown( UINT _uiKeyCode, UINT _uiFlags ) {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return LSW_H_CONTINUE; }
		return phecControl->KeyDown( _uiKeyCode, _uiFlags );
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
				//delete m_vTabs[_iTab].pheiInterface;
				// Clipboards that refer to the data are rendered or released, so that the file is really closed.
				CCopyPasteManager::ReleaseInterface( m_vTabs[_iTab].pheiInterface.get() );
				// Find results and a locked Find range of the closed data are dropped.
				m_frFindResults.RemoveSource( m_vTabs[_iTab].pheiInterface.get() );
				if ( m_pheiFindLock == m_vTabs[_iTab].pheiInterface.get() ) {
					m_pheiFindLock = nullptr;
					m_fpFindParms.bLockToSelection = false;
				}
				m_vTabs.erase( m_vTabs.begin() + _iTab );
			}
			auto ptTab = Tab();
			if ( ptTab ) { TabSelChanged( ptTab, ptTab->GetCurSel(), nullptr ); }
		}
	}

	/**
	 * Creates a new, untitled file in a new tab.
	 *
	 * \param _nfType The kind of file.
	 * \return Returns true if the file was created.
	 **/
	bool CDeusHexMachinaWindow::NewFile( MX_NEW_FILE _nfType ) {
		struct MX_NEW_TYPE {
			MX_NEW_FILE								nfType;
			CHexEditorControl::MX_EDIT_AS			eaEditAs;
			bool									bSetCharSet;
			CCharSets::MX_CHAR_SETS					csCharSet;
			const char *							pcExt;					// Encrypted.
			size_t									sExtLen;
		};
		static const MX_NEW_TYPE ntTypes[] = {
			{ MX_NF_TEXT,		CHexEditorControl::MX_EA_TEXT,		false,	CCharSets::MX_CS_ASCII,		_T_LEN_82DF8146__txt },
			{ MX_NF_HEX,		CHexEditorControl::MX_EA_HEX,		false,	CCharSets::MX_CS_ASCII,		_T_LEN_34CF84EE__bin },
			{ MX_NF_UNICODE,	CHexEditorControl::MX_EA_UTF16,		true,	CCharSets::MX_CS_UNICODE,	_T_LEN_82DF8146__txt },
			{ MX_NF_UTF8,		CHexEditorControl::MX_EA_UTF8,		true,	CCharSets::MX_CS_UTF8,		_T_LEN_82DF8146__txt },
			{ MX_NF_C,			CHexEditorControl::MX_EA_CODE,		false,	CCharSets::MX_CS_ASCII,		_T_LEN_54027AA2__cpp },
			{ MX_NF_HTML,		CHexEditorControl::MX_EA_TAGGED,	false,	CCharSets::MX_CS_ASCII,		_T_LEN_81EE4181__html },
			{ MX_NF_PHP,		CHexEditorControl::MX_EA_CODE,		false,	CCharSets::MX_CS_ASCII,		_T_LEN_C879FFD2__php },
			{ MX_NF_XML,		CHexEditorControl::MX_EA_TAGGED,	false,	CCharSets::MX_CS_ASCII,		_T_LEN_AF1C0660__xml },
		};
		const MX_NEW_TYPE * pntType = nullptr;
		for ( const auto &ntThis : ntTypes ) {
			if ( ntThis.nfType == _nfType ) { pntType = &ntThis; }
		}
		if ( !pntType ) { return false; }

		try {
			// "Untitled1.txt", "Untitled2.bin", etc.
			std::wstring wsName = _DEC_WS_503970B4_Untitled;
			wsName += std::to_wstring( ++m_ui32Untitled );
			wsName += mx::CStringDecoder::DecodeToWString( pntType->pcExt, pntType->sExtLen );

			// A UTF-16 file starts with its byte-order mark, so that it is recognized as UTF-16 when opened again.
			CHexEditorInterface::CBuffer bInitial;
			if ( _nfType == MX_NF_UNICODE ) { bInitial = { 0xFF, 0xFE }; }

			std::shared_ptr<CHexEditorInterface> spInterface;
			{
				std::unique_ptr<CHexEditorFile> upIfc( new( std::nothrow ) CHexEditorFile() );
				if ( !upIfc || !upIfc->CreateNew( wsName, bInitial ) ) {
					SetStatusBarText( _DEC_WS_A77664BF_Failed_to_save_the_file_.c_str(), true );
					return false;
				}
				spInterface = std::shared_ptr<CHexEditorInterface>( upIfc.release() );
			}
			if ( !AddTab( spInterface ) ) { return false; }

			// AddTab() selects the new tab.
			auto phecControl = CurrentEditor();
			if ( phecControl ) {
				phecControl->SetViewType( pntType->eaEditAs );
				if ( pntType->bSetCharSet ) { phecControl->SetCharacterSet( pntType->csCharSet ); }
				UpdateStatusBar_ViewType();
				UpdateStatusBar_CharSet();
			}
			m_nfLastNew = _nfType;
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Performs a Save As operation on the current file.
	 **/
	void CDeusHexMachinaWindow::SaveAs() {
		if ( CurrentFile() ) { SaveTab( CurrentTab(), true ); }
	}

	/**
	 * Performs a Save operation on the current file (Save As if it has never been saved).
	 **/
	void CDeusHexMachinaWindow::Save() {
		if ( CanSave() ) { SaveTab( CurrentTab(), false ); }
	}

	/**
	 * Saves a file.  An untitled file, or Save As, asks for a path.  A read-only file can only be saved under another name, after
	 *	which the tab edits the new file.
	 *
	 * \param _sIdx The tab.
	 * \param _bAs If true, asks for a new path (Save As).
	 * \return Returns true if the file was saved; false if it failed or was cancelled.
	 **/
	bool CDeusHexMachinaWindow::SaveTab( size_t _sIdx, bool _bAs ) {
		CHexEditorFile * phefFile = FileAt( _sIdx );
		if ( !phefFile ) { return false; }
		try {
			// Real-time edits still being collected become their Undo item first, so that the saved state is a whole one.
			if ( m_vTabs[_sIdx].phecWidget ) { m_vTabs[_sIdx].phecWidget->FlushRealTimeUndo(); }

			const bool bReadOnly = phefFile->ReadOnly();
			std::filesystem::path pDst = phefFile->Path();
			if ( _bAs || phefFile->IsUntitled() ) {
				const std::filesystem::path pSuggest = phefFile->IsUntitled() ? std::filesystem::path( static_cast<const std::wstring &>(phefFile->DisplayName()) ) : phefFile->Path();
				if ( !AskSavePath( _DEC_WS_2EB0FAD7_Save_As.c_str(), pSuggest, true, pDst ) ) { return false; }
			}

			std::error_code ecError;
			const bool bSelf = !phefFile->IsUntitled() && std::filesystem::equivalent( pDst, phefFile->Path(), ecError ) && !ecError;
			if ( bReadOnly && (bSelf || !(_bAs || phefFile->IsUntitled())) ) {
				FileError( _DEC_WS_A77664BF_Failed_to_save_the_file_, pDst, ERROR_FILE_READ_ONLY );
				return false;
			}
			if ( IsOpenInTab( pDst, _sIdx ) ) {
				lsw::CBase::MessageBoxError( Wnd(), _DEC_WS_88F67A0C_That_file_is_open_in_the_Hex_Editor___Close_it_first_.c_str() );
				return false;
			}

			DWORD dwError = ERROR_SUCCESS;
			if ( bReadOnly ) {
				// The read-only file cannot be edited, so after writing the copy the tab edits the copy.
				auto fWrite = [phefFile]( HANDLE _hFile, DWORD &_dwErr ) -> bool { return phefFile->WriteRange( _hFile, 0, phefFile->Size(), _dwErr ); };
				if ( !CLargeCoWFileWindow::WriteThenReplace( pDst, fWrite, dwError ) ) {
					FileError( _DEC_WS_A77664BF_Failed_to_save_the_file_, pDst, dwError );
					return false;
				}
				if ( !ReopenTab( _sIdx, pDst ) ) { return false; }
			}
			else {
				// Keep File Time: the file's times from before the save are put back afterwards.
				WIN32_FILE_ATTRIBUTE_DATA fadOld {};
				const bool bKeepTime = bSelf && phefFile->KeepFileTime() &&
					::GetFileAttributesExW( pDst.c_str(), GetFileExInfoStandard, &fadOld );
				if ( !phefFile->SaveTo( pDst, dwError ) ) {
					FileError( _DEC_WS_A77664BF_Failed_to_save_the_file_, pDst, dwError );
					return false;
				}
				if ( bKeepTime ) {
					HANDLE hFile = ::CreateFileW( pDst.c_str(), FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
						nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
					if ( hFile != INVALID_HANDLE_VALUE ) {
						::SetFileTime( hFile, &fadOld.ftCreationTime, nullptr, &fadOld.ftLastWriteTime );
						::CloseHandle( hFile );
					}
				}
				UpdateTabText( _sIdx );
			}

			if ( m_pmhMemHack && pDst.has_parent_path() ) {
				auto oOptions = m_pmhMemHack->Options();
				oOptions.heHexEditorOptions.wsLastOpenFileDirectory = pDst.parent_path().generic_wstring();
				m_pmhMemHack->SetOptions( oOptions );
			}
			SetStatusBarText( (_DEC_WS_678A7FB8_Saved__ + pDst.generic_wstring()).c_str(), false );
			UpdateToolBar();
			return true;
		}
		catch ( ... ) {
			SetStatusBarText( _DEC_WS_F39F91A5_Out_of_memory_.c_str(), true );
			return false;
		}
	}

	/**
	 * Writes a copy of the current file without changing which file is being edited.
	 **/
	void CDeusHexMachinaWindow::SaveACopy() {
		const size_t sIdx = CurrentTab();
		CHexEditorFile * phefFile = FileAt( sIdx );
		if ( !phefFile ) { return; }
		try {
			std::filesystem::path pDst;
			const std::filesystem::path pSuggest = phefFile->IsUntitled() ? std::filesystem::path( static_cast<const std::wstring &>(phefFile->DisplayName()) ) : phefFile->Path();
			if ( !AskSavePath( _DEC_WS_AE5470C0_Save_a_Copy.c_str(), pSuggest, true, pDst ) ) { return; }
			// A copy over the file itself is just a save.
			std::error_code ecError;
			if ( !phefFile->IsUntitled() && std::filesystem::equivalent( pDst, phefFile->Path(), ecError ) && !ecError ) {
				SaveTab( sIdx, false );
				return;
			}
			if ( IsOpenInTab( pDst, SIZE_MAX ) ) {
				lsw::CBase::MessageBoxError( Wnd(), _DEC_WS_88F67A0C_That_file_is_open_in_the_Hex_Editor___Close_it_first_.c_str() );
				return;
			}
			DWORD dwError = ERROR_SUCCESS;
			auto fWrite = [phefFile]( HANDLE _hFile, DWORD &_dwErr ) -> bool { return phefFile->WriteRange( _hFile, 0, phefFile->Size(), _dwErr ); };
			if ( !CLargeCoWFileWindow::WriteThenReplace( pDst, fWrite, dwError ) ) {
				FileError( _DEC_WS_A77664BF_Failed_to_save_the_file_, pDst, dwError );
				return;
			}
			SetStatusBarText( (_DEC_WS_678A7FB8_Saved__ + pDst.generic_wstring()).c_str(), false );
		}
		catch ( ... ) { SetStatusBarText( _DEC_WS_F39F91A5_Out_of_memory_.c_str(), true ); }
	}

	/**
	 * Writes the selected bytes (a file's or a process's) to a new file.
	 **/
	void CDeusHexMachinaWindow::SaveSelection() {
		const size_t sIdx = CurrentTab();
		auto phecControl = CurrentEditor();
		if ( !phecControl || sIdx >= m_vTabs.size() || !phecControl->HasSelection() ) { return; }
		CHexEditorInterface * pheiSource = m_vTabs[sIdx].pheiInterface.get();
		if ( !pheiSource ) { return; }
		try {
			phecControl->FlushRealTimeUndo();
			// The selection's address and size are added to the name: "name_00001000_00000200.ext".
			constexpr uint64_t ui64Group = 64ULL * 1024ULL;
			uint64_t ui64Total = 0;
			{
				std::vector<MX_SEL_RANGE> vRanges;
				for ( uint64_t I = 0; ; ++I ) {
					if ( !phecControl->GatherSelection( vRanges, ui64Group, I ) ) { throw std::bad_alloc(); }
					if ( vRanges.empty() ) { break; }
					for ( const auto &srThis : vRanges ) {
						if ( srThis.ui64Start < pheiSource->Size() ) { ui64Total += std::min( srThis.ui64Size, pheiSource->Size() - srThis.ui64Start ); }
					}
				}
			}
			std::wstring wsName;
			std::wstring wsExt;
			std::filesystem::path pDir;
			if ( CHexEditorFile * phefFile = FileAt( sIdx ) ) {
				const std::filesystem::path pName( static_cast<const std::wstring &>(phefFile->DisplayName()) );
				wsName = pName.stem().wstring();
				wsExt = pName.extension().wstring();
				if ( phefFile->Path().has_parent_path() ) { pDir = phefFile->Path().parent_path(); }
			}
			else {
				// A process: its name, without anything a file name cannot hold.
				wsName = pheiSource->TabString();
				wsExt = _DEC_WS_34CF84EE__bin;
				const CSecureWString swsBad = _DEC_WS_8A350921____________;
				for ( auto &wcThis : wsName ) {
					if ( wcThis < L' ' || swsBad.find( wcThis ) != std::wstring::npos ) { wcThis = L'_'; }
				}
				while ( wsName.size() && (wsName.back() == L' ' || wsName.back() == L'.') ) { wsName.pop_back(); }
				if ( wsName.empty() ) { wsName = _DEC_WS_EDC13103_Memory; }
			}
			wchar_t wszRange[64];
			::swprintf_s( wszRange, _DEC_WS_BE34A111____8llX___8llX.c_str(), static_cast<unsigned long long>(phecControl->SelectionStart()), static_cast<unsigned long long>(ui64Total) );
			wsName += wszRange;
			wsName += wsExt;

			std::filesystem::path pDst;
			if ( !AskSavePath( _DEC_WS_0E40133E_Save_Selection.c_str(), pDir.empty() ? std::filesystem::path( wsName ) : pDir / wsName, true, pDst ) ) { return; }
			if ( IsOpenInTab( pDst, SIZE_MAX ) ) {
				lsw::CBase::MessageBoxError( Wnd(), _DEC_WS_88F67A0C_That_file_is_open_in_the_Hex_Editor___Close_it_first_.c_str() );
				return;
			}
			DWORD dwError = ERROR_SUCCESS;
			auto fGather = [phecControl, ui64Group]( std::vector<MX_SEL_RANGE> &_vRanges, uint64_t _ui64Idx ) -> bool {
				return phecControl->GatherSelection( _vRanges, ui64Group, _ui64Idx );
			};
			if ( !WriteRangesTo( pheiSource, fGather, pDst, dwError ) ) {
				FileError( _DEC_WS_A77664BF_Failed_to_save_the_file_, pDst, dwError );
				return;
			}
			SetStatusBarText( (_DEC_WS_678A7FB8_Saved__ + pDst.generic_wstring()).c_str(), false );
		}
		catch ( ... ) { SetStatusBarText( _DEC_WS_F39F91A5_Out_of_memory_.c_str(), true ); }
	}

	/**
	 * Saves every modified file (read-only files are skipped).
	 **/
	void CDeusHexMachinaWindow::SaveAll() {
		for ( size_t I = 0; I < m_vTabs.size(); ++I ) {
			CHexEditorFile * phefFile = FileAt( I );
			if ( !phefFile || phefFile->ReadOnly() ) { continue; }
			if ( m_vTabs[I].phecWidget ) { m_vTabs[I].phecWidget->FlushRealTimeUndo(); }
			if ( !phefFile->IsModified() ) { continue; }
			// An untitled file asks for its name; cancelling it skips only that file.
			SaveTab( I, false );
		}
	}

	/**
	 * Discards the current file's unsaved changes and reloads it from disk.
	 **/
	void CDeusHexMachinaWindow::RevertRefresh() {
		const size_t sIdx = CurrentTab();
		CHexEditorFile * phefFile = FileAt( sIdx );
		if ( !phefFile || phefFile->IsUntitled() ) { return; }
		try {
			if ( m_vTabs[sIdx].phecWidget ) { m_vTabs[sIdx].phecWidget->FlushRealTimeUndo(); }
			const std::filesystem::path pPath = phefFile->Path();
			if ( phefFile->IsModified() ) {
				const std::wstring wsMsg = _DEC_WS_B2D793C8_Discard_all_unsaved_changes_and_reload_the_file_from_disk_ + _DEC_WS_2BB8AB61__r_n_r_n + pPath.generic_wstring();
				if ( ::MessageBoxW( Wnd(), wsMsg.c_str(), _DEC_WS_A303F04B_Revert_Refresh.c_str(), MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2 ) != IDYES ) { return; }
			}
			// The tab's settings stay as they were.
			const bool bReadOnlyMode = phefFile->ReadOnlyMode();
			const bool bKeepFileTime = phefFile->KeepFileTime();
			if ( ReopenTab( sIdx, pPath ) ) {
				if ( CHexEditorFile * phefNew = FileAt( sIdx ) ) {
					phefNew->SetReadOnlyMode( bReadOnlyMode );
					phefNew->SetKeepFileTime( bKeepFileTime );
					UpdateTabText( sIdx );
					UpdateStatusBar_InsertOverwrite();
				}
				SetStatusBarText( (_DEC_WS_EC777104_Reverted__ + pPath.generic_wstring()).c_str(), false );
			}
		}
		catch ( ... ) { SetStatusBarText( _DEC_WS_F39F91A5_Out_of_memory_.c_str(), true ); }
	}

	/**
	 * Renames the current file on disk.
	 **/
	void CDeusHexMachinaWindow::RenameFile() {
		const size_t sIdx = CurrentTab();
		CHexEditorFile * phefFile = FileAt( sIdx );
		if ( !phefFile || phefFile->IsUntitled() ) { return; }
		try {
			const std::filesystem::path pOld = phefFile->Path();
			std::filesystem::path pDst;
			if ( !AskSavePath( _DEC_WS_8A0B3AD4_Rename_File.c_str(), pOld, false, pDst ) ) { return; }
			std::error_code ecError;
			// Only a change of case may use the name of an existing file (the file itself).
			if ( std::filesystem::exists( pDst, ecError ) && !(std::filesystem::equivalent( pDst, pOld, ecError ) && !ecError) ) {
				lsw::CBase::MessageBoxError( Wnd(), _DEC_WS_4E7D86AC_A_file_with_that_name_already_exists_.c_str(), _DEC_WS_8A0B3AD4_Rename_File.c_str() );
				return;
			}
			DWORD dwError = ERROR_SUCCESS;
			if ( !phefFile->Rename( pDst, dwError ) ) {
				FileError( _DEC_WS_7A9C5609_Failed_to_rename_the_file_, pOld, dwError );
				return;
			}
			UpdateTabText( sIdx );
			SetStatusBarText( (_DEC_WS_77F568A5_Renamed_to__ + pDst.generic_wstring()).c_str(), false );
		}
		catch ( ... ) { SetStatusBarText( _DEC_WS_F39F91A5_Out_of_memory_.c_str(), true ); }
	}

	/**
	 * Closes the current file and permanently deletes it from disk.
	 **/
	void CDeusHexMachinaWindow::DeleteCurrentFile() {
		const size_t sIdx = CurrentTab();
		CHexEditorFile * phefFile = FileAt( sIdx );
		if ( !phefFile || phefFile->IsUntitled() ) { return; }
		try {
			const std::filesystem::path pPath = phefFile->Path();
			const std::wstring wsMsg = _DEC_WS_146D99BE_Close_the_file_and_permanently_delete_it_from_disk___This_cannot_be_undone_ + _DEC_WS_2BB8AB61__r_n_r_n + pPath.generic_wstring();
			if ( ::MessageBoxW( Wnd(), wsMsg.c_str(), _DEC_WS_94B2CB3D_Delete_File.c_str(), MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2 ) != IDYES ) { return; }

			// Nothing may keep the file open: clipboards that refer to it are rendered or released, then the tab is closed.
			CCopyPasteManager::ReleaseInterface( m_vTabs[sIdx].pheiInterface.get() );
			RemoveTab( sIdx );
			if ( !::DeleteFileW( pPath.c_str() ) ) {
				FileError( _DEC_WS_7F4C7825_Failed_to_delete_the_file_, pPath, ::GetLastError() );
				return;
			}
			SetStatusBarText( (_DEC_WS_B8E08C71_Deleted__ + pPath.generic_wstring()).c_str(), false );
		}
		catch ( ... ) { SetStatusBarText( _DEC_WS_F39F91A5_Out_of_memory_.c_str(), true ); }
	}

	/**
	 * Offers to save every modified file, as before closing them all.
	 *
	 * \return Returns false if the user cancelled (or a save failed), in which case nothing should be closed.
	 **/
	bool CDeusHexMachinaWindow::QueryCloseAll() {
		for ( size_t I = 0; I < m_vTabs.size(); ++I ) {
			if ( !QueryCloseTab( I ) ) { return false; }
		}
		return true;
	}

	/**
	 * Offers to save a tab's file if it is modified, as before closing it.
	 *
	 * \param _sIdx The tab.
	 * \return Returns false if the user cancelled (or the save failed), in which case the tab should stay open.
	 **/
	bool CDeusHexMachinaWindow::QueryCloseTab( size_t _sIdx ) {
		CHexEditorFile * phefFile = FileAt( _sIdx );
		if ( !phefFile ) { return true; }
		if ( m_vTabs[_sIdx].phecWidget ) { m_vTabs[_sIdx].phecWidget->FlushRealTimeUndo(); }
		if ( !phefFile->IsModified() ) { return true; }
		try {
			// Show the file being asked about.
			auto ptTab = Tab();
			if ( ptTab && size_t( ptTab->GetCurSel() ) != _sIdx ) {
				ptTab->SetCurSel( int( _sIdx ) );
				TabSelChanged( ptTab, int( _sIdx ), nullptr );
			}
			const std::wstring wsMsg = _DEC_WS_B42F7DCB_Save_the_changes_to_this_file_ + _DEC_WS_2BB8AB61__r_n_r_n + std::wstring( phefFile->StatusString() );
			switch ( ::MessageBoxW( Wnd(), wsMsg.c_str(), _DEC_WS_3213087F_Unsaved_Changes.c_str(), MB_YESNOCANCEL | MB_ICONWARNING ) ) {
				case IDYES : { return SaveTab( _sIdx, false ); }
				case IDNO : { return true; }
				default : { return false; }
			}
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Closes a tab without asking anything.
	 *
	 * \param _sIdx The tab.
	 **/
	void CDeusHexMachinaWindow::RemoveTab( size_t _sIdx ) {
		auto ptTab = Tab();
		if ( !ptTab || _sIdx >= m_vTabs.size() ) { return; }
		CCopyPasteManager::ReleaseInterface( m_vTabs[_sIdx].pheiInterface.get() );
		// Find results and a locked Find range of the closed data are dropped.
		m_frFindResults.RemoveSource( m_vTabs[_sIdx].pheiInterface.get() );
		if ( m_pheiFindLock == m_vTabs[_sIdx].pheiInterface.get() ) {
			m_pheiFindLock = nullptr;
			m_fpFindParms.bLockToSelection = false;
		}
		ptTab->DeleteItem( int( _sIdx ) );
		m_vTabs.erase( m_vTabs.begin() + _sIdx );
		TabSelChanged( ptTab, ptTab->GetCurSel(), nullptr );
	}

	/**
	 * Asked before a tab is closed with its close box.  Offers to save a modified file.
	 *
	 * \param _pwChild The tab control.
	 * \param _iTab The tab being closed.
	 * \return Returns false to keep the tab open.
	 */
	bool CDeusHexMachinaWindow::ChildTabClosing( CWidget * _pwChild, INT _iTab ) {
		if ( _pwChild == Tab() && _iTab >= 0 ) { return QueryCloseTab( size_t( _iTab ) ); }
		return true;
	}

	/**
	 * Gets the file being edited in a tab.
	 *
	 * \param _sIdx The tab.
	 * \return Returns the file, or nullptr if the tab does not exist or does not edit a file (a process).
	 **/
	CHexEditorFile * CDeusHexMachinaWindow::FileAt( size_t _sIdx ) {
		if ( _sIdx >= m_vTabs.size() || !m_vTabs[_sIdx].pheiInterface ) { return nullptr; }
		if ( m_vTabs[_sIdx].pheiInterface->Type() != CHexEditorInterface::MX_HET_FILE ) { return nullptr; }
		return static_cast<CHexEditorFile *>(m_vTabs[_sIdx].pheiInterface.get());
	}

	/**
	 * Gets the file being edited in the current tab.
	 *
	 * \return Returns the file, or nullptr if there is no tab or it does not edit a file (a process).
	 **/
	CHexEditorFile * CDeusHexMachinaWindow::CurrentFile() {
		return FileAt( CurrentTab() );
	}

	/**
	 * Gets the index of the current tab.
	 *
	 * \return Returns the index, or SIZE_MAX if there is no tab.
	 **/
	size_t CDeusHexMachinaWindow::CurrentTab() {
		auto ptTab = Tab();
		if ( !ptTab ) { return SIZE_MAX; }
		const int iSel = ptTab->GetCurSel();
		if ( iSel < 0 || size_t( iSel ) >= m_vTabs.size() ) { return SIZE_MAX; }
		return size_t( iSel );
	}

	/**
	 * Makes a tab edit a file freshly opened from disk in place of what it edits now (Revert/Refresh, or after saving a read-only
	 *	file under a new name).  The tab is closed if the file cannot be opened.
	 *
	 * \param _sIdx The tab.
	 * \param _pPath The file to open.
	 * \return Returns true if the file was opened.
	 **/
	bool CDeusHexMachinaWindow::ReopenTab( size_t _sIdx, const std::filesystem::path &_pPath ) {
		if ( _sIdx >= m_vTabs.size() ) { return false; }
		MX_HEX_TAB &htTab = m_vTabs[_sIdx];
		try {
			// The old file must be closed first, or the new one could only be opened read-only.  Clipboards that refer to it are
			//	rendered or released, and it is emptied (the control still refers to it until it is given the new one).
			CCopyPasteManager::ReleaseInterface( htTab.pheiInterface.get() );
			if ( htTab.pheiInterface && htTab.pheiInterface->Type() == CHexEditorInterface::MX_HET_FILE ) {
				static_cast<CHexEditorFile *>(htTab.pheiInterface.get())->Reset();
			}

			std::shared_ptr<CHexEditorInterface> spInterface;
			{
				std::unique_ptr<CHexEditorFile> upIfc( new( std::nothrow ) CHexEditorFile() );
				if ( !upIfc || !upIfc->SetFile( _pPath ) ) {
					FileError( _DEC_WS_A5900C55_Failed_to_reload_the_file_, _pPath, ::GetLastError() );
					RemoveTab( _sIdx );
					return false;
				}
				spInterface = std::shared_ptr<CHexEditorInterface>( upIfc.release() );
			}
			// The view keeps its place as far as the new data allows.
			const uint64_t ui64Caret = htTab.phecWidget ? htTab.phecWidget->SelectionStart() : 0;
			std::shared_ptr<CHexEditorInterface> spOld = std::move( htTab.pheiInterface );
			htTab.pheiInterface = spInterface;
			if ( htTab.phecWidget ) {
				htTab.phecWidget->SetStream( spInterface.get(), this );
				htTab.phecWidget->SetCaretAddr( std::min( ui64Caret, spInterface->Size() ), 0, false, true );
			}
			spOld.reset();

			UpdateTabText( _sIdx );
			if ( _sIdx == CurrentTab() ) { TabSelChanged( Tab(), int( _sIdx ), nullptr ); }
			return true;
		}
		catch ( ... ) {
			SetStatusBarText( _DEC_WS_F39F91A5_Out_of_memory_.c_str(), true );
			return false;
		}
	}

	/**
	 * Updates a tab's text from its data source.
	 *
	 * \param _sIdx The tab.
	 **/
	void CDeusHexMachinaWindow::UpdateTabText( size_t _sIdx ) {
		auto ptTab = Tab();
		if ( !ptTab || _sIdx >= m_vTabs.size() || !m_vTabs[_sIdx].pheiInterface ) { return; }
		try {
			const CSecureWString swsName = m_vTabs[_sIdx].pheiInterface->TabString();
			ptTab->SetItemTextW( int( _sIdx ), swsName.c_str() );
		}
		catch ( ... ) {}
	}

	/**
	 * Enables and disables the toolbar buttons for the current tab.
	 **/
	void CDeusHexMachinaWindow::UpdateToolBar() {
		CToolBar * ptbToolBar = static_cast<CToolBar *>(FindChild( Layout::MX_W_TOOLBAR0 ));
		if ( !ptbToolBar ) { return; }
		const bool bFile = CurrentFile() != nullptr;
		const bool bTab = CurrentTab() != SIZE_MAX;
		auto phecControl = bTab ? CurrentEditor() : nullptr;
		const bool bSel = phecControl && phecControl->HasSelection();
		// The toolbar only redraws buttons whose state changes, so this is cheap enough to call on every caret move.
		ptbToolBar->EnableButton( Layout::MX_M_FILE_SAVE, CanSave() );
		ptbToolBar->EnableButton( Layout::MX_M_FILE_SAVE_AS, bFile );
		ptbToolBar->EnableButton( Layout::MX_M_FILE_SAVE_ALL, CanSaveAll() );
		ptbToolBar->EnableButton( Layout::MX_M_FILE_CLOSE, bTab );
		// Cut removes bytes, so it also needs a writable data source.
		ptbToolBar->EnableButton( Layout::MX_M_EDIT_CUT, bSel && phecControl->CanDelete() );
		ptbToolBar->EnableButton( Layout::MX_M_EDIT_COPY, bSel );
		ptbToolBar->EnableButton( Layout::MX_M_EDIT_PASTE, phecControl && phecControl->CanPaste() );
		ptbToolBar->EnableButton( Layout::MX_M_EDIT_UNDO, phecControl && phecControl->CanUndo() );
		ptbToolBar->EnableButton( Layout::MX_M_EDIT_REDO, phecControl && phecControl->CanRedo() );
		ptbToolBar->EnableButton( Layout::MX_M_FIND_FIND, phecControl != nullptr );
		ptbToolBar->EnableButton( Layout::MX_M_FIND_GOTO, phecControl != nullptr );
		ptbToolBar->EnableButton( Layout::MX_M_TOOLS_HISTOGRAM, phecControl != nullptr );
	}

	/**
	 * Determines whether the current file can be saved under its own name (it is a file, not read-only).
	 *
	 * \return Returns true if Save is available.
	 **/
	bool CDeusHexMachinaWindow::CanSave() {
		CHexEditorFile * phefFile = CurrentFile();
		return phefFile && !phefFile->ReadOnly();
	}

	/**
	 * Determines whether any open file can be saved (Save All).
	 *
	 * \return Returns true if at least one file is not read-only.
	 **/
	bool CDeusHexMachinaWindow::CanSaveAll() {
		for ( size_t I = 0; I < m_vTabs.size(); ++I ) {
			CHexEditorFile * phefFile = FileAt( I );
			if ( phefFile && !phefFile->ReadOnly() ) { return true; }
		}
		return false;
	}

	/**
	 * Asks for the path of a file to write.
	 *
	 * \param _pwcTitle The title of the dialog.
	 * \param _pSuggested The path (or just the name) to suggest.
	 * \param _bOverwritePrompt If true, asks before an existing file is chosen.
	 * \param _pChosen Receives the chosen path.
	 * \return Returns true if a path was chosen.
	 **/
	bool CDeusHexMachinaWindow::AskSavePath( LPCWSTR _pwcTitle, const std::filesystem::path &_pSuggested, bool _bOverwritePrompt, std::filesystem::path &_pChosen ) {
		try {
			OPENFILENAMEW ofnSaveFile = { sizeof( ofnSaveFile ) };
			CSecureWString szFileName;
			szFileName.resize( 0xFFFF + 2 );
			const std::wstring wsName = _pSuggested.filename().wstring();
			std::memcpy( szFileName.data(), wsName.c_str(), std::min( wsName.size(), szFileName.size() - 2 ) * sizeof( wchar_t ) );

			std::wstring wsDir;
			if ( _pSuggested.has_parent_path() ) { wsDir = _pSuggested.parent_path().wstring(); }
			else if ( m_pmhMemHack ) { wsDir = m_pmhMemHack->Options().heHexEditorOptions.wsLastOpenFileDirectory; }

			CSecureWString wsFilter = _DEC_WS_6331FDE9_All_Files_______0____0;
			ofnSaveFile.hwndOwner = Wnd();
			ofnSaveFile.lpstrFilter = wsFilter.c_str();
			ofnSaveFile.lpstrFile = szFileName.data();
			ofnSaveFile.nMaxFile = DWORD( szFileName.size() );
			ofnSaveFile.lpstrTitle = _pwcTitle;
			ofnSaveFile.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_NOREADONLYRETURN | OFN_HIDEREADONLY | (_bOverwritePrompt ? OFN_OVERWRITEPROMPT : 0);
			ofnSaveFile.lpstrInitialDir = wsDir.size() ? wsDir.c_str() : nullptr;

			if ( !::GetSaveFileNameW( &ofnSaveFile ) ) { return false; }
			_pChosen = std::filesystem::path( ofnSaveFile.lpstrFile );
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Determines whether a path is a file open in any tab (which must not be overwritten except by saving that tab).
	 *
	 * \param _pPath The path.
	 * \param _sSkip A tab to ignore, or SIZE_MAX.
	 * \return Returns true if another tab edits the file.
	 **/
	bool CDeusHexMachinaWindow::IsOpenInTab( const std::filesystem::path &_pPath, size_t _sSkip ) {
		std::error_code ecError;
		if ( !std::filesystem::exists( _pPath, ecError ) ) { return false; }
		for ( size_t I = 0; I < m_vTabs.size(); ++I ) {
			if ( I == _sSkip ) { continue; }
			CHexEditorFile * phefFile = FileAt( I );
			if ( !phefFile || phefFile->IsUntitled() ) { continue; }
			if ( std::filesystem::equivalent( _pPath, phefFile->Path(), ecError ) && !ecError ) { return true; }
		}
		return false;
	}

	/**
	 * Writes bytes read from a data source to a file (through a temporary file that then replaces it).
	 *
	 * \param _pheiSource The data source.
	 * \param _fGather Gets the ranges to write, a group at a time (filling an empty vector after the last group), in order.  Returns
	 *	false on a memory failure.
	 * \param _pDst The file to write.
	 * \param _dwError Receives the Windows error code on failure.
	 * \return Returns true if the file was written.
	 **/
	bool CDeusHexMachinaWindow::WriteRangesTo( CHexEditorInterface * _pheiSource, const std::function<bool ( std::vector<MX_SEL_RANGE> &, uint64_t )> &_fGather,
		const std::filesystem::path &_pDst, DWORD &_dwError ) {
		if ( !_pheiSource ) {
			_dwError = ERROR_INVALID_PARAMETER;
			return false;
		}
		auto fWrite = [&]( HANDLE _hFile, DWORD &_dwErr ) -> bool {
			try {
				constexpr uint64_t ui64Chunk = 8ULL * 1024ULL * 1024ULL;
				const uint64_t ui64Size = _pheiSource->Size();
				std::vector<MX_SEL_RANGE> vRanges;
				CHexEditorInterface::CBuffer bBuffer;
				for ( uint64_t G = 0; ; ++G ) {
					if ( !_fGather( vRanges, G ) ) {
						_dwErr = ERROR_NOT_ENOUGH_MEMORY;
						return false;
					}
					if ( vRanges.empty() ) { return true; }
					for ( const auto &srThis : vRanges ) {
						// Column selections can run past the end of the data.
						if ( srThis.ui64Start >= ui64Size ) { continue; }
						const uint64_t ui64End = srThis.ui64Start + std::min( srThis.ui64Size, ui64Size - srThis.ui64Start );
						for ( uint64_t A = srThis.ui64Start; A < ui64End; ) {
							const size_t sThis = size_t( std::min( ui64End - A, ui64Chunk ) );
							if ( !_pheiSource->Read( A, bBuffer, sThis ) || bBuffer.size() < sThis ) {
								_dwErr = ERROR_READ_FAULT;
								return false;
							}
							if ( !CLargeCoWFileWindow::WriteAll( _hFile, bBuffer.data(), sThis, _dwErr ) ) { return false; }
							A += sThis;
						}
					}
				}
			}
			catch ( ... ) {
				_dwErr = ERROR_NOT_ENOUGH_MEMORY;
				return false;
			}
		};
		return CLargeCoWFileWindow::WriteThenReplace( _pDst, fWrite, _dwError );
	}

	/**
	 * Shows an error about a file.
	 *
	 * \param _swsMsg What failed.
	 * \param _pPath The file.
	 * \param _dwError The Windows error code.
	 **/
	void CDeusHexMachinaWindow::FileError( const CSecureWString &_swsMsg, const std::filesystem::path &_pPath, DWORD _dwError ) {
		try {
			std::wstring wsText;
			lsw::CBase::AppendError( (std::wstring( _swsMsg ) + _DEC_WS_2BB8AB61__r_n_r_n + _pPath.generic_wstring()).c_str(), wsText, _dwError );
			lsw::CBase::MessageBoxError( Wnd(), wsText.c_str() );
			SetStatusBarText( _swsMsg.c_str(), true );
		}
		catch ( ... ) {}
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
			//auto wsTabname = _pPath.filename().generic_wstring();

			/*CHexEditorInterface * pheiInterface = new( std::nothrow ) CHexEditorFile();
			if ( !pheiInterface || !static_cast<CHexEditorFile*>(pheiInterface)->SetFile( _pPath ) ) {
				delete pheiInterface;
				return;
			}*/
			{
				std::error_code ecError;
				for ( size_t I = 0; I < m_vTabs.size(); ++I ) {
					CHexEditorFile * phefFile = FileAt( I );
					if ( !phefFile || phefFile->IsUntitled() ) { continue; }
					if ( std::filesystem::equivalent( _pPath, phefFile->Path(), ecError ) && !ecError ) {
						auto ptTab = Tab();
						if ( ptTab ) {
							ptTab->SetCurSel( int( I ) );
							TabSelChanged( ptTab, int( I ), nullptr );
						}
						return;
					}
				}
			}

			std::shared_ptr<CHexEditorInterface> spInterface;
			DWORD dwWriteError = ERROR_SUCCESS;
			{
				std::unique_ptr<CHexEditorFile> upIfc( new( std::nothrow ) CHexEditorFile() );
				if ( !upIfc || !upIfc->SetFile( _pPath ) ) { return; }
				dwWriteError = upIfc->WriteOpenError();

				// Transfer ownership into shared_ptr<base>.
				spInterface = std::shared_ptr<CHexEditorInterface>( upIfc.release() );
			}

			if ( AddTab( spInterface ) && dwWriteError != ERROR_SUCCESS ) {
				// Say why the file is read-only.
				switch ( dwWriteError ) {
					case ERROR_SHARING_VIOLATION : {}					MX_FALLTHROUGH
					case ERROR_LOCK_VIOLATION : {
						SetStatusBarText( _DEC_WS_17C5E241_Opened_read_only__the_file_is_in_use_by_another_program_.c_str(), true );
						break;
					}
					case ERROR_ACCESS_DENIED : {}						MX_FALLTHROUGH
					case ERROR_WRITE_PROTECT : {
						SetStatusBarText( _DEC_WS_A963AFA9_Opened_read_only__write_access_to_the_file_was_denied_.c_str(), true );
						break;
					}
					default : {
						SetStatusBarText( _DEC_WS_C800688C_Opened_read_only__the_file_could_not_be_opened_for_writing_.c_str(), true );
					}
				}
			}
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
				//CHexEditorInterface * pheiInterface = new( std::nothrow ) CHexEditorProcess();
				//if ( !pheiInterface /*|| !static_cast<CHexEditorProcess*>(pheiInterface)->SetFile( _pPath )*/ ) {
				//	return false;
				//}
				std::shared_ptr<CHexEditorInterface> spInterface;
				{
					std::unique_ptr<CHexEditorInterface> upIfc( new( std::nothrow ) CHexEditorProcess() );
					if ( !upIfc ) { return false; }

					// Transfer ownership into shared_ptr<base>.
					spInterface = std::shared_ptr<CHexEditorInterface>( upIfc.release() );
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
				for ( size_t I = 0; I < std::size( dwAttempts ); ++I ) {
					if ( static_cast<CHexEditorProcess *>(spInterface.get())->SetProcess( dwId, CProcess::MX_OPM_FIXED, dwAttempts[I] ) ) {
						break;
					}
				}
				if ( !static_cast<CHexEditorProcess *>(spInterface.get())->Process().ProcIsOpened() ) {
					//delete pheiInterface;
					return false;
				}

				return AddTab( spInterface );
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
			/*CHexEditorInterface * pheiInterface = new( std::nothrow ) CHexEditorCurProcess( m_pmhMemHack->Process() );
			if ( !pheiInterface ) {
				return false;
			}*/
			std::shared_ptr<CHexEditorInterface> spInterface;
			{
				std::unique_ptr<CHexEditorInterface> upIfc( new( std::nothrow ) CHexEditorCurProcess( m_pmhMemHack->Process() ) );
				if ( !upIfc ) { return false; }

				// Transfer ownership into shared_ptr<base>.
				spInterface = std::shared_ptr<CHexEditorInterface>( upIfc.release() );
				spInterface->SetDefaultAddress( _ui64StartAddress );
			}
			if ( !static_cast<CHexEditorCurProcess *>(spInterface.get())->Process().ProcIsOpened() ) {
				//delete pheiInterface;
				return false;
			}

			return AddTab( spInterface );
		}
		catch ( ... ) {}
		return false;
	}

	// Closes the active tab.
	void CDeusHexMachinaWindow::CloseTab() {
		const size_t sIdx = CurrentTab();
		if ( sIdx == SIZE_MAX ) { return; }
		// Modified files are offered to be saved first; cancelling keeps the tab open.
		if ( !QueryCloseTab( sIdx ) ) { return; }
		RemoveTab( sIdx );
	}

	// Close all tabs.
	void CDeusHexMachinaWindow::CloseAllTabs() {
		if ( !QueryCloseAll() ) { return; }
		while ( m_vTabs.size() ) {
			RemoveTab( m_vTabs.size() - 1 );
		}
	}

	// Performs an Undo.
	void CDeusHexMachinaWindow::Undo() {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }
		CSecureWString wsMsg;
		bool bWarning = !phecControl->Undo( wsMsg );
		LSW_SETREDRAW srRedraw( StatusBar() );
		SetStatusBarText( wsMsg.c_str(), bWarning );
		UpdateStatusBar_PosValue_StartSize();
		UpdateStatusBar_Size();
	}

	// Performs a Redo.
	void CDeusHexMachinaWindow::Redo() {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }
		CSecureWString wsMsg;
		bool bWarning = !phecControl->Redo( wsMsg );
		LSW_SETREDRAW srRedraw( StatusBar() );
		SetStatusBarText( wsMsg.c_str(), bWarning );
		UpdateStatusBar_PosValue_StartSize();
		UpdateStatusBar_Size();
	}

	// Performs a Cut.
	void CDeusHexMachinaWindow::Cut() {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }
		CSecureWString wsMsg;
		bool bWarning = !phecControl->Cut( wsMsg );
		LSW_SETREDRAW srRedraw( StatusBar() );
		SetStatusBarText( wsMsg.c_str(), bWarning );
		UpdateStatusBar_PosValue_StartSize();
		UpdateStatusBar_Size();
	}

	// Performs a Copy As.
	void CDeusHexMachinaWindow::Copy( MX_COPY_AS _caFormat ) {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }
		CSecureWString wsMsg;
		bool bWarning = !phecControl->CopyAs( _caFormat, wsMsg );
		SetStatusBarText( wsMsg.c_str(), bWarning );
	}

	/**
	 * Performs a Paste from the active clipboard into the current editor.
	 **/
	void CDeusHexMachinaWindow::Paste() {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }
		CSecureWString wsMsg;
		bool bWarning = !phecControl->Paste( wsMsg );
		LSW_SETREDRAW srRedraw( StatusBar() );
		SetStatusBarText( wsMsg.c_str(), bWarning );
		UpdateStatusBar_PosValue_StartSize();
		UpdateStatusBar_Size();
	}

	/**
	 * Makes a clipboard the active clipboard for Cut, Copy, and Paste.
	 * 
	 * \param _sIdx The clipboard index (0 is the Windows clipboard).
	 **/
	void CDeusHexMachinaWindow::SelectClipboard( size_t _sIdx ) {
		if ( !CCopyPasteManager::SetActiveClipboard( _sIdx ) ) { return; }
		UpdateStatusBar_Clipboard();
		try {
			CSecureWString swsMsg = _DEC_WS_BB6B1B5E_Active_clipboard__;
			if ( _sIdx == CCopyPasteManager::WindowsClipboard ) {
				swsMsg += _DEC_WS_C783AD2B_Windows_Clipboard;
			}
			else {
				swsMsg += _DEC_WS_BFD22120_Clipboard_;
				swsMsg += std::to_wstring( _sIdx );
			}
			SetStatusBarText( swsMsg.c_str(), false );
		}
		catch ( ... ) {}
	}

	/**
	 * Empties all clipboards (the Windows clipboard only if it holds this application's data).
	 **/
	void CDeusHexMachinaWindow::ClearClipboards() {
		CCopyPasteManager::ClearAll();
		SetStatusBarText( _DEC_WS_82513983_Cleared_all_clipboards_.c_str(), false );
	}

	// Delete the selection.
	void CDeusHexMachinaWindow::DeleteSelectedOrCaret() {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }
		CSecureWString wsMsg;
		bool bWarning = !phecControl->DeleteSelectedOrCaret( wsMsg );
		LSW_SETREDRAW srRedraw( StatusBar() );
		SetStatusBarText( wsMsg.c_str(), bWarning );
		UpdateStatusBar_PosValue_StartSize();
		UpdateStatusBar_Size();
	}

	// Deletes the character prior to the caret (backspace).
	void CDeusHexMachinaWindow::DeletePriorToCaret() {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }
		CSecureWString wsMsg;
		bool bWarning = !phecControl->DeletePriorToCaret( wsMsg );
		LSW_SETREDRAW srRedraw( StatusBar() );
		SetStatusBarText( wsMsg.c_str(), bWarning );
		UpdateStatusBar_PosValue_StartSize();
		UpdateStatusBar_Size();
	}

	// Select all.
	void CDeusHexMachinaWindow::SelectAll() {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }
		phecControl->SelectAll();
	}

	// Select word.
	void CDeusHexMachinaWindow::SelectWord() {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }
		phecControl->SelectWord();
	}

	// Select line.
	void CDeusHexMachinaWindow::SelectLine() {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }
		phecControl->SelectLine();
	}

	// Reverse the selection.
	void CDeusHexMachinaWindow::ReverseSelection() {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }
		phecControl->ReverseSelection();
	}

	// Sets the caret to the beginning of the selection.
	void CDeusHexMachinaWindow::MarkSelectionStart() {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }
		CSecureWString wsMsg;
		bool bWarning = !phecControl->MarkSelectionStart( wsMsg );
		SetStatusBarText( wsMsg.c_str(), bWarning );
	}

	// Sets the caret to the end of the selection.
	void CDeusHexMachinaWindow::MarkSelectionEnd() {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }
		CSecureWString wsMsg;
		bool bWarning = !phecControl->MarkSelectionEnd( wsMsg );
		SetStatusBarText( wsMsg.c_str(), bWarning );
	}

	// Moves the selection back.
	void CDeusHexMachinaWindow::MoveSelectionBack() {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }
		CSecureWString wsMsg;
		bool bWarning = !phecControl->MoveSelectionBack( wsMsg );
		SetStatusBarText( wsMsg.c_str(), bWarning );
	}

	// Moves the selection forward.
	void CDeusHexMachinaWindow::MoveSelectionForward() {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }
		CSecureWString wsMsg;
		bool bWarning = !phecControl->MoveSelectionForward( wsMsg );
		SetStatusBarText( wsMsg.c_str(), bWarning );
	}

	// Toggle column mode.
	void CDeusHexMachinaWindow::ToggleColumnSelection() {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }
		phecControl->SetSelectionIsColumn( !phecControl->SelectionIsColumn() );
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

	/**
	 * Shows the Mini Map Options dialog for the current hex editor.
	 **/
	void CDeusHexMachinaWindow::ShowMiniMapOptions() {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }
		phecControl->FlushRealTimeUndo();

		CMiniMapOptionsLayout::MX_MINI_MAP_OPTIONS_PARMS mmopParms;
		mmopParms.mmOptions = phecControl->GetMiniMapOptions();
		mmopParms.phecControl = phecControl;
		if ( TRUE == CMiniMapOptionsLayout::CreateMiniMapOptionsDialog( this, mmopParms ) ) {
			phecControl->SetMiniMapOptions( mmopParms.mmOptions );
			if ( mmopParms.bHide ) {
				phecControl->SetShowMiniMap( false );
			}
		}
		::SetFocus( phecControl->Wnd() );
	}

	/**
	 * Shows the Select Range dialog and selects the range entered.
	 **/
	void CDeusHexMachinaWindow::ShowSelectRange() {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }
		phecControl->FlushRealTimeUndo();

		CSelectRangeLayout::MX_SELECT_RANGE_PARMS srpParms;
		srpParms.ui64DataSize = phecControl->Size();
		srpParms.ui64Bias = phecControl->StartingAddress();
		srpParms.bHex = phecControl->GetAddressGutterFmt() == CHexEditorControl::MX_AF_BYTES_HEX ||
			phecControl->GetAddressGutterFmt() == CHexEditorControl::MX_AF_SHORT_HEX;
		srpParms.bEnd = m_bSelectRangeEnd;
		// The current selection (or the caret) is the default.
		bool bHaveSel = false;
		if ( phecControl->HasSelection() ) {
			// The whole span of the selection (a column selection is selected again as a normal one).
			try {
				std::vector<MX_SEL_RANGE> vRanges;
				uint64_t ui64Start = UINT64_MAX, ui64End = 0;
				for ( uint64_t I = 0; phecControl->GatherSelection( vRanges, 1024ULL * 1024ULL, I ) && vRanges.size(); ++I ) {
					for ( const auto &srThis : vRanges ) {
						ui64Start = std::min( ui64Start, srThis.ui64Start );
						ui64End = std::max( ui64End, srThis.ui64Start + srThis.ui64Size );
					}
				}
				if ( ui64Start < ui64End ) {
					srpParms.ui64Start = ui64Start;
					srpParms.ui64Size = ui64End - ui64Start;
					bHaveSel = true;
				}
			}
			catch ( ... ) {}
		}
		if ( !bHaveSel ) {
			srpParms.ui64Start = phecControl->GetCaretAddr();
			srpParms.ui64Size = 0;
		}
		// Shown (and entered) with the custom starting address added.
		srpParms.ui64Start += srpParms.ui64Bias;

		if ( TRUE == CSelectRangeLayout::CreateSelectRangeDialog( this, srpParms ) ) {
			m_bSelectRangeEnd = srpParms.bEnd;
			phecControl->SelectRange( srpParms.ui64Start - srpParms.ui64Bias, srpParms.ui64Size );
			UpdateStatusBar_PosValue_StartSize();
		}
	}

	/**
	 * Shows the Find dialog, which searches the current tab while it is open.
	 **/
	void CDeusHexMachinaWindow::ShowFind() {
		auto phecControl = CurrentEditor();
		const size_t sTab = CurrentTab();
		if ( !phecControl || sTab >= m_vTabs.size() || !m_vTabs[sTab].pheiInterface ) { return; }
		try {
			phecControl->FlushRealTimeUndo();
			const CHexEditorInterface * pheiSource = m_vTabs[sTab].pheiInterface.get();
			// A locked range belongs to the data it was made in.
			if ( m_pheiFindLock != pheiSource ) {
				m_pheiFindLock = nullptr;
				m_fpFindParms.bLockToSelection = false;
			}
			m_ui64FindAnchor = phecControl->GetSelctStartOrCaret();

			CFindLayout::MX_FIND_DIALOG_PARMS fdpParms;
			fdpParms.pfpParms = &m_fpFindParms;
			fdpParms.pvHistory = &m_vFindHistory;
			fdpParms.bHasSelection = phecControl->HasSelection() && !phecControl->SelectionIsColumn();
			fdpParms.ui64SelStart = phecControl->GetSelctStartOrCaret();
			fdpParms.ui64SelSize = fdpParms.bHasSelection ? phecControl->GetTotalSelectedBytes() : 0;
			fdpParms.fAction = [this, pheiSource]( CFindLayout::MX_FIND_ACTION _faAction, CSecureWString &_swsStatus ) {
				m_pheiFindLock = m_fpFindParms.bLockToSelection ? pheiSource : nullptr;
				const bool bFound = DoFind( m_fpFindParms, _faAction, _swsStatus );
				UpdateStatusBar_PosValue_StartSize();
				return bFound;
			};
			CFindLayout::CreateFindDialog( this, fdpParms );
			m_pheiFindLock = m_fpFindParms.bLockToSelection ? pheiSource : nullptr;
		}
		catch ( ... ) {
			SetStatusBarText( _DEC_WS_F39F91A5_Out_of_memory_.c_str(), true );
		}
	}

	/**
	 * Finds the next or previous match of the last search (Find -> Find Next/Find Previous).  With Quick Find Next/Previous,
	 *	the selected bytes are searched for instead.
	 *
	 * \param _bUp Searches backward.
	 **/
	void CDeusHexMachinaWindow::FindAgain( bool _bUp ) {
		auto phecControl = CurrentEditor();
		const size_t sTab = CurrentTab();
		if ( !phecControl || sTab >= m_vTabs.size() || !m_vTabs[sTab].pheiInterface ) { return; }
		try {
			CHexFind::MX_FIND_PARMS fpParms = m_fpFindParms;
			const uint64_t ui64SelSize = phecControl->HasSelection() ? phecControl->GetTotalSelectedBytes() : 0;
			if ( fpParms.bQuickFind && ui64SelSize && ui64SelSize <= 1024 && !phecControl->SelectionIsColumn() ) {
				// The selected bytes are searched for (the Find dialog keeps its text).
				std::vector<uint8_t> vBytes;
				if ( m_vTabs[sTab].pheiInterface->Read( phecControl->GetSelctStartOrCaret(), vBytes, size_t( ui64SelSize ) ) && vBytes.size() ) {
					std::string sHex;
					CUtilities::BytesToHex( vBytes.data(), vBytes.size(), sHex );
					fpParms.swsQuery.clear();
					for ( char cThis : sHex ) { fpParms.swsQuery.push_back( wchar_t( cThis ) ); }
					fpParms.ftType = CHexFind::MX_FT_HEX;
					fpParms.bRegex = fpParms.bWildcards = fpParms.bTypeSpecifiers = fpParms.bLockToSelection = false;
				}
			}
			else if ( fpParms.swsQuery.empty() ) {
				ShowFind();
				return;
			}
			if ( fpParms.bLockToSelection && m_pheiFindLock != m_vTabs[sTab].pheiInterface.get() ) { fpParms.bLockToSelection = false; }

			CSecureWString swsStatus;
			const bool bFound = DoFind( fpParms, _bUp ? CFindLayout::MX_FA_PREV : CFindLayout::MX_FA_NEXT, swsStatus );
			UpdateStatusBar_PosValue_StartSize();
			SetStatusBarText( swsStatus.c_str(), !bFound );
		}
		catch ( ... ) {
			SetStatusBarText( _DEC_WS_F39F91A5_Out_of_memory_.c_str(), true );
		}
	}

	/**
	 * Searches the current tab.  Find Next/Previous selects the match; Find All stores the matches in the Find results.
	 *
	 * \param _fpParms The search.
	 * \param _faAction What to do.
	 * \param _swsStatus Receives a description of the result.
	 * \return Returns true if a match was selected (Find Next/Previous) or if the search was done (Find All, even if nothing
	 *	was found).
	 **/
	bool CDeusHexMachinaWindow::DoFind( const CHexFind::MX_FIND_PARMS &_fpParms, CFindLayout::MX_FIND_ACTION _faAction, CSecureWString &_swsStatus ) {
		auto phecControl = CurrentEditor();
		const size_t sTab = CurrentTab();
		if ( !phecControl || sTab >= m_vTabs.size() || !m_vTabs[sTab].pheiInterface ) { return false; }
		try {
			std::shared_ptr<CHexEditorInterface> spSource = m_vTabs[sTab].pheiInterface;
			phecControl->FlushRealTimeUndo();

			// The view decides the character set of Text and the byte order of numbers and Unicode.
			CHexFind::MX_FIND_PARMS fpParms = _fpParms;
			fpParms.csCharSet = phecControl->GetCharacterSet();
			fpParms.bBigEndian = phecControl->IsBigEndian();
			CHexFind hfFind;
			if ( !hfFind.Compile( fpParms, _swsStatus ) ) { return false; }

			const uint64_t ui64Size = spSource->Size();
			uint64_t ui64Start = 0, ui64End = ui64Size;
			if ( fpParms.bLockToSelection && m_pheiFindLock == spSource.get() ) {
				ui64Start = std::min( fpParms.ui64LockStart, ui64Size );
				ui64End = std::min( ui64Size - ui64Start, fpParms.ui64LockSize ) + ui64Start;
			}
			CHexEditorInterface * pheiSource = spSource.get();
			CHexFind::CReader rReader = [pheiSource]( uint64_t _ui64Addr, size_t _sSize, std::vector<uint8_t> &_vDst ) {
				return pheiSource->Read( _ui64Addr, _vDst, _sSize );
			};
			struct MX_FIND_WAIT {
				~MX_FIND_WAIT() {
					// The windows are enabled before the dialog goes, so that one of them is activated.
					for ( auto hWnd : vDisabled ) { ::EnableWindow( hWnd, TRUE ); }
					delete pwDialog;
					// The window that was active (the Find dialog, perhaps) is active again.
					if ( bPumping && hActive && ::IsWindow( hActive ) ) { ::SetActiveWindow( hActive ); }
					::SetCursor( hOldCursor );
				}

				/**
				 * Called as the search progresses.
				 *
				 * \param _dProgress How far the search has gone (0 to 1).
				 * \return Returns false to stop the search.
				 **/
				bool												Progress( double _dProgress ) {
					if ( ::GetAsyncKeyState( VK_ESCAPE ) & 0x8000 ) { bCancel = true; }
					if ( bCancel ) { return false; }
					const ULONGLONG ullNow = ::GetTickCount64();
					const ULONGLONG ullElapsed = ullNow - ullStart;
					if ( ullElapsed < 1000 ) { return true; }
					if ( !bPumping ) {
						bPumping = true;
						hActive = ::GetActiveWindow();
						const HWND hWnds[] = { ::GetAncestor( hOwner, GA_ROOTOWNER ), ::GetAncestor( hOwner, GA_ROOT ), ::GetActiveWindow() };
						for ( HWND hThis : hWnds ) {
							if ( hThis && ::IsWindowEnabled( hThis ) && std::find( vDisabled.begin(), vDisabled.end(), hThis ) == vDisabled.end() ) {
								::EnableWindow( hThis, FALSE );
								vDisabled.push_back( hThis );
							}
						}
					}
					if ( !pwDialog && ullElapsed >= 10000 ) {
						fppParms.pbCancel = &bCancel;
						pwDialog = CFindLayout::CreateFindProgress( pwOwner, fppParms );
					}
					// Messages are handled every 50 milliseconds.
					if ( ullNow - ullLastPump < 50 ) { return true; }
					ullLastPump = ullNow;
					if ( pwDialog ) { static_cast<CFindProgressDialog *>(pwDialog)->SetProgress( _dProgress ); }
					MSG mMsg;
					while ( ::PeekMessageW( &mMsg, NULL, 0, 0, PM_REMOVE ) ) {
						if ( mMsg.message == WM_QUIT ) {
							// Put back for the main loop.
							::PostQuitMessage( int( mMsg.wParam ) );
							bCancel = true;
							break;
						}
						if ( pwDialog && pwDialog->Wnd() && ::IsDialogMessageW( pwDialog->Wnd(), &mMsg ) ) { continue; }
						::TranslateMessage( &mMsg );
						::DispatchMessageW( &mMsg );
					}
					return !bCancel;
				}


				// == Members.
				CWidget *											pwOwner = nullptr;							/**< The Hex Editor window. */
				HWND												hOwner = NULL;								/**< Its window. */
				HWND												hActive = NULL;								/**< The window that was active when messages began to be handled. */
				ULONGLONG											ullStart = 0;								/**< When the search started. */
				ULONGLONG											ullLastPump = 0;							/**< When messages were last handled. */
				HCURSOR												hOldCursor = NULL;							/**< The cursor before the search. */
				bool												bCancel = false;							/**< The search is to stop. */
				bool												bPumping = false;							/**< Messages are being handled. */
				std::vector<HWND>									vDisabled;									/**< The windows disabled during the search. */
				CFindLayout::MX_FIND_PROGRESS_PARMS					fppParms;									/**< The parameters of the dialog. */
				CWidget *											pwDialog = nullptr;							/**< The dialog that can stop the search. */
			} fwWait;
			fwWait.pwOwner = this;
			fwWait.hOwner = Wnd();
			fwWait.ullStart = fwWait.ullLastPump = ::GetTickCount64();
			fwWait.hOldCursor = ::SetCursor( ::LoadCursorW( NULL, IDC_WAIT ) );
			CHexFind::CProgress pProgress = [&fwWait]( double _dProgress ) { return fwWait.Progress( _dProgress ); };

			// While messages were handled, the tab may have been closed (a process that ended); the control is found again.
			auto fControl = [&]() -> CHexEditorControl * {
				for ( const auto & htTab : m_vTabs ) {
					if ( htTab.pheiInterface == spSource ) { return htTab.phecWidget; }
				}
				return nullptr;
			};

			if ( _faAction == CFindLayout::MX_FA_ALL ) {
				CFindResults::MX_FIND_SET fsSet;
				bool bTooMany = false;
				if ( (!hfFind.FindAll( rReader, ui64Start, ui64End, fpParms.bLimit ? fpParms.ui64Limit : 0, fsSet.vMatches, bTooMany, pProgress ) && !bTooMany) ||
					fwWait.bCancel ) {
					_swsStatus = _DEC_WS_CE5EEB4C_The_search_was_stopped_;
					return false;
				}
				phecControl = fControl();
				const size_t sFound = fsSet.vMatches.size();
				const CHexFind::MX_FIND_MATCH fmFirst = sFound ? fsSet.vMatches[0] : CHexFind::MX_FIND_MATCH();
				fsSet.swsQuery = fpParms.swsQuery;
				fsSet.ftType = hfFind.Type();
				fsSet.wpSource = spSource;
				fsSet.swsSourceName = spSource->TabString();
				fsSet.ui64RangeStart = ui64Start;
				fsSet.ui64RangeEnd = ui64End;
				fsSet.bCustomColor = fpParms.bCustomColor;
				fsSet.bCustomFore = fpParms.bCustomFore;
				fsSet.crFore = fpParms.crFore;
				fsSet.crBack = fpParms.crBack;
				fsSet.bTooMany = bTooMany;
				m_frFindResults.Add( std::move( fsSet ), fpParms.bMultipleRanges );

				if ( bTooMany ) {
					_swsStatus = _DEC_WS_BBA58F10_Stopped_after_the_limit_of_ + std::to_wstring( sFound ) + _DEC_WS_FBEC4128__occurrences_;
				}
				else if ( !sFound ) { _swsStatus = _DEC_WS_01A4AED5_Not_found_; }
				else if ( sFound == 1 ) { _swsStatus = _DEC_WS_3507F809_1_occurrence_found_; }
				else { _swsStatus = std::to_wstring( sFound ) + _DEC_WS_4C8957DE__occurrences_found_; }
				SetStatusBarText( _swsStatus.c_str(), sFound == 0 );
				// The first one is selected.
				if ( sFound && phecControl ) { phecControl->SelectRange( fmFirst.ui64Addr, fmFirst.ui64Size ); }
				return true;
			}

			const bool bUp = _faAction == CFindLayout::MX_FA_PREV || (_faAction == CFindLayout::MX_FA_INCREMENTAL && fpParms.bUp);
			uint64_t ui64From;
			if ( _faAction == CFindLayout::MX_FA_INCREMENTAL ) { ui64From = m_ui64FindAnchor; }
			else if ( phecControl->HasSelection() ) {
				// Searching forward starts after the start of the selection (the last match), so the next one is found.
				ui64From = phecControl->GetSelctStartOrCaret() + (bUp ? 0 : 1);
			}
			else { ui64From = phecControl->GetCaretAddr(); }

			CHexFind::MX_FIND_MATCH fmMatch;
			bool bWrapped = false;
			const bool bFound = hfFind.FindNext( rReader, ui64Start, ui64End, ui64From, bUp, fpParms.bWrap, fmMatch, pProgress, bWrapped );
			if ( fwWait.bCancel ) {
				_swsStatus = _DEC_WS_CE5EEB4C_The_search_was_stopped_;
				return false;
			}
			if ( !bFound ) {
				_swsStatus = _DEC_WS_01A4AED5_Not_found_;
				return false;
			}
			phecControl = fControl();
			if ( !phecControl ) { return false; }
			phecControl->SelectRange( fmMatch.ui64Addr, fmMatch.ui64Size );
			const uint64_t ui64Shown = fmMatch.ui64Addr + phecControl->StartingAddress();
			std::wstring wsHex;
			_swsStatus = bWrapped ? _DEC_WS_20207F04_Found_at__wrapped__ : _DEC_WS_3B4C0E0C_Found_at_;
			_swsStatus += std::format( L"{} [{}]", ui64Shown, CUtilities::ToHex( ui64Shown, wsHex, 0 ) );
			return true;
		}
		catch ( ... ) {
			_swsStatus = _DEC_WS_F39F91A5_Out_of_memory_;
			return false;
		}
	}

	/**
	 * Shows the Hex Editor's Expression Evaluator (Tools -> Expression Evaluator), creating it if needed.  If it is docked
	 *	behind other dockables, its tab is selected.
	 **/
	void CDeusHexMachinaWindow::ShowExpEval() {
		CMultiSplitter * pmsSplitter = MultiSplitter();
		if ( !pmsSplitter ) { return; }
		if ( !m_peeExpEval ) {
			m_peeExpEval = static_cast<CExpEvalWindow *>(CExpressionEvaluatorLayout::CreateExpEvalWindow( pmsSplitter ));
			if ( !m_peeExpEval ) { return; }
			if ( m_pmhMemHack ) {
				m_peeExpEval->SetUpdateSpeed( 1000 / std::max( m_pmhMemHack->Options().dwExpressionRefresh, static_cast<DWORD>(1) ) );
			}
			m_peeExpEval->AddDockTarget( pmsSplitter );
			// [address] reads from the file or process in the current tab.
			m_peeExpEval->SetAddressHandler( ExpAddressHandler, reinterpret_cast<uintptr_t>(this) );
		}
		else {
			pmsSplitter->SelectDocked( m_peeExpEval );
			m_peeExpEval->SetVisible( TRUE );
		}
	}

	/**
	 * Informs that a child was removed from a child control (or from this one).  Forgets the Expression Evaluator when it is
	 *	closed.
	 *
	 * \param _pwChild The child that was removed.
	 **/
	void CDeusHexMachinaWindow::ChildWasRemoved( const CWidget * _pwChild ) {
		if ( m_peeExpEval && static_cast<const CWidget *>(m_peeExpEval) == _pwChild ) { m_peeExpEval = nullptr; }
		lsw::CMainWindow::ChildWasRemoved( _pwChild );
	}

	/**
	 * Reads values for the Expression Evaluator's [address] expressions from the current tab.  Addresses are those shown (they
	 *	include the starting address), and values are read in the byte order of the view.
	 *
	 * \param _ui64Address The address.
	 * \param _ctType The type to read.
	 * \param _uiptrData The CDeusHexMachinaWindow.
	 * \param _peecContainer The expression container.
	 * \param _rResult Receives the value.
	 * \return Returns true if the value was read.
	 **/
	bool __stdcall CDeusHexMachinaWindow::ExpAddressHandler( uint64_t _ui64Address, ee::EE_CAST_TYPES _ctType, uintptr_t _uiptrData,
		ee::CExpEvalContainer * /*_peecContainer*/, ee::CExpEvalContainer::EE_RESULT &_rResult ) {
		_rResult.ncType = ee::EE_NC_INVALID;
		CDeusHexMachinaWindow * pdhmwThis = reinterpret_cast<CDeusHexMachinaWindow *>(_uiptrData);
		if ( !pdhmwThis ) { return false; }
		CHexEditorControl * phecControl = pdhmwThis->CurrentEditor();
		const size_t sTab = pdhmwThis->CurrentTab();
		if ( !phecControl || sTab >= pdhmwThis->m_vTabs.size() || !pdhmwThis->m_vTabs[sTab].pheiInterface ) { return false; }

		size_t sSize;
		switch ( _ctType ) {
			case ee::EE_CT_INT8 : {}								MX_FALLTHROUGH
			case ee::EE_CT_UINT8 : { sSize = 1; break; }
			case ee::EE_CT_INT16 : {}								MX_FALLTHROUGH
			case ee::EE_CT_UINT16 : {}								MX_FALLTHROUGH
			case ee::EE_CT_FLOAT16 : { sSize = 2; break; }
			case ee::EE_CT_INT32 : {}								MX_FALLTHROUGH
			case ee::EE_CT_UINT32 : {}								MX_FALLTHROUGH
			case ee::EE_CT_FLOAT : { sSize = 4; break; }
			case ee::EE_CT_INT64 : {}								MX_FALLTHROUGH
			case ee::EE_CT_UINT64 : {}								MX_FALLTHROUGH
			case ee::EE_CT_DOUBLE : { sSize = 8; break; }
			default : { return false; }
		}
		const uint64_t ui64Bias = phecControl->StartingAddress();
		if ( _ui64Address < ui64Bias ) { return false; }
		try {
			std::vector<uint8_t> vBytes;
			if ( !pdhmwThis->m_vTabs[sTab].pheiInterface->Read( _ui64Address - ui64Bias, vBytes, sSize ) || vBytes.size() < sSize ) { return false; }
			if ( phecControl->IsBigEndian() ) { std::reverse( vBytes.begin(), vBytes.begin() + sSize ); }
			uint64_t ui64Raw = 0;
			std::memcpy( &ui64Raw, vBytes.data(), sSize );
			switch ( _ctType ) {
				case ee::EE_CT_INT8 : {
					_rResult.ncType = ee::EE_NC_SIGNED;
					_rResult.u.i64Val = int8_t( ui64Raw );
					return true;
				}
				case ee::EE_CT_INT16 : {
					_rResult.ncType = ee::EE_NC_SIGNED;
					_rResult.u.i64Val = int16_t( ui64Raw );
					return true;
				}
				case ee::EE_CT_INT32 : {
					_rResult.ncType = ee::EE_NC_SIGNED;
					_rResult.u.i64Val = int32_t( ui64Raw );
					return true;
				}
				case ee::EE_CT_INT64 : {
					_rResult.ncType = ee::EE_NC_SIGNED;
					_rResult.u.i64Val = int64_t( ui64Raw );
					return true;
				}
				case ee::EE_CT_UINT8 : {}							MX_FALLTHROUGH
				case ee::EE_CT_UINT16 : {}							MX_FALLTHROUGH
				case ee::EE_CT_UINT32 : {}							MX_FALLTHROUGH
				case ee::EE_CT_UINT64 : {
					_rResult.ncType = ee::EE_NC_UNSIGNED;
					_rResult.u.ui64Val = ui64Raw;
					return true;
				}
				case ee::EE_CT_FLOAT16 : {
					_rResult.ncType = ee::EE_NC_FLOATING;
					_rResult.u.dVal = ee::CFloat16( uint16_t( ui64Raw ) ).Value();
					return true;
				}
				case ee::EE_CT_FLOAT : {
					float fVal;
					std::memcpy( &fVal, &ui64Raw, sizeof( fVal ) );
					_rResult.ncType = ee::EE_NC_FLOATING;
					_rResult.u.dVal = fVal;
					return true;
				}
				case ee::EE_CT_DOUBLE : {
					_rResult.ncType = ee::EE_NC_FLOATING;
					std::memcpy( &_rResult.u.dVal, &ui64Raw, sizeof( _rResult.u.dVal ) );
					return true;
				}
				default : {}
			}
		}
		catch ( ... ) {}
		_rResult.ncType = ee::EE_NC_INVALID;
		return false;
	}

	/**
	 * Shows the Set Starting Address dialog and sets the address shown for the first byte of the current tab.
	 **/
	void CDeusHexMachinaWindow::ShowSetStartingAddress() {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }
		try {
			CNumberInputLayout::MX_NUMBER_DIALOG_PARMS ndpParms;
			ndpParms.swsHeaderText = _DEC_WS_37FCFE22_Set_Starting_Address;
			ndpParms.swsLabelText = _DEC_WS_598BFEA5_Starting__Address_;
			ndpParms.bShowHexDec = true;
			ndpParms.bHexInput = phecControl->GetAddressGutterFmt() != CHexEditorControl::MX_AF_BYTES_DEC &&
				phecControl->GetAddressGutterFmt() != CHexEditorControl::MX_AF_SHORT_DEC &&
				phecControl->GetAddressGutterFmt() != CHexEditorControl::MX_AF_LINE_NUMBER;
			ndpParms.pfVerifyFunc = &CDeusHexMachinaWindow::StartingAddress_Verify;
			ndpParms.pvVerifyParm = this;
			// The current starting address, written the way it will be read.
			const uint64_t ui64Current = phecControl->StartingAddress();
			if ( ndpParms.bHexInput ) {
				const char * pcHex = CUtilities::ToHex( ui64Current, 1, false );
				for ( const char * pcThis = pcHex; pcThis && (*pcThis); ++pcThis ) { ndpParms.swsDefaultText.push_back( wchar_t( (*pcThis) ) ); }
			}
			else {
				ndpParms.swsDefaultText = std::to_wstring( ui64Current );
			}

			if ( TRUE == CNumberInputLayout::CreateNumberInputDialog( this, ndpParms ) ) {
				const auto rCast = ee::CExpEvalContainer::ConvertResult( ndpParms.rResult, ee::EE_NC_UNSIGNED );
				phecControl->SetStartingAddress( rCast.u.ui64Val );
				UpdateStatusBar_PosValue_StartSize();
			}
		}
		catch ( ... ) {}
	}

	/**
	 * Shows the Export Hex dialog and writes the current file, process, or selection to a text file in the format chosen.
	 **/
	void CDeusHexMachinaWindow::ExportHex() {
		const size_t sIdx = CurrentTab();
		auto phecControl = CurrentEditor();
		if ( sIdx == SIZE_MAX || !phecControl || !m_vTabs[sIdx].pheiInterface ) { return; }
		CHexEditorInterface * pheiSource = m_vTabs[sIdx].pheiInterface.get();
		try {
			phecControl->FlushRealTimeUndo();

			// The settings last used, with this tab's name, file, and selection.
			CExportHexLayout::MX_EXPORT_HEX_PARMS ehpParms = m_ehpLastExport;
			size_t sTypes = 0;
			const CExportHexLayout::MX_EXPORT_TYPE * petTypes = CExportHexLayout::Types( sTypes );
			if ( ehpParms.ui32Type >= sTypes ) { ehpParms.ui32Type = 0; }
			ehpParms.bHasSelection = phecControl->HasSelection();
			ehpParms.bSelection = ehpParms.bHasSelection;
			ehpParms.bHexInput = phecControl->GetAddressGutterFmt() == CHexEditorControl::MX_AF_BYTES_HEX ||
				phecControl->GetAddressGutterFmt() == CHexEditorControl::MX_AF_SHORT_HEX;

			std::filesystem::path pDefault;
			if ( CHexEditorFile * phefFile = FileAt( sIdx ) ) {
				ehpParms.wsSourceName = phefFile->IsUntitled() ? std::wstring( phefFile->DisplayName() ) : phefFile->Path().wstring();
				pDefault = phefFile->IsUntitled() ? std::filesystem::path( static_cast<const std::wstring &>(phefFile->DisplayName()) ) : phefFile->Path();
			}
			else {
				// A process: its name, without anything a file name cannot hold.
				std::wstring wsName = pheiSource->TabString();
				ehpParms.wsSourceName = wsName;
				const CSecureWString swsBad = _DEC_WS_8A350921____________;
				for ( auto &wcThis : wsName ) {
					if ( wcThis < L' ' || swsBad.find( wcThis ) != std::wstring::npos ) { wcThis = L'_'; }
				}
				if ( wsName.empty() ) { wsName = _DEC_WS_EDC13103_Memory; }
				pDefault = std::filesystem::path( wsName );
			}
			if ( !pDefault.has_parent_path() && m_pmhMemHack ) {
				const std::wstring wsLastDir = m_pmhMemHack->Options().heHexEditorOptions.wsLastOpenFileDirectory;
				if ( wsLastDir.size() ) { pDefault = std::filesystem::path( wsLastDir ) / pDefault; }
			}
			pDefault.replace_extension( std::filesystem::path( static_cast<const std::wstring &>(mx::CStringDecoder::DecodeToWString( petTypes[ehpParms.ui32Type].pcExt, petTypes[ehpParms.ui32Type].sExtLen )) ) );
			ehpParms.wsPath = pDefault.wstring();

			if ( TRUE != CExportHexLayout::CreateExportHexDialog( this, ehpParms ) ) { return; }
			m_ehpLastExport = ehpParms;
			m_ehpLastExport.wsPath.clear();

			const std::filesystem::path pDst( ehpParms.wsPath );
			if ( IsOpenInTab( pDst, SIZE_MAX ) ) {
				lsw::CBase::MessageBoxError( Wnd(), _DEC_WS_88F67A0C_That_file_is_open_in_the_Hex_Editor___Close_it_first_.c_str() );
				return;
			}

			// What to export.
			const MX_COPY_AS caFmt = petTypes[ehpParms.ui32Type].caFormat;
			CCopyFormatter::MX_COPY_SOURCE csSource;
			csSource.fRead = [pheiSource]( uint64_t _ui64Addr, std::vector<uint8_t> &_vDst, size_t _sSize ) {
				return pheiSource->Read( _ui64Addr, _vDst, _sSize );
			};
			csSource.ui64Size = pheiSource->Size();
			csSource.ui32BytesPerRow = ehpParms.ui32BytesPerRow;
			if ( ehpParms.bSelection && phecControl->HasSelection() ) {
				csSource.sSelection = phecControl->Selection();
				// A column selection's lines are rows of the view.
				if ( csSource.sSelection.smMode != MX_SM_NORMAL ) { csSource.ui32BytesPerRow = phecControl->BytesPerRow(); }
			}
			else {
				if ( !csSource.ui64Size ) { return; }
				csSource.sSelection.bHas = true;
				csSource.sSelection.smMode = MX_SM_NORMAL;
				csSource.sSelection.sn.ui64Start = 0;
				csSource.sSelection.sn.ui64End = csSource.ui64Size - 1;
			}
			csSource.caCopyAs = phecControl->CopyParms( caFmt );
			csSource.caCopyAs.wsName = std::filesystem::path( ehpParms.wsSourceName ).filename().wstring();
			csSource.caCopyAs.bByteTextSpaces = ehpParms.bSpaces;
			csSource.caCopyAs.bByteTextLineFeeds = ehpParms.bLineFeeds;
			if ( CExportHexLayout::IsRecordFormat( caFmt ) ) {
				csSource.caCopyAs.bWordAddresses = ehpParms.bWords;
				// The first exported byte is at 0, at its own (displayed) address, or at the custom address.  Addresses wrap like
				//	the records' own address arithmetic.
				const uint64_t ui64First = CCopyFormatter::TotalRanges( csSource ) ? CCopyFormatter::RangeAt( csSource, 0 ).ui64Start : 0;
				switch ( ehpParms.smStart ) {
					case CExportHexLayout::MX_SM_ZERO : { csSource.caCopyAs.caAddress.ui64Bias = 0 - ui64First; break; }
					case CExportHexLayout::MX_SM_CUSTOM : { csSource.caCopyAs.caAddress.ui64Bias = ehpParms.ui64CustomStart - ui64First; break; }
					default : { csSource.caCopyAs.caAddress.ui64Bias = phecControl->StartingAddress(); }
				}
			}

			// Written to a temporary file that then replaces the destination.
			class CHandleSink : public CCopySink {
			public :
				CHandleSink( HANDLE _hFile, DWORD &_dwError ) : m_hFile( _hFile ), m_dwError( _dwError ) {}
				virtual bool											Write( const void * _pvData, size_t _sSize ) override {
					if ( !CLargeCoWFileWindow::WriteAll( m_hFile, _pvData, _sSize, m_dwError ) ) { return false; }
					m_sSize += _sSize;
					return true;
				}
				virtual size_t											Size() const override { return m_sSize; }
			protected :
				HANDLE													m_hFile;
				DWORD &													m_dwError;
				size_t													m_sSize = 0;
			};
			CSecureWString swsMsg;
			DWORD dwError = ERROR_SUCCESS;
			auto fWrite = [&]( HANDLE _hFile, DWORD &_dwErr ) -> bool {
				CHandleSink hsSink( _hFile, _dwErr );
				if ( !CCopyFormatter::RenderFile( csSource, hsSink, swsMsg ) ) {
					if ( _dwErr == ERROR_SUCCESS ) { _dwErr = ERROR_READ_FAULT; }
					return false;
				}
				return true;
			};
			HCURSOR hOld = ::SetCursor( ::LoadCursorW( NULL, IDC_WAIT ) );
			const bool bWritten = CLargeCoWFileWindow::WriteThenReplace( pDst, fWrite, dwError );
			::SetCursor( hOld );
			if ( !bWritten ) {
				FileError( _DEC_WS_9F15717B_Failed_to_export_the_file_, pDst, dwError );
				return;
			}
			if ( m_pmhMemHack && pDst.has_parent_path() ) {
				auto oOptions = m_pmhMemHack->Options();
				oOptions.heHexEditorOptions.wsLastOpenFileDirectory = pDst.parent_path().generic_wstring();
				m_pmhMemHack->SetOptions( oOptions );
			}
			SetStatusBarText( (_DEC_WS_1695187A_Exported__ + pDst.generic_wstring()).c_str(), false );
		}
		catch ( ... ) { SetStatusBarText( _DEC_WS_F39F91A5_Out_of_memory_.c_str(), true ); }
	}

	/**
	 * Asks for a file of hex text (or Intel HEX, Motorola, Base64, etc.), converts it to bytes, and opens the bytes as a new file.
	 **/
	void CDeusHexMachinaWindow::ImportHex() {
		try {
			// The file.
			OPENFILENAMEW ofnOpenFile = { sizeof( ofnOpenFile ) };
			CSecureWString szFileName;
			szFileName.resize( 0xFFFF + 2 );
			CSecureWString wsFilter = _DEC_WS_F4F6D09A_All_Supported_Types_0__txt___c___cpp___h___java___py___hex___ihx___s19___s28___s37___srec___mot___b64___uue___uu_0Hex_Text____txt__0__txt_0Decimal_Text____txt__0__txt_0Binary_Text____txt__0__txt_0Source_Code____c___cpp___h___java___py__0__c___cpp___h___java___py_0Intel_Hex____hex___ihx__0__hex___ihx_0Motorola_S_Records____s19___s28___s37___srec___mot__0__s19___s28___s37___srec___mot_0Base64____b64__0__b64_0Uuencoding____uue___uu__0__uue___uu_0All_Files_______0____0_0;
			CSecureWString swsTitle = _DEC_WS_2FF0701C_Import_Hex;
			std::wstring wsDir;
			if ( m_pmhMemHack ) { wsDir = m_pmhMemHack->Options().heHexEditorOptions.wsLastOpenFileDirectory; }
			ofnOpenFile.hwndOwner = Wnd();
			ofnOpenFile.lpstrFilter = wsFilter.c_str();
			ofnOpenFile.nFilterIndex = m_dwLastImportFilter;
			ofnOpenFile.lpstrFile = szFileName.data();
			ofnOpenFile.nMaxFile = DWORD( szFileName.size() );
			ofnOpenFile.lpstrTitle = swsTitle.c_str();
			ofnOpenFile.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
			ofnOpenFile.lpstrInitialDir = wsDir.size() ? wsDir.c_str() : nullptr;
			if ( !::GetOpenFileNameW( &ofnOpenFile ) ) { return; }
			m_dwLastImportFilter = ofnOpenFile.nFilterIndex;
			const std::filesystem::path pPath( ofnOpenFile.lpstrFile );

			// The filter chosen decides the kind of text (the first and last filters decide from the contents).
			static const CHexImport::MX_IMPORT_TYPE itTypes[] = {
				CHexImport::MX_IT_AUTO,
				CHexImport::MX_IT_AUTO,
				CHexImport::MX_IT_HEX_TEXT,
				CHexImport::MX_IT_DECIMAL_TEXT,
				CHexImport::MX_IT_BINARY_TEXT,
				CHexImport::MX_IT_SOURCE_CODE,
				CHexImport::MX_IT_INTEL_HEX,
				CHexImport::MX_IT_MOTOROLA,
				CHexImport::MX_IT_BASE64,
				CHexImport::MX_IT_UUENCODING,
			};
			const CHexImport::MX_IMPORT_TYPE itType = ofnOpenFile.nFilterIndex < std::size( itTypes ) ? itTypes[ofnOpenFile.nFilterIndex] : CHexImport::MX_IT_AUTO;

			// Read it all (text files of more than 1 gigabyte are not imported).
			std::vector<uint8_t> vText;
			{
				HANDLE hFile = ::CreateFileW( pPath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
				if ( hFile == INVALID_HANDLE_VALUE ) {
					FileError( _DEC_WS_43CE34EC_Failed_to_read_the_file_, pPath, ::GetLastError() );
					return;
				}
				LARGE_INTEGER liSize {};
				bool bOk = ::GetFileSizeEx( hFile, &liSize ) != FALSE;
				DWORD dwError = bOk ? ERROR_SUCCESS : ::GetLastError();
				if ( bOk && uint64_t( liSize.QuadPart ) > 1024ULL * 1024ULL * 1024ULL ) {
					bOk = false;
					dwError = ERROR_FILE_TOO_LARGE;
				}
				if ( bOk ) {
					vText.resize( size_t( liSize.QuadPart ) );
					size_t sDone = 0;
					while ( bOk && sDone < vText.size() ) {
						DWORD dwRead = 0;
						const DWORD dwThis = DWORD( std::min<size_t>( vText.size() - sDone, 16 * 1024 * 1024 ) );
						if ( !::ReadFile( hFile, vText.data() + sDone, dwThis, &dwRead, nullptr ) || !dwRead ) {
							bOk = false;
							dwError = ::GetLastError();
						}
						sDone += dwRead;
					}
				}
				::CloseHandle( hFile );
				if ( !bOk ) {
					FileError( _DEC_WS_43CE34EC_Failed_to_read_the_file_, pPath, dwError );
					return;
				}
			}

			CHexImport::MX_IMPORT_OPTIONS ioOptions;
			CHexImport::MX_IMPORT_RESULT irResult;
			CSecureWString swsError;
			HCURSOR hOld = ::SetCursor( ::LoadCursorW( NULL, IDC_WAIT ) );
			const bool bImported = CHexImport::Import( vText.data(), vText.size(), itType, ioOptions, irResult, swsError );
			::SetCursor( hOld );
			vText = std::vector<uint8_t>();
			if ( !bImported ) {
				lsw::CBase::MessageBoxError( Wnd(), (swsError + _DEC_WS_2BB8AB61__r_n_r_n + pPath.generic_wstring()).c_str(), swsTitle.c_str() );
				return;
			}

			// A new file named after the one imported.
			std::wstring wsName = pPath.stem().wstring();
			wsName += _DEC_WS_34CF84EE__bin;
			std::shared_ptr<CHexEditorInterface> spInterface;
			{
				std::unique_ptr<CHexEditorFile> upIfc( new( std::nothrow ) CHexEditorFile() );
				if ( !upIfc || !upIfc->CreateNew( wsName, irResult.vData ) ) {
					SetStatusBarText( _DEC_WS_F39F91A5_Out_of_memory_.c_str(), true );
					return;
				}
				spInterface = std::shared_ptr<CHexEditorInterface>( upIfc.release() );
			}
			if ( !AddTab( spInterface ) ) { return; }
			// Intel HEX and Motorola files whose leading blank area was skipped start at their first data byte.
			auto phecControl = CurrentEditor();
			if ( phecControl ) {
				phecControl->SetViewType( CHexEditorControl::MX_EA_HEX );
				if ( irResult.ui64StartAddress ) { phecControl->SetStartingAddress( irResult.ui64StartAddress ); }
				UpdateStatusBar_ViewType();
				UpdateStatusBar_PosValue_StartSize();
			}
			if ( m_pmhMemHack && pPath.has_parent_path() ) {
				auto oOptions = m_pmhMemHack->Options();
				oOptions.heHexEditorOptions.wsLastOpenFileDirectory = pPath.parent_path().generic_wstring();
				m_pmhMemHack->SetOptions( oOptions );
			}
			SetStatusBarText( (_DEC_WS_EAFEFFCA_Imported__ + pPath.generic_wstring()).c_str(), false );
		}
		catch ( ... ) { SetStatusBarText( _DEC_WS_F39F91A5_Out_of_memory_.c_str(), true ); }
	}

	/**
	 * Shows the Change Font dialog and applies the font chosen to the Hex Editor (all tabs share it).
	 **/
	void CDeusHexMachinaWindow::ShowChangeFont() {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }
		CHexEditorControl::MX_FONT_SET * pfsSet = phecControl->Font();
		if ( !pfsSet ) { return; }

		CFontDialogLayout::MX_FONT_DIALOG_PARMS fdpParms;
		fdpParms.lfFont = pfsSet->lfFontParms;
		fdpParms.i32PointSize = pfsSet->i32PointSize;
		fdpParms.ui32Dpi = m_wDpiY ? m_wDpiY : USER_DEFAULT_SCREEN_DPI;
		if ( TRUE != CFontDialogLayout::CreateFontDialog( this, fdpParms ) ) { return; }

		if ( !pfsSet->fFont.CreateFontIndirectW( &fdpParms.lfFont ) ) {
			lsw::CBase::MessageBoxError( Wnd(), _DEC_WS_1468A1DF_Internal_error_.c_str(), _DEC_WS_EE6F169E_Change_Font.c_str() );
			return;
		}
		pfsSet->lfFontParms = fdpParms.lfFont;
		pfsSet->i32PointSize = fdpParms.i32PointSize;
		CHexEditorControl::ComputeFontMetrics( (*pfsSet), WORD( fdpParms.ui32Dpi ), Wnd() );
		phecControl->RecalcAndInvalidate();
		RecalcAllBut( phecControl );
	}

	/**
	 * Gets the path of the file whose properties are shown for a tab: the file, or the executable of the process.
	 *
	 * \param _sIdx The tab.
	 * \param _pPath Receives the path.
	 * \return Returns true if the tab has such a file.
	 **/
	bool CDeusHexMachinaWindow::PropertiesPath( size_t _sIdx, std::filesystem::path &_pPath ) {
		if ( _sIdx >= m_vTabs.size() || !m_vTabs[_sIdx].pheiInterface ) { return false; }
		try {
			switch ( m_vTabs[_sIdx].pheiInterface->Type() ) {
				case CHexEditorInterface::MX_HET_FILE : {
					const CHexEditorFile * phefFile = static_cast<const CHexEditorFile *>(m_vTabs[_sIdx].pheiInterface.get());
					if ( phefFile->IsUntitled() ) { return false; }
					_pPath = phefFile->Path();
					break;
				}
				case CHexEditorInterface::MX_HET_PROCESS : {
					_pPath = static_cast<const CHexEditorProcess *>(m_vTabs[_sIdx].pheiInterface.get())->Process().QueryProcessImageName( 0 );
					break;
				}
				case CHexEditorInterface::MX_HET_CUR_PROCESS : {
					_pPath = static_cast<const CHexEditorCurProcess *>(m_vTabs[_sIdx].pheiInterface.get())->Process().QueryProcessImageName( 0 );
					break;
				}
				default : { return false; }
			}
			return !_pPath.empty();
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Shows the Windows Properties dialog of the current file (or of the executable of the current process).
	 **/
	void CDeusHexMachinaWindow::ShowProperties() {
		try {
			std::filesystem::path pPath;
			if ( !PropertiesPath( CurrentTab(), pPath ) ) { return; }
			const CSecureWString swsVerb = _DEC_WS_87C331C7_properties;
			SHELLEXECUTEINFOW seiInfo = { sizeof( seiInfo ) };
			seiInfo.fMask = SEE_MASK_INVOKEIDLIST;
			seiInfo.hwnd = Wnd();
			seiInfo.lpVerb = swsVerb.c_str();
			seiInfo.lpFile = pPath.c_str();
			seiInfo.nShow = SW_SHOWNORMAL;
			if ( !::ShellExecuteExW( &seiInfo ) ) {
				FileError( _DEC_WS_2128232B_Failed_to_show_the_properties_of_the_file_, pPath, ::GetLastError() );
			}
		}
		catch ( ... ) {}
	}

	/**
	 * Turns Read Only mode on or off for the current tab (not available if the data source itself cannot be written).
	 **/
	void CDeusHexMachinaWindow::ToggleReadOnly() {
		const size_t sIdx = CurrentTab();
		if ( sIdx == SIZE_MAX || !m_vTabs[sIdx].pheiInterface || m_vTabs[sIdx].pheiInterface->SourceReadOnly() ) { return; }
		// Anything being typed is finished (or abandoned) first.
		if ( m_vTabs[sIdx].phecWidget ) {
			m_vTabs[sIdx].phecWidget->CancelEdit();
			m_vTabs[sIdx].phecWidget->FlushRealTimeUndo();
		}
		m_vTabs[sIdx].pheiInterface->SetReadOnlyMode( !m_vTabs[sIdx].pheiInterface->ReadOnlyMode() );
		UpdateTabText( sIdx );
		UpdateStatusBar_InsertOverwrite();
		UpdateToolBar();
		if ( m_vTabs[sIdx].phecWidget ) { ::InvalidateRect( m_vTabs[sIdx].phecWidget->Wnd(), nullptr, FALSE ); }
	}

	/**
	 * Turns Keep File Time on or off for the current file.
	 **/
	void CDeusHexMachinaWindow::ToggleKeepFileTime() {
		CHexEditorFile * phefFile = CurrentFile();
		if ( !phefFile || phefFile->IsUntitled() ) { return; }
		phefFile->SetKeepFileTime( !phefFile->KeepFileTime() );
	}

	// Shows the Go To dialog.
	void CDeusHexMachinaWindow::ShowGoTo( uint64_t _ui64DefaultAddress ) {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }

		CNumberInputLayout::MX_NUMBER_DIALOG_PARMS ndpParms;
		ndpParms.swsHeaderText = _DEC_WS_3CAE39A1_Go_To___;
		ndpParms.swsLabelText = _DEC_WS_3A737A93_Go_to_Address_;
		ndpParms.bHexInput = phecControl->GetAddressGutterFmt() == CHexEditorControl::MX_AF_BYTES_HEX ||
			phecControl->GetAddressGutterFmt() == CHexEditorControl::MX_AF_SHORT_HEX;
		ndpParms.pfVerifyFunc = &CDeusHexMachinaWindow::GoTo_Verify;
		ndpParms.pvVerifyParm = this;

		{
			auto oOptions = m_pmhMemHack->Options();
			for ( size_t I = 0; I < oOptions.heHexEditorOptions.vGoToHistory.size(); ++I ) {
				ndpParms.vComboItems.push_back( CUtilities::MX_COMBO_ENTRY{ .pwcName = oOptions.heHexEditorOptions.vGoToHistory[I].c_str(), .lpParm = LPARAM( I ) } );
			}
		}

		if ( TRUE == CNumberInputLayout::CreateNumberInputDialog( this, ndpParms ) ) {
			// Addresses are entered as shown (with the custom starting address added).
			const auto rCast = ee::CExpEvalContainer::ConvertResult( ndpParms.rResult, ee::EE_NC_UNSIGNED );
			phecControl->SetCaretAddr( rCast.u.ui64Val - phecControl->StartingAddress() );
			if ( m_pmhMemHack ) {
				auto oOptions = m_pmhMemHack->Options();
				CUtilities::AddOrMove( oOptions.heHexEditorOptions.vGoToHistory, ndpParms.swsExpression );
				m_pmhMemHack->SetOptions( oOptions );
			}
		}
	}

	// Shows the Set Line SPacing dialog.
	void CDeusHexMachinaWindow::ShowSetLineSpacing() {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }

		CNumberInputLayout::MX_NUMBER_DIALOG_PARMS ndpParms;
		ndpParms.swsHeaderText = _DEC_WS_BC168EB1_Default_Hex_Line_Spacing;
		ndpParms.swsLabelText = _DEC_WS_C8FB637C_Enter_the_extra_spacing_between_each_row_of_the_editor_in_pixels_;
		ndpParms.bHexInput = false;
		{
			auto oOptions = m_pmhMemHack->Options();
			for ( size_t I = 0; I < oOptions.heHexEditorOptions.vLineSpacingHistory.size(); ++I ) {
				ndpParms.vComboItems.push_back( CUtilities::MX_COMBO_ENTRY{ .pwcName = oOptions.heHexEditorOptions.vLineSpacingHistory[I].c_str(), .lpParm = LPARAM( I ) } );
			}
		}
		ndpParms.pfVerifyFunc = &CDeusHexMachinaWindow::ConstantRange_Verify<std::numeric_limits<uint16_t>::max()>;
		ndpParms.pvVerifyParm = this;
		if ( TRUE == CNumberInputLayout::CreateNumberInputDialog( this, ndpParms ) ) {
			phecControl->SetHexLineSpacing( uint16_t( ndpParms.rResult.u.ui64Val ) );
			RecalcAllBut( phecControl );
			if ( m_pmhMemHack ) {
				auto oOptions = m_pmhMemHack->Options();
				CUtilities::AddOrMove( oOptions.heHexEditorOptions.vLineSpacingHistory, ndpParms.swsExpression );
				m_pmhMemHack->SetOptions( oOptions );
			}
		}
	}

	// Shows the Set Custom Line Width dialog.
	void CDeusHexMachinaWindow::ShowSetCustomLineWidth() {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }

		CNumberInputLayout::MX_NUMBER_DIALOG_PARMS ndpParms;
		ndpParms.swsHeaderText = _DEC_WS_6C93F6AE_Custom_Width;
		ndpParms.swsLabelText = _DEC_WS_9A033140_Enter_the_number_of_bytes_per_row_;
		ndpParms.bHexInput = false;
		{
			auto oOptions = m_pmhMemHack->Options();
			for ( size_t I = 0; I < oOptions.heHexEditorOptions.vLineWidthHistory.size(); ++I ) {
				ndpParms.vComboItems.push_back( CUtilities::MX_COMBO_ENTRY{ .pwcName = oOptions.heHexEditorOptions.vLineWidthHistory[I].c_str(), .lpParm = LPARAM( I ) } );
			}
		}
		ndpParms.pfVerifyFunc = &CDeusHexMachinaWindow::ConstantRange_Verify<std::numeric_limits<uint32_t>::max()>;
		ndpParms.pvVerifyParm = this;
		if ( TRUE == CNumberInputLayout::CreateNumberInputDialog( this, ndpParms ) ) {
			phecControl->SetLineWidth( uint32_t( ndpParms.rResult.u.ui64Val ) );
			RecalcAllBut( phecControl );
			if ( m_pmhMemHack ) {
				auto oOptions = m_pmhMemHack->Options();
				CUtilities::AddOrMove( oOptions.heHexEditorOptions.vLineWidthHistory, ndpParms.swsExpression );
				m_pmhMemHack->SetOptions( oOptions );
			}
		}
	}

	// Shows the Set Group Width dialog.
	void CDeusHexMachinaWindow::ShowSetGroupWidth() {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }

		CNumberInputLayout::MX_NUMBER_DIALOG_PARMS ndpParms;
		ndpParms.swsHeaderText = _DEC_WS_0FF4D032_Custom_Group_By;
		ndpParms.swsLabelText = _DEC_WS_2375A567_Enter_the_number_of_bytes_by_which_to_group_;
		ndpParms.bHexInput = false;
		{
			auto oOptions = m_pmhMemHack->Options();
			for ( size_t I = 0; I < oOptions.heHexEditorOptions.vGroupWidthHistory.size(); ++I ) {
				ndpParms.vComboItems.push_back( CUtilities::MX_COMBO_ENTRY{ .pwcName = oOptions.heHexEditorOptions.vGroupWidthHistory[I].c_str(), .lpParm = LPARAM( I ) } );
			}
		}
		ndpParms.pfVerifyFunc = &CDeusHexMachinaWindow::ConstantRange_Verify<std::numeric_limits<uint32_t>::max()>;
		ndpParms.pvVerifyParm = this;
		if ( TRUE == CNumberInputLayout::CreateNumberInputDialog( this, ndpParms ) ) {
			phecControl->SetGroupSize( uint32_t( ndpParms.rResult.u.ui64Val ) );
			RecalcAllBut( phecControl );
			if ( m_pmhMemHack ) {
				auto oOptions = m_pmhMemHack->Options();
				CUtilities::AddOrMove( oOptions.heHexEditorOptions.vGroupWidthHistory, ndpParms.swsExpression );
				m_pmhMemHack->SetOptions( oOptions );
			}
		}
	}

	// Shows the Set Division Lines dialog.
	void CDeusHexMachinaWindow::ShowSetDivisionLines() {
		auto phecControl = CurrentEditor();
		if ( !phecControl ) { return; }

		CNumberInputLayout::MX_NUMBER_DIALOG_PARMS ndpParms;
		ndpParms.swsHeaderText = _DEC_WS_7F6D8973_Custom_Division_Lines;
		ndpParms.swsLabelText = _DEC_WS_2B021194_Enter_the_number_of_bytes_between_division_lines_;
		ndpParms.bHexInput = false;
		{
			auto oOptions = m_pmhMemHack->Options();
			for ( size_t I = 0; I < oOptions.heHexEditorOptions.vDivisionLinesHistory.size(); ++I ) {
				ndpParms.vComboItems.push_back( CUtilities::MX_COMBO_ENTRY{ .pwcName = oOptions.heHexEditorOptions.vDivisionLinesHistory[I].c_str(), .lpParm = LPARAM( I ) } );
			}
		}
		ndpParms.pfVerifyFunc = &CDeusHexMachinaWindow::ConstantRange_Verify<std::numeric_limits<uint64_t>::max()>;
		ndpParms.pvVerifyParm = this;
		if ( TRUE == CNumberInputLayout::CreateNumberInputDialog( this, ndpParms ) ) {
			phecControl->SetDivionLines( uint64_t( ndpParms.rResult.u.ui64Val ) );
			RecalcAllBut( phecControl );
			if ( m_pmhMemHack ) {
				auto oOptions = m_pmhMemHack->Options();
				CUtilities::AddOrMove( oOptions.heHexEditorOptions.vDivisionLinesHistory, ndpParms.swsExpression );
				m_pmhMemHack->SetOptions( oOptions );
			}
		}
	}

	// Sets a status-bar item’s text and "warning" status.
	void CDeusHexMachinaWindow::SetStatusBarText( const wchar_t * _pwcText, bool _bWarning, size_t _sIdx, bool _bUpdateStore ) {
		auto psbStatus = StatusBar();
		if ( psbStatus ) {
			psbStatus->SetItemTextColors( MX_GetRgbValue( ::GetSysColor( COLOR_WINDOWTEXT ) ), MX_GetRgbValue( ::GetSysColor( COLOR_GRAYTEXT ) ), INT(_sIdx ) );
			psbStatus->SetItemBkColor( MX_GetRgbValue( _bWarning ? ::GetSysColor( COLOR_3DSHADOW ) : ::GetSysColor( COLOR_3DFACE ) ), INT(_sIdx ) );
			if ( _pwcText ) {
				psbStatus->SetTextW( INT( _sIdx ), 0, _pwcText );
			}
		}
		if ( _sIdx == 0 && _bUpdateStore ) {
			if ( _pwcText ) { m_swsLastStatusText = _pwcText; }
			else { m_swsLastStatusText.clear(); }
			m_bLastStatusTextIsWarning = _bWarning;
		}
	}

	// Update the status-bar Insert/Overwrite part.
	void CDeusHexMachinaWindow::UpdateStatusBar_InsertOverwrite() {
		auto psbStatus = StatusBar();
		if ( psbStatus ) {
			// A data source that cannot change size (a process, a read-only file) is always in Overwrite mode.
			const size_t sIdx = CurrentTab();
			const bool bOverwriteOnly = sIdx != SIZE_MAX && m_vTabs[sIdx].pheiInterface && m_vTabs[sIdx].pheiInterface->OverwriteOnly();
			psbStatus->SetTextW( MX_SB_INSERT_OVERWRITE, 0, ((m_bOverwrite || bOverwriteOnly) ? _DEC_WS_ED03D9E5_OVR : _DEC_WS_1C920D98_INS).c_str() );
		}
	}

	/**
	 * Updates the status-bar Clipboard part to show the active clipboard.
	 **/
	void CDeusHexMachinaWindow::UpdateStatusBar_Clipboard() {
		// Paste depends on the active clipboard.
		UpdateToolBar();
		auto psbStatus = StatusBar();
		if ( psbStatus ) {
			const size_t sIdx = CCopyPasteManager::ActiveClipboard();
			if ( sIdx == CCopyPasteManager::WindowsClipboard ) {
				psbStatus->SetTextW( MX_SB_CLIPBOARD, 0, L"\U0001F4CB" );
			}
			else {
				const wchar_t wcText[] = { L'\xD83D', L'\xDCCB', L' ', wchar_t( L'0' + sIdx ), L'\0' };
				psbStatus->SetTextW( MX_SB_CLIPBOARD, 0, wcText );
			}
		}
	}

	// Update the status-bar Little-Endian/Big-Endian part.
	void CDeusHexMachinaWindow::UpdateStatusBar_Endian() {
		auto phecControl = CurrentEditor();
		if ( phecControl ) {
			auto psbStatus = StatusBar();
			if ( psbStatus ) {
				psbStatus->SetTextW( MX_SB_ENDIAN, 0, phecControl->IsBigEndian() ? L"BIG" : L"LIT" );
			}
		}
	}

	// Update the status-bar Character-Set part.
	void CDeusHexMachinaWindow::UpdateStatusBar_CharSet() {
		auto phecControl = CurrentEditor();
		if ( phecControl ) {
			auto psbStatus = StatusBar();
			if ( psbStatus ) {
				psbStatus->SetTextA( MX_SB_CHAR_SET, 0, phecControl->CharSetStatusBarCode().c_str() );
			}
		}
	}

	// Update the status-bar View Type part.
	void CDeusHexMachinaWindow::UpdateStatusBar_ViewType() {
		auto phecControl = CurrentEditor();
		if ( phecControl ) {
			auto psbStatus = StatusBar();
			if ( psbStatus ) {
				psbStatus->SetTextW( MX_SB_VIEW_TYPE, 0, phecControl->GetViewTypeAsString().c_str() );
			}
		}
	}

	// Update the status-bar Size part.
	void CDeusHexMachinaWindow::UpdateStatusBar_Size() {
		// The size changes with edits, which also change Undo/Redo.
		UpdateToolBar();
		auto phecControl = CurrentEditor();
		if ( phecControl ) {
			auto psbStatus = StatusBar();
			if ( psbStatus ) {
				CSecureWString swsTmp = _DEC_WS_FBA37944_Size__;
				
				const std::locale lLoc( "" );
				swsTmp += std::format( lLoc, L"{:L}\n", phecControl->Size() );
				/*CSecureWString swsSize = std::to_wstring( phecControl->Size() );
				int32_t iCommIdx = int32_t( swsSize.size() ) - 3;
				while ( iCommIdx > 0 ) {
					swsSize.insert( swsSize.begin() + iCommIdx, L',' );
					iCommIdx -= 3;
				}
				swsTmp += swsSize;*/

				psbStatus->SetTextW( MX_SB_FILE_SIZE, 0, swsTmp.c_str() );
			}
		}
	}

	// Updates the status-bar selection ranges/current position/value.
	void CDeusHexMachinaWindow::UpdateStatusBar_PosValue_StartSize() {
		// Called when the caret, the selection, or the data under the caret changes, which is what Cut/Copy/Undo/Redo depend on.
		UpdateToolBar();
		try {
			auto phecControl = CurrentEditor();
			if ( phecControl ) {
				auto psbStatus = StatusBar();
				if ( psbStatus ) {
					std::wstring wsTmp, wsTmp2;
					uint64_t ui64Start = phecControl->GetSelctStartOrCaret();
					// A custom starting address is added to the position shown, and OFFSET says so.
					const uint64_t ui64Bias = phecControl->StartingAddress();
					if ( ui64Bias ) { wsTmp = _DEC_WS_6BCDC989_OFFSET_; }
					if ( phecControl->HasSelection() ) {
						wsTmp += _DEC_WS_37B96B54_Start__;
					}
					else {
						wsTmp += _DEC_WS_D504F73B_Pos__;
					}
					const uint64_t ui64Shown = ui64Start + ui64Bias;
					wsTmp += std::format( L"{} [{}]", ui64Shown, CUtilities::ToHex( ui64Shown, wsTmp2, 0 ) );
					psbStatus->SetTextW( MX_SB_POSITION_START, 0, wsTmp.c_str() );

					if ( phecControl->HasSelection() ) {
						wsTmp = _DEC_WS_EA6389C2_Sel__;
						auto ui64Total = phecControl->GetTotalSelectedBytes();
						wsTmp2.clear();
						wsTmp += std::format( L"{} [{}]", ui64Total, CUtilities::ToHex( ui64Total, wsTmp2, 0 ) );
						psbStatus->SetTextW( MX_SB_VALUE_SEL, 0, wsTmp.c_str() );
					}
					else {
						if ( ui64Start >= phecControl->Size() ) {
							psbStatus->SetTextW( MX_SB_VALUE_SEL, 0, L"" );
						}
						else {
							wsTmp = _DEC_WS_ADE191E5_Val__;
							std::vector<uint8_t> vReadMe;
							if ( phecControl->Read( ui64Start, vReadMe, 1 ) ) {
								wsTmp2.clear();
								wsTmp += std::format( L"{} [{}]", vReadMe[0], CUtilities::ToHex( vReadMe[0], wsTmp2, 0 ) );
								psbStatus->SetTextW( MX_SB_VALUE_SEL, 0, wsTmp.c_str() );
							}
							else {
								wsTmp += _DEC_WS_968230B3_N_A;
							}
							psbStatus->SetTextW( MX_SB_VALUE_SEL, 0, wsTmp.c_str() );
						}
					}
				}
			}
		}
		catch ( ... ) {}
	}

	/**
	 * Called when the mouse enters a tab.
	 * 
	 * \param _iIdx The tab the mouse began to hover over.
	 **/
	void CDeusHexMachinaWindow::TabMouseEnterTab( INT _iIdx ) {
		auto ptTab = Tab();
		if ( !ptTab ) { return; }
		size_t sIdx = size_t( _iIdx );
		if ( sIdx >= m_vTabs.size() ) { return; }
		try {
			if ( m_vTabs[sIdx].phecWidget ) {
				SetStatusBarText( m_vTabs[sIdx].phecWidget->StatusString().c_str(), false, 0, false );
			}
		}
		catch ( ... ) {}
	}

	/**
	 * Called when the mouse leaves a tab.
	 * 
	 * \param _iIdx INdex of the tab the mouse left.
	 **/
	void CDeusHexMachinaWindow::TabMouseLeftTab( INT _iIdx ) {
		SetStatusBarText( m_swsLastStatusText.c_str(), m_bLastStatusTextIsWarning, 0 );
	}

	// Called by the main window before the application closes.
	bool CDeusHexMachinaWindow::WillClose() {
		for ( auto I = m_vChildren.size(); I--; ) {
			if ( m_vChildren[I] ) {
				if ( m_vChildren[I]->Id() == CNumberInputLayout::MX_NI_DIALOG ) {
					::EndDialog( m_vChildren[I]->Wnd(), 0 );
					/*m_vChildren[I]->SetParent( nullptr );
					RemoveChild( m_vChildren[I] );*/
					return true;
				}
			}
		}
		return false;
	}

	// Prepares to create the window.  Creates the atom if necessary.
	void CDeusHexMachinaWindow::PrepareWindow() {
		if ( !m_aAtom ) {
			lsw::CWndClassEx wceEx;
			wceEx.SetInstance( lsw::CBase::GetThisHandle() );
			WCHAR szStr[23];
			mx::CUtilities::RandomString( szStr, std::size( szStr ) );
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
				case Layout::MX_M_FILE_SAVE : {
					// Not for processes or read-only files.
					::EnableMenuItem( _hMenu, uiId, MF_BYCOMMAND | (CanSave() ? MF_ENABLED : MF_GRAYED) );
					break;
				}
				case Layout::MX_M_FILE_SAVE_AS : {}									MX_FALLTHROUGH
				case Layout::MX_M_FILE_SAVE_A_COPY : {
					// Files only.  A read-only file can be saved under another name.
					::EnableMenuItem( _hMenu, uiId, MF_BYCOMMAND | (CurrentFile() ? MF_ENABLED : MF_GRAYED) );
					break;
				}
				case Layout::MX_M_FILE_SAVE_SELECTION : {
					// Files and processes.
					::EnableMenuItem( _hMenu, uiId, MF_BYCOMMAND | ((bTabIsOpen && phecControl->HasSelection()) ? MF_ENABLED : MF_GRAYED) );
					break;
				}
				case Layout::MX_M_FILE_SAVE_ALL : {
					::EnableMenuItem( _hMenu, uiId, MF_BYCOMMAND | (CanSaveAll() ? MF_ENABLED : MF_GRAYED) );
					break;
				}
				case Layout::MX_M_FILE_CLOSE : {}									MX_FALLTHROUGH
				case Layout::MX_M_FILE_CLOSE_ALL : {
					::EnableMenuItem( _hMenu, uiId, MF_BYCOMMAND | (bTabIsOpen ? MF_ENABLED : MF_GRAYED) );
					break;
				}
				case Layout::MX_M_FILE_REVERT_REFRESH : {}							MX_FALLTHROUGH
				case Layout::MX_M_FILE_RENAME_FILE : {}								MX_FALLTHROUGH
				case Layout::MX_M_FILE_DELETE_FILE : {
					// Files that exist on disk (not processes, not new files that were never saved).
					const CHexEditorFile * phefFile = CurrentFile();
					::EnableMenuItem( _hMenu, uiId, MF_BYCOMMAND | ((phefFile && !phefFile->IsUntitled()) ? MF_ENABLED : MF_GRAYED) );
					break;
				}

				case Layout::MX_M_EDIT_UNDO : {
					EnableMenuItem(
						_hMenu, uiId,
						MF_BYCOMMAND | ((phecControl && phecControl->CanUndo()) ? MF_ENABLED : MF_GRAYED) );
					break;
				}
				case Layout::MX_M_EDIT_REDO : {
					EnableMenuItem(
						_hMenu, uiId,
						MF_BYCOMMAND | ((phecControl && phecControl->CanRedo()) ? MF_ENABLED : MF_GRAYED) );
					break;
				}
				case Layout::MX_M_EDIT_PASTE : {
					EnableMenuItem(
						_hMenu, uiId,
						MF_BYCOMMAND | ((phecControl && phecControl->CanPaste()) ? MF_ENABLED : MF_GRAYED) );
					break;
				}
				case Layout::MX_M_EDIT_CLIPBOARD_0 : {}								MX_FALLTHROUGH
				case Layout::MX_M_EDIT_CLIPBOARD_1 : {}								MX_FALLTHROUGH
				case Layout::MX_M_EDIT_CLIPBOARD_2 : {}								MX_FALLTHROUGH
				case Layout::MX_M_EDIT_CLIPBOARD_3 : {}								MX_FALLTHROUGH
				case Layout::MX_M_EDIT_CLIPBOARD_4 : {}								MX_FALLTHROUGH
				case Layout::MX_M_EDIT_CLIPBOARD_5 : {}								MX_FALLTHROUGH
				case Layout::MX_M_EDIT_CLIPBOARD_6 : {}								MX_FALLTHROUGH
				case Layout::MX_M_EDIT_CLIPBOARD_7 : {}								MX_FALLTHROUGH
				case Layout::MX_M_EDIT_CLIPBOARD_8 : {}								MX_FALLTHROUGH
				case Layout::MX_M_EDIT_CLIPBOARD_9 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE,
						.fState = UINT( (size_t( uiId - Layout::MX_M_EDIT_CLIPBOARD_0 ) == CCopyPasteManager::ActiveClipboard()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_EDIT_REAL_TIME_FILE_EDITING : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE,
						.fState = UINT( RealTimeFileEditing() ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_EDIT_REAL_TIME_RAM_TEXT_EDITING : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE,
						.fState = UINT( RealTimeRamTextEditing() ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_EDIT_INSERT_MODE : {
					// Process memory cannot grow, so it is always in Overwrite mode.
					const bool bProcess = phecControl && phecControl->IsProcess();
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE,
						.fState = UINT( ((!IsOverwriteMode() && !bProcess) ? MFS_CHECKED : MFS_UNCHECKED) | (bProcess ? MFS_GRAYED : MFS_ENABLED) ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_EDIT_OVERWRITE_MODE : {
					const bool bProcess = phecControl && phecControl->IsProcess();
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE,
						.fState = UINT( (IsOverwriteMode() || bProcess) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_EDIT_CLEAR_ALL_CHANGES : {}						MX_FALLTHROUGH
				case Layout::MX_M_EDIT_CLEAR_CHANGES : {
					::EnableMenuItem(
						_hMenu, uiId,
						MF_BYCOMMAND | ((phecControl && phecControl->HasChanges()) ? MF_ENABLED : MF_GRAYED) );
					break;
				}
				case Layout::MX_M_EDIT_CUT : {
					// Cut removes bytes, so it also needs a writable data source (as on the toolbar).
					EnableMenuItem(
						_hMenu, uiId,
						MF_BYCOMMAND | ((phecControl && phecControl->HasSelection() && phecControl->CanDelete()) ? MF_ENABLED : MF_GRAYED) );
					break;
				}
				case Layout::MX_M_EDIT_COPY : {}									MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_ : {}								MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_HEX : {}								MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_DECIMAL : {}							MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_BINARY : {}							MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_C : {}								MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_JAVA : {}							MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_PYTHON : {}							MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_C_STRING : {}						MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_CS_STRING : {}						MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_JAVA_STRING : {}						MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_JS_STRING : {}						MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_PYTHON_STRING : {}					MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_8_BIT_HEX : {}						MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_16_BIT_HEX : {}						MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_32_BIT_HEX : {}						MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_MOTOROLA_S19 : {}					MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_MOTOROLA_S28 : {}					MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_MOTOROLA_S37 : {}					MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_HEX_EDITOR_AREAS : {}				MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_WEB_PAGE : {}						MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_RICH_TEXT : {}						MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_BASE64 : {}							MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_UUENCODING : {}						MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_UINT8 : {}							MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_INT8 : {}							MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_UINT16 : {}							MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_INT16 : {}							MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_UINT32 : {}							MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_INT32 : {}							MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_UINT64 : {}							MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_INT64 : {}							MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_P16 : {}								MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_P32 : {}								MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_P64 : {}								MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_FLOAT10 : {}							MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_FLOAT11 : {}							MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_BFLOAT : {}							MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_FLOAT16 : {}							MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_FLOAT32 : {}							MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_FLOAT64 : {}							MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_PCM8 : {}							MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_PCM16 : {}							MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_PCM24 : {}							MX_FALLTHROUGH
				case Layout::MX_M_EDIT_COPY_AS_PCM32 : {}							MX_FALLTHROUGH
				case Layout::MX_M_EDIT_DELETE : {
					EnableMenuItem(
						_hMenu, uiId,
						MF_BYCOMMAND | ((phecControl && phecControl->HasSelection()) ? MF_ENABLED : MF_GRAYED) );
					break;
				}

				case Layout::MX_M_SELECT_SELECT_ALL : {
					::EnableMenuItem(
						_hMenu, uiId,
						MF_BYCOMMAND | ((phecControl && !phecControl->IsProcess()) ? MF_ENABLED : MF_GRAYED) );
					break;
				}
				case Layout::MX_M_SELECT_WORD : {
					::EnableMenuItem(
						_hMenu, uiId,
						MF_BYCOMMAND | ((phecControl && phecControl->Size()) ? MF_ENABLED : MF_GRAYED) );
					break;
				}
				case Layout::MX_M_SELECT_LINE : {
					::EnableMenuItem(
						_hMenu, uiId,
						MF_BYCOMMAND | ((phecControl && phecControl->Size()) ? MF_ENABLED : MF_GRAYED) );
					break;
				}
				case Layout::MX_M_SELECT_REVSERSE_SELECTION : {
					::EnableMenuItem(
						_hMenu, uiId,
						MF_BYCOMMAND | ((phecControl && phecControl->HasSelection()) ? MF_ENABLED : MF_GRAYED) );
					break;
				}
				case Layout::MX_M_SELECT_MARK_SELECTION_START : {
					::EnableMenuItem(
						_hMenu, uiId,
						MF_BYCOMMAND | ((phecControl && phecControl->HasSelection()) ? MF_ENABLED : MF_GRAYED) );
					break;
				}
				case Layout::MX_M_SELECT_MARK_SELECTION_END : {
					::EnableMenuItem(
						_hMenu, uiId,
						MF_BYCOMMAND | ((phecControl && phecControl->HasSelection()) ? MF_ENABLED : MF_GRAYED) );
					break;
				}
				case Layout::MX_M_SELECT_COLUMN_MODE : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->SelectionIsColumn()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_SELECT_SELECT_RANGE : {
					::EnableMenuItem( _hMenu, uiId, MF_BYCOMMAND | (bTabIsOpen ? MF_ENABLED : MF_GRAYED) );
					break;
				}
				case Layout::MX_M_SELECT_EXPAND_SELECTION : {
					// Not available yet.
					::EnableMenuItem( _hMenu, uiId, MF_BYCOMMAND | MF_GRAYED );
					break;
				}
				case Layout::MX_M_EDIT_CLEAR_CLICKED_CHANGE : {
					::EnableMenuItem( _hMenu, uiId, MF_BYCOMMAND | ((phecControl && phecControl->HasChanges()) ? MF_ENABLED : MF_GRAYED) );
					break;
				}
				case Layout::MX_M_EDIT_READ_ONLY : {
					// Checked when edits are refused; only Read Only mode can be turned off (not a read-only file or process).
					const size_t sIdx = CurrentTab();
					const CHexEditorInterface * pheiThis = (sIdx != SIZE_MAX) ? m_vTabs[sIdx].pheiInterface.get() : nullptr;
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE,
						.fState = UINT( ((pheiThis && pheiThis->ReadOnly()) ? MFS_CHECKED : MFS_UNCHECKED) |
							((pheiThis && !pheiThis->SourceReadOnly()) ? MFS_ENABLED : MFS_GRAYED) ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_TOOLS_EXPRESSION_EVALUATOR : {
					::CheckMenuItem( _hMenu, uiId, MF_BYCOMMAND | (m_peeExpEval ? MF_CHECKED : MF_UNCHECKED) );
					break;
				}
				case Layout::MX_M_EDIT_KEEP_FILE_TIME : {
					const CHexEditorFile * phefFile = CurrentFile();
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE,
						.fState = UINT( ((phefFile && phefFile->KeepFileTime()) ? MFS_CHECKED : MFS_UNCHECKED) |
							((phefFile && !phefFile->IsUntitled()) ? MFS_ENABLED : MFS_GRAYED) ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_EDIT_PROPERTIES : {
					std::filesystem::path pPath;
					::EnableMenuItem( _hMenu, uiId, MF_BYCOMMAND | (PropertiesPath( CurrentTab(), pPath ) ? MF_ENABLED : MF_GRAYED) );
					break;
				}
				case Layout::MX_M_VIEW_FONT_CHANGE_FONT : {}						MX_FALLTHROUGH
				case Layout::MX_M_VIEW_ADDRESSES_SET_STARTING_ADDRESS : {}			MX_FALLTHROUGH
				case Layout::MX_M_FILE_EXPORT_HEX : {
					::EnableMenuItem( _hMenu, uiId, MF_BYCOMMAND | (bTabIsOpen ? MF_ENABLED : MF_GRAYED) );
					break;
				}
				case Layout::MX_M_SELECT_MOVE_SELECTION_BACK : {
					::EnableMenuItem(
						_hMenu, uiId,
						MF_BYCOMMAND | ((phecControl && phecControl->HasSelection()) ? MF_ENABLED : MF_GRAYED) );
					break;
				}
				case Layout::MX_M_SELECT_MOVE_SELECTION_FORWARD : {
					::EnableMenuItem(
						_hMenu, uiId,
						MF_BYCOMMAND | ((phecControl && phecControl->HasSelection()) ? MF_ENABLED : MF_GRAYED) );
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
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetAddressGutterFmt() == CHexEditorControl::MX_AF_BYTES_HEX) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_ADDRESSES_DISPLAY_FORMATS_BYTE_DEC : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetAddressGutterFmt() == CHexEditorControl::MX_AF_BYTES_DEC) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_ADDRESSES_DISPLAY_FORMATS_BYTE_OCT : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetAddressGutterFmt() == CHexEditorControl::MX_AF_BYTES_OCT) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_ADDRESSES_DISPLAY_FORMATS_LINE_DEC : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetAddressGutterFmt() == CHexEditorControl::MX_AF_LINE_NUMBER) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_ADDRESSES_DISPLAY_FORMATS_SHORT_HEX : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetAddressGutterFmt() == CHexEditorControl::MX_AF_SHORT_HEX) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_ADDRESSES_DISPLAY_FORMATS_SHORT_DEC : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetAddressGutterFmt() == CHexEditorControl::MX_AF_SHORT_DEC) ? MFS_CHECKED : MFS_UNCHECKED ) };
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
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == MX_DF_HEX) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LEFT_AREA_CHAR : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == MX_DF_CHAR) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LEFT_AREA_OCTAL : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == MX_DF_OCT) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LEFT_AREA_BINARY : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == MX_DF_BIN) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LEFT_AREA_DECIMAL : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == MX_DF_DEC) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LEFT_AREA_UINT8 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == MX_DF_UINT8) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LEFT_AREA_INT8 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == MX_DF_INT8) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LEFT_AREA_UINT16 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == MX_DF_UINT16) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LEFT_AREA_INT16 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == MX_DF_INT16) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LEFT_AREA_UINT32 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == MX_DF_UINT32) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LEFT_AREA_INT32 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == MX_DF_INT32) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LEFT_AREA_UINT64 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == MX_DF_UINT64) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LEFT_AREA_INT64 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == MX_DF_INT64) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LEFT_AREA_POINTER16 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == MX_DF_POINTER16) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LEFT_AREA_POINTER32 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == MX_DF_POINTER32) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LEFT_AREA_POINTER64 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == MX_DF_POINTER64) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LEFT_AREA_FLOAT10 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == MX_DF_FLOAT10) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LEFT_AREA_FLOAT11 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == MX_DF_FLOAT11) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LEFT_AREA_BFLOAT16 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == MX_DF_BFLOAT16) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LEFT_AREA_FLOAT16 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == MX_DF_FLOAT16) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LEFT_AREA_FLOAT32 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == MX_DF_FLOAT32) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LEFT_AREA_FLOAT64 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == MX_DF_FLOAT64) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LEFT_AREA_PCM8 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == MX_DF_PCM8) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LEFT_AREA_PCM16 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == MX_DF_PCM16) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LEFT_AREA_PCM24 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == MX_DF_PCM24) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_LEFT_AREA_PCM32 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetLeftAreaFormat() == MX_DF_PCM32) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}

				case Layout::MX_M_VIEW_RIGHT_AREA_HEX : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == MX_DF_HEX && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_CHAR : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == MX_DF_CHAR && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_OCTAL : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == MX_DF_OCT && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_BINARY : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == MX_DF_BIN && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_DECIMAL : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == MX_DF_DEC && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_UINT8 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == MX_DF_UINT8 && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_INT8 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == MX_DF_INT8 && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_UINT16 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == MX_DF_UINT16 && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_INT16 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == MX_DF_INT16 && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_UINT32 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == MX_DF_UINT32 && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_INT32 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == MX_DF_INT32 && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_UINT64 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == MX_DF_UINT64 && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_INT64 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == MX_DF_INT64 && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_POINTER16 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == MX_DF_POINTER16 && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_POINTER32 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == MX_DF_POINTER32 && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_POINTER64 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == MX_DF_POINTER64 && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_FLOAT10 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == MX_DF_FLOAT10 && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_FLOAT11 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == MX_DF_FLOAT11 && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_BFLOAT16 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == MX_DF_BFLOAT16 && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_FLOAT16 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == MX_DF_FLOAT16 && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_FLOAT32 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == MX_DF_FLOAT32 && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_FLOAT64 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == MX_DF_FLOAT64 && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_PCM8 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == MX_DF_PCM8 && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_PCM16 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == MX_DF_PCM16 && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_PCM24 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == MX_DF_PCM24 && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RIGHT_AREA_PCM32 : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRightAreaFormat() == MX_DF_PCM32 && !phecControl->RightAreaHidden()) ? MFS_CHECKED : MFS_UNCHECKED ) };
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
				case Layout::MX_M_VIEW_RULER_DISPLAY_FMT_HEX : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRulerFormat() == CHexEditorControl::MX_RF_HEX) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_RULER_DISPLAY_FMT_DEC : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetRulerFormat() == CHexEditorControl::MX_RF_DEC) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_MINI_MAP_SHOW : {
					MENUITEMINFOW miiInfo = { .cbSize = sizeof( MENUITEMINFOW ), .fMask = MIIM_STATE, .fState = UINT( (phecControl && phecControl->GetShowMiniMap()) ? MFS_CHECKED : MFS_UNCHECKED ) };
					::SetMenuItemInfoW( _hMenu, uiId, FALSE, &miiInfo );
					break;
				}
				case Layout::MX_M_VIEW_MINI_MAP_OPTIONS : {
					::EnableMenuItem( _hMenu, uiId, MF_BYCOMMAND | (phecControl ? MF_ENABLED : MF_GRAYED) );
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
	bool CDeusHexMachinaWindow::AddTab( const std::shared_ptr<CHexEditorInterface> &_spInterface ) {
		MX_HEX_TAB htTab;
		htTab.pheiInterface = _spInterface;
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
					WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP ,		// dwStyle
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

				auto swsName = _spInterface->TabString();
				TCITEMW tciItem = { 0 };
				tciItem.mask = TCIF_TEXT;
				tciItem.pszText = const_cast<LPWSTR>(swsName.c_str());

				

				if ( ptTab->InsertItem( 0, &tciItem, htTab.phecWidget ) == -1 ) {
					delete htTab.phecWidget;
					//delete htTab.pheiInterface;
					return false;
				}

				htTab.phecWidget->SetStream( htTab.pheiInterface.get(), this );

				LSW_RECT rInternalSize = ptTab->WindowRect().ScreenToClient( ptTab->Wnd() );
				ptTab->AdjustRect( FALSE, &rInternalSize );
				::MoveWindow( htTab.phecWidget->Wnd(), rInternalSize.left, rInternalSize.top, rInternalSize.Width(), rInternalSize.Height(), TRUE );
				UpdateStatusBar_Endian();
				UpdateStatusBar_CharSet();
				UpdateStatusBar_ViewType();
				UpdateStatusBar_Size();
				UpdateStatusBar_PosValue_StartSize();
				UpdateStatusBar_InsertOverwrite();
				UpdateToolBar();
				return true;
			}
		}
		catch ( ... ) {
			delete htTab.phecWidget;
			//delete htTab.pheiInterface;
		}
		return false;
	}

	// Verification function for the Go To dialog.
	/**
	 * Checks the address entered in the Set Starting Address dialog.
	 *
	 * \param _pvThis The CDeusHexMachinaWindow.
	 * \param _ndpParms The dialog's parameters, holding the result.
	 * \return Returns true if the result is an unsigned integer.
	 **/
	bool CDeusHexMachinaWindow::StartingAddress_Verify( void * /*_pvThis*/, const mx::CNumberInputLayout::MX_NUMBER_DIALOG_PARMS &_ndpParms ) {
		auto rCastResult = ee::CExpEvalContainer::ConvertResult( _ndpParms.rResult, ee::EE_NC_UNSIGNED );
		if ( rCastResult.ncType != ee::EE_NC_UNSIGNED ) {
			auto swsTitle = _DEC_WS_9607671B_Invalid_Expression;
			auto swsError = _DEC_WS_16D063D0_The_given_expression_must_evaluate_to_an_unsigned_integer_;
			lsw::CBase::MessageBoxError( _ndpParms.pwThis->Wnd(), swsError.c_str(), swsTitle.c_str() );
			return false;
		}
		return true;
	}

	bool CDeusHexMachinaWindow::GoTo_Verify( void * _pvThis, const mx::CNumberInputLayout::MX_NUMBER_DIALOG_PARMS &_ndpParms ) {
		auto pdhmwThis = static_cast<CDeusHexMachinaWindow *>(_pvThis);
		auto rCastResult = ee::CExpEvalContainer::ConvertResult( _ndpParms.rResult, ee::EE_NC_UNSIGNED );
		if ( rCastResult.ncType != ee::EE_NC_UNSIGNED ) {
			auto swsTitle = _DEC_WS_9607671B_Invalid_Expression;
			auto swsError = _DEC_WS_16D063D0_The_given_expression_must_evaluate_to_an_unsigned_integer_;
			lsw::CBase::MessageBoxError( _ndpParms.pwThis->Wnd(), swsError.c_str(), swsTitle.c_str() );
			return false;
		}
		auto phecControl = pdhmwThis->CurrentEditor();
		if ( !phecControl ) { return true; }	// Internal error but unrecoverable here.
		// Addresses are entered as shown (with the custom starting address added).
		const uint64_t ui64Bias = phecControl->StartingAddress();
		if ( rCastResult.u.ui64Val < ui64Bias || rCastResult.u.ui64Val - ui64Bias > phecControl->Size() ) {
			auto swsTitle = _DEC_WS_9607671B_Invalid_Expression;
			auto swsError = _DEC_WS_D08434A5_Address_out_of_range_;
			lsw::CBase::MessageBoxError( _ndpParms.pwThis->Wnd(), swsError.c_str(), swsTitle.c_str() );
			return false;
		}
		return true;
	}

}	// namespace mx
