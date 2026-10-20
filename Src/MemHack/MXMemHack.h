#pragma once

#include "../MXMhsX.h"
#include "../FoundAddresses/MXFoundAddressManager.h"
#include "../Hotkeys/MXHotkeyManager.h"
#include "../Options/MXOptions.h"
#include "../Programs/MXPrograms.h"
#include "../RegEx/MXOnigurumaSystem.h"
#include "../Search/MXSearcher.h"
#include "../Utilities/MXStream.h"
#include "MXProcess.h"

#include <CriticalSection/LSWCriticalSection.h>
#include <LSONJson.h>
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
		virtual void						Detach( BOOL _bCallProcessDetach );

		/**
		 * Called (on the main thread) when the attached process closes on its own.  Detaches from it and marks it as waiting to be re-attached
		 *	(see CanSearchForReattach()).
		 *
		 * \param _dwId The ID of the process that closed.
		 * \return Returns true if _dwId is the attached process and it was detached.  Returns false if the notification is stale.
		 */
		virtual bool						ProcessClosed( DWORD _dwId );

		/**
		 * Determines whether the closed process should be searched for now: it closed and nothing has been opened since, the automatic re-attach
		 *	option is enabled, no process is open, and its path or file name is known.  The information needed for the search is kept whether
		 *	or not the option is enabled, so enabling the option later starts the search.
		 *
		 * \return Returns true if FindReattachProcess() should be called periodically.
		 */
		bool								CanSearchForReattach() const;

		/**
		 * Searches the running processes for the closed process.  If the Open Process "Path" option is enabled and the closed process's full
		 *	path is known, a process matches by full path (only processes with the same file name are opened to check their paths).  Otherwise
		 *	a process matches by file name alone (no process is opened).
		 *
		 * \return Returns the ID of the first matching process or DWINVALID if there is none.
		 */
		DWORD								FindReattachProcess();

		/**
		 * Records that a process found by FindReattachProcess() could not be opened, so that the search skips it from then on.
		 *
		 * \param _dwId The ID of the process that could not be opened.
		 */
		void								ReattachFailed( DWORD _dwId );

		// Gets the options.
		const MX_OPTIONS &					Options() const;

		// Gets the Floating-Point Studio options for read and write.
		MX_FLOAT_STUDIO_OPTIONS &			FloatStudioOptions();

		// Sets the options.
		virtual void						SetOptions( const MX_OPTIONS &_oOptions );

		// Gets the process.
		const CProcess &					Process() const { return m_pProcess; }

		// Gets the process.
		CProcess &							Process() { return m_pProcess; }

		// Gets the searcher.
		CSearcher &							Searcher() { return m_sSearcher; }

		// Gets the searcher.
		const CSearcher &					Searcher() const { return m_sSearcher; }

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

		// Gets the Found Address Manager.
		CFoundAddressManager::MX_LOCK		FoundAddressManager() { return CFoundAddressManager::MX_LOCK( &m_famFoundAddresses ); }

		// Applies the lock to all timer-based locked Found Addresses.
		void								ApplyFoundAddressLocks( bool _bAllLocked = false );

		// Executes a program by index.
		bool								ExecuteProgramByIdx( size_t _stIdx );

		// Updates the window title.
		virtual void						UpdateWindowTitle() {}

		//// Saves all program settings.
		//virtual bool						SaveSettings( const std::wstring &_wsPath, bool _bAsJson ) const;

		//// Loads all program settings.
		//virtual bool						LoadSettings( const std::wstring &_wsPath, bool _bAsJson );

		// Saves to JSON format if _peJson is not nullptr, otherwise it saves to binary stored in _psBinary.
		virtual bool						SaveSettings( lson::CJson::LSON_ELEMENT * _peJson, CStream * _psBinary, const MX_OPTIONS &_oOptions ) const;

		// Loads settings from either a JSON object or a byte buffer.
		virtual bool						LoadSettings( lson::CJson * _pjJson, CStream * _psBinary, MX_OPTIONS &_oOptions );

		// Reads data from an area of memory in the current process. The entire area to be read must be accessible or the operation fails.
		// Preprocesses the data (applies byteswapping), which means an area larger than the requested size must be read.  _sBufferOffset returns the offset into _vBuffer where the requested data is actually stored.
		virtual bool						ReadProcessMemory_PreProcessed( uint64_t _ui64BaseAddress, std::vector<uint8_t> &_vBuffer, SIZE_T _nSize, size_t &_sBufferOffset, SIZE_T * _lpNumberOfBytesRead = nullptr ) const {
			return Process().ReadProcessMemory_PreProcessed( _ui64BaseAddress, _vBuffer, _nSize, _sBufferOffset, static_cast<CUtilities::MX_BYTESWAP>(Options().bsByteswap), _lpNumberOfBytesRead );
		}

		// Reads data from an area of memory in the current process. The entire area to be read must be accessible or the operation fails.
		// Preprocesses the data (applies byteswapping), which means an area larger than the requested size must be read.  _sBufferOffset returns the offset into _vBuffer where the requested data is actually stored.
		virtual bool						ReadProcessMemory_PreProcessed( uint64_t _ui64BaseAddress, LPVOID _lpvBuffer, SIZE_T _nSize, size_t &_sBufferOffset, SIZE_T * _lpNumberOfBytesRead = nullptr ) const {
			return Process().ReadProcessMemory_PreProcessed( _ui64BaseAddress, _lpvBuffer, _nSize, _sBufferOffset, static_cast<CUtilities::MX_BYTESWAP>(Options().bsByteswap), _lpNumberOfBytesRead );
		}

		// Writes data to an area of memory in the current process.  If the data is preprocessed and misaligned, it could result in up to 3 writes to memory.
		virtual bool						WriteProcessMemory_PreProcessed( uint64_t _ui64BaseAddress, const LPVOID _lpvBuffer, SIZE_T _nSize, SIZE_T * _lpNumberOfBytesWritten = nullptr ) {
			return Process().WriteProcessMemory_PreProcessed( _ui64BaseAddress, _lpvBuffer, _nSize, static_cast<CUtilities::MX_BYTESWAP>(Options().bsByteswap), _lpNumberOfBytesWritten );
		}

		// Writes data to an area of memory in the current process.  If the data is preprocessed and misaligned, it could result in up to 3 writes to memory.
		virtual bool						WriteProcessMemory_PreProcessed( uint64_t _ui64BaseAddress, const std::vector<uint8_t> &_vBuffer, SIZE_T * _lpNumberOfBytesWritten = nullptr ) {
			return Process().WriteProcessMemory_PreProcessed( _ui64BaseAddress, reinterpret_cast<LPCVOID>(_vBuffer.data()), _vBuffer.size(), static_cast<CUtilities::MX_BYTESWAP>(Options().bsByteswap), _lpNumberOfBytesWritten );
		}

		// Writes data to an area of memory in the current process.  If the data is preprocessed and misaligned, it could result in up to 3 writes to memory.
		virtual bool						WriteProcessMemory_PreProcessed( uint64_t _ui64BaseAddress, const std::vector<uint8_t> &_vBuffer, CUtilities::MX_BYTESWAP _bsSwap, SIZE_T * _lpNumberOfBytesWritten = nullptr ) {
			return Process().WriteProcessMemory_PreProcessed( _ui64BaseAddress, reinterpret_cast<LPCVOID>(_vBuffer.data()), _vBuffer.size(), _bsSwap, _lpNumberOfBytesWritten );
		}

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
		// The list of found addresses.
		CFoundAddressManager				m_famFoundAddresses;
		/** The full path of the last process opened, or empty if it is not known. */
		CSecureWString						m_wsReattachPath;
		/** The file name of the last process opened, or empty if it is not known. */
		CSecureWString						m_wsReattachName;
		/** The ID of the last process opened. */
		DWORD								m_dwReattachId = DWINVALID;
		/** Set when the last process opened closes; cleared when any process is opened. */
		bool								m_bReattachPending = false;
		/** Processes matching the closed process that could not be opened. */
		std::set<DWORD>						m_sReattachSkip;


		// == Functions.
		/**
		 * Gets the file name of a running process from a process snapshot (the process is not opened).
		 *
		 * \param _dwId The ID of the process.
		 * \return Returns the process's file name or an empty string if it is not found.
		 */
		static CSecureWString				ProcessNameById( DWORD _dwId );

		/**
		 * Gets the full path of a process.
		 *
		 * \param _hProcess A handle to the process with PROCESS_QUERY_LIMITED_INFORMATION access.
		 * \return Returns the full path of the process or an empty string on failure.
		 */
		static CSecureWString				ImagePath( HANDLE _hProcess );

		/**
		 * Compares two strings ordinally, ignoring case (as the file system compares file names).
		 *
		 * \param _pwcLeft The left string.
		 * \param _wsRight The right string.
		 * \return Returns true if the strings are equal, ignoring case.
		 */
		static bool							EqualNoCase( const wchar_t * _pwcLeft, const std::wstring &_wsRight );

		/**
		 * Compares two strings ordinally, ignoring case (as the file system compares file names).
		 *
		 * \param _wsLeft The left string.
		 * \param _wsRight The right string.
		 * \return Returns true if the strings are equal, ignoring case.
		 */
		static bool							EqualNoCase( const std::wstring &_wsLeft, const std::wstring &_wsRight );

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

		// Saves to JSON format if _peJson is not nullptr, otherwise it saves to binary stored in _psBinary.
		virtual bool						SaveEditSettings( lson::CJson::LSON_ELEMENT * _peJson, CStream * _psBinary, const MX_OPTIONS &_oOptions ) const;

		// Loads edit settings from either a JSON object or a byte buffer.
		virtual bool						LoadEditSettings( const lson::CJsonContainer::LSON_JSON_VALUE * _pjJson, lson::CJsonContainer * _pjcContainer, CStream * _psBinary, MX_OPTIONS &_oOptions, uint32_t _ui32Version );
	};

}	// namespace mx

