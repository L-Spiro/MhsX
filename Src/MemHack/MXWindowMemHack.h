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

		/**
		 * Updates the window title.
		 **/
		virtual void						UpdateWindowTitle();

		// Failed to open a process.
		virtual void						FailedToOpenProcess( DWORD _dwId );

		// Detach from the current process.
		virtual void						Detach();

		// Sets the options.
		virtual void						SetOptions( const MX_OPTIONS &_oOptions );

		// Adds user programs to the menu.
		void								AddUserProgramsToMenu();



	protected :
		// == Members.
		// The window.
		CMhsMainWindow *					m_pmmwWindow;


	private :
		typedef CMemHack					Parent;
	};

}	// namespace mx