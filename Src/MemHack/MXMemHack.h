#pragma once
#include "../MXMhsX.h"
#include "../Hotkeys/MXHotkeyManager.h"
#include "../Options/MXOptions.h"
#include "../RegEx/MXOnigurumaSystem.h"
#include "../Search/MXSearcher.h"
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
		virtual void						SetOptions( const MX_OPTIONS &_oOptions );

		// Gets the process.
		const CProcess &					Process() const { return m_pProcess; }

		// Gets the searcher.
		CSearcher &							Searcher() { return m_sSearcher; }

		// Gets the hotkey manager.
		CHotkeyManager &					HotkeyManager() { return m_hmHotkeys; }

		// Gets the hotkey manager.
		const CHotkeyManager &				HotkeyManager() const { return m_hmHotkeys; }

		// The address reader for expressions.
		static bool __stdcall				ExpAddressHandler( uint64_t _ui64Address, ee::EE_CAST_TYPES _tType, uintptr_t _uiptrData, ee::CExpEvalContainer * _peecContainer, ee::CExpEvalContainer::EE_RESULT &_rResult );

		


	protected :
		// == Members.
		// The process.
		CProcess							m_pProcess;
		// Options.
		mutable MX_OPTIONS					m_oOptions;
		// Searcher.
		CSearcher							m_sSearcher;
		// Regex system.
		COnigurumaSystem					m_osRegexSystem;
		// Hotkeys.
		CHotkeyManager						m_hmHotkeys;
	};

}	// namespace mx

