#pragma once

#include "../LSWWin.h"
#include <EEExpEval.h>
#include <EEExpEvalLexer.h>
#include <Gen/EEExpEvalParser.h>

namespace lsw {

	class CExpression {
	public :
		CExpression();
		~CExpression();


		// == Functions.
		// Sets the string expression.  This is costly, so do it as few times as possible.
		bool							SetExpression( const char * _pcExp );

		// Resets the expression back to scratch. Removes all memory allocated for the expression evaluator.
		void							Reset();

		// Gets a pointer to the container.
		ee::CExpEvalContainer *			GetContainer() { return m_peecContainer; }

		// Gets a constant pointer to the container.
		const ee::CExpEvalContainer *	GetContainer() const { return m_peecContainer; }

		// Evaluates the expression.
		int64_t							Evaluate() const;

		// Sets whether or not to treat it as hex.
		void							SetTreatsAsHex( bool _bVal ) { m_bTreatAsHex = _bVal != false; }


	protected :
		// == Members.
		// Container.  Holds the parsed data and evaluates the syntax tree to get a result.
		ee::CExpEvalContainer *			m_peecContainer;

		bool							m_bTreatAsHex;
	};

}	// namespace lsw
