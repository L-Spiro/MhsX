#pragma once

#include "Oniguruma/oniguruma.h"

namespace mx {

	class COnigurumaRegion {
	public :
		COnigurumaRegion() :
			m_porRegion( ::onig_region_new() ) {
		}
		~COnigurumaRegion() {
			::onig_region_free( m_porRegion, 1 );
		}


		// == Functions.
		// Gets the refion pointer.
		OnigRegion *			Get() { return m_porRegion; }


	protected :
		// == Members.
		// The region.
		OnigRegion *			m_porRegion;
	};

}	// namespace mx
