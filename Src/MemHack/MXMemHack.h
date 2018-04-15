#pragma once
#include "../MXMhsX.h"
#include "../Options/MXOptions.h"
#include "MXProcess.h"
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
		virtual bool						OpenProcess( DWORD _dwId );

		// Opened a process.
		virtual void						OpenedProcess();

		// Failed to open a process.
		virtual void						FailedToOpenProcess( DWORD _dwId );

		// Detach from the current process.
		virtual void						Detach();

		// Gets the options.
		const MX_OPTIONS &					Options() const;

		// Sets the options.
		void								SetOptions( const MX_OPTIONS &_oOptions );

		// Gets the process.
		const CProcess &					Process() const { return m_pProcess; }

		


	protected :
		// == Members.
		// The process.
		CProcess							m_pProcess;

		// Options.
		mutable MX_OPTIONS					m_oOptions;
	};

}	// namespace mx

