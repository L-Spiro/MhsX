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
		/**
		 * \brief Sets the number of elements in the array.
		 *
		 * \param _sNewSize The new size, in elements.
		 * \return Returns true if the resize succeeded; false otherwise.
		 *
		 * \note This function catches std::bad_alloc and returns false on allocation failure.
		 */
		virtual bool								SetSize( size_t _sNewSize ) {
			try {
				m_vArray.resize( _sNewSize );
			}
			catch ( const std::bad_alloc /*& _eE*/ ) {
				return false;
			}
			return true;
		}

		/**
		 * \brief Gets the number of elements in the array.
		 *
		 * \return Returns the current size, in elements.
		 */
		virtual size_t								GetSize() const {
			return m_vArray.size();
		}

		/**
		 * \brief Initializes the array with values interpolated between a start and end expression.
		 *
		 * \param _rStart The starting value/expression for interpolation.
		 * \param _rEnd The ending value/expression for interpolation.
		 * \return Returns true if initialization succeeded; false otherwise.
		 *
		 * \note If the array is empty, this function succeeds and performs no work.
		 * \note If the array has exactly 1 element, that element is set to \p _rStart after conversion to EE_NC_FLOATING.
		 * \note For arrays with more than 1 element, both endpoints are converted to EE_NC_FLOATING and interpolated linearly in double.
		 * \note Each interpolated value is converted to the array element type via static_cast<_tnBaseType>().
		 * \note Conversion failures in ConvertResultOrObject() cause this function to return false.
		 */
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

		/**
		 * \brief Reads a value from the array.
		 *
		 * \param _sIdx The zero-based element index to read.
		 * \param _rRet Receives the value stored at \p _sIdx as EE_NC_FLOATING.
		 * \return Returns true if the read succeeded; false otherwise.
		 *
		 * \note Returns false if \p _sIdx is out of range.
		 * \note The returned value is always reported as EE_NC_FLOATING and stored in \p _rRet.u.dVal.
		 */
		virtual bool								ReadValue( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }
			_rRet.ncType = EE_NC_FLOATING;
			_rRet.u.dVal = m_vArray[_sIdx];
			return true;
		}

		/**
		 * \brief Writes a value to the array.
		 *
		 * \param _sIdx The zero-based element index to write.
		 * \param _rRet On input, supplies the value to write. On output, receives the stored value as EE_NC_FLOATING.
		 * \return Returns true if the write succeeded; false otherwise.
		 *
		 * \note The input \p _rRet is converted to EE_NC_FLOATING via ConvertResultOrObject().
		 * \note The stored value is static_cast to the array element type (_tnBaseType).
		 * \note The output \p _rRet is always reported as EE_NC_FLOATING and stored in \p _rRet.u.dVal.
		 * \note Returns false if \p _sIdx is out of range or if conversion fails.
		 */
		virtual bool								WriteValue( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }

			CExpEvalContainer::EE_RESULT rTemp = m_peecContainer->ConvertResultOrObject( _rRet, EE_NC_FLOATING );
			if ( rTemp.ncType == EE_NC_INVALID ) { return false; }
			m_vArray[_sIdx] = static_cast<_tnBaseType>(rTemp.u.dVal);
			_rRet.ncType = EE_NC_FLOATING;
			_rRet.u.dVal = m_vArray[_sIdx];
			return true;
		}

		/**
		 * \brief Applies the \c += operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify.
		 * \param _rRet On input, supplies the right-hand operand. On output, receives the resulting stored value as EE_NC_FLOATING.
		 * \return Returns true if the operation succeeded; false otherwise.
		 *
		 * \note The right-hand operand is converted to EE_NC_FLOATING via ConvertResultOrObject().
		 * \note The result is stored in the array element type (_tnBaseType) and reported back as EE_NC_FLOATING.
		 * \note Returns false if \p _sIdx is out of range or if conversion fails.
		 */
		virtual bool								PlusEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }

			CExpEvalContainer::EE_RESULT rTemp = m_peecContainer->ConvertResultOrObject( _rRet, EE_NC_FLOATING );
			if ( rTemp.ncType == EE_NC_INVALID ) { return false; }
			m_vArray[_sIdx] += static_cast<_tnBaseType>(rTemp.u.dVal);
			_rRet.ncType = EE_NC_FLOATING;
			_rRet.u.dVal = m_vArray[_sIdx];
			return true;
		}

		/**
		 * \brief Applies the \c -= operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify.
		 * \param _rRet On input, supplies the right-hand operand. On output, receives the resulting stored value as EE_NC_FLOATING.
		 * \return Returns true if the operation succeeded; false otherwise.
		 *
		 * \note The right-hand operand is converted to EE_NC_FLOATING via ConvertResultOrObject().
		 * \note The result is stored in the array element type (_tnBaseType) and reported back as EE_NC_FLOATING.
		 * \note Returns false if \p _sIdx is out of range or if conversion fails.
		 */
		virtual bool								MinusEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }

			CExpEvalContainer::EE_RESULT rTemp = m_peecContainer->ConvertResultOrObject( _rRet, EE_NC_FLOATING );
			if ( rTemp.ncType == EE_NC_INVALID ) { return false; }
			m_vArray[_sIdx] -= static_cast<_tnBaseType>(rTemp.u.dVal);
			_rRet.ncType = EE_NC_FLOATING;
			_rRet.u.dVal = m_vArray[_sIdx];
			return true;
		}

		/**
		 * \brief Applies the \c *= operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify.
		 * \param _rRet On input, supplies the right-hand operand. On output, receives the resulting stored value as EE_NC_FLOATING.
		 * \return Returns true if the operation succeeded; false otherwise.
		 *
		 * \note The right-hand operand is converted to EE_NC_FLOATING via ConvertResultOrObject().
		 * \note The result is stored in the array element type (_tnBaseType) and reported back as EE_NC_FLOATING.
		 * \note Returns false if \p _sIdx is out of range or if conversion fails.
		 */
		virtual bool								TimesEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }

			CExpEvalContainer::EE_RESULT rTemp = m_peecContainer->ConvertResultOrObject( _rRet, EE_NC_FLOATING );
			if ( rTemp.ncType == EE_NC_INVALID ) { return false; }
			m_vArray[_sIdx] *= static_cast<_tnBaseType>(rTemp.u.dVal);
			_rRet.ncType = EE_NC_FLOATING;
			_rRet.u.dVal = m_vArray[_sIdx];
			return true;
		}

		/**
		 * \brief Applies the \c /= operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify.
		 * \param _rRet On input, supplies the right-hand operand. On output, receives the resulting stored value as EE_NC_FLOATING.
		 * \return Returns true if the operation succeeded; false otherwise.
		 *
		 * \note The right-hand operand is converted to EE_NC_FLOATING via ConvertResultOrObject().
		 * \note Division by zero behavior depends on the underlying floating-point semantics.
		 * \note The result is stored in the array element type (_tnBaseType) and reported back as EE_NC_FLOATING.
		 * \note Returns false if \p _sIdx is out of range or if conversion fails.
		 */
		virtual bool								DivideEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }

			CExpEvalContainer::EE_RESULT rTemp = m_peecContainer->ConvertResultOrObject( _rRet, EE_NC_FLOATING );
			if ( rTemp.ncType == EE_NC_INVALID ) { return false; }
			m_vArray[_sIdx] /= static_cast<_tnBaseType>(rTemp.u.dVal);
			_rRet.ncType = EE_NC_FLOATING;
			_rRet.u.dVal = m_vArray[_sIdx];
			return true;
		}

		/**
		 * \brief Applies the \c %= operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify.
		 * \param _rRet On input, supplies the right-hand operand. On output, receives the resulting stored value as EE_NC_FLOATING.
		 * \return Returns true if the operation succeeded; false otherwise.
		 *
		 * \note The right-hand operand is converted to EE_NC_FLOATING via ConvertResultOrObject().
		 * \note The operation is performed using std::fmod() and the result is cast back to _tnBaseType.
		 * \note Returns false if \p _sIdx is out of range or if conversion fails.
		 */
		virtual bool								ModEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }

			CExpEvalContainer::EE_RESULT rTemp = m_peecContainer->ConvertResultOrObject( _rRet, EE_NC_FLOATING );
			if ( rTemp.ncType == EE_NC_INVALID ) { return false; }
			m_vArray[_sIdx] = static_cast<_tnBaseType>(std::fmod( m_vArray[_sIdx], rTemp.u.dVal ));
			_rRet.ncType = EE_NC_FLOATING;
			_rRet.u.dVal = m_vArray[_sIdx];
			return true;
		}

		/**
		 * \brief Applies the \c <<= operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify.
		 * \param _rRet On input, supplies the shift amount. On output, receives the resulting stored value as EE_NC_FLOATING.
		 * \return Returns true if the operation succeeded; false otherwise.
		 *
		 * \note The shift amount is converted to EE_NC_FLOATING via ConvertResultOrObject().
		 * \note For floating-point arrays, this is implemented as multiplication by pow(2, shift).
		 * \note Returns false if \p _sIdx is out of range or if conversion fails.
		 */
		virtual bool								ShiftLeftEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }

			CExpEvalContainer::EE_RESULT rTemp = m_peecContainer->ConvertResultOrObject( _rRet, EE_NC_FLOATING );
			if ( rTemp.ncType == EE_NC_INVALID ) { return false; }
			m_vArray[_sIdx] = static_cast<_tnBaseType>(m_vArray[_sIdx] * ::pow( 2.0, rTemp.u.dVal ));
			_rRet.ncType = EE_NC_FLOATING;
			_rRet.u.dVal = m_vArray[_sIdx];
			return true;
		}

		/**
		 * \brief Applies the \c >>= operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify.
		 * \param _rRet On input, supplies the shift amount. On output, receives the resulting stored value as EE_NC_FLOATING.
		 * \return Returns true if the operation succeeded; false otherwise.
		 *
		 * \note The shift amount is converted to EE_NC_FLOATING via ConvertResultOrObject().
		 * \note For floating-point arrays, this is implemented as division by pow(2, shift).
		 * \note Returns false if \p _sIdx is out of range or if conversion fails.
		 */
		virtual bool								ShiftRightEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }

			CExpEvalContainer::EE_RESULT rTemp = m_peecContainer->ConvertResultOrObject( _rRet, EE_NC_FLOATING );
			if ( rTemp.ncType == EE_NC_INVALID ) { return false; }
			m_vArray[_sIdx] = static_cast<_tnBaseType>(m_vArray[_sIdx] / ::pow( 2.0, rTemp.u.dVal ));
			_rRet.ncType = EE_NC_FLOATING;
			_rRet.u.dVal = m_vArray[_sIdx];
			return true;
		}

		/**
		 * \brief Applies the \c ^= operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify (unused).
		 * \param _rRet The right-hand operand and return value (unused).
		 * \return Returns false.
		 *
		 * \note Bitwise operators are not supported for this floating-point array type.
		 */
		virtual bool								CarrotEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			static_cast<void>(_sIdx);
			static_cast<void>(_rRet);
			return false;
		}

		/**
		 * \brief Applies the \c |= operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify (unused).
		 * \param _rRet The right-hand operand and return value (unused).
		 * \return Returns false.
		 *
		 * \note Bitwise operators are not supported for this floating-point array type.
		 */
		virtual bool								OrEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			static_cast<void>(_sIdx);
			static_cast<void>(_rRet);
			return false;
		}

		/**
		 * \brief Applies the \c &= operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify (unused).
		 * \param _rRet The right-hand operand and return value (unused).
		 * \return Returns false.
		 *
		 * \note Bitwise operators are not supported for this floating-point array type.
		 */
		virtual bool								AndEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			static_cast<void>(_sIdx);
			static_cast<void>(_rRet);
			return false;
		}

	protected :
		// == Members.
		std::vector<_tnBaseType>					m_vArray;
	};

}	// namespace ee
