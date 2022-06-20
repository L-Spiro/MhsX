#pragma once

#include "../EEExpEval.h"
#include "../EEExpEvalContainer.h"
#include <vector>

namespace ee {

	class CArrayBase {
	public :
		CArrayBase( CExpEvalContainer * _peecContainer ) :
			m_peecContainer( _peecContainer ) {
		}
		virtual										~CArrayBase() {}


		// == Functions.
		// Sets the size of the array.
		virtual bool								SetSize( size_t _sNewSize ) = 0;

		// Gets the size of the array.
		virtual size_t								GetSize() const = 0;

		// Initializes the array with values interpolated between the start and end expressions.
		virtual bool								Initialize( const CExpEvalContainer::EE_RESULT &_rStart, const CExpEvalContainer::EE_RESULT &_rEnd ) = 0;

		// Reads a value.
		virtual bool								ReadValue( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) = 0;

		// Writes a value.  _rRet is updated with the actual return value, which will change if the input value is too large or of the wrong type etc.
		virtual bool								WriteValue( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) = 0;

		// Operator +=.
		virtual bool								PlusEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) = 0;

		// Operator -=.
		virtual bool								MinusEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) = 0;

		// Operator *=.
		virtual bool								TimesEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) = 0;

		// Operator /=.
		virtual bool								DivideEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) = 0;

		// Operator %=.
		virtual bool								ModEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) = 0;

		// Operator <<=.
		virtual bool								ShiftLeftEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) = 0;

		// Operator >>=.
		virtual bool								ShiftRightEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) = 0;

		// Operator ^=.
		virtual bool								CarrotEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) = 0;

		// Operator |=.
		virtual bool								OrEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) = 0;

		// Operator &=.
		virtual bool								AndEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) = 0;


	protected :
		// == Members.
		CExpEvalContainer *							m_peecContainer;
	};

}	// namespace ee
