#include "MXConverterWindow.h"
#include "../Layouts/MXConverterLayout.h"
#include "../System/MXSystem.h"
#include "../Utilities/MXUtilities.h"

#include <Rebar/LSWRebar.h>
#include <ToolBar/LSWToolBar.h>

using namespace lsw;

namespace mx {

	// == Members.
	// The edit color for rare formats.
	HBRUSH CConverterWindow::m_hRarest = nullptr;

	// The edit color for float formats.
	HBRUSH CConverterWindow::m_hFloats = nullptr;

	// The edit color for signed formats.
	HBRUSH CConverterWindow::m_hSign = nullptr;

	// The edit color for unsigned formats.
	HBRUSH CConverterWindow::m_hUnsign = nullptr;

	// The edit color for the hex edit.
	HBRUSH CConverterWindow::m_hHex = nullptr;

	// The edit color for the binary edit.
	HBRUSH CConverterWindow::m_hBinary = nullptr;

	// The edit color for the string edits.
	HBRUSH CConverterWindow::m_hStrings = nullptr;

	// The interlocked counter.
	LONG CConverterWindow::m_lCount = 0;

	// Data for each edit.
	CConverterWindow::MX_CONV_INFO CConverterWindow::m_ciInfo[] = {
		{ CConverterLayout::MX_CWI_INT8, ee::EE_NC_SIGNED, (sizeof( int8_t ) * 8), false, false, GetValue, SetValueSigned<int8_t> },
		{ CConverterLayout::MX_CWI_UINT8, ee::EE_NC_UNSIGNED, (sizeof( uint8_t ) * 8), false, false, GetValue, SetValueUnsigned<uint8_t> },

		{ CConverterLayout::MX_CWI_CHAR, ee::EE_NC_SIGNED, (sizeof( char ) * 8), false, false, GetValueChar, SetValueChar },
		{ CConverterLayout::MX_CWI_UTF8, ee::EE_NC_SIGNED, (sizeof( char ) * 8), false, false, GetValueUtf8, SetValueUtf8 },

		{ CConverterLayout::MX_CWI_INT16_LE, ee::EE_NC_SIGNED, (sizeof( int16_t ) * 8), false, false, GetValue, SetValueSigned<int16_t> },
		{ CConverterLayout::MX_CWI_INT16_BE, ee::EE_NC_SIGNED, (sizeof( int16_t ) * 8), true, false, GetValue, SetValueSigned<int16_t> },
		{ CConverterLayout::MX_CWI_UINT16_LE, ee::EE_NC_UNSIGNED, (sizeof( uint16_t ) * 8), false, false, GetValue, SetValueUnsigned<uint16_t> },
		{ CConverterLayout::MX_CWI_UINT16_BE, ee::EE_NC_UNSIGNED, (sizeof( uint16_t ) * 8), true, false, GetValue, SetValueUnsigned<uint16_t> },

		{ CConverterLayout::MX_CWI_INT32_LE, ee::EE_NC_SIGNED, (sizeof( int32_t ) * 8), false, false, GetValue, SetValueSigned<int32_t> },
		{ CConverterLayout::MX_CWI_INT32_BE, ee::EE_NC_SIGNED, (sizeof( int32_t ) * 8), true, false, GetValue, SetValueSigned<int32_t> },
		{ CConverterLayout::MX_CWI_UINT32_LE, ee::EE_NC_UNSIGNED, (sizeof( uint32_t ) * 8), false, false, GetValue, SetValueUnsigned<uint32_t> },
		{ CConverterLayout::MX_CWI_UINT32_BE, ee::EE_NC_UNSIGNED, (sizeof( uint32_t ) * 8), true, false, GetValue, SetValueUnsigned<uint32_t> },

		{ CConverterLayout::MX_CWI_INT64_LE, ee::EE_NC_SIGNED, (sizeof( int64_t ) * 8), false, false, GetValue, SetValueSigned<int64_t> },
		{ CConverterLayout::MX_CWI_INT64_BE, ee::EE_NC_SIGNED, (sizeof( int64_t ) * 8), true, false, GetValue, SetValueSigned<int64_t> },
		{ CConverterLayout::MX_CWI_UINT64_LE, ee::EE_NC_UNSIGNED, (sizeof( uint64_t ) * 8), false, false, GetValue, SetValueUnsigned<uint64_t> },
		{ CConverterLayout::MX_CWI_UINT64_BE, ee::EE_NC_UNSIGNED, (sizeof( uint64_t ) * 8), true, false, GetValue, SetValueUnsigned<uint64_t> },

		{ CConverterLayout::MX_CWI_UTF16_LE, ee::EE_NC_SIGNED, (sizeof( int16_t ) * 8), false, false, GetValueUtf16, SetValueUtf16 },
		{ CConverterLayout::MX_CWI_UTF16_BE, ee::EE_NC_SIGNED, (sizeof( int16_t ) * 8), false, false, GetValueUtf16_BE, SetValueUtf16_BE },
		{ CConverterLayout::MX_CWI_UTF32_LE, ee::EE_NC_SIGNED, (sizeof( int32_t ) * 8), false, false, GetValueUtf32, SetValueUtf32 },
		{ CConverterLayout::MX_CWI_UTF32_BE, ee::EE_NC_SIGNED, (sizeof( int32_t ) * 8), false, false, GetValueUtf32_BE, SetValueUtf32_BE },

		{ CConverterLayout::MX_CWI_FLOAT10_LE, ee::EE_NC_FLOATING, 10, false, false, GetValue, SetValueFloating<5, 6, true, false, MX_FLOAT10_SIG_DIG> },
		{ CConverterLayout::MX_CWI_FLOAT10_BE, ee::EE_NC_FLOATING, 10, true, false, GetValue, SetValueFloating<5, 6, true, false, MX_FLOAT10_SIG_DIG> },

		{ CConverterLayout::MX_CWI_FLOAT11_LE, ee::EE_NC_FLOATING, 11, false, false, GetValue, SetValueFloating<5, 7, true, false, MX_FLOAT11_SIG_DIG> },
		{ CConverterLayout::MX_CWI_FLOAT11_BE, ee::EE_NC_FLOATING, 11, true, false, GetValue, SetValueFloating<5, 7, true, false, MX_FLOAT11_SIG_DIG> },

		{ CConverterLayout::MX_CWI_FLOAT14_LE, ee::EE_NC_FLOATING, 14, false, false, GetValue, SetValueFloating<5, 10, true, false, MX_FLOAT14_SIG_DIG> },
		{ CConverterLayout::MX_CWI_FLOAT14_BE, ee::EE_NC_FLOATING, 14, true, false, GetValue, SetValueFloating<5, 10, true, false, MX_FLOAT14_SIG_DIG> },

		{ CConverterLayout::MX_CWI_FLOAT16_LE, ee::EE_NC_FLOATING, 16, false, false, GetValue, SetValueFloating<5, 11, true, true, MX_FLOAT16_SIG_DIG> },
		{ CConverterLayout::MX_CWI_FLOAT16_BE, ee::EE_NC_FLOATING, 16, true, false, GetValue, SetValueFloating<5, 11, true, true, MX_FLOAT16_SIG_DIG> },

		{ CConverterLayout::MX_CWI_FLOAT24_LE, ee::EE_NC_FLOATING, 24, false, false, GetValue, SetValueFloating<7, 16, false, true, MX_FLOAT24_SIG_DIG> },
		{ CConverterLayout::MX_CWI_FLOAT24_BE, ee::EE_NC_FLOATING, 24, true, false, GetValue, SetValueFloating<7, 16, false, true, MX_FLOAT24_SIG_DIG> },

		{ CConverterLayout::MX_CWI_FLOAT32_LE, ee::EE_NC_FLOATING, 32, false, false, GetValue, SetValueFloating<8, 24, true, true, MX_FLOAT32_SIG_DIG> },
		{ CConverterLayout::MX_CWI_FLOAT32_BE, ee::EE_NC_FLOATING, 32, true, false, GetValue, SetValueFloating<8, 24, true, true, MX_FLOAT32_SIG_DIG> },

		{ CConverterLayout::MX_CWI_FLOAT64_LE, ee::EE_NC_FLOATING, 64, false, false, GetValue, SetValueFloating<11, 53, true, true, MX_FLOAT64_SIG_DIG> },
		{ CConverterLayout::MX_CWI_FLOAT64_BE, ee::EE_NC_FLOATING, 64, true, false, GetValue, SetValueFloating<11, 53, true, true, MX_FLOAT64_SIG_DIG> },

		{ CConverterLayout::MX_CWI_HEX_LE, ee::EE_NC_UNSIGNED, 64, false, true, GetValueHex, SetValueHex },
		{ CConverterLayout::MX_CWI_HEX_BE, ee::EE_NC_UNSIGNED, 64, true, true, GetValueHex, SetValueHex },

		{ CConverterLayout::MX_CWI_BINARY_LE, ee::EE_NC_UNSIGNED, 64, false, false, GetValueBinary, SetValueBinary },
		{ CConverterLayout::MX_CWI_BINARY_BE, ee::EE_NC_UNSIGNED, 64, true, false, GetValueBinary, SetValueBinary },
	};

	CConverterWindow::CConverterWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CDockable( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_wLastUpdateCtl( 0 ) {

		if ( ::InterlockedIncrement( &m_lCount ) == 1 ) {
			{
				LOGBRUSH lbBrush = {
					BS_SOLID,
					CHelpers::MixColorRef( ::GetSysColor( COLOR_WINDOW ), ::GetSysColor( COLOR_ACTIVECAPTION ), 0.5 )
				};
				m_hRarest = ::CreateBrushIndirect( &lbBrush );
			}
			{
				LOGBRUSH lbBrush = {
					BS_SOLID,
					CHelpers::MixColorRef( ::GetSysColor( COLOR_WINDOW ), ::GetSysColor( COLOR_INACTIVECAPTION ), 0.5 )
				};
				m_hFloats = ::CreateBrushIndirect( &lbBrush );
			}

			{
				LOGBRUSH lbBrush = {
					BS_SOLID,
					CHelpers::MixColorRef( ::GetSysColor( COLOR_WINDOW ), ::GetSysColor( COLOR_INACTIVECAPTION ), 0.5 )
				};
				lbBrush.lbColor = RGB( GetBValue( lbBrush.lbColor ), GetGValue( lbBrush.lbColor ), GetRValue( lbBrush.lbColor ) );
				m_hSign = ::CreateBrushIndirect( &lbBrush );
			}
			{
				LOGBRUSH lbBrush = {
					BS_SOLID,
					CHelpers::MixColorRef( ::GetSysColor( COLOR_WINDOW ), ::GetSysColor( COLOR_INACTIVECAPTION ), 0.5 )
				};
				lbBrush.lbColor = RGB( GetRValue( lbBrush.lbColor ), GetBValue( lbBrush.lbColor ), GetGValue( lbBrush.lbColor ) );
				m_hUnsign = ::CreateBrushIndirect( &lbBrush );
			}

			{
				LOGBRUSH lbBrush = {
					BS_SOLID,
					CHelpers::MixColorRef( ::GetSysColor( COLOR_WINDOW ), ::GetSysColor( COLOR_ACTIVECAPTION ), 0.25 )
				};
				//lbBrush.lbColor = RGB( GetGValue( lbBrush.lbColor ), GetBValue( lbBrush.lbColor ), GetRValue( lbBrush.lbColor ) );
				m_hHex = ::CreateBrushIndirect( &lbBrush );
			}
			{
				LOGBRUSH lbBrush = {
					BS_SOLID,
					CHelpers::MixColorRef( ::GetSysColor( COLOR_WINDOW ), ::GetSysColor( COLOR_INACTIVECAPTION ), 0.25 )
				};
				//lbBrush.lbColor = RGB( GetBValue( lbBrush.lbColor ), GetGValue( lbBrush.lbColor ), GetRValue( lbBrush.lbColor ) );
				m_hBinary = ::CreateBrushIndirect( &lbBrush );
			}
			{
				LOGBRUSH lbBrush = {
					BS_SOLID,
					CHelpers::MixColorRef( ::GetSysColor( COLOR_WINDOW ), ::GetSysColor( COLOR_INACTIVECAPTION ), 0.25 )
				};
				lbBrush.lbColor = RGB( GetRValue( lbBrush.lbColor ), GetBValue( lbBrush.lbColor ), GetGValue( lbBrush.lbColor ) );
				m_hStrings = ::CreateBrushIndirect( &lbBrush );
			}
			
		}


		static const struct {
			LPCWSTR				lpwsImageName;
			DWORD				dwConst;
		} sImages[] = {
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
	CConverterWindow::~CConverterWindow() {
		if ( ::InterlockedDecrement( &m_lCount ) == 0 ) {
			::DeleteObject( m_hRarest );
			m_hRarest = nullptr;

			::DeleteObject( m_hFloats );
			m_hFloats = nullptr;

			::DeleteObject( m_hSign );
			m_hSign = nullptr;

			::DeleteObject( m_hUnsign );
			m_hUnsign = nullptr;

			::DeleteObject( m_hHex );
			m_hHex = nullptr;

			::DeleteObject( m_hBinary );
			m_hBinary = nullptr;

			::DeleteObject( m_hStrings );
			m_hStrings = nullptr;
		}
	}

	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CConverterWindow::InitDialog() {
		CToolBar * plvToolBar = static_cast<CToolBar *>(FindChild( CConverterLayout::MX_CWI_TOOLBAR0 ));
		CRebar * plvRebar = static_cast<CRebar *>(FindChild( CConverterLayout::MX_CWI_REBAR0 ));

		// ==== TOOL BAR ==== //
		if ( plvToolBar ) {
			plvToolBar->SetImageList( 0, m_iImages );
#define MX_TOOL_STR( TXT )					reinterpret_cast<INT_PTR>(TXT)
//#define MX_TOOL_STR( TXT )					0
			const TBBUTTON bButtons[] = {
				// iBitmap									idCommand									fsState				fsStyle			bReserved	dwData	iString
				{ m_iImageMap[MX_I_SCIENTIFIC_NOTATION],	CConverterLayout::MX_BC_SCINOT,	TBSTATE_ENABLED,	BTNS_CHECK,		{ 0 },		0,		MX_TOOL_STR( L"Scientific" ) },
				//{ m_iImageMap[MX_I_EXT_OUTPUT],				CConverterLayout::MX_BC_EXTOUT,	TBSTATE_ENABLED,	BTNS_CHECK,		{ 0 },		0,		MX_TOOL_STR( L"Ext. Output" ) },
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
					riRebarInfo.SetId( CConverterLayout::MX_CWI_TOOLBAR0 );
					plvRebar->InsertBand( -1, riRebarInfo );
				}
			}
		}

		if ( plvRebar ) {
			LSW_RECT rRebarRect = ClientRect( this );
			::MoveWindow( plvRebar->Wnd(), 0, 0, rRebarRect.Width(), plvRebar->WindowRect( this ).Height(), FALSE );

			plvRebar->UpdateRects();
		}
		
		ForceSizeUpdate();
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CConverterWindow::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {
		switch ( _Id ) {
			case CConverterLayout::MX_CWI_INT8 : {}
			case CConverterLayout::MX_CWI_UINT8 : {}
			case CConverterLayout::MX_CWI_CHAR : {}
			case CConverterLayout::MX_CWI_UTF8 : {}
			case CConverterLayout::MX_CWI_INT16_LE : {}
			case CConverterLayout::MX_CWI_UINT16_LE : {}
			case CConverterLayout::MX_CWI_INT32_LE : {}
			case CConverterLayout::MX_CWI_UINT32_LE : {}
			case CConverterLayout::MX_CWI_INT64_LE : {}
			case CConverterLayout::MX_CWI_UINT64_LE : {}
			case CConverterLayout::MX_CWI_INT16_BE : {}
			case CConverterLayout::MX_CWI_UINT16_BE : {}
			case CConverterLayout::MX_CWI_INT32_BE : {}
			case CConverterLayout::MX_CWI_UINT32_BE : {}
			case CConverterLayout::MX_CWI_INT64_BE : {}
			case CConverterLayout::MX_CWI_UINT64_BE : {}
			case CConverterLayout::MX_CWI_UTF16_LE : {}
			case CConverterLayout::MX_CWI_UTF16_BE : {}
			case CConverterLayout::MX_CWI_UTF32_LE : {}
			case CConverterLayout::MX_CWI_UTF32_BE : {}
			case CConverterLayout::MX_CWI_FLOAT10_LE : {}
			case CConverterLayout::MX_CWI_FLOAT10_BE : {}
			case CConverterLayout::MX_CWI_FLOAT11_LE : {}
			case CConverterLayout::MX_CWI_FLOAT11_BE : {}
			case CConverterLayout::MX_CWI_FLOAT14_LE : {}
			case CConverterLayout::MX_CWI_FLOAT14_BE : {}
			case CConverterLayout::MX_CWI_FLOAT16_LE : {}
			case CConverterLayout::MX_CWI_FLOAT16_BE : {}
			case CConverterLayout::MX_CWI_FLOAT24_LE : {}
			case CConverterLayout::MX_CWI_FLOAT24_BE : {}
			case CConverterLayout::MX_CWI_FLOAT32_LE : {}
			case CConverterLayout::MX_CWI_FLOAT32_BE : {}
			case CConverterLayout::MX_CWI_FLOAT64_LE : {}
			case CConverterLayout::MX_CWI_FLOAT64_BE : {}
			case CConverterLayout::MX_CWI_HEX_LE : {}
			case CConverterLayout::MX_CWI_HEX_BE : {}
			case CConverterLayout::MX_CWI_BINARY_LE : {}
			case CConverterLayout::MX_CWI_BINARY_BE : {
				switch ( _wCtrlCode ) {
					case EN_CHANGE : {
						for ( auto I = MX_ELEMENTS( m_ciInfo ); I-- > 0; ) {
							if ( m_ciInfo[I].wId == _Id ) {
								UpdateAll( _pwSrc, _Id, m_ciInfo[I] );
								break;
							}
						}
						//UpdateResult();
						break;
					}
				}
				break;
			}
			case CConverterLayout::MX_BC_SCINOT : {
				if ( _wCtrlCode == STN_CLICKED ) {
					UpdateAll();
				}
				break;
			}
		}
		return LSW_H_HANDLED;
	}

	// Virtual client rectangle.  Can be used for things that need to be adjusted based on whether or not status bars, toolbars, etc. are present.
	const LSW_RECT CConverterWindow::VirtualClientRect( const CWidget * pwChild ) const {
		LSW_RECT rTemp = ClientRect( this );
		const CRebar * plvRebar = static_cast<const CRebar *>(FindChild( CConverterLayout::MX_CWI_REBAR0 ));
		if ( plvRebar ) {
			LSW_RECT rRebar = plvRebar->ClientRect( this );
			rTemp.top += rRebar.Height();
		}
		return rTemp;
	}

	// WM_CTLCOLOREDIT.
	CWidget::LSW_HANDLED CConverterWindow::CtlColorEdit( HDC _hDc, CWidget * _pwControl, HBRUSH &_hBrush ) {
		
		if ( _pwControl ) {
			{
				WORD wId[] = {
					CConverterLayout::MX_CWI_FLOAT10_LE, CConverterLayout::MX_CWI_FLOAT10_BE,
					CConverterLayout::MX_CWI_FLOAT11_LE, CConverterLayout::MX_CWI_FLOAT11_BE,
					CConverterLayout::MX_CWI_FLOAT14_LE, CConverterLayout::MX_CWI_FLOAT14_BE,
					CConverterLayout::MX_CWI_FLOAT24_LE, CConverterLayout::MX_CWI_FLOAT24_BE,
				};
				for ( auto I = MX_ELEMENTS( wId ); I--; ) {
					if ( _pwControl->Id() == wId[I] ) {
						//_hBrush = static_cast<HBRUSH>(::GetStockObject( LTGRAY_BRUSH )); //::GetSysColorBrush( COLOR_APPWORKSPACE );
						_hBrush = m_hRarest;
						return LSW_H_HANDLED;
					}
				}
			}
			{
				WORD wId[] = {
					CConverterLayout::MX_CWI_FLOAT16_LE, CConverterLayout::MX_CWI_FLOAT16_BE,
					CConverterLayout::MX_CWI_FLOAT32_LE, CConverterLayout::MX_CWI_FLOAT32_BE,
					CConverterLayout::MX_CWI_FLOAT64_LE, CConverterLayout::MX_CWI_FLOAT64_BE,
				};
				for ( auto I = MX_ELEMENTS( wId ); I--; ) {
					if ( _pwControl->Id() == wId[I] ) {
						//_hBrush = static_cast<HBRUSH>(::GetStockObject( LTGRAY_BRUSH )); //::GetSysColorBrush( COLOR_APPWORKSPACE );
						_hBrush = m_hFloats;
						return LSW_H_HANDLED;
					}
				}
			}
			{
				WORD wId[] = {
					CConverterLayout::MX_CWI_INT8, CConverterLayout::MX_CWI_CHAR,
					CConverterLayout::MX_CWI_INT16_LE, CConverterLayout::MX_CWI_INT16_BE,
					CConverterLayout::MX_CWI_INT32_LE, CConverterLayout::MX_CWI_INT32_BE,
					CConverterLayout::MX_CWI_INT64_LE, CConverterLayout::MX_CWI_INT64_BE,
				};
				for ( auto I = MX_ELEMENTS( wId ); I--; ) {
					if ( _pwControl->Id() == wId[I] ) {
						//_hBrush = static_cast<HBRUSH>(::GetStockObject( LTGRAY_BRUSH )); //::GetSysColorBrush( COLOR_APPWORKSPACE );
						_hBrush = m_hSign;
						return LSW_H_HANDLED;
					}
				}
			}
			{
				WORD wId[] = {
					CConverterLayout::MX_CWI_UINT8,
					CConverterLayout::MX_CWI_UINT16_LE, CConverterLayout::MX_CWI_UINT16_BE,
					CConverterLayout::MX_CWI_UINT32_LE, CConverterLayout::MX_CWI_UINT32_BE,
					CConverterLayout::MX_CWI_UINT64_LE, CConverterLayout::MX_CWI_UINT64_BE,
				};
				for ( auto I = MX_ELEMENTS( wId ); I--; ) {
					if ( _pwControl->Id() == wId[I] ) {
						_hBrush = m_hUnsign;
						return LSW_H_HANDLED;
					}
				}
			}

			{
				WORD wId[] = {
					CConverterLayout::MX_CWI_HEX_LE, CConverterLayout::MX_CWI_HEX_BE,
				};
				for ( auto I = MX_ELEMENTS( wId ); I--; ) {
					if ( _pwControl->Id() == wId[I] ) {
						_hBrush = m_hHex;
						return LSW_H_HANDLED;
					}
				}
			}
			{
				WORD wId[] = {
					CConverterLayout::MX_CWI_BINARY_LE, CConverterLayout::MX_CWI_BINARY_BE,
				};
				for ( auto I = MX_ELEMENTS( wId ); I--; ) {
					if ( _pwControl->Id() == wId[I] ) {
						_hBrush = m_hBinary;
						return LSW_H_HANDLED;
					}
				}
			}
			{
				WORD wId[] = {
					CConverterLayout::MX_CWI_UTF8,
					CConverterLayout::MX_CWI_UTF16_LE, CConverterLayout::MX_CWI_UTF16_BE,
					CConverterLayout::MX_CWI_UTF32_LE, CConverterLayout::MX_CWI_UTF32_BE,
				};
				for ( auto I = MX_ELEMENTS( wId ); I--; ) {
					if ( _pwControl->Id() == wId[I] ) {
						_hBrush = m_hStrings;
						return LSW_H_HANDLED;
					}
				}
			}
		}
		return LSW_H_CONTINUE;
	}

	// WM_CTLCOLORSTATIC.
	CWidget::LSW_HANDLED CConverterWindow::CtlColorStatic( HDC _hDc, CWidget * _pwControl, HBRUSH &_hBrush ) {
		/*if ( _pwControl && (_pwControl->Id() == CConverterLayout::MX_CWI_FLOAT10_LE ||
			_pwControl->Id() == CConverterLayout::MX_CWI_FLOAT10_BE)) {
			_hBrush = ::GetSysColorBrush( COLOR_INACTIVEBORDER );
			return LSW_H_HANDLED;
		}*/
		return LSW_H_CONTINUE;
	}

	// Gets the value from the given text edit.
	ee::CExpEvalContainer::EE_RESULT CConverterWindow::GetValue( CWidget * _pwControl, ee::EE_NUM_CONSTANTS _ncType, size_t & _sSize, bool _bHex ) {
		BOOL bValid = FALSE;
		ee::CExpEvalContainer::EE_RESULT rRes;
		_pwControl->SetTreatAsHex( _bHex );
		_pwControl->SetAddressHandler( _pwControl->Parent()->GetAddressHandler(), _pwControl->Parent()->GetAddressHandlerData() );
		if ( !_pwControl->GetTextAsExpression( rRes, &bValid ) || !bValid ) {
			rRes.ncType = ee::EE_NC_INVALID;
			return rRes;
		}
		switch ( _ncType ) {
			case ee::EE_NC_SIGNED : {
				rRes = ee::CExpEvalContainer::ConvertResult( rRes, ee::EE_NC_SIGNED );
				rRes.u.i64Val = (rRes.u.i64Val << (64ULL - _sSize)) >> (64ULL - _sSize);
				break;
			}
			case ee::EE_NC_UNSIGNED : {
				rRes = ee::CExpEvalContainer::ConvertResult( rRes, ee::EE_NC_UNSIGNED );
				rRes.u.ui64Val = (rRes.u.ui64Val << (64ULL - _sSize)) >> (64ULL - _sSize);
				break;
			}
			case ee::EE_NC_FLOATING : {
				rRes = ee::CExpEvalContainer::ConvertResult( rRes, ee::EE_NC_FLOATING );
				ee::CFloatX fFloat;
				switch ( _sSize ) {
					case 10 : {
						fFloat.CreateFromDouble( rRes.u.dVal, 5, 6, true, false );
						break;
					}
					case 11 : {
						fFloat.CreateFromDouble( rRes.u.dVal, 5, 7, true, false );
						break;
					}
					case 14 : {
						fFloat.CreateFromDouble( rRes.u.dVal, 5, 10, true, false );
						break;
					}
					case 16 : {
						fFloat.CreateFromDouble( rRes.u.dVal, 5, 11, true, true );
						break;
					}
					case 24 : {
						fFloat.CreateFromDouble( rRes.u.dVal, 7, 16, false, true );
						break;
					}
					case 32 : {
						fFloat.CreateFromDouble( rRes.u.dVal, 8, 24, true, true );
						break;
					}
					case 64 : {
						fFloat.CreateFromDouble( rRes.u.dVal, 11, 53, true, true );
						break;
					}
				}
				rRes.u.ui64Val = fFloat.AsUint64();
				break;
			}
		}

		return rRes;
	}

	// Gets the value from the given text edit.
	ee::CExpEvalContainer::EE_RESULT CConverterWindow::GetValueHex( CWidget * _pwControl, ee::EE_NUM_CONSTANTS _ncType, size_t & _sSize, bool _bHex ) {
		BOOL bValid = FALSE;
		ee::CExpEvalContainer::EE_RESULT rRes;
		_pwControl->SetTreatAsHex( _bHex );
		_pwControl->SetAddressHandler( _pwControl->Parent()->GetAddressHandler(), _pwControl->Parent()->GetAddressHandlerData() );
		if ( !_pwControl->GetTextAsExpression( rRes, &bValid ) || !bValid ) {
			rRes.ncType = ee::EE_NC_INVALID;
			return rRes;
		}

		switch ( rRes.ncType ) {
			case ee::EE_NC_SIGNED : {
				uint64_t ui64TestVal = rRes.u.i64Val < 0 ? -rRes.u.i64Val : rRes.u.i64Val;
				_sSize = 8;
				for ( size_t I = 8; I--; ) {
					if ( ui64TestVal & (0xFFULL << (I * 8ULL)) ) {
						_sSize = (I + 1) * 8;
						break;
					}
				}
				/*if ( ui64TestVal & 0xFFFFFFFF00000000ULL ) {
					_sSize = 64;
				}
				else if ( ui64TestVal & 0xFFFFFFFFFF000000ULL ) {
					_sSize = 32;
				}
				else if ( ui64TestVal & 0xFFFFFFFFFFFF0000ULL ) {
					_sSize = 24;
				}
				else if ( ui64TestVal & 0xFFFFFFFFFFFFFF00ULL ) {
					_sSize = 16;
				}
				else {
					_sSize = 8;
				}*/
				break;
			}
			case ee::EE_NC_UNSIGNED : {
				_sSize = 8;
				for ( size_t I = 8; I--; ) {
					if ( rRes.u.ui64Val & (0xFFULL << (I * 8ULL)) ) {
						_sSize = (I + 1) * 8;
						break;
					}
				}
				/*
				if ( rRes.u.ui64Val & 0xFFFFFFFF00000000ULL ) {
					_sSize = 64;
				}
				else if ( rRes.u.ui64Val & 0xFFFFFFFFFF000000ULL ) {
					_sSize = 32;
				}
				else if ( rRes.u.ui64Val & 0xFFFFFFFFFFFF0000ULL ) {
					_sSize = 24;
				}
				else if ( rRes.u.ui64Val & 0xFFFFFFFFFFFFFF00ULL ) {
					_sSize = 16;
				}
				else {
					_sSize = 8;
				}*/
				break;
			}
			case ee::EE_NC_FLOATING : {
				_sSize = 64;
				break;
			}
		}
		return rRes;
	}

	// Gets the value from the given text edit.
	ee::CExpEvalContainer::EE_RESULT CConverterWindow::GetValueChar( CWidget * _pwControl, ee::EE_NUM_CONSTANTS _ncType, size_t & _sSize, bool /*_bHex*/ ) {
		ee::CExpEvalContainer::EE_RESULT rRes;
		CSecureString ssTmp = _pwControl->GetTextA();
		if ( !ssTmp.size() ) {
			rRes.ncType = ee::EE_NC_INVALID;
			return rRes;
		}
		rRes.ncType = _ncType;
		rRes.u.ui64Val = CUtilities::ResolveEscape( ssTmp.c_str(), ssTmp.size(), _sSize, false );
		_sSize = 8;
		return rRes;
	}

	// Gets the value from the given text edit.
	ee::CExpEvalContainer::EE_RESULT CConverterWindow::GetValueUtf8( CWidget * _pwControl, ee::EE_NUM_CONSTANTS _ncType, size_t & _sSize, bool _bHex ) {
		ee::CExpEvalContainer::EE_RESULT rRes;
		CSecureString ssUtf8 = CUtilities::ToUtf8( _pwControl->GetTextW() );
		if ( !ssUtf8.size() ) {
			rRes.ncType = ee::EE_NC_INVALID;
			return rRes;
		}
		CSecureString ssResolved;
		CUtilities::ResolveAllEscapes( ssUtf8, ssResolved, true );
		rRes.ncType = _ncType;


		size_t sLen = 1;
		_sSize = 0;
		for ( size_t I = 0; I < ssResolved.size() && sLen; I += sLen ) {
			uint64_t uiNext = CUtilities::NextUtf8Char( &ssResolved[I], ssResolved.size() - I, &sLen );
			if ( _sSize + sLen < sizeof( uint64_t ) ) {
				_sSize += sLen;
			}
			else if ( _sSize + sLen == sizeof( uint64_t ) ) {
				_sSize += sLen;
				break;
			}
			else {
				break;
			}
		}
		rRes.u.ui64Val = 0;
		std::memcpy( &rRes.u.ui64Val, ssResolved.data(), _sSize );

		_sSize *= 8;
		return rRes;
	}

	// Gets the value from the given text edit.
	ee::CExpEvalContainer::EE_RESULT CConverterWindow::GetValueUtf16( CWidget * _pwControl, ee::EE_NUM_CONSTANTS _ncType, size_t & _sSize, bool _bHex ) {
		ee::CExpEvalContainer::EE_RESULT rRes;
		CSecureString ssUtf8 = CUtilities::ToUtf8( _pwControl->GetTextW() );
		if ( !ssUtf8.size() ) {
			rRes.ncType = ee::EE_NC_INVALID;
			return rRes;
		}
		CSecureString ssResolved;
		CUtilities::ResolveAllEscapes( ssUtf8, ssResolved, true );
		CSecureWString wUtf16 = CUtilities::ToUtf16( ssResolved );
		rRes.ncType = _ncType;


		size_t sLen = 1;
		_sSize = 0;
		for ( size_t I = 0; I < wUtf16.size() && sLen; I += sLen ) {
			uint64_t uiNext = CUtilities::NextUtf16Char( &wUtf16[I], wUtf16.size() - I, &sLen );
			const size_t sFinalLen = sLen * sizeof( wchar_t );
			if ( _sSize + sFinalLen < sizeof( uint64_t ) ) {
				_sSize += sFinalLen;
			}
			else if ( _sSize + sFinalLen == sizeof( uint64_t ) ) {
				_sSize += sFinalLen;
				break;
			}
			else {
				break;
			}
		}
		rRes.u.ui64Val = 0;
		std::memcpy( &rRes.u.ui64Val, wUtf16.data(), _sSize );

		_sSize *= 8;
		return rRes;
	}

	// Gets the value from the given text edit.
	ee::CExpEvalContainer::EE_RESULT CConverterWindow::GetValueUtf16_BE( CWidget * _pwControl, ee::EE_NUM_CONSTANTS _ncType, size_t & _sSize, bool _bHex ) {
		ee::CExpEvalContainer::EE_RESULT rRes = GetValueUtf16( _pwControl, _ncType, _sSize, _bHex );
		uint16_t * pui16Array = reinterpret_cast<uint16_t *>(&rRes.u.ui64Val);
		for ( size_t I = 0; I < 4; ++I ) {
			pui16Array[I] = ::_byteswap_ushort( pui16Array[I] );
		}
		return rRes;
	}

	// Gets the value from the given text edit.
	ee::CExpEvalContainer::EE_RESULT CConverterWindow::GetValueUtf32( CWidget * _pwControl, ee::EE_NUM_CONSTANTS _ncType, size_t & _sSize, bool _bHex ) {
		ee::CExpEvalContainer::EE_RESULT rRes;
		CSecureWString swsTmp = _pwControl->GetTextW();
		CSecureString ssUtf8 = CUtilities::ToUtf8( swsTmp );
		if ( !ssUtf8.size() ) {
			rRes.ncType = ee::EE_NC_INVALID;
			return rRes;
		}
		CSecureString ssResolved;
		CUtilities::ResolveAllEscapes( ssUtf8, ssResolved, true );
		std::vector<uint32_t> vUtf32 = CUtilities::ToUtf32( ssResolved );
		rRes.ncType = _ncType;


		size_t sLen = 1;
		_sSize = 0;
		for ( size_t I = 0; I < vUtf32.size() && sLen; I += sLen ) {
			uint64_t uiNext = CUtilities::NextUtf32Char( &vUtf32[I], vUtf32.size() - I, &sLen );
			const size_t sFinalLen = sLen * sizeof( uint32_t );
			if ( _sSize + sFinalLen < sizeof( uint64_t ) ) {
				_sSize += sFinalLen;
			}
			else if ( _sSize + sFinalLen == sizeof( uint64_t ) ) {
				_sSize += sFinalLen;
				break;
			}
			else {
				break;
			}
		}
		rRes.u.ui64Val = 0;
		std::memcpy( &rRes.u.ui64Val, vUtf32.data(), _sSize );

		_sSize *= 8;
		return rRes;
	}

	// Gets the value from the given text edit.
	ee::CExpEvalContainer::EE_RESULT CConverterWindow::GetValueUtf32_BE( CWidget * _pwControl, ee::EE_NUM_CONSTANTS _ncType, size_t & _sSize, bool _bHex ) {
		ee::CExpEvalContainer::EE_RESULT rRes = GetValueUtf32( _pwControl, _ncType, _sSize, _bHex );
		uint32_t * pui32Array = reinterpret_cast<uint32_t *>(&rRes.u.ui64Val);
		for ( size_t I = 0; I < 2; ++I ) {
			pui32Array[I] = ::_byteswap_ulong( pui32Array[I] );
		}
		return rRes;
	}

	// Gets the value from the given text edit.
	ee::CExpEvalContainer::EE_RESULT CConverterWindow::GetValueBinary( CWidget * _pwControl, ee::EE_NUM_CONSTANTS _ncType, size_t & _sSize, bool _bHex ) {
		std::string sText = _pwControl->GetTextA();
		ee::CExpEvalContainer::EE_RESULT rRes;
		rRes.ncType = ee::EE_NC_UNSIGNED;
		rRes.u.ui64Val = ee::StoULL( sText.c_str(), 2 );
		_sSize = 8;
		for ( size_t I = 8; I--; ) {
			if ( rRes.u.ui64Val & (0xFFULL << (I * 8ULL)) ) {
				_sSize = (I + 1) * 8;
				break;
			}
		}
		/*if ( rRes.u.ui64Val & 0xFFFFFFFF00000000ULL ) {
			_sSize = 64;
		}
		else if ( rRes.u.ui64Val & 0xFFFFFFFFFF000000ULL ) {
			_sSize = 32;
		}
		else if ( rRes.u.ui64Val & 0xFFFFFFFFFFFF0000ULL ) {
			_sSize = 24;
		}
		else if ( rRes.u.ui64Val & 0xFFFFFFFFFFFFFF00ULL ) {
			_sSize = 16;
		}
		else {
			_sSize = 8;
		}*/
		return rRes;
	}

	// Writes the value to the given edit.
	void CConverterWindow::SetValueStd( CWidget * _pwControl, ee::CExpEvalContainer::EE_RESULT _rRes, ee::EE_NUM_CONSTANTS _ncType, size_t _sSize, bool _bByteSwap, size_t _sSrcSize ) {
		if ( _bByteSwap ) {
			_rRes = ByteSwapBySize( _rRes, _sSize );
		}
		CSecureWString swsTmp;
		switch ( _ncType ) {
			case ee::EE_NC_SIGNED : {
				_rRes.u.i64Val = (_rRes.u.i64Val << (64ULL - _sSize)) >> (64ULL - _sSize);
				CUtilities::ToSigned( _rRes.u.i64Val, swsTmp );
				break;
			}
			case ee::EE_NC_UNSIGNED : {
				_rRes.u.ui64Val = (_rRes.u.ui64Val << (64ULL - _sSize)) >> (64ULL - _sSize);
				CUtilities::ToUnsigned( _rRes.u.ui64Val, swsTmp );
				break;
			}
			case ee::EE_NC_FLOATING : {
				ee::CFloatX fFloat;
				bool bSci = false;
				switch ( _sSize ) {
					case 10 : {
						fFloat.CreateFromBits( _rRes.u.ui64Val, 5, 6, true, false );
						CUtilities::ToDouble( fFloat.AsDouble(), swsTmp, bSci ? -MX_FLOAT10_SIG_DIG : 0 );
						break;
					}
					case 11 : {
						fFloat.CreateFromBits( _rRes.u.ui64Val, 5, 7, true, false );
						CUtilities::ToDouble( fFloat.AsDouble(), swsTmp, bSci ? -MX_FLOAT11_SIG_DIG : 0 );
						break;
					}
					case 14 : {
						fFloat.CreateFromBits( _rRes.u.ui64Val, 5, 10, true, false );
						CUtilities::ToDouble( fFloat.AsDouble(), swsTmp, bSci ? -MX_FLOAT14_SIG_DIG : 0 );
						break;
					}
					case 16 : {
						fFloat.CreateFromBits( _rRes.u.ui64Val, 5, 11, true, true );
						CUtilities::ToDouble( fFloat.AsDouble(), swsTmp, bSci ? -MX_FLOAT16_SIG_DIG : 0 );
						break;
					}
					case 24 : {
						fFloat.CreateFromBits( _rRes.u.ui64Val, 7, 16, false, true );
						CUtilities::ToDouble( fFloat.AsDouble(), swsTmp, bSci ? -MX_FLOAT24_SIG_DIG : 0 );
						break;
					}
					case 32 : {
						fFloat.CreateFromBits( _rRes.u.ui64Val, 8, 24, true, true );
						CUtilities::ToDouble( fFloat.AsDouble(), swsTmp, bSci ? -MX_FLOAT32_SIG_DIG : 0 );
						break;
					}
					case 64 : {
						fFloat.CreateFromBits( _rRes.u.ui64Val, 11, 53, true, true );
						CUtilities::ToDouble( fFloat.AsDouble(), swsTmp, bSci ? -MX_FLOAT64_SIG_DIG : 0 );
						break;
					}
				}
			}
		}
		_pwControl->SetTextW( swsTmp.c_str() );
	}

	// Writes the value to the given edit.
	void CConverterWindow::SetValueHex( CWidget * _pwControl, ee::CExpEvalContainer::EE_RESULT _rRes, ee::EE_NUM_CONSTANTS _ncType, size_t _sSize, bool _bByteSwap, size_t _sSrcSize, bool _bAltDisp ) {
		if ( _bByteSwap ) {
			_rRes = ByteSwapBySize( _rRes, _sSrcSize );
		}
		_rRes.u.ui64Val = (_rRes.u.ui64Val << (64ULL - _sSrcSize)) >> (64ULL - _sSrcSize);
		CSecureWString swsTmp;
		CUtilities::ToHex( _rRes.u.ui64Val, swsTmp, (_sSrcSize + 3) / 4 );


		_pwControl->SetTextW( swsTmp.c_str() );
	}

	// Writes the value to the given edit.
	void CConverterWindow::SetValueChar( CWidget * _pwControl, ee::CExpEvalContainer::EE_RESULT _rRes, ee::EE_NUM_CONSTANTS _ncType, size_t _sSize, bool _bByteSwap, size_t _sSrcSize, bool _bAltDisp ) {
		CSecureString ssTmp = CSecureString( reinterpret_cast<char *>(&_rRes.u.ui64Val), 1 );
		CSecureWString swsFinal = _bAltDisp ? CUtilities::ToUtf16( CUtilities::EscapeAllW( ssTmp, false ) ) :
			CUtilities::ToUtf16( CUtilities::EscapeStandard( ssTmp, false ) );
		_pwControl->SetTextW( swsFinal.c_str() );
	}

	// Writes the value to the given edit.
	void CConverterWindow::SetValueUtf8( CWidget * _pwControl, ee::CExpEvalContainer::EE_RESULT _rRes, ee::EE_NUM_CONSTANTS _ncType, size_t _sSize, bool _bByteSwap, size_t _sSrcSize, bool _bAltDisp ) {
		const char * pcText = reinterpret_cast<const char *>(&_rRes.u.ui64Val);
		CSecureString ssTmp;
		for ( size_t I = 0; I < ((_sSrcSize + (sizeof( char ) - 1)) >> 3); ++I ) {
			ssTmp.push_back( pcText[I] );
		}
		_pwControl->SetTextW( (_bAltDisp ? CUtilities::ToUtf16( CUtilities::EscapeAllW( ssTmp, false ) ) :
			CUtilities::ToUtf16( CUtilities::EscapeStandard( ssTmp, false ) ) ).c_str() );
	}

	// Writes the value to the given edit.
	void CConverterWindow::SetValueUtf16( CWidget * _pwControl, ee::CExpEvalContainer::EE_RESULT _rRes, ee::EE_NUM_CONSTANTS _ncType, size_t _sSize, bool _bByteSwap, size_t _sSrcSize, bool _bAltDisp ) {
		const wchar_t * pcText = reinterpret_cast<const wchar_t *>(&_rRes.u.ui64Val);
		CSecureWString ssTmp;
		const size_t sSize = ((_sSrcSize + (sizeof( wchar_t ) * 8 - 1)) >> 4);	// Round up to the nearest full char size.
		for ( size_t I = 0; I < sSize; ++I ) {
			ssTmp.push_back( pcText[I] );
		}
		_pwControl->SetTextW( (_bAltDisp ? CUtilities::EscapeAllW( ssTmp, false ) :
			CUtilities::EscapeStandard( ssTmp, false )).c_str() );
	}

	// Writes the value to the given edit.
	void CConverterWindow::SetValueUtf16_BE( CWidget * _pwControl, ee::CExpEvalContainer::EE_RESULT _rRes, ee::EE_NUM_CONSTANTS _ncType, size_t _sSize, bool _bByteSwap, size_t _sSrcSize, bool _bAltDisp ) {
		const wchar_t * pcText = reinterpret_cast<const wchar_t *>(&_rRes.u.ui64Val);
		CSecureWString ssTmp;
		const size_t sSize = ((_sSrcSize + (sizeof( wchar_t ) * 8 - 1)) >> 4);	// Round up to the nearest full char size.
		for ( size_t I = 0; I < sSize; ++I ) {
			ssTmp.push_back( ::_byteswap_ushort( pcText[I] ) );
		}
		_pwControl->SetTextW( (_bAltDisp ? CUtilities::EscapeAllW( ssTmp, false ) :
			CUtilities::EscapeStandard( ssTmp, false )).c_str() );
	}

	// Writes the value to the given edit.
	void CConverterWindow::SetValueUtf32( CWidget * _pwControl, ee::CExpEvalContainer::EE_RESULT _rRes, ee::EE_NUM_CONSTANTS _ncType, size_t _sSize, bool _bByteSwap, size_t _sSrcSize, bool _bAltDisp ) {
		const uint32_t * pcText = reinterpret_cast<const uint32_t *>(&_rRes.u.ui64Val);
		std::vector<uint32_t> ssTmp;
		const size_t sSize = ((_sSrcSize + (sizeof( uint32_t ) * 8 - 1)) >> 5);	// Round up to the nearest full char size.
		for ( size_t I = 0; I < sSize; ++I ) {
			ssTmp.push_back( pcText[I] );
		}
		std::vector<uint32_t> vTmp;
		if ( _bAltDisp ) {
			vTmp = CUtilities::EscapeAllW( ssTmp, false );
		}
		else {
			vTmp = CUtilities::EscapeStandard( ssTmp, false );
			
		}
		_pwControl->SetTextW( CUtilities::Utf32StringToWString( vTmp.data(), vTmp.size() ).c_str() );
	}

	// Writes the value to the given edit.
	void CConverterWindow::SetValueUtf32_BE( CWidget * _pwControl, ee::CExpEvalContainer::EE_RESULT _rRes, ee::EE_NUM_CONSTANTS _ncType, size_t _sSize, bool _bByteSwap, size_t _sSrcSize, bool _bAltDisp ) {
		const uint32_t * pcText = reinterpret_cast<const uint32_t *>(&_rRes.u.ui64Val);
		std::vector<uint32_t> ssTmp;
		const size_t sSize = ((_sSrcSize + (sizeof( uint32_t ) * 8 - 1)) >> 5);	// Round up to the nearest full char size.
		for ( size_t I = 0; I < sSize; ++I ) {
			ssTmp.push_back( ::_byteswap_ulong( pcText[I] ) );
		}
		std::vector<uint32_t> vTmp;
		if ( _bAltDisp ) {
			vTmp = CUtilities::EscapeAllW( ssTmp, false );
		}
		else {
			vTmp = CUtilities::EscapeStandard( ssTmp, false );
			
		}
		_pwControl->SetTextW( CUtilities::Utf32StringToWString( vTmp.data(), vTmp.size() ).c_str() );
	}

	// Writes the value to the given edit.
	void CConverterWindow::SetValueBinary( CWidget * _pwControl, ee::CExpEvalContainer::EE_RESULT _rRes, ee::EE_NUM_CONSTANTS _ncType, size_t _sSize, bool _bByteSwap, size_t _sSrcSize, bool _bAltDisp ) {
		if ( _bByteSwap ) {
			_rRes = ByteSwapBySize( _rRes, _sSrcSize );
		}
		_rRes.u.ui64Val = (_rRes.u.ui64Val << (64ULL - _sSrcSize)) >> (64ULL - _sSrcSize);
		CSecureWString swsTmp;
		CUtilities::ToBinary( _rRes.u.ui64Val, swsTmp, _sSrcSize );


		_pwControl->SetTextW( swsTmp.c_str() );
	}

	// Byteswaps based on the number of bits passed.
	ee::CExpEvalContainer::EE_RESULT CConverterWindow::ByteSwapBySize( ee::CExpEvalContainer::EE_RESULT _rVal, size_t _sSize ) {
		ee::CExpEvalContainer::EE_RESULT rRes;
		rRes.ncType = _rVal.ncType;
		//_rVal.u.ui64Val <<= (8 - _sSize) % 8;

		uint8_t * pui8Value = reinterpret_cast<uint8_t *>(&_rVal.u.ui64Val);
		size_t sBytes = (_sSize + 7) / 8;
		for ( auto I = sBytes >> 1; I--; ) {
			uint8_t ui8Temp = pui8Value[I];
			pui8Value[I] = pui8Value[sBytes-I-1];
			pui8Value[sBytes-I-1] = ui8Temp;
		}

		switch ( _rVal.ncType ) {
			case ee::EE_NC_SIGNED : {
				rRes.u.i64Val = (_rVal.u.i64Val << (64ULL - _sSize)) >> (64ULL - _sSize);
				break;
			}
			case ee::EE_NC_FLOATING : {}
			case ee::EE_NC_UNSIGNED : {
				rRes.u.ui64Val = (_rVal.u.ui64Val << (64ULL - _sSize)) >> (64ULL - _sSize);
				break;
			}
		}

		return rRes;
	}

	// Updates all texts given an edit that was modified to trigger the update.
	void CConverterWindow::UpdateAll( CWidget * _pwControl, WORD _wId, const MX_CONV_INFO &_ciInfo ) {
		static LONG lCount = 0;
		if ( ::InterlockedIncrement( &lCount ) == 1 ) {
			m_wLastUpdateCtl = _wId;
			size_t sSize = _ciInfo.sBits;
			ee::CExpEvalContainer::EE_RESULT rRes = _ciInfo.pfGetValueFunc( _pwControl, _ciInfo.ncType, sSize, _ciInfo.bHexInput );
			if ( rRes.ncType != ee::EE_NC_INVALID ) {

				if ( _ciInfo.bBigEnd ) {
					rRes = ByteSwapBySize( rRes, sSize );
				}


				bool bAltDisp = false;
				CToolBar * plvToolBar = static_cast<CToolBar *>(FindChild( CConverterLayout::MX_CWI_TOOLBAR0 ));
				if ( plvToolBar ) {
					bAltDisp = plvToolBar->IsChecked( CConverterLayout::MX_BC_SCINOT ) ? true : false;
				}
				for ( auto I = MX_ELEMENTS( m_ciInfo ); I--; ) {
					if ( m_ciInfo[I].wId != _wId ) {
						m_ciInfo[I].pfSetValueFunc( FindChild( m_ciInfo[I].wId ), rRes, m_ciInfo[I].ncType, m_ciInfo[I].sBits, m_ciInfo[I].bBigEnd, sSize,
							bAltDisp );
					}
				}
			}
		}
		::InterlockedDecrement( &lCount );
	}

	// Updates all texts based on which was last used for the update..
	void CConverterWindow::UpdateAll() {
		if ( !m_wLastUpdateCtl ) { return; }

		for ( auto I = MX_ELEMENTS( m_ciInfo ); I-- > 0; ) {
			if ( m_ciInfo[I].wId == m_wLastUpdateCtl ) {
				UpdateAll( FindChild( m_wLastUpdateCtl ), m_wLastUpdateCtl, m_ciInfo[I] );
				break;
			}
		}
	}

}	// namespace mx

