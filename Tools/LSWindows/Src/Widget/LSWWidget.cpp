#include "LSWWidget.h"
#include "../Base/LSWBase.h"
#include "../Docking/LSWDockable.h"
#include "../ListView/LSWListView.h"
#include "../Tab/LSWTab.h"
#include "../ToolTip/LSWToolTip.h"
#include "../TreeList/LSWTreeList.h"
#include <Object/EEObject.h>
#include <cassert>
#include <codecvt>
#include <locale>
#include <windowsx.h>

namespace lsw {

	CWidget::CWidget( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t /*_ui64Data*/ ) :
		m_ui64UserData( 0 ),
		m_hWnd( NULL ),
		m_wId( _wlLayout.wId ),
		m_bEnabled( _wlLayout.bEnabled ),
		m_bActive( _wlLayout.bActive ),
		m_bTreatAsHex( FALSE ),
		m_pwParent( _pwParent ),
		m_bShowAsActive( FALSE ),
		m_iLastHit( HTNOWHERE ),
		m_bInDestructor( FALSE ),
		m_dwTooltipStyle( _wlLayout.dwToolTipStyle ),
		m_dwTooltipStyleEx( _wlLayout.dwToolTipStyleEx ),
		m_hTooltip( NULL ),
		m_pfahAddressHandler( nullptr ),
		m_uiptrAddressHandlerData( 0 ),
		m_pfahAddressWriteHandler( nullptr ),
		m_uiptrAddressWriteHandlerData( 0 ),
		m_bHasFocus( false ) {

		m_rStartingRect.left = _wlLayout.iLeft;
		m_rStartingRect.top = _wlLayout.iTop;
		m_rStartingRect.SetWidth( _wlLayout.dwWidth );
		m_rStartingRect.SetHeight( _wlLayout.dwHeight );
		m_rStartingClientRect = m_rStartingRect;

		m_dwExtendedStyles = _wlLayout.dwStyleEx;

		if ( _wlLayout.pcToolTip ) {
			m_sTooltipText.assign( _wlLayout.pcToolTip, _wlLayout.sToolTipLen );
		}

		if ( _bCreateWidget ) {
			m_hWnd = ::CreateWindowExW( _wlLayout.dwStyleEx,
				_wlLayout.lpwcClass,
				_wlLayout.pwcText,
				_wlLayout.dwStyle,
				_wlLayout.iLeft, _wlLayout.iTop,
				_wlLayout.dwWidth, _wlLayout.dwHeight,
				_pwParent ? _pwParent->Wnd() : NULL,
				(_wlLayout.dwStyle & WS_CHILD) ? reinterpret_cast<HMENU>(static_cast<UINT_PTR>(_wlLayout.wId)) : _hMenu,
				CBase::GetThisHandle(),
				static_cast<CWidget *>(this) );
			assert( m_hWnd );
			::SetWindowLongPtrW( Wnd(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this) );
			UpdateRects();
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
		m_bInDestructor = TRUE;
		if ( m_hWnd != NULL ) {
			::DestroyWindow( m_hWnd );
			m_hWnd = NULL;
		}
		if ( m_hTooltip ) {
			HWND hTool = m_hTooltip;
			m_hTooltip = NULL;
			::DestroyWindow( hTool );
		}

		if ( m_pwParent ) {
			m_pwParent->RemoveChild( this );
			m_pwParent = nullptr;
		}
		for ( size_t I = 0; I < m_vChildren.size(); ++I ) {
			CWidget * pwTemp = m_vChildren[I];
			m_vChildren[I] = nullptr;
			delete pwTemp;			
		}
		m_vChildren.clear();
		m_bInDestructor = FALSE;
	}

	
	// == Functions.
	// The default message handler.
	LRESULT CALLBACK CWidget::WindowProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
		LRESULT lrWndRes = 0;
		INT_PTR ipDlgRes = 0;
		WndDlgProc( _hWnd, _uMsg, _wParam, _lParam, FALSE, lrWndRes, ipDlgRes );
		return lrWndRes;
	}

	// The default dialog message handler.
	INT_PTR CALLBACK CWidget::DialogProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
		LRESULT lrWndRes = 0;
		INT_PTR ipDlgRes = 0;
		WndDlgProc( _hWnd, _uMsg, _wParam, _lParam, TRUE, lrWndRes, ipDlgRes );
		return ipDlgRes;
	}

	// The ancestor widget.
	CWidget * CWidget::Ancestor() {
		CWidget * pwNext = Parent();
		while ( pwNext && pwNext->Parent() ) {
			pwNext = pwNext->Parent();
		}
		return pwNext;
	}

	// The ancestor widget.
	const CWidget * CWidget::Ancestor() const {
		const CWidget * pwNext = Parent();
		while ( pwNext && pwNext->Parent() ) {
			pwNext = pwNext->Parent();
		}
		return pwNext;
	}

	// Gets the window text.
	std::string CWidget::GetTextA() const {
		INT iLen = GetTextLengthA() + 1;
		CHAR * pcBuffer = new( std::nothrow ) CHAR[iLen];
		GetTextA( pcBuffer, iLen );
		std::string sRet = pcBuffer;
		delete [] pcBuffer;
		return sRet;
	}

	// Gets the window text.
	std::wstring CWidget::GetTextW() const {
		INT iLen = GetTextLengthW() + 1;
		WCHAR * pwcBuffer = new( std::nothrow ) WCHAR[iLen];
		GetTextW( pwcBuffer, iLen );
		std::wstring sRet = pwcBuffer;
		delete [] pwcBuffer;
		return sRet;
	}

	// Gets the window text in UTF-8.
	std::string CWidget::GetTextUTF8() const {
		std::wstring wText = GetTextW();
		return ee::CExpEval::WStringToString( wText );
	}

	// Get the value of the text as an expression.
	BOOL CWidget::GetTextAsExpression( ee::CExpEvalContainer::EE_RESULT &_eResult, BOOL * _pbExpIsValid, std::string * _psObjStringResult ) const {
		if ( _pbExpIsValid ) { (*_pbExpIsValid) = FALSE; }
		CExpression eExp;
		eExp.SetTreatsAsHex( TreatAsHex() != FALSE );

		

		std::string sText = GetTextUTF8();
		if ( sText.size() == 0 ) { return FALSE; }
		if ( !eExp.SetExpression( sText.c_str() ) ) { return FALSE; }
		if ( !eExp.GetContainer() ) { return FALSE; }
		eExp.GetContainer()->SetAddressHandler( m_pfahAddressHandler, m_uiptrAddressHandlerData );
		eExp.GetContainer()->SetAddressWriteHandler( m_pfahAddressWriteHandler, m_uiptrAddressWriteHandlerData );
		if ( _pbExpIsValid ) { (*_pbExpIsValid) = TRUE; }
		
		if ( !eExp.GetContainer()->Resolve( _eResult ) ) { return FALSE; }
		if ( (_eResult.ncType) == ee::EE_NC_OBJECT ) {
			ee::CObject * poObj = _eResult.u.poObj;
			_eResult.u.poObj = nullptr;
			if ( _psObjStringResult && poObj ) {
				return poObj->ToString( (*_psObjStringResult) ) ? TRUE : FALSE;
			}
			return FALSE;
		}
		return TRUE;
	}

	// Get the value of the text as an int64_t expression.
	BOOL CWidget::GetTextAsInt64Expression( ee::CExpEvalContainer::EE_RESULT &_eResult ) const {
		if ( !GetTextAsExpression( _eResult ) ) { return FALSE; }
		_eResult = ee::CExpEvalContainer::ConvertResult( _eResult, ee::EE_NC_SIGNED );
		return TRUE;
	}

	// Get the value of the text as a uint64_t expression.
	BOOL CWidget::GetTextAsUInt64Expression( ee::CExpEvalContainer::EE_RESULT &_eResult ) const {
		if ( !GetTextAsExpression( _eResult ) ) { return FALSE; }
		_eResult = ee::CExpEvalContainer::ConvertResult( _eResult, ee::EE_NC_UNSIGNED );
		return TRUE;
	}

	// Get the value of the text as a double expression.
	BOOL CWidget::GetTextAsDoubleExpression( ee::CExpEvalContainer::EE_RESULT &_eResult ) const {
		if ( !GetTextAsExpression( _eResult ) ) { return FALSE; }
		_eResult = ee::CExpEvalContainer::ConvertResult( _eResult, ee::EE_NC_FLOATING );
		return TRUE;
	}

	// If the function succeeds, the return value is the pointer to the window that previously had the keyboard focus.
	CWidget * CWidget::SetFocus() const {
		if ( !Wnd() ) { return nullptr; }
		HWND hWnd = ::SetFocus( Wnd() );
		CWidget * pwRet = nullptr;
		if ( hWnd ) {
			pwRet = LSW_WIN2CLASS( hWnd );
		}
		return pwRet;
	}

	// Gets the value of the boolean that tracks the window's focus.
	bool CWidget::GetFocus() const {
		return m_bHasFocus;
	}

	// Sets the parent window.
	CWidget * CWidget::SetParent( CWidget * _pwParent ) {
		HWND hWnd = _pwParent ? _pwParent->Wnd() : NULL;
		hWnd = ::SetParent( Wnd(), hWnd );
		return LSW_WIN2CLASS( hWnd );
	}

	// Updates all rectangles with the current window rectangles.  If a control changes size and you wish to set the new size as its "base" size,
	//	call this.
	VOID CWidget::UpdateRects() {
		/*::GetWindowRect( Wnd(), &m_rRect );
		::GetClientRect( Wnd(), &m_rClientRect );*/
		::GetWindowRect( Wnd(), &m_rStartingRect );
		//::GetClientRect( Wnd(), &m_rStartingClientRect );
		::GetWindowRect( Wnd(), &m_rStartingClientRect );
		if ( Parent() ) {
			LSW_RECT rPar = Parent()->ClientRect( this ).ClientToScreen( Parent()->Wnd() );
			m_rStartingClientRect.MoveBy( { -rPar.left, -rPar.top } );
		}
		//m_rStartingClientRect = m_rStartingClientRect.ClientToScreen( Wnd() );
		//m_rStartingClientRect.MoveBy( { -m_rStartingRect.left, -m_rStartingRect.top } );
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

	// Gets a pointer to a child with the given ID.
	CWidget * CWidget::FindChild( WORD _wId ) {
		for ( size_t I = m_vChildren.size(); I--; ) {
			if ( m_vChildren[I]->Id() == _wId ) {
				return m_vChildren[I];
			}
			CWidget * pwTemp = m_vChildren[I]->FindChild( _wId );
			if ( pwTemp ) { return pwTemp; }
		}
		return nullptr;
	}

	// Gets a pointer to a child with the given ID.
	const CWidget * CWidget::FindChild( WORD _wId ) const {
		for ( size_t I = m_vChildren.size(); I--; ) {
			if ( m_vChildren[I]->Id() == _wId ) {
				return m_vChildren[I];
			}
			CWidget * pwTemp = m_vChildren[I]->FindChild( _wId );
			if ( pwTemp ) { return pwTemp; }
		}
		return nullptr;
	}

	// Gets a pointer to a parent with the given ID.
	CWidget * CWidget::FindParent( WORD _wId ) {
		CWidget * pwParent = Parent();
		while ( pwParent ) {
			if ( pwParent->Id() == _wId ) { return pwParent; }
			pwParent = pwParent->Parent();
		}
		return nullptr;
	}

	// Gets a pointer to a parent with the given ID.
	const CWidget * CWidget::FindParent( WORD _wId ) const {
		const CWidget * pwParent = Parent();
		while ( pwParent ) {
			if ( pwParent->Id() == _wId ) { return pwParent; }
			pwParent = pwParent->Parent();
		}
		return nullptr;
	}

	// Set the parent.
	void CWidget::SetWidgetParent( CWidget * _pwParent ) {
		if ( m_pwParent == _pwParent ) { return; }
		if ( m_pwParent ) {
			m_pwParent->RemoveChild( this );
		}
		m_pwParent = _pwParent;
		if ( m_pwParent ) {
			m_pwParent->AddChild( this );
		}
	}

	// Translate a child's tooltip text.
	std::wstring CWidget::TranslateTooltip( const std::string &_sText ) { 
		return ee::CExpEval::ToUtf16( _sText );
	}

	// Sets a given font on all children of a window.
	BOOL CALLBACK CWidget::EnumChildWindows_SetFont( HWND _hWnd, LPARAM _lParam ) {
		::SendMessageW( _hWnd, WM_SETFONT, static_cast<WPARAM>(_lParam), TRUE );
		return TRUE;
	}

	// Converts a point from pixels to dialog units.
	POINT CWidget::PixelsToDialogUnits( HWND _hWnd, LONG _lX, LONG _lY ) {
		POINT pRet = {};
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
	/**
	 * The WM_SIZE handler.
	 *
	 * \param _wParam The type of resizing requested.
	 * \param _lWidth The new width of the client area.
	 * \param _lHeight The new height of the client area.
	 * \return Returns a LSW_HANDLED enumeration.
	 */
	CWidget::LSW_HANDLED CWidget::Size( WPARAM /*_wParam*/, LONG /*_lWidth*/, LONG /*_lHeight*/ ) {
		::EnumChildWindows( Wnd(), EnumChildWindows_ResizeControls, 0 );
		return LSW_H_CONTINUE;
	}

	// WM_MOVE.
	CWidget::LSW_HANDLED CWidget::Move( LONG /*_lX*/, LONG /*_lY*/ ) {
		::EnumChildWindows( Wnd(), EnumChildWindows_ResizeControls, 0 );
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
		ChildWasRemoved( _pwChild );
	}

	// Add a child.
	void CWidget::AddChild( CWidget * _pwChild ) {
		if ( !HasChild( _pwChild ) ) {
			m_vChildren.push_back( _pwChild );
		}
	}

	// Informs that a child was removed from a child control (IE this control's child had a child control removed from it).
	// Is also called on the control from which a child was removed for convenience.
	void CWidget::ChildWasRemoved( const CWidget * _pwChild ) {
		if ( Parent() ) {
			Parent()->ChildWasRemoved( _pwChild );
		}
	}

	// Evaluates expressions to determine a new rectangle for the control.
	void CWidget::EvalNewSize() {
		LSW_RECT rNewSize = WindowRect( this );
		// Each axis has 3 possible expressions.
		// X axis has left, width, and right.
		// Y axis has top, height, and bottom.
		// Only up to 2 on each axis can logically be defined.
		BOOL bResize = FALSE;


		// X AXIS.
		if ( m_eLeft.GetContainer() ) {
			rNewSize.left = static_cast<LONG>(m_eLeft.Evaluate());
			if ( m_eWidth.GetContainer() ) {
				rNewSize.SetWidth( static_cast<LONG>(m_eWidth.Evaluate()) );
			}
			else if ( m_eRight.GetContainer() ) {
				rNewSize.right = static_cast<LONG>(m_eRight.Evaluate());
			}
			bResize = TRUE;
		}
		else if ( m_eRight.GetContainer() ) {
			// Left not defined but right is.
			rNewSize.right = static_cast<LONG>(m_eRight.Evaluate());
			if ( m_eWidth.GetContainer() ) {
				rNewSize.left = rNewSize.right - static_cast<LONG>(m_eWidth.Evaluate());
			}
			bResize = TRUE;
		}
		else if ( m_eWidth.GetContainer() ) {
			// Only width is defined.
			rNewSize.SetWidth( static_cast<LONG>(m_eWidth.Evaluate()) );
			bResize = TRUE;
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
			bResize = TRUE;
		}
		else if ( m_eBottom.GetContainer() ) {
			// Top not defined but bottom is.
			rNewSize.bottom = static_cast<LONG>(m_eBottom.Evaluate());
			if ( m_eHeight.GetContainer() ) {
				rNewSize.top = rNewSize.bottom - static_cast<LONG>(m_eHeight.Evaluate());
			}
			bResize = TRUE;
		}
		else if ( m_eHeight.GetContainer() ) {
			// Only width is defined.
			rNewSize.SetHeight( static_cast<LONG>(m_eHeight.Evaluate()) );
			bResize = TRUE;
		}

		if ( bResize ) {
			if ( Parent() ) {
				POINT pUpperLeft = rNewSize.UpperLeft(), pBottomRight = rNewSize.BottomRight();
				LSW_RECT rParRect = Parent()->ClientRect( this );
				::ScreenToClient( Parent()->Wnd(), &pUpperLeft );
				::ScreenToClient( Parent()->Wnd(), &pBottomRight );
				rNewSize.left = pUpperLeft.x;
				rNewSize.top = pUpperLeft.y;
				rNewSize.right = pBottomRight.x;
				rNewSize.bottom = pBottomRight.y;
			}
			::MoveWindow( Wnd(), rNewSize.left, rNewSize.top, rNewSize.Width(), rNewSize.Height(), FALSE );
		}
		/*::GetWindowRect( Wnd(), &m_rRect );
		::GetClientRect( Wnd(), &m_rClientRect );*/
	}

	// Setting the HWND after the control has been created.
	void CWidget::InitControl( HWND _hWnd ) {
		m_hWnd = _hWnd;

		if ( m_sTooltipText.size() && !m_hTooltip ) {
			HWND hParent = Parent() ? Parent()->Wnd() : Wnd();
			m_hTooltip = ::CreateWindowExW( m_dwTooltipStyleEx,
				TOOLTIPS_CLASSW,
				NULL,
				m_dwTooltipStyle,
				CW_USEDEFAULT, CW_USEDEFAULT,
				CW_USEDEFAULT, CW_USEDEFAULT,
				hParent,
				0,
				CBase::GetThisHandle(),
				NULL );
			if ( m_hTooltip ) {
				::SetWindowPos( m_hTooltip, HWND_TOPMOST, 0, 0, 0, 0,
					SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );

				TOOLINFOW tiToolInfo = { sizeof( tiToolInfo ) };
				tiToolInfo.hwnd = hParent;
				tiToolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
				tiToolInfo.uId = reinterpret_cast<UINT_PTR>(Wnd());
				tiToolInfo.lpszText = LPSTR_TEXTCALLBACKW;
				::SendMessageW( m_hTooltip, TTM_ADDTOOLW, 0, reinterpret_cast<LPARAM>(&tiToolInfo) );
			}
		}
	}

	// Adds a dockable window to the list of dockable windows.
	void CWidget::AddDockable( CDockable * _pdDock ) {
		for ( size_t I = 0; I < m_vDockables.size(); ++I ) {
			::SetActiveWindow( m_vDockables[I]->Wnd() );
			if ( m_vDockables[I] == _pdDock ) { return; }
		}
		m_vDockables.push_back( _pdDock );
	}

	// Removes a dockable window from the list of dockable windows.
	void CWidget::RemDockable( CDockable * _pdDock ) {
		for ( size_t I = m_vDockables.size(); I--; ) {
			if ( m_vDockables[I] == _pdDock ) { m_vDockables.erase( m_vDockables.begin() + I ); }
		}
	}

	// Gets the array of dockables, optionally including this object.
	void CWidget::GetDockables( std::vector<CWidget *> &_vReturn, BOOL _bIncludeParent ) {
		_vReturn.clear();
		for ( size_t I = 0; I < m_vDockables.size(); ++I ) {
			_vReturn.push_back( m_vDockables[I] );
		}
		if ( _bIncludeParent ) {
			_vReturn.push_back( this );
		}
	}

	// Handle WM_NCACTIVATE for dockables.  Should be called on the owner window.
	//	_pwWnd = Pointer to window that received WM_NCACTIVATE (can be the owner or one of its tool windows).
	//	_wParam = WPARAM of the WM_NCACTIVATE message.
	//	_lParam = LPARAM of the WM_NCACTIVATE message.
	LRESULT CALLBACK CWidget::DockNcActivate( CWidget * _pwWnd, WPARAM _wParam, LPARAM _lParam, BOOL _bCallDefault ) {
		LSW_HANDLED hHandled = _pwWnd->NcActivate( static_cast<BOOL>(_wParam), _lParam );

		/*if ( _lParam == -1 ) {
			if ( hHandled == LSW_H_HANDLED ) { return FALSE; }
			return _bCallDefault ? ::DefWindowProcW( _pwWnd->Wnd(), WM_NCACTIVATE, _wParam, 0L ) :
				FALSE;
		}*/

		BOOL bKeepActive = static_cast<BOOL>(_wParam);
		BOOL bSyncOthers = TRUE;

		HWND hOther = reinterpret_cast<HWND>(_lParam);

		std::vector<CWidget *> vDocks;
		GetDockables( vDocks, TRUE );

		//if ( _lParam ) {
			for ( size_t I = 0; I < vDocks.size(); ++I ) {
				if ( hOther == vDocks[I]->Wnd() ) {
					bKeepActive = TRUE;
					bSyncOthers = FALSE;
					break;
				}
			}
		//}

		if ( _lParam == -1 ) {
			if ( hHandled == LSW_H_HANDLED ) { return FALSE; }
			return _bCallDefault ? ::DefWindowProcW( _pwWnd->Wnd(), WM_NCACTIVATE, bKeepActive, 0L ) :
				FALSE;
		}

		if ( bSyncOthers ) {
			for ( size_t I = 0; I < vDocks.size(); ++I ) {
				// Spoof the message, but not if it is the window that called this.
				if ( vDocks[I]->Wnd() != _pwWnd->Wnd() && _pwWnd->Wnd() != hOther ) {
					::SendMessageW( vDocks[I]->Wnd(), WM_NCACTIVATE, bKeepActive, static_cast<LPARAM>(-1L) );
				}
			}
		}

		if ( hHandled == LSW_H_HANDLED ) { return FALSE; }

		return _bCallDefault ? ::DefWindowProcW( _pwWnd->Wnd(), WM_NCACTIVATE, bKeepActive, _lParam ) :
			FALSE;
	}

	// Handle WM_ENABLE for Should be called on the owner window.
	//	_pwWnd = Pointer to window that received WM_ENABLE (can be the owner or one of its tool windows).
	//	_wParam = WPARAM of the WM_ENABLE message.
	//	_lParam = LPARAM of the WM_ENABLE message.
	LRESULT CALLBACK CWidget::DockEnable( CWidget * _pwWnd, WPARAM _wParam, LPARAM _lParam, BOOL _bCallDefault ) {
		LSW_HANDLED hHandled = _pwWnd->Enable( static_cast<BOOL>(_wParam) );

		std::vector<CWidget *> vDocks;
		GetDockables( vDocks, FALSE );
		

		for ( size_t I = 0; I < vDocks.size(); ++I ) {
			if ( vDocks[I]->Wnd() != _pwWnd->Wnd() ) {
				LSW_HANDLED hHandledThis = vDocks[I]->Enable( static_cast<BOOL>(_wParam) );
				if ( hHandledThis != LSW_H_HANDLED ) {
					if ( _bCallDefault ) {
						vDocks[I]->SetEnabled( static_cast<BOOL>(_wParam) );
						::DefWindowProcW( vDocks[I]->Wnd(), WM_ENABLE, _wParam, _lParam );
					}
				}
				//::SendMessageW( vDocks[I]->Wnd(), WM_ENABLE, _wParam, _lParam );
			}
		}

		if ( hHandled == LSW_H_HANDLED ) { return 0; }
		return _bCallDefault ? ::DefWindowProcW( _pwWnd->Wnd(), WM_ENABLE, _wParam, _lParam ) :
			0;
	}

	// Attaches a control/window to its CWidget.
	BOOL CALLBACK CWidget::EnumChildWindows_AttachWindowToWidget( HWND _hWnd, LPARAM _lParam ) {
		std::vector<CWidget *> * pvWidgets = reinterpret_cast<std::vector<CWidget *> *>(_lParam);
		int iId = ::GetDlgCtrlID( _hWnd );
		for ( size_t I = 0; I < pvWidgets->size(); ++I ) {
			if ( (*pvWidgets)[I]->Id() == iId ) {
				//(*pvWidgets)[I]->m_hWnd = _hWnd;
				::SetWindowLongPtrW( _hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>((*pvWidgets)[I]) );
				(*pvWidgets)[I]->InitControl( _hWnd );
				return TRUE;
			}
		}
		return TRUE;
	}

	// Applies enabled/disabled settings.
	BOOL CALLBACK CWidget::EnumChildWindows_SetEnabled( HWND _hWnd, LPARAM /*_lParam*/ ) {
		CWidget * pwThis = LSW_WIN2CLASS( _hWnd );
		if ( pwThis ) {
			pwThis->SetEnabled( pwThis->Enabled() );
		}
		return TRUE;
	}

	// Sets all the starting rectangles for all widgets.
	BOOL CALLBACK CWidget::EnumChildWindows_SetStartingRect( HWND _hWnd, LPARAM /*_lParam*/ ) {
		CWidget * pwThis = LSW_WIN2CLASS( _hWnd );
		if ( pwThis ) {
			pwThis->UpdateRects();
		}
		return TRUE;
	}

	// Resizes all controls when the window resizes.
	BOOL CALLBACK CWidget::EnumChildWindows_ResizeControls( HWND _hWnd, LPARAM /*_lParam*/ ) {
		CWidget * pwThis = LSW_WIN2CLASS( _hWnd );
		if ( pwThis ) {
			pwThis->EvalNewSize();
		}
		return TRUE;
	}

	// Evaluates "??" inside expressions.  ?? = this pointer.
	bool __stdcall CWidget::WidgetUserVarHandler( uintptr_t _uiptrData, ee::CExpEvalContainer * /*_peecContainer*/, ee::CExpEvalContainer::EE_RESULT &_rResult ) {
		_rResult.ncType = ee::EE_NC_UNSIGNED;
		_rResult.u.ui64Val = _uiptrData;
		return true;
	}

	// Evaluates member access in expressions.
	bool __stdcall CWidget::WidgetMemberAccessHandler( const ee::CExpEvalContainer::EE_RESULT &_rLeft, const std::string &_sMember, uintptr_t /*_uiptrData*/, ee::CExpEvalContainer * _peecContainer, ee::CExpEvalContainer::EE_RESULT &_rResult ) {
		// Expecting that _rLeft evaluates to a CWidget *.
		if ( _rLeft.ncType != ee::EE_NC_UNSIGNED ) {
			return false;
		}
		CWidget * pwThis = reinterpret_cast<CWidget *>(static_cast<uintptr_t>(_rLeft.u.ui64Val));
		if ( !pwThis ) {
			return false;
		}

		CWidget * pwCaller = reinterpret_cast<CWidget *>(static_cast<uintptr_t>(_peecContainer->UserData()));
		
		// Figure out what is being accessed on the widget and return accordingly.
		// Note that all coordinates are in screen space.
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
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->WindowRect( pwCaller ).Width());
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "H" ) == 0 ) {
				// Accessing this height.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->WindowRect( pwCaller ).Height());
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "L" ) == 0 ) {
				// Accessing this left.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->WindowRect( pwCaller ).left);
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "R" ) == 0 ) {
				// Accessing this right.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->WindowRect( pwCaller ).right);
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "T" ) == 0 ) {
				// Accessing this top.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->WindowRect( pwCaller ).top);
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "B" ) == 0 ) {
				// Accessing this bottom.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->WindowRect( pwCaller ).bottom);
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
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->ClientRect( pwCaller ).Width());
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "CH" ) == 0 ) {
				// Accessing this client height.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->ClientRect( pwCaller ).Height());
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "CL" ) == 0 ) {
				// Accessing this client left.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->ClientRect( pwCaller ).UpperLeftClientToScreen( pwThis->Wnd() ).x);
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "CR" ) == 0 ) {
				// Accessing this client right.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->ClientRect( pwCaller ).BottomRightClientToScreen( pwThis->Wnd() ).x);
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "CT" ) == 0 ) {
				// Accessing this client top.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->ClientRect( pwCaller ).UpperLeftClientToScreen( pwThis->Wnd() ).y);
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "CB" ) == 0 ) {
				// Accessing this client bottom.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->ClientRect( pwCaller ).BottomRightClientToScreen( pwThis->Wnd() ).y);
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
		// =======================================
		// VIRTUAL CLIENT RECT.
		// =======================================
		if ( _sMember.size() == 3 ) {
			if ( ::_stricmp( _sMember.c_str(), "VCW" ) == 0 ) {
				// Accessing this client width.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->VirtualClientRect( pwCaller ).Width());
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "VCH" ) == 0 ) {
				// Accessing this client height.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->VirtualClientRect( pwCaller ).Height());
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "VCL" ) == 0 ) {
				// Accessing this client left.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->VirtualClientRect( pwCaller ).UpperLeftClientToScreen( pwThis->Wnd() ).x);
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "VCR" ) == 0 ) {
				// Accessing this client right.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->VirtualClientRect( pwCaller ).BottomRightClientToScreen( pwThis->Wnd() ).x);
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "VCT" ) == 0 ) {
				// Accessing this client top.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->VirtualClientRect( pwCaller ).UpperLeftClientToScreen( pwThis->Wnd() ).y);
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "VCB" ) == 0 ) {
				// Accessing this client bottom.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->VirtualClientRect( pwCaller ).BottomRightClientToScreen( pwThis->Wnd() ).y);
				return true;
			}
			// =======================================
			// STARTING CLIENT RECT.
			// =======================================
			if ( ::_stricmp( _sMember.c_str(), "SCW" ) == 0 ) {
				// Accessing this starting width.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->StartClientRect().Width());
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "SCH" ) == 0 ) {
				// Accessing this starting height.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->StartClientRect().Height());
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "SCL" ) == 0 ) {
				// Accessing this starting left.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->StartClientRect().UpperLeftClientToScreen( pwThis->Wnd() ).x);
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "SCR" ) == 0 ) {
				// Accessing this starting right.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->StartClientRect().BottomRightClientToScreen( pwThis->Wnd() ).x);
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "SCT" ) == 0 ) {
				// Accessing this starting top.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->StartClientRect().top);
				return true;
			}
			if ( ::_stricmp( _sMember.c_str(), "SCB" ) == 0 ) {
				// Accessing this starting bottom.
				_rResult.ncType = ee::EE_NC_SIGNED;
				_rResult.u.ui64Val = static_cast<uint64_t>(pwThis->StartClientRect().BottomRightClientToScreen( pwThis->Wnd() ).y);
				return true;
			}
		}

		return false;
	}

	// Handles control setup.
	VOID CWidget::ControlSetup( CWidget * _pwParent, const std::vector<CWidget *> &_vWidgetList ) {
		/*::GetWindowRect( _pwParent->Wnd(), &_pwParent->m_rRect );
		::GetClientRect( _pwParent->Wnd(), &_pwParent->m_rClientRect );*/
		::GetWindowRect( _pwParent->Wnd(), &_pwParent->m_rStartingRect );
		::GetWindowRect( _pwParent->Wnd(), &_pwParent->m_rStartingClientRect );
		if ( _pwParent->Parent() ) {
			LSW_RECT rPar = _pwParent->Parent()->ClientRect( nullptr ).ClientToScreen( _pwParent->Parent()->Wnd() );
			_pwParent->m_rStartingClientRect.MoveBy( { -rPar.left, -rPar.top } );
		}
		/*::GetClientRect( _pwParent->Wnd(), &_pwParent->m_rStartingClientRect );
		_pwParent->m_rStartingClientRect = _pwParent->m_rStartingClientRect.ClientToScreen( _pwParent->Wnd() );
		_pwParent->m_rStartingClientRect.MoveBy( { -_pwParent->m_rStartingRect.left, -_pwParent->m_rStartingRect.top } );*/

		::EnumChildWindows( _pwParent->Wnd(), EnumChildWindows_AttachWindowToWidget, reinterpret_cast<LPARAM>(&_vWidgetList) );
		::EnumChildWindows( _pwParent->Wnd(), EnumChildWindows_SetEnabled, reinterpret_cast<LPARAM>(&_vWidgetList) );
		::EnumChildWindows( _pwParent->Wnd(), EnumChildWindows_SetStartingRect, reinterpret_cast<LPARAM>(&_vWidgetList) );
	}

	// The default message handler.
	VOID CALLBACK CWidget::WndDlgProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam, BOOL _bIsDlg, LRESULT &_lrWndResult, INT_PTR &_ipDiagResult ) {
#define LSW_WNDRET( VAL )				if ( !_bIsDlg ) { _lrWndResult = VAL; }
#define LSW_DLGRET( VAL )				if ( _bIsDlg ) { _ipDiagResult = VAL; }
#define LSW_RET( WNDVAL, DLGVAL )		LSW_WNDRET( WNDVAL ) LSW_DLGRET( DLGVAL ) return

#if 0
		std::string sMes;
		//CHelpers::WindowsMessageToString( _uMsg, sMes, false );
		CBase::MessageToText( _uMsg, sMes );
		::OutputDebugStringA( sMes.c_str() );
		::OutputDebugStringA( "\r\n" );

		WCHAR szName[128];
		::GetClassNameW( _hWnd, szName, 128 );
		::OutputDebugStringW( szName );
		::OutputDebugStringA( "\r\n" );
#endif	// #if 0

		CWidget * pmwThis = LSW_WIN2CLASS( _hWnd );
		switch ( _uMsg ) {
			// =======================================
			// Create/Destroy.
			// =======================================
			case WM_NCCREATE : {
				CREATESTRUCTW * pcsCreate = reinterpret_cast<CREATESTRUCTW *>(_lParam);
				::SetWindowLongPtrW( _hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pcsCreate->lpCreateParams) );
				pmwThis = reinterpret_cast<CWidget *>(pcsCreate->lpCreateParams);
				pmwThis->m_hWnd = _hWnd;

				// ControlSetup() called by the layout manager because WM_NCCREATE is inside a constructor.

				LSW_HANDLED hHandled = pmwThis->NcCreate( (*pcsCreate) );
				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
				break;
			}
			case WM_CREATE : {
				CREATESTRUCTW * pcsCreate = reinterpret_cast<CREATESTRUCTW *>(_lParam);
				LSW_HANDLED hHandled = pmwThis->Create( (*pcsCreate) );
				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
				break;
			}
			case WM_INITDIALOG : {
				std::vector<CWidget *> * pvWidgets = reinterpret_cast<std::vector<CWidget *> *>(_lParam);
				pmwThis = (*pvWidgets)[0];
				::SetWindowLongPtrW( _hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pmwThis) );
				pmwThis->InitControl( _hWnd );
				
				ControlSetup( pmwThis, (*pvWidgets) );

				// Window rect.
#define MX_X	384
#define MX_Y	153

#if 1
				// For window borders.
#define MX_OFF_X	(7 - 4)
#define MX_OFF_Y	(30 - 4)
#else
#define MX_OFF_X	0
#define MX_OFF_Y	0
#endif
				POINT pConvOrg = PixelsToDialogUnits( _hWnd, 585 - MX_X - MX_OFF_X, 189 - MX_Y - MX_OFF_Y );
				//POINT pConv = PixelsToDialogUnits( _hWnd, 559 - 548 - 7, 486 - 453 - 30 );
				POINT pConvClient = PixelsToDialogUnits( _hWnd, 290, 47 );
				
				pmwThis->InitDialog();
				LSW_RET( TRUE, TRUE );	// Return TRUE to pass focus on to the control specified by _wParam.
			}
			case WM_DESTROY : {
				if ( pmwThis ) {
					LSW_HANDLED hHandled = pmwThis->Destroy();
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
				}
				break;
			}
			case WM_NCDESTROY : {
				if ( pmwThis ) {
					LSW_HANDLED hHandled = pmwThis->NcDestroy();
					if ( !_bIsDlg ) {
						::SetWindowLongPtrW( _hWnd, GWLP_USERDATA, 0L );
						pmwThis->m_hWnd = NULL;	// Destructor calls ::DestroyWindow(), which would send WM_DESTROY and WM_NCDESTROY again.
						if ( !pmwThis->m_bInDestructor ) {
							delete pmwThis;
						}
					}
					// If it is a dialog, CLayoutManager::DestroyDialogBoxTemplate() deletes the CWidget object.
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
				}
				break;
			}
			case WM_CLOSE : {
				LSW_HANDLED hHandled = pmwThis->Close();
				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
				break;
			}
			case WM_CANCELMODE : {
				LSW_HANDLED hHandled = pmwThis->CancelMode();
				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
				break;
			}

			// =======================================
			// Sizing.
			// =======================================
			case WM_SIZING : {
				LSW_HANDLED hHandled = pmwThis->Sizing( static_cast<INT>(_wParam), reinterpret_cast<LSW_RECT *>(_lParam) );

				// An application should return TRUE if it processes this message.
				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( TRUE, TRUE ); }
				break;
			}
			case WM_SIZE : {
				LSW_HANDLED hHandled;
				switch ( _wParam ) {
					case SIZE_MINIMIZED : {
						// Width and height are the sizes in the task bar.
						hHandled = pmwThis->Minimized( LOWORD( _lParam ), HIWORD( _lParam ) );
						break;
					}
					default : {
						LSW_RECT rTemp;
						::GetWindowRect( _hWnd, &rTemp );
						hHandled = pmwThis->Size( _wParam, rTemp.Width(), rTemp.Height() );
						::RedrawWindow( _hWnd, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW );
					}
				}
				// If an application processes this message, it should return zero.
				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
				break;
			}
			case WM_MOVE : {
				if ( !::IsIconic( _hWnd ) ) {
					/*::GetWindowRect( _hWnd, &pmwThis->m_rRect );
					::GetClientRect( _hWnd, &pmwThis->m_rClientRect );*/
					POINTS pPoint = MAKEPOINTS( _lParam );
					LSW_HANDLED hHandled = pmwThis->Move( pPoint.x, pPoint.y );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
				}
				break;
			}
			case WM_WINDOWPOSCHANGED : {
				WINDOWPOS * pwpPos = reinterpret_cast<WINDOWPOS *>(_lParam);
				LSW_HANDLED hHandled = pmwThis->WindowPosChanged( pwpPos );
				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
				break;
			}
			case WM_GETMINMAXINFO : {
				if ( pmwThis ) {
					MINMAXINFO * pmmiInfo = reinterpret_cast<MINMAXINFO *>(_lParam);
					if ( pmmiInfo ) {
						LSW_HANDLED hHandled = pmwThis->GetMinMaxInfo( pmmiInfo );
						// If an application processes this message, it should return zero.
						if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					}
				}
				break;
			}

			// =======================================
			// Commands.
			// =======================================
			case WM_COMMAND : {
				LSW_HANDLED hHandled = LSW_H_CONTINUE;
				WORD wId = LOWORD( _wParam );


				if ( _lParam ) {
					// Sent by a control.
					CWidget * pwSrc = LSW_WIN2CLASS( reinterpret_cast<HWND>(_lParam) );
					hHandled = pmwThis->Command( HIWORD( _wParam ), wId, pwSrc );
				}
				else {
					switch ( HIWORD( _wParam ) ) {
						case BN_CLICKED : {
							hHandled = pmwThis->MenuCommand( wId );
							break;
						}
						case 1 : {
							hHandled = pmwThis->AcceleratorCommand( wId );
							break;
						}
						default : {
						
						}
					}
				}

				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
				break;
			}

			// =======================================
			// Notifications.
			// =======================================
			case WM_NOTIFY : {
				LPNMHDR lpHdr = reinterpret_cast<LPNMHDR>(_lParam);
				switch ( lpHdr->code ) {
					case LVN_COLUMNCLICK : {
						LPNMLISTVIEW plvListView = reinterpret_cast<LPNMLISTVIEW>(_lParam);
						HWND hFrom = plvListView->hdr.hwndFrom;
						CWidget * pmwTemp = LSW_WIN2CLASS( hFrom );
						if ( pmwTemp ) {
							CListView * plvView = static_cast<CListView *>(pmwTemp);
							plvView->SortItems( plvListView->iSubItem );
						}
						LSW_RET( 1, TRUE );
					}
					case LVN_DELETEALLITEMS : {
						LPNMLISTVIEW plvListView = reinterpret_cast<LPNMLISTVIEW>(_lParam);
						HWND hFrom = plvListView->hdr.hwndFrom;
						CWidget * pmwTemp = LSW_WIN2CLASS( hFrom );
						BOOL bRet = TRUE;
						if ( pmwTemp ) {
							CListView * plvView = static_cast<CListView *>(pmwTemp);
							bRet = plvView->DeleteAllNotify();
						}
						LSW_RET( bRet, bRet );
					}
					case LVN_GETDISPINFOW : {
						LPNMLISTVIEW plvListView = reinterpret_cast<LPNMLISTVIEW>(_lParam);
						NMLVDISPINFOW * plvdiInfo = reinterpret_cast<NMLVDISPINFOW *>(_lParam);
						HWND hFrom = plvListView->hdr.hwndFrom;
						CWidget * pmwTemp = LSW_WIN2CLASS( hFrom );
						if ( pmwTemp ) {
							CListView * plvView = static_cast<CListView *>(pmwTemp);
							plvView->GetDispInfoNotify( plvdiInfo );
						}
						LSW_RET( 1, TRUE );
					}
					case LVN_ITEMCHANGED : {
						LPNMLISTVIEW lplvStateChange = reinterpret_cast<LPNMLISTVIEW>(_lParam);
						CWidget * pmwSrc = LSW_WIN2CLASS( lplvStateChange->hdr.hwndFrom );
						if ( pmwSrc ) {
							if ( pmwSrc->Notify_ItemChanged( lplvStateChange ) == LSW_HANDLED::LSW_H_HANDLED ) { LSW_RET( 1, TRUE ); }
						}
						LSW_RET( 1, TRUE );
					}
					case LVN_ODSTATECHANGED : {
						LPNMLVODSTATECHANGE lpscStateChange = reinterpret_cast<LPNMLVODSTATECHANGE>(_lParam);
						CWidget * pmwSrc = LSW_WIN2CLASS( lpscStateChange->hdr.hwndFrom );
						if ( pmwSrc ) {
							if ( pmwSrc->Notify_OdStateChange( lpscStateChange ) == LSW_HANDLED::LSW_H_HANDLED ) { LSW_RET( 1, TRUE ); }
						}
						LSW_RET( 1, TRUE );
					}
					case NM_CUSTOMDRAW : {
						LPNMCUSTOMDRAW pcdCustomDraw = reinterpret_cast<LPNMCUSTOMDRAW>(_lParam);
						CWidget * pmwSrc = LSW_WIN2CLASS( pcdCustomDraw->hdr.hwndFrom );
						if ( pmwSrc ) {
							if ( pmwSrc->IsListView() ) {
								LPNMLVCUSTOMDRAW pcdListViewDraw = reinterpret_cast<LPNMLVCUSTOMDRAW>(_lParam);
								//HWND hFrom = pcdCustomDraw->hdr.hwndFrom;
								if ( pcdListViewDraw->nmcd.dwDrawStage == CDDS_PREPAINT ) {
									DWORD dwRet = pmwSrc->Notify_CustomDraw_PrePaint( pcdListViewDraw );
									if ( _bIsDlg ) {
										::SetWindowLongPtrW( _hWnd, DWLP_MSGRESULT, dwRet );
									}
									LSW_RET( dwRet, dwRet != CDRF_DODEFAULT );
								}
								if ( pcdListViewDraw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT ) {
									DWORD dwRet = pmwSrc->Notify_CustomDraw_ItemPrePaint( pcdListViewDraw );
									if ( _bIsDlg ) {
										::SetWindowLongPtrW( _hWnd, DWLP_MSGRESULT, dwRet );
									}
									LSW_RET( dwRet, dwRet != CDRF_DODEFAULT );
								}
								if ( (CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pcdListViewDraw->nmcd.dwDrawStage ) {
									DWORD dwRet = pmwSrc->Notify_CustomDraw_ItemPrePaintSubItem( pcdListViewDraw );
									if ( _bIsDlg ) {
										::SetWindowLongPtrW( _hWnd, DWLP_MSGRESULT, dwRet );
									}
									LSW_RET( dwRet, dwRet != CDRF_DODEFAULT );
								}
							}
						}
						if ( _bIsDlg ) {
							::SetWindowLongPtrW( _hWnd, DWLP_MSGRESULT, CDRF_DODEFAULT );
						}
						LSW_RET( 1, TRUE );
					}
					case NM_DBLCLK : {
						HWND hFrom = lpHdr->hwndFrom;
						CWidget * pmwTemp = LSW_WIN2CLASS( hFrom );
						if ( pmwTemp ) {
							if ( pmwTemp->DblClk( lpHdr ) == LSW_HANDLED::LSW_H_HANDLED ) { LSW_RET( 1, TRUE ); }
						}
						pmwThis->DblClk( lpHdr, static_cast<WORD>(lpHdr->idFrom), pmwTemp );
						LSW_RET( 1, TRUE );
					}
					case TBN_QUERYINSERT : {
						HWND hFrom = lpHdr->hwndFrom;
						CWidget * pmwTemp = LSW_WIN2CLASS( hFrom );
						if ( pmwTemp ) {
							if ( pmwTemp->TbnQueryInsert( reinterpret_cast<LPNMTOOLBARW>(_lParam) ) == LSW_HANDLED::LSW_H_HANDLED ) { LSW_RET( TRUE, TRUE ); }
						}
						break;
					}
					case TBN_QUERYDELETE : {
						HWND hFrom = lpHdr->hwndFrom;
						CWidget * pmwTemp = LSW_WIN2CLASS( hFrom );
						if ( pmwTemp ) {
							if ( pmwTemp->TbnQueryDelete( reinterpret_cast<LPNMTOOLBARW>(_lParam) ) == LSW_HANDLED::LSW_H_HANDLED ) { LSW_RET( TRUE, TRUE ); }
						}
						break;
					}
					case TBN_RESET : {
						HWND hFrom = lpHdr->hwndFrom;
						CWidget * pmwTemp = LSW_WIN2CLASS( hFrom );
						if ( pmwTemp ) {
							if ( pmwTemp->TbnReset() == LSW_HANDLED::LSW_H_HANDLED ) { LSW_RET( TBNRF_ENDCUSTOMIZE, TBNRF_ENDCUSTOMIZE ); }
						}
						break;
					}
					case TBN_GETBUTTONINFO : {
						HWND hFrom = lpHdr->hwndFrom;
						CWidget * pmwTemp = LSW_WIN2CLASS( hFrom );
						if ( pmwTemp ) {
							if ( pmwTemp->TbnGetButtonInfo( reinterpret_cast<LPNMTOOLBARW>(_lParam) ) == LSW_HANDLED::LSW_H_HANDLED ) { LSW_RET( TRUE, TRUE ); }
						}
						break;
					}
					case LSW_TAB_NM_CLOSE : {
						LSW_NMTABCLOSE * ptcClose = reinterpret_cast<LSW_NMTABCLOSE *>(_lParam);
						if ( ptcClose->iTab != -1 && ptcClose->pwWidget ) {
							static_cast<CTab *>(ptcClose->pwWidget)->DeleteItem( ptcClose->iTab );
						}
						if ( ptcClose->pwWidget->Parent() ) {
							ptcClose->pwWidget->Parent()->ChildTabClosed( ptcClose->pwWidget, ptcClose->iTab );
						}
						break;
					}
					case TCN_SELCHANGE : {
						CWidget * pwSrc = LSW_WIN2CLASS( lpHdr->hwndFrom );
						if ( pwSrc ) {
							CTab * ptTab = static_cast<CTab *>(pwSrc);
							ptTab->SetCurSel( ptTab->GetCurSel() );
						}
						break;
					}
					case TVN_ITEMEXPANDED : {
						LPNMTREEVIEWW ptvTreeView = reinterpret_cast<LPNMTREEVIEWW>(_lParam);
						HWND hFrom = ptvTreeView->hdr.hwndFrom;
						CWidget * pmwTemp = LSW_WIN2CLASS( hFrom );
						if ( pmwTemp ) {
							CTreeList * ptlTree = static_cast<CTreeList *>(pmwTemp);
							ptlTree->ItemExpandedNotify( ptvTreeView );
						}
						else {
							static_cast<CTreeList *>(pmwThis)->ItemExpandedNotify( ptvTreeView );
						}
						LSW_RET( 1, TRUE );
					}
					case TTN_GETDISPINFOW : {
						LPNMTTDISPINFOW pdiInfo = reinterpret_cast<LPNMTTDISPINFOW>(_lParam);
						HWND hFrom;
						if ( pdiInfo->uFlags & TTF_IDISHWND ) {
							hFrom = reinterpret_cast<HWND>(pdiInfo->hdr.idFrom);
						}
						else {
							hFrom = ::GetDlgItem( _hWnd, static_cast<int>(pdiInfo->hdr.idFrom) );
						}
						CWidget * pmwTemp = LSW_WIN2CLASS( hFrom );
						if ( pmwTemp && pmwTemp->m_sTooltipText.size() ) {
							std::wstring wsText = pmwThis->TranslateTooltip( pmwTemp->m_sTooltipText );
							::wcsncpy_s( pdiInfo->szText, wsText.c_str(), _TRUNCATE );
							pdiInfo->szText[LSW_ELEMENTS( pdiInfo->szText )-1] = L'\0';
					}
						LSW_RET( 1, TRUE );
					}
				}
				//LSW_RET( 1, TRUE );
				break;
			}

			// =======================================
			// Drawing.
			// =======================================
			case WM_ERASEBKGND : {
				LSW_HANDLED hHandled = pmwThis->EraseBkgnd( reinterpret_cast<HDC>(_wParam) );
				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 1, 1 ); }
				break;
			}
			case WM_PAINT : {
				LSW_HANDLED hHandled = pmwThis->Paint();
				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
				break;
			}
			case WM_NCPAINT : {
				LSW_HANDLED hHandled = pmwThis->NcPaint( reinterpret_cast<HRGN>(_wParam) );
				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
				break;
			}
			case WM_CTLCOLOREDIT : {
				HBRUSH hBrush = NULL;
				CWidget * pwControl = LSW_WIN2CLASS( reinterpret_cast<HWND>(_lParam) );
				LSW_HANDLED hHandled = pmwThis->CtlColorEdit( reinterpret_cast<HDC>(_wParam), pwControl, hBrush );
				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( reinterpret_cast<LRESULT>(hBrush), reinterpret_cast<INT_PTR>(hBrush) ); }
				break;
			}
			case WM_CTLCOLORLISTBOX : {
				HBRUSH hBrush = NULL;
				CWidget * pwControl = LSW_WIN2CLASS( reinterpret_cast<HWND>(_lParam) );
				LSW_HANDLED hHandled = pmwThis->CtlColorListBox( reinterpret_cast<HDC>(_wParam), pwControl, hBrush );
				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( reinterpret_cast<LRESULT>(hBrush), reinterpret_cast<INT_PTR>(hBrush) ); }
				break;
			}
			case WM_CTLCOLORSTATIC : {
				HBRUSH hBrush = NULL;
				CWidget * pwControl = LSW_WIN2CLASS( reinterpret_cast<HWND>(_lParam) );
				LSW_HANDLED hHandled = pmwThis->CtlColorStatic( reinterpret_cast<HDC>(_wParam), pwControl, hBrush );
				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( reinterpret_cast<LRESULT>(hBrush), reinterpret_cast<INT_PTR>(hBrush) ); }
				break;
			}
			case WM_CTLCOLORDLG : {
				HBRUSH hBrush = NULL;
				CWidget * pwControl = LSW_WIN2CLASS( reinterpret_cast<HWND>(_lParam) );
				LSW_HANDLED hHandled = pmwThis->CtlColorDlg( reinterpret_cast<HDC>(_wParam), pwControl, hBrush );
				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( reinterpret_cast<LRESULT>(hBrush), reinterpret_cast<INT_PTR>(hBrush) ); }
				break;
			}

			// =======================================
			// Activation.
			// =======================================
			case WM_ACTIVATE : {
				BOOL bMinimized = HIWORD( _wParam ) != 0;
				WORD _wState = LOWORD( _wParam );
				CWidget * pwPrev =  nullptr;
				if ( _lParam ) {
					pwPrev = LSW_WIN2CLASS( reinterpret_cast<HWND>(_lParam) );
				}
				LSW_HANDLED hHandled = pmwThis->Activate( bMinimized, _wState, pwPrev );
				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
				break;
			}
			case WM_NCACTIVATE : {
				if ( pmwThis ) {
					LSW_RET( pmwThis->DockNcActivate( pmwThis, _wParam, _lParam, TRUE ), pmwThis->DockNcActivate( pmwThis, _wParam, _lParam, FALSE ) );
				}
				break;
			}
			case WM_ENABLE : {
				if ( pmwThis ) {
					LSW_RET( pmwThis->DockEnable( pmwThis, _wParam, _lParam, TRUE ), pmwThis->DockEnable( pmwThis, _wParam, _lParam, FALSE ) );
				}
				break;
			}
			case WM_SETFOCUS : {
				if ( pmwThis ) {
					pmwThis->m_bHasFocus = true;
					LSW_HANDLED hHandled = pmwThis->SetFocus( reinterpret_cast<HWND>(_wParam) );
					// An application should return zero if it processes this message.
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
				}
				break;
			}
			case WM_KILLFOCUS : {
				if ( pmwThis ) {
					pmwThis->m_bHasFocus = false;
					LSW_HANDLED hHandled = pmwThis->KillFocus( reinterpret_cast<HWND>(_wParam) );
					// An application should return zero if it processes this message.
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
				}
				break;
			}

			// =======================================
			// Keyboard.
			// =======================================
			case WM_INPUT : {
				LSW_HANDLED hHandled = pmwThis->Input( static_cast<INT>(_wParam), reinterpret_cast<HRAWINPUT>(_lParam) );

				// Return value
				//	If an application processes this message, it should return zero.
				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
				break;
			}
			case WM_INPUT_DEVICE_CHANGE : {
				LSW_HANDLED hHandled = pmwThis->InputDeviceChanged( static_cast<INT>(_wParam), reinterpret_cast<HANDLE>(_lParam) );

				// Return value
				//	If an application processes this message, it should return zero.
				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
				break;
			}
			case WM_KEYDOWN : {
				LSW_HANDLED hHandled = pmwThis->KeyDown( static_cast<UINT>(_wParam), static_cast<UINT>(_lParam) );

				// Return value
				//	An application should return zero if it processes this message.
				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
				break;
			}
			case WM_KEYUP : {
				LSW_HANDLED hHandled = pmwThis->KeyUp( static_cast<UINT>(_wParam), static_cast<UINT>(_lParam) );

				// Return value
				//	An application should return zero if it processes this message.
				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
				break;
			}
			case WM_SYSKEYDOWN : {
				LSW_HANDLED hHandled = pmwThis->SysKeyDown( static_cast<UINT>(_wParam), static_cast<UINT>(_lParam) );

				// Return value
				//	An application should return zero if it processes this message.
				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
				break;
			}
			case WM_SYSKEYUP : {
				LSW_HANDLED hHandled = pmwThis->SysKeyUp( static_cast<UINT>(_wParam), static_cast<UINT>(_lParam) );

				// Return value
				//	An application should return zero if it processes this message.
				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
				break;
			}

			// =======================================
			// Mouse.
			// =======================================
			{
				case WM_CAPTURECHANGED : {
					HWND hWnd = reinterpret_cast<HWND>(_lParam);
					LSW_HANDLED hHandled = pmwThis->CaptureChanged( LSW_WIN2CLASS( hWnd ) );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
				case WM_LBUTTONDBLCLK : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->LButtonDblClk( static_cast<DWORD>(_wParam), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
				case WM_LBUTTONDOWN : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->LButtonDown( static_cast<DWORD>(_wParam), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
				case WM_LBUTTONUP : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->LButtonUp( static_cast<DWORD>(_wParam), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
				case WM_MBUTTONDBLCLK : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->MButtonDblClk( static_cast<DWORD>(_wParam), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
				case WM_MBUTTONDOWN : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->MButtonDown( static_cast<DWORD>(_wParam), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
				case WM_MBUTTONUP : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->MButtonUp( static_cast<DWORD>(_wParam), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
				case WM_MOUSEACTIVATE : {
					HWND hWnd = reinterpret_cast<HWND>(_wParam);
					INT iRetCode = MA_ACTIVATE;
					LSW_HANDLED hHandled = pmwThis->MouseActivate( LSW_WIN2CLASS( hWnd ), static_cast<INT>(_lParam), iRetCode );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( iRetCode, iRetCode ); }
					break;
				}
				case WM_MOUSEHOVER : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->MouseHover( static_cast<DWORD>(_wParam), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
#ifdef WM_MOUSEHWHEEL
				case WM_MOUSEHWHEEL : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->MouseHWheel( static_cast<DWORD>(_wParam), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
#endif	// #ifdef WM_MOUSEHWHEEL
				case WM_MOUSELEAVE : {
					LSW_HANDLED hHandled = pmwThis->MouseLeave();
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
				case WM_MOUSEMOVE : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->MouseMove( static_cast<DWORD>(_wParam), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
				case WM_MOUSEWHEEL : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->MouseWheel( static_cast<DWORD>(_wParam), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
				case WM_NCHITTEST : {
					INT iRetCode = HTNOWHERE;
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->NcHitTest( pPos, iRetCode );
					if ( hHandled == LSW_H_HANDLED ) {
						pmwThis->m_iLastHit = iRetCode;
						LSW_RET( iRetCode, iRetCode );
					}
					break;
				}
				case WM_NCLBUTTONDBLCLK : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->NcButtonDblClk( static_cast<INT>(_wParam), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
				case WM_NCLBUTTONDOWN : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->NcLButtonDown( static_cast<INT>(_wParam), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
				case WM_NCLBUTTONUP : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->NcLButtonUp( static_cast<INT>(_wParam), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
				case WM_NCMBUTTONDBLCLK : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->NcMButtonDblClk( static_cast<INT>(_wParam), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
				case WM_NCMBUTTONDOWN : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->NcMButtonDown( static_cast<INT>(_wParam), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
				case WM_NCMBUTTONUP : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->NcMButtonUp( static_cast<INT>(_wParam), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
#ifdef WM_NCMOUSEHOVER
				case WM_NCMOUSEHOVER : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->NcMouseHover( static_cast<INT>(_wParam), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
#endif	// #ifdef WM_NCMOUSEHOVER
#ifdef WM_NCMOUSELEAVE
				case WM_NCMOUSELEAVE : {
					if ( pmwThis ) {
						LSW_HANDLED hHandled = pmwThis->NcMouseLeave();
						if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					}
					break;
				}
#endif	// #ifdef WM_NCMOUSELEAVE
				case WM_NCMOUSEMOVE : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->NcMouseMove( static_cast<INT>(_wParam), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
				case WM_NCRBUTTONDBLCLK : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->NcRButtonDblClk( static_cast<INT>(_wParam), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
				case WM_NCRBUTTONDOWN : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->NcRButtonDown( static_cast<INT>(_wParam), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
				case WM_NCRBUTTONUP : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->NcRButtonUp( static_cast<INT>(_wParam), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
#ifdef WM_NCXBUTTONDBLCLK
				case WM_NCXBUTTONDBLCLK : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->NcXButtonDblClk( static_cast<INT>(_wParam), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
#endif	// #ifdef WM_NCXBUTTONDBLCLK
#ifdef WM_NCXBUTTONDOWN
				case WM_NCXBUTTONDOWN : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->NcXButtonDown( static_cast<INT>(_wParam), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
#endif	// #ifdef WM_NCXBUTTONDOWN
#ifdef WM_NCXBUTTONUP
				case WM_NCXBUTTONUP : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->NcXButtonUp( static_cast<INT>(_wParam), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
#endif	// #ifdef WM_NCXBUTTONUP
				case WM_RBUTTONDBLCLK : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->RButtonDblClk( static_cast<INT>(_wParam), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
				case WM_RBUTTONDOWN : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->RButtonDown( static_cast<INT>(_wParam), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
				case WM_RBUTTONUP : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->RButtonUp( static_cast<INT>(_wParam), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
#ifdef WM_XBUTTONDBLCLK
				case WM_XBUTTONDBLCLK : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->XButtonDblClk( GET_KEYSTATE_WPARAM( _wParam ), GET_XBUTTON_WPARAM( _wParam ), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
#endif	// #ifdef WM_XBUTTONDBLCLK
#ifdef WM_XBUTTONDOWN
				case WM_XBUTTONDOWN : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->XButtonDown( GET_KEYSTATE_WPARAM( _wParam ), GET_XBUTTON_WPARAM( _wParam ), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
#endif	// #ifdef WM_XBUTTONDOWN
#ifdef WM_XBUTTONUP
				case WM_XBUTTONUP : {
					POINTS pPos = {
						GET_X_LPARAM( _lParam ),
						GET_Y_LPARAM( _lParam ),
					};
					LSW_HANDLED hHandled = pmwThis->XButtonUp( GET_KEYSTATE_WPARAM( _wParam ), GET_XBUTTON_WPARAM( _wParam ), pPos );
					if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
					break;
				}
#endif	// #ifdef WM_XBUTTONUP
			}

			// =======================================
			// Cursor.
			// =======================================
			case WM_SETCURSOR : {
				HWND hWnd = reinterpret_cast<HWND>(_wParam);
				CWidget * pwWidget = LSW_WIN2CLASS( hWnd );
				LSW_HANDLED hHandled = pmwThis->SetCursor( pwWidget, LOWORD( _lParam ), HIWORD( _lParam ) );
				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( TRUE, TRUE ); }
				break;
			}

			// =======================================
			// Scroll.
			// =======================================
			case WM_HSCROLL : {
				LSW_HANDLED hHandled = pmwThis->HScroll( HIWORD( _wParam ), LOWORD( _wParam ), reinterpret_cast<HWND>(_lParam) );
				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
				break;
			}
			case WM_VSCROLL : {
				LSW_HANDLED hHandled = pmwThis->VScroll( HIWORD( _wParam ), LOWORD( _wParam ), reinterpret_cast<HWND>(_lParam) );
				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
				break;
			}

			// =======================================
			// Timer.
			// =======================================
			case WM_TIMER : {
				LSW_HANDLED hHandled = pmwThis->Timer( static_cast<UINT_PTR>(_wParam), reinterpret_cast<TIMERPROC>(_lParam) );
				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
				break;
			}

			// =======================================
			// Hotkeys.
			// =======================================
			case WM_HOTKEY : {
				switch ( _wParam ) {
					case IDHOT_SNAPDESKTOP : {
						// Handle.
						break;
					}
					case IDHOT_SNAPWINDOW : {
						// Handle.
						break;
					}
					default : {
						LSW_HANDLED hHandled = pmwThis->Hotkey( static_cast<INT>(_wParam), static_cast<INT>(HIWORD( _lParam )), static_cast<INT>(LOWORD( _lParam )) );
						if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
						break;
					}
				}
				break;
			}

			// =======================================
			// Menus.
			// =======================================
			case WM_CONTEXTMENU : {
				if ( !_wParam ) { LSW_RET( 0, 0 ); }
				CWidget * pwControl = LSW_WIN2CLASS( reinterpret_cast<HWND>(_wParam) );
				HWND hThis = reinterpret_cast<HWND>(_wParam);
				while ( !pwControl ) {
					hThis = ::GetParent( hThis );
					if ( !hThis ) { LSW_RET( 0, 0 ); }
					pwControl = LSW_WIN2CLASS( hThis );
				}
				LSW_HANDLED hHandled = pmwThis->ContextMenu( pwControl, GET_X_LPARAM( _lParam ), GET_Y_LPARAM( _lParam ) );
				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
				break;
			}
			case WM_INITMENUPOPUP : {
				LSW_HANDLED hHandled = pmwThis->InitMenuPopup( reinterpret_cast<HMENU>(_wParam), LOWORD( _lParam ), static_cast<BOOL>(HIWORD( _lParam )) );
				// If an application processes this message, it should return zero.
				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
				break;
			}

			// =======================================
			// Devices.
			// =======================================
			case WM_DEVICECHANGE : {
				LSW_HANDLED hHandled = pmwThis->DeviceChange( static_cast<WORD>(_wParam), _lParam );
				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( BROADCAST_QUERY_DENY, BROADCAST_QUERY_DENY ); }
				break;
			}
			case WM_SYSCOMMAND : {
				LSW_HANDLED hHandled = pmwThis->SysCommand( static_cast<WORD>(_wParam), GET_X_LPARAM( _lParam ), GET_Y_LPARAM( _lParam ) );
				if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 0, 0 ); }
				break;
			}
		}

		if ( _uMsg >= WM_USER && _uMsg <= 0x7FFF ) {
			LSW_HANDLED hHandled = pmwThis->CustomPrivateMsg( _uMsg, _wParam, _lParam );
			if ( hHandled == LSW_H_HANDLED ) { LSW_RET( 1, 1 ); }
		}

		LSW_WNDRET( ::DefWindowProcW( _hWnd, _uMsg, _wParam, _lParam ) );
		LSW_DLGRET( FALSE );

#undef LSW_RET
#undef LSW_DLGRET
#undef LSW_WNDRET
	}

}	// namespace lsw
