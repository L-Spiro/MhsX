#include "EEVector.h"


namespace ee {

	// == Functions.
	/**
	 * Converts to another object of the given type.
	 *
	 * \param _ncType The destination type.
	 * \return If the vector has only 1 element, that element is converted to the given target type, otherwise EE_NC_INVALID is returned.
	 */
	CExpEvalContainer::EE_RESULT CVector::ConvertTo( EE_NUM_CONSTANTS _ncType ) {
		CExpEvalContainer::EE_RESULT rRet;
		if ( m_vBacking.size() == 1 ) {
			return m_peecContainer->ConvertResultOrObject( m_vBacking[0], _ncType );
		}
		rRet.ncType = EE_NC_INVALID;
		rRet.u.poObj = nullptr;
		return rRet;
	}

	/**
	 * Initializes this object given another object.
	 *
	 * \param _rObj The source object from which to initialize this object.
	 * \return Returns true if a valid conversion from the given object can be made to a vector.
	 */
	bool CVector::InitializeFrom( const CExpEvalContainer::EE_RESULT &_rObj ) {
		switch ( _rObj.ncType ) {
			case EE_NC_OBJECT : {
				if ( _rObj.u.poObj ) {
					if ( _rObj.u.poObj->Type() & CObject::EE_BIT_VECTOR ) {
						try {
							m_vBacking = reinterpret_cast<const CVector &>(_rObj.u.poObj).m_vBacking;
						}
						catch ( ... ) {
						}
					}
				}
				break;
			}
		}
		return false;
	}

	/**
	 * Creates a string representation of the object, with the string usually assumed to be in UTF-8 format.
	 *
	 * \param _sString Holds the returned string.
	 * \return Returns true if the object can be converted to a string and no memory errors were encountered when doing so.
	 */
	bool CVector::ToString( std::string &_sString ) {
		std::string sTmp;
		try {
			_sString = "{";

			for ( size_t I = 0; I < m_vBacking.size(); ++I ) {
				if ( !m_peecContainer->ToStringResultOrObject( m_vBacking[I], sTmp ) ) {
					_sString += "<error>";
				}
				else {
					_sString += sTmp;
				}
				if ( I != m_vBacking.size() - 1 ) {
					_sString += ", ";
				}
			}

			_sString += "}";
		}
		catch ( ... ) {}
		return false;
	}

	/**
	 * Creates a formatted string representation of the vector by passing the format string to each element of the vector.
	 *
	 * \param _sFormat The format string.
	 * \return Returns a string containing each element in the vector formatted according to the format string provied in _sFormat.
	 */
	std::string CVector::FormattedString( const std::string &_sFormat ) {
		std::string sTmp, sRet;
		try {
			sRet = "{";

			for ( size_t I = 0; I < m_vBacking.size(); ++I ) {
				if ( !m_peecContainer->ToFormatStringResultOrObject( m_vBacking[I], _sFormat, sTmp ) ) {
					sRet += "<error>";
				}
				else {
					sRet += sTmp;
				}
				if ( I != m_vBacking.size() - 1 ) {
					sRet += ", ";
				}
			}

			sRet += "}";
		}
		catch ( ... ) {}
		return sRet;
	}

	/**
	 * Array access.
	 *
	 * \param _i64Idx Index of the item to access.
	 * \return Returns the item at the given index or an invalid item.
	 */
	CExpEvalContainer::EE_RESULT CVector::ArrayAccess( int64_t _i64Idx ) {
		size_t sIdx = size_t( _i64Idx );
		if ( _i64Idx < 0 ) {
			sIdx = ArrayIndexToLinearIndex( _i64Idx, m_vBacking.size() );
		}
		if ( sIdx < m_vBacking.size() ) {
			return m_vBacking[sIdx];
		}
		CExpEvalContainer::EE_RESULT rRet;
		rRet.ncType = EE_NC_INVALID;
		rRet.u.poObj = nullptr;
		return rRet;
	}

	/**
	 * Extended array access.
	 *
	 * \param _i64Idx0 The start index.
	 * \param _i64Idx1 The end index.
	 * \param _ui32Mask From-start or to-end flags.
	 * \return Returns the resulting vector created from selecting the given items.
	 */
	CExpEvalContainer::EE_RESULT CVector::ArrayAccessEx( int64_t _i64Idx0, int64_t _i64Idx1, uint32_t _ui32Mask ) {
		CExpEvalContainer::EE_RESULT rRet = { EE_NC_INVALID };
		
		size_t sIdx0 = 0;
		if ( _ui32Mask & EE_AEF_START ) {
			if ( _i64Idx0 < 0 ) {
				sIdx0 = ArrayIndexToLinearIndex( _i64Idx0, m_vBacking.size() );
			}
			else {
				sIdx0 = static_cast<size_t>(_i64Idx0);
				if ( static_cast<int64_t>(sIdx0) != _i64Idx0 || sIdx0 >= m_vBacking.size() ) { rRet.u.poObj = nullptr; return rRet; }
			}
		}

		size_t sIdx1;
		if ( _ui32Mask & EE_AEF_END ) {
			if ( _i64Idx1 < 0 ) {
				sIdx1 = ArrayIndexToLinearIndex( _i64Idx1, m_vBacking.size() );
			}
			else {
				sIdx1 = static_cast<size_t>(_i64Idx1);
				if ( static_cast<int64_t>(sIdx1) != _i64Idx1 || sIdx1 >= m_vBacking.size() ) { rRet.u.poObj = nullptr; return rRet; }
			}
		}
		else {
			sIdx1 = m_vBacking.size();
		}

		CVector * psObj = reinterpret_cast<CVector *>(m_peecContainer->AllocateObject<CVector>());
		if ( psObj ) {
			try {
				for ( size_t I = sIdx0; I < sIdx1; ++I ) {
					psObj->m_vBacking.push_back( m_vBacking[I] );
				}
			}
			catch ( ... ) {
				m_peecContainer->DeallocateObject( psObj );
				return rRet;
			}
			rRet = psObj->CreateResult();
		}
		return rRet;
	}

	// == Operators.
	/**
	 * Operator ==.  Returns true if _rRet is another vector of the same length and every element in _rRet compares equal to every element in this vector.
	 *
	 * \param _rRet The comparison object.
	 * \return Returns true if _rRet is a vector, has the same number of items, and each item compares == to each respective item in this vector.
	 */
	bool CVector::Equals( CExpEvalContainer::EE_RESULT &_rRet ) {
		switch ( _rRet.ncType ) {
			case EE_NC_OBJECT : {
				if ( _rRet.u.poObj ) {
					if ( _rRet.u.poObj->Type() & CObject::EE_BIT_VECTOR ) {
						if ( reinterpret_cast<const CVector &>(_rRet.u.poObj).m_vBacking.size() != m_vBacking.size() ) { return false; }
						for ( auto I = m_vBacking.size(); I--; ) {
							if ( !m_peecContainer->EqualResultOrObject( m_vBacking[I], reinterpret_cast<const CVector &>(_rRet.u.poObj).m_vBacking[I] ) ) { return false; }
						}
						return true;
					}
				}
				break;
			}
		}
		return false;
	}

	/**
	 * Operator +.  If _rRet is another vector, a new vector is returned containing the contents of this vector with _rRet appended, otherwise a new vector is returned containing this vector and the new item appended.
	 *
	 * \param _rRet The concatenation object.
	 * \return If _rRet is another vector, a new vector is returned with both this and _rRet’s contents, otherwise an invalid object is returned.
	 */
	CExpEvalContainer::EE_RESULT CVector::Plus( CExpEvalContainer::EE_RESULT &_rRet ) {
		CExpEvalContainer::EE_RESULT rRet = { EE_NC_INVALID };
		if ( _rRet.ncType == EE_NC_OBJECT ) {
			if ( !_rRet.u.poObj ) { return rRet; }
			if ( (_rRet.u.poObj->Type() & CObject::EE_BIT_VECTOR) ) {
				CVector * psObj = reinterpret_cast<CVector *>(m_peecContainer->AllocateObject<CVector>());
				if ( psObj ) {
					try {
						psObj->m_vBacking = m_vBacking;
						const size_t stSize = reinterpret_cast<const CVector &>(_rRet.u.poObj).m_vBacking.size();
						for ( size_t I = 0; I < stSize; ++I ) {
							psObj->m_vBacking.push_back( reinterpret_cast<const CVector &>(_rRet.u.poObj).m_vBacking[I] );
						}
					}
					catch ( ... ) {
						m_peecContainer->DeallocateObject( psObj );
						return rRet;
					}
					rRet = psObj->CreateResult();
				}
				return rRet;
			}
		}
		CVector * psObj = reinterpret_cast<CVector *>(m_peecContainer->AllocateObject<CVector>());
		if ( psObj ) {
			try {
				psObj->m_vBacking = m_vBacking;
				psObj->m_vBacking.push_back( _rRet );
			}
			catch ( ... ) {
				m_peecContainer->DeallocateObject( psObj );
				return rRet;
			}
			rRet = psObj->CreateResult();
		}

		return rRet;
	}

	/**
	 * Operator +=.  Appends the item or all of the items in the vector if the item is a vector.
	 *
	 * \param _rRet The concatenation object.
	 * \return Returns true unless there were memory failures during the concatenation process.
	 */
	bool CVector::PlusEquals( CExpEvalContainer::EE_RESULT &_rRet ) {
		if ( _rRet.ncType == EE_NC_OBJECT ) {
			if ( !_rRet.u.poObj ) { return false; }
			if ( (_rRet.u.poObj->Type() & CObject::EE_BIT_VECTOR) ) {
				try {
					const size_t stSize = reinterpret_cast<const CVector &>(_rRet.u.poObj).m_vBacking.size();
					for ( size_t I = 0; I < stSize; ++I ) {
						m_vBacking.push_back( reinterpret_cast<const CVector &>(_rRet.u.poObj).m_vBacking[I] );
					}
				}
				catch ( ... ) {
					return false;
				}
				return true;
			}
		}
		
		try {
			m_vBacking.push_back( _rRet );
			return true;
		}
		catch ( ... ) {
			return false;
		}
	}

}	// namespace ee
