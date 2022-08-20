#pragma once

#include "EEObject.h"


namespace ee {

	class CStringRef : public CObject {
	public :
		CStringRef( CExpEvalContainer * _peecContainer, size_t _stStrIdx ) :
			CObject( _peecContainer ),
			m_stSrcIdx( _stStrIdx ),
			m_ui64Len( 0 ) {
		}


		// == Functions.
		// Gets the object type.
		virtual uint32_t							Type() const { return CObject::Type() | CObject::EE_BIT_STRING_REF; }

		// Gets the string index.
		inline size_t								GetStrIdx() const { return m_stSrcIdx; }

		// Converts to another object of the given type.
		virtual CExpEvalContainer::EE_RESULT		ConvertTo( EE_NUM_CONSTANTS _ncType );

		// Initializes this object given another object.
		virtual bool								InitializeFrom( const CExpEvalContainer::EE_RESULT &_rObj ) { return false; }

		// Creates a string representation of the object, with the string usually assumed to be in UTF-8 format.
		virtual bool								ToString( std::string &_sString ) {
			_sString = m_peecContainer->m_vStrings[GetStrIdx()];
			return true;
		}

		// Gives a reference to the std::string object in the container.
		const std::string &							StringRef() const {
			return m_peecContainer->m_vStrings[GetStrIdx()];
		}

		// Array access.
		virtual CExpEvalContainer::EE_RESULT		ArrayAccess( int64_t _i64Idx );

		// Extended array access.
		virtual CExpEvalContainer::EE_RESULT		ArrayAccessEx( int64_t _i64Idx0, int64_t _i64Idx1, uint32_t _ui32Mask ) {
			return { EE_NC_OBJECT };
		}


		// == Operators.
		// Operator +=.
		//virtual bool								PlusEquals( CExpEvalContainer::EE_RESULT &_rRet );


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
			if ( !m_ui64Len ) { m_ui64Len = CountUtfCodePoints( m_peecContainer->m_vStrings[GetStrIdx()] ); }
			rRes.u.ui64Val = m_ui64Len;
			return rRes;
		}
		
		// Gets the octadecimal form of the object as a string (0o****).
		virtual CExpEvalContainer::EE_RESULT		Oct() const;

		// Returns the ordinal value of the object as a Unicode character (always EE_NC_UNSIGNED).
		virtual CExpEvalContainer::EE_RESULT		Ord() const;


	protected :
		// == Members.
		// Index into m_peecContainer->m_vStrings where this string lives.
		size_t										m_stSrcIdx;
		// The saved result of Len(), since this string never changes.
		mutable uint64_t							m_ui64Len;
		// Array access optimizer.
		mutable std::vector<size_t>					m_vArrayOpt;
	};

}	// namespace ee
