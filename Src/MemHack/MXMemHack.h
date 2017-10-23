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
		bool								OpenProcess( DWORD _dwId );

		// Detach from the current process.
		void								Detach();

		// Gets the options.
		const MX_OPTIONS &					Options() const { return m_oOptions; }

		// Sets the options.
		void								SetOptions( const MX_OPTIONS &_oOptions ) { m_oOptions = _oOptions; }


	protected :
		// == Members.
		// The process.
		CProcess							m_pProcess;

		// Options.
		MX_OPTIONS							m_oOptions;
	};

}	// namespace mx

