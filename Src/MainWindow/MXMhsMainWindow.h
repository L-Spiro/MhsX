#pragma once

#include <MainWindow/LSWMainWindow.h>
#include <ImageList/LSWImageList.h>
#include <Images/LSWBitmap.h>

using namespace lsw;

namespace mx {
	
	class CMhsMainWindow : public lsw::CMainWindow {
	public :
		CMhsMainWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL );


		// == Enumerations.
		// Images.
		enum MX_IMAGES {
			MX_I_OPENPROCESS,
			MX_I_OPENFORDEBUG,

			MX_I_ADDENTRY,
			MX_I_OPENENTRY,
			MX_I_SAVEENTRY,

			MX_I_TOTAL,
		};


		// == Functions.
		// WM_INITDIALOG.
		virtual LSW_HANDLED					InitDialog();

		// WM_COMMAND from control.
		virtual LSW_HANDLED					Command( WORD _Id, HWND _hControl );

		// WM_COMMAND from menu.
		virtual LSW_HANDLED					MenuCommand( WORD _Id );


	protected :
		// == Members.
		// Image list.
		CImageList							m_iImages;

		// Images.
		CBitmap								m_bBitmaps[MX_I_TOTAL];

		// Image mapping.
		INT									m_iImageMap[MX_I_TOTAL];

	};

}	// namespace mx
