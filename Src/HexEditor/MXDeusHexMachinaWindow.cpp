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
		//CUtilities::PrintTotalGuiObjects( GR_GDIOBJECTS );
		static const struct {
			LPCWSTR				lpwsImageName;
			DWORD				dwConst;
		} sImages[] = {
			{ L"52", MX_I_OPENFILE },
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
			{ m_iImageMap[MX_I_OPENFILE],		Layout::MX_M_FILE_OPENFILE,					TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Open File" ) },
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
				// Last status message.
				//{ 450, TRUE },
				// Current process ID.
				//{ 450 + 48, TRUE },
				{ rRebarRect.Width() - psbStatus->ClientRect( this ).Height() - 32, TRUE },

				{ rRebarRect.Width() - psbStatus->ClientRect( this ).Height(), TRUE },
			};
			psbStatus->SetParts( spParts, MX_ELEMENTS( spParts ) );
		}

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
