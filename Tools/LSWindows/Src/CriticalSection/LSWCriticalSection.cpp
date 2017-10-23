#include "LSWCriticalSection.h"

namespace lsw {

	CCriticalSection::CCriticalSection() {
		::InitializeCriticalSection( &m_csCrit );
	}
	CCriticalSection::~CCriticalSection() {
		::DeleteCriticalSection( &m_csCrit );
	}

	// == Functions.
	// Enters the critical section.
	VOID CCriticalSection::EnterCriticalSection() {
		::EnterCriticalSection( &m_csCrit );
	}

	// Leaves the critical section.
	VOID CCriticalSection::LeaveCriticalSection() {
		::LeaveCriticalSection( &m_csCrit );
	}

}	// namespace lsw
