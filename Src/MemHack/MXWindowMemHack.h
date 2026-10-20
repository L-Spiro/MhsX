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

		// Updates the window title.
		virtual void						UpdateWindowTitle();

		// Failed to open a process.
		virtual void						FailedToOpenProcess( DWORD _dwId );

		// Detach from the current process.
		virtual void						Detach( BOOL _bCallProcessDetach );

		/**
		 * Called (on the main thread) when the attached process closes on its own.  Detaches from it, reports it in the status bar, and starts
		 *	searching for it to re-open if the automatic re-attach option is enabled.
		 *
		 * \param _dwId The ID of the process that closed.
		 * \return Returns true if _dwId is the attached process and it was detached.  Returns false if the notification is stale.
		 */
		virtual bool						ProcessClosed( DWORD _dwId );

		// Sets the options.
		virtual void						SetOptions( const MX_OPTIONS &_oOptions );

		// Adds user programs to the menu.
		void								AddUserProgramsToMenu();

		// Gets a pointer to the window.
		CMhsMainWindow *					Window() { return m_pmmwWindow; }


	protected :
		// == Members.
		// The window.
		CMhsMainWindow *					m_pmmwWindow;


		// == Functions.
		// The process detatch callback.
		static void							DetatchCallback( void * _pvParm1,  uintptr_t _uiptrParm2 );

		/**
		 * The process-closed callback.  Called on the process-monitoring thread the moment the attached process closes.
		 *
		 * \param _pvParm1 The CWindowMemHack object.
		 * \param _uiptrId The ID of the process that closed.
		 */
		static void							ClosedCallback( void * _pvParm1, uintptr_t _uiptrId );


	private :
		typedef CMemHack					Parent;
	};

}	// namespace mx