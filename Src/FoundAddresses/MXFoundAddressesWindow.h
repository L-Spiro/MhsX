#pragma once

#include "../MXMhsX.h"

#include <Docking/LSWDockable.h>
#include <ImageList/LSWImageList.h>
#include <Images/LSWBitmap.h>
#include <ListView/LSWListView.h>

using namespace lsw;

namespace mx {
	
	class CFoundAddressesWindow : public lsw::CDockable {
	public :
		CFoundAddressesWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL );
		~CFoundAddressesWindow();


		// == Enumerations.
		// Images.
		enum MX_IMAGES {
			MX_I_SEARCH,
			MX_I_SUBSEARCH,
			MX_I_ADD,

			MX_I_TOTAL
		};

		


		// == Functions.
		// WM_INITDIALOG.
		virtual LSW_HANDLED					InitDialog();

		// Virtual client rectangle.  Can be used for things that need to be adjusted based on whether or not status bars, toolbars, etc. are present.
		virtual const LSW_RECT				VirtualClientRect( const CWidget * pwChild ) const;

		// Gets a pointer to the list view.
		CListView *							ListView();


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
