#pragma once
#include "../MXMhsX.h"
#include "../MemHack/MXProcess.h"
#include "../Utilities/MXUtilities.h"
#include <Float16/EEFloat16.h>
#include "MXAddressChunkList.h"
#include "MXSearchResultBase.h"

#include <cassert>
#include <filesystem>


namespace mx {

	class CSearcher {
	public :
		CSearcher( CProcess * _ppProcess ) :
			m_ppProcess( _ppProcess ),
			m_psrbSearchResults( nullptr ),
			m_psrbCurTmpSearchResults( nullptr ),
			m_i32LockCount( 0 ),
			m_bLastSearchWasSubsearch( false ),
			m_ui64LastSearchTotal( 0 ) {
			assert( m_ppProcess != nullptr );
		}
		~CSearcher() {
			m_ppProcess = nullptr;

			{
				LSW_ENT_CRIT( m_csCrit );
				delete m_psrbSearchResults;
				m_psrbSearchResults = nullptr;
			}

			delete m_psrbCurTmpSearchResults;
			m_psrbCurTmpSearchResults = nullptr;
		}


		// == Types.
		struct MX_SEARCH_CHUNK_OPTIONS {
			bool							bStaticOnly = false;
			DWORD							dwTypes = 0;
		};

		// Search parameters.
		struct MX_SEARCH_PARMS {
			uint64_t						ui64AddressFrom = 0;
			uint64_t						ui64AddressTo = 0;
			uint64_t						ui64MaxChunkLen = 0;
			DWORD							dwRegions = 0;

			// Data-type search.
			CUtilities::MX_DATA_TYPE		dtLVal;
			CUtilities::MX_DATA_TYPE		dtRVal;
			CUtilities::MX_SEARCH_TYPES		stType = CUtilities::MX_ST_DATATYPE_SEARCH;
			int32_t							i32EvalType = 0;

			// String search.
			std::vector<uint8_t>			vStringSearchValue;
			std::vector<uint8_t>			vStringMetaData;
			CUtilities::MX_WILDCARD_USAGE	wuWildcardUsage = CUtilities::MX_WU_NONE;
			CUtilities::MX_STRING_SEARCH_TYPES
											sstSearchType = CUtilities::MX_SST_ASCII;
			uint32_t						ui32StringSearchFlags = 0;
			uint32_t						ui32StringSearchFlagsEx = 0;
			uint32_t						ui32RegexFlags = 0;
			CUtilities::MX_DATA_TYPES		dtStringCharType = CUtilities::MX_DT_UINT32;
			CUtilities::MX_REGEX_FLAVORS	rfRegexFlavor = CUtilities::MX_RF_ONIGURUMA;
			CUtilities::MX_REGEX_ENCODING	reRegexEncoding = CUtilities::MX_RE_UTF8;

			// Expression search.
			CSecureString					ssExpression;
			CUtilities::MX_EXP_SEARCH_QQ_MEANING
											esqmExpSearchType = CUtilities::MX_ESQM_VALUE;
			uint32_t						ui32ExpSearchSize = 0;
			
			// Epsilon settings.
			double							dEpsilonValue = 0.0;
			bool							bUseEpsilon = true;
			bool							bUseSmartEpsilon = true;

			// Misc.
			bool							bAligned = true;
			bool							bInvertResults = false;
			int32_t							ui32Alignment = 0;
			CUtilities::MX_BYTESWAP			bsByteSwapping = CUtilities::MX_BS_NONE;

			// System settings & works.
			bool *							pbAbort = nullptr;
			int								iThreadPriority = 0;
			bool							bPause = false;
		};

		// Subsearch parameters.
		struct MX_SUBSEARCH_PARMS {
			uint64_t						ui64MaxChunkLen = 0;
			CUtilities::MX_SEARCH_TYPES		stInitialType = CUtilities::MX_ST_DATATYPE_SEARCH;			// Initial search type.
			int32_t							i32EvalType = 0;			// MX_SUB_EVAL_TYPES.
			CUtilities::MX_DATA_TYPE		dtLVal;
			CUtilities::MX_DATA_TYPE		dtRVal;

			CSecureString					ssExpression;

			// Epsilon settings.
			double							dEpsilonValue = 0.0;
			bool							bUseEpsilon = false;
			bool							bUseSmartEpsilon = true;

			// Misc.
			bool							bInvertResults = false;
			CUtilities::MX_BYTESWAP			bsByteSwapping = CUtilities::MX_BS_NONE;

			// System settings & works.
			bool *							pbAbort = nullptr;
			int								iThreadPriority = 0;
			bool							bPause = false;
		};

		// Safely locks the searcher.
		struct MX_SEARCH_LOCK {
			MX_SEARCH_LOCK( CSearcher * _psSearcher ) :
				psSearcher( _psSearcher ) {
				if ( psSearcher ) { psSearcher->Lock(); }
			}
			~MX_SEARCH_LOCK() {
				if ( psSearcher ) { psSearcher->Unlock(); }
			}


		protected :
			CSearcher *						psSearcher;
		};


		// == Functions.
		// List a list of chunks.
		bool								GenerateChunks( CAddressChunkList &_aclList, DWORD _dwRegionMask,
			uint64_t _ui64MaxLen, uint64_t _ui64Start = 0, uint64_t _ui64End = static_cast<uint64_t>(-16) );

		// Searches the target process.
		bool								Search( const MX_SEARCH_PARMS &_spParms, HWND _hProgressUpdate, uint64_t * _pui64TotalFound );

		// Performs a subsearch.
		bool								Subsearch( const MX_SUBSEARCH_PARMS &_spParms, HWND _hProgressUpdate, uint64_t * _pui64TotalFound );

		// Locks for reading a value quickly.  Unlock when done with the returned pointer.
		virtual void						Lock() const { m_csCrit.EnterCriticalSection(); ++m_i32LockCount; }

		// Gets the search results.  Should be called between Lock() and Unlock().
		const CSearchResultBase *			SearchResults() const { return m_i32LockCount ? m_psrbSearchResults : nullptr; }

		// Gets the total number of results.  Should be called between Lock() and Unlock().
		uint64_t							TotalResults() const { return SearchResults() ? SearchResults()->TotalResults() : 0; }

		// Gets the number of results between built-in Lock() and Unlock() calls.
		uint64_t							TotalResultsQuick() { MX_SEARCH_LOCK slLock( this ); return TotalResults(); }

		// Unlocks after done with the buffer returned by GetResultFast().
		virtual void						Unlock() const { --m_i32LockCount; assert( m_i32LockCount >= 0 ); m_csCrit.LeaveCriticalSection(); }

		// Gets the search time in microseconds.
		uint64_t							SearchTime() const { return m_ui64LastSearchTime; }

		// Gets the last search type.
		CUtilities::MX_SEARCH_TYPES			LastSearchType() const { return m_spLastSearchParms.stType; }

		// Gets the last search parameters.
		const MX_SEARCH_PARMS &				LastSearchParms() const { return m_spLastSearchParms; }

		// Gets the last subsearch parameters.
		const MX_SUBSEARCH_PARMS &			LastSubsearchParms() const { return m_spLastSubSearchParms; }

		// Gets the last search total.
		uint64_t							LastSearchTotalResults() const { return m_ui64LastSearchTotal; }

		// Determines if the last search was a subsearch or not.
		bool								LastSearchWasSubsearch() const { return m_bLastSearchWasSubsearch; }

		// Performs pre-processing byteswapping.
		static inline void					PreprocessByteSwap( void * _pvData, size_t _stLen, const MX_SEARCH_PARMS &_spParms ) { return PreprocessByteSwap( _pvData, _stLen, _spParms.bsByteSwapping ); }

		// Performs pre-processing byteswapping.
		static void							PreprocessByteSwap( void * _pvData, size_t _stLen, CUtilities::MX_BYTESWAP _bsSwap );

		// RAM buffer preprocessing function.
		static void							RamProcPreprocessByteSwap( void * _pvData, size_t _stLen, uint64_t _ui64Parm ) {
			const MX_SEARCH_PARMS * pspParms = reinterpret_cast<const MX_SEARCH_PARMS *>(_ui64Parm);
			PreprocessByteSwap( _pvData, _stLen, (*pspParms) );
		}

		/**
		 * Gets the search save directory for a given search index.
		 * 
		 * \param _ui32Idx The search index.
		 * \return Returns the path to the given search index.
		 **/
		static std::filesystem::path		SearchDir( uint32_t _ui32Idx ) {
			auto aPath = std::filesystem::path( CSystem::GetCurDirW() );
			aPath.append( std::to_wstring( _ui32Idx ) );
			return aPath;
		}

		/**
		 * Gets the path to a search file given the search index and sub-search index.
		 * 
		 * \param _ui32Idx The search index.
		 * \param _ui32SubIdx The sub-search index.
		 * \return Returns the path to the given search index.
		 **/
		static std::filesystem::path		SearchDir( uint32_t _ui32Idx, uint32_t _ui32SubIdx ) {
			auto aPath = SearchDir( _ui32Idx );
			aPath.append( std::to_wstring( _ui32Idx ) + L".mhss" );
			return aPath;
		}


	protected :
		// == Types.
		typedef bool (__stdcall *			MX_EXACT_VAL_COMP)( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms );
		typedef bool (__stdcall *			MX_REL_COMP)( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight );
		typedef bool (__stdcall *			MX_RANGE_COMP)( const CUtilities::MX_DATA_TYPE &_patLow, const CUtilities::MX_DATA_TYPE &_patHigh, const CUtilities::MX_DATA_TYPE &_patVal );
		typedef bool (__stdcall *			MX_CHANGED_BY)( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &_spParms );
		typedef bool (CSearcher::*			MX_CHANGED_BY_SELECTOR)( CSearcher::MX_CHANGED_BY &, uint32_t &, CSearcher::MX_SEARCH_PARMS &, bool );

		// String search template.
		struct MX_STRING_SEARCH_UNPACKED {
			std::vector<CUtilities::MX_CHAR_DATA>
											vDataBuffer;
			std::vector<WORD>				vBuffer1, vBuffer2, vBuffer3;
		};

		// UTF string search thread.
		struct MX_UTF_SEARCH_THREAD {
			CSearchResultBase *				psrbResults;
			MX_SEARCH_PARMS *				pspParms;
			MX_STRING_SEARCH_UNPACKED *		pssuUnpackedSrcString;
			const CAddressChunkList *		paclChunks;
			CProcess *						ppProcess;
			HWND							hProgressUpdate;
			size_t							stStartIdx;
			size_t							stEndIdx;	// Exclusive.
			uint32_t						ui32Size;
			volatile int32_t				i32Result;	// 0 = pending, 1 = success, -1 = failure.
			volatile LONG *					plProgress;
		};

		// Raw array search thread.
		struct MX_RAW_ARRAY_SEARCH_THREAD {
			CSearchResultBase *				psrbResults;
			MX_SEARCH_PARMS *				pspParms;
			const CAddressChunkList *		paclChunks;
			CProcess *						ppProcess;
			HWND							hProgressUpdate;
			size_t							stStartIdx;
			size_t							stEndIdx;	// Exclusive.
			uint32_t						ui32Size;
			volatile int32_t				i32Result;	// 0 = pending, 1 = success, -1 = failure.
			volatile LONG *					plProgress;
		};

		// Expression search thread.
		struct MX_EXP_SEARCH_THREAD {
			CSearchResultBase *				psrbResults;
			MX_SEARCH_PARMS *				pspParms;
			const CAddressChunkList *		paclChunks;
			CProcess *						ppProcess;
			HWND							hProgressUpdate;
			size_t							stStartIdx;
			size_t							stEndIdx;	// Exclusive.
			uint32_t						ui32Size;
			volatile int32_t				i32Result;	// 0 = pending, 1 = success, -1 = failure.
			volatile LONG *					plProgress;
		};

		// Precached reads for dynamic expression searches.
		struct MX_EXP_SEARCH_CACHE {
			CProcess *						ppProcess;	// The process object for reading any address.
			uint8_t *						pui8Base;	// The base data.
			size_t							sBaseSize;	// Cache size.
			uint64_t						ui64Address;
		};

		// Subsearch divisions.  Divides a subsearch into threadable sections.
		struct MX_SUBSEARCH_DIV {
			uint64_t						ui64Start;	// The result from which to start checking.
			uint64_t						ui64Total;	// The total number of results to check.
		};

		// Primitive-search thread.
		struct MX_PRIM_SEARCH_THREAD {
			CSearchResultBase *				psrbResults;
			CSearchResultBase *				psrbPrevResults;
			union {
				MX_SEARCH_PARMS *			pspParms;
				//MX_SUBSEARCH_PARMS *		pspSubParms;
			};
			const CAddressChunkList *		paclChunks;
			CProcess *						ppProcess;
			std::vector<MX_SUBSEARCH_DIV> *	pvSubsearchDiv;
			MX_EXACT_VAL_COMP				evcCompFunc;
			MX_REL_COMP						rcRelCompFunc;
			MX_RANGE_COMP					rcRangeCompFunc;
			MX_CHANGED_BY					cbChangedByCompFunc;
			HWND							hProgressUpdate;
			size_t							stStartIdx;
			size_t							stEndIdx;	// Exclusive.
			uint32_t						ui32Size;
			volatile int32_t				i32Result;	// 0 = pending, 1 = success, -1 = failure.
			volatile LONG *					plProgress;
		};


		// == Members.
		// The process.
		CProcess *							m_ppProcess;
		// The results.
		CSearchResultBase *					m_psrbSearchResults;
		// Results during a search (can be discarded if there are no search results).
		CSearchResultBase *					m_psrbCurTmpSearchResults;
		// Critical section.
		CCriticalSection					m_csCrit;
		// Lock count.
		mutable int32_t						m_i32LockCount;
		// Search time.
		uint64_t							m_ui64LastSearchTime;
		// Last search results.
		uint64_t							m_ui64LastSearchTotal;
		// Last search parameters.
		MX_SEARCH_PARMS						m_spLastSearchParms;
		// Last subsearch parameters.
		MX_SUBSEARCH_PARMS					m_spLastSubSearchParms;
		// Number of new searches made.
		uint32_t							m_ui32SearchCnt = 0;
		// Number of searches to retain on disk.
		uint32_t							m_ui32SearchRetain = 5;
		// Was the last search a subsearch?
		bool								m_bLastSearchWasSubsearch;


		// == Functions.
		// Divides the results into treadable parts.
		bool								DivideResultsForSubsearch( CSearchResultBase * _psrbResults, std::vector<MX_SUBSEARCH_DIV> &_vResult,
			uint64_t _ui64MinResults, const CAddressChunkList &_aclList );

		// ========
		// Searches
		// ========
		// Exact-value search on primitives.
		bool								ExactValuePrimitiveSearch( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate );

		// Greater-than search on primitives.
		bool								GreaterThanPrimitiveSearch( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate );

		// Less-than search on primitives.
		bool								LessThanPrimitiveSearch( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate );

		// In-range search on primitives.
		bool								InRangePrimitiveSearch( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate );

		// Unkown search on primitives.
		bool								UnknownPrimitiveSearch( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate );

		// ASCII/regular string search.
		template <typename _tType>
		bool								NormalStringSearch( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate );

		// A single UTF string search.
		template <typename _tType>
		bool								UtfAnyStringSearch( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate );

		// UTF-* search.
		bool								UtfAllStringSearch( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate );

		// A raw array search.
		template <typename _tType>
		bool								RawArraySearch( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate );

		// A raw array of multiple types search.
		bool								RawArrayMultiType( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate );

		// A regular-expression search.
		bool								RegexSearch( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate );

		// An Expression Search where ?? is a predefined type.
		template <typename _tType, unsigned _sIsFloat16>
		bool								ExpressionSearchPredefinedType( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate );

		// An Expression Search where ?? is an address.
		bool								ExpressionSearchAddressFixedSize( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate );

		// An Expression Search where ?? is an address and the size depends on the script return value.
		bool								ExpressionSearchAddressDynamicSize( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate );


		// ===========
		// Subsearches
		// ===========
		// Exact-value subsearch on primitives.
		bool								ExactValuePrimitiveSubsearch( MX_SUBSEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate );

		// Not-equal-to subsearch on primitives.
		bool								NotEqualToPrimitiveSubsearch( MX_SUBSEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate );

		// Greater-than subsearch on primitives.
		bool								IncreasedPrimitiveSubsearch( MX_SUBSEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate );

		// Less-than subsearch on primitives.
		bool								DecreasedPrimitiveSubsearch( MX_SUBSEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate );

		// In-range subsearch on primitives.
		bool								InRangePrimitiveSubsearch( MX_SUBSEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate );

		// Greater-than subsearch on primitives.
		bool								GreaterThanPrimitiveSubsearch( MX_SUBSEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate );

		// Less-than subsearch on primitives.
		bool								LessThanPrimitiveSubsearch( MX_SUBSEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate );

		// Same-as-before subsearch on primitives.
		bool								SameAsBeforePrimitiveSubsearch( MX_SUBSEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate );

		// Different-from-before subsearch on primitives.
		bool								DifferentFromBeforePrimitiveSubsearch( MX_SUBSEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate );

		// Changed-by subsearch on primitives.
		bool								ChangedByPrimitiveSubsearch( MX_SUBSEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate,
			MX_CHANGED_BY_SELECTOR _cbsFunctor );

		// Quick-Expression subsearch on primitives.
		template <typename _tType, unsigned _sIsFloat16>
		bool								QuickExpPrimitiveSubsearch( MX_SUBSEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate );


		// =======
		// Helpers
		// =======
		// Gets the alignment for a primitive type.
		bool								PrimitiveAlignment( MX_SEARCH_PARMS &_spParms, CUtilities::MX_DATA_TYPES _dtType, CProcess * _ppProcess );

		// Creates an MX_SEARCH_PARMS object from a MX_SUBSEARCH_PARMS object.
		static MX_SEARCH_PARMS				SearchObjToSubsearchObj( const MX_SUBSEARCH_PARMS &_spParms );

		// Gets the comparison function and size.
		bool								GetExactValCmpFuncAndSize( MX_EXACT_VAL_COMP &_evcCompFunc, uint32_t &_ui32Size, MX_SEARCH_PARMS &_spParms, bool _bConvertBothF16 );

		// Gets the comparison function and size.
		bool								GetGreaterThanCmpFuncAndSize( MX_REL_COMP &_rcCompFunc, uint32_t &_ui32Size, MX_SEARCH_PARMS &_spParms, bool _bConvertBothF16 );

		// Gets the comparison function and size.
		bool								GetLessThanCmpFuncAndSize( MX_REL_COMP &_rcCompFunc, uint32_t &_ui32Size, MX_SEARCH_PARMS &_spParms, bool _bConvertBothF16 );

		// Gets the comparison function and size.
		bool								GetRangeCmpFuncAndSize( MX_RANGE_COMP &_rcCompFunc, uint32_t &_ui32Size, MX_SEARCH_PARMS &_spParms, bool _bConvertBothF16 );

		// Gets the comparison function and size.
		bool								GetChangedByCmpFuncAndSize( MX_CHANGED_BY &_cbCompFunc, uint32_t &_ui32Size, MX_SEARCH_PARMS &_spParms, bool _bConvertBothF16 );

		// Gets the comparison function and size.
		bool								GetChangedByRangeCmpFuncAndSize( MX_CHANGED_BY &_cbCompFunc, uint32_t &_ui32Size, MX_SEARCH_PARMS &_spParms, bool _bConvertBothF16 );

		// Gets the comparison function and size.
		bool								GetChangedByPercCmpFuncAndSize( MX_CHANGED_BY &_cbCompFunc, uint32_t &_ui32Size, MX_SEARCH_PARMS &_spParms, bool _bConvertBothF16 );

		// Gets the comparison function and size.
		bool								GetChangedByRangePercCmpFuncAndSize( MX_CHANGED_BY &_cbCompFunc, uint32_t &_ui32Size, MX_SEARCH_PARMS &_spParms, bool _bConvertBothF16 );

		// User-variable function handler.
		static bool __stdcall 				UserVarHandler( uintptr_t _uiptrData, ee::CExpEvalContainer * _peecContainer, ee::CExpEvalContainer::EE_RESULT &_rResult );

		// The address reader for expressions.
		static bool __stdcall				ExpAddressHandler( uint64_t _ui64Address, ee::EE_CAST_TYPES _tType, uintptr_t _uiptrData, ee::CExpEvalContainer * _peecContainer, ee::CExpEvalContainer::EE_RESULT &_rResult );

		// The address reader for dynamic expressions.
		static bool __stdcall				ExpAddressHandler_LocalPrecache( uint64_t _ui64Address, ee::EE_CAST_TYPES _tType, uintptr_t _uiptrData, ee::CExpEvalContainer * _peecContainer, ee::CExpEvalContainer::EE_RESULT &_rResult );


		// ==============
		// == Threads. ==
		// ==============
		// Called on the chunk list to scan multiple parts at the same time.
		static DWORD WINAPI					PrimitiveExactValueSearchThreadProc( LPVOID _lpParameter );

		// Called on the chunk list to scan multiple parts at the same time.
		static DWORD WINAPI					PrimitiveRelativeSearchThreadProc( LPVOID _lpParameter );

		// Called on the chunk list to scan multiple parts at the same time.
		static DWORD WINAPI					PrimitiveRangeSearchThreadProc( LPVOID _lpParameter );

		// Called on the chunk list to scan multiple parts at the same time.
		static DWORD WINAPI					StringNormalSearchThreadProc( LPVOID _lpParameter );

		// Called on the chunk list to scan multiple parts at the same time.
		static DWORD WINAPI					StringUtfSearchThreadProc( LPVOID _lpParameter );

		// Called on the chunk list to scan multiple parts at the same time.
		static DWORD WINAPI					StringAllUtfSearchThreadProc( LPVOID _lpParameter );

		// Called on the chunk list to scan multiple parts at the same time.
		template <typename _tType>
		static DWORD WINAPI					StringRawArraySearchThreadProc( LPVOID _lpParameter );

		// Called on the chunk list to scan multiple parts at the same time.
		static DWORD WINAPI					StringRegexSearchThreadProc( LPVOID _lpParameter );

		// Called on the chunk list to scan multiple parts at the same time.
		static DWORD WINAPI					StringMultiArraySearchThreadProc( LPVOID _lpParameter );

		// Called on the chunk list to scan multiple parts at the same time.
		template <typename _tType, unsigned _sIsFloat16>
		static DWORD WINAPI					PredefExpressionSearchThreadProc( LPVOID _lpParameter );

		// Called on the chunk list to scan multiple parts at the same time.
		static DWORD WINAPI					FixedSizeExpressionSearchThreadProc( LPVOID _lpParameter );

		// Called on the chunk list to scan multiple parts at the same time.
		static DWORD WINAPI					DynamicSizeExpressionSearchThreadProc( LPVOID _lpParameter );

		// Scans multiple parts of the current results at the same time.
		static DWORD WINAPI					PrimitiveExactValueSubsearchThreadProc( LPVOID _lpParameter );

		// Scans multiple parts of the current results at the same time.
		static DWORD WINAPI					PrimitiveNotEqualToSubsearchThreadProc( LPVOID _lpParameter );

		// Scans multiple parts of the current results at the same time.
		static DWORD WINAPI					PrimitiveIncreasedSubsearchThreadProc( LPVOID _lpParameter );

		// Scans multiple parts of the current results at the same time.
		static DWORD WINAPI					PrimitiveDecreasedSubsearchThreadProc( LPVOID _lpParameter );

		// Scans multiple parts of the current results at the same time.
		static DWORD WINAPI					PrimitiveInRangeSubsearchThreadProc( LPVOID _lpParameter );

		// Scans multiple parts of the current results at the same time.
		static DWORD WINAPI					PrimitiveGreaterThanLessThanSubsearchThreadProc( LPVOID _lpParameter );

		// Scans multiple parts of the current results at the same time.
		static DWORD WINAPI					PrimitiveSameAsBeforeSubsearchThreadProc( LPVOID _lpParameter );

		// Scans multiple parts of the current results at the same time.
		static DWORD WINAPI					PrimitiveChangedBySubsearchThreadProc( LPVOID _lpParameter );

		// Scans multiple parts of the current results at the same time.
		template <typename _tType, unsigned _sIsFloat16>
		static DWORD WINAPI					PrimitiveQuickExpSubsearchThreadProc( LPVOID _lpParameter );


		// ==================
		// == Exact value. ==
		// ==================
		// Exact-value char/byte comparison.
		static bool __stdcall				Cmp_ExactVal8( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS & ) { return _patLeft.u.UInt8 == _patRight.u.UInt8; }

		// Exact-value int16/uint16 comparison.
		static bool __stdcall				Cmp_ExactVal16( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS & ) { return _patLeft.u.UInt16 == _patRight.u.UInt16; }

		// Exact-value int32/uint32/float/void * 32 comparison.
		static bool __stdcall				Cmp_ExactVal32( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS & ) { return _patLeft.u.UInt32 == _patRight.u.UInt32; }

		// Exact-value int64/uint64/double/void * 64 comparison.
		static bool __stdcall				Cmp_ExactVal64( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS & ) { return _patLeft.u.UInt64 == _patRight.u.UInt64; }

		// Quick 16-bit epsilon compare.
		static bool __stdcall				Cmp_Float16Epsilon( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms ) {
			return std::fabs( _patLeft.u.Float64 - ee::CFloat16( _patRight.u.UInt16 ).Value() ) <= _spParms.dEpsilonValue;
		}

		// Quick 16-bit epsilon compare.
		static bool __stdcall				Cmp_Float16Epsilon_ConvertBoth( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms ) {
			return std::fabs( ee::CFloat16( _patLeft.u.UInt16 ).Value() - ee::CFloat16( _patRight.u.UInt16 ).Value() ) <= _spParms.dEpsilonValue;
		}

		// Smart 16-bit epsilon compare.
		static bool __stdcall				Cmp_Float16RelativeEpsilon( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms ) {
			CUtilities::MX_DATA_TYPE dtRight;
			dtRight.u.Float64 = ee::CFloat16( _patRight.u.UInt16 ).Value();
			return Cmp_Float64RelativeEpsilon( _patLeft, dtRight, _spParms );
		}

		// Smart 16-bit epsilon compare.
		static bool __stdcall				Cmp_Float16RelativeEpsilon_ConvertBoth( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms ) {
			CUtilities::MX_DATA_TYPE dtRight, dtLeft;
			dtRight.u.Float64 = ee::CFloat16( _patRight.u.UInt16 ).Value();
			dtLeft.u.Float64 = ee::CFloat16( _patLeft.u.UInt16 ).Value();
			return Cmp_Float64RelativeEpsilon( dtLeft, dtRight, _spParms );
		}

		// Quick 32-bit epsilon compare.
		static bool __stdcall				Cmp_Float32Epsilon( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms ) {
			return ee::CExpEval::Epsilon( _patLeft.u.Float32, _patRight.u.Float32, static_cast<float>(_spParms.dEpsilonValue) );
		}

		// Smart 32-bit epsilon compare.
		static bool __stdcall				Cmp_Float32RelativeEpsilon( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms ) {
			return ee::CExpEval::RelativeEpsilon( static_cast<float>(_patLeft.u.Float32),
				static_cast<float>(_patRight.u.Float32),
				static_cast<float>(_spParms.dEpsilonValue) );
		}

		// Quick 64-bit epsilon compare.
		static bool __stdcall				Cmp_Float64Epsilon( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms ) {
			return ee::CExpEval::Epsilon( _patLeft.u.Float64, _patRight.u.Float64, _spParms.dEpsilonValue );
		}

		// Smart 64-bit epsilon compare.
		static bool __stdcall				Cmp_Float64RelativeEpsilon( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms ) {
			return ee::CExpEval::RelativeEpsilon( _patLeft.u.Float64,
				_patRight.u.Float64,
				_spParms.dEpsilonValue );
		}

		// Float compare with NaN results returning true.  This allows the negation of the compare to be used in "Not Equal To" searches.
		static bool __stdcall				Cmp_Float16WithNaN( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms ) {
			double dRight = ee::CFloat16( _patRight.u.UInt16 ).Value();
			return _patLeft.u.Float64 == dRight || std::isnan( dRight );
		}

		// Float compare with NaN results returning true.  This allows the negation of the compare to be used in "Not Equal To" searches.
		static bool __stdcall				Cmp_Float16WithNaN_ConvertBoth( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms ) {
			double dRight = ee::CFloat16( _patRight.u.UInt16 ).Value();
			return ee::CFloat16( _patLeft.u.UInt16 ).Value() == dRight || std::isnan( dRight );
		}

		// Float compare with NaN results returning true.  This allows the negation of the compare to be used in "Not Equal To" searches.
		static bool __stdcall				Cmp_Float32WithNaN( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms ) { return (_patLeft.u.Float32 == _patRight.u.Float32) || std::isnan( _patRight.u.Float32 ); }

		// Float compare with NaN results returning true.  This allows the negation of the compare to be used in "Not Equal To" searches.
		static bool __stdcall				Cmp_Float64WithNaN( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms ) { return (_patLeft.u.Float64 == _patRight.u.Float64) || std::isnan( _patRight.u.Float64 ); }

		// Quick 16-bit epsilon compare with NaN results returning true.  This allows the negation of the compare to be used in "Not Equal To" searches.
		static bool __stdcall				Cmp_Float16EpsilonWithNaN( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms ) {
			CUtilities::MX_DATA_TYPE dtRight;
			dtRight.u.Float64 = ee::CFloat16( _patRight.u.UInt16 ).Value();
			return Cmp_Float64Epsilon( _patLeft, dtRight, _spParms ) || std::isnan( dtRight.u.Float64 );
		}

		// Quick 16-bit epsilon compare with NaN results returning true.  This allows the negation of the compare to be used in "Not Equal To" searches.
		static bool __stdcall				Cmp_Float16EpsilonWithNaN_ConvertBoth( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms ) {
			CUtilities::MX_DATA_TYPE dtRight, dtLeft;
			dtRight.u.Float64 = ee::CFloat16( _patRight.u.UInt16 ).Value();
			dtLeft.u.Float64 = ee::CFloat16( _patLeft.u.UInt16 ).Value();
			return Cmp_Float64Epsilon( dtLeft, dtRight, _spParms ) || std::isnan( dtRight.u.Float64 );
		}

		// Quick 32-bit epsilon compare with NaN results returning true.  This allows the negation of the compare to be used in "Not Equal To" searches.
		static bool __stdcall				Cmp_Float32EpsilonWithNaN( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms ) {
			return Cmp_Float32Epsilon( _patLeft, _patRight, _spParms ) || std::isnan( _patRight.u.Float32 );
		}

		// Quick 64-bit epsilon compare with NaN results returning true.  This allows the negation of the compare to be used in "Not Equal To" searches.
		static bool __stdcall				Cmp_Float64EpsilonWithNaN( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms ) {
			return Cmp_Float64Epsilon( _patLeft, _patRight, _spParms ) || std::isnan( _patRight.u.Float64 );
		}

		// Smart 16-bit epsilon compare with NaN results returning true.  This allows the negation of the compare to be used in "Not Equal To" searches.
		static bool __stdcall				Cmp_Float16RelativeEpsilonWithNaN( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms ) {
			CUtilities::MX_DATA_TYPE dtRight;
			dtRight.u.Float64 = ee::CFloat16( _patRight.u.UInt16 ).Value();
			return Cmp_Float64RelativeEpsilon( _patLeft, dtRight, _spParms ) || std::isnan( dtRight.u.Float64 );
		}

		// Smart 16-bit epsilon compare with NaN results returning true.  This allows the negation of the compare to be used in "Not Equal To" searches.
		static bool __stdcall				Cmp_Float16RelativeEpsilonWithNaN_ConvertBoth( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms ) {
			CUtilities::MX_DATA_TYPE dtRight, dtLeft;
			dtRight.u.Float64 = ee::CFloat16( _patRight.u.UInt16 ).Value();
			dtLeft.u.Float64 = ee::CFloat16( _patLeft.u.UInt16 ).Value();
			return Cmp_Float64RelativeEpsilon( dtLeft, dtRight, _spParms ) || std::isnan( dtRight.u.Float64 );
		}

		// Smart 32-bit epsilon compare with NaN results returning true.  This allows the negation of the compare to be used in "Not Equal To" searches.
		static bool __stdcall				Cmp_Float32RelativeEpsilonWithNaN( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms ) {
			return Cmp_Float32RelativeEpsilon( _patLeft, _patRight, _spParms ) || std::isnan( _patRight.u.Float32 );
		}

		// Smart 64-bit epsilon compare with NaN results returning true.  This allows the negation of the compare to be used in "Not Equal To" searches.
		static bool __stdcall				Cmp_Float64RelativeEpsilonWithNaN( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms ) {
			return Cmp_Float64RelativeEpsilon( _patLeft, _patRight, _spParms ) || std::isnan( _patRight.u.Float64 );
		}


		// ===================
		// == Greater-than. ==
		// ===================
		// Greater-than primitive compare.
		static bool __stdcall				Cmp_GreaterThanSigned8( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return _patRight.u.Int8 > _patLeft.u.Int8; }

		// Greater-than primitive compare.
		static bool __stdcall				Cmp_GreaterThanSigned16( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return _patRight.u.Int16 > _patLeft.u.Int16; }

		// Greater-than primitive compare.
		static bool __stdcall				Cmp_GreaterThanSigned32( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return _patRight.u.Int32 > _patLeft.u.Int32; }

		// Greater-than primitive compare.
		static bool __stdcall				Cmp_GreaterThanSigned64( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return _patRight.u.Int64 > _patLeft.u.Int64; }

		// Greater-than primitive compare.
		static bool __stdcall				Cmp_GreaterThanUnsigned8( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return _patRight.u.UInt8 > _patLeft.u.UInt8; }

		// Greater-than primitive compare.
		static bool __stdcall				Cmp_GreaterThanUnsigned16( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return _patRight.u.UInt16 > _patLeft.u.UInt16; }

		// Greater-than primitive compare.
		static bool __stdcall				Cmp_GreaterThanUnsigned32( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return _patRight.u.UInt32 > _patLeft.u.UInt32; }

		// Greater-than primitive compare.
		static bool __stdcall				Cmp_GreaterThanUnsigned64( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return _patRight.u.UInt64 > _patLeft.u.UInt64; }

		// Greater-than primitive compare.
		static bool __stdcall				Cmp_GreaterThanFloat16( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) {
			return ee::CFloat16( _patRight.u.UInt16 ).Value() > _patLeft.u.Float64;
		}

		// Greater-than primitive compare.
		static bool __stdcall				Cmp_GreaterThanFloat16_ConvertBoth( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) {
			return ee::CFloat16( _patRight.u.UInt16 ).Value() > ee::CFloat16(  _patLeft.u.UInt16 ).Value();
		}

		// Greater-than primitive compare.
		static bool __stdcall				Cmp_GreaterThanFloat16WithNaN( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) {
			double dVal = ee::CFloat16( _patRight.u.UInt16 ).Value();
			return (dVal > _patLeft.u.Float64) || std::isnan( dVal );
		}
		// Greater-than primitive compare.
		static bool __stdcall				Cmp_GreaterThanFloat16WithNaN_ConvertBoth( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) {
			double dVal = ee::CFloat16( _patRight.u.UInt16 ).Value();
			return (dVal > ee::CFloat16( _patLeft.u.UInt16 ).Value()) || std::isnan( dVal );
		}

		// Greater-than primitive compare.
		static bool __stdcall				Cmp_GreaterThanFloat32( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return _patRight.u.Float32 > _patLeft.u.Float32; }

		// Greater-than primitive compare.
		static bool __stdcall				Cmp_GreaterThanFloat32WithNaN( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return (_patRight.u.Float32 > _patLeft.u.Float32) || std::isnan( _patRight.u.Float32 ); }

		// Greater-than primitive compare.
		static bool __stdcall				Cmp_GreaterThanFloat64( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return _patRight.u.Float64 > _patLeft.u.Float64; }

		// Greater-than primitive compare.
		static bool __stdcall				Cmp_GreaterThanFloat64WithNaN( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return (_patRight.u.Float64 > _patLeft.u.Float64) || std::isnan( _patRight.u.Float64 ); }


		// ================
		// == Less-than. ==
		// ================
		// Less-than primitive compare.
		static bool __stdcall				Cmp_LessThanSigned8( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return _patRight.u.Int8 < _patLeft.u.Int8; }

		// Less-than primitive compare.
		static bool __stdcall				Cmp_LessThanSigned16( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return _patRight.u.Int16 < _patLeft.u.Int16; }

		// Less-than primitive compare.
		static bool __stdcall				Cmp_LessThanSigned32( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return _patRight.u.Int32 < _patLeft.u.Int32; }

		// Less-than primitive compare.
		static bool __stdcall				Cmp_LessThanSigned64( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return _patRight.u.Int64 < _patLeft.u.Int64; }

		// Less-than primitive compare.
		static bool __stdcall				Cmp_LessThanUnsigned8( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return _patRight.u.UInt8 < _patLeft.u.UInt8; }

		// Less-than primitive compare.
		static bool __stdcall				Cmp_LessThanUnsigned16( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return _patRight.u.UInt16 < _patLeft.u.UInt16; }

		// Less-than primitive compare.
		static bool __stdcall				Cmp_LessThanUnsigned32( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return _patRight.u.UInt32 < _patLeft.u.UInt32; }

		// Less-than primitive compare.
		static bool __stdcall				Cmp_LessThanUnsigned64( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return _patRight.u.UInt64 < _patLeft.u.UInt64; }

		// Less-than primitive compare.
		static bool __stdcall				Cmp_LessThanFloat16( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) {
			return ee::CFloat16( _patRight.u.UInt16 ).Value() < _patLeft.u.Float64;
		}

		// Less-than primitive compare.
		static bool __stdcall				Cmp_LessThanFloat16_ConvertBoth( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) {
			return ee::CFloat16( _patRight.u.UInt16 ).Value() < ee::CFloat16( _patLeft.u.UInt16 ).Value();
		}

		// Less-than primitive compare.
		static bool __stdcall				Cmp_LessThanFloat16WithNaN( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) {
			double dVal = ee::CFloat16( _patRight.u.UInt16 ).Value();
			return (dVal < _patLeft.u.Float64) || std::isnan( dVal );
		}

		// Less-than primitive compare.
		static bool __stdcall				Cmp_LessThanFloat16WithNaN_ConvertBoth( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) {
			double dVal = ee::CFloat16( _patRight.u.UInt16 ).Value();
			return (dVal < ee::CFloat16( _patLeft.u.UInt16 ).Value()) || std::isnan( dVal );
		}

		// Less-than primitive compare.
		static bool __stdcall				Cmp_LessThanFloat32( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return _patRight.u.Float32 < _patLeft.u.Float32; }

		// Less-than primitive compare.
		static bool __stdcall				Cmp_LessThanFloat32WithNaN( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return (_patRight.u.Float32 < _patLeft.u.Float32) || std::isnan( _patRight.u.Float32 ); }

		// Less-than primitive compare.
		static bool __stdcall				Cmp_LessThanFloat64( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return _patRight.u.Float64 < _patLeft.u.Float64; }

		// Less-than primitive compare.
		static bool __stdcall				Cmp_LessThanFloat64WithNaN( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return (_patRight.u.Float64 < _patLeft.u.Float64) || std::isnan( _patRight.u.Float64 ); }


		// ============
		// == Range. ==
		// ============
		// In-range primitive compare.
		static bool __stdcall				Cmp_InRangeInt8( const CUtilities::MX_DATA_TYPE &_patLow, const CUtilities::MX_DATA_TYPE &_patHigh, const CUtilities::MX_DATA_TYPE &_patVal ) { return _patVal.u.Int8 >= _patLow.u.Int8 && _patVal.u.Int8 <= _patHigh.u.Int8; }

		// In-range primitive compare.
		static bool __stdcall				Cmp_InRangeInt16( const CUtilities::MX_DATA_TYPE &_patLow, const CUtilities::MX_DATA_TYPE &_patHigh, const CUtilities::MX_DATA_TYPE &_patVal ) { return _patVal.u.Int16 >= _patLow.u.Int16 && _patVal.u.Int16 <= _patHigh.u.Int16; }

		// In-range primitive compare.
		static bool __stdcall				Cmp_InRangeInt32( const CUtilities::MX_DATA_TYPE &_patLow, const CUtilities::MX_DATA_TYPE &_patHigh, const CUtilities::MX_DATA_TYPE &_patVal ) { return _patVal.u.Int32 >= _patLow.u.Int32 && _patVal.u.Int32 <= _patHigh.u.Int32; }

		// In-range primitive compare.
		static bool __stdcall				Cmp_InRangeInt64( const CUtilities::MX_DATA_TYPE &_patLow, const CUtilities::MX_DATA_TYPE &_patHigh, const CUtilities::MX_DATA_TYPE &_patVal ) { return _patVal.u.Int64 >= _patLow.u.Int64 && _patVal.u.Int64 <= _patHigh.u.Int64; }

		// In-range primitive compare.
		static bool __stdcall				Cmp_InRangeUInt8( const CUtilities::MX_DATA_TYPE &_patLow, const CUtilities::MX_DATA_TYPE &_patHigh, const CUtilities::MX_DATA_TYPE &_patVal ) { return _patVal.u.UInt8 >= _patLow.u.UInt8 && _patVal.u.UInt8 <= _patHigh.u.UInt8; }

		// In-range primitive compare.
		static bool __stdcall				Cmp_InRangeUInt16( const CUtilities::MX_DATA_TYPE &_patLow, const CUtilities::MX_DATA_TYPE &_patHigh, const CUtilities::MX_DATA_TYPE &_patVal ) { return _patVal.u.UInt16 >= _patLow.u.UInt16 && _patVal.u.UInt16 <= _patHigh.u.UInt16; }

		// In-range primitive compare.
		static bool __stdcall				Cmp_InRangeUInt32( const CUtilities::MX_DATA_TYPE &_patLow, const CUtilities::MX_DATA_TYPE &_patHigh, const CUtilities::MX_DATA_TYPE &_patVal ) { return _patVal.u.UInt32 >= _patLow.u.UInt32 && _patVal.u.UInt32 <= _patHigh.u.UInt32; }

		// In-range primitive compare.
		static bool __stdcall				Cmp_InRangeUInt64( const CUtilities::MX_DATA_TYPE &_patLow, const CUtilities::MX_DATA_TYPE &_patHigh, const CUtilities::MX_DATA_TYPE &_patVal ) { return _patVal.u.UInt64 >= _patLow.u.UInt64 && _patVal.u.UInt64 <= _patHigh.u.UInt64; }

		// In-range primitive compare.
		static bool __stdcall				Cmp_InRangeFloat16( const CUtilities::MX_DATA_TYPE &_patLow, const CUtilities::MX_DATA_TYPE &_patHigh, const CUtilities::MX_DATA_TYPE &_patVal ) {
			double dVal = ee::CFloat16( _patVal.u.UInt16 ).Value();
			return dVal >= _patLow.u.Float64 && dVal <= _patHigh.u.Float64;
		}

		// In-range primitive compare.
		static bool __stdcall				Cmp_InRangeFloat16WithNaN( const CUtilities::MX_DATA_TYPE &_patLow, const CUtilities::MX_DATA_TYPE &_patHigh, const CUtilities::MX_DATA_TYPE &_patVal ) {
			double dVal = ee::CFloat16( _patVal.u.UInt16 ).Value();
			return (dVal >= _patLow.u.Float64 && dVal <= _patHigh.u.Float64) || std::isnan( dVal );
		}

		// In-range primitive compare.
		static bool __stdcall				Cmp_InRangeFloat32( const CUtilities::MX_DATA_TYPE &_patLow, const CUtilities::MX_DATA_TYPE &_patHigh, const CUtilities::MX_DATA_TYPE &_patVal ) { return _patVal.u.Float32 >= _patLow.u.Float32 && _patVal.u.Float32 <= _patHigh.u.Float32; }

		// In-range primitive compare.
		static bool __stdcall				Cmp_InRangeFloat32WithNaN( const CUtilities::MX_DATA_TYPE &_patLow, const CUtilities::MX_DATA_TYPE &_patHigh, const CUtilities::MX_DATA_TYPE &_patVal ) { return (_patVal.u.Float32 >= _patLow.u.Float32 && _patVal.u.Float32 <= _patHigh.u.Float32) || std::isnan( _patVal.u.Float32 ); }

		// In-range primitive compare.
		static bool __stdcall				Cmp_InRangeFloat64( const CUtilities::MX_DATA_TYPE &_patLow, const CUtilities::MX_DATA_TYPE &_patHigh, const CUtilities::MX_DATA_TYPE &_patVal ) { return _patVal.u.Float64 >= _patLow.u.Float64 && _patVal.u.Float64 <= _patHigh.u.Float64; }

		// In-range primitive compare.
		static bool __stdcall				Cmp_InRangeFloat64WithNaN( const CUtilities::MX_DATA_TYPE &_patLow, const CUtilities::MX_DATA_TYPE &_patHigh, const CUtilities::MX_DATA_TYPE &_patVal ) { return (_patVal.u.Float64 >= _patLow.u.Float64 && _patVal.u.Float64 <= _patHigh.u.Float64) || std::isnan( _patVal.u.Float64 ); }


		// =====================
		// == Changed-by. ==
		// =====================
		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByInt8( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &/*_spParms*/ ) {
			return _patCur.u.Int8 - _patPrev.u.Int8 == _patTest.u.Int8;
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByInt16( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &/*_spParms*/ ) {
			return _patCur.u.Int16 - _patPrev.u.Int16 == _patTest.u.Int16;
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByInt32( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &/*_spParms*/ ) {
			return _patCur.u.Int32 - _patPrev.u.Int32 == _patTest.u.Int32;
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByInt64( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &/*_spParms*/ ) {
			return _patCur.u.Int64 - _patPrev.u.Int64 == _patTest.u.Int64;
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByUInt8( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &/*_spParms*/ ) {
			return _patCur.u.UInt8 - _patPrev.u.UInt8 == _patTest.u.UInt8;
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByUInt16( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &/*_spParms*/ ) {
			return _patCur.u.UInt16 - _patPrev.u.UInt16 == _patTest.u.UInt16;
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByUInt32( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &/*_spParms*/ ) {
			return _patCur.u.UInt32 - _patPrev.u.UInt32 == _patTest.u.UInt32;
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByUInt64( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &/*_spParms*/ ) {
			return _patCur.u.UInt64 - _patPrev.u.UInt64 == _patTest.u.UInt64;
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByFloat16( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &/*_spParms*/ ) {
			return ee::CFloat16( _patCur.u.UInt16 ).Value() - ee::CFloat16( _patPrev.u.UInt16 ).Value() == _patTest.u.Float64;
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByFloat16WithNaN( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &/*_spParms*/ ) {
			double dVal = ee::CFloat16( _patCur.u.UInt16 ).Value() - ee::CFloat16( _patPrev.u.UInt16 ).Value();
			return (dVal == _patTest.u.Float64) || std::isnan( dVal );
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByFloat32( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &/*_spParms*/ ) {
			return _patCur.u.Float32 - _patPrev.u.Float32 == _patTest.u.Float32;
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByFloat32WithNaN( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &/*_spParms*/ ) {
			float fVal = _patCur.u.Float32 - _patPrev.u.Float32;
			return (fVal == _patTest.u.Float32) || std::isnan( fVal );
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByFloat64( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &/*_spParms*/ ) {
			return _patCur.u.Float64 - _patPrev.u.Float64 == _patTest.u.Float64;
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByFloat64WithNaN( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &/*_spParms*/ ) {
			double dVal = _patCur.u.Float64 - _patPrev.u.Float64;
			return (dVal == _patTest.u.Float64) || std::isnan( dVal );
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByFloat16RelativeEpsilon( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &_spParms ) {
			double dVal = ee::CFloat16( _patCur.u.UInt16 ).Value() - ee::CFloat16( _patPrev.u.UInt16 ).Value();
			return ee::CExpEval::RelativeEpsilon( dVal,
				_patTest.u.Float64,
				_spParms.dEpsilonValue );
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByFloat16Epsilon( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &_spParms ) {
			double dVal = ee::CFloat16( _patCur.u.UInt16 ).Value() - ee::CFloat16( _patPrev.u.UInt16 ).Value();
			return ee::CExpEval::Epsilon( dVal, _patTest.u.Float64, _spParms.dEpsilonValue );
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByFloat16RelativeEpsilonWithNaN( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &_spParms ) {
			double dVal = ee::CFloat16( _patCur.u.UInt16 ).Value() - ee::CFloat16( _patPrev.u.UInt16 ).Value();
			return ee::CExpEval::RelativeEpsilon( dVal,
				_patTest.u.Float64,
				_spParms.dEpsilonValue ) || std::isnan( dVal );
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByFloat16EpsilonWithNaN( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &_spParms ) {
			double dVal = ee::CFloat16( _patCur.u.UInt16 ).Value() - ee::CFloat16( _patPrev.u.UInt16 ).Value();
			return ee::CExpEval::Epsilon( dVal, _patTest.u.Float64, _spParms.dEpsilonValue ) || std::isnan( dVal );
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByFloat32RelativeEpsilon( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &_spParms ) {
			double dVal = _patCur.u.Float32 - _patPrev.u.Float32;
			return ee::CExpEval::RelativeEpsilon( dVal,
				static_cast<double>(_patTest.u.Float32),
				_spParms.dEpsilonValue );
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByFloat32Epsilon( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &_spParms ) {
			double dVal = _patCur.u.Float32 - _patPrev.u.Float32;
			return ee::CExpEval::Epsilon( dVal, static_cast<double>(_patTest.u.Float32), _spParms.dEpsilonValue );
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByFloat32RelativeEpsilonWithNaN( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &_spParms ) {
			double dVal = _patCur.u.Float32 - _patPrev.u.Float32;
			return ee::CExpEval::RelativeEpsilon( dVal,
				static_cast<double>(_patTest.u.Float32),
				_spParms.dEpsilonValue ) || std::isnan( dVal );
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByFloat32EpsilonWithNaN( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &_spParms ) {
			double dVal = _patCur.u.Float32 - _patPrev.u.Float32;
			return ee::CExpEval::Epsilon( dVal, static_cast<double>(_patTest.u.Float32), _spParms.dEpsilonValue ) || std::isnan( dVal );
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByFloat64RelativeEpsilon( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &_spParms ) {
			double dVal = _patCur.u.Float64 - _patPrev.u.Float64;
			return ee::CExpEval::RelativeEpsilon( dVal,
				_patTest.u.Float64,
				_spParms.dEpsilonValue );
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByFloat64Epsilon( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &_spParms ) {
			double dVal = _patCur.u.Float64 - _patPrev.u.Float64;
			return ee::CExpEval::Epsilon( dVal, _patTest.u.Float64, _spParms.dEpsilonValue );
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByFloat64RelativeEpsilonWithNaN( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &_spParms ) {
			double dVal = _patCur.u.Float64 - _patPrev.u.Float64;
			return ee::CExpEval::RelativeEpsilon( dVal,
				_patTest.u.Float64,
				_spParms.dEpsilonValue ) || std::isnan( dVal );
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByFloat64EpsilonWithNaN( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &_spParms ) {
			double dVal = _patCur.u.Float64 - _patPrev.u.Float64;
			return ee::CExpEval::Epsilon( dVal, _patTest.u.Float64, _spParms.dEpsilonValue ) || std::isnan( dVal );
		}


		// =======================
		// == Changed-by-range. ==
		// =======================
		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByRangeInt8( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &/*_patTest*/, const MX_SEARCH_PARMS &_spParms ) {
			int8_t i8Val = static_cast<int8_t>(_patCur.u.Int8 - _patPrev.u.Int8);
			return i8Val >= _spParms.dtLVal.u.Int8 && i8Val <= _spParms.dtRVal.u.Int8;
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByRangeInt16( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &/*_patTest*/, const MX_SEARCH_PARMS &_spParms ) {
			int16_t i16Val = static_cast<int16_t>(_patCur.u.Int16 - _patPrev.u.Int16);
			return i16Val >= _spParms.dtLVal.u.Int16 && i16Val <= _spParms.dtRVal.u.Int16;
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByRangeInt32( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &/*_patTest*/, const MX_SEARCH_PARMS &_spParms ) {
			int32_t i32Val = static_cast<int32_t>(_patCur.u.Int32 - _patPrev.u.Int32);
			return i32Val >= _spParms.dtLVal.u.Int32 && i32Val <= _spParms.dtRVal.u.Int32;
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByRangeInt64( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &/*_patTest*/, const MX_SEARCH_PARMS &_spParms ) {
			int64_t i64Val = static_cast<int64_t>(_patCur.u.Int64 - _patPrev.u.Int64);
			return i64Val >= _spParms.dtLVal.u.Int64 && i64Val <= _spParms.dtRVal.u.Int64;
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByRangeUInt8( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &/*_patTest*/, const MX_SEARCH_PARMS &_spParms ) {
			uint8_t ui8Val = static_cast<uint8_t>(_patCur.u.UInt8 - _patPrev.u.UInt8);
			return ui8Val >= _spParms.dtLVal.u.UInt8 && ui8Val <= _spParms.dtRVal.u.UInt8;
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByRangeUInt16( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &/*_patTest*/, const MX_SEARCH_PARMS &_spParms ) {
			uint16_t ui16Val = static_cast<uint16_t>(_patCur.u.UInt16 - _patPrev.u.UInt16);
			return ui16Val >= _spParms.dtLVal.u.UInt16 && ui16Val <= _spParms.dtRVal.u.UInt16;
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByRangeUInt32( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &/*_patTest*/, const MX_SEARCH_PARMS &_spParms ) {
			uint32_t ui32Val = static_cast<uint32_t>(_patCur.u.UInt32 - _patPrev.u.UInt32);
			return ui32Val >= _spParms.dtLVal.u.UInt32 && ui32Val <= _spParms.dtRVal.u.UInt32;
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByRangeUInt64( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &/*_patTest*/, const MX_SEARCH_PARMS &_spParms ) {
			uint64_t ui64Val = static_cast<uint64_t>(_patCur.u.UInt64 - _patPrev.u.UInt64);
			return ui64Val >= _spParms.dtLVal.u.UInt64 && ui64Val <= _spParms.dtRVal.u.UInt64;
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByRangeFloat16( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &/*_patTest*/, const MX_SEARCH_PARMS &_spParms ) {
			double dVal = ee::CFloat16( _patCur.u.UInt16 ).Value() - ee::CFloat16( _patPrev.u.UInt16 ).Value();
			return dVal >= _spParms.dtLVal.u.Float64 && dVal <= _spParms.dtRVal.u.Float64;
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByRangeFloat32( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &/*_patTest*/, const MX_SEARCH_PARMS &_spParms ) {
			double dVal = _patCur.u.Float32 - _patPrev.u.Float32;
			return dVal >= _spParms.dtLVal.u.Float32 && dVal <= _spParms.dtRVal.u.Float32;
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByRangeFloat64( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &/*_patTest*/, const MX_SEARCH_PARMS &_spParms ) {
			double dVal = _patCur.u.Float64 - _patPrev.u.Float64;
			return dVal >= _spParms.dtLVal.u.Float64 && dVal <= _spParms.dtRVal.u.Float64;
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByRangeFloat16WithNaN( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &/*_patTest*/, const MX_SEARCH_PARMS &_spParms ) {
			double dVal = ee::CFloat16( _patCur.u.UInt16 ).Value() - ee::CFloat16( _patPrev.u.UInt16 ).Value();
			return (dVal >= _spParms.dtLVal.u.Float64 && dVal <= _spParms.dtRVal.u.Float64) || std::isnan( dVal );
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByRangeFloat32WithNaN( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &/*_patTest*/, const MX_SEARCH_PARMS &_spParms ) {
			double dVal = _patCur.u.Float32 - _patPrev.u.Float32;
			return (dVal >= _spParms.dtLVal.u.Float32 && dVal <= _spParms.dtRVal.u.Float32) || std::isnan( dVal );
		}

		// Changed-by primitive compare.
		static bool __stdcall				Cmp_ChangedByRangeFloat64WithNaN( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &/*_patTest*/, const MX_SEARCH_PARMS &_spParms ) {
			double dVal = _patCur.u.Float64 - _patPrev.u.Float64;
			return (dVal >= _spParms.dtLVal.u.Float64 && dVal <= _spParms.dtRVal.u.Float64) || std::isnan( dVal );
		}


		// =========================
		// == Changed-by-percent. ==
		// =========================
		// Changed-by-percent primitive compare.
		static bool __stdcall				Cmp_ChangedByPercInt8( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &/*_spParms*/ ) {
			if ( !_patPrev.u.Int8 ) { return false; }
			return static_cast<double>(_patCur.u.Int8) / static_cast<double>(_patPrev.u.Int8) == _patTest.u.Float64;
		}

		// Changed-by-percent primitive compare.
		static bool __stdcall				Cmp_ChangedByPercInt16( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &/*_spParms*/ ) {
			if ( !_patPrev.u.Int16 ) { return false; }
			return static_cast<double>(_patCur.u.Int16) / static_cast<double>(_patPrev.u.Int16) == _patTest.u.Float64;
		}

		// Changed-by-percent primitive compare.
		static bool __stdcall				Cmp_ChangedByPercInt32( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &/*_spParms*/ ) {
			if ( !_patPrev.u.Int32 ) { return false; }
			return static_cast<double>(_patCur.u.Int32) / static_cast<double>(_patPrev.u.Int32) == _patTest.u.Float64;
		}

		// Changed-by-percent primitive compare.
		static bool __stdcall				Cmp_ChangedByPercInt64( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &/*_spParms*/ ) {
			if ( !_patPrev.u.Int64 ) { return false; }
			return static_cast<double>(_patCur.u.Int64) / static_cast<double>(_patPrev.u.Int64) == _patTest.u.Float64;
		}

		// Changed-by-percent primitive compare.
		static bool __stdcall				Cmp_ChangedByPercUInt8( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &/*_spParms*/ ) {
			if ( !_patPrev.u.UInt8 ) { return false; }
			return static_cast<double>(_patCur.u.UInt8) / static_cast<double>(_patPrev.u.UInt8) == _patTest.u.Float64;
		}

		// Changed-by-percent primitive compare.
		static bool __stdcall				Cmp_ChangedByPercUInt16( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &/*_spParms*/ ) {
			if ( !_patPrev.u.UInt16 ) { return false; }
			return static_cast<double>(_patCur.u.UInt16) / static_cast<double>(_patPrev.u.UInt16) == _patTest.u.Float64;
		}

		// Changed-by-percent primitive compare.
		static bool __stdcall				Cmp_ChangedByPercUInt32( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &/*_spParms*/ ) {
			if ( !_patPrev.u.UInt32 ) { return false; }
			return static_cast<double>(_patCur.u.UInt32) / static_cast<double>(_patPrev.u.UInt32) == _patTest.u.Float64;
		}

		// Changed-by-percent primitive compare.
		static bool __stdcall				Cmp_ChangedByPercUInt64( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &/*_spParms*/ ) {
			if ( !_patPrev.u.UInt64 ) { return false; }
			return static_cast<double>(_patCur.u.UInt64) / static_cast<double>(_patPrev.u.UInt64) == _patTest.u.Float64;
		}

		// Changed-by-percent primitive compare.
		static bool __stdcall				Cmp_ChangedByPercFloat16( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &/*_spParms*/ ) {
			double dTmp = ee::CFloat16( _patPrev.u.UInt16 ).Value();
			if ( !dTmp ) { return false; }
			return ee::CFloat16( _patCur.u.UInt16 ).Value() / dTmp == _patTest.u.Float64;
		}

		// Changed-by-percent primitive compare.
		static bool __stdcall				Cmp_ChangedByPercFloat16WithNaN( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &/*_spParms*/ ) {
			double dTmp = ee::CFloat16( _patPrev.u.UInt16 ).Value();
			if ( !dTmp ) { return false; }
			double dVal = ee::CFloat16( _patCur.u.UInt16 ).Value() / dTmp;
			return (dVal == _patTest.u.Float64) || std::isnan( dVal );
		}

		// Changed-by-percent primitive compare.
		static bool __stdcall				Cmp_ChangedByPercFloat32( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &/*_spParms*/ ) {
			if ( !_patPrev.u.Float32 ) { return false; }
			return _patCur.u.Float32 / _patPrev.u.Float32 == _patTest.u.Float64;
		}

		// Changed-by-percent primitive compare.
		static bool __stdcall				Cmp_ChangedByPercFloat32WithNaN( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &/*_spParms*/ ) {
			if ( !_patPrev.u.Float32 ) { return false; }
			double dVal = _patCur.u.Float32 / _patPrev.u.Float32;
			return (dVal == _patTest.u.Float64) || std::isnan( dVal );
		}

		// Changed-by-percent primitive compare.
		static bool __stdcall				Cmp_ChangedByPercFloat64( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &/*_spParms*/ ) {
			if ( !_patPrev.u.Float64 ) { return false; }
			return _patCur.u.Float64 / _patPrev.u.Float64 == _patTest.u.Float64;
		}

		// Changed-by-percent primitive compare.
		static bool __stdcall				Cmp_ChangedByPercFloat64WithNaN( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &/*_spParms*/ ) {
			if ( !_patPrev.u.Float64 ) { return false; }
			double dVal = _patCur.u.Float64 / _patPrev.u.Float64;
			return (dVal == _patTest.u.Float64) || std::isnan( dVal );
		}

		// Changed-by-percent primitive compare.
		static bool __stdcall				Cmp_ChangedByPercFloat16RelativeEpsilon( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &_spParms ) {
			double dTmp = ee::CFloat16( _patPrev.u.UInt16 ).Value();
			if ( !dTmp ) { return false; }
			return ee::CExpEval::RelativeEpsilon( ee::CFloat16( _patCur.u.UInt16 ).Value() / dTmp,
				_patTest.u.Float64,
				_spParms.dEpsilonValue );
		}

		// Changed-by-percent primitive compare.
		static bool __stdcall				Cmp_ChangedByPercFloat16RelativeEpsilonWithNaN( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &_spParms ) {
			double dTmp = ee::CFloat16( _patPrev.u.UInt16 ).Value();
			if ( !dTmp ) { return false; }
			double dVal = ee::CFloat16( _patCur.u.UInt16 ).Value() / dTmp;
			return ee::CExpEval::RelativeEpsilon( dVal,
				_patTest.u.Float64,
				_spParms.dEpsilonValue ) || std::isnan( dVal );
		}

		// Changed-by-percent primitive compare.
		static bool __stdcall				Cmp_ChangedByPercFloat16Epsilon( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &_spParms ) {
			double dTmp = ee::CFloat16( _patPrev.u.UInt16 ).Value();
			if ( !dTmp ) { return false; }
			return ee::CExpEval::Epsilon( ee::CFloat16( _patCur.u.UInt16 ).Value() / dTmp,
				_patTest.u.Float64,
				_spParms.dEpsilonValue );
		}

		// Changed-by-percent primitive compare.
		static bool __stdcall				Cmp_ChangedByPercFloat16EpsilonWithNaN( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &_spParms ) {
			double dTmp = ee::CFloat16( _patPrev.u.UInt16 ).Value();
			if ( !dTmp ) { return false; }
			double dVal = ee::CFloat16( _patCur.u.UInt16 ).Value() / dTmp;
			return ee::CExpEval::Epsilon( dVal,
				_patTest.u.Float64,
				_spParms.dEpsilonValue ) || std::isnan( dVal );
		}

		// Changed-by-percent primitive compare.
		static bool __stdcall				Cmp_ChangedByPercFloat32RelativeEpsilon( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &_spParms ) {
			if ( !_patPrev.u.Float32 ) { return false; }
			return ee::CExpEval::RelativeEpsilon( static_cast<double>(_patCur.u.Float32 / _patPrev.u.Float32),
				_patTest.u.Float64,
				_spParms.dEpsilonValue );
		}

		// Changed-by-percent primitive compare.
		static bool __stdcall				Cmp_ChangedByPercFloat32RelativeEpsilonWithNaN( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &_spParms ) {
			if ( !_patPrev.u.Float32 ) { return false; }
			double dVal = _patCur.u.Float32 / _patPrev.u.Float32;
			return ee::CExpEval::RelativeEpsilon( dVal,
				_patTest.u.Float64,
				_spParms.dEpsilonValue ) || std::isnan( dVal );
		}

		// Changed-by-percent primitive compare.
		static bool __stdcall				Cmp_ChangedByPercFloat32Epsilon( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &_spParms ) {
			if ( !_patPrev.u.Float32 ) { return false; }
			return ee::CExpEval::Epsilon( static_cast<double>(_patCur.u.Float32 / _patPrev.u.Float32),
				_patTest.u.Float64,
				_spParms.dEpsilonValue );
		}

		// Changed-by-percent primitive compare.
		static bool __stdcall				Cmp_ChangedByPercFloat32EpsilonWithNaN( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &_spParms ) {
			if ( !_patPrev.u.Float32 ) { return false; }
			double dVal = _patCur.u.Float32 / _patPrev.u.Float32;
			return ee::CExpEval::Epsilon( dVal,
				_patTest.u.Float64,
				_spParms.dEpsilonValue ) || std::isnan( dVal );
		}

		// Changed-by-percent primitive compare.
		static bool __stdcall				Cmp_ChangedByPercFloat64RelativeEpsilon( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &_spParms ) {
			if ( !_patPrev.u.Float64 ) { return false; }
			return ee::CExpEval::RelativeEpsilon( _patCur.u.Float64 / _patPrev.u.Float64,
				_patTest.u.Float64,
				_spParms.dEpsilonValue );
		}

		// Changed-by-percent primitive compare.
		static bool __stdcall				Cmp_ChangedByPercFloat64RelativeEpsilonWithNaN( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &_spParms ) {
			if ( !_patPrev.u.Float64 ) { return false; }
			double dVal = _patCur.u.Float64 / _patPrev.u.Float64;
			return ee::CExpEval::RelativeEpsilon( dVal,
				_patTest.u.Float64,
				_spParms.dEpsilonValue ) || std::isnan( dVal );
		}

		// Changed-by-percent primitive compare.
		static bool __stdcall				Cmp_ChangedByPercFloat64Epsilon( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &_spParms ) {
			if ( !_patPrev.u.Float64 ) { return false; }
			return ee::CExpEval::Epsilon( _patCur.u.Float64 / _patPrev.u.Float64,
				_patTest.u.Float64,
				_spParms.dEpsilonValue );
		}

		// Changed-by-percent primitive compare.
		static bool __stdcall				Cmp_ChangedByPercFloat64EpsilonWithNaN( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &_patTest, const MX_SEARCH_PARMS &_spParms ) {
			if ( !_patPrev.u.Float64 ) { return false; }
			double dVal = _patCur.u.Float64 / _patPrev.u.Float64;
			return ee::CExpEval::Epsilon( dVal,
				_patTest.u.Float64,
				_spParms.dEpsilonValue ) || std::isnan( dVal );
		}


		// ===============================
		// == Changed-by-range-percent. ==
		// ===============================
		// Changed-by-range (percent) primitive compare.
		static bool __stdcall				Cmp_ChangedByRangePercInt8( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &/*_patTest*/, const MX_SEARCH_PARMS &_spParms ) {
			if ( !_patPrev.u.Int8 ) { return false; }
			double dPerc = static_cast<double>(_patCur.u.Int8) / static_cast<double>(_patPrev.u.Int8);
			return dPerc >= _spParms.dtLVal.u.Float64 && dPerc <= _spParms.dtRVal.u.Float64;
		}

		// Changed-by-range (percent) primitive compare.
		static bool __stdcall				Cmp_ChangedByRangePercInt16( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &/*_patTest*/, const MX_SEARCH_PARMS &_spParms ) {
			if ( !_patPrev.u.Int16 ) { return false; }
			double dPerc = static_cast<double>(_patCur.u.Int16) / static_cast<double>(_patPrev.u.Int16);
			return dPerc >= _spParms.dtLVal.u.Float64 && dPerc <= _spParms.dtRVal.u.Float64;
		}

		// Changed-by-range (percent) primitive compare.
		static bool __stdcall				Cmp_ChangedByRangePercInt32( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &/*_patTest*/, const MX_SEARCH_PARMS &_spParms ) {
			if ( !_patPrev.u.Int32 ) { return false; }
			double dPerc = static_cast<double>(_patCur.u.Int32) / static_cast<double>(_patPrev.u.Int32);
			return dPerc >= _spParms.dtLVal.u.Float64 && dPerc <= _spParms.dtRVal.u.Float64;
		}

		// Changed-by-range (percent) primitive compare.
		static bool __stdcall				Cmp_ChangedByRangePercInt64( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &/*_patTest*/, const MX_SEARCH_PARMS &_spParms ) {
			if ( !_patPrev.u.Int64 ) { return false; }
			double dPerc = static_cast<double>(_patCur.u.Int64) / static_cast<double>(_patPrev.u.Int64);
			return dPerc >= _spParms.dtLVal.u.Float64 && dPerc <= _spParms.dtRVal.u.Float64;
		}

		// Changed-by-range (percent) primitive compare.
		static bool __stdcall				Cmp_ChangedByRangePercUInt8( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &/*_patTest*/, const MX_SEARCH_PARMS &_spParms ) {
			if ( !_patPrev.u.UInt8 ) { return false; }
			double dPerc = static_cast<double>(_patCur.u.UInt8) / static_cast<double>(_patPrev.u.UInt8);
			return dPerc >= _spParms.dtLVal.u.Float64 && dPerc <= _spParms.dtRVal.u.Float64;
		}

		// Changed-by-range (percent) primitive compare.
		static bool __stdcall				Cmp_ChangedByRangePercUInt16( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &/*_patTest*/, const MX_SEARCH_PARMS &_spParms ) {
			if ( !_patPrev.u.UInt16 ) { return false; }
			double dPerc = static_cast<double>(_patCur.u.UInt16) / static_cast<double>(_patPrev.u.UInt16);
			return dPerc >= _spParms.dtLVal.u.Float64 && dPerc <= _spParms.dtRVal.u.Float64;
		}

		// Changed-by-range (percent) primitive compare.
		static bool __stdcall				Cmp_ChangedByRangePercUInt32( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &/*_patTest*/, const MX_SEARCH_PARMS &_spParms ) {
			if ( !_patPrev.u.UInt32 ) { return false; }
			double dPerc = static_cast<double>(_patCur.u.UInt32) / static_cast<double>(_patPrev.u.UInt32);
			return dPerc >= _spParms.dtLVal.u.Float64 && dPerc <= _spParms.dtRVal.u.Float64;
		}

		// Changed-by-range (percent) primitive compare.
		static bool __stdcall				Cmp_ChangedByRangePercUInt64( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &/*_patTest*/, const MX_SEARCH_PARMS &_spParms ) {
			if ( !_patPrev.u.UInt64 ) { return false; }
			double dPerc = static_cast<double>(_patCur.u.UInt64) / static_cast<double>(_patPrev.u.UInt64);
			return dPerc >= _spParms.dtLVal.u.Float64 && dPerc <= _spParms.dtRVal.u.Float64;
		}

		// Changed-by-range (percent) primitive compare.
		static bool __stdcall				Cmp_ChangedByRangePercFloat16( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &/*_patTest*/, const MX_SEARCH_PARMS &_spParms ) {
			double dTmp = ee::CFloat16( _patPrev.u.UInt16 ).Value();
			if ( !dTmp ) { return false; }
			double dPerc = ee::CFloat16( _patCur.u.UInt16 ).Value() / dTmp;
			return dPerc >= _spParms.dtLVal.u.Float64 && dPerc <= _spParms.dtRVal.u.Float64;
		}

		// Changed-by-range (percent) primitive compare.
		static bool __stdcall				Cmp_ChangedByRangePercFloat16WithNaN( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &/*_patTest*/, const MX_SEARCH_PARMS &_spParms ) {
			double dTmp = ee::CFloat16( _patPrev.u.UInt16 ).Value();
			if ( !dTmp ) { return std::isnan( ee::CFloat16( _patCur.u.UInt16 ).Value() ); }
			double dPerc = ee::CFloat16( _patCur.u.UInt16 ).Value() / dTmp;
			return (dPerc >= _spParms.dtLVal.u.Float64 && dPerc <= _spParms.dtRVal.u.Float64) || std::isnan( dPerc );
		}

		// Changed-by-range (percent) primitive compare.
		static bool __stdcall				Cmp_ChangedByRangePercFloat32( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &/*_patTest*/, const MX_SEARCH_PARMS &_spParms ) {
			if ( !_patPrev.u.Float32 ) { return false; }
			double dPerc = _patCur.u.Float32 / _patPrev.u.Float32;
			return dPerc >= _spParms.dtLVal.u.Float64 && dPerc <= _spParms.dtRVal.u.Float64;
		}

		// Changed-by-range (percent) primitive compare.
		static bool __stdcall				Cmp_ChangedByRangePercFloat32WithNaN( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &/*_patTest*/, const MX_SEARCH_PARMS &_spParms ) {
			if ( !_patPrev.u.Float32 ) { return std::isnan( _patCur.u.Float32 ); }
			double dPerc = _patCur.u.Float32 / _patPrev.u.Float32;
			return (dPerc >= _spParms.dtLVal.u.Float64 && dPerc <= _spParms.dtRVal.u.Float64) || std::isnan( dPerc );
		}

		// Changed-by-range (percent) primitive compare.
		static bool __stdcall				Cmp_ChangedByRangePercFloat64( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &/*_patTest*/, const MX_SEARCH_PARMS &_spParms ) {
			if ( !_patPrev.u.Float64 ) { return false; }
			double dPerc = _patCur.u.Float64 / _patPrev.u.Float64;
			return dPerc >= _spParms.dtLVal.u.Float64 && dPerc <= _spParms.dtRVal.u.Float64;
		}

		// Changed-by-range (percent) primitive compare.
		static bool __stdcall				Cmp_ChangedByRangePercFloat64WithNaN( const CUtilities::MX_DATA_TYPE &_patPrev, const CUtilities::MX_DATA_TYPE &_patCur, const CUtilities::MX_DATA_TYPE &/*_patTest*/, const MX_SEARCH_PARMS &_spParms ) {
			if ( !_patPrev.u.Float64 ) { return std::isnan( _patCur.u.Float64 ); }
			double dPerc = _patCur.u.Float64 / _patPrev.u.Float64;
			return (dPerc >= _spParms.dtLVal.u.Float64 && dPerc <= _spParms.dtRVal.u.Float64) || std::isnan( dPerc );
		}


		// =====================
		// == String compare. ==
		// =====================
		// Compares 2 strings that have been composed, having toupper() and similar conversions already done if necessary, and the final characters to compare are all stored in uint32_t values.
		static bool __stdcall				Cmp_ComposedStringCompare( const MX_STRING_SEARCH_UNPACKED &_ssuLeft,
			const MX_STRING_SEARCH_UNPACKED &_ssuRight,
			size_t _sLen,
			size_t _sIdx,
			uint32_t _ui32Flags, uint32_t _ui32FlagsEx,
			size_t &_sResultLen,
			uint32_t _uiMaxWildcardLen = 1024 );

		// Compares 2 arrays of primitives.
		template <typename _tType>
		static bool __stdcall				Cmp_DataArrayCompare( const void * _pvLeft, size_t _sLeftSizeInCharacters,
			const std::vector<uint8_t> &_vMeta,
			const void * _pvRight, size_t _sRightSizeInBytes,
			size_t &_sResultLen,
			uint32_t _uiMaxWildcardLen = 10240 );

		// Compares 2 arrays of primitives of various types.
		static bool __stdcall				Cmp_DataArrayCompare_MultiType( const void * _pvLeft,
			const std::vector<uint8_t> &_vMeta,
			const void * _pvRight, size_t _sRightSizeInBytes,
			size_t &_sResultLen,
			uint32_t _uiMaxWildcardLen = 10240 );


		// Checks for their being a left whole-word boundary at a given index into an array.
		static bool __stdcall				String_WholeWordLeft( const MX_STRING_SEARCH_UNPACKED &_ssuRight, size_t _sIdx, uint32_t _ui32Align );

		// Checks for their being a left whole-word boundary at a given index into an array.
		static bool __stdcall				String_WholeWordRight( const MX_STRING_SEARCH_UNPACKED &_ssuRight, size_t _sIdx, size_t _sNextChar );
	};

}	// namespace mx
