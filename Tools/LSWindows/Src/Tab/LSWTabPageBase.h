#pragma once

#include "../LSWWin.h"
#include "../Layout/LSWWidgetLayout.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CTabPageBase : public CWidget {
	public :
		CTabPageBase( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		// Get the name of the page.
		virtual std::wstring					GetName() const { return std::wstring(); }


	protected :
		// == Functions.

	private :
		typedef CWidget							Parent;
	};
		
}	// namespace lsw
