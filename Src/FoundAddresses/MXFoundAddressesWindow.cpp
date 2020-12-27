#include "MXFoundAddressesWindow.h"
#include "../Layouts/MXFoundAddressLayout.h"
#include "../Layouts/MXMainWindowLayout.h"
#include "../MainWindow/MXMhsMainWindow.h"
#include "../System/MXSystem.h"

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
				{ m_iImageMap[MX_I_SUBSEARCH],		CFoundAddressLayout::MX_BC_SUBSEARCH,		TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Sub Search") },
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
				{ _T_LEN_C2F3561D_Address, 80 },
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
					::swprintf_s( szBuffer, mx::CStringDecoder::DecodeToWString( _T_54965420_Distance_from___16I64X, _LEN_54965420 ).c_str(),
						m_pmmwMhsWindow->MemHack()->Searcher().LastSearchParms().dtLVal.u.Int64 );
				}
				else {
					::swprintf_s( szBuffer, mx::CStringDecoder::DecodeToWString( _T_50BACBAF_Distance_from___8I64X, _LEN_50BACBAF ).c_str(),
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

}	// namespace mx
