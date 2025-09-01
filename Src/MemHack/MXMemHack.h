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
		virtual bool						ReadProcessMemory_PreProcessed( uint64_t _ui64BaseAddress, std::vector<uint8_t> &_vBuffer, SIZE_T _nSize, size_t &_sBufferOffset, SIZE_T * _lpNumberOfBytesRead = nullptr ) const;

		// Reads data from an area of memory in the current process. The entire area to be read must be accessible or the operation fails.
		// Preprocesses the data (applies byteswapping), which means an area larger than the requested size must be read.  _sBufferOffset returns the offset into _vBuffer where the requested data is actually stored.
		virtual bool						ReadProcessMemory_PreProcessed( uint64_t _ui64BaseAddress, LPVOID _lpvBuffer, SIZE_T _nSize, size_t &_sBufferOffset, SIZE_T * _lpNumberOfBytesRead = nullptr ) const;

		// Reads data from an area of memory in the current process. The entire area to be read must be accessible or the operation fails.
		// Preprocesses the data (applies byteswapping), which means an area larger than the requested size must be read.  _sBufferOffset returns the offset into _vBuffer where the requested data is actually stored.
		virtual bool						ReadProcessMemory_PreProcessed( uint64_t _ui64BaseAddress, std::vector<uint8_t> &_vBuffer, SIZE_T _nSize, size_t &_sBufferOffset, CUtilities::MX_BYTESWAP _bsSwap, SIZE_T * _lpNumberOfBytesRead = nullptr ) const;

		// Reads data from an area of memory in the current process. The entire area to be read must be accessible or the operation fails.
		// Preprocesses the data (applies byteswapping), which means an area larger than the requested size must be read.  _sBufferOffset returns the offset into _vBuffer where the requested data is actually stored.
		virtual bool						ReadProcessMemory_PreProcessed( uint64_t _ui64BaseAddress, LPVOID _lpvBuffer, SIZE_T _nSize, size_t &_sBufferOffset, CUtilities::MX_BYTESWAP _bsSwap, SIZE_T * _lpNumberOfBytesRead = nullptr ) const;

		// Writes data to an area of memory in the current process.  If the data is preprocessed and misaligned, it could result in up to 3 writes to memory.
		virtual bool						WriteProcessMemory_PreProcessed( uint64_t _ui64BaseAddress, const LPVOID _lpvBuffer, SIZE_T _nSize, SIZE_T * _lpNumberOfBytesWritten = nullptr ) {
			return WriteProcessMemory_PreProcessed( _ui64BaseAddress, _lpvBuffer, _nSize, Searcher().LastSearchParms().bsByteSwapping, _lpNumberOfBytesWritten );
		}

		// Writes data to an area of memory in the current process.  If the data is preprocessed and misaligned, it could result in up to 3 writes to memory.
		virtual bool						WriteProcessMemory_PreProcessed( uint64_t _ui64BaseAddress, const std::vector<uint8_t> &_vBuffer, SIZE_T * _lpNumberOfBytesWritten = nullptr ) {
			return WriteProcessMemory_PreProcessed( _ui64BaseAddress, reinterpret_cast<LPCVOID>(_vBuffer.data()), _vBuffer.size(), Searcher().LastSearchParms().bsByteSwapping, _lpNumberOfBytesWritten );
		}

		// Writes data to an area of memory in the current process.  If the data is preprocessed and misaligned, it could result in up to 3 writes to memory.
		virtual bool						WriteProcessMemory_PreProcessed( uint64_t _ui64BaseAddress, const std::vector<uint8_t> &_vBuffer, CUtilities::MX_BYTESWAP _bsSwap, SIZE_T * _lpNumberOfBytesWritten = nullptr ) {
			return WriteProcessMemory_PreProcessed( _ui64BaseAddress, reinterpret_cast<LPCVOID>(_vBuffer.data()), _vBuffer.size(), _bsSwap, _lpNumberOfBytesWritten );
		}

		// Writes data to an area of memory in the current process.  If the data is preprocessed and misaligned, it could result in up to 3 writes to memory.
		virtual bool						WriteProcessMemory_PreProcessed( uint64_t _ui64BaseAddress, LPCVOID _lpvBuffer, SIZE_T _nSize, CUtilities::MX_BYTESWAP _bsSwap, SIZE_T * _lpNumberOfBytesWritten = nullptr );

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

		// Saves to JSON format if _peJson is not nullptr, otherwise it saves to binary stored in _psBinary.
		virtual bool						SaveEditSettings( lson::CJson::LSON_ELEMENT * _peJson, CStream * _psBinary, const MX_OPTIONS &_oOptions ) const;

		// Loads edit settings from either a JSON object or a byte buffer.
		virtual bool						LoadEditSettings( const lson::CJsonContainer::LSON_JSON_VALUE * _pjJson, lson::CJsonContainer * _pjcContainer, CStream * _psBinary, MX_OPTIONS &_oOptions, uint32_t _ui32Version );
	};

}	// namespace mx

