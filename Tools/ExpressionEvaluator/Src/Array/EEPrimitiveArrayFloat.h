#pragma once

#include "EEArrayBase.h"


namespace ee {

	template <typename _tnBaseType>
	class CPrimitiveArrayFloat : public CArrayBase {
	public :
		CPrimitiveArrayFloat( CExpEvalContainer * _peecContainer ) :
			CArrayBase( _peecContainer ) {
		}


		// == Functions.
		// Sets the size of the array.
		virtual bool								SetSize( size_t _sNewSize ) {
			try {
				m_vArray.resize( _sNewSize );
			}
			catch ( const std::bad_alloc /*& _eE*/ ) {
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
				CExpEvalContainer::EE_RESULT rTemp = m_peecContainer->ConvertResultOrObject( _rStart, EE_NC_FLOATING );
				if ( rTemp.ncType == EE_NC_INVALID ) { return false; }
				m_vArray[0] = static_cast<_tnBaseType>(rTemp.u.dVal);
			}
			else {
				CExpEvalContainer::EE_RESULT rTemp0 = m_peecContainer->ConvertResultOrObject( _rStart, EE_NC_FLOATING );
				if ( rTemp0.ncType == EE_NC_INVALID ) { return false; }
				CExpEvalContainer::EE_RESULT rTemp1 = m_peecContainer->ConvertResultOrObject( _rEnd, EE_NC_FLOATING );
				if ( rTemp1.ncType == EE_NC_INVALID ) { return false; }
				double dDivisor = static_cast<double>(m_vArray.size() - 1);
				double dRange = rTemp1.u.dVal - rTemp0.u.dVal;
				for ( size_t I = m_vArray.size(); I--; ) {
					m_vArray[I] = static_cast<_tnBaseType>(dRange * I / dDivisor + rTemp0.u.dVal);
				}
			}
			return true;
		}

		// Reads a value.
		virtual bool								ReadValue( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }
			_rRet.ncType = EE_NC_FLOATING;
			_rRet.u.dVal = m_vArray[_sIdx];
			return true;
		}

		// Writes a value.  _rRet is updated with the actual return value, which will change if the input value is too large or of the wrong type etc.
		virtual bool								WriteValue( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }

			CExpEvalContainer::EE_RESULT rTemp = m_peecContainer->ConvertResultOrObject( _rRet, EE_NC_FLOATING );
			if ( rTemp.ncType == EE_NC_INVALID ) { return false; }
			m_vArray[_sIdx] = static_cast<_tnBaseType>(rTemp.u.dVal);
			_rRet.ncType = EE_NC_FLOATING;
			_rRet.u.dVal = m_vArray[_sIdx];
			return true;
		}

		// Operator +=.
		virtual bool								PlusEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }

			CExpEvalContainer::EE_RESULT rTemp = m_peecContainer->ConvertResultOrObject( _rRet, EE_NC_FLOATING );
			if ( rTemp.ncType == EE_NC_INVALID ) { return false; }
			m_vArray[_sIdx] += static_cast<_tnBaseType>(rTemp.u.dVal);
			_rRet.ncType = EE_NC_FLOATING;
			_rRet.u.dVal = m_vArray[_sIdx];
			return true;
		}

		// Operator -=.
		virtual bool								MinusEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }

			CExpEvalContainer::EE_RESULT rTemp = m_peecContainer->ConvertResultOrObject( _rRet, EE_NC_FLOATING );
			if ( rTemp.ncType == EE_NC_INVALID ) { return false; }
			m_vArray[_sIdx] -= static_cast<_tnBaseType>(rTemp.u.dVal);
			_rRet.ncType = EE_NC_FLOATING;
			_rRet.u.dVal = m_vArray[_sIdx];
			return true;
		}

		// Operator *=.
		virtual bool								TimesEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }

			CExpEvalContainer::EE_RESULT rTemp = m_peecContainer->ConvertResultOrObject( _rRet, EE_NC_FLOATING );
			if ( rTemp.ncType == EE_NC_INVALID ) { return false; }
			m_vArray[_sIdx] *= static_cast<_tnBaseType>(rTemp.u.dVal);
			_rRet.ncType = EE_NC_FLOATING;
			_rRet.u.dVal = m_vArray[_sIdx];
			return true;
		}

		// Operator /=.
		virtual bool								DivideEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }

			CExpEvalContainer::EE_RESULT rTemp = m_peecContainer->ConvertResultOrObject( _rRet, EE_NC_FLOATING );
			if ( rTemp.ncType == EE_NC_INVALID ) { return false; }
			m_vArray[_sIdx] /= static_cast<_tnBaseType>(rTemp.u.dVal);
			_rRet.ncType = EE_NC_FLOATING;
			_rRet.u.dVal = m_vArray[_sIdx];
			return true;
		}

		// Operator %=.
		virtual bool								ModEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }

			CExpEvalContainer::EE_RESULT rTemp = m_peecContainer->ConvertResultOrObject( _rRet, EE_NC_FLOATING );
			if ( rTemp.ncType == EE_NC_INVALID ) { return false; }
			m_vArray[_sIdx] = static_cast<_tnBaseType>(std::fmod( m_vArray[_sIdx], rTemp.u.dVal ));
			_rRet.ncType = EE_NC_FLOATING;
			_rRet.u.dVal = m_vArray[_sIdx];
			return true;
		}

		// Operator <<=.
		virtual bool								ShiftLeftEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }

			CExpEvalContainer::EE_RESULT rTemp = m_peecContainer->ConvertResultOrObject( _rRet, EE_NC_FLOATING );
			if ( rTemp.ncType == EE_NC_INVALID ) { return false; }
			m_vArray[_sIdx] = static_cast<_tnBaseType>(m_vArray[_sIdx] * ::pow( 2.0, rTemp.u.dVal ));
			_rRet.ncType = EE_NC_FLOATING;
			_rRet.u.dVal = m_vArray[_sIdx];
			return true;
		}

		// Operator >>=.
		virtual bool								ShiftRightEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }

			CExpEvalContainer::EE_RESULT rTemp = m_peecContainer->ConvertResultOrObject( _rRet, EE_NC_FLOATING );
			if ( rTemp.ncType == EE_NC_INVALID ) { return false; }
			m_vArray[_sIdx] = static_cast<_tnBaseType>(m_vArray[_sIdx] / ::pow( 2.0, rTemp.u.dVal ));
			_rRet.ncType = EE_NC_FLOATING;
			_rRet.u.dVal = m_vArray[_sIdx];
			return true;
		}

		// Operator ^=.
		virtual bool								CarrotEquals( size_t /*_sIdx*/, CExpEvalContainer::EE_RESULT &/*_rRet*/ ) {
			return false;
		}

		// Operator |=.
		virtual bool								OrEquals( size_t /*_sIdx*/, CExpEvalContainer::EE_RESULT &/*_rRet*/ ) {
			return false;
		}

		// Operator &=.
		virtual bool								AndEquals( size_t /*_sIdx*/, CExpEvalContainer::EE_RESULT &/*_rRet*/ ) {
			return false;
		}

	protected :
		// == Members.
		std::vector<_tnBaseType>					m_vArray;
	};

}	// namespace ee
