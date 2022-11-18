#include "MXStringTheoryWindow.h"
#include "../CodePages/MXCodePages.h"
#include "../Layouts/MXLayoutMacros.h"
#include "../Layouts/MXStringTheoryLayout.h"
#include "../Layouts/MXStringTheoryOperationsLayout.h"
#include "../Utilities/MXUtilities.h"
#include <Base/LSWBase.h>
#include <Base/LSWWndClassEx.h>
#include <ComboBox/LSWComboBox.h>
#include <Edit/LSWEdit.h>


namespace mx {

	// == Members.
	// The main window class.
	ATOM CStringTheoryWindow::m_aAtom = 0;

	CStringTheoryWindow::CStringTheoryWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout.ChangeClass( reinterpret_cast<LPCWSTR>(m_aAtom) ), _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
	}
	CStringTheoryWindow::~CStringTheoryWindow() {
	}

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CStringTheoryWindow::InitDialog() {
		CComboBox * pcbCombo = static_cast<CComboBox *>(FindChild( CStringTheoryLayout::MX_STW_UNICODE_CHARS_COMBO ));
		if ( pcbCombo ) {
			pcbCombo->InitStorage( ee::CUnicode::TotalNames(), ee::CUnicode::TotalNames() * 17 );
			for ( size_t I = 0; I < ee::CUnicode::TotalNames(); I++ ) {
				std::wstring sTemp;
				ee::CUnicode::GetName( sTemp, I );
				sTemp.push_back( L':' );
				sTemp.push_back( L' ' );
				uint32_t uiLen;
				uint32_t uiConverted = ee::CExpEval::Utf32ToUtf16( ee::CUnicode::GetUnicodeNum( I ), uiLen );
				for ( uint32_t J = 0; J < uiLen; ++J ) {
					sTemp.push_back( static_cast<uint16_t>(uiConverted) );
					uiConverted >>= 16;
				}
				INT iIndex = pcbCombo->AddString( sTemp.c_str() );
				pcbCombo->SetItemData( iIndex, I );
			}
			//pcbCombo->AutoSetMinListWidth();

			//pcbCombo->SetCurSelByItemData( m_swdData.uiUtfHelperIdx );
			pcbCombo->SetCurSelByItemData( 0 );
		}

		pcbCombo = static_cast<CComboBox *>(FindChild( CStringTheoryLayout::MX_STW_ENCODING_COMBO ));
		if ( pcbCombo ) {
			UINT uiSysCodePag = CCodePages::GetSystemDefaultAnsiCodePage();
			std::vector<CCodePages::MX_CODE_PAGE> vCodePages;
			CCodePages::GetSystemCodePages( vCodePages, true );
			pcbCombo->InitStorage( vCodePages.size(), vCodePages.size() * 15 );
			for ( size_t I = 0; I < vCodePages.size(); ++I ) {
				INT iIndex = pcbCombo->AddString( vCodePages[I].swsName.c_str() );
				pcbCombo->SetItemData( iIndex, vCodePages[I].uiCodePage );
			}

			pcbCombo->SetCurSelByItemData( CCodePages::MX_utf_8 );
		}
		pcbCombo = static_cast<CComboBox *>(FindChild( CStringTheoryLayout::MX_STW_INPUT_ENCODING_COMBO ));
		if ( pcbCombo ) {
			UINT uiSysCodePag = CCodePages::GetSystemDefaultAnsiCodePage();
			std::vector<CCodePages::MX_CODE_PAGE> vCodePages;
			CCodePages::GetSystemCodePages( vCodePages, true );
			pcbCombo->InitStorage( vCodePages.size(), vCodePages.size() * 15 );
			for ( size_t I = 0; I < vCodePages.size(); ++I ) {
				INT iIndex = pcbCombo->AddString( vCodePages[I].swsName.c_str() );
				pcbCombo->SetItemData( iIndex, vCodePages[I].uiCodePage );
			}

			pcbCombo->SetCurSelByItemData( CCodePages::MX_utf_8 );
		}

		CEdit * peEdit = static_cast<CEdit *>(FindChild( CStringTheoryLayout::MX_STW_INPUT_UTF8 ));
		if ( peEdit ) { peEdit->LimitText( 0x7FFFFFFFE ); }
		peEdit = static_cast<CEdit *>(FindChild( CStringTheoryLayout::MX_STW_OUTPUT_EDIT ));
		if ( peEdit ) { peEdit->LimitText( 0x7FFFFFFFE ); }

		UpdateDialog();
		return CWidget::InitDialog();
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CStringTheoryWindow::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {
		switch ( _Id ) {
			/*case CNewStringSearchLayout::MX_SSI_CANCEL : {
				return Close();
			}*/
			case CStringTheoryLayout::MX_STW_UNICODE_CHARS_COMBO : {
				if ( /*_wCtrlCode == CBN_EDITCHANGE || _wCtrlCode == CBN_SELCHANGE*/_wCtrlCode == CBN_SELENDOK ) {
					CComboBox * pcbCombo = static_cast<CComboBox *>(FindChild( _Id ));
					if ( pcbCombo ) {
						size_t sIdx = static_cast<size_t>(pcbCombo->GetCurSelItemData());
						std::wstring sTemp;
						CWidget * pwTmp;
						if ( (pwTmp = FindChild( CStringTheoryLayout::MX_STW_UNICODE_4_ESCAPE )) && pwTmp->IsChecked() ) {
							uint32_t ui32Len;
							uint32_t uiConverted = ee::CExpEval::Utf32ToUtf16( ee::CUnicode::GetUnicodeNum( sIdx ), ui32Len );
							for ( uint32_t I = 0; I < ui32Len; ++I ) {
								wchar_t szBuffer[16];
								std::swprintf( szBuffer, _DEC_WS_48C2E559___u__4X.c_str(), uiConverted & 0xFFFF );
								uiConverted >>= 16;
								sTemp += szBuffer;
							}
						}
						else if ( (pwTmp = FindChild( CStringTheoryLayout::MX_STW_UNICODE_8_ESCAPE )) && pwTmp->IsChecked() ) {
							wchar_t szBuffer[16];
							std::swprintf( szBuffer, _DEC_WS_25B68551___U__8X.c_str(), ee::CUnicode::GetUnicodeNum( sIdx ) );
							sTemp = szBuffer;
						}
						else {
							sTemp.push_back( L'\\' );
							sTemp.push_back( L'N' );
							sTemp.push_back( L'{' );
							ee::CUnicode::GetName( sTemp, sIdx );
							sTemp.push_back( L'}' );
						}

						CEdit * peEdit = static_cast<CEdit *>(FindChild( CStringTheoryLayout::MX_STW_INPUT_UTF8 ));
						if ( peEdit ) {
							peEdit->ReplaceSel( true, sTemp.c_str() );
						}
					}
					UpdateDialog();
				}
				break;
			}
			case CStringTheoryLayout::MX_STW_RESOLVE_ESCAPES_CHECK : {}
			case CStringTheoryLayout::MX_STW_RESOLVE_HTML_CHECK : {}
			case CStringTheoryLayout::MX_STW_INPUT_IS_HEX_CHECK : {}
			case CStringTheoryLayout::MX_STW_NO_CASE : {}
			case CStringTheoryLayout::MX_STW_TOUPPER : {}
			case CStringTheoryLayout::MX_STW_TOLOWER : {}
			case CStringTheoryLayout::MX_STW_NO_KANA : {}
			case CStringTheoryLayout::MX_STW_TO_HIRAGANA : {}
			case CStringTheoryLayout::MX_STW_TO_KATAKANA : {}
			case CStringTheoryLayout::MX_STW_FULL_WIDTH : {}
			case CStringTheoryLayout::MX_STW_STD_DIGITS : {}
			case CStringTheoryLayout::MX_STW_NORMALIZE : {}
			case CStringTheoryLayout::MX_STW_NORMALIZEC : {}
			case CStringTheoryLayout::MX_STW_NORMALIZED : {}
			case CStringTheoryLayout::MX_STW_NORMALIZEKC : {}
			case CStringTheoryLayout::MX_STW_NORMALIZEKD : {}
			case CStringTheoryLayout::MX_STW_COMPOSITE : {}
			case CStringTheoryLayout::MX_STW_DEFAULT_CHAR : {}
			case CStringTheoryLayout::MX_STW_DISCARD_NONSPACING : {}
			case CStringTheoryLayout::MX_STW_SEP_CHARS : {}
			case CStringTheoryLayout::MX_STW_NO_BEST_FIT : {}
			case CStringTheoryLayout::MX_STW_ESPACE_ONLY_NUL_AND_BS : {}
			case CStringTheoryLayout::MX_STW_ESPACE_ALL_UNPRINTABLE_AND_BS : {}
			case CStringTheoryLayout::MX_STW_ESPACE_STANDARD : {}
			case CStringTheoryLayout::MX_STW_ESPACE_ONLY_NUL : {}
			case CStringTheoryLayout::MX_STW_ESPACE_ALL_UNPRINTABLE : {}
			case CStringTheoryLayout::MX_STW_ESPACE_ALL : {
				if ( _wCtrlCode == STN_CLICKED ) {
					UpdateDialog();
				}
				break;
			}
			case CStringTheoryLayout::MX_STW_INPUT_UTF8 : {
				if ( _wCtrlCode == EN_CHANGE ) {
					UpdateDialog();
				}
				break;
			}
			case CStringTheoryLayout::MX_STW_INPUT_ENCODING_COMBO : {}
			case CStringTheoryLayout::MX_STW_ENCODING_COMBO : {
				if ( _wCtrlCode == CBN_EDITCHANGE || _wCtrlCode == CBN_SELCHANGE ) {
					UpdateDialog();
				}
				break;
			}
			case CStringTheoryLayout::MX_STW_ADD_MODIFER : {
				if ( CStringTheoryOperationsLayout::CreateOperationsDialog( this, nullptr, 0 ) ) {
				}
				break;
			}
		}
		return CMainWindow::Command( _wCtrlCode, _Id, _pwSrc );
	}

	// WM_GETMINMAXINFO.
	CWidget::LSW_HANDLED CStringTheoryWindow::GetMinMaxInfo( MINMAXINFO * _pmmiInfo ) {
		const CWidget * pwChild = FindChild( CStringTheoryLayout::MX_STW_PREDEF_GROUP );
		if ( pwChild ) {
			LSW_RECT lrRect = pwChild->WindowRect();
			LSW_RECT lrMyClient = ClientRect();
			LSW_RECT lrMyWindow = WindowRect();
			LSW_RECT lrLocalRect = lrRect.ScreenToClient( Wnd() );
			LONG lWidth = (lrLocalRect.right + lrLocalRect.left) +
				(lrMyWindow.Width() - lrMyClient.Width());
			
			_pmmiInfo->ptMinTrackSize.x = lWidth;
			return LSW_H_HANDLED;
		}
		return LSW_H_CONTINUE;
	}
		
	// Sets the text in the main input box.
	void CStringTheoryWindow::SetText( const wchar_t * _pwcTet ) {
		CEdit * peEdit = static_cast<CEdit *>(FindChild( CStringTheoryLayout::MX_STW_INPUT_UTF8 ));
		if ( peEdit ) {
			peEdit->SetTextW( _pwcTet );
		}
	}

	// Updates the dialog.
	void CStringTheoryWindow::UpdateDialog() {
		CWidget * pwTmp;
		CWidget * pwTmpDst;
#define MX_IS_CHECKED( ID )			((pwTmp = FindChild( CStringTheoryLayout::ID )) && pwTmp->IsChecked())

		CComboBox * pcbEncodingCombo = static_cast<CComboBox *>(FindChild( CStringTheoryLayout::MX_STW_ENCODING_COMBO ));
		CComboBox * pcbInputEncodingCombo = static_cast<CComboBox *>(FindChild( CStringTheoryLayout::MX_STW_INPUT_ENCODING_COMBO ));

		CEdit * peEdit = static_cast<CEdit *>(FindChild( CStringTheoryLayout::MX_STW_INPUT_UTF8 ));
		if ( peEdit && pcbEncodingCombo ) {
			// ==================
			// String Extraction.
			// ==================
			bool bIsHex = MX_IS_CHECKED( MX_STW_INPUT_IS_HEX_CHECK );
			UINT uiSrcEncoding = pcbInputEncodingCombo->GetCurSelItemData();

			CSecureString ssUtf8;
			if ( bIsHex ) {
				ssUtf8 = CUtilities::FromHexString( peEdit->GetTextW() );
			}
			else {
				ssUtf8 = ee::CExpEval::ToUtf8( peEdit->GetTextW() );
			}
			

			// ==================
			// Embedded Encoding.
			// ==================
			CSecureString ssResolved;
			if ( MX_IS_CHECKED( MX_STW_RESOLVE_ESCAPES_CHECK ) ) {
				CUtilities::ResolveAllEscapes( ssUtf8, ssResolved, !bIsHex || (uiSrcEncoding == CCodePages::MX_utf_8) );
			}
			else {
				ssResolved = ssUtf8;
			}
			if ( MX_IS_CHECKED( MX_STW_RESOLVE_HTML_CHECK ) ) {
				CSecureString ssTmp = ssResolved;
				ssResolved.SecureClear();
				CUtilities::ResolveAllHtmlXmlEscapes( ssTmp, ssResolved, !bIsHex || (uiSrcEncoding == CCodePages::MX_utf_8) );
			}


			// ===========
			// Processing.
			// ===========
			CSecureWString swsUtf16;
			if ( bIsHex ) {
				swsUtf16 = CUtilities::MultiByteToWideChar( uiSrcEncoding,
					0,
					ssResolved );
			}
			else {
				swsUtf16 = ee::CExpEval::ToUtf16( ssResolved );
			}

			if ( MX_IS_CHECKED( MX_STW_TO_HIRAGANA ) ) {
				swsUtf16 = CUtilities::ToHiragana( swsUtf16 );
			}
			if ( MX_IS_CHECKED( MX_STW_TO_KATAKANA ) ) {
				swsUtf16 = CUtilities::ToKatakana( swsUtf16 );
			}
			if ( MX_IS_CHECKED( MX_STW_FULL_WIDTH ) ) {
				swsUtf16 = CUtilities::ToHalfwidth( swsUtf16 );
			}
			if ( MX_IS_CHECKED( MX_STW_STD_DIGITS ) ) {
				swsUtf16 = CUtilities::ToAsciiNumerics( swsUtf16 );
			}


			bool bNormalized = false;
			if ( MX_IS_CHECKED( MX_STW_NORMALIZEC ) ) {
				swsUtf16 = CUtilities::NormalizeString( NormalizationC, swsUtf16 );
				bNormalized = true;
			}
			else if ( MX_IS_CHECKED( MX_STW_NORMALIZED ) ) {
				swsUtf16 = CUtilities::NormalizeString( NormalizationD, swsUtf16 );
				bNormalized = true;
			}
			else if ( MX_IS_CHECKED( MX_STW_NORMALIZEKC ) ) {
				swsUtf16 = CUtilities::NormalizeString( NormalizationKC, swsUtf16 );
				bNormalized = true;
			}
			else if ( MX_IS_CHECKED( MX_STW_NORMALIZEKD ) ) {
				swsUtf16 = CUtilities::NormalizeString( NormalizationKD, swsUtf16 );
				bNormalized = true;
			}

			if ( MX_IS_CHECKED( MX_STW_TOUPPER ) ) {
				swsUtf16 = CUtilities::ToUpper( swsUtf16 );
			}
			if ( MX_IS_CHECKED( MX_STW_TOLOWER ) ) {
				swsUtf16 = CUtilities::ToLower( swsUtf16 );
			}


			// =========
			// Encoding.
			// =========
			UINT uiEncoding = pcbEncodingCombo->GetCurSelItemData();
			DWORD dwFlags = 0;
			bool bMustBe0 = CUtilities::MultiByteWideChar_FlagsMustBe0( uiEncoding );
			if ( !bMustBe0 ) {
				dwFlags |= MX_IS_CHECKED( MX_STW_COMPOSITE ) ? WC_COMPOSITECHECK : 0;
				if ( dwFlags & WC_COMPOSITECHECK ) {
					dwFlags |= MX_IS_CHECKED( MX_STW_DEFAULT_CHAR ) ? WC_DEFAULTCHAR : 0;
					dwFlags |= MX_IS_CHECKED( MX_STW_DISCARD_NONSPACING ) ? WC_DISCARDNS : 0;
					dwFlags |= MX_IS_CHECKED( MX_STW_SEP_CHARS ) ? WC_SEPCHARS : 0;
				}
				dwFlags |= MX_IS_CHECKED( MX_STW_NO_BEST_FIT ) ? WC_NO_BEST_FIT_CHARS : 0;
			}
			CSecureString ssEncodedFinal = CUtilities::WideCharToMultiByte( uiEncoding, dwFlags,
				swsUtf16, NULL, NULL );
			

			// =========
			// Printing.
			// =========
			DWORD dwError;
			DWORD dwBackToUtf16Flags = 0;
			if ( !bMustBe0 ) {
				dwBackToUtf16Flags = ((dwFlags & WC_COMPOSITECHECK) || bNormalized) ? MB_COMPOSITE : MB_PRECOMPOSED;
			}
			CSecureWString swsPrintFinal = CUtilities::MultiByteToWideChar( uiEncoding,
				dwBackToUtf16Flags,
				ssEncodedFinal,
				&dwError );

			if ( MX_IS_CHECKED( MX_STW_ESPACE_ONLY_NUL ) ) {
				swsPrintFinal = CUtilities::EscapeNulOnly( swsPrintFinal, false );
			}
			else if ( MX_IS_CHECKED( MX_STW_ESPACE_ONLY_NUL_AND_BS ) ) {
				swsPrintFinal = CUtilities::EscapeNulOnly( swsPrintFinal, true );
			}
			else if ( MX_IS_CHECKED( MX_STW_ESPACE_ALL_UNPRINTABLE ) ) {
				swsPrintFinal = CUtilities::EscapeUnprintable( swsPrintFinal, false, true );
			}
			else if ( MX_IS_CHECKED( MX_STW_ESPACE_ALL_UNPRINTABLE_AND_BS ) ) {
				swsPrintFinal = CUtilities::EscapeUnprintable( swsPrintFinal, true, true );
			}
			else if ( MX_IS_CHECKED( MX_STW_ESPACE_ALL ) ) {
				switch ( uiEncoding ) {
					case CCodePages::MX_utf_32 : {}
					case CCodePages::MX_utf_32BE : {
						std::vector<uint32_t> vTmp;
						for ( size_t I = 0; I < ssEncodedFinal.size(); ) {
							uint32_t ui32Char = 0;
							for ( size_t J = 0; J < sizeof( uint32_t ) && I < ssEncodedFinal.size(); ++J, ++I ) {
								ui32Char <<= 8;
								ui32Char |= static_cast<uint8_t>(ssEncodedFinal[I]);
							}
							vTmp.push_back( uiEncoding == CCodePages::MX_utf_32 ?
								::_byteswap_ulong( ui32Char ) :
								ui32Char );
						}

						std::vector<uint32_t> vTmp2 = CUtilities::EscapeAllW( vTmp, true );
						swsPrintFinal = ee::CExpEval::Utf32StringToWString( vTmp2.data(), vTmp2.size() );
						break;
					}
					case CCodePages::MX_utf_16 : {}
					case CCodePages::MX_utf_16BE : {
						//CUtilities::Bytesw
						swsPrintFinal = CUtilities::EscapeAllW( swsPrintFinal, true );
						break;
					}
					case CCodePages::MX_utf_8 : {
						swsPrintFinal = ee::CExpEval::ToUtf16( CUtilities::EscapeAllW( ssEncodedFinal, true ) );
						break;
					}
					default : {
						swsPrintFinal = ee::CExpEval::ToUtf16( CUtilities::EscapeAllNonUtf( ssEncodedFinal, true ) );
					}
				}
			}
			else if ( MX_IS_CHECKED( MX_STW_ESPACE_STANDARD ) ) {
				swsPrintFinal = CUtilities::EscapeStandard( swsPrintFinal, true );
			}

			CEdit * peOutputEdit = static_cast<CEdit *>(FindChild( CStringTheoryLayout::MX_STW_OUTPUT_EDIT ));
			
			if ( peOutputEdit ) {
				int iLines = peOutputEdit->GetFirstVisibleLine();
				peOutputEdit->SetTextW( swsPrintFinal.c_str() );
				peOutputEdit->LineScroll( 0, iLines );
			}
			peOutputEdit = static_cast<CEdit *>(FindChild( CStringTheoryLayout::MX_STW_OUTPUT_HEX_EDIT ));
			if ( peOutputEdit ) {
				CSecureString ssHex;
				CUtilities::BytesToHexWithSpaces( ssEncodedFinal.data(), ssEncodedFinal.size(), ssHex );
				peOutputEdit->SetTextA( ssHex.c_str() );
			}



			bool bIsComposite = MX_IS_CHECKED( MX_STW_COMPOSITE );
			if ( pwTmpDst = FindChild( CStringTheoryLayout::MX_STW_DEFAULT_CHAR ) ) { pwTmpDst->SetEnabled( bIsComposite && !bMustBe0 ); }
			if ( pwTmpDst = FindChild( CStringTheoryLayout::MX_STW_DISCARD_NONSPACING ) ) { pwTmpDst->SetEnabled( bIsComposite && !bMustBe0 ); }
			if ( pwTmpDst = FindChild( CStringTheoryLayout::MX_STW_SEP_CHARS ) ) { pwTmpDst->SetEnabled( bIsComposite && !bMustBe0 ); }

			if ( pwTmpDst = FindChild( CStringTheoryLayout::MX_STW_COMPOSITE ) ) { pwTmpDst->SetEnabled( !bMustBe0 ); }
			if ( pwTmpDst = FindChild( CStringTheoryLayout::MX_STW_NO_BEST_FIT ) ) { pwTmpDst->SetEnabled( !bMustBe0 ); }


			if ( pwTmpDst = FindChild( CStringTheoryLayout::MX_STW_INPUT_ENCODING_COMBO ) ) { pwTmpDst->SetEnabled( MX_IS_CHECKED( MX_STW_INPUT_IS_HEX_CHECK ) ); }
		}

		

#undef MX_IS_CHECKED
	}

	// Prepares to create the window.  Creates the atom if necessary.
	void CStringTheoryWindow::PrepareStringTheory() {
		if ( !m_aAtom ) {
			lsw::CWndClassEx wceEx;
			wceEx.SetInstance( lsw::CBase::GetThisHandle() );
			WCHAR szStr[9];
			mx::CUtilities::RandomString( szStr, MX_ELEMENTS( szStr ) );
			wceEx.SetClassName( szStr );
			wceEx.SetBackgroundBrush( reinterpret_cast<HBRUSH>(CTLCOLOR_DLG + 1) );
			wceEx.SetWindPro( CWidget::WindowProc );
			m_aAtom = lsw::CBase::RegisterClassExW( wceEx.Obj() );
		}
	}

	// WM_CTLCOLORSTATIC.
	CWidget::LSW_HANDLED CStringTheoryWindow::CtlColorStatic( HDC _hDc, CWidget * _pwControl, HBRUSH &_hBrush ) {
		if ( _pwControl && _pwControl->Id() == CStringTheoryLayout::MX_STW_OUTPUT_EDIT ) {
			_hBrush = ::GetSysColorBrush( COLOR_WINDOW );
			return LSW_H_HANDLED;
		}
		return LSW_H_CONTINUE;
	}

}	// namespace mx
