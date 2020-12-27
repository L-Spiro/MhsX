#include "MXFloatingPointStudioWindow.h"
#include "../Layouts/MXFloatingPointStudioLayout.h"
#include "../Layouts/MXLayoutMacros.h"
#include "../Layouts/MXLayoutManager.h"
#include "../Strings/MXStringDecoder.h"
#include "../System/MXSystem.h"
#include "../Utilities/MXUtilities.h"
#include <Base/LSWBase.h>
#include <Base/LSWWndClassEx.h>
#include <CheckButton/LSWCheckButton.h>
#include <Static/LSWStatic.h>

namespace mx {

	// == Members.
	// The main window class.
	ATOM CFloatingPointStudioWindow::m_aAtom = 0;

	CFloatingPointStudioWindow::CFloatingPointStudioWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout.ChangeClass( reinterpret_cast<LPCWSTR>(m_aAtom) ), _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
		
	}
	CFloatingPointStudioWindow::~CFloatingPointStudioWindow() {
	}

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CFloatingPointStudioWindow::InitDialog() {
		int iX = ::GetSystemMetrics( SM_CXMENUCHECK );
		int iY = ::GetSystemMetrics( SM_CYMENUCHECK );
		const int iShave = MX_FPS_CHECKBOX_TRIM;

		LONG lTop = 0;
		LONG lLeft = 0;

		lLeft = 0;
		for ( WORD I = 0; I < 65; ++I ) {
			LSW_WIDGET_LAYOUT wlLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager())->FixLayout( LSW_WIDGET_LAYOUT{
				LSW_LT_LABEL,							// ltType
				static_cast<WORD>(CFloatingPointStudioLayout::MX_C_KEY_LABEL_START + I),	// wId
				nullptr,								// lpwcClass
				TRUE,									// bEnabled
				TRUE,									// bActive
				lLeft + iShave,							// iLeft
				lTop + iShave,							// iTop
				static_cast<DWORD>(iX - iShave * 2),	// dwWidth
				static_cast<DWORD>(MX_DEF_STATIC_HEIGHT_PXL),						// dwHeight
				MX_STATICSTYLE | SS_CENTER,				// dwStyle
				0,										// dwStyleEx
				L"S",								// pwcText
				0,										// sTextLen
				static_cast<DWORD>(CFloatingPointStudioLayout::MX_C_MAINWINDOW),		// dwParentId

	//#if 0
				nullptr, 0,								// pcLeftSizeExp
				nullptr, 0,								// pcRightSizeExp
				nullptr, 0,								// pcTopSizeExp
				nullptr, 0,								// pcBottomSizeExp
				nullptr, 0,								// pcWidthSizeExp
				nullptr, 0,								// pcHeightSizeExp
	//#endif
			} );
			CStatic * psStatic = static_cast<CStatic *>(static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager())->CreateWidget( wlLayout, this, TRUE, NULL, 0 ));
			if ( psStatic ) {
				::SendMessageW( psStatic->Wnd(), WM_SETFONT, reinterpret_cast<WPARAM>(CBase::MessageBoxFont()), TRUE );
				/*LSW_RECT rRect;
				rRect.Zero();
				Button_GetIdealSize( psStatic->Wnd(), &rRect );
				if ( rRect.Width() ) {
				}*/
			}
			/*if ( pbButton ) {
				LSW_RECT rRect;
				Button_GetIdealSize( pbButton->Wnd(), &rRect );
				if ( rRect.Width() ) {
				}
			}*/
			lLeft += iX - iShave * 2;
		}
		lTop += MX_DEF_STATIC_HEIGHT_PXL;
		lLeft = 0;
		for ( WORD I = 0; I < 65; ++I ) {
			LSW_WIDGET_LAYOUT wlLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager())->FixLayout( LSW_WIDGET_LAYOUT{
				LSW_LT_CHECK,							// ltType
				static_cast<WORD>(CFloatingPointStudioLayout::MX_C_CHECK_START + I),	// wId
				nullptr,								// lpwcClass
				TRUE,									// bEnabled
				TRUE,									// bActive
				lLeft + iShave,							// iLeft
				lTop + iShave,							// iTop
				static_cast<DWORD>(iX - iShave * 2),	// dwWidth
				static_cast<DWORD>(iY - iShave * 2),	// dwHeight
				MX_CHECKSTYLE,							// dwStyle
				0,										// dwStyleEx
				nullptr,								// pwcText
				0,										// sTextLen
				static_cast<DWORD>(CFloatingPointStudioLayout::MX_C_MAINWINDOW),		// dwParentId

	//#if 0
				nullptr, 0,								// pcLeftSizeExp
				nullptr, 0,								// pcRightSizeExp
				nullptr, 0,								// pcTopSizeExp
				nullptr, 0,								// pcBottomSizeExp
				nullptr, 0,								// pcWidthSizeExp
				nullptr, 0,								// pcHeightSizeExp
	//#endif
			} );
			CCheckButton * pbButton = static_cast<CCheckButton *>(static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager())->CreateWidget( wlLayout, this, TRUE, NULL, 0 ));
			/*if ( pbButton ) {
				LSW_RECT rRect;
				Button_GetIdealSize( pbButton->Wnd(), &rRect );
				if ( rRect.Width() ) {
				}
			}*/
			lLeft += iX - iShave * 2;
		}
		lTop += iY - iShave * 2;
		lLeft = 0;
		for ( WORD I = 0; I < 65; ++I ) {
			LSW_WIDGET_LAYOUT wlLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager())->FixLayout( LSW_WIDGET_LAYOUT{
				LSW_LT_LABEL,							// ltType
				static_cast<WORD>(CFloatingPointStudioLayout::MX_C_LABEL_START + I),	// wId
				nullptr,								// lpwcClass
				TRUE,									// bEnabled
				TRUE,									// bActive
				lLeft + iShave,							// iLeft
				lTop + iShave,							// iTop
				static_cast<DWORD>(iX - iShave * 2),	// dwWidth
				static_cast<DWORD>(MX_DEF_STATIC_HEIGHT_PXL * 2),						// dwHeight
				MX_STATICSTYLE | SS_CENTER,				// dwStyle
				0,										// dwStyleEx
				L"6\r\n5",								// pwcText
				0,										// sTextLen
				static_cast<DWORD>(CFloatingPointStudioLayout::MX_C_MAINWINDOW),		// dwParentId

	//#if 0
				nullptr, 0,								// pcLeftSizeExp
				nullptr, 0,								// pcRightSizeExp
				nullptr, 0,								// pcTopSizeExp
				nullptr, 0,								// pcBottomSizeExp
				nullptr, 0,								// pcWidthSizeExp
				nullptr, 0,								// pcHeightSizeExp
	//#endif
			} );
			CStatic * psStatic = static_cast<CStatic *>(static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager())->CreateWidget( wlLayout, this, TRUE, NULL, 0 ));
			if ( psStatic ) {
				::SendMessageW( psStatic->Wnd(), WM_SETFONT, reinterpret_cast<WPARAM>(CBase::MessageBoxFont()), TRUE );
				/*LSW_RECT rRect;
				rRect.Zero();
				Button_GetIdealSize( psStatic->Wnd(), &rRect );
				if ( rRect.Width() ) {
				}*/
			}
			/*if ( pbButton ) {
				LSW_RECT rRect;
				Button_GetIdealSize( pbButton->Wnd(), &rRect );
				if ( rRect.Width() ) {
				}
			}*/
			lLeft += iX - iShave * 2;
		}
		//::EnumChildWindows( Wnd(), CWidget::EnumChildWindows_SetFont, reinterpret_cast<LPARAM>(CBase::MessageBoxFont()) );
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CFloatingPointStudioWindow::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {
		switch ( _Id ) {
			/*case CFloatingPointStudioLayout::MX_PEW_OPENPE : {
				break;
			}*/
		}
		return LSW_H_CONTINUE;
	}

	// WM_CLOSE.
	CWidget::LSW_HANDLED CFloatingPointStudioWindow::Close() {
		
		//::EndDialog( Wnd(), 0 );
		::DestroyWindow( Wnd() );
		//CUtilities::PrintTotalGuiObjects( GR_GDIOBJECTS );
		return LSW_H_HANDLED;
	}

	// WM_ERASEBKGND.
	CWidget::LSW_HANDLED CFloatingPointStudioWindow::EraseBkgnd( HDC _hDc ) {
		return LSW_H_HANDLED;
	}

	// Prepares to create the window.  Creates the atom if necessary.
	void CFloatingPointStudioWindow::PrepareFloatingPointStudio() {
		if ( !m_aAtom ) {
			lsw::CWndClassEx wceEx;
			wceEx.SetInstance( lsw::CBase::GetThisHandle() );
			WCHAR szStr[9];
			mx::CUtilities::RandomString( szStr, MX_ELEMENTS( szStr ) );
			wceEx.SetClassName( szStr );
			wceEx.SetBackgroundBrush( reinterpret_cast<HBRUSH>(CTLCOLOR_DLG + 1) );
			wceEx.SetWindPro( CWidget::WindowProc );
			m_aAtom = lsw::CBase::RegisterClassExW( wceEx.Obj() );
		}
	}

}	// namespace mx
