#pragma once

#include "EEArrayBase.h"


namespace ee {

	class CDefaultArray : public CArrayBase {
	public :
		CDefaultArray( CExpEvalContainer * _peecContainer ) :
			CArrayBase( _peecContainer ) {
		}


		// == Functions.
		// Sets the size of the array.
		virtual bool								SetSize( size_t _sNewSize ) {
			try {
				m_vArray.resize( _sNewSize );
			}
			catch ( const std::bad_alloc & /*_eE*/ ) {
				return false;
			}
			return true;
		}

		// Gets the size of the array.
		virtual size_t								GetSize() const {
			return m_vArray.size();
		}

		// Initializes the array with values interpolated between the start and end expressions.
		virtual bool								Initialize( const CExpEvalContainer::EE_RESULT &_rStart, const CExpEvalContainer::EE_RESULT &_rEnd ) {
			if ( !m_vArray.size() ) { return true; }
			if ( m_vArray.size() == 1 ) {
				m_vArray[0] = _rStart;
			}
			else {
				ee::EE_NUM_CONSTANTS ncType = CExpEvalContainer::GetCastType( _rStart.ncType, _rEnd.ncType );
				switch ( ncType ) {
					case EE_NC_FLOATING : {
						CExpEvalContainer::EE_RESULT rTemp0 = m_peecContainer->ConvertResultOrObject( _rStart, EE_NC_FLOATING );
						if ( rTemp0.ncType == EE_NC_INVALID ) { return false; }
						CExpEvalContainer::EE_RESULT rTemp1 = m_peecContainer->ConvertResultOrObject( _rEnd, EE_NC_FLOATING );
						if ( rTemp1.ncType == EE_NC_INVALID ) { return false; }
						double dDivisor = static_cast<double>(m_vArray.size() - 1);
						double dRange = rTemp1.u.dVal - rTemp0.u.dVal;
						for ( size_t I = m_vArray.size(); I--; ) {
							CExpEvalContainer::EE_RESULT rThis;
							rThis.ncType = EE_NC_FLOATING;
							rThis.u.dVal = dRange * I / dDivisor + rTemp0.u.dVal;
							m_vArray[I] = rThis;
						}
						break;
					}
					case EE_NC_SIGNED : {
						// Neither input was a float; we can assume both inputs were in int form.
						CExpEvalContainer::EE_RESULT ncIntStart = m_peecContainer->ConvertResultOrObject( _rStart, EE_NC_SIGNED );
						if ( ncIntStart.ncType == EE_NC_INVALID ) { return false; }
						CExpEvalContainer::EE_RESULT rTemp0 = m_peecContainer->ConvertResultOrObject( _rStart, EE_NC_FLOATING );
						if ( rTemp0.ncType == EE_NC_INVALID ) { return false; }
						CExpEvalContainer::EE_RESULT rTemp1 = m_peecContainer->ConvertResultOrObject( _rEnd, EE_NC_FLOATING );
						if ( rTemp1.ncType == EE_NC_INVALID ) { return false; }
						double dDivisor = static_cast<double>(m_vArray.size() - 1);
						double dRange = static_cast<double>(_rEnd.u.i64Val - _rStart.u.i64Val);
						for ( size_t I = m_vArray.size(); I--; ) {
							CExpEvalContainer::EE_RESULT rThis;
							rThis.ncType = EE_NC_SIGNED;
							rThis.u.i64Val = static_cast<int64_t>(dRange * I / dDivisor + rTemp0.u.dVal) + ncIntStart.u.i64Val;
							m_vArray[I] = rThis;
						}
						break;
					}
					case EE_NC_UNSIGNED : {
						// Neither input was a float; we can assume both inputs were in int form.
						CExpEvalContainer::EE_RESULT ncIntStart = m_peecContainer->ConvertResultOrObject( _rStart, EE_NC_UNSIGNED );
						if ( ncIntStart.ncType == EE_NC_INVALID ) { return false; }
						CExpEvalContainer::EE_RESULT rTemp0 = m_peecContainer->ConvertResultOrObject( _rStart, EE_NC_FLOATING );
						if ( rTemp0.ncType == EE_NC_INVALID ) { return false; }
						CExpEvalContainer::EE_RESULT rTemp1 = m_peecContainer->ConvertResultOrObject( _rEnd, EE_NC_FLOATING );
						if ( rTemp1.ncType == EE_NC_INVALID ) { return false; }
						double dDivisor = static_cast<double>(m_vArray.size() - 1);
						double dRange = static_cast<double>(_rEnd.u.ui64Val - _rStart.u.ui64Val);
						for ( size_t I = m_vArray.size(); I--; ) {
							CExpEvalContainer::EE_RESULT rThis;
							rThis.ncType = EE_NC_UNSIGNED;
							rThis.u.ui64Val = static_cast<uint64_t>(dRange * I / dDivisor + rTemp0.u.dVal) + ncIntStart.u.ui64Val;
							m_vArray[I] = rThis;
						}
						break;
					}
					default : { return false; }
				}
				
			}
			return true;
		}

		// Reads a value.
		virtual bool								ReadValue( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }
			_rRet = m_vArray[_sIdx];
			return true;
		}

		// Writes a value.  _rRet is updated with the actual return value, which will change if the input value is too large or of the wrong type etc.
		virtual bool								WriteValue( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }

			m_vArray[_sIdx] = _rRet;
			_rRet = m_vArray[_sIdx];
			return true;
		}

		// Operator +=.
		virtual bool								PlusEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }
			ee::EE_NUM_CONSTANTS ncType = CExpEvalContainer::GetCastType( m_vArray[_sIdx].ncType, _rRet.ncType );

			CExpEvalContainer::EE_RESULT rTemp = m_peecContainer->ConvertResultOrObject( _rRet, ncType );
			if ( rTemp.ncType == EE_NC_INVALID ) { return false; }
			m_vArray[_sIdx] = m_peecContainer->ConvertResultOrObject( m_vArray[_sIdx], ncType );
			if ( rTemp.ncType == EE_NC_INVALID ) { return false; }
			switch ( ncType ) {
				case EE_NC_FLOATING : { m_vArray[_sIdx].u.dVal += rTemp.u.dVal; break; }
				case EE_NC_SIGNED : { m_vArray[_sIdx].u.i64Val += rTemp.u.i64Val; break; }
				case EE_NC_UNSIGNED : { m_vArray[_sIdx].u.ui64Val += rTemp.u.ui64Val; break; }
				// case EE_NC_OBJECT : {}
				default : { return false; }
			}
			_rRet = m_vArray[_sIdx];
			return true;
		}

		// Operator -=.
		virtual bool								MinusEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }
			ee::EE_NUM_CONSTANTS ncType = CExpEvalContainer::GetCastType( m_vArray[_sIdx].ncType, _rRet.ncType );

			CExpEvalContainer::EE_RESULT rTemp = m_peecContainer->ConvertResultOrObject( _rRet, ncType );
			if ( rTemp.ncType == EE_NC_INVALID ) { return false; }
			m_vArray[_sIdx] = m_peecContainer->ConvertResultOrObject( m_vArray[_sIdx], ncType );
			if ( m_vArray[_sIdx].ncType == EE_NC_INVALID ) { return false; }
			switch ( ncType ) {
				case EE_NC_FLOATING : { m_vArray[_sIdx].u.dVal -= rTemp.u.dVal; break; }
				case EE_NC_SIGNED : { m_vArray[_sIdx].u.i64Val -= rTemp.u.i64Val; break; }
				case EE_NC_UNSIGNED : { m_vArray[_sIdx].u.ui64Val -= rTemp.u.ui64Val; break; }
				default : { return false; }
			}
			_rRet = m_vArray[_sIdx];
			return true;
		}

		// Operator *=.
		virtual bool								TimesEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }
			ee::EE_NUM_CONSTANTS ncType = CExpEvalContainer::GetCastType( m_vArray[_sIdx].ncType, _rRet.ncType );

			CExpEvalContainer::EE_RESULT rTemp = m_peecContainer->ConvertResultOrObject( _rRet, ncType );
			if ( rTemp.ncType == EE_NC_INVALID ) { return false; }
			m_vArray[_sIdx] = m_peecContainer->ConvertResultOrObject( m_vArray[_sIdx], ncType );
			if ( m_vArray[_sIdx].ncType == EE_NC_INVALID ) { return false; }
			switch ( ncType ) {
				case EE_NC_FLOATING : { m_vArray[_sIdx].u.dVal *= rTemp.u.dVal; break; }
				case EE_NC_SIGNED : { m_vArray[_sIdx].u.i64Val *= rTemp.u.i64Val; break; }
				case EE_NC_UNSIGNED : { m_vArray[_sIdx].u.ui64Val *= rTemp.u.ui64Val; break; }
				default : { return false; }
			}
			_rRet = m_vArray[_sIdx];
			return true;
		}

		// Operator /=.
		virtual bool								DivideEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }
			ee::EE_NUM_CONSTANTS ncType = CExpEvalContainer::GetCastType( m_vArray[_sIdx].ncType, _rRet.ncType );

			CExpEvalContainer::EE_RESULT rTemp = m_peecContainer->ConvertResultOrObject( _rRet, ncType );
			if ( rTemp.ncType == EE_NC_INVALID ) { return false; }
			m_vArray[_sIdx] = m_peecContainer->ConvertResultOrObject( m_vArray[_sIdx], ncType );
			if ( m_vArray[_sIdx].ncType == EE_NC_INVALID ) { return false; }
			switch ( ncType ) {
				case EE_NC_FLOATING : { m_vArray[_sIdx].u.dVal /= rTemp.u.dVal; break; }
				case EE_NC_SIGNED : { m_vArray[_sIdx].u.i64Val /= rTemp.u.i64Val; break; }
				case EE_NC_UNSIGNED : { m_vArray[_sIdx].u.ui64Val /= rTemp.u.ui64Val; break; }
				default : { return false; }
			}
			_rRet = m_vArray[_sIdx];
			return true;
		}

		// Operator %=.
		virtual bool								ModEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }
			ee::EE_NUM_CONSTANTS ncType = CExpEvalContainer::GetCastType( m_vArray[_sIdx].ncType, _rRet.ncType );

			CExpEvalContainer::EE_RESULT rTemp = m_peecContainer->ConvertResultOrObject( _rRet, ncType );
			if ( rTemp.ncType == EE_NC_INVALID ) { return false; }
			m_vArray[_sIdx] = m_peecContainer->ConvertResultOrObject( m_vArray[_sIdx], ncType );
			if ( m_vArray[_sIdx].ncType == EE_NC_INVALID ) { return false; }
			switch ( ncType ) {
				case EE_NC_FLOATING : { m_vArray[_sIdx].u.dVal = std::fmod( m_vArray[_sIdx].u.dVal, rTemp.u.dVal ); break; }
				case EE_NC_SIGNED : { m_vArray[_sIdx].u.i64Val %= rTemp.u.i64Val; break; }
				case EE_NC_UNSIGNED : { m_vArray[_sIdx].u.ui64Val %= rTemp.u.ui64Val; break; }
				default : { return false; }
			}
			_rRet = m_vArray[_sIdx];
			return true;
		}

		// Operator <<=.
		virtual bool								ShiftLeftEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }
			ee::EE_NUM_CONSTANTS ncType = CExpEvalContainer::GetCastType( m_vArray[_sIdx].ncType, _rRet.ncType );

			CExpEvalContainer::EE_RESULT rTemp = m_peecContainer->ConvertResultOrObject( _rRet, ncType );
			if ( rTemp.ncType == EE_NC_INVALID ) { return false; }
			m_vArray[_sIdx] = m_peecContainer->ConvertResultOrObject( m_vArray[_sIdx], ncType );
			if ( m_vArray[_sIdx].ncType == EE_NC_INVALID ) { return false; }
			switch ( ncType ) {
				case EE_NC_FLOATING : { m_vArray[_sIdx].u.dVal *= std::pow( 2.0, rTemp.u.dVal ); break; }
				case EE_NC_SIGNED : { m_vArray[_sIdx].u.i64Val <<= rTemp.u.i64Val; break; }
				case EE_NC_UNSIGNED : { m_vArray[_sIdx].u.ui64Val <<= rTemp.u.ui64Val; break; }
				default : { return false; }
			}
			_rRet = m_vArray[_sIdx];
			return true;
		}

		// Operator >>=.
		virtual bool								ShiftRightEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }
			ee::EE_NUM_CONSTANTS ncType = CExpEvalContainer::GetCastType( m_vArray[_sIdx].ncType, _rRet.ncType );

			CExpEvalContainer::EE_RESULT rTemp = m_peecContainer->ConvertResultOrObject( _rRet, ncType );
			if ( rTemp.ncType == EE_NC_INVALID ) { return false; }
			m_vArray[_sIdx] = m_peecContainer->ConvertResultOrObject( m_vArray[_sIdx], ncType );
			if ( m_vArray[_sIdx].ncType == EE_NC_INVALID ) { return false; }
			switch ( ncType ) {
				case EE_NC_FLOATING : { m_vArray[_sIdx].u.dVal /= std::pow( 2.0, rTemp.u.dVal ); break; }
				case EE_NC_SIGNED : { m_vArray[_sIdx].u.i64Val <<= rTemp.u.i64Val; break; }
				case EE_NC_UNSIGNED : { m_vArray[_sIdx].u.ui64Val <<= rTemp.u.ui64Val; break; }
				default : { return false; }
			}
			_rRet = m_vArray[_sIdx];
			return true;
		}

		// Operator ^=.
		virtual bool								CarrotEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }
			ee::EE_NUM_CONSTANTS ncType = CExpEvalContainer::GetCastType( m_vArray[_sIdx].ncType, _rRet.ncType );

			CExpEvalContainer::EE_RESULT rTemp = m_peecContainer->ConvertResultOrObject( _rRet, ncType );
			if ( rTemp.ncType == EE_NC_INVALID ) { return false; }
			m_vArray[_sIdx] = m_peecContainer->ConvertResultOrObject( m_vArray[_sIdx], ncType );
			if ( m_vArray[_sIdx].ncType == EE_NC_INVALID ) { return false; }
			switch ( ncType ) {
				case EE_NC_FLOATING : { return false; }
				case EE_NC_SIGNED : { m_vArray[_sIdx].u.i64Val ^= rTemp.u.i64Val; break; }
				case EE_NC_UNSIGNED : { m_vArray[_sIdx].u.ui64Val ^= rTemp.u.ui64Val; break; }
				default : { return false; }
			}
			_rRet = m_vArray[_sIdx];
			return true;
		}

		// Operator |=.
		virtual bool								OrEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }
			ee::EE_NUM_CONSTANTS ncType = CExpEvalContainer::GetCastType( m_vArray[_sIdx].ncType, _rRet.ncType );

			CExpEvalContainer::EE_RESULT rTemp = m_peecContainer->ConvertResultOrObject( _rRet, ncType );
			if ( rTemp.ncType == EE_NC_INVALID ) { return false; }
			m_vArray[_sIdx] = m_peecContainer->ConvertResultOrObject( m_vArray[_sIdx], ncType );
			if ( m_vArray[_sIdx].ncType == EE_NC_INVALID ) { return false; }
			switch ( ncType ) {
				case EE_NC_FLOATING : { return false; }
				case EE_NC_SIGNED : { m_vArray[_sIdx].u.i64Val |= rTemp.u.i64Val; break; }
				case EE_NC_UNSIGNED : { m_vArray[_sIdx].u.ui64Val |= rTemp.u.ui64Val; break; }
				default : { return false; }
			}
			_rRet = m_vArray[_sIdx];
			return true;
		}

		// Operator &=.
		virtual bool								AndEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }
			ee::EE_NUM_CONSTANTS ncType = CExpEvalContainer::GetCastType( m_vArray[_sIdx].ncType, _rRet.ncType );

			CExpEvalContainer::EE_RESULT rTemp = m_peecContainer->ConvertResultOrObject( _rRet, ncType );
			if ( rTemp.ncType == EE_NC_INVALID ) { return false; }
			m_vArray[_sIdx] = m_peecContainer->ConvertResultOrObject( m_vArray[_sIdx], ncType );
			if ( m_vArray[_sIdx].ncType == EE_NC_INVALID ) { return false; }
			switch ( ncType ) {
				case EE_NC_FLOATING : { return false; }
				case EE_NC_SIGNED : { m_vArray[_sIdx].u.i64Val &= rTemp.u.i64Val; break; }
				case EE_NC_UNSIGNED : { m_vArray[_sIdx].u.ui64Val &= rTemp.u.ui64Val; break; }
				default : { return false; }
			}
			_rRet = m_vArray[_sIdx];
			return true;
		}

	protected :
		// == Members.
		std::vector<CExpEvalContainer::EE_RESULT>	m_vArray;
	};

}	// namespace ee
