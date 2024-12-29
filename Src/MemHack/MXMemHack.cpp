#include "MXMemHack.h"
#include "../Files/MXFile.h"
#include "../Utilities/MXUtilities.h"
#include <Base/LSWBase.h>
#include <EEExpEval.h>

#include <shellapi.h>

// TEMP.
/*#include "../Search/MXSearcher.h"
#include "../System/MXSystem.h"*/

namespace mx {

	CMemHack::CMemHack() :
		m_sSearcher( &m_pProcess ) {
		m_vPrograms.push_back( MX_PROGRAM() );
		m_vPrograms.push_back( MX_PROGRAM() );
		m_vPrograms[0].wsMenuName = _DEC_WS_E1053BE4_Windows_C_alculator___;
		m_vPrograms[0].wsPath = _DEC_WS_9A9A2865_Calc_exe;
		m_vPrograms[1].wsMenuName = _DEC_WS_A16E1680_Windows__Notepad___;
		m_vPrograms[1].wsPath = _DEC_WS_6DAC8A48_Notepad_exe;
	}
	CMemHack::~CMemHack() {
		Detach( TRUE );
	}

	// == Functions.
	// Open a process by its ID.
	bool CMemHack::OpenProcess( DWORD _dwId ) {
		Detach( TRUE );

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
	void CMemHack::Detach( BOOL _bCallProcessDetach ) {
		if ( _bCallProcessDetach ) {
			m_pProcess.Detach();
		}
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

	// Gets the Floating-Point Studio options for read and write.
	MX_FLOAT_STUDIO_OPTIONS & CMemHack::FloatStudioOptions() {
		return m_fsoOptions;
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

	// Saves all program settings.
	bool CMemHack::SaveSettings( const std::wstring &_wsPath, bool _bAsJson ) const {
		CSecureWString wsFinalPath = CSystem::GetSelfPathW();
		if ( _wsPath.size() ) {
			wsFinalPath += _wsPath;
		}
		else {
			wsFinalPath += _bAsJson ? _DEC_WS_3F15B0CA_config_json : _DEC_WS_7B969963_app_config;
		}
		std::vector<uint8_t> vBuffer;
		if ( _bAsJson ) {
			lson::CJson::LSON_ELEMENT eRoot;
			lson::CJson::CreateObjectElement( "", eRoot );
			if ( !SaveSettings( &eRoot, nullptr, Options() ) ) { return false; }
			if ( !lson::CJson::WriteElement( eRoot, vBuffer, 0 ) ) { return false; }
			/*vBuffer.push_back( 0 );
			::OutputDebugStringA( reinterpret_cast<LPCSTR>(vBuffer.data()) );*/
		}
		else {
			
			CStream sStream( vBuffer );
			if ( !SaveSettings( nullptr, &sStream, Options() ) ) { return false; }
		}
		
		CFile fFile;
		if ( !fFile.CreateNewFile( wsFinalPath.c_str(), FALSE ) ) { return false; }
		if ( !fFile.Write( vBuffer.data(), static_cast<DWORD>(vBuffer.size()) ) ) { return false; }
		
		return true;
	}

	// Loads all program settings.
	bool CMemHack::LoadSettings( const std::wstring &_wsPath, bool _bAsJson ) {
		CSecureWString wsFinalPath = CSystem::GetSelfPathW();
		if ( _wsPath.size() ) {
			wsFinalPath += _wsPath;
		}
		else {
			wsFinalPath += _bAsJson ? _DEC_WS_3F15B0CA_config_json : _DEC_WS_7B969963_app_config;
		}
		std::vector<uint8_t> vBuffer;
		CFile fFile;
		fFile.LoadToMemory( wsFinalPath.c_str(), vBuffer );
		if ( vBuffer.size() == 0 ) { return false; }

		if ( _bAsJson ) {
			lson::CJson jSon;
			vBuffer.push_back( 0 );

			if ( !jSon.SetJson( reinterpret_cast<const char *>(vBuffer.data()) ) ) { return false; }
			if ( !LoadSettings( &jSon, nullptr, m_oOptions ) ) { return false; }
		}
		else {
			CStream sStream( vBuffer );
			if ( !LoadSettings( nullptr, &sStream, m_oOptions ) ) { return false; }
		}
		SetOptions( m_oOptions );

		return true;
	}

	// Saves to JSON format if _peJson is not nullptr, otherwise it saves to binary stored in _psBinary.
	bool CMemHack::SaveSettings( lson::CJson::LSON_ELEMENT * _peJson, CStream * _psBinary, const MX_OPTIONS &_oOptions ) const {
		if ( _peJson == nullptr && nullptr == _psBinary ) { return false; }
		try {
			uint32_t ui32Version = 1;
			if ( _peJson ) {
				MX_JSON_NUMBER( _DEC_S_70A1EA5F_Version, ui32Version, _peJson );

				_peJson->vObjectMembers.push_back( std::make_unique<lson::CJson::LSON_ELEMENT>() );
				lson::CJson::CreateObjectElement( _DEC_S_01940FD6_General, (*_peJson->vObjectMembers[_peJson->vObjectMembers.size()-1]) );
				if ( !SaveGeneralSettings( _peJson->vObjectMembers[_peJson->vObjectMembers.size()-1].get(), _psBinary, _oOptions ) ) { return false; }

				_peJson->vObjectMembers.push_back( std::make_unique<lson::CJson::LSON_ELEMENT>() );
				lson::CJson::CreateObjectElement( _DEC_S_AD6A7CD0_OpenProc, (*_peJson->vObjectMembers[_peJson->vObjectMembers.size()-1]) );
				if ( !SaveOpenProcSettings( _peJson->vObjectMembers[_peJson->vObjectMembers.size()-1].get(), _psBinary, _oOptions ) ) { return false; }

				_peJson->vObjectMembers.push_back( std::make_unique<lson::CJson::LSON_ELEMENT>() );
				lson::CJson::CreateObjectElement( _DEC_S_28CEB4AC_SearchSettings, (*_peJson->vObjectMembers[_peJson->vObjectMembers.size()-1]) );
				if ( !SaveSearchSettings( _peJson->vObjectMembers[_peJson->vObjectMembers.size()-1].get(), _psBinary, _oOptions ) ) { return false; }
			}
			else {
				_psBinary->WriteUi32( ui32Version );
				if ( !SaveGeneralSettings( nullptr, _psBinary, _oOptions ) ) { return false; }
				if ( !SaveOpenProcSettings( nullptr, _psBinary, _oOptions ) ) { return false; }
				if ( !SaveSearchSettings( nullptr, _psBinary, _oOptions ) ) { return false; }
			}
		}
		catch ( ... ) {
			return false;
		}
		return true;
	}

	// Loads settings from either a JSON object or a byte buffer.
	bool CMemHack::LoadSettings( lson::CJson * _pjJson, CStream * _psBinary, MX_OPTIONS &_oOptions ) {
		if ( nullptr == _pjJson && nullptr == _psBinary ) { return false; }
		if ( _pjJson ) {
			if ( !_pjJson->GetContainer() ) { return false; }
			size_t stRoot = _pjJson->GetContainer()->GetRoot();
			const lson::CJsonContainer::LSON_JSON_VALUE & jvRoot = _pjJson->GetContainer()->GetValue( stRoot );

			const lson::CJsonContainer::LSON_JSON_VALUE * pjvVal = _pjJson->GetContainer()->GetMemberByName( jvRoot, _DEC_S_70A1EA5F_Version );
			if ( !pjvVal ) { return false; }
			if ( pjvVal->vtType != lson::CJsonContainer::LSON_VT_DECIMAL ) { return false; }
			uint32_t ui32Version = static_cast<uint32_t>(pjvVal->u.dDecimal);

			pjvVal = _pjJson->GetContainer()->GetMemberByName( jvRoot, _DEC_S_01940FD6_General );
			if ( !pjvVal || !LoadGeneralSettings( pjvVal, _pjJson->GetContainer(), nullptr, _oOptions, ui32Version ) ) { return false; }

			pjvVal = _pjJson->GetContainer()->GetMemberByName( jvRoot, _DEC_S_AD6A7CD0_OpenProc );
			if ( !pjvVal || !LoadOpenProcSettings( pjvVal, _pjJson->GetContainer(), nullptr, _oOptions, ui32Version ) ) { return false; }

			pjvVal = _pjJson->GetContainer()->GetMemberByName( jvRoot, _DEC_S_28CEB4AC_SearchSettings );
			if ( !pjvVal || !LoadSearchSettings( pjvVal, _pjJson->GetContainer(), nullptr, _oOptions, ui32Version ) ) { return false; }
		}
		else {
			uint32_t ui32Version;
			if ( !_psBinary->ReadUi32( ui32Version ) ) { return false; }

			if ( !LoadGeneralSettings( nullptr, nullptr, _psBinary, _oOptions, ui32Version ) ) { return false; }
			if ( !LoadOpenProcSettings( nullptr, nullptr, _psBinary, _oOptions, ui32Version ) ) { return false; }
			if ( !LoadSearchSettings( nullptr, nullptr, _psBinary, _oOptions, ui32Version ) ) { return false; }
		}
		return true;
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
					_rResult.u.dVal = ee::CFloat16( ui16Val ).Value();
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
						ee::CFloat16 f16Val = ee::CFloat16( static_cast<double>(_rResult.u.ui64Val) );
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
						ee::CFloat16 f16Val = ee::CFloat16( static_cast<double>(_rResult.u.i64Val) );
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
						ee::CFloat16 f16Val = ee::CFloat16( static_cast<double>(_rResult.u.dVal) );
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

	// Saves to JSON format if _peJson is not nullptr, otherwise it saves to binary stored in _psBinary.
	bool CMemHack::SaveGeneralSettings( lson::CJson::LSON_ELEMENT * _peJson, CStream * _psBinary, const MX_OPTIONS &_oOptions ) const {
		if ( _peJson ) {
			MX_JSON_NUMBER( _DEC_S_A28FE3CD_FoundAddressRate, _oOptions.dwFoundAddressRefresh, _peJson );
			MX_JSON_NUMBER( _DEC_S_CD01A1C1_MainRefreshRate, _oOptions.dwMainRefresh, _peJson );
			MX_JSON_NUMBER( _DEC_S_6BC41969_LockedRefreshRate, _oOptions.dwLockedRefresh, _peJson );
			MX_JSON_NUMBER( _DEC_S_07F2BA28_ExpressionRefreshRate, _oOptions.dwExpressionRefresh, _peJson );

			MX_JSON_BOOL( _DEC_S_9385205C_DataAsCodeName, _oOptions.bDataTypesAsCodeNames, _peJson );
			MX_JSON_BOOL( _DEC_S_13C49137_DataSizes, _oOptions.bDataTypeSizes, _peJson );
			MX_JSON_BOOL( _DEC_S_98EB9780_DataRanges, _oOptions.bDataTypeRanges, _peJson );
			MX_JSON_BOOL( _DEC_S_2DFB9F4D_UseXx, _oOptions.bUse0x, _peJson );
			MX_JSON_BOOL( _DEC_S_ABAB9E1C_UseOo, _oOptions.bUse0o, _peJson );
			MX_JSON_BOOL( _DEC_S_B412829D_ShortEnums, _oOptions.bShortEnums, _peJson );
		}
		else {
			if ( !_psBinary->Write( _oOptions.dwFoundAddressRefresh ) ) { return false; }
			if ( !_psBinary->Write( _oOptions.dwMainRefresh ) ) { return false; }
			if ( !_psBinary->Write( _oOptions.dwLockedRefresh ) ) { return false; }
			if ( !_psBinary->Write( _oOptions.dwExpressionRefresh ) ) { return false; }

			if ( !_psBinary->WriteUi8( _oOptions.bDataTypesAsCodeNames ? TRUE : FALSE ) ) { return false; }
			if ( !_psBinary->WriteUi8( _oOptions.bDataTypeSizes ? TRUE : FALSE ) ) { return false; }
			if ( !_psBinary->WriteUi8( _oOptions.bDataTypeRanges ? TRUE : FALSE ) ) { return false; }
			if ( !_psBinary->WriteUi8( _oOptions.bUse0x ? TRUE : FALSE ) ) { return false; }
			if ( !_psBinary->WriteUi8( _oOptions.bUse0o ? TRUE : FALSE ) ) { return false; }
			if ( !_psBinary->WriteUi8( _oOptions.bShortEnums ? TRUE : FALSE ) ) { return false; }
		}
		return true;
	}

	// Loads general settings from either a JSON object or a byte buffer.
	bool CMemHack::LoadGeneralSettings( const lson::CJsonContainer::LSON_JSON_VALUE * _pjJson, lson::CJsonContainer * _pjcContainer, CStream * _psBinary, MX_OPTIONS &_oOptions, uint32_t /*_ui32Version*/ ) {
		if ( nullptr == _pjJson && _psBinary == nullptr ) { return false; }
		if ( _pjJson ) {
			const lson::CJsonContainer::LSON_JSON_VALUE * pjvVal;
			MX_JSON_GET_NUMBER( _DEC_S_A28FE3CD_FoundAddressRate, _oOptions.dwFoundAddressRefresh, DWORD, pjvVal, _pjJson );
			MX_JSON_GET_NUMBER( _DEC_S_CD01A1C1_MainRefreshRate, _oOptions.dwMainRefresh, DWORD, pjvVal, _pjJson );
			MX_JSON_GET_NUMBER( _DEC_S_6BC41969_LockedRefreshRate, _oOptions.dwLockedRefresh, DWORD, pjvVal, _pjJson );
			MX_JSON_GET_NUMBER( _DEC_S_07F2BA28_ExpressionRefreshRate, _oOptions.dwExpressionRefresh, DWORD, pjvVal, _pjJson );

			MX_JSON_GET_BOOL( _DEC_S_9385205C_DataAsCodeName, _oOptions.bDataTypesAsCodeNames, pjvVal, _pjJson );
			MX_JSON_GET_BOOL( _DEC_S_13C49137_DataSizes, _oOptions.bDataTypeSizes, pjvVal, _pjJson );
			MX_JSON_GET_BOOL( _DEC_S_98EB9780_DataRanges, _oOptions.bDataTypeRanges, pjvVal, _pjJson );
			MX_JSON_GET_BOOL( _DEC_S_2DFB9F4D_UseXx, _oOptions.bUse0x, pjvVal, _pjJson );
			MX_JSON_GET_BOOL( _DEC_S_ABAB9E1C_UseOo, _oOptions.bUse0o, pjvVal, _pjJson );
			MX_JSON_GET_BOOL( _DEC_S_B412829D_ShortEnums, _oOptions.bShortEnums, pjvVal, _pjJson );
		}
		else {
			if ( !_psBinary->Read( _oOptions.dwFoundAddressRefresh ) ) { return false; }
			if ( !_psBinary->Read( _oOptions.dwMainRefresh ) ) { return false; }
			if ( !_psBinary->Read( _oOptions.dwLockedRefresh ) ) { return false; }
			if ( !_psBinary->Read( _oOptions.dwExpressionRefresh ) ) { return false; }
			uint8_t ui8Tmp;
			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bDataTypesAsCodeNames = ui8Tmp ? TRUE : FALSE;
			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bDataTypeSizes = ui8Tmp ? TRUE : FALSE;
			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bDataTypeRanges = ui8Tmp ? TRUE : FALSE;
			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bUse0x = ui8Tmp ? TRUE : FALSE;
			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bUse0o = ui8Tmp ? TRUE : FALSE;
			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bShortEnums = ui8Tmp ? TRUE : FALSE;

		}
		return true;
	}

	// Saves to JSON format if _peJson is not nullptr, otherwise it saves to binary stored in _psBinary.
	bool CMemHack::SaveOpenProcSettings( lson::CJson::LSON_ELEMENT * _peJson, CStream * _psBinary, const MX_OPTIONS &_oOptions ) const {
		if ( _peJson ) {
			MX_JSON_NUMBER( _DEC_S_3C318291_OpenProcFlags, _oOptions.dwOpenProc, _peJson );
		}
		else {
			if ( !_psBinary->Write( _oOptions.dwOpenProc ) ) { return false; }
		}
		return true;
	}

	// Loads open-process settings from either a JSON object or a byte buffer.
	bool CMemHack::LoadOpenProcSettings( const lson::CJsonContainer::LSON_JSON_VALUE * _pjJson, lson::CJsonContainer * _pjcContainer, CStream * _psBinary, MX_OPTIONS &_oOptions, uint32_t _ui32Version ) {
		if ( nullptr == _pjJson && _psBinary == nullptr ) { return false; }
		if ( _pjJson ) {
			const lson::CJsonContainer::LSON_JSON_VALUE * pjvVal;
			MX_JSON_GET_NUMBER( _DEC_S_3C318291_OpenProcFlags, _oOptions.dwOpenProc, DWORD, pjvVal, _pjJson );
		}
		else {
			if ( !_psBinary->Read( _oOptions.dwOpenProc ) ) { return false; }
		}
		return true;
	}

	// Saves to JSON format if _peJson is not nullptr, otherwise it saves to binary stored in _psBinary.
	bool CMemHack::SaveSearchSettings( lson::CJson::LSON_ELEMENT * _peJson, CStream * _psBinary, const MX_OPTIONS &_oOptions ) const {
		if ( _peJson ) {
			MX_JSON_BOOL( _DEC_S_7880ABD5_MemImage, _oOptions.bMemImage, _peJson );
			MX_JSON_BOOL( _DEC_S_5503B2D4_MemPrivate, _oOptions.bMemPrivate, _peJson );
			MX_JSON_BOOL( _DEC_S_C1B1C624_MemMapped, _oOptions.bMemMapped, _peJson );

			MX_JSON_NUMBER( _DEC_S_9346CF9D_ThreadPriority, _oOptions.iThreadPriority, _peJson );
			MX_JSON_NUMBER( _DEC_S_908E909C_BufferSize, _oOptions.ui64BufferSize, _peJson );

			MX_JSON_BOOL( _DEC_S_9C78FB44_PauseTarget, _oOptions.bPauseTarget, _peJson );
			MX_JSON_BOOL( _DEC_S_017D084F_UseEpsilon, _oOptions.bUseEpsilon, _peJson );
			MX_JSON_BOOL( _DEC_S_8FF5F221_SmartEpsilon, _oOptions.bSmartEpsilon, _peJson );
			MX_JSON_NUMBER( _DEC_S_7183A384_Epsilon, _oOptions.dEpsilon, _peJson );
			MX_JSON_NUMBER( _DEC_S_AE3F9CFF_Alignment, _oOptions.ui32Alignment, _peJson );
			MX_JSON_BOOL( _DEC_S_22E9689D_Aligned, _oOptions.bAligned, _peJson );
			MX_JSON_BOOL( _DEC_S_C5BA9DF2_SameAsOriginal, _oOptions.bSameAsOriginal, _peJson );

			MX_JSON_BOOL( _DEC_S_13EE0CBD_MatchCase, _oOptions.bMatchCase, _peJson );
			MX_JSON_BOOL( _DEC_S_E2A98D3C_WholeWord, _oOptions.bWholeWord, _peJson );
			MX_JSON_BOOL( _DEC_S_D29678C3_IsHex, _oOptions.bIsHex, _peJson );
			MX_JSON_BOOL( _DEC_S_0769B140_ResolveEscapes, _oOptions.bResolveEscapes, _peJson );
			MX_JSON_BOOL( _DEC_S_220D47BC_Wildcards, _oOptions.bWildcard, _peJson );
			MX_JSON_BOOL( _DEC_S_87690053_LingIgnoreCase, _oOptions.bLingIgnoreCase, _peJson );
			MX_JSON_BOOL( _DEC_S_4AAB5CCD_LinkIgnoreDiacritic, _oOptions.bLingIgnoreDiacritic, _peJson );
			MX_JSON_BOOL( _DEC_S_D24B1BCF_IgnoreKana, _oOptions.bIgnoreKana, _peJson );
			MX_JSON_BOOL( _DEC_S_CEB15581_IgnoreNoSpace, _oOptions.bIgnoreNoSpace, _peJson );
			MX_JSON_BOOL( _DEC_S_80E4546E_IgnoreSymbols, _oOptions.bIgnoreSymbols, _peJson );
			MX_JSON_BOOL( _DEC_S_C0E4A373_IgnoreWidth, _oOptions.bIgnoreWidth, _peJson );
			MX_JSON_BOOL( _DEC_S_2937FA36_RegexSingleLine, _oOptions.bRegexSingleLine, _peJson );
			MX_JSON_BOOL( _DEC_S_4F5DA621_RegexMultiLine, _oOptions.bRegexMultiLine, _peJson );
			MX_JSON_BOOL( _DEC_S_6AE0CEA2_RegexExtended, _oOptions.bRegexExtended, _peJson );
			MX_JSON_BOOL( _DEC_S_3EE2DC3A_RegexFindLongest, _oOptions.bRegexFindLongest, _peJson );
			MX_JSON_BOOL( _DEC_S_AA86E6A0_RegexNegateSingleLine, _oOptions.bRegexNegateSingleLine, _peJson );
			
			MX_JSON_NUMBER( _DEC_S_095E5602_RegexFlavor, _oOptions.uiRegexFlavor, _peJson );
			MX_JSON_NUMBER( _DEC_S_12EECDA8_ByteSwap, _oOptions.bsByteswap, _peJson );
			{
				CSecureWString swsTmp = CUtilities::EscapeNonJson( _oOptions.wsFromText, false );
				_peJson->vObjectMembers.push_back( std::make_unique<lson::CJson::LSON_ELEMENT>() );
				lson::CJson::CreateStringElement( _DEC_S_19280E4E_From, ee::CExpEval::ToUtf8( swsTmp ), (*_peJson->vObjectMembers[_peJson->vObjectMembers.size()-1]) );
			}
			{
				CSecureWString swsTmp = CUtilities::EscapeNonJson( _oOptions.wsToText, false );
				_peJson->vObjectMembers.push_back( std::make_unique<lson::CJson::LSON_ELEMENT>() );
				lson::CJson::CreateStringElement( _DEC_S_4203F666_To, ee::CExpEval::ToUtf8( swsTmp ), (*_peJson->vObjectMembers[_peJson->vObjectMembers.size()-1]) );
			}

			_peJson->vObjectMembers.push_back( std::make_unique<lson::CJson::LSON_ELEMENT>() );
			lson::CJson::CreateArrayElement( _DEC_S_1A995A53_FromHistory, (*_peJson->vObjectMembers[_peJson->vObjectMembers.size()-1]) );
			{
				lson::CJson::LSON_ELEMENT * peArray = _peJson->vObjectMembers[_peJson->vObjectMembers.size()-1].get();
				for ( size_t I = 0; I < _oOptions.vFromHistory.size(); ++I ) {
					{
						CSecureWString swsTmp = CUtilities::EscapeNonJson( _oOptions.vFromHistory[I], false );
						peArray->vObjectMembers.push_back( std::make_unique<lson::CJson::LSON_ELEMENT>() );
						lson::CJson::CreateStringElement( std::to_string( I ), ee::CExpEval::ToUtf8( swsTmp ), (*peArray->vObjectMembers[peArray->vObjectMembers.size()-1]) );
					}
				}
			}

			_peJson->vObjectMembers.push_back( std::make_unique<lson::CJson::LSON_ELEMENT>() );
			lson::CJson::CreateArrayElement( _DEC_S_B517206A_ToHistory, (*_peJson->vObjectMembers[_peJson->vObjectMembers.size()-1]) );
			{
				lson::CJson::LSON_ELEMENT * peArray = _peJson->vObjectMembers[_peJson->vObjectMembers.size()-1].get();
				for ( size_t I = 0; I < _oOptions.vToHistory.size(); ++I ) {
					{
						CSecureWString swsTmp = CUtilities::EscapeNonJson( _oOptions.vToHistory[I], false );
						peArray->vObjectMembers.push_back( std::make_unique<lson::CJson::LSON_ELEMENT>() );
						lson::CJson::CreateStringElement( std::to_string( I ), ee::CExpEval::ToUtf8( swsTmp ), (*peArray->vObjectMembers[peArray->vObjectMembers.size()-1]) );
					}
				}
			}

		}
		else {
			if ( !_psBinary->WriteUi8( _oOptions.bMemImage ? TRUE : FALSE ) ) { return false; }
			if ( !_psBinary->WriteUi8( _oOptions.bMemPrivate ? TRUE : FALSE ) ) { return false; }
			if ( !_psBinary->WriteUi8( _oOptions.bMemMapped ? TRUE : FALSE ) ) { return false; }

			if ( !_psBinary->WriteI32( _oOptions.iThreadPriority ) ) { return false; }
			if ( !_psBinary->WriteUi64( _oOptions.ui64BufferSize ) ) { return false; }

			if ( !_psBinary->WriteUi8( _oOptions.bPauseTarget ? TRUE : FALSE ) ) { return false; }
			if ( !_psBinary->WriteUi8( _oOptions.bUseEpsilon ? TRUE : FALSE ) ) { return false; }
			if ( !_psBinary->WriteUi8( _oOptions.bSmartEpsilon ? TRUE : FALSE ) ) { return false; }
			if ( !_psBinary->WriteF64( _oOptions.dEpsilon ) ) { return false; }
			if ( !_psBinary->WriteUi32( _oOptions.ui32Alignment ) ) { return false; }
			if ( !_psBinary->WriteUi8( _oOptions.bAligned ? TRUE : FALSE ) ) { return false; }
			if ( !_psBinary->WriteUi8( _oOptions.bSameAsOriginal ? TRUE : FALSE ) ) { return false; }

			if ( !_psBinary->WriteUi8( _oOptions.bMatchCase ? TRUE : FALSE ) ) { return false; }
			if ( !_psBinary->WriteUi8( _oOptions.bWholeWord ? TRUE : FALSE ) ) { return false; }
			if ( !_psBinary->WriteUi8( _oOptions.bIsHex ? TRUE : FALSE ) ) { return false; }
			if ( !_psBinary->WriteUi8( _oOptions.bResolveEscapes ? TRUE : FALSE ) ) { return false; }
			if ( !_psBinary->WriteUi8( _oOptions.bWildcard ? TRUE : FALSE ) ) { return false; }
			if ( !_psBinary->WriteUi8( _oOptions.bLingIgnoreCase ? TRUE : FALSE ) ) { return false; }
			if ( !_psBinary->WriteUi8( _oOptions.bLingIgnoreDiacritic ? TRUE : FALSE ) ) { return false; }
			if ( !_psBinary->WriteUi8( _oOptions.bIgnoreKana ? TRUE : FALSE ) ) { return false; }
			if ( !_psBinary->WriteUi8( _oOptions.bIgnoreNoSpace ? TRUE : FALSE ) ) { return false; }
			if ( !_psBinary->WriteUi8( _oOptions.bIgnoreSymbols ? TRUE : FALSE ) ) { return false; }
			if ( !_psBinary->WriteUi8( _oOptions.bIgnoreWidth ? TRUE : FALSE ) ) { return false; }
			if ( !_psBinary->WriteUi8( _oOptions.bRegexSingleLine ? TRUE : FALSE ) ) { return false; }
			if ( !_psBinary->WriteUi8( _oOptions.bRegexMultiLine ? TRUE : FALSE ) ) { return false; }
			if ( !_psBinary->WriteUi8( _oOptions.bRegexExtended ? TRUE : FALSE ) ) { return false; }
			if ( !_psBinary->WriteUi8( _oOptions.bRegexFindLongest ? TRUE : FALSE ) ) { return false; }
			if ( !_psBinary->WriteUi8( _oOptions.bRegexNegateSingleLine ? TRUE : FALSE ) ) { return false; }

			if ( !_psBinary->WriteUi32( _oOptions.uiRegexFlavor ) ) { return false; }
			if ( !_psBinary->WriteUi32( _oOptions.bsByteswap ) ) { return false; }

			if ( !_psBinary->WriteStringU16( _oOptions.wsFromText ) ) { return false; }
			if ( !_psBinary->WriteStringU16( _oOptions.wsToText ) ) { return false; }

			if ( !_psBinary->WriteUi32( _oOptions.vFromHistory.size() ) ) { return false; }
			for ( size_t I = 0; I < _oOptions.vFromHistory.size(); ++I ) {
				if ( !_psBinary->WriteStringU16( _oOptions.vFromHistory[I] ) ) { return false; }
			}
			if ( !_psBinary->WriteUi32( _oOptions.vToHistory.size() ) ) { return false; }
			for ( size_t I = 0; I < _oOptions.vToHistory.size(); ++I ) {
				if ( !_psBinary->WriteStringU16( _oOptions.vToHistory[I] ) ) { return false; }
			}

		}
		return true;
	}

	// Loads search settings from either a JSON object or a byte buffer.
	bool CMemHack::LoadSearchSettings( const lson::CJsonContainer::LSON_JSON_VALUE * _pjJson, lson::CJsonContainer * _pjcContainer, CStream * _psBinary, MX_OPTIONS &_oOptions, uint32_t _ui32Version ) {
		if ( nullptr == _pjJson && _psBinary == nullptr ) { return false; }
		if ( _pjJson ) {
			const lson::CJsonContainer::LSON_JSON_VALUE * pjvVal;
			MX_JSON_GET_BOOL( _DEC_S_7880ABD5_MemImage, _oOptions.bMemImage, pjvVal, _pjJson );
			MX_JSON_GET_BOOL( _DEC_S_5503B2D4_MemPrivate, _oOptions.bMemPrivate, pjvVal, _pjJson );
			MX_JSON_GET_BOOL( _DEC_S_C1B1C624_MemMapped, _oOptions.bMemMapped, pjvVal, _pjJson );

			MX_JSON_GET_NUMBER( _DEC_S_9346CF9D_ThreadPriority, _oOptions.iThreadPriority, INT, pjvVal, _pjJson );
			MX_JSON_GET_NUMBER( _DEC_S_908E909C_BufferSize, _oOptions.ui64BufferSize, UINT64, pjvVal, _pjJson );

			MX_JSON_GET_BOOL( _DEC_S_9C78FB44_PauseTarget, _oOptions.bPauseTarget, pjvVal, _pjJson );
			MX_JSON_GET_BOOL( _DEC_S_017D084F_UseEpsilon, _oOptions.bUseEpsilon, pjvVal, _pjJson );
			MX_JSON_GET_BOOL( _DEC_S_8FF5F221_SmartEpsilon, _oOptions.bSmartEpsilon, pjvVal, _pjJson );
			MX_JSON_GET_NUMBER( _DEC_S_7183A384_Epsilon, _oOptions.dEpsilon, DOUBLE, pjvVal, _pjJson );
			MX_JSON_GET_NUMBER( _DEC_S_AE3F9CFF_Alignment, _oOptions.ui32Alignment, UINT32, pjvVal, _pjJson );
			MX_JSON_GET_BOOL( _DEC_S_22E9689D_Aligned, _oOptions.bAligned, pjvVal, _pjJson );
			MX_JSON_GET_BOOL( _DEC_S_C5BA9DF2_SameAsOriginal, _oOptions.bSameAsOriginal, pjvVal, _pjJson );

			MX_JSON_GET_BOOL( _DEC_S_13EE0CBD_MatchCase, _oOptions.bMatchCase, pjvVal, _pjJson );
			MX_JSON_GET_BOOL( _DEC_S_E2A98D3C_WholeWord, _oOptions.bWholeWord, pjvVal, _pjJson );
			MX_JSON_GET_BOOL( _DEC_S_D29678C3_IsHex, _oOptions.bIsHex, pjvVal, _pjJson );
			MX_JSON_GET_BOOL( _DEC_S_0769B140_ResolveEscapes, _oOptions.bResolveEscapes, pjvVal, _pjJson );
			MX_JSON_GET_BOOL( _DEC_S_220D47BC_Wildcards, _oOptions.bWildcard, pjvVal, _pjJson );
			MX_JSON_GET_BOOL( _DEC_S_87690053_LingIgnoreCase, _oOptions.bLingIgnoreCase, pjvVal, _pjJson );
			MX_JSON_GET_BOOL( _DEC_S_4AAB5CCD_LinkIgnoreDiacritic, _oOptions.bLingIgnoreDiacritic, pjvVal, _pjJson );
			MX_JSON_GET_BOOL( _DEC_S_D24B1BCF_IgnoreKana, _oOptions.bIgnoreKana, pjvVal, _pjJson );
			MX_JSON_GET_BOOL( _DEC_S_CEB15581_IgnoreNoSpace, _oOptions.bIgnoreNoSpace, pjvVal, _pjJson );
			MX_JSON_GET_BOOL( _DEC_S_80E4546E_IgnoreSymbols, _oOptions.bIgnoreSymbols, pjvVal, _pjJson );
			MX_JSON_GET_BOOL( _DEC_S_C0E4A373_IgnoreWidth, _oOptions.bIgnoreWidth, pjvVal, _pjJson );
			MX_JSON_GET_BOOL( _DEC_S_2937FA36_RegexSingleLine, _oOptions.bRegexSingleLine, pjvVal, _pjJson );
			MX_JSON_GET_BOOL( _DEC_S_4F5DA621_RegexMultiLine, _oOptions.bRegexMultiLine, pjvVal, _pjJson );
			MX_JSON_GET_BOOL( _DEC_S_6AE0CEA2_RegexExtended, _oOptions.bRegexExtended, pjvVal, _pjJson );
			MX_JSON_GET_BOOL( _DEC_S_3EE2DC3A_RegexFindLongest, _oOptions.bRegexFindLongest, pjvVal, _pjJson );
			MX_JSON_GET_BOOL( _DEC_S_AA86E6A0_RegexNegateSingleLine, _oOptions.bRegexNegateSingleLine, pjvVal, _pjJson );
			
			MX_JSON_GET_NUMBER( _DEC_S_095E5602_RegexFlavor, _oOptions.uiRegexFlavor, UINT32, pjvVal, _pjJson );
			MX_JSON_GET_NUMBER( _DEC_S_12EECDA8_ByteSwap, _oOptions.bsByteswap, UINT32, pjvVal, _pjJson );

			MX_JSON_GET_STRING( _DEC_S_19280E4E_From, _oOptions.wsFromText, pjvVal, _pjJson );
			MX_JSON_GET_STRING( _DEC_S_4203F666_To, _oOptions.wsToText, pjvVal, _pjJson )


			pjvVal = _pjcContainer->GetMemberByName( (*_pjJson), _DEC_S_1A995A53_FromHistory );
			if ( pjvVal && pjvVal->vtType == lson::CJsonContainer::LSON_VT_ARRAY ) {
				for ( size_t I = 0; I < pjvVal->vArray.size(); ++I ) {
					const lson::CJsonContainer::LSON_JSON_VALUE & jvArrayVal = _pjcContainer->GetValue( pjvVal->vArray[I] );
					if ( jvArrayVal.vtType == lson::CJsonContainer::LSON_VT_STRING ) {
						CSecureString ssTmp;
						CUtilities::ResolveAllEscapes( _pjcContainer->GetString( jvArrayVal.u.stString ), ssTmp, true );
						_oOptions.vFromHistory.push_back( ee::CExpEval::ToUtf16( ssTmp ) );
					}
				}
			}

			pjvVal = _pjcContainer->GetMemberByName( (*_pjJson), _DEC_S_B517206A_ToHistory );
			if ( pjvVal && pjvVal->vtType == lson::CJsonContainer::LSON_VT_ARRAY ) {
				for ( size_t I = 0; I < pjvVal->vArray.size(); ++I ) {
					const lson::CJsonContainer::LSON_JSON_VALUE & jvArrayVal = _pjcContainer->GetValue( pjvVal->vArray[I] );
					if ( jvArrayVal.vtType == lson::CJsonContainer::LSON_VT_STRING ) {
						CSecureString ssTmp;
						CUtilities::ResolveAllEscapes( _pjcContainer->GetString( jvArrayVal.u.stString ), ssTmp, true );
						_oOptions.vToHistory.push_back( ee::CExpEval::ToUtf16( ssTmp ) );
					}
				}
			}

		}
		else {
			uint8_t ui8Tmp;
			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bMemImage = ui8Tmp ? TRUE : FALSE;
			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bMemPrivate = ui8Tmp ? TRUE : FALSE;
			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bMemMapped = ui8Tmp ? TRUE : FALSE;

			if ( !_psBinary->ReadI32( _oOptions.iThreadPriority ) ) { return false; }
			if ( !_psBinary->ReadUi64( _oOptions.ui64BufferSize ) ) { return false; }

			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bPauseTarget = ui8Tmp ? TRUE : FALSE;
			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bUseEpsilon = ui8Tmp ? TRUE : FALSE;
			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bSmartEpsilon = ui8Tmp ? TRUE : FALSE;
			if ( !_psBinary->ReadF64( _oOptions.dEpsilon ) ) { return false; }
			if ( !_psBinary->ReadUi32( _oOptions.ui32Alignment ) ) { return false; }
			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bAligned = ui8Tmp ? TRUE : FALSE;
			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bSameAsOriginal = ui8Tmp ? TRUE : FALSE;

			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bMatchCase = ui8Tmp ? TRUE : FALSE;
			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bWholeWord = ui8Tmp ? TRUE : FALSE;
			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bIsHex = ui8Tmp ? TRUE : FALSE;
			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bResolveEscapes = ui8Tmp ? TRUE : FALSE;
			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bWildcard = ui8Tmp ? TRUE : FALSE;
			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bLingIgnoreCase = ui8Tmp ? TRUE : FALSE;
			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bLingIgnoreDiacritic = ui8Tmp ? TRUE : FALSE;
			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bIgnoreKana = ui8Tmp ? TRUE : FALSE;
			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bIgnoreNoSpace = ui8Tmp ? TRUE : FALSE;
			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bIgnoreSymbols = ui8Tmp ? TRUE : FALSE;
			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bIgnoreWidth = ui8Tmp ? TRUE : FALSE;
			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bRegexSingleLine = ui8Tmp ? TRUE : FALSE;
			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bRegexMultiLine = ui8Tmp ? TRUE : FALSE;
			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bRegexExtended = ui8Tmp ? TRUE : FALSE;
			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bRegexFindLongest = ui8Tmp ? TRUE : FALSE;
			if ( !_psBinary->ReadUi8( ui8Tmp ) ) { return false; }
			_oOptions.bRegexNegateSingleLine = ui8Tmp ? TRUE : FALSE;

			if ( !_psBinary->ReadUi32( _oOptions.uiRegexFlavor ) ) { return false; }
			if ( !_psBinary->ReadUi32( _oOptions.bsByteswap ) ) { return false; }

			if ( !_psBinary->ReadStringU16( _oOptions.wsFromText ) ) { return false; }
			if ( !_psBinary->ReadStringU16( _oOptions.wsToText ) ) { return false; }

			uint32_t ui32Tmp;
			if ( !_psBinary->ReadUi32( ui32Tmp ) ) { return false; }
			for ( size_t I = 0; I < ui32Tmp; ++I ) {
				CSecureWString wsTmp;
				if ( !_psBinary->ReadStringU16( wsTmp ) ) { return false; }
				_oOptions.vFromHistory.push_back( wsTmp );
			}
			if ( !_psBinary->ReadUi32( ui32Tmp ) ) { return false; }
			for ( size_t I = 0; I < ui32Tmp; ++I ) {
				CSecureWString wsTmp;
				if ( !_psBinary->ReadStringU16( wsTmp ) ) { return false; }
				_oOptions.vToHistory.push_back( wsTmp );
			}
		}
		return true;
	}

}	// namespace mx
