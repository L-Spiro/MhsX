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
		// Gets the object type.
		virtual uint32_t							Type() const { return CObject::Type() | CObject::EE_BIT_STRING; }

		// Sets the string to a given value.
		CString &									SetString( const std::string &_sString ) {
			m_sObj = _sString;
			Dirty();
			return (*this);
		}

		// Gets the string.
		const std::string &							GetString() const { return m_sObj; }

		// Converts to another object of the given type.
		virtual CExpEvalContainer::EE_RESULT		ConvertTo( EE_NUM_CONSTANTS _ncType );

		// Initializes this object given another object.
		virtual bool								InitializeFrom( const CExpEvalContainer::EE_RESULT &_rObj );

		// Creates a string representation of the object, with the string usually assumed to be in UTF-8 format.
		virtual bool								ToString( std::string &_sString, uint32_t _ui32Depth, uint32_t _ui32Flags = EE_TF_NONE ) {
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

		// Creates a formatted string representation of the object.
		virtual std::string							FormattedString( const std::string &_sFormat, uint32_t _ui32Flags = EE_TF_NONE ) {
			if ( _ui32Flags & CObject::EE_TF_C_STRING ) {
				auto sTmp = CExpEval::ToCString( m_sObj );
				return std::vformat( _sFormat, std::make_format_args( sTmp ) );
			}
			//return std::format( _sFormat, m_sObj );
			return std::vformat( _sFormat, std::make_format_args( m_sObj ) );
		}

		// Array access.
		virtual CExpEvalContainer::EE_RESULT		ArrayAccess( int64_t _i64Idx );

		// Extended array access.
		virtual CExpEvalContainer::EE_RESULT		ArrayAccessEx( int64_t _i64Idx0, int64_t _i64Idx1, uint32_t _ui32Mask );

		// Gets the array length.
		virtual size_t								ArrayLength();


		// == Operators.
		// Operator ==.
		virtual bool								Equals( CExpEvalContainer::EE_RESULT &_rRet );

		// Operator +.
		virtual CExpEvalContainer::EE_RESULT		Plus( CExpEvalContainer::EE_RESULT &_rRet );

		// Operator *.
		virtual CExpEvalContainer::EE_RESULT		Multiply( CExpEvalContainer::EE_RESULT &_rRet );

		// Operator +=.
		virtual bool								PlusEquals( CExpEvalContainer::EE_RESULT &_rRet );


		// == Base API functions.
		// Gets the object as an ASCII string.
		virtual CExpEvalContainer::EE_RESULT		Ascii() const;

		// Gets the binary form of the object as a string (0b****).
		virtual CExpEvalContainer::EE_RESULT		Bin() const;

		// Gets the boolean form of the object as a string (0b****).
		virtual CExpEvalContainer::EE_RESULT		Bool() const;

		// Returns the character that represents the specified Unicode. 
		virtual CExpEvalContainer::EE_RESULT		Chr() const;

		// Gets the string interpreted to its best fit and then converted to float. 
		virtual CExpEvalContainer::EE_RESULT		Float() const;

		// Gets the hexadecimal form of the object as a string (0x****).
		virtual CExpEvalContainer::EE_RESULT		Hex() const;

		// Gets the string interpreted to its best fit and then converted to int64_t. 
		virtual CExpEvalContainer::EE_RESULT		Int() const;

		// Gets the number of elements in the object.  For arrays, maps, sets, lists, etc., this is the number of items.  For Unicode strings, this is the number of UTF code points.
		virtual CExpEvalContainer::EE_RESULT		Len() const {
			CExpEvalContainer::EE_RESULT rRes = { EE_NC_UNSIGNED };
			if ( !m_ui64Len ) { m_ui64Len = ee::CExpEval::CountUtfCodePoints( m_sObj ); }
			rRes.u.ui64Val = m_ui64Len;
			return rRes;
		}

		// Gets the octadecimal form of the object as a string (0o****).
		virtual CExpEvalContainer::EE_RESULT		Oct() const;

		// Returns the ordinal value of the object as a Unicode character (always EE_NC_UNSIGNED).
		virtual CExpEvalContainer::EE_RESULT		Ord() const;

		// Gets the backing string directly.
		const std::string &							GetBacking() const { return m_sObj; }

		// Append an item to the end of the string.
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

		// Pops the back item.
		virtual CExpEvalContainer::EE_RESULT		PopBack();

		// Gets the capacity.
		virtual CExpEvalContainer::EE_RESULT		Capacity() const {
			CExpEvalContainer::EE_RESULT rRet = { .ncType = EE_NC_UNSIGNED };
			rRet.u.ui64Val = m_sObj.capacity();
			return rRet;
		}

		// Gets whether the string is empty or not.
		virtual CExpEvalContainer::EE_RESULT		Empty() const {
			CExpEvalContainer::EE_RESULT rRet = { .ncType = EE_NC_UNSIGNED };
			rRet.u.ui64Val = m_sObj.empty();
			return rRet;
		}

		// Clears the string.
		virtual CExpEvalContainer::EE_RESULT		Clear() { m_sObj.clear(); Dirty(); return CreateResult(); }

		// Appends to the string.
		virtual CExpEvalContainer::EE_RESULT		Append( CExpEvalContainer::EE_RESULT &_rRet ) { return PushBack( _rRet ); }

		// Gets a value in the string.
		virtual CExpEvalContainer::EE_RESULT		At( size_t _sIdx );

		// Inserts a value at a given index in the string.
		virtual CExpEvalContainer::EE_RESULT		Insert( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet );

		// Erases a value at an index in the string.
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

		// Gets the max size.
		virtual CExpEvalContainer::EE_RESULT		MaxSize() const {
			CExpEvalContainer::EE_RESULT rRet = { .ncType = EE_NC_UNSIGNED };
			rRet.u.ui64Val = m_sObj.max_size();
			return rRet;
		}

		// Gets the size.
		virtual CExpEvalContainer::EE_RESULT		Size() const {
			return Len();
		}

		// Reserves memory for X number of elements.
		virtual CExpEvalContainer::EE_RESULT		Reserve( size_t _sTotal ) {
			m_sObj.reserve( _sTotal );
			return CreateResult();
		}

		// Resize to X elements.
		virtual CExpEvalContainer::EE_RESULT		Resize( size_t _sTotal ) {
			m_sObj.resize( _sTotal );
			return CreateResult();
		}

		// Shrinks to fit.
		virtual CExpEvalContainer::EE_RESULT		ShrinkToFit() {
			m_sObj.shrink_to_fit();
			return CreateResult();
		}

		// Swaps the string with another.
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
		// The actual string object we manipulate.
		std::string									m_sObj;
		// The saved result of Len(), set to 0 when the string changes.
		mutable uint64_t							m_ui64Len;
		// Array access optimizer.
		mutable std::vector<size_t>					m_vArrayOpt;


		// == Functions.
		// Resets the object.
		void										Reset();

		// Resets only the optimization tables.
		void										Dirty() const;
	};

}	// namespace ee
