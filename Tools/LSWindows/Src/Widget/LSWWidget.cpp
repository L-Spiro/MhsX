#include "LSWWidget.h"
#include "../Base/LSWBase.h"

namespace lsw {

	CWidget::CWidget( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget ) :
		m_hWnd( NULL ),
		m_dwId( _wlLayout.wId ),
		m_bEnabled( _wlLayout.bEnabled ),
		m_pwParent( _pwParent ) {

		m_rStartingRect.left = _wlLayout.iLeft;
		m_rStartingRect.top = _wlLayout.iTop;
		m_rStartingRect.SetWidth( _wlLayout.dwWidth );
		m_rStartingRect.SetHeight( _wlLayout.dwHeight );

		if ( _bCreateWidget ) {
			m_hWnd = ::CreateWindowExW( 0,
				_wlLayout.lpwcClass,
				_wlLayout.pwcText,
				_wlLayout.dwStyle,
				_wlLayout.iLeft, _wlLayout.iTop,
				_wlLayout.dwWidth, _wlLayout.dwHeight,
				_pwParent ? _pwParent->Wnd() : NULL,
				(_wlLayout.dwStyle & WS_CHILD) ? reinterpret_cast<HMENU>(static_cast<UINT_PTR>(_wlLayout.wId)) : NULL,
				CBase::GetThisHandle(),
				static_cast<CWidget *>(this) );
		}

		if ( _pwParent != nullptr ) {
			_pwParent->AddChild( this );
		}
		else {
			m_pwParent = nullptr;
		}

		// Apply expressions.
#ifdef _DEBUG
#define LSW_ERROR_PRINT( TEXTEXP, ERROR )	::MessageBoxA( NULL, _wlLayout.TEXTEXP, "Invalid " #ERROR "Expression", MB_OK )
#else
#define LSW_ERROR_PRINT( TEXTEXP, ERROR )
#endif	// #ifdef _DEBUG

#define LSW_SET_EXP( TEXTEXP, EXP, ERROR )																			\
	if ( _wlLayout.TEXTEXP ) {																						\
		if ( !EXP.SetExpression( _wlLayout.TEXTEXP ) ) {															\
			LSW_ERROR_PRINT( TEXTEXP, ERROR );																		\
			EXP.Reset();																							\
		}																											\
		else {																										\
			EXP.GetContainer()->ExpWasParsed();																		\
			EXP.GetContainer()->SetUserHandler( WidgetUserVarHandler, reinterpret_cast<uintptr_t>(this) );			\
			EXP.GetContainer()->SetMemberAccessHandler( WidgetMemberAccessHandler, 0 );								\
		}																											\
	}


		LSW_SET_EXP( pcWidthSizeExp, m_eWidth, Width )
		LSW_SET_EXP( pcHeightSizeExp, m_eHeight, Height )
		LSW_SET_EXP( pcLeftSizeExp, m_eLeft, Left )
		LSW_SET_EXP( pcRightSizeExp, m_eRight, Right )
		LSW_SET_EXP( pcTopSizeExp, m_eTop, Top )
		LSW_SET_EXP( pcBottomSizeExp, m_eBottom, Bottom )

		

#undef LSW_SET_EXP
#undef LSW_ERROR_PRINT
	}
	CWidget::~CWidget() {
		if ( m_pwParent ) {
			m_pwParent->RemoveChild( this );
		}
		for ( size_t I = 0; I < m_vChildren.size(); ++I ) {
			CWidget * pwTemp = m_vChildren[I];
			m_vChildren[I] = nullptr;
			delete m_vChildren[I];			
		}
		m_vChildren.clear();
		
		if ( m_hWnd ) {
			::DestroyWindow( m_hWnd );
			m_hWnd = NULL;
		}
	}

	
	// == Functions.
	// The default message handler.
	LRESULT CALLBACK CWidget::WindowProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
		CWidget * pmwThis = reinterpret_cast<CWidget *>(::GetWindowLongPtrW( _hWnd, GWLP_USERDATA ));
		switch ( _uMsg ) {
			// =======================================
			// Create/Destroy.
			// =======================================
			case WM_NCCREATE : {
				CREATESTRUCTW * pcsCreate = reinterpret_cast<CREATESTRUCTW *>(_lParam);
				::SetWindowLongPtrW( _hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pcsCreate->lpCreateParams) );
				pmwThis = reinterpret_cast<CWidget *>(pcsCreate->lpCreateParams);
				pmwThis->m_hWnd = _hWnd;
				::GetWindowRect( _hWnd, &pmwThis->m_rRect );
				::GetClientRect( _hWnd, &pmwThis->m_rClientRect );
				::GetWindowRect( _hWnd, &pmwThis->m_rStartingRect );

				// Attach all controls to their CWidget counterparts.
				::EnumChildWindows( _hWnd, EnumChildWindows_AttachWindowToWidget, _lParam );
				::EnumChildWindows( _hWnd, EnumChildWindows_SetEnabled, _lParam );
				::EnumChildWindows( _hWnd, EnumChildWindows_SetStartingRect, _lParam );

				LSW_HANDLED hHandled = pmwThis->NcCreate( (*pcsCreate) );
				if ( hHandled == LSW_H_HANDLED ) { return 0; }
				break;
			}
			case WM_CREATE : {
				CREATESTRUCTW * pcsCreate = reinterpret_cast<CREATESTRUCTW *>(_lParam);
				LSW_HANDLED hHandled = pmwThis->Create( (*pcsCreate) );
				if ( hHandled == LSW_H_HANDLED ) { return 0; }
				break;
			}
			case WM_DESTROY : {
				LSW_HANDLED hHandled = pmwThis->Destroy();
				if ( hHandled == LSW_H_HANDLED ) { return 0; }
				break;
			}
			case WM_NCDESTROY : {
				LSW_HANDLED hHandled = pmwThis->NcDestroy();
				if ( hHandled == LSW_H_HANDLED ) { return 0; }
				break;
			}
			case WM_CLOSE : {
				LSW_HANDLED hHandled = pmwThis->Close();
				if ( hHandled == LSW_H_HANDLED ) { return 0; }
				break;
			}

			// =======================================
			// Sizing.
			// =======================================
			case WM_SIZE : {
				LSW_HANDLED hHandled;
				switch ( _wParam ) {
					case SIZE_MINIMIZED : {
						// Width and height are the sizes in the task bar.
						hHandled = pmwThis->Minimized( LOWORD( _lParam ), HIWORD( _lParam ) );
						break;
					}
					default : {
						::GetWindowRect( _hWnd, &pmwThis->m_rRect );
						::GetClientRect( _hWnd, &pmwThis->m_rClientRect );
						hHandled = pmwThis->Size( _wParam, pmwThis->m_rRect.Width(), pmwThis->m_rRect.Height() );
					}
				}
				
				if ( hHandled == LSW_H_HANDLED ) { return 0; }
				break;
			}
			case WM_MOVE : {
				if ( !::IsIconic( _hWnd ) ) {
					::GetWindowRect( _hWnd, &pmwThis->m_rRect );
					::GetClientRect( _hWnd, &pmwThis->m_rClientRect );
					POINTS pPoint = MAKEPOINTS( _lParam );
					LSW_HANDLED hHandled = pmwThis->Move( pPoint.x, pPoint.y );
					if ( hHandled == LSW_H_HANDLED ) { return 0; }
				}
				break;
			}

			// =======================================
			// Commands
			// =======================================
			case WM_COMMAND : {
				LSW_HANDLED hHandled;
				WORD wId = LOWORD( _wParam );
				if ( _lParam ) {
					// Sent by a control.
					hHandled = pmwThis->Command( wId, reinterpret_cast<HWND>(_lParam) );
				}
				else {
					// Sent by a menu or accelerator.
					switch ( HIWORD( _wParam ) ) {
						case 0 : {
							hHandled = pmwThis->MenuCommand( wId );
							break;
						}
						case 1 : {
							hHandled = pmwThis->AcceleratorCommand( wId );
							break;
						}
						default : {
							// Error.
						}
					}
				}
				if ( hHandled == LSW_H_HANDLED ) { return 0; }
				break;
			}
		}
		return ::DefWindowProcW( _hWnd, _uMsg, _wParam, _lParam );
	}

	// The default dialog message handler.
	INT_PTR CALLBACK CWidget::DialogProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
		CWidget * pmwThis = reinterpret_cast<CWidget *>(::GetWindowLongPtrW( _hWnd, DWLP_USER ));

		switch ( _uMsg ) {
			// =======================================
			// Create/Destroy.
			// =======================================
			case WM_INITDIALOG : {
				std::vector<CWidget *> * pvWidgets = reinterpret_cast<std::vector<CWidget *> *>(_lParam);
				pmwThis = (*pvWidgets)[0];
				::SetWindowLongPtrW( _hWnd, DWLP_USER, reinterpret_cast<LONG_PTR>(pmwThis) );
				pmwThis->m_hWnd = _hWnd;
				::GetWindowRect( _hWnd, &pmwThis->m_rRect );
				::GetClientRect( _hWnd, &pmwThis->m_rClientRect );
				::GetWindowRect( _hWnd, &pmwThis->m_rStartingRect );


				// Attach all controls to their CWidget counterparts.
				::EnumChildWindows( _hWnd, EnumChildWindows_AttachWindowToWidget, _lParam );
				::EnumChildWindows( _hWnd, EnumChildWindows_SetEnabled, _lParam );
				::EnumChildWindows( _hWnd, EnumChildWindows_SetStartingRect, _lParam );

				POINT pConvOrg = PixelsToDialogUnits( _hWnd, 574 - 551 - 7, 633 - 302 - 30 );
				POINT pConv = PixelsToDialogUnits( _hWnd, 567, 206 );
				POINT pConvClient = PixelsToDialogUnits( _hWnd, 443, 23 );
				
				/*NONCLIENTMETRICSW ncmMetrics;
				ncmMetrics.cbSize = sizeof( ncmMetrics );
				::SystemParametersInfoW( SPI_GETNONCLIENTMETRICS, ncmMetrics.cbSize, &ncmMetrics, 0 );
				HFONT hFont = ::CreateFontIndirectW( &ncmMetrics.lfMessageFont );*/


				// ::SetWindowLong( _hWnd, DWL_MSGRESULT, lResult );
				//FONT 8, "MS Shell Dlg", 400, 0, 0x1
				//SendMessage( _hWnd, WM_SETFONT,  (WPARAM)GetStockObject(DEFAULT_GUI_FONT), true );
				//EnumChildWindows(_hWnd, (WNDENUMPROC)SetFont, (LPARAM)GetStockObject(SYSTEM_FIXED_FONT));
				//EnumChildWindows(_hWnd, (WNDENUMPROC)SetFont, (LPARAM)hFont);
				return TRUE;	// Return TRUE to pass focus on to the control specified by _wParam.
			}
			case WM_CLOSE : {
				LSW_HANDLED hHandled = pmwThis->Close();
				if ( hHandled == LSW_H_HANDLED ) { return FALSE; }
				break;
			}
			// =======================================
			// Sizing.
			// =======================================
			case WM_SIZE : {
				LSW_HANDLED hHandled;
				switch ( _wParam ) {
					case SIZE_MINIMIZED : {
						// Width and height are the sizes in the task bar.
						hHandled = pmwThis->Minimized( LOWORD( _lParam ), HIWORD( _lParam ) );
						break;
					}
					default : {
						::GetWindowRect( _hWnd, &pmwThis->m_rRect );
						::GetClientRect( _hWnd, &pmwThis->m_rClientRect );
						hHandled = pmwThis->Size( _wParam, pmwThis->m_rRect.Width(), pmwThis->m_rRect.Height() );
						::RedrawWindow( _hWnd, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW );
					}
				}
				
				if ( hHandled == LSW_H_HANDLED ) { return 0; }
				break;
			}
			case WM_MOVE : {
				if ( !::IsIconic( _hWnd ) ) {
					::GetWindowRect( _hWnd, &pmwThis->m_rRect );
					::GetClientRect( _hWnd, &pmwThis->m_rClientRect );
					POINTS pPoint = MAKEPOINTS( _lParam );
					LSW_HANDLED hHandled = pmwThis->Move( pPoint.x, pPoint.y );
					if ( hHandled == LSW_H_HANDLED ) { return 0; }
				}
				break;
			}
			case WM_SETFONT : {
				/*HFONT hFont = (HFONT)_wParam;
				::EnumChildWindows( _hWnd, EnumChildWindows_SetFont, (LPARAM)hFont );*/
				return TRUE;
			}
		}
		return FALSE;
	}

	// Do we have a given child widget?
	bool CWidget::HasChild( const CWidget * _pwChild ) const {
		for ( size_t I = m_vChildren.size(); I--; ) {
			if ( m_vChildren[I] == _pwChild ) {
				return true;
			}
		}
		return false;
	}

	// Sets a given font on all children of a window.
	BOOL CALLBACK CWidget::EnumChildWindows_SetFont( HWND _hWnd, LPARAM _lParam ) {
		::SendMessageW( _hWnd, WM_SETFONT, static_cast<WPARAM>(_lParam), TRUE );
		return TRUE;
	}

	// Converts a point from pixels to dialog units.
	POINT CWidget::PixelsToDialogUnits( HWND _hWnd, LONG _lX, LONG _lY ) {
		POINT pRet;
		for ( LONG I = 0; I < 5000; ++I ) {
			RECT rX;
			rX.left = I;
			::MapDialogRect( _hWnd, &rX );
			if ( rX.left >= _lX ) {
				pRet.x = I;
				break;
			}
		}
		for ( LONG I = 0; I < 5000; ++I ) {
			RECT rY;
			rY.top = I;
			::MapDialogRect( _hWnd, &rY );
			if ( rY.top >= _lY ) {
				pRet.y = I;
				break;
			}
		}
		return pRet;
	}

	// == Message Handlers.
	// WM_NCCREATE.
	CWidget::LSW_HANDLED CWidget::NcCreate( const CREATESTRUCTW &_csCreateParms ) {
		return LSW_H_CONTINUE;
	}

	// WM_CREATE.
	CWidget::LSW_HANDLED CWidget::Create( const CREATESTRUCTW &_csCreateParms ) {
		return LSW_H_CONTINUE;
	}

	// WM_DESTROY.
	CWidget::LSW_HANDLED CWidget::Destroy() {
		return LSW_H_CONTINUE;
	}

	// WM_NCDESTROY.
	CWidget::LSW_HANDLED CWidget::NcDestroy() {
		return LSW_H_CONTINUE;
	}

	// WM_CLOSE.
	CWidget::LSW_HANDLED CWidget::Close() {
		return LSW_H_CONTINUE;
	}

	// WM_SIZE.
	CWidget::LSW_HANDLED CWidget::Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight ) {
		::EnumChildWindows( Wnd(), EnumChildWindows_ResizeControls, 0 );
		return LSW_H_CONTINUE;
	}

	// WM_SIZE, SIZE_MINIMIZED.
	CWidget::LSW_HANDLED CWidget::Minimized( LONG _lWidth, LONG _lHeight ) {
		return LSW_H_CONTINUE;
	}

	// WM_MOVE.
	CWidget::LSW_HANDLED CWidget::Move( LONG _lX, LONG _lY ) {
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CWidget::Command( WORD _Id, HWND _hControl ) {
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from menu.
	CWidget::LSW_HANDLED CWidget::MenuCommand( WORD _Id ) {
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from accelerator.
	CWidget::LSW_HANDLED CWidget::AcceleratorCommand( WORD _Id ) {
		return LSW_H_CONTINUE;
	}

	// == Functions.
	// Remove a child.
	void CWidget::RemoveChild( const CWidget * _pwChild ) {
		for ( size_t I = m_vChildren.size(); I--; ) {
			if ( m_vChildren[I] == _pwChild ) {
				m_vChildren.erase( m_vChildren.begin() + I );
			}
		}
	}

	// Add a chld.
	void CWidget::AddChild( CWidget * _pwChild ) {
		if ( !HasChild( _pwChild ) ) {
			m_vChildren.push_back( _pwChild );
		}
	}

	// Set the parent.
	void CWidget::SetParent( CWidget * _pwParent ) {
		if ( m_pwParent ) {
			m_pwParent->RemoveChild( this );
		}
		m_pwParent = _pwParent;
		if ( m_pwParent ) {
			m_pwParent->AddChild( this );
		}
	}

	// Evaluates expressions to determine a new rectangle for the control.
	void CWidget::EvalNewSize() {
		LSW_RECT rNewSize = WindowRect();
		// Each axis has 3 possible expressions.
		// X axis has left, width, and right.
		// Y axis has top, height, and bottom.
		// Only up to 2 on each axis can logically be defined.


		// X AXIS.
		if ( m_eLeft.GetContainer() ) {
			rNewSize.left = static_cast<LONG>(m_eLeft.Evaluate());
			if ( m_eWidth.GetContainer() ) {
				rNewSize.SetWidth( static_cast<LONG>(m_eWidth.Evaluate()) );
			}
			else if ( m_eRight.GetContainer() ) {
				rNewSize.right = static_cast<LONG>(m_eRight.Evaluate());
			}
		}
		else if ( m_eRight.GetContainer() ) {
			// Left not defined but right is.
			rNewSize.right = static_cast<LONG>(m_eRight.Evaluate());
			if ( m_eWidth.GetContainer() ) {
				rNewSize.left = rNewSize.right - static_cast<LONG>(m_eWidth.Evaluate());
			}
		}
		else if ( m_eWidth.GetContainer() ) {
			// Only width is defined.
			rNewSize.SetWidth( static_cast<LONG>(m_eWidth.Evaluate()) );
		}


		// Y AXIS.
		if ( m_eTop.GetContainer() ) {
			rNewSize.top = static_cast<LONG>(m_eTop.Evaluate());
			if ( m_eHeight.GetContainer() ) {
				rNewSize.SetHeight( static_cast<LONG>(m_eHeight.Evaluate()) );
			}
			else if ( m_eBottom.GetContainer() ) {
				rNewSize.bottom = static_cast<LONG>(m_eBottom.Evaluate());
			}
		}
		else if ( m_eBottom.GetContainer() ) {
			// Top not defined but bottom is.
			rNewSize.bottom = static_cast<LONG>(m_eBottom.Evaluate());
			if ( m_eHeight.GetContainer() ) {
				rNewSize.top = rNewSize.bottom - static_cast<LONG>(m_eHeight.Evaluate());
			}
		}
		else if ( m_eHeight.GetContainer() ) {
			// Only width is defined.
			rNewSize.SetHeight( static_cast<LONG>(m_eHeight.Evaluate()) );
		}

		if ( Parent() ) {
			POINT pUpperLeft = rNewSize.UpperLeft(), pBottomRight = rNewSize.BottomRight();
			::ScreenToClient( Parent()->Wnd(), &pUpperLeft );
			::ScreenToClient( Parent()->Wnd(), &pBottomRight );
			rNewSize.left = pUpperLeft.x;
			rNewSize.top = pUpperLeft.y;
			rNewSize.right = pBottomRight.x;
			rNewSize.bottom = pBottomRight.y;
		}
		::MoveWindow( Wnd(), rNewSize.left, rNewSize.top, rNewSize.Width(), rNewSize.Height(), FALSE );
	}

	// Attaches a control/window to its CWidget.
	BOOL CALLBACK CWidget::EnumChildWindows_AttachWindowToWidget( HWND _hWnd, LPARAM _lParam ) {
		std::vector<CWidget *> * pvWidgets = reinterpret_cast<std::vector<CWidget *> *>(_lParam);
		int iId = ::GetDlgCtrlID( _hWnd );
		for ( size_t I = 0; I < pvWidgets->size(); ++I ) {
			if ( (*pvWidgets)[I]->Id() == iId ) {
				(*pvWidgets)[I]->m_hWnd = _hWnd;
				::SetWindowLongPtrW( _hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>((*pvWidgets)[I]) );
				return TRUE;
			}
		}
		return TRUE;
	}

	// Applies enabled/disabled settings.
	BOOL CALLBACK CWidget::EnumChildWindows_SetEnabled( HWND _hWnd, LPARAM _lParam ) {
		CWidget * pwThis = reinterpret_cast<CWidget *>(::GetWindowLongPtrW( _hWnd, GWLP_USERDATA ));
		if ( pwThis ) {
			if ( !pwThis->Enabled() ) {
				::EnableWindow( _hWnd, FALSE );
			}
		}
		return TRUE;
	}

	// Sets all the starting rectangles for all widgets.
	BOOL CALLBACK CWidget::EnumChildWindows_SetStartingRect( HWND _hWnd, LPARAM _lParam ) {
		CWidget * pwThis = reinterpret_cast<CWidget *>(::GetWindowLongPtrW( _hWnd, GWLP_USERDATA ));
		if ( pwThis ) {
			::GetWindowRect( _hWnd, &pwThis->m_rRect );
			::GetClientRect( _hWnd, &pwThis->m_rClientRect );
			::GetWindowRect( _hWnd, &pwThis->m_rStartingRect );
		}
		return TRUE;
	}

	// Resizes all controls when the window resizes.
	BOOL CALLBACK CWidget::EnumChildWindows_ResizeControls( HWND _hWnd, LPARAM _lParam ) {
		CWidget * pwThis = reinterpret_cast<CWidget *>(::GetWindowLongPtrW( _hWnd, GWLP_USERDATA ));
		if ( pwThis ) {
			pwThis->EvalNewSize();
		}
		return TRUE;
	}

	// Evaluates "??" inside expressions.  ?? = this pointer.
	bool __stdcall CWidget::WidgetUserVarHandler( uintptr_t _uiptrData, ee::CExpEvalContainer * _peecContainer, ee::CExpEvalContainer::EE_RESULT &_rResult ) {
		_rResult.ncType = ee::EE_NC_UNSIGNED;
		_rResult.u.ui64Val = _uiptrData;
		return true;
	}

	// Evaluates member access in expressions.
	bool __stdcall CWidget::WidgetMemberAccessHandler( const ee::CExpEvalContainer::EE_RESULT &_rLeft, const std::string &_sMember, uintptr_t _uiptrData, ee::CExpEvalContainer * _peecContainer, ee::CExpEvalContainer::EE_RESULT &_rResult ) {
		// Expecting that _rLeft evaluates to a CWidget *.
		if ( _rLeft.ncType != ee::EE_NC_UNSIGNED ) { return false; }
		CWidget * pwThis = reinterpret_cast<CWidget *>(static_cast<uintptr_t>(_rLeft.u.ui64Val));
		if ( !pwThis ) { return false; }
		
		// Figure out what is being accessed on the widget and return accordingly.
		// =======================================
		// PARENT.
		// =======================================
		if ( _sMember.size() == 1 ) {
			if ( ::_stricmp( _sMember.c_str(), "P" ) == 0 ) {
				// Accessing parent.
				_rResult.ncType = ee::EE_NC_UNSIGNED;
				_rResult.u.ui64Val = reinterpret_cast<uint64_t>(pwThis->Parent());
				return true;
			}
			// =======================================
			// WINDOW RECT.
			// =======================================
			if ( ::_stricmp( _sMember.c_str(), "W" ) == 0 ) {
				// Accessing this width.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->WindowRect().Width());
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "H" ) == 0 ) {
				// Accessing this height.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->WindowRect().Height());
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "L" ) == 0 ) {
				// Accessing this left.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->WindowRect().left);
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "R" ) == 0 ) {
				// Accessing this right.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->WindowRect().right);
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "T" ) == 0 ) {
				// Accessing this top.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->WindowRect().top);
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "B" ) == 0 ) {
				// Accessing this bottom.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->WindowRect().bottom);
				return true;
			}
		}
		// =======================================
		// CLIENT RECT.
		// =======================================
		if ( _sMember.size() == 2 ) {
			if ( ::_stricmp( _sMember.c_str(), "CW" ) == 0 ) {
				// Accessing this client width.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->ClientRect().Width());
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "CH" ) == 0 ) {
				// Accessing this client height.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->ClientRect().Height());
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "CL" ) == 0 ) {
				// Accessing this client left.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->ClientRect().left);
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "CR" ) == 0 ) {
				// Accessing this client right.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->ClientRect().right);
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "CT" ) == 0 ) {
				// Accessing this client top.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->ClientRect().top);
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "CB" ) == 0 ) {
				// Accessing this client bottom.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->ClientRect().bottom);
				return true;
			}
			// =======================================
			// START RECT.
			// =======================================
			if ( ::_stricmp( _sMember.c_str(), "SW" ) == 0 ) {
				// Accessing this start width.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->StartRect().Width());
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "SH" ) == 0 ) {
				// Accessing this start height.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->StartRect().Height());
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "SL" ) == 0 ) {
				// Accessing this start left.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->StartRect().left);
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "SR" ) == 0 ) {
				// Accessing this start right.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->StartRect().right);
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "ST" ) == 0 ) {
				// Accessing this start top.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->StartRect().top);
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "SB" ) == 0 ) {
				// Accessing this start bottom.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->StartRect().bottom);
				return true;
			}
			// =======================================
			// PROPERTIES.
			// =======================================
			if ( ::_stricmp( _sMember.c_str(), "ID" ) == 0 ) {
				// Accessing this ID.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->Id());
				return true;
			}
		}

		return false;
	}

}	// namespace lsw
