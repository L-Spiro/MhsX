#include "MXWindowMemHack.h"
#include "../Layouts/MXMainWindowLayout.h"
#include "../Layouts/MXLayoutManager.h"
#include <Base/LSWBase.h>


namespace mx {

	CWindowMemHack::CWindowMemHack() {
		mx::CMainWindowLayout::CreateMainWindow( this );
	}
	CWindowMemHack::~CWindowMemHack() {
	}

}	// namespace mx