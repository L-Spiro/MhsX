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
	bool CVector::ToString( std::string &_sString, uint32_t _ui32Depth, uint32_t _ui32Flags ) {
		if ( _ui32Depth >= 32 ) {
			_sString = "<...>";
			return true;
		}
		std::string sTmp;
		try {
			_sString = "{";

			for ( size_t I = 0; I < m_vBacking.size(); ++I ) {
				if ( !m_peecContainer->ToStringResultOrObject( m_vBacking[I], sTmp, _ui32Depth + 1, _ui32Flags | CObject::EE_TF_C_STRING ) ) {
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
		catch ( ... ) { return false;}
		return true;
	}

	/**
	 * Creates a formatted string representation of the vector by passing the format string to each element of the vector.
	 *
	 * \param _sFormat The format string.
	 * \return Returns a string containing each element in the vector formatted according to the format string provied in _sFormat.
	 */
	std::string CVector::FormattedString( const std::string &_sFormat, uint32_t _ui32Flags ) {
		std::string sTmp, sRet;
		try {
			sRet = "{";

			for ( size_t I = 0; I < m_vBacking.size(); ++I ) {
				if ( !m_peecContainer->ToFormatStringResultOrObject( m_vBacking[I], _sFormat, sTmp, _ui32Flags ) ) {
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
				if ( static_cast<int64_t>(sIdx1) != _i64Idx1 || sIdx1 > m_vBacking.size() ) { rRet.u.poObj = nullptr; return rRet; }
			}
		}
		else {
			sIdx1 = m_vBacking.size();
		}

		ee::CVector * psObj = reinterpret_cast<ee::CVector *>(m_peecContainer->AllocateObject<ee::CVector>());
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
						if ( reinterpret_cast<const ee::CVector *>(_rRet.u.poObj)->m_vBacking.size() != m_vBacking.size() ) { return false; }
						for ( auto I = m_vBacking.size(); I--; ) {
							if ( !m_peecContainer->EqualResultOrObject( m_vBacking[I], reinterpret_cast<const ee::CVector *>(_rRet.u.poObj)->m_vBacking[I] ) ) { return false; }
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
		if ( _rRet.ncType == EE_NC_INVALID ) { return { .ncType = EE_NC_INVALID }; }
		ee::CVector * psObj = reinterpret_cast<ee::CVector *>(m_peecContainer->AllocateObject<ee::CVector>());
		if ( !psObj ) { return { .ncType = EE_NC_INVALID }; }

		if ( _rRet.ncType == EE_NC_OBJECT && _rRet.u.poObj ) {
			if ( (_rRet.u.poObj->Type() & CObject::EE_BIT_VECTOR) ) {
				try {
					psObj->m_vBacking.reserve( m_vBacking.size() + static_cast<const ee::CVector *>(_rRet.u.poObj)->m_vBacking.size() );
					for ( size_t I = 0; I < m_vBacking.size(); ++I ) {
						psObj->m_vBacking.push_back( m_vBacking[I] );
					}
					for ( size_t I = 0; I < static_cast<const ee::CVector *>(_rRet.u.poObj)->m_vBacking.size(); ++I ) {
						psObj->m_vBacking.push_back( static_cast<const ee::CVector *>(_rRet.u.poObj)->m_vBacking[I] );
					}
				}
				catch ( ... ) { m_peecContainer->DeallocateObject( psObj ); return { .ncType = EE_NC_INVALID }; }
				_rRet = static_cast<const ee::CVector *>(_rRet.u.poObj)->CreateResult();
				return psObj->CreateResult();
			}

			try {
				psObj->m_vBacking = m_vBacking;
				if ( psObj->Append( _rRet ).ncType == EE_NC_INVALID ) {
					m_peecContainer->DeallocateObject( psObj );
					return { .ncType = EE_NC_INVALID };
				}
				return psObj->CreateResult();
			}
			catch ( ... ) {
				m_peecContainer->DeallocateObject( psObj );
				return { .ncType = EE_NC_INVALID };
			}
		}
		
		if ( psObj->Resize( m_vBacking.size() ).ncType == EE_NC_INVALID ) { m_peecContainer->DeallocateObject( psObj ); return { .ncType = EE_NC_INVALID }; }
		for ( auto I = m_vBacking.size(); I--; ) {
			auto aCode = m_peecContainer->PerformOp( m_vBacking[I], '+', _rRet, psObj->m_vBacking[I] );
			if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { m_peecContainer->DeallocateObject( psObj ); return { .ncType = EE_NC_INVALID }; }
		}

		return psObj->CreateResult();
	}

	/**
	 * Operator -.  If _rRet is an object, errors, otherwise a new vector is returned with element-wise subtraction of the given value.
	 *
	 * \param _rRet The concatenation object.
	 * \return If _rRet is another vector, a new vector is returned with both this and _rRet’s contents, otherwise an invalid object is returned.
	 */
	CExpEvalContainer::EE_RESULT CVector::Minus( CExpEvalContainer::EE_RESULT &_rRet ) {
		if ( _rRet.ncType == EE_NC_INVALID ) { return { .ncType = EE_NC_INVALID }; }
		ee::CVector * psObj = reinterpret_cast<ee::CVector *>(m_peecContainer->AllocateObject<ee::CVector>());
		if ( !psObj ) { return { .ncType = EE_NC_INVALID }; }

		if ( psObj->Resize( m_vBacking.size() ).ncType == EE_NC_INVALID ) { m_peecContainer->DeallocateObject( psObj ); return { .ncType = EE_NC_INVALID }; }

		if ( _rRet.ncType == EE_NC_OBJECT ) {
			if ( !_rRet.u.poObj || !(_rRet.u.poObj->Type() & CObject::EE_BIT_VECTOR) ) { m_peecContainer->DeallocateObject( psObj ); return { .ncType = EE_NC_INVALID }; }
			if ( m_vBacking.size() != static_cast<ee::CVector *>(_rRet.u.poObj)->m_vBacking.size() ) { m_peecContainer->DeallocateObject( psObj ); return { .ncType = EE_NC_INVALID }; }
			for ( auto I = m_vBacking.size(); I--; ) {
				auto aCode = m_peecContainer->PerformOp( m_vBacking[I], '-', static_cast<ee::CVector *>(_rRet.u.poObj)->m_vBacking[I], psObj->m_vBacking[I] );
				if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { m_peecContainer->DeallocateObject( psObj ); return { .ncType = EE_NC_INVALID }; }
			}
			return psObj->CreateResult();
		}
		
		for ( auto I = m_vBacking.size(); I--; ) {
			auto aCode = m_peecContainer->PerformOp( m_vBacking[I], '-', _rRet, psObj->m_vBacking[I] );
			if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { m_peecContainer->DeallocateObject( psObj ); return { .ncType = EE_NC_INVALID }; }
		}

		return psObj->CreateResult();
	}

	/**
	 * Operator *.  If _rRet is a vector, element-wise multiplication is performed, otherwise a new vector is returned with scalar multiplication of the given value.
	 *
	 * \param _rRet The concatenation object.
	 * \return If _rRet is another vector, a new vector is returned with both this and _rRet’s contents, otherwise an invalid object is returned.
	 */
	CExpEvalContainer::EE_RESULT CVector::Multiply( CExpEvalContainer::EE_RESULT &_rRet ) {
		if ( _rRet.ncType == EE_NC_INVALID ) { return { .ncType = EE_NC_INVALID }; }
		ee::CVector * psObj = reinterpret_cast<ee::CVector *>(m_peecContainer->AllocateObject<ee::CVector>());
		if ( !psObj ) { return { .ncType = EE_NC_INVALID }; }

		if ( _rRet.ncType == EE_NC_OBJECT ) {
			if ( !_rRet.u.poObj || !(_rRet.u.poObj->Type() & CObject::EE_BIT_VECTOR) ) { m_peecContainer->DeallocateObject( psObj ); return { .ncType = EE_NC_INVALID }; }
			if ( Mul( static_cast<ee::CVector *>(_rRet.u.poObj), psObj ).ncType == EE_NC_INVALID ) { m_peecContainer->DeallocateObject( psObj ); return { .ncType = EE_NC_INVALID }; }
			return psObj->CreateResult();
		}
		if ( Mul( _rRet, psObj ).ncType == EE_NC_INVALID ) { m_peecContainer->DeallocateObject( psObj ); return { .ncType = EE_NC_INVALID }; }
		return psObj->CreateResult();
	}

	/**
	 * Operator /.  If _rRet is a vector, element-wise division is performed, otherwise a new vector is returned with scalar division by the given value.
	 *
	 * \param _rRet The concatenation object.
	 * \return If _rRet is another vector, a new vector is returned with both this and _rRet’s contents, otherwise an invalid object is returned.
	 */
	CExpEvalContainer::EE_RESULT CVector::Divide( CExpEvalContainer::EE_RESULT &_rRet ) {
		if ( _rRet.ncType == EE_NC_INVALID ) { return { .ncType = EE_NC_INVALID }; }
		ee::CVector * psObj = reinterpret_cast<ee::CVector *>(m_peecContainer->AllocateObject<ee::CVector>());
		if ( !psObj ) { return { .ncType = EE_NC_INVALID }; }

		if ( _rRet.ncType == EE_NC_OBJECT ) {
			if ( !_rRet.u.poObj || !(_rRet.u.poObj->Type() & CObject::EE_BIT_VECTOR) ) { return { .ncType = EE_NC_INVALID }; }
			if ( Div( static_cast<ee::CVector *>(_rRet.u.poObj), psObj ).ncType == EE_NC_INVALID ) { m_peecContainer->DeallocateObject( psObj ); return { .ncType = EE_NC_INVALID }; }
			return psObj->CreateResult();
		}
		if ( Div( _rRet, psObj ).ncType == EE_NC_INVALID ) { m_peecContainer->DeallocateObject( psObj ); return { .ncType = EE_NC_INVALID }; }
		return psObj->CreateResult();
	}


	/**
	 * Operator +=.  Appends the item or all of the items in the vector if the item is a vector.
	 *
	 * \param _rRet The concatenation object.
	 * \return Returns true unless there were memory failures during the concatenation process.
	 */
	bool CVector::PlusEquals( CExpEvalContainer::EE_RESULT &_rRet ) {
		if ( _rRet.ncType == EE_NC_INVALID ) { return false; }

		if ( _rRet.ncType == EE_NC_OBJECT ) {
			if ( !_rRet.u.poObj ) { return false; }
			if ( (_rRet.u.poObj->Type() & CObject::EE_BIT_VECTOR) ) {
				try {
					const size_t stSize = static_cast<const ee::CVector *>(_rRet.u.poObj)->m_vBacking.size();
					for ( size_t I = 0; I < stSize; ++I ) {
						m_vBacking.push_back( static_cast<const ee::CVector *>(_rRet.u.poObj)->m_vBacking[I] );
					}
				}
				catch ( ... ) {
					return false;
				}
				_rRet = CreateResult();
				return true;
			}
			try {
				m_vBacking.push_back( _rRet );
				_rRet = CreateResult();
				return true;
			}
			catch ( ... ) {
				return false;
			}
		}
		
		for ( auto I = m_vBacking.size(); I--; ) {
			auto aCode = m_peecContainer->PerformOp( m_vBacking[I], '+', _rRet, m_vBacking[I] );
			if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return false; }
		}

		_rRet = CreateResult();
		return true;
	}

	/**
	 * Operator -=.  Subtracts the given scalar from all elements in this vector.
	 *
	 * \param _rRet The concatenation object.
	 * \return Returns true unless there were memory failures during the concatenation process.
	 */
	bool CVector::MinusEquals( CExpEvalContainer::EE_RESULT &_rRet ) {
		if ( _rRet.ncType == EE_NC_INVALID ) { return false; }

		if ( _rRet.ncType == EE_NC_OBJECT ) {
			if ( !_rRet.u.poObj || !(_rRet.u.poObj->Type() & CObject::EE_BIT_VECTOR) ) { return false; }
			if ( m_vBacking.size() != static_cast<ee::CVector *>(_rRet.u.poObj)->m_vBacking.size() ) { return false; }
			for ( auto I = m_vBacking.size(); I--; ) {
				auto aCode = m_peecContainer->PerformOp( m_vBacking[I], '-', static_cast<ee::CVector *>(_rRet.u.poObj)->m_vBacking[I], m_vBacking[I] );
				if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return false; }
			}
			_rRet = CreateResult();
			return true;
		}
		
		for ( auto I = m_vBacking.size(); I--; ) {
			auto aCode = m_peecContainer->PerformOp( m_vBacking[I], '-', _rRet, m_vBacking[I] );
			if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return false; }
		}

		_rRet = CreateResult();
		return true;
	}

	/**
	 * Operator *=.  Performs scalar or element-wise * with self and given operand.
	 *
	 * \param _rRet The concatenation object.
	 * \return Returns true unless there were memory failures during the concatenation process.
	 */
	bool CVector::TimesEquals( CExpEvalContainer::EE_RESULT &_rRet ) {
		if ( _rRet.ncType == EE_NC_INVALID ) { return false; }

		if ( _rRet.ncType == EE_NC_OBJECT ) {
			if ( !_rRet.u.poObj || !(_rRet.u.poObj->Type() & CObject::EE_BIT_VECTOR) ) { return false; }
			if ( m_vBacking.size() != static_cast<ee::CVector *>(_rRet.u.poObj)->m_vBacking.size() ) { return false; }
			for ( auto I = m_vBacking.size(); I--; ) {
				auto aCode = m_peecContainer->PerformOp( m_vBacking[I], '*', static_cast<ee::CVector *>(_rRet.u.poObj)->m_vBacking[I], m_vBacking[I] );
				if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return false; }
			}
			_rRet = CreateResult();
			return true;
		}
		
		for ( auto I = m_vBacking.size(); I--; ) {
			auto aCode = m_peecContainer->PerformOp( m_vBacking[I], '*', _rRet, m_vBacking[I] );
			if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return false; }
		}

		_rRet = CreateResult();
		return true;
	}

	/**
	 * Operator /=.  Performs scalar or element-wise / with self and given operand.
	 *
	 * \param _rRet The concatenation object.
	 * \return Returns true unless there were memory failures during the concatenation process.
	 */
	bool CVector::DivideEquals( CExpEvalContainer::EE_RESULT &_rRet ) {
		if ( _rRet.ncType == EE_NC_INVALID ) { return false; }

		if ( _rRet.ncType == EE_NC_OBJECT ) {
			if ( !_rRet.u.poObj || !(_rRet.u.poObj->Type() & CObject::EE_BIT_VECTOR) ) { return false; }
			if ( m_vBacking.size() != static_cast<ee::CVector *>(_rRet.u.poObj)->m_vBacking.size() ) { return false; }
			for ( auto I = m_vBacking.size(); I--; ) {
				auto aCode = m_peecContainer->PerformOp( m_vBacking[I], '/', static_cast<ee::CVector *>(_rRet.u.poObj)->m_vBacking[I], m_vBacking[I] );
				if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return false; }
			}
			_rRet = CreateResult();
			return true;
		}
		
		for ( auto I = m_vBacking.size(); I--; ) {
			auto aCode = m_peecContainer->PerformOp( m_vBacking[I], '/', _rRet, m_vBacking[I] );
			if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return false; }
		}

		_rRet = CreateResult();
		return true;
	}

	// Sums all elements.
	CExpEvalContainer::EE_RESULT CVector::Sum() {
		if ( !m_vBacking.size() ) { return { .ncType = EE_NC_INVALID }; }
		CExpEvalContainer::EE_RESULT rRet = { .ncType = EE_NC_UNSIGNED };
		rRet.u.ui64Val = 0;
		for ( std::vector<CExpEvalContainer::EE_RESULT>::size_type I = 0; I < m_vBacking.size(); ++I ) {
			CExpEvalContainer::EE_RESULT rTmp;
			auto aCode = m_peecContainer->PerformOp( rRet, '+', m_vBacking[I], rTmp );
			if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return { .ncType = EE_NC_INVALID }; }
			rRet = rTmp;
		}
		return rRet;
	}

	// Element-wise addition.
	CExpEvalContainer::EE_RESULT CVector::Add( const ee::CVector * _pvOther, ee::CVector * _pvReturn ) {
		_pvReturn->Clear();
		
		if ( _pvReturn->Resize( std::max( m_vBacking.size(), _pvOther->m_vBacking.size() ) ).ncType == EE_NC_INVALID ) { return { .ncType = EE_NC_INVALID }; }
		for ( auto I = _pvReturn->m_vBacking.size(); I--; ) {
			if ( I >= m_vBacking.size() ) {
				_pvReturn->m_vBacking[I] = _pvOther->m_vBacking[I];
			}
			else if ( I >= _pvOther->m_vBacking.size() ) {
				_pvReturn->m_vBacking[I] = m_vBacking[I];
			}
			else {
				auto aCode = m_peecContainer->PerformOp( m_vBacking[I], '+', _pvOther->m_vBacking[I], _pvReturn->m_vBacking[I] );
				if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return { .ncType = EE_NC_INVALID }; }
			}
		}

		return _pvReturn->CreateResult();
	}

	// Element-wise subtraction.
	CExpEvalContainer::EE_RESULT CVector::Sub( const ee::CVector * _pvOther, ee::CVector * _pvReturn ) {
		_pvReturn->Clear();
		
		if ( _pvReturn->Resize( std::max( m_vBacking.size(), _pvOther->m_vBacking.size() ) ).ncType == EE_NC_INVALID ) { return { .ncType = EE_NC_INVALID }; }
		for ( auto I = _pvReturn->m_vBacking.size(); I--; ) {
			if ( I >= m_vBacking.size() ) {
				_pvReturn->m_vBacking[I] = _pvOther->m_vBacking[I];
			}
			else if ( I >= _pvOther->m_vBacking.size() ) {
				_pvReturn->m_vBacking[I] = m_vBacking[I];
			}
			else {
				auto aCode = m_peecContainer->PerformOp( m_vBacking[I], '-', _pvOther->m_vBacking[I], _pvReturn->m_vBacking[I] );
				if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return { .ncType = EE_NC_INVALID }; }
			}
		}

		return _pvReturn->CreateResult();
	}

	// Element-wise multiplication.
	CExpEvalContainer::EE_RESULT CVector::Mul( const ee::CVector * _pvOther, ee::CVector * _pvReturn ) {
		_pvReturn->Clear();
		
		if ( _pvReturn->Resize( std::max( m_vBacking.size(), _pvOther->m_vBacking.size() ) ).ncType == EE_NC_INVALID ) { return { .ncType = EE_NC_INVALID }; }
		for ( auto I = _pvReturn->m_vBacking.size(); I--; ) {
			if ( I >= m_vBacking.size() ) {
				_pvReturn->m_vBacking[I] = _pvOther->m_vBacking[I];
			}
			else if ( I >= _pvOther->m_vBacking.size() ) {
				_pvReturn->m_vBacking[I] = m_vBacking[I];
			}
			else {
				auto aCode = m_peecContainer->PerformOp( m_vBacking[I], '*', _pvOther->m_vBacking[I], _pvReturn->m_vBacking[I] );
				if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return { .ncType = EE_NC_INVALID }; }
			}
		}

		return _pvReturn->CreateResult();
	}

	// Scalar multiplication.
	CExpEvalContainer::EE_RESULT CVector::Mul( const CExpEvalContainer::EE_RESULT &_rScalar, ee::CVector * _pvReturn ) {
		if ( _rScalar.ncType == EE_NC_INVALID ) { return { .ncType = EE_NC_INVALID }; }
		_pvReturn->Clear();

		if ( _pvReturn->Resize( m_vBacking.size() ).ncType == EE_NC_INVALID ) { return { .ncType = EE_NC_INVALID }; }
		for ( auto I = _pvReturn->m_vBacking.size(); I--; ) {
			auto aCode = m_peecContainer->PerformOp( m_vBacking[I], '*', _rScalar, _pvReturn->m_vBacking[I] );
			if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return { .ncType = EE_NC_INVALID }; }
		}

		return _pvReturn->CreateResult();
	}

	// Element-wise division.
	CExpEvalContainer::EE_RESULT CVector::Div( const ee::CVector * _pvOther, ee::CVector * _pvReturn ) {
		try {
			_pvReturn->Clear();
		
			if ( _pvReturn->Resize( std::max( m_vBacking.size(), _pvOther->m_vBacking.size() ) ).ncType == EE_NC_INVALID ) { return { .ncType = EE_NC_INVALID }; }
			for ( auto I = _pvReturn->m_vBacking.size(); I--; ) {
				if ( I >= m_vBacking.size() ) {
					_pvReturn->m_vBacking[I] = _pvOther->m_vBacking[I];
				}
				else if ( I >= _pvOther->m_vBacking.size() ) {
					_pvReturn->m_vBacking[I] = m_vBacking[I];
				}
				else {
					auto aCode = m_peecContainer->PerformOp( m_vBacking[I], '/', _pvOther->m_vBacking[I], _pvReturn->m_vBacking[I] );
					if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return { .ncType = EE_NC_INVALID }; }
				}
			}

			return _pvReturn->CreateResult();
		}
		catch ( ... ) { return { .ncType = EE_NC_INVALID }; }
	}

	// Scalar division.
	CExpEvalContainer::EE_RESULT CVector::Div( const CExpEvalContainer::EE_RESULT &_rScalar, ee::CVector * _pvReturn ) {
		if ( _rScalar.ncType == EE_NC_INVALID ) { return { .ncType = EE_NC_INVALID }; }
		try {
			_pvReturn->Clear();

			if ( _pvReturn->Resize( m_vBacking.size() ).ncType == EE_NC_INVALID ) { return { .ncType = EE_NC_INVALID }; }
			for ( auto I = _pvReturn->m_vBacking.size(); I--; ) {
				auto aCode = m_peecContainer->PerformOp( m_vBacking[I], '/', _rScalar, _pvReturn->m_vBacking[I] );
				if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return { .ncType = EE_NC_INVALID }; }
			}

			return _pvReturn->CreateResult();
		}
		catch ( ... ) { return { .ncType = EE_NC_INVALID }; }
	}

	// Dot product.
	CExpEvalContainer::EE_RESULT CVector::Dot( const ee::CVector * _pvOther ) {
		if ( m_vBacking.size() != _pvOther->m_vBacking.size() ) { return { .ncType = EE_NC_INVALID }; }

		CExpEvalContainer::EE_RESULT rRet = { .ncType = EE_NC_UNSIGNED };
		rRet.u.ui64Val = 0;
		for ( auto I = m_vBacking.size(); I--; ) {
			CExpEvalContainer::EE_RESULT rTmp;
			auto aCode = m_peecContainer->PerformOp( m_vBacking[I], '*', _pvOther->m_vBacking[I], rTmp );
			if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return { .ncType = EE_NC_INVALID }; }
			CExpEvalContainer::EE_RESULT rTmp2;
			aCode = m_peecContainer->PerformOp( rRet, '+', rTmp, rTmp2 );
			if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return { .ncType = EE_NC_INVALID }; }
			rRet = rTmp2;
		}

		return rRet;
	}

	// Cross product.
	CExpEvalContainer::EE_RESULT CVector::Cross( const ee::CVector * _pvOther, ee::CVector * _pvReturn ) {
		if ( m_vBacking.size() != _pvOther->m_vBacking.size() ) { return { .ncType = EE_NC_INVALID }; }
		//if ( m_vBacking.size() != 2 && m_vBacking.size() != 3 ) { return { .ncType = EE_NC_INVALID }; }

		CExpEvalContainer::EE_RESULT rRet;
		switch ( m_vBacking.size() ) {
			case 2 : {
				// 2D Cross Product, result is a scalar representing the z component.
				//	T tResult = vA[0] * vB[1] - vA[1] * vB[0];
				CExpEvalContainer::EE_RESULT rTmp, rTmp2;
				// vA[0] * vB[1]
				auto aCode = m_peecContainer->PerformOp( m_vBacking[0], '*', _pvOther->m_vBacking[1], rTmp );
				if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return { .ncType = EE_NC_INVALID }; }
				// vA[1] * vB[0]
				aCode = m_peecContainer->PerformOp( m_vBacking[1], '*', _pvOther->m_vBacking[0], rTmp2 );
				if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return { .ncType = EE_NC_INVALID }; }
				// (vA[0] * vB[1]) - (vA[1] * vB[0])
				aCode = m_peecContainer->PerformOp( rTmp, '-', rTmp2, rRet );
				if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return { .ncType = EE_NC_INVALID }; }
				return rRet;
			}
			case 3 : {
				// 3D Cross Product.
				//	std::vector<T> vResult( 3 );
				//	vResult[0] = vA[1] * vB[2] - vA[2] * vB[1];
				//	vResult[1] = vA[2] * vB[0] - vA[0] * vB[2];
				//	vResult[2] = vA[0] * vB[1] - vA[1] * vB[0];
				try {
					_pvReturn->m_vBacking.resize( 3 );
				}
				catch ( ... ) { return { .ncType = EE_NC_INVALID }; }
				CExpEvalContainer::EE_RESULT rTmp, rTmp2;
				// vA[1] * vB[2]
				auto aCode = m_peecContainer->PerformOp( m_vBacking[1], '*', _pvOther->m_vBacking[2], rTmp );
				if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return { .ncType = EE_NC_INVALID }; }
				// vA[2] * vB[1]
				aCode = m_peecContainer->PerformOp( m_vBacking[2], '*', _pvOther->m_vBacking[1], rTmp2 );
				if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return { .ncType = EE_NC_INVALID }; }
				// vResult[0] = vA[1] * vB[2] - vA[2] * vB[1];
				aCode = m_peecContainer->PerformOp( rTmp, '-', rTmp2, _pvReturn->m_vBacking[0] );
				if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return { .ncType = EE_NC_INVALID }; }

				// vA[2] * vB[0]
				aCode = m_peecContainer->PerformOp( m_vBacking[2], '*', _pvOther->m_vBacking[0], rTmp );
				if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return { .ncType = EE_NC_INVALID }; }
				// vA[0] * vB[2]
				aCode = m_peecContainer->PerformOp( m_vBacking[0], '*', _pvOther->m_vBacking[2], rTmp2 );
				if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return { .ncType = EE_NC_INVALID }; }
				// vResult[1] = vA[2] * vB[0] - vA[0] * vB[2];
				aCode = m_peecContainer->PerformOp( rTmp, '-', rTmp2, _pvReturn->m_vBacking[1] );
				if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return { .ncType = EE_NC_INVALID }; }

				// vA[0] * vB[1]
				aCode = m_peecContainer->PerformOp( m_vBacking[0], '*', _pvOther->m_vBacking[1], rTmp );
				if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return { .ncType = EE_NC_INVALID }; }
				// vA[1] * vB[0]
				aCode = m_peecContainer->PerformOp( m_vBacking[1], '*', _pvOther->m_vBacking[0], rTmp2 );
				if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return { .ncType = EE_NC_INVALID }; }
				// vResult[2] = vA[0] * vB[1] - vA[1] * vB[0];
				aCode = m_peecContainer->PerformOp( rTmp, '-', rTmp2, _pvReturn->m_vBacking[2] );
				if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return { .ncType = EE_NC_INVALID }; }

				return _pvReturn->CreateResult();
			}
			default : { return { .ncType = EE_NC_INVALID }; }
		}
	}

	// Gets the magnitude of the vector.
	CExpEvalContainer::EE_RESULT CVector::Mag() {
		CExpEvalContainer::EE_RESULT rRet = CExpEvalContainer::DefaultResult();

		for ( auto I = m_vBacking.size(); I--; ) {
			CExpEvalContainer::EE_RESULT rTmp;
			auto aCode = m_peecContainer->PerformOp( m_vBacking[I], '*', m_vBacking[I], rTmp );
			if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return { .ncType = EE_NC_INVALID }; }
			CExpEvalContainer::EE_RESULT rTmp2;
			aCode = m_peecContainer->PerformOp( rRet, '+', rTmp, rTmp2 );
			if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return { .ncType = EE_NC_INVALID }; }
			rRet = rTmp2;
		}
		rRet = m_peecContainer->ConvertResultOrObject( rRet, EE_NC_FLOATING );
		rRet.u.dVal = std::sqrt( rRet.u.dVal );
		return rRet;
	}

	// Gets the squared magnitude of the vector.
	CExpEvalContainer::EE_RESULT CVector::MagSq() {
		CExpEvalContainer::EE_RESULT rRet = CExpEvalContainer::DefaultResult();

		for ( auto I = m_vBacking.size(); I--; ) {
			CExpEvalContainer::EE_RESULT rTmp;
			auto aCode = m_peecContainer->PerformOp( m_vBacking[I], '*', m_vBacking[I], rTmp );
			if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return { .ncType = EE_NC_INVALID }; }
			CExpEvalContainer::EE_RESULT rTmp2;
			aCode = m_peecContainer->PerformOp( rRet, '+', rTmp, rTmp2 );
			if ( aCode != CExpEvalContainer::EE_EC_SUCCESS ) { return { .ncType = EE_NC_INVALID }; }
			rRet = rTmp2;
		}
		//rRet = m_peecContainer->ConvertResultOrObject( rRet, EE_NC_FLOATING );
		return rRet;
	}

	// Return a normalized copy of this vector.
	CExpEvalContainer::EE_RESULT CVector::Normalize( ee::CVector * _pvReturn ) {
		return Div( Mag(), _pvReturn );
	}

}	// namespace ee
