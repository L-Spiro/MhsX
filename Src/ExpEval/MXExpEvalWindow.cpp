#include "MXExpEvalWindow.h"
#include "../Layouts/MXExpressionEvaluatorLayout.h"
#include "../System/MXSystem.h"

#include <Object/EEObject.h>
#include <Rebar/LSWRebar.h>
#include <ToolBar/LSWToolBar.h>
//#include <Unicode/EEUnicode.h>	// TEMP.

namespace mx {

	CExpEvalWindow::CExpEvalWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CDockable( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_uiptrUpdateListTimer( 0 ),
		m_uiUpdateSpeed( 100 ),
		m_dDecoding( CUtilities::MX_D_STANDARD ) {
		static const struct {
			LPCWSTR				lpwsImageName;
			DWORD				dwConst;
		} sImages[] = {
			{ L"60", MX_I_ADD },
			{ L"59", MX_I_REMOVE },
			{ L"50", MX_I_SCIENTIFIC_NOTATION },
			//{ L"32", MX_I_EXT_OUTPUT },
		};
		m_iImages.Create( 24, 24, ILC_COLOR32, MX_I_TOTAL, MX_I_TOTAL );

		for ( size_t I = 0; I < MX_ELEMENTS( sImages ); ++I ) {
			CSecureWString wsTemp = CSystem::GetResourcesPathW();
			wsTemp += sImages[I].lpwsImageName;
			wsTemp += L".bmp";

			m_bBitmaps[sImages[I].dwConst].LoadFromFile( wsTemp.c_str(), 0, 0, LR_CREATEDIBSECTION );
			m_iImageMap[sImages[I].dwConst] = m_iImages.Add( m_bBitmaps[sImages[I].dwConst].Handle() );
		}

	}
	CExpEvalWindow::~CExpEvalWindow() {
		if ( m_uiptrUpdateListTimer ) {
			CSystem::KillTimer( Wnd(), m_uiptrUpdateListTimer );
		}
		for ( size_t I = 0; I < m_vExpressions.size(); ++I ) {
			delete m_vExpressions[I];
			m_vExpressions[I] = nullptr;
		}
		m_vExpressions.clear();
	}

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CExpEvalWindow::InitDialog() {
		CToolBar * plvToolBar = static_cast<CToolBar *>(FindChild( CExpressionEvaluatorLayout::MX_EE_TOOLBAR0 ));
		CRebar * plvRebar = static_cast<CRebar *>(FindChild( CExpressionEvaluatorLayout::MX_EE_REBAR0 ));


		// ==== TOOL BAR ==== //
		if ( plvToolBar ) {
			plvToolBar->SetImageList( 0, m_iImages );
#define MX_TOOL_STR( TXT )					reinterpret_cast<INT_PTR>(TXT)
//#define MX_TOOL_STR( TXT )					0
			const TBBUTTON bButtons[] = {
				// iBitmap									idCommand									fsState				fsStyle			bReserved	dwData	iString
				{ m_iImageMap[MX_I_ADD],					CExpressionEvaluatorLayout::MX_BC_ADD,		TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Add" ) },
				{ m_iImageMap[MX_I_REMOVE],					CExpressionEvaluatorLayout::MX_BC_REMOVE,	TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Remove" ) },
				{ -1,										0,											TBSTATE_ENABLED,	BTNS_SEP,		{ 0 },		0,		0 },
				{ m_iImageMap[MX_I_SCIENTIFIC_NOTATION],	CExpressionEvaluatorLayout::MX_BC_SCINOT,	TBSTATE_ENABLED,	BTNS_CHECK,		{ 0 },		0,		MX_TOOL_STR( L"Scientific" ) },
				//{ m_iImageMap[MX_I_EXT_OUTPUT],				CExpressionEvaluatorLayout::MX_BC_EXTOUT,	TBSTATE_ENABLED,	BTNS_CHECK,		{ 0 },		0,		MX_TOOL_STR( L"Ext. Output" ) },
			};
#undef MX_TOOL_STR

			plvToolBar->AddButtons( bButtons, MX_ELEMENTS( bButtons ) );

			if ( plvRebar ) {
				plvRebar->SetImageList( m_iImages );
				{
					LSW_REBARBANDINFO riRebarInfo;
					riRebarInfo.SetColors( ::GetSysColor( COLOR_BTNTEXT ), ::GetSysColor( COLOR_BTNFACE ) );
					riRebarInfo.SetStyle( RBBS_CHILDEDGE |
					  RBBS_FIXEDBMP );
					riRebarInfo.SetChild( plvToolBar->Wnd() );
					riRebarInfo.SetChildSize( plvToolBar->GetMinBoundingRect().Width(), plvToolBar->GetMinBoundingRect().Height() );
					riRebarInfo.SetId( CExpressionEvaluatorLayout::MX_EE_TOOLBAR0 );
					plvRebar->InsertBand( -1, riRebarInfo );
				}
			}
		}


		if ( plvRebar ) {
			LSW_RECT rRebarRect = ClientRect( this );
			::MoveWindow( plvRebar->Wnd(), 0, 0, rRebarRect.Width(), plvRebar->WindowRect( this ).Height(), FALSE );

			plvRebar->UpdateRects();
		}


		// ==== INPUT EDIT ==== //
		CEdit * peEdit = InputEdit();
		if ( peEdit ) {
			peEdit->LimitText( 0x7FFFFFFFE );
		}


		// ==== OUTPUT COMBO ==== //
		CComboBox * pcbOutputFormat = OutputCombo();
		if ( pcbOutputFormat ) {
			const struct {
				const char *			pcText;
				size_t					sLen;
				CUtilities::MX_DECODING	dParam;
			} aTitles[] = {
				{ _T_LEN_E981B5D1_Standard, CUtilities::MX_D_STANDARD },
				{ _T_LEN_DCCBE329_Extended, CUtilities::MX_D_EXTENDED },
				{ _T_LEN_4F544F37_Time___Date, CUtilities::MX_D_TIME },
			};
			for ( size_t I = 0; I < MX_ELEMENTS( aTitles ); I++ ) {
				INT iIndex = pcbOutputFormat->AddString( mx::CStringDecoder::DecodeToWString( aTitles[I].pcText, aTitles[I].sLen ).c_str() );
				if ( iIndex != -1 ) {
					pcbOutputFormat->SetItemData( iIndex, static_cast<LPARAM>(aTitles[I].dParam) );
				}
			}
			pcbOutputFormat->SetCurSel( 0 );


			/*for ( size_t I = 0; I < ee::CUnicode::TotalNames(); I++ ) {
				std::wstring sTemp;
				ee::CUnicode::GetName( sTemp, I );
				pcbOutputFormat->AddString( sTemp.c_str() );
			}*/


			/*LSW_RECT rTemp = pcbOutputFormat->GetItemRect( 0 );
			rTemp = pcbOutputFormat->GetItemRect( 1 );
			rTemp = pcbOutputFormat->GetItemRect( 0 );*/
			pcbOutputFormat->AutoSetMinListWidth();
		}

		// ==== LIST VIEW ==== //
		CListView * plvAddressList = ListView();
		if ( plvAddressList ) {
			const struct {
				const char * _pcText;
				size_t sLen;
				DWORD dwWidth;
			} aTitles[] = {
				{ _T_976D55D1_Expression, _LEN_976D55D1, 120 },
				{ _T_14C6C425_Result, _LEN_14C6C425, 200 },
			};
			for ( INT I = 0; I < MX_ELEMENTS( aTitles ); I++ ) {
				INT iCol = plvAddressList->AddColumn( mx::CStringDecoder::DecodeToWString( aTitles[I]._pcText, aTitles[I].sLen ).c_str() );
				BOOL bAdded = plvAddressList->SetColumnWidth( iCol, aTitles[I].dwWidth );
				if ( !bAdded ) { break; }
			}
			//plvAddressList->SetColumnWidth( plvAddressList->GetTotalColumns(), LVSCW_AUTOSIZE_USEHEADER );
		}


		ForceSizeUpdate();

		std::random_device rdRand;
		std::mt19937 mGen( rdRand() );
		std::uniform_int_distribution<> uidDist( MX_T_UPDATE_LIST, MX_T_UPDATE_LIST + 16 );
		m_uiptrUpdateListTimer = CSystem::SetTimer( Wnd(), uidDist( mGen ), m_uiUpdateSpeed, NULL );
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CExpEvalWindow::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {
		switch ( _Id ) {
			case CExpressionEvaluatorLayout::MX_BC_ADD : {
				CEdit * pcbCombo = InputEdit();
				if ( pcbCombo ) {
					//CSecureString sText = pcbCombo->GetTextA();
					CSecureWString wsText = pcbCombo->GetTextW();
					CSecureString ssUtf8 = ee::CExpEval::ToUtf8( wsText );
					AddExpression( ssUtf8.c_str() );
				}				
				break;
			}
			case CExpressionEvaluatorLayout::MX_BC_REMOVE : {
				RemoveSelectedExpressions();
				break;
			}
			case CExpressionEvaluatorLayout::MX_BC_SCINOT : {
				UpdateResult();
				break;
			}
			case CExpressionEvaluatorLayout::MX_EE_EXP_EDIT : {
				switch ( _wCtrlCode ) {
					case EN_CHANGE : {
						UpdateResult();
						break;
					}
				}
				break;
			}
			case CExpressionEvaluatorLayout::MX_EE_EXP_OUTPUTCOMBO : {
				switch ( _wCtrlCode ) {
					case CBN_SELCHANGE : {
						CComboBox * pcbOutputFormat = OutputCombo();
						m_dDecoding = pcbOutputFormat ?
							static_cast<CUtilities::MX_DECODING>(pcbOutputFormat->GetItemData( pcbOutputFormat->GetCurSel() )) :
							CUtilities::MX_D_STANDARD;
						UpdateResult();
						break;
					}
				}
				break;
			}
		}
		return LSW_H_CONTINUE;
	}

	// Virtual client rectangle.  Can be used for things that need to be adjusted based on whether or not status bars, toolbars, etc. are present.
	const LSW_RECT CExpEvalWindow::VirtualClientRect( const CWidget * pwChild ) const {
		LSW_RECT rTemp = ClientRect( this );
		const CRebar * plvRebar = static_cast<const CRebar *>(FindChild( CExpressionEvaluatorLayout::MX_EE_REBAR0 ));
		if ( plvRebar ) {
			LSW_RECT rRebar = plvRebar->ClientRect( this );
			rTemp.top += rRebar.Height();
		}
		return rTemp;
	}

	// Gets a pointer to the list view.
	CListView * CExpEvalWindow::ListView() {
		return static_cast<CListView *>(FindChild( CExpressionEvaluatorLayout::MX_EE_LIST ));
	}

	// Gets a pointer to the input combo box.
	CEdit * CExpEvalWindow::InputEdit() {
		return static_cast<CEdit *>(FindChild( CExpressionEvaluatorLayout::MX_EE_EXP_EDIT ));
	}

	// Gets a pointer to the output combo box.
	CComboBox * CExpEvalWindow::OutputCombo() {
		return static_cast<CComboBox *>(FindChild( CExpressionEvaluatorLayout::MX_EE_EXP_OUTPUTCOMBO ));
	}

	// Gets a pointer to the result edit.
	CEdit * CExpEvalWindow::Edit() {
		return static_cast<CEdit *>(FindChild( CExpressionEvaluatorLayout::MX_EE_EXP_RESULT ));
	}

	// Updates the text result.
	void CExpEvalWindow::UpdateResult() {
		CEdit * peEdit = Edit();
		if ( !peEdit ) { return; }
		CEdit * pcbCombo = InputEdit();
		if ( !pcbCombo ) { return; }
		ee::CExpEvalContainer::EE_RESULT eResult;
		BOOL bValid;
		pcbCombo->SetAddressHandler( m_pfahAddressHandler, m_uiptrAddressHandlerData );
		pcbCombo->SetAddressWriteHandler( m_pfahAddressWriteHandler, m_uiptrAddressWriteHandlerData );
		std::string sObj;
		BOOL bSuccess = pcbCombo->GetTextAsExpression( eResult, &bValid, &sObj );

		if ( bSuccess ) {
			if ( eResult.ncType == ee::EE_NC_OBJECT ) {
				peEdit->SetTextW( ee::CExpEval::StringToWString( sObj ).c_str() );
			}
			else {
				std::wstring sTemp;
				CToolBar * plvToolBar = static_cast<CToolBar *>(FindChild( CExpressionEvaluatorLayout::MX_EE_TOOLBAR0 ));
				int32_t iSciNot = 0;
				if ( plvToolBar ) {
					iSciNot = plvToolBar->IsChecked( CExpressionEvaluatorLayout::MX_BC_SCINOT ) ? DBL_DECIMAL_DIG : 0;
				}
			
				CUtilities::PrintExpResult( eResult, sTemp, iSciNot, m_dDecoding );
				peEdit->SetTextW( sTemp.c_str() );
			}
		}
		else {
			peEdit->SetTextA( bValid ? _DEC_S_533B8966_Unresolvable.c_str() : _DEC_S_3424431C_Invalid.c_str() );
		}
	}

	// Adds an expression to the list.
	void CExpEvalWindow::AddExpression( const char * _pcExp ) {
		// Must point somewhere and not be empty.
		if ( _pcExp && (*_pcExp) != '\0' ) {
			CListView * plvList = ListView();
			if ( plvList ) {
				LVITEMW iItem = {
					LVIF_TEXT
				};
				CSecureWString wsExp = ee::CExpEval::ToUtf16( _pcExp );
				iItem.pszText = const_cast<LPWSTR>(wsExp.c_str());
				INT iIdx = plvList->InsertItem( iItem );
				m_vExpressions.insert( m_vExpressions.begin() + iIdx, new CExpression() );
				if ( m_vExpressions[iIdx] ) {
					m_vExpressions[iIdx]->SetExpression( _pcExp );
				}
			}
		}
	}

	// Removes the selected expressions from the list.
	void CExpEvalWindow::RemoveSelectedExpressions() {
		CListView * plvList = ListView();
		if ( plvList ) {
			for ( int I = plvList->GetItemCount(); --I >= 0; ) {
				if ( plvList->GetItemState( I, LVIS_SELECTED ) ) {
					plvList->DeleteItem( I );
					delete m_vExpressions[I];
					m_vExpressions.erase( m_vExpressions.begin() + I );
				}
			}
		}
	}

	// Sets the update speed.
	void CExpEvalWindow::SetUpdateSpeed( uint32_t _ui32Speed ) {
		m_uiUpdateSpeed = _ui32Speed;
		if ( m_uiptrUpdateListTimer ) {
			// Overwrites the existing timer. ID does not change, meaning = is technically redundant, but let's assume
			//	there is a possibility later it could change.  Solid code.
			m_uiptrUpdateListTimer = CSystem::SetTimer( Wnd(), m_uiptrUpdateListTimer, m_uiUpdateSpeed, NULL );
		}
	}

	// == Functions.
	// WM_CTLCOLORSTATIC.
	CWidget::LSW_HANDLED CExpEvalWindow::CtlColorStatic( HDC _hDc, CWidget * _pwControl, HBRUSH &_hBrush ) {
		if ( _pwControl && _pwControl->Id() == CExpressionEvaluatorLayout::MX_EE_EXP_RESULT ) {
			_hBrush = ::GetSysColorBrush( COLOR_WINDOW );
			return LSW_H_HANDLED;
		}
		return LSW_H_CONTINUE;
	}

	// WM_TIMER.
	CWidget::LSW_HANDLED CExpEvalWindow::Timer( UINT_PTR _uiptrId, TIMERPROC _tpProc ) {
		if ( _uiptrId == m_uiptrUpdateListTimer ) {
			std::string sObj;
			// Update the list.
			CListView * plvList = ListView();
			if ( plvList ) {
				int iTotal = plvList->GetItemCount();
				for ( int I = 0; I < iTotal; ++I ) {
					if ( plvList->IsItemVisible( I ) ) {
						if ( I < m_vExpressions.size() ) {
							if ( m_vExpressions[I]->GetContainer() ) {
								ee::CExpEvalContainer::EE_RESULT eResult;
								if ( m_vExpressions[I]->GetContainer()->Resolve( eResult ) ) {
									if ( eResult.ncType == ee::EE_NC_OBJECT ) {
										ee::CObject * poObj = eResult.u.poObj;
										eResult.u.poObj = nullptr;
										if ( poObj ) {
											if ( poObj->ToString( sObj ) ) {
												plvList->SetItemText( I, 1, ee::CExpEval::StringToWString( sObj ).c_str() );
												continue;
											}
										}
										plvList->SetTextA( _DEC_S_533B8966_Unresolvable.c_str() );
										continue;
									}
									std::wstring sTemp;
									CToolBar * plvToolBar = static_cast<CToolBar *>(FindChild( CExpressionEvaluatorLayout::MX_EE_TOOLBAR0 ));
									int32_t iSciNot = -3;
									if ( plvToolBar ) {
										iSciNot = plvToolBar->IsChecked( CExpressionEvaluatorLayout::MX_BC_SCINOT ) ? DBL_DECIMAL_DIG : -(DBL_DECIMAL_DIG >> 0);
									}
									CComboBox * pcbOutputFormat = OutputCombo();
									LPARAM lpDecoder = pcbOutputFormat ?
										pcbOutputFormat->GetItemData( pcbOutputFormat->GetCurSel() ) :
										CUtilities::MX_D_STANDARD;

									CUtilities::PrintExpResult( eResult, sTemp, iSciNot, m_dDecoding );
									plvList->SetItemText( I, 1, sTemp.c_str() );
									continue;
								}
								else {
									plvList->SetTextA( _DEC_S_533B8966_Unresolvable.c_str() );
									continue;
								}
							}
						}
						plvList->SetTextA( _DEC_S_3424431C_Invalid.c_str() );
						/*CSecureString sText;
						plvList->GetItemText( I, 0, sText );
						ee::CExpEvalContainer::EE_RESULT eResult;
						CExpression eExp;
						if ( eExp.SetExpression( sText.c_str() ) ) {
							if ( eExp.GetContainer() ) {
								if ( eExp.GetContainer()->Resolve( eResult ) ) {
									std::wstring sTemp;
									CToolBar * plvToolBar = static_cast<CToolBar *>(FindChild( CExpressionEvaluatorLayout::MX_EE_TOOLBAR0 ));
									int32_t iSciNot = -1;
									if ( plvToolBar ) {
										iSciNot = plvToolBar->IsChecked( CExpressionEvaluatorLayout::MX_BC_SCINOT ) ? DBL_DECIMAL_DIG : -1;
									}
									CComboBox * pcbOutputFormat = OutputCombo();
									LPARAM lpDecoder = pcbOutputFormat ?
										pcbOutputFormat->GetItemData( pcbOutputFormat->GetCurSel() ) :
										CUtilities::MX_D_STANDARD;

									CUtilities::PrintExpResult( eResult, sTemp, iSciNot, m_dDecoding );
									plvList->SetItemText( I, 1, sTemp.c_str() );
									continue;
								}
								else {
									plvList->SetTextA( _DEC_S_533B8966_Unresolvable.c_str() );
									continue;
								}
							}
						}
						plvList->SetTextA( _DEC_S_3424431C_Invalid.c_str() );*/
					}
				}
			}
		}
		return LSW_H_CONTINUE;
	}

}	// namespace mx
