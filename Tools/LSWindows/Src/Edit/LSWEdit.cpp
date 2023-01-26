#include "LSWEdit.h"
#include "../Base/LSWBase.h"

namespace lsw {

	// == Members.
	// Window property.
	WCHAR CEdit::m_szProp[2] = { 0 };

	CEdit::CEdit( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		m_wpOrigProc( nullptr ),
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {

		if ( !m_szProp[0] ) {
			m_szProp[0] = L'A' + ((reinterpret_cast<UINT_PTR>(_pwParent) >> 2) & 0x0F) + 16;
		}
	}

	// == Functions.
	// Setting the HWND after the control has been created.
	void CEdit::InitControl( HWND _hWnd ) {
		CWidget::InitControl( _hWnd );

		m_wpOrigProc = CHelpers::SetWndProc( Wnd(), EditOverride );
		::SetPropW( Wnd(), m_szProp, reinterpret_cast<HANDLE>(this) );
	}

	// Sets the character limit.
	void CEdit::LimitText( int _iLen ) {
		if ( Wnd() ) {
			::SendMessageW( Wnd(), EM_LIMITTEXT, static_cast<WPARAM>(_iLen), 0L );
		}
	}

	// Selects a range of text.
	VOID CEdit::SetSel( INT _iStart, INT _iEnd ) const {
		if ( Wnd() ) {
			::SendMessageW( Wnd(), EM_SETSEL, static_cast<WPARAM>(_iStart), static_cast<LPARAM>(_iEnd) );
		}
	}

	// Replaces the selected text in an edit control or a rich edit control with the specified text.
	void CEdit::ReplaceSel( bool _bCanUndo, const wchar_t * _pwcText ) {
		if ( Wnd() ) {
			::SendMessageW( Wnd(), EM_REPLACESEL, _bCanUndo, reinterpret_cast<LPARAM>(_pwcText) );
		}
	}

	// Gets the zero-based index of the uppermost visible line in a multiline edit control.
	int CEdit::GetFirstVisibleLine() const {
		return static_cast<int>(::SendMessageW( Wnd(), EM_GETFIRSTVISIBLELINE, 0L, 0L ));
	}

	// Scrolls the text in a multiline edit control.
	BOOL CEdit::LineScroll( INT _iHorScrollInChars, INT _iVerScrollInLines ) {
		if ( Wnd() ) {
			return static_cast<int>(::SendMessageW( Wnd(), EM_LINESCROLL, static_cast<WPARAM>(_iHorScrollInChars), static_cast<LPARAM>(_iVerScrollInLines) ));
		}
		return FALSE;
	}

	// Edit window procedure.
	LRESULT CALLBACK CEdit::EditOverride( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
		CEdit * ptlThis = reinterpret_cast<CEdit *>(::GetPropW( _hWnd, m_szProp ));
		WNDPROC wpOrig = ptlThis->m_wpOrigProc;
		switch ( _uMsg ) {
			// =======================================
			// Keyboard shortcuts.
			// =======================================
			case WM_KEYDOWN : {
				UINT uiCode = static_cast<UINT>(_wParam);
				//DWORD dwFlags = static_cast<DWORD>(_lParam);
				if ( uiCode == 'A' ) {
					SHORT sState = ::GetAsyncKeyState( VK_CONTROL );
					if ( sState & 0x8000 ) {
						ptlThis->SelectAll();
						return 0;
					}
				}
				break;
			}
		}

		return ::CallWindowProc( wpOrig, _hWnd, _uMsg, _wParam, _lParam );
	}

}	// namespace lsw
