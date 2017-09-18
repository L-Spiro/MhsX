#pragma once

#include "../LSWWin.h"

namespace lsw {

	class CGlobalAlloc {
	public :
		CGlobalAlloc( UINT _uFlags, SIZE_T _dwBytes ) :
			m_gGlobal( ::GlobalAlloc( _uFlags, _dwBytes ) ),
			m_dwLock( 0 ) {
		}
		~CGlobalAlloc() {
			while ( m_dwLock-- ) {
				::GlobalUnlock( m_gGlobal );
			}
			if ( m_gGlobal ) {
				::GlobalFree( m_gGlobal );
			}
		}


		// == Functions.
		// Lock the object.
		LPVOID								Lock() const { ++m_dwLock; return ::GlobalLock( m_gGlobal ); }

		// Unlock the object.
		VOID								Unlock() const {
			if ( m_dwLock ) {
				if ( ::GlobalUnlock( m_gGlobal ) ) {
					--m_dwLock;
				}
			}
		}

	protected :
		// == Members.
		// The allocation.
		HGLOBAL								m_gGlobal;

		// Lock count.
		mutable DWORD						m_dwLock;
	};

}	// namespace lsw
