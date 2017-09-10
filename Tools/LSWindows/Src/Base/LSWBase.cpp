#include "LSWBase.h"

namespace lsw {

	// == Functions.
	ATOM CBase::RegisterClassExW( const CWndClassEx &_wceClss ) {
		return ::RegisterClassExW( &_wceClss.Obj() );
	}

}	// namespace lsw
