#include "MXMhsMainWindow.h"
#include "../Layouts/MXMainWindowLayout.h"
#include "../Layouts/MXOpenProcessLayout.h"
#include "../System/MXSystem.h"
#include <Base/LSWBase.h>
#include <Rebar/LSWRebar.h>
#include <ToolBar/LSWToolBar.h>

namespace mx {

	CMhsMainWindow::CMhsMainWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu ) {

		static const struct {
			LPCWSTR				lpwsImageName;
			DWORD				dwConst;
		} sImages[] = {
			{ L"73", MX_I_OPENPROCESS },
			{ L"97", MX_I_OPENFORDEBUG },

			{ L"1", MX_I_ADDENTRY },
			{ L"52", MX_I_OPENENTRY },
			{ L"22", MX_I_SAVEENTRY },
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

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CMhsMainWindow::InitDialog() {
		CToolBar * plvToolBar = static_cast<CToolBar *>(FindChild( CMainWindowLayout::MX_MWI_TOOLBAR0 ));
		CRebar * plvRebar = static_cast<CRebar *>(FindChild( CMainWindowLayout::MX_MWI_REBAR0 ));

		// == Toolbar.
		plvToolBar->SetImageList( 0, m_iImages );
//#define MX_TOOL_STR( TXT )					reinterpret_cast<INT_PTR>(TXT)
#define MX_TOOL_STR( TXT )						0
		const TBBUTTON bButtons[] = {
			// iBitmap							idCommand									fsState				fsStyle			bReserved	dwData	iString
			{ m_iImageMap[MX_I_OPENPROCESS],	CMainWindowLayout::MX_MWMI_OPENPROCESS,		TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Open Process" ) },
			{ m_iImageMap[MX_I_OPENFORDEBUG],	CMainWindowLayout::MX_MWMI_OPENFORDEBUG,	TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Open for Debug") },
			{ -1,								0,											TBSTATE_ENABLED,	BTNS_SEP,		{ 0 },		0,		MX_TOOL_STR( L"Open for Debug" ) },
			{ m_iImageMap[MX_I_ADDENTRY],		CMainWindowLayout::MX_MWMI_ADDENTRY,		TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Add Entry" ) },
			{ m_iImageMap[MX_I_OPENENTRY],		CMainWindowLayout::MX_MWMI_OPENSAVEFILE,	TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Open Save File" ) },
			{ m_iImageMap[MX_I_SAVEENTRY],		CMainWindowLayout::MX_MWMI_SAVE,			TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Save Entries" ) },
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

	   LSW_RECT rRebarRect = ClientRect();
	   ::MoveWindow( plvRebar->Wnd(), rRebarRect.left, rRebarRect.top, rRebarRect.Width(), plvRebar->WindowRect().Height(), FALSE );

		plvRebar->UpdateRects();

		RECT rRect;
		::GetClientRect( Wnd(), &rRect );
		RECT rRect2;
		::GetWindowRect( Wnd(), &rRect2 );

		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CMhsMainWindow::Command( WORD _Id, HWND _hControl ) {
		switch ( _Id ) {
			case CMainWindowLayout::MX_MWMI_OPENPROCESS : {
				COpenProcessLayout::CreateOpenProcessDialog( this );
				break;
			}
		}
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from menu.
	CWidget::LSW_HANDLED CMhsMainWindow::MenuCommand( WORD _Id ) {
		switch ( _Id ) {
			case CMainWindowLayout::MX_MWMI_OPENPROCESS : {
				COpenProcessLayout::CreateOpenProcessDialog( this );
				break;
			}
		}
		return LSW_H_CONTINUE;
	}

}	// namespace mx
