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
		std::unique_ptr<CPreProcLexer> ppplLexer = std::make_unique<CPreProcLexer>( &isStream, nullptr );
		if ( !ppplLexer ) {
			return EE_E_ERROR;
		}
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
		EE_MACROS &_mMacros ) {

		bool bErrored = false;
		std::string sTmp = _sFile;
		ee::CExpEval::RemoveComments<std::string>( ee::CExpEval::RemoveChar( sTmp, '\r' ) );
		std::vector<std::string> vLines = ee::CExpEval::Tokenize<std::string>( sTmp, '\n', true, &bErrored );
		if ( bErrored ) {
			// Tokenize() can only fail due to memory issues.
			return EE_E_OUT_OF_MEMORY;
		}
		ee::CExpEval::MergeBackslashedLines( vLines );

		std::string sDirective, sParms;
		std::vector<EE_CLEAR_STATE> vClearStates;
		size_t sIfDepth = 0;
		for ( size_t I = 0; I < vLines.size(); ++I ) {
			if ( ee::CExpEval::PreprocessingDirectives( vLines[I], sDirective, sParms ) ) {
				if ( sDirective == "if" ) {
					vClearStates.push_back( EE_CS_NONE );
					CPreProc ppPreProc;
					CPreProc::EE_ERRORS eError = ppPreProc.Parse( sParms );
					if ( eError == CPreProc::EE_E_ERROR ) { return CPreProc::EE_E_SYNTAX_IF; }
					bool bRes;
					if ( !ppPreProc.GetResult( _mMacros, bRes ) ) { return CPreProc::EE_E_SYNTAX_IF; }

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

					vClearStates[sIfDepth] = (_mMacros.find( pdThis ) == _mMacros.end()) ? EE_CS_CLEAR : EE_CS_NONE;
					vLines[I].clear();
					++sIfDepth;
					continue;
				}
				else if ( sDirective == "ifndef" ) {
					vClearStates.push_back( EE_CS_NONE );
					CPreProc::EE_PREPROC_DEFINE pdThis;
					pdThis.sName = sParms;

					vClearStates[sIfDepth] = (_mMacros.find( pdThis ) != _mMacros.end()) ? EE_CS_CLEAR : EE_CS_NONE;
					vLines[I].clear();
					++sIfDepth;
					continue;
				}
				else if ( sDirective == "else" ) {
					if ( sIfDepth == 0 ) { return CPreProc::EE_E_SYNTAX_ELSE; }
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
					if ( sIfDepth == 0 ) { return CPreProc::EE_E_SYNTAX_ELIF; }
					vLines[I].clear();
					if ( vClearStates[sIfDepth-1] == EE_CS_NONE ) {
						// Previous #if was true, so scan to the matching #endif.
						vClearStates[sIfDepth-1] = EE_CS_SEEKING_ENDIF;
					}
					else if ( vClearStates[sIfDepth-1] == EE_CS_CLEAR ) {
						CPreProc ppPreProc;
						CPreProc::EE_ERRORS eError = ppPreProc.Parse( sParms );
						if ( eError == CPreProc::EE_E_ERROR ) { return CPreProc::EE_E_SYNTAX_ELIF; }
						bool bRes;
						if ( !ppPreProc.GetResult( _mMacros, bRes ) ) { return CPreProc::EE_E_SYNTAX_ELIF; }

						vClearStates[sIfDepth-1] = !bRes ? EE_CS_CLEAR : EE_CS_NONE;
					}
					continue;
				}
				else if ( sDirective == "endif" ) {
					vLines[I].clear();
					if ( !vClearStates.size() ) { return CPreProc::EE_E_UNMATCHED_ENDIF; }
					--sIfDepth;
					vClearStates.pop_back();
					continue;
				}

				if ( (std::find( vClearStates.begin(), vClearStates.end(), EE_CS_CLEAR ) != vClearStates.end()) ||
					(std::find( vClearStates.begin(), vClearStates.end(), EE_CS_SEEKING_ENDIF ) != vClearStates.end()) ) {
					vLines[I].clear();
				}
				else if ( sDirective == "define" ) {
					CPreProc::EE_ERRORS eDefine = ParseDefine( sParms, _mMacros );
					if ( eDefine != EE_E_SUCCESS ) { return eDefine; }
					vLines[I].clear();
				}
				else if ( sDirective == "undef" ) {
					CPreProc::EE_PREPROC_DEFINE pdThis;
					if ( !ee::CExpEval::IsIdentifier( sParms ) ) { return EE_E_SYNTAX_UNDEF; }
					pdThis.sName = sParms;
					auto aFound = _mMacros.find( pdThis );
					if ( aFound != _mMacros.end() ) { _mMacros.erase( aFound ); }
					vLines[I].clear();
				}
			}
			else if ( (std::find( vClearStates.begin(), vClearStates.end(), EE_CS_CLEAR ) != vClearStates.end()) ||
				(std::find( vClearStates.begin(), vClearStates.end(), EE_CS_SEEKING_ENDIF ) != vClearStates.end()) ) {
				vLines[I].clear();
			}
			else if ( !ExpandMacros( vLines[I], _mMacros ) ) {
				return EE_E_MACRO_EXPANSION;
			}
		}


		_sResult = ee::CExpEval::Reconstitute( vLines, '\n', &bErrored );
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
						//_i64Return = CStd::AtoI64( _mMacros.GetByIndex( ui32Index ).c_str() );
						_i64Return = ee::CExpEval::StoULL( aItem->second.c_str() );
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

	/**
	 * Handles a #define.
	 *
	 * \param _sLine The text after "#define".  Parsed to determin the name, parameters, and definition of a macro.
	 * \param _mMacros Set of macros to which to add the new macro.
	 * \return Returns true if the text is valid and the macro was added.
	 */
	CPreProc::EE_ERRORS CPreProc::ParseDefine( const std::string &_sLine, CPreProc::EE_MACROS &_mMacros ) {
		try {
			CPreProc::EE_PREPROC_DEFINE pdDefine;
			size_t stPos = 0;
			bool bIsStart = true;
			for ( ; stPos < _sLine.size(); ++stPos ) {
				if ( ee::CExpEval::IsWhiteSpace( _sLine[stPos] ) || _sLine[stPos] == '(' ) { break; }
				if ( !ee::CExpEval::IsIdentifier( _sLine[stPos], bIsStart ) ) { return EE_E_SYNTAX_DEFINE; }
			}
			if ( !stPos ) { return EE_E_SYNTAX_DEFINE; }
			pdDefine.sName = _sLine.substr( 0, stPos );


			// Optionally add parameters.
			if ( _sLine[stPos] == '(' ) {
				// Get parameters.
				std::string sParm;
				bIsStart = true;
				bool bFoundWs = false;
				for ( ++stPos; stPos < _sLine.size(); ++stPos ) {
					if ( ee::CExpEval::IsWhiteSpace( _sLine[stPos] ) ) {
						if ( !bIsStart ) { bFoundWs = true; }
						continue;
					}
					if ( _sLine[stPos] == ',' || _sLine[stPos] == ')' ) {
					
						pdDefine.sParms.push_back( sParm );
						sParm.clear();
						bIsStart = true;
						bFoundWs = false;
					}
					else {
						if ( !ee::CExpEval::IsIdentifier( _sLine[stPos], bIsStart ) ) { return EE_E_SYNTAX_DEFINE; }
						if ( bFoundWs ) { return EE_E_SYNTAX_DEFINE; }
						sParm.push_back( _sLine[stPos] );
					}
					if ( _sLine[stPos] == ')' ) { ++stPos; break; }
				}
			}

			// Skip to the definition.
			while ( ee::CExpEval::IsWhiteSpace( _sLine[stPos] ) ) { ++stPos; }

			_mMacros[pdDefine] = _sLine.substr( stPos );
			return EE_E_SUCCESS;
		}
		catch ( ... ) {
			return EE_E_OUT_OF_MEMORY;
		}
	}

	/**
	 * Replace all macros in a given string.
	 *
	 * \param _sString The string in which to expand macros.
	 * \param _mMacros The macro dictionary.
	 * \return Returns true if the expansion succeeds.  Otherwise fills _sError with the error string and returns false.
	 */
	bool CPreProc::ExpandMacros( std::string &_sString, const CPreProc::EE_MACROS &_mMacros ) {
		std::set<std::string> sReplaced;
		return ExpandMacros( _sString, _mMacros, sReplaced );
	}

	/**
	 * Replace all macros in a given string.
	 *
	 * \param _sString The string in which to expand macros.
	 * \param _mMacros The macro dictionary.
	 * \param _sUsedValues Values that have already been expanded.
	 * \return Returns true if the expansion succeeds.  Otherwise fills _sError with the error string and returns false.
	 */
	bool CPreProc::ExpandMacros( std::string &_sString, const CPreProc::EE_MACROS &_mMacros, const std::set<std::string> &_sUsedValues ) {
		std::string sFinal, sIdentifier;
		sFinal.reserve( _sString.size() );
		const char * pcStr = _sString.c_str();
		
		for ( size_t I = 0; I < _sString.size(); ++I ) {
			size_t sStrLen = ee::CExpEval::CodeStringLength( _sString, I );
			if ( sStrLen ) {
				sFinal.append( _sString, I, sStrLen );
				I += sStrLen;
				if ( I == _sString.size() ) { break; }
				--I;
				continue;
			}

			bool bIdentStart = true;
			sIdentifier.clear();
			while ( ee::CExpEval::IsIdentifier( pcStr[I], bIdentStart ) && I < _sString.size() ) {
				sIdentifier.push_back( pcStr[I++] );
			}
			if ( sIdentifier.size() ) {
				// An identifier was eaten.
				if ( _sUsedValues.find( sIdentifier ) == _sUsedValues.end() ) {
					// It hasn't been replaced.
					CPreProc::EE_PREPROC_DEFINE pdTestMe;
					pdTestMe.sName = std::move( sIdentifier );	// pdTestMe.sName is now the official home of the identifier.
					auto aFound = _mMacros.find( pdTestMe );
					if ( aFound != _mMacros.end() ) {
						if ( aFound->first.sParms.size() ) {
							size_t stNewPos;
							std::vector<std::string> vParms;
							if ( GetMacroParms( _sString, I, stNewPos, vParms ) ) {
								std::string sReplacement;
								if ( GetMacroReplacementString( aFound, vParms, _mMacros, _sUsedValues, sReplacement ) ) {
									sFinal.append( sReplacement );
									I = stNewPos - 1;
									continue;
								}
							}
						}
						else {
							std::string sReplacement;
							std::vector<std::string> vParms;
							if ( GetMacroReplacementString( aFound, vParms, _mMacros, _sUsedValues, sReplacement ) ) {
								sFinal.append( sReplacement );
								--I;
								continue;
							}
						}
					}
					// Not a macro or failed macro expansion.
					sFinal.append( pdTestMe.sName );
					--I;
				}
				else {
					// It was replaced so just copy it to the output.
					sFinal.append( sIdentifier );
				}
			}
			else {
				sFinal.push_back( pcStr[I] );
			}
		}

		_sString = sFinal;
		return true;
	}

	/**
	 * Gets the parameters for a function-style macro.  If no parameters are given, returns false.
	 *
	 * \param _sString The string from which to extract function-style macro parameters.
	 * \param _stPos The position at which to begin the extraction.
	 * \param _stNewPos The position after successful extraction.
	 * \param _vRet The returned parameters, as strings.
	 * \return Returns true if the parameters were extracted.  False indicates no parameters present or an invalid parameter string.
	 */
	bool CPreProc::GetMacroParms( const std::string &_sString, size_t _stPos, size_t &_stNewPos, std::vector<std::string> &_vRet ) {
		_stNewPos = _stPos;
		// Skip any whitespace.
		while ( _stNewPos < _sString.size() && ee::CExpEval::IsWhiteSpace( _sString[_stNewPos] ) ) { ++_stNewPos; }
		if ( _stNewPos == _sString.size() ) { return false; }
		// The next character must be '('.
		if ( _sString[_stNewPos] != '(' ) { return false; }


		uint32_t ui32ParDepth = 1;
		std::string sCur;
		for ( ++_stNewPos; _stNewPos < _sString.size(); ++_stNewPos ) {
			char cThis = _sString[_stNewPos];
			if ( cThis == ',' && ui32ParDepth == 1 ) {
				if ( !sCur.size() ) { return false; }
				_vRet.push_back( ee::CExpEval::TrimWhitespace( sCur ) );
				sCur.clear();
			}
			else {
				
				sCur.push_back( cThis );
				if ( cThis == '(' ) {
					++ui32ParDepth;
				}
				else if ( cThis == ')' ) {
					--ui32ParDepth;
					if ( ui32ParDepth == 0 ) {
						sCur.pop_back();
						_vRet.push_back( ee::CExpEval::TrimWhitespace( sCur ) );
						++_stNewPos;
						return true;
					}
				}
			}
		}
		// No closing ) found.
		return false;

	}

	/**
	 * Gets the replacement string for a given macro and optional parameters.  Returns false if there is an error expanding the macro.
	 *
	 * \param _iMacro Iterator of the macro to expand.
	 * \param _vParms Optional macro parameters.  If this does not match the number of parameters the macro receives, false is returned.
	 * \param _mMacros The macro dictionary.
	 * \param _sUsedValues Values that have already been expanded.  Allows recursion to be avoided.
	 * \param _sRet The returned fully expanded string.
	 * \return Returns true if there were no errors during macro expansion.
	 */
	bool CPreProc::GetMacroReplacementString( CPreProc::EE_MACROS::const_iterator &_iMacro, std::vector<std::string> &_vParms,
		const CPreProc::EE_MACROS &_mMacros, const std::set<std::string> &_sUsedValues,
		std::string &_sRet ) {
		if ( _iMacro->first.sParms.size() != _vParms.size() ) {
			return false;
		}

		// The primary replacement.
		std::string sFinal, sIdentifier;
		sFinal.reserve( _iMacro->second.size() );
		const char * pcStr = _iMacro->second.c_str();
		
		EE_TOKEN tToken = EE_T_NONE;
		char cLastChar = '\0';
		for ( size_t I = 0; I < _iMacro->second.size(); ++I ) {
			size_t sStrLen = ee::CExpEval::CodeStringLength( _iMacro->second, I );
			if ( sStrLen ) {
				sFinal.append( _iMacro->second, I, sStrLen );
				I += sStrLen;
				if ( I == _iMacro->second.size() ) { break; }
				cLastChar = '\0';
				tToken = EE_T_NONE;
				--I;
				continue;
			}

			bool bIdentStart = true;
			sIdentifier.clear();
			while ( ee::CExpEval::IsIdentifier( pcStr[I], bIdentStart ) && I < _iMacro->second.size() ) {
				sIdentifier.push_back( pcStr[I++] );
			}
			if ( sIdentifier.size() ) {
				--I;
				
				// An identifier was eaten.
				size_t stIdx = 0;
				for ( ; stIdx < _iMacro->first.sParms.size(); ++stIdx ) {
					if ( _iMacro->first.sParms[stIdx] == sIdentifier ) {
						if ( tToken == EE_T_STRINGIZE ) {
							while ( sFinal.size() && sFinal[sFinal.size()-1] != '#' ) {
								sFinal.pop_back();
							}
							sFinal.pop_back();
							sFinal.push_back( '\"' );
							if ( _vParms[stIdx].find( '"', 0 ) == std::string::npos ) {
								sFinal.append( _vParms[stIdx] );
							}
							else {
								sFinal.append( ee::CExpEval::EscapeQuotes<std::string>( _vParms[stIdx], true ) );
							}
							sFinal.push_back( '\"' );
						}
						else {
							std::set<std::string> sReplaced = _sUsedValues;
							sReplaced.insert( _iMacro->first.sName );
							std::string sTmp = _vParms[stIdx];
							if ( !ExpandMacros( sTmp, _mMacros, sReplaced ) ) { return false; }

							sFinal.append( sTmp );
						}
						break;
					}
				}
				if ( stIdx == _iMacro->first.sParms.size() ) {
					sFinal.append( sIdentifier );
				}
				tToken = EE_T_NONE;
				cLastChar = '\0';
			}
			else {
				sFinal.push_back( pcStr[I] );
				if ( pcStr[I] == '#' ) {
					tToken = (cLastChar == '#' && tToken != EE_T_CONCAT) ? EE_T_CONCAT : EE_T_STRINGIZE;
				}
				else if ( !ee::CExpEval::IsWhiteSpace( pcStr[I] ) ) {
					tToken = EE_T_NONE;
				}
				cLastChar = pcStr[I];
			}
		}
		// Handle concatenation.
		size_t stLastPos = 0;
		cLastChar = '\0';
		pcStr = sFinal.c_str();
		for ( size_t I = 0; I < sFinal.size(); ++I ) {
			size_t sStrLen = ee::CExpEval::CodeStringLength( sFinal, I );
			if ( sStrLen ) {
				_sRet.append( sFinal, I, sStrLen );
				I += sStrLen;
				if ( I == sFinal.size() ) { break; }
				cLastChar = '\0';
				tToken = EE_T_NONE;
				stLastPos = _sRet.size();
				--I;
				continue;
			}

			
			_sRet.push_back( pcStr[I] );
			if ( pcStr[I] == '#' ) {
				tToken = (cLastChar == '#' && tToken != EE_T_CONCAT) ? EE_T_CONCAT : EE_T_STRINGIZE;
				if ( tToken == EE_T_CONCAT ) {
					_sRet.erase( stLastPos );
					while ( ++I < sFinal.size() && ee::CExpEval::IsWhiteSpace( pcStr[I] ) ) {}
					--I;
					continue;
				}
			}
			else if ( !ee::CExpEval::IsWhiteSpace( pcStr[I] ) ) {
				tToken = EE_T_NONE;
				stLastPos = _sRet.size();
			}
			else if ( tToken == EE_T_STRINGIZE ) {
				// Is whitespace.
				tToken = EE_T_NONE;
				stLastPos = _sRet.size();
			}
			cLastChar = pcStr[I];
		}


		std::set<std::string> sReplaced = _sUsedValues;
		sReplaced.insert( _iMacro->first.sName );
		if ( !ExpandMacros( _sRet, _mMacros, sReplaced ) ) { return false; }
		return true;
	}

}	// namespace ee
