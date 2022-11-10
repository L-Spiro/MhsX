#include "MXFoundAddressesWindow.h"
#include "../Layouts/MXFoundAddressLayout.h"
#include "../Layouts/MXLayoutMacros.h"
#include "../Layouts/MXLayoutManager.h"
#include "../Layouts/MXMainWindowLayout.h"
#include "../MainWindow/MXMhsMainWindow.h"
#include "../System/MXSystem.h"
#include "MXFoundAddressesListView.h"

#include <Base/LSWBase.h>
#include <Rebar/LSWRebar.h>
#include <ToolBar/LSWToolBar.h>
#include <UpDown/LSWUpDown.h>

namespace mx {

	CFoundAddressesWindow::CFoundAddressesWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CDockable( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pmmwMhsWindow( reinterpret_cast<CMhsMainWindow *>(_ui64Data) ),
		m_uiptrUpdateListTimer( 0 ),
		m_uiUpdateSpeed( 100 ) {
		static const struct {
			LPCWSTR				lpwsImageName;
			DWORD				dwConst;
#if 0
		} sImages[] = {
			{ L"65s", MX_I_SEARCH },
			{ L"98s", MX_I_SUBSEARCH },

			{ L"95s", MX_I_ADD },
		};
		m_iImages.Create( 16, 16, ILC_COLOR32, MX_I_TOTAL, MX_I_TOTAL );
#else
		} sImages[] = {
			{ L"65", MX_I_SEARCH },
			{ L"98", MX_I_SUBSEARCH },

			{ L"95", MX_I_ADD },
		};
		m_iImages.Create( 24, 24, ILC_COLOR32, MX_I_TOTAL, MX_I_TOTAL );
#endif	// #if 1

		for ( size_t I = 0; I < MX_I_TOTAL; ++I ) {
			CSecureWString wsTemp = CSystem::GetResourcesPathW();
			wsTemp += sImages[I].lpwsImageName;
			wsTemp += L".bmp";

			m_bBitmaps[sImages[I].dwConst].LoadFromFile( wsTemp.c_str(), 0, 0, LR_CREATEDIBSECTION );
			m_iImageMap[sImages[I].dwConst] = m_iImages.Add( m_bBitmaps[sImages[I].dwConst].Handle() );
		}
	}
	CFoundAddressesWindow::~CFoundAddressesWindow() {
		if ( m_uiptrUpdateListTimer ) {
			CSystem::KillTimer( Wnd(), m_uiptrUpdateListTimer );
		}
	}

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CFoundAddressesWindow::InitDialog() {
		CToolBar * plvToolBar = static_cast<CToolBar *>(FindChild( CFoundAddressLayout::MX_FAW_TOOLBAR0 ));
		CRebar * plvRebar = static_cast<CRebar *>(FindChild( CFoundAddressLayout::MX_FAW_REBAR0 ));


		// ==== TOOL BAR ==== //
		if ( plvToolBar ) {
			plvToolBar->SetImageList( 0, m_iImages );
#define MX_TOOL_STR( TXT )					reinterpret_cast<INT_PTR>(TXT)
//#define MX_TOOL_STR( TXT )						0
			const TBBUTTON bButtons[] = {
				// iBitmap							idCommand									fsState				fsStyle			bReserved	dwData	iString
				{ m_iImageMap[MX_I_SEARCH],			CFoundAddressLayout::MX_BC_SEARCH,			TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Search" ) },
				{ m_iImageMap[MX_I_SUBSEARCH],		CFoundAddressLayout::MX_BC_SUBSEARCH,		TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Subsearch") },
				{ -1,								0,											TBSTATE_ENABLED,	BTNS_SEP,		{ 0 },		0,		0 },
				{ m_iImageMap[MX_I_ADD],			CFoundAddressLayout::MX_BC_ADD,				TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Add" ) },
				//{ -1,								0,											TBSTATE_ENABLED,	BTNS_SEP,		{ 0 },		0,		0 },
				//{ m_iImageMap[MX_I_ADD],			CFoundAddressLayout::MX_BC_ADD,				TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		0 },
			};
#undef MX_TOOL_STR

			plvToolBar->AddButtons( bButtons, MX_ELEMENTS( bButtons ) );

			if ( plvRebar ) {
				plvRebar->SetImageList( m_iImages );
				{
					LSW_REBARBANDINFO riRebarInfo;
					riRebarInfo.SetColors( ::GetSysColor( COLOR_BTNTEXT ), ::GetSysColor( COLOR_BTNFACE ) );
					riRebarInfo.SetStyle( RBBS_CHILDEDGE | RBBS_FIXEDBMP | RBBS_NOGRIPPER | RBBS_TOPALIGN );
					riRebarInfo.SetChild( plvToolBar->Wnd() );
					riRebarInfo.SetChildSize( plvToolBar->GetMinBoundingRect().Width(), plvToolBar->GetMinBoundingRect().Height() );
					riRebarInfo.SetId( CFoundAddressLayout::MX_FAW_TOOLBAR0 );
					plvRebar->InsertBand( -1, riRebarInfo );
				}

				/*CUpDown * plvUpDownPrefix = static_cast<CUpDown *>(FindChild( CFoundAddressLayout::MX_FAW_PADDINGLEFT ));
				if ( plvUpDownPrefix ) {
					LSW_REBARBANDINFO riRebarInfo;
					riRebarInfo.SetColors( ::GetSysColor( COLOR_BTNTEXT ), ::GetSysColor( COLOR_BTNFACE ) );
					riRebarInfo.SetStyle( RBBS_CHILDEDGE | RBBS_FIXEDBMP | RBBS_NOGRIPPER | RBBS_TOPALIGN );
					riRebarInfo.SetChild( plvUpDownPrefix->Wnd() );
					riRebarInfo.SetChildSize( 12, plvUpDownPrefix->ClientRect().Height() );
					riRebarInfo.SetId( CFoundAddressLayout::MX_FAW_PADDINGLEFT );
					plvRebar->InsertBand( -1, riRebarInfo );
				}*/
			}
		}


		if ( plvRebar ) {
			LSW_RECT rRebarRect = ClientRect( this );
			::MoveWindow( plvRebar->Wnd(), 0, 0, rRebarRect.Width(), plvRebar->WindowRect( this ).Height(), FALSE );

			plvRebar->UpdateRects();
		}


		// ==== LIST VIEW ==== //
		CListView * plvAddressList = ListView();
		if ( plvAddressList ) {
			

			const struct {
				const char * _pcText;
				size_t sLen;
				DWORD dwWidth;
			} aTitles[] = {
				{ _T_LEN_C2F3561D_Address, 86 },
				{ _T_LEN_DCB67730_Value, 120 },
				{ _T_LEN_31A2F4D5_Current_Value, 120 },
			};
			for ( INT I = 0; I < MX_ELEMENTS( aTitles ); I++ ) {
				INT iCol = plvAddressList->AddColumn( mx::CStringDecoder::DecodeToWString( aTitles[I]._pcText, aTitles[I].sLen ).c_str() );
				BOOL bAdded = plvAddressList->SetColumnWidth( iCol, aTitles[I].dwWidth );
				if ( !bAdded ) { break; }
			}
			//plvAddressList->SetItemCountEx( 5, LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL );
			//plvAddressList->SetColumnWidth( plvAddressList->GetTotalColumns(), LVSCW_AUTOSIZE_USEHEADER );
		}


		ForceSizeUpdate();

		std::random_device rdRand;
		std::mt19937 mGen( rdRand() );
		std::uniform_int_distribution<> uidDist( MX_T_UPDATE_LIST, MX_T_UPDATE_LIST + 16 );
		m_uiptrUpdateListTimer = CSystem::SetTimer( Wnd(), uidDist( mGen ), m_uiUpdateSpeed, NULL );
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CFoundAddressesWindow::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {
		switch ( _Id ) {
			case CFoundAddressLayout::MX_BC_SEARCH : {
				CMhsMainWindow * pmmwMain = static_cast<CMhsMainWindow *>(FindParent( mx::CMainWindowLayout::MX_MWI_MAINWINDOW ));
				pmmwMain->ShowLastSearch();
				break;
			}
			case CFoundAddressLayout::MX_BC_SUBSEARCH : {
				CMhsMainWindow * pmmwMain = static_cast<CMhsMainWindow *>(FindParent( mx::CMainWindowLayout::MX_MWI_MAINWINDOW ));
				pmmwMain->ShowStandardSubsearch();
				break;
			}
		}
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from menu.
	CWidget::LSW_HANDLED CFoundAddressesWindow::MenuCommand( WORD _Id ) {
		switch ( _Id ) {
			case CFoundAddressLayout::MX_BC_COPY_ADDRESS : {
				CopySelectedAddresses();
				break;
			}
			case CFoundAddressLayout::MX_BC_COPY_VALUE : {
				CopySelectedValues();
				break;
			}
			case CFoundAddressLayout::MX_BC_COPY_CUR_VALUE : {
				CopySelectedCurValues();
				break;
			}
			case CFoundAddressLayout::MX_BC_COPY_ALL : {
				CopySelectedText();
				break;
			}
		}
		return LSW_H_CONTINUE;
	}

	// Virtual client rectangle.  Can be used for things that need to be adjusted based on whether or not status bars, toolbars, etc. are present.
	const LSW_RECT CFoundAddressesWindow::VirtualClientRect( const CWidget * pwChild ) const {
		LSW_RECT rTemp = ClientRect( this );
		const CRebar * plvRebar = static_cast<const CRebar *>(FindChild( CFoundAddressLayout::MX_FAW_REBAR0 ));
		if ( plvRebar ) {
			LSW_RECT rRebar = plvRebar->ClientRect( this );
			rTemp.top += rRebar.Height();
		}
		return rTemp;
	}

	// Gets a pointer to the list view.
	CListView * CFoundAddressesWindow::ListView() { return static_cast<CListView *>(FindChild( CFoundAddressLayout::MX_FAW_LIST )); }

	// Gets a pointer to the list view.
	const CListView * CFoundAddressesWindow::ListView() const { return static_cast<const CListView *>(FindChild( CFoundAddressLayout::MX_FAW_LIST )); }

	// Updates the header column after a new search.
	void CFoundAddressesWindow::UpdateHeaders( CUtilities::MX_SEARCH_TYPES _stType ) {
		CListView * plvList = ListView();
		if ( plvList ) {

			// Backup the widths.
			for ( INT I = 0; I < plvList->GetTotalColumns(); ++I ) {
				if ( I == m_vHeaderWidths.size() ) {
					m_vHeaderWidths.push_back( plvList->GetColumnWidth( I ) );
				}
				else {
					m_vHeaderWidths[I] = plvList->GetColumnWidth( I );
				}
			}
			// Remove columns.
			plvList->DeleteAllColumns();

			std::vector<CSecureWString> vHeaders;
			vHeaders.push_back( mx::CStringDecoder::DecodeToWString( _T_C2F3561D_Address, _LEN_C2F3561D ).c_str() );
			vHeaders.push_back( mx::CStringDecoder::DecodeToWString( _T_DCB67730_Value, _LEN_DCB67730 ).c_str() );
		
			if ( m_pmmwMhsWindow->MemHack()->Searcher().LastSearchType() == CUtilities::MX_ST_POINTER_SEARCH ) {
				wchar_t szBuffer[128];
				if ( m_pmmwMhsWindow->MemHack()->Searcher().LastSearchParms().dtLVal.u.Int64 > UINT32_MAX ) {
					std::swprintf( szBuffer, mx::CStringDecoder::DecodeToWString( _T_54965420_Distance_from___16I64X, _LEN_54965420 ).c_str(),
						m_pmmwMhsWindow->MemHack()->Searcher().LastSearchParms().dtLVal.u.Int64 );
				}
				else {
					std::swprintf( szBuffer, mx::CStringDecoder::DecodeToWString( _T_50BACBAF_Distance_from___8I64X, _LEN_50BACBAF ).c_str(),
						m_pmmwMhsWindow->MemHack()->Searcher().LastSearchParms().dtLVal.u.Int64 );
				}
				vHeaders.push_back( szBuffer );
				std::memset( szBuffer, 0, sizeof( szBuffer ) );
			}

			if ( m_pmmwMhsWindow->MemHack()->Searcher().LastSearchType() != CUtilities::MX_ST_STRING_SEARCH &&
				m_pmmwMhsWindow->MemHack()->Searcher().LastSearchType() != CUtilities::MX_ST_GROUP_SEARCH ) {
				vHeaders.push_back( mx::CStringDecoder::DecodeToWString( _T_31A2F4D5_Current_Value, _LEN_31A2F4D5 ).c_str() );
			}


			for ( size_t I = 0; I < vHeaders.size(); ++I ) {
				INT iCol = plvList->AddColumn( vHeaders[I].c_str() );
				INT iWidth = iCol >= m_vHeaderWidths.size() ? 120 : m_vHeaderWidths[iCol];
				BOOL bAdded = plvList->SetColumnWidth( iCol, iWidth );
			}

#if 0
			LONG lWid = 0;
			char cKey[] = {
				'A', 'B', 'C', 'D', 'E', 'F'
			};
			for ( size_t I = 0; I < MX_ELEMENTS( cKey ); ++I ) {
				// 7XXXXXXXXXX
				CSecureString sTmp = "7";
				size_t sTotal = m_pmmwMhsWindow->MemHack()->Process().Is32Bit() ? 7 : 10;
				for ( size_t J = 0; J < sTotal; ++J ) {
					sTmp.push_back( cKey[I] );
				}
				if ( m_pmmwMhsWindow->MemHack()->Searcher().LastSearchParms().stType == CUtilities::MX_ST_STRING_SEARCH &&
					m_pmmwMhsWindow->MemHack()->Searcher().LastSearchParms().sstSearchType == CUtilities::MX_SST_UTFALL ) {
					const struct {
						const char * pcText;
						size_t sLen;
					} sData[] = {
						{ _T_LEN_0E813C50_UTF_8 },
						{ _T_LEN_A71F1195_UTF_16 },
						{ _T_LEN_9244B70E_UTF_32 },
						{ _T_LEN_26FC5333_UTF_16_BE },
						{ _T_LEN_D35E9704_UTF_32_BE },
					};
					for ( size_t K = 0; K < MX_ELEMENTS( sData ); ++K ) {
						CSecureString sNewTmp = sTmp;
						sNewTmp.push_back( ' ' );
						sNewTmp += CStringDecoder::DecodeToString( sData[K].pcText, sData[K].sLen );
						LONG lTmp = plvList->GetStringWidth( sNewTmp );
						if ( lTmp > lWid ) {
							lWid = lTmp;
						}
					}
				}
				else {
					LONG lTmp = plvList->GetStringWidth( sTmp );
					if ( lTmp > lWid ) {
						lWid = lTmp;
					}
				}
			}
			plvList->SetColumnWidth( 0, lWid + 1 );
#endif
		}
	}

	// Adds the found addresses to the list.
	void CFoundAddressesWindow::AddFoundAddresses() {
		CListView * plvList = ListView();
		if ( plvList ) {
			plvList->DeleteAll();
			CSearcher::MX_SEARCH_LOCK slSearchLock( &m_pmmwMhsWindow->MemHack()->Searcher() );
			plvList->SetItemCountEx( m_pmmwMhsWindow->MemHack()->Searcher().TotalResults(), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL );
		}
	}

	// Sets the update speed.
	void CFoundAddressesWindow::SetUpdateSpeed( uint32_t _ui32Speed ) {
		m_uiUpdateSpeed = _ui32Speed;
		if ( m_uiptrUpdateListTimer ) {
			// Overwrites the existing timer. ID does not change, meaning = is technically redundant, but let's assume
			//	there is a possibility later it could change.  Solid code.
			m_uiptrUpdateListTimer = CSystem::SetTimer( Wnd(), m_uiptrUpdateListTimer, m_uiUpdateSpeed, NULL );
		}
	}

	// Copies the selected texts' address lines.
	void CFoundAddressesWindow::CopySelectedAddresses() const {
		const CFoundAddressesListView * plvAddressList = static_cast<const CFoundAddressesListView *>(ListView());
		if ( plvAddressList ) {
			std::vector<int> vSelections;
			if ( plvAddressList->GetSelectedItems( vSelections ) ) {
				CSecureWString swsString;
				for ( size_t I = 0; I < vSelections.size(); I++ ) {
					if ( swsString.size() ) {
						swsString.push_back( L'\r' );
						swsString.push_back( L'\n' );
					}
					plvAddressList->GetAddressText( swsString, vSelections[I] );
				}
				if ( swsString.size() ) {
					lsw::LSW_CLIPBOARD cbClipBoard( Wnd(), true );
					cbClipBoard.SetText( swsString.c_str(), swsString.size() );
				}
			}
		}
	}

	// Copies the selected texts' value lines.
	void CFoundAddressesWindow::CopySelectedValues() const {
		const CFoundAddressesListView * plvAddressList = static_cast<const CFoundAddressesListView *>(ListView());
		if ( plvAddressList ) {
			std::vector<int> vSelections;
			if ( plvAddressList->GetSelectedItems( vSelections ) ) {
				CSecureWString swsString;
				for ( size_t I = 0; I < vSelections.size(); I++ ) {
					if ( swsString.size() ) {
						swsString.push_back( L'\r' );
						swsString.push_back( L'\n' );
					}
					plvAddressList->GetValueText( swsString, vSelections[I] );
				}
				if ( swsString.size() ) {
					lsw::LSW_CLIPBOARD cbClipBoard( Wnd(), true );
					cbClipBoard.SetText( swsString.c_str(), swsString.size() );
				}
			}
		}
	}

	// Copies the selected texts' current value lines if any.
	void CFoundAddressesWindow::CopySelectedCurValues() const {
		int iLine = CurValIndex();
		if ( iLine == -1 ) { return; }

		const CFoundAddressesListView * plvAddressList = static_cast<const CFoundAddressesListView *>(ListView());
		if ( plvAddressList ) {
			std::vector<int> vSelections;
			if ( plvAddressList->GetSelectedItems( vSelections ) ) {
				CSecureWString swsString;
				for ( size_t I = 0; I < vSelections.size(); I++ ) {
					if ( swsString.size() ) {
						swsString.push_back( L'\r' );
						swsString.push_back( L'\n' );
					}
					plvAddressList->GetOtherText( swsString, vSelections[I], iLine );
				}
				if ( swsString.size() ) {
					lsw::LSW_CLIPBOARD cbClipBoard( Wnd(), true );
					cbClipBoard.SetText( swsString.c_str(), swsString.size() );
				}
			}
		}
	}

	// Copies the selected rows' text.
	void CFoundAddressesWindow::CopySelectedText() const {
		int iLine = CurValIndex();
		const CFoundAddressesListView * plvAddressList = static_cast<const CFoundAddressesListView *>(ListView());
		if ( plvAddressList ) {
			std::vector<int> vSelections;
			if ( plvAddressList->GetSelectedItems( vSelections ) ) {
				CSecureWString swsString;
				for ( size_t I = 0; I < vSelections.size(); I++ ) {
					if ( swsString.size() ) {
						swsString.push_back( L'\r' );
						swsString.push_back( L'\n' );
					}
					plvAddressList->GetAddressText( swsString, vSelections[I] );
					swsString.push_back( L'\t' );
					plvAddressList->GetValueText( swsString, vSelections[I] );
					if ( iLine != -1 ) {
						swsString.push_back( L'\t' );
						plvAddressList->GetOtherText( swsString, vSelections[I], iLine );
					}
				}
				if ( swsString.size() ) {
					lsw::LSW_CLIPBOARD cbClipBoard( Wnd(), true );
					cbClipBoard.SetText( swsString.c_str(), swsString.size() );
				}
			}
		}
	}

	// Gets the index of the current-value header or -1.
	int CFoundAddressesWindow::CurValIndex() const {
		if ( m_pmmwMhsWindow->MemHack()->Searcher().LastSearchType() != CUtilities::MX_ST_STRING_SEARCH &&
			m_pmmwMhsWindow->MemHack()->Searcher().LastSearchType() != CUtilities::MX_ST_GROUP_SEARCH ) {

			// 0 = Address.
			// 1 = Value.
			int iRet = 2;
			if ( m_pmmwMhsWindow->MemHack()->Searcher().LastSearchType() == CUtilities::MX_ST_POINTER_SEARCH ) {
				++iRet;
			}
			return iRet;
		}
		return -1;
	}

	// WM_TIMER.
	CWidget::LSW_HANDLED CFoundAddressesWindow::Timer( UINT_PTR _uiptrId, TIMERPROC _tpProc ) {
		if ( _uiptrId == m_uiptrUpdateListTimer ) {
			// Update the list.
			CListView * plvList = ListView();
			if ( plvList ) {
				::RedrawWindow( plvList->Wnd(), NULL, NULL, RDW_INVALIDATE | /*RDW_NOCHILDREN | */RDW_UPDATENOW | RDW_FRAME );
			}
		}
		return LSW_H_CONTINUE;
	}

	// WM_CONTEXTMENU.
	CWidget::LSW_HANDLED CFoundAddressesWindow::ContextMenu( CWidget * _pwControl, INT _iX, INT _iY ) {
		switch ( _pwControl->Id() ) {
			case CFoundAddressLayout::MX_FAW_LIST : {
				LSW_MENU_ITEM miMenuBar[] = {
					//bIsSeperator	dwId													bCheckable	bChecked	bEnabled	lpwcText, stTextLen												bSkip
					{ FALSE,		CFoundAddressLayout::MX_BC_COPY_ADDRESS,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_43FCD42E_Copy__Address, _LEN_43FCD42E ),		FALSE },
					{ FALSE,		CFoundAddressLayout::MX_BC_COPY_VALUE,					FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_43A1870B_Copy__Value, _LEN_43A1870B ),			FALSE },
					{ FALSE,		CFoundAddressLayout::MX_BC_COPY_CUR_VALUE,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_C0FE03C5_Copy__Current_Value, _LEN_C0FE03C5 ),	CurValIndex() == -1 },
					{ FALSE,		CFoundAddressLayout::MX_BC_COPY_ALL,					FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_9B7D368F_Copy_A_ll, _LEN_9B7D368F ),			FALSE },
				};

				const LSW_MENU_LAYOUT miMenus[] = {
					{
						MX_M_CONTEXT_MENU,
						0,
						0,
						MX_ELEMENTS( miMenuBar ),
						miMenuBar
					},
					/*{
						MX_MWMI_MENU_FILE,
						MX_MWMI_MENU_BAR,
						MX_MWMI_FILE,
						MX_ELEMENTS( m_miFileMenu ),
						m_miFileMenu
					},*/
				};
				mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
				plmLayout->CreatePopupMenuEx( this, miMenus, MX_ELEMENTS( miMenus ), _iX, _iY );
				break;
			}
		}
		return lsw::CDockable::ContextMenu( _pwControl, _iX, _iY );
	}

}	// namespace mx
