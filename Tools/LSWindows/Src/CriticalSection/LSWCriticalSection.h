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
			CEnterCrit( CCriticalSection &_csCrit ) :
				m_pcsCrit( &_csCrit ) {
				m_pcsCrit->EnterCriticalSection();
			}
			~CEnterCrit() {
				m_pcsCrit->LeaveCriticalSection();
			}


		protected :
			// == Members.
			// The critical section.
			CCriticalSection *					m_pcsCrit;
		};


		// == Functions.
		// Enters the critical section.
		VOID									EnterCriticalSection();

		// Leaves the critical section.
		VOID									LeaveCriticalSection();

	protected :
		// == Members.
		// The critical section.
		CRITICAL_SECTION						m_csCrit;
	};

}	// namespace lsw
