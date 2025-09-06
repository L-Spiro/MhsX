#include "MXFoundAddressesListView.h"
#include "../MainWindow/MXMhsMainWindow.h"
#include "../Search/MXSearchResultRef.h"
#include <Float16/EEFloat16.h>

#include <strsafe.h>


namespace mx {

	CFoundAddressesListView::CFoundAddressesListView( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CListView( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_pmmwMhsWindow( reinterpret_cast<CMhsMainWindow *>(_ui64Data) ) {
	}

	// == Functions.
	// Requesting information (notification responder).
	BOOL CFoundAddressesListView::GetDispInfoNotify( NMLVDISPINFOW * _plvdiInfo ) {
		if ( m_pmmwMhsWindow ) {
			INT iMask = _plvdiInfo->item.mask;
			//_plvdiInfo->item.mask |= LVIF_DI_SETITEM;

			if ( iMask & LVIF_TEXT ) {
				if ( _plvdiInfo->item.pszText && _plvdiInfo->item.cchTextMax > 0 ) {
					uint64_t ui64Address;
					const uint8_t * pui8Data;
					CSearcher::MX_SEARCH_LOCK slSearchLock( &m_pmmwMhsWindow->MemHack()->Searcher() );
					const CSearchResultBase * psrbResults = m_pmmwMhsWindow->MemHack()->Searcher().SearchResults();
					if ( psrbResults ) {
						psrbResults->Lock();
						if ( psrbResults->GetResultFast( _plvdiInfo->item.iItem, ui64Address, pui8Data ) ) {
							if ( _plvdiInfo->item.iSubItem == 0 ) {
								auto iTexLen = _plvdiInfo->item.cchTextMax;
								// Address.
								if ( m_pmmwMhsWindow->MemHack()->Process().Is32Bit() ) {
									auto iPrinted = std::swprintf( _plvdiInfo->item.pszText, iTexLen, L"%.8I64X", ui64Address );
									iTexLen -= iPrinted;
								}
								else {
									auto iPrinted = std::swprintf( _plvdiInfo->item.pszText, iTexLen, L"%.12I64X", ui64Address );
									iTexLen -= iPrinted;
								}
								if ( m_pmmwMhsWindow->MemHack()->Searcher().LastSearchParms().stType == CUtilities::MX_ST_STRING_SEARCH &&
									m_pmmwMhsWindow->MemHack()->Searcher().LastSearchParms().sstSearchType == CUtilities::MX_SST_UTFALL ) {

									const CSearchResultRef::MX_ADDRESS_REF & arRef = (*reinterpret_cast<const CSearchResultRef::MX_ADDRESS_REF *>(pui8Data));
									if ( iTexLen ) {
										::wcsncat_s( _plvdiInfo->item.pszText, iTexLen, L" ", iTexLen - 1 );
										--iTexLen;
										if ( iTexLen ) {
											CSecureWString swsTmp;

											switch ( arRef.uiData ) {
												case CUtilities::MX_SST_UTF8 : {
													swsTmp = _DEC_WS_0E813C50_UTF_8;
													break;
												}
												case CUtilities::MX_SST_UTF16 : {
													swsTmp = _DEC_WS_A71F1195_UTF_16.c_str();
													break;
												}
												case CUtilities::MX_SST_UTF32 : {
													swsTmp = _DEC_WS_9244B70E_UTF_32.c_str();
													break;
												}
												case CUtilities::MX_SST_UTF16_BE : {
													swsTmp = _DEC_WS_26FC5333_UTF_16_BE.c_str();
													break;
												}
												case CUtilities::MX_SST_UTF32_BE : {
													swsTmp = _DEC_WS_D35E9704_UTF_32_BE.c_str();
													break;
												}
											}
											::wcsncat( _plvdiInfo->item.pszText, swsTmp.c_str(), iTexLen - 1 );
										}
									}
								}
							}
							else if ( _plvdiInfo->item.iSubItem == 1 ) {
								// Value.
								switch ( m_pmmwMhsWindow->MemHack()->Searcher().LastSearchParms().stType ) {
									case CUtilities::MX_ST_DATATYPE_SEARCH : {
										GetDataTypeSearchValue( _plvdiInfo->item.pszText, _plvdiInfo->item.cchTextMax,
											m_pmmwMhsWindow->MemHack()->Searcher().LastSearchParms().dtLVal.dtType,
											pui8Data );
										break;
									}
									case CUtilities::MX_ST_STRING_SEARCH : {
										GetStringSearchValue( _plvdiInfo->item.pszText, _plvdiInfo->item.cchTextMax, ui64Address, pui8Data );
										break;
									}
									case CUtilities::MX_ST_EXP_SEARCH : {
										if ( m_pmmwMhsWindow->MemHack()->Searcher().LastSearchParms().esqmExpSearchType == CUtilities::MX_ESQM_VALUE ) {
											GetDataTypeSearchValue( _plvdiInfo->item.pszText, _plvdiInfo->item.cchTextMax,
											m_pmmwMhsWindow->MemHack()->Searcher().LastSearchParms().dtLVal.dtType,
											pui8Data );
										}
										else if ( m_pmmwMhsWindow->MemHack()->Searcher().LastSearchParms().ui32ExpSearchSize == 0 ) {
											GetExpressionSearchValue( _plvdiInfo->item.pszText, _plvdiInfo->item.cchTextMax,
											ui64Address, pui8Data );
										}
										else {
											GetExpressionSearchValueFixed( _plvdiInfo->item.pszText, _plvdiInfo->item.cchTextMax,
											ui64Address, pui8Data );
										}
										break;
									}
									case CUtilities::MX_ST_POINTER_SEARCH : {
										break;
									}
									case CUtilities::MX_ST_GROUP_SEARCH : {
										break;
									}
								}
							}
							else if ( _plvdiInfo->item.iSubItem == 2 ) {
								switch ( m_pmmwMhsWindow->MemHack()->Searcher().LastSearchParms().stType ) {
									case CUtilities::MX_ST_DATATYPE_SEARCH : {
										// Current Value.
										uint8_t ui8CurVal[64];
										DWORD dwSize = CUtilities::DataTypeSize( m_pmmwMhsWindow->MemHack()->Searcher().LastSearchParms().dtLVal.dtType );
										size_t sOffset;
										if ( !m_pmmwMhsWindow->MemHack()->Process().ReadProcessMemory_PreProcessed( ui64Address,
											ui8CurVal, size_t( dwSize ), sOffset, m_pmmwMhsWindow->MemHack()->Searcher().LastSearchParms().bsByteSwapping ) ) {
											std::swprintf( _plvdiInfo->item.pszText, _plvdiInfo->item.cchTextMax, L"%s", L"N/A" );
										}
										else {
											GetDataTypeSearchValue( _plvdiInfo->item.pszText, _plvdiInfo->item.cchTextMax,
												m_pmmwMhsWindow->MemHack()->Searcher().LastSearchParms().dtLVal.dtType,
												&ui8CurVal[sOffset] );
										}
										break;
									}
									case CUtilities::MX_ST_EXP_SEARCH : {
										if ( m_pmmwMhsWindow->MemHack()->Searcher().LastSearchParms().esqmExpSearchType == CUtilities::MX_ESQM_VALUE ) {
											// Current Value.
											uint8_t ui8CurVal[64];
											DWORD dwSize = CUtilities::DataTypeSize( m_pmmwMhsWindow->MemHack()->Searcher().LastSearchParms().dtLVal.dtType );
											size_t sOffset;
											if ( !m_pmmwMhsWindow->MemHack()->Process().ReadProcessMemory_PreProcessed( ui64Address,
												ui8CurVal, size_t( dwSize ), sOffset, m_pmmwMhsWindow->MemHack()->Searcher().LastSearchParms().bsByteSwapping ) ) {
												std::swprintf( _plvdiInfo->item.pszText, _plvdiInfo->item.cchTextMax, L"%s", L"N/A" );
											}
											else {
												GetDataTypeSearchValue( _plvdiInfo->item.pszText, _plvdiInfo->item.cchTextMax,
													m_pmmwMhsWindow->MemHack()->Searcher().LastSearchParms().dtLVal.dtType,
													&ui8CurVal[sOffset] );
											}
										}
										else if ( m_pmmwMhsWindow->MemHack()->Searcher().LastSearchParms().ui32ExpSearchSize != 0 ) {
											std::vector<uint8_t> vData;

											size_t sOffset;
											if ( !m_pmmwMhsWindow->MemHack()->Process().ReadProcessMemory_PreProcessed( ui64Address,
												vData, size_t( m_pmmwMhsWindow->MemHack()->Searcher().LastSearchParms().ui32ExpSearchSize ), sOffset, m_pmmwMhsWindow->MemHack()->Searcher().LastSearchParms().bsByteSwapping ) ) {
												std::swprintf( _plvdiInfo->item.pszText, _plvdiInfo->item.cchTextMax, L"%s", L"N/A" );
											}
											else {
												GetExpressionSearchValueFixed( _plvdiInfo->item.pszText, _plvdiInfo->item.cchTextMax,
													ui64Address,
													&vData[sOffset] );
											}
										}
										break;
									}
								}
							}
						}
						else {
							std::swprintf( _plvdiInfo->item.pszText, _plvdiInfo->item.cchTextMax, L"%s", L"---" );
						}
						psrbResults->Unlock();
					}
				}
			}
		}
		return TRUE;
	}

	// Gets the text for an address line.
	bool CFoundAddressesListView::GetAddressText( std::wstring &_wsResult, int _iIndex ) const {
		NMLVDISPINFOW dvInfo = { 0 };
		dvInfo.item.mask = LVIF_TEXT;
		dvInfo.item.iItem = _iIndex;
		dvInfo.item.iSubItem = 0;
		WCHAR wszBuffer[32];
		dvInfo.item.pszText = wszBuffer;
		dvInfo.item.cchTextMax = MX_ELEMENTS( wszBuffer );
		if ( const_cast<CFoundAddressesListView *>(this)->GetDispInfoNotify( &dvInfo ) ) {
			_wsResult.append( wszBuffer );
			return true;
		}
		return false;
	}

	// Gets the value text for a line.
	bool CFoundAddressesListView::GetValueText( std::wstring &_wsResult, int _iIndex ) const {
		NMLVDISPINFOW dvInfo = { 0 };
		dvInfo.item.mask = LVIF_TEXT;
		dvInfo.item.iItem = _iIndex;
		dvInfo.item.iSubItem = 1;
		std::vector<WCHAR> vBuffer;
		vBuffer.resize( 1024 * 64 );
		dvInfo.item.pszText = &vBuffer[0];
		dvInfo.item.cchTextMax = vBuffer.size();
		if ( const_cast<CFoundAddressesListView *>(this)->GetDispInfoNotify( &dvInfo ) ) {
			vBuffer[vBuffer.size()-1] = L'\0';
			_wsResult.append( vBuffer.data() );
			return true;
		}
		return false;
	}

	// Gets the other text for a line.
	bool CFoundAddressesListView::GetOtherText( std::wstring &_wsResult, int _iIndex, int _iSub ) const {
		NMLVDISPINFOW dvInfo = { 0 };
		dvInfo.item.mask = LVIF_TEXT;
		dvInfo.item.iItem = _iIndex;
		dvInfo.item.iSubItem = _iSub;
		std::vector<WCHAR> vBuffer;
		vBuffer.resize( 1024 * 64 );
		dvInfo.item.pszText = &vBuffer[0];
		dvInfo.item.cchTextMax = vBuffer.size();
		if ( const_cast<CFoundAddressesListView *>(this)->GetDispInfoNotify( &dvInfo ) ) {
			vBuffer[vBuffer.size()-1] = L'\0';
			_wsResult.append( vBuffer.data() );
			return true;
		}
		return false;
	}

	// Gets the display text for a data-type search result.
	bool CFoundAddressesListView::GetDataTypeSearchValue( LPWSTR _pwStr, int _iMaxLen, uint32_t _ui32Type, const uint8_t * _pui8Value ) const {
		switch ( _ui32Type ) {
			case CUtilities::MX_DT_INT8 : {
				char szBuffer[32];
				int32_t i32Temp = (*reinterpret_cast<const int8_t *>(_pui8Value));
				CUtilities::ByteToCString( (*_pui8Value), szBuffer, FALSE, FALSE );
				int iOffset = std::swprintf( _pwStr, _iMaxLen, L"%d (%S)", i32Temp, szBuffer );
				break;
			}
			case CUtilities::MX_DT_UINT8 : {
				std::swprintf( _pwStr, _iMaxLen, L"%u", (*_pui8Value) );
				break;
			}
			case CUtilities::MX_DT_INT16 : {
				std::swprintf( _pwStr, _iMaxLen, L"%d", (*reinterpret_cast<const int16_t *>(_pui8Value)) );
				break;
			}
			case CUtilities::MX_DT_UINT16 : {
				std::swprintf( _pwStr, _iMaxLen, L"%u", (*reinterpret_cast<const uint16_t *>(_pui8Value)) );
				break;
			}
			case CUtilities::MX_DT_INT32 : {
				std::swprintf( _pwStr, _iMaxLen, L"%d", (*reinterpret_cast<const int32_t *>(_pui8Value)) );
				break;
			}
			case CUtilities::MX_DT_UINT32 : {
				std::swprintf( _pwStr, _iMaxLen, L"%u", (*reinterpret_cast<const uint32_t *>(_pui8Value)) );
				break;
			}
			case CUtilities::MX_DT_INT64 : {
				std::swprintf( _pwStr, _iMaxLen, L"%I64d", (*reinterpret_cast<const int64_t *>(_pui8Value)) );
				break;
			}
			case CUtilities::MX_DT_UINT64 : {
				std::swprintf( _pwStr, _iMaxLen, L"%I64u", (*reinterpret_cast<const uint64_t *>(_pui8Value)) );
				break;
			}
			case CUtilities::MX_DT_FLOAT16 : {
				std::swprintf( _pwStr, _iMaxLen, L"%.9g", ee::CFloat16( _pui8Value ).Value() );
				break;
			}
			case CUtilities::MX_DT_FLOAT : {
				std::swprintf( _pwStr, _iMaxLen, L"%.13g", (*reinterpret_cast<const float *>(_pui8Value)) );
				break;
			}
			case CUtilities::MX_DT_DOUBLE : {
				std::swprintf( _pwStr, _iMaxLen, L"%.27g", (*reinterpret_cast<const double *>(_pui8Value)) );
				break;
			}
			default : {
				std::swprintf( _pwStr, _iMaxLen, L"N/A" );
				return false;
			}
		}
		return true;
	}

	// Gets the display test for a string search result.
	bool CFoundAddressesListView::GetStringSearchValue( LPWSTR _pwStr, int _iMaxLen, uint64_t _uiAddress, const uint8_t * _pui8Value ) const {
		const CSearchResultRef::MX_ADDRESS_REF & arRef = (*reinterpret_cast<const CSearchResultRef::MX_ADDRESS_REF *>(_pui8Value));
		if ( !arRef.uiSize ) { return true; }

		// Current Value.
		std::vector<uint8_t> vData;

		size_t sOffset;
		if ( !m_pmmwMhsWindow->MemHack()->Process().ReadProcessMemory_PreProcessed( _uiAddress,
			vData, size_t( arRef.uiSize ), sOffset, m_pmmwMhsWindow->MemHack()->Searcher().LastSearchParms().bsByteSwapping ) ) {
			std::swprintf( _pwStr, _iMaxLen, L"%s", L"N/A" );
			return false;
		}
		int iIdx = 0;
#define MX_PRINT_CHAR( X )				if ( iIdx < _iMaxLen ) { _pwStr[iIdx++] = X; }			\
										else { _pwStr[_iMaxLen-1] = L'\0'; return true; }


		CUtilities::MX_STRING_SEARCH_TYPES sstMainSearchType = static_cast<CUtilities::MX_STRING_SEARCH_TYPES>(arRef.uiData & 0xFF);
		UINT uiCodePage = CCodePages::GetSystemDefaultAnsiCodePage();
		if ( sstMainSearchType == CUtilities::MX_SST_REGEX ) {
			CUtilities::MX_REGEX_ENCODING reEncoding = static_cast<CUtilities::MX_REGEX_ENCODING>((arRef.uiData >> 8) & 0xFF);
			uiCodePage = CUtilities::RegexCodePageToCodePage( reEncoding );
			//switch ( reEncoding ) {
			//	case CUtilities::MX_RE_UTF16_LE : {
			//		// Handle below.
			//		sstMainSearchType = CUtilities::MX_SST_UTF16;
			//		break;
			//	}
			//	case CUtilities::MX_RE_UTF32_LE : {
			//		// Handle below.
			//		sstMainSearchType = CUtilities::MX_SST_UTF32;
			//		break;
			//	}
			//	case CUtilities::MX_RE_UTF16_BE : {
			//		// Byteswap and then handle normally.
			//		wchar_t * pwcByteSwapMe = reinterpret_cast<wchar_t *>(&vData.data()[sOffset]);
			//		size_t sLen = arRef.uiSize / sizeof( (*pwcByteSwapMe) );
			//		for ( size_t I = 0; I < sLen; ++I ) {
			//			pwcByteSwapMe[I] = ::_byteswap_ushort( pwcByteSwapMe[I] );
			//		}
			//		// Handle below.
			//		sstMainSearchType = CUtilities::MX_SST_UTF16;
			//		break;
			//	}
			//	case CUtilities::MX_RE_UTF32_BE : {
			//		// Byteswap and then handle normally.
			//		uint32_t * puiByteSwapMe = reinterpret_cast<uint32_t *>(&vData.data()[sOffset]);
			//		size_t sLen = arRef.uiSize / sizeof( (*puiByteSwapMe) );
			//		for ( size_t I = 0; I < sLen; ++I ) {
			//			puiByteSwapMe[I] = ::_byteswap_ulong( puiByteSwapMe[I] );
			//		}
			//		// Handle below.
			//		sstMainSearchType = CUtilities::MX_SST_UTF32;
			//		break;
			//	}
			//}
		}
		else if ( sstMainSearchType == CUtilities::MX_SST_UTF8 ) {
			uiCodePage = CCodePages::MX_utf_8;
		}
		else if ( sstMainSearchType == CUtilities::MX_SST_UTF16 ) {
			uiCodePage = CCodePages::MX_utf_16;
		}
		else if ( sstMainSearchType == CUtilities::MX_SST_UTF16_BE ) {
			uiCodePage = CCodePages::MX_utf_16BE;
			// Byteswap and then handle normally.
			//wchar_t * pwcByteSwapMe = reinterpret_cast<wchar_t *>(&vData.data()[sOffset]);
			//size_t sLen = arRef.uiSize / sizeof( (*pwcByteSwapMe) );
			//for ( size_t I = 0; I < sLen; ++I ) {
			//	pwcByteSwapMe[I] = ::_byteswap_ushort( pwcByteSwapMe[I] );
			//}
			//// Handle below.
			//sstMainSearchType = CUtilities::MX_SST_UTF16;
		}
		else if ( sstMainSearchType == CUtilities::MX_SST_UTF32 ) {
			uiCodePage = CCodePages::MX_utf_32;
		}
		else if ( sstMainSearchType == CUtilities::MX_SST_UTF32_BE ) {
			uiCodePage = CCodePages::MX_utf_32BE;
			// Byteswap and then handle normally.
			//uint32_t * puiByteSwapMe = reinterpret_cast<uint32_t *>(&vData.data()[sOffset]);
			//size_t sLen = arRef.uiSize / sizeof( (*puiByteSwapMe) );
			//for ( size_t I = 0; I < sLen; ++I ) {
			//	puiByteSwapMe[I] = ::_byteswap_ulong( puiByteSwapMe[I] );
			//}
			//// Handle below.
			//sstMainSearchType = CUtilities::MX_SST_UTF32;
		}
		else if ( sstMainSearchType == CUtilities::MX_SST_UTFALL ) {
			uiCodePage = arRef.uiData;
		}

		switch ( sstMainSearchType ) {
			//case CUtilities::MX_SST_ASCII : {
			//	CSecureString sTmp;
			//	CUtilities::BytesToCString( &vData.data()[sOffset], arRef.uiSize, sTmp );
			//	for ( size_t I = 0; I < sTmp.size(); ++I ) {
			//		MX_PRINT_CHAR( static_cast<uint8_t>(sTmp[I]) );
			//	}
			//	if ( iIdx < _iMaxLen ) { _pwStr[iIdx] = L'\0'; }
			//	return true;
			//}
			//case CUtilities::MX_SST_REGEX : {
			//	switch ( (arRef.uiData >> 8) & 0xFF ) {
			//		case CUtilities::MX_RE_UTF16_LE : {
			//			// Regular UTF-16 on Windows.

			//		}
			//	}
			//	// Fall-through.
			//}																	MX_FALLTHROUGH
			//case CUtilities::MX_SST_UTF8 : {
			//	CSecureWString swsTemp = ee::CExpEval::StringToWString( reinterpret_cast<const char *>(&vData.data()[sOffset]), arRef.uiSize );
			//	swsTemp = CUtilities::EscapeUnprintable( swsTemp, true, false );
			//	for ( size_t I = 0; I < swsTemp.size(); ++I ) {
			//		if ( !swsTemp[I] ) {
			//			MX_PRINT_CHAR( L'\\' );
			//			MX_PRINT_CHAR( L'0' );
			//		}
			//		else {
			//			MX_PRINT_CHAR( swsTemp[I] );
			//		}
			//	}
			//	if ( iIdx < _iMaxLen ) { _pwStr[iIdx] = L'\0'; }
			//	return true;
			//}
			//case CUtilities::MX_SST_UTF16 : {
			//	CSecureWString swsTemp( reinterpret_cast<const wchar_t *>(&vData.data()[sOffset]), arRef.uiSize / sizeof( wchar_t ) );
			//	swsTemp = CUtilities::EscapeUnprintable( swsTemp, true, false );
			//	for ( size_t I = 0; I < swsTemp.size(); ++I ) {
			//		if ( !swsTemp[I] ) {
			//			MX_PRINT_CHAR( L'\\' );
			//			MX_PRINT_CHAR( L'0' );
			//		}
			//		else {
			//			MX_PRINT_CHAR( swsTemp[I] );
			//		}
			//	}
			//	if ( iIdx < _iMaxLen ) { _pwStr[iIdx] = L'\0'; }
			//	return true;
			//}
			//case CUtilities::MX_SST_UTF32 : {
			//	CSecureWString swsTemp = ee::CExpEval::Utf32StringToWString( reinterpret_cast<const uint32_t *>(&vData.data()[sOffset]), arRef.uiSize / sizeof( uint32_t ) );
			//	swsTemp = CUtilities::EscapeUnprintable( swsTemp, true, false );
			//	for ( size_t I = 0; I < swsTemp.size(); ++I ) {
			//		if ( !swsTemp[I] ) {
			//			MX_PRINT_CHAR( L'\\' );
			//			MX_PRINT_CHAR( L'0' );
			//		}
			//		else {
			//			MX_PRINT_CHAR( swsTemp[I] );
			//		}
			//	}
			//	if ( iIdx < _iMaxLen ) { _pwStr[iIdx] = L'\0'; }
			//	return true;
			//}
			case CUtilities::MX_SST_RAW : {
				CUtilities::MX_DATA_TYPES dtDataType = static_cast<CUtilities::MX_DATA_TYPES>(arRef.uiData >> 8);
				if ( dtDataType == CUtilities::MX_DT_VOID ) {
					CSecureString ssTmp;
					CUtilities::BytesToHexWithSpaces( &vData.data()[sOffset], arRef.uiSize, ssTmp );
					/*CSecureWString swsTemp = CUtilities::PrimitiveArrayToStringW( vData.data(), arRef.uiSize, 
						CUtilities::MX_DT_UINT8, 0,
						false ? -7 : -11 );*/
					for ( size_t I = 0; I < ssTmp.size(); ++I ) {
						MX_PRINT_CHAR( ssTmp[I] );
					}
					if ( iIdx < _iMaxLen ) { _pwStr[iIdx] = L'\0'; }
				}
				else {
					int iSig = -MX_FLOAT16_SIG_DIG;
					if ( dtDataType == CUtilities::MX_DT_FLOAT ) {
						iSig = -MX_FLOAT32_SIG_DIG;
					}
					else if ( dtDataType == CUtilities::MX_DT_FLOAT ) {
						iSig = -MX_FLOAT64_SIG_DIG;
					}
					CSecureWString swsTemp = CUtilities::PrimitiveArrayToStringW( &vData.data()[sOffset], arRef.uiSize, 
						dtDataType, 0,
						iSig );
					for ( size_t I = 0; I < swsTemp.size(); ++I ) {
						MX_PRINT_CHAR( swsTemp[I] );
					}
					if ( iIdx < _iMaxLen ) { _pwStr[iIdx] = L'\0'; }
				}
				return true;
			}
			default : {
				CSecureString ssTmp;
				ssTmp.assign( reinterpret_cast<const char *>(&vData.data()[sOffset]),
				reinterpret_cast<const char *>(&vData.data()[sOffset]) + arRef.uiSize );
				auto swsResult = CUtilities::EscapeUnprintable( CUtilities::MultiByteToWideChar( uiCodePage, 0, ssTmp ), true, false );
				
				for ( size_t I = 0; I < swsResult.size(); ++I ) {
					MX_PRINT_CHAR( swsResult[I] );
				}
				if ( iIdx < _iMaxLen ) { _pwStr[iIdx] = L'\0'; }
				return true;
			}
		}
#undef MX_PRINT_CHAR
		return false;
	}

	// Gets the display test for an expression search result.
	bool CFoundAddressesListView::GetExpressionSearchValue( LPWSTR _pwStr, int _iMaxLen, uint64_t _uiAddress, const uint8_t * _pui8Value ) const {
		const CSearchResultRef::MX_ADDRESS_REF & arRef = (*reinterpret_cast<const CSearchResultRef::MX_ADDRESS_REF *>(_pui8Value));
		if ( !arRef.uiSize ) { return true; }
		uint32_t uiSize = std::min( arRef.uiSize, static_cast<uint32_t>(_iMaxLen / 3 + 1) );

		// Current Value.
		std::vector<uint8_t> vData;

		size_t sOffset;
		if ( !m_pmmwMhsWindow->MemHack()->Process().ReadProcessMemory_PreProcessed( _uiAddress,
			vData, size_t( uiSize ), sOffset, m_pmmwMhsWindow->MemHack()->Searcher().LastSearchParms().bsByteSwapping ) ) {
			std::swprintf( _pwStr, _iMaxLen, L"%s", L"N/A" );
			return false;
		}
		int iIdx = 0;
#define MX_PRINT_CHAR( X )				if ( iIdx < _iMaxLen ) { _pwStr[iIdx++] = X; }			\
										else { _pwStr[_iMaxLen-1] = L'\0'; return true; }

		CSecureString ssTmp;
		CUtilities::BytesToHexWithSpaces( &vData.data()[sOffset], uiSize, ssTmp );
		for ( size_t I = 0; I < ssTmp.size(); ++I ) {
			MX_PRINT_CHAR( ssTmp[I] );
		}
		if ( iIdx < _iMaxLen ) { _pwStr[iIdx] = L'\0'; }
		else {
			_pwStr[_iMaxLen-1] = L'\0';
		}

#undef MX_PRINT_CHAR
		return true;
	}

	// Gets the display test for an expression search result.
	bool CFoundAddressesListView::GetExpressionSearchValueFixed( LPWSTR _pwStr, int _iMaxLen, uint64_t _uiAddress, const uint8_t * _pui8Value ) const {
		// Current Value.

int iIdx = 0;
#define MX_PRINT_CHAR( X )				if ( iIdx < _iMaxLen ) { _pwStr[iIdx++] = X; }			\
										else { _pwStr[_iMaxLen-1] = L'\0'; return true; }

		CSecureString ssTmp;
		CUtilities::BytesToHexWithSpaces( _pui8Value, m_pmmwMhsWindow->MemHack()->Searcher().LastSearchParms().ui32ExpSearchSize, ssTmp );
		for ( size_t I = 0; I < ssTmp.size(); ++I ) {
			MX_PRINT_CHAR( ssTmp[I] );
		}
		if ( iIdx < _iMaxLen ) { _pwStr[iIdx] = L'\0'; }
		else { _pwStr[_iMaxLen-1] = L'\0'; }

#undef MX_PRINT_CHAR
		return true;
	}

	/**
	 * The WM_NOTIFY -> NM_CUSTOMDRAW -> CDDS_ITEMPREPAINT handler.
	 *
	 * \param _lpcdParm The notifacation structure.
	 * \return Returns an LSW_HANDLED code.
	 */
	DWORD CFoundAddressesListView::Notify_CustomDraw_ItemPrePaint( LPNMLVCUSTOMDRAW _lpcdParm ) {
		{
			uint64_t ui64Address;
			const uint8_t * pui8Data;
			CSearcher::MX_SEARCH_LOCK slSearchLock( &m_pmmwMhsWindow->MemHack()->Searcher() );
			const CSearchResultBase * psrbResults = m_pmmwMhsWindow->MemHack()->Searcher().SearchResults();
			if ( psrbResults ) {
				psrbResults->Lock();
				if ( psrbResults->GetResultFast( _lpcdParm->nmcd.dwItemSpec, ui64Address, pui8Data ) ) {
					psrbResults->Unlock();
					bool bStatic = m_pmmwMhsWindow->MemHack()->Process().AddressIsInModule( ui64Address );
					if ( bStatic ) {
						_lpcdParm->clrText = ::GetSysColor( COLOR_WINDOWTEXT );
						_lpcdParm->clrTextBk = (_lpcdParm->nmcd.dwItemSpec & 1) ? RGB( 0x83, 0xA5, 0xCB ) : RGB( 0x70, 0x8E, 0xAE );
						return CDRF_NEWFONT;
					}
				}
				else { psrbResults->Unlock(); }
			}
		}
		return lsw::CListView::Notify_CustomDraw_ItemPrePaint( _lpcdParm );
	}

}	// namespace mx
