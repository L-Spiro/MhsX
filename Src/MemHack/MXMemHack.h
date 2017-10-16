#pragma once
#include <LSWWin.h>
#include <Helpers/LSWHelpers.h>


namespace mx {

	// Base class for the MHS object.
	class CMemHack {
	public :
		CMemHack();
		~CMemHack();


		// == Functions.
		// Open a process by its ID.
		bool								OpenProcess( DWORD _dwId );

		// Detach from the current process.
		void								Detach();


	protected :
		// == Members.
		// The process handle.
		lsw::LSW_HANDLE						m_hProc;

		// The process ID.
		DWORD								m_dwProcId;
	};

}	// namespace mx

