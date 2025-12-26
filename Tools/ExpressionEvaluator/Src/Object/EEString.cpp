#include "EEString.h"
#include "EEStringBaseApi.h"


namespace ee {

	// == Functions.
	/**
	 * \brief Converts this string object to another evaluator value type.
	 *
	 * The conversion semantics depend on _ncType and may involve parsing the string, converting to a
	 * numeric type, or producing another object instance.
	 *
	 * \param _ncType The destination type.
	 * \return Returns the converted value, or EE_NC_INVALID on failure.
	 */
	CExpEvalContainer::EE_RESULT CString::ConvertTo( EE_NUM_CONSTANTS _ncType ) {
		CExpEvalContainer::EE_RESULT rRet;
		switch ( _ncType ) {
			case EE_NC_UNSIGNED : {
				bool bOverflowed = false;
				size_t sEaten = 0;
				rRet.u.ui64Val = ee::CExpEval::StoULL( m_sObj.c_str(), 10, &sEaten, UINT64_MAX, &bOverflowed );
				if ( !bOverflowed && sEaten ) {
					rRet.ncType = _ncType;
					return rRet;
				}
				break;
			}
			case EE_NC_SIGNED : {
				bool bOverflowed = false;
				size_t sEaten = 0;
				rRet.u.ui64Val = ee::CExpEval::StoULL( m_sObj.c_str(), 10, &sEaten, UINT64_MAX / 2ULL + 1ULL, &bOverflowed );
				if ( !bOverflowed && sEaten ) {
					rRet.ncType = _ncType;
					return rRet;
				}
				break;
			}
			case EE_NC_FLOATING : {
				bool bErrored = false;
				size_t sEaten = 0;
				rRet.u.dVal = ee::CExpEval::AtoF( m_sObj.c_str(), &sEaten, &bErrored );
				if ( !bErrored && sEaten ) {
					rRet.ncType = _ncType;
					return rRet;
				}
				break;
			}
			default : {}
		}
		rRet.ncType = EE_NC_INVALID;
		rRet.u.poObj = nullptr;
		return rRet;
	}

	/**
	 * \brief Initializes this object from an evaluator result.
	 *
	 * This replaces the current contents of the string using the conversion rules for the incoming
	 * value/object and invalidates cached length/optimization data.
	 *
	 * \param _rObj Source evaluator result.
	 * \return Returns true if initialization succeeded; false otherwise.
	 */
	bool CString::InitializeFrom( const CExpEvalContainer::EE_RESULT &_rObj ) {
		switch ( _rObj.ncType ) {
			case EE_NC_UNSIGNED : {
				try {
					m_sObj = std::to_string( _rObj.u.ui64Val );
				}
				catch ( const std::bad_alloc & ) {
					return false;
				}
				return true;
			}
			case EE_NC_SIGNED : {
				try {
					m_sObj = std::to_string( _rObj.u.i64Val );
				}
				catch ( const std::bad_alloc & ) {
					return false;
				}
				return true;
			}
			case EE_NC_FLOATING : {
				try {
					m_sObj = std::to_string( _rObj.u.dVal );
				}
				catch ( const std::bad_alloc & ) {
					return false;
				}
				return true;
			}
			case EE_NC_OBJECT : {
				if ( _rObj.u.poObj ) {
					 return _rObj.u.poObj->ToString( m_sObj, 0 );
				}
			}
			default : {}
		}
		return false;
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
	CExpEvalContainer::EE_RESULT CString::ArrayAccess( int64_t _i64Idx ) {
		CExpEvalContainer::EE_RESULT rRet = { EE_NC_UNSIGNED };
		
		size_t sIdx;
		if ( _i64Idx < 0 ) {
			if ( !m_ui64Len ) { m_ui64Len = ee::CExpEval::CountUtfCodePoints( m_sObj ); }
			if ( m_ui64Len != static_cast<size_t>(m_ui64Len) ) { rRet.u.ui64Val = EE_UTF_INVALID; return rRet; }

			sIdx = ArrayIndexToLinearIndex( _i64Idx, static_cast<size_t>(m_ui64Len) );
		}
		else {
			sIdx = static_cast<size_t>(_i64Idx);
			if ( static_cast<int64_t>(sIdx) != _i64Idx ) { rRet.u.ui64Val = EE_UTF_INVALID; return rRet; }
		}
		if ( sIdx == size_t( -1 ) ) { rRet.u.ui64Val = EE_UTF_INVALID; return rRet; }
		uint32_t ui32Val;
		CStringBaseApi::UpdateArrayAccessOptimizer( m_sObj, m_vArrayOpt, sIdx, ui32Val );
		rRet.u.ui64Val = ui32Val;
		return rRet;
	}

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
	CExpEvalContainer::EE_RESULT CString::ArrayAccessEx( int64_t _i64Idx0, int64_t _i64Idx1, uint32_t _ui32Mask ) {
		CExpEvalContainer::EE_RESULT rRet = { EE_NC_INVALID };
		
		size_t sIdx0 = 0;
		if ( _ui32Mask & EE_AEF_START ) {
			if ( _i64Idx0 < 0 ) {
				if ( !m_ui64Len ) { m_ui64Len = ee::CExpEval::CountUtfCodePoints( m_sObj ); }
				if ( m_ui64Len != static_cast<size_t>(m_ui64Len) ) { rRet.u.ui64Val = EE_UTF_INVALID; return rRet; }

				sIdx0 = ArrayIndexToLinearIndex( _i64Idx0, static_cast<size_t>(m_ui64Len) );
			}
			else {
				sIdx0 = static_cast<size_t>(_i64Idx0);
				if ( static_cast<int64_t>(sIdx0) != _i64Idx0 ) { rRet.u.ui64Val = EE_UTF_INVALID; return rRet; }
			}
			if ( sIdx0 == size_t( -1 ) ) { rRet.u.ui64Val = EE_UTF_INVALID; return rRet; }
			sIdx0 = ee::CExpEval::GetUtf8CharPosByIdx( m_sObj, sIdx0 );
		}

		size_t sIdx1;
		if ( _ui32Mask & EE_AEF_END ) {
			if ( _i64Idx1 < 0 ) {
				if ( !m_ui64Len ) { m_ui64Len = ee::CExpEval::CountUtfCodePoints( m_sObj ); }
				if ( m_ui64Len != static_cast<size_t>(m_ui64Len) ) { rRet.u.ui64Val = EE_UTF_INVALID; return rRet; }

				sIdx1 = ArrayIndexToLinearIndex( _i64Idx1, static_cast<size_t>(m_ui64Len) );
			}
			else {
				sIdx1 = static_cast<size_t>(_i64Idx1);
				if ( static_cast<int64_t>(sIdx1) != _i64Idx1 ) { rRet.u.ui64Val = EE_UTF_INVALID; return rRet; }
			}
			if ( sIdx1 == size_t( -1 ) ) { rRet.u.ui64Val = EE_UTF_INVALID; return rRet; }
			sIdx1 = ee::CExpEval::GetUtf8CharPosByIdx( m_sObj, sIdx1 );
		}
		else {
			sIdx1 = m_sObj.size();
		}
		CString * psObj = reinterpret_cast<CString *>(m_peecContainer->AllocateObject<CString>());
		if ( psObj ) {
			try {
				for ( size_t I = sIdx0; I < sIdx1; ++I ) {
					psObj->m_sObj.push_back( m_sObj[I] );
				}
				psObj->Dirty();
				rRet = psObj->CreateResult();
			}
			catch ( ... ) {
				m_peecContainer->DeallocateObject( psObj );
			}
		}
		return rRet;
	}

	/**
	 * \brief Gets the length of the string when treated as an array.
	 *
	 * The length is the number of Unicode code points in the UTF-8 string.
	 *
	 * \return Returns the number of code points.
	 */
	size_t CString::ArrayLength() {
		if ( !m_ui64Len ) { m_ui64Len = ee::CExpEval::CountUtfCodePoints( m_sObj ); }
		if ( m_ui64Len != size_t( m_ui64Len ) ) { return size_t( ~0 ); }
		return size_t( m_ui64Len );
	}

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
	bool CString::Equals( CExpEvalContainer::EE_RESULT &_rRet ) {
		if ( _rRet.ncType == EE_NC_OBJECT ) {
			if ( _rRet.u.poObj && (_rRet.u.poObj->Type() & (EE_BIT_STRING)) ) {
				return std::strcmp( m_sObj.c_str(), static_cast<CString *>(_rRet.u.poObj)->m_sObj.c_str() ) == 0;
			}
			/*else if ( _rRet.u.poObj && (_rRet.u.poObj->Type() & (EE_BIT_STRING_REF)) ) {
				std::string sTmp;
				if ( !_rRet.u.poObj->ToString( sTmp ) ) { return false; }
				return std::strcmp( m_sObj.c_str(), static_cast<CStringRef *>(_rRet.u.poObj)->StringRef().c_str() ) == 0;
			}*/
		}
		return false;
	}

	/**
	 * \brief Concatenates this string with another evaluator value (operator+).
	 *
	 * \param _rRet Right-hand operand and/or result carrier, depending on evaluator conventions.
	 * \return Returns the concatenation result, or EE_NC_INVALID on failure.
	 */
	CExpEvalContainer::EE_RESULT CString::Plus( CExpEvalContainer::EE_RESULT &_rRet ) {
		if ( _rRet.ncType == EE_NC_OBJECT ) {
			if ( _rRet.u.poObj && (_rRet.u.poObj->Type() & (EE_BIT_STRING)) ) {
				CExpEvalContainer::EE_RESULT rRet = { EE_NC_INVALID };
				
				CString * psObj = reinterpret_cast<CString *>(m_peecContainer->AllocateObject<CString>());
				if ( psObj ) {
					try {
						psObj->m_sObj = m_sObj + static_cast<CString *>(_rRet.u.poObj)->m_sObj;
						psObj->Dirty();
						rRet = psObj->CreateResult();
					}
					catch ( ... ) {
						m_peecContainer->DeallocateObject( psObj );
					}
				}

				return rRet;
			}
		}
		return { EE_NC_INVALID };
	}

	/**
	 * \brief Repeats this string by a multiplier (operator*).
	 *
	 * The right-hand operand is typically interpreted as an integer repeat count.
	 *
	 * \param _rRet Right-hand operand and/or result carrier, depending on evaluator conventions.
	 * \return Returns the repeated-string result, or EE_NC_INVALID on failure.
	 */
	CExpEvalContainer::EE_RESULT CString::Multiply( CExpEvalContainer::EE_RESULT &_rRet ) {
		if ( (_rRet.ncType == EE_NC_SIGNED && _rRet.u.i64Val >= 0) || _rRet.ncType == EE_NC_UNSIGNED ) {
			CExpEvalContainer::EE_RESULT rRet = { EE_NC_INVALID };
			CString * psObj = reinterpret_cast<CString *>(m_peecContainer->AllocateObject<CString>());
			if ( psObj ) {
				try {
					psObj->m_sObj.reserve( size_t( m_sObj.size() * _rRet.u.ui64Val ) );
					for ( uint64_t I = 0; I < _rRet.u.ui64Val; ++I ) {
						psObj->m_sObj += m_sObj;
					}
					psObj->Dirty();
					rRet = psObj->CreateResult();
				}
				catch ( ... ) {
					m_peecContainer->DeallocateObject( psObj );
				}
			}			
			return rRet;
		}
		return { EE_NC_INVALID };
	}

	/**
	 * \brief Performs in-place concatenation (operator+=).
	 *
	 * \param _rRet Right-hand operand and/or result carrier, depending on evaluator conventions.
	 * \return Returns true if the operation succeeded; false otherwise.
	 */
	bool CString::PlusEquals( CExpEvalContainer::EE_RESULT &_rRet ) {
		switch ( _rRet.ncType ) {
			case EE_NC_SIGNED : {}
			case EE_NC_UNSIGNED : {
				uint64_t ui64Val = _rRet.u.ui64Val;
				for ( auto I = 0; I < sizeof( _rRet.u.ui64Val ) / sizeof( char32_t ); ++I ) {
					uint32_t ui32Len;
					uint32_t ui32BackToUtf8 = ee::CExpEval::Utf32ToUtf8( static_cast<uint32_t>(ui64Val), ui32Len );
					if ( !ui32Len ) {
						m_sObj += std::to_string( ui64Val );
						break;
					}
					else {
						for ( uint32_t J = 0; J < ui32Len; ++J ) {
							m_sObj.push_back( static_cast<uint8_t>(ui32BackToUtf8) );
							ui32BackToUtf8 >>= 8;
						}
					}
					ui64Val >>= 32ULL;
				}
				Dirty();
				_rRet = CreateResult();
				return true;
			}
			//case EE_NC_SIGNED : {
			//	//m_sObj += std::to_string( _rRet.u.i64Val );
			//	uint64_t ui64Val = _rRet.u.ui64Val;
			//	for ( auto I = 0; I < sizeof( _rRet.u.ui64Val ) / sizeof( char32_t ); ++I ) {
			//		uint32_t ui32Len;
			//		uint32_t ui32BackToUtf8 = Utf32ToUtf8( static_cast<uint32_t>(ui64Val), ui32Len );
			//		if ( !ui32Len ) {
			//			m_sObj += std::to_string( ui64Val );
			//			break;
			//		}
			//		else {
			//			for ( uint32_t J = 0; J < ui32Len; ++J ) {
			//				m_sObj.push_back( static_cast<uint8_t>(ui32BackToUtf8) );
			//				ui32BackToUtf8 >>= 8;
			//			}
			//		}
			//		ui64Val >>= 32ULL;
			//	}
			//	Dirty();
			//	_rRet = CreateResult();
			//	return true;
			//}
			case EE_NC_FLOATING : {
				m_sObj += std::to_string( _rRet.u.dVal );
				Dirty();
				_rRet = CreateResult();
				return true;
			}
			case EE_NC_OBJECT : {
				if ( _rRet.u.poObj && (_rRet.u.poObj->Type() & EE_BIT_STRING) ) {
					std::string sTmp;
					if ( !_rRet.u.poObj->ToString( sTmp, 0 ) ) { return false; }
					m_sObj += sTmp;
					Dirty();
					_rRet = CreateResult();
					return true;
				}
				return false;
			}
			default : {}
		}
		return false;
	}

	// == Base API functions.
	/**
	 * \brief Gets the object as an ASCII string.
	 *
	 * \return Returns an EE_RESULT representing the ASCII-converted string, or EE_NC_INVALID on failure.
	 */
	CExpEvalContainer::EE_RESULT CString::Ascii() const {
		return CStringBaseApi::Ascii( m_sObj, m_peecContainer );
	}

	/**
	 * \brief Gets the binary form of the object as a string (0b****).
	 *
	 * \return Returns an EE_RESULT containing the binary representation, or EE_NC_INVALID on failure.
	 */
	CExpEvalContainer::EE_RESULT CString::Bin() const {
		return CStringBaseApi::Bin( m_sObj, m_peecContainer );
	}

	/**
	 * \brief Gets the boolean form of the object as a string (0b****).
	 *
	 * \return Returns an EE_RESULT containing the boolean representation, or EE_NC_INVALID on failure.
	 */
	CExpEvalContainer::EE_RESULT CString::Bool() const {
		return CStringBaseApi::Bool( m_sObj, m_peecContainer );
	}

	/**
	 * \brief Returns the character that represents the specified Unicode.
	 *
	 * \return Returns an EE_RESULT containing the character result, or EE_NC_INVALID on failure.
	 */
	CExpEvalContainer::EE_RESULT CString::Chr() const {
		return CStringBaseApi::Chr( m_sObj, m_peecContainer );
	}

	/**
	 * \brief Gets the string interpreted to its best fit and then converted to float.
	 *
	 * \return Returns an EE_RESULT containing the floating-point value, or EE_NC_INVALID on failure.
	 */
	CExpEvalContainer::EE_RESULT CString::Float() const {
		return CStringBaseApi::Float( m_sObj, m_peecContainer );
	}

	/**
	 * \brief Gets the hexadecimal form of the object as a string (0x****).
	 *
	 * \return Returns an EE_RESULT containing the hex representation, or EE_NC_INVALID on failure.
	 */
	CExpEvalContainer::EE_RESULT CString::Hex() const {
		return CStringBaseApi::Hex( m_sObj, m_peecContainer );
	}

	/**
	 * \brief Gets the string interpreted to its best fit and then converted to int64_t.
	 *
	 * \return Returns an EE_RESULT containing the integer value, or EE_NC_INVALID on failure.
	 */
	CExpEvalContainer::EE_RESULT CString::Int() const {
		return CStringBaseApi::Int( m_sObj, m_peecContainer );
	}

	/**
	 * \brief Gets the octadecimal form of the object as a string (0o****).
	 *
	 * \return Returns an EE_RESULT containing the octal representation, or EE_NC_INVALID on failure.
	 */
	CExpEvalContainer::EE_RESULT CString::Oct() const {
		return CStringBaseApi::Oct( m_sObj, m_peecContainer );
	}

	/**
	 * \brief Returns the ordinal value of the object as a Unicode character.
	 *
	 * The returned value is always EE_NC_UNSIGNED.
	 *
	 * \return Returns an EE_RESULT containing the ordinal value, or EE_NC_INVALID on failure.
	 */
	CExpEvalContainer::EE_RESULT CString::Ord() const {
		return CStringBaseApi::Ord( m_sObj, m_peecContainer );
	}

	/**
	 * \brief Removes and returns the last element of the string.
	 *
	 * \return Returns the removed element as an EE_RESULT, or EE_NC_INVALID if the string is empty or on failure.
	 */
	CExpEvalContainer::EE_RESULT CString::PopBack() {
		auto aLen = Len();
		if ( !aLen.u.ui64Val ) { return CreateResult(); }
		--aLen.u.ui64Val;
		
		if ( m_vArrayOpt.size() <= aLen.u.ui64Val ) {
			uint32_t ui32Val;
			CStringBaseApi::UpdateArrayAccessOptimizer( m_sObj, m_vArrayOpt, size_t( aLen.u.ui64Val ), ui32Val );
		}
		while ( m_sObj.size() > m_vArrayOpt[size_t( aLen.u.ui64Val )] ) {
			m_sObj.pop_back();
		}
		//Dirty();
		m_ui64Len = aLen.u.ui64Val;
		m_vArrayOpt.pop_back();
		return CreateResult();
	}

	/**
	 * \brief Gets a value in the string at the given index.
	 *
	 * Indexing is interpreted in terms of Unicode code points (not raw bytes).
	 *
	 * \param _sIdx The index to access.
	 * \return Returns the accessed value as an EE_RESULT, or EE_NC_INVALID on failure.
	 */
	CExpEvalContainer::EE_RESULT CString::At( size_t _sIdx ) {
		CExpEvalContainer::EE_RESULT rRet = { .ncType = EE_NC_UNSIGNED, };
		_sIdx = ArrayIndexToLinearIndex( _sIdx, static_cast<size_t>(Len().u.ui64Val) );
		if ( _sIdx == EE_INVALID_IDX ) { rRet.ncType = EE_NC_INVALID; return rRet; }
		
		uint32_t ui32Val;
		CStringBaseApi::UpdateArrayAccessOptimizer( m_sObj, m_vArrayOpt, _sIdx, ui32Val );
		rRet.u.ui64Val = ui32Val;
		return rRet;
	}

	/**
	 * \brief Inserts a value at a given index in the string.
	 *
	 * Indexing is interpreted in terms of Unicode code points (not raw bytes).
	 *
	 * \param _sIdx The insertion index.
	 * \param _rRet The value to insert.
	 * \return Returns an EE_RESULT representing the inserted/affected value, or EE_NC_INVALID on failure.
	 */
	CExpEvalContainer::EE_RESULT CString::Insert( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) {
		CExpEvalContainer::EE_RESULT rRet = { .ncType = EE_NC_UNSIGNED };
		if ( _sIdx != static_cast<size_t>(Len().u.ui64Val) ) {
			_sIdx = ArrayIndexToLinearIndex( _sIdx, static_cast<size_t>(Len().u.ui64Val) );
			if ( _sIdx == EE_INVALID_IDX ) { rRet.ncType = EE_NC_INVALID; return rRet; }
		}

		if ( _rRet.ncType == EE_NC_OBJECT && _rRet.u.poObj && _rRet.u.poObj->Type() & CObject::EE_BIT_STRING ) {
			ee::CString * psThis = static_cast<CString *>(_rRet.u.poObj);
			uint32_t ui32Val;
			CStringBaseApi::UpdateArrayAccessOptimizer( m_sObj, m_vArrayOpt, _sIdx, ui32Val );
			m_sObj.insert( m_sObj.begin() + m_vArrayOpt[_sIdx], psThis->m_sObj.begin(), psThis->m_sObj.end() );
		}
		else {
			CExpEvalContainer::EE_RESULT rVal = m_peecContainer->ConvertResultOrObject( _rRet, EE_NC_UNSIGNED );
			if ( rVal.ncType == EE_NC_INVALID ) { rRet.ncType = EE_NC_INVALID; return rRet; }

			uint32_t ui32Char, ui32Len;
			ui32Char = ee::CExpEval::Utf32ToUtf8( uint32_t( rVal.u.ui64Val ), ui32Len );
			//if ( EE_UTF_INVALID == ui32Char ) { rRet.ncType = EE_NC_INVALID; return rRet; }

			uint32_t ui32Val;
			CStringBaseApi::UpdateArrayAccessOptimizer( m_sObj, m_vArrayOpt, _sIdx, ui32Val );
			while ( ui32Len-- ) {
				m_sObj.insert( m_sObj.begin() + m_vArrayOpt[_sIdx], std::string::value_type( ui32Char >> (ui32Len * 8)) );
			}
		}
		Dirty();
		return CreateResult();
	}

	/**
	 * \brief Resets the object to its default state.
	 *
	 * This clears the backing string and resets cached/optimization state.
	 */
	void CString::Reset() {
		m_sObj.clear();
		Dirty();
	}

	/**
	 * \brief Invalidates cached length and indexing optimization state.
	 *
	 * Call this after any modification to the backing string.
	 */
	void CString::Dirty() const {
		m_ui64Len = 0;
		m_vArrayOpt.clear();
	}

}	// namespace ee
