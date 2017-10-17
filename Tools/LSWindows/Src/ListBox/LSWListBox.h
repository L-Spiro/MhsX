#pragma once

#include "../LSWWin.h"
#include "../Layout/LSWWidgetLayout.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CListBox : public CWidget {
	public :
		CListBox( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL );


		// == Functions.
		// Adds a string to the list box.  The return value is the zero-based index of the string in the list box. If an error occurs,
		//	the return value is LB_ERR. If there is insufficient space to store the new string, the return value is LB_ERRSPACE.
		INT									AddString( LPCSTR _lpStr );

		// Adds a string to the list box.  The return value is the zero-based index of the string in the list box. If an error occurs,
		//	the return value is LB_ERR. If there is insufficient space to store the new string, the return value is LB_ERRSPACE.
		INT									AddString( LPCWSTR _lpStr );



	protected :
		// == Functions.

	private :
		typedef CWidget						Parent;
	};
		
}	// namespace lsw
