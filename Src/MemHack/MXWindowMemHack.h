#pragma once
#include "../MXMhsX.h"
#include "MXMemHack.h"
#include <LSWWin.h>


namespace mx {

	class CMhsMainWindow;

	// Base class for the MHS object.
	class CWindowMemHack : public CMemHack {
	public :
		CWindowMemHack();
		~CWindowMemHack();


		// == Functions.
		// Opened a process.
		virtual void						OpenedProcess();

		// Failed to open a process.
		virtual void						FailedToOpenProcess( DWORD _dwId );

		// Sets the options.
		virtual void						SetOptions( const MX_OPTIONS &_oOptions );



	protected :
		// == Members.
		// The window.
		CMhsMainWindow *					m_pmmwWindow;


	private :
		typedef CMemHack					Parent;
	};

}	// namespace mx