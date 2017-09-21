#pragma once

#include <Layout/LSWLayoutManager.h>

using namespace lsw;

namespace mx {

	enum MX_LAYOUT_TYPES : DWORD {
		MX_MAIN_WINDOW				= LSW_LT_CUSTOM,
		MX_OPEN_PROCESS_WINDOW,
	};

	class CLayoutManager : public lsw::CLayoutManager {
	public :
		// == Functions.
		// Creates a class based on its type.
		virtual CWidget *				CreateWidget( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget );
	};

}	// namespace mx
