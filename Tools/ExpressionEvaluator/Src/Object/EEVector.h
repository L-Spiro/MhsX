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
		// Gets the object as an ASCII string.
		virtual CExpEvalContainer::EE_RESULT		Ascii() const { return { EE_NC_INVALID }; }

		// Gets the binary form of the object as a string (0b****).
		virtual CExpEvalContainer::EE_RESULT		Bin() const { return { EE_NC_INVALID }; }

		// Gets the boolean form of the object as a string (0b****).
		virtual CExpEvalContainer::EE_RESULT		Bool() const { return { EE_NC_INVALID }; }

		// Returns the character that represents the specified Unicode. 
		virtual CExpEvalContainer::EE_RESULT		Chr() const { return { EE_NC_INVALID }; }

		// Gets the string interpreted to its best fit and then converted to float. 
		virtual CExpEvalContainer::EE_RESULT		Float() const { return { EE_NC_INVALID }; }

		// Gets the hexadecimal form of the object as a string (0x****).
		virtual CExpEvalContainer::EE_RESULT		Hex() const { return { EE_NC_INVALID }; }

		// Gets the string interpreted to its best fit and then converted to int64_t. 
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

		// Gets the octadecimal form of the object as a string (0o****).
		virtual CExpEvalContainer::EE_RESULT		Oct() const { return { EE_NC_INVALID }; }

		// Returns the ordinal value of the object as a Unicode character (always EE_NC_UNSIGNED).
		virtual CExpEvalContainer::EE_RESULT		Ord() const { return { EE_NC_INVALID }; }

		// Append an item to the end of the vector.
		virtual CExpEvalContainer::EE_RESULT		PushBack( CExpEvalContainer::EE_RESULT &_rRet ) {
			/*if ( _rRet.ncType == EE_NC_OBJECT && _rRet.u.poObj && (_rRet.u.poObj->Type() & CObject::EE_BIT_VECTOR) &&
				static_cast<ee::CVector *>(_rRet.u.poObj) == this ) { return { .ncType = EE_NC_INVALID }; }*/
			m_vBacking.push_back( _rRet );
			return _rRet;
		}

		// Gets the capacity.
		virtual CExpEvalContainer::EE_RESULT		Capacity() const {
			CExpEvalContainer::EE_RESULT rRet = { .ncType = EE_NC_UNSIGNED };
			rRet.u.ui64Val = m_vBacking.capacity();
			return rRet;
		}

		// Gets whether the vector is empty or not.
		virtual CExpEvalContainer::EE_RESULT		Empty() const {
			CExpEvalContainer::EE_RESULT rRet = { .ncType = EE_NC_UNSIGNED };
			rRet.u.ui64Val = m_vBacking.empty();
			return rRet;
		}

		// Clears the array.
		virtual CExpEvalContainer::EE_RESULT		Clear() { m_vBacking.clear(); return CreateResult(); }

		// Appends to the array.
		virtual CExpEvalContainer::EE_RESULT		Append( CExpEvalContainer::EE_RESULT &_rRet ) { return PushBack( _rRet ); }

		// Pops the back item and returns it.
		virtual CExpEvalContainer::EE_RESULT		PopBack() {
			if ( !m_vBacking.size() ) { return { .ncType = EE_NC_INVALID }; }
			auto aTmp = m_vBacking[m_vBacking.size()-1];
			m_vBacking.pop_back();
			return aTmp;
		}

		// Gets a value at an index in the array.
		virtual CExpEvalContainer::EE_RESULT		At( size_t _sIdx ) {
			_sIdx = ArrayIndexToLinearIndex( _sIdx, m_vBacking.size() );
			CExpEvalContainer::EE_RESULT rRet = { .ncType = EE_NC_UNSIGNED };
			if ( _sIdx == EE_INVALID_IDX ) { rRet.ncType = EE_NC_INVALID; return rRet; }
			return m_vBacking.at( _sIdx );
		}

		// Assigns a value at a given index in the array.
		virtual CExpEvalContainer::EE_RESULT		Assign( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			/*if ( _rRet.ncType == EE_NC_OBJECT && _rRet.u.poObj && (_rRet.u.poObj->Type() & CObject::EE_BIT_VECTOR) &&
				static_cast<ee::CVector *>(_rRet.u.poObj) == this ) { return { .ncType = EE_NC_INVALID }; }*/
			m_vBacking.assign( _sIdx, _rRet );
			return CreateResult();
		}

		// Inserts a value at a given index in the array.
		virtual CExpEvalContainer::EE_RESULT		Insert( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
			_sIdx = ArrayIndexToLinearIndex( _sIdx, m_vBacking.size() );
			CExpEvalContainer::EE_RESULT rRet = { .ncType = EE_NC_UNSIGNED };
			if ( _sIdx == EE_INVALID_IDX ) { rRet.ncType = EE_NC_INVALID; return rRet; }

			m_vBacking.insert( m_vBacking.begin() + _sIdx, _rRet );
			return CreateResult();
		}

		// Erases a value at an index in the array.
		virtual CExpEvalContainer::EE_RESULT		Erase( size_t _sIdx ) {
			_sIdx = ArrayIndexToLinearIndex( _sIdx, m_vBacking.size() );
			CExpEvalContainer::EE_RESULT rRet = { .ncType = EE_NC_UNSIGNED };
			if ( _sIdx == EE_INVALID_IDX ) { rRet.ncType = EE_NC_INVALID; return rRet; }

			rRet = m_vBacking.at( _sIdx );
			m_vBacking.erase( m_vBacking.begin() + _sIdx );
			return rRet;
		}

		// Gets the max size.
		virtual CExpEvalContainer::EE_RESULT		MaxSize() const {
			CExpEvalContainer::EE_RESULT rRet = { .ncType = EE_NC_UNSIGNED };
			rRet.u.ui64Val = m_vBacking.max_size();
			return rRet;
		}

		// Gets the size.
		virtual CExpEvalContainer::EE_RESULT		Size() const {
			return Len();
		}

		// Reserves memory for X number of elements.
		virtual CExpEvalContainer::EE_RESULT		Reserve( size_t _sTotal ) {
			m_vBacking.reserve( _sTotal );
			return CreateResult();
		}

		// Resize to X elements.
		virtual CExpEvalContainer::EE_RESULT		Resize( size_t _sTotal ) {
			m_vBacking.resize( _sTotal );
			return CreateResult();
		}

		// Shrinks to fit.
		virtual CExpEvalContainer::EE_RESULT		ShrinkToFit() {
			m_vBacking.shrink_to_fit();
			return CreateResult();
		}

		// Swaps the vector with another.
		virtual CExpEvalContainer::EE_RESULT		Swap( CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( _rRet.ncType != EE_NC_OBJECT ) { return { .ncType = EE_NC_INVALID }; }
			if ( !_rRet.u.poObj ) { return { .ncType = EE_NC_INVALID }; }
			if ( !(_rRet.u.poObj->Type() & CObject::EE_BIT_VECTOR) ) { return { .ncType = EE_NC_INVALID }; }
			m_vBacking.swap( static_cast<ee::CVector *>(_rRet.u.poObj)->m_vBacking );
			return CreateResult();
		}

		// Sums all elements.
		virtual CExpEvalContainer::EE_RESULT		Sum();

		// Element-wise addition.
		virtual CExpEvalContainer::EE_RESULT		Add( const ee::CVector * _pvOther, ee::CVector * _pvReturn );

		// Element-wise subtraction.
		virtual CExpEvalContainer::EE_RESULT		Sub( const ee::CVector * _pvOther, ee::CVector * _pvReturn );

		// Element-wise multiplication.
		virtual CExpEvalContainer::EE_RESULT		Mul( const ee::CVector * _pvOther, ee::CVector * _pvReturn );

		// Scalar multiplication.
		virtual CExpEvalContainer::EE_RESULT		Mul( const CExpEvalContainer::EE_RESULT &_rScalar, ee::CVector * _pvReturn );

		// Element-wise division.
		virtual CExpEvalContainer::EE_RESULT		Div( const ee::CVector * _pvOther, ee::CVector * _pvReturn );

		// Scalar division.
		virtual CExpEvalContainer::EE_RESULT		Div( const CExpEvalContainer::EE_RESULT &_rScalar, ee::CVector * _pvReturn );

		// Dot product.
		virtual CExpEvalContainer::EE_RESULT		Dot( const ee::CVector * _pvOther );

		// Cross product.
		virtual CExpEvalContainer::EE_RESULT		Cross( const ee::CVector * _pvOther, ee::CVector * _pvReturn );

		// Gets the magnitude of the vector.
		virtual CExpEvalContainer::EE_RESULT		Mag();

		// Gets the squared magnitude of the vector.
		virtual CExpEvalContainer::EE_RESULT		MagSq();

		// Return a normalized copy of this vector.
		virtual CExpEvalContainer::EE_RESULT		Normalize( ee::CVector * _pvReturn );

	protected :
		// == Members.
		/** The backing vector. */
		std::vector<CExpEvalContainer::EE_RESULT>	m_vBacking;

	};

}	// namespace ee
