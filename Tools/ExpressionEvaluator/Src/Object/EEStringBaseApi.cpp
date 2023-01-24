#include "EEStringBaseApi.h"
#include "EEString.h"

#include <iomanip>
#include <sstream>

namespace ee {

	// == Functions.
	// Creates an EE_RESULT using a best-fit interpretation of the string as a number.
	CExpEvalContainer::EE_RESULT CStringBaseApi::BestFitResult( const std::string &_sIn ) {
		CExpEvalContainer::EE_RESULT rTmp;
		uint8_t ui8Base;
		EE_STRING_NUMBER_CLASS sncClass = ee::CExpEval::ClassifyString( _sIn, &ui8Base );
		switch ( sncClass ) {
			case EE_SNC_UNSIGNED : {
				bool bOverflow = false;
				rTmp.u.ui64Val = ee::CExpEval::StoULL( _sIn.c_str(),
					ui8Base,
					nullptr,
					UINT64_MAX,
					&bOverflow );
				if ( bOverflow && ui8Base == 0 ) {
					rTmp.u.dVal = ee::CExpEval::AtoF( _sIn.c_str(), nullptr, &bOverflow );
					if ( !bOverflow ) {
						rTmp.ncType = EE_NC_FLOATING;
						break;
					}
				}

				rTmp.ncType = EE_NC_UNSIGNED;
				break;
			}
			case EE_SNC_SIGNED : {
				bool bOverflow = false;
				rTmp.u.ui64Val = ee::CExpEval::StoULL( _sIn.c_str(),
					ui8Base,
					nullptr,
					UINT64_MAX / 2 + 1,
					&bOverflow );
				rTmp.ncType = EE_NC_SIGNED;
				if ( bOverflow && ui8Base == 0 ) {
					rTmp.u.dVal = ee::CExpEval::AtoF( _sIn.c_str(), nullptr, &bOverflow );
					if ( !bOverflow ) {
						rTmp.ncType = EE_NC_FLOATING;
						break;
					}
				}

				
				break;
			}
			case EE_NC_FLOATING : {
				rTmp.ncType = EE_NC_FLOATING;
				bool bErrored = false;
				size_t sEaten = 0;
				rTmp.u.dVal = ee::CExpEval::AtoF( _sIn.c_str(), &sEaten, &bErrored );
				if ( bErrored || !sEaten ) {
					rTmp.u.dVal = ::nan( "" );
				}
				break;
			}
			default : {
				rTmp.ncType = EE_NC_FLOATING;
				rTmp.u.dVal = std::nan( "" );
			}
		}
		return rTmp;
	}

	// Gets the object as an ASCII string.
	CExpEvalContainer::EE_RESULT CStringBaseApi::Ascii( const std::string &_sIn, CExpEvalContainer * _peecCont ) {
		CExpEvalContainer::EE_RESULT rRet = { EE_NC_INVALID };
		CString * psObj = reinterpret_cast<CString *>(_peecCont->AllocateObject<CString>());
		if ( psObj ) {
			psObj->m_sObj = ee::CExpEval::ToAscii( _sIn );
			psObj->Dirty();
			rRet = psObj->CreateResult();
		}

		return rRet;
	}

	// Gets the binary form of the object as a string (0b****).
	CExpEvalContainer::EE_RESULT CStringBaseApi::Bin( const std::string &_sIn, CExpEvalContainer * _peecCont ) {
		CExpEvalContainer::EE_RESULT rRet = { EE_NC_INVALID };

		CExpEvalContainer::EE_RESULT rTmp = BestFitResult( _sIn );
		CString * psObj = reinterpret_cast<CString *>(_peecCont->AllocateObject<CString>());
		if ( psObj ) {
			switch ( rTmp.ncType ) {
				case EE_NC_UNSIGNED : {
					psObj->SetString( ee::CExpEval::ToBinary( rTmp.u.ui64Val ) );
					break;
				}
				case EE_NC_SIGNED : {
					psObj->SetString( ee::CExpEval::ToBinary( rTmp.u.i64Val ) );
					break;
				}
				case EE_NC_FLOATING : {
					psObj->SetString( ee::CExpEval::ToBinary( rTmp.u.dVal ) );
					break;
				}
				default : {}
			}
			rRet = psObj->CreateResult();
		}

		return rRet;
	}

	// Gets the boolean form of the object as a string (0b****).
	CExpEvalContainer::EE_RESULT CStringBaseApi::Bool( const std::string &_sIn, CExpEvalContainer * _peecCont ) {
		CExpEvalContainer::EE_RESULT rRet = { EE_NC_INVALID };


		CString * psObj = reinterpret_cast<CString *>(_peecCont->AllocateObject<CString>());
		if ( psObj ) {
			psObj->SetString( _sIn.size() ? "True" : "False" );
			return psObj->CreateResult();
		}

		return rRet;
	}

	// Returns the character that represents the specified Unicode. 
	CExpEvalContainer::EE_RESULT CStringBaseApi::Chr( const std::string &_sIn, CExpEvalContainer * _peecCont ) {
		CExpEvalContainer::EE_RESULT rRet = { EE_NC_INVALID };

		CExpEvalContainer::EE_RESULT rTmp = BestFitResult( _sIn );
		CString * psObj = reinterpret_cast<CString *>(_peecCont->AllocateObject<CString>());
		if ( psObj ) {
			rTmp = _peecCont->ConvertResultOrObject( rTmp, EE_NC_UNSIGNED );
			uint32_t ui32Len;
			uint32_t ui32Val = ee::CExpEval::Utf32ToUtf8( static_cast<uint32_t>(rTmp.u.ui64Val), ui32Len );
			std::string sTmp;
			for ( uint32_t I = 0; I < ui32Len; ++I ) {
				sTmp.push_back( static_cast<std::string::value_type>(ui32Val >> (I * sizeof( std::string::value_type ) * 8)) );
			}
			psObj->SetString( sTmp );
			rRet = psObj->CreateResult();
		}

		return rRet;
	}

	// Gets the string interpreted to its best fit and then converted to float. 
	CExpEvalContainer::EE_RESULT CStringBaseApi::Float( const std::string &_sIn, CExpEvalContainer * _peecCont ) {
		CExpEvalContainer::EE_RESULT rTmp = BestFitResult( _sIn );
		return _peecCont->ConvertResultOrObject( rTmp, EE_NC_FLOATING );
	}

	// Gets the string interpreted to its best fit and then converted to int64_t. 
	CExpEvalContainer::EE_RESULT CStringBaseApi::Int( const std::string &_sIn, CExpEvalContainer * _peecCont ) {
		CExpEvalContainer::EE_RESULT rTmp = BestFitResult( _sIn );
		return _peecCont->ConvertResultOrObject( rTmp, EE_NC_SIGNED );
	}

	// Gets the hexadecimal form of the object as a string (0x****).
	CExpEvalContainer::EE_RESULT CStringBaseApi::Hex( const std::string &_sIn, CExpEvalContainer * _peecCont ) {
		CExpEvalContainer::EE_RESULT rRet = { EE_NC_INVALID };

		CExpEvalContainer::EE_RESULT rTmp = BestFitResult( _sIn );
		CString * psObj = reinterpret_cast<CString *>(_peecCont->AllocateObject<CString>());
		if ( psObj ) {
			switch ( rTmp.ncType ) {
				case EE_NC_UNSIGNED : {
					psObj->SetString( ee::CExpEval::ToHex( rTmp.u.ui64Val ) );
					break;
				}
				case EE_NC_SIGNED : {
					psObj->SetString( ee::CExpEval::ToHex( rTmp.u.i64Val ) );
					break;
				}
				case EE_NC_FLOATING : {
					psObj->SetString( ee::CExpEval::ToHex( rTmp.u.dVal ) );
					break;
				}
				default : {}
			}
			rRet = psObj->CreateResult();
		}

		return rRet;
	}

	// Gets the octadecimal form of the object as a string (0o****).
	CExpEvalContainer::EE_RESULT CStringBaseApi::Oct( const std::string &_sIn, CExpEvalContainer * _peecCont ) {
		CExpEvalContainer::EE_RESULT rRet = { EE_NC_INVALID };

		CExpEvalContainer::EE_RESULT rTmp = BestFitResult( _sIn );
		CString * psObj = reinterpret_cast<CString *>(_peecCont->AllocateObject<CString>());
		if ( psObj ) {
			switch ( rTmp.ncType ) {
				case EE_NC_UNSIGNED : {
					psObj->SetString( ee::CExpEval::ToOct( rTmp.u.ui64Val ) );
					break;
				}
				case EE_NC_SIGNED : {
					psObj->SetString( ee::CExpEval::ToOct( rTmp.u.i64Val ) );
					break;
				}
				case EE_NC_FLOATING : {
					psObj->SetString( ee::CExpEval::ToOct( rTmp.u.dVal ) );
					break;
				}
				default : {}
			}
			rRet = psObj->CreateResult();
		}

		return rRet;
	}

	// Returns the ordinal value of the object as a Unicode character (always EE_NC_UNSIGNED).
	CExpEvalContainer::EE_RESULT CStringBaseApi::Ord( const std::string &_sIn, CExpEvalContainer * /*_peecCont*/ ) {
		CExpEvalContainer::EE_RESULT rTmp = { EE_NC_UNSIGNED };
		if ( !_sIn.size() ) {
			rTmp.u.ui64Val = 0;
			return rTmp;
		}
		rTmp.u.ui64Val = ee::CExpEval::NextUtf8Char( &_sIn[0], _sIn.size() );
		return rTmp;
	}

	// Updates an optimization table for improving array-access speeds.
	void CStringBaseApi::UpdateArrayAccessOptimizer( const std::string &_sIn,
		std::vector<size_t> &_vArray,
		size_t _stIdx, uint32_t &_ui32Result ) {
		if ( _stIdx >= _vArray.size() ) {
			_vArray.resize( _stIdx + 10, EE_UTF_INVALID );
			// Simplifies logic below.
			_vArray[0] = 0;
		}
		size_t I = _stIdx;
		if ( _vArray[I] == EE_UTF_INVALID ) {
			while ( I-- ) {
				if ( _vArray[I] != EE_UTF_INVALID ) { break; }
			}
		}
		size_t sSize = 0;
		_ui32Result = EE_UTF_INVALID;
		uint32_t ui32This = EE_UTF_INVALID;
		for ( ; I <= _stIdx; ++I ) {
			size_t sCheckIdx = _vArray[I];
			ui32This = ee::CExpEval::NextUtf8Char( &_sIn[sCheckIdx], _sIn.size() - sCheckIdx, &sSize );
			if ( !sSize ) { return; }

			if ( I + 1 < _vArray.size() ) {
				_vArray[I+1] = sCheckIdx + sSize;
			}
			if ( I == _stIdx ) {
				_ui32Result = ui32This;
				return;
			}
		}
		
	}

}	// namespace ee
