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
	VOID CCriticalSection::EnterCriticalSection() const {
		::EnterCriticalSection( const_cast<LPCRITICAL_SECTION>(&m_csCrit) );
	}

	// Leaves the critical section.
	VOID CCriticalSection::LeaveCriticalSection() const {
		::LeaveCriticalSection( const_cast<LPCRITICAL_SECTION>(&m_csCrit) );
	}

}	// namespace lsw
