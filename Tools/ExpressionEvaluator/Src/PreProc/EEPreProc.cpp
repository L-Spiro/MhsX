/**
 * Copyright L. Spiro 2014
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: Parses a single expression related to #if and #elif.
 */

#include "EEPreProc.h"
#include "EEPreProcLexer.h"
#include <algorithm>
#include <sstream>


namespace ee {

	// == Operators.
	/**
	 * Copy operator.
	 *
	 * \param _pdOther The define to copy.
	 * \return Returns this object after the copy.
	 */
	CPreProc::EE_PREPROC_DEFINE & CPreProc::EE_PREPROC_DEFINE::operator = ( const EE_PREPROC_DEFINE &_pdOther ) {
		sName = _pdOther.sName;
		sParms = _pdOther.sParms;
		return (*this);
	}

	/**
	 * Compare this object against other.  Returns true if the name of this object comes before
	 *	the name of the given object.
	 *
	 * \param _pdOther The object against which to compare this object.
	 * \return Returns true if the name of this object comes before the name of the given object.
	 */
	bool CPreProc::EE_PREPROC_DEFINE::operator < ( const EE_PREPROC_DEFINE &_pdOther ) const {
		return sName < _pdOther.sName;
	}

	/**
	 * Check for equality between this object and another.
	 *
	 * \param _pdOther The object against which to check for equality.
	 * \return Returns true if the names of both objects are the same.
	 */
	bool CPreProc::EE_PREPROC_DEFINE::operator == ( const EE_PREPROC_DEFINE &_pdOther ) const {
		return sName == _pdOther.sName;
	}

	// == Various constructors.
	CPreProc::CPreProc() {
	}

	// == Functions.
	/**
	 * Gets the container.
	 *
	 * \return Returns the container.
	 */
	CPreProcContainer & CPreProc::GetContainer() {
		return m_ppcContainer;
	}

	/**
	 * Parse an expression, returning an error code.
	 *
	 * \param _sExp The expression to parse.
	 * \return Returns an error code depending on the result of the expression.  EE_E_ERROR is returned
	 *	if the expression is bad, EE_E_SUCCESS indicates that the expression is valid.
	 */
	CPreProc::EE_ERRORS CPreProc::Parse( const std::string &_sExp ) {
		std::istringstream isStream( _sExp );

		// Streams created.  Create the parsers.
		//CPreProcLexer * ppplLexer = LSENEW CPreProcLexer( &isStream, NULL );
		std::unique_ptr<CPreProcLexer> ppplLexer = std::make_unique<CPreProcLexer>( &isStream, nullptr );
		if ( !ppplLexer ) {
			return EE_E_ERROR;
		}
		//CPreProcParser * ppppParser = LSENEW CPreProcParser( ppplLexer, &m_ppcContainer );
		std::unique_ptr<CPreProcParser> ppppParser = std::make_unique<CPreProcParser>( ppplLexer.get(), &m_ppcContainer );
		if ( !ppppParser ) {
			return EE_E_ERROR;
		}


		if ( ppppParser->parse() == 0 ) {
			// Parsed.
			return EE_E_SUCCESS;
		}
		return EE_E_ERROR;
	}

	/**
	 * Evaluate the parsed expression with the given macro dictionary.
	 *
	 * \param _mMacros The macros.
	 * \param _bRet Holds the result of the expression.
	 * \return Returns true if the expression was successfully evaluated.
	 */
	bool CPreProc::GetResult( const EE_MACROS &_mMacros, bool &_bRet ) const {
		int64_t i64Ret;
		if ( !EvalNode( m_ppcContainer.TotalNodes() - 1, i64Ret, _mMacros ) ) { return false; }
		_bRet = i64Ret != 0ULL;
		return true;
	}

	/**
	 * Fully preprocesses an in-memory file.
	 *
	 * \param _sFile The file to preprocess.
	 * \param _sResult The preprocessed result.
	 * \param _mMacros Predefined macros.
	 * \return Returns an error code indicating success or a type of failure.
	 */
	CPreProc::EE_ERRORS CPreProc::PreProcessFile( const std::string &_sFile,
		std::string &_sResult,
		const EE_MACROS &_mMacros ) {

		EE_MACROS mMacroCopy = _mMacros;

		bool bErrored = false;
		std::string sTmp = _sFile;
		RemoveCPlusPlusComments( sTmp );
		std::vector<std::string> vLines = Tokenize<std::string>( RemoveChar( sTmp, '\r' ), '\n', true, &bErrored );
		if ( bErrored ) {
			// Tokenize() can only fail due to memory issues.
			return EE_E_OUT_OF_MEMORY;
		}
		MergeBackslashedLines( vLines );

		// Remove C-style comments.
		for ( auto I = vLines.size(); I--; ) {
			RemoveCComments( vLines[I] );
		}

		std::string sDirective, sParms;
		std::vector<EE_CLEAR_STATE> vClearStates;
		size_t sIfDepth = 0;
		for ( size_t I = 0; I < vLines.size(); ++I ) {
			if ( PreprocessingDirectives( vLines[I], sDirective, sParms ) ) {
				if ( sDirective == "if" ) {
					vClearStates.push_back( EE_CS_NONE );
					CPreProc ppPreProc;
					CPreProc::EE_ERRORS eError = ppPreProc.Parse( sParms );
					if ( eError == CPreProc::EE_E_ERROR ) { return EE_E_SYNTAX_IF; }
					bool bRes;
					if ( !ppPreProc.GetResult( mMacroCopy, bRes ) ) { return EE_E_SYNTAX_IF; }

					if ( !bRes ) {
						vClearStates[sIfDepth] = EE_CS_CLEAR;
					}
					vLines[I].clear();
					++sIfDepth;
					continue;
				}
				else if ( sDirective == "ifdef" ) {
					vClearStates.push_back( EE_CS_NONE );
					CPreProc::EE_PREPROC_DEFINE pdThis;
					pdThis.sName = sParms;

					vClearStates[sIfDepth] = (mMacroCopy.find( pdThis ) == mMacroCopy.end()) ? EE_CS_CLEAR : EE_CS_NONE;
					vLines[I].clear();
					++sIfDepth;
					continue;
				}
				else if ( sDirective == "ifndef" ) {
					vClearStates.push_back( EE_CS_NONE );
					CPreProc::EE_PREPROC_DEFINE pdThis;
					pdThis.sName = sParms;

					vClearStates[sIfDepth] = (mMacroCopy.find( pdThis ) != mMacroCopy.end()) ? EE_CS_CLEAR : EE_CS_NONE;
					vLines[I].clear();
					++sIfDepth;
					continue;
				}
				else if ( sDirective == "else" ) {
					vLines[I].clear();
					if ( vClearStates[sIfDepth-1] == EE_CS_NONE ) {
						// Previous #if was true, so scan to the matching #endif.
						vClearStates[sIfDepth-1] = EE_CS_SEEKING_ENDIF;
					}
					else if ( vClearStates[sIfDepth-1] == EE_CS_CLEAR ) {
						// Previous #if was false, so this area is true.
						vClearStates[sIfDepth-1] = EE_CS_NONE;
					}
					continue;
				}
				else if ( sDirective == "elif" ) {
					vLines[I].clear();
					if ( vClearStates[sIfDepth-1] == EE_CS_NONE ) {
						// Previous #if was true, so scan to the matching #endif.
						vClearStates[sIfDepth-1] = EE_CS_SEEKING_ENDIF;
					}
					else if ( vClearStates[sIfDepth-1] == EE_CS_CLEAR ) {
						CPreProc ppPreProc;
						CPreProc::EE_ERRORS eError = ppPreProc.Parse( sParms );
						if ( eError == CPreProc::EE_E_ERROR ) { return EE_E_SYNTAX_ELIF; }
						bool bRes;
						if ( !ppPreProc.GetResult( _mMacros, bRes ) ) { return EE_E_SYNTAX_ELIF; }

						if ( !bRes ) {
							vClearStates[sIfDepth-1] = EE_CS_CLEAR;
						}
						else {
							vClearStates[sIfDepth-1] = EE_CS_NONE;
						}
					}
					continue;
				}
				else if ( sDirective == "endif" ) {
					vLines[I].clear();
					--sIfDepth;
					vClearStates.pop_back();
					continue;
				}

				if ( (std::find( vClearStates.begin(), vClearStates.end(), EE_CS_CLEAR ) != vClearStates.end()) ||
					(std::find( vClearStates.begin(), vClearStates.end(), EE_CS_SEEKING_ENDIF ) != vClearStates.end()) ) {
					vLines[I].clear();
				}
			}
			else if ( (std::find( vClearStates.begin(), vClearStates.end(), EE_CS_CLEAR ) != vClearStates.end()) ||
				(std::find( vClearStates.begin(), vClearStates.end(), EE_CS_SEEKING_ENDIF ) != vClearStates.end()) ) {
				vLines[I].clear();
			}
		}


		_sResult = Reconstitute( vLines, '\n', &bErrored );
		if ( bErrored ) {
			// Reconstitute() can only fail due to memory issues.
			return EE_E_OUT_OF_MEMORY;
		}
		return EE_E_SUCCESS;
	}

	/**
	 * Process a single node and return the result of that node.
	 *
	 * \param _stIndex The index of the node to process.
	 * \param _i64Return Holds the return value of the node.
	 * \param _mMacros The macros.
	 * \return Returns true if the node was evaluated.
	 */
	bool CPreProc::EvalNode( size_t _stIndex, int64_t &_i64Return, const EE_MACROS &_mMacros ) const {
		const EE_PREPROC_SYNTAX_NODES::EE_NODE_DATA & ndData = m_ppcContainer.GetNode( _stIndex );
		switch ( ndData.ppnNodeType ) {
			case EE_PPN_NUMERICCONSTANT : {
				_i64Return = ndData.nuNodeData.i64Const;
				return true;
			}
			case EE_PPN_IDENTIFIER : {
				// Should be in the macro dictionary.
				EE_PREPROC_DEFINE pdThis;
				pdThis.sName = m_ppcContainer.GetString( ndData.nuNodeData.ui32IdentifierIndex );
				//uint32_t ui32Index;
				auto aItem = _mMacros.find( pdThis );
				//if ( !_mMacros.ItemExists( pdThis, ui32Index ) ) {
				if ( aItem == _mMacros.end() ) {
					_i64Return = 0LL;
				}
				else {
					//if ( _mMacros.GetKeyByIndex( ui32Index ).sParms.Length() ) {
					if ( aItem->first.sParms.size() ) {
						_i64Return = 0LL;
					}
					//else if ( _mMacros.GetByIndex( ui32Index ).Length() == 0 ) {
					else if ( aItem->second.size() == 0 ) {
						return false;
					}
					else {
						//_i64Return = CStd::AtoI64( _mMacros.GetByIndex( ui32Index ).CStr() );
						_i64Return = StoULL( aItem->second.c_str() );
					}
				}
				return true;
			}
			case EE_PPN_UNARY : {
				if ( !EvalNode( ndData.nueNodeDataEx.stNodeIndexEx, _i64Return, _mMacros ) ) { return false; }
				switch ( ndData.nuNodeData.ui32UnaryOp ) {
					case '!' : {
						_i64Return = !_i64Return;
						break;
					}
					case '~' : {
						_i64Return = ~_i64Return;
						break;
					}
					case '-' : {
						_i64Return = -_i64Return;
						break;
					}
					case '+' : {
						_i64Return = +_i64Return;
						break;
					}
				}

				return true;
			}
			case EE_PPN_MATH : {
				int64_t i64Left, i64Right;
				if ( !EvalNode( ndData.nuNodeData.stNodeIndex, i64Left, _mMacros ) ) { return false; }
				if ( !EvalNode( ndData.nueNodeDataEx.stNodeIndexEx, i64Right, _mMacros ) ) { return false; }
#define EE_HELP_ME_HERE( CASE, OP )		case CASE : { _i64Return = i64Left OP i64Right; break; }
				switch ( ndData.nuoOp.ui32Op ) {
					EE_HELP_ME_HERE( '*', * )
					EE_HELP_ME_HERE( '/', / )
					EE_HELP_ME_HERE( '%', % )
					EE_HELP_ME_HERE( '+', + )
					EE_HELP_ME_HERE( '-', - )
					EE_HELP_ME_HERE( CPreProcParser::token::EE_PPT_SHIFT_RIGHT, >> )
					EE_HELP_ME_HERE( CPreProcParser::token::EE_PPT_SHIFT_LEFT, << )
					EE_HELP_ME_HERE( '>', > )
					EE_HELP_ME_HERE( '<', < )
					EE_HELP_ME_HERE( CPreProcParser::token::EE_PPT_GE, >= )
					EE_HELP_ME_HERE( CPreProcParser::token::EE_PPT_LE, <= )
					EE_HELP_ME_HERE( CPreProcParser::token::EE_PPT_EQUALS, == )
					EE_HELP_ME_HERE( CPreProcParser::token::EE_PPT_NOT_EQUALS, != )
					EE_HELP_ME_HERE( '|', | )
					EE_HELP_ME_HERE( '&', & )
					EE_HELP_ME_HERE( '^', ^ )
					EE_HELP_ME_HERE( CPreProcParser::token::EE_PPT_AND, && )
					EE_HELP_ME_HERE( CPreProcParser::token::EE_PPT_OR, || )
					default : { return false; }
				}
				return true;
			}
			case EE_PPN_TERTIARY : {
				// Condition.
				if ( !EvalNode( ndData.nuNodeData.stNodeIndex, _i64Return, _mMacros ) ) { return false; }
				if ( _i64Return ) {
					// Left.
					return EvalNode( ndData.nueNodeDataEx.stNodeIndexEx, _i64Return, _mMacros );
				}
				else {
					return EvalNode( ndData.nueNodeDataEx.stNodeIndexEx, _i64Return, _mMacros );
				}
			}
			case EE_PPN_DEFINED : {
				// Should be in the macro dictionary.
				EE_PREPROC_DEFINE pdThis;
				const EE_PREPROC_SYNTAX_NODES::EE_NODE_DATA & ndString = m_ppcContainer.GetNode( ndData.nuNodeData.stNodeIndex );
				pdThis.sName = m_ppcContainer.GetString( ndString.nuNodeData.ui32IdentifierIndex );
				//uint32_t ui32Index;
				auto aItem = _mMacros.find( pdThis );
				//if ( !_mMacros.ItemExists( pdThis, ui32Index ) ) {
				if ( aItem == _mMacros.end() ) {
					_i64Return = 0LL;
				}
				else {
					_i64Return = 1LL;
				}
				return true;
			}
			case EE_PPN_TU : { return false; }
		}
		return false;
	}

}	// namespace ee
