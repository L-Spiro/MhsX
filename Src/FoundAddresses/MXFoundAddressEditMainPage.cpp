#include "MXFoundAddressEditMainPage.h"
#include "../CodePages/MXCodePages.h"
#include "../FoundAddresses/MXFoundAddressBase.h"
#include "../FoundAddresses/MXFoundAddressManager.h"
#include "../Layouts/MXFoundAddressEditLayout.h"
#include "../MemHack/MXMemHack.h"
#include "../Strings/MXStringDecoder.h"

#include <Button/LSWColorButton.h>
#include <CheckButton/LSWCheckButton.h>

#include <EEExpEvalContainer.h>


namespace mx {

	CFoundAddressEditMainPage::CFoundAddressEditMainPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		CFoundAddressEditPageBase( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data )/*,
		m_poOptions( reinterpret_cast<MX_OPTIONS *>(_ui64Data) )*/ {
	}

	// == Functions.
	// Get the name of the page.
	std::wstring CFoundAddressEditMainPage::GetName() const {
		return _DEC_WS_01940FD6_General;
	}

	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CFoundAddressEditMainPage::InitDialog() {
		// Selectable values.
		auto pwTmp = FindChild( Layout::MX_FAEI_VALUE_TYPE_COMBO );
		if ( pwTmp ) {
			CUtilities::FillComboWithEditTypes( pwTmp, GatherDataTypes() );
		}
		pwTmp = FindChild( Layout::MX_FAEI_QL_LOCK_TYPE_COMBO );
		if ( pwTmp ) {
			CUtilities::FillComboBoxWithLocktypes( pwTmp, GatherLockTypes() );
		}
		pwTmp = FindChild( Layout::MX_FAEI_QL_STRING_LOCK_TYPE_COMBO );
		if ( pwTmp ) {
			CUtilities::FillComboBoxWithLocktypes( pwTmp, CUtilities::MX_LT_EXACT );
		}

		pwTmp = FindChild( Layout::MX_FAEI_QL_LOCKED_CHECK );
		if ( pwTmp ) {
			pwTmp->CheckButton( GatherLockedStates() );
		}

		pwTmp = FindChild( Layout::MX_FAEI_VALUE_TYPE_ESCAPE_CHECK );
		if ( pwTmp ) {
			pwTmp->CheckButton( true );
		}

		pwTmp = FindChild( Layout::MX_FAEI_VALUE_TYPE_STRING_COMBO );
		if ( pwTmp ) {
			CUtilities::FillComboBoxWithCodePages( pwTmp, GatherCodePages() );
		}
		pwTmp = FindChild( Layout::MX_FAEI_VALUE_TYPE_DATA_COMBO );
		if ( pwTmp ) {
			CUtilities::FillComboBoxWithStdDataTypes( pwTmp, GatherDataTypes() );
		}

		{
			auto pcbTmp = reinterpret_cast<lsw::CColorButton *>(FindChild( CFoundAddressEditLayout::MX_FAEI_GENERAL_COLOR_BUTTON ));
			if ( pcbTmp ) {
				pcbTmp->SetColor( GatherColors() );
				pcbTmp->SetColors( m_pParms.pmhMemHack->Options().vEditColorHistory );
			}
		}
		

		// History.
		pwTmp = FindChild( CFoundAddressEditLayout::MX_FAEI_GENERAL_DESC_COMBO );
		if ( pwTmp ) {
			CUtilities::FillComboWithStrings( pwTmp, m_pParms.pmhMemHack->Options().vEditDescriptionHistory, -1, -1 );
		}
		pwTmp = FindChild( CFoundAddressEditLayout::MX_FAEI_VALUE_CUR_VAL_COMBO );
		if ( pwTmp ) {
			CUtilities::FillComboWithStrings( pwTmp, m_pParms.pmhMemHack->Options().vEditCurValHistory, -1, -1 );
		}
		pwTmp = FindChild( CFoundAddressEditLayout::MX_FAEI_VALUE_ARRAY_LEN_COMBO );
		if ( pwTmp ) {
			CUtilities::FillComboWithStrings( pwTmp, m_pParms.pmhMemHack->Options().vEditArrayLengthHistory, -1, -1 );
		}
		pwTmp = FindChild( CFoundAddressEditLayout::MX_FAEI_VALUE_ARRAY_STRIDE_COMBO );
		if ( pwTmp ) {
			CUtilities::FillComboWithStrings( pwTmp, m_pParms.pmhMemHack->Options().vEditArrayStrideHistory, -1, -1 );
		}
		pwTmp = FindChild( CFoundAddressEditLayout::MX_FAEI_QL_VALUE_WHEN_LOCKED_LEFT_COMBO );
		if ( pwTmp ) {
			CUtilities::FillComboWithStrings( pwTmp, m_pParms.pmhMemHack->Options().vEditLockedLeftHistory, -1, -1 );
		}
		pwTmp = FindChild( CFoundAddressEditLayout::MX_FAEI_QL_VALUE_WHEN_LOCKED_RIGHT_COMBO );
		if ( pwTmp ) {
			CUtilities::FillComboWithStrings( pwTmp, m_pParms.pmhMemHack->Options().vEditLockedRightHistory, -1, -1 );
		}


		// Derived values
		pwTmp = FindChild( Layout::MX_FAEI_GENERAL_DESC_COMBO );
		if ( pwTmp ) {
			auto wsText = GatherDescriptions();
			pwTmp->SetTextW( wsText.c_str() );
		}
		pwTmp = FindChild( Layout::MX_FAEI_VALUE_CUR_VAL_COMBO );
		if ( pwTmp ) {
			pwTmp->LimitText();
			auto wsText = GatherCurValue();
			pwTmp->SetTextW( wsText.c_str() );
		}

		pwTmp = FindChild( Layout::MX_FAEI_VALUE_ARRAY_LEN_COMBO );
		if ( pwTmp ) {
			auto wsText = GatherArrayLens();
			pwTmp->SetTextW( wsText.c_str() );
		}
		pwTmp = FindChild( Layout::MX_FAEI_VALUE_ARRAY_STRIDE_COMBO );
		if ( pwTmp ) {
			auto wsText = GatherArrayStrides();
			pwTmp->SetTextW( wsText.c_str() );
		}

		pwTmp = FindChild( Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_LEFT_COMBO );
		if ( pwTmp ) {
			pwTmp->LimitText();
			auto wsText = GatherLockedValue();
			pwTmp->SetTextW( wsText.c_str() );
		}
		pwTmp = FindChild( Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_RIGHT_COMBO );
		if ( pwTmp ) {
			pwTmp->LimitText();
			auto wsText = GatherMaxLockedValue();
			pwTmp->SetTextW( wsText.c_str() );
		}
		pwTmp = FindChild( Layout::MX_FAEI_INFO_ADDRESS_EDIT );
		if ( pwTmp ) {
			auto wsText = GatherAddress();
			pwTmp->SetTextW( wsText.c_str() );
		}
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CFoundAddressEditMainPage::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {

		Update();
		return LSW_H_CONTINUE;
	}

	// Verifies the options, returning an error string in case of error.
	BOOL CFoundAddressEditMainPage::Verify( std::wstring &_wsError, CWidget * &_pwWidget ) {
		try {
			bool bIsMulti = m_pParms.vSelection.size() >= 2;
		
			int iType = -1;
			auto pwTmp = FindChild( Layout::MX_FAEI_VALUE_TYPE_COMBO );
			if ( pwTmp && pwTmp->GetCurSel() != -1 ) {
				iType = pwTmp->GetCurSelItemData();
			}
			int iLockType = -1;
			pwTmp = FindChild( Layout::MX_FAEI_QL_LOCK_TYPE_COMBO );
			if ( pwTmp && pwTmp->GetCurSel() != -1 ) {
				iLockType = pwTmp->GetCurSelItemData();
			}


			ee::CExpEvalContainer::EE_RESULT rRes;
			pwTmp = FindChild( Layout::MX_FAEI_VALUE_ARRAY_LEN_COMBO );
			rRes.u.i64Val = -1;
			if ( pwTmp ) {
				if ( !pwTmp->GetTextAsInt64Expression( rRes ) ) {
					if ( !bIsMulti ) {
						_wsError = _DEC_WS_43849434_Invalid_array_length_;
						_pwWidget = pwTmp;
						return FALSE;
					}
					else {
						rRes.u.i64Val = -1;
					}
				}
				else if ( rRes.u.i64Val < 0 || rRes.u.i64Val > CFoundAddress::MaxArrayLen() ) {
					_wsError = _DEC_WS_467E0365_Array_length_must_be_between_0_and_;
					_wsError += std::to_wstring( CFoundAddress::MaxArrayLen() );
					_wsError += L'.';
					_pwWidget = pwTmp;
					return FALSE;
				}
			}
			int32_t i32ArrayLen = int32_t( rRes.u.i64Val );

			pwTmp = FindChild( Layout::MX_FAEI_VALUE_ARRAY_STRIDE_COMBO );
			rRes.u.i64Val = -1;
			if ( pwTmp ) {
				if ( !pwTmp->GetTextAsInt64Expression( rRes ) ) {
					if ( !bIsMulti ) {
						_wsError = _DEC_WS_09B2D04F_Invalid_stride_;
						_pwWidget = pwTmp;
						return FALSE;
					}
					else {
						rRes.u.i64Val = -1;
					}
				}
				else if ( rRes.u.i64Val < 0 || rRes.u.i64Val > CFoundAddress::MaxArrayLen() ) {
					_wsError = _DEC_WS_F033357E_Stride_must_be_between_0_and_;
					_wsError += std::to_wstring( CFoundAddress::MaxArrayStride() );
					_wsError += L'.';
					_pwWidget = pwTmp;
					return FALSE;
				}
			}
			int32_t i32ArrayStride = int32_t( rRes.u.i64Val );


			CSecureWString swsValue;
			pwTmp = FindChild( Layout::MX_FAEI_VALUE_CUR_VAL_COMBO );
			if ( pwTmp ) {
				swsValue = pwTmp->GetTextW();
			}

			CSecureWString swsLeft;
			pwTmp = FindChild( Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_LEFT_COMBO );
			if ( pwTmp ) {
				swsLeft = pwTmp->GetTextW();
			}
			CSecureWString swsRight;
			pwTmp = FindChild( Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_RIGHT_COMBO );
			if ( pwTmp ) {
				swsRight = pwTmp->GetTextW();
			}

			bool bHex = false;
			pwTmp = FindChild( Layout::MX_FAEI_VALUE_TYPE_SHOW_AS_HEX_CHECK );
			if ( pwTmp ) {
				bHex = pwTmp->IsChecked();
			}

			bool bEscape = false;
			pwTmp = FindChild( Layout::MX_FAEI_VALUE_TYPE_ESCAPE_CHECK );
			if ( pwTmp ) {
				bEscape = pwTmp->IsChecked();
			}

			int iCodePage = -1;
			pwTmp = FindChild( Layout::MX_FAEI_VALUE_TYPE_STRING_COMBO );
			if ( pwTmp && pwTmp->GetCurSel() != -1 ) {
				iCodePage = int( pwTmp->GetCurSelItemData() );
			}


			{
				auto pfamMan = m_pParms.pmhMemHack->FoundAddressManager();
				for ( size_t I = 0; I < m_pParms.vSelection.size(); ++I ) {
					auto pfabThis = pfamMan->GetById( size_t( m_pParms.vSelection[I] ) );
					if ( pfabThis ) {
						if ( pfabThis->Type() == MX_FAT_FOUND_ADDRESS ) {
							auto pfaThis = static_cast<CFoundAddress *>(pfabThis);
							CUtilities::MX_DATA_TYPES dtThisType = (iType == -1) ? DataType( pfaThis ) : static_cast<CUtilities::MX_DATA_TYPES>(iType);

							if ( dtThisType == CUtilities::MX_DT_STRING ) {
								UINT uiCodePage = (iCodePage == -1) ? pfaThis->CodePage() : UINT( iCodePage );

								CSecureWString swsCopy = swsValue;
								if ( bEscape ) {
									CSecureString ssResolved;
									CUtilities::ResolveAllEscapes( ee::CExpEval::ToUtf8( swsCopy ), ssResolved, true );
									swsCopy = ee::CExpEval::ToUtf16( ssResolved );
								}


								CSecureString ssMultiByte;
								DWORD dwError;
								auto sCodePaged = CUtilities::WideCharToMultiByte( uiCodePage, 0, swsCopy, nullptr, nullptr, &dwError );
								switch ( dwError ) {
									case ERROR_SUCCESS : {
										break;
									}
									case ERROR_NOT_ENOUGH_MEMORY : {
										_wsError = _DEC_WS_F39F91A5_Out_of_memory_;
										_pwWidget = FindChild( Layout::MX_FAEI_VALUE_CUR_VAL_COMBO );
										return FALSE;
									}
									default : {
										_wsError = _DEC_WS_FA427FE5_String_conversion_error__;
										_wsError += ee::CExpEval::ToUtf16( ee::CExpEval::ToHex( uint64_t( dwError ) ) );
										_pwWidget = FindChild( Layout::MX_FAEI_VALUE_CUR_VAL_COMBO );
										return FALSE;
									}
								}
							}
							else {
								uint32_t ui32ArrayLen = (i32ArrayLen == -1) ? pfaThis->ArrayLen() : uint32_t( i32ArrayLen );
								if ( ui32ArrayLen > CFoundAddress::MaxArrayLen() ) {
									_wsError = _DEC_WS_467E0365_Array_length_must_be_between_0_and_;
									_wsError += std::to_wstring( CFoundAddress::MaxArrayLen() );
									_wsError += L'.';
									_pwWidget = FindChild( Layout::MX_FAEI_VALUE_ARRAY_LEN_COMBO );
									return FALSE;
								}

								uint32_t ui32Stride = (i32ArrayStride == -1) ? pfaThis->ArrayStride() : uint32_t( i32ArrayStride );
								if ( ui32Stride > CFoundAddress::MaxArrayStride() ) {
									_wsError = _DEC_WS_F033357E_Stride_must_be_between_0_and_;
									_wsError += std::to_wstring( CFoundAddress::MaxArrayStride() );
									_wsError += L'.';
									_pwWidget = FindChild( Layout::MX_FAEI_VALUE_ARRAY_STRIDE_COMBO );
									return FALSE;
								}

								bool bContiguous = ui32Stride <= CUtilities::DataTypeSize( dtThisType );

								bool bValid = true;
								CSecureWString swsCurVal = swsValue.size() ? swsValue : pfaThis->ValueText( &bValid );
								CSecureWString swsLeftLockVal = swsLeft.size() ? swsLeft : pfaThis->ValueWhenLockedText();
								if ( !bValid ) {
									// The Current Value text is empty, AND the current value canÅft be read from memory.
									// Since this address lost the ability to access the target memory, its contents donÅft really matter.  Just match it to the Locked Value.
									swsCurVal = swsLeftLockVal;
								}

								std::vector<std::vector<uint8_t>> vRawValues;
								CSecureWString swsError;
								auto ui32NewArrayLen = CUtilities::WStringToArrayBytes( vRawValues, swsCurVal, dtThisType, ui32ArrayLen, bHex ? 16 : 10, bContiguous, swsError );
								if ( swsError.size() ) {
									_wsError = swsError;
									_pwWidget = FindChild( Layout::MX_FAEI_VALUE_CUR_VAL_COMBO );
									return FALSE;
								}
								if ( ui32ArrayLen != 0 ) {
									if ( ui32NewArrayLen > ui32ArrayLen ) {
										_wsError = _DEC_WS_8869E389_The_current_value_has_more_elements_than_specified_by_Array_Length_;
										_pwWidget = FindChild( Layout::MX_FAEI_VALUE_CUR_VAL_COMBO );
										return FALSE;
									}
								}

								
								std::vector<std::vector<uint8_t>> vRawLockLeft;
								auto ui32LeftArrayLen = CUtilities::WStringToArrayBytes( vRawLockLeft, swsLeftLockVal, dtThisType, ui32ArrayLen, bHex ? 16 : 10, bContiguous, swsError );
								if ( swsError.size() ) {
									_wsError = swsError;
									_pwWidget = FindChild( Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_LEFT_COMBO );
									return FALSE;
								}
								if ( ui32ArrayLen != 0 ) {
									if ( ui32LeftArrayLen > ui32ArrayLen ) {
										_wsError = _DEC_WS_FAD3DED1_The_locked_value_has_more_elements_than_specified_by_Array_Length_;
										_pwWidget = FindChild( Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_LEFT_COMBO );
										return FALSE;
									}
								}
								else if ( ui32LeftArrayLen != ui32NewArrayLen ) {
									_wsError = _DEC_WS_DD4CBCAB_If_Array_Length_is_0__the_current_value_and_locked_value_s__must_have_the_same_number_of_elements_;
									_pwWidget = FindChild( Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_LEFT_COMBO );
									return FALSE;
								}

								CUtilities::MX_LOCK_TYPES ltLockType = (iLockType == -1) ? pfaThis->LockType() : CUtilities::MX_LOCK_TYPES( iLockType );
								if ( CUtilities::MX_LT_RANGE == ltLockType ) {
									CSecureWString swsRightLockVal = swsRight.size() ? swsRight : pfaThis->MaxValueWhenLockedText();
									std::vector<std::vector<uint8_t>> vRawLockRight;
									auto ui32RightArrayLen = CUtilities::WStringToArrayBytes( vRawLockRight, swsRight, dtThisType, ui32ArrayLen, bHex ? 16 : 10, bContiguous, swsError );
									if ( swsError.size() ) {
										_wsError = swsError;
										_pwWidget = FindChild( Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_RIGHT_COMBO );
										return FALSE;
									}
									if ( ui32ArrayLen != 0 ) {
										if ( ui32RightArrayLen > ui32ArrayLen ) {
											_wsError = _DEC_WS_FAD3DED1_The_locked_value_has_more_elements_than_specified_by_Array_Length_;
											_pwWidget = FindChild( Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_RIGHT_COMBO );
											return FALSE;
										}
									}
									else if ( ui32RightArrayLen != ui32NewArrayLen ) {
										_wsError = _DEC_WS_DD4CBCAB_If_Array_Length_is_0__the_current_value_and_locked_value_s__must_have_the_same_number_of_elements_;
										_pwWidget = FindChild( Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_RIGHT_COMBO );
										return FALSE;
									}

									std::vector<ee::CExpEvalContainer::EE_RESULT> vLeftExp, vRightExp;
									if ( CUtilities::ArrayBytesToResult( vRawLockLeft, nullptr, dtThisType, ui32LeftArrayLen, bContiguous, vLeftExp ) != ui32LeftArrayLen ) {
										_wsError = _DEC_WS_1468A1DF_Internal_error_;
										_pwWidget = FindChild( Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_RIGHT_COMBO );
										return FALSE;
									}
									if ( CUtilities::ArrayBytesToResult( vRawLockRight, nullptr, dtThisType, ui32RightArrayLen, bContiguous, vRightExp ) != ui32RightArrayLen ) {
										_wsError = _DEC_WS_1468A1DF_Internal_error_;
										_pwWidget = FindChild( Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_RIGHT_COMBO );
										return FALSE;
									}

									size_t sLen = std::min<size_t>( vLeftExp.size(), vRightExp.size() );
									for ( size_t I = 0; I < sLen; ++I ) {
										ee::CExpEvalContainer::EE_RESULT rTmp;
										if ( ee::CExpEvalContainer::EE_EC_SUCCESS != ee::CExpEvalContainer::PerformOp_S( vLeftExp[I], '>', vRightExp[I], rTmp ) ) {
											_wsError = _DEC_WS_247BC127_Internal_error_verifying_range_value_consistency_;
											_pwWidget = FindChild( Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_RIGHT_COMBO );
											return FALSE;
										}
										if ( rTmp.u.ui64Val ) {
											CSecureWString swsLeft, swsRight;
											CUtilities::ToDataTypeString( vLeftExp[I], static_cast<CUtilities::MX_DATA_TYPES>(iType), swsLeft );
											CUtilities::ToDataTypeString( vRightExp[I], static_cast<CUtilities::MX_DATA_TYPES>(iType), swsRight );
											_wsError = _DEC_WS_E2E06A5A_Invalid_range_min_max__;
											_wsError += swsLeft;
											_wsError += _DEC_WS_4FBEB86D__is_greater_than_;
											_wsError += swsRight;
											_wsError += _DEC_WS_4F2FE7D3___index_;
											_wsError += std::to_wstring( I );
											_wsError += _DEC_WS_D94980DB___;

											_pwWidget = FindChild( Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_RIGHT_COMBO );
											return FALSE;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		catch ( ... ) {
			_wsError = _DEC_WS_F39F91A5_Out_of_memory_;
			_pwWidget = FindChild( Layout::MX_FAEI_VALUE_CUR_VAL_COMBO );
			return FALSE;
		}

		return TRUE;
	}

	// Copies all the settings to the MX_OPTIONS structure.
	BOOL CFoundAddressEditMainPage::Finalize() {
		try {
			bool bIsMulti = m_pParms.vSelection.size() >= 2;
		
			int iType = -1;
			auto pwTmp = FindChild( Layout::MX_FAEI_VALUE_TYPE_COMBO );
			if ( pwTmp && pwTmp->GetCurSel() != -1 ) {
				iType = pwTmp->GetCurSelItemData();
			}
			int iLockType = -1;
			pwTmp = FindChild( Layout::MX_FAEI_QL_LOCK_TYPE_COMBO );
			if ( pwTmp && pwTmp->GetCurSel() != -1 ) {
				iLockType = pwTmp->GetCurSelItemData();
			}


			ee::CExpEvalContainer::EE_RESULT rRes;
			pwTmp = FindChild( Layout::MX_FAEI_VALUE_ARRAY_LEN_COMBO );
			rRes.u.i64Val = -1;
			if ( pwTmp ) {
				if ( !pwTmp->GetTextAsInt64Expression( rRes ) ) {
					if ( !bIsMulti ) { return FALSE; }
					else { rRes.u.i64Val = -1; }
				}
				else if ( rRes.u.i64Val < 0 || rRes.u.i64Val > CFoundAddress::MaxArrayLen() ) { return FALSE; }
			}
			int32_t i32ArrayLen = int32_t( rRes.u.i64Val );

			pwTmp = FindChild( Layout::MX_FAEI_VALUE_ARRAY_STRIDE_COMBO );
			rRes.u.i64Val = -1;
			if ( pwTmp ) {
				if ( !pwTmp->GetTextAsInt64Expression( rRes ) ) {
					if ( !bIsMulti ) { return FALSE; }
					else { rRes.u.i64Val = -1; }
				}
				else if ( rRes.u.i64Val < 0 || rRes.u.i64Val > CFoundAddress::MaxArrayLen() ) { return FALSE; }
			}
			int32_t i32ArrayStride = int32_t( rRes.u.i64Val );


			CSecureWString swsDesc;
			pwTmp = FindChild( Layout::MX_FAEI_GENERAL_DESC_COMBO );
			if ( pwTmp ) {
				swsDesc = pwTmp->GetTextW();
			}
			bool bAddArrayIndices = false;
			pwTmp = FindChild( Layout::MX_FAEI_GENERAL_ARRAY_CHECK );
			if ( pwTmp ) {
				bAddArrayIndices = pwTmp->IsChecked();
			}

			bool bSetColor = false;
			RGBQUAD rgbColor;
			auto pcbTmp = reinterpret_cast<lsw::CColorButton *>(FindChild( CFoundAddressEditLayout::MX_FAEI_GENERAL_COLOR_BUTTON ));
			if ( pcbTmp ) {
				bSetColor = pcbTmp->ColorPicked() != FALSE;
				auto crColor = pcbTmp->GetColor();
				rgbColor.rgbRed = GetRValue( crColor );
				rgbColor.rgbGreen = GetGValue( crColor );
				rgbColor.rgbBlue = GetBValue( crColor );
				rgbColor.rgbReserved = 0xFF;
				if ( rgbColor.rgbRed == rgbColor.rgbGreen && rgbColor.rgbGreen == rgbColor.rgbBlue && rgbColor.rgbBlue == 0xFF ) {
					rgbColor.rgbReserved = 0x00;
				}

				auto oTmp = m_pParms.pmhMemHack->Options();
				oTmp.vEditColorHistory = pcbTmp->GetColors();
				m_pParms.pmhMemHack->SetOptions( oTmp );
			}


			if ( swsDesc.size() || !bIsMulti ) {
				auto pfamMan = m_pParms.pmhMemHack->FoundAddressManager();
				for ( size_t I = 0; I < m_pParms.vSelection.size(); ++I ) {
					auto pfaThis = pfamMan->GetById( size_t( m_pParms.vSelection[I] ) );
					if ( pfaThis ) {
						if ( bAddArrayIndices ) {
							CSecureWString swsTmp = swsDesc;
							swsTmp += L'[';
							CUtilities::ToSigned( I, swsTmp, uint32_t( std::ceil( std::log10( double( m_pParms.vSelection.size() ) ) ) ) );
							swsTmp += L']';
							pfaThis->SetDescriptionText( swsTmp );
						}
						else {
							pfaThis->SetDescriptionText( swsDesc );
						}
					}
				}
			}
			if ( bSetColor ) {
				auto pfamMan = m_pParms.pmhMemHack->FoundAddressManager();
				for ( size_t I = 0; I < m_pParms.vSelection.size(); ++I ) {
					auto pfaThis = pfamMan->GetById( size_t( m_pParms.vSelection[I] ) );
					if ( pfaThis ) {
						pfaThis->SetColor( rgbColor.rgbRed, rgbColor.rgbGreen, rgbColor.rgbBlue, rgbColor.rgbReserved );
					}
				}
			}


			CSecureWString swsValue;
			pwTmp = FindChild( Layout::MX_FAEI_VALUE_CUR_VAL_COMBO );
			if ( pwTmp ) {
				swsValue = pwTmp->GetTextW();
			}

			CSecureWString swsLeft;
			pwTmp = FindChild( Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_LEFT_COMBO );
			if ( pwTmp ) {
				swsLeft = pwTmp->GetTextW();
			}
			CSecureWString swsRight;
			pwTmp = FindChild( Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_RIGHT_COMBO );
			if ( pwTmp ) {
				swsRight = pwTmp->GetTextW();
			}

			bool bHex = false;
			pwTmp = FindChild( Layout::MX_FAEI_VALUE_TYPE_SHOW_AS_HEX_CHECK );
			if ( pwTmp ) {
				bHex = pwTmp->IsChecked();
			}

			bool bEscape = false;
			pwTmp = FindChild( Layout::MX_FAEI_VALUE_TYPE_ESCAPE_CHECK );
			if ( pwTmp ) {
				bEscape = pwTmp->IsChecked();
			}

			int32_t i32Locked = BST_INDETERMINATE;
			pwTmp = FindChild( Layout::MX_FAEI_QL_LOCKED_CHECK );
			if ( pwTmp ) {
				i32Locked = pwTmp->CheckState();
			}

			int iCodePage = -1;
			pwTmp = FindChild( Layout::MX_FAEI_VALUE_TYPE_STRING_COMBO );
			if ( pwTmp && pwTmp->GetCurSel() != -1 ) {
				iCodePage = int( pwTmp->GetCurSelItemData() );
			}



			{
				auto pfamMan = m_pParms.pmhMemHack->FoundAddressManager();
				for ( size_t I = 0; I < m_pParms.vSelection.size(); ++I ) {
					auto pfabThis = pfamMan->GetById( size_t( m_pParms.vSelection[I] ) );
					if ( pfabThis ) {
						if ( pfabThis->Type() == MX_FAT_FOUND_ADDRESS ) {
							auto pfaThis = static_cast<CFoundAddress *>(pfabThis);
							bool bLocked = (i32Locked == BST_INDETERMINATE) ? pfaThis->Locked() : (i32Locked == BST_CHECKED);
							pfaThis->SetLocked( false );
							CUtilities::MX_DATA_TYPES dtThisType = (iType == -1) ? DataType( pfaThis ) : static_cast<CUtilities::MX_DATA_TYPES>(iType);
							pfaThis->Dirty();
							uint64_t ui64Address = pfaThis->FinalAddress();

							if ( dtThisType == CUtilities::MX_DT_STRING ) {
								UINT uiCodePage = (iCodePage == -1) ? pfaThis->CodePage() : UINT( iCodePage );

								CSecureWString swsCopy = swsValue;
								if ( bEscape && swsCopy.size() ) {
									CSecureString ssResolved;
									CUtilities::ResolveAllEscapes( ee::CExpEval::ToUtf8( swsCopy ), ssResolved, true );
									swsCopy = ee::CExpEval::ToUtf16( ssResolved );
								}


								DWORD dwError;
								auto sCodePaged = CUtilities::WideCharToMultiByte( uiCodePage, 0, swsCopy, nullptr, nullptr, &dwError );
								if ( dwError == ERROR_SUCCESS ) {
									if ( !bLocked && swsValue.size() ) {
										// Write the current value to RAM.
										m_pParms.pmhMemHack->Process().WriteProcessMemory_PreProcessed( ui64Address, sCodePaged.data(), sCodePaged.size(), pfaThis->PreProcessing() );
									}
									
									swsCopy = swsLeft;
									if ( bEscape && swsCopy.size() ) {
										CSecureString ssResolved;
										CUtilities::ResolveAllEscapes( ee::CExpEval::ToUtf8( swsCopy ), ssResolved, true );
										swsCopy = ee::CExpEval::ToUtf16( ssResolved );
									}
									auto sLockedCodePaged = CUtilities::WideCharToMultiByte( uiCodePage, 0, swsCopy, nullptr, nullptr, &dwError );
									if ( dwError == ERROR_SUCCESS ) {
										// Set the current value as a string.
										pfaThis->SetAsString( sLockedCodePaged, uiCodePage );
									}
								}
							}
							else {
								uint32_t ui32ArrayLen = (i32ArrayLen == -1) ? pfaThis->ArrayLen() : uint32_t( i32ArrayLen );
								if ( ui32ArrayLen > CFoundAddress::MaxArrayLen() ) { continue; }

								uint32_t ui32Stride = (i32ArrayStride == -1) ? pfaThis->ArrayStride() : uint32_t( i32ArrayStride );
								if ( ui32Stride > CFoundAddress::MaxArrayStride() ) { continue; }
								uint32_t ui32ActualStride = std::max<uint32_t>( ui32Stride, CUtilities::DataTypeSize( dtThisType ) );

								bool bContiguous = ui32Stride <= CUtilities::DataTypeSize( dtThisType );

								bool bValid = true;
								CSecureWString swsCurVal = swsValue.size() ? swsValue : pfaThis->ValueText( &bValid );
								CSecureWString swsLeftLockVal = swsLeft.size() ? swsLeft : pfaThis->ValueWhenLockedText();
								if ( !bValid ) {
									swsCurVal = swsLeftLockVal;
								}

								std::vector<std::vector<uint8_t>> vRawValues;
								CSecureWString swsError;
								auto ui32NewArrayLen = CUtilities::WStringToArrayBytes( vRawValues, swsCurVal, dtThisType, ui32ArrayLen, bHex ? 16 : 10, bContiguous, swsError );
								if ( swsError.size() ) { continue; }
								if ( ui32ArrayLen != 0 ) {
									if ( ui32NewArrayLen > ui32ArrayLen ) { continue; }
								}

								if ( !bLocked && swsValue.size() ) {
									// Write the current value to RAM.
									CFoundAddressBase::WriteProcessMemory_PreProcessed( m_pParms.pmhMemHack,
										ui64Address, vRawValues, bContiguous, ui32ActualStride, pfaThis->PreProcessing() );
								}


								
								std::vector<std::vector<uint8_t>> vRawLockLeft;
								auto ui32LeftArrayLen = CUtilities::WStringToArrayBytes( vRawLockLeft, swsLeftLockVal, dtThisType, ui32ArrayLen, bHex ? 16 : 10, bContiguous, swsError );
								if ( swsError.size() ) { continue; }
								if ( ui32ArrayLen != 0 ) {
									if ( ui32LeftArrayLen > ui32ArrayLen ) { continue; }
								}
								else if ( ui32LeftArrayLen != ui32NewArrayLen ) { continue; }

								CUtilities::MX_LOCK_TYPES ltLockType = (iLockType == -1) ? pfaThis->LockType() : CUtilities::MX_LOCK_TYPES( iLockType );
								pfaThis->SetLockType( ltLockType );
								if ( CUtilities::MX_LT_RANGE == ltLockType ) {
									CSecureWString swsRightLockVal = swsRight.size() ? swsRight : pfaThis->MaxValueWhenLockedText();
									std::vector<std::vector<uint8_t>> vRawLockRight;
									auto ui32RightArrayLen = CUtilities::WStringToArrayBytes( vRawLockRight, swsRightLockVal, dtThisType, ui32ArrayLen, bHex ? 16 : 10, bContiguous, swsError );
									if ( swsError.size() ) { continue; }
									if ( ui32ArrayLen != 0 ) {
										if ( ui32RightArrayLen > ui32ArrayLen ) { continue; }
									}
									else if ( ui32RightArrayLen != ui32NewArrayLen ) { continue; }

									
									pfaThis->SetAsDataType( vRawLockLeft, dtThisType, ui32LeftArrayLen, ui32Stride, &vRawLockRight );
								}
								else {
									pfaThis->SetAsDataType( vRawLockLeft, dtThisType, ui32LeftArrayLen, ui32Stride );
								}
							}
							
							pfaThis->SetLocked( bLocked );
						}
					}
				}
			}
		}
		catch ( ... ) {
			return FALSE;
		}

		MX_OPTIONS oOption = m_pParms.pmhMemHack->Options();
		auto pwTmp = FindChild( CFoundAddressEditLayout::MX_FAEI_GENERAL_DESC_COMBO );
		if ( pwTmp ) {
			CUtilities::AddOrMove<CSecureWString>( oOption.vEditDescriptionHistory, pwTmp->GetTextW() );
		}
		pwTmp = FindChild( CFoundAddressEditLayout::MX_FAEI_VALUE_CUR_VAL_COMBO );
		if ( pwTmp ) {
			CUtilities::AddOrMove<CSecureWString>( oOption.vEditCurValHistory, pwTmp->GetTextW() );
		}
		pwTmp = FindChild( CFoundAddressEditLayout::MX_FAEI_VALUE_ARRAY_LEN_COMBO );
		if ( pwTmp ) {
			CUtilities::AddOrMove<CSecureWString>( oOption.vEditArrayLengthHistory, pwTmp->GetTextW() );
		}
		pwTmp = FindChild( CFoundAddressEditLayout::MX_FAEI_VALUE_ARRAY_STRIDE_COMBO );
		if ( pwTmp ) {
			CUtilities::AddOrMove<CSecureWString>( oOption.vEditArrayStrideHistory, pwTmp->GetTextW() );
		}
		pwTmp = FindChild( CFoundAddressEditLayout::MX_FAEI_QL_VALUE_WHEN_LOCKED_LEFT_COMBO );
		if ( pwTmp ) {
			CUtilities::AddOrMove<CSecureWString>( oOption.vEditLockedLeftHistory, pwTmp->GetTextW() );
		}
		pwTmp = FindChild( CFoundAddressEditLayout::MX_FAEI_QL_VALUE_WHEN_LOCKED_RIGHT_COMBO );
		if ( pwTmp ) {
			CUtilities::AddOrMove<CSecureWString>( oOption.vEditLockedRightHistory, pwTmp->GetTextW() );
		}
		m_pParms.pmhMemHack->SetOptions( oOption );

		return TRUE;
	}

	// Updates the contents of page based on settings etc.
	void CFoundAddressEditMainPage::Update() {
		int iType = -1;
		auto pwTmp = FindChild( Layout::MX_FAEI_VALUE_TYPE_COMBO );
		if ( pwTmp ) {
			iType = pwTmp->GetCurSelItemData();
		}
		int iValWhenLocked = -1;
		pwTmp = FindChild( Layout::MX_FAEI_QL_LOCK_TYPE_COMBO );
		if ( pwTmp ) {
			iValWhenLocked = pwTmp->GetCurSelItemData();
		}

		pwTmp = FindChild( Layout::MX_FAEI_VALUE_ARRAY_LEN_COMBO );
		ee::CExpEvalContainer::EE_RESULT rRes;
		rRes.u.i64Val = 0;
		if ( pwTmp ) {
			rRes.u.i64Val = -1;
			pwTmp->GetTextAsInt64Expression( rRes );
		}
		CSecureWString swsCurText;
		auto pwCurVal = FindChild( Layout::MX_FAEI_VALUE_CUR_VAL_COMBO );
		if ( pwCurVal ) {
			swsCurText = pwCurVal->GetTextW();
		}

		bool bIsMulti = m_pParms.vSelection.size() >= 2;

		
		{
			struct {
				Layout::MX_FOUND_ADDRESS_EDIT_IDS						wId;
				bool													bCondition0;
			} cControls[] = {
				{ Layout::MX_FAEI_GENERAL_ARRAY_CHECK,					bIsMulti },
				{ Layout::MX_FAEI_VALUE_TYPE_DATA_COMBO,				iType == LPARAM( CUtilities::MX_DT_VOID ) || iType == -1 },

				{ Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_RIGHT_LABEL,		iValWhenLocked == LPARAM( CUtilities::MX_LT_RANGE ) && iType != LPARAM( CUtilities::MX_DT_STRING ) },
				{ Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_RIGHT_COMBO,		iValWhenLocked == LPARAM( CUtilities::MX_LT_RANGE ) && iType != LPARAM( CUtilities::MX_DT_STRING ) },

				{ Layout::MX_FAEI_VALUE_TYPE_STRING_COMBO,				iType == LPARAM( CUtilities::MX_DT_STRING ) },

				{ Layout::MX_FAEI_VALUE_ARRAY_LEN_LABEL,				iType != LPARAM( CUtilities::MX_DT_STRING ) },
				{ Layout::MX_FAEI_VALUE_ARRAY_LEN_COMBO,				iType != LPARAM( CUtilities::MX_DT_STRING ) },

				{ Layout::MX_FAEI_VALUE_ARRAY_STRIDE_LABEL,				rRes.u.i64Val != 1 && iType != LPARAM( CUtilities::MX_DT_STRING ) },
				{ Layout::MX_FAEI_VALUE_ARRAY_STRIDE_COMBO,				rRes.u.i64Val != 1 && iType != LPARAM( CUtilities::MX_DT_STRING ) },
			};
			for ( auto I = MX_ELEMENTS( cControls ); I--; ) {
				auto pwThis = FindChild( cControls[I].wId );
				if ( pwThis ) {
					pwThis->SetEnabled( cControls[I].bCondition0 );
				}
			}
		}

		{
			struct {
				Layout::MX_FOUND_ADDRESS_EDIT_IDS						wId;
				bool													bCondition0;
			} cControls[] = {
				{ Layout::MX_FAEI_VALUE_CUR_VAL_DESC_LABEL,				swsCurText.size() != 0 },

				{ Layout::MX_FAEI_VALUE_TYPE_SHOW_AS_HEX_CHECK,			iType != LPARAM( CUtilities::MX_DT_STRING ) && !CUtilities::DataTypeIsFloat( static_cast<CUtilities::MX_DATA_TYPES>(iType) ) },
				{ Layout::MX_FAEI_VALUE_TYPE_SCIENTIFIC_CHECK,			!!CUtilities::DataTypeIsFloat( static_cast<CUtilities::MX_DATA_TYPES>(iType) ) },
				{ Layout::MX_FAEI_VALUE_TYPE_ESCAPE_CHECK,				iType == LPARAM( CUtilities::MX_DT_STRING ) },

				{ Layout::MX_FAEI_QL_LOCK_TYPE_COMBO,					iType != LPARAM( CUtilities::MX_DT_STRING ) },
				{ Layout::MX_FAEI_QL_STRING_LOCK_TYPE_COMBO,			iType == LPARAM( CUtilities::MX_DT_STRING ) },

				{ Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_LEFT_LABEL,		iValWhenLocked != LPARAM( CUtilities::MX_LT_RANGE ) || iType == LPARAM( CUtilities::MX_DT_STRING ) },
				{ Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_MIN_LABEL,		iValWhenLocked == LPARAM( CUtilities::MX_LT_RANGE ) && iType != LPARAM( CUtilities::MX_DT_STRING ) },
			};
			for ( auto I = MX_ELEMENTS( cControls ); I--; ) {
				auto pwThis = FindChild( cControls[I].wId );
				if ( pwThis ) {
					pwThis->SetVisible( cControls[I].bCondition0 );
				}
			}
		}

		
		
		auto pwLabel = FindChild( Layout::MX_FAEI_INFO_DESC_LABEL );
		if ( pwLabel ) {
			try {
				// Update the explanation text.
				CSecureWString swsExpText;
				switch ( iType ) {
					case LPARAM( CUtilities::MX_DT_STRING ) : {
						// ===================
						// CURRENT VALUE
						// ===================
						swsExpText += _DEC_WS_320BB0E4_Value_;
						swsExpText += L' ';
						UINT uiSysCodePage = CCodePages::GetSystemDefaultAnsiCodePage();
						auto pwCodePage = FindChild( Layout::MX_FAEI_VALUE_TYPE_STRING_COMBO );
						if ( pwCodePage ) {
							uiSysCodePage = UINT( pwCodePage->GetCurSelItemData() );
						}
						bool bEscape = false;
						auto pwEscape = FindChild( Layout::MX_FAEI_VALUE_TYPE_ESCAPE_CHECK );
						if ( pwEscape ) {
							bEscape = !!pwEscape->IsChecked();
						}
						
						if ( !swsCurText.size() && bIsMulti ) {
							// Empty string on multi-select means don't change the texts.
							swsExpText += _DEC_WS_43645600__multiple_items_;
						}
						else if ( swsCurText.size() ) {
							// UTF-16 -> Code Page -> UTF-16.
							swsExpText += StringToDescr( swsCurText, bEscape, uiSysCodePage );
						}

						// Current value handled.
						swsExpText += L'\r';
						swsExpText += L'\n';



						// ===================
						// VALUE WHEN LOCKED
						// ===================
						swsExpText += _DEC_WS_E64C54B0_Value_When_Locked__;

						pwCurVal = FindChild( Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_LEFT_COMBO );
						if ( pwCurVal ) {
							swsCurText = pwCurVal->GetTextW();
						}
						if ( !swsCurText.size() && bIsMulti ) {
							// Empty string on multi-select means don't change the texts.
							swsExpText += _DEC_WS_43645600__multiple_items_;
						}
						else if ( swsCurText.size() ) {
							// UTF-16 -> Code Page -> UTF-16.
							swsExpText += StringToDescr( swsCurText, bEscape, uiSysCodePage );
						}
						break;
					}
					default : {
						do {
							// ===================
							// CURRENT VALUE
							// ===================
							swsExpText += _DEC_WS_320BB0E4_Value_;
							swsExpText += L' ';
							ee::CExpEvalContainer::EE_RESULT rRes;
							if ( CUtilities::DataTypeIsFloat( static_cast<CUtilities::MX_DATA_TYPES>(iType) ) ) {
								int32_t i32Digits = 17;
								bool bScientific = false;
								auto pwCurVal = FindChild( Layout::MX_FAEI_VALUE_TYPE_SCIENTIFIC_CHECK );
								if ( pwCurVal && pwCurVal->IsChecked() ) {
									bScientific = true;
								}

								uint32_t ui32DesiredArrayLen = 0;
								pwCurVal = FindChild( Layout::MX_FAEI_VALUE_ARRAY_LEN_COMBO );
								if ( pwCurVal ) {
									ee::CExpEvalContainer::EE_RESULT rRes;
									if ( bIsMulti && !pwCurVal->GetTextW().size() ) {
									}
									else {
										if ( !pwCurVal->GetTextAsUInt64Expression( rRes ) ) {
											swsExpText += _DEC_WS_43849434_Invalid_array_length_;
											break;
										}
										if ( rRes.u.ui64Val > CFoundAddress::MaxArrayLen() ) {
											swsExpText += _DEC_WS_467E0365_Array_length_must_be_between_0_and_;
											swsExpText += std::to_wstring( CFoundAddress::MaxArrayLen() );
											swsExpText += L'.';
											break;
										}
										ui32DesiredArrayLen = uint32_t( rRes.u.ui64Val );
									}
								}

								uint32_t ui32Stride = 0;
								pwCurVal = FindChild( Layout::MX_FAEI_VALUE_ARRAY_STRIDE_COMBO );
								if ( pwCurVal ) {
									ee::CExpEvalContainer::EE_RESULT rRes;
									if ( bIsMulti && !pwCurVal->GetTextW().size() ) {
									}
									else {
										if ( !pwCurVal->GetTextAsUInt64Expression( rRes ) ) {
											swsExpText += _DEC_WS_09B2D04F_Invalid_stride_;
											break;
										}
										if ( rRes.u.ui64Val > CFoundAddress::MaxArrayStride() ) {
											swsExpText += _DEC_WS_F033357E_Stride_must_be_between_0_and_;
											swsExpText += std::to_wstring( CFoundAddress::MaxArrayStride() );
											swsExpText += L'.';
											break;
										}
										ui32Stride = uint32_t( rRes.u.ui64Val );
									}
								}
								pwCurVal = FindChild( Layout::MX_FAEI_VALUE_CUR_VAL_COMBO );
								uint32_t ui32ArrayLen = ui32DesiredArrayLen;
								if ( pwCurVal ) {
									auto swsCurText = pwCurVal->GetTextW();
									if ( !swsCurText.size() && bIsMulti ) {
										// Empty string on multi-select means don't change the texts.
										swsExpText += _DEC_WS_43645600__multiple_items_;
									}
									else {
										swsExpText += FloatToDesc( swsCurText, static_cast<CUtilities::MX_DATA_TYPES>(iType), bScientific, ui32ArrayLen, ui32Stride, nullptr );
									}

								}

								if ( ui32DesiredArrayLen ) {
									if ( ui32ArrayLen > ui32DesiredArrayLen ) {
										swsExpText += L'\r';
										swsExpText += L'\n';
										swsExpText += _DEC_WS_8869E389_The_current_value_has_more_elements_than_specified_by_Array_Length_;
										break;
									}
								}

								// Current value handled.
								swsExpText += L'\r';
								swsExpText += L'\n';



								// ===================
								// VALUE WHEN LOCKED
								// ===================
								swsExpText += _DEC_WS_E64C54B0_Value_When_Locked__;

								pwCurVal = FindChild( Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_LEFT_COMBO );
								uint32_t ui32LeftArrayLen = ui32DesiredArrayLen;
								std::vector<ee::CExpEvalContainer::EE_RESULT> vLeftResults;
								if ( pwCurVal ) {
									auto swsCurText = pwCurVal->GetTextW();
									if ( !swsCurText.size() && bIsMulti ) {
										// Empty string on multi-select means don't change the texts.
										swsExpText += _DEC_WS_43645600__multiple_items_;
									}
									else {
										swsExpText += FloatToDesc( swsCurText, static_cast<CUtilities::MX_DATA_TYPES>(iType), bScientific, ui32LeftArrayLen, ui32Stride, &vLeftResults );
									}
								}

								if ( !ui32DesiredArrayLen ) {
									if ( ui32ArrayLen != ui32LeftArrayLen ) {
										swsExpText += L'\r';
										swsExpText += L'\n';
										swsExpText += _DEC_WS_DD4CBCAB_If_Array_Length_is_0__the_current_value_and_locked_value_s__must_have_the_same_number_of_elements_;
										break;
									}
								}
								else {
									if ( ui32LeftArrayLen > ui32DesiredArrayLen ) {
										swsExpText += L'\r';
										swsExpText += L'\n';
										swsExpText += _DEC_WS_FAD3DED1_The_locked_value_has_more_elements_than_specified_by_Array_Length_;
										break;
									}
								}

								if ( iValWhenLocked == LPARAM( CUtilities::MX_LT_RANGE ) ) {
									pwCurVal = FindChild( Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_RIGHT_COMBO );
									uint32_t ui32RighttArrayLen = ui32DesiredArrayLen;
									if ( pwCurVal ) {
										swsExpText += L'\r';
										swsExpText += L'\n';
										swsExpText += _DEC_WS_B09DF1A4_To_;
										swsExpText += L' ';
										auto swsCurText = pwCurVal->GetTextW();
										std::vector<ee::CExpEvalContainer::EE_RESULT> vRightResults;
										if ( !swsCurText.size() && bIsMulti ) {
											// Empty string on multi-select means don't change the texts.
											swsExpText += _DEC_WS_43645600__multiple_items_;
										}
										else {
											swsExpText += FloatToDesc( swsCurText, static_cast<CUtilities::MX_DATA_TYPES>(iType), bScientific, ui32RighttArrayLen, ui32Stride, &vRightResults );
										}

										// Min and max values must be checked against each other.
										size_t sLen = std::min<size_t>( vLeftResults.size(), vRightResults.size() );
										swsExpText += L'\r';
										swsExpText += L'\n';
										for ( size_t I = 0; I < sLen; ++I ) {
											ee::CExpEvalContainer::EE_RESULT rTmp;
											if ( ee::CExpEvalContainer::EE_EC_SUCCESS != ee::CExpEvalContainer::PerformOp_S( vLeftResults[I], '>', vRightResults[I], rTmp ) ) {
												swsExpText += _DEC_WS_247BC127_Internal_error_verifying_range_value_consistency_;
												break;
											}
											if ( rTmp.u.ui64Val ) {
												CSecureWString swsLeft, swsRight;
												CUtilities::ToDataTypeString( vLeftResults[I], static_cast<CUtilities::MX_DATA_TYPES>(iType), swsLeft );
												CUtilities::ToDataTypeString( vRightResults[I], static_cast<CUtilities::MX_DATA_TYPES>(iType), swsRight );
												//swsExpText += std::format( _DEC_WS_4B09D6E2_Invalid_range_min_max_____must_be_lower_than_____index_____, swsLeft, swsRight, I );
												swsExpText += _DEC_WS_E2E06A5A_Invalid_range_min_max__;
												swsExpText += swsLeft;
												swsExpText += _DEC_WS_4FBEB86D__is_greater_than_;
												swsExpText += swsRight;
												swsExpText += _DEC_WS_4F2FE7D3___index_;
												swsExpText += std::to_wstring( I );
												swsExpText += _DEC_WS_D94980DB___;

												if ( !ui32DesiredArrayLen ) {
													if ( ui32ArrayLen != ui32RighttArrayLen ) {
														swsExpText += L'\r';
														swsExpText += L'\n';
														swsExpText += _DEC_WS_DD4CBCAB_If_Array_Length_is_0__the_current_value_and_locked_value_s__must_have_the_same_number_of_elements_;
														break;
													}
												}
												else {
													if ( ui32RighttArrayLen > ui32DesiredArrayLen ) {
														swsExpText += L'\r';
														swsExpText += L'\n';
														swsExpText += _DEC_WS_40FB27FE_The_range_lock_max_value_has_more_elements_than_specified_by_Array_Length_;
														break;
													}
												}
												break;
											}
										}
									}
								}
							}
							else {
								bool bHex = false;
								auto pwCurVal = FindChild( Layout::MX_FAEI_VALUE_TYPE_SHOW_AS_HEX_CHECK );
								if ( pwCurVal && pwCurVal->IsChecked() ) {
									bHex = true;
								}

								uint32_t ui32DesiredArrayLen = 0;
								pwCurVal = FindChild( Layout::MX_FAEI_VALUE_ARRAY_LEN_COMBO );
								if ( pwCurVal ) {
									ee::CExpEvalContainer::EE_RESULT rRes;
									if ( bIsMulti && !pwCurVal->GetTextW().size() ) {
									}
									else {
										if ( !pwCurVal->GetTextAsUInt64Expression( rRes ) ) {
											swsExpText += _DEC_WS_43849434_Invalid_array_length_;
											break;
										}
										if ( rRes.u.ui64Val > CFoundAddress::MaxArrayLen() ) {
											swsExpText += _DEC_WS_467E0365_Array_length_must_be_between_0_and_;
											swsExpText += std::to_wstring( CFoundAddress::MaxArrayLen() );
											swsExpText += L'.';
											break;
										}
										ui32DesiredArrayLen = uint32_t( rRes.u.ui64Val );
									}
								}

								uint32_t ui32Stride = 0;
								pwCurVal = FindChild( Layout::MX_FAEI_VALUE_ARRAY_STRIDE_COMBO );
								if ( pwCurVal ) {
									ee::CExpEvalContainer::EE_RESULT rRes;
									if ( bIsMulti && !pwCurVal->GetTextW().size() ) {
									}
									else {
										if ( !pwCurVal->GetTextAsUInt64Expression( rRes ) ) {
											swsExpText += _DEC_WS_09B2D04F_Invalid_stride_;
											break;
										}
										if ( rRes.u.ui64Val > CFoundAddress::MaxArrayStride() ) {
											swsExpText += _DEC_WS_F033357E_Stride_must_be_between_0_and_;
											swsExpText += std::to_wstring( CFoundAddress::MaxArrayStride() );
											swsExpText += L'.';
											break;
										}
										ui32Stride = uint32_t( rRes.u.ui64Val );
									}
								}
								pwCurVal = FindChild( Layout::MX_FAEI_VALUE_CUR_VAL_COMBO );
								uint32_t ui32ArrayLen = ui32DesiredArrayLen;
								if ( pwCurVal ) {
									auto swsCurText = pwCurVal->GetTextW();
									if ( !swsCurText.size() && bIsMulti ) {
										// Empty string on multi-select means don't change the texts.
										swsExpText += _DEC_WS_43645600__multiple_items_;
									}
									else {
										swsExpText += IntegralToDesc( swsCurText, static_cast<CUtilities::MX_DATA_TYPES>(iType), bHex, ui32ArrayLen, ui32Stride, nullptr );
									}
								}

								if ( ui32DesiredArrayLen ) {
									if ( ui32ArrayLen > ui32DesiredArrayLen ) {
										swsExpText += L'\r';
										swsExpText += L'\n';
										swsExpText += _DEC_WS_8869E389_The_current_value_has_more_elements_than_specified_by_Array_Length_;
										break;
									}
								}

								// Current value handled.
								swsExpText += L'\r';
								swsExpText += L'\n';



								// ===================
								// VALUE WHEN LOCKED
								// ===================
								swsExpText += _DEC_WS_E64C54B0_Value_When_Locked__;

								pwCurVal = FindChild( Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_LEFT_COMBO );
								uint32_t ui32LeftArrayLen = ui32DesiredArrayLen;
								std::vector<ee::CExpEvalContainer::EE_RESULT> vLeftResults;
								if ( pwCurVal ) {
									auto swsCurText = pwCurVal->GetTextW();
									if ( !swsCurText.size() && bIsMulti ) {
										// Empty string on multi-select means don't change the texts.
										swsExpText += _DEC_WS_43645600__multiple_items_;
									}
									else {
										swsExpText += IntegralToDesc( swsCurText, static_cast<CUtilities::MX_DATA_TYPES>(iType), bHex, ui32LeftArrayLen, ui32Stride, &vLeftResults );
									}
								}

								if ( !ui32DesiredArrayLen ) {
									if ( ui32ArrayLen != ui32LeftArrayLen ) {
										swsExpText += L'\r';
										swsExpText += L'\n';
										swsExpText += _DEC_WS_DD4CBCAB_If_Array_Length_is_0__the_current_value_and_locked_value_s__must_have_the_same_number_of_elements_;
										break;
									}
								}
								else {
									if ( ui32LeftArrayLen > ui32DesiredArrayLen ) {
										swsExpText += L'\r';
										swsExpText += L'\n';
										swsExpText += _DEC_WS_FAD3DED1_The_locked_value_has_more_elements_than_specified_by_Array_Length_;
										break;
									}
								}

								if ( iValWhenLocked == LPARAM( CUtilities::MX_LT_RANGE ) ) {
									pwCurVal = FindChild( Layout::MX_FAEI_QL_VALUE_WHEN_LOCKED_RIGHT_COMBO );
									uint32_t ui32RighttArrayLen = ui32DesiredArrayLen;
									if ( pwCurVal ) {
										swsExpText += L'\r';
										swsExpText += L'\n';
										swsExpText += _DEC_WS_B09DF1A4_To_;
										swsExpText += L' ';
										auto swsCurText = pwCurVal->GetTextW();
										std::vector<ee::CExpEvalContainer::EE_RESULT> vRightResults;
										if ( !swsCurText.size() && bIsMulti ) {
											// Empty string on multi-select means don't change the texts.
											swsExpText += _DEC_WS_43645600__multiple_items_;
										}
										else {
											swsExpText += IntegralToDesc( swsCurText, static_cast<CUtilities::MX_DATA_TYPES>(iType), bHex, ui32RighttArrayLen, ui32Stride, &vRightResults );
										}

										// Min and max values must be checked against each other.
										size_t sLen = std::min<size_t>( vLeftResults.size(), vRightResults.size() );
										swsExpText += L'\r';
										swsExpText += L'\n';
										for ( size_t I = 0; I < sLen; ++I ) {
											ee::CExpEvalContainer::EE_RESULT rTmp;
											if ( ee::CExpEvalContainer::EE_EC_SUCCESS != ee::CExpEvalContainer::PerformOp_S( vLeftResults[I], '>', vRightResults[I], rTmp ) ) {
												swsExpText += _DEC_WS_247BC127_Internal_error_verifying_range_value_consistency_;
												break;
											}
											if ( rTmp.u.ui64Val ) {
												CSecureWString swsLeft, swsRight;
												CUtilities::ToDataTypeString( vLeftResults[I], static_cast<CUtilities::MX_DATA_TYPES>(iType), swsLeft );
												CUtilities::ToDataTypeString( vRightResults[I], static_cast<CUtilities::MX_DATA_TYPES>(iType), swsRight );
												//swsExpText += std::format( _DEC_WS_4B09D6E2_Invalid_range_min_max_____must_be_lower_than_____index_____, swsLeft, swsRight, I );
												swsExpText += _DEC_WS_E2E06A5A_Invalid_range_min_max__;
												swsExpText += swsLeft;
												swsExpText += _DEC_WS_4FBEB86D__is_greater_than_;
												swsExpText += swsRight;
												swsExpText += _DEC_WS_4F2FE7D3___index_;
												swsExpText += std::to_wstring( I );
												swsExpText += _DEC_WS_D94980DB___;

												if ( !ui32DesiredArrayLen ) {
													if ( ui32ArrayLen != ui32RighttArrayLen ) {
														swsExpText += L'\r';
														swsExpText += L'\n';
														swsExpText += _DEC_WS_DD4CBCAB_If_Array_Length_is_0__the_current_value_and_locked_value_s__must_have_the_same_number_of_elements_;
														break;
													}
												}
												else {
													if ( ui32RighttArrayLen > ui32DesiredArrayLen ) {
														swsExpText += L'\r';
														swsExpText += L'\n';
														swsExpText += _DEC_WS_40FB27FE_The_range_lock_max_value_has_more_elements_than_specified_by_Array_Length_;
														break;
													}
												}
												break;
											}
										}
									}
								}
							}
						} while ( false );
					}
				}
				



				pwLabel->SetTextW( swsExpText.c_str() );
			}
			catch ( ... ) {
				pwLabel->SetTextW( _DEC_WS_F39F91A5_Out_of_memory_.c_str() );
			}
		}
		

	}

	// Gathers all descriptions and returns the Description string.
	CSecureWString CFoundAddressEditMainPage::GatherDescriptions() const {
		if ( !m_pParms.pmhMemHack || !m_pParms.vSelection.size() ) { return CSecureWString(); }
		CSecureWString wswRet;
		auto famManager = m_pParms.pmhMemHack->FoundAddressManager();
		auto faTmp = famManager->GetById( m_pParms.vSelection[0] );
		if ( !faTmp ) { return CSecureWString(); }
		wswRet = faTmp->DescriptionText();

		for ( size_t I = 1; I < m_pParms.vSelection.size(); ++I ) {
			faTmp = famManager->GetById( m_pParms.vSelection[I] );
			if ( faTmp ) {
				if ( faTmp->DescriptionText() != wswRet ) { return CSecureWString(); }
			}
		}

		return wswRet;
	}

	// Gathers all colors.
	COLORREF CFoundAddressEditMainPage::GatherColors() const {
		if ( !m_pParms.pmhMemHack || !m_pParms.vSelection.size() ) { return RGB( 0xFF, 0xFF, 0xFF ); }
		COLORREF crRet = RGB( 0xFF, 0xFF, 0xFF );
		auto famManager = m_pParms.pmhMemHack->FoundAddressManager();
		bool bFound = false;

		for ( size_t I = 0; I < m_pParms.vSelection.size(); ++I ) {
			auto faTmp = famManager->GetById( m_pParms.vSelection[I] );
			if ( faTmp && faTmp->Type() == MX_FAT_FOUND_ADDRESS ) {
				auto rgbColor = reinterpret_cast<CFoundAddress *>(faTmp)->BaseColor();
				COLORREF crConverted = RGB( rgbColor.rgbRed, rgbColor.rgbGreen, rgbColor.rgbBlue );
				if ( !bFound ) {
					crRet = crConverted;
					bFound = true;
					continue;
				}
				if ( crConverted != crRet ) { return RGB( 0xFF, 0xFF, 0xFF ); }
			}
		}

		return crRet;
	}

	// Gathers all current value strings.
	CSecureWString CFoundAddressEditMainPage::GatherCurValue() const {
		if ( !m_pParms.pmhMemHack || !m_pParms.vSelection.size() ) { return CSecureWString(); }
		CSecureWString wswRet;
		auto famManager = m_pParms.pmhMemHack->FoundAddressManager();
		auto faTmp = famManager->GetById( m_pParms.vSelection[0] );
		if ( !faTmp ) { return CSecureWString(); }
		bool bRead;
		wswRet = faTmp->ValueText( &bRead );
		if ( !bRead ) { return CSecureWString(); }
		for ( size_t I = 1; I < m_pParms.vSelection.size(); ++I ) {
			faTmp = famManager->GetById( m_pParms.vSelection[I] );
			if ( faTmp ) {
				if ( faTmp->ValueText( &bRead ) != wswRet ) { return CSecureWString(); }
				if ( !bRead ) { return CSecureWString(); }
			}
		}

		return wswRet;
	}

	// Gathers all locked-value strings.
	CSecureWString CFoundAddressEditMainPage::GatherLockedValue() const {
		if ( !m_pParms.pmhMemHack || !m_pParms.vSelection.size() ) { return CSecureWString(); }
		CSecureWString wswRet;
		auto famManager = m_pParms.pmhMemHack->FoundAddressManager();
		auto faTmp = famManager->GetById( m_pParms.vSelection[0] );
		if ( !faTmp ) { return CSecureWString(); }
		wswRet = faTmp->ValueWhenLockedText();

		for ( size_t I = 1; I < m_pParms.vSelection.size(); ++I ) {
			faTmp = famManager->GetById( m_pParms.vSelection[I] );
			if ( faTmp ) {
				if ( faTmp->ValueWhenLockedText() != wswRet ) { return CSecureWString(); }
			}
		}

		return wswRet;
	}

	// Gathers all max locked-value strings.
	CSecureWString CFoundAddressEditMainPage::GatherMaxLockedValue() const {
		if ( !m_pParms.pmhMemHack || !m_pParms.vSelection.size() ) { return CSecureWString(); }
		CSecureWString wswRet;
		auto famManager = m_pParms.pmhMemHack->FoundAddressManager();
		auto faTmp = famManager->GetById( m_pParms.vSelection[0] );
		if ( !faTmp ) { return CSecureWString(); }
		wswRet = faTmp->MaxValueWhenLockedText();

		for ( size_t I = 1; I < m_pParms.vSelection.size(); ++I ) {
			faTmp = famManager->GetById( m_pParms.vSelection[I] );
			if ( faTmp ) {
				if ( faTmp->MaxValueWhenLockedText() != wswRet ) { return CSecureWString(); }
			}
		}

		return wswRet;
	}

	// Gathers all address strings.
	CSecureWString CFoundAddressEditMainPage::GatherAddress() const {
		if ( !m_pParms.pmhMemHack || !m_pParms.vSelection.size() ) { return CSecureWString(); }
		CSecureWString wswRet;
		auto famManager = m_pParms.pmhMemHack->FoundAddressManager();
		auto faTmp = famManager->GetById( m_pParms.vSelection[0] );
		if ( !faTmp ) { return CSecureWString(); }
		wswRet = faTmp->AddressText();

		for ( size_t I = 1; I < m_pParms.vSelection.size(); ++I ) {
			faTmp = famManager->GetById( m_pParms.vSelection[I] );
			if ( faTmp ) {
				if ( faTmp->AddressText() != wswRet ) { return CSecureWString(); }
			}
		}

		return wswRet;
	}

	// Gathers all data types.
	CUtilities::MX_DATA_TYPES CFoundAddressEditMainPage::GatherDataTypes() const {
		if ( !m_pParms.pmhMemHack || !m_pParms.vSelection.size() ) { return CUtilities::MX_DATA_TYPES( -1 ); }
		CUtilities::MX_DATA_TYPES dtRet = CUtilities::MX_DATA_TYPES( -1 );
		auto famManager = m_pParms.pmhMemHack->FoundAddressManager();
		bool bFound = false;

		for ( size_t I = 0; I < m_pParms.vSelection.size(); ++I ) {
			auto faTmp = famManager->GetById( m_pParms.vSelection[I] );
			if ( faTmp && faTmp->Type() == MX_FAT_FOUND_ADDRESS ) {
				if ( !bFound ) {
					dtRet = DataType( faTmp );
					bFound = true;
					continue;
				}
				if ( DataType( faTmp ) != dtRet ) { return CUtilities::MX_DATA_TYPES( -1 ); }
			}
		}

		return dtRet;
	}

	// Gathers all code pages.
	LRESULT CFoundAddressEditMainPage::GatherCodePages() const {
		if ( !m_pParms.pmhMemHack || !m_pParms.vSelection.size() ) { return LRESULT( -1 ); }
		LRESULT lrRet = LRESULT( -1 );
		auto famManager = m_pParms.pmhMemHack->FoundAddressManager();
		bool bFound = false;

		for ( size_t I = 0; I < m_pParms.vSelection.size(); ++I ) {
			auto faTmp = famManager->GetById( m_pParms.vSelection[I] );
			if ( faTmp && faTmp->Type() == MX_FAT_FOUND_ADDRESS ) {
				if ( !bFound ) {
					lrRet = reinterpret_cast<CFoundAddress *>(faTmp)->CodePage();
					bFound = true;
					continue;
				}
				if ( reinterpret_cast<CFoundAddress *>(faTmp)->CodePage() != lrRet ) { return LRESULT( -1 ); }
			}
		}

		return lrRet;
	}

	// Gathers all lock types.
	CUtilities::MX_LOCK_TYPES CFoundAddressEditMainPage::GatherLockTypes() const {
		if ( !m_pParms.pmhMemHack || !m_pParms.vSelection.size() ) { return CUtilities::MX_LOCK_TYPES( -1 ); }
		CUtilities::MX_LOCK_TYPES dtRet = CUtilities::MX_LOCK_TYPES( -1 );
		auto famManager = m_pParms.pmhMemHack->FoundAddressManager();
		bool bFound = false;

		for ( size_t I = 0; I < m_pParms.vSelection.size(); ++I ) {
			auto faTmp = famManager->GetById( m_pParms.vSelection[I] );
			if ( faTmp && faTmp->Type() == MX_FAT_FOUND_ADDRESS ) {
				if ( !bFound ) {
					dtRet = CUtilities::MX_LOCK_TYPES( LockType( faTmp ) );
					bFound = true;
					continue;
				}
				if ( CUtilities::MX_LOCK_TYPES( LockType( faTmp ) ) != dtRet ) { return CUtilities::MX_LOCK_TYPES( -1 ); }
			}
		}

		return dtRet;
	}

	// Gathers all locked states.
	int CFoundAddressEditMainPage::GatherLockedStates() const {
		if ( !m_pParms.pmhMemHack || !m_pParms.vSelection.size() ) { return BST_INDETERMINATE; }
		bool bLocked = false;
		auto famManager = m_pParms.pmhMemHack->FoundAddressManager();
		bool bFound = false;

		for ( size_t I = 0; I < m_pParms.vSelection.size(); ++I ) {
			auto faTmp = famManager->GetById( m_pParms.vSelection[I] );
			if ( faTmp && faTmp->Type() == MX_FAT_FOUND_ADDRESS ) {
				if ( !bFound ) {
					bLocked = reinterpret_cast<CFoundAddress *>(faTmp)->Locked();
					bFound = true;
					continue;
				}
				if ( reinterpret_cast<CFoundAddress *>(faTmp)->Locked() != bLocked ) { return BST_INDETERMINATE; }
			}
		}

		return bLocked ? BST_CHECKED : BST_UNCHECKED;
	}

	// Gathers all array lengths.
	CSecureWString CFoundAddressEditMainPage::GatherArrayLens() const {
		if ( !m_pParms.pmhMemHack || !m_pParms.vSelection.size() ) { return CSecureWString(); }
		CUtilities::MX_LOCK_TYPES dtRet = CUtilities::MX_LOCK_TYPES( -1 );
		auto famManager = m_pParms.pmhMemHack->FoundAddressManager();
		bool bFound = false;

		uint32_t ui32Len = 0;
		for ( size_t I = 0; I < m_pParms.vSelection.size(); ++I ) {
			auto faTmp = famManager->GetById( m_pParms.vSelection[I] );
			if ( faTmp && faTmp->Type() == MX_FAT_FOUND_ADDRESS ) {
				if ( !bFound ) {
					ui32Len = reinterpret_cast<CFoundAddress *>(faTmp)->ArrayLen();
					bFound = true;
					continue;
				}
				if ( reinterpret_cast<CFoundAddress *>(faTmp)->ArrayLen() != ui32Len ) { return CSecureWString(); }
			}
		}

		return CSecureWString( std::to_wstring( ui32Len ) );
	}

	// Gathers all array strides.
	CSecureWString CFoundAddressEditMainPage::GatherArrayStrides() const {
		if ( !m_pParms.pmhMemHack || !m_pParms.vSelection.size() ) { return CSecureWString(); }
		CUtilities::MX_LOCK_TYPES dtRet = CUtilities::MX_LOCK_TYPES( -1 );
		auto famManager = m_pParms.pmhMemHack->FoundAddressManager();
		bool bFound = false;

		uint16_t ui16Stride = 0;
		for ( size_t I = 0; I < m_pParms.vSelection.size(); ++I ) {
			auto faTmp = famManager->GetById( m_pParms.vSelection[I] );
			if ( faTmp && faTmp->Type() == MX_FAT_FOUND_ADDRESS ) {
				if ( !bFound ) {
					ui16Stride = reinterpret_cast<CFoundAddress *>(faTmp)->ArrayStride();
					bFound = true;
					continue;
				}
				if ( reinterpret_cast<CFoundAddress *>(faTmp)->ArrayStride() != ui16Stride ) { return CSecureWString(); }
			}
		}

		return CSecureWString( std::to_wstring( ui16Stride ) );
	}

	// Gets the lock type for a given found address.
	LPARAM CFoundAddressEditMainPage::LockType( CFoundAddressBase * _pfabFoundAddress ) {
		if ( _pfabFoundAddress->Type() != MX_FAT_FOUND_ADDRESS ) { return LPARAM( -1 ); }
		return LPARAM( reinterpret_cast<CFoundAddress *>(_pfabFoundAddress)->LockType() );
	}

	// Gets the data type for a given found address.
	CUtilities::MX_DATA_TYPES CFoundAddressEditMainPage::DataType( CFoundAddressBase * _pfabFoundAddress ) {
		if ( _pfabFoundAddress->Type() != MX_FAT_FOUND_ADDRESS ) { return CUtilities::MX_DATA_TYPES( -1 ); }
		if ( reinterpret_cast<CFoundAddress *>(_pfabFoundAddress)->ValueType() == CUtilities::MX_VT_STRING ) { return CUtilities::MX_DT_STRING; }
		if ( reinterpret_cast<CFoundAddress *>(_pfabFoundAddress)->ValueType() == CUtilities::MX_VT_BLOB ) { return CUtilities::MX_DATA_TYPES( -1 ); }
		return reinterpret_cast<CFoundAddress *>(_pfabFoundAddress)->DataType();
	}

	// Converts a string to its description.
	CSecureWString CFoundAddressEditMainPage::StringToDescr( const CSecureWString &_swsString, bool _bResolveEscapes, UINT _uiCodePage ) {
		CSecureWString swsExpText;
		CSecureWString swsCopy = _swsString;
		if ( _bResolveEscapes ) {
			CSecureString ssResolved;
			CUtilities::ResolveAllEscapes( ee::CExpEval::ToUtf8( swsCopy ), ssResolved, true );
			swsCopy = ee::CExpEval::ToUtf16( ssResolved );
		}
		CSecureString ssMultiByte;
		DWORD dwError;
		auto sCodePaged = CUtilities::WideCharToMultiByte( _uiCodePage, 0, swsCopy, nullptr, nullptr, &dwError );
		switch ( dwError ) {
			case ERROR_SUCCESS : {
				swsExpText += CUtilities::EscapeNulOnly( CUtilities::MultiByteToWideChar( _uiCodePage, 0, sCodePaged ), false, true );
				swsExpText += L'\r';
				swsExpText += L'\n';
				CSecureString ssTmp;
				CUtilities::BytesToHexWithSpaces( sCodePaged.data(), sCodePaged.size(), ssTmp );
				swsExpText += ee::CExpEval::ToUtf16( ssTmp );
				swsExpText += L' ';
				swsExpText += L'(';
				swsExpText += std::to_wstring( sCodePaged.size() );
				swsExpText += sCodePaged.size() == 1 ? _DEC_WS_4A6BA8AC__byte_ : _DEC_WS_0FCE4654__bytes_;
				break;
			}
			case ERROR_NOT_ENOUGH_MEMORY : {
				swsExpText += L'<';
				swsExpText += _DEC_WS_F39F91A5_Out_of_memory_;
				swsExpText += L'>';
				break;
			}
			default : {
				swsExpText += L'<';
				swsExpText += _DEC_WS_FA427FE5_String_conversion_error__;
				swsExpText += ee::CExpEval::ToUtf16( ee::CExpEval::ToHex( uint64_t( dwError ) ) );
				swsExpText += L'>';
			}
		}
		return swsExpText;
	}

	// Converts a float string to is description.
	CSecureWString CFoundAddressEditMainPage::FloatToDesc( const CSecureWString &_swsString, CUtilities::MX_DATA_TYPES _dtType, bool _bScentific, uint32_t &_ui32ArrayLen, uint32_t _ui32Stride, std::vector<ee::CExpEvalContainer::EE_RESULT> * _pvExps ) {
		//if ( !_swsString.size() ) { _ui32ArrayLen = 0; return CSecureWString(); }
		int32_t i32Digits = -MX_FLOAT64_SIG_DIG;
		switch ( _dtType ) {
			case CUtilities::MX_DT_FLOAT : {
				i32Digits = -MX_FLOAT32_SIG_DIG;
				break;
			}
			case CUtilities::MX_DT_FLOAT16 : {
				i32Digits = -MX_FLOAT16_SIG_DIG;
				break;
			}
		}
		if ( _bScentific ) {
			i32Digits = -i32Digits;
		}

		
		std::vector<std::vector<uint8_t>> vRawValues;
		bool bContiguous = _ui32Stride <= CUtilities::DataTypeSize( _dtType );
		CSecureWString swsError;
		_ui32ArrayLen = CUtilities::WStringToArrayBytes( vRawValues, _swsString, _dtType, _ui32ArrayLen, 0, bContiguous, swsError );
		if ( !_ui32ArrayLen ) {
			_ui32ArrayLen = 0; return swsError;
		}
		CSecureWString swsReturn;
		auto ui32BackToString = CUtilities::ArrayBytesToWString( vRawValues, nullptr, swsReturn, _dtType, _ui32ArrayLen, i32Digits, bContiguous, swsError );
		if ( !ui32BackToString ) {
			_ui32ArrayLen = 0; return swsError;
		}
		if ( _ui32ArrayLen != ui32BackToString ) {
			_ui32ArrayLen = 0; return _DEC_WS_1468A1DF_Internal_error_;
		}

		swsReturn += L' ';
		swsReturn += L'(';
		swsReturn += std::to_wstring( _ui32ArrayLen );
		swsReturn += _ui32ArrayLen == 1 ? _DEC_WS_1AEC022F__item_ : _DEC_WS_7B2F1364__items_;

		if ( _pvExps ) {
			CUtilities::ArrayBytesToResult( vRawValues, nullptr, _dtType, _ui32ArrayLen, bContiguous, (*_pvExps) );
		}
		return swsReturn;
	}

	// Converts a not-float string to its description.
	CSecureWString CFoundAddressEditMainPage::IntegralToDesc( const CSecureWString &_swsString, CUtilities::MX_DATA_TYPES _dtType, bool _bIsHex, uint32_t &_ui32ArrayLen, uint32_t _ui32Stride, std::vector<ee::CExpEvalContainer::EE_RESULT> * _pvExps ) {
		int iBase = _bIsHex ? 16 : -1;

		std::vector<std::vector<uint8_t>> vRawValues;
		bool bContiguous = _ui32Stride <= CUtilities::DataTypeSize( _dtType );
		CSecureWString swsError;
		_ui32ArrayLen = CUtilities::WStringToArrayBytes( vRawValues, _swsString, _dtType, _ui32ArrayLen, iBase, bContiguous, swsError );
		if ( !_ui32ArrayLen ) {
			_ui32ArrayLen = 0; return swsError;
		}
		CSecureWString swsReturn;
		auto ui32BackToString = CUtilities::ArrayBytesToWString( vRawValues, nullptr, swsReturn, _dtType, _ui32ArrayLen, 0, bContiguous, swsError );
		if ( !ui32BackToString ) {
			_ui32ArrayLen = 0; return swsError;
		}
		if ( _ui32ArrayLen != ui32BackToString ) {
			_ui32ArrayLen = 0; return _DEC_WS_1468A1DF_Internal_error_;
		}

		swsReturn += L' ';
		swsReturn += L'(';
		swsReturn += std::to_wstring( _ui32ArrayLen );
		swsReturn += _ui32ArrayLen == 1 ? _DEC_WS_1AEC022F__item_ : _DEC_WS_7B2F1364__items_;

		if ( _pvExps ) {
			CUtilities::ArrayBytesToResult( vRawValues, nullptr, _dtType, _ui32ArrayLen, bContiguous, (*_pvExps) );
		}
		return swsReturn;
	}

}	// namespace mx
