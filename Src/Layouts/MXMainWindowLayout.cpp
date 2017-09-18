#include "MXMainWindowLayout.h"
#include "../Strings/MXStringDecoder.h"
#include "../System/MXSystem.h"

#include <Base/LSWBase.h>
#include <Base/LSWWndClassEx.h>
#include <Layout/LSWLayoutManager.h>
#include <random>

namespace mx {

	// == Members.
	LSW_WIDGET_LAYOUT CMainWindowLayout::m_wlMainWindow[] = {
		{
			LSW_LT_MAINWINDOW,						// ltType
			MX_MWI_MAINWINDOW,						// dwId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			64,										// iLeft
			64,										// iTop
			772,									// dwWidth
			605,									// dwHeight
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,		// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			MX_MWI_NONE,							// dwParentId
		},
		{
			LSW_LT_LISTVIEW,						// ltType
			MX_MWI_STOREDADDRESSES,					// dwId
			WC_LISTVIEWW,							// lpwcClass
			TRUE,									// bEnabled
			64,										// iLeft
			64,										// iTop
			50,									// dwWidth
			50,									// dwHeight
			WS_CHILD | LVS_REPORT | LVS_EDITLABELS | WS_VISIBLE,		// dwStyle
			0,										// dwStyleEx
			nullptr,								// pwcText
			MX_MWI_MAINWINDOW,						// dwParentId
		},
	};

	// The class for the main window.
	ATOM CMainWindowLayout::m_aMainClass = 0;

	// The main window.
	CWidget * CMainWindowLayout::m_pwMainWindow = nullptr;

	// == Functions.
	// Creates the main window.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc., and registers the main window class.
	BOOL CMainWindowLayout::CreateMainWindow() {
		if ( !m_aMainClass ) {
			// Register the window classes we need.
			WCHAR szTemp[5];
			std::random_device rdRand;   // non-deterministic generator  
			std::mt19937 mGen( rdRand() );  // to seed mersenne twister.  
			std::uniform_int_distribution<> uidDist( 'a', 'z' );

			for ( SIZE_T I = 0; I < MX_ELEMENTS( szTemp ); ++I ) {
				szTemp[I] = uidDist( mGen );
			}
			szTemp[MX_ELEMENTS( szTemp )-1] = L'\0';
			lsw::CWndClassEx wceEx( lsw::CWidget::WindowProc, szTemp );
			wceEx.SetBackgroundBrush( reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1) );
			m_aMainClass = lsw::CBase::RegisterClassExW( wceEx.Obj() );

			
		}
		WCHAR szTempTitle[_LEN_F8A1A494+1];
		_DEC_F8A1A494_L__Spiro_MHS_X( szTempTitle );
		LSW_WIDGET_LAYOUT * _pwMain = CHelpers::FindLayout( m_wlMainWindow, MX_ELEMENTS( m_wlMainWindow ), MX_MWI_MAINWINDOW );

		_pwMain->lpwcClass = reinterpret_cast<LPCWSTR>(m_aMainClass);
		_pwMain->pwcText = szTempTitle;
		m_pwMainWindow = lsw::CBase::LayoutManager()->CreateWindowX( m_wlMainWindow, MX_ELEMENTS( m_wlMainWindow ) );
		_pwMain->lpwcClass = nullptr;
		_pwMain->pwcText = nullptr;
		::ZeroMemory( szTempTitle, MX_ELEMENTS( szTempTitle ) );
	}

}	// namespace mx
