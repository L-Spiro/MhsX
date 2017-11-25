#pragma once

#include "../MXMhsX.h"

#include <Docking/LSWDockable.h>
#include <Edit/LSWEdit.h>
#include <ImageList/LSWImageList.h>
#include <Images/LSWBitmap.h>
#include <ListView/LSWListView.h>

using namespace lsw;

namespace mx {
	
	class CExpEvalWindow : public lsw::CDockable {
	public :
		CExpEvalWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL );
		~CExpEvalWindow();


		// == Enumerations.
		// Images.
		enum MX_IMAGES {
			MX_I_ADD,
			MX_I_REMOVE,

			MX_I_TOTAL
		};

		


		// == Functions.
		// WM_INITDIALOG.
		virtual LSW_HANDLED					InitDialog();

		// WM_COMMAND from control.
		virtual LSW_HANDLED					Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc );

		// Virtual client rectangle.  Can be used for things that need to be adjusted based on whether or not status bars, toolbars, etc. are present.
		virtual const LSW_RECT				VirtualClientRect( const CWidget * pwChild ) const;

		// Gets a pointer to the list view.
		CListView *							ListView();

		// Gets a pointer to the result edit.
		CEdit *								Edit();


	protected :
		// == Members.
		// Image list.
		CImageList							m_iImages;

		// Images.
		CBitmap								m_bBitmaps[MX_I_TOTAL];

		// Image mapping.
		INT									m_iImageMap[MX_I_TOTAL];


		// == Functions.
		// WM_CTLCOLORSTATIC.
		virtual LSW_HANDLED					CtlColorStatic( HDC _hDc, CWidget * _pwControl, HBRUSH &_hBrush );

	};

}	// namespace mx
