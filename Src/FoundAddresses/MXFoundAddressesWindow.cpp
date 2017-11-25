#include "MXFoundAddressesWindow.h"
#include "../Layouts/MXFoundAddressLayout.h"
#include "../System/MXSystem.h"

#include <Rebar/LSWRebar.h>
#include <ToolBar/LSWToolBar.h>

namespace mx {

	CFoundAddressesWindow::CFoundAddressesWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu ) :
		lsw::CDockable( _wlLayout, _pwParent, _bCreateWidget, _hMenu ) {
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
			std::wstring wsTemp = L"Resources/";
			wsTemp += sImages[I].lpwsImageName;
			wsTemp += L".bmp";

			m_bBitmaps[sImages[I].dwConst].LoadFromFile( wsTemp.c_str(), 0, 0, LR_CREATEDIBSECTION );
			m_iImageMap[sImages[I].dwConst] = m_iImages.Add( m_bBitmaps[sImages[I].dwConst].Handle() );
		}
	}
	CFoundAddressesWindow::~CFoundAddressesWindow() {
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
			};
#undef MX_TOOL_STR

			plvToolBar->AddButtons( bButtons, MX_ELEMENTS( bButtons ) );

			if ( plvRebar ) {
				plvRebar->SetImageList( m_iImages );
				{
					LSW_REBARBANDINFO riRebarInfo;
					riRebarInfo.SetColors( ::GetSysColor( COLOR_BTNTEXT ), ::GetSysColor( COLOR_BTNFACE ) );
					riRebarInfo.SetStyle( RBBS_CHILDEDGE |
					  RBBS_FIXEDBMP );
					riRebarInfo.SetChild( plvToolBar->Wnd() );
					riRebarInfo.SetChildSize( plvToolBar->GetMinBoundingRect().Width(), plvToolBar->GetMinBoundingRect().Height() );
					riRebarInfo.SetId( CFoundAddressLayout::MX_FAW_TOOLBAR0 );
					plvRebar->InsertBand( -1, riRebarInfo );
				}
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
				{ _T_C2F3561D_Address, _LEN_C2F3561D, 80 },
				{ _T_DCB67730_Value, _LEN_DCB67730, 120 },
				{ _T_31A2F4D5_Current_Value, _LEN_31A2F4D5, 120 },
			};
			for ( INT I = 0; I < MX_ELEMENTS( aTitles ); I++ ) {
				INT iCol = plvAddressList->AddColumn( mx::CStringDecoder::DecodeToWString( aTitles[I]._pcText, aTitles[I].sLen ).c_str() );
				BOOL bAdded = plvAddressList->SetColumnWidth( iCol, aTitles[I].dwWidth );
				if ( !bAdded ) { break; }
			}
			//plvAddressList->SetColumnWidth( plvAddressList->GetTotalColumns(), LVSCW_AUTOSIZE_USEHEADER );
		}


		{
			LSW_RECT rRect;
			// Send a fake WM_SIZE message to cause the window to recalculate and redraw its layout.
			::GetClientRect( Wnd(), &rRect );
			::SendMessageW( Wnd(), WM_SIZE, SIZE_RESTORED, MAKELPARAM( rRect.Width(), rRect.Height() ) );
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

}	// namespace mx
