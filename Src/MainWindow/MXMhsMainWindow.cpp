#include "MXMhsMainWindow.h"
#include "../Layouts/MXMainWindowLayout.h"
#include "../Layouts/MXOpenProcessLayout.h"
#include "../System/MXSystem.h"
#include <Base/LSWBase.h>
#include <ComboBox/LSWComboBox.h>
#include <ComboBox/LSWComboBoxEx.h>
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
		m_iImages.Create( 24, 24, ILC_COLOR32, MX_I_TOTAL, MX_I_TOTAL * 2 );

		for ( size_t I = 0; I < MX_I_TOTAL; ++I ) {
			std::wstring wsTemp = L"Resources/";
			wsTemp += sImages[I].lpwsImageName;
			wsTemp += L".bmp";

			m_bBitmaps[sImages[I].dwConst].LoadFromFile( wsTemp.c_str(), 0, 0, LR_CREATEDIBSECTION );
			m_iImageMap[sImages[I].dwConst] = m_iImages.Add( m_bBitmaps[sImages[I].dwConst].Handle() );
		}

		
		/*::CreateWindowExW( WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_TOOLWINDOW,
			L"ReBarWindow32",
			NULL,
			WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TBSTYLE_TRANSPARENT | TBSTYLE_REGISTERDROP | TBSTYLE_LIST | TBSTYLE_FLAT | CCS_NODIVIDER | CCS_NOPARENTALIGN | CCS_NORESIZE | CCS_NOMOVEY,
			0, 20,
			_wlLayout.dwWidth, 150,
			Wnd(),
			(HMENU)2,
			::GetModuleHandleA( NULL ),
			NULL );*/

		/*
		::CreateWindowExA( WS_EX_LEFT | WS_EX_LTRREADING| WS_EX_RIGHTSCROLLBAR | WS_EX_TOOLWINDOW | TBSTYLE_EX_DOUBLEBUFFER,
			"ToolbarWindow32",
			"",
			WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TBSTYLE_TRANSPARENT | TBSTYLE_REGISTERDROP | TBSTYLE_LIST | TBSTYLE_FLAT | CCS_NODIVIDER | CCS_NOPARENTALIGN | CCS_NORESIZE | CCS_TOP,
			0, 0,
			_wlLayout.dwWidth, 50,
			Wnd(),
			NULL,
			::GetModuleHandleA( NULL ),
			NULL );
		*/

	}

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CMhsMainWindow::InitDialog() {
		CToolBar * plvToolBar = static_cast<CToolBar *>(FindChild( CMainWindowLayout::MX_MWI_TOOLBAR0 ));
		CComboBox * pcbeBox = static_cast<CComboBox *>(FindChild( 50 ));

		static WCHAR * ptcBlah[] = {
			L"Super",
			L"Nifty",
			L"Puff ",
		};
		for (DWORD idx=0; idx < 3; idx++) {
			pcbeBox->AddString( ptcBlah[idx] );
		}

		CRebar * plvRebar = static_cast<CRebar *>(FindChild( CMainWindowLayout::MX_MWI_REBAR0 ));


		// == Toolbar.
		plvToolBar->SetImageList( 0, m_iImages );
		const TBBUTTON bButtons[] = {
			// iBitmap							idCommand								fsState				fsStyle			bReserved	dwData	iString
			{ m_iImageMap[MX_I_OPENPROCESS],	CMainWindowLayout::MX_MWMI_OPENPROCESS, TBSTATE_ENABLED,	BTNS_AUTOSIZE, { 0 },		0,		0/*reinterpret_cast<INT_PTR>(L"Open Process")*/ },
			{ m_iImageMap[MX_I_OPENFORDEBUG],	CMainWindowLayout::MX_MWMI_OPENFORDEBUG, TBSTATE_ENABLED,	BTNS_AUTOSIZE, { 0 },		0,		0/*reinterpret_cast<INT_PTR>(L"Open for Debug")*/ },
		};
		plvToolBar->AddButtons( bButtons, MX_ELEMENTS( bButtons ) );


		REBARINFO rbi;
		// Set the image list for the coolbar.
		rbi.cbSize = sizeof(rbi);
		rbi.fMask = RBIM_IMAGELIST;
		rbi.himl = m_iImages.Handle();
		SendMessage( plvRebar->Wnd(), RB_SETBARINFO, 0L, (LPARAM)&rbi );


		// Initialize REBARBANDINFO.
		REBARBANDINFO rbBand;
		rbBand.cbSize = sizeof(REBARBANDINFO);
		rbBand.fMask = RBBIM_COLORS |     // clrFore and clrBack are valid
		   RBBIM_CHILD |                  // hwndChild is valid
		   RBBIM_CHILDSIZE |              // cxMinChild and cyMinChild are valid
		   RBBIM_STYLE |                  // fStyle is valid
		   RBBIM_ID |                     // wID is valid
		   RBBIM_TEXT |                   // lpText is valid
		   RBBIM_IMAGE |                  // iImage is valid
		   0;              // hbmBack is valid
		rbBand.clrFore = GetSysColor(COLOR_BTNTEXT);
		rbBand.clrBack = GetSysColor(COLOR_BTNFACE);
		rbBand.fStyle = RBBS_NOVERT |     // Do not display in vertical orientation.
		   RBBS_CHILDEDGE |
		   RBBS_FIXEDBMP;
		//rbBand.hbmBack = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BACK));
		rbBand.lpText = TEXT("Cool sites:");
		rbBand.hwndChild = pcbeBox->Wnd();
		rbBand.cxMinChild = 24;
		rbBand.cyMinChild = 24;
		rbBand.iImage = m_iImageMap[MX_I_ADDENTRY];

		// Add the combo box band to the end.
		SendMessage( plvRebar->Wnd(), RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);



		// Initialize REBARBANDINFO for all coolbar bands.
	   rbBand.cbSize = sizeof(REBARBANDINFO);
	   rbBand.fMask = RBBIM_COLORS |  // clrFore and clrBack are valid
		  RBBIM_CHILD |                     // hwndChild is valid
		  RBBIM_CHILDSIZE |                 // cxMinChild and cyMinChild are valid
		  RBBIM_STYLE |                     // fStyle is valid
		  RBBIM_ID |                              // wID is valid
		  0;                 // hbmBack is valid
	   rbBand.clrFore = GetSysColor(COLOR_BTNTEXT);
	   rbBand.clrBack = GetSysColor(COLOR_BTNFACE);
	   rbBand.fStyle = 0 |  // Do not display in vertical orientation.
		  RBBS_CHILDEDGE |
		  RBBS_FIXEDBMP;
	   rbBand.hwndChild = plvToolBar->Wnd();
	   rbBand.wID = CMainWindowLayout::MX_MWI_TOOLBAR0;
	   rbBand.cxMinChild = plvToolBar->GetMinBoundingRect().Width();
	   rbBand.cyMinChild = plvToolBar->GetMinBoundingRect().Height();

	   // Insert band into coolbar.
	   SendMessage( plvRebar->Wnd(), RB_INSERTBAND, (UINT) -1, 
		   (LPARAM)(LPREBARBANDINFO)&rbBand);

	   LSW_RECT rRebarRect = ClientRect();
	   ::MoveWindow( plvRebar->Wnd(), rRebarRect.left, rRebarRect.top, rRebarRect.Width(), plvRebar->WindowRect().Height(), FALSE );


	   RECT rTemp1;
		::GetClientRect( plvToolBar->Wnd(), &rTemp1 );

		plvRebar->UpdateRects();
		pcbeBox->UpdateRects();

#if 0
		CComboBoxEx * pcbeBox = static_cast<CComboBoxEx *>(FindChild( 50 ));
		//LPVOID lpvPrev = (LPVOID)SendMessage( pcbeBox->Wnd(), CBEM_SETIMAGELIST, 0L, (LPARAM)m_iImages.Handle() );

		static WCHAR * ptcBlah[] = {
			L"Super",
			L"Nifty",
			L"Puff ",
		};
		// Add strings to the combo box.
		INT iRet;
		for (DWORD idx=0; idx < 3; idx++)
		{
			COMBOBOXEXITEMW cbI = { 0 };
			// Each item has text, an lParam with extra data, and an image.
			cbI.mask = CBEIF_TEXT | CBEIF_LPARAM | 0/*CBEIF_IMAGE*/;    
			cbI.pszText = ptcBlah[idx];         // Name of the link.
			cbI.cchTextMax = 5;    
			cbI.lParam = (LPARAM)idx;   // Pass the URL as extra data.
			cbI.iItem = -1;          // Add the item to the end of the list.
			cbI.iImage = idx;   // Image to display.

			// Add the item to the combo box drop-down list.
			iRet = SendMessageW( pcbeBox->Wnd(), CBEM_INSERTITEMW, 0L,(LPARAM)&cbI);
		}
#endif	// #if 0

#if 0
		// Initialize REBARBANDINFO.
		REBARBANDINFOW rbBand;
rbBand.cbSize = sizeof(REBARBANDINFOW);
rbBand.fMask = RBBIM_COLORS |     // clrFore and clrBack are valid
   RBBIM_CHILD |                  // hwndChild is valid
   RBBIM_CHILDSIZE |              // cxMinChild and cyMinChild are valid
   RBBIM_STYLE |                  // fStyle is valid
   RBBIM_ID |                     // wID is valid
   RBBIM_TEXT |                   // lpText is valid
   RBBIM_IMAGE |                  // iImage is valid
   0;              // hbmBack is valid
rbBand.clrFore = GetSysColor(COLOR_BTNTEXT);
rbBand.clrBack = GetSysColor(COLOR_BTNFACE);
rbBand.fStyle = RBBS_NOVERT |     // Do not display in vertical orientation.
   RBBS_CHILDEDGE |
   RBBS_FIXEDBMP;
rbBand.lpText = TEXT("Cool sites:");
rbBand.hwndChild = hWndCombo;
rbBand.cxMinChild = MIN_COMBOCX;
rbBand.cyMinChild = MIN_CY;
rbBand.iImage = idxFirstImage;

// Add the combo box band to the end.
SendMessage(hWndRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);
#endif
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
