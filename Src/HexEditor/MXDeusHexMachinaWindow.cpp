#include "MXDeusHexMachinaWindow.h"
#include "../Utilities/MXUtilities.h"

#include <Base/LSWWndClassEx.h>


namespace mx {

	// == Members.
	// The main window class.
	ATOM CDeusHexMachinaWindow::m_aAtom = 0;

	CDeusHexMachinaWindow::CDeusHexMachinaWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout.ChangeClass( reinterpret_cast<LPCWSTR>(m_aAtom) ), _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
		//CUtilities::PrintTotalGuiObjects( GR_GDIOBJECTS );
		/*static const struct {
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
		}*/
	}
	CDeusHexMachinaWindow::~CDeusHexMachinaWindow() {
	}

	// == Functions.
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

}	// namespace mx
