#pragma once

#include "../LSWWin.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CTab : public CWidget {
	public :
		CTab( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CTab();


		// == Functions.
		// Virtual client rectangle.  Can be used for things that need to be adjusted based on whether or not status bars, toolbars, etc. are present.
		virtual const LSW_RECT				VirtualClientRect( const CWidget * pwChild ) const;

		// Calculates a tab control's display area given a window rectangle, or calculates the window rectangle that would correspond to a specified display area.
		void								AdjustRect( BOOL _bLarger, RECT * prRect ) const;

		// Inserts a new tab in a tab control. Returns the index of the new tab if successful, or -1 otherwise.
		int									InsertItem( int _iItem, LPTCITEMW _piItem, CWidget * _pwWidget );

		// Inserts a new tab in a tab control. Returns the index of the new tab if successful, or -1 otherwise.
		int									InsertItem( int _iItem, LPTCITEMA _piItem, CWidget * _pwWidget );

		// Removes an item from a tab control.
		void								DeleteItem( int _iItem, bool _bDestroyChildWindow = true );

		// Retrieves the number of tabs in the tab control.
		int									GetItemCount() const;

		// Selects a tab in a tab control.  Returns the index of the previously selected tab if successful, or -1 otherwise.
		int									SetCurSel( int _iItem );

		// Determines the currently selected tab in a tab control.
		int									GetCurSel() const;

		// Determines which tab, if any, is at a specified screen position.  Returns the index of the tab, or -1 if no tab is at the specified position.
		int									HitTest( LPTCHITTESTINFO _lphtiInfo ) const;

		// Gets the rectangle of a given tab.
		LSW_RECT							GetItemRect( INT _iTab ) const;

		// Gets the width of the text in a given tab.
		int									GetTabTextWidth( INT _iTab ) const;

		// Gets the width of the longest text in any tab.
		int									GetLongestTextWidth() const;

		// Sets the rectangle of a given tab.
		DWORD								SetItemSize( int _iW, int _iH );

		// Gets item text.
		std::string							GetItemTextA( int _iItem ) const;

		// Gets item text.
		std::wstring						GetItemTextW( int _iItem ) const;

		// Setting the HWND after the control has been created.
		virtual void						InitControl( HWND _hWnd );

		// Gets the original tab proc.
		WNDPROC								OriginalProc() const { return reinterpret_cast<WNDPROC>(m_lpOriginProc); }

		// Determines the type of control this is.
		virtual uint32_t					WidgetType() const { return LSW_LT_TAB; }

		// Returns true if this is a CTab class.
		virtual bool						IsTab() const { return true; }

		/**
		 * Sets whether to show the close boxes or not.
		 *
		 * \param _bShowClose If true, close boxes are shown in the tabs.
		 */
		void								SetShowCloseBoxes( bool _bShowClose ) { m_bShowClose = _bShowClose; }

	protected :
		// == Types.
		// A tab.
		struct LSW_TAB {
			CWidget *						pwWidget;
			size_t							sTextLen;
		};

		
		// == Members.
		/** Data for each tab. */
		std::vector<LSW_TAB>				m_vTabs;
		/** Original tab function. */
		LONG_PTR							m_lpOriginProc;
		/** If true, close boxes appear in the tabs. */
		bool								m_bShowClose;


		// == Functions.
		// // WM_SIZE.
		virtual LSW_HANDLED					Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight );

		// WM_MOVE.
		virtual LSW_HANDLED					Move( LONG _lX, LONG _lY );

		// Determines the close rectangle.
		static LSW_RECT						GetCloseRect( const LSW_RECT &_rTabRect, bool _bHasFocus );

		// The custom tab message handler.
		static LRESULT CALLBACK				CustomTabProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam );


	};

}	// namespace lsw
