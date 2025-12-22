#pragma once
#include "../MXMhsX.h"
#include "../IO/MXInOutInterface.h"
#include "../Strings/MXSecureWString.h"
#include "../Utilities/MXUtilities.h"

#include <CriticalSection/LSWCriticalSection.h>
#include <Helpers/LSWHelpers.h>
#include <Threads/LSWThread.h.>

#include <set>
#include <map>

using namespace lsw;

namespace mx {

	class CProcess : public CInOutInterface {
	public :
		CProcess();
		~CProcess();


		// == Enumerations.
		// Mode to use for opening the process.
		enum MX_OPEN_PROC_MODE {
			MX_OPM_CONSERVATIVE,		// Use as few flags as necessary to work with the process.  Re-opens the process each time a new OpenProcess()
										//	flag needs to be added.  For example, PROCESS_VM_READ will only be used when ReadProcessMemory() is called
										//  for the first time.
			MX_OPM_NORMAL,				// Opens the process using
										//	PROCESS_CREATE_THREAD | PROCESS_VM_READ | PROCESS_VM_WRITE |
										//	PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION | PROCESS_SUSPEND_RESUME
			MX_OPM_FIXED,				// Opens the process using flags specified by the user.
		};


		// == Types.
		// The callback function type for detatch events.
		typedef void					(* PfDetatchCallback)( void *, uintptr_t );

		/**
		 * \brief Half-open address range [ui64Start, ui64Start + ui64Size).
		 *
		 * Optimized membership check uses a single unsigned comparison:
		 *     (addr - start) < size
		 */
		struct MX_ADDRESS_RANGE {
			uint64_t					ui64Start;
			uint64_t					ui64Size;
			

			// == Operators.
			/** Ordering for std::map/set keyed by ranges (start-only, strict-weak). */
			inline bool					operator < ( const MX_ADDRESS_RANGE &_arOther ) const {
				return ui64Start < _arOther.ui64Start;
			}

			inline bool					operator == ( const MX_ADDRESS_RANGE &_arOther ) const {
				return ui64Start == _arOther.ui64Start && ui64Size == _arOther.ui64Size;
			}


			// == Functions.
			/** Returns end (exclusive). */
			inline uint64_t				End() const {
				return ui64Start + ui64Size;
			}

			/** Fast membership test. */
			inline bool					Contains( uint64_t _ui64Addr ) const {
				return (_ui64Addr - ui64Start) < ui64Size;
			}

			/** True if this immediately precedes _arOther (contiguous). */
			inline bool					IsContiguousWith( const MX_ADDRESS_RANGE &_arOther ) const {
				return End() == _arOther.ui64Start;
			}

			/** Extend to include _arOther (assumes contiguous and same backing store). */
			inline void					MergeContiguous( const MX_ADDRESS_RANGE &_arOther ) {
				ui64Size += _arOther.ui64Size;
			}
		};

		/**
		 * \brief Wrapper around MODULEENTRY32W with UTF-8 name/path and set-friendly operators.
		 *
		 * Blank line after the \brief.
		 *
		 * Ordered by case-insensitive module name (file name, not path).
		 * This makes std::set lookups by name O(log N).
		 */
		struct MX_MODULE_ENTRY {
			MODULEENTRY32W				meEntry;				/**< Original Toolhelp module entry. */
			std::string					sNameUtf8;				/**< Module file name (e.g., "kernel32.dll"), UTF-8. */
			std::string					sPathUtf8;				/**< Full path (UTF-8). */
			std::string					sKeyLower;				/**< Lower-cased name for case-insensitive ordering. */


			// == Operators.
			/** std::set ordering: case-insensitive by file name. */
			inline bool					operator < ( const MX_MODULE_ENTRY &_meEntry ) const {
				return sKeyLower < _meEntry.sKeyLower;
			}

			/** Equality by key (case-insensitive name). */
			inline bool					operator == ( const MX_MODULE_ENTRY &_meEntry ) const {
				return sKeyLower == _meEntry.sKeyLower;
			}
			
			
			// == Functions.
			/** Range helpers. */
			inline uint64_t				Start() const {
				return static_cast<uint64_t>(reinterpret_cast<uintptr_t>(meEntry.modBaseAddr));
			}
			inline uint64_t				Size() const {
				return static_cast<uint64_t>(meEntry.modBaseSize);
			}
			inline MX_ADDRESS_RANGE		Range() const {
				return MX_ADDRESS_RANGE { Start(), Size() };
			}
		};


		// == Functions.
		// Open the given process with the given mode and flags.
		virtual bool					OpenProc( DWORD _dwId, MX_OPEN_PROC_MODE _opmMode, DWORD _dwFlags );

		// Detach from the current process, if there is one.
		virtual void					Detach();

		// Gets the process ID.
		DWORD							ProcId() const { LSW_ENT_CRIT( m_csCrit ); return m_dwId; }

		// Gets the handle to the process.
		HANDLE							ProcHandle() const { LSW_ENT_CRIT( m_csCrit ); return m_hProcHandle.hHandle; }

		// Gets the mode.
		MX_OPEN_PROC_MODE				Mode() const { LSW_ENT_CRIT( m_csCrit ); return m_opmMode; }

		// Gets the flags used to open the process.
		DWORD							Flags() const { LSW_ENT_CRIT( m_csCrit ); return m_dwOpenProcFlags; }

		// Is a process open?
		bool							ProcIsOpened() const { LSW_ENT_CRIT( m_csCrit ); return m_hProcHandle.Valid() && m_dwId != DWINVALID && m_opOpenProcThreadMonitor.aAtom == 0; }

		// Can we read from the target process?
		inline bool						Readable() const { LSW_ENT_CRIT( m_csCrit ); return ProcIsOpened() && ((m_dwOpenProcFlags & PROCESS_VM_READ) != 0); }

		// Can we write to the target process?
		inline bool						Writeable() const { LSW_ENT_CRIT( m_csCrit ); return ProcIsOpened() && ((m_dwOpenProcFlags & (PROCESS_VM_WRITE | PROCESS_VM_OPERATION)) == (PROCESS_VM_WRITE | PROCESS_VM_OPERATION)); }

		// Reads data from an area of memory in a specified process. The entire area to be read must be accessible or the operation fails.
		virtual bool					ReadProcessMemory( uint64_t _ui64BaseAddress, LPVOID _lpBuffer, SIZE_T _nSize, SIZE_T * _lpNumberOfBytesRead ) const;

		// Read from the given data stream.
		virtual bool					Read( uint64_t _ui64Addr, void * _pvDest, size_t _sSize, size_t * _psBytesRead = nullptr ) const {
			SIZE_T stRead;
			if ( ReadProcessMemory( _ui64Addr, _pvDest, _sSize, &stRead ) ) {
				if ( _psBytesRead ) {
					(*_psBytesRead) = stRead;
				}
				return true;
			}
			return false;
		}

		// Reads data from an area of memory in the current process. The entire area to be read must be accessible or the operation fails.
		// Preprocesses the data (applies byteswapping), which means an area larger than the requested size must be read.  _sBufferOffset returns the offset into _vBuffer where the requested data is actually stored.
		virtual bool					ReadProcessMemory_PreProcessed( uint64_t _ui64BaseAddress, LPVOID _lpvBuffer, SIZE_T _nSize, size_t &_sBufferOffset, CUtilities::MX_BYTESWAP _bsSwap, SIZE_T * _lpNumberOfBytesRead = nullptr ) const;

		// Reads data from an area of memory in the current process. The entire area to be read must be accessible or the operation fails.
		// Preprocesses the data (applies byteswapping), which means an area larger than the requested size must be read.  _sBufferOffset returns the offset into _vBuffer where the requested data is actually stored.
		virtual bool					ReadProcessMemory_PreProcessed( uint64_t _ui64BaseAddress, std::vector<uint8_t> &_vBuffer, SIZE_T _nSize, size_t &_sBufferOffset, CUtilities::MX_BYTESWAP _bsSwap, SIZE_T * _lpNumberOfBytesRead = nullptr ) const;

		// Writes data to an area of memory in a specified process. The entire area to be written to must be accessible or the operation fails.
		virtual bool					WriteProcessMemory( uint64_t _ui64BaseAddress, LPCVOID _lpBuffer, SIZE_T _nSize, SIZE_T * _lpNumberOfBytesWritten );

		// Write to the given data stream.
		virtual bool					Write( uint64_t _ui64Addr, const void * _pvSrc, size_t _sSize, size_t * _psBytesWritten = nullptr ) {
			SIZE_T stWritten;
			if ( WriteProcessMemory( _ui64Addr, _pvSrc, _sSize, &stWritten ) ) {
				if ( _psBytesWritten ) {
					(*_psBytesWritten) = stWritten;
				}
				return true;
			}
			return false;
		}

		// Writes data to an area of memory in the current process.  If the data is preprocessed and misaligned, it could result in up to 3 writes to memory.
		virtual bool					WriteProcessMemory_PreProcessed( uint64_t _ui64BaseAddress, LPCVOID _lpvBuffer, SIZE_T _nSize, CUtilities::MX_BYTESWAP _bsSwap, SIZE_T * _lpNumberOfBytesWritten = nullptr );

		// Remote std::strlen() and std::strcpy().
		template <typename _tType = std::string>
		inline size_t					RemoteStrLen_StrCpy( uint64_t _ui64BaseAddress, _tType * _psStr, CUtilities::MX_BYTESWAP _bsSwap ) const {
			if MX_UNLIKELY( _ui64BaseAddress > UINTPTR_MAX ) { return 0; }
			using tCharType = typename _tType::value_type;

			size_t sRet = 0;
			try {
				size_t sOffset;
				DWORD dwPageSize = mx::CSystem::GetSystemInfo().dwPageSize;
				uint64_t ui64NextPage = ((_ui64BaseAddress / dwPageSize) + 1) * dwPageSize;	// No need to handle wrap-around.
				size_t sBufferSize = size_t( ui64NextPage - _ui64BaseAddress );
				// If the buffer size is not large enough to hold even a single character, go to the next next page.
				if ( sBufferSize < sizeof( tCharType ) ) {
					ui64NextPage = ((_ui64BaseAddress / dwPageSize) + 2) * dwPageSize;
					sBufferSize = size_t( ui64NextPage - _ui64BaseAddress );
				}
				SIZE_T sRead = 0;
				std::vector<uint8_t> vLocalBuffer;
				while ( ReadProcessMemory_PreProcessed( _ui64BaseAddress, vLocalBuffer, sBufferSize, sOffset, _bsSwap, &sRead ) ) {
					if ( !sRead ) { break; }
					sRead = sRead / sizeof( tCharType ) * sizeof( tCharType );
					_ui64BaseAddress += sRead;
					for ( size_t I = sOffset; I < sRead; I += sizeof( tCharType ) ) {
						if ( !(*reinterpret_cast<tCharType *>(&vLocalBuffer[I])) ) {
							if ( _psStr ) {
								_psStr->append( reinterpret_cast<const tCharType *>(&vLocalBuffer.data()[sOffset]), (I - sOffset) / sizeof( tCharType ) );
							}
							return sRet;
						}
						++sRet;
					}

					// The whole buffer contains no 0's.
					if ( _psStr ) {
						_psStr->append( reinterpret_cast<const tCharType *>(&vLocalBuffer.data()[sOffset]), (sRead - sOffset) / sizeof( tCharType ) );
					}

					ui64NextPage = ((_ui64BaseAddress / dwPageSize) + 1) * dwPageSize;	// No need to handle wrap-around.
					sBufferSize = size_t( ui64NextPage - _ui64BaseAddress );
					// If the buffer size is not large enough to hold even a single character, go to the next next page.
					if ( sBufferSize < sizeof( tCharType ) ) {
						ui64NextPage = ((_ui64BaseAddress / dwPageSize) + 2) * dwPageSize;
						sBufferSize = size_t( ui64NextPage - _ui64BaseAddress );
					}
				}
				sRead = sRead / sizeof( tCharType ) * sizeof( tCharType );
				if ( sRead ) {
					for ( size_t I = sOffset; I < sRead; I += sizeof( tCharType ) ) {
						if ( !(*reinterpret_cast<tCharType *>(&vLocalBuffer[I])) ) {
							if ( _psStr ) {
								_psStr->append( reinterpret_cast<const tCharType *>(&vLocalBuffer.data()[sOffset]), (I - sOffset) / sizeof( tCharType ) );
							}
							return sRet;
						}
						++sRet;
					}
					if ( _psStr ) {
						_psStr->append( reinterpret_cast<const tCharType *>(&vLocalBuffer.data()[sOffset]), (sRead - sOffset) / sizeof( tCharType ) );
					}
				}
			}
			catch ( ... ) {}
			return sRet;
		}

		// Changes the protection on a region of committed pages in the virtual address space of a specified process.
		virtual bool					VirtualProtectEx( uint64_t _ui64Address, SIZE_T _dwSize, DWORD _flNewProtect, PDWORD _lpflOldProtect );

		// Retrieves information about a range of pages within the virtual address space of a specified process.
		virtual bool					VirtualQueryEx( uint64_t _ui64Address, PMEMORY_BASIC_INFORMATION64 _lpBuffer ) const;

		// Determines whether the specified process is running under WOW64.
		virtual bool					IsWow64Process() const;

		// Determines if the process uses a 32-bit addressing space.
		virtual bool					Is32Bit() const;

		// Gets the full path of the process.  Returns an empty string if there is no process opened.
		virtual CSecureWString			QueryProcessImageName( DWORD _dwFlags = 0 ) const;

		// Gets the base and size of a region given an address.
		virtual bool					GetChunk( uint64_t _ui64Address, uint64_t &_uiBaseAddress, uint64_t &_uiRegionSize );

		// Gets the chunks that overlap the given range of addresses.  Call within try/catch.
		virtual std::vector<uint64_t>	GetChunks( uint64_t _ui64StartAddress, uint64_t _ui64EndAddress );

		// Resets all assocations with the current process.
		virtual void					Reset();

		// Pauses the target process.
		virtual LONG					SuspendProcess() const;

		// Resume the target process.
		virtual LONG					ResumeProcess() const;

		// Sets the detatch callback.
		void							SetDetatchCallback( PfDetatchCallback _pfFunc, void * _pvParm1,  uintptr_t _uiptrParm2 );

		/**
		 * \brief Finds the module covering an address, if any. Returns nullptr if not found.
		 */
		inline MODULEENTRY32W			FindModuleForAddress( uint64_t _ui64Addr ) const {
			LSW_ENT_CRIT( m_csCrit );
			if MX_UNLIKELY( m_dwId == DWINVALID ) { return { .dwSize = 0 }; }
			if MX_UNLIKELY( !m_mModuleCache.size() ) {
				m_mModuleCache = GetStaticAddressRangesByPid( m_dwId );
			}
			auto pmeEntry = FindModuleForAddress( m_mModuleCache, _ui64Addr );
			MODULEENTRY32W meNull = { .dwSize = 0 };
			return pmeEntry ? (*pmeEntry) : meNull;
		}

		// Determines if an address is part of a module in the target process.
		inline bool						AddressIsInModule( uint64_t _ui64Addr ) const {
			LSW_ENT_CRIT( m_csCrit );
			if MX_UNLIKELY( m_dwId == DWINVALID ) { return false; }
			if MX_UNLIKELY( !m_mModuleCache.size() ) {
				m_mModuleCache = GetStaticAddressRangesByPid( m_dwId );
			}
			auto pmeEntry = FindModuleForAddress( m_mModuleCache, _ui64Addr );
			return pmeEntry != nullptr;
		}

		// Resets the internal module cache so that it can be freshly generated next time it is accessed.
		inline void						ClearModuleCache() {
			LSW_ENT_CRIT( m_csCrit );
			m_mModuleCache.clear();
			m_sModulesByName.clear();

			m_ui64LastIsReadablePage = 0;
			m_bLastIsReadableResult = false;
		}

		/**
		 * Finds a module by UTF-8 name (case-insensitive). Name is the file name, e.g., "kernel32.dll".
		 *
		 * \param _sModules     Set of modules (ordered by case-insensitive name).
		 * \param _sNameUtf8    Name to find (UTF-8).
		 * \return Returns pointer to the found entry or nullptr if not found.
		 */
		inline MX_MODULE_ENTRY			FindModuleByName( std::string_view _sNameUtf8 ) {
			MX_MODULE_ENTRY meRet;
			meRet.meEntry.dwSize = 0;
			LSW_ENT_CRIT( m_csCrit );
			if MX_UNLIKELY( m_dwId == DWINVALID ) { return meRet; }
			if MX_UNLIKELY( !m_sModulesByName.size() ) {
				GatherModulesByPid( m_dwId, m_sModulesByName );
			}
			auto pmeEntry = FindModuleByName( m_sModulesByName, _sNameUtf8 );
			return pmeEntry ? (*pmeEntry) : meRet;
		}

		/**
		 * \brief Returns true if the page containing the address is committed and readable (not GUARD/NOACCESS).
		 *
		 * \param _ui64Addr Address to test.
		 * \return true if the page looks readable by protection flags; false otherwise.
		 *
		 * \note This is a best-effort check. Protections can change between the query and use.
		 */
		bool							IsReadableByQuery( uint64_t _ui64Addr ) const;

		/**
		 * \brief Probes a single remote address by reading 1 byte.
		 *
		 * \param _ui64Addr Remote virtual address to test.
		 * \return true if a 1-byte read succeeds; false otherwise.
		 *
		 * \note Use a 64-bit build for 64-bit targets to avoid address truncation.
		 */
		bool							IsAddressReadable( uint64_t _ui64Addr ) const;

		/**
		 * \brief Returns true if the page containing the address is committed and writable (not GUARD/NOACCESS).
		 *
		 * \param _ui64Addr Remote address to test (single pointer).
		 * \return Returns true if the containing page is MEM_COMMIT and writable by protection flags; false otherwise.
		 *
		 * \note When State != MEM_COMMIT, Protect/Type are undefined and must be ignored.
		 * \note PAGE_WRITECOPY (and EXECUTE_WRITECOPY) indicate copy-on-write; the first write creates a private copy in the target.
		 *       From a tool perspective, they still count as writable.
		 */
		bool							IsWritableByQuery( uint64_t _ui64Addr ) const;

		/**
		 * \brief Returns true if the page containing the address is committed and executable.
		 *
		 * \param _ui64Addr Remote virtual address to test (single pointer).
		 * \return true if the page is MEM_COMMIT and executable; false otherwise.
		 *
		 * \note When State != MEM_COMMIT, Protect/Type are undefined and must be ignored.
		 * \note Control Flow Guard (CFG) may still block indirect calls/jumps even on executable pages.
		 */
		bool							IsExecutableByQuery( uint64_t _ui64Addr ) const;

		/**
		 * \brief Reads the image base from the target's PEB (robust path).
		 *
		 * \param _hProc Handle to target process opened with PROCESS_QUERY_INFORMATION (or PROCESS_QUERY_LIMITED_INFORMATION) | PROCESS_VM_READ.
		 * \return Base address of the main module on success; nullptr on failure.
		 *
		 * \note Same-bitness path. For cross-bitness (32-bit tool Å® 64-bit target), use the WOW64
		 *       variants (NtWow64QueryInformationProcess64 + NtWow64ReadVirtualMemory64) or build x64.
		 */
		uint64_t						GetMainModuleBase_PEB() const;

		/**
		 * \brief Gets the base address of the main module using ToolHelp (easy path).
		 *
		 * \param _dwPid Target process ID.
		 * \return Base address of the main module on success; nullptr on failure.
		 *
		 * \note Works when caller and target have compatible bitness (x64Å®x64 or x86Å®x86).
		 *       A 32-bit caller cannot enumerate 64-bit modules of a 64-bit target.
		 */
		static uint64_t					GetMainModuleBase_Toolhelp( DWORD _dwPid );

		/**
		 * \brief Gathers static (image) ranges using Toolhelp for a given PID.
		 *
		 * Each module yields one range: [modBaseAddr, modBaseAddr + modBaseSize).
		 * The map value is the MODULEENTRY32W record for that module.
		 *
		 * \param _dwPid            Target process ID.
		 * \param _mOut             Output map of ranges Å® MODULEENTRY32W.
		 * \return Returns true on success (snapshot opened and at least attempted).
		 */
		static inline bool				GatherStaticAddressRangesByPid( DWORD _dwPid,
			std::map<MX_ADDRESS_RANGE, MODULEENTRY32W> &_mOut ) {

			_mOut.clear();

			if ( _dwPid == 0 || DWINVALID == _dwPid ) { return false; }

			LSW_HANDLE hSnap = CSystem::CreateToolhelp32Snapshot( TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, _dwPid );
			if ( !hSnap.Valid()) { return false; }

			MODULEENTRY32W meEntry;
			meEntry.dwSize = sizeof( meEntry );

			if ( !CSystem::Module32FirstW( hSnap.hHandle, &meEntry ) ) { return false; }

			do {
				// Some tools zero-check these; be defensive.
				if ( meEntry.modBaseAddr != nullptr && meEntry.modBaseSize != 0 ) {
					MX_ADDRESS_RANGE arRange {
						static_cast<uint64_t>(reinterpret_cast<uintptr_t>(meEntry.modBaseAddr)),
						static_cast<uint64_t>(meEntry.modBaseSize)
					};

					// Insert; keys are unique by start. If a duplicate start appears, keep the first.
					_mOut.emplace( arRange, meEntry );
				}
				meEntry.dwSize = sizeof( meEntry );
			}
			while ( CSystem::Module32NextW( hSnap.hHandle, &meEntry ) );
			return true;
		}

		/**
		 * \brief Convenience: builds and returns the static range map for a PID.
		 */
		static inline std::map<MX_ADDRESS_RANGE, MODULEENTRY32W>
										GetStaticAddressRangesByPid( DWORD _dwPid ) {
			std::map<MX_ADDRESS_RANGE, MODULEENTRY32W> mOut;
			GatherStaticAddressRangesByPid( _dwPid, mOut );
			return mOut;
		}

		/**
		 * \brief Finds the module covering an address, if any. Returns nullptr if not found.
		 */
		static inline const MODULEENTRY32W *
										FindModuleForAddress(
			const std::map<MX_ADDRESS_RANGE, MODULEENTRY32W> &_mMap,
			uint64_t _ui64Addr ) {

			// First range with start > addr.
			MX_ADDRESS_RANGE arKey { _ui64Addr, 1 };
			auto itUpper = _mMap.upper_bound( arKey );
			if ( itUpper == _mMap.begin() ) {
				return nullptr;
			}
			--itUpper;
			return itUpper->first.Contains( _ui64Addr ) ? &itUpper->second : nullptr;
		}

		/**
		 * Enumerates modules in the target PID into a std::set<MX_MODULE_ENTRY> ordered by name.
		 *
		 * \param _dwPid    Target process ID.
		 * \param _sOut     Output set of modules (cleared first).
		 * \return Returns true if enumeration succeeded and at least attempted.
		 */
		static inline bool				GatherModulesByPid( DWORD _dwPid, std::set<MX_MODULE_ENTRY> &_sOut ) {
			_sOut.clear();

			if ( _dwPid == 0 || DWINVALID == _dwPid ) { return false; }

			LSW_HANDLE hSnap = CSystem::CreateToolhelp32Snapshot( TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, _dwPid );
			if ( !hSnap.Valid()) { return false; }

			MODULEENTRY32W meEntry;
			meEntry.dwSize = sizeof( meEntry );

			if ( !CSystem::Module32FirstW( hSnap.hHandle, &meEntry ) ) { return false; }

			do {
				if ( meEntry.modBaseAddr != nullptr && meEntry.modBaseSize != 0 ) {
					MX_MODULE_ENTRY meThis {};
					meThis.meEntry = meEntry;

					// szModule and szExePath are WCHAR buffers. Copy to std::wstring first.
					std::wstring wName( meEntry.szModule );
					std::wstring wPath( meEntry.szExePath );

					meThis.sNameUtf8 = ee::CExpEval::ToUtf8( wName );
					meThis.sPathUtf8 = ee::CExpEval::ToUtf8( wPath );
					meThis.sKeyLower = CUtilities::ToLower( meThis.sNameUtf8 );

					_sOut.emplace( std::move( meThis ) );
				}
				meEntry.dwSize = sizeof( meEntry );
			}
			while ( CSystem::Module32NextW( hSnap.hHandle, &meEntry ) );
			return true;
		}

		/**
		 * Finds a module by UTF-8 name (case-insensitive). Name is the file name, e.g., "kernel32.dll".
		 *
		 * \param _sModules     Set of modules (ordered by case-insensitive name).
		 * \param _sNameUtf8    Name to find (UTF-8).
		 * \return Returns pointer to the found entry or nullptr if not found.
		 */
		static inline const MX_MODULE_ENTRY *
										FindModuleByName(
			const std::set<MX_MODULE_ENTRY> &_sModules,
			std::string_view _sNameUtf8 ) {

			MX_MODULE_ENTRY key {};
			key.sKeyLower = CUtilities::ToLower( std::string( _sNameUtf8 ) );

			auto pmeEntry = _sModules.find( key );
			return (pmeEntry == _sModules.end()) ? nullptr : &(*pmeEntry);
		}

		/**
		 * \brief Returns true if the given protection value allows execution.
		 *
		 * \param _dwProt Page protection from MEMORY_BASIC_INFORMATION::Protect.
		 * \return true if executable; false otherwise.
		 */
		static bool						ProtIsExecutable( DWORD _dwProt ) {
			// Mask off non-core modifiers that don't affect exec permission.
			const DWORD dwCore = (_dwProt & ~(PAGE_GUARD | PAGE_NOCACHE | PAGE_WRITECOMBINE | PAGE_TARGETS_INVALID | PAGE_TARGETS_NO_UPDATE));
			switch ( dwCore ) {
				case PAGE_EXECUTE : {}				MX_FALLTHROUGH
				case PAGE_EXECUTE_READ : {}			MX_FALLTHROUGH
				case PAGE_EXECUTE_READWRITE : {}	MX_FALLTHROUGH
				case PAGE_EXECUTE_WRITECOPY : { return true; }
				default : { return false; }
			}
		}

		/**
		 * \brief Tests whether a protection mask implies read capability.
		 *
		 * \param _dwProt The MEMORY_BASIC_INFORMATION::Protect value.
		 * \return Returns true if the protection allows reading; false otherwise.
		 */
		static bool						ProtIsReadable( DWORD _dwProt ) {
			// Mask out modifiers that don't affect read permission.
			const DWORD dwCoreProt = (_dwProt & ~(PAGE_GUARD | PAGE_NOCACHE | PAGE_WRITECOMBINE));
			switch ( dwCoreProt ) {
				case PAGE_READONLY : {}				MX_FALLTHROUGH
				case PAGE_READWRITE : {}			MX_FALLTHROUGH
				case PAGE_WRITECOPY : {}			MX_FALLTHROUGH
				case PAGE_EXECUTE_READ : {}			MX_FALLTHROUGH
				case PAGE_EXECUTE_READWRITE : {}	MX_FALLTHROUGH
				case PAGE_EXECUTE_WRITECOPY : { return true; }
				default : { return false; }
			}
		}

		/**
		 * \brief Tests whether a protection mask implies write capability.
		 *
		 * \param _dwProt The MEMORY_BASIC_INFORMATION::Protect value.
		 * \return Returns true if the protection allows writing; false otherwise.
		 */
		static bool						ProtIsWritable( DWORD _dwProt ) {
			// Mask out modifiers that don't affect write permission.
			const DWORD dwCore = (_dwProt & ~(PAGE_GUARD | PAGE_NOCACHE | PAGE_WRITECOMBINE | PAGE_TARGETS_INVALID | PAGE_TARGETS_NO_UPDATE));

			switch ( dwCore ) {
				case PAGE_READWRITE : {}			MX_FALLTHROUGH
				case PAGE_WRITECOPY : {}			MX_FALLTHROUGH
				case PAGE_EXECUTE_READWRITE : {}	MX_FALLTHROUGH
				case PAGE_EXECUTE_WRITECOPY : { return true; }
				default : { return false; }
			}
		}


	protected :
		// == Types.
		// Open a process structure.
		struct MX_OPEN_PROC {
			volatile LONG				aAtom;							// The the atom is set on the main thread, unset on the monitoring thread. (ATOM defines to WORD on MINWIN defined, which is a bug).
			HANDLE						hHandle = NULL;					// The handle returned by OpenProcess().

			MX_OPEN_PROC() :
				aAtom( 1 ) {}
		};


		// == Members.
		// Process ID.
		DWORD							m_dwId = DWINVALID;
		// Handle to the process.  May be recreated many times, so do not make copies.
		LSW_HANDLE						m_hProcHandle;

		// OpenProcess() flags.
		DWORD							m_dwOpenProcFlags = 0;

		// Mode for accessing the target process.
		MX_OPEN_PROC_MODE				m_opmMode;

		// Critical section.
		CCriticalSection				m_csCrit;
		
		// The thread to signal when the process is no longer open.
		lsw::CThread					m_tProcOpenThread;

		// Thread parameters.
		MX_OPEN_PROC					m_opOpenProcThreadMonitor;

		// The detatch event callback.
		PfDetatchCallback				m_pfDetatchCallback;
		// Detatch parameter 1.
		void *							m_pvDetatchParm1;
		// Detatch parameter 2.
		uintptr_t						m_uiptrDetatchParm2;

		// Module address ranges for determining where static addresses are.
		mutable std::map<MX_ADDRESS_RANGE, MODULEENTRY32W>
										m_mModuleCache;

		// Module information sorted by name.
		mutable std::set<MX_MODULE_ENTRY>
										m_sModulesByName;

		// The last "is readable" page.
		mutable uint64_t				m_ui64LastIsReadablePage = 0;
		// The last "is readable" result.
		mutable bool					m_bLastIsReadableResult = false;

		// The last "is executable" page.
		mutable uint64_t				m_ui64LastIsExecuteablePage = 0;
		// The last "is executable" result.
		mutable bool					m_bLastIsExecuteableResult = false;



		// == Functions.
		// Internal open process.
		bool							OpenProcInternal( DWORD _dwId, DWORD _dwFlags );

		// Opens the target process conservatively.
		bool							OpenProcConservative( DWORD _dwId );

		// Reads data from an area of memory in a specified process. The entire area to be read must be accessible or the operation fails.
		bool							ReadProcessMemoryInternal( uint64_t _ui64Address, LPVOID _lpBuffer, SIZE_T _nSize, SIZE_T * _lpNumberOfBytesRead ) const;

		// Writes data to an area of memory in a specified process. The entire area to be written to must be accessible or the operation fails.
		bool							WriteProcessMemoryInternal( uint64_t _ui64Address, LPCVOID _lpBuffer, SIZE_T _nSize, SIZE_T * _lpNumberOfBytesWritten );

		// Changes the protection on a region of committed pages in the virtual address space of a specified process.
		bool							VirtualProtectExInternal( uint64_t _ui64Address, SIZE_T _dwSize, DWORD _flNewProtect, PDWORD _lpflOldProtect );

		// Retrieves information about a range of pages within the virtual address space of a specified process.
		size_t							VirtualQueryExInternal( uint64_t _lpAddress, PMEMORY_BASIC_INFORMATION64 _lpBuffer ) const;

		// Open-process thread.
		//void							CreateProcessThread( CProcess & _pProcess );

		// Open-process thread.  Runs for as long as the target process is open.
		static DWORD WINAPI				ThreadProc( LPVOID _lpParameter );

	};

}	// namespace mx
