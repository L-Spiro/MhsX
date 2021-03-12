#pragma once
#include "../MXMhsX.h"
#include "../Float16/MXFloat16.h"
#include "../MemHack/MXProcess.h"
#include "../Utilities/MXUtilities.h"
#include "MXAddressChunkList.h"
#include "MXSearchResultBase.h"
#include <cassert>


namespace mx {

	class CSearcher {
	public :
		CSearcher( CProcess * _ppProcess ) :
			m_ppProcess( _ppProcess ),
			m_psrbSearchResults( nullptr ),
			m_psrbCurTmpSearchResults( nullptr ),
			m_i32LockCount( 0 ) {
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
			bool							bStaticOnly;
			DWORD							dwTypes;
		};

		// Search parameters.
		struct MX_SEARCH_PARMS {
			uint64_t						ui64AddressFrom;
			uint64_t						ui64AddressTo;
			uint64_t						ui64MaxChunkLen;
			DWORD							dwRegions;

			// Data-type search.
			CUtilities::MX_DATA_TYPE		dtLVal;
			CUtilities::MX_DATA_TYPE		dtRVal;
			CUtilities::MX_SEARCH_TYPES		stType;
			int32_t							i32EvalType;

			// String search.
			std::vector<uint8_t>			vStringSearchValue;
			std::vector<uint8_t>			vStringMetaData;
			CUtilities::MX_WILDCARD_USAGE	wuWildcardUsage;
			CUtilities::MX_STRING_SEARCH_TYPES
											sstSearchType;
			uint32_t						ui32StringSearchFlags;
			uint32_t						ui32StringSearchFlagsEx;
			uint32_t						ui32RegexFlags;
			CUtilities::MX_DATA_TYPES		dtStringCharType;
			CUtilities::MX_REGEX_FLAVORS	rfRegexFlavor;
			CUtilities::MX_REGEX_ENCODING	reRegexEncoding;

			// Expression search.
			CSecureString					ssExpression;
			CUtilities::MX_EXP_SEARCH_QQ_MEANING
											esqmExpSearchType;
			uint32_t						ui32ExpSearchSize;
			
			// Epsilon settings.
			double							dEpsilonValue;
			bool							bUseEpsilon;
			bool							bUseSmartEpsilon;

			// Misc.
			bool							bAligned;
			bool							bInvertResults;
			int32_t							ui32Alignment;
			CUtilities::MX_BYTESWAP			bsByteSwapping;

			// System settings & works.
			bool *							pbAbort;
			int								iThreadPriority;
			bool							bPause;
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
		bool								GenerateChunks( CAddressChunkList &_aclList, const MX_SEARCH_PARMS &_spParms,
			uint64_t _ui64MaxLen, uint64_t _ui64Start = 0, uint64_t _ui64End = static_cast<uint64_t>(-16) );

		// Searches the target process.
		bool								Search( const MX_SEARCH_PARMS &_spParms, HWND _hProgressUpdate, uint64_t * _pui64TotalFound );

		// Locks for reading a value quickly.  Unlock when done with the returned pointer.
		virtual void						Lock() const { m_csCrit.EnterCriticalSection(); ++m_i32LockCount; }

		// Gets the search results.  Should be called between Lock() and Unlock().
		const CSearchResultBase *			SearchResults() const { return m_i32LockCount ? m_psrbSearchResults : nullptr; }

		// Gets the total number of results.  Should be called between Lock() and Unlock().
		uint64_t							TotalResults() const { return SearchResults() ? SearchResults()->TotalResults() : 0; }

		// Unlocks after done with the buffer returned by GetResultFast().
		virtual void						Unlock() const { --m_i32LockCount; assert( m_i32LockCount >= 0 ); m_csCrit.LeaveCriticalSection(); }

		// Gets the search time in microseconds.
		uint64_t							SearchTime() const { return m_ui64LastSearchTime; }

		// Gets the last search type.
		CUtilities::MX_SEARCH_TYPES			LastSearchType() const { return m_spLastSearchParms.stType; }

		// Gets the last search parameters.
		const MX_SEARCH_PARMS &				LastSearchParms() const { return m_spLastSearchParms; }

		// Performs pre-processing byteswapping.
		static void							PreprocessByteSwap( void * _pvData, size_t _stLen, const MX_SEARCH_PARMS &_spParms );


	protected :
		// == Types.
		typedef bool (__stdcall *			MX_EXACT_VAL_COMP)( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms );
		typedef bool (__stdcall *			MX_REL_COMP)( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight );
		typedef bool (__stdcall *			MX_RANGE_COMP)( const CUtilities::MX_DATA_TYPE &_patLow, const CUtilities::MX_DATA_TYPE &_patHigh, const CUtilities::MX_DATA_TYPE &_patVal );

		// Primitive-search thread.
		struct MX_PRIM_SEARCH_THREAD {
			CSearchResultBase *				psrbResults;
			MX_SEARCH_PARMS *				pspParms;
			const CAddressChunkList *		paclChunks;
			CProcess *						ppProcess;
			MX_EXACT_VAL_COMP				evcCompFunc;
			MX_REL_COMP						rcRelCompFunc;
			MX_RANGE_COMP					rcRangeCompFunc;
			HWND							hProgressUpdate;
			size_t							stStartIdx;
			size_t							stEndIdx;	// Exclusive.
			uint32_t						ui32Size;
			volatile int32_t				i32Result;	// 0 = pending, 1 = success, -1 = failure.
			volatile LONG *					plProgress;
		};

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
		// Last search parameters.
		MX_SEARCH_PARMS						m_spLastSearchParms;


		// == Functions.
		// Exact-value search on primitives.
		bool								ExactValuePrimitiveSearch( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate );

		// Greater-than serach on primitives.
		bool								GreaterThanPrimitiveSearch( MX_SEARCH_PARMS &_spParms, const CAddressChunkList &_aclChunks, HWND _hProgressUpdate );

		// Less-than serach on primitives.
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

		// Gets the alignment for a primitive type.
		bool								PrimitiveAlignment( MX_SEARCH_PARMS &_spParms, CUtilities::MX_DATA_TYPES _dtType, CProcess * _ppProcess );

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
			return std::fabs( _patLeft.u.Float64 - CFloat16( _patRight.u.UInt16 ).Value() ) <= _spParms.dEpsilonValue;
		}

		// Smart 16-bit epsilon compare.
		static bool __stdcall				Cmp_Float16RelativeEpsilon( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms ) {
			CUtilities::MX_DATA_TYPE dtRight;
			dtRight.u.Float64 = CFloat16( _patRight.u.UInt16 ).Value();
			return Cmp_Float64RelativeEpsilon( _patLeft, dtRight, _spParms );
		}

		// Quick 32-bit epsilon compare.
		static bool __stdcall				Cmp_Float32Epsilon( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms ) {
			return ee::Epsilon( _patLeft.u.Float32, _patRight.u.Float32, static_cast<float>(_spParms.dEpsilonValue) );
		}

		// Smart 32-bit epsilon compare.
		static bool __stdcall				Cmp_Float32RelativeEpsilon( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms ) {
			return ee::RelativeEpsilon( static_cast<float>(_patLeft.u.Float32),
				static_cast<float>(_patRight.u.Float32),
				static_cast<float>(_spParms.dEpsilonValue) );
		}

		// Quick 64-bit epsilon compare.
		static bool __stdcall				Cmp_Float64Epsilon( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms ) {
			return ee::Epsilon( _patLeft.u.Float64, _patRight.u.Float64, _spParms.dEpsilonValue );
		}

		// Smart 64-bit epsilon compare.
		static bool __stdcall				Cmp_Float64RelativeEpsilon( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms ) {
			return ee::RelativeEpsilon( _patLeft.u.Float64,
				_patRight.u.Float64,
				_spParms.dEpsilonValue );
		}

		// Float compare with NaN results returning true.  This allows the negation of the compare to be used in "Not Equal To" searches.
		static bool __stdcall				Cmp_Float16WithNaN( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms ) {
			double dRight = CFloat16( _patRight.u.UInt16 ).Value();
			return _patLeft.u.Float64 == dRight || std::isnan( dRight );
		}

		// Float compare with NaN results returning true.  This allows the negation of the compare to be used in "Not Equal To" searches.
		static bool __stdcall				Cmp_Float32WithNaN( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms ) { return _patLeft.u.Float32 == _patRight.u.Float32 || std::isnan( _patRight.u.Float32 ); }

		// Float compare with NaN results returning true.  This allows the negation of the compare to be used in "Not Equal To" searches.
		static bool __stdcall				Cmp_Float64WithNaN( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms ) { return _patLeft.u.Float64 == _patRight.u.Float64 || std::isnan( _patRight.u.Float64 ); }

		// Quick 16-bit epsilon compare with NaN results returning true.  This allows the negation of the compare to be used in "Not Equal To" searches.
		static bool __stdcall				Cmp_Float16EpsilonWithNaN( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight, const MX_SEARCH_PARMS &_spParms ) {
			CUtilities::MX_DATA_TYPE dtRight;
			dtRight.u.Float64 = CFloat16( _patRight.u.UInt16 ).Value();
			return Cmp_Float64Epsilon( _patLeft, dtRight, _spParms ) || std::isnan( dtRight.u.Float64 );
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
			dtRight.u.Float64 = CFloat16( _patRight.u.UInt16 ).Value();
			return Cmp_Float64RelativeEpsilon( _patLeft, dtRight, _spParms ) || std::isnan( dtRight.u.Float64 );
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
			return CFloat16( _patRight.u.UInt16 ).Value() > _patLeft.u.Float64;
		}

		// Greater-than primitive compare.
		static bool __stdcall				Cmp_GreaterThanFloat16WithNaN( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) {
			double dVal = CFloat16( _patRight.u.UInt16 ).Value();
			return dVal > _patLeft.u.Float64 || std::isnan( dVal );
		}

		// Greater-than primitive compare.
		static bool __stdcall				Cmp_GreaterThanFloat32( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return _patRight.u.Float32 > _patLeft.u.Float32; }

		// Greater-than primitive compare.
		static bool __stdcall				Cmp_GreaterThanFloat32WithNaN( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return _patRight.u.Float32 > _patLeft.u.Float32 || std::isnan( _patRight.u.Float32 ); }

		// Greater-than primitive compare.
		static bool __stdcall				Cmp_GreaterThanFloat64( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return _patRight.u.Float64 > _patLeft.u.Float64; }

		// Greater-than primitive compare.
		static bool __stdcall				Cmp_GreaterThanFloat64WithNaN( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return _patRight.u.Float64 > _patLeft.u.Float64 || std::isnan( _patRight.u.Float64 ); }

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
			return CFloat16( _patRight.u.UInt16 ).Value() < _patLeft.u.Float64;
		}

		// Less-than primitive compare.
		static bool __stdcall				Cmp_LessThanFloat16WithNaN( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) {
			double dVal = CFloat16( _patRight.u.UInt16 ).Value();
			return dVal < _patLeft.u.Float64 || std::isnan( dVal );
		}

		// Less-than primitive compare.
		static bool __stdcall				Cmp_LessThanFloat32( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return _patRight.u.Float32 < _patLeft.u.Float32; }

		// Less-than primitive compare.
		static bool __stdcall				Cmp_LessThanFloat32WithNaN( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return _patRight.u.Float32 < _patLeft.u.Float32 || std::isnan( _patRight.u.Float32 ); }

		// Less-than primitive compare.
		static bool __stdcall				Cmp_LessThanFloat64( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return _patRight.u.Float64 < _patLeft.u.Float64; }

		// Less-than primitive compare.
		static bool __stdcall				Cmp_LessThanFloat64WithNaN( const CUtilities::MX_DATA_TYPE &_patLeft, const CUtilities::MX_DATA_TYPE &_patRight ) { return _patRight.u.Float64 < _patLeft.u.Float64 || std::isnan( _patRight.u.Float64 ); }

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
			double dVal = CFloat16( _patVal.u.UInt16 ).Value();
			return dVal >= _patLow.u.Float64 && dVal <= _patHigh.u.Float64;
		}

		// In-range primitive compare.
		static bool __stdcall				Cmp_InRangeFloat16WithNaN( const CUtilities::MX_DATA_TYPE &_patLow, const CUtilities::MX_DATA_TYPE &_patHigh, const CUtilities::MX_DATA_TYPE &_patVal ) {
			double dVal = CFloat16( _patVal.u.UInt16 ).Value();
			return dVal >= _patLow.u.Float64 && dVal <= _patHigh.u.Float64 || std::isnan( dVal );
		}

		// In-range primitive compare.
		static bool __stdcall				Cmp_InRangeFloat32( const CUtilities::MX_DATA_TYPE &_patLow, const CUtilities::MX_DATA_TYPE &_patHigh, const CUtilities::MX_DATA_TYPE &_patVal ) { return _patVal.u.Float32 >= _patLow.u.Float32 && _patVal.u.Float32 <= _patHigh.u.Float32; }

		// In-range primitive compare.
		static bool __stdcall				Cmp_InRangeFloat32WithNaN( const CUtilities::MX_DATA_TYPE &_patLow, const CUtilities::MX_DATA_TYPE &_patHigh, const CUtilities::MX_DATA_TYPE &_patVal ) { return _patVal.u.Float32 >= _patLow.u.Float32 && _patVal.u.Float32 <= _patHigh.u.Float32 || std::isnan( _patVal.u.Float32 ); }

		// In-range primitive compare.
		static bool __stdcall				Cmp_InRangeFloat64( const CUtilities::MX_DATA_TYPE &_patLow, const CUtilities::MX_DATA_TYPE &_patHigh, const CUtilities::MX_DATA_TYPE &_patVal ) { return _patVal.u.Float64 >= _patLow.u.Float64 && _patVal.u.Float64 <= _patHigh.u.Float64; }

		// In-range primitive compare.
		static bool __stdcall				Cmp_InRangeFloat64WithNaN( const CUtilities::MX_DATA_TYPE &_patLow, const CUtilities::MX_DATA_TYPE &_patHigh, const CUtilities::MX_DATA_TYPE &_patVal ) { return _patVal.u.Float64 >= _patLow.u.Float64 && _patVal.u.Float64 <= _patHigh.u.Float64 || std::isnan( _patVal.u.Float64 ); }

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
