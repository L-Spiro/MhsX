#include "MXSearcher.h"
#include "../RamBuffer/MXRamBuffer.h"
#include "../RegEx/MXOnigurumaRegex.h"
#include "../RegEx/MXOnigurumaRegion.h"
#include "../RegEx/MXOnigurumaSystem.h"
#include "../System/MXSystem.h"
#include "MXSearchResult1.h"
#include "MXSearchResult2.h"
#include "MXSearchResultLocker.h"
#include "MXSearchResultRef.h"

#include <algorithm>
#include <Expressions/LSWExpression.h>


//#define MX_SINGLE_THREAD_SEARCH
//#define MX_GATHER_THREAD_PRIORITY				THREAD_PRIORITY_NORMAL

#define MX_THREADED_GATHER( SEARCHRESULTTYPE )																															\
	for ( size_t I = 0; I < stMax; ++I ) {																																\
		{																																								\
			if ( aThreadStruct[I].tThread.Id() != 0 ) { aThreadStruct[I].tThread.WaitFor( INFINITY ); }																	\
		}																																								\
		while ( aThreadStruct[I].pstSearchThreadParms.i32Result == 0 ) { ::Sleep( 1 ); }																				\
		if ( aThreadStruct[I].pstSearchThreadParms.i32Result != 1 ) { bAllPass = false; }																				\
																																										\
		if ( bAllPass && !static_cast<SEARCHRESULTTYPE *>(m_psrbCurTmpSearchResults)->AppendResults( aThreadStruct[I].srResults ) ) { bAllPass = false; }				\
		aThreadStruct[I].srResults.Reset();																																\
	}


namespace mx {

	// == Functions.
	// Fill a list of chunks.
	bool CSearcher::GenerateChunks( CAddressChunkList &_aclList, DWORD _dwRegionMask,
		uint64_t _ui64MaxLen, uint64_t _ui64Start, uint64_t _ui64End ) {
		if ( !_ui64MaxLen ) { return false; }
		uint64_t aStart = _ui64Start;
		MEMORY_BASIC_INFORMATION64 mbiInfo;
		uint64_t ui64TotalAreaAdded = 0;
		LSW_ENT_CRIT( m_csCrit );
		while ( aStart < _ui64End ) {
			if ( !m_ppProcess->VirtualQueryEx( aStart, &mbiInfo ) ) { break; }
			if ( mbiInfo.BaseAddress > _ui64End ) { break; }

			// Clamp start.
			if ( mbiInfo.BaseAddress < aStart ) {
				uint64_t ui64Dif = aStart - mbiInfo.BaseAddress;
				mbiInfo.BaseAddress += ui64Dif;
				mbiInfo.RegionSize -= ui64Dif;
			}
			// Clamp end.
			if ( mbiInfo.BaseAddress + mbiInfo.RegionSize > _ui64End ) {
				uint64_t ui64Dif = (mbiInfo.BaseAddress + mbiInfo.RegionSize) - _ui64End;
				mbiInfo.RegionSize -= ui64Dif;
			}

			// Move the cursor to the next address.
			aStart += mbiInfo.RegionSize;
			if ( _dwRegionMask != UINT32_MAX ) {
				if ( mbiInfo.State & MEM_FREE ) { continue; }
				if ( mbiInfo.State & MEM_RESERVE ) { continue; }
				if ( mbiInfo.Protect & PAGE_NOACCESS ) { continue; }
				if ( !(_dwRegionMask & mbiInfo.Type) ) { continue; }
			}

			// Add the current chunk in separate chunks, each a maximum of _ui64MaxLen bytes.
			while ( mbiInfo.RegionSize ) {
				uint64_t ui64Size = std::min( mbiInfo.RegionSize, _ui64MaxLen );
				_aclList.Add( mbiInfo.BaseAddress, ui64Size );
				mbiInfo.BaseAddress += ui64Size;
				ui64TotalAreaAdded += ui64Size;
				mbiInfo.RegionSize -= ui64Size;
			}

		}

		return true;
	}

	// Searches the target process.
	bool CSearcher::Search( const MX_SEARCH_PARMS &_spParms, HWND _hProgressUpdate, uint64_t * _pui64TotalFound ) {
		uint64_t ui64SearchTimeStart = CUtilities::CurTimeInMicros();

		MX_SEARCH_PARMS spParmsCopy = _spParms;
		spParmsCopy.ui64AddressFrom = std::min( _spParms.ui64AddressFrom, _spParms.ui64AddressTo );
		spParmsCopy.ui64AddressTo = std::max( _spParms.ui64AddressFrom, _spParms.ui64AddressTo );

		CAddressChunkList aclChunks;
		if ( !GenerateChunks( aclChunks, _spParms.dwRegions, _spParms.ui64MaxChunkLen, spParmsCopy.ui64AddressFrom, spParmsCopy.ui64AddressTo ) ) {
			return false;
		}

		bool bRet = false;
		{
			lsw::LSW_THREAD_PRIORITY tpThreadPri( spParmsCopy.iThreadPriority );
			switch ( _spParms.stType ) {
				case CUtilities::MX_ST_DATATYPE_SEARCH : {
					switch ( _spParms.i32EvalType ) {
						case CUtilities::MX_ET_EXACT : {
							bRet = ExactValuePrimitiveSearch( spParmsCopy, aclChunks, _hProgressUpdate );
							break;
						}
						case CUtilities::MX_ET_GREATER_THAN : {
							bRet = GreaterThanPrimitiveSearch( spParmsCopy, aclChunks, _hProgressUpdate );
							break;
						}
						case CUtilities::MX_ET_LESS_THAN : {
							bRet = LessThanPrimitiveSearch( spParmsCopy, aclChunks, _hProgressUpdate );
							break;
						}
						case CUtilities::MX_ET_RANGE : {
							if ( CUtilities::DataTypeIsFloat( spParmsCopy.dtLVal.dtType ) ) {
								switch ( spParmsCopy.dtLVal.dtType ) {
									case CUtilities::MX_DT_FLOAT : {
										spParmsCopy.dtLVal.u.Float32 = std::min( _spParms.dtLVal.u.Float32, _spParms.dtRVal.u.Float32 );
										spParmsCopy.dtRVal.u.Float32 = std::max( _spParms.dtLVal.u.Float32, _spParms.dtRVal.u.Float32 );
										break;
									}
									case CUtilities::MX_DT_FLOAT16 : {}
									case CUtilities::MX_DT_DOUBLE : {
										spParmsCopy.dtLVal.u.Float64 = std::min( _spParms.dtLVal.u.Float64, _spParms.dtRVal.u.Float64 );
										spParmsCopy.dtRVal.u.Float64 = std::max( _spParms.dtLVal.u.Float64, _spParms.dtRVal.u.Float64 );
										break;
									}
									default : { return false; }
								}
							}
							else if ( CUtilities::DataTypeIsSigned( spParmsCopy.dtLVal.dtType ) ) {
								spParmsCopy.dtLVal.u.Int64 = std::min( _spParms.dtLVal.u.Int64, _spParms.dtRVal.u.Int64 );
								spParmsCopy.dtRVal.u.Int64 = std::max( _spParms.dtLVal.u.Int64, _spParms.dtRVal.u.Int64 );
							}
							else {
								spParmsCopy.dtLVal.u.UInt64 = std::min( _spParms.dtLVal.u.UInt64, _spParms.dtRVal.u.UInt64 );
								spParmsCopy.dtRVal.u.UInt64 = std::max( _spParms.dtLVal.u.UInt64, _spParms.dtRVal.u.UInt64 );
							}
							bRet = InRangePrimitiveSearch( spParmsCopy, aclChunks, _hProgressUpdate );
							break;
						}
						case CUtilities::MX_ET_UNKNOWN : {
							bRet = UnknownPrimitiveSearch( spParmsCopy, aclChunks, _hProgressUpdate );
							break;
						}
						case CUtilities::MX_ET_QUICK_EXP : {
							// ================
							// == Alignment. ==
							// ================
							if ( !PrimitiveAlignment( spParmsCopy, spParmsCopy.dtLVal.dtType, m_ppProcess ) ) { return false; }
							// ================
							// ================
							switch ( _spParms.dtLVal.dtType ) {
								case CUtilities::MX_DT_INT8 : {
									bRet = ExpressionSearchPredefinedType<int8_t, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_UINT8 : {
									bRet = ExpressionSearchPredefinedType<uint8_t, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_INT16 : {
									bRet = ExpressionSearchPredefinedType<int16_t, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_UINT16 : {
									bRet = ExpressionSearchPredefinedType<uint16_t, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_INT32 : {
									bRet = ExpressionSearchPredefinedType<int32_t, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_UINT32 : {
									bRet = ExpressionSearchPredefinedType<uint32_t, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_INT64 : {
									bRet = ExpressionSearchPredefinedType<int64_t, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_UINT64 : {
									bRet = ExpressionSearchPredefinedType<uint64_t, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_FLOAT : {
									bRet = ExpressionSearchPredefinedType<float, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_DOUBLE : {
									bRet = ExpressionSearchPredefinedType<double, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_FLOAT16 : {
									bRet = ExpressionSearchPredefinedType<uint16_t, true>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
							}
							break;
						}
						default : { return false; }
					}	// switch ( _spParms.i32EvalType )
					break;
				}
				case CUtilities::MX_ST_STRING_SEARCH : {
					switch ( _spParms.sstSearchType ) {
						case CUtilities::MX_SST_ASCII : {
							bRet = NormalStringSearch<int8_t>( spParmsCopy, aclChunks, _hProgressUpdate );
							break;
						}
						case CUtilities::MX_SST_UTF8 : {
							bRet = UtfAnyStringSearch<uint8_t>( spParmsCopy, aclChunks, _hProgressUpdate );
							break;
						}
						case CUtilities::MX_SST_UTF16_BE : {}
						case CUtilities::MX_SST_UTF16 : {
							bRet = UtfAnyStringSearch<uint16_t>( spParmsCopy, aclChunks, _hProgressUpdate );
							break;
						}
						case CUtilities::MX_SST_UTF32_BE : {}
						case CUtilities::MX_SST_UTF32 : {
							bRet = UtfAnyStringSearch<uint32_t>( spParmsCopy, aclChunks, _hProgressUpdate );
							break;
						}
						case CUtilities::MX_SST_UTFALL : {
							bRet = UtfAllStringSearch( spParmsCopy, aclChunks, _hProgressUpdate );
							break;
						}
						case CUtilities::MX_SST_RAW : {
							switch ( _spParms.dtStringCharType ) {
								case CUtilities::MX_DT_INT8 : {
									bRet = RawArraySearch<int8_t>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_UINT8 : {
									bRet = RawArraySearch<uint8_t>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_INT16 : {
									bRet = RawArraySearch<int16_t>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_UINT16 : {
									bRet = RawArraySearch<uint16_t>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_INT32 : {
									bRet = RawArraySearch<int32_t>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_UINT32 : {
									bRet = RawArraySearch<uint32_t>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_INT64 : {
									bRet = RawArraySearch<int64_t>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_UINT64 : {
									bRet = RawArraySearch<uint64_t>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_FLOAT16 : {
									bRet = RawArraySearch<uint16_t>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_FLOAT : {
									bRet = RawArraySearch<float>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_DOUBLE : {
									bRet = RawArraySearch<double>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_VOID : {
									bRet = RawArrayMultiType( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								default : { return false; }
							}
							break;
						}
						case CUtilities::MX_SST_REGEX : {
							bRet = RegexSearch( spParmsCopy, aclChunks, _hProgressUpdate );
							break;
						}
						default : { return false; }
					}
					break;
				}
				case CUtilities::MX_ST_EXP_SEARCH : {
					switch ( _spParms.esqmExpSearchType ) {
						case CUtilities::MX_ESQM_ADDRESS : {
							if ( spParmsCopy.ui32ExpSearchSize ) {
								bRet = ExpressionSearchAddressFixedSize( spParmsCopy, aclChunks, _hProgressUpdate );
							}
							else {
								bRet = ExpressionSearchAddressDynamicSize( spParmsCopy, aclChunks, _hProgressUpdate );
							}
							break;
						}
						case CUtilities::MX_ESQM_VALUE : {
							switch ( _spParms.dtLVal.dtType ) {
								case CUtilities::MX_DT_INT8 : {
									bRet = ExpressionSearchPredefinedType<int8_t, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_UINT8 : {
									bRet = ExpressionSearchPredefinedType<uint8_t, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_INT16 : {
									bRet = ExpressionSearchPredefinedType<int16_t, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_UINT16 : {
									bRet = ExpressionSearchPredefinedType<uint16_t, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_INT32 : {
									bRet = ExpressionSearchPredefinedType<int32_t, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_UINT32 : {
									bRet = ExpressionSearchPredefinedType<uint32_t, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_INT64 : {
									bRet = ExpressionSearchPredefinedType<int64_t, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_UINT64 : {
									bRet = ExpressionSearchPredefinedType<uint64_t, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_FLOAT : {
									bRet = ExpressionSearchPredefinedType<float, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_DOUBLE : {
									bRet = ExpressionSearchPredefinedType<double, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_FLOAT16 : {
									bRet = ExpressionSearchPredefinedType<uint16_t, true>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
							}
							break;
						}
					}
					break;
				}
				default : { return false; }
			}	// switch ( _spParms.stType )
		}



		if ( bRet ) {
			// Keep the new search results only if there are more than 0 returns.
			m_psrbCurTmpSearchResults->Finalize();
			if ( _pui64TotalFound ) {
				(*_pui64TotalFound) = m_psrbCurTmpSearchResults->TotalResults();
			}
			if ( m_psrbCurTmpSearchResults->TotalResults() ) {
				LSW_ENT_CRIT( m_csCrit );
				CSearchResultBase * srbDeleteMe = m_psrbSearchResults;
				m_psrbSearchResults = m_psrbCurTmpSearchResults;
				m_psrbCurTmpSearchResults = nullptr;
				delete srbDeleteMe;

				m_bLastSearchWasSubsearch = false;
				m_ui64LastSearchTotal = 0;

				m_spLastSearchParms = spParmsCopy;
				m_ui64LastSearchTime = CUtilities::CurTimeInMicros() - ui64SearchTimeStart;
			}
			
		}


		// Delete the temporary results if any are hanging around.
		delete m_psrbCurTmpSearchResults;
		m_psrbCurTmpSearchResults = nullptr;
		return bRet;
	}

	bool CSearcher::Subsearch( const MX_SUBSEARCH_PARMS &_spParms, HWND _hProgressUpdate, uint64_t * _pui64TotalFound ) {
		uint64_t ui64SearchTimeStart = CUtilities::CurTimeInMicros();

		MX_SUBSEARCH_PARMS spParmsCopy = _spParms;

		CAddressChunkList aclChunks;
		uint64_t ui64From, ui64To;
		if ( !m_psrbSearchResults->GetEncapsulatingAddresses( ui64From, ui64To ) ) { return false; }
		if ( !GenerateChunks( aclChunks, UINT32_MAX, INT64_MAX, ui64From, ui64To ) ) { return false; }

		bool bRet = false;
		{
			lsw::LSW_THREAD_PRIORITY tpThreadPri( spParmsCopy.iThreadPriority );
			switch ( spParmsCopy.stInitialType ) {
				case CUtilities::MX_ST_DATATYPE_SEARCH : {
					switch ( spParmsCopy.i32EvalType ) {
						case CUtilities::MX_SET_EXACT : {
							bRet = ExactValuePrimitiveSubsearch( spParmsCopy, aclChunks, _hProgressUpdate );
							break;
						}
						case CUtilities::MX_SET_NOT_EQUAL_TO : {
							bRet = NotEqualToPrimitiveSubsearch( spParmsCopy, aclChunks, _hProgressUpdate );
							break;
						}
						case CUtilities::MX_SET_INCREASED : {
							bRet = IncreasedPrimitiveSubsearch( spParmsCopy, aclChunks, _hProgressUpdate );
							break;
						}
						case CUtilities::MX_SET_DECREASED : {
							bRet = DecreasedPrimitiveSubsearch( spParmsCopy, aclChunks, _hProgressUpdate );
							break;
						}
						case CUtilities::MX_SET_RANGE : {
							if ( CUtilities::DataTypeIsFloat( spParmsCopy.dtLVal.dtType ) ) {
								switch ( spParmsCopy.dtLVal.dtType ) {
									case CUtilities::MX_DT_FLOAT : {
										spParmsCopy.dtLVal.u.Float32 = std::min( _spParms.dtLVal.u.Float32, _spParms.dtRVal.u.Float32 );
										spParmsCopy.dtRVal.u.Float32 = std::max( _spParms.dtLVal.u.Float32, _spParms.dtRVal.u.Float32 );
										break;
									}
									case CUtilities::MX_DT_FLOAT16 : {}
									case CUtilities::MX_DT_DOUBLE : {
										spParmsCopy.dtLVal.u.Float64 = std::min( _spParms.dtLVal.u.Float64, _spParms.dtRVal.u.Float64 );
										spParmsCopy.dtRVal.u.Float64 = std::max( _spParms.dtLVal.u.Float64, _spParms.dtRVal.u.Float64 );
										break;
									}
									default : { return false; }
								}
							}
							else if ( CUtilities::DataTypeIsSigned( spParmsCopy.dtLVal.dtType ) ) {
								spParmsCopy.dtLVal.u.Int64 = std::min( _spParms.dtLVal.u.Int64, _spParms.dtRVal.u.Int64 );
								spParmsCopy.dtRVal.u.Int64 = std::max( _spParms.dtLVal.u.Int64, _spParms.dtRVal.u.Int64 );
							}
							else {
								spParmsCopy.dtLVal.u.UInt64 = std::min( _spParms.dtLVal.u.UInt64, _spParms.dtRVal.u.UInt64 );
								spParmsCopy.dtRVal.u.UInt64 = std::max( _spParms.dtLVal.u.UInt64, _spParms.dtRVal.u.UInt64 );
							}
							bRet = InRangePrimitiveSubsearch( spParmsCopy, aclChunks, _hProgressUpdate );
							break;
						}
						case CUtilities::MX_SET_SAME_AS_BEFORE : {
							bRet = SameAsBeforePrimitiveSubsearch( spParmsCopy, aclChunks, _hProgressUpdate );
							break;
						}
						case CUtilities::MX_SET_DIFFERENT_FROM_BEFORE : {
							bRet = DifferentFromBeforePrimitiveSubsearch( spParmsCopy, aclChunks, _hProgressUpdate );
							break;
						}
						case CUtilities::MX_SET_CHANGED_BY : {
							bRet = ChangedByPrimitiveSubsearch( spParmsCopy, aclChunks, _hProgressUpdate,
								&CSearcher::GetChangedByCmpFuncAndSize );
							break;
						}
						case CUtilities::MX_SET_CHANGED_BY_PERCENT : {
							bRet = ChangedByPrimitiveSubsearch( spParmsCopy, aclChunks, _hProgressUpdate,
								&CSearcher::GetChangedByPercCmpFuncAndSize );
							break;
						}
						case CUtilities::MX_SET_CHANGED_BY_RANGE : {
							bRet = ChangedByPrimitiveSubsearch( spParmsCopy, aclChunks, _hProgressUpdate,
								&CSearcher::GetChangedByRangeCmpFuncAndSize );
							break;
						}
						case CUtilities::MX_SET_CHANGED_BY_RANGE_PERCENT : {
							bRet = ChangedByPrimitiveSubsearch( spParmsCopy, aclChunks, _hProgressUpdate,
								&CSearcher::GetChangedByRangePercCmpFuncAndSize );
							break;
						}
						case CUtilities::MX_SET_QUICK_EXP : {
							switch ( _spParms.dtLVal.dtType ) {
								case CUtilities::MX_DT_INT8 : {
									bRet = QuickExpPrimitiveSubsearch<int8_t, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_UINT8 : {
									bRet = QuickExpPrimitiveSubsearch<uint8_t, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_INT16 : {
									bRet = QuickExpPrimitiveSubsearch<int16_t, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_UINT16 : {
									bRet = QuickExpPrimitiveSubsearch<uint16_t, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_INT32 : {
									bRet = QuickExpPrimitiveSubsearch<int32_t, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_UINT32 : {
									bRet = QuickExpPrimitiveSubsearch<uint32_t, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_INT64 : {
									bRet = QuickExpPrimitiveSubsearch<int64_t, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_UINT64 : {
									bRet = QuickExpPrimitiveSubsearch<uint64_t, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_FLOAT : {
									bRet = QuickExpPrimitiveSubsearch<float, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_DOUBLE : {
									bRet = QuickExpPrimitiveSubsearch<double, false>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
								case CUtilities::MX_DT_FLOAT16 : {
									bRet = QuickExpPrimitiveSubsearch<uint16_t, true>( spParmsCopy, aclChunks, _hProgressUpdate );
									break;
								}
							}
							//bRet = QuickExpPrimitiveSubsearch( spParmsCopy, aclChunks, _hProgressUpdate );
							break;
						}
						default : { return false; }
					}
					break;
				}
				case CUtilities::MX_ST_STRING_SEARCH : {
					break;
				}
				case CUtilities::MX_ST_EXP_SEARCH : {
					break;
				}
				default : { return false; }
			}
		}



		if ( bRet ) {
			// Keep the new search results only if there are more than 0 returns.
			m_psrbCurTmpSearchResults->Finalize();
			if ( _pui64TotalFound ) {
				(*_pui64TotalFound) = m_psrbCurTmpSearchResults->TotalResults();
			}
			if ( m_psrbCurTmpSearchResults->TotalResults() ) {
				LSW_ENT_CRIT( m_csCrit );


				CSearchResultBase * psrbDeleteMe = m_psrbSearchResults;
				m_ui64LastSearchTotal = m_psrbSearchResults->TotalResults();
				m_psrbSearchResults = m_psrbCurTmpSearchResults;
				m_psrbCurTmpSearchResults = nullptr;
				delete psrbDeleteMe;

				m_bLastSearchWasSubsearch = true;

				m_spLastSubSearchParms = spParmsCopy;
				m_ui64LastSearchTime = CUtilities::CurTimeInMicros() - ui64SearchTimeStart;
			}
			
		}


		// Delete the temporary results if any are hanging around.
		delete m_psrbCurTmpSearchResults;
		m_psrbCurTmpSearchResults = nullptr;

		return bRet;
	}

	// Performs pre-processing byteswapping.
	void CSearcher::PreprocessByteSwap( void * _pvData, size_t _stLen, const MX_SEARCH_PARMS &_spParms ) {
		if ( !_stLen ) { return; }
		switch ( _spParms.bsByteSwapping ) {
			case CUtilities::MX_BS_NONE : { return; }
			case CUtilities::MX_BS_2BYTE : {
				uint16_t * puiData = reinterpret_cast<uint16_t *>(_pvData);
				for ( size_t I = _stLen >> 1; I--; ) {
					puiData[I] = ::_byteswap_ushort( puiData[I] );
				}
				return;
			}
			case CUtilities::MX_BS_4BYTE : {
				uint32_t * puiData = reinterpret_cast<uint32_t *>(_pvData);
				for ( size_t I = _stLen >> 2; I--; ) {
					puiData[I] = ::_byteswap_ulong( puiData[I] );
				}
				return;
			}
			case CUtilities::MX_BS_8BYTE : {
				uint64_t * puiData = reinterpret_cast<uint64_t *>(_pvData);
				for ( size_t I = _stLen >> 3; I--; ) {
					puiData[I] = ::_byteswap_uint64( puiData[I] );
				}
				return;
			}
		}
	}

	// Divides the results into treadable parts.
	bool CSearcher::DivideResultsForSubsearch( CSearchResultBase * _psrbResults, std::vector<MX_SUBSEARCH_DIV> &_vResult,
		uint64_t _ui64MinResults, const CAddressChunkList &_aclList ) {
		size_t sChunk = 0;
		MX_SUBSEARCH_DIV sdDiv = { 0, 0 };
		while ( true ) {
			uint64_t ui64Cur = sdDiv.ui64Start + _ui64MinResults;
			//uint64_t ui64Cur = UINT64_MAX;
			if ( ui64Cur >= _psrbResults->TotalResults() ) {
				sdDiv.ui64Total = _psrbResults->TotalResults() - sdDiv.ui64Start;
				if ( sdDiv.ui64Total ) {
					_vResult.push_back( sdDiv );
				}
				return true;
			}
			// Find the chunk in which this final address lies.
			const uint8_t * pui8Val;
			if ( !_psrbResults->GetResultFast( ui64Cur, ui64Cur, pui8Val ) ) { return false; }
			while ( sChunk < _aclList.Chunks().size() ) {
				CAddressChunkList::MX_CHUNK cThisChunk = _aclList.Chunks()[sChunk];
				if ( _aclList.Chunks()[sChunk].ui64Start <= ui64Cur && (_aclList.Chunks()[sChunk].ui64Start + _aclList.Chunks()[sChunk].ui64Len) > ui64Cur ) {
					// Include all the addresses included by this chunk.
					uint64_t ui64LastInChunk = _psrbResults->GetIndexOfFirstItemWithAddressOrAbove( (_aclList.Chunks()[sChunk].ui64Start + _aclList.Chunks()[sChunk].ui64Len) );
					if ( ui64LastInChunk == static_cast<uint64_t>(-1) ) { return false; }	// Huh?
					if ( ui64LastInChunk < sdDiv.ui64Start ) { return false; }				// What?
					
					sdDiv.ui64Total = ui64LastInChunk - sdDiv.ui64Start;
					if ( !sdDiv.ui64Total ) {
						return false;
					}
					_vResult.push_back( sdDiv );
					sdDiv.ui64Start += sdDiv.ui64Total;
					break;
				}
				
				++sChunk;
			}
			if ( sChunk == _aclList.Chunks().size() ) {
				// We somehow went past the end of the list of chunks.
				// Not really possible and this section wouldn't be searchable if it were true, so just add all remaining items.
				sdDiv.ui64Total = _psrbResults->TotalResults() - sdDiv.ui64Start;
				_vResult.push_back( sdDiv );
				return true;
			}
		}
	}

	// Exact-value search on primitives.
	bool CSearcher::ExactValuePrimitiveSearch( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate ) {
		delete m_psrbCurTmpSearchResults;
		m_psrbCurTmpSearchResults = new ( std::nothrow ) CSearchResult1();
		if ( !m_psrbCurTmpSearchResults ) { return false; }
		_spParms.bInvertResults = _spParms.bInvertResults ? true : false;	// Force to 0 or 1.


		// ==============================================
		// == Create the comparison function and size. ==
		// ==============================================
		MX_EXACT_VAL_COMP evcCompFunc = nullptr;
		uint32_t ui32Size = 0;
		if ( !GetExactValCmpFuncAndSize( evcCompFunc, ui32Size, _spParms, false ) ) { return false; }
		// ==============================================
		// ==============================================


		// ================
		// == Alignment. ==
		// ================
		if ( !PrimitiveAlignment( _spParms, _spParms.dtLVal.dtType, m_ppProcess ) ) { return false; }
		// ================
		// ================


		// ====================
		// == Do the search. ==
		// ====================
		bool bAllPass = true;
#ifndef MX_SINGLE_THREAD_SEARCH
		const size_t stThreads = 8;
		struct {
			CThread tThread;
			CSearchResult1 srResults;
			MX_PRIM_SEARCH_THREAD pstSearchThreadParms;
		} aThreadStruct[stThreads];
		LONG lProgTotal = 0;
		size_t stMax = std::min( _aclChunks.Chunks().size(), stThreads );
		for ( size_t I = 0; I < stMax; ++I ) {
			aThreadStruct[I].pstSearchThreadParms.hProgressUpdate = _hProgressUpdate;
			aThreadStruct[I].pstSearchThreadParms.paclChunks = &_aclChunks;
			aThreadStruct[I].pstSearchThreadParms.pspParms = &_spParms;
			aThreadStruct[I].pstSearchThreadParms.psrbResults = &aThreadStruct[I].srResults;
			aThreadStruct[I].pstSearchThreadParms.ppProcess = m_ppProcess;
			aThreadStruct[I].pstSearchThreadParms.evcCompFunc = evcCompFunc;
			aThreadStruct[I].pstSearchThreadParms.stStartIdx = (_aclChunks.Chunks().size() * I / stMax);
			aThreadStruct[I].pstSearchThreadParms.stEndIdx = (_aclChunks.Chunks().size() * (I + 1) / stMax);
			aThreadStruct[I].pstSearchThreadParms.ui32Size = ui32Size;
			aThreadStruct[I].pstSearchThreadParms.i32Result = 0;
			aThreadStruct[I].pstSearchThreadParms.plProgress = &lProgTotal;
			
			aThreadStruct[I].tThread.CreateThread( PrimitiveExactValueSearchThreadProc, &aThreadStruct[I].pstSearchThreadParms );
		}
		MX_THREADED_GATHER( CSearchResult1 );
#else
		std::vector<uint8_t> vBuffer;
		uint64_t uiLargestChunk = std::max( _aclChunks.LargestChunk(), static_cast<uint64_t>(ui32Size) );
		while ( !vBuffer.size() ) {
			try {
				vBuffer.resize( uiLargestChunk );
			}
			catch ( const std::bad_alloc & /*_eE*/ ) {}
			uiLargestChunk = std::max( uiLargestChunk, 1024ULL * 1024ULL ) - 1024ULL * 1024ULL;
			if ( !uiLargestChunk && !vBuffer.size() ) {
				// Unable to allocate any memory at all??  In practice this code can't be executed.
				return false;
			}
		}
		// At this point vBuffer.size() is guaranteed not to be 0.
		CSearchResultLocker srlSearchLock( m_psrbCurTmpSearchResults );
		for ( size_t I = 0; I < _aclChunks.Chunks().size(); ++I ) {
			for ( uint64_t J = 0; J < _aclChunks.Chunks()[I].ui64Len; ) {
				// Buffer as much of the chunk as possible.
				size_t sSize = std::min( _aclChunks.Chunks()[I].ui64Len - J, vBuffer.size() );
				if ( sSize < ui32Size ) { break; }
				if ( m_ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(_aclChunks.Chunks()[I].ui64Start + J), &vBuffer[0], sSize, nullptr ) ) {
					PreprocessByteSwap( vBuffer.data(), sSize, _spParms );
					// Go over the chunk and compare.
					 
					size_t sEnd = sSize - ui32Size;
					for ( uint64_t K = 0; K <= sEnd; ) {
						if ( evcCompFunc( _spParms.dtLVal, (*reinterpret_cast<CUtilities::MX_DATA_TYPE *>(&vBuffer[K])), _spParms ) ^ _spParms.bInvertResults ) {
							if ( !m_psrbCurTmpSearchResults->Add( _aclChunks.Chunks()[I].ui64Start + J + K,
								&vBuffer[K], ui32Size ) ) {
								return false;
							}
						}
						K += _spParms.ui32Alignment;
					}

					J += sSize;
				}
				else {
					break;
				}
			}

			if ( !(I & 0x3) && _hProgressUpdate ) {
				::SendNotifyMessageW( _hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>(((I + 1.0) / _aclChunks.Chunks().size()) * 100.0), 0L );
			}
			if ( _spParms.pbAbort && (*_spParms.pbAbort) ) { break; }
		}
#endif	// #ifndef MX_SINGLE_THREAD_SEARCH
		// ====================
		// ====================

		return bAllPass;
	}

	// Greater-than search on primitives.
	bool CSearcher::GreaterThanPrimitiveSearch( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate ) {
		delete m_psrbCurTmpSearchResults;
		m_psrbCurTmpSearchResults = new ( std::nothrow ) CSearchResult1();
		if ( !m_psrbCurTmpSearchResults ) { return false; }
		_spParms.bInvertResults = _spParms.bInvertResults ? true : false;	// Force to 0 or 1.


		// ==============================================
		// == Create the comparison function and size. ==
		// ==============================================
		MX_REL_COMP rcCompFunc = nullptr;
		uint32_t ui32Size = 0;
		if ( !GetGreaterThanCmpFuncAndSize( rcCompFunc, ui32Size, _spParms, false ) ) { return false; }
		// ==============================================
		// ==============================================


		// ================
		// == Alignment. ==
		// ================
		if ( !PrimitiveAlignment( _spParms, _spParms.dtLVal.dtType, m_ppProcess ) ) { return false; }
		// ================
		// ================


		// ====================
		// == Do the search. ==
		// ====================
		bool bAllPass = true;
#ifndef MX_SINGLE_THREAD_SEARCH
		const size_t stThreads = 8;
		struct {
			CThread tThread;
			CSearchResult1 srResults;
			MX_PRIM_SEARCH_THREAD pstSearchThreadParms;
		} aThreadStruct[stThreads];
		LONG lProgTotal = 0;
		size_t stMax = std::min( _aclChunks.Chunks().size(), stThreads );
		for ( size_t I = 0; I < stMax; ++I ) {
			aThreadStruct[I].pstSearchThreadParms.hProgressUpdate = _hProgressUpdate;
			aThreadStruct[I].pstSearchThreadParms.paclChunks = &_aclChunks;
			aThreadStruct[I].pstSearchThreadParms.pspParms = &_spParms;
			aThreadStruct[I].pstSearchThreadParms.psrbResults = &aThreadStruct[I].srResults;
			aThreadStruct[I].pstSearchThreadParms.ppProcess = m_ppProcess;
			aThreadStruct[I].pstSearchThreadParms.rcRelCompFunc = rcCompFunc;
			aThreadStruct[I].pstSearchThreadParms.stStartIdx = (_aclChunks.Chunks().size() * I / stMax);
			aThreadStruct[I].pstSearchThreadParms.stEndIdx = (_aclChunks.Chunks().size() * (I + 1) / stMax);
			aThreadStruct[I].pstSearchThreadParms.ui32Size = ui32Size;
			aThreadStruct[I].pstSearchThreadParms.i32Result = 0;
			aThreadStruct[I].pstSearchThreadParms.plProgress = &lProgTotal;
			
			aThreadStruct[I].tThread.CreateThread( PrimitiveRelativeSearchThreadProc, &aThreadStruct[I].pstSearchThreadParms );
		}
		MX_THREADED_GATHER( CSearchResult1 );
#else 
		std::vector<uint8_t> vBuffer;
		uint64_t uiLargestChunk = std::max( _aclChunks.LargestChunk(), 1ULL );
		while ( !vBuffer.size() ) {
			try {
				vBuffer.resize( uiLargestChunk );
			}
			catch ( const std::bad_alloc & /*_eE*/ ) {}
			uiLargestChunk = std::max( uiLargestChunk, 1024ULL * 1024ULL ) - 1024ULL * 1024ULL;
			if ( !uiLargestChunk && !vBuffer.size() ) {
				// Unable to allocate any memory at all??  In practice this code can't be executed.
				return false;
			}
		}
		// At this point vBuffer.size() is guaranteed not to be 0.
		CSearchResultLocker srlSearchLock( m_psrbCurTmpSearchResults );
		for ( size_t I = 0; I < _aclChunks.Chunks().size(); ++I ) {
			for ( uint64_t J = 0; J < _aclChunks.Chunks()[I].ui64Len; ) {
				// Buffer as much of the chunk as possible.
				size_t sSize = std::min( static_cast<size_t>(_aclChunks.Chunks()[I].ui64Len - J), vBuffer.size() );
				if ( sSize < ui32Size ) { break; }
				if ( m_ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(_aclChunks.Chunks()[I].ui64Start + J), &vBuffer[0], sSize, nullptr ) ) {
					PreprocessByteSwap( vBuffer.data(), sSize, _spParms );
					// Go over the chunk and compare.
					 
					size_t sEnd = sSize - ui32Size;
					for ( uint64_t K = 0; K <= sEnd; ) {
						if ( rcCompFunc( _spParms.dtLVal, (*reinterpret_cast<CUtilities::MX_DATA_TYPE *>(&vBuffer[K])) ) ^ _spParms.bInvertResults ) {
							if ( !m_psrbCurTmpSearchResults->Add( _aclChunks.Chunks()[I].ui64Start + J + K,
								&vBuffer[K], ui32Size ) ) {
								return false;
							}
						}
						K += _spParms.ui32Alignment;
					}

					J += sSize;
				}
				else {
					break;
				}
			}

			if ( !(I & 0x3) && _hProgressUpdate ) {
				::SendNotifyMessageW( _hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>(((I + 1.0) / _aclChunks.Chunks().size()) * 100.0), 0L );
			}
			if ( _spParms.pbAbort && (*_spParms.pbAbort) ) { break; }
		}
#endif	// #ifndef MX_SINGLE_THREAD_SEARCH
		// ====================
		// ====================

		return bAllPass;
	}

	// Less-than search on primitives.
	bool CSearcher::LessThanPrimitiveSearch( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate ) {
		delete m_psrbCurTmpSearchResults;
		m_psrbCurTmpSearchResults = new ( std::nothrow ) CSearchResult1();
		if ( !m_psrbCurTmpSearchResults ) { return false; }
		_spParms.bInvertResults = _spParms.bInvertResults ? true : false;	// Force to 0 or 1.


		// ==============================================
		// == Create the comparison function and size. ==
		// ==============================================
		MX_REL_COMP rcCompFunc = nullptr;
		uint32_t ui32Size = 0;
		if ( !GetLessThanCmpFuncAndSize( rcCompFunc, ui32Size, _spParms, false ) ) { return false; }
		// ==============================================
		// ==============================================


		// ================
		// == Alignment. ==
		// ================
		if ( !PrimitiveAlignment( _spParms, _spParms.dtLVal.dtType, m_ppProcess ) ) { return false; }
		// ================
		// ================


		// ====================
		// == Do the search. ==
		// ====================
		bool bAllPass = true;
#ifndef MX_SINGLE_THREAD_SEARCH
		const size_t stThreads = 8;
		struct {
			CThread tThread;
			CSearchResult1 srResults;
			MX_PRIM_SEARCH_THREAD pstSearchThreadParms;
		} aThreadStruct[stThreads];
		LONG lProgTotal = 0;
		size_t stMax = std::min( _aclChunks.Chunks().size(), stThreads );
		for ( size_t I = 0; I < stMax; ++I ) {
			aThreadStruct[I].pstSearchThreadParms.hProgressUpdate = _hProgressUpdate;
			aThreadStruct[I].pstSearchThreadParms.paclChunks = &_aclChunks;
			aThreadStruct[I].pstSearchThreadParms.pspParms = &_spParms;
			aThreadStruct[I].pstSearchThreadParms.psrbResults = &aThreadStruct[I].srResults;
			aThreadStruct[I].pstSearchThreadParms.ppProcess = m_ppProcess;
			aThreadStruct[I].pstSearchThreadParms.rcRelCompFunc = rcCompFunc;
			aThreadStruct[I].pstSearchThreadParms.stStartIdx = (_aclChunks.Chunks().size() * I / stMax);
			aThreadStruct[I].pstSearchThreadParms.stEndIdx = (_aclChunks.Chunks().size() * (I + 1) / stMax);
			aThreadStruct[I].pstSearchThreadParms.ui32Size = ui32Size;
			aThreadStruct[I].pstSearchThreadParms.i32Result = 0;
			aThreadStruct[I].pstSearchThreadParms.plProgress = &lProgTotal;
			
			aThreadStruct[I].tThread.CreateThread( PrimitiveRelativeSearchThreadProc, &aThreadStruct[I].pstSearchThreadParms );
		}
		MX_THREADED_GATHER( CSearchResult1 );
#else
		std::vector<uint8_t> vBuffer;
		uint64_t uiLargestChunk = std::max( _aclChunks.LargestChunk(), 1ULL );
		while ( !vBuffer.size() ) {
			try {
				vBuffer.resize( uiLargestChunk );
			}
			catch ( const std::bad_alloc & /*_eE*/ ) {}
			uiLargestChunk = std::max( uiLargestChunk, 1024ULL * 1024ULL ) - 1024ULL * 1024ULL;
			if ( !uiLargestChunk && !vBuffer.size() ) {
				// Unable to allocate any memory at all??  In practice this code can't be executed.
				return false;
			}
		}
		// At this point vBuffer.size() is guaranteed not to be 0.
		CSearchResultLocker srlSearchLock( m_psrbCurTmpSearchResults );
		for ( size_t I = 0; I < _aclChunks.Chunks().size(); ++I ) {
			for ( uint64_t J = 0; J < _aclChunks.Chunks()[I].ui64Len; ) {
				// Buffer as much of the chunk as possible.
				size_t sSize = std::min( static_cast<size_t>(_aclChunks.Chunks()[I].ui64Len - J), vBuffer.size() );
				if ( sSize < ui32Size ) { break; }
				if ( m_ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(_aclChunks.Chunks()[I].ui64Start + J), &vBuffer[0], sSize, nullptr ) ) {
					PreprocessByteSwap( vBuffer.data(), sSize, _spParms );
					// Go over the chunk and compare.
					 
					size_t sEnd = sSize - ui32Size;
					for ( uint64_t K = 0; K <= sEnd; ) {
						if ( rcCompFunc( _spParms.dtLVal, (*reinterpret_cast<CUtilities::MX_DATA_TYPE *>(&vBuffer[K])) ) ^ _spParms.bInvertResults ) {
							if ( !m_psrbCurTmpSearchResults->Add( _aclChunks.Chunks()[I].ui64Start + J + K,
								&vBuffer[K], ui32Size ) ) {
								return false;
							}
						}
						K += _spParms.ui32Alignment;
					}

					J += sSize;
				}
				else {
					break;
				}
			}

			if ( !(I & 0x3) && _hProgressUpdate ) {
				::SendNotifyMessageW( _hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>(((I + 1.0) / _aclChunks.Chunks().size()) * 100.0), 0L );
			}
			if ( _spParms.pbAbort && (*_spParms.pbAbort) ) { break; }
		}
#endif	// #ifndef MX_SINGLE_THREAD_SEARCH
		// ====================
		// ====================

		return bAllPass;
	}

	// In-range search on primitives.
	bool CSearcher::InRangePrimitiveSearch( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate ) {
		if ( !CUtilities::DataTypeIsFloat( _spParms.dtLVal.dtType ) &&
			CUtilities::DataTypeIsMax( _spParms.dtRVal ) &&
			CUtilities::DataTypeIsMin( _spParms.dtLVal ) ) {
			return UnknownPrimitiveSearch( _spParms, _aclChunks, _hProgressUpdate );
		}

		delete m_psrbCurTmpSearchResults;
		m_psrbCurTmpSearchResults = new ( std::nothrow ) CSearchResult1();
		if ( !m_psrbCurTmpSearchResults ) { return false; }
		_spParms.bInvertResults = _spParms.bInvertResults ? true : false;	// Force to 0 or 1.
		


		// ==============================================
		// == Create the comparison function and size. ==
		// ==============================================
		MX_RANGE_COMP rcCompFunc = nullptr;
		uint32_t ui32Size = 0;
		if ( !GetRangeCmpFuncAndSize( rcCompFunc, ui32Size, _spParms, false ) ) { return false; }
		// ==============================================
		// ==============================================


		// ================
		// == Alignment. ==
		// ================
		if ( !PrimitiveAlignment( _spParms, _spParms.dtLVal.dtType, m_ppProcess ) ) { return false; }
		// ================
		// ================


		// ====================
		// == Do the search. ==
		// ====================
		bool bAllPass = true;
#ifndef MX_SINGLE_THREAD_SEARCH
		const size_t stThreads = 8;
		struct {
			CThread tThread;
			CSearchResult1 srResults;
			MX_PRIM_SEARCH_THREAD pstSearchThreadParms;
		} aThreadStruct[stThreads];
		LONG lProgTotal = 0;
		size_t stMax = std::min( _aclChunks.Chunks().size(), stThreads );
		for ( size_t I = 0; I < stMax; ++I ) {
			aThreadStruct[I].pstSearchThreadParms.hProgressUpdate = _hProgressUpdate;
			aThreadStruct[I].pstSearchThreadParms.paclChunks = &_aclChunks;
			aThreadStruct[I].pstSearchThreadParms.pspParms = &_spParms;
			aThreadStruct[I].pstSearchThreadParms.psrbResults = &aThreadStruct[I].srResults;
			aThreadStruct[I].pstSearchThreadParms.ppProcess = m_ppProcess;
			aThreadStruct[I].pstSearchThreadParms.rcRangeCompFunc = rcCompFunc;
			aThreadStruct[I].pstSearchThreadParms.stStartIdx = (_aclChunks.Chunks().size() * I / stMax);
			aThreadStruct[I].pstSearchThreadParms.stEndIdx = (_aclChunks.Chunks().size() * (I + 1) / stMax);
			aThreadStruct[I].pstSearchThreadParms.ui32Size = ui32Size;
			aThreadStruct[I].pstSearchThreadParms.i32Result = 0;
			aThreadStruct[I].pstSearchThreadParms.plProgress = &lProgTotal;
			
			aThreadStruct[I].tThread.CreateThread( PrimitiveRangeSearchThreadProc, &aThreadStruct[I].pstSearchThreadParms );
		}
		MX_THREADED_GATHER( CSearchResult1 );
#else
		std::vector<uint8_t> vBuffer;
		uint64_t uiLargestChunk = std::max( _aclChunks.LargestChunk(), 1ULL );
		while ( !vBuffer.size() ) {
			try {
				vBuffer.resize( uiLargestChunk );
			}
			catch ( const std::bad_alloc & /*_eE*/ ) {}
			uiLargestChunk = std::max( uiLargestChunk, 1024ULL * 1024ULL ) - 1024ULL * 1024ULL;
			if ( !uiLargestChunk && !vBuffer.size() ) {
				// Unable to allocate any memory at all??  In practice this code can't be executed.
				return false;
			}
		}
		// At this point vBuffer.size() is guaranteed not to be 0.
		CSearchResultLocker srlSearchLock( m_psrbCurTmpSearchResults );
		for ( size_t I = 0; I < _aclChunks.Chunks().size(); ++I ) {
			for ( uint64_t J = 0; J < _aclChunks.Chunks()[I].ui64Len; ) {
				// Buffer as much of the chunk as possible.
				size_t sSize = std::min( static_cast<size_t>(_aclChunks.Chunks()[I].ui64Len - J), vBuffer.size() );
				if ( sSize < ui32Size ) { break; }
				if ( m_ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(_aclChunks.Chunks()[I].ui64Start + J), &vBuffer[0], sSize, nullptr ) ) {
					PreprocessByteSwap( vBuffer.data(), sSize, _spParms );
					// Go over the chunk and compare.
					 
					size_t sEnd = sSize - ui32Size;
					for ( uint64_t K = 0; K <= sEnd; ) {
						if ( rcCompFunc( _spParms.dtLVal, _spParms.dtRVal, (*reinterpret_cast<CUtilities::MX_DATA_TYPE *>(&vBuffer[K])) ) ^ _spParms.bInvertResults ) {
							if ( !m_psrbCurTmpSearchResults->Add( _aclChunks.Chunks()[I].ui64Start + J + K,
								&vBuffer[K], ui32Size ) ) {
								return false;
							}
						}
						K += _spParms.ui32Alignment;
					}

					J += sSize;
				}
				else {
					break;
				}
			}

			if ( !(I & 0x3) && _hProgressUpdate ) {
				::SendNotifyMessageW( _hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>(((I + 1.0) / _aclChunks.Chunks().size()) * 100.0), 0L );
			}
			if ( _spParms.pbAbort && (*_spParms.pbAbort) ) { break; }
		}
#endif	// #ifndef MX_SINGLE_THREAD_SEARCH
		// ====================
		// ====================

		return bAllPass;
	}

	// Unkown search on primitives.
	bool CSearcher::UnknownPrimitiveSearch( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate ) {
		delete m_psrbCurTmpSearchResults;
		m_psrbCurTmpSearchResults = new ( std::nothrow ) CSearchResult2();
		if ( !m_psrbCurTmpSearchResults ) { return false; }
		if ( _spParms.bInvertResults ) {
			// The inverse of all results is no results.  But that is not a failing condition.
			return true;
		}


		// ===================
		// == Get the size. ==
		// ===================
		uint32_t ui32Size = CUtilities::DataTypeSize( _spParms.dtLVal.dtType );
		if ( ui32Size == DWINVALID ) { return false; }
		// ===================
		// ===================


		// ================
		// == Alignment. ==
		// ================
		if ( !PrimitiveAlignment( _spParms, _spParms.dtLVal.dtType, m_ppProcess ) ) { return false; }
		// ================
		// ================


		// ====================
		// == Do the search. ==
		// ====================
		CSearchResultLocker srlSearchLock( m_psrbCurTmpSearchResults );
		for ( size_t I = 0; I < _aclChunks.Chunks().size(); ++I ) {
			uint64_t ui64TotalItems = (_aclChunks.Chunks()[I].ui64Len - ui32Size) / _spParms.ui32Alignment + 1;
			reinterpret_cast<CSearchResult2 *>(m_psrbCurTmpSearchResults)->Add( m_ppProcess, _aclChunks.Chunks()[I].ui64Start, static_cast<uint16_t>(ui32Size),
				ui64TotalItems, static_cast<uint16_t>(_spParms.ui32Alignment) );

			if ( !(I & 0x1) && _hProgressUpdate ) {
				::SendNotifyMessageW( _hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>(((I + 1.0) / _aclChunks.Chunks().size()) * 100.0), 0L );
			}
			if ( _spParms.pbAbort && (*_spParms.pbAbort) ) { break; }
		}
		// ====================
		// ====================

		return true;
	}

	// ASCII/regular string search.
	template <typename _tType>
	bool CSearcher::NormalStringSearch( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate ) {
		delete m_psrbCurTmpSearchResults;
		m_psrbCurTmpSearchResults = new ( std::nothrow ) CSearchResultRef();
		if ( !m_psrbCurTmpSearchResults ) { return false; }


		// ================
		// == Alignment. ==
		// ================
		_spParms.ui32Alignment = _spParms.bAligned ? alignof( _tType ) : 1;
		// ================
		// ================


		// =======================================
		// == Prepare the string for searching. ==
		// =======================================
		MX_STRING_SEARCH_UNPACKED ssuSearchString;
		if ( ! _spParms.vStringSearchValue.size() ) { return false; }
		if ( !CUtilities::BuildStringDatabaseAnsi( &_spParms.vStringSearchValue[0], _spParms.vStringSearchValue.size(), _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
			ssuSearchString.vBuffer1, ssuSearchString.vBuffer2, ssuSearchString.vBuffer3 ) ) { return false; }
		if ( !CUtilities::ComposeDataIntoSearchableStringAnsi( &_spParms.vStringSearchValue[0], _spParms.vStringSearchValue.size(), _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
			ssuSearchString.vBuffer1, ssuSearchString.vBuffer3, ssuSearchString.vDataBuffer, true /* Ignored. */, false /* Ignored. */ ) ) { return false; }
		// =======================================
		// =======================================


		// ====================
		// == Do the search. ==
		// ====================
		bool bAllPass = true;
#ifndef MX_SINGLE_THREAD_SEARCH
		const size_t stThreads = 8;
		struct {
			CThread						tThread;
			CSearchResultRef			srResults;
			MX_UTF_SEARCH_THREAD		pstSearchThreadParms;
		} aThreadStruct[stThreads];
		LONG lProgTotal = 0;
		size_t stMax = std::min( _aclChunks.Chunks().size(), stThreads );
		for ( size_t I = 0; I < stMax; ++I ) {
			aThreadStruct[I].pstSearchThreadParms.hProgressUpdate = _hProgressUpdate;
			aThreadStruct[I].pstSearchThreadParms.paclChunks = &_aclChunks;
			aThreadStruct[I].pstSearchThreadParms.pspParms = &_spParms;
			aThreadStruct[I].pstSearchThreadParms.psrbResults = &aThreadStruct[I].srResults;
			aThreadStruct[I].pstSearchThreadParms.ppProcess = m_ppProcess;
			aThreadStruct[I].pstSearchThreadParms.stStartIdx = (_aclChunks.Chunks().size() * I / stMax);
			aThreadStruct[I].pstSearchThreadParms.stEndIdx = (_aclChunks.Chunks().size() * (I + 1) / stMax);
			aThreadStruct[I].pstSearchThreadParms.i32Result = 0;
			aThreadStruct[I].pstSearchThreadParms.plProgress = &lProgTotal;
			aThreadStruct[I].pstSearchThreadParms.pssuUnpackedSrcString = &ssuSearchString;
			
			aThreadStruct[I].tThread.CreateThread( StringNormalSearchThreadProc, &aThreadStruct[I].pstSearchThreadParms );
		}
		MX_THREADED_GATHER( CSearchResultRef );
#else
		// Local raw buffer.
		std::vector<uint8_t> vBuffer;
		// Pre-op buffers.
		MX_STRING_SEARCH_UNPACKED ssuData;
		uint64_t uiLargestChunk = std::max( _aclChunks.LargestChunk(), 1ULL );
		bool bContinue = false;
		while ( !bContinue ) {
			bContinue = false;
			try {
				vBuffer.resize( uiLargestChunk );
				if ( CUtilities::NeedsType1( _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx ) ) { ssuData.vBuffer1.resize( uiLargestChunk ); }
				if ( CUtilities::NeedsType2( _spParms.ui32StringSearchFlags ) ) { ssuData.vBuffer2.resize( uiLargestChunk ); }
				if ( CUtilities::NeedsType3( _spParms.ui32StringSearchFlags ) ) { ssuData.vBuffer3.resize( uiLargestChunk ); }
				ssuData.vDataBuffer.resize( uiLargestChunk );
				bContinue = true;
				break;
			}
			catch ( const std::bad_alloc & /*_eE*/ ) {}
			uiLargestChunk = std::max( uiLargestChunk, 1024ULL * 1024ULL ) - 1024ULL * 1024ULL;
			if ( !uiLargestChunk && !bContinue ) {
				// Unable to allocate any memory at all??  In practice this code can't be executed.
				return false;
			}
		}


		bool bWholeWord = (_spParms.ui32StringSearchFlagsEx & CUtilities::MX_SSF_WHOLE_WORD) != 0;
		// At this point vBuffer.size() is guaranteed not to be 0.
		CSearchResultLocker srlSearchLock( m_psrbCurTmpSearchResults );
		for ( size_t I = 0; I < _aclChunks.Chunks().size(); ++I ) {
			for ( uint64_t J = 0; J < _aclChunks.Chunks()[I].ui64Len; ) {
				// Buffer as much of the chunk as possible.
				size_t sSize = std::min( static_cast<size_t>(_aclChunks.Chunks()[I].ui64Len - J), vBuffer.size() );
				if ( m_ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(_aclChunks.Chunks()[I].ui64Start + J), &vBuffer[0], sSize, nullptr ) ) {
					PreprocessByteSwap( vBuffer.data(), sSize, _spParms );

					// Pre-op.
					if ( !CUtilities::BuildStringDatabaseAnsi( &vBuffer[0], sSize, _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
						ssuData.vBuffer1, ssuData.vBuffer2, ssuData.vBuffer3 ) ) { return false; }
					if ( !CUtilities::ComposeDataIntoSearchableStringAnsi( &vBuffer[0], sSize, _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
						ssuData.vBuffer1, ssuData.vBuffer3, ssuData.vDataBuffer, true /* Ignored. */, false /* Ignored. */ ) ) { return false; }

					// Go over the chunk and compare.
					size_t sEnd = sSize;
					for ( uint64_t K = 0; K < sEnd; ) {
						if ( !bWholeWord || String_WholeWordLeft( ssuData, K, _spParms.ui32Alignment ) ) {
							size_t sLen;
							if ( Cmp_ComposedStringCompare( ssuSearchString, ssuData,
								sSize,
								K,
								_spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
								sLen ) ) {
								if ( !m_psrbCurTmpSearchResults->Add( _aclChunks.Chunks()[I].ui64Start + J + K,
									sLen, _spParms.sstSearchType ) ) {
									return false;
								}
								K += sLen;
								continue;
							}
						}
						K += _spParms.ui32Alignment;
					}

					J += sSize;
				}
				else {
					break;
				}
			}

			if ( !(I & 0x3) && _hProgressUpdate ) {
				::SendNotifyMessageW( _hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>(((I + 1.0) / _aclChunks.Chunks().size()) * 100.0), 0L );
			}
			if ( _spParms.pbAbort && (*_spParms.pbAbort) ) { break; }
		}
#endif	// #ifndef MX_SINGLE_THREAD_SEARCH
		// ====================
		// ====================

		return bAllPass;
	}

	// A single UTF string search.
	template <typename _tType>
	bool CSearcher::UtfAnyStringSearch( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate ) {
		delete m_psrbCurTmpSearchResults;
		m_psrbCurTmpSearchResults = new ( std::nothrow ) CSearchResultRef();
		if ( !m_psrbCurTmpSearchResults ) { return false; }


		// ================
		// == Alignment. ==
		// ================
		_spParms.ui32Alignment = _spParms.bAligned ? alignof( _tType ) : 1;
		// ================
		// ================


		// =======================================
		// == Prepare the string for searching. ==
		// =======================================
		MX_STRING_SEARCH_UNPACKED ssuSearchString;
		if ( ! _spParms.vStringSearchValue.size() ) { return false; }
		switch ( _spParms.sstSearchType ) {
			case CUtilities::MX_SST_UTFALL : {}
			case CUtilities::MX_SST_UTF8 : {
				if ( !CUtilities::BuildStringDatabaseUtf8( &_spParms.vStringSearchValue[0], _spParms.vStringSearchValue.size(), _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
					ssuSearchString.vBuffer1, ssuSearchString.vBuffer2, ssuSearchString.vBuffer3 ) ) { return false; }
				if ( !CUtilities::ComposeDataIntoSearchableStringUtf8( &_spParms.vStringSearchValue[0], _spParms.vStringSearchValue.size(), _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
					ssuSearchString.vBuffer1, ssuSearchString.vBuffer3, ssuSearchString.vDataBuffer, true /* Ignored. */, false /* Ignored. */ ) ) { return false; }
				break;
			}
			case CUtilities::MX_SST_UTF16_BE : {}
			case CUtilities::MX_SST_UTF16 : {
				if ( !CUtilities::BuildStringDatabaseUtf16( &_spParms.vStringSearchValue[0], _spParms.vStringSearchValue.size(), _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
					ssuSearchString.vBuffer1, ssuSearchString.vBuffer2, ssuSearchString.vBuffer3 ) ) { return false; }
				if ( !CUtilities::ComposeDataIntoSearchableStringUtf16( &_spParms.vStringSearchValue[0], _spParms.vStringSearchValue.size(), _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
					ssuSearchString.vBuffer1, ssuSearchString.vBuffer3, ssuSearchString.vDataBuffer, _spParms.bAligned, _spParms.sstSearchType == CUtilities::MX_SST_UTF16_BE ) ) { return false; }
				break;
			}
			case CUtilities::MX_SST_UTF32_BE : {}
			case CUtilities::MX_SST_UTF32 : {
				if ( !CUtilities::BuildStringDatabaseUtf32( &_spParms.vStringSearchValue[0], _spParms.vStringSearchValue.size(), _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
					ssuSearchString.vBuffer1, ssuSearchString.vBuffer2, ssuSearchString.vBuffer3 ) ) { return false; }
				if ( !CUtilities::ComposeDataIntoSearchableStringUtf32( &_spParms.vStringSearchValue[0], _spParms.vStringSearchValue.size(), _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
					ssuSearchString.vBuffer1, ssuSearchString.vBuffer3, ssuSearchString.vDataBuffer, _spParms.bAligned, _spParms.sstSearchType == CUtilities::MX_SST_UTF32_BE ) ) { return false; }
				break;
			}
		}
		// =======================================
		// =======================================


		// ====================
		// == Do the search. ==
		// ====================
		bool bAllPass = true;
#ifndef MX_SINGLE_THREAD_SEARCH
		const size_t stThreads = 16;
		struct {
			CThread						tThread;
			CSearchResultRef			srResults;
			MX_UTF_SEARCH_THREAD		pstSearchThreadParms;
		} aThreadStruct[stThreads];
		LONG lProgTotal = 0;
		size_t stMax = std::min( _aclChunks.Chunks().size(), stThreads );
		for ( size_t I = 0; I < stMax; ++I ) {
			aThreadStruct[I].pstSearchThreadParms.hProgressUpdate = _hProgressUpdate;
			aThreadStruct[I].pstSearchThreadParms.paclChunks = &_aclChunks;
			aThreadStruct[I].pstSearchThreadParms.pspParms = &_spParms;
			aThreadStruct[I].pstSearchThreadParms.psrbResults = &aThreadStruct[I].srResults;
			aThreadStruct[I].pstSearchThreadParms.ppProcess = m_ppProcess;
			aThreadStruct[I].pstSearchThreadParms.stStartIdx = (_aclChunks.Chunks().size() * I / stMax);
			aThreadStruct[I].pstSearchThreadParms.stEndIdx = (_aclChunks.Chunks().size() * (I + 1) / stMax);
			aThreadStruct[I].pstSearchThreadParms.i32Result = 0;
			aThreadStruct[I].pstSearchThreadParms.plProgress = &lProgTotal;
			aThreadStruct[I].pstSearchThreadParms.pssuUnpackedSrcString = &ssuSearchString;
			
			aThreadStruct[I].tThread.CreateThread( StringUtfSearchThreadProc, &aThreadStruct[I].pstSearchThreadParms );
		}
		MX_THREADED_GATHER( CSearchResultRef );
#else
		// Local raw buffer.
		std::vector<uint8_t> vBuffer;
		// Pre-op buffers.
		MX_STRING_SEARCH_UNPACKED ssuData;
		uint64_t uiLargestChunk = std::max( _aclChunks.LargestChunk(), 1ULL );
		bool bContinue = false;
		while ( !bContinue ) {
			bContinue = false;
			try {
				vBuffer.resize( uiLargestChunk );
				if ( CUtilities::NeedsType1( _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx ) ) { ssuData.vBuffer1.resize( uiLargestChunk ); }
				if ( CUtilities::NeedsType2( _spParms.ui32StringSearchFlags ) ) { ssuData.vBuffer2.resize( uiLargestChunk ); }
				if ( CUtilities::NeedsType3( _spParms.ui32StringSearchFlags ) ) { ssuData.vBuffer3.resize( uiLargestChunk ); }
				ssuData.vDataBuffer.resize( uiLargestChunk );
				bContinue = true;
				break;
			}
			catch ( const std::bad_alloc & /*_eE*/ ) {}
			uiLargestChunk = std::max( uiLargestChunk, 1024ULL * 1024ULL ) - 1024ULL * 1024ULL;
			if ( !uiLargestChunk && !bContinue ) {
				// Unable to allocate any memory at all??  In practice this code can't be executed.
				return false;
			}
		}


		bool bWholeWord = (_spParms.ui32StringSearchFlagsEx & CUtilities::MX_SSF_WHOLE_WORD) != 0;
		// At this point vBuffer.size() is guaranteed not to be 0.
		CSearchResultLocker srlSearchLock( m_psrbCurTmpSearchResults );
		for ( size_t I = 0; I < _aclChunks.Chunks().size(); ++I ) {
			for ( uint64_t J = 0; J < _aclChunks.Chunks()[I].ui64Len; ) {
				// Buffer as much of the chunk as possible.
				size_t sSize = std::min( static_cast<size_t>(_aclChunks.Chunks()[I].ui64Len - J), vBuffer.size() );
				if ( m_ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(_aclChunks.Chunks()[I].ui64Start + J), &vBuffer[0], sSize, nullptr ) ) {
					PreprocessByteSwap( vBuffer.data(), sSize, _spParms );

					// Pre-op.
					switch ( _spParms.sstSearchType ) {
						case CUtilities::MX_SST_UTF8 : {
							if ( !CUtilities::BuildStringDatabaseUtf8( &vBuffer[0], sSize, _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
								ssuData.vBuffer1, ssuData.vBuffer2, ssuData.vBuffer3 ) ) { return false; }
							if ( !CUtilities::ComposeDataIntoSearchableStringUtf8( &vBuffer[0], sSize, _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
								ssuData.vBuffer1, ssuData.vBuffer3, ssuData.vDataBuffer, true /* Ignored. */, false /* Ignored. */ ) ) { return false; }
							break;
						}
						case CUtilities::MX_SST_UTF16 : {
							if ( !CUtilities::BuildStringDatabaseUtf16( &vBuffer[0], sSize, _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
								ssuData.vBuffer1, ssuData.vBuffer2, ssuData.vBuffer3 ) ) { return false; }
							if ( !CUtilities::ComposeDataIntoSearchableStringUtf16( &vBuffer[0], sSize, _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
								ssuData.vBuffer1, ssuData.vBuffer3, ssuData.vDataBuffer, _spParms.bAligned, false ) ) { return false; }
							break;
						}
						case CUtilities::MX_SST_UTF32 : {
							if ( !CUtilities::BuildStringDatabaseUtf32( &vBuffer[0], sSize, _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
								ssuData.vBuffer1, ssuData.vBuffer2, ssuData.vBuffer3 ) ) { return false; }
							if ( !CUtilities::ComposeDataIntoSearchableStringUtf32( &vBuffer[0], sSize, _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
								ssuData.vBuffer1, ssuData.vBuffer3, ssuData.vDataBuffer, _spParms.bAligned, false ) ) { return false; }
							break;
						}
						case CUtilities::MX_SST_UTF16_BE : {
							if ( !CUtilities::BuildStringDatabaseUtf16( &vBuffer[0], sSize, _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
								ssuData.vBuffer1, ssuData.vBuffer2, ssuData.vBuffer3 ) ) { return false; }
							if ( !CUtilities::ComposeDataIntoSearchableStringUtf16( &vBuffer[0], sSize, _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
								ssuData.vBuffer1, ssuData.vBuffer3, ssuData.vDataBuffer, _spParms.bAligned, true ) ) { return false; }
							break;
						}
						case CUtilities::MX_SST_UTF32_BE : {
							if ( !CUtilities::BuildStringDatabaseUtf32( &vBuffer[0], sSize, _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
								ssuData.vBuffer1, ssuData.vBuffer2, ssuData.vBuffer3 ) ) { return false; }
							if ( !CUtilities::ComposeDataIntoSearchableStringUtf32( &vBuffer[0], sSize, _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
								ssuData.vBuffer1, ssuData.vBuffer3, ssuData.vDataBuffer, _spParms.bAligned, true ) ) { return false; }
							break;
						}
					}

					// Go over the chunk and compare.
					size_t sEnd = sSize;
					for ( uint64_t K = 0; K < sEnd; ) {
						if ( !bWholeWord || String_WholeWordLeft( ssuData, K, _spParms.ui32Alignment ) ) {
							size_t sLen;
							if ( Cmp_ComposedStringCompare( ssuSearchString, ssuData,
								sSize,
								K,
								_spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
								sLen ) ) {
								if ( !m_psrbCurTmpSearchResults->Add( _aclChunks.Chunks()[I].ui64Start + J + K,
									sLen, _spParms.sstSearchType ) ) {
									return false;
								}
								K += sLen;
								continue;
							}
						}
						K += _spParms.ui32Alignment;
					}

					J += sSize;
				}
				else {
					break;
				}
			}

			if ( !(I & 0x3) && _hProgressUpdate ) {
				::SendNotifyMessageW( _hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>(((I + 1.0) / _aclChunks.Chunks().size()) * 100.0), 0L );
			}
			if ( _spParms.pbAbort && (*_spParms.pbAbort) ) { break; }
		}

#endif	// #ifndef MX_SINGLE_THREAD_SEARCH
		// ====================
		// ====================

		return bAllPass;
	}

	// UTF-* search.
	bool CSearcher::UtfAllStringSearch( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate ) {
		delete m_psrbCurTmpSearchResults;
		m_psrbCurTmpSearchResults = new ( std::nothrow ) CSearchResultRef();
		if ( !m_psrbCurTmpSearchResults ) { return false; }


		// ================
		// == Alignment. ==
		// ================
		_spParms.ui32Alignment = 1;
		// ================
		// ================


		// =======================================
		// == Prepare the string for searching. ==
		// =======================================
		MX_STRING_SEARCH_UNPACKED ssuSearchString;
		if ( ! _spParms.vStringSearchValue.size() ) { return false; }
		switch ( _spParms.sstSearchType ) {
			case CUtilities::MX_SST_UTFALL : {}
			case CUtilities::MX_SST_UTF8 : {
				if ( !CUtilities::BuildStringDatabaseUtf8( &_spParms.vStringSearchValue[0], _spParms.vStringSearchValue.size(), _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
					ssuSearchString.vBuffer1, ssuSearchString.vBuffer2, ssuSearchString.vBuffer3 ) ) { return false; }
				if ( !CUtilities::ComposeDataIntoSearchableStringUtf8( &_spParms.vStringSearchValue[0], _spParms.vStringSearchValue.size(), _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
					ssuSearchString.vBuffer1, ssuSearchString.vBuffer3, ssuSearchString.vDataBuffer, true /* Ignored. */, false /* Ignored. */ ) ) { return false; }
				break;
			}
			case CUtilities::MX_SST_UTF16_BE : {}
			case CUtilities::MX_SST_UTF16 : {
				if ( !CUtilities::BuildStringDatabaseUtf16( &_spParms.vStringSearchValue[0], _spParms.vStringSearchValue.size(), _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
					ssuSearchString.vBuffer1, ssuSearchString.vBuffer2, ssuSearchString.vBuffer3 ) ) { return false; }
				if ( !CUtilities::ComposeDataIntoSearchableStringUtf16( &_spParms.vStringSearchValue[0], _spParms.vStringSearchValue.size(), _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
					ssuSearchString.vBuffer1, ssuSearchString.vBuffer3, ssuSearchString.vDataBuffer, _spParms.bAligned, _spParms.sstSearchType == CUtilities::MX_SST_UTF16_BE ) ) { return false; }
				break;
			}
			case CUtilities::MX_SST_UTF32_BE : {}
			case CUtilities::MX_SST_UTF32 : {
				if ( !CUtilities::BuildStringDatabaseUtf32( &_spParms.vStringSearchValue[0], _spParms.vStringSearchValue.size(), _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
					ssuSearchString.vBuffer1, ssuSearchString.vBuffer2, ssuSearchString.vBuffer3 ) ) { return false; }
				if ( !CUtilities::ComposeDataIntoSearchableStringUtf32( &_spParms.vStringSearchValue[0], _spParms.vStringSearchValue.size(), _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
					ssuSearchString.vBuffer1, ssuSearchString.vBuffer3, ssuSearchString.vDataBuffer, _spParms.bAligned, _spParms.sstSearchType == CUtilities::MX_SST_UTF32_BE ) ) { return false; }
				break;
			}
		}
		// =======================================
		// =======================================



		// ====================
		// == Do the search. ==
		// ====================
		bool bAllPass = true;
#ifndef MX_SINGLE_THREAD_SEARCH
		const size_t stThreads = 32;
		struct {
			CThread						tThread;
			CSearchResultRef			srResults;
			MX_UTF_SEARCH_THREAD		pstSearchThreadParms;
		} aThreadStruct[stThreads];
		LONG lProgTotal = 0;
		size_t stMax = std::min( _aclChunks.Chunks().size(), stThreads );
		for ( size_t I = 0; I < stMax; ++I ) {
			aThreadStruct[I].pstSearchThreadParms.hProgressUpdate = _hProgressUpdate;
			aThreadStruct[I].pstSearchThreadParms.paclChunks = &_aclChunks;
			aThreadStruct[I].pstSearchThreadParms.pspParms = &_spParms;
			aThreadStruct[I].pstSearchThreadParms.psrbResults = &aThreadStruct[I].srResults;
			aThreadStruct[I].pstSearchThreadParms.ppProcess = m_ppProcess;
			aThreadStruct[I].pstSearchThreadParms.stStartIdx = (_aclChunks.Chunks().size() * I / stMax);
			aThreadStruct[I].pstSearchThreadParms.stEndIdx = (_aclChunks.Chunks().size() * (I + 1) / stMax);
			aThreadStruct[I].pstSearchThreadParms.i32Result = 0;
			aThreadStruct[I].pstSearchThreadParms.plProgress = &lProgTotal;
			aThreadStruct[I].pstSearchThreadParms.pssuUnpackedSrcString = &ssuSearchString;
			
			aThreadStruct[I].tThread.CreateThread( StringAllUtfSearchThreadProc, &aThreadStruct[I].pstSearchThreadParms );
		}
		MX_THREADED_GATHER( CSearchResultRef );
#else
		// Local raw buffer.
		std::vector<uint8_t> vBuffer;
		// Pre-op buffers.
		MX_STRING_SEARCH_UNPACKED ssuData[5];
		uint64_t uiLargestChunk = std::max( _aclChunks.LargestChunk(), 1ULL );
		bool bContinue = false;
		while ( !bContinue ) {
			bContinue = false;
			try {
				for ( size_t I = 0; I < 5; ++I ) {
					vBuffer.resize( uiLargestChunk );
					if ( CUtilities::NeedsType1( _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx ) ) { ssuData[I].vBuffer1.resize( uiLargestChunk ); }
					if ( CUtilities::NeedsType2( _spParms.ui32StringSearchFlags ) ) { ssuData[I].vBuffer2.resize( uiLargestChunk ); }
					if ( CUtilities::NeedsType3( _spParms.ui32StringSearchFlags ) ) { ssuData[I].vBuffer3.resize( uiLargestChunk ); }
					ssuData[I].vDataBuffer.resize( uiLargestChunk );
				}
				bContinue = true;
				break;
			}
			catch ( const std::bad_alloc & /*_eE*/ ) {}
			uiLargestChunk = std::max( uiLargestChunk, 1024ULL * 1024ULL ) - 1024ULL * 1024ULL;
			if ( !uiLargestChunk && !bContinue ) {
				// Unable to allocate any memory at all??  In practice this code can't be executed.
				return false;
			}
		}


		bool bWholeWord = (_spParms.ui32StringSearchFlagsEx & CUtilities::MX_SSF_WHOLE_WORD) != 0;
		// At this point vBuffer.size() is guaranteed not to be 0.
		CSearchResultLocker srlSearchLock( m_psrbCurTmpSearchResults );
		const uint32_t ui32Align[5] = {
			alignof( int8_t ),
			alignof( int16_t ),
			alignof( int32_t ),
			alignof( int16_t ),
			alignof( int32_t ),
		};
		const CUtilities::MX_STRING_SEARCH_TYPES sstTypes[5] = {
			CUtilities::MX_SST_UTF8,
			CUtilities::MX_SST_UTF16,
			CUtilities::MX_SST_UTF32,
			CUtilities::MX_SST_UTF16_BE,
			CUtilities::MX_SST_UTF32_BE,
		};
		for ( size_t I = 0; I < _aclChunks.Chunks().size(); ++I ) {
			for ( uint64_t J = 0; J < _aclChunks.Chunks()[I].ui64Len; ) {
				// Buffer as much of the chunk as possible.
				size_t sSize = std::min( static_cast<size_t>(_aclChunks.Chunks()[I].ui64Len - J), vBuffer.size() );
				if ( m_ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(_aclChunks.Chunks()[I].ui64Start + J), &vBuffer[0], sSize, nullptr ) ) {
					PreprocessByteSwap( vBuffer.data(), sSize, _spParms );

					// Pre-op.
					if ( !CUtilities::BuildStringDatabaseUtf8( &vBuffer[0], sSize, _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
						ssuData[0].vBuffer1, ssuData[0].vBuffer2, ssuData[0].vBuffer3 ) ) { return false; }
					if ( !CUtilities::ComposeDataIntoSearchableStringUtf8( &vBuffer[0], sSize, _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
						ssuData[0].vBuffer1, ssuData[0].vBuffer3, ssuData[0].vDataBuffer, false, false ) ) { return false; }
					if ( !CUtilities::BuildStringDatabaseUtf16( &vBuffer[0], sSize, _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
						ssuData[1].vBuffer1, ssuData[1].vBuffer2, ssuData[1].vBuffer3 ) ) { return false; }
					if ( !CUtilities::ComposeDataIntoSearchableStringUtf16( &vBuffer[0], sSize, _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
						ssuData[1].vBuffer1, ssuData[1].vBuffer3, ssuData[1].vDataBuffer, false, false ) ) { return false; }
					if ( !CUtilities::BuildStringDatabaseUtf32( &vBuffer[0], sSize, _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
						ssuData[2].vBuffer1, ssuData[2].vBuffer2, ssuData[2].vBuffer3 ) ) { return false; }
					if ( !CUtilities::ComposeDataIntoSearchableStringUtf32( &vBuffer[0], sSize, _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
						ssuData[2].vBuffer1, ssuData[2].vBuffer3, ssuData[2].vDataBuffer, false, false ) ) { return false; }
					if ( !CUtilities::BuildStringDatabaseUtf16( &vBuffer[0], sSize, _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
						ssuData[3].vBuffer1, ssuData[3].vBuffer2, ssuData[3].vBuffer3 ) ) { return false; }
					if ( !CUtilities::ComposeDataIntoSearchableStringUtf16( &vBuffer[0], sSize, _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
						ssuData[3].vBuffer1, ssuData[3].vBuffer3, ssuData[3].vDataBuffer, false, true ) ) { return false; }
					if ( !CUtilities::BuildStringDatabaseUtf32( &vBuffer[0], sSize, _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
						ssuData[4].vBuffer1, ssuData[4].vBuffer2, ssuData[4].vBuffer3 ) ) { return false; }
					if ( !CUtilities::ComposeDataIntoSearchableStringUtf32( &vBuffer[0], sSize, _spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
						ssuData[4].vBuffer1, ssuData[4].vBuffer3, ssuData[4].vDataBuffer, false, true ) ) { return false; }

					// Go over the chunk and compare.
					size_t sEnd = sSize;
					for ( uint64_t K = 0; K < sEnd; ) {
						for ( size_t M = 0; M < 3; ++M ) {
							if ( !bWholeWord || String_WholeWordLeft( ssuData[M], K, ui32Align[M] ) ) {
								size_t sLen;
								if ( Cmp_ComposedStringCompare( ssuSearchString, ssuData[M],
									sSize,
									K,
									_spParms.ui32StringSearchFlags, _spParms.ui32StringSearchFlagsEx,
									sLen ) ) {
									if ( !m_psrbCurTmpSearchResults->Add( _aclChunks.Chunks()[I].ui64Start + J + K,
										sLen, sstTypes[M] ) ) {
										return false;
									}
								}
							}
						}
						K += _spParms.ui32Alignment;
					}

					J += sSize;
				}
				else {
					break;
				}
			}

			if ( !(I & 0x3) && _hProgressUpdate ) {
				::SendNotifyMessageW( _hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>(((I + 1.0) / _aclChunks.Chunks().size()) * 100.0), 0L );
			}
			if ( _spParms.pbAbort && (*_spParms.pbAbort) ) { break; }
		}

#endif	// #ifndef MX_SINGLE_THREAD_SEARCH
		// ====================
		// ====================

		return bAllPass;
	}

	// A raw array search.
	template <typename _tType>
	bool CSearcher::RawArraySearch( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate ) {
		delete m_psrbCurTmpSearchResults;
		m_psrbCurTmpSearchResults = new ( std::nothrow ) CSearchResultRef();
		if ( !m_psrbCurTmpSearchResults ) { return false; }


		// ================
		// == Alignment. ==
		// ================
		_spParms.ui32Alignment = _spParms.bAligned ? alignof( _tType ) : 1;
		// ================
		// ================


		// =======================================
		// == Prepare the string for searching. ==
		// =======================================
		if ( _spParms.wuWildcardUsage == CUtilities::MX_WU_NONE ) {
			// Boyor Moore preparation.
		}
		size_t sLeftLenInChars = _spParms.vStringSearchValue.size() / sizeof( _tType );
		// =======================================
		// =======================================


		// ====================
		// == Do the search. ==
		// ====================
		bool bAllPass = true;
#ifndef MX_SINGLE_THREAD_SEARCH
		const size_t stThreads = 8;
		struct {
			CThread						tThread;
			CSearchResultRef			srResults;
			MX_RAW_ARRAY_SEARCH_THREAD	pstSearchThreadParms;
		} aThreadStruct[stThreads];
		LONG lProgTotal = 0;
		size_t stMax = std::min( _aclChunks.Chunks().size(), stThreads );
		for ( size_t I = 0; I < stMax; ++I ) {
			aThreadStruct[I].pstSearchThreadParms.hProgressUpdate = _hProgressUpdate;
			aThreadStruct[I].pstSearchThreadParms.paclChunks = &_aclChunks;
			aThreadStruct[I].pstSearchThreadParms.pspParms = &_spParms;
			aThreadStruct[I].pstSearchThreadParms.psrbResults = &aThreadStruct[I].srResults;
			aThreadStruct[I].pstSearchThreadParms.ppProcess = m_ppProcess;
			aThreadStruct[I].pstSearchThreadParms.stStartIdx = (_aclChunks.Chunks().size() * I / stMax);
			aThreadStruct[I].pstSearchThreadParms.stEndIdx = (_aclChunks.Chunks().size() * (I + 1) / stMax);
			aThreadStruct[I].pstSearchThreadParms.i32Result = 0;
			aThreadStruct[I].pstSearchThreadParms.plProgress = &lProgTotal;
			
			aThreadStruct[I].tThread.CreateThread( StringRawArraySearchThreadProc<_tType>, &aThreadStruct[I].pstSearchThreadParms );
		}
		
		MX_THREADED_GATHER( CSearchResultRef );
#else
		// Local raw buffer.
		std::vector<uint8_t> vBuffer;
		// Pre-op buffers.
		uint64_t uiLargestChunk = std::max( _aclChunks.LargestChunk(), 1ULL );
		bool bContinue = false;
		while ( !bContinue ) {
			bContinue = false;
			try {
				vBuffer.resize( uiLargestChunk );
				bContinue = true;
				break;
			}
			catch ( const std::bad_alloc & /*_eE*/ ) {}
			uiLargestChunk = std::max( uiLargestChunk, 1024ULL * 1024ULL ) - 1024ULL * 1024ULL;
			if ( !uiLargestChunk && !bContinue ) {
				// Unable to allocate any memory at all??  In practice this code can't be executed.
				return false;
			}
		}


		// At this point vBuffer.size() is guaranteed not to be 0.
		CSearchResultLocker srlSearchLock( m_psrbCurTmpSearchResults );
		for ( size_t I = 0; I < _aclChunks.Chunks().size(); ++I ) {
			for ( uint64_t J = 0; J < _aclChunks.Chunks()[I].ui64Len; ) {
				// Buffer as much of the chunk as possible.
				size_t sSize = std::min( static_cast<size_t>(_aclChunks.Chunks()[I].ui64Len - J), vBuffer.size() );
				if ( m_ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(_aclChunks.Chunks()[I].ui64Start + J), &vBuffer[0], sSize, nullptr ) ) {
					PreprocessByteSwap( vBuffer.data(), sSize, _spParms );

					// Go over the chunk and compare.
					size_t sEnd = sSize;
					for ( uint64_t K = 0; K < sEnd; ) {
						size_t sLen;
						if ( Cmp_DataArrayCompare<_tType>( reinterpret_cast<const void *>(_spParms.vStringSearchValue.data()), sLeftLenInChars,
							_spParms.vStringMetaData,
							&vBuffer[K], sEnd - K,
							sLen ) ) {
							if ( !m_psrbCurTmpSearchResults->Add( _aclChunks.Chunks()[I].ui64Start + J + K,
								sLen, (_spParms.dtStringCharType << 8) | _spParms.sstSearchType ) ) {
								return false;
							}
						}
						K += _spParms.ui32Alignment;
					}

					J += sSize;
				}
				else {
					break;
				}
			}

			if ( !(I & 0x3) && _hProgressUpdate ) {
				::SendNotifyMessageW( _hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>(((I + 1.0) / _aclChunks.Chunks().size()) * 100.0), 0L );
			}
			if ( _spParms.pbAbort && (*_spParms.pbAbort) ) { break; }
		}

#endif	// #ifndef MX_SINGLE_THREAD_SEARCH
		// ====================
		// ====================

		return bAllPass;
	}

	// A raw array of multiple types search.
	bool CSearcher::RawArrayMultiType( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate ) {
		delete m_psrbCurTmpSearchResults;
		m_psrbCurTmpSearchResults = new ( std::nothrow ) CSearchResultRef();
		if ( !m_psrbCurTmpSearchResults ) { return false; }


		// ================
		// == Alignment. ==
		// ================
		_spParms.ui32Alignment = 1;
		// ================
		// ================


		// =======================================
		// == Prepare the string for searching. ==
		// =======================================
		if ( _spParms.wuWildcardUsage == CUtilities::MX_WU_NONE ) {
			// Boyor Moore preparation.
		}
		// =======================================
		// =======================================


		// ====================
		// == Do the search. ==
		// ====================
		bool bAllPass = true;
#ifndef MX_SINGLE_THREAD_SEARCH
		const size_t stThreads = 8;
		struct {
			CThread						tThread;
			CSearchResultRef			srResults;
			MX_RAW_ARRAY_SEARCH_THREAD	pstSearchThreadParms;
		} aThreadStruct[stThreads];
		LONG lProgTotal = 0;
		size_t stMax = std::min( _aclChunks.Chunks().size(), stThreads );
		for ( size_t I = 0; I < stMax; ++I ) {
			aThreadStruct[I].pstSearchThreadParms.hProgressUpdate = _hProgressUpdate;
			aThreadStruct[I].pstSearchThreadParms.paclChunks = &_aclChunks;
			aThreadStruct[I].pstSearchThreadParms.pspParms = &_spParms;
			aThreadStruct[I].pstSearchThreadParms.psrbResults = &aThreadStruct[I].srResults;
			aThreadStruct[I].pstSearchThreadParms.ppProcess = m_ppProcess;
			aThreadStruct[I].pstSearchThreadParms.stStartIdx = (_aclChunks.Chunks().size() * I / stMax);
			aThreadStruct[I].pstSearchThreadParms.stEndIdx = (_aclChunks.Chunks().size() * (I + 1) / stMax);
			aThreadStruct[I].pstSearchThreadParms.i32Result = 0;
			aThreadStruct[I].pstSearchThreadParms.plProgress = &lProgTotal;
			
			aThreadStruct[I].tThread.CreateThread( StringMultiArraySearchThreadProc, &aThreadStruct[I].pstSearchThreadParms );
		}
		
		MX_THREADED_GATHER( CSearchResultRef );
#else
		// Local raw buffer.
		std::vector<uint8_t> vBuffer;
		// Pre-op buffers.
		uint64_t uiLargestChunk = std::max( _aclChunks.LargestChunk(), 1ULL );
		bool bContinue = false;
		while ( !bContinue ) {
			bContinue = false;
			try {
				vBuffer.resize( uiLargestChunk );
				bContinue = true;
				break;
			}
			catch ( const std::bad_alloc & /*_eE*/ ) {}
			uiLargestChunk = std::max( uiLargestChunk, 1024ULL * 1024ULL ) - 1024ULL * 1024ULL;
			if ( !uiLargestChunk && !bContinue ) {
				// Unable to allocate any memory at all??  In practice this code can't be executed.
				return false;
			}
		}


		// At this point vBuffer.size() is guaranteed not to be 0.
		CSearchResultLocker srlSearchLock( m_psrbCurTmpSearchResults );
		for ( size_t I = 0; I < _aclChunks.Chunks().size(); ++I ) {
			for ( uint64_t J = 0; J < _aclChunks.Chunks()[I].ui64Len; ) {
				// Buffer as much of the chunk as possible.
				size_t sSize = std::min( static_cast<size_t>(_aclChunks.Chunks()[I].ui64Len - J), vBuffer.size() );
				if ( m_ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(_aclChunks.Chunks()[I].ui64Start + J), &vBuffer[0], sSize, nullptr ) ) {
					PreprocessByteSwap( vBuffer.data(), sSize, _spParms );

					// Go over the chunk and compare.
					size_t sEnd = sSize;
					for ( uint64_t K = 0; K < sEnd; ) {
						size_t sLen;
						if ( Cmp_DataArrayCompare_MultiType( reinterpret_cast<const void *>(_spParms.vStringSearchValue.data()),
							_spParms.vStringMetaData,
							&vBuffer[K], sEnd - K,
							sLen ) ) {
							if ( !m_psrbCurTmpSearchResults->Add( _aclChunks.Chunks()[I].ui64Start + J + K,
								sLen, (_spParms.dtStringCharType << 8) | _spParms.sstSearchType ) ) {
								return false;
							}
						}
						K += _spParms.ui32Alignment;
					}

					J += sSize;
				}
				else {
					break;
				}
			}

			if ( !(I & 0x3) && _hProgressUpdate ) {
				::SendNotifyMessageW( _hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>(((I + 1.0) / _aclChunks.Chunks().size()) * 100.0), 0L );
			}
			if ( _spParms.pbAbort && (*_spParms.pbAbort) ) { break; }
		}

#endif	// #ifndef MX_SINGLE_THREAD_SEARCH
		// ====================
		// ====================

		return bAllPass;
	}

	// A regular-expression search.
	bool CSearcher::RegexSearch( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate ) {
		delete m_psrbCurTmpSearchResults;
		m_psrbCurTmpSearchResults = new ( std::nothrow ) CSearchResultRef();
		if ( !m_psrbCurTmpSearchResults ) { return false; }


		// ================
		// == Alignment. ==
		// ================
		_spParms.ui32Alignment = 1;
		// ================
		// ================


		// ====================
		// == Do the search. ==
		// ====================
		bool bAllPass = true;
#ifndef MX_SINGLE_THREAD_SEARCH
		const size_t stThreads = 32;
		struct {
			CThread						tThread;
			CSearchResultRef			srResults;
			MX_RAW_ARRAY_SEARCH_THREAD	pstSearchThreadParms;
		} aThreadStruct[stThreads];
		LONG lProgTotal = 0;
		size_t stMax = std::min( _aclChunks.Chunks().size(), stThreads );
		for ( size_t I = 0; I < stMax; ++I ) {
			aThreadStruct[I].pstSearchThreadParms.hProgressUpdate = _hProgressUpdate;
			aThreadStruct[I].pstSearchThreadParms.paclChunks = &_aclChunks;
			aThreadStruct[I].pstSearchThreadParms.pspParms = &_spParms;
			aThreadStruct[I].pstSearchThreadParms.psrbResults = &aThreadStruct[I].srResults;
			aThreadStruct[I].pstSearchThreadParms.ppProcess = m_ppProcess;
			aThreadStruct[I].pstSearchThreadParms.stStartIdx = (_aclChunks.Chunks().size() * I / stMax);
			aThreadStruct[I].pstSearchThreadParms.stEndIdx = (_aclChunks.Chunks().size() * (I + 1) / stMax);
			aThreadStruct[I].pstSearchThreadParms.i32Result = 0;
			aThreadStruct[I].pstSearchThreadParms.plProgress = &lProgTotal;
			
			aThreadStruct[I].tThread.CreateThread( StringRegexSearchThreadProc, &aThreadStruct[I].pstSearchThreadParms );
		}
		
		MX_THREADED_GATHER( CSearchResultRef );
#else
		{
			// =======================================
			// == Prepare the string for searching. ==
			// =======================================
			COnigurumaRegex orRegex( _spParms.vStringSearchValue.data(), _spParms.vStringSearchValue.size(),
				COnigurumaSystem::RegexOptionsToLibrary( _spParms.ui32RegexFlags ),
				COnigurumaSystem::RegexEncodingToLibrary( _spParms.reRegexEncoding ),
				COnigurumaSystem::RegexFlavorToLibrary( _spParms.rfRegexFlavor ) );
			if ( orRegex.Get() != ONIG_NORMAL ) {
				/*
				char s[ONIG_MAX_ERROR_MESSAGE_LEN];
				onig_error_code_to_str((UChar* )s, r, &einfo);
				fprintf(stderr, "ERROR: %s\n", s);
				*/
				return false;
			}

			COnigurumaRegion orRegion;
			// Local raw buffer.
			std::vector<uint8_t> vBuffer;
			// Pre-op buffers.
			uint64_t uiLargestChunk = std::max( _aclChunks.LargestChunk(), 1ULL );
			bool bContinue = false;
			while ( !bContinue ) {
				bContinue = false;
				try {
					vBuffer.resize( uiLargestChunk );
					bContinue = true;
					break;
				}
				catch ( const std::bad_alloc & /*_eE*/ ) {}
				uiLargestChunk = std::max( uiLargestChunk, 1024ULL * 1024ULL ) - 1024ULL * 1024ULL;
				if ( !uiLargestChunk && !bContinue ) {
					// Unable to allocate any memory at all??  In practice this code can't be executed.
					return false;
				}
			}


			// At this point vBuffer.size() is guaranteed not to be 0.
			CSearchResultLocker srlSearchLock( m_psrbCurTmpSearchResults );
			for ( size_t I = 0; I < _aclChunks.Chunks().size(); ++I ) {
				for ( uint64_t J = 0; J < _aclChunks.Chunks()[I].ui64Len; ) {
					// Buffer as much of the chunk as possible.
					size_t sSize = std::min( static_cast<size_t>(_aclChunks.Chunks()[I].ui64Len - J), vBuffer.size() );
					if ( m_ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(_aclChunks.Chunks()[I].ui64Start + J), &vBuffer[0], sSize, nullptr ) ) {
						PreprocessByteSwap( vBuffer.data(), sSize, _spParms );

						// Go over the chunk and compare.
						size_t sEnd = sSize;
						for ( uint64_t K = 0; K < sEnd; ) {
							size_t sLen;
							size_t sAdjust = _spParms.ui32Alignment;
							int iMatch = orRegex.Match( vBuffer.data(), sSize, K, orRegion.Get(), 0 );
							if ( iMatch > 0 ) {
								for ( int iRegs = 0; iRegs < orRegion.Get()->num_regs; ++iRegs ) {
									sLen = orRegion.Get()->end[iRegs] - orRegion.Get()->beg[iRegs];
									sAdjust = std::max( sAdjust, sLen );
									if ( !m_psrbCurTmpSearchResults->Add( _aclChunks.Chunks()[I].ui64Start + J + K,
										sLen, (_spParms.reRegexEncoding << 8) | _spParms.sstSearchType ) ) {
										return false;
									}
								}
							}
							K += sAdjust;
						}

						J += sSize;
					}
					else {
						break;
					}
				}

				if ( !(I & 0x3) && _hProgressUpdate ) {
					::SendNotifyMessageW( _hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>(((I + 1.0) / _aclChunks.Chunks().size()) * 100.0), 0L );
				}
				if ( _spParms.pbAbort && (*_spParms.pbAbort) ) { break; }
			}
		}
#endif	// #ifndef MX_SINGLE_THREAD_SEARCH
		// ====================
		// ====================

		return bAllPass;
	}

	// An Expression Search where ?? is a predefined type.
	template <typename _tType, unsigned _sIsFloat16>
	bool CSearcher::ExpressionSearchPredefinedType( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate ) {
		delete m_psrbCurTmpSearchResults;
		m_psrbCurTmpSearchResults = new ( std::nothrow ) CSearchResult1();
		if ( !m_psrbCurTmpSearchResults ) { return false; }

		const uint32_t ui32Size = sizeof( _tType );


		bool bAllPass = true;
#ifndef MX_SINGLE_THREAD_SEARCH
		const size_t stThreads = 32;
		struct {
			CThread						tThread;
			CSearchResult1				srResults;
			MX_EXP_SEARCH_THREAD		pstSearchThreadParms;
		} aThreadStruct[stThreads];
		LONG lProgTotal = 0;
		size_t stMax = std::min( _aclChunks.Chunks().size(), stThreads );
		for ( size_t I = 0; I < stMax; ++I ) {
			aThreadStruct[I].pstSearchThreadParms.hProgressUpdate = _hProgressUpdate;
			aThreadStruct[I].pstSearchThreadParms.paclChunks = &_aclChunks;
			aThreadStruct[I].pstSearchThreadParms.pspParms = &_spParms;
			aThreadStruct[I].pstSearchThreadParms.psrbResults = &aThreadStruct[I].srResults;
			aThreadStruct[I].pstSearchThreadParms.ppProcess = m_ppProcess;
			aThreadStruct[I].pstSearchThreadParms.stStartIdx = (_aclChunks.Chunks().size() * I / stMax);
			aThreadStruct[I].pstSearchThreadParms.stEndIdx = (_aclChunks.Chunks().size() * (I + 1) / stMax);
			aThreadStruct[I].pstSearchThreadParms.i32Result = 0;
			aThreadStruct[I].pstSearchThreadParms.plProgress = &lProgTotal;
			aThreadStruct[I].pstSearchThreadParms.ui32Size = ui32Size;
			
			aThreadStruct[I].tThread.CreateThread( PredefExpressionSearchThreadProc<_tType, _sIsFloat16>, &aThreadStruct[I].pstSearchThreadParms );
		}
		MX_THREADED_GATHER( CSearchResult1 );
#else
		// =================
		// == Expression. ==
		// =================
		CExpression eExp;
		if ( !eExp.SetExpression( _spParms.ssExpression.c_str() ) ) { return false; }
		if ( !eExp.GetContainer() ) { return false; }
		if ( !eExp.GetContainer()->HasAccessToUserData() ) { return false; }
		// Since the address is not given to the script, we can't make meaningful predictions about what it will try to access.
		// Don't try to optimize it; just let it read what it reads.
		eExp.GetContainer()->SetAddressHandler( ExpAddressHandler, reinterpret_cast<uintptr_t>(m_ppProcess) );

		// UserVarHandler just copies whatever the uintptr_t points to, so just point it to rRes and then just update rRes during the search.
		ee::CExpEvalContainer::EE_RESULT rRes;
		eExp.GetContainer()->SetUserHandler( UserVarHandler, reinterpret_cast<uintptr_t>(&rRes) );
		bool bIsFloat = false;
		bool bIsSigned = false;
		if ( CUtilities::DataTypeIsFloat( _spParms.dtLVal.dtType ) ) { rRes.ncType = ee::EE_NC_FLOATING; bIsFloat = true; }
		else if ( CUtilities::DataTypeIsSigned( _spParms.dtLVal.dtType ) ) { rRes.ncType = ee::EE_NC_SIGNED; bIsSigned = true; }
		else { rRes.ncType = ee::EE_NC_UNSIGNED; }
		// =================
		// =================


		// ====================
		// == Do the search. ==
		// ====================
		std::vector<uint8_t> vBuffer;
		uint64_t uiLargestChunk = std::max( _aclChunks.LargestChunk(), static_cast<uint64_t>(ui32Size) );
		while ( !vBuffer.size() ) {
			try {
				vBuffer.resize( uiLargestChunk );
			}
			catch ( const std::bad_alloc & /*_eE*/ ) {}
			uiLargestChunk = std::max( uiLargestChunk, 1024ULL * 1024ULL ) - 1024ULL * 1024ULL;
			if ( !uiLargestChunk && !vBuffer.size() ) {
				// Unable to allocate any memory at all??  In practice this code can't be executed.
				return false;
			}
		}
		// At this point vBuffer.size() is guaranteed not to be 0.
		CSearchResultLocker srlSearchLock( m_psrbCurTmpSearchResults );
		for ( size_t I = 0; I < _aclChunks.Chunks().size(); ++I ) {
			for ( uint64_t J = 0; J < _aclChunks.Chunks()[I].ui64Len; ) {
				// Buffer as much of the chunk as possible.
				size_t sSize = std::min( _aclChunks.Chunks()[I].ui64Len - J, vBuffer.size() );
				if ( sSize < ui32Size ) { break; }
				if ( m_ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(_aclChunks.Chunks()[I].ui64Start + J), &vBuffer[0], sSize, nullptr ) ) {
					PreprocessByteSwap( vBuffer.data(), sSize, _spParms );
					// Go over the chunk and compare.
					size_t sEnd = sSize - ui32Size;
					for ( uint64_t K = 0; K <= sEnd; ) {
						if ( _sIsFloat16 ) {
							rRes.u.dVal = CFloat16( (*reinterpret_cast<uint16_t *>(&vBuffer[K])) ).Value();
						}
						else {
							if ( bIsFloat ) {
								rRes.u.dVal = (*reinterpret_cast<_tType *>(&vBuffer[K]));
							}
							else if ( bIsSigned ) {
								rRes.u.i64Val = (*reinterpret_cast<_tType *>(&vBuffer[K]));
							}
							else {
								rRes.u.ui64Val = (*reinterpret_cast<_tType *>(&vBuffer[K]));
							}
						}
						ee::CExpEvalContainer::EE_RESULT rThisRes;
						if ( eExp.GetContainer()->Resolve( rThisRes ) ) {
							rThisRes = ee::CExpEvalContainer::ConvertResult( rThisRes, ee::EE_NC_UNSIGNED );
							if ( rThisRes.u.ui64Val ) {
								if ( !m_psrbCurTmpSearchResults->Add( _aclChunks.Chunks()[I].ui64Start + J + K,
									&vBuffer[K], ui32Size ) ) {
									return false;
								}
							}
						}
						/*if ( evcCompFunc( _spParms.dtLVal, (*reinterpret_cast<CUtilities::MX_DATA_TYPE *>(&vBuffer[K])), _spParms ) ^ _spParms.bInvertResults ) {
							if ( !m_psrbCurTmpSearchResults->Add( _aclChunks.Chunks()[I].ui64Start + J + K,
								&vBuffer[K], ui32Size ) ) {
								return false;
							}
						}*/
						K += _spParms.ui32Alignment;
					}

					J += sSize;
				}
				else {
					break;
				}
			}

			if ( !(I & 0x3) && _hProgressUpdate ) {
				::SendNotifyMessageW( _hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>(((I + 1.0) / _aclChunks.Chunks().size()) * 100.0), 0L );
			}
			if ( _spParms.pbAbort && (*_spParms.pbAbort) ) { break; }
		}
#endif	// #ifndef MX_SINGLE_THREAD_SEARCH

		return bAllPass;
	}

	// An Expression Search where ?? is an address.
	bool CSearcher::ExpressionSearchAddressFixedSize( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate ) {
		delete m_psrbCurTmpSearchResults;
		m_psrbCurTmpSearchResults = new ( std::nothrow ) CSearchResult1();
		if ( !m_psrbCurTmpSearchResults ) { return false; }

		const uint32_t ui32Size = _spParms.ui32ExpSearchSize;


		bool bAllPass = true;
#ifndef MX_SINGLE_THREAD_SEARCH
		const size_t stThreads = 32;
		struct {
			CThread						tThread;
			CSearchResult1				srResults;
			MX_EXP_SEARCH_THREAD		pstSearchThreadParms;
		} aThreadStruct[stThreads];
		LONG lProgTotal = 0;
		size_t stMax = std::min( _aclChunks.Chunks().size(), stThreads );
		for ( size_t I = 0; I < stMax; ++I ) {
			aThreadStruct[I].pstSearchThreadParms.hProgressUpdate = _hProgressUpdate;
			aThreadStruct[I].pstSearchThreadParms.paclChunks = &_aclChunks;
			aThreadStruct[I].pstSearchThreadParms.pspParms = &_spParms;
			aThreadStruct[I].pstSearchThreadParms.psrbResults = &aThreadStruct[I].srResults;
			aThreadStruct[I].pstSearchThreadParms.ppProcess = m_ppProcess;
			aThreadStruct[I].pstSearchThreadParms.stStartIdx = (_aclChunks.Chunks().size() * I / stMax);
			aThreadStruct[I].pstSearchThreadParms.stEndIdx = (_aclChunks.Chunks().size() * (I + 1) / stMax);
			aThreadStruct[I].pstSearchThreadParms.i32Result = 0;
			aThreadStruct[I].pstSearchThreadParms.plProgress = &lProgTotal;
			aThreadStruct[I].pstSearchThreadParms.ui32Size = ui32Size;
			
			aThreadStruct[I].tThread.CreateThread( FixedSizeExpressionSearchThreadProc, &aThreadStruct[I].pstSearchThreadParms );
		}
		MX_THREADED_GATHER( CSearchResult1 );
#else
		// =================
		// == Expression. ==
		// =================
		CExpression eExp;
		if ( !eExp.SetExpression( _spParms.ssExpression.c_str() ) ) { return false; }
		if ( !eExp.GetContainer() ) { return false; }
		if ( !eExp.GetContainer()->HasAccessToUserData() ) { return false; }
		if ( !eExp.GetContainer()->HasAddressRead() ) { return false; }
		// The given address is expected to be dereferenced plus likely neighboring addresses, so we can use an address handler
		//	with a few extra features to improve performance.
		eExp.GetContainer()->SetAddressHandler( ExpAddressHandler, reinterpret_cast<uintptr_t>(m_ppProcess) );

		// UserVarHandler just copies whatever the uintptr_t points to, so just point it to rRes and then just update rRes during the search.
		ee::CExpEvalContainer::EE_RESULT rRes;
		eExp.GetContainer()->SetUserHandler( UserVarHandler, reinterpret_cast<uintptr_t>(&rRes) );
		rRes.ncType = ee::EE_NC_UNSIGNED;
		// =================
		// =================


		// ====================
		// == Do the search. ==
		// ====================
		std::vector<uint8_t> vBuffer;
		uint64_t uiLargestChunk = std::max( _aclChunks.LargestChunk(), static_cast<uint64_t>(ui32Size) );
		while ( !vBuffer.size() ) {
			try {
				vBuffer.resize( uiLargestChunk );
			}
			catch ( const std::bad_alloc & /*_eE*/ ) {}
			uiLargestChunk = std::max( uiLargestChunk, 1024ULL * 1024ULL ) - 1024ULL * 1024ULL;
			if ( !uiLargestChunk && !vBuffer.size() ) {
				// Unable to allocate any memory at all??  In practice this code can't be executed.
				return false;
			}
		}
		// At this point vBuffer.size() is guaranteed not to be 0.
		CSearchResultLocker srlSearchLock( m_psrbCurTmpSearchResults );
		for ( size_t I = 0; I < _aclChunks.Chunks().size(); ++I ) {
			for ( uint64_t J = 0; J < _aclChunks.Chunks()[I].ui64Len; ) {
				// Buffer as much of the chunk as possible.
				size_t sSize = std::min( _aclChunks.Chunks()[I].ui64Len - J, vBuffer.size() );
				if ( sSize < ui32Size ) { break; }
				if ( m_ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(_aclChunks.Chunks()[I].ui64Start + J), &vBuffer[0], sSize, nullptr ) ) {
					PreprocessByteSwap( vBuffer.data(), sSize, _spParms );
					// Go over the chunk and compare.
					size_t sEnd = sSize - ui32Size;
					for ( uint64_t K = 0; K <= sEnd; ) {
						rRes.u.ui64Val = _aclChunks.Chunks()[I].ui64Start + J + K;
						ee::CExpEvalContainer::EE_RESULT rThisRes;
						if ( eExp.GetContainer()->Resolve( rThisRes ) ) {
							rThisRes = ee::CExpEvalContainer::ConvertResult( rThisRes, ee::EE_NC_UNSIGNED );
							if ( rThisRes.u.ui64Val ) {
								if ( !m_psrbCurTmpSearchResults->Add( _aclChunks.Chunks()[I].ui64Start + J + K,
									&vBuffer[K], ui32Size ) ) {
									return false;
								}
							}
						}
						K += _spParms.ui32Alignment;
					}

					J += sSize;
				}
				else {
					break;
				}
			}

			if ( !(I & 0x3) && _hProgressUpdate ) {
				::SendNotifyMessageW( _hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>(((I + 1.0) / _aclChunks.Chunks().size()) * 100.0), 0L );
			}
			if ( _spParms.pbAbort && (*_spParms.pbAbort) ) { break; }
		}
#endif	// #ifndef MX_SINGLE_THREAD_SEARCH

		return bAllPass;
	}

	// An Expression Search where ?? is an address and the size depends on the script return value.
	bool CSearcher::ExpressionSearchAddressDynamicSize( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate ) {
		delete m_psrbCurTmpSearchResults;
		m_psrbCurTmpSearchResults = new ( std::nothrow ) CSearchResultRef();
		if ( !m_psrbCurTmpSearchResults ) { return false; }

		const uint32_t ui32Size = 1;


		bool bAllPass = true;
#ifndef MX_SINGLE_THREAD_SEARCH
		const size_t stThreads = 32;
		struct {
			CThread						tThread;
			CSearchResultRef			srResults;
			MX_EXP_SEARCH_THREAD		pstSearchThreadParms;
		} aThreadStruct[stThreads];
		LONG lProgTotal = 0;
		size_t stMax = std::min( _aclChunks.Chunks().size(), stThreads );
		for ( size_t I = 0; I < stMax; ++I ) {
			aThreadStruct[I].pstSearchThreadParms.hProgressUpdate = _hProgressUpdate;
			aThreadStruct[I].pstSearchThreadParms.paclChunks = &_aclChunks;
			aThreadStruct[I].pstSearchThreadParms.pspParms = &_spParms;
			aThreadStruct[I].pstSearchThreadParms.psrbResults = &aThreadStruct[I].srResults;
			aThreadStruct[I].pstSearchThreadParms.ppProcess = m_ppProcess;
			aThreadStruct[I].pstSearchThreadParms.stStartIdx = (_aclChunks.Chunks().size() * I / stMax);
			aThreadStruct[I].pstSearchThreadParms.stEndIdx = (_aclChunks.Chunks().size() * (I + 1) / stMax);
			aThreadStruct[I].pstSearchThreadParms.i32Result = 0;
			aThreadStruct[I].pstSearchThreadParms.plProgress = &lProgTotal;
			aThreadStruct[I].pstSearchThreadParms.ui32Size = ui32Size;
			
			aThreadStruct[I].tThread.CreateThread( DynamicSizeExpressionSearchThreadProc, &aThreadStruct[I].pstSearchThreadParms );
		}
		MX_THREADED_GATHER( CSearchResultRef );
#else
		// =================
		// == Expression. ==
		// =================
		CExpression eExp;
		if ( !eExp.SetExpression( _spParms.ssExpression.c_str() ) ) { return false; }
		if ( !eExp.GetContainer() ) { return false; }
		if ( !eExp.GetContainer()->HasAccessToUserData() ) { return false; }
		if ( !eExp.GetContainer()->HasAddressRead() ) { return false; }
		// The given address is expected to be dereferenced plus likely neighboring addresses, so we can use an address handler
		//	with a few extra features to improve performance.
		eExp.GetContainer()->SetAddressHandler( ExpAddressHandler, reinterpret_cast<uintptr_t>(m_ppProcess) );

		// UserVarHandler just copies whatever the uintptr_t points to, so just point it to rRes and then just update rRes during the search.
		ee::CExpEvalContainer::EE_RESULT rRes;
		eExp.GetContainer()->SetUserHandler( UserVarHandler, reinterpret_cast<uintptr_t>(&rRes) );
		rRes.ncType = ee::EE_NC_UNSIGNED;
		// =================
		// =================


		// ====================
		// == Do the search. ==
		// ====================
		std::vector<uint8_t> vBuffer;
		uint64_t uiLargestChunk = std::max( _aclChunks.LargestChunk(), static_cast<uint64_t>(ui32Size) );
		while ( !vBuffer.size() ) {
			try {
				vBuffer.resize( uiLargestChunk );
			}
			catch ( const std::bad_alloc & /*_eE*/ ) {}
			uiLargestChunk = std::max( uiLargestChunk, 1024ULL * 1024ULL ) - 1024ULL * 1024ULL;
			if ( !uiLargestChunk && !vBuffer.size() ) {
				// Unable to allocate any memory at all??  In practice this code can't be executed.
				return false;
			}
		}
		// At this point vBuffer.size() is guaranteed not to be 0.
		CSearchResultLocker srlSearchLock( m_psrbCurTmpSearchResults );
		for ( size_t I = 0; I < _aclChunks.Chunks().size(); ++I ) {
			for ( uint64_t J = 0; J < _aclChunks.Chunks()[I].ui64Len; ) {
				// Buffer as much of the chunk as possible.
				size_t sSize = std::min( _aclChunks.Chunks()[I].ui64Len - J, vBuffer.size() );
				if ( sSize < ui32Size ) { break; }
				if ( m_ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(_aclChunks.Chunks()[I].ui64Start + J), &vBuffer[0], sSize, nullptr ) ) {
					PreprocessByteSwap( vBuffer.data(), sSize, _spParms );
					// Go over the chunk and compare.
					size_t sEnd = sSize - ui32Size;
					for ( uint64_t K = 0; K <= sEnd; ) {
						rRes.u.ui64Val = _aclChunks.Chunks()[I].ui64Start + J + K;
						ee::CExpEvalContainer::EE_RESULT rThisRes;
						if ( eExp.GetContainer()->Resolve( rThisRes ) ) {
							rThisRes = ee::CExpEvalContainer::ConvertResult( rThisRes, ee::EE_NC_UNSIGNED );
							if ( rThisRes.u.ui64Val ) {
								if ( !m_psrbCurTmpSearchResults->Add( _aclChunks.Chunks()[I].ui64Start + J + K,
									rThisRes.u.ui64Val, 0 ) ) {
									return false;
								}
							}
						}
						K += _spParms.ui32Alignment;
					}

					J += sSize;
				}
				else {
					break;
				}
			}

			if ( !(I & 0x3) && _hProgressUpdate ) {
				::SendNotifyMessageW( _hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>(((I + 1.0) / _aclChunks.Chunks().size()) * 100.0), 0L );
			}
			if ( _spParms.pbAbort && (*_spParms.pbAbort) ) { break; }
		}
#endif	// #ifndef MX_SINGLE_THREAD_SEARCH

		return bAllPass;
	}

	// Exact-value subsearch on primitives.
	bool CSearcher::ExactValuePrimitiveSubsearch( MX_SUBSEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate ) {
		delete m_psrbCurTmpSearchResults;
		m_psrbCurTmpSearchResults = new ( std::nothrow ) CSearchResult1();
		if ( !m_psrbCurTmpSearchResults ) { return false; }
		CSearchResultLocker srlSearchLock( m_psrbSearchResults );
		std::vector<MX_SUBSEARCH_DIV> vDivisions;
		if ( !DivideResultsForSubsearch( m_psrbSearchResults, vDivisions, _spParms.ui64MaxChunkLen, _aclChunks ) ) {
			return false;
		}
		_spParms.bInvertResults = _spParms.bInvertResults ? true : false;	// Force to 0 or 1.


		// ==============================================
		// == Create the comparison function and size. ==
		// ==============================================
		MX_EXACT_VAL_COMP evcCompFunc = nullptr;
		uint32_t ui32Size = 0;
		MX_SEARCH_PARMS spSearchParms = SearchObjToSubsearchObj( _spParms );
		if ( !GetExactValCmpFuncAndSize( evcCompFunc, ui32Size, spSearchParms, false ) ) { return false; }
		// ==============================================
		// ==============================================



		// ====================
		// == Do the search. ==
		// ====================
		bool bAllPass = true;
#ifndef MX_SINGLE_THREAD_SEARCH
		const size_t stThreads = 8;
		struct {
			CThread tThread;
			CSearchResult1 srResults;
			MX_PRIM_SEARCH_THREAD pstSearchThreadParms;
		} aThreadStruct[stThreads];
		LONG lProgTotal = 0;
		size_t stMax = std::min( vDivisions.size(), stThreads );
		for ( size_t I = 0; I < stMax; ++I ) {
			aThreadStruct[I].pstSearchThreadParms.hProgressUpdate = _hProgressUpdate;
			aThreadStruct[I].pstSearchThreadParms.paclChunks = &_aclChunks;
			aThreadStruct[I].pstSearchThreadParms.pspParms = &spSearchParms;
			aThreadStruct[I].pstSearchThreadParms.psrbResults = &aThreadStruct[I].srResults;
			aThreadStruct[I].pstSearchThreadParms.ppProcess = m_ppProcess;
			aThreadStruct[I].pstSearchThreadParms.evcCompFunc = evcCompFunc;
			aThreadStruct[I].pstSearchThreadParms.stStartIdx = (vDivisions.size() * I / stMax);
			aThreadStruct[I].pstSearchThreadParms.stEndIdx = (vDivisions.size() * (I + 1) / stMax);
			aThreadStruct[I].pstSearchThreadParms.ui32Size = ui32Size;
			aThreadStruct[I].pstSearchThreadParms.i32Result = 0;
			aThreadStruct[I].pstSearchThreadParms.plProgress = &lProgTotal;
			aThreadStruct[I].pstSearchThreadParms.pvSubsearchDiv = &vDivisions;
			aThreadStruct[I].pstSearchThreadParms.psrbPrevResults = m_psrbSearchResults;
			aThreadStruct[I].tThread.CreateThread( PrimitiveExactValueSubsearchThreadProc, &aThreadStruct[I].pstSearchThreadParms );
		}
		MX_THREADED_GATHER( CSearchResult1 );
#endif

		return bAllPass;
	}

	// Not-equal-to subsearch on primitives.
	bool CSearcher::NotEqualToPrimitiveSubsearch( MX_SUBSEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate ) {
		delete m_psrbCurTmpSearchResults;
		m_psrbCurTmpSearchResults = new ( std::nothrow ) CSearchResult1();
		if ( !m_psrbCurTmpSearchResults ) { return false; }
		CSearchResultLocker srlSearchLock( m_psrbSearchResults );
		std::vector<MX_SUBSEARCH_DIV> vDivisions;
		if ( !DivideResultsForSubsearch( m_psrbSearchResults, vDivisions, _spParms.ui64MaxChunkLen, _aclChunks ) ) {
			return false;
		}
		// Swap the invert test because this is a negative search of equal-to values.
		_spParms.bInvertResults = _spParms.bInvertResults ? false : true;	// Force to 0 or 1.


		// ==============================================
		// == Create the comparison function and size. ==
		// ==============================================
		MX_EXACT_VAL_COMP evcCompFunc = nullptr;
		uint32_t ui32Size = 0;
		MX_SEARCH_PARMS spSearchParms = SearchObjToSubsearchObj( _spParms );
		if ( !GetExactValCmpFuncAndSize( evcCompFunc, ui32Size, spSearchParms, false ) ) { return false; }
		// ==============================================
		// ==============================================



		// ====================
		// == Do the search. ==
		// ====================
		bool bAllPass = true;
#ifndef MX_SINGLE_THREAD_SEARCH
		const size_t stThreads = 8;
		struct {
			CThread tThread;
			CSearchResult1 srResults;
			MX_PRIM_SEARCH_THREAD pstSearchThreadParms;
		} aThreadStruct[stThreads];
		LONG lProgTotal = 0;
		size_t stMax = std::min( vDivisions.size(), stThreads );
		for ( size_t I = 0; I < stMax; ++I ) {
			aThreadStruct[I].pstSearchThreadParms.hProgressUpdate = _hProgressUpdate;
			aThreadStruct[I].pstSearchThreadParms.paclChunks = &_aclChunks;
			aThreadStruct[I].pstSearchThreadParms.pspParms = &spSearchParms;
			aThreadStruct[I].pstSearchThreadParms.psrbResults = &aThreadStruct[I].srResults;
			aThreadStruct[I].pstSearchThreadParms.ppProcess = m_ppProcess;
			aThreadStruct[I].pstSearchThreadParms.evcCompFunc = evcCompFunc;
			aThreadStruct[I].pstSearchThreadParms.stStartIdx = (vDivisions.size() * I / stMax);
			aThreadStruct[I].pstSearchThreadParms.stEndIdx = (vDivisions.size() * (I + 1) / stMax);
			aThreadStruct[I].pstSearchThreadParms.ui32Size = ui32Size;
			aThreadStruct[I].pstSearchThreadParms.i32Result = 0;
			aThreadStruct[I].pstSearchThreadParms.plProgress = &lProgTotal;
			aThreadStruct[I].pstSearchThreadParms.pvSubsearchDiv = &vDivisions;
			aThreadStruct[I].pstSearchThreadParms.psrbPrevResults = m_psrbSearchResults;
			aThreadStruct[I].tThread.CreateThread( PrimitiveNotEqualToSubsearchThreadProc, &aThreadStruct[I].pstSearchThreadParms );
		}
		MX_THREADED_GATHER( CSearchResult1 );
#endif

		return bAllPass;
	}

	// Greater-than subsearch on primitives.
	bool CSearcher::IncreasedPrimitiveSubsearch( MX_SUBSEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate ) {
		delete m_psrbCurTmpSearchResults;
		m_psrbCurTmpSearchResults = new ( std::nothrow ) CSearchResult1();
		if ( !m_psrbCurTmpSearchResults ) { return false; }
		CSearchResultLocker srlSearchLock( m_psrbSearchResults );
		std::vector<MX_SUBSEARCH_DIV> vDivisions;
		if ( !DivideResultsForSubsearch( m_psrbSearchResults, vDivisions, _spParms.ui64MaxChunkLen, _aclChunks ) ) {
			return false;
		}
		_spParms.bInvertResults = _spParms.bInvertResults ? true : false;	// Force to 0 or 1.


		// ==============================================
		// == Create the comparison function and size. ==
		// ==============================================
		MX_SEARCH_PARMS spSearchParms = SearchObjToSubsearchObj( _spParms );
		MX_REL_COMP rcCompFunc = nullptr;
		uint32_t ui32Size = 0;
		if ( !GetGreaterThanCmpFuncAndSize( rcCompFunc, ui32Size, spSearchParms, true ) ) { return false; }
		// ==============================================
		// ==============================================



		// ====================
		// == Do the search. ==
		// ====================
		bool bAllPass = true;
#ifndef MX_SINGLE_THREAD_SEARCH
		const size_t stThreads = 8;
		struct {
			CThread tThread;
			CSearchResult1 srResults;
			MX_PRIM_SEARCH_THREAD pstSearchThreadParms;
		} aThreadStruct[stThreads];
		LONG lProgTotal = 0;
		size_t stMax = std::min( vDivisions.size(), stThreads );
		for ( size_t I = 0; I < stMax; ++I ) {
			aThreadStruct[I].pstSearchThreadParms.hProgressUpdate = _hProgressUpdate;
			aThreadStruct[I].pstSearchThreadParms.paclChunks = &_aclChunks;
			aThreadStruct[I].pstSearchThreadParms.pspParms = &spSearchParms;
			aThreadStruct[I].pstSearchThreadParms.psrbResults = &aThreadStruct[I].srResults;
			aThreadStruct[I].pstSearchThreadParms.ppProcess = m_ppProcess;
			aThreadStruct[I].pstSearchThreadParms.rcRelCompFunc = rcCompFunc;
			aThreadStruct[I].pstSearchThreadParms.stStartIdx = (vDivisions.size() * I / stMax);
			aThreadStruct[I].pstSearchThreadParms.stEndIdx = (vDivisions.size() * (I + 1) / stMax);
			aThreadStruct[I].pstSearchThreadParms.ui32Size = ui32Size;
			aThreadStruct[I].pstSearchThreadParms.i32Result = 0;
			aThreadStruct[I].pstSearchThreadParms.plProgress = &lProgTotal;
			aThreadStruct[I].pstSearchThreadParms.pvSubsearchDiv = &vDivisions;
			aThreadStruct[I].pstSearchThreadParms.psrbPrevResults = m_psrbSearchResults;
			aThreadStruct[I].tThread.CreateThread( PrimitiveIncreasedSubsearchThreadProc, &aThreadStruct[I].pstSearchThreadParms );
		}
		MX_THREADED_GATHER( CSearchResult1 );
#endif

		return bAllPass;
	}

	// Greater-than subsearch on primitives.
	bool CSearcher::DecreasedPrimitiveSubsearch( MX_SUBSEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate ) {
		delete m_psrbCurTmpSearchResults;
		m_psrbCurTmpSearchResults = new ( std::nothrow ) CSearchResult1();
		if ( !m_psrbCurTmpSearchResults ) { return false; }
		CSearchResultLocker srlSearchLock( m_psrbSearchResults );
		std::vector<MX_SUBSEARCH_DIV> vDivisions;
		if ( !DivideResultsForSubsearch( m_psrbSearchResults, vDivisions, _spParms.ui64MaxChunkLen, _aclChunks ) ) {
			return false;
		}
		_spParms.bInvertResults = _spParms.bInvertResults ? true : false;	// Force to 0 or 1.


		// ==============================================
		// == Create the comparison function and size. ==
		// ==============================================
		MX_SEARCH_PARMS spSearchParms = SearchObjToSubsearchObj( _spParms );
		MX_REL_COMP rcCompFunc = nullptr;
		uint32_t ui32Size = 0;
		if ( !GetLessThanCmpFuncAndSize( rcCompFunc, ui32Size, spSearchParms, true ) ) { return false; }
		// ==============================================
		// ==============================================



		// ====================
		// == Do the search. ==
		// ====================
		bool bAllPass = true;
#ifndef MX_SINGLE_THREAD_SEARCH
		const size_t stThreads = 8;
		struct {
			CThread tThread;
			CSearchResult1 srResults;
			MX_PRIM_SEARCH_THREAD pstSearchThreadParms;
		} aThreadStruct[stThreads];
		LONG lProgTotal = 0;
		size_t stMax = std::min( vDivisions.size(), stThreads );
		for ( size_t I = 0; I < stMax; ++I ) {
			aThreadStruct[I].pstSearchThreadParms.hProgressUpdate = _hProgressUpdate;
			aThreadStruct[I].pstSearchThreadParms.paclChunks = &_aclChunks;
			aThreadStruct[I].pstSearchThreadParms.pspParms = &spSearchParms;
			aThreadStruct[I].pstSearchThreadParms.psrbResults = &aThreadStruct[I].srResults;
			aThreadStruct[I].pstSearchThreadParms.ppProcess = m_ppProcess;
			aThreadStruct[I].pstSearchThreadParms.rcRelCompFunc = rcCompFunc;
			aThreadStruct[I].pstSearchThreadParms.stStartIdx = (vDivisions.size() * I / stMax);
			aThreadStruct[I].pstSearchThreadParms.stEndIdx = (vDivisions.size() * (I + 1) / stMax);
			aThreadStruct[I].pstSearchThreadParms.ui32Size = ui32Size;
			aThreadStruct[I].pstSearchThreadParms.i32Result = 0;
			aThreadStruct[I].pstSearchThreadParms.plProgress = &lProgTotal;
			aThreadStruct[I].pstSearchThreadParms.pvSubsearchDiv = &vDivisions;
			aThreadStruct[I].pstSearchThreadParms.psrbPrevResults = m_psrbSearchResults;
			aThreadStruct[I].tThread.CreateThread( PrimitiveDecreasedSubsearchThreadProc, &aThreadStruct[I].pstSearchThreadParms );
		}
		MX_THREADED_GATHER( CSearchResult1 );
#endif

		return bAllPass;
	}

	// In-range subsearch on primitives.
	bool CSearcher::InRangePrimitiveSubsearch( MX_SUBSEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate ) {
		delete m_psrbCurTmpSearchResults;
		m_psrbCurTmpSearchResults = new ( std::nothrow ) CSearchResult1();
		if ( !m_psrbCurTmpSearchResults ) { return false; }
		CSearchResultLocker srlSearchLock( m_psrbSearchResults );
		std::vector<MX_SUBSEARCH_DIV> vDivisions;
		if ( !DivideResultsForSubsearch( m_psrbSearchResults, vDivisions, _spParms.ui64MaxChunkLen, _aclChunks ) ) {
			return false;
		}
		_spParms.bInvertResults = _spParms.bInvertResults ? true : false;	// Force to 0 or 1.


		// ==============================================
		// == Create the comparison function and size. ==
		// ==============================================
		MX_SEARCH_PARMS spSearchParms = SearchObjToSubsearchObj( _spParms );
		MX_RANGE_COMP rcCompFunc = nullptr;
		uint32_t ui32Size = 0;
		if ( !GetRangeCmpFuncAndSize( rcCompFunc, ui32Size, spSearchParms, false ) ) { return false; }
		// ==============================================
		// ==============================================



		// ====================
		// == Do the search. ==
		// ====================
		bool bAllPass = true;
#ifndef MX_SINGLE_THREAD_SEARCH
		const size_t stThreads = 8;
		struct {
			CThread tThread;
			CSearchResult1 srResults;
			MX_PRIM_SEARCH_THREAD pstSearchThreadParms;
		} aThreadStruct[stThreads];
		LONG lProgTotal = 0;
		size_t stMax = std::min( vDivisions.size(), stThreads );
		for ( size_t I = 0; I < stMax; ++I ) {
			aThreadStruct[I].pstSearchThreadParms.hProgressUpdate = _hProgressUpdate;
			aThreadStruct[I].pstSearchThreadParms.paclChunks = &_aclChunks;
			aThreadStruct[I].pstSearchThreadParms.pspParms = &spSearchParms;
			aThreadStruct[I].pstSearchThreadParms.psrbResults = &aThreadStruct[I].srResults;
			aThreadStruct[I].pstSearchThreadParms.ppProcess = m_ppProcess;
			aThreadStruct[I].pstSearchThreadParms.rcRangeCompFunc = rcCompFunc;
			aThreadStruct[I].pstSearchThreadParms.stStartIdx = (vDivisions.size() * I / stMax);
			aThreadStruct[I].pstSearchThreadParms.stEndIdx = (vDivisions.size() * (I + 1) / stMax);
			aThreadStruct[I].pstSearchThreadParms.ui32Size = ui32Size;
			aThreadStruct[I].pstSearchThreadParms.i32Result = 0;
			aThreadStruct[I].pstSearchThreadParms.plProgress = &lProgTotal;
			aThreadStruct[I].pstSearchThreadParms.pvSubsearchDiv = &vDivisions;
			aThreadStruct[I].pstSearchThreadParms.psrbPrevResults = m_psrbSearchResults;
			aThreadStruct[I].tThread.CreateThread( PrimitiveInRangeSubsearchThreadProc, &aThreadStruct[I].pstSearchThreadParms );
		}
		MX_THREADED_GATHER( CSearchResult1 );
#endif

		return bAllPass;
	}

	// In-range subsearch on primitives.
	bool CSearcher::SameAsBeforePrimitiveSubsearch( MX_SUBSEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate ) {
		delete m_psrbCurTmpSearchResults;
		m_psrbCurTmpSearchResults = new ( std::nothrow ) CSearchResult1();
		if ( !m_psrbCurTmpSearchResults ) { return false; }
		CSearchResultLocker srlSearchLock( m_psrbSearchResults );
		std::vector<MX_SUBSEARCH_DIV> vDivisions;
		if ( !DivideResultsForSubsearch( m_psrbSearchResults, vDivisions, _spParms.ui64MaxChunkLen, _aclChunks ) ) {
			return false;
		}
		_spParms.bInvertResults = _spParms.bInvertResults ? true : false;	// Force to 0 or 1.


		// ==============================================
		// == Create the comparison function and size. ==
		// ==============================================
		MX_EXACT_VAL_COMP evcCompFunc = nullptr;
		uint32_t ui32Size = 0;
		MX_SEARCH_PARMS spSearchParms = SearchObjToSubsearchObj( _spParms );
		if ( !GetExactValCmpFuncAndSize( evcCompFunc, ui32Size, spSearchParms, true ) ) { return false; }
		// ==============================================
		// ==============================================



		// ====================
		// == Do the search. ==
		// ====================
		bool bAllPass = true;
#ifndef MX_SINGLE_THREAD_SEARCH
		const size_t stThreads = 8;
		struct {
			CThread tThread;
			CSearchResult1 srResults;
			MX_PRIM_SEARCH_THREAD pstSearchThreadParms;
		} aThreadStruct[stThreads];
		LONG lProgTotal = 0;
		size_t stMax = std::min( vDivisions.size(), stThreads );
		for ( size_t I = 0; I < stMax; ++I ) {
			aThreadStruct[I].pstSearchThreadParms.hProgressUpdate = _hProgressUpdate;
			aThreadStruct[I].pstSearchThreadParms.paclChunks = &_aclChunks;
			aThreadStruct[I].pstSearchThreadParms.pspParms = &spSearchParms;
			aThreadStruct[I].pstSearchThreadParms.psrbResults = &aThreadStruct[I].srResults;
			aThreadStruct[I].pstSearchThreadParms.ppProcess = m_ppProcess;
			aThreadStruct[I].pstSearchThreadParms.evcCompFunc = evcCompFunc;
			aThreadStruct[I].pstSearchThreadParms.stStartIdx = (vDivisions.size() * I / stMax);
			aThreadStruct[I].pstSearchThreadParms.stEndIdx = (vDivisions.size() * (I + 1) / stMax);
			aThreadStruct[I].pstSearchThreadParms.ui32Size = ui32Size;
			aThreadStruct[I].pstSearchThreadParms.i32Result = 0;
			aThreadStruct[I].pstSearchThreadParms.plProgress = &lProgTotal;
			aThreadStruct[I].pstSearchThreadParms.pvSubsearchDiv = &vDivisions;
			aThreadStruct[I].pstSearchThreadParms.psrbPrevResults = m_psrbSearchResults;
			aThreadStruct[I].tThread.CreateThread( PrimitiveSameAsBeforeSubsearchThreadProc, &aThreadStruct[I].pstSearchThreadParms );
		}
		MX_THREADED_GATHER( CSearchResult1 );
#endif

		return bAllPass;
	}

	// In-range subsearch on primitives.
	bool CSearcher::DifferentFromBeforePrimitiveSubsearch( MX_SUBSEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate ) {
		delete m_psrbCurTmpSearchResults;
		m_psrbCurTmpSearchResults = new ( std::nothrow ) CSearchResult1();
		if ( !m_psrbCurTmpSearchResults ) { return false; }
		CSearchResultLocker srlSearchLock( m_psrbSearchResults );
		std::vector<MX_SUBSEARCH_DIV> vDivisions;
		if ( !DivideResultsForSubsearch( m_psrbSearchResults, vDivisions, _spParms.ui64MaxChunkLen, _aclChunks ) ) {
			return false;
		}
		// Swap the invert test because this is a negative search of same-as-before values.
		_spParms.bInvertResults = _spParms.bInvertResults ? false : true;	// Force to 0 or 1.


		// ==============================================
		// == Create the comparison function and size. ==
		// ==============================================
		MX_EXACT_VAL_COMP evcCompFunc = nullptr;
		uint32_t ui32Size = 0;
		MX_SEARCH_PARMS spSearchParms = SearchObjToSubsearchObj( _spParms );
		if ( !GetExactValCmpFuncAndSize( evcCompFunc, ui32Size, spSearchParms, true ) ) { return false; }
		// ==============================================
		// ==============================================



		// ====================
		// == Do the search. ==
		// ====================
		bool bAllPass = true;
#ifndef MX_SINGLE_THREAD_SEARCH
		const size_t stThreads = 8;
		struct {
			CThread tThread;
			CSearchResult1 srResults;
			MX_PRIM_SEARCH_THREAD pstSearchThreadParms;
		} aThreadStruct[stThreads];
		LONG lProgTotal = 0;
		size_t stMax = std::min( vDivisions.size(), stThreads );
		for ( size_t I = 0; I < stMax; ++I ) {
			aThreadStruct[I].pstSearchThreadParms.hProgressUpdate = _hProgressUpdate;
			aThreadStruct[I].pstSearchThreadParms.paclChunks = &_aclChunks;
			aThreadStruct[I].pstSearchThreadParms.pspParms = &spSearchParms;
			aThreadStruct[I].pstSearchThreadParms.psrbResults = &aThreadStruct[I].srResults;
			aThreadStruct[I].pstSearchThreadParms.ppProcess = m_ppProcess;
			aThreadStruct[I].pstSearchThreadParms.evcCompFunc = evcCompFunc;
			aThreadStruct[I].pstSearchThreadParms.stStartIdx = (vDivisions.size() * I / stMax);
			aThreadStruct[I].pstSearchThreadParms.stEndIdx = (vDivisions.size() * (I + 1) / stMax);
			aThreadStruct[I].pstSearchThreadParms.ui32Size = ui32Size;
			aThreadStruct[I].pstSearchThreadParms.i32Result = 0;
			aThreadStruct[I].pstSearchThreadParms.plProgress = &lProgTotal;
			aThreadStruct[I].pstSearchThreadParms.pvSubsearchDiv = &vDivisions;
			aThreadStruct[I].pstSearchThreadParms.psrbPrevResults = m_psrbSearchResults;
			aThreadStruct[I].tThread.CreateThread( PrimitiveSameAsBeforeSubsearchThreadProc, &aThreadStruct[I].pstSearchThreadParms );
		}
		MX_THREADED_GATHER( CSearchResult1 );
#endif

		return bAllPass;
	}

	// In-range subsearch on primitives.
	//template <bool (CSearcher::* T)( CSearcher::MX_CHANGED_BY &, uint32_t &, CSearcher::MX_SEARCH_PARMS &, bool )>
	bool CSearcher::ChangedByPrimitiveSubsearch( MX_SUBSEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate,
		MX_CHANGED_BY_SELECTOR _cbsFunctor )  {
		delete m_psrbCurTmpSearchResults;
		m_psrbCurTmpSearchResults = new ( std::nothrow ) CSearchResult1();
		if ( !m_psrbCurTmpSearchResults ) { return false; }
		CSearchResultLocker srlSearchLock( m_psrbSearchResults );
		std::vector<MX_SUBSEARCH_DIV> vDivisions;
		if ( !DivideResultsForSubsearch( m_psrbSearchResults, vDivisions, _spParms.ui64MaxChunkLen, _aclChunks ) ) {
			return false;
		}
		_spParms.bInvertResults = _spParms.bInvertResults ? true : false;	// Force to 0 or 1.


		// ==============================================
		// == Create the comparison function and size. ==
		// ==============================================
		MX_CHANGED_BY cbCompFunc = nullptr;
		uint32_t ui32Size = 0;
		MX_SEARCH_PARMS spSearchParms = SearchObjToSubsearchObj( _spParms );
		if ( !(this->*_cbsFunctor)( cbCompFunc, ui32Size, spSearchParms, true ) ) { return false; }
		// ==============================================
		// ==============================================



		// ====================
		// == Do the search. ==
		// ====================
		bool bAllPass = true;
#ifndef MX_SINGLE_THREAD_SEARCH
		const size_t stThreads = 8;
		struct {
			CThread tThread;
			CSearchResult1 srResults;
			MX_PRIM_SEARCH_THREAD pstSearchThreadParms;
		} aThreadStruct[stThreads];
		LONG lProgTotal = 0;
		size_t stMax = std::min( vDivisions.size(), stThreads );
		for ( size_t I = 0; I < stMax; ++I ) {
			aThreadStruct[I].pstSearchThreadParms.hProgressUpdate = _hProgressUpdate;
			aThreadStruct[I].pstSearchThreadParms.paclChunks = &_aclChunks;
			aThreadStruct[I].pstSearchThreadParms.pspParms = &spSearchParms;
			aThreadStruct[I].pstSearchThreadParms.psrbResults = &aThreadStruct[I].srResults;
			aThreadStruct[I].pstSearchThreadParms.ppProcess = m_ppProcess;
			aThreadStruct[I].pstSearchThreadParms.cbChangedByCompFunc = cbCompFunc;
			aThreadStruct[I].pstSearchThreadParms.stStartIdx = (vDivisions.size() * I / stMax);
			aThreadStruct[I].pstSearchThreadParms.stEndIdx = (vDivisions.size() * (I + 1) / stMax);
			aThreadStruct[I].pstSearchThreadParms.ui32Size = ui32Size;
			aThreadStruct[I].pstSearchThreadParms.i32Result = 0;
			aThreadStruct[I].pstSearchThreadParms.plProgress = &lProgTotal;
			aThreadStruct[I].pstSearchThreadParms.pvSubsearchDiv = &vDivisions;
			aThreadStruct[I].pstSearchThreadParms.psrbPrevResults = m_psrbSearchResults;
			aThreadStruct[I].tThread.CreateThread( PrimitiveChangedBySubsearchThreadProc, &aThreadStruct[I].pstSearchThreadParms );
		}
		MX_THREADED_GATHER( CSearchResult1 );
#endif

		return bAllPass;
	}

	// Quick-Expression subsearch on primitives.
	template <typename _tType, unsigned _sIsFloat16>
	bool CSearcher::QuickExpPrimitiveSubsearch( MX_SUBSEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate ) {

		delete m_psrbCurTmpSearchResults;
		m_psrbCurTmpSearchResults = new ( std::nothrow ) CSearchResult1();
		if ( !m_psrbCurTmpSearchResults ) { return false; }
		CSearchResultLocker srlSearchLock( m_psrbSearchResults );
		std::vector<MX_SUBSEARCH_DIV> vDivisions;
		if ( !DivideResultsForSubsearch( m_psrbSearchResults, vDivisions, _spParms.ui64MaxChunkLen, _aclChunks ) ) {
			return false;
		}
		_spParms.bInvertResults = _spParms.bInvertResults ? true : false;	// Force to 0 or 1.


		// ============================================
		// == Create size and search-parameter copy. ==
		// ============================================
		MX_SEARCH_PARMS spSearchParms = SearchObjToSubsearchObj( _spParms );
		uint32_t ui32Size = CUtilities::DataTypeSize( _spParms.dtLVal.dtType );
		// ============================================
		// ============================================



		// ====================
		// == Do the search. ==
		// ====================
		bool bAllPass = true;
#ifndef MX_SINGLE_THREAD_SEARCH
		const size_t stThreads = 16;
		struct {
			CThread tThread;
			CSearchResult1 srResults;
			MX_PRIM_SEARCH_THREAD pstSearchThreadParms;
		} aThreadStruct[stThreads];
		LONG lProgTotal = 0;
		size_t stMax = std::min( vDivisions.size(), stThreads );
		for ( size_t I = 0; I < stMax; ++I ) {
			aThreadStruct[I].pstSearchThreadParms.hProgressUpdate = _hProgressUpdate;
			aThreadStruct[I].pstSearchThreadParms.paclChunks = &_aclChunks;
			aThreadStruct[I].pstSearchThreadParms.pspParms = &spSearchParms;
			aThreadStruct[I].pstSearchThreadParms.psrbResults = &aThreadStruct[I].srResults;
			aThreadStruct[I].pstSearchThreadParms.ppProcess = m_ppProcess;
			aThreadStruct[I].pstSearchThreadParms.stStartIdx = (vDivisions.size() * I / stMax);
			aThreadStruct[I].pstSearchThreadParms.stEndIdx = (vDivisions.size() * (I + 1) / stMax);
			aThreadStruct[I].pstSearchThreadParms.ui32Size = ui32Size;
			aThreadStruct[I].pstSearchThreadParms.i32Result = 0;
			aThreadStruct[I].pstSearchThreadParms.plProgress = &lProgTotal;
			aThreadStruct[I].pstSearchThreadParms.pvSubsearchDiv = &vDivisions;
			aThreadStruct[I].pstSearchThreadParms.psrbPrevResults = m_psrbSearchResults;
			aThreadStruct[I].tThread.CreateThread( PrimitiveQuickExpSubsearchThreadProc<_tType, _sIsFloat16>, &aThreadStruct[I].pstSearchThreadParms );
		}
		MX_THREADED_GATHER( CSearchResult1 );
#endif

		return bAllPass;
	}

	// Gets the alignment for a primitive type.
	bool CSearcher::PrimitiveAlignment( MX_SEARCH_PARMS &_spParms, CUtilities::MX_DATA_TYPES _dtType, CProcess * _ppProcess ) {
		_spParms.ui32Alignment = 1;
		if ( _spParms.bAligned ) {
			_spParms.ui32Alignment = CUtilities::DataTypeAlign( _spParms.dtLVal.dtType );
			if ( _spParms.ui32Alignment == DWINVALID ) {
				// It is a pointer.
				if ( m_ppProcess->IsWow64Process() ) {
					// We know it is 32-bit address space.
					_spParms.ui32Alignment = 4;
				}
				else {
					// It is the address space of the machine.
					if ( CSystem::Is32Bit() ) {
						_spParms.ui32Alignment = 4;
					}
					else if ( CSystem::Is64Bit() ) {
						_spParms.ui32Alignment = 8;
					}
					else { return false; }
				}
			}
		}
		return true;
	}

	// Creates an MX_SEARCH_PARMS object from a MX_SUBSEARCH_PARMS object.
	CSearcher::MX_SEARCH_PARMS CSearcher::SearchObjToSubsearchObj( const MX_SUBSEARCH_PARMS &_spParms ) {
		MX_SEARCH_PARMS spSearchParms;
		spSearchParms.bInvertResults = _spParms.bInvertResults;
		spSearchParms.dtLVal = _spParms.dtLVal;
		spSearchParms.dtRVal = _spParms.dtRVal;
		spSearchParms.dEpsilonValue = _spParms.dEpsilonValue;
		spSearchParms.bUseEpsilon = _spParms.bUseEpsilon;
		spSearchParms.bUseSmartEpsilon = _spParms.bUseSmartEpsilon;
		spSearchParms.stType = _spParms.stInitialType;
		spSearchParms.bsByteSwapping = _spParms.bsByteSwapping;
		spSearchParms.ui64MaxChunkLen = _spParms.ui64MaxChunkLen;
		spSearchParms.pbAbort = _spParms.pbAbort;
		spSearchParms.ssExpression = _spParms.ssExpression;
		return spSearchParms;
	}

	// Gets the comparison function and size.
	bool CSearcher::GetExactValCmpFuncAndSize( MX_EXACT_VAL_COMP &_evcCompFunc, uint32_t &_ui32Size, MX_SEARCH_PARMS &_spParms, bool _bConvertBothF16 ) {
		bool bLisFloat = CUtilities::DataTypeIsFloat( _spParms.dtLVal.dtType );
		if ( bLisFloat && _spParms.bUseEpsilon ) {
			_ui32Size = CUtilities::DataTypeSize( _spParms.dtLVal.dtType );
			switch ( _ui32Size ) {
				case sizeof( uint16_t ) : {
					if ( _spParms.bInvertResults ) {
						if ( _bConvertBothF16 ) {
							_evcCompFunc = _spParms.bUseSmartEpsilon ? Cmp_Float16RelativeEpsilonWithNaN_ConvertBoth : Cmp_Float16EpsilonWithNaN_ConvertBoth;
						}
						else {
							_evcCompFunc = _spParms.bUseSmartEpsilon ? Cmp_Float16RelativeEpsilonWithNaN : Cmp_Float16EpsilonWithNaN;
						}
					}
					else {
						if ( _bConvertBothF16 ) {
							_evcCompFunc = _spParms.bUseSmartEpsilon ? Cmp_Float16RelativeEpsilon_ConvertBoth : Cmp_Float16Epsilon_ConvertBoth;
						}
						else {
							_evcCompFunc = _spParms.bUseSmartEpsilon ? Cmp_Float16RelativeEpsilon : Cmp_Float16Epsilon;
						}
					}
					break;
				}
				case sizeof( float ) : {
					if ( _spParms.bInvertResults ) {
						_evcCompFunc = _spParms.bUseSmartEpsilon ? Cmp_Float32RelativeEpsilonWithNaN : Cmp_Float32EpsilonWithNaN;
					}
					else {
						_evcCompFunc = _spParms.bUseSmartEpsilon ? Cmp_Float32RelativeEpsilon : Cmp_Float32Epsilon;
					}
					break;
				}
				case sizeof( double ) : {
					if ( _spParms.bInvertResults ) {
						_evcCompFunc = _spParms.bUseSmartEpsilon ? Cmp_Float64RelativeEpsilonWithNaN : Cmp_Float64EpsilonWithNaN;
					}
					else {
						_evcCompFunc = _spParms.bUseSmartEpsilon ? Cmp_Float64RelativeEpsilon : Cmp_Float64Epsilon;
					}
					break;
				}
				default : { return false; }
			}
		}
		else {
			_ui32Size = CUtilities::DataTypeSize( _spParms.dtLVal.dtType );
			switch ( _ui32Size ) {
				case DWINVALID : {
					// CUtilities::MX_DT_VOID == Pointer32 or Pointer64.
					if ( m_ppProcess->IsWow64Process() ) {
						// We know it is 32-bit address space.
						_evcCompFunc = Cmp_ExactVal32;
						_ui32Size = 4;
					}
					else {
						// It is the address space of the machine.
						if ( CSystem::Is32Bit() ) {
							_evcCompFunc = Cmp_ExactVal32;
							_ui32Size = 4;
						}
						else if ( CSystem::Is64Bit() ) {
							_evcCompFunc = Cmp_ExactVal64;
							_ui32Size = 8;
						}
						else { return false; }
					}
					break;
				}
				case 1 : { _evcCompFunc = Cmp_ExactVal8; break; }
				case 2 : {
					if ( bLisFloat && _spParms.bInvertResults ) {
						if ( _bConvertBothF16 ) {
							_evcCompFunc = Cmp_Float16WithNaN_ConvertBoth;
						}
						else {
							_evcCompFunc = Cmp_Float16WithNaN;
						}
					}
					else {
						if ( _bConvertBothF16 ) {
							_evcCompFunc = Cmp_ExactVal16;
						}
						else {
							_evcCompFunc = Cmp_ExactVal16;
						}
						if ( _spParms.dtLVal.dtType == CUtilities::MX_DT_FLOAT16 ) {
							_spParms.dtLVal.u.UInt16 = CFloat16::DoubleToUi16( _spParms.dtLVal.u.Double );
							//_spParms.dtLVal.dtType = CUtilities::MX_DT_FLOAT16;
						}
					}
					break;
				}
				case 4 : {
					_evcCompFunc = (bLisFloat && _spParms.bInvertResults) ? Cmp_Float32WithNaN : Cmp_ExactVal32;
					break;
				}
				case 8 : {
					_evcCompFunc = (bLisFloat && _spParms.bInvertResults) ? Cmp_Float64WithNaN : Cmp_ExactVal64;
					break;
				}
				default: { return false; }
			}
		}
		return true;
	}

	// Gets the comparison function and size.
	bool CSearcher::GetGreaterThanCmpFuncAndSize( MX_REL_COMP &_rcCompFunc, uint32_t &_ui32Size, MX_SEARCH_PARMS &_spParms, bool _bConvertBothF16 ) {
		_ui32Size = CUtilities::DataTypeSize( _spParms.dtLVal.dtType );
		if ( _ui32Size == DWINVALID ) {
			// CUtilities::MX_DT_VOID == Pointer32 or Pointer64.
			if ( m_ppProcess->IsWow64Process() ) {
				// We know it is 32-bit address space.
				_rcCompFunc = Cmp_GreaterThanUnsigned32;
				_ui32Size = 4;
			}
			else {
				// It is the address space of the machine.
				if ( CSystem::Is32Bit() ) {
					_rcCompFunc = Cmp_GreaterThanUnsigned32;
					_ui32Size = 4;
				}
				else if ( CSystem::Is64Bit() ) {
					_rcCompFunc = Cmp_GreaterThanUnsigned64;
					_ui32Size = 8;
				}
				else { return false; }
			}
		}
		else {
			switch ( _spParms.dtLVal.dtType ) {
				case CUtilities::MX_DT_INT8 : {
					_rcCompFunc = Cmp_GreaterThanSigned8;
					break;
				}
				case CUtilities::MX_DT_UINT8 : {
					_rcCompFunc = Cmp_GreaterThanUnsigned8;
					break;
				}
				case CUtilities::MX_DT_INT16 : {
					_rcCompFunc = Cmp_GreaterThanSigned16;
					break;
				}
				case CUtilities::MX_DT_UINT16 : {
					_rcCompFunc = Cmp_GreaterThanUnsigned16;
					break;
				}
				case CUtilities::MX_DT_INT32 : {
					_rcCompFunc = Cmp_GreaterThanSigned32;
					break;
				}
				case CUtilities::MX_DT_UINT32 : {
					_rcCompFunc = Cmp_GreaterThanUnsigned32;
					break;
				}
				case CUtilities::MX_DT_INT64 : {
					_rcCompFunc = Cmp_GreaterThanSigned64;
					break;
				}
				case CUtilities::MX_DT_UINT64 : {
					_rcCompFunc = Cmp_GreaterThanUnsigned64;
					break;
				}
				case CUtilities::MX_DT_FLOAT16 : {
					if ( _bConvertBothF16 ) {
						_rcCompFunc = _spParms.bInvertResults ? Cmp_GreaterThanFloat16WithNaN_ConvertBoth : Cmp_GreaterThanFloat16_ConvertBoth;
					}
					else {
						_rcCompFunc = _spParms.bInvertResults ? Cmp_GreaterThanFloat16WithNaN : Cmp_GreaterThanFloat16;
					}
					break;
				}
				case CUtilities::MX_DT_FLOAT : {
					_rcCompFunc = _spParms.bInvertResults ? Cmp_GreaterThanFloat32WithNaN : Cmp_GreaterThanFloat32;
					break;
				}
				case CUtilities::MX_DT_DOUBLE : {
					_rcCompFunc = _spParms.bInvertResults ? Cmp_GreaterThanFloat64WithNaN : Cmp_GreaterThanFloat64;
					break;
				}
				default : { return false; }
			}
		}
		return true;
	}

	// Gets the comparison function and size.
	bool CSearcher::GetLessThanCmpFuncAndSize( MX_REL_COMP &_rcCompFunc, uint32_t &_ui32Size, MX_SEARCH_PARMS &_spParms, bool _bConvertBothF16 ) {
		_ui32Size = CUtilities::DataTypeSize( _spParms.dtLVal.dtType );
		if ( _ui32Size == DWINVALID ) {
			// CUtilities::MX_DT_VOID == Pointer32 or Pointer64.
			if ( m_ppProcess->IsWow64Process() ) {
				// We know it is 32-bit address space.
				_rcCompFunc = Cmp_LessThanUnsigned32;
				_ui32Size = 4;
			}
			else {
				// It is the address space of the machine.
				if ( CSystem::Is32Bit() ) {
					_rcCompFunc = Cmp_LessThanUnsigned32;
					_ui32Size = 4;
				}
				else if ( CSystem::Is64Bit() ) {
					_rcCompFunc = Cmp_LessThanUnsigned64;
					_ui32Size = 8;
				}
				else { return false; }
			}
		}
		else {
			switch ( _spParms.dtLVal.dtType ) {
				case CUtilities::MX_DT_INT8 : {
					_rcCompFunc = Cmp_LessThanSigned8;
					break;
				}
				case CUtilities::MX_DT_UINT8 : {
					_rcCompFunc = Cmp_LessThanUnsigned8;
					break;
				}
				case CUtilities::MX_DT_INT16 : {
					_rcCompFunc = Cmp_LessThanSigned16;
					break;
				}
				case CUtilities::MX_DT_UINT16 : {
					_rcCompFunc = Cmp_LessThanUnsigned16;
					break;
				}
				case CUtilities::MX_DT_INT32 : {
					_rcCompFunc = Cmp_LessThanSigned32;
					break;
				}
				case CUtilities::MX_DT_UINT32 : {
					_rcCompFunc = Cmp_LessThanUnsigned32;
					break;
				}
				case CUtilities::MX_DT_INT64 : {
					_rcCompFunc = Cmp_LessThanSigned64;
					break;
				}
				case CUtilities::MX_DT_UINT64 : {
					_rcCompFunc = Cmp_LessThanUnsigned64;
					break;
				}
				case CUtilities::MX_DT_FLOAT16 : {
					if ( _bConvertBothF16 ) {
						_rcCompFunc = _spParms.bInvertResults ? Cmp_LessThanFloat16WithNaN_ConvertBoth : Cmp_LessThanFloat16_ConvertBoth;
					}
					else {
						_rcCompFunc = _spParms.bInvertResults ? Cmp_LessThanFloat16WithNaN : Cmp_LessThanFloat16;
					}
					break;
				}
				case CUtilities::MX_DT_FLOAT : {
					_rcCompFunc = _spParms.bInvertResults ? Cmp_LessThanFloat32WithNaN : Cmp_LessThanFloat32;
					break;
				}
				case CUtilities::MX_DT_DOUBLE : {
					_rcCompFunc = _spParms.bInvertResults ? Cmp_LessThanFloat64WithNaN : Cmp_LessThanFloat64;
					break;
				}
				default : { return false; }
			}
		}
		return true;
	}

	// Gets the comparison function and size.
	bool CSearcher::GetRangeCmpFuncAndSize( MX_RANGE_COMP &_rcCompFunc, uint32_t &_ui32Size, MX_SEARCH_PARMS &_spParms, bool _bConvertBothF16 ) {
		_ui32Size = CUtilities::DataTypeSize( _spParms.dtLVal.dtType );
		if ( _ui32Size == DWINVALID ) {
			// CUtilities::MX_DT_VOID == Pointer32 or Pointer64.
			if ( m_ppProcess->IsWow64Process() ) {
				// We know it is 32-bit address space.
				_rcCompFunc = Cmp_InRangeUInt32;
				_ui32Size = 4;
			}
			else {
				// It is the address space of the machine.
				if ( CSystem::Is32Bit() ) {
					_rcCompFunc = Cmp_InRangeUInt32;
					_ui32Size = 4;
				}
				else if ( CSystem::Is64Bit() ) {
					_rcCompFunc = Cmp_InRangeUInt64;
					_ui32Size = 8;
				}
				else { return false; }
			}
		}
		else {
			switch ( _spParms.dtLVal.dtType ) {
				case CUtilities::MX_DT_INT8 : {
					_rcCompFunc = Cmp_InRangeInt8;
					break;
				}
				case CUtilities::MX_DT_UINT8 : {
					_rcCompFunc = Cmp_InRangeUInt8;
					break;
				}
				case CUtilities::MX_DT_INT16 : {
					_rcCompFunc = Cmp_InRangeInt16;
					break;
				}
				case CUtilities::MX_DT_UINT16 : {
					_rcCompFunc = Cmp_InRangeUInt16;
					break;
				}
				case CUtilities::MX_DT_INT32 : {
					_rcCompFunc = Cmp_InRangeInt32;
					break;
				}
				case CUtilities::MX_DT_UINT32 : {
					_rcCompFunc = Cmp_InRangeUInt32;
					break;
				}
				case CUtilities::MX_DT_INT64 : {
					_rcCompFunc = Cmp_InRangeInt64;
					break;
				}
				case CUtilities::MX_DT_UINT64 : {
					_rcCompFunc = Cmp_InRangeUInt64;
					break;
				}
				case CUtilities::MX_DT_FLOAT16 : {
					if ( _bConvertBothF16 ) {
						_rcCompFunc = _spParms.bInvertResults ? Cmp_InRangeFloat16WithNaN : Cmp_InRangeFloat16;
					}
					else {
						_rcCompFunc = _spParms.bInvertResults ? Cmp_InRangeFloat16WithNaN : Cmp_InRangeFloat16;
					}
					break;
				}
				case CUtilities::MX_DT_FLOAT : {
					_rcCompFunc = _spParms.bInvertResults ? Cmp_InRangeFloat32WithNaN : Cmp_InRangeFloat32;
					break;
				}
				case CUtilities::MX_DT_DOUBLE : {
					_rcCompFunc = _spParms.bInvertResults ? Cmp_InRangeFloat64WithNaN : Cmp_InRangeFloat64;
					break;
				}
				default : { return false; }
			}
		}
		return true;
	}

	// Gets the comparison function and size.
	bool CSearcher::GetChangedByCmpFuncAndSize( MX_CHANGED_BY &_cbCompFunc, uint32_t &_ui32Size, MX_SEARCH_PARMS &_spParms, bool _bConvertBothF16 ) {
		bool bLisFloat = CUtilities::DataTypeIsFloat( _spParms.dtLVal.dtType );

		_ui32Size = CUtilities::DataTypeSize( _spParms.dtLVal.dtType );
		if ( bLisFloat && _spParms.bUseEpsilon ) {
			switch ( _ui32Size ) {
				case sizeof( uint16_t ) : {
					if ( _spParms.bInvertResults ) {
						_cbCompFunc = _spParms.bUseSmartEpsilon ? Cmp_ChangedByFloat16RelativeEpsilonWithNaN : Cmp_ChangedByFloat16EpsilonWithNaN;
					}
					else {
						_cbCompFunc = _spParms.bUseSmartEpsilon ? Cmp_ChangedByFloat16RelativeEpsilon : Cmp_ChangedByFloat16Epsilon;
					}
					break;
				}
				case sizeof( float ) : {
					if ( _spParms.bInvertResults ) {
						_cbCompFunc = _spParms.bUseSmartEpsilon ? Cmp_ChangedByFloat32RelativeEpsilonWithNaN : Cmp_ChangedByFloat32EpsilonWithNaN;
					}
					else {
						_cbCompFunc = _spParms.bUseSmartEpsilon ? Cmp_ChangedByFloat32RelativeEpsilon : Cmp_ChangedByFloat32Epsilon;
					}
					break;
				}
				case sizeof( double ) : {
					if ( _spParms.bInvertResults ) {
						_cbCompFunc = _spParms.bUseSmartEpsilon ? Cmp_ChangedByFloat64RelativeEpsilonWithNaN : Cmp_ChangedByFloat64EpsilonWithNaN;
					}
					else {
						_cbCompFunc = _spParms.bUseSmartEpsilon ? Cmp_ChangedByFloat64RelativeEpsilon : Cmp_ChangedByFloat64Epsilon;
					}
					break;
				}
				default : { return false; }
			}
		}
		else {

			if ( _ui32Size == DWINVALID ) {
				// CUtilities::MX_DT_VOID == Pointer32 or Pointer64.
				if ( m_ppProcess->IsWow64Process() ) {
					// We know it is 32-bit address space.
					_cbCompFunc = Cmp_ChangedByInt32;
					_ui32Size = 4;
				}
				else {
					// It is the address space of the machine.
					if ( CSystem::Is32Bit() ) {
						_cbCompFunc = Cmp_ChangedByInt32;
						_ui32Size = 4;
					}
					else if ( CSystem::Is64Bit() ) {
						_cbCompFunc = Cmp_ChangedByInt64;
						_ui32Size = 8;
					}
					else { return false; }
				}
			}
			else {
				switch ( _spParms.dtLVal.dtType ) {
					case CUtilities::MX_DT_INT8 : {
						_cbCompFunc = Cmp_ChangedByInt8;
						break;
					}
					case CUtilities::MX_DT_UINT8 : {
						_cbCompFunc = Cmp_ChangedByUInt8;
						break;
					}
					case CUtilities::MX_DT_INT16 : {
						_cbCompFunc = Cmp_ChangedByInt16;
						break;
					}
					case CUtilities::MX_DT_UINT16 : {
						_cbCompFunc = Cmp_ChangedByUInt16;
						break;
					}
					case CUtilities::MX_DT_INT32 : {
						_cbCompFunc = Cmp_ChangedByInt32;
						break;
					}
					case CUtilities::MX_DT_UINT32 : {
						_cbCompFunc = Cmp_ChangedByUInt32;
						break;
					}
					case CUtilities::MX_DT_INT64 : {
						_cbCompFunc = Cmp_ChangedByInt64;
						break;
					}
					case CUtilities::MX_DT_UINT64 : {
						_cbCompFunc = Cmp_ChangedByUInt64;
						break;
					}
					case CUtilities::MX_DT_FLOAT16 : {
						if ( _bConvertBothF16 ) {
							_cbCompFunc = _spParms.bInvertResults ? Cmp_ChangedByFloat16WithNaN : Cmp_ChangedByFloat16;
						}
						else {
							_cbCompFunc = _spParms.bInvertResults ? Cmp_ChangedByFloat16WithNaN : Cmp_ChangedByFloat16;
						}
						break;
					}
					case CUtilities::MX_DT_FLOAT : {
						_cbCompFunc = _spParms.bInvertResults ? Cmp_ChangedByFloat32WithNaN : Cmp_ChangedByFloat32;
						break;
					}
					case CUtilities::MX_DT_DOUBLE : {
						_cbCompFunc = _spParms.bInvertResults ? Cmp_ChangedByFloat64WithNaN : Cmp_ChangedByFloat64;
						break;
					}
					default : { return false; }
				}
			}
		}
		return true;
	}

	// Gets the comparison function and size.
	bool CSearcher::GetChangedByRangeCmpFuncAndSize( MX_CHANGED_BY &_cbCompFunc, uint32_t &_ui32Size, MX_SEARCH_PARMS &_spParms, bool _bConvertBothF16 ) {
		_ui32Size = CUtilities::DataTypeSize( _spParms.dtLVal.dtType );

		CUtilities::MX_DATA_TYPE dtLCopy = _spParms.dtLVal;
		CUtilities::MX_DATA_TYPE dtRCopy = _spParms.dtRVal;
		if ( _ui32Size == DWINVALID ) {
			// CUtilities::MX_DT_VOID == Pointer32 or Pointer64.
			if ( m_ppProcess->IsWow64Process() ) {
				// We know it is 32-bit address space.
				_cbCompFunc = Cmp_ChangedByRangeInt32;
				_ui32Size = 4;
				_spParms.dtLVal.u.Int32 = std::min( dtLCopy.u.Int32, dtRCopy.u.Int32 );
				_spParms.dtRVal.u.Int32 = std::max( dtRCopy.u.Int32, dtLCopy.u.Int32 );
			}
			else {
				// It is the address space of the machine.
				if ( CSystem::Is32Bit() ) {
					_cbCompFunc = Cmp_ChangedByRangeInt32;
					_ui32Size = 4;
					_spParms.dtLVal.u.Int32 = std::min( dtLCopy.u.Int32, dtRCopy.u.Int32 );
					_spParms.dtRVal.u.Int32 = std::max( dtRCopy.u.Int32, dtLCopy.u.Int32 );
				}
				else if ( CSystem::Is64Bit() ) {
					_cbCompFunc = Cmp_ChangedByRangeInt64;
					_ui32Size = 8;
					_spParms.dtLVal.u.Int64 = std::min( dtLCopy.u.Int64, dtRCopy.u.Int64 );
					_spParms.dtRVal.u.Int64 = std::max( dtRCopy.u.Int64, dtLCopy.u.Int64 );
				}
				else { return false; }
			}
		}
		else {
			switch ( _spParms.dtLVal.dtType ) {
				case CUtilities::MX_DT_INT8 : {
					_cbCompFunc = Cmp_ChangedByRangeInt8;
					_spParms.dtLVal.u.Int8 = std::min( dtLCopy.u.Int8, dtRCopy.u.Int8 );
					_spParms.dtRVal.u.Int8 = std::max( dtRCopy.u.Int8, dtLCopy.u.Int8 );
					break;
				}
				case CUtilities::MX_DT_UINT8 : {
					_cbCompFunc = Cmp_ChangedByRangeUInt8;
					_spParms.dtLVal.u.UInt8 = std::min( dtLCopy.u.UInt8, dtRCopy.u.UInt8 );
					_spParms.dtRVal.u.UInt8 = std::max( dtRCopy.u.UInt8, dtLCopy.u.UInt8 );
					break;
				}
				case CUtilities::MX_DT_INT16 : {
					_cbCompFunc = Cmp_ChangedByRangeInt16;
					_spParms.dtLVal.u.Int16 = std::min( dtLCopy.u.Int16, dtRCopy.u.Int16 );
					_spParms.dtRVal.u.Int16 = std::max( dtRCopy.u.Int16, dtLCopy.u.Int16 );
					break;
				}
				case CUtilities::MX_DT_UINT16 : {
					_cbCompFunc = Cmp_ChangedByRangeUInt16;
					_spParms.dtLVal.u.UInt16 = std::min( dtLCopy.u.UInt16, dtRCopy.u.UInt16 );
					_spParms.dtRVal.u.UInt16 = std::max( dtRCopy.u.UInt16, dtLCopy.u.UInt16 );
					break;
				}
				case CUtilities::MX_DT_INT32 : {
					_cbCompFunc = Cmp_ChangedByRangeInt32;
					_spParms.dtLVal.u.Int32 = std::min( dtLCopy.u.Int32, dtRCopy.u.Int32 );
					_spParms.dtRVal.u.Int32 = std::max( dtRCopy.u.Int32, dtLCopy.u.Int32 );
					break;
				}
				case CUtilities::MX_DT_UINT32 : {
					_cbCompFunc = Cmp_ChangedByRangeUInt32;
					_spParms.dtLVal.u.UInt32 = std::min( dtLCopy.u.UInt32, dtRCopy.u.UInt32 );
					_spParms.dtRVal.u.UInt32 = std::max( dtRCopy.u.UInt32, dtLCopy.u.UInt32 );
					break;
				}
				case CUtilities::MX_DT_INT64 : {
					_cbCompFunc = Cmp_ChangedByRangeInt64;
					_spParms.dtLVal.u.Int64 = std::min( dtLCopy.u.Int64, dtRCopy.u.Int64 );
					_spParms.dtRVal.u.Int64 = std::max( dtRCopy.u.Int64, dtLCopy.u.Int64 );
					break;
				}
				case CUtilities::MX_DT_UINT64 : {
					_cbCompFunc = Cmp_ChangedByRangeUInt64;
					_spParms.dtLVal.u.UInt64 = std::min( dtLCopy.u.UInt64, dtRCopy.u.UInt64 );
					_spParms.dtRVal.u.UInt64 = std::max( dtRCopy.u.UInt64, dtLCopy.u.UInt64 );
					break;
				}
				case CUtilities::MX_DT_FLOAT16 : {
					if ( _bConvertBothF16 ) {
						_cbCompFunc = _spParms.bInvertResults ? Cmp_ChangedByRangeFloat16WithNaN : Cmp_ChangedByRangeFloat16;
					}
					else {
						_cbCompFunc = _spParms.bInvertResults ? Cmp_ChangedByRangeFloat16WithNaN : Cmp_ChangedByRangeFloat16;
					}
					_spParms.dtLVal.u.Float64 = std::min( dtLCopy.u.Float64, dtRCopy.u.Float64 );
					_spParms.dtRVal.u.Float64 = std::max( dtRCopy.u.Float64, dtLCopy.u.Float64 );
					break;
				}
				case CUtilities::MX_DT_FLOAT : {
					_cbCompFunc = _spParms.bInvertResults ? Cmp_ChangedByRangeFloat32WithNaN : Cmp_ChangedByRangeFloat32;
					_spParms.dtLVal.u.Float32 = std::min( dtLCopy.u.Float32, dtRCopy.u.Float32 );
					_spParms.dtRVal.u.Float32 = std::max( dtRCopy.u.Float32, dtLCopy.u.Float32 );
					break;
				}
				case CUtilities::MX_DT_DOUBLE : {
					_cbCompFunc = _spParms.bInvertResults ? Cmp_ChangedByRangeFloat64WithNaN : Cmp_ChangedByRangeFloat64;
					_spParms.dtLVal.u.Float64 = std::min( dtLCopy.u.Float64, dtRCopy.u.Float64 );
					_spParms.dtRVal.u.Float64 = std::max( dtRCopy.u.Float64, dtLCopy.u.Float64 );
					break;
				}
				default : { return false; }
			}
		}
		return true;
	}

	// Gets the comparison function and size.
	bool CSearcher::GetChangedByPercCmpFuncAndSize( MX_CHANGED_BY &_cbCompFunc, uint32_t &_ui32Size, MX_SEARCH_PARMS &_spParms, bool _bConvertBothF16 ) {
		bool bLisFloat = CUtilities::DataTypeIsFloat( _spParms.dtLVal.dtType );

		_ui32Size = CUtilities::DataTypeSize( _spParms.dtLVal.dtType );
		if ( bLisFloat && _spParms.bUseEpsilon ) {
			switch ( _ui32Size ) {
				case sizeof( uint16_t ) : {
					if ( _spParms.bInvertResults ) {
						_cbCompFunc = _spParms.bUseSmartEpsilon ? Cmp_ChangedByPercFloat16RelativeEpsilonWithNaN : Cmp_ChangedByPercFloat16EpsilonWithNaN;
					}
					else {
						_cbCompFunc = _spParms.bUseSmartEpsilon ? Cmp_ChangedByPercFloat16RelativeEpsilon : Cmp_ChangedByPercFloat16Epsilon;
					}
					break;
				}
				case sizeof( float ) : {
					if ( _spParms.bInvertResults ) {
						_cbCompFunc = _spParms.bUseSmartEpsilon ? Cmp_ChangedByPercFloat32RelativeEpsilonWithNaN : Cmp_ChangedByPercFloat32EpsilonWithNaN;
					}
					else {
						_cbCompFunc = _spParms.bUseSmartEpsilon ? Cmp_ChangedByPercFloat32RelativeEpsilon : Cmp_ChangedByPercFloat32Epsilon;
					}
					break;
				}
				case sizeof( double ) : {
					if ( _spParms.bInvertResults ) {
						_cbCompFunc = _spParms.bUseSmartEpsilon ? Cmp_ChangedByPercFloat64RelativeEpsilonWithNaN : Cmp_ChangedByPercFloat64EpsilonWithNaN;
					}
					else {
						_cbCompFunc = _spParms.bUseSmartEpsilon ? Cmp_ChangedByPercFloat64RelativeEpsilon : Cmp_ChangedByPercFloat64Epsilon;
					}
					break;
				}
				default : { return false; }
			}
		}
		else {

			if ( _ui32Size == DWINVALID ) {
				// CUtilities::MX_DT_VOID == Pointer32 or Pointer64.
				if ( m_ppProcess->IsWow64Process() ) {
					// We know it is 32-bit address space.
					_cbCompFunc = Cmp_ChangedByPercInt32;
					_ui32Size = 4;
				}
				else {
					// It is the address space of the machine.
					if ( CSystem::Is32Bit() ) {
						_cbCompFunc = Cmp_ChangedByPercInt32;
						_ui32Size = 4;
					}
					else if ( CSystem::Is64Bit() ) {
						_cbCompFunc = Cmp_ChangedByPercInt64;
						_ui32Size = 8;
					}
					else { return false; }
				}
			}
			else {
				switch ( _spParms.dtLVal.dtType ) {
					case CUtilities::MX_DT_INT8 : {
						_cbCompFunc = Cmp_ChangedByPercInt8;
						break;
					}
					case CUtilities::MX_DT_UINT8 : {
						_cbCompFunc = Cmp_ChangedByPercUInt8;
						break;
					}
					case CUtilities::MX_DT_INT16 : {
						_cbCompFunc = Cmp_ChangedByPercInt16;
						break;
					}
					case CUtilities::MX_DT_UINT16 : {
						_cbCompFunc = Cmp_ChangedByPercUInt16;
						break;
					}
					case CUtilities::MX_DT_INT32 : {
						_cbCompFunc = Cmp_ChangedByPercInt32;
						break;
					}
					case CUtilities::MX_DT_UINT32 : {
						_cbCompFunc = Cmp_ChangedByPercUInt32;
						break;
					}
					case CUtilities::MX_DT_INT64 : {
						_cbCompFunc = Cmp_ChangedByPercInt64;
						break;
					}
					case CUtilities::MX_DT_UINT64 : {
						_cbCompFunc = Cmp_ChangedByPercUInt64;
						break;
					}
					case CUtilities::MX_DT_FLOAT16 : {
						if ( _bConvertBothF16 ) {
							_cbCompFunc = _spParms.bInvertResults ? Cmp_ChangedByPercFloat16WithNaN : Cmp_ChangedByPercFloat16;
						}
						else {
							_cbCompFunc = _spParms.bInvertResults ? Cmp_ChangedByPercFloat16WithNaN : Cmp_ChangedByPercFloat16;
						}
						break;
					}
					case CUtilities::MX_DT_FLOAT : {
						_cbCompFunc = _spParms.bInvertResults ? Cmp_ChangedByPercFloat32WithNaN : Cmp_ChangedByPercFloat32;
						break;
					}
					case CUtilities::MX_DT_DOUBLE : {
						_cbCompFunc = _spParms.bInvertResults ? Cmp_ChangedByPercFloat64WithNaN : Cmp_ChangedByPercFloat64;
						break;
					}
					default : { return false; }
				}
			}
		}
		return true;
	}

	// Gets the comparison function and size.
	bool CSearcher::GetChangedByRangePercCmpFuncAndSize( MX_CHANGED_BY &_cbCompFunc, uint32_t &_ui32Size, MX_SEARCH_PARMS &_spParms, bool _bConvertBothF16 ) {
		_ui32Size = CUtilities::DataTypeSize( _spParms.dtLVal.dtType );

		CUtilities::MX_DATA_TYPE dtLCopy = _spParms.dtLVal;
		CUtilities::MX_DATA_TYPE dtRCopy = _spParms.dtRVal;
		_spParms.dtLVal.u.Float64 = std::min( dtLCopy.u.Float64, dtRCopy.u.Float64 );
		_spParms.dtRVal.u.Float64 = std::max( dtRCopy.u.Float64, dtLCopy.u.Float64 );
		if ( _ui32Size == DWINVALID ) {
			// CUtilities::MX_DT_VOID == Pointer32 or Pointer64.
			if ( m_ppProcess->IsWow64Process() ) {
				// We know it is 32-bit address space.
				_cbCompFunc = Cmp_ChangedByRangePercInt32;
				_ui32Size = 4;
			}
			else {
				// It is the address space of the machine.
				if ( CSystem::Is32Bit() ) {
					_cbCompFunc = Cmp_ChangedByRangePercInt32;
					_ui32Size = 4;
				}
				else if ( CSystem::Is64Bit() ) {
					_cbCompFunc = Cmp_ChangedByRangePercInt64;
					_ui32Size = 8;
				}
				else { return false; }
			}
		}
		else {
			switch ( _spParms.dtLVal.dtType ) {
				case CUtilities::MX_DT_INT8 : {
					_cbCompFunc = Cmp_ChangedByRangePercInt8;
					break;
				}
				case CUtilities::MX_DT_UINT8 : {
					_cbCompFunc = Cmp_ChangedByRangePercUInt8;
					break;
				}
				case CUtilities::MX_DT_INT16 : {
					_cbCompFunc = Cmp_ChangedByRangePercInt16;
					break;
				}
				case CUtilities::MX_DT_UINT16 : {
					_cbCompFunc = Cmp_ChangedByRangePercUInt16;
					break;
				}
				case CUtilities::MX_DT_INT32 : {
					_cbCompFunc = Cmp_ChangedByRangePercInt32;
					break;
				}
				case CUtilities::MX_DT_UINT32 : {
					_cbCompFunc = Cmp_ChangedByRangePercUInt32;
					break;
				}
				case CUtilities::MX_DT_INT64 : {
					_cbCompFunc = Cmp_ChangedByRangePercInt64;
					break;
				}
				case CUtilities::MX_DT_UINT64 : {
					_cbCompFunc = Cmp_ChangedByRangePercUInt64;
					break;
				}
				case CUtilities::MX_DT_FLOAT16 : {
					if ( _bConvertBothF16 ) {
						_cbCompFunc = _spParms.bInvertResults ? Cmp_ChangedByRangePercFloat16WithNaN : Cmp_ChangedByRangePercFloat16;
					}
					else {
						_cbCompFunc = _spParms.bInvertResults ? Cmp_ChangedByRangePercFloat16WithNaN : Cmp_ChangedByRangePercFloat16;
					}
					break;
				}
				case CUtilities::MX_DT_FLOAT : {
					_cbCompFunc = _spParms.bInvertResults ? Cmp_ChangedByRangePercFloat32WithNaN : Cmp_ChangedByRangePercFloat32;
					break;
				}
				case CUtilities::MX_DT_DOUBLE : {
					_cbCompFunc = _spParms.bInvertResults ? Cmp_ChangedByRangePercFloat64WithNaN : Cmp_ChangedByRangePercFloat64;
					break;
				}
				default : { return false; }
			}
		}
		return true;
	}

	// User-variable function handler.
	bool __stdcall CSearcher::UserVarHandler( uintptr_t _uiptrData, ee::CExpEvalContainer * _peecContainer, ee::CExpEvalContainer::EE_RESULT &_rResult ) {
		::memcpy( &_rResult, reinterpret_cast<const void *>(_uiptrData), sizeof( _rResult ) );
		return true;
	}

	// The address reader for expressions.
	bool __stdcall CSearcher::ExpAddressHandler( uint64_t _ui64Address, ee::EE_CAST_TYPES _ctType, uintptr_t _uiptrData, ee::CExpEvalContainer * _peecContainer, ee::CExpEvalContainer::EE_RESULT &_rResult ) {
		CProcess * ppProcess = reinterpret_cast<CProcess *>(_uiptrData);
		switch ( _ctType ) {
			case ee::EE_CT_INT8 : {
				_rResult.ncType = ee::EE_NC_SIGNED;
				int8_t i8Val;
				if ( ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(_ui64Address), &i8Val, sizeof( i8Val ), NULL ) ) {
					_rResult.u.i64Val = i8Val;
					return true;
				}
				break;
			}
			case ee::EE_CT_INT16 : {
				_rResult.ncType = ee::EE_NC_SIGNED;
				int16_t i16Val;
				if ( ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(_ui64Address), &i16Val, sizeof( i16Val ), NULL ) ) {
					_rResult.u.i64Val = i16Val;
					return true;
				}
				break;
			}
			case ee::EE_CT_INT32 : {
				_rResult.ncType = ee::EE_NC_SIGNED;
				int32_t i32Val;
				if ( ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(_ui64Address), &i32Val, sizeof( i32Val ), NULL ) ) {
					_rResult.u.i64Val = i32Val;
					return true;
				}
				break;
			}
			case ee::EE_CT_INT64 : {
				_rResult.ncType = ee::EE_NC_SIGNED;
				if ( ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(_ui64Address), &_rResult.u.i64Val, sizeof( _rResult.u.i64Val ), NULL ) ) {
					return true;
				}
				break;
			}

			case ee::EE_CT_UINT8 : {
				_rResult.ncType = ee::EE_NC_UNSIGNED;
				_rResult.u.ui64Val = 0;
				return ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(_ui64Address), &_rResult.u.ui64Val, sizeof( uint8_t ), NULL );
			}
			case ee::EE_CT_UINT16 : {
				_rResult.ncType = ee::EE_NC_UNSIGNED;
				_rResult.u.ui64Val = 0;
				return ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(_ui64Address), &_rResult.u.ui64Val, sizeof( uint16_t ), NULL );
			}
			case ee::EE_CT_UINT32 : {
				_rResult.ncType = ee::EE_NC_UNSIGNED;
				_rResult.u.ui64Val = 0;
				return ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(_ui64Address), &_rResult.u.ui64Val, sizeof( uint32_t ), NULL );
			}
			case ee::EE_CT_UINT64 : {
				_rResult.ncType = ee::EE_NC_UNSIGNED;
				_rResult.u.ui64Val = 0;
				return ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(_ui64Address), &_rResult.u.ui64Val, sizeof( uint64_t ), NULL );
			}


			case ee::EE_CT_FLOAT16 : {
				_rResult.ncType = ee::EE_NC_FLOATING;
				uint16_t ui16Val = 0;
				if ( ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(_ui64Address), &ui16Val, sizeof( ui16Val ), NULL ) ) {
					_rResult.u.dVal = CFloat16( ui16Val ).Value();
					return true;
				}
				break;
			}
			case ee::EE_CT_FLOAT : {
				_rResult.ncType = ee::EE_NC_FLOATING;
				float fVal = 0.0f;
				if ( ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(_ui64Address), &fVal, sizeof( fVal ), NULL ) ) {
					_rResult.u.dVal = fVal;
					return true;
				}
				break;
			}
			case ee::EE_CT_DOUBLE : {
				_rResult.ncType = ee::EE_NC_FLOATING;
				_rResult.u.dVal = 0;
				return ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(_ui64Address), &_rResult.u.dVal, sizeof( _rResult.u.dVal ), NULL );
			}
		}
		_rResult.ncType = ee::EE_NC_INVALID;
		return false;
	}

	// The address reader for dynamic expressions.
	bool __stdcall CSearcher::ExpAddressHandler_LocalPrecache( uint64_t _ui64Address, ee::EE_CAST_TYPES _tType, uintptr_t _uiptrData, ee::CExpEvalContainer * _peecContainer, ee::CExpEvalContainer::EE_RESULT &_rResult ) {
		MX_EXP_SEARCH_CACHE * pescCache = reinterpret_cast<MX_EXP_SEARCH_CACHE *>(_uiptrData);
		switch ( _tType ) {
#define MX_CASE( CASE, TYPE, BROADTYPE, MEMBER )																								\
	case CASE : {																																\
		if ( _ui64Address >= pescCache->ui64Address && _ui64Address <= (pescCache->ui64Address + pescCache->sBaseSize - sizeof( TYPE )) ) {		\
			_rResult.ncType = ee::BROADTYPE;																									\
			TYPE tVal;																															\
			std::memcpy( &tVal, &pescCache->pui8Base[_ui64Address-pescCache->ui64Address], sizeof( TYPE ) );									\
			_rResult.u.MEMBER = tVal;																											\
			return true;																														\
		}																																		\
		break;																																	\
	}

			MX_CASE( ee::EE_CT_INT8, int8_t, EE_NC_SIGNED, i64Val )
			MX_CASE( ee::EE_CT_INT16, int16_t, EE_NC_SIGNED, i64Val )
			MX_CASE( ee::EE_CT_INT32, int32_t, EE_NC_SIGNED, i64Val )
			MX_CASE( ee::EE_CT_INT64, int64_t, EE_NC_SIGNED, i64Val )

			MX_CASE( ee::EE_CT_UINT8, uint8_t, EE_NC_UNSIGNED, ui64Val )
			MX_CASE( ee::EE_CT_UINT16, uint16_t, EE_NC_UNSIGNED, ui64Val )
			MX_CASE( ee::EE_CT_UINT32, uint32_t, EE_NC_UNSIGNED, ui64Val )
			MX_CASE( ee::EE_CT_UINT64, uint64_t, EE_NC_UNSIGNED, ui64Val )

			MX_CASE( ee::EE_CT_FLOAT, float, EE_NC_FLOATING, dVal )
			MX_CASE( ee::EE_CT_DOUBLE, double, EE_NC_FLOATING, dVal )
#undef MX_CASE

			case ee::EE_CT_FLOAT16 : {
				if ( _ui64Address >= pescCache->ui64Address && _ui64Address <= (pescCache->ui64Address + pescCache->sBaseSize - sizeof( CFloat16 )) ) {
					_rResult.ncType = ee::EE_NC_FLOATING;
					uint16_t ui16Val;
					std::memcpy( &ui16Val, &pescCache->pui8Base[_ui64Address-pescCache->ui64Address], sizeof( ui16Val ) );
					_rResult.u.dVal = CFloat16( ui16Val ).Value();
					return true;
				}
				break;
			}
		}
		return ExpAddressHandler( _ui64Address, _tType, reinterpret_cast<uintptr_t>(pescCache->ppProcess), _peecContainer, _rResult );
	}

	// Called on the chunk list to scan multiple parts at the same time.
	DWORD WINAPI CSearcher::PrimitiveExactValueSearchThreadProc( LPVOID _lpParameter ) {
		MX_PRIM_SEARCH_THREAD * ppstParms = static_cast<MX_PRIM_SEARCH_THREAD *>(_lpParameter);
		CSearchResultLocker srlSearchLock( ppstParms->psrbResults );
		lsw::LSW_THREAD_PRIORITY tpThreadPri( ppstParms->pspParms->iThreadPriority );

		std::vector<uint8_t> vBuffer;
		uint64_t uiLargestChunk = std::max( ppstParms->paclChunks->LargestChunk( ppstParms->stStartIdx, ppstParms->stEndIdx ), 1ULL );
		while ( !vBuffer.size() ) {
			try {
				vBuffer.resize( uiLargestChunk );
			}
			catch ( const std::bad_alloc & /*_eE*/ ) {}
			uiLargestChunk = std::max( uiLargestChunk, 1024ULL * 1024ULL ) - 1024ULL * 1024ULL;
			if ( !uiLargestChunk && !vBuffer.size() ) {
				// Unable to allocate any memory at all??  In practice this code can't be executed.
				ppstParms->i32Result = -1;
				return false;
			}
		}

		// At this point vBuffer.size() is guaranteed not to be 0.
		for ( size_t I = ppstParms->stStartIdx; I < ppstParms->stEndIdx; ++I ) {
			for ( uint64_t J = 0; J < ppstParms->paclChunks->Chunks()[I].ui64Len; ) {
				// Buffer as much of the chunk as possible.
				size_t sSize = std::min( static_cast<size_t>(ppstParms->paclChunks->Chunks()[I].ui64Len - J), vBuffer.size() );
				if ( sSize < ppstParms->ui32Size ) { break; }
				if ( ppstParms->ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(ppstParms->paclChunks->Chunks()[I].ui64Start + J), &vBuffer[0], sSize, nullptr ) ) {
					PreprocessByteSwap( vBuffer.data(), sSize, (*ppstParms->pspParms) );
					// Go over the chunk and compare.
					
					size_t sEnd = sSize - ppstParms->ui32Size;
					for ( uint64_t K = 0; K <= sEnd; ) {
						if ( ppstParms->evcCompFunc( ppstParms->pspParms->dtLVal, (*reinterpret_cast<CUtilities::MX_DATA_TYPE *>(&vBuffer[K])), (*ppstParms->pspParms) ) ^ ppstParms->pspParms->bInvertResults ) {
							if ( !ppstParms->psrbResults->Add( ppstParms->paclChunks->Chunks()[I].ui64Start + J + K,
								&vBuffer[K], ppstParms->ui32Size ) ) {
								ppstParms->i32Result = -1;
								return false;
							}
						}
						K += ppstParms->pspParms->ui32Alignment;
					}

					J += sSize;
				}
				else {
					break;
				}
			}

			if ( !((*ppstParms->plProgress) & 0x7) && ppstParms->hProgressUpdate ) {
				::SendNotifyMessageW( ppstParms->hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>((((*ppstParms->plProgress) + 1.0) / ppstParms->paclChunks->Chunks().size()) * 100.0), 0L );
			}
			::InterlockedIncrement( ppstParms->plProgress );
			if ( ppstParms->pspParms->pbAbort && (*ppstParms->pspParms->pbAbort) ) { break; }
		}

		ppstParms->i32Result = 1;
		return true;
	}

	// Called on the chunk list to scan multiple parts at the same time.
	DWORD WINAPI CSearcher::PrimitiveRelativeSearchThreadProc( LPVOID _lpParameter ) {
		MX_PRIM_SEARCH_THREAD * ppstParms = static_cast<MX_PRIM_SEARCH_THREAD *>(_lpParameter);
		CSearchResultLocker srlSearchLock( ppstParms->psrbResults );
		lsw::LSW_THREAD_PRIORITY tpThreadPri( ppstParms->pspParms->iThreadPriority );

		std::vector<uint8_t> vBuffer;
		uint64_t uiLargestChunk = std::max( ppstParms->paclChunks->LargestChunk( ppstParms->stStartIdx, ppstParms->stEndIdx ), 1ULL );
		while ( !vBuffer.size() ) {
			try {
				vBuffer.resize( uiLargestChunk );
			}
			catch ( const std::bad_alloc & /*_eE*/ ) {}
			uiLargestChunk = std::max( uiLargestChunk, 1024ULL * 1024ULL ) - 1024ULL * 1024ULL;
			if ( !uiLargestChunk && !vBuffer.size() ) {
				// Unable to allocate any memory at all??  In practice this code can't be executed.
				ppstParms->i32Result = -1;
				return false;
			}
		}

		// At this point vBuffer.size() is guaranteed not to be 0.
		for ( size_t I = ppstParms->stStartIdx; I < ppstParms->stEndIdx; ++I ) {
			for ( uint64_t J = 0; J < ppstParms->paclChunks->Chunks()[I].ui64Len; ) {
				// Buffer as much of the chunk as possible.
				size_t sSize = std::min( static_cast<size_t>(ppstParms->paclChunks->Chunks()[I].ui64Len - J), vBuffer.size() );
				if ( sSize < ppstParms->ui32Size ) { break; }
				if ( ppstParms->ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(ppstParms->paclChunks->Chunks()[I].ui64Start + J), &vBuffer[0], sSize, nullptr ) ) {
					PreprocessByteSwap( vBuffer.data(), sSize, (*ppstParms->pspParms) );
					// Go over the chunk and compare.
					 
					size_t sEnd = sSize - ppstParms->ui32Size;
					for ( uint64_t K = 0; K <= sEnd; ) {
						if ( ppstParms->rcRelCompFunc( ppstParms->pspParms->dtLVal, (*reinterpret_cast<CUtilities::MX_DATA_TYPE *>(&vBuffer[K])) ) ^ ppstParms->pspParms->bInvertResults ) {
							if ( !ppstParms->psrbResults->Add( ppstParms->paclChunks->Chunks()[I].ui64Start + J + K,
								&vBuffer[K], ppstParms->ui32Size ) ) {
								ppstParms->i32Result = -1;
								return false;
							}
						}
						K += ppstParms->pspParms->ui32Alignment;
					}

					J += sSize;
				}
				else {
					break;
				}
			}

			if ( !((*ppstParms->plProgress) & 0x7) && ppstParms->hProgressUpdate ) {
				::SendNotifyMessageW( ppstParms->hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>((((*ppstParms->plProgress) + 1.0) / ppstParms->paclChunks->Chunks().size()) * 100.0), 0L );
			}
			::InterlockedIncrement( ppstParms->plProgress );
			if ( ppstParms->pspParms->pbAbort && (*ppstParms->pspParms->pbAbort) ) { break; }
		}

		ppstParms->i32Result = 1;
		return true;
	}

	// Called on the chunk list to scan multiple parts at the same time.
	DWORD WINAPI CSearcher::PrimitiveRangeSearchThreadProc( LPVOID _lpParameter ) {
		MX_PRIM_SEARCH_THREAD * ppstParms = static_cast<MX_PRIM_SEARCH_THREAD *>(_lpParameter);
		CSearchResultLocker srlSearchLock( ppstParms->psrbResults );
		lsw::LSW_THREAD_PRIORITY tpThreadPri( ppstParms->pspParms->iThreadPriority );

		std::vector<uint8_t> vBuffer;
		uint64_t uiLargestChunk = std::max( ppstParms->paclChunks->LargestChunk( ppstParms->stStartIdx, ppstParms->stEndIdx ), 1ULL );
		while ( !vBuffer.size() ) {
			try {
				vBuffer.resize( uiLargestChunk );
			}
			catch ( const std::bad_alloc & /*_eE*/ ) {}
			uiLargestChunk = std::max( uiLargestChunk, 1024ULL * 1024ULL ) - 1024ULL * 1024ULL;
			if ( !uiLargestChunk && !vBuffer.size() ) {
				// Unable to allocate any memory at all??  In practice this code can't be executed.
				ppstParms->i32Result = -1;
				return false;
			}
		}

		// At this point vBuffer.size() is guaranteed not to be 0.
		for ( size_t I = ppstParms->stStartIdx; I < ppstParms->stEndIdx; ++I ) {
			for ( uint64_t J = 0; J < ppstParms->paclChunks->Chunks()[I].ui64Len; ) {
				// Buffer as much of the chunk as possible.
				size_t sSize = std::min( static_cast<size_t>(ppstParms->paclChunks->Chunks()[I].ui64Len - J), vBuffer.size() );
				if ( sSize < ppstParms->ui32Size ) { break; }
				if ( ppstParms->ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(ppstParms->paclChunks->Chunks()[I].ui64Start + J), &vBuffer[0], sSize, nullptr ) ) {
					PreprocessByteSwap( vBuffer.data(), sSize, (*ppstParms->pspParms) );
					// Go over the chunk and compare.
					 
					size_t sEnd = sSize - ppstParms->ui32Size;
					for ( uint64_t K = 0; K <= sEnd; ) {
						if ( ppstParms->rcRangeCompFunc( ppstParms->pspParms->dtLVal, ppstParms->pspParms->dtRVal, (*reinterpret_cast<CUtilities::MX_DATA_TYPE *>(&vBuffer[K])) ) ^ ppstParms->pspParms->bInvertResults ) {
							if ( !ppstParms->psrbResults->Add( ppstParms->paclChunks->Chunks()[I].ui64Start + J + K,
								&vBuffer[K], ppstParms->ui32Size ) ) {
								ppstParms->i32Result = -1;
								return false;
							}
						}
						K += ppstParms->pspParms->ui32Alignment;
					}

					J += sSize;
				}
				else {
					break;
				}
			}

			if ( !((*ppstParms->plProgress) & 0x7) && ppstParms->hProgressUpdate ) {
				::SendNotifyMessageW( ppstParms->hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>((((*ppstParms->plProgress) + 1.0) / ppstParms->paclChunks->Chunks().size()) * 100.0), 0L );
			}
			::InterlockedIncrement( ppstParms->plProgress );
			if ( ppstParms->pspParms->pbAbort && (*ppstParms->pspParms->pbAbort) ) { break; }
		}

		ppstParms->i32Result = 1;
		return true;
	}

	// Called on the chunk list to scan multiple parts at the same time.
	DWORD WINAPI CSearcher::StringNormalSearchThreadProc( LPVOID _lpParameter ) {
		MX_UTF_SEARCH_THREAD * pustParms = static_cast<MX_UTF_SEARCH_THREAD *>(_lpParameter);
		CSearchResultLocker srlSearchLock( pustParms->psrbResults );
		lsw::LSW_THREAD_PRIORITY tpThreadPri( pustParms->pspParms->iThreadPriority );


		// Local raw buffer.
		std::vector<uint8_t> vBuffer;
		// Pre-op buffers.
		MX_STRING_SEARCH_UNPACKED ssuData;
		uint64_t uiLargestChunk = std::max( pustParms->paclChunks->LargestChunk( pustParms->stStartIdx, pustParms->stEndIdx ), 1ULL );
		bool bContinue = false;
		while ( !bContinue ) {
			bContinue = false;
			try {
				vBuffer.resize( uiLargestChunk );
				if ( CUtilities::NeedsType1( pustParms->pspParms->ui32StringSearchFlags, pustParms->pspParms->ui32StringSearchFlagsEx ) ) { ssuData.vBuffer1.resize( uiLargestChunk ); }
				if ( CUtilities::NeedsType2( pustParms->pspParms->ui32StringSearchFlags ) ) { ssuData.vBuffer2.resize( uiLargestChunk ); }
				if ( CUtilities::NeedsType3( pustParms->pspParms->ui32StringSearchFlags ) ) { ssuData.vBuffer3.resize( uiLargestChunk ); }
				ssuData.vDataBuffer.resize( uiLargestChunk );
				bContinue = true;
				break;
			}
			catch ( const std::bad_alloc & /*_eE*/ ) {}
			uiLargestChunk = std::max( uiLargestChunk, 1024ULL * 1024ULL ) - 1024ULL * 1024ULL;
			if ( !uiLargestChunk && !bContinue ) {
				// Unable to allocate any memory at all??  In practice this code can't be executed.
				pustParms->i32Result = -1;
				return false;
			}
		}


		bool bWholeWord = (pustParms->pspParms->ui32StringSearchFlagsEx & CUtilities::MX_SSF_WHOLE_WORD) != 0;
		// At this point vBuffer.size() is guaranteed not to be 0.
		for ( size_t I = pustParms->stStartIdx; I < pustParms->stEndIdx; ++I ) {
			for ( uint64_t J = 0; J < pustParms->paclChunks->Chunks()[I].ui64Len; ) {
				// Buffer as much of the chunk as possible.
				size_t sSize = std::min( static_cast<size_t>(pustParms->paclChunks->Chunks()[I].ui64Len - J), vBuffer.size() );
				if ( sSize < 1 ) { break; }
				if ( pustParms->ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(pustParms->paclChunks->Chunks()[I].ui64Start + J), &vBuffer[0], sSize, nullptr ) ) {
					PreprocessByteSwap( vBuffer.data(), sSize, (*pustParms->pspParms) );

					if ( pustParms->pspParms->pbAbort && (*pustParms->pspParms->pbAbort) ) { break; }
					// Pre-op.
					if ( !CUtilities::BuildStringDatabaseAnsi( &vBuffer[0], sSize,
						pustParms->pspParms->ui32StringSearchFlags, pustParms->pspParms->ui32StringSearchFlagsEx,
						ssuData.vBuffer1, ssuData.vBuffer2, ssuData.vBuffer3 ) ) { pustParms->i32Result = -1; return false; }
					if ( !CUtilities::ComposeDataIntoSearchableStringAnsi( &vBuffer[0], sSize, pustParms->pspParms->ui32StringSearchFlags, 0,
						ssuData.vBuffer1, ssuData.vBuffer3, ssuData.vDataBuffer, true /* Ignored. */, false /* Ignored. */ ) ) { pustParms->i32Result = -1; return false; }
					// Go over the chunk and compare.
					size_t sEnd = sSize;
					for ( uint64_t K = 0; K < sEnd; ) {
						if ( !bWholeWord || String_WholeWordLeft( ssuData, K, pustParms->pspParms->ui32Alignment ) ) {
							size_t sLen;
							if ( Cmp_ComposedStringCompare( (*pustParms->pssuUnpackedSrcString), ssuData,
								sSize,
								K,
								pustParms->pspParms->ui32StringSearchFlags, pustParms->pspParms->ui32StringSearchFlagsEx,
								sLen ) ) {
								if ( !pustParms->psrbResults->Add( pustParms->paclChunks->Chunks()[I].ui64Start + J + K,
									sLen, pustParms->pspParms->sstSearchType ) ) {
									pustParms->i32Result = -1;
									return false;
								}
								K += sLen;
								continue;
							}
						}
						K += pustParms->pspParms->ui32Alignment;
					}

					J += sSize;
				}
				else {
					break;
				}
			}

			if ( !((*pustParms->plProgress) & 0x7) && pustParms->hProgressUpdate ) {
				::SendNotifyMessageW( pustParms->hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>((((*pustParms->plProgress) + 1.0) / pustParms->paclChunks->Chunks().size()) * 100.0), 0L );
			}
			::InterlockedIncrement( pustParms->plProgress );
			if ( pustParms->pspParms->pbAbort && (*pustParms->pspParms->pbAbort) ) { break; }
		}

		pustParms->i32Result = 1;
		return true;
	}

	// Called on the chunk list to scan multiple parts at the same time.
	DWORD WINAPI CSearcher::StringUtfSearchThreadProc( LPVOID _lpParameter ) {
		MX_UTF_SEARCH_THREAD * pustParms = static_cast<MX_UTF_SEARCH_THREAD *>(_lpParameter);
		CSearchResultLocker srlSearchLock( pustParms->psrbResults );
		lsw::LSW_THREAD_PRIORITY tpThreadPri( pustParms->pspParms->iThreadPriority );


		// Local raw buffer.
		std::vector<uint8_t> vBuffer;
		// Pre-op buffers.
		MX_STRING_SEARCH_UNPACKED ssuData;
		uint64_t uiLargestChunk = std::max( pustParms->paclChunks->LargestChunk( pustParms->stStartIdx, pustParms->stEndIdx ), 1ULL );
		bool bContinue = false;
		while ( !bContinue ) {
			bContinue = false;
			try {
				vBuffer.resize( uiLargestChunk );
				if ( CUtilities::NeedsType1( pustParms->pspParms->ui32StringSearchFlags, pustParms->pspParms->ui32StringSearchFlagsEx ) ) { ssuData.vBuffer1.resize( uiLargestChunk ); }
				if ( CUtilities::NeedsType2( pustParms->pspParms->ui32StringSearchFlags ) ) { ssuData.vBuffer2.resize( uiLargestChunk ); }
				if ( CUtilities::NeedsType3( pustParms->pspParms->ui32StringSearchFlags ) ) { ssuData.vBuffer3.resize( uiLargestChunk ); }
				ssuData.vDataBuffer.resize( uiLargestChunk );
				bContinue = true;
				break;
			}
			catch ( const std::bad_alloc & /*_eE*/ ) {}
			uiLargestChunk = std::max( uiLargestChunk, 1024ULL * 1024ULL ) - 1024ULL * 1024ULL;
			if ( !uiLargestChunk && !bContinue ) {
				// Unable to allocate any memory at all??  In practice this code can't be executed.
				pustParms->i32Result = -1;
				return false;
			}
		}


		bool bWholeWord = (pustParms->pspParms->ui32StringSearchFlagsEx & CUtilities::MX_SSF_WHOLE_WORD) != 0;
		// At this point vBuffer.size() is guaranteed not to be 0.
		for ( size_t I = pustParms->stStartIdx; I < pustParms->stEndIdx; ++I ) {
			for ( uint64_t J = 0; J < pustParms->paclChunks->Chunks()[I].ui64Len; ) {
				// Buffer as much of the chunk as possible.
				size_t sSize = std::min( static_cast<size_t>(pustParms->paclChunks->Chunks()[I].ui64Len - J), vBuffer.size() );
				if ( sSize < 1 ) { break; }
				if ( pustParms->ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(pustParms->paclChunks->Chunks()[I].ui64Start + J), &vBuffer[0], sSize, nullptr ) ) {
					PreprocessByteSwap( vBuffer.data(), sSize, (*pustParms->pspParms) );

					if ( pustParms->pspParms->pbAbort && (*pustParms->pspParms->pbAbort) ) { break; }
					// Pre-op.
					switch ( pustParms->pspParms->sstSearchType ) {
						case CUtilities::MX_SST_UTF8 : {
							if ( !CUtilities::BuildStringDatabaseUtf8( &vBuffer[0], sSize,
								pustParms->pspParms->ui32StringSearchFlags, pustParms->pspParms->ui32StringSearchFlagsEx,
								ssuData.vBuffer1, ssuData.vBuffer2, ssuData.vBuffer3 ) ) { pustParms->i32Result = -1; return false; }
							if ( !CUtilities::ComposeDataIntoSearchableStringUtf8( &vBuffer[0], sSize, pustParms->pspParms->ui32StringSearchFlags, 0,
								ssuData.vBuffer1, ssuData.vBuffer3, ssuData.vDataBuffer, true /* Ignored. */, false /* Ignored. */ ) ) { pustParms->i32Result = -1; return false; }
							break;
						}
						case CUtilities::MX_SST_UTF16 : {
							if ( !CUtilities::BuildStringDatabaseUtf16( &vBuffer[0], sSize,
								pustParms->pspParms->ui32StringSearchFlags, pustParms->pspParms->ui32StringSearchFlagsEx,
								ssuData.vBuffer1, ssuData.vBuffer2, ssuData.vBuffer3 ) ) { pustParms->i32Result = -1; return false; }
							if ( !CUtilities::ComposeDataIntoSearchableStringUtf16( &vBuffer[0], sSize, pustParms->pspParms->ui32StringSearchFlags, 0,
								ssuData.vBuffer1, ssuData.vBuffer3, ssuData.vDataBuffer, pustParms->pspParms->bAligned, false ) ) { pustParms->i32Result = -1; return false; }
							break;
						}
						case CUtilities::MX_SST_UTF32 : {
							if ( !CUtilities::BuildStringDatabaseUtf32( &vBuffer[0], sSize,
								pustParms->pspParms->ui32StringSearchFlags, pustParms->pspParms->ui32StringSearchFlagsEx,
								ssuData.vBuffer1, ssuData.vBuffer2, ssuData.vBuffer3 ) ) { pustParms->i32Result = -1; return false; }
							if ( !CUtilities::ComposeDataIntoSearchableStringUtf32( &vBuffer[0], sSize, pustParms->pspParms->ui32StringSearchFlags, 0,
								ssuData.vBuffer1, ssuData.vBuffer3, ssuData.vDataBuffer, pustParms->pspParms->bAligned, false ) ) { pustParms->i32Result = -1; return false; }
							break;
						}
						case CUtilities::MX_SST_UTF16_BE : {
							if ( !CUtilities::BuildStringDatabaseUtf16( &vBuffer[0], sSize,
								pustParms->pspParms->ui32StringSearchFlags, pustParms->pspParms->ui32StringSearchFlagsEx,
								ssuData.vBuffer1, ssuData.vBuffer2, ssuData.vBuffer3 ) ) { pustParms->i32Result = -1; return false; }
							if ( !CUtilities::ComposeDataIntoSearchableStringUtf16( &vBuffer[0], sSize, pustParms->pspParms->ui32StringSearchFlags, 0,
								ssuData.vBuffer1, ssuData.vBuffer3, ssuData.vDataBuffer, pustParms->pspParms->bAligned, true ) ) { pustParms->i32Result = -1; return false; }
							break;
						}
						case CUtilities::MX_SST_UTF32_BE : {
							if ( !CUtilities::BuildStringDatabaseUtf32( &vBuffer[0], sSize,
								pustParms->pspParms->ui32StringSearchFlags, pustParms->pspParms->ui32StringSearchFlagsEx,
								ssuData.vBuffer1, ssuData.vBuffer2, ssuData.vBuffer3 ) ) { pustParms->i32Result = -1; return false; }
							if ( !CUtilities::ComposeDataIntoSearchableStringUtf32( &vBuffer[0], sSize, pustParms->pspParms->ui32StringSearchFlags, 0,
								ssuData.vBuffer1, ssuData.vBuffer3, ssuData.vDataBuffer, pustParms->pspParms->bAligned, true ) ) { pustParms->i32Result = -1; return false; }
							break;
						}
					}
					if ( pustParms->pspParms->pbAbort && (*pustParms->pspParms->pbAbort) ) { break; }
					// Go over the chunk and compare.
					size_t sEnd = sSize;
					for ( uint64_t K = 0; K < sEnd; ) {
						if ( !bWholeWord || String_WholeWordLeft( ssuData, K, pustParms->pspParms->ui32Alignment ) ) {
							size_t sLen;
							if ( Cmp_ComposedStringCompare( (*pustParms->pssuUnpackedSrcString), ssuData,
								sSize,
								K,
								pustParms->pspParms->ui32StringSearchFlags, pustParms->pspParms->ui32StringSearchFlagsEx,
								sLen ) ) {
								if ( !pustParms->psrbResults->Add( pustParms->paclChunks->Chunks()[I].ui64Start + J + K,
									sLen, pustParms->pspParms->sstSearchType ) ) {
									pustParms->i32Result = -1;
									return false;
								}
								K += sLen;
								continue;
							}
						}
						K += pustParms->pspParms->ui32Alignment;
					}

					J += sSize;
				}
				else {
					break;
				}
			}

			if ( !((*pustParms->plProgress) & 0x7) && pustParms->hProgressUpdate ) {
				::SendNotifyMessageW( pustParms->hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>((((*pustParms->plProgress) + 1.0) / pustParms->paclChunks->Chunks().size()) * 100.0), 0L );
			}
			::InterlockedIncrement( pustParms->plProgress );
			if ( pustParms->pspParms->pbAbort && (*pustParms->pspParms->pbAbort) ) { break; }
		}

		pustParms->i32Result = 1;
		return true;
	}

	// Called on the chunk list to scan multiple parts at the same time.
	DWORD WINAPI CSearcher::StringAllUtfSearchThreadProc( LPVOID _lpParameter ) {
		MX_UTF_SEARCH_THREAD * pustParms = static_cast<MX_UTF_SEARCH_THREAD *>(_lpParameter);
		CSearchResultLocker srlSearchLock( pustParms->psrbResults );
		lsw::LSW_THREAD_PRIORITY tpThreadPri( pustParms->pspParms->iThreadPriority );


		// Local raw buffer.
		std::vector<uint8_t> vBuffer;
		// Pre-op buffers.
		MX_STRING_SEARCH_UNPACKED ssuData[5];
		uint64_t uiLargestChunk = std::max( pustParms->paclChunks->LargestChunk( pustParms->stStartIdx, pustParms->stEndIdx ), 1ULL );
		bool bContinue = false;
		while ( !bContinue ) {
			bContinue = false;
			try {
				for ( size_t I = 0; I < 5; ++I ) {
					vBuffer.resize( uiLargestChunk );
					if ( CUtilities::NeedsType1( pustParms->pspParms->ui32StringSearchFlags, pustParms->pspParms->ui32StringSearchFlagsEx ) ) { ssuData[I].vBuffer1.resize( uiLargestChunk ); }
					if ( CUtilities::NeedsType2( pustParms->pspParms->ui32StringSearchFlags ) ) { ssuData[I].vBuffer2.resize( uiLargestChunk ); }
					if ( CUtilities::NeedsType3( pustParms->pspParms->ui32StringSearchFlags ) ) { ssuData[I].vBuffer3.resize( uiLargestChunk ); }
					ssuData[I].vDataBuffer.resize( uiLargestChunk );
				}
				bContinue = true;
				break;
			}
			catch ( const std::bad_alloc & /*_eE*/ ) {}
			uiLargestChunk = std::max( uiLargestChunk, 1024ULL * 1024ULL ) - 1024ULL * 1024ULL;
			if ( !uiLargestChunk && !bContinue ) {
				// Unable to allocate any memory at all??  In practice this code can't be executed.
				pustParms->i32Result = -1;
				return false;
			}
		}

		const uint32_t ui32Align[5] = {
			alignof( int8_t ),
			alignof( int16_t ),
			alignof( int32_t ),
			alignof( int16_t ),
			alignof( int32_t ),
		};
		const CUtilities::MX_STRING_SEARCH_TYPES sstTypes[5] = {
			CUtilities::MX_SST_UTF8,
			CUtilities::MX_SST_UTF16,
			CUtilities::MX_SST_UTF32,
			CUtilities::MX_SST_UTF16_BE,
			CUtilities::MX_SST_UTF32_BE,
		};
		
		bool bWholeWord = (pustParms->pspParms->ui32StringSearchFlagsEx & CUtilities::MX_SSF_WHOLE_WORD) != 0;
		// At this point vBuffer.size() is guaranteed not to be 0.
		for ( size_t I = pustParms->stStartIdx; I < pustParms->stEndIdx; ++I ) {
			for ( uint64_t J = 0; J < pustParms->paclChunks->Chunks()[I].ui64Len; ) {
				// Buffer as much of the chunk as possible.
				size_t sSize = std::min( static_cast<size_t>(pustParms->paclChunks->Chunks()[I].ui64Len - J), vBuffer.size() );
				if ( sSize < pustParms->ui32Size ) { break; }
				if ( pustParms->ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(pustParms->paclChunks->Chunks()[I].ui64Start + J), &vBuffer[0], sSize, nullptr ) ) {
					PreprocessByteSwap( vBuffer.data(), sSize, (*pustParms->pspParms) );

					if ( pustParms->pspParms->pbAbort && (*pustParms->pspParms->pbAbort) ) { break; }
					// Pre-op.
					if ( !CUtilities::BuildStringDatabaseUtf8( &vBuffer[0], sSize,
						pustParms->pspParms->ui32StringSearchFlags, pustParms->pspParms->ui32StringSearchFlagsEx,
						ssuData[0].vBuffer1, ssuData[0].vBuffer2, ssuData[0].vBuffer3 ) ) { pustParms->i32Result = -1; return false; }
					if ( !CUtilities::ComposeDataIntoSearchableStringUtf8( &vBuffer[0], sSize, pustParms->pspParms->ui32StringSearchFlags, 0,
						ssuData[0].vBuffer1, ssuData[0].vBuffer3, ssuData[0].vDataBuffer, false, false ) ) { pustParms->i32Result = -1; return false; }
					if ( pustParms->pspParms->pbAbort && (*pustParms->pspParms->pbAbort) ) { break; }

					if ( !CUtilities::BuildStringDatabaseUtf16( &vBuffer[0], sSize,
						pustParms->pspParms->ui32StringSearchFlags, pustParms->pspParms->ui32StringSearchFlagsEx,
						ssuData[1].vBuffer1, ssuData[1].vBuffer2, ssuData[1].vBuffer3 ) ) { pustParms->i32Result = -1; return false; }
					if ( !CUtilities::ComposeDataIntoSearchableStringUtf16( &vBuffer[0], sSize, pustParms->pspParms->ui32StringSearchFlags, 0,
						ssuData[1].vBuffer1, ssuData[1].vBuffer3, ssuData[1].vDataBuffer, false, false ) ) { pustParms->i32Result = -1; return false; }
					if ( pustParms->pspParms->pbAbort && (*pustParms->pspParms->pbAbort) ) { break; }

					if ( !CUtilities::BuildStringDatabaseUtf32( &vBuffer[0], sSize,
						pustParms->pspParms->ui32StringSearchFlags, pustParms->pspParms->ui32StringSearchFlagsEx,
						ssuData[2].vBuffer1, ssuData[2].vBuffer2, ssuData[2].vBuffer3 ) ) { pustParms->i32Result = -1; return false; }
					if ( !CUtilities::ComposeDataIntoSearchableStringUtf32( &vBuffer[0], sSize, pustParms->pspParms->ui32StringSearchFlags, 0,
						ssuData[2].vBuffer1, ssuData[2].vBuffer3, ssuData[2].vDataBuffer, false, false ) ) { pustParms->i32Result = -1; return false; }

					// == BE.
					if ( !CUtilities::BuildStringDatabaseUtf16( &vBuffer[0], sSize,
						pustParms->pspParms->ui32StringSearchFlags, pustParms->pspParms->ui32StringSearchFlagsEx,
						ssuData[3].vBuffer1, ssuData[3].vBuffer2, ssuData[3].vBuffer3 ) ) { pustParms->i32Result = -1; return false; }
					if ( !CUtilities::ComposeDataIntoSearchableStringUtf16( &vBuffer[0], sSize, pustParms->pspParms->ui32StringSearchFlags, 0,
						ssuData[3].vBuffer1, ssuData[3].vBuffer3, ssuData[3].vDataBuffer, false, true ) ) { pustParms->i32Result = -1; return false; }
					if ( pustParms->pspParms->pbAbort && (*pustParms->pspParms->pbAbort) ) { break; }

					if ( !CUtilities::BuildStringDatabaseUtf32( &vBuffer[0], sSize,
						pustParms->pspParms->ui32StringSearchFlags, pustParms->pspParms->ui32StringSearchFlagsEx,
						ssuData[4].vBuffer1, ssuData[4].vBuffer2, ssuData[4].vBuffer3 ) ) { pustParms->i32Result = -1; return false; }
					if ( !CUtilities::ComposeDataIntoSearchableStringUtf32( &vBuffer[0], sSize, pustParms->pspParms->ui32StringSearchFlags, 0,
						ssuData[4].vBuffer1, ssuData[4].vBuffer3, ssuData[4].vDataBuffer, false, true ) ) { pustParms->i32Result = -1; return false; }
					if ( pustParms->pspParms->pbAbort && (*pustParms->pspParms->pbAbort) ) { break; }
					// Go over the chunk and compare.
					size_t sEnd = sSize;
					size_t sOnlyMatchAfter[5] = { 0 };
					for ( uint64_t K = 0; K < sEnd; ) {
						size_t sMinLen = 0;
						uint32_t uiSuccessCount = 0;
						for ( size_t M = 0; M < 5; ++M ) {
							if ( K < sOnlyMatchAfter[M] ) { continue; }
							if ( !bWholeWord || String_WholeWordLeft( ssuData[M], K, ui32Align[M] ) ) {
								size_t sLen;
								if ( Cmp_ComposedStringCompare( (*pustParms->pssuUnpackedSrcString), ssuData[M],
									sSize,
									K,
									pustParms->pspParms->ui32StringSearchFlags, pustParms->pspParms->ui32StringSearchFlagsEx,
									sLen ) ) {
									if ( !sMinLen ) { sMinLen = sLen; }
									else { sMinLen = std::min( sMinLen, sLen ); }
									if ( !pustParms->psrbResults->Add( pustParms->paclChunks->Chunks()[I].ui64Start + J + K,
										sLen, sstTypes[M] ) ) {
										pustParms->i32Result = -1;
										return false;
									}
									sOnlyMatchAfter[M] = K + sLen;
									++uiSuccessCount;
								}
							}
						}
						if ( uiSuccessCount == 3 ) { K += sMinLen; }
						else { ++K; }
					}
					J += sSize;
				}
				else {
					break;
				}
			}

			if ( !((*pustParms->plProgress) & 0x7) && pustParms->hProgressUpdate ) {
				::SendNotifyMessageW( pustParms->hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>((((*pustParms->plProgress) + 1.0) / pustParms->paclChunks->Chunks().size()) * 100.0), 0L );
			}
			::InterlockedIncrement( pustParms->plProgress );
			if ( pustParms->pspParms->pbAbort && (*pustParms->pspParms->pbAbort) ) { break; }
		}

		pustParms->i32Result = 1;
		return true;
	}

	// Called on the chunk list to scan multiple parts at the same time.
	template <typename _tType>
	DWORD WINAPI CSearcher::StringRawArraySearchThreadProc( LPVOID _lpParameter ) {
		MX_RAW_ARRAY_SEARCH_THREAD * pustParms = static_cast<MX_RAW_ARRAY_SEARCH_THREAD *>(_lpParameter);
		CSearchResultLocker srlSearchLock( pustParms->psrbResults );
		lsw::LSW_THREAD_PRIORITY tpThreadPri( pustParms->pspParms->iThreadPriority );


		size_t sLeftLenInChars = pustParms->pspParms->vStringSearchValue.size() / sizeof( _tType );


		// Local raw buffer.
		std::vector<uint8_t> vBuffer;
		// Pre-op buffers.
		uint64_t uiLargestChunk = std::max( pustParms->paclChunks->LargestChunk( pustParms->stStartIdx, pustParms->stEndIdx ), 1ULL );
		bool bContinue = false;
		while ( !bContinue ) {
			bContinue = false;
			try {
				vBuffer.resize( uiLargestChunk );
				bContinue = true;
				break;
			}
			catch ( const std::bad_alloc & /*_eE*/ ) {}
			uiLargestChunk = std::max( uiLargestChunk, 1024ULL * 1024ULL ) - 1024ULL * 1024ULL;
			if ( !uiLargestChunk && !bContinue ) {
				// Unable to allocate any memory at all??  In practice this code can't be executed.
				pustParms->i32Result = -1;
				return false;
			}
		}


		// At this point vBuffer.size() is guaranteed not to be 0.
		for ( size_t I = pustParms->stStartIdx; I < pustParms->stEndIdx; ++I ) {
			for ( uint64_t J = 0; J < pustParms->paclChunks->Chunks()[I].ui64Len; ) {
				// Buffer as much of the chunk as possible.
				size_t sSize = std::min( static_cast<size_t>(pustParms->paclChunks->Chunks()[I].ui64Len - J), vBuffer.size() );
				if ( sSize < 1 ) { break; }
				if ( pustParms->ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(pustParms->paclChunks->Chunks()[I].ui64Start + J), &vBuffer[0], sSize, nullptr ) ) {
					PreprocessByteSwap( vBuffer.data(), sSize, (*pustParms->pspParms) );

					// Go over the chunk and compare.
					size_t sEnd = sSize;
					for ( uint64_t K = 0; K < sEnd; ) {
						size_t sLen;
						if ( Cmp_DataArrayCompare<_tType>( reinterpret_cast<const void *>(pustParms->pspParms->vStringSearchValue.data()), sLeftLenInChars,
							pustParms->pspParms->vStringMetaData,
							&vBuffer[K], sEnd - K,
							sLen ) ) {
							if ( !pustParms->psrbResults->Add( pustParms->paclChunks->Chunks()[I].ui64Start + J + K,
								sLen, (pustParms->pspParms->dtStringCharType << 8) | pustParms->pspParms->sstSearchType ) ) {
								pustParms->i32Result = -1;
								return false;
							}
							K += sLen;
							continue;
						}
						K += pustParms->pspParms->ui32Alignment;
					}

					J += sSize;
				}
				else {
					break;
				}
			}

			if ( !((*pustParms->plProgress) & 0x7) && pustParms->hProgressUpdate ) {
				::SendNotifyMessageW( pustParms->hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>((((*pustParms->plProgress) + 1.0) / pustParms->paclChunks->Chunks().size()) * 100.0), 0L );
			}
			::InterlockedIncrement( pustParms->plProgress );
			if ( pustParms->pspParms->pbAbort && (*pustParms->pspParms->pbAbort) ) { break; }
		}

		pustParms->i32Result = 1;
		return true;
	}

	// Called on the chunk list to scan multiple parts at the same time.
	DWORD WINAPI CSearcher::StringRegexSearchThreadProc( LPVOID _lpParameter ) {
		MX_RAW_ARRAY_SEARCH_THREAD * pustParms = static_cast<MX_RAW_ARRAY_SEARCH_THREAD *>(_lpParameter);
		CSearchResultLocker srlSearchLock( pustParms->psrbResults );
		lsw::LSW_THREAD_PRIORITY tpThreadPri( pustParms->pspParms->iThreadPriority );

		// =======================================
		// == Prepare the string for searching. ==
		// =======================================
		COnigurumaRegex orRegex( pustParms->pspParms->vStringSearchValue.data(), pustParms->pspParms->vStringSearchValue.size(),
			COnigurumaSystem::RegexOptionsToLibrary( pustParms->pspParms->ui32RegexFlags ),
			COnigurumaSystem::RegexEncodingToLibrary( pustParms->pspParms->reRegexEncoding ),
			COnigurumaSystem::RegexFlavorToLibrary( pustParms->pspParms->rfRegexFlavor ) );
		if ( orRegex.Get() != ONIG_NORMAL ) {
			pustParms->i32Result = -1;
			return false;
		}

		COnigurumaRegion orRegion;
		// Local raw buffer.
		std::vector<uint8_t> vBuffer;
		// Pre-op buffers.
		uint64_t uiLargestChunk = std::max( pustParms->paclChunks->LargestChunk( pustParms->stStartIdx, pustParms->stEndIdx ), 1ULL );
		bool bContinue = false;
		while ( !bContinue ) {
			bContinue = false;
			try {
				vBuffer.resize( uiLargestChunk );
				bContinue = true;
				break;
			}
			catch ( const std::bad_alloc & /*_eE*/ ) {}
			uiLargestChunk = std::max( uiLargestChunk, 1024ULL * 1024ULL ) - 1024ULL * 1024ULL;
			if ( !uiLargestChunk && !bContinue ) {
				// Unable to allocate any memory at all??  In practice this code can't be executed.
				pustParms->i32Result = -1;
				return false;
			}
		}


		// At this point vBuffer.size() is guaranteed not to be 0.
		for ( size_t I = pustParms->stStartIdx; I < pustParms->stEndIdx; ++I ) {
			for ( uint64_t J = 0; J < pustParms->paclChunks->Chunks()[I].ui64Len; ) {
				// Buffer as much of the chunk as possible.
				size_t sSize = std::min( static_cast<size_t>(pustParms->paclChunks->Chunks()[I].ui64Len - J), vBuffer.size() );
				if ( sSize < 1 ) { break; }
				if ( pustParms->ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(pustParms->paclChunks->Chunks()[I].ui64Start + J), &vBuffer[0], sSize, nullptr ) ) {
					PreprocessByteSwap( vBuffer.data(), sSize, (*pustParms->pspParms) );

					// Go over the chunk and compare.
					size_t sEnd = sSize;
					for ( uint64_t K = 0; K < sEnd; ) {
						size_t sLen, sAdjust = pustParms->pspParms->ui32Alignment;
						int iMatch = orRegex.Match( vBuffer.data(), sSize, K, orRegion.Get(), 0 );
						if ( iMatch > 0 ) {
							for ( int iRegs = 0; iRegs < orRegion.Get()->num_regs; ++iRegs ) {
								sLen = orRegion.Get()->end[iRegs] - orRegion.Get()->beg[iRegs];
								sAdjust = std::max( sAdjust, sLen );
								if ( !pustParms->psrbResults->Add( pustParms->paclChunks->Chunks()[I].ui64Start + J + K,
									sLen, (pustParms->pspParms->reRegexEncoding << 8) | pustParms->pspParms->sstSearchType ) ) {
									pustParms->i32Result = -1;
									return false;
								}
							}
						}
						K += sAdjust;
					}

					J += sSize;
				}
				else {
					break;
				}
			}

			if ( !((*pustParms->plProgress) & 0x7) && pustParms->hProgressUpdate ) {
				::SendNotifyMessageW( pustParms->hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>((((*pustParms->plProgress) + 1.0) / pustParms->paclChunks->Chunks().size()) * 100.0), 0L );
			}
			::InterlockedIncrement( pustParms->plProgress );
			if ( pustParms->pspParms->pbAbort && (*pustParms->pspParms->pbAbort) ) { break; }
		}

		pustParms->i32Result = 1;
		return true;
	}

	// Called on the chunk list to scan multiple parts at the same time.
	DWORD WINAPI CSearcher::StringMultiArraySearchThreadProc( LPVOID _lpParameter ) {
		MX_RAW_ARRAY_SEARCH_THREAD * pustParms = static_cast<MX_RAW_ARRAY_SEARCH_THREAD *>(_lpParameter);
		CSearchResultLocker srlSearchLock( pustParms->psrbResults );
		lsw::LSW_THREAD_PRIORITY tpThreadPri( pustParms->pspParms->iThreadPriority );

		// =======================================
		// == Prepare the string for searching. ==
		// =======================================
		// Local raw buffer.
		std::vector<uint8_t> vBuffer;
		// Pre-op buffers.
		uint64_t uiLargestChunk = std::max( pustParms->paclChunks->LargestChunk( pustParms->stStartIdx, pustParms->stEndIdx ), 1ULL );
		bool bContinue = false;
		while ( !bContinue ) {
			bContinue = false;
			try {
				vBuffer.resize( uiLargestChunk );
				bContinue = true;
				break;
			}
			catch ( const std::bad_alloc & /*_eE*/ ) {}
			uiLargestChunk = std::max( uiLargestChunk, 1024ULL * 1024ULL ) - 1024ULL * 1024ULL;
			if ( !uiLargestChunk && !bContinue ) {
				// Unable to allocate any memory at all??  In practice this code can't be executed.
				pustParms->i32Result = -1;
				return false;
			}
		}


		// At this point vBuffer.size() is guaranteed not to be 0.
		for ( size_t I = pustParms->stStartIdx; I < pustParms->stEndIdx; ++I ) {
			for ( uint64_t J = 0; J < pustParms->paclChunks->Chunks()[I].ui64Len; ) {
				// Buffer as much of the chunk as possible.
				size_t sSize = std::min( static_cast<size_t>(pustParms->paclChunks->Chunks()[I].ui64Len - J), vBuffer.size() );
				if ( sSize < 1 ) { break; }
				if ( pustParms->ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(pustParms->paclChunks->Chunks()[I].ui64Start + J), &vBuffer[0], sSize, nullptr ) ) {
					PreprocessByteSwap( vBuffer.data(), sSize, (*pustParms->pspParms) );

					// Go over the chunk and compare.
					size_t sEnd = sSize;
					for ( uint64_t K = 0; K < sEnd; ) {
						size_t sLen;
						if ( Cmp_DataArrayCompare_MultiType( reinterpret_cast<const void *>(pustParms->pspParms->vStringSearchValue.data()),
							pustParms->pspParms->vStringMetaData,
							&vBuffer[K], sEnd - K,
							sLen ) ) {
							if ( !pustParms->psrbResults->Add( pustParms->paclChunks->Chunks()[I].ui64Start + J + K,
								sLen, (pustParms->pspParms->dtStringCharType << 8) | pustParms->pspParms->sstSearchType ) ) {
								pustParms->i32Result = -1;
								return false;
							}
						}
						K += pustParms->pspParms->ui32Alignment;
					}

					J += sSize;
				}
				else {
					break;
				}
			}

			if ( !((*pustParms->plProgress) & 0x7) && pustParms->hProgressUpdate ) {
				::SendNotifyMessageW( pustParms->hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>((((*pustParms->plProgress) + 1.0) / pustParms->paclChunks->Chunks().size()) * 100.0), 0L );
			}
			::InterlockedIncrement( pustParms->plProgress );
			if ( pustParms->pspParms->pbAbort && (*pustParms->pspParms->pbAbort) ) { break; }
		}

		pustParms->i32Result = 1;
		return true;
	}

	// Called on the chunk list to scan multiple parts at the same time.
	template <typename _tType, unsigned _sIsFloat16>
	DWORD WINAPI CSearcher::PredefExpressionSearchThreadProc( LPVOID _lpParameter ) {
		MX_EXP_SEARCH_THREAD * pustParms = static_cast<MX_EXP_SEARCH_THREAD *>(_lpParameter);
		CSearchResultLocker srlSearchLock( pustParms->psrbResults );
		lsw::LSW_THREAD_PRIORITY tpThreadPri( pustParms->pspParms->iThreadPriority );


		// =================
		// == Expression. ==
		// =================
		CExpression eExp;
		if ( !eExp.SetExpression( pustParms->pspParms->ssExpression.c_str() ) ) { pustParms->i32Result = -1; return false; }
		if ( !eExp.GetContainer() ) { pustParms->i32Result = -1; return false; }
		if ( !eExp.GetContainer()->HasAccessToUserData() ) { pustParms->i32Result = -1; return false; }
		// Since the address is not given to the script, we can't make meaningful predictions about what it will try to access.
		// Don't try to optimize it; just let it read what it reads.
		eExp.GetContainer()->SetAddressHandler( ExpAddressHandler, reinterpret_cast<uintptr_t>(pustParms->ppProcess) );

		// UserVarHandler just copies whatever the uintptr_t points to, so just point it to rRes and then just update rRes during the search.
		ee::CExpEvalContainer::EE_RESULT rRes;
		eExp.GetContainer()->SetUserHandler( UserVarHandler, reinterpret_cast<uintptr_t>(&rRes) );
		bool bIsFloat = false;
		bool bIsSigned = false;
		if ( CUtilities::DataTypeIsFloat( pustParms->pspParms->dtLVal.dtType ) ) { rRes.ncType = ee::EE_NC_FLOATING; bIsFloat = true; }
		else if ( CUtilities::DataTypeIsSigned( pustParms->pspParms->dtLVal.dtType ) ) { rRes.ncType = ee::EE_NC_SIGNED; bIsSigned = true; }
		else { rRes.ncType = ee::EE_NC_UNSIGNED; }
		// =================
		// =================


		std::vector<uint8_t> vBuffer;
		uint64_t uiLargestChunk = std::max( pustParms->paclChunks->LargestChunk( pustParms->stStartIdx, pustParms->stEndIdx ), 1ULL );
		while ( !vBuffer.size() ) {
			try {
				vBuffer.resize( uiLargestChunk );
			}
			catch ( const std::bad_alloc & /*_eE*/ ) {}
			uiLargestChunk = std::max( uiLargestChunk, 1024ULL * 1024ULL ) - 1024ULL * 1024ULL;
			if ( !uiLargestChunk && !vBuffer.size() ) {
				// Unable to allocate any memory at all??  In practice this code can't be executed.
				pustParms->i32Result = -1;
				return false;
			}
		}

		// At this point vBuffer.size() is guaranteed not to be 0.
		for ( size_t I = pustParms->stStartIdx; I < pustParms->stEndIdx; ++I ) {
			for ( uint64_t J = 0; J < pustParms->paclChunks->Chunks()[I].ui64Len; ) {
				// Buffer as much of the chunk as possible.
				size_t sSize = std::min( static_cast<size_t>(pustParms->paclChunks->Chunks()[I].ui64Len - J), vBuffer.size() );
				if ( sSize < pustParms->ui32Size ) { break; }
				if ( pustParms->ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(pustParms->paclChunks->Chunks()[I].ui64Start + J), &vBuffer[0], sSize, nullptr ) ) {
					PreprocessByteSwap( vBuffer.data(), sSize, (*pustParms->pspParms) );
					// Go over the chunk and compare.
					
					size_t sEnd = sSize - pustParms->ui32Size;
					for ( uint64_t K = 0; K <= sEnd; ) {
						if ( _sIsFloat16 ) {
							rRes.u.dVal = CFloat16( (*reinterpret_cast<uint16_t *>(&vBuffer[K])) ).Value();
						}
						else {
							if ( bIsFloat ) {
								rRes.u.dVal = (*reinterpret_cast<_tType *>(&vBuffer[K]));
							}
							else if ( bIsSigned ) {
								rRes.u.i64Val = (*reinterpret_cast<_tType *>(&vBuffer[K]));
							}
							else {
								rRes.u.ui64Val = (*reinterpret_cast<_tType *>(&vBuffer[K]));
							}
						}
						ee::CExpEvalContainer::EE_RESULT rThisRes;
						if ( eExp.GetContainer()->Resolve( rThisRes ) ) {
							rThisRes = ee::CExpEvalContainer::ConvertResult( rThisRes, ee::EE_NC_UNSIGNED );
							if ( (rThisRes.u.ui64Val && !pustParms->pspParms->bInvertResults) ||
								!rThisRes.u.ui64Val && pustParms->pspParms->bInvertResults) {
								if ( !pustParms->psrbResults->Add( pustParms->paclChunks->Chunks()[I].ui64Start + J + K,
									&vBuffer[K], pustParms->ui32Size ) ) {
									pustParms->i32Result = -1;
									return false;
								}
							}
						}
						/*if ( pustParms->evcCompFunc( pustParms->pspParms->dtLVal, (*reinterpret_cast<CUtilities::MX_DATA_TYPE *>(&vBuffer[K])), (*pustParms->pspParms) ) ^ pustParms->pspParms->bInvertResults ) {
							if ( !pustParms->psrbResults->Add( pustParms->paclChunks->Chunks()[I].ui64Start + J + K,
								&vBuffer[K], pustParms->ui32Size ) ) {
								pustParms->i32Result = -1;
								return false;
							}
						}*/
						K += pustParms->pspParms->ui32Alignment;
					}

					J += sSize;
				}
				else {
					break;
				}
			}

			if ( !((*pustParms->plProgress) & 0x7) && pustParms->hProgressUpdate ) {
				::SendNotifyMessageW( pustParms->hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>((((*pustParms->plProgress) + 1.0) / pustParms->paclChunks->Chunks().size()) * 100.0), 0L );
			}
			::InterlockedIncrement( pustParms->plProgress );
			if ( pustParms->pspParms->pbAbort && (*pustParms->pspParms->pbAbort) ) { break; }
		}

		pustParms->i32Result = 1;
		return true;
	}

	// Called on the chunk list to scan multiple parts at the same time.
	DWORD WINAPI CSearcher::FixedSizeExpressionSearchThreadProc( LPVOID _lpParameter ) {
		MX_EXP_SEARCH_THREAD * pustParms = static_cast<MX_EXP_SEARCH_THREAD *>(_lpParameter);
		CSearchResultLocker srlSearchLock( pustParms->psrbResults );
		lsw::LSW_THREAD_PRIORITY tpThreadPri( pustParms->pspParms->iThreadPriority );


		// =================
		// == Expression. ==
		// =================
		CExpression eExp;
		if ( !eExp.SetExpression( pustParms->pspParms->ssExpression.c_str() ) ) { pustParms->i32Result = -1; return false; }
		if ( !eExp.GetContainer() ) { pustParms->i32Result = -1; return false; }
		if ( !eExp.GetContainer()->HasAccessToUserData() ) { pustParms->i32Result = -1; return false; }
		// Since ?? is an address, optimize for reading local addresses.
		MX_EXP_SEARCH_CACHE escSearchCache;
		escSearchCache.ppProcess = pustParms->ppProcess;		
		eExp.GetContainer()->SetAddressHandler( ExpAddressHandler_LocalPrecache, reinterpret_cast<uintptr_t>(&escSearchCache) );

		// UserVarHandler just copies whatever the uintptr_t points to, so just point it to rRes and then just update rRes during the search.
		ee::CExpEvalContainer::EE_RESULT rRes;
		eExp.GetContainer()->SetUserHandler( UserVarHandler, reinterpret_cast<uintptr_t>(&rRes) );
		bool bIsFloat = false;
		bool bIsSigned = false;
		if ( CUtilities::DataTypeIsFloat( pustParms->pspParms->dtLVal.dtType ) ) { rRes.ncType = ee::EE_NC_FLOATING; bIsFloat = true; }
		else if ( CUtilities::DataTypeIsSigned( pustParms->pspParms->dtLVal.dtType ) ) { rRes.ncType = ee::EE_NC_SIGNED; bIsSigned = true; }
		else { rRes.ncType = ee::EE_NC_UNSIGNED; }
		// =================
		// =================


		std::vector<uint8_t> vBuffer;
		uint64_t uiLargestChunk = std::max( pustParms->paclChunks->LargestChunk( pustParms->stStartIdx, pustParms->stEndIdx ), 1ULL );
		while ( !vBuffer.size() ) {
			try {
				vBuffer.resize( uiLargestChunk );
			}
			catch ( const std::bad_alloc & /*_eE*/ ) {}
			uiLargestChunk = std::max( uiLargestChunk, 1024ULL * 1024ULL ) - 1024ULL * 1024ULL;
			if ( !uiLargestChunk && !vBuffer.size() ) {
				// Unable to allocate any memory at all??  In practice this code can't be executed.
				pustParms->i32Result = -1;
				return false;
			}
		}
		escSearchCache.pui8Base = vBuffer.data();
		

		// At this point vBuffer.size() is guaranteed not to be 0.
		for ( size_t I = pustParms->stStartIdx; I < pustParms->stEndIdx; ++I ) {
			for ( uint64_t J = 0; J < pustParms->paclChunks->Chunks()[I].ui64Len; ) {
				// Buffer as much of the chunk as possible.
				size_t sSize = std::min( static_cast<size_t>(pustParms->paclChunks->Chunks()[I].ui64Len - J), vBuffer.size() );
				if ( sSize < pustParms->ui32Size ) { break; }
				if ( pustParms->ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(pustParms->paclChunks->Chunks()[I].ui64Start + J), &vBuffer[0], sSize, nullptr ) ) {
					escSearchCache.sBaseSize = sSize;
					escSearchCache.ui64Address = pustParms->paclChunks->Chunks()[I].ui64Start + J;
					PreprocessByteSwap( vBuffer.data(), sSize, (*pustParms->pspParms) );
					// Go over the chunk and compare.
					
					size_t sEnd = sSize - pustParms->ui32Size;
					for ( uint64_t K = 0; K <= sEnd; ) {
						rRes.u.ui64Val = pustParms->paclChunks->Chunks()[I].ui64Start + J + K;
						ee::CExpEvalContainer::EE_RESULT rThisRes;
						if ( eExp.GetContainer()->Resolve( rThisRes ) ) {
							rThisRes = ee::CExpEvalContainer::ConvertResult( rThisRes, ee::EE_NC_UNSIGNED );
							if ( rThisRes.u.ui64Val ) {
								if ( !pustParms->psrbResults->Add( pustParms->paclChunks->Chunks()[I].ui64Start + J + K,
									&vBuffer[K], pustParms->ui32Size ) ) {
									pustParms->i32Result = -1;
									return false;
								}
							}
						}
						K += pustParms->pspParms->ui32Alignment;
					}

					J += sSize;
				}
				else {
					break;
				}
			}

			if ( !((*pustParms->plProgress) & 0x7) && pustParms->hProgressUpdate ) {
				::SendNotifyMessageW( pustParms->hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>((((*pustParms->plProgress) + 1.0) / pustParms->paclChunks->Chunks().size()) * 100.0), 0L );
			}
			::InterlockedIncrement( pustParms->plProgress );
			if ( pustParms->pspParms->pbAbort && (*pustParms->pspParms->pbAbort) ) { break; }
		}

		pustParms->i32Result = 1;
		return true;
	}

	// Called on the chunk list to scan multiple parts at the same time.
	DWORD WINAPI CSearcher::DynamicSizeExpressionSearchThreadProc( LPVOID _lpParameter ) {
		MX_EXP_SEARCH_THREAD * pustParms = static_cast<MX_EXP_SEARCH_THREAD *>(_lpParameter);
		CSearchResultLocker srlSearchLock( pustParms->psrbResults );
		lsw::LSW_THREAD_PRIORITY tpThreadPri( pustParms->pspParms->iThreadPriority );


		// =================
		// == Expression. ==
		// =================
		CExpression eExp;
		if ( !eExp.SetExpression( pustParms->pspParms->ssExpression.c_str() ) ) { pustParms->i32Result = -1; return false; }
		if ( !eExp.GetContainer() ) { pustParms->i32Result = -1; return false; }
		if ( !eExp.GetContainer()->HasAccessToUserData() ) { pustParms->i32Result = -1; return false; }
		// Since ?? is an address, optimize for reading local addresses.
		MX_EXP_SEARCH_CACHE escSearchCache;
		escSearchCache.ppProcess = pustParms->ppProcess;		
		eExp.GetContainer()->SetAddressHandler( ExpAddressHandler_LocalPrecache, reinterpret_cast<uintptr_t>(&escSearchCache) );

		// UserVarHandler just copies whatever the uintptr_t points to, so just point it to rRes and then just update rRes during the search.
		ee::CExpEvalContainer::EE_RESULT rRes;
		eExp.GetContainer()->SetUserHandler( UserVarHandler, reinterpret_cast<uintptr_t>(&rRes) );
		bool bIsFloat = false;
		bool bIsSigned = false;
		if ( CUtilities::DataTypeIsFloat( pustParms->pspParms->dtLVal.dtType ) ) { rRes.ncType = ee::EE_NC_FLOATING; bIsFloat = true; }
		else if ( CUtilities::DataTypeIsSigned( pustParms->pspParms->dtLVal.dtType ) ) { rRes.ncType = ee::EE_NC_SIGNED; bIsSigned = true; }
		else { rRes.ncType = ee::EE_NC_UNSIGNED; }
		// =================
		// =================


		std::vector<uint8_t> vBuffer;
		uint64_t uiLargestChunk = std::max( pustParms->paclChunks->LargestChunk( pustParms->stStartIdx, pustParms->stEndIdx ), 1ULL );
		while ( !vBuffer.size() ) {
			try {
				vBuffer.resize( uiLargestChunk );
			}
			catch ( const std::bad_alloc & /*_eE*/ ) {}
			uiLargestChunk = std::max( uiLargestChunk, 1024ULL * 1024ULL ) - 1024ULL * 1024ULL;
			if ( !uiLargestChunk && !vBuffer.size() ) {
				// Unable to allocate any memory at all??  In practice this code can't be executed.
				pustParms->i32Result = -1;
				return false;
			}
		}
		escSearchCache.pui8Base = vBuffer.data();


		// At this point vBuffer.size() is guaranteed not to be 0.
		for ( size_t I = pustParms->stStartIdx; I < pustParms->stEndIdx; ++I ) {
			for ( uint64_t J = 0; J < pustParms->paclChunks->Chunks()[I].ui64Len; ) {
				// Buffer as much of the chunk as possible.
				size_t sSize = std::min( static_cast<size_t>(pustParms->paclChunks->Chunks()[I].ui64Len - J), vBuffer.size() );
				if ( sSize < pustParms->ui32Size ) { break; }
				if ( pustParms->ppProcess->ReadProcessMemory( reinterpret_cast<LPCVOID>(pustParms->paclChunks->Chunks()[I].ui64Start + J), &vBuffer[0], sSize, nullptr ) ) {
					escSearchCache.sBaseSize = sSize;
					escSearchCache.ui64Address = pustParms->paclChunks->Chunks()[I].ui64Start + J;
					PreprocessByteSwap( vBuffer.data(), sSize, (*pustParms->pspParms) );
					// Go over the chunk and compare.
					
					size_t sEnd = sSize - pustParms->ui32Size;
					for ( uint64_t K = 0; K <= sEnd; ) {
						rRes.u.ui64Val = pustParms->paclChunks->Chunks()[I].ui64Start + J + K;
						ee::CExpEvalContainer::EE_RESULT rThisRes;
						if ( eExp.GetContainer()->Resolve( rThisRes ) ) {
							rThisRes = ee::CExpEvalContainer::ConvertResult( rThisRes, ee::EE_NC_UNSIGNED );
							if ( rThisRes.u.ui64Val ) {
								if ( !pustParms->psrbResults->Add( pustParms->paclChunks->Chunks()[I].ui64Start + J + K,
									rThisRes.u.ui64Val, 0 ) ) {
									pustParms->i32Result = -1;
									return false;
								}
							}
						}
						K += pustParms->pspParms->ui32Alignment;
					}

					J += sSize;
				}
				else {
					break;
				}
			}

			if ( !((*pustParms->plProgress) & 0x7) && pustParms->hProgressUpdate ) {
				::SendNotifyMessageW( pustParms->hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>((((*pustParms->plProgress) + 1.0) / pustParms->paclChunks->Chunks().size()) * 100.0), 0L );
			}
			::InterlockedIncrement( pustParms->plProgress );
			if ( pustParms->pspParms->pbAbort && (*pustParms->pspParms->pbAbort) ) { break; }
		}

		pustParms->i32Result = 1;
		return true;
	}

	// Scans multiple parts of the current results at the same time.
	DWORD WINAPI CSearcher::PrimitiveExactValueSubsearchThreadProc( LPVOID _lpParameter ) {
		MX_PRIM_SEARCH_THREAD * ppstParms = static_cast<MX_PRIM_SEARCH_THREAD *>(_lpParameter);
		CSearchResultLocker srlSearchLock( ppstParms->psrbResults );
		lsw::LSW_THREAD_PRIORITY tpThreadPri( ppstParms->pspParms->iThreadPriority );

		CRamBuffer rbBuffer( ppstParms->ppProcess, ppstParms->paclChunks, ppstParms->pspParms->ui64MaxChunkLen );
		rbBuffer.SetPreproc( RamProcPreprocessByteSwap, reinterpret_cast<uint64_t>(ppstParms->pspParms) );

		size_t stResultIdx = 0;
		uint64_t ui64ResultAccumulator = 0;
		for ( size_t I = ppstParms->stStartIdx; I < ppstParms->stEndIdx; ++I ) {
			for ( size_t J = 0; J < (*ppstParms->pvSubsearchDiv)[I].ui64Total; ++J ) {
				uint64_t ui64Addr;
				if ( !ppstParms->psrbPrevResults->GetResultFast_Locked( static_cast<size_t>(J + (*ppstParms->pvSubsearchDiv)[I].ui64Start), ui64Addr, stResultIdx, ui64ResultAccumulator ) ) {
					ppstParms->i32Result = -1;
					return false;
				}
				CUtilities::MX_DATA_TYPE dtVal;
				if ( rbBuffer.Read( ui64Addr, &dtVal, ppstParms->ui32Size ) ) {
					if ( ppstParms->evcCompFunc( ppstParms->pspParms->dtLVal, dtVal, (*ppstParms->pspParms) ) ^ ppstParms->pspParms->bInvertResults ) {
						if ( !ppstParms->psrbResults->Add( ui64Addr,
							reinterpret_cast<uint8_t *>(&dtVal), ppstParms->ui32Size ) ) {
							ppstParms->i32Result = -1;
							return false;
						}
					}
				}
			}
			if ( !((*ppstParms->plProgress) & 0x1) && ppstParms->hProgressUpdate ) {
				::SendNotifyMessageW( ppstParms->hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>((((*ppstParms->plProgress) + 1.0) / (ppstParms->pvSubsearchDiv->size())) * 100.0), 0L );
			}
			::InterlockedIncrement( ppstParms->plProgress );
			if ( ppstParms->pspParms->pbAbort && (*ppstParms->pspParms->pbAbort) ) { break; }
		}

		ppstParms->i32Result = 1;
		return true;
	}

	// Scans multiple parts of the current results at the same time.
	DWORD WINAPI CSearcher::PrimitiveNotEqualToSubsearchThreadProc( LPVOID _lpParameter ) {
		MX_PRIM_SEARCH_THREAD * ppstParms = static_cast<MX_PRIM_SEARCH_THREAD *>(_lpParameter);
		CSearchResultLocker srlSearchLock( ppstParms->psrbResults );
		lsw::LSW_THREAD_PRIORITY tpThreadPri( ppstParms->pspParms->iThreadPriority );

		CRamBuffer rbBuffer( ppstParms->ppProcess, ppstParms->paclChunks, ppstParms->pspParms->ui64MaxChunkLen );
		rbBuffer.SetPreproc( RamProcPreprocessByteSwap, reinterpret_cast<uint64_t>(ppstParms->pspParms) );

		size_t stResultIdx = 0;
		uint64_t ui64ResultAccumulator = 0;
		for ( size_t I = ppstParms->stStartIdx; I < ppstParms->stEndIdx; ++I ) {
			for ( size_t J = 0; J < (*ppstParms->pvSubsearchDiv)[I].ui64Total; ++J ) {
				uint64_t ui64Addr;
				if ( !ppstParms->psrbPrevResults->GetResultFast_Locked( static_cast<size_t>(J + (*ppstParms->pvSubsearchDiv)[I].ui64Start), ui64Addr, stResultIdx, ui64ResultAccumulator ) ) {
					ppstParms->i32Result = -1;
					return false;
				}
				CUtilities::MX_DATA_TYPE dtVal;
				if ( rbBuffer.Read( ui64Addr, &dtVal, ppstParms->ui32Size ) ) {
					// ppstParms->pspParms->bInvertResults was swapped so we don't have to use !ppstParms->evcCompFunc().  Swap our logic here to match.
					if ( (ppstParms->evcCompFunc( ppstParms->pspParms->dtLVal, dtVal, (*ppstParms->pspParms) )) ^ ppstParms->pspParms->bInvertResults ) {
						if ( !ppstParms->psrbResults->Add( ui64Addr,
							reinterpret_cast<uint8_t *>(&dtVal), ppstParms->ui32Size ) ) {
							ppstParms->i32Result = -1;
							return false;
						}
					}
				}
			}
			if ( !((*ppstParms->plProgress) & 0x1) && ppstParms->hProgressUpdate ) {
				::SendNotifyMessageW( ppstParms->hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>((((*ppstParms->plProgress) + 1.0) / (ppstParms->pvSubsearchDiv->size())) * 100.0), 0L );
			}
			::InterlockedIncrement( ppstParms->plProgress );
			if ( ppstParms->pspParms->pbAbort && (*ppstParms->pspParms->pbAbort) ) { break; }
		}

		ppstParms->i32Result = 1;
		return true;
	}

	// Scans multiple parts of the current results at the same time.
	DWORD WINAPI CSearcher::PrimitiveIncreasedSubsearchThreadProc( LPVOID _lpParameter ) {
		MX_PRIM_SEARCH_THREAD * ppstParms = static_cast<MX_PRIM_SEARCH_THREAD *>(_lpParameter);
		CSearchResultLocker srlSearchLock( ppstParms->psrbResults );
		lsw::LSW_THREAD_PRIORITY tpThreadPri( ppstParms->pspParms->iThreadPriority );

		CRamBuffer rbBuffer( ppstParms->ppProcess, ppstParms->paclChunks, ppstParms->pspParms->ui64MaxChunkLen );
		rbBuffer.SetPreproc( RamProcPreprocessByteSwap, reinterpret_cast<uint64_t>(ppstParms->pspParms) );

		size_t stResultIdx = 0;
		uint64_t ui64ResultAccumulator = 0;
		for ( size_t I = ppstParms->stStartIdx; I < ppstParms->stEndIdx; ++I ) {
			for ( size_t J = 0; J < (*ppstParms->pvSubsearchDiv)[I].ui64Total; ++J ) {
				uint64_t ui64Addr;
				const uint8_t * pui8Val;
				if ( !ppstParms->psrbPrevResults->GetResultFast_Locked( static_cast<size_t>(J + (*ppstParms->pvSubsearchDiv)[I].ui64Start), ui64Addr, pui8Val, stResultIdx, ui64ResultAccumulator ) ) {
					ppstParms->i32Result = -1;
					return false;
				}
				CUtilities::MX_DATA_TYPE dtVal;
				if ( rbBuffer.Read( ui64Addr, &dtVal, ppstParms->ui32Size ) ) {
					if ( (ppstParms->rcRelCompFunc( (*reinterpret_cast<const CUtilities::MX_DATA_TYPE *>(pui8Val)), dtVal )) ^ ppstParms->pspParms->bInvertResults ) {
						if ( !ppstParms->psrbResults->Add( ui64Addr,
							reinterpret_cast<uint8_t *>(&dtVal), ppstParms->ui32Size ) ) {
							ppstParms->i32Result = -1;
							return false;
						}
					}
				}
			}
			if ( !((*ppstParms->plProgress) & 0x1) && ppstParms->hProgressUpdate ) {
				::SendNotifyMessageW( ppstParms->hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>((((*ppstParms->plProgress) + 1.0) / (ppstParms->pvSubsearchDiv->size())) * 100.0), 0L );
			}
			::InterlockedIncrement( ppstParms->plProgress );
			if ( ppstParms->pspParms->pbAbort && (*ppstParms->pspParms->pbAbort) ) { break; }
		}

		ppstParms->i32Result = 1;
		return true;
	}

	// Scans multiple parts of the current results at the same time.
	DWORD WINAPI CSearcher::PrimitiveDecreasedSubsearchThreadProc( LPVOID _lpParameter ) {
		MX_PRIM_SEARCH_THREAD * ppstParms = static_cast<MX_PRIM_SEARCH_THREAD *>(_lpParameter);
		CSearchResultLocker srlSearchLock( ppstParms->psrbResults );
		lsw::LSW_THREAD_PRIORITY tpThreadPri( ppstParms->pspParms->iThreadPriority );

		CRamBuffer rbBuffer( ppstParms->ppProcess, ppstParms->paclChunks, ppstParms->pspParms->ui64MaxChunkLen );
		rbBuffer.SetPreproc( RamProcPreprocessByteSwap, reinterpret_cast<uint64_t>(ppstParms->pspParms) );

		size_t stResultIdx = 0;
		uint64_t ui64ResultAccumulator = 0;
		for ( size_t I = ppstParms->stStartIdx; I < ppstParms->stEndIdx; ++I ) {
			for ( size_t J = 0; J < (*ppstParms->pvSubsearchDiv)[I].ui64Total; ++J ) {
				uint64_t ui64Addr;
				const uint8_t * pui8Val;
				if ( !ppstParms->psrbPrevResults->GetResultFast_Locked( static_cast<size_t>(J + (*ppstParms->pvSubsearchDiv)[I].ui64Start), ui64Addr, pui8Val, stResultIdx, ui64ResultAccumulator ) ) {
					ppstParms->i32Result = -1;
					return false;
				}
				CUtilities::MX_DATA_TYPE dtVal;
				if ( rbBuffer.Read( ui64Addr, &dtVal, ppstParms->ui32Size ) ) {
					if ( (ppstParms->rcRelCompFunc( (*reinterpret_cast<const CUtilities::MX_DATA_TYPE *>(pui8Val)), dtVal )) ^ ppstParms->pspParms->bInvertResults ) {
						if ( !ppstParms->psrbResults->Add( ui64Addr,
							reinterpret_cast<uint8_t *>(&dtVal), ppstParms->ui32Size ) ) {
							ppstParms->i32Result = -1;
							return false;
						}
					}
				}
			}
			if ( !((*ppstParms->plProgress) & 0x1) && ppstParms->hProgressUpdate ) {
				::SendNotifyMessageW( ppstParms->hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>((((*ppstParms->plProgress) + 1.0) / (ppstParms->pvSubsearchDiv->size())) * 100.0), 0L );
			}
			::InterlockedIncrement( ppstParms->plProgress );
			if ( ppstParms->pspParms->pbAbort && (*ppstParms->pspParms->pbAbort) ) { break; }
		}

		ppstParms->i32Result = 1;
		return true;
	}

	// Scans multiple parts of the current results at the same time.
	DWORD WINAPI CSearcher::PrimitiveInRangeSubsearchThreadProc( LPVOID _lpParameter ) {
		MX_PRIM_SEARCH_THREAD * ppstParms = static_cast<MX_PRIM_SEARCH_THREAD *>(_lpParameter);
		CSearchResultLocker srlSearchLock( ppstParms->psrbResults );
		lsw::LSW_THREAD_PRIORITY tpThreadPri( ppstParms->pspParms->iThreadPriority );

		CRamBuffer rbBuffer( ppstParms->ppProcess, ppstParms->paclChunks, ppstParms->pspParms->ui64MaxChunkLen );
		rbBuffer.SetPreproc( RamProcPreprocessByteSwap, reinterpret_cast<uint64_t>(ppstParms->pspParms) );

		size_t stResultIdx = 0;
		uint64_t ui64ResultAccumulator = 0;
		for ( size_t I = ppstParms->stStartIdx; I < ppstParms->stEndIdx; ++I ) {
			for ( size_t J = 0; J < (*ppstParms->pvSubsearchDiv)[I].ui64Total; ++J ) {
				uint64_t ui64Addr;
				if ( !ppstParms->psrbPrevResults->GetResultFast_Locked( static_cast<size_t>(J + (*ppstParms->pvSubsearchDiv)[I].ui64Start), ui64Addr, stResultIdx, ui64ResultAccumulator ) ) {
					ppstParms->i32Result = -1;
					return false;
				}
				CUtilities::MX_DATA_TYPE dtVal;
				if ( rbBuffer.Read( ui64Addr, &dtVal, ppstParms->ui32Size ) ) {
					if ( (ppstParms->rcRangeCompFunc( ppstParms->pspParms->dtLVal, ppstParms->pspParms->dtRVal, dtVal )) ^ ppstParms->pspParms->bInvertResults ) {
						if ( !ppstParms->psrbResults->Add( ui64Addr,
							reinterpret_cast<uint8_t *>(&dtVal), ppstParms->ui32Size ) ) {
							ppstParms->i32Result = -1;
							return false;
						}
					}
				}
			}
			if ( !((*ppstParms->plProgress) & 0x1) && ppstParms->hProgressUpdate ) {
				::SendNotifyMessageW( ppstParms->hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>((((*ppstParms->plProgress) + 1.0) / (ppstParms->pvSubsearchDiv->size())) * 100.0), 0L );
			}
			::InterlockedIncrement( ppstParms->plProgress );
			if ( ppstParms->pspParms->pbAbort && (*ppstParms->pspParms->pbAbort) ) { break; }
		}

		ppstParms->i32Result = 1;
		return true;
	}

	// Scans multiple parts of the current results at the same time.
	DWORD WINAPI CSearcher::PrimitiveSameAsBeforeSubsearchThreadProc( LPVOID _lpParameter ) {
		MX_PRIM_SEARCH_THREAD * ppstParms = static_cast<MX_PRIM_SEARCH_THREAD *>(_lpParameter);
		CSearchResultLocker srlSearchLock( ppstParms->psrbResults );
		lsw::LSW_THREAD_PRIORITY tpThreadPri( ppstParms->pspParms->iThreadPriority );

		CRamBuffer rbBuffer( ppstParms->ppProcess, ppstParms->paclChunks, ppstParms->pspParms->ui64MaxChunkLen );
		rbBuffer.SetPreproc( RamProcPreprocessByteSwap, reinterpret_cast<uint64_t>(ppstParms->pspParms) );

		size_t stResultIdx = 0;
		uint64_t ui64ResultAccumulator = 0;
		for ( size_t I = ppstParms->stStartIdx; I < ppstParms->stEndIdx; ++I ) {
			for ( size_t J = 0; J < (*ppstParms->pvSubsearchDiv)[I].ui64Total; ++J ) {
				uint64_t ui64Addr;
				const uint8_t * pui8Val;
				if ( !ppstParms->psrbPrevResults->GetResultFast_Locked( static_cast<size_t>(J + (*ppstParms->pvSubsearchDiv)[I].ui64Start), ui64Addr, pui8Val, stResultIdx, ui64ResultAccumulator ) ) {
					ppstParms->i32Result = -1;
					return false;
				}
				CUtilities::MX_DATA_TYPE dtVal;
				if ( rbBuffer.Read( ui64Addr, &dtVal, ppstParms->ui32Size ) ) {
					if ( ppstParms->evcCompFunc( (*reinterpret_cast<const CUtilities::MX_DATA_TYPE *>(pui8Val)), dtVal, (*ppstParms->pspParms) ) ^ ppstParms->pspParms->bInvertResults ) {
						if ( !ppstParms->psrbResults->Add( ui64Addr,
							reinterpret_cast<uint8_t *>(&dtVal), ppstParms->ui32Size ) ) {
							ppstParms->i32Result = -1;
							return false;
						}
					}
				}
			}
			if ( !((*ppstParms->plProgress) & 0x1) && ppstParms->hProgressUpdate ) {
				::SendNotifyMessageW( ppstParms->hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>((((*ppstParms->plProgress) + 1.0) / (ppstParms->pvSubsearchDiv->size())) * 100.0), 0L );
			}
			::InterlockedIncrement( ppstParms->plProgress );
			if ( ppstParms->pspParms->pbAbort && (*ppstParms->pspParms->pbAbort) ) { break; }
		}

		ppstParms->i32Result = 1;
		return true;
	}

	// Scans multiple parts of the current results at the same time.
	DWORD WINAPI CSearcher::PrimitiveChangedBySubsearchThreadProc( LPVOID _lpParameter ) {
		MX_PRIM_SEARCH_THREAD * ppstParms = static_cast<MX_PRIM_SEARCH_THREAD *>(_lpParameter);
		CSearchResultLocker srlSearchLock( ppstParms->psrbResults );
		lsw::LSW_THREAD_PRIORITY tpThreadPri( ppstParms->pspParms->iThreadPriority );

		CRamBuffer rbBuffer( ppstParms->ppProcess, ppstParms->paclChunks, ppstParms->pspParms->ui64MaxChunkLen );
		rbBuffer.SetPreproc( RamProcPreprocessByteSwap, reinterpret_cast<uint64_t>(ppstParms->pspParms) );

		size_t stResultIdx = 0;
		uint64_t ui64ResultAccumulator = 0;
		for ( size_t I = ppstParms->stStartIdx; I < ppstParms->stEndIdx; ++I ) {
			for ( size_t J = 0; J < (*ppstParms->pvSubsearchDiv)[I].ui64Total; ++J ) {
				uint64_t ui64Addr;
				const uint8_t * pui8Val;
				if ( !ppstParms->psrbPrevResults->GetResultFast_Locked( static_cast<size_t>(J + (*ppstParms->pvSubsearchDiv)[I].ui64Start), ui64Addr, pui8Val, stResultIdx, ui64ResultAccumulator ) ) {
					ppstParms->i32Result = -1;
					return false;
				}
				CUtilities::MX_DATA_TYPE dtVal;
				if ( rbBuffer.Read( ui64Addr, &dtVal, ppstParms->ui32Size ) ) {
					if ( ppstParms->cbChangedByCompFunc( (*reinterpret_cast<const CUtilities::MX_DATA_TYPE *>(pui8Val)), dtVal, ppstParms->pspParms->dtLVal, (*ppstParms->pspParms) ) ^ ppstParms->pspParms->bInvertResults ) {
						if ( !ppstParms->psrbResults->Add( ui64Addr,
							reinterpret_cast<uint8_t *>(&dtVal), ppstParms->ui32Size ) ) {
							ppstParms->i32Result = -1;
							return false;
						}
					}
				}
			}
			if ( !((*ppstParms->plProgress) & 0x1) && ppstParms->hProgressUpdate ) {
				::SendNotifyMessageW( ppstParms->hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>((((*ppstParms->plProgress) + 1.0) / (ppstParms->pvSubsearchDiv->size())) * 100.0), 0L );
			}
			::InterlockedIncrement( ppstParms->plProgress );
			if ( ppstParms->pspParms->pbAbort && (*ppstParms->pspParms->pbAbort) ) { break; }
		}

		ppstParms->i32Result = 1;
		return true;
	}

	// Scans multiple parts of the current results at the same time.
	template <typename _tType, unsigned _sIsFloat16>
	DWORD WINAPI CSearcher::PrimitiveQuickExpSubsearchThreadProc( LPVOID _lpParameter ) {
		MX_PRIM_SEARCH_THREAD * ppstParms = static_cast<MX_PRIM_SEARCH_THREAD *>(_lpParameter);
		CSearchResultLocker srlSearchLock( ppstParms->psrbResults );
		lsw::LSW_THREAD_PRIORITY tpThreadPri( ppstParms->pspParms->iThreadPriority );

		CRamBuffer rbBuffer( ppstParms->ppProcess, ppstParms->paclChunks, ppstParms->pspParms->ui64MaxChunkLen );
		rbBuffer.SetPreproc( RamProcPreprocessByteSwap, reinterpret_cast<uint64_t>(ppstParms->pspParms) );


		// =================
		// == Expression. ==
		// =================
		CExpression eExp;
		if ( !eExp.SetExpression( ppstParms->pspParms->ssExpression.c_str() ) ) { ppstParms->i32Result = -1; return false; }
		if ( !eExp.GetContainer() ) { ppstParms->i32Result = -1; return false; }
		// Since the address is not given to the script, we can't make meaningful predictions about what it will try to access.
		// Don't try to optimize it; just let it read what it reads.
		eExp.GetContainer()->SetAddressHandler( ExpAddressHandler, reinterpret_cast<uintptr_t>(ppstParms->ppProcess) );

		// UserVarHandler just copies whatever the uintptr_t points to, so just point it to rRes and then just update rRes during the search.
		ee::CExpEvalContainer::EE_RESULT rRes;
		eExp.GetContainer()->SetUserHandler( UserVarHandler, reinterpret_cast<uintptr_t>(&rRes) );
		bool bIsFloat = false;
		bool bIsSigned = false;
		if ( CUtilities::DataTypeIsFloat( ppstParms->pspParms->dtLVal.dtType ) ) { rRes.ncType = ee::EE_NC_FLOATING; bIsFloat = true; }
		else if ( CUtilities::DataTypeIsSigned( ppstParms->pspParms->dtLVal.dtType ) ) { rRes.ncType = ee::EE_NC_SIGNED; bIsSigned = true; }
		else { rRes.ncType = ee::EE_NC_UNSIGNED; }
		// =================
		// =================

		size_t stResultIdx = 0;
		uint64_t ui64ResultAccumulator = 0;
		ee::CExpEvalContainer::EE_RESULT rNumberedParm;
		if ( bIsFloat ) {
			rNumberedParm.ncType = ee::EE_NC_FLOATING;
		}
		else if ( bIsSigned ) {
			rNumberedParm.ncType = ee::EE_NC_SIGNED;
		}
		else {
			rNumberedParm.ncType = ee::EE_NC_UNSIGNED;
		}
		for ( size_t I = ppstParms->stStartIdx; I < ppstParms->stEndIdx; ++I ) {
			for ( size_t J = 0; J < (*ppstParms->pvSubsearchDiv)[I].ui64Total; ++J ) {
				uint64_t ui64Addr;
				const uint8_t * pui8Val;
				if ( !ppstParms->psrbPrevResults->GetResultFast_Locked( static_cast<size_t>(J + (*ppstParms->pvSubsearchDiv)[I].ui64Start), ui64Addr, pui8Val, stResultIdx, ui64ResultAccumulator ) ) {
					ppstParms->i32Result = -1;
					return false;
				}
				CUtilities::MX_DATA_TYPE dtVal;
				dtVal.u.UInt64 = 0;
				if ( rbBuffer.Read( ui64Addr, &dtVal, ppstParms->ui32Size ) ) {
					if ( _sIsFloat16 ) {
						rRes.u.dVal = CFloat16( dtVal.u.UInt16 ).Value();
						rNumberedParm.u.dVal = CFloat16( (*reinterpret_cast<const uint16_t *>(pui8Val)) ).Value();
					}
					else {
						if ( bIsFloat ) {
							rRes.u.dVal = (*reinterpret_cast<_tType *>(&dtVal.u.Float64));
							rNumberedParm.u.dVal = (*reinterpret_cast<const _tType *>(pui8Val));
						}
						else if ( bIsSigned ) {
							rRes.u.i64Val = (*reinterpret_cast<_tType *>(&dtVal.u.Int64));
							rNumberedParm.u.i64Val = (*reinterpret_cast<const _tType *>(pui8Val));
						}
						else {
							rRes.u.ui64Val = (*reinterpret_cast<_tType *>(&dtVal.u.UInt64));
							rNumberedParm.u.ui64Val = (*reinterpret_cast<const _tType *>(pui8Val));
						}
					}
					ee::CExpEvalContainer::EE_RESULT rThisRes;
					eExp.GetContainer()->SetNumberedParm( 0, rNumberedParm );
					if ( eExp.GetContainer()->Resolve( rThisRes ) ) {
						rThisRes = ee::CExpEvalContainer::ConvertResult( rThisRes, ee::EE_NC_UNSIGNED );
						if ( (rThisRes.u.ui64Val && !ppstParms->pspParms->bInvertResults) ||
							!rThisRes.u.ui64Val && ppstParms->pspParms->bInvertResults) {
							/*if ( dtVal.u.UInt64 != 0 ) {
								volatile int oiuisdu = 0;
							}*/
							if ( !ppstParms->psrbResults->Add( ui64Addr,
								reinterpret_cast<uint8_t *>(&dtVal), ppstParms->ui32Size ) ) {
								ppstParms->i32Result = -1;
								return false;
							}
						}
					}
				}
			}
			if ( !((*ppstParms->plProgress) & 0x1) && ppstParms->hProgressUpdate ) {
				::SendNotifyMessageW( ppstParms->hProgressUpdate, PBM_SETPOS, static_cast<WPARAM>((((*ppstParms->plProgress) + 1.0) / (ppstParms->pvSubsearchDiv->size())) * 100.0), 0L );
			}
			::InterlockedIncrement( ppstParms->plProgress );
			if ( ppstParms->pspParms->pbAbort && (*ppstParms->pspParms->pbAbort) ) { break; }
		}

		ppstParms->i32Result = 1;
		return true;
	}

	// Compares 2 strings that have been composed, having toupper() and similar conversions already done if necessary, and the final characters to compare are all stored in uint32_t values.
	bool __stdcall CSearcher::Cmp_ComposedStringCompare( const MX_STRING_SEARCH_UNPACKED &_ssuLeft,
		const MX_STRING_SEARCH_UNPACKED &_ssuRight,
		size_t _sLen,
		size_t _sIdx,
		uint32_t _ui32Flags, uint32_t _ui32FlagsEx,
		size_t &_sResultLen,
		uint32_t _uiMaxWildcardLen ) {
#define MX_ADVANCE( IDX, ARRAY, LEN )		IDX = IDX < LEN ? (IDX + ARRAY[IDX].ui8UftLen) : LEN
#define MX_ADVANCE_I						MX_ADVANCE( I, _ssuLeft.vDataBuffer, _ssuLeft.vDataBuffer.size() );
#define MX_ADVANCE_J						sLastChar = J; MX_ADVANCE( J, _ssuRight.vDataBuffer, _sLen );
		_sResultLen = 0;
		// Assume it is a match until proven otherwise.
		size_t J = _sIdx;
		size_t sLastChar = J;
		_sLen = std::min( _sLen, _ssuRight.vDataBuffer.size() );
		bool bFoundAtLeastOne = false;
		
		for ( size_t I = 0; I < _ssuLeft.vDataBuffer.size(); ) {
			// If we are ignoring non-spacing characters.
			if ( _ui32Flags & CUtilities::MX_SSF_NORM_IGNORENONSPACE && bFoundAtLeastOne ) {
				while ( I < _ssuLeft.vDataBuffer.size() && (_ssuLeft.vBuffer3[I] & C3_NONSPACING) ) { MX_ADVANCE_I; }
				if ( I == _ssuLeft.vDataBuffer.size() ) { break; }
				while ( J < _sLen && (_ssuRight.vBuffer3[J] & C3_NONSPACING) ) { MX_ADVANCE_J; }
			}
			bFoundAtLeastOne = true;
			if ( _ui32FlagsEx & CUtilities::MX_SSF_WILDCARD ) {
				if ( _ssuLeft.vDataBuffer[I].ui32SearchChar == '?' ) { MX_ADVANCE_J; MX_ADVANCE_I; continue; }
				if ( _ssuLeft.vDataBuffer[I].ui32SearchChar == '*' ) {
					uint32_t ui32WildcardCount = _uiMaxWildcardLen;
					// Skip all * and ?.
					while ( I < _ssuLeft.vDataBuffer.size() && (_ssuLeft.vDataBuffer[I].ui32SearchChar == '*' || _ssuLeft.vDataBuffer[I].ui32SearchChar == '?') ) {
						if ( _ssuLeft.vDataBuffer[I].ui32SearchChar == '?' ) {
							MX_ADVANCE_J;
						}
						else if ( _ssuLeft.vDataBuffer[I].ui32SearchChar == '*' ) {
							ui32WildcardCount += _uiMaxWildcardLen;
						}
						MX_ADVANCE_I;
					}
					if ( I == _ssuLeft.vDataBuffer.size() ) {
						// * is the last character in the source string, so auto-true.
						// We could eat the entire second array, but just eat 1 character, or up to the next word boundary.
						if ( _ui32FlagsEx & CUtilities::MX_SSF_WHOLE_WORD ) {
							while ( J < _sLen && !String_WholeWordRight( _ssuRight, sLastChar, J ) ) { MX_ADVANCE_J; }
						}
						else {
							MX_ADVANCE_J;
						}
						break;
					}
					// Otherwise search for the next match in the source string.
					else {
						while ( J < _sLen ) {
							if ( _ssuRight.vDataBuffer[J].ui32SearchChar == _ssuLeft.vDataBuffer[I].ui32SearchChar ) { break; }
							MX_ADVANCE_J;
							if ( !--ui32WildcardCount ) { return false; }
						}
					}
				}
			}
			// Not a wildcard character.
			if ( J >= _sLen ) { return false; }
			if ( _ssuLeft.vDataBuffer[I].ui32SearchChar != _ssuRight.vDataBuffer[J].ui32SearchChar ) { return false; }
			MX_ADVANCE_I;
			MX_ADVANCE_J;
		}
		_sResultLen = J - _sIdx;
		if ( _ui32FlagsEx & CUtilities::MX_SSF_WHOLE_WORD ) {
			if ( !String_WholeWordRight( _ssuRight, sLastChar, J ) ) { return false; }
		}
#undef MX_ADVANCE_I
#undef MX_ADVANCE_I
#undef MX_ADVANCE
		return true;
	}

	// Compares 2 arrays of primitives.
	template <typename _tType>
	bool __stdcall CSearcher::Cmp_DataArrayCompare( const void * _pvLeft, size_t _sLeftSizeInCharacters,
		const std::vector<uint8_t> &_vMeta,
		const void * _pvRight, size_t _sRightSizeInBytes,
		size_t &_sResultLen,
		uint32_t _uiMaxWildcardLen ) {
		const _tType * ptLeft = reinterpret_cast<const _tType *>(_pvLeft);
		const _tType * ptRight = reinterpret_cast<const _tType *>(_pvRight);
		_sRightSizeInBytes /= sizeof( _tType );
		_sResultLen = 0;
		size_t J = 0;
		for ( size_t I = 0; I < _sLeftSizeInCharacters; ) {
			if ( _vMeta[I] == '?' ) {
				if ( J == _sRightSizeInBytes ) { return false; }	// Has to be at least 1 character to match the wildcard ?.
				++J;
				++I;
				continue;
			}
			if ( _vMeta[I] == '*' ) {
				uint32_t ui32WildcardCount = _uiMaxWildcardLen;
				while ( I < _sLeftSizeInCharacters ) {
					if ( _vMeta[I] == '?' ) {
						if ( J == _sRightSizeInBytes ) { return false; }	// Has to be at least 1 character to match the wildcard ?.
						++J;
						++I;
						continue;
					}
					if ( _vMeta[I] == '*' ) {
						ui32WildcardCount += _uiMaxWildcardLen;
						++I;
						continue;
					}
					break;
				}


				if ( I == _sLeftSizeInCharacters ) {
					// * is the last character in the source string, so auto-true.
					// We could eat the entire second array, but just eat 1 character, or up to the next word boundary.
					++J;
					break;
				}
				// Otherwise search for the next match in the source string.
				else {
					while ( J < _sRightSizeInBytes ) {
						if ( ptRight[J] == ptLeft[I] ) { break; }
						++J;
						if ( !--ui32WildcardCount ) { return false; }
					}
				}


			}

			// Not a wildcard character.
			if ( J >= _sRightSizeInBytes ) { return false; }
			if ( ptLeft[I] != ptRight[J] ) { return false; }
			++J;
			++I;
		}
		_sResultLen = J * sizeof( _tType );
		return true;
	}

	// Compares 2 arrays of primitives of various types.
	bool __stdcall CSearcher::Cmp_DataArrayCompare_MultiType( const void * _pvLeft,
		const std::vector<uint8_t> &_vMeta,
		const void * _pvRight, size_t _sRightSizeInBytes,
		size_t &_sResultLen,
		uint32_t _uiMaxWildcardLen ) {
		size_t J = 0;
		size_t sIdx = 0;
		const uint8_t * puiLeft = reinterpret_cast<const uint8_t *>(_pvLeft);
		const uint8_t * puiRight = reinterpret_cast<const uint8_t *>(_pvRight);

		for ( size_t I = 0; I < _vMeta.size(); ++I ) {
			CUtilities::MX_DATA_TYPES dtType = static_cast<CUtilities::MX_DATA_TYPES>(_vMeta[I] & CUtilities::MX_DT_MASK);
			size_t sSize = CUtilities::DataTypeSize( dtType );
			switch ( _vMeta[I] & CUtilities::MX_WF_MASK ) {
				case CUtilities::MX_WF_ANY_CHAR : {
					J += sSize;
					if ( J > _sRightSizeInBytes ) { return false; }
					break;
				}
				case CUtilities::MX_WF_ANY : {
					uint32_t ui32WildcardCount = _uiMaxWildcardLen;
					size_t sLookAhead = 0;
					size_t sThisSize = sSize;
					while ( I < _vMeta.size() ) {
						uint8_t ui8Key = _vMeta[I] & CUtilities::MX_WF_MASK;
						//dtType = static_cast<CUtilities::MX_DATA_TYPES>(_vMeta[I] & CUtilities::MX_DT_MASK);
						//sSize = CUtilities::DataTypeSize( dtType );
						if ( ui8Key == CUtilities::MX_WF_ANY_CHAR ) {
							sLookAhead += CUtilities::DataTypeSize( static_cast<CUtilities::MX_DATA_TYPES>(_vMeta[I] & CUtilities::MX_DT_MASK) );
							continue;
						}
						else if ( ui8Key == CUtilities::MX_WF_ANY ) {
							ui32WildcardCount += _uiMaxWildcardLen;
							++I;
							continue;
						}
						else { break; }
					}

					if ( I == _vMeta.size() ) {
						// * is the last character in the source string, so auto-true.
						// We could eat the entire second array, but just eat 1 character, or up to the next word boundary.
						J += sSize;
						break;
					}
					// Otherwise search for the next match in the source string.
					else {
						dtType = static_cast<CUtilities::MX_DATA_TYPES>(_vMeta[I] & CUtilities::MX_DT_MASK);
						sSize = CUtilities::DataTypeSize( dtType );
						// Now we have ui32WildcardCount items to scan such that for a given item index IDX, if IDX+sLookAhead == _vMeta[I], then we matched this
						//	field and can move to the next.
						bool bFound = false;
						for ( uint32_t K = 0; K < ui32WildcardCount && !bFound; ++K ) {
							size_t sOffset = K * sThisSize + sLookAhead + J;
							if ( sOffset + sSize > _sRightSizeInBytes ) { return false; }
							switch ( dtType ) {
								case CUtilities::MX_DT_INT8 : {}
								case CUtilities::MX_DT_UINT8 : {
									if ( puiLeft[sIdx] == puiRight[sOffset] ) {
										bFound = true;
										J = sOffset + sSize;
										sIdx += sSize;
									}
									break;
								}
								case CUtilities::MX_DT_FLOAT16 : {}
								case CUtilities::MX_DT_INT16 : {}
								case CUtilities::MX_DT_UINT16 : {
									if ( (*reinterpret_cast<const uint16_t *>(&puiLeft[sIdx])) == (*reinterpret_cast<const uint16_t *>(&puiRight[sOffset])) ) {
										bFound = true;
										J = sOffset + sSize;
										sIdx += sSize;
									}
									break;
								}
								case CUtilities::MX_DT_INT32 : {}
								case CUtilities::MX_DT_UINT32 : {
									if ( (*reinterpret_cast<const uint32_t *>(&puiLeft[sIdx])) == (*reinterpret_cast<const uint32_t *>(&puiRight[sOffset])) ) {
										bFound = true;
										J = sOffset + sSize;
										sIdx += sSize;
									}
									break;
								}
								case CUtilities::MX_DT_INT64 : {}
								case CUtilities::MX_DT_UINT64 : {
									if ( (*reinterpret_cast<const uint64_t *>(&puiLeft[sIdx])) == (*reinterpret_cast<const uint64_t *>(&puiRight[sOffset])) ) {
										bFound = true;
										J = sOffset + sSize;
										sIdx += sSize;
									}
									break;
								}
								case CUtilities::MX_DT_FLOAT : {
									if ( (*reinterpret_cast<const float *>(&puiLeft[sIdx])) == (*reinterpret_cast<const float *>(&puiRight[sOffset])) ) {
										bFound = true;
										J = sOffset + sSize;
										sIdx += sSize;
									}
									break;
								}
								case CUtilities::MX_DT_DOUBLE : {
									if ( (*reinterpret_cast<const double *>(&puiLeft[sIdx])) == (*reinterpret_cast<const double *>(&puiRight[sOffset])) ) {
										bFound = true;
										J = sOffset + sSize;
										sIdx += sSize;
									}
									break;
								}
							}
						}
						if ( !bFound ) { return false; }
					}
					break;
				}
				default : {
					// Not a wildcard character.
					if ( J + sSize > _sRightSizeInBytes ) { return false; }
					switch ( dtType ) {
						case CUtilities::MX_DT_INT8 : {}
						case CUtilities::MX_DT_UINT8 : {
							if ( puiLeft[sIdx] != puiRight[J] ) { return false; }
							break;
						}
						case CUtilities::MX_DT_FLOAT16 : {}
						case CUtilities::MX_DT_INT16 : {}
						case CUtilities::MX_DT_UINT16 : {
							if ( (*reinterpret_cast<const uint16_t *>(&puiLeft[sIdx])) != (*reinterpret_cast<const uint16_t *>(&puiRight[J])) ) { return false; }
							break;
						}
						case CUtilities::MX_DT_INT32 : {}
						case CUtilities::MX_DT_UINT32 : {
							if ( (*reinterpret_cast<const uint32_t *>(&puiLeft[sIdx])) != (*reinterpret_cast<const uint32_t *>(&puiRight[J])) ) { return false; }
							break;
						}
						case CUtilities::MX_DT_INT64 : {}
						case CUtilities::MX_DT_UINT64 : {
							if ( (*reinterpret_cast<const uint64_t *>(&puiLeft[sIdx])) != (*reinterpret_cast<const uint64_t *>(&puiRight[J])) ) { return false; }
							break;
						}
						case CUtilities::MX_DT_FLOAT : {
							if ( (*reinterpret_cast<const float *>(&puiLeft[sIdx])) != (*reinterpret_cast<const float *>(&puiRight[J])) ) { return false; }
							break;
						}
						case CUtilities::MX_DT_DOUBLE : {
							if ( (*reinterpret_cast<const double *>(&puiLeft[sIdx])) != (*reinterpret_cast<const double *>(&puiRight[J])) ) { return false; }
							break;
						}
					}
					sIdx += sSize;
					J += sSize;
				}
			}
		}

		_sResultLen = J;
		return true;
	}

	// Checks for their being a left whole-word boundary at a given index into an array.
	bool __stdcall CSearcher::String_WholeWordLeft( const MX_STRING_SEARCH_UNPACKED &_ssuRight, size_t _sIdx, uint32_t _ui32Align ) {
		if ( _sIdx < _ui32Align || _sIdx >= _ssuRight.vBuffer1.size() ) { return true; }	// Nothing to the left.
		// If the given character is not a word character, auto-pass.
		bool bIsWordChar = (_ssuRight.vBuffer1[_sIdx] & (C1_ALPHA | C1_DIGIT)) || _ssuRight.vDataBuffer[_sIdx].ui32SearchChar == '_';
		if ( !bIsWordChar ) { return true; }
		bIsWordChar = (_ssuRight.vBuffer1[_sIdx-_ui32Align] & (C1_ALPHA | C1_DIGIT)) || _ssuRight.vDataBuffer[_sIdx-_ui32Align].ui32SearchChar == '_';
		return !bIsWordChar;
	}

	// Checks for their being a left whole-word boundary at a given index into an array.
	bool __stdcall CSearcher::String_WholeWordRight( const MX_STRING_SEARCH_UNPACKED &_ssuRight, size_t _sIdx, size_t _sNextChar ) {
		if ( _sNextChar >= _ssuRight.vBuffer1.size() ) { return true; }	// Nothing to the right.
		// If the given character is not a word character, auto-pass.
		bool bIsWordChar = (_ssuRight.vBuffer1[_sIdx] & (C1_ALPHA | C1_DIGIT)) || _ssuRight.vDataBuffer[_sIdx].ui32SearchChar == '_';
		if ( !bIsWordChar ) { return true; }
		bIsWordChar = (_ssuRight.vBuffer1[_sNextChar] & (C1_ALPHA | C1_DIGIT)) || _ssuRight.vDataBuffer[_sNextChar].ui32SearchChar == '_';
		return !bIsWordChar;
	}

}	// namespace mx
