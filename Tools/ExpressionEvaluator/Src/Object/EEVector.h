#pragma once

#include "EEObject.h"


namespace ee {

	class CVector : public CObject {
	public :
		CVector( CExpEvalContainer * _peecContainer ) :
			CObject( _peecContainer ) {
		}


		// == Functions.
		/**
		 * Gets the object type.
		 *
		 * \return Returns CObject::EE_BIT_VECTOR.
		 */
		virtual uint32_t							Type() const { return CObject::Type() | CObject::EE_BIT_VECTOR; }

		/**
		 * Converts to another object of the given type.
		 *
		 * \param _ncType The destination type.
		 * \return If the vector has only 1 element, that element is converted to the given target type, otherwise EE_NC_INVALID is returned.
		 */
		virtual CExpEvalContainer::EE_RESULT		ConvertTo( EE_NUM_CONSTANTS _ncType );

		/**
		 * Initializes this object given another object.
		 *
		 * \param _rObj The source object from which to initialize this object.
		 * \return Returns true if a valid conversion from the given object can be made to a vector.
		 */
		virtual bool								InitializeFrom( const CExpEvalContainer::EE_RESULT &_rObj );

		/**
		 * Creates a string representation of the object, with the string usually assumed to be in UTF-8 format.
		 *
		 * \param _sString Holds the returned string.
		 * \return Returns true if the object can be converted to a string and no memory errors were encountered when doing so.
		 */
		virtual bool								ToString( std::string &_sString, uint32_t _ui32Depth, uint32_t _ui32Flags = EE_TF_NONE );

		/**
		 * Creates a formatted string representation of the vector by passing the format string to each element of the vector.
		 *
		 * \param _sFormat The format string.
		 * \return Returns a string containing each element in the vector formatted according to the format string provied in _sFormat.
		 */
		virtual std::string							FormattedString( const std::string &_sFormat, uint32_t _ui32Flags = EE_TF_NONE );

		/**
		 * Array access.
		 *
		 * \param _i64Idx Index of the item to access.
		 * \return Returns the item at the given index or an invalid item.
		 */
		virtual CExpEvalContainer::EE_RESULT		ArrayAccess( int64_t _i64Idx );

		/**
		 * Extended array access.
		 *
		 * \param _i64Idx0 The start index.
		 * \param _i64Idx1 The end index.
		 * \param _ui32Mask From-start or to-end flags.
		 * \return Returns the resulting vector created from selecting the given items.
		 */
		virtual CExpEvalContainer::EE_RESULT		ArrayAccessEx( int64_t _i64Idx0, int64_t _i64Idx1, uint32_t _ui32Mask );

		/**
		 * Gets the array length.
		 *
		 * \return Returns the length of the vector.
		 */
		virtual size_t								ArrayLength() { return m_vBacking.size(); }


		// == Operators.
		/**
		 * Operator ==.  Returns true if _rRet is another vector of the same length and every element in _rRet compares equal to every element in this vector.
		 *
		 * \param _rRet The comparison object.
		 * \return Returns true if _rRet is a vector, has the same number of items, and each item compares == to each respective item in this vector.
		 */
		virtual bool								Equals( CExpEvalContainer::EE_RESULT &_rRet );

		/**
		 * Operator +.  If _rRet is another vector, a new vector is returned containing the contents of this vector with _rRet appended, otherwise a new vector is returned containing this vector and the new item appended.
		 *
		 * \param _rRet The concatenation object.
		 * \return If _rRet is another vector, a new vector is returned with both this and _rRet’s contents, otherwise an invalid object is returned.
		 */
		virtual CExpEvalContainer::EE_RESULT		Plus( CExpEvalContainer::EE_RESULT &_rRet );

		/**
		 * Operator -.  If _rRet is an object, element, otherwise a new vector is returned with element-wise subtraction of the given value.
		 *
		 * \param _rRet The concatenation object.
		 * \return If _rRet is another vector, a new vector is returned with both this and _rRet’s contents, otherwise an invalid object is returned.
		 */
		virtual CExpEvalContainer::EE_RESULT		Minus( CExpEvalContainer::EE_RESULT &_rRet );

		/**
		 * Operator *.  If _rRet is a vector, element-wise multiplication is performed, otherwise a new vector is returned with scalar multiplication of the given value.
		 *
		 * \param _rRet The concatenation object.
		 * \return If _rRet is another vector, a new vector is returned with both this and _rRet’s contents, otherwise an invalid object is returned.
		 */
		virtual CExpEvalContainer::EE_RESULT		Multiply( CExpEvalContainer::EE_RESULT &_rRet );

		/**
		 * Operator /.  If _rRet is a vector, element-wise division is performed, otherwise a new vector is returned with scalar division by the given value.
		 *
		 * \param _rRet The concatenation object.
		 * \return If _rRet is another vector, a new vector is returned with both this and _rRet’s contents, otherwise an invalid object is returned.
		 */
		virtual CExpEvalContainer::EE_RESULT		Divide( CExpEvalContainer::EE_RESULT &_rRet );

		/**
		 * Operator +=.  Appends the item or all of the items in the vector if the item is a vector.
		 *
		 * \param _rRet The concatenation object.
		 * \return Returns true unless there were memory failures during the concatenation process.
		 */
		virtual bool								PlusEquals( CExpEvalContainer::EE_RESULT &_rRet );

		/**
		 * Operator -=.  Subtracts the given scalar from all elements in this vector.
		 *
		 * \param _rRet The concatenation object.
		 * \return Returns true unless there were memory failures during the concatenation process.
		 */
		virtual bool								MinusEquals( CExpEvalContainer::EE_RESULT &_rRet );

		/**
		 * Operator *=.  Performs scalar or element-wise * with self and given operand.
		 *
		 * \param _rRet The concatenation object.
		 * \return Returns true unless there were memory failures during the concatenation process.
		 */
		virtual bool								TimesEquals( CExpEvalContainer::EE_RESULT &_rRet );

		/**
		 * Operator /=.  Performs scalar or element-wise / with self and given operand.
		 *
		 * \param _rRet The concatenation object.
		 * \return Returns true unless there were memory failures during the concatenation process.
		 */
		virtual bool								DivideEquals( CExpEvalContainer::EE_RESULT &_rRet );

		// == Base API functions.
		/**
		 * \brief Gets the object as an ASCII string.
		 *
		 * Base implementation returns EE_NC_INVALID.  Derived types may override to provide an ASCII-only
		 * representation of the object.
		 *
		 * \return Returns an EE_RESULT containing the ASCII string result, or EE_NC_INVALID if unsupported.
		 */
		virtual CExpEvalContainer::EE_RESULT		Ascii() const { return { EE_NC_INVALID }; }

		/**
		 * \brief Gets the binary form of the object as a string (0b****).
		 *
		 * Base implementation returns EE_NC_INVALID.  Derived types may override to format the object as a
		 * binary literal string.
		 *
		 * \return Returns an EE_RESULT containing the binary string result, or EE_NC_INVALID if unsupported.
		 */
		virtual CExpEvalContainer::EE_RESULT		Bin() const { return { EE_NC_INVALID }; }

		/**
		 * \brief Gets the boolean form of the object as a string (0b****).
		 *
		 * Base implementation returns EE_NC_INVALID.  Derived types may override to convert the object to a
		 * boolean value and format it as a binary literal string (typically "0b0" or "0b1").
		 *
		 * \return Returns an EE_RESULT containing the boolean string result, or EE_NC_INVALID if unsupported.
		 */
		virtual CExpEvalContainer::EE_RESULT		Bool() const { return { EE_NC_INVALID }; }

		/**
		 * \brief Returns the character that represents the specified Unicode.
		 *
		 * Base implementation returns EE_NC_INVALID.  Derived types may override to interpret the object as a
		 * Unicode code point and return the corresponding character.
		 *
		 * \return Returns an EE_RESULT containing the character result, or EE_NC_INVALID if unsupported.
		 */
		virtual CExpEvalContainer::EE_RESULT		Chr() const { return { EE_NC_INVALID }; }

		/**
		 * \brief Gets the string interpreted to its best fit and then converted to float.
		 *
		 * Base implementation returns EE_NC_INVALID.  Derived types may override to interpret the object as a
		 * number and return a floating-point result.
		 *
		 * \return Returns an EE_RESULT containing the floating-point value, or EE_NC_INVALID if unsupported.
		 */
		virtual CExpEvalContainer::EE_RESULT		Float() const { return { EE_NC_INVALID }; }

		/**
		 * \brief Gets the hexadecimal form of the object as a string (0x****).
		 *
		 * Base implementation returns EE_NC_INVALID.  Derived types may override to format the object as a
		 * hexadecimal literal string.
		 *
		 * \return Returns an EE_RESULT containing the hexadecimal string result, or EE_NC_INVALID if unsupported.
		 */
		virtual CExpEvalContainer::EE_RESULT		Hex() const { return { EE_NC_INVALID }; }

		/**
		 * \brief Gets the string interpreted to its best fit and then converted to int64_t.
		 *
		 * Base implementation returns EE_NC_INVALID.  Derived types may override to interpret the object as a
		 * number and return a signed 64-bit integer result.
		 *
		 * \return Returns an EE_RESULT containing the integer value, or EE_NC_INVALID if unsupported.
		 */
		virtual CExpEvalContainer::EE_RESULT		Int() const { return { EE_NC_INVALID }; }

		/**
		 * Gets the backing vector directly.
		 * 
		 * \return Returns a reference to the backing string.
		 **/
		std::vector<CExpEvalContainer::EE_RESULT> &	GetBacking() { return m_vBacking; }

		/**
		 * Gets the number of elements in the object.  For arrays, maps, sets, lists, etc., this is the number of items.  For Unicode strings, this is the number of UTF code points.
		 *
		 * \return Returns the number of items in the vector.
		 */
		virtual CExpEvalContainer::EE_RESULT		Len() const {
			CExpEvalContainer::EE_RESULT rRes = { EE_NC_UNSIGNED };
			rRes.u.ui64Val = m_vBacking.size();
			return rRes;
		}

		/**
		 * Converts the array to an array of primitives.
		 * 
		 * \param _vRet The array to fill with the converted values.
		 * \return Returns true if the array was allocated and all values were successfully cast to the target type.
		 **/
		template <typename _tVecType, EE_NUM_CONSTANTS _ncType>
		bool										ToPrimitiveArray( std::vector<_tVecType> &_vRet ) const {
			try {
				if ( !m_vBacking.size() ) {
					_vRet.clear();
					return true;
				}
				_vRet.resize( m_vBacking.size() );
				for ( auto I = m_vBacking.size(); I--; ) {
					auto rTmp = m_peecContainer->ConvertResultOrObject( m_vBacking[I], _ncType );
					if ( rTmp.ncType != _ncType ) { return false; }
					if constexpr ( _ncType == EE_NC_SIGNED ) {
						_vRet[I] = _tVecType( rTmp.u.i64Val );
					}
					else if constexpr ( _ncType == EE_NC_UNSIGNED ) {
						_vRet[I] = _tVecType( rTmp.u.ui64Val );
					}
					else if constexpr ( _ncType == EE_NC_FLOATING ) {
						_vRet[I] = _tVecType( rTmp.u.dVal );
					}
				}
			}
			catch ( ... ) { return false; }
			return true;
		}

		/**
		 * Sets the array based off a vector of primitives.
		 * 
		 * \param _vRet The array of primitives to be used to fill this vector.
		 * \return Returns true if this vector was able to size correctly.
		 **/
		template <typename _tVecType, EE_NUM_CONSTANTS _ncType>
		bool										FromPrimitiveArray( const std::vector<_tVecType> &_vRet ) {
			try {
				if ( !_vRet.size() ) {
					m_vBacking.clear();
					return true;
				}
				m_vBacking.resize( _vRet.size() );
				for ( auto I = _vRet.size(); I--; ) {
					m_vBacking[I].ncType = _ncType;
					if constexpr ( _ncType == EE_NC_SIGNED ) {
						m_vBacking[I].u.i64Val = _tVecType( _vRet[I] );
					}
					else if constexpr ( _ncType == EE_NC_UNSIGNED ) {
						m_vBacking[I].u.ui64Val = _tVecType( _vRet[I] );
					}
					else if constexpr ( _ncType == EE_NC_FLOATING ) {
						m_vBacking[I].u.dVal = _tVecType( _vRet[I] );
					}
				}
			}
			catch ( ... ) { return false; }
			return true;
		}

		/**
		 * \brief Gets the octadecimal form of the object as a string (0o****).
		 *
		 * Base implementation returns EE_NC_INVALID.  Derived types may override to format the object as an
		 * octal literal string.
		 *
		 * \return Returns an EE_RESULT containing the octal string result, or EE_NC_INVALID if unsupported.
		 */
		virtual CExpEvalContainer::EE_RESULT		Oct() const { return { EE_NC_INVALID }; }

		/**
		 * \brief Returns the ordinal value of the object as a Unicode character (always EE_NC_UNSIGNED).
		 *
		 * Base implementation returns EE_NC_INVALID.  Derived types may override to interpret the object as a
		 * character and return its ordinal (code point) value as an unsigned result.
		 *
		 * \return Returns an EE_RESULT containing the ordinal value, or EE_NC_INVALID if unsupported.
		 */
		virtual CExpEvalContainer::EE_RESULT		Ord() const { return { EE_NC_INVALID }; }

		/**
		 * \brief Append an item to the end of the vector.
		 *
		 * Appends the given value to this vector's backing store.  The value is stored as-is (EE_RESULT), and
		 * the appended value is returned.
		 *
		 * \param _rRet The value to append.
		 * \return Returns the appended value on success.
		 */
		virtual CExpEvalContainer::EE_RESULT		PushBack( CExpEvalContainer::EE_RESULT &_rRet ) {
			/*if ( _rRet.ncType == EE_NC_OBJECT && _rRet.u.poObj && (_rRet.u.poObj->Type() & CObject::EE_BIT_VECTOR) &&
				static_cast<ee::CVector *>(_rRet.u.poObj) == this ) { return { .ncType = EE_NC_INVALID }; }*/
			m_vBacking.push_back( _rRet );
			return _rRet;
		}

		/**
		 * \brief Gets the capacity.
		 *
		 * \return Returns an EE_RESULT of type EE_NC_UNSIGNED containing the current capacity.
		 */
		virtual CExpEvalContainer::EE_RESULT		Capacity() const {
			CExpEvalContainer::EE_RESULT rRet = { .ncType = EE_NC_UNSIGNED };
			rRet.u.ui64Val = m_vBacking.capacity();
			return rRet;
		}

		/**
		 * \brief Gets whether the vector is empty or not.
		 *
		 * \return Returns an EE_RESULT of type EE_NC_UNSIGNED containing 1 if empty; 0 otherwise.
		 */
		virtual CExpEvalContainer::EE_RESULT		Empty() const {
			CExpEvalContainer::EE_RESULT rRet = { .ncType = EE_NC_UNSIGNED };
			rRet.u.ui64Val = m_vBacking.empty();
			return rRet;
		}

		/**
		 * \brief Clears the array.
		 *
		 * Removes all elements from the vector.
		 *
		 * \return Returns an EE_RESULT representing this object.
		 */
		virtual CExpEvalContainer::EE_RESULT		Clear() { m_vBacking.clear(); return CreateResult(); }

		/**
		 * \brief Appends to the array.
		 *
		 * This is an alias of PushBack().
		 *
		 * \param _rRet The value to append.
		 * \return Returns the appended value on success.
		 */
		virtual CExpEvalContainer::EE_RESULT		Append( CExpEvalContainer::EE_RESULT &_rRet ) { return PushBack( _rRet ); }

		/**
		 * \brief Pops the back item and returns it.
		 *
		 * \return Returns the removed element, or EE_NC_INVALID if the vector is empty.
		 */
		virtual CExpEvalContainer::EE_RESULT		PopBack() {
			if ( !m_vBacking.size() ) { return { .ncType = EE_NC_INVALID }; }
			auto aTmp = m_vBacking[m_vBacking.size()-1];
			m_vBacking.pop_back();
			return aTmp;
		}

		/**
		 * \brief Gets a value at an index in the array.
		 *
		 * The provided index is converted using ArrayIndexToLinearIndex(), allowing support for negative or
		 * extended indexing rules as defined by the evaluator.  If the translated index is invalid, an
		 * EE_NC_INVALID result is returned.
		 *
		 * \param _sIdx The index of the element to read.
		 * \return Returns the element at the given index, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		At( size_t _sIdx ) {
			_sIdx = ArrayIndexToLinearIndex( _sIdx, m_vBacking.size() );
			CExpEvalContainer::EE_RESULT rRet = { .ncType = EE_NC_UNSIGNED };
			if ( _sIdx == EE_INVALID_IDX ) { rRet.ncType = EE_NC_INVALID; return rRet; }
			return m_vBacking.at( _sIdx );
		}

		/**
		 * \brief Assigns a value at a given index in the array.
		 *
		 * Assigns _rRet into the vector using std::vector::assign().  Note that std::vector::assign( count, value )
		 * replaces the entire contents of the vector with count copies of value.
		 *
		 * \param _sIdx The element count parameter passed to std::vector::assign().
		 * \param _rRet The value assigned into the vector.
		 * \return Returns an EE_RESULT representing this object.
		 */
		virtual CExpEvalContainer::EE_RESULT		Assign( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			/*if ( _rRet.ncType == EE_NC_OBJECT && _rRet.u.poObj && (_rRet.u.poObj->Type() & CObject::EE_BIT_VECTOR) &&
				static_cast<ee::CVector *>(_rRet.u.poObj) == this ) { return { .ncType = EE_NC_INVALID }; }*/
			m_vBacking.assign( _sIdx, _rRet );
			return CreateResult();
		}

		/**
		 * \brief Inserts a value at a given index in the array.
		 *
		 * The provided index is converted using ArrayIndexToLinearIndex().  If the translated index is invalid,
		 * EE_NC_INVALID is returned.  On success, the element is inserted and this object is returned.
		 *
		 * \param _sIdx The insertion index.
		 * \param _rRet The value to insert.
		 * \return Returns an EE_RESULT representing this object, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		Insert( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			_sIdx = ArrayIndexToLinearIndex( _sIdx, m_vBacking.size() );
			CExpEvalContainer::EE_RESULT rRet = { .ncType = EE_NC_UNSIGNED };
			if ( _sIdx == EE_INVALID_IDX ) { rRet.ncType = EE_NC_INVALID; return rRet; }

			m_vBacking.insert( m_vBacking.begin() + _sIdx, _rRet );
			return CreateResult();
		}

		/**
		 * \brief Erases a value at an index in the array.
		 *
		 * The provided index is converted using ArrayIndexToLinearIndex().  If the translated index is invalid,
		 * EE_NC_INVALID is returned.  On success, the erased element is returned.
		 *
		 * \param _sIdx The index of the element to erase.
		 * \return Returns the erased element, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		Erase( size_t _sIdx ) {
			_sIdx = ArrayIndexToLinearIndex( _sIdx, m_vBacking.size() );
			CExpEvalContainer::EE_RESULT rRet = { .ncType = EE_NC_UNSIGNED };
			if ( _sIdx == EE_INVALID_IDX ) { rRet.ncType = EE_NC_INVALID; return rRet; }

			rRet = m_vBacking.at( _sIdx );
			m_vBacking.erase( m_vBacking.begin() + _sIdx );
			return rRet;
		}

		/**
		 * \brief Gets the max size.
		 *
		 * \return Returns an EE_RESULT of type EE_NC_UNSIGNED containing the maximum size of the vector.
		 */
		virtual CExpEvalContainer::EE_RESULT		MaxSize() const {
			CExpEvalContainer::EE_RESULT rRet = { .ncType = EE_NC_UNSIGNED };
			rRet.u.ui64Val = m_vBacking.max_size();
			return rRet;
		}

		/**
		 * \brief Gets the size.
		 *
		 * \return Returns the logical element count (Len()).
		 */
		virtual CExpEvalContainer::EE_RESULT		Size() const {
			return Len();
		}

		/**
		 * \brief Reserves memory for X number of elements.
		 *
		 * \param _sTotal The minimum capacity to reserve.
		 * \return Returns an EE_RESULT representing this object.
		 */
		virtual CExpEvalContainer::EE_RESULT		Reserve( size_t _sTotal ) {
			m_vBacking.reserve( _sTotal );
			return CreateResult();
		}

		/**
		 * \brief Resize to X elements.
		 *
		 * \param _sTotal The new size of the vector in elements.
		 * \return Returns an EE_RESULT representing this object.
		 */
		virtual CExpEvalContainer::EE_RESULT		Resize( size_t _sTotal ) {
			m_vBacking.resize( _sTotal );
			return CreateResult();
		}

		/**
		 * \brief Shrinks to fit.
		 *
		 * Requests removal of unused capacity.
		 *
		 * \return Returns an EE_RESULT representing this object.
		 */
		virtual CExpEvalContainer::EE_RESULT		ShrinkToFit() {
			m_vBacking.shrink_to_fit();
			return CreateResult();
		}

		/**
		 * \brief Swaps the vector with another.
		 *
		 * Expects _rRet to be an object result whose object is a vector.  On success, this vector's backing store
		 * is swapped with the other's and this object is returned.
		 *
		 * \param _rRet The other vector object to swap with.
		 * \return Returns an EE_RESULT representing this object, or EE_NC_INVALID on type mismatch.
		 */
		virtual CExpEvalContainer::EE_RESULT		Swap( CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( _rRet.ncType != EE_NC_OBJECT ) { return { .ncType = EE_NC_INVALID }; }
			if ( !_rRet.u.poObj ) { return { .ncType = EE_NC_INVALID }; }
			if ( !(_rRet.u.poObj->Type() & CObject::EE_BIT_VECTOR) ) { return { .ncType = EE_NC_INVALID }; }
			m_vBacking.swap( static_cast<ee::CVector *>(_rRet.u.poObj)->m_vBacking );
			return CreateResult();
		}

		/**
		 * \brief Sums all elements.
		 *
		 * Computes and returns the sum of all elements in the vector using the evaluator's numeric rules.
		 *
		 * \return Returns the sum result, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		Sum();

		/**
		 * \brief Element-wise addition.
		 *
		 * Adds this vector to _pvOther element-by-element and writes the result to _pvReturn.
		 *
		 * \param _pvOther The other vector to add.
		 * \param _pvReturn Receives the element-wise sum.
		 * \return Returns an EE_RESULT representing _pvReturn, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		Add( const ee::CVector * _pvOther, ee::CVector * _pvReturn );

		/**
		 * \brief Element-wise subtraction.
		 *
		 * Subtracts _pvOther from this vector element-by-element and writes the result to _pvReturn.
		 *
		 * \param _pvOther The other vector to subtract.
		 * \param _pvReturn Receives the element-wise difference.
		 * \return Returns an EE_RESULT representing _pvReturn, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		Sub( const ee::CVector * _pvOther, ee::CVector * _pvReturn );

		/**
		 * \brief Element-wise multiplication.
		 *
		 * Multiplies this vector by _pvOther element-by-element and writes the result to _pvReturn.
		 *
		 * \param _pvOther The other vector to multiply.
		 * \param _pvReturn Receives the element-wise product.
		 * \return Returns an EE_RESULT representing _pvReturn, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		Mul( const ee::CVector * _pvOther, ee::CVector * _pvReturn );

		/**
		 * \brief Scalar multiplication.
		 *
		 * Multiplies each element of this vector by the given scalar and writes the result to _pvReturn.
		 *
		 * \param _rScalar The scalar to apply.
		 * \param _pvReturn Receives the scaled vector.
		 * \return Returns an EE_RESULT representing _pvReturn, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		Mul( const CExpEvalContainer::EE_RESULT &_rScalar, ee::CVector * _pvReturn );

		/**
		 * \brief Element-wise division.
		 *
		 * Divides this vector by _pvOther element-by-element and writes the result to _pvReturn.
		 *
		 * \param _pvOther The other vector to divide by.
		 * \param _pvReturn Receives the element-wise quotient.
		 * \return Returns an EE_RESULT representing _pvReturn, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		Div( const ee::CVector * _pvOther, ee::CVector * _pvReturn );

		/**
		 * \brief Scalar division.
		 *
		 * Divides each element of this vector by the given scalar and writes the result to _pvReturn.
		 *
		 * \param _rScalar The scalar divisor to apply.
		 * \param _pvReturn Receives the scaled vector.
		 * \return Returns an EE_RESULT representing _pvReturn, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		Div( const CExpEvalContainer::EE_RESULT &_rScalar, ee::CVector * _pvReturn );

		/**
		 * \brief Dot product.
		 *
		 * Computes the dot product of this vector and _pvOther.
		 *
		 * \param _pvOther The other vector.
		 * \return Returns the dot product result, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		Dot( const ee::CVector * _pvOther );

		/**
		 * \brief Cross product.
		 *
		 * Computes the cross product of this vector and _pvOther and writes the resulting vector to _pvReturn.
		 *
		 * \param _pvOther The other vector.
		 * \param _pvReturn Receives the cross-product vector.
		 * \return Returns an EE_RESULT representing _pvReturn, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		Cross( const ee::CVector * _pvOther, ee::CVector * _pvReturn );

		/**
		 * \brief Gets the magnitude of the vector.
		 *
		 * \return Returns the vector magnitude, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		Mag();

		/**
		 * \brief Gets the squared magnitude of the vector.
		 *
		 * \return Returns the squared magnitude, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		MagSq();

		/**
		 * \brief Return a normalized copy of this vector.
		 *
		 * Computes a normalized version of this vector and writes it to _pvReturn.
		 *
		 * \param _pvReturn Receives the normalized vector.
		 * \return Returns an EE_RESULT representing _pvReturn, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		Normalize( ee::CVector * _pvReturn );

	protected :
		// == Members.
		/** The backing vector. */
		std::vector<CExpEvalContainer::EE_RESULT>	m_vBacking;

	};

}	// namespace ee
