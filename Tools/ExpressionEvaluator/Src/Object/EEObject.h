#pragma once

#include "../EEExpEval.h"
#include "../Api/EEBaseApi.h"
#include "../EEExpEvalContainer.h"
#include <vector>

namespace ee {

	class CObject : public CBaseApi {
	public :
		CObject( CExpEvalContainer * _peecContainer ) :
			m_peecContainer( _peecContainer ) {
		}
		virtual										~CObject() {}


		// == Enumerations.
		// The built-in types.
		enum EE_BUILT_IN_TYPES : uint32_t {
			EE_BIT_STRING_REF						= (1 << 0),						// CStringRef
			EE_BIT_STRING							= (1 << 1),						// CString
			EE_BIT_VECTOR							= (1 << 2),						// CVector
		};

		// Extended array access flags.
		enum EE_ARRAY_EX_FLAGS : uint32_t {
			EE_AEF_START							= (1 << 0),						// _i64Idx0 is valid.
			EE_AEF_END								= (1 << 1),						// _i64Idx1 is valid.
		};


		// == Functions.
		// Gets the object type.
		virtual uint32_t							Type() const = 0 { return 0; }

		// Creates a string representation of the object, with the string usually assumed to be in UTF-8 format.
		virtual bool								ToString( std::string &_sString ) = 0 {
			_sString = "<null>";
			return false;
		}

		// Creates a formatted string representation of the object.
		virtual std::string							FormattedString( const std::string &_sFormat ) = 0 {
			return std::string( "<null>" );
		}

		// Converts to another object of the given type.
		virtual CExpEvalContainer::EE_RESULT		ConvertTo( EE_NUM_CONSTANTS _ncType ) = 0 {
			CExpEvalContainer::EE_RESULT rRet = { EE_NC_INVALID };
			return rRet;
		}

		// Initializes this object given another object.
		virtual bool								InitializeFrom( const CExpEvalContainer::EE_RESULT &_rObj ) = 0 {
			return false;
		}

		// Creates a Result from this object.
		CExpEvalContainer::EE_RESULT				CreateResult() {
			CExpEvalContainer::EE_RESULT rMe = { EE_NC_OBJECT };
			rMe.u.poObj = this;
			return rMe;
		}

		// Creates a Result from this object.
		CExpEvalContainer::EE_RESULT				CreateResult() const {
			CExpEvalContainer::EE_RESULT rMe = { EE_NC_OBJECT };
			rMe.u.poObj = const_cast<CObject *>(this);
			return rMe;
		}

		// Array access.
		virtual CExpEvalContainer::EE_RESULT		ArrayAccess( int64_t _i64Idx ) = 0 {
			return { EE_NC_OBJECT };
		}

		// Extended array access.
		virtual CExpEvalContainer::EE_RESULT		ArrayAccessEx( int64_t _i64Idx0, int64_t _i64Idx1, uint32_t _ui32Mask ) = 0 {
			return { EE_NC_OBJECT };
		}

		// Gets the array length.
		virtual size_t								ArrayLength() { return 1; }

		// Operator ==.
		virtual bool								Equals( CExpEvalContainer::EE_RESULT &_rRet ) { return false; }

		// Operator +.
		virtual CExpEvalContainer::EE_RESULT		Plus( CExpEvalContainer::EE_RESULT &_rRet ) { return { EE_NC_INVALID }; }

		// Operator -.
		virtual CExpEvalContainer::EE_RESULT		Minus( CExpEvalContainer::EE_RESULT &_rRet ) { return { EE_NC_INVALID }; }

		// Operator *.
		virtual CExpEvalContainer::EE_RESULT		Multiply( CExpEvalContainer::EE_RESULT &_rRet ) { return { EE_NC_INVALID }; }

		// Operator /.
		virtual CExpEvalContainer::EE_RESULT		Divide( CExpEvalContainer::EE_RESULT &_rRet ) { return { EE_NC_INVALID }; }

		// Operator +=.
		virtual bool								PlusEquals( CExpEvalContainer::EE_RESULT &_rRet ) { return false; }

		// Operator -=.
		virtual bool								MinusEquals( CExpEvalContainer::EE_RESULT &_rRet ) { return false; }

		// Operator *=.
		virtual bool								TimesEquals( CExpEvalContainer::EE_RESULT &_rRet ) { return false; }

		// Operator /=.
		virtual bool								DivideEquals( CExpEvalContainer::EE_RESULT &_rRet ) { return false; }

		// Operator %=.
		virtual bool								ModEquals( CExpEvalContainer::EE_RESULT &_rRet ) { return false; }

		// Operator <<=.
		virtual bool								ShiftLeftEquals( CExpEvalContainer::EE_RESULT &_rRet ) { return false; }

		// Operator >>=.
		virtual bool								ShiftRightEquals( CExpEvalContainer::EE_RESULT &_rRet ) { return false; }

		// Operator ^=.
		virtual bool								CarrotEquals( CExpEvalContainer::EE_RESULT &_rRet ) { return false; }

		// Operator |=.
		virtual bool								OrEquals( CExpEvalContainer::EE_RESULT &_rRet ) { return false; }

		// Operator &=.
		virtual bool								AndEquals( CExpEvalContainer::EE_RESULT &_rRet ) { return false; }


	protected :
		// == Members.
		CExpEvalContainer *							m_peecContainer;
	};

}	// namespace ee
