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

		// Converts to another object of the given type.
		virtual CExpEvalContainer::EE_RESULT		ConvertTo( EE_NUM_CONSTANTS _ncType );

		// Initializes this object given another object.
		virtual bool								InitializeFrom( const CExpEvalContainer::EE_RESULT &_rObj );

		// Creates a string representation of the object, with the string usually assumed to be in UTF-8 format.
		virtual bool								ToString( std::string &_sString ) {
			_sString = m_sObj;
			return true;
		}

		// Creates a formatted string representation of the object.
		virtual std::string							FormattedString( const std::string &_sFormat ) {
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
