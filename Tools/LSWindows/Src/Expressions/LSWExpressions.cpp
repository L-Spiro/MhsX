#include "LSWExpression.h"
#include <sstream>

namespace lsw {

	CExpression::CExpression() :
		m_peecContainer( nullptr ) {
	}
	CExpression::~CExpression() {
		Reset();
	}

	// == Functions.
	// Sets the string expression.  This is costly, so do it as few times as possible.
	bool CExpression::SetExpression( const char * _pcExp ) {
		Reset();

		std::istringstream sStream( _pcExp );

		ee::CExpEvalLexer * peelLexer = new ee::CExpEvalLexer( &sStream );
		if ( !peelLexer ) { Reset(); return false; }

		m_peecContainer = new ee::CExpEvalContainer( peelLexer );
		if ( !m_peecContainer ) {
			delete peelLexer;
			Reset();
			return false;
		}

		 ee::CExpEvalParser * peepParser = new ee::CExpEvalParser( peelLexer, m_peecContainer );
		if ( !peepParser ) {
			delete peelLexer;
			Reset();
			return false;
		}

		int iRet = peepParser->parse();
		delete peelLexer;
		delete peepParser;

		return iRet == 0;
	}

	// Resets the expression back to scratch. Removes all memory allocated for the expression evaluator.
	void CExpression::Reset() {
		delete m_peecContainer;
		m_peecContainer = nullptr;
	}

	// Evaluates the expression.
	int64_t CExpression::Evaluate() const {
		if ( !m_peecContainer ) { return 0; }
		ee::CExpEvalContainer::EE_RESULT rRes;
		if ( !m_peecContainer->Resolve( rRes ) ) { return 0; }

		return ee::CExpEvalContainer::ConvertResult( rRes, ee::EE_NC_SIGNED ).u.i64Val;
	}

}	// namespace lsw
