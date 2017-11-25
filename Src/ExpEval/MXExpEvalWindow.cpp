#include "MXExpEvalWindow.h"
#include "../Layouts/MXExpressionEvaluatorLayout.h"
#include "../System/MXSystem.h"
#include "../Utilities/MXUtilities.h"

#include <Rebar/LSWRebar.h>
#include <ToolBar/LSWToolBar.h>

namespace mx {

	CExpEvalWindow::CExpEvalWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu ) :
		lsw::CDockable( _wlLayout, _pwParent, _bCreateWidget, _hMenu ) {
		static const struct {
			LPCWSTR				lpwsImageName;
			DWORD				dwConst;
#if 0
		} sImages[] = {
			{ L"60s", MX_I_ADD },
			{ L"59s", MX_I_REMOVE },
		};
		m_iImages.Create( 16, 16, ILC_COLOR32, MX_I_TOTAL, MX_I_TOTAL );
#else
		} sImages[] = {
			{ L"60", MX_I_ADD },
			{ L"59", MX_I_REMOVE },
		};
		m_iImages.Create( 24, 24, ILC_COLOR32, MX_I_TOTAL, MX_I_TOTAL );
#endif	// #if 1

		for ( size_t I = 0; I < MX_I_TOTAL; ++I ) {
			std::wstring wsTemp = L"Resources/";
			wsTemp += sImages[I].lpwsImageName;
			wsTemp += L".bmp";

			m_bBitmaps[sImages[I].dwConst].LoadFromFile( wsTemp.c_str(), 0, 0, LR_CREATEDIBSECTION );
			m_iImageMap[sImages[I].dwConst] = m_iImages.Add( m_bBitmaps[sImages[I].dwConst].Handle() );
		}
	}
	CExpEvalWindow::~CExpEvalWindow() {
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
//#define MX_TOOL_STR( TXT )						0
			const TBBUTTON bButtons[] = {
				// iBitmap							idCommand									fsState				fsStyle			bReserved	dwData	iString
				{ m_iImageMap[MX_I_ADD],			CExpressionEvaluatorLayout::MX_BC_ADD,		TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Add" ) },
				{ m_iImageMap[MX_I_REMOVE],			CExpressionEvaluatorLayout::MX_BC_REMOVE,	TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Remove") },
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


		{
			LSW_RECT rRect;
			// Send a fake WM_SIZE message to cause the window to recalculate and redraw its layout.
			::GetClientRect( Wnd(), &rRect );
			::SendMessageW( Wnd(), WM_SIZE, SIZE_RESTORED, MAKELPARAM( rRect.Width(), rRect.Height() ) );
		}

		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CExpEvalWindow::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {
		switch ( _Id ) {
			case CExpressionEvaluatorLayout::MX_EE_EXP_COMBO : {
				CEdit * peEdit = Edit();
				if ( !peEdit ) { return LSW_H_CONTINUE; }
				switch ( _wCtrlCode ) {
					case CBN_EDITCHANGE : {
						ee::CExpEvalContainer::EE_RESULT eResult;
						BOOL bValid;
						BOOL bSuccess = _pwSrc->GetTextAsExpression( eResult, &bValid );
						
						if ( bSuccess ) {
							std::string sTemp;
							CUtilities::PrintExpResult( eResult, sTemp );
							peEdit->SetTextA( sTemp.c_str() );
						}
						else {
							peEdit->SetTextA( bValid ? _DEC_S_533B8966_Unresolvable.c_str() : _DEC_S_3424431C_Invalid.c_str() );
						}
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

	// Gets a pointer to the result edit.
	CEdit * CExpEvalWindow::Edit() {
		return static_cast<CEdit *>(FindChild( CExpressionEvaluatorLayout::MX_EE_EXP_RESULT ));
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

}	// namespace mx
