#pragma once

#include "EEObject.h"

#include <format>


namespace ee {

	class CString : public CObject {
		friend class								CStringBaseApi;
	public :
		CString( CExpEvalContainer * _peecContainer ) :
			CObject( _peecContainer ),
			m_ui64Len( 0 ) {
		}


		// == Functions.
		/**
		 * \brief Gets the runtime type identifier of this object.
		 *
		 * The returned value includes the base object type flags as well as the built-in string flag so the
		 * evaluator can identify this object as a string-capable type.
		 *
		 * \return Returns the object type bitmask for this instance.
		 */
		virtual uint32_t							Type() const { return CObject::Type() | CObject::EE_BIT_STRING; }

		/**
		 * \brief Sets the backing string to the given value.
		 *
		 * This replaces the entire contents of the string and invalidates any cached length/array-lookup data.
		 *
		 * \param _sString The new string value.
		 * \return Returns a reference to this object.
		 */
		CString &									SetString( const std::string &_sString ) {
			m_sObj = _sString;
			Dirty();
			return (*this);
		}

		/**
		 * \brief Gets the current string value.
		 *
		 * \return Returns a const reference to the backing string.
		 */
		const std::string &							GetString() const { return m_sObj; }

		/**
		 * \brief Converts this string object to another evaluator value type.
		 *
		 * The conversion semantics depend on _ncType and may involve parsing the string, converting to a
		 * numeric type, or producing another object instance.
		 *
		 * \param _ncType The destination type.
		 * \return Returns the converted value, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		ConvertTo( EE_NUM_CONSTANTS _ncType );

		/**
		 * \brief Initializes this object from an evaluator result.
		 *
		 * This replaces the current contents of the string using the conversion rules for the incoming
		 * value/object and invalidates cached length/optimization data.
		 *
		 * \param _rObj Source evaluator result.
		 * \return Returns true if initialization succeeded; false otherwise.
		 */
		virtual bool								InitializeFrom( const CExpEvalContainer::EE_RESULT &_rObj );

		/**
		 * \brief Creates a string representation of the object.
		 *
		 * When EE_TF_C_STRING is set, the result is formatted as a C-style quoted string literal with
		 * necessary escaping applied (via CExpEval::ToCString()).
		 *
		 * \param _sString Receives the textual representation.
		 * \param _ui32Depth Current formatting depth (unused for strings).
		 * \param _ui32Flags Formatting flags (EE_TOSTRING_FLAGS).
		 * \return Returns true.
		 */
		virtual bool								ToString( std::string &_sString, uint32_t /*_ui32Depth*/, uint32_t _ui32Flags = EE_TF_NONE ) {
			if ( _ui32Flags & CObject::EE_TF_C_STRING ) {
				_sString.reserve( m_sObj.size() + 2 );
				_sString = "\"";
				/*for ( std::string::size_type I = 0; I < m_sObj.size(); ++I ) {
					if ( m_sObj[I] == '"' || m_sObj[I] == '\\' ) {
						_sString.push_back( '\\' );
					}
					_sString.push_back( m_sObj[I] );
				}*/
				_sString += CExpEval::ToCString( m_sObj );
				_sString += "\"";
			}
			else {
				_sString = m_sObj;
			}
			return true;
		}

		/**
		 * \brief Creates a formatted string representation of the object.
		 *
		 * The format string is applied using std::vformat().  When EE_TF_C_STRING is set, the string is first
		 * converted to an escaped C-string form (without surrounding quotes) before formatting.
		 *
		 * \param _sFormat A format string compatible with std::format()/std::vformat().
		 * \param _ui32Flags Formatting flags (EE_TOSTRING_FLAGS).
		 * \return Returns the formatted representation.
		 */
		virtual std::string							FormattedString( const std::string &_sFormat, uint32_t _ui32Flags = EE_TF_NONE ) {
			if ( _ui32Flags & CObject::EE_TF_C_STRING ) {
				auto sTmp = CExpEval::ToCString( m_sObj );
				return std::vformat( _sFormat, std::make_format_args( sTmp ) );
			}
			//return std::format( _sFormat, m_sObj );
			return std::vformat( _sFormat, std::make_format_args( m_sObj ) );
		}

		/**
		 * \brief Performs array-style indexing on the string.
		 *
		 * Indexing is interpreted in terms of Unicode code points (not raw bytes).  Implementations may use
		 * cached lookup tables to accelerate repeated access.
		 *
		 * \param _i64Idx The index to access.
		 * \return Returns the accessed element as an EE_RESULT, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		ArrayAccess( int64_t _i64Idx );

		/**
		 * \brief Performs extended array-style indexing on the string.
		 *
		 * Depending on _ui32Mask (EE_ARRAY_EX_FLAGS), this may access a single element, a range/slice, or
		 * another container-specific interpretation of two indices.
		 *
		 * \param _i64Idx0 The first index value (valid if EE_AEF_START is set).
		 * \param _i64Idx1 The second index value (valid if EE_AEF_END is set).
		 * \param _ui32Mask A mask of EE_ARRAY_EX_FLAGS indicating which indices are valid.
		 * \return Returns the accessed element/range as an EE_RESULT, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		ArrayAccessEx( int64_t _i64Idx0, int64_t _i64Idx1, uint32_t _ui32Mask );

		/**
		 * \brief Gets the length of the string when treated as an array.
		 *
		 * The length is the number of Unicode code points in the UTF-8 string.
		 *
		 * \return Returns the number of code points.
		 */
		virtual size_t								ArrayLength();


		// == Operators.
		/**
		 * \brief Compares this string with another evaluator value for equality (operator==).
		 *
		 * Implementations typically compare against another string object or a value that can be converted
		 * to a string.
		 *
		 * \param _rRet Right-hand operand and/or result carrier, depending on evaluator conventions.
		 * \return Returns true if the comparison was performed; false otherwise.
		 */
		virtual bool								Equals( CExpEvalContainer::EE_RESULT &_rRet );

		/**
		 * \brief Concatenates this string with another evaluator value (operator+).
		 *
		 * \param _rRet Right-hand operand and/or result carrier, depending on evaluator conventions.
		 * \return Returns the concatenation result, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		Plus( CExpEvalContainer::EE_RESULT &_rRet );

		/**
		 * \brief Repeats this string by a multiplier (operator*).
		 *
		 * The right-hand operand is typically interpreted as an integer repeat count.
		 *
		 * \param _rRet Right-hand operand and/or result carrier, depending on evaluator conventions.
		 * \return Returns the repeated-string result, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		Multiply( CExpEvalContainer::EE_RESULT &_rRet );

		/**
		 * \brief Performs in-place concatenation (operator+=).
		 *
		 * \param _rRet Right-hand operand and/or result carrier, depending on evaluator conventions.
		 * \return Returns true if the operation succeeded; false otherwise.
		 */
		virtual bool								PlusEquals( CExpEvalContainer::EE_RESULT &_rRet );


		// == Base API functions.
		/**
		 * \brief Gets the object as an ASCII string.
		 *
		 * \return Returns an EE_RESULT representing the ASCII-converted string, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		Ascii() const;

		/**
		 * \brief Gets the binary form of the object as a string (0b****).
		 *
		 * \return Returns an EE_RESULT containing the binary representation, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		Bin() const;

		/**
		 * \brief Gets the boolean form of the object as a string (0b****).
		 *
		 * \return Returns an EE_RESULT containing the boolean representation, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		Bool() const;

		/**
		 * \brief Returns the character that represents the specified Unicode.
		 *
		 * \return Returns an EE_RESULT containing the character result, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		Chr() const;

		/**
		 * \brief Gets the string interpreted to its best fit and then converted to float.
		 *
		 * \return Returns an EE_RESULT containing the floating-point value, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		Float() const;

		/**
		 * \brief Gets the hexadecimal form of the object as a string (0x****).
		 *
		 * \return Returns an EE_RESULT containing the hex representation, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		Hex() const;

		/**
		 * \brief Gets the string interpreted to its best fit and then converted to int64_t.
		 *
		 * \return Returns an EE_RESULT containing the integer value, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		Int() const;

		/**
		 * \brief Gets the number of elements in the object.
		 *
		 * For Unicode strings, this is the number of UTF code points (not bytes).  The value is cached and
		 * recomputed only when the string changes.
		 *
		 * \return Returns an EE_RESULT of type EE_NC_UNSIGNED containing the number of code points.
		 */
		virtual CExpEvalContainer::EE_RESULT		Len() const {
			CExpEvalContainer::EE_RESULT rRes = { EE_NC_UNSIGNED };
			if ( !m_ui64Len ) { m_ui64Len = ee::CExpEval::CountUtfCodePoints( m_sObj ); }
			rRes.u.ui64Val = m_ui64Len;
			return rRes;
		}

		/**
		 * \brief Gets the octadecimal form of the object as a string (0o****).
		 *
		 * \return Returns an EE_RESULT containing the octal representation, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		Oct() const;

		/**
		 * \brief Returns the ordinal value of the object as a Unicode character.
		 *
		 * The returned value is always EE_NC_UNSIGNED.
		 *
		 * \return Returns an EE_RESULT containing the ordinal value, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		Ord() const;

		/**
		 * \brief Gets the backing string directly.
		 *
		 * \return Returns a const reference to the backing string storage.
		 */
		const std::string &							GetBacking() const { return m_sObj; }

		/**
		 * \brief Appends a value to the end of the string.
		 *
		 * If _rRet is a string object, its contents are appended.  Otherwise, _rRet is converted to an unsigned
		 * integer and appended as a single byte/character.  The object is marked dirty afterward.
		 *
		 * \param _rRet The value to append.
		 * \return Returns an EE_RESULT referencing this object, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		PushBack( CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( _rRet.ncType == EE_NC_OBJECT ) {
				if ( !_rRet.u.poObj ) { return { .ncType = EE_NC_INVALID }; };
				if ( !(_rRet.u.poObj->Type() & CObject::EE_BIT_STRING ) ) { return { .ncType = EE_NC_INVALID }; };
				m_sObj.append( static_cast<ee::CString *>(_rRet.u.poObj)->m_sObj );
			}
			else {
				CExpEvalContainer::EE_RESULT rIdx = m_peecContainer->ConvertResultOrObject( _rRet, EE_NC_UNSIGNED );
				if ( rIdx.ncType == EE_NC_INVALID ) { return { .ncType = EE_NC_INVALID }; }
				m_sObj.push_back( static_cast<std::string::value_type>(rIdx.u.ui64Val) );
			}
			Dirty();
			return CreateResult();
		}

		/**
		 * \brief Removes and returns the last element of the string.
		 *
		 * \return Returns the removed element as an EE_RESULT, or EE_NC_INVALID if the string is empty or on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		PopBack();

		/**
		 * \brief Gets the capacity of the backing string in bytes.
		 *
		 * \return Returns an EE_RESULT of type EE_NC_UNSIGNED containing the capacity.
		 */
		virtual CExpEvalContainer::EE_RESULT		Capacity() const {
			CExpEvalContainer::EE_RESULT rRet = { .ncType = EE_NC_UNSIGNED };
			rRet.u.ui64Val = m_sObj.capacity();
			return rRet;
		}

		/**
		 * \brief Gets whether the string is empty.
		 *
		 * \return Returns an EE_RESULT of type EE_NC_UNSIGNED containing 1 if empty; otherwise 0.
		 */
		virtual CExpEvalContainer::EE_RESULT		Empty() const {
			CExpEvalContainer::EE_RESULT rRet = { .ncType = EE_NC_UNSIGNED };
			rRet.u.ui64Val = m_sObj.empty();
			return rRet;
		}

		/**
		 * \brief Clears the string contents.
		 *
		 * Cached length/optimization data is invalidated.
		 *
		 * \return Returns an EE_RESULT referencing this object.
		 */
		virtual CExpEvalContainer::EE_RESULT		Clear() { m_sObj.clear(); Dirty(); return CreateResult(); }

		/**
		 * \brief Appends to the string.
		 *
		 * This is an alias of PushBack().
		 *
		 * \param _rRet The value to append.
		 * \return Returns an EE_RESULT referencing this object, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		Append( CExpEvalContainer::EE_RESULT &_rRet ) { return PushBack( _rRet ); }

		/**
		 * \brief Gets a value in the string at the given index.
		 *
		 * Indexing is interpreted in terms of Unicode code points (not raw bytes).
		 *
		 * \param _sIdx The index to access.
		 * \return Returns the accessed value as an EE_RESULT, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		At( size_t _sIdx );

		/**
		 * \brief Inserts a value at a given index in the string.
		 *
		 * Indexing is interpreted in terms of Unicode code points (not raw bytes).
		 *
		 * \param _sIdx The insertion index.
		 * \param _rRet The value to insert.
		 * \return Returns an EE_RESULT representing the inserted/affected value, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		Insert( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet );

		/**
		 * \brief Erases a value at an index in the string.
		 *
		 * The index is interpreted as a code-point index.  The removed value is returned as EE_NC_UNSIGNED.
		 *
		 * \param _sIdx The code-point index to erase.
		 * \return Returns the removed code point as EE_NC_UNSIGNED, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		Erase( size_t _sIdx ) {
			_sIdx = ArrayIndexToLinearIndex( _sIdx, static_cast<size_t>(Len().u.ui64Val) );
			CExpEvalContainer::EE_RESULT rRet = { .ncType = EE_NC_UNSIGNED };
			if ( _sIdx == EE_INVALID_IDX ) { rRet.ncType = EE_NC_INVALID; return rRet; }
			size_t sSize = 0;
			size_t I = 0;
			size_t sEraseStart = 0;
			size_t sPos = 0;
			for ( ; I <= _sIdx; ++I, sPos += sSize ) {
				if ( sPos >= m_sObj.size() ) { rRet.ncType = EE_NC_INVALID; return rRet; }
				rRet.u.ui64Val = ee::CExpEval::NextUtf8Char( &m_sObj[sPos], m_sObj.size() - sPos, &sSize );
				if ( !sSize ) { rRet.ncType = EE_NC_INVALID; return rRet; }
				sEraseStart = sPos;
			}
			while ( sSize-- ) {
				m_sObj.erase( m_sObj.begin() + sEraseStart );
			}
			Dirty();
			return rRet;
		}

		/**
		 * \brief Gets the maximum size of the backing string in bytes.
		 *
		 * \return Returns an EE_RESULT of type EE_NC_UNSIGNED containing the maximum size.
		 */
		virtual CExpEvalContainer::EE_RESULT		MaxSize() const {
			CExpEvalContainer::EE_RESULT rRet = { .ncType = EE_NC_UNSIGNED };
			rRet.u.ui64Val = m_sObj.max_size();
			return rRet;
		}

		/**
		 * \brief Gets the size of the string in Unicode code points.
		 *
		 * This is equivalent to Len().
		 *
		 * \return Returns an EE_RESULT of type EE_NC_UNSIGNED containing the number of code points.
		 */
		virtual CExpEvalContainer::EE_RESULT		Size() const {
			return Len();
		}

		/**
		 * \brief Reserves storage for the given number of bytes/characters.
		 *
		 * \param _sTotal The capacity to reserve.
		 * \return Returns an EE_RESULT referencing this object.
		 */
		virtual CExpEvalContainer::EE_RESULT		Reserve( size_t _sTotal ) {
			m_sObj.reserve( _sTotal );
			return CreateResult();
		}

		/**
		 * \brief Resizes the backing string to the given number of bytes/characters.
		 *
		 * \param _sTotal The new size.
		 * \return Returns an EE_RESULT referencing this object.
		 */
		virtual CExpEvalContainer::EE_RESULT		Resize( size_t _sTotal ) {
			m_sObj.resize( _sTotal );
			return CreateResult();
		}

		/**
		 * \brief Shrinks the backing string capacity to fit its current size.
		 *
		 * \return Returns an EE_RESULT referencing this object.
		 */
		virtual CExpEvalContainer::EE_RESULT		ShrinkToFit() {
			m_sObj.shrink_to_fit();
			return CreateResult();
		}

		/**
		 * \brief Swaps the contents of this string with another string object.
		 *
		 * The right-hand operand must be an object with the EE_BIT_STRING flag set.  Length-cache and
		 * array-optimizer state is swapped as well.
		 *
		 * \param _rRet The other object to swap with.
		 * \return Returns an EE_RESULT referencing this object, or EE_NC_INVALID on failure.
		 */
		virtual CExpEvalContainer::EE_RESULT		Swap( CExpEvalContainer::EE_RESULT &_rRet ) {
			if ( _rRet.ncType != EE_NC_OBJECT ) { return { .ncType = EE_NC_INVALID }; }
			if ( !_rRet.u.poObj ) { return { .ncType = EE_NC_INVALID }; }
			if ( !(_rRet.u.poObj->Type() & CObject::EE_BIT_STRING) ) { return { .ncType = EE_NC_INVALID }; }
			m_sObj.swap( static_cast<ee::CString *>(_rRet.u.poObj)->m_sObj );
			std::swap( m_ui64Len, static_cast<ee::CString *>(_rRet.u.poObj)->m_ui64Len );
			m_vArrayOpt.swap( static_cast<ee::CString *>(_rRet.u.poObj)->m_vArrayOpt );
			/*Dirty();
			static_cast<ee::CString *>(_rRet.u.poObj)->Dirty();*/
			return CreateResult();
		}


	protected :
		// == Members.
		std::string									m_sObj;							/**< The backing UTF-8 string that this object manipulates. */
		
		/**
		 * The cached result of Len().
		 *
		 * This is reset by Dirty() when the string contents change.
		 */
		mutable uint64_t							m_ui64Len;
		
		/**
		 * Array-access optimizer.
		 *
		 * Implementations may cache byte offsets for code-point indices to accelerate repeated indexing.
		 */
		mutable std::vector<size_t>					m_vArrayOpt;


		// == Functions.
		/**
		 * \brief Resets the object to its default state.
		 *
		 * This clears the backing string and resets cached/optimization state.
		 */
		void										Reset();

		/**
		 * \brief Invalidates cached length and indexing optimization state.
		 *
		 * Call this after any modification to the backing string.
		 */
		void										Dirty() const;
	};

}	// namespace ee
