#pragma once

#include "EEArrayBase.h"


namespace ee {

	class CDefaultArray : public CArrayBase {
	public :
		CDefaultArray( CExpEvalContainer * _peecContainer ) :
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
			catch ( const std::bad_alloc & /*_eE*/ ) {
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
		 * \note If the array has exactly 1 element, that element is set to \p _rStart.
		 * \note The interpolation mode is chosen by CExpEvalContainer::GetCastType() using \p _rStart and \p _rEnd:
		 * - EE_NC_FLOATING: linear interpolation in double.
		 * - EE_NC_SIGNED / EE_NC_UNSIGNED: interpolation in double followed by conversion back to integer storage.
		 * \note This function depends on ConvertResultOrObject() for type coercion; conversion failure yields false.
		 */
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

		/**
		 * \brief Reads a value from the array.
		 *
		 * \param _sIdx The zero-based element index to read.
		 * \param _rRet Receives the value stored at \p _sIdx.
		 * \return Returns true if the read succeeded; false otherwise.
		 *
		 * \note Returns false if \p _sIdx is out of range.
		 */
		virtual bool								ReadValue( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }
			_rRet = m_vArray[_sIdx];
			return true;
		}

		/**
		 * \brief Writes a value to the array.
		 *
		 * \param _sIdx The zero-based element index to write.
		 * \param _rRet On input, supplies the value to write. On output, receives the stored value.
		 * \return Returns true if the write succeeded; false otherwise.
		 *
		 * \note The output value in \p _rRet may differ from the input if the element type enforces conversion or clamping.
		 * \note Returns false if \p _sIdx is out of range.
		 */
		virtual bool								WriteValue( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( m_vArray.size() <= _sIdx ) { return false; }

			m_vArray[_sIdx] = _rRet;
			_rRet = m_vArray[_sIdx];
			return true;
		}

		/**
		 * \brief Applies the \c += operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify.
		 * \param _rRet On input, supplies the right-hand operand. On output, receives the resulting stored value.
		 * \return Returns true if the operation succeeded; false otherwise.
		 *
		 * \note Both operands are coerced to a common numeric type via GetCastType() and ConvertResultOrObject().
		 * \note Returns false if \p _sIdx is out of range or if type conversion fails.
		 */
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

		/**
		 * \brief Applies the \c -= operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify.
		 * \param _rRet On input, supplies the right-hand operand. On output, receives the resulting stored value.
		 * \return Returns true if the operation succeeded; false otherwise.
		 *
		 * \note Both operands are coerced to a common numeric type via GetCastType() and ConvertResultOrObject().
		 * \note Returns false if \p _sIdx is out of range or if type conversion fails.
		 */
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

		/**
		 * \brief Applies the \c *= operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify.
		 * \param _rRet On input, supplies the right-hand operand. On output, receives the resulting stored value.
		 * \return Returns true if the operation succeeded; false otherwise.
		 *
		 * \note Both operands are coerced to a common numeric type via GetCastType() and ConvertResultOrObject().
		 * \note Returns false if \p _sIdx is out of range or if type conversion fails.
		 */
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

		/**
		 * \brief Applies the \c /= operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify.
		 * \param _rRet On input, supplies the right-hand operand. On output, receives the resulting stored value.
		 * \return Returns true if the operation succeeded; false otherwise.
		 *
		 * \note Both operands are coerced to a common numeric type via GetCastType() and ConvertResultOrObject().
		 * \note Division by zero handling is implementation-defined (integer division will follow C++ rules).
		 * \note Returns false if \p _sIdx is out of range or if type conversion fails.
		 */
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

		/**
		 * \brief Applies the \c %= operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify.
		 * \param _rRet On input, supplies the right-hand operand. On output, receives the resulting stored value.
		 * \return Returns true if the operation succeeded; false otherwise.
		 *
		 * \note Both operands are coerced to a common numeric type via GetCastType() and ConvertResultOrObject().
		 * \note For floating-point operands, std::fmod() is used.
		 * \note Returns false if \p _sIdx is out of range or if type conversion fails.
		 */
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

		/**
		 * \brief Applies the \c <<= operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify.
		 * \param _rRet On input, supplies the shift amount. On output, receives the resulting stored value.
		 * \return Returns true if the operation succeeded; false otherwise.
		 *
		 * \note Both operands are coerced to a common numeric type via GetCastType() and ConvertResultOrObject().
		 * \note For floating-point values, this is implemented as multiplication by pow(2, shift).
		 * \note Returns false if \p _sIdx is out of range or if type conversion fails.
		 */
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

		/**
		 * \brief Applies the \c >>= operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify.
		 * \param _rRet On input, supplies the shift amount. On output, receives the resulting stored value.
		 * \return Returns true if the operation succeeded; false otherwise.
		 *
		 * \note Both operands are coerced to a common numeric type via GetCastType() and ConvertResultOrObject().
		 * \note For floating-point values, this is implemented as division by pow(2, shift).
		 * \note Returns false if \p _sIdx is out of range or if type conversion fails.
		 */
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

		/**
		 * \brief Applies the \c ^= operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify.
		 * \param _rRet On input, supplies the right-hand operand. On output, receives the resulting stored value.
		 * \return Returns true if the operation succeeded; false otherwise.
		 *
		 * \note Only integer types are supported. Floating-point returns false.
		 * \note Both operands are coerced to a common numeric type via GetCastType() and ConvertResultOrObject().
		 * \note This is named CarrotEquals() to represent the caret (^) operator.
		 */
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

		/**
		 * \brief Applies the \c |= operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify.
		 * \param _rRet On input, supplies the right-hand operand. On output, receives the resulting stored value.
		 * \return Returns true if the operation succeeded; false otherwise.
		 *
		 * \note Only integer types are supported. Floating-point returns false.
		 * \note Both operands are coerced to a common numeric type via GetCastType() and ConvertResultOrObject().
		 */
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

		/**
		 * \brief Applies the \c &= operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify.
		 * \param _rRet On input, supplies the right-hand operand. On output, receives the resulting stored value.
		 * \return Returns true if the operation succeeded; false otherwise.
		 *
		 * \note Only integer types are supported. Floating-point returns false.
		 * \note Both operands are coerced to a common numeric type via GetCastType() and ConvertResultOrObject().
		 */
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
