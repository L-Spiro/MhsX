#pragma once
#include "../LSWWin.h"
#include "../Layout/LSWWidgetLayout.h"
#include "../Widget/LSWWidget.h"


namespace lsw {

	class CSplitter : public CWidget {
	public :
		CSplitter( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL );
		~CSplitter();


		// == Enumerations.
		// Splitter styles.
		enum LSW_SPLITTER_STYLES : DWORD {
			LSW_SS_HORIZONTAL				= 0x01,
			LSW_SS_VERTICAL					= 0x02,
		};


	protected :
		// == Members.
		// Are we dragging?
		BOOL								m_bDragging;

		// Last-drawn point.
		POINT								m_pLastPoint;

		// Splitter type.
		DWORD								m_dwSplitterType;

		// Width of the bar.
		DWORD								m_dwBarWidth;


		// == Functions.
		// Draw the XOR bar.
		void								DrawXorBar( HDC _hDc, INT _iX1, INT _iY1, INT _iWidth, INT _iHeight );

		// Draw the bar for this control.
		void								DrawBar( POINT &_pPoint, RECT _rClient, BOOL _bEnforceNew );

		// WM_LBUTTONDOWN.
		virtual LSW_HANDLED					LButtonDown( DWORD _dwVirtKeys, const POINTS &_pCursorPos );

		// WM_MOUSEMOVE.
		virtual LSW_HANDLED					MouseMove( DWORD _dwVirtKeys, const POINTS &_pCursorPos );

		// WM_LBUTTONUP.
		virtual LSW_HANDLED					LButtonUp( DWORD _dwVirtKeys, const POINTS &_pCursorPos );
	};

}	// namespace lsw
