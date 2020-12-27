#pragma once

#include "../LSWWin.h"

#define LSW_ENT_CRIT( OBJ )						lsw::CCriticalSection::CEnterCrit ecCrit( OBJ )

namespace lsw {

	class CCriticalSection {
	public :
		CCriticalSection();
		~CCriticalSection();


		// == Types.
		class CEnterCrit {
		public :
			CEnterCrit( const CCriticalSection &_csCrit ) :
				m_pcsCrit( &_csCrit ) {
				m_pcsCrit->EnterCriticalSection();
			}
			~CEnterCrit() {
				m_pcsCrit->LeaveCriticalSection();
				m_pcsCrit = nullptr;
			}


		protected :
			// == Members.
			// The critical section.
			const CCriticalSection *			m_pcsCrit;
		};


		// == Functions.
		// Enters the critical section.
		VOID									EnterCriticalSection() const;

		// Leaves the critical section.
		VOID									LeaveCriticalSection() const;

	protected :
		// == Members.
		// The critical section.
		CRITICAL_SECTION						m_csCrit;
	};

}	// namespace lsw
