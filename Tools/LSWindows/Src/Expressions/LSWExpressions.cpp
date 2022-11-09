#include "LSWExpression.h"
#include <PreProc/EEPreProc.h>
#include <sstream>

namespace lsw {

	CExpression::CExpression() :
		m_peecContainer( nullptr ),
		m_bTreatAsHex( nullptr ) {
	}
	CExpression::~CExpression() {
		Reset();
	}

	// == Functions.
	// Sets the string expression.  This is costly, so do it as few times as possible.
	bool CExpression::SetExpression( const char * _pcExp ) {
		Reset();

		std::string sPreProc;
		ee::CPreProc::EE_MACROS mMacros;
		ee::CPreProc::EE_ERRORS ePreProcError = ee::CPreProc::PreProcessFile( _pcExp,
			sPreProc,
			mMacros );

		std::istringstream sStream( sPreProc );

		ee::CExpEvalLexer * peelLexer = new( std::nothrow ) ee::CExpEvalLexer( &sStream );
		if ( !peelLexer ) { Reset(); return false; }

		m_peecContainer = new( std::nothrow ) ee::CExpEvalContainer( peelLexer );
		if ( !m_peecContainer ) {
			delete peelLexer;
			Reset();
			return false;
		}

		 ee::CExpEvalParser * peepParser = new( std::nothrow ) ee::CExpEvalParser( peelLexer, m_peecContainer );
		if ( !peepParser ) {
			delete peelLexer;
			Reset();
			return false;
		}
		peelLexer->SetContainer( (*m_peecContainer) );
		m_peecContainer->SetTreatAsHex( m_bTreatAsHex );
		int iRet = 1;
		try {
			iRet = peepParser->parse();
		}
		catch ( const std::bad_alloc & /*_eE*/ ) {}
		catch ( ee::CExpEvalContainer::EE_ERROR_CODES ) {}
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

		return m_peecContainer->ConvertResultOrObject( rRes, ee::EE_NC_SIGNED ).u.i64Val;
	}

}	// namespace lsw
