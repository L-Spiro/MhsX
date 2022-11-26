#include "MXMemHack.h"
#include "../Utilities/MXUtilities.h"
#include <Base/LSWBase.h>
#include <shellapi.h>

// TEMP.
/*#include "../Search/MXSearcher.h"
#include "../System/MXSystem.h"*/

namespace mx {

	CMemHack::CMemHack() :
		m_sSearcher( &m_pProcess ) {
		// Default options.
		m_oOptions.pmhMemHackObj = this;
		m_oOptions.dwFoundAddressRefresh = 10;
		m_oOptions.dwMainRefresh = 10;
		m_oOptions.dwLockedRefresh = 1000;
		m_oOptions.dwExpressionRefresh = 100;
		m_oOptions.bDataTypesAsCodeNames = TRUE;
		m_oOptions.bDataTypeSizes = TRUE;
		m_oOptions.bDataTypeRanges = TRUE;
		m_oOptions.bUse0x = TRUE;
		m_oOptions.bUse0o = FALSE;
		m_oOptions.bShortEnums = TRUE;
		// ====
		m_oOptions.dwOpenProc = MX_OP_SHOW_ALL;
		// ====
		m_oOptions.bMemImage = TRUE;
		m_oOptions.bMemPrivate = TRUE;
		m_oOptions.bMemMapped = TRUE;
		m_oOptions.iThreadPriority = THREAD_PRIORITY_NORMAL;
		m_oOptions.ui64BufferSize = 4 * 1024 * 1024;
		m_oOptions.bPreCache = TRUE;
		m_oOptions.bPostFlush = TRUE;
		m_oOptions.bPauseTarget = FALSE;
		m_oOptions.bUseEpsilon = TRUE;
		m_oOptions.bSmartEpsilon = TRUE;
		m_oOptions.dEpsilon = EE_FLOAT16_EPSILON;// 0.0009765625f
		m_oOptions.ui32Alignment = alignof( uint32_t );
		m_oOptions.bAligned = TRUE;
		m_oOptions.bSameAsOriginal = TRUE;

		m_oOptions.bMatchCase = FALSE;
		m_oOptions.bWholeWord = FALSE;
		m_oOptions.bIsHex = FALSE;
		m_oOptions.bResolveEscapes = TRUE;
		m_oOptions.bWildcard = FALSE;

		m_oOptions.bLingIgnoreCase = FALSE;
		m_oOptions.bLingIgnoreDiacritic = FALSE;
		m_oOptions.bIgnoreKana = FALSE;
		m_oOptions.bIgnoreNoSpace = FALSE;
		m_oOptions.bIgnoreSymbols = FALSE;
		m_oOptions.bIgnoreWidth = FALSE;

		m_oOptions.bRegexSingleLine = FALSE;
		m_oOptions.bRegexMultiLine = FALSE;
		m_oOptions.bRegexExtended = TRUE;
		m_oOptions.bRegexFindLongest = FALSE;
		m_oOptions.bRegexNegateSingleLine = TRUE;
		m_oOptions.uiRegexFlavor = CUtilities::MX_RF_ONIGURUMA;
		m_oOptions.bsByteswap = CUtilities::MX_BS_NONE;
	}
	CMemHack::~CMemHack() {
		Detach();
	}

	// == Functions.
	// Open a process by its ID.
	bool CMemHack::OpenProcess( DWORD _dwId ) {
		Detach();

		const CProcess::MX_OPEN_PROC_MODE opmMode = CProcess::MX_OPM_FIXED;
		switch ( opmMode ) {
			case CProcess::MX_OPM_FIXED : {
				// PROCESS_CREATE_THREAD: CreateThread()
				// PROCESS_VM_READ: ReadProcessMemory()
				// PROCESS_VM_WRITE: WriteProcessMemory()
				// PROCESS_VM_OPERATION: VirtualProtectEx(), WriteProcessMemory()
				// PROCESS_QUERY_INFORMATION: OpenProcessToken(), GetProcessMemoryInfo()
				// PROCESS_SUSPEND_RESUME: NtSuspendProcess(), NtResumeProcess()
				// PROCESS_QUERY_LIMITED_INFORMATION: GetProcessMemoryInfo()

				DWORD dwAttempts[] = {
					PROCESS_CREATE_THREAD |											// The full range of what we might want to do.
						PROCESS_VM_READ | PROCESS_VM_WRITE |
						PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION |
						PROCESS_SUSPEND_RESUME,
					PROCESS_CREATE_THREAD |											// Maybe we can live without suspending the process.
						PROCESS_VM_READ | PROCESS_VM_WRITE |
						PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION,

					PROCESS_QUERY_INFORMATION,										// The bare minimum.
					/*PROCESS_CREATE_THREAD |											// Maybe we can live without suspending the process.
						PROCESS_VM_READ | PROCESS_VM_WRITE |
						PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION,*/
				};
				for ( size_t I = 0; I < MX_ELEMENTS( dwAttempts ); ++I ) {
					if ( m_pProcess.OpenProc( _dwId, opmMode, dwAttempts[I] ) ) {
						break;
					}
				}
				break;
			}
			default : {
				m_pProcess.OpenProc( _dwId, opmMode, 0 );
			}
		}

		if ( !m_pProcess.ProcIsOpened() ) {
			FailedToOpenProcess( _dwId );
			return false;
		}
		
		OpenedProcess();

		return true;
	}

	// Opened a process.
	void CMemHack::OpenedProcess() {
		/*CSearcher sSearcher( &m_pProcess );
		CAddressChunkList adcList;
		uint64_t ui64ChunkSize = 1024 * 1024 * 32;
		uint64_t uiMin = (uint64_t)mx::CSystem::GetSystemInfo().lpMinimumApplicationAddress;
		uint64_t uiMax = (uint64_t)mx::CSystem::GetSystemInfo().lpMaximumApplicationAddress;
		sSearcher.GenerateChunks( adcList, ui64ChunkSize );
		ui64ChunkSize = 31ULL * 1024 * 1024 * 1024;*/
	}

	// Failed to open a process.
	void CMemHack::FailedToOpenProcess( DWORD _dwId ) {
	}

	// Detach from the current process.
	void CMemHack::Detach() {
		m_pProcess.Detach();
		//m_hProc.Reset();
	}

	// Gets the options.
	const MX_OPTIONS & CMemHack::Options() const {
		// Gather system-wide options into our structure.
		m_oOptions.bDataTypesAsCodeNames = (CUtilities::Options.dwDataTypeOptions & CUtilities::MX_DTO_CODENAMES) ? TRUE : FALSE;
		m_oOptions.bDataTypeRanges = (CUtilities::Options.dwDataTypeOptions & CUtilities::MX_DTO_SHOWRANGES) ? TRUE : FALSE;
		m_oOptions.bDataTypeSizes = (CUtilities::Options.dwDataTypeOptions & CUtilities::MX_DTO_SHOWSIZES) ? TRUE : FALSE;
		m_oOptions.bUse0x = CUtilities::Options.bUse0xForHex;
		m_oOptions.bUse0o = CUtilities::Options.bUse0oForOct;
		m_oOptions.bShortEnums = CUtilities::Options.bShortenEnumNames;
		return m_oOptions;
	}

	// Sets the options.
	void CMemHack::SetOptions( const MX_OPTIONS &_oOptions ) {
		m_oOptions = _oOptions;
		// Disperse options across the system.
		DWORD dwDataTypeOpts = 0;
		dwDataTypeOpts |= m_oOptions.bDataTypesAsCodeNames ? CUtilities::MX_DTO_CODENAMES : 0;
		dwDataTypeOpts |= m_oOptions.bDataTypeRanges ? CUtilities::MX_DTO_SHOWRANGES : 0;
		dwDataTypeOpts |= m_oOptions.bDataTypeSizes ? CUtilities::MX_DTO_SHOWSIZES : 0;
		CUtilities::Options.dwDataTypeOptions = dwDataTypeOpts;
		CUtilities::Options.bUse0xForHex = m_oOptions.bUse0x;
		CUtilities::Options.bUse0oForOct = m_oOptions.bUse0o;
		CUtilities::Options.bShortenEnumNames = m_oOptions.bShortEnums;
	}

	// Executes a program by index.
	bool CMemHack::ExecuteProgramByIdx( size_t _stIdx ) {
		if ( _stIdx >= m_vPrograms.size() ) { return false; }
		HINSTANCE hReturn = ::ShellExecuteW( NULL,
			L"open",
			m_vPrograms[_stIdx].wsPath.c_str(),
			m_vPrograms[_stIdx].wsArgs.c_str(),
			m_vPrograms[_stIdx].wsWorkingDir.c_str(),
			m_vPrograms[_stIdx].iShowCmd );
		return reinterpret_cast<INT_PTR>(hReturn) > 32;
	}

	// The address reader for expressions.
	bool __stdcall CMemHack::ExpAddressHandler( uint64_t _ui64Address, ee::EE_CAST_TYPES _ctType, uintptr_t _uiptrData, ee::CExpEvalContainer * _peecContainer, ee::CExpEvalContainer::EE_RESULT &_rResult ) {
		CMemHack * _mhMemHack = reinterpret_cast<CMemHack *>(_uiptrData);
		switch ( _ctType ) {
			case ee::EE_CT_INT8 : {
				_rResult.ncType = ee::EE_NC_SIGNED;
				int8_t i8Val;
				if ( _mhMemHack->Process().ReadProcessMemory( reinterpret_cast<LPCVOID>(_ui64Address), &i8Val, sizeof( i8Val ), NULL ) ) {
					_rResult.u.i64Val = i8Val;
					return true;
				}
				break;
			}
			case ee::EE_CT_INT16 : {
				_rResult.ncType = ee::EE_NC_SIGNED;
				int16_t i16Val;
				if ( _mhMemHack->Process().ReadProcessMemory( reinterpret_cast<LPCVOID>(_ui64Address), &i16Val, sizeof( i16Val ), NULL ) ) {
					_rResult.u.i64Val = i16Val;
					return true;
				}
				break;
			}
			case ee::EE_CT_INT32 : {
				_rResult.ncType = ee::EE_NC_SIGNED;
				int32_t i32Val;
				if ( _mhMemHack->Process().ReadProcessMemory( reinterpret_cast<LPCVOID>(_ui64Address), &i32Val, sizeof( i32Val ), NULL ) ) {
					_rResult.u.i64Val = i32Val;
					return true;
				}
				break;
			}
			case ee::EE_CT_INT64 : {
				_rResult.ncType = ee::EE_NC_SIGNED;
				if ( _mhMemHack->Process().ReadProcessMemory( reinterpret_cast<LPCVOID>(_ui64Address), &_rResult.u.i64Val, sizeof( _rResult.u.i64Val ), NULL ) ) {
					return true;
				}
				break;
			}

			case ee::EE_CT_UINT8 : {
				_rResult.ncType = ee::EE_NC_UNSIGNED;
				_rResult.u.ui64Val = 0;
				return _mhMemHack->Process().ReadProcessMemory( reinterpret_cast<LPCVOID>(_ui64Address), &_rResult.u.ui64Val, sizeof( uint8_t ), NULL );
			}
			case ee::EE_CT_UINT16 : {
				_rResult.ncType = ee::EE_NC_UNSIGNED;
				_rResult.u.ui64Val = 0;
				return _mhMemHack->Process().ReadProcessMemory( reinterpret_cast<LPCVOID>(_ui64Address), &_rResult.u.ui64Val, sizeof( uint16_t ), NULL );
			}
			case ee::EE_CT_UINT32 : {
				_rResult.ncType = ee::EE_NC_UNSIGNED;
				_rResult.u.ui64Val = 0;
				return _mhMemHack->Process().ReadProcessMemory( reinterpret_cast<LPCVOID>(_ui64Address), &_rResult.u.ui64Val, sizeof( uint32_t ), NULL );
			}
			case ee::EE_CT_UINT64 : {
				_rResult.ncType = ee::EE_NC_UNSIGNED;
				_rResult.u.ui64Val = 0;
				return _mhMemHack->Process().ReadProcessMemory( reinterpret_cast<LPCVOID>(_ui64Address), &_rResult.u.ui64Val, sizeof( uint64_t ), NULL );
			}


			case ee::EE_CT_FLOAT16 : {
				_rResult.ncType = ee::EE_NC_FLOATING;
				uint16_t ui16Val = 0;
				if ( _mhMemHack->Process().ReadProcessMemory( reinterpret_cast<LPCVOID>(_ui64Address), &ui16Val, sizeof( ui16Val ), NULL ) ) {
					_rResult.u.dVal = CFloat16( ui16Val ).Value();
					return true;
				}
				break;
			}
			case ee::EE_CT_FLOAT : {
				_rResult.ncType = ee::EE_NC_FLOATING;
				float fVal = 0.0f;
				if ( _mhMemHack->Process().ReadProcessMemory( reinterpret_cast<LPCVOID>(_ui64Address), &fVal, sizeof( fVal ), NULL ) ) {
					_rResult.u.dVal = fVal;
					return true;
				}
				break;
			}
			case ee::EE_CT_DOUBLE : {
				_rResult.ncType = ee::EE_NC_FLOATING;
				_rResult.u.dVal = 0;
				return _mhMemHack->Process().ReadProcessMemory( reinterpret_cast<LPCVOID>(_ui64Address), &_rResult.u.dVal, sizeof( _rResult.u.dVal ), NULL );
			}
		}
		_rResult.ncType = ee::EE_NC_INVALID;
		return false;
	}

	// The address writer for expressions.
	bool __stdcall CMemHack::ExpAddressWriteHandler( uint64_t _ui64Address, ee::EE_CAST_TYPES _ctType, uintptr_t _uiptrData, ee::CExpEvalContainer * _peecContainer, ee::CExpEvalContainer::EE_RESULT &_rResult ) {
		CMemHack * _mhMemHack = reinterpret_cast<CMemHack *>(_uiptrData);
		switch ( _rResult.ncType ) {
			case ee::EE_NC_UNSIGNED : {
				switch ( _ctType ) {
#define MX_SIGNED( BITS )																																\
	case ee::EE_CT_INT ## BITS : {																														\
		_rResult.ncType = ee::EE_NC_SIGNED;																												\
		int ## BITS ## _t iVal = static_cast<int ## BITS ## _t>(_rResult.u.ui64Val);																	\
		if ( _mhMemHack->m_pProcess.WriteProcessMemory( reinterpret_cast<LPVOID>(_ui64Address), &iVal, sizeof( iVal ), NULL ) ) {						\
			_rResult.u.i64Val = iVal;																													\
			return true;																																\
		}																																				\
		break;																																			\
	}
					MX_SIGNED( 8 )
					MX_SIGNED( 16 )
					MX_SIGNED( 32 )
					MX_SIGNED( 64 )
#undef MX_SIGNED

#define MX_UNSIGNED( BITS )																																\
	case ee::EE_CT_UINT ## BITS : {																														\
		_rResult.ncType = ee::EE_NC_UNSIGNED;																											\
		uint ## BITS ## _t iVal = static_cast<uint ## BITS ## _t>(_rResult.u.ui64Val);																	\
		if ( _mhMemHack->m_pProcess.WriteProcessMemory( reinterpret_cast<LPVOID>(_ui64Address), &iVal, sizeof( iVal ), NULL ) ) {						\
			_rResult.u.ui64Val = iVal;																													\
			return true;																																\
		}																																				\
		break;																																			\
	}
					MX_UNSIGNED( 8 )
					MX_UNSIGNED( 16 )
					MX_UNSIGNED( 32 )
					MX_UNSIGNED( 64 )
#undef MX_UNSIGNED

					case ee::EE_CT_FLOAT : {
						_rResult.ncType = ee::EE_NC_FLOATING;
						float fVal = static_cast<float>(_rResult.u.ui64Val);
						if ( _mhMemHack->m_pProcess.WriteProcessMemory( reinterpret_cast<LPVOID>(_ui64Address), &fVal, sizeof( fVal ), NULL ) ) {
							_rResult.u.dVal = fVal;
							return true;
						}
						break;
					}
					case ee::EE_CT_DOUBLE : {
						_rResult.ncType = ee::EE_NC_FLOATING;
						double dVal = static_cast<double>(_rResult.u.ui64Val);
						if ( _mhMemHack->m_pProcess.WriteProcessMemory( reinterpret_cast<LPVOID>(_ui64Address), &dVal, sizeof( dVal ), NULL ) ) {
							_rResult.u.dVal = dVal;
							return true;
						}
						break;
					}
					case ee::EE_CT_FLOAT16 : {
						_rResult.ncType = ee::EE_NC_FLOATING;
						CFloat16 f16Val = CFloat16( static_cast<double>(_rResult.u.ui64Val) );
						uint16_t uiVal = f16Val.RawValue();
						if ( _mhMemHack->m_pProcess.WriteProcessMemory( reinterpret_cast<LPVOID>(_ui64Address), &uiVal, sizeof( uiVal ), NULL ) ) {
							_rResult.u.dVal = f16Val.Value();
							return true;
						}
						break;
					}
				}
			}
			case ee::EE_NC_SIGNED : {
				switch ( _ctType ) {
#define MX_SIGNED( BITS )																																\
	case ee::EE_CT_INT ## BITS : {																														\
		_rResult.ncType = ee::EE_NC_SIGNED;																												\
		int ## BITS ## _t iVal = static_cast<int ## BITS ## _t>(_rResult.u.i64Val);																		\
		if ( _mhMemHack->m_pProcess.WriteProcessMemory( reinterpret_cast<LPVOID>(_ui64Address), &iVal, sizeof( iVal ), NULL ) ) {						\
			_rResult.u.i64Val = iVal;																													\
			return true;																																\
		}																																				\
		break;																																			\
	}
					MX_SIGNED( 8 )
					MX_SIGNED( 16 )
					MX_SIGNED( 32 )
					MX_SIGNED( 64 )
#undef MX_SIGNED

#define MX_UNSIGNED( BITS )																																\
	case ee::EE_CT_UINT ## BITS : {																														\
		_rResult.ncType = ee::EE_NC_UNSIGNED;																											\
		uint ## BITS ## _t iVal = static_cast<uint ## BITS ## _t>(_rResult.u.i64Val);																	\
		if ( _mhMemHack->m_pProcess.WriteProcessMemory( reinterpret_cast<LPVOID>(_ui64Address), &iVal, sizeof( iVal ), NULL ) ) {						\
			_rResult.u.i64Val = iVal;																													\
			return true;																																\
		}																																				\
		break;																																			\
	}
					MX_UNSIGNED( 8 )
					MX_UNSIGNED( 16 )
					MX_UNSIGNED( 32 )
					MX_UNSIGNED( 64 )
#undef MX_UNSIGNED

					case ee::EE_CT_FLOAT : {
						_rResult.ncType = ee::EE_NC_FLOATING;
						float fVal = static_cast<float>(_rResult.u.i64Val);
						if ( _mhMemHack->m_pProcess.WriteProcessMemory( reinterpret_cast<LPVOID>(_ui64Address), &fVal, sizeof( fVal ), NULL ) ) {
							_rResult.u.dVal = fVal;
							return true;
						}
						break;
					}
					case ee::EE_CT_DOUBLE : {
						_rResult.ncType = ee::EE_NC_FLOATING;
						double dVal = static_cast<double>(_rResult.u.i64Val);
						if ( _mhMemHack->m_pProcess.WriteProcessMemory( reinterpret_cast<LPVOID>(_ui64Address), &dVal, sizeof( dVal ), NULL ) ) {
							_rResult.u.dVal = dVal;
							return true;
						}
						break;
					}
					case ee::EE_CT_FLOAT16 : {
						_rResult.ncType = ee::EE_NC_FLOATING;
						CFloat16 f16Val = CFloat16( static_cast<double>(_rResult.u.i64Val) );
						uint16_t uiVal = f16Val.RawValue();
						if ( _mhMemHack->m_pProcess.WriteProcessMemory( reinterpret_cast<LPVOID>(_ui64Address), &uiVal, sizeof( uiVal ), NULL ) ) {
							_rResult.u.dVal = f16Val.Value();
							return true;
						}
						break;
					}
				}
			}
			case ee::EE_NC_FLOATING : {
				switch ( _ctType ) {
#define MX_SIGNED( BITS )																																\
	case ee::EE_CT_INT ## BITS : {																														\
		_rResult.ncType = ee::EE_NC_SIGNED;																												\
		int ## BITS ## _t iVal = static_cast<int ## BITS ## _t>(_rResult.u.dVal);																		\
		if ( _mhMemHack->m_pProcess.WriteProcessMemory( reinterpret_cast<LPVOID>(_ui64Address), &iVal, sizeof( iVal ), NULL ) ) {						\
			_rResult.u.i64Val = iVal;																													\
			return true;																																\
		}																																				\
		break;																																			\
	}
					MX_SIGNED( 8 )
					MX_SIGNED( 16 )
					MX_SIGNED( 32 )
					MX_SIGNED( 64 )
#undef MX_SIGNED

#define MX_UNSIGNED( BITS )																																\
	case ee::EE_CT_UINT ## BITS : {																														\
		_rResult.ncType = ee::EE_NC_UNSIGNED;																											\
		uint ## BITS ## _t iVal = static_cast<uint ## BITS ## _t>(_rResult.u.dVal);																		\
		if ( _mhMemHack->m_pProcess.WriteProcessMemory( reinterpret_cast<LPVOID>(_ui64Address), &iVal, sizeof( iVal ), NULL ) ) {						\
			_rResult.u.dVal = iVal;																														\
			return true;																																\
		}																																				\
		break;																																			\
	}
					MX_UNSIGNED( 8 )
					MX_UNSIGNED( 16 )
					MX_UNSIGNED( 32 )
					MX_UNSIGNED( 64 )
#undef MX_UNSIGNED

					case ee::EE_CT_FLOAT : {
						_rResult.ncType = ee::EE_NC_FLOATING;
						float fVal = static_cast<float>(_rResult.u.dVal);
						if ( _mhMemHack->m_pProcess.WriteProcessMemory( reinterpret_cast<LPVOID>(_ui64Address), &fVal, sizeof( fVal ), NULL ) ) {
							_rResult.u.dVal = fVal;
							return true;
						}
						break;
					}
					case ee::EE_CT_DOUBLE : {
						_rResult.ncType = ee::EE_NC_FLOATING;
						double dVal = static_cast<double>(_rResult.u.dVal);
						if ( _mhMemHack->m_pProcess.WriteProcessMemory( reinterpret_cast<LPVOID>(_ui64Address), &dVal, sizeof( dVal ), NULL ) ) {
							_rResult.u.dVal = dVal;
							return true;
						}
						break;
					}
					case ee::EE_CT_FLOAT16 : {
						_rResult.ncType = ee::EE_NC_FLOATING;
						CFloat16 f16Val = CFloat16( static_cast<double>(_rResult.u.dVal) );
						uint16_t uiVal = f16Val.RawValue();
						if ( _mhMemHack->m_pProcess.WriteProcessMemory( reinterpret_cast<LPVOID>(_ui64Address), &uiVal, sizeof( uiVal ), NULL ) ) {
							_rResult.u.dVal = f16Val.Value();
							return true;
						}
						break;
					}
				}
			}
		}

		_rResult.ncType = ee::EE_NC_INVALID;
		return false;
	}

}	// namespace mx
