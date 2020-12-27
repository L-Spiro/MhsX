#pragma once

#include "../LSWWin.h"
#include "../Layout/LSWWidgetLayout.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CEdit : public CWidget {
	public :
		CEdit( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		// Setting the HWND after the control has been created.
		virtual void						InitControl( HWND _hWnd );

		// Sets the character limit.
		virtual void						LimitText( int _iLen );

		// Selects a range of text.
		virtual VOID						SetSel( INT _iStart, INT _iEnd ) const;

		// Selects all of the text.
		virtual VOID						SelectAll() const { SetSel( 0, -1 ); }

		// Unselects the text.
		virtual VOID						DeselectAll() const { SetSel( -1, 0 ); }

		// Determines the type of control this is.
		virtual uint32_t					WidgetType() const { return LSW_LT_EDIT; }

		// Returns true if this is a CEdit class.
		virtual bool						IsEdit() const { return true; }

		// Replaces the selected text in an edit control or a rich edit control with the specified text.
		virtual void						ReplaceSel( bool _bCanUndo, const wchar_t * _pwcText );

		// Gets the zero-based index of the uppermost visible line in a multiline edit control.
		virtual int							GetFirstVisibleLine() const;

		// Scrolls the text in a multiline edit control.
		virtual BOOL						LineScroll( INT _iHorScrollInChars, INT _iVerScrollInLines );


	protected :
		// == Members.
		// Original message handler.
		WNDPROC								m_wpOrigProc;

		// Window property.
		static WCHAR						m_szProp[2];


		// == Functions.
		// Edit window procedure.
		static LRESULT CALLBACK				EditOverride( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam );

	private :
		typedef CWidget						Parent;
	};
		
}	// namespace lsw
