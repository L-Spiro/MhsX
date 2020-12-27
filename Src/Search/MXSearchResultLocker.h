#pragma once
#include "MXSearchResultBase.h"

namespace mx {

	class CSearchResultLocker {
	public :
		CSearchResultLocker( CSearchResultBase * _psrbBase ) :
			m_srbBase( _psrbBase ) {
			m_srbBase->Lock();
		}
		~CSearchResultLocker() {
			m_srbBase->Unlock();
		}


	protected :
		// == Members.
		CSearchResultBase *		m_srbBase;
	};

}	// namespace mx
