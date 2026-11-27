#include "MXFindResultsWindow.h"
#include "../Layouts/MXFindResultsLayout.h"
#include "../Layouts/MXLayoutMacros.h"
#include "../Layouts/MXLayoutManager.h"
#include "../Layouts/MXUiIds.h"
#include "../Strings/MXStringDecoder.h"
#include "../Strings/MXStringMacros.h"

#include <Base/LSWBase.h>
#include <Helpers/LSWHelpers.h>

#include <algorithm>
#include <CommCtrl.h>
#include <windowsx.h>


namespace mx {

	/** The ID of the map's subclass. */
	static constexpr UINT_PTR							MX_FIND_RESULTS_MAP_SUBCLASS = 0x4D58464D;
	/** The ID of the list's subclass. */
	static constexpr UINT_PTR							MX_FIND_RESULTS_LIST_SUBCLASS = 0x4D58464C;
	/** How often the results are checked for changes (milliseconds). */
	static constexpr UINT								MX_FIND_RESULTS_REFRESH_MS = 250;
	/** The most bytes shown in the Value column. */
	static constexpr size_t								MX_FIND_RESULTS_VALUE_BYTES = 16;
	/** The most rows copied at once. */
	static constexpr size_t								MX_FIND_RESULTS_MAX_COPY = 1000000;
	/** The color of the matches on the map. */
	static constexpr COLORREF							MX_FIND_RESULTS_MATCH_COLOR = RGB( 0xFF, 0x8C, 0x00 );
	/** The color of the bar of the map. */
	static constexpr COLORREF							MX_FIND_RESULTS_BAR_COLOR = RGB( 0xC8, 0xC8, 0xC8 );

	// == CFindResultsListView.
	CFindResultsListView::CFindResultsListView( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CListView( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
	}

	/**
	 * Supplies the text of an item (LVN_GETDISPINFOW).
	 *
	 * \param _plvdiInfo The request.
	 * \return Returns TRUE.
	 **/
	BOOL CFindResultsListView::GetDispInfoNotify( NMLVDISPINFOW * _plvdiInfo ) {
		CFindResultsWindow * pfrwOwner = static_cast<CFindResultsWindow *>(CWidget::Parent());
		if ( pfrwOwner && _plvdiInfo ) { pfrwOwner->GetDispInfo( _plvdiInfo ); }
		return TRUE;
	}

	/**
	 * Handles LVN_ITEMCHANGED.  A newly focused item is shown in the hex editor.
	 *
	 * \param _lplvParm The notification.
	 * \return Returns a LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CFindResultsListView::Notify_ItemChanged( LPNMLISTVIEW _lplvParm ) {
		CFindResultsWindow * pfrwOwner = static_cast<CFindResultsWindow *>(CWidget::Parent());
		if ( pfrwOwner && _lplvParm ) { pfrwOwner->ItemChanged( _lplvParm ); }
		return LSW_H_HANDLED;
	}


	// == CFindResultsWindow.
	CFindResultsWindow::CFindResultsWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CDockable( _wlLayout.AddStyle( WS_CLIPCHILDREN ), _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
	}
	CFindResultsWindow::~CFindResultsWindow() {
		m_tRefresh.Stop();
		CWidget * pwMap = Map();
		if ( pwMap && pwMap->Wnd() ) {
			::RemoveWindowSubclass( pwMap->Wnd(), MapSubclassProc, MX_FIND_RESULTS_MAP_SUBCLASS );
		}
		CListView * plvList = ListView();
		if ( plvList && plvList->Wnd() ) {
			::RemoveWindowSubclass( plvList->Wnd(), ListSubclassProc, MX_FIND_RESULTS_LIST_SUBCLASS );
		}
		m_pfrhHost = nullptr;
	}

	// == Functions.
	/**
	 * Handles WM_INITDIALOG.  Sets up the list and the map and starts watching the results.
	 *
	 * \return Returns a LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CFindResultsWindow::InitDialog() {
		CListView * plvList = ListView();
		if ( plvList ) {
			const DWORD dwExStyle = LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP;
			::SendMessageW( plvList->Wnd(), LVM_SETEXTENDEDLISTVIEWSTYLE, dwExStyle, dwExStyle );
			const INT iAddress = plvList->AddColumn( _DEC_WS_C2F3561D_Address.c_str() );
			plvList->SetColumnWidth( iAddress, 120 );
			const INT iValue = plvList->AddColumn( _DEC_WS_DCB67730_Value.c_str() );
			plvList->SetColumnWidth( iValue, 200 );
			plvList->SetRightColumnSnap( TRUE );
			::SetWindowSubclass( plvList->Wnd(), ListSubclassProc, MX_FIND_RESULTS_LIST_SUBCLASS, reinterpret_cast<DWORD_PTR>(this) );
		}
		CWidget * pwMap = Map();
		if ( pwMap ) {
			::SetWindowSubclass( pwMap->Wnd(), MapSubclassProc, MX_FIND_RESULTS_MAP_SUBCLASS, reinterpret_cast<DWORD_PTR>(this) );
		}
		m_tRefresh.Start( Wnd(), MX_TI_FIND_RESULTS_REFRESH, MX_FIND_RESULTS_REFRESH_MS, nullptr );
		Refresh( true );
		LayOut();
		return CParent::InitDialog();
	}

	/**
	 * Handles WM_CONTEXTMENU.  Shows the Find Results menu.
	 *
	 * \param _pwControl The control that was right-clicked.
	 * \param _iX The horizontal position of the cursor in screen coordinates, or -1 if the menu was opened with the keyboard.
	 * \param _iY The vertical position of the cursor in screen coordinates, or -1 if the menu was opened with the keyboard.
	 * \return Returns a LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CFindResultsWindow::ContextMenu( CWidget * /*_pwControl*/, INT _iX, INT _iY ) {
		CListView * plvList = ListView();
		if ( !plvList ) { return LSW_H_CONTINUE; }
		POINT ptMenu = { _iX, _iY };
		if ( _iX == -1 && _iY == -1 ) {
			RECT rItem = {};
			const int iRow = FocusedRow();
			if ( iRow >= 0 && ListView_GetItemRect( plvList->Wnd(), iRow, &rItem, LVIR_BOUNDS ) ) {
				ptMenu = { rItem.left + 16, rItem.bottom };
			}
			else {
				ptMenu = { 0, 0 };
			}
			::ClientToScreen( plvList->Wnd(), &ptMenu );
		}
		const bool bRows = TotalRows() != 0;
		const bool bSelected = bRows && ListView_GetSelectedCount( plvList->Wnd() ) != 0;
		LSW_MENU_ITEM miMain[] = {
			//bIsSeperator	dwId						bCheckable	bChecked	bEnabled	lpwcText, stTextLen							bSkip
			{ FALSE,		MX_FRC_COPY,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_446FE619__Copy, _LEN_446FE619 ),								FALSE },
			{ FALSE,		MX_FRC_COPY_ALL,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_70142490_Copy__All, _LEN_70142490 ),							FALSE },
			{ TRUE,			0,							FALSE,		FALSE,		FALSE,		nullptr,  0,								FALSE },
			{ FALSE,		MX_FRC_SELECT_ALL,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_E64123EB_Select_A_ll, _LEN_E64123EB ),							FALSE },
			{ TRUE,			0,							FALSE,		FALSE,		FALSE,		nullptr,  0,								FALSE },
			{ FALSE,		MX_FRC_CLEAR,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_2757C08B_C_lear_Results, _LEN_2757C08B ),						FALSE },
		};
		const LSW_MENU_LAYOUT miMenus[] = {
			{ 1, 0, 0, std::size( miMain ), miMain },
		};
		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		HMENU hMenu = plmLayout ? plmLayout->CreateDisplayPopupMenu( miMenus, std::size( miMenus ) ) : NULL;
		if ( !hMenu ) { return LSW_H_HANDLED; }
		::EnableMenuItem( hMenu, MX_FRC_COPY, MF_BYCOMMAND | (bSelected ? MF_ENABLED : MF_GRAYED) );
		for ( UINT uiId : { UINT( MX_FRC_COPY_ALL ), UINT( MX_FRC_SELECT_ALL ), UINT( MX_FRC_CLEAR ) } ) {
			::EnableMenuItem( hMenu, uiId, MF_BYCOMMAND | (bRows ? MF_ENABLED : MF_GRAYED) );
		}
		const UINT uiCmd = static_cast<UINT>(::TrackPopupMenu( hMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_NONOTIFY, ptMenu.x, ptMenu.y, 0, Wnd(), nullptr ));
		::DestroyMenu( hMenu );
		switch ( uiCmd ) {
			case MX_FRC_COPY : {
				CopyRows( false );
				break;
			}
			case MX_FRC_COPY_ALL : {
				CopyRows( true );
				break;
			}
			case MX_FRC_SELECT_ALL : {
				m_bUpdating = true;
				ListView_SetItemState( plvList->Wnd(), -1, LVIS_SELECTED, LVIS_SELECTED );
				m_bUpdating = false;
				break;
			}
			case MX_FRC_CLEAR : {
				if ( m_pfrhHost ) { m_pfrhHost->FindResultsClear(); }
				Refresh( true );
				break;
			}
		}
		return LSW_H_HANDLED;
	}

	/**
	 * Handles WM_TIMER.  Updates the list when the results change.
	 *
	 * \param _uiptrId The timer identifier.
	 * \param _tpProc The timer callback, if any.
	 * \return Returns a LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CFindResultsWindow::Timer( UINT_PTR _uiptrId, TIMERPROC /*_tpProc*/ ) {
		if ( _uiptrId != MX_TI_FIND_RESULTS_REFRESH ) { return LSW_H_CONTINUE; }
		if ( !::IsWindowVisible( Wnd() ) ) { return LSW_H_HANDLED; }
		Refresh( false );
		if ( (++m_ui32Ticks % 4) == 0 ) {
			CListView * plvList = ListView();
			if ( plvList && TotalRows() ) { ::InvalidateRect( plvList->Wnd(), nullptr, FALSE ); }
		}
		return LSW_H_HANDLED;
	}

	/**
	 * The WM_SIZE handler.  Places the map and the list.
	 *
	 * \param _wParam The type of resizing requested.
	 * \param _lWidth The new width of the client area.
	 * \param _lHeight The new height of the client area.
	 * \return Returns a LSW_HANDLED enumeration.
	 **/
	CWidget::LSW_HANDLED CFindResultsWindow::Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight ) {
		LSW_HANDLED hRet = CParent::Size( _wParam, _lWidth, _lHeight );
		LayOut();
		return hRet;
	}

	/**
	 * Sets the object that supplies the results.
	 *
	 * \param _pfrhHost The host.
	 **/
	void CFindResultsWindow::SetHost( CFindResultsHost * _pfrhHost ) {
		m_pfrhHost = _pfrhHost;
		Refresh( true );
	}

	/**
	 * Updates the list and the map if the results changed.
	 *
	 * \param _bForce If true, they are updated even if the results did not change.
	 **/
	void CFindResultsWindow::Refresh( bool _bForce ) {
		CListView * plvList = ListView();
		if ( !plvList ) { return; }
		const uint64_t ui64Revision = m_pfrhHost ? m_pfrhHost->FindResultsList().Revision() : 0;
		if ( _bForce || !m_bHaveRevision || ui64Revision != m_ui64Revision ) {
			try {
				m_vSetStart.clear();
				m_vSetStart.push_back( 0 );
				if ( m_pfrhHost ) {
					for ( const auto & fsSet : m_pfrhHost->FindResultsList().Sets() ) {
						m_vSetStart.push_back( m_vSetStart.back() + fsSet.vMatches.size() );
					}
				}
			}
			catch ( ... ) {
				m_vSetStart.clear();
			}
			m_ui64Revision = ui64Revision;
			m_bHaveRevision = true;
			m_bMapCurrent = false;
			m_bMapDirty = true;

			m_bUpdating = true;
			ListView_SetItemState( plvList->Wnd(), -1, 0, LVIS_SELECTED | LVIS_FOCUSED );
			const size_t sTotal = std::min<size_t>( TotalRows(), size_t( INT_MAX ) );
			ListView_SetItemCountEx( plvList->Wnd(), int( sTotal ), LVSICF_NOSCROLL );
			if ( sTotal ) { ListView_EnsureVisible( plvList->Wnd(), 0, FALSE ); }
			m_bUpdating = false;
			::InvalidateRect( plvList->Wnd(), nullptr, FALSE );

			CWidget * pwMap = Map();
			if ( pwMap ) {
				lsw::LSW_HDC hdDc( pwMap->Wnd() );
				if ( hdDc.hDc ) {
					SIZE sSize = {};
					{
						lsw::LSW_SELECTOBJECT soFont( hdDc.hDc, MapFont() );
						const std::wstring wsCount = CountText();
						::GetTextExtentPoint32W( hdDc.hDc, wsCount.c_str(), int( wsCount.size() ), &sSize );
					}
					const int iWidth = std::max<int>( 24, sSize.cx + 8 );
					if ( iWidth != m_iMapWidth ) {
						m_iMapWidth = iWidth;
						LayOut();
					}
				}
			}
		}
		UpdateMapSource();
	}

	/**
	 * Supplies the text of an item of the list.
	 *
	 * \param _plvdiInfo The request.
	 **/
	void CFindResultsWindow::GetDispInfo( NMLVDISPINFOW * _plvdiInfo ) {
		if ( !(_plvdiInfo->item.mask & LVIF_TEXT) || !_plvdiInfo->item.pszText || _plvdiInfo->item.cchTextMax <= 0 ) { return; }
		_plvdiInfo->item.pszText[0] = L'\0';
		if ( _plvdiInfo->item.iItem < 0 ) { return; }
		try {
			std::wstring wsText;
			if ( CellText( size_t( _plvdiInfo->item.iItem ), _plvdiInfo->item.iSubItem, wsText ) ) {
				::wcsncpy_s( _plvdiInfo->item.pszText, size_t( _plvdiInfo->item.cchTextMax ), wsText.c_str(), _TRUNCATE );
			}
		}
		catch ( ... ) {}
	}

	/**
	 * Called when an item of the list changes state.  A newly focused item is shown in the hex editor.
	 *
	 * \param _lplvParm The notification.
	 **/
	void CFindResultsWindow::ItemChanged( LPNMLISTVIEW _lplvParm ) {
		if ( m_bUpdating || _lplvParm->iItem < 0 || !(_lplvParm->uChanged & LVIF_STATE) ) { return; }
		if ( (_lplvParm->uNewState & LVIS_FOCUSED) && !(_lplvParm->uOldState & LVIS_FOCUSED) ) {
			ShowRow( size_t( _lplvParm->iItem ) );
		}
	}

	/**
	 * Determines whether the keyboard focus is in the window (so that Ctrl+C copies from it rather than from the hex editor).
	 *
	 * \return Returns true if the window or one of its children has the focus.
	 **/
	bool CFindResultsWindow::HasFocus() const {
		HWND hFocus = ::GetFocus();
		return hFocus && Wnd() && (hFocus == Wnd() || ::IsChild( Wnd(), hFocus ));
	}

	/**
	 * Gets the list view.
	 *
	 * \return Returns the list view, or nullptr.
	 **/
	CListView * CFindResultsWindow::ListView() {
		CWidget * pwList = FindChild( CFindResultsLayout::MX_FRI_LIST );
		return pwList ? static_cast<CListView *>(pwList) : nullptr;
	}

	/**
	 * Gets the map control.
	 *
	 * \return Returns the map control, or nullptr.
	 **/
	CWidget * CFindResultsWindow::Map() {
		return FindChild( CFindResultsLayout::MX_FRI_MAP );
	}

	/**
	 * Finds the set and match of a row.
	 *
	 * \param _sRow The row.
	 * \param _rRow Receives the set and match.
	 * \return Returns true if the row exists.
	 **/
	bool CFindResultsWindow::GetRow( size_t _sRow, MX_ROW &_rRow ) const {
		if ( !m_pfrhHost || m_vSetStart.size() < 2 || _sRow >= m_vSetStart.back() ) { return false; }
		const CFindResults & frResults = m_pfrhHost->FindResultsList();
		if ( frResults.Revision() != m_ui64Revision || frResults.Sets().size() + 1 != m_vSetStart.size() ) { return false; }
		auto aIt = std::upper_bound( m_vSetStart.begin(), m_vSetStart.end(), _sRow );
		if ( aIt == m_vSetStart.begin() ) { return false; }
		const size_t sSet = size_t( (aIt - m_vSetStart.begin()) - 1 );
		if ( sSet >= frResults.Sets().size() ) { return false; }
		const size_t sMatch = _sRow - m_vSetStart[sSet];
		if ( sMatch >= frResults.Sets()[sSet].vMatches.size() ) { return false; }
		_rRow.pfsSet = &frResults.Sets()[sSet];
		_rRow.sSet = sSet;
		_rRow.sMatch = sMatch;
		return true;
	}

	/**
	 * Gets the text of a cell.
	 *
	 * \param _sRow The row.
	 * \param _iColumn The column (0 = Address, 1 = Value).
	 * \param _wsText Receives the text.
	 * \return Returns true if the row exists.
	 **/
	bool CFindResultsWindow::CellText( size_t _sRow, int _iColumn, std::wstring &_wsText ) {
		_wsText.clear();
		MX_ROW rRow;
		if ( !GetRow( _sRow, rRow ) ) { return false; }
		const CHexFind::MX_FIND_MATCH & fmMatch = rRow.pfsSet->vMatches[rRow.sMatch];
		auto spSource = rRow.pfsSet->wpSource.lock();
		static const wchar_t wcDigits[] = { L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', L'A', L'B', L'C', L'D', L'E', L'F' };
		if ( _iColumn == 0 ) {
			const uint64_t ui64Addr = fmMatch.ui64Addr + ((spSource && m_pfrhHost) ? m_pfrhHost->FindResultsStartingAddress( spSource.get() ) : 0);
			// 0x2D or 2Dh, by the global option.
			CUtilities::ToHex( ui64Addr, _wsText, 1, true );
			return true;
		}
		if ( _iColumn == 1 ) {
			if ( !spSource ) { return true; }
			const size_t sRead = size_t( std::min<uint64_t>( fmMatch.ui64Size, MX_FIND_RESULTS_VALUE_BYTES ) );
			CHexEditorInterface::CBuffer vBytes;
			if ( !sRead || !spSource->Read( fmMatch.ui64Addr, vBytes, sRead ) ) { return true; }
			for ( size_t I = 0; I < vBytes.size() && I < sRead; ++I ) {
				if ( I ) { _wsText.push_back( L' ' ); }
				_wsText.push_back( wcDigits[vBytes[I]>>4] );
				_wsText.push_back( wcDigits[vBytes[I]&0xF] );
			}
			if ( fmMatch.ui64Size > sRead ) {
				_wsText.push_back( L' ' );
				_wsText.append( 3, L'.' );
			}
			return true;
		}
		return true;
	}

	/**
	 * Gets the focused row.
	 *
	 * \return Returns the focused row, or -1.
	 **/
	int CFindResultsWindow::FocusedRow() {
		CListView * plvList = ListView();
		if ( !plvList ) { return -1; }
		return ListView_GetNextItem( plvList->Wnd(), -1, LVNI_FOCUSED );
	}

	/**
	 * Selects and focuses a row (showing it in the hex editor).
	 *
	 * \param _sRow The row.
	 **/
	void CFindResultsWindow::GoToRow( size_t _sRow ) {
		CListView * plvList = ListView();
		if ( !plvList || _sRow >= TotalRows() || _sRow > size_t( INT_MAX ) ) { return; }
		const bool bWasFocused = FocusedRow() == int( _sRow );
		m_bUpdating = true;
		ListView_SetItemState( plvList->Wnd(), -1, 0, LVIS_SELECTED );
		m_bUpdating = false;
		// Focusing the row shows it (ItemChanged()); a row that is already focused is shown here.
		ListView_SetItemState( plvList->Wnd(), int( _sRow ), LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
		ListView_EnsureVisible( plvList->Wnd(), int( _sRow ), FALSE );
		if ( bWasFocused ) { ShowRow( _sRow ); }
	}

	/**
	 * Shows a row's match in the hex editor.
	 *
	 * \param _sRow The row.
	 **/
	void CFindResultsWindow::ShowRow( size_t _sRow ) {
		MX_ROW rRow;
		if ( !m_pfrhHost || !GetRow( _sRow, rRow ) ) { return; }
		const CHexFind::MX_FIND_MATCH fmMatch = rRow.pfsSet->vMatches[rRow.sMatch];
		auto spSource = rRow.pfsSet->wpSource.lock();
		if ( !spSource ) {
			m_pfrhHost->FindResultsStatus( _DEC_WS_BA008C99_The_data_of_this_result_is_no_longer_open_, true );
			return;
		}
		m_ui64MapCurrent = fmMatch.ui64Addr;
		m_bMapCurrent = true;
		// The host can change the results (tabs closing), so nothing from rRow is used after this.
		m_pfrhHost->FindResultsSelect( spSource.get(), fmMatch.ui64Addr, fmMatch.ui64Size );
		UpdateMapSource();
		CWidget * pwMap = Map();
		if ( pwMap ) { ::InvalidateRect( pwMap->Wnd(), nullptr, FALSE ); }
	}

	/**
	 * Copies rows ("Address<Tab>Value" lines) to the clipboard.
	 *
	 * \param _bAll If true, every row is copied; otherwise the selected rows.
	 **/
	void CFindResultsWindow::CopyRows( bool _bAll ) {
		CListView * plvList = ListView();
		if ( !plvList || !m_pfrhHost ) { return; }
		try {
			std::wstring wsText, wsCell;
			size_t sRows = 0;
			bool bTooMany = false;
			auto fAdd = [&]( size_t _sRow ) {
				if ( CellText( _sRow, 0, wsCell ) ) {
					wsText += wsCell;
					wsText.push_back( L'\t' );
					CellText( _sRow, 1, wsCell );
					wsText += wsCell;
					wsText.push_back( L'\r' );
					wsText.push_back( L'\n' );
					++sRows;
				}
			};
			if ( _bAll ) {
				const size_t sTotal = TotalRows();
				for ( size_t I = 0; I < sTotal; ++I ) {
					if ( sRows == MX_FIND_RESULTS_MAX_COPY ) { bTooMany = true; break; }
					fAdd( I );
				}
			}
			else {
				for ( int I = ListView_GetNextItem( plvList->Wnd(), -1, LVNI_SELECTED ); I >= 0; I = ListView_GetNextItem( plvList->Wnd(), I, LVNI_SELECTED ) ) {
					if ( sRows == MX_FIND_RESULTS_MAX_COPY ) { bTooMany = true; break; }
					fAdd( size_t( I ) );
				}
			}
			if ( !sRows ) { return; }
			bool bOk;
			{
				lsw::LSW_CLIPBOARD cbClipboard( Wnd(), true );
				bOk = cbClipboard.IsOpen() && cbClipboard.SetText( wsText.c_str(), wsText.size() );
			}
			if ( !bOk ) {
				m_pfrhHost->FindResultsStatus( _DEC_WS_6D7AE665_The_results_could_not_be_copied_, true );
				return;
			}
			CSecureWString swsMsg = _DEC_WS_DA32D2F3_Copied_;
			swsMsg += std::to_wstring( sRows );
			swsMsg += (sRows == 1) ? _DEC_WS_8B41AB1C__result_ : _DEC_WS_154BDCBF__results_;
			if ( bTooMany ) { swsMsg += _DEC_WS_B6BFF7D7___Only_the_first_1_000_000_were_copied_; }
			m_pfrhHost->FindResultsStatus( swsMsg, bTooMany );
		}
		catch ( ... ) {
			m_pfrhHost->FindResultsStatus( _DEC_WS_F39F91A5_Out_of_memory_, true );
		}
	}

	/**
	 * Places the map and the list.
	 **/
	void CFindResultsWindow::LayOut() {
		if ( !Wnd() ) { return; }
		const LSW_RECT rClient = ClientRect( nullptr );
		const LONG lHeight = std::max<LONG>( 0, rClient.bottom - rClient.top );
		const LONG lWidth = std::max<LONG>( 0, rClient.right - rClient.left );
		const LONG lMap = std::min<LONG>( m_iMapWidth, lWidth );
		CWidget * pwMap = Map();
		if ( pwMap && pwMap->Wnd() ) {
			::MoveWindow( pwMap->Wnd(), rClient.left, rClient.top, lMap, lHeight, TRUE );
		}
		CListView * plvList = ListView();
		if ( plvList && plvList->Wnd() ) {
			const LONG lLeft = std::min<LONG>( lWidth, lMap + 2 );
			::MoveWindow( plvList->Wnd(), rClient.left + lLeft, rClient.top, lWidth - lLeft, lHeight, TRUE );
		}
	}

	/**
	 * Decides which data the map shows (that of the focused row, the current tab, or the first set), and marks the map for
	 *	rebuilding if it changed.
	 **/
	void CFindResultsWindow::UpdateMapSource() {
		const CHexEditorInterface * pheiNew = nullptr;
		uint64_t ui64Size = 0;
		if ( m_pfrhHost ) {
			const auto & vSets = m_pfrhHost->FindResultsList().Sets();
			MX_ROW rRow;
			const int iFocused = FocusedRow();
			const CHexEditorInterface * pheiCurrent = m_pfrhHost->FindResultsCurrentSource();
			auto fLive = [&]( const CHexEditorInterface * _pheiWant ) -> bool {
				for ( const auto & fsSet : vSets ) {
					auto spSource = fsSet.wpSource.lock();
					if ( spSource && (!_pheiWant || spSource.get() == _pheiWant) ) {
						pheiNew = spSource.get();
						ui64Size = spSource->Size();
						return true;
					}
				}
				return false;
			};
			if ( !(iFocused >= 0 && GetRow( size_t( iFocused ), rRow ) && rRow.pfsSet->wpSource.lock() && fLive( rRow.pfsSet->wpSource.lock().get() )) ) {
				if ( !(pheiCurrent && fLive( pheiCurrent )) ) {
					fLive( nullptr );
				}
			}
		}
		if ( pheiNew != m_pheiMapSource || ui64Size != m_ui64MapSize ) {
			m_pheiMapSource = pheiNew;
			m_ui64MapSize = ui64Size;
			m_bMapDirty = true;
		}
		if ( m_bMapDirty ) {
			CWidget * pwMap = Map();
			if ( pwMap ) { ::InvalidateRect( pwMap->Wnd(), nullptr, FALSE ); }
		}
	}

	/**
	 * Makes the map: marks the pixel rows that have matches.
	 *
	 * \param _iHeight The height of the bar in pixels.
	 **/
	void CFindResultsWindow::BuildMap( int _iHeight ) {
		m_bMapDirty = false;
		try {
			m_vMapRows.assign( size_t( std::max( _iHeight, 0 ) ), 0 );
		}
		catch ( ... ) {
			m_vMapRows.clear();
			return;
		}
		if ( !m_pfrhHost || !m_pheiMapSource || !m_ui64MapSize || m_vMapRows.empty() ) { return; }
		const double dScale = double( m_vMapRows.size() ) / double( m_ui64MapSize );
		const size_t sLast = m_vMapRows.size() - 1;
		for ( const auto & fsSet : m_pfrhHost->FindResultsList().Sets() ) {
			auto spSource = fsSet.wpSource.lock();
			if ( !spSource || spSource.get() != m_pheiMapSource ) { continue; }
			for ( const auto & fmMatch : fsSet.vMatches ) {
				const uint64_t ui64End = fmMatch.ui64Addr + (fmMatch.ui64Size ? fmMatch.ui64Size - 1 : 0);
				const size_t sFrom = std::min( size_t( double( fmMatch.ui64Addr ) * dScale ), sLast );
				const size_t sTo = std::min( size_t( double( ui64End ) * dScale ), sLast );
				for ( size_t I = sFrom; I <= sTo; ++I ) { m_vMapRows[I] = 1; }
			}
		}
	}

	/**
	 * Gets the rectangle of the bar of the map (the part above the count).
	 *
	 * \param _rClient The client rectangle of the map.
	 * \return Returns the rectangle of the bar.
	 **/
	RECT CFindResultsWindow::MapBarRect( const RECT &_rClient ) const {
		const int iText = ::GetSystemMetrics( SM_CYSMCAPTION );
		RECT rBar = _rClient;
		rBar.left += 2;
		rBar.right -= 2;
		rBar.top += 2;
		rBar.bottom = std::max<LONG>( rBar.top, _rClient.bottom - iText - 2 );
		if ( rBar.right < rBar.left ) { rBar.right = rBar.left; }
		return rBar;
	}

	/**
	 * Draws the map.
	 *
	 * \param _hDc The device context.
	 * \param _rClient The client rectangle of the map.
	 **/
	void CFindResultsWindow::PaintMap( HDC _hDc, const RECT &_rClient ) {
		::FillRect( _hDc, &_rClient, ::GetSysColorBrush( COLOR_3DFACE ) );
		RECT rBar = MapBarRect( _rClient );
		if ( rBar.bottom - rBar.top > 2 && rBar.right - rBar.left > 2 ) {
			::FillRect( _hDc, &rBar, lsw::CBase::BrushCache().Brush( MX_FIND_RESULTS_BAR_COLOR ) );
			::FrameRect( _hDc, &rBar, ::GetSysColorBrush( COLOR_3DSHADOW ) );
			RECT rInner = rBar;
			::InflateRect( &rInner, -1, -1 );
			const int iHeight = rInner.bottom - rInner.top;
			if ( m_bMapDirty || m_vMapRows.size() != size_t( iHeight ) ) { BuildMap( iHeight ); }
			HBRUSH hMatch = lsw::CBase::BrushCache().Brush( MX_FIND_RESULTS_MATCH_COLOR );
			if ( hMatch ) {
				for ( size_t I = 0; I < m_vMapRows.size(); ) {
					if ( !m_vMapRows[I] ) { ++I; continue; }
					size_t J = I + 1;
					while ( J < m_vMapRows.size() && m_vMapRows[J] ) { ++J; }
					RECT rRun = { rInner.left, rInner.top + LONG( I ), rInner.right, rInner.top + LONG( J ) };
					::FillRect( _hDc, &rRun, hMatch );
					I = J;
				}
			}
			if ( m_bMapCurrent && m_ui64MapSize && iHeight > 0 ) {
				const LONG lY = LONG( std::min<double>( double( m_ui64MapCurrent ) * double( iHeight ) / double( m_ui64MapSize ), double( iHeight - 1 ) ) );
				RECT rCur = { rBar.left, rInner.top + lY - 1, rBar.right, rInner.top + lY + 2 };
				::FillRect( _hDc, &rCur, ::GetSysColorBrush( COLOR_HIGHLIGHT ) );
			}
		}
		RECT rText = _rClient;
		rText.top = rBar.bottom;
		const std::wstring wsCount = CountText();
		lsw::LSW_SELECTOBJECT soFont( _hDc, MapFont() );
		lsw::LSW_SETBKMODE sbmMode( _hDc, TRANSPARENT );
		lsw::LSW_SETTEXTCOLOR stcColor( _hDc, ::GetSysColor( COLOR_BTNTEXT ) );
		::DrawTextW( _hDc, wsCount.c_str(), int( wsCount.size() ), &rText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX );
	}

	/**
	 * Gets the font of the map.
	 *
	 * \return Returns the font of the map (or the default GUI font).
	 **/
	HFONT CFindResultsWindow::MapFont() {
		CWidget * pwMap = Map();
		HFONT hFont = pwMap ? reinterpret_cast<HFONT>(::SendMessageW( pwMap->Wnd(), WM_GETFONT, 0, 0 )) : NULL;
		return hFont ? hFont : static_cast<HFONT>(::GetStockObject( DEFAULT_GUI_FONT ));
	}

	/**
	 * Goes to the match nearest the address at a point on the map.
	 *
	 * \param _iY The vertical position in the map's client area.
	 **/
	void CFindResultsWindow::MapClick( int _iY ) {
		CWidget * pwMap = Map();
		if ( !pwMap || !m_pfrhHost || !m_pheiMapSource || !m_ui64MapSize ) { return; }
		const CFindResults & frResults = m_pfrhHost->FindResultsList();
		if ( frResults.Revision() != m_ui64Revision || frResults.Sets().size() + 1 != m_vSetStart.size() ) { return; }
		RECT rClient;
		::GetClientRect( pwMap->Wnd(), &rClient );
		RECT rInner = MapBarRect( rClient );
		::InflateRect( &rInner, -1, -1 );
		const int iHeight = rInner.bottom - rInner.top;
		if ( iHeight <= 0 ) { return; }
		const int iY = std::clamp( _iY - int( rInner.top ), 0, iHeight - 1 );
		const double dAddr = (double( iY ) + 0.5) * double( m_ui64MapSize ) / double( iHeight );
		const uint64_t ui64Addr = dAddr >= double( m_ui64MapSize ) ? m_ui64MapSize - 1 : uint64_t( dAddr );

		bool bFound = false;
		size_t sBestRow = 0;
		uint64_t ui64BestDist = 0;
		const auto & vSets = frResults.Sets();
		for ( size_t I = 0; I < vSets.size(); ++I ) {
			auto spSource = vSets[I].wpSource.lock();
			if ( !spSource || spSource.get() != m_pheiMapSource || vSets[I].vMatches.empty() ) { continue; }
			const auto & vMatches = vSets[I].vMatches;
			auto aIt = std::lower_bound( vMatches.begin(), vMatches.end(), ui64Addr, []( const CHexFind::MX_FIND_MATCH &_fmMatch, uint64_t _ui64Addr ) {
				return _fmMatch.ui64Addr < _ui64Addr;
			} );
			for ( int J = 0; J < 2; ++J ) {
				auto aThis = aIt;
				if ( J == 1 ) {
					if ( aIt == vMatches.begin() ) { continue; }
					--aThis;
				}
				else if ( aIt == vMatches.end() ) { continue; }
				const uint64_t ui64Dist = ((*aThis).ui64Addr > ui64Addr) ? (*aThis).ui64Addr - ui64Addr : ui64Addr - (*aThis).ui64Addr;
				if ( !bFound || ui64Dist < ui64BestDist ) {
					bFound = true;
					ui64BestDist = ui64Dist;
					sBestRow = m_vSetStart[I] + size_t( aThis - vMatches.begin() );
				}
			}
		}
		if ( bFound && FocusedRow() != int( sBestRow ) ) { GoToRow( sBestRow ); }
	}

	/**
	 * Gets the text shown below the map (the number of results).
	 *
	 * \return Returns the text.
	 **/
	std::wstring CFindResultsWindow::CountText() const {
		try {
			return std::to_wstring( TotalRows() );
		}
		catch ( ... ) { return std::wstring(); }
	}

	/**
	 * The window procedure of the list (a click on the focused row shows it again; Enter shows the focused row).
	 *
	 * \param _hWnd The list.
	 * \param _uMsg The message.
	 * \param _wParam The message's WPARAM.
	 * \param _lParam The message's LPARAM.
	 * \param _uIdSubclass The ID of the subclass.
	 * \param _dwRefData The window (CFindResultsWindow *).
	 * \return Returns the result of the message.
	 **/
	LRESULT CALLBACK CFindResultsWindow::ListSubclassProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam, UINT_PTR /*_uIdSubclass*/, DWORD_PTR _dwRefData ) {
		CFindResultsWindow * pfrwThis = reinterpret_cast<CFindResultsWindow *>(_dwRefData);
		switch ( _uMsg ) {
			case WM_LBUTTONDOWN : {
				const int iBefore = pfrwThis->FocusedRow();
				LVHITTESTINFO htiHit = {};
				htiHit.pt = { GET_X_LPARAM( _lParam ), GET_Y_LPARAM( _lParam ) };
				const int iHit = ListView_HitTest( _hWnd, &htiHit );
				const LRESULT lrRet = ::DefSubclassProc( _hWnd, _uMsg, _wParam, _lParam );
				if ( iHit >= 0 && iHit == iBefore && !(_wParam & (MK_SHIFT | MK_CONTROL)) ) {
					pfrwThis->ShowRow( size_t( iHit ) );
				}
				return lrRet;
			}
			case WM_KEYDOWN : {
				if ( _wParam == VK_RETURN ) {
					const int iRow = pfrwThis->FocusedRow();
					if ( iRow >= 0 ) { pfrwThis->ShowRow( size_t( iRow ) ); }
					return 0;
				}
				break;
			}
			case WM_GETDLGCODE : {
				if ( _lParam && reinterpret_cast<const MSG *>(_lParam)->message == WM_KEYDOWN && reinterpret_cast<const MSG *>(_lParam)->wParam == VK_RETURN ) {
					return ::DefSubclassProc( _hWnd, _uMsg, _wParam, _lParam ) | DLGC_WANTMESSAGE;
				}
				break;
			}
			case WM_NCDESTROY : {
				::RemoveWindowSubclass( _hWnd, ListSubclassProc, MX_FIND_RESULTS_LIST_SUBCLASS );
				break;
			}
		}
		return ::DefSubclassProc( _hWnd, _uMsg, _wParam, _lParam );
	}

	/**
	 * The window procedure of the map.
	 *
	 * \param _hWnd The map.
	 * \param _uMsg The message.
	 * \param _wParam The message's WPARAM.
	 * \param _lParam The message's LPARAM.
	 * \param _uIdSubclass The ID of the subclass.
	 * \param _dwRefData The window (CFindResultsWindow *).
	 * \return Returns the result of the message.
	 **/
	LRESULT CALLBACK CFindResultsWindow::MapSubclassProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam, UINT_PTR /*_uIdSubclass*/, DWORD_PTR _dwRefData ) {
		CFindResultsWindow * pfrwThis = reinterpret_cast<CFindResultsWindow *>(_dwRefData);
		switch ( _uMsg ) {
			case WM_ERASEBKGND : { return 1; }
			case WM_PAINT : {
				lsw::LSW_BEGINPAINT bpPaint( _hWnd );
				if ( bpPaint.hDc ) {
					RECT rClient;
					::GetClientRect( _hWnd, &rClient );
					HDC hMem = ::CreateCompatibleDC( bpPaint.hDc );
					HBITMAP hBmp = hMem ? ::CreateCompatibleBitmap( bpPaint.hDc, std::max<LONG>( 1, rClient.right ), std::max<LONG>( 1, rClient.bottom ) ) : NULL;
					if ( hMem && hBmp ) {
						lsw::LSW_SELECTOBJECT soBmp( hMem, hBmp );
						pfrwThis->PaintMap( hMem, rClient );
						::BitBlt( bpPaint.hDc, 0, 0, rClient.right, rClient.bottom, hMem, 0, 0, SRCCOPY );
					}
					else {
						pfrwThis->PaintMap( bpPaint.hDc, rClient );
					}
					if ( hBmp ) { ::DeleteObject( hBmp ); }
					if ( hMem ) { ::DeleteDC( hMem ); }
				}
				return 0;
			}
			case WM_LBUTTONDOWN : {
				::SetCapture( _hWnd );
				pfrwThis->m_bMapDragging = true;
				pfrwThis->MapClick( GET_Y_LPARAM( _lParam ) );
				return 0;
			}
			case WM_MOUSEMOVE : {
				if ( pfrwThis->m_bMapDragging && (_wParam & MK_LBUTTON) ) { pfrwThis->MapClick( GET_Y_LPARAM( _lParam ) ); }
				return 0;
			}
			case WM_LBUTTONUP : {
				if ( pfrwThis->m_bMapDragging ) {
					pfrwThis->m_bMapDragging = false;
					if ( ::GetCapture() == _hWnd ) { ::ReleaseCapture(); }
				}
				return 0;
			}
			case WM_CAPTURECHANGED : {
				pfrwThis->m_bMapDragging = false;
				break;
			}
			case WM_SIZE : {
				pfrwThis->m_bMapDirty = true;
				::InvalidateRect( _hWnd, nullptr, FALSE );
				break;
			}
			case WM_NCDESTROY : {
				::RemoveWindowSubclass( _hWnd, MapSubclassProc, MX_FIND_RESULTS_MAP_SUBCLASS );
				break;
			}
		}
		return ::DefSubclassProc( _hWnd, _uMsg, _wParam, _lParam );
	}

}	// namespace mx
