#pragma once
#include "../MXMhsX.h"
#include "../Hotkeys/MXHotkeyManager.h"
#include "../Options/MXOptions.h"
#include "../Programs/MXPrograms.h"
#include "../RegEx/MXOnigurumaSystem.h"
#include "../Search/MXSearcher.h"
#include "../Utilities/MXStream.h"
#include <CriticalSection/LSWCriticalSection.h>
#include "MXProcess.h"
#include <LSONJson.h>
#include <LSWWin.h>
#include <Helpers/LSWHelpers.h>


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// Creates a number JSON element.
#define MX_JSON_NUMBER( NAME, NUMBER, JSON )																																							\
	(JSON)->vObjectMembers.push_back( std::make_unique<lson::CJson::LSON_ELEMENT>() );																													\
	lson::CJson::CreateNumberElement( (NAME), static_cast<double>(NUMBER), (*(JSON)->vObjectMembers[(JSON)->vObjectMembers.size()-1]) )

// Creates a true or false element.
#define MX_JSON_BOOL( NAME, VAL, JSON )																																									\
	(JSON)->vObjectMembers.push_back( std::make_unique<lson::CJson::LSON_ELEMENT>() );																													\
	if ( VAL ) {																																														\
		lson::CJson::CreateTrueElement( (NAME), (*(JSON)->vObjectMembers[(JSON)->vObjectMembers.size()-1]) );																							\
	}																																																	\
	else {																																																\
		lson::CJson::CreateFalseElement( (NAME), (*(JSON)->vObjectMembers[(JSON)->vObjectMembers.size()-1]) );																							\
	}

// Gets a numbered JSON element.
#define MX_JSON_GET_NUMBER( NAME, STORETO, TYPE, LOCAL, PARENT )																																		\
	LOCAL = _pjcContainer->GetMemberByName( (*PARENT), (NAME) );																																		\
	if ( LOCAL && LOCAL->vtType == lson::CJsonContainer::LSON_VT_DECIMAL ) {																															\
		STORETO = static_cast<TYPE>(LOCAL->u.dDecimal);																																					\
	}

// Gets a boolean JSON element.
#define MX_JSON_GET_BOOL( NAME, STORETO, LOCAL, PARENT )																																				\
	LOCAL = _pjcContainer->GetMemberByName( (*PARENT), (NAME) );																																		\
	if ( LOCAL && (LOCAL->vtType == lson::CJsonContainer::LSON_VT_TRUE || LOCAL->vtType == lson::CJsonContainer::LSON_VT_FALSE) ) {																		\
		STORETO = LOCAL->vtType == lson::CJsonContainer::LSON_VT_TRUE ? TRUE : FALSE;																													\
	}

#define MX_JSON_GET_STRING( NAME, STORETO, LOCAL, PARENT )																																				\
	LOCAL = _pjcContainer->GetMemberByName( (*PARENT), (NAME) );																																		\
	if ( LOCAL && LOCAL->vtType == lson::CJsonContainer::LSON_VT_STRING ) {																																\
		CSecureString ssTmp;																																											\
		CUtilities::ResolveAllEscapes( _pjcContainer->GetString( LOCAL->u.stString ), ssTmp, true );																									\
		STORETO = ee::CExpEval::ToUtf16( ssTmp );																																						\
	}

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
		virtual void						Detach( BOOL _bCallProcessDetach );

		// Gets the options.
		const MX_OPTIONS &					Options() const;

		// Gets the Floating-Point Studio options for read and write.
		MX_FLOAT_STUDIO_OPTIONS &			FloatStudioOptions();

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

		// Locks programs for modification.
		void								LockPrograms() const { m_csProgramCrit.EnterCriticalSection(); }

		// Unlocks programs for modification.
		void								UnlockPrograms() const { m_csProgramCrit.LeaveCriticalSection(); }

		// Gets a reference to the programs.
		std::vector<MX_PROGRAM> &			Programs() { return m_vPrograms; }

		// Gets a const reference to the programs.
		const std::vector<MX_PROGRAM> &		Programs() const { return m_vPrograms; }

		// Executes a program by index.
		bool								ExecuteProgramByIdx( size_t _stIdx );

		// Updates the window title.
		virtual void						UpdateWindowTitle() {}

		// Saves all program settings.
		virtual bool						SaveSettings( const std::wstring &_wsPath, bool _bAsJson ) const;

		// Loads all program settings.
		virtual bool						LoadSettings( const std::wstring &_wsPath, bool _bAsJson );

		// Saves to JSON format if _peJson is not nullptr, otherwise it saves to binary stored in _psBinary.
		virtual bool						SaveSettings( lson::CJson::LSON_ELEMENT * _peJson, CStream * _psBinary, const MX_OPTIONS &_oOptions ) const;

		// Loads settings from either a JSON object or a byte buffer.
		virtual bool						LoadSettings( lson::CJson * _pjJson, CStream * _psBinary, MX_OPTIONS &_oOptions );

		// The address reader for expressions.
		static bool __stdcall				ExpAddressHandler( uint64_t _ui64Address, ee::EE_CAST_TYPES _ctType, uintptr_t _uiptrData, ee::CExpEvalContainer * _peecContainer, ee::CExpEvalContainer::EE_RESULT &_rResult );

		// The address writer for expressions.
		static bool __stdcall				ExpAddressWriteHandler( uint64_t _ui64Address, ee::EE_CAST_TYPES _ctType, uintptr_t _uiptrData, ee::CExpEvalContainer * _peecContainer, ee::CExpEvalContainer::EE_RESULT &_rResult );

		


	protected :
		// == Members.
		// The process.
		CProcess							m_pProcess;
		// Options.
		mutable MX_OPTIONS					m_oOptions;
		// Floating-Point Studio options.
		MX_FLOAT_STUDIO_OPTIONS				m_fsoOptions;
		// Searcher.
		CSearcher							m_sSearcher;
		// Regex system.
		COnigurumaSystem					m_osRegexSystem;
		// Hotkeys.
		CHotkeyManager						m_hmHotkeys;
		// Programs.
		std::vector<MX_PROGRAM>				m_vPrograms;
		// Program critical section.
		mutable CCriticalSection			m_csProgramCrit;


		// == Functions.
		// Saves to JSON format if _peJson is not nullptr, otherwise it saves to binary stored in _psBinary.
		virtual bool						SaveGeneralSettings( lson::CJson::LSON_ELEMENT * _peJson, CStream * _psBinary, const MX_OPTIONS &_oOptions ) const;

		// Loads general settings from either a JSON object or a byte buffer.
		virtual bool						LoadGeneralSettings( const lson::CJsonContainer::LSON_JSON_VALUE * _pjJson, lson::CJsonContainer * _pjcContainer, CStream * _psBinary, MX_OPTIONS &_oOptions, uint32_t _ui32Version );

		// Saves to JSON format if _peJson is not nullptr, otherwise it saves to binary stored in _psBinary.
		virtual bool						SaveOpenProcSettings( lson::CJson::LSON_ELEMENT * _peJson, CStream * _psBinary, const MX_OPTIONS &_oOptions ) const;

		// Loads open-process settings from either a JSON object or a byte buffer.
		virtual bool						LoadOpenProcSettings( const lson::CJsonContainer::LSON_JSON_VALUE * _pjJson, lson::CJsonContainer * _pjcContainer, CStream * _psBinary, MX_OPTIONS &_oOptions, uint32_t _ui32Version );

		// Saves to JSON format if _peJson is not nullptr, otherwise it saves to binary stored in _psBinary.
		virtual bool						SaveSearchSettings( lson::CJson::LSON_ELEMENT * _peJson, CStream * _psBinary, const MX_OPTIONS &_oOptions ) const;

		// Loads search settings from either a JSON object or a byte buffer.
		virtual bool						LoadSearchSettings( const lson::CJsonContainer::LSON_JSON_VALUE * _pjJson, lson::CJsonContainer * _pjcContainer, CStream * _psBinary, MX_OPTIONS &_oOptions, uint32_t _ui32Version );
	};

}	// namespace mx

