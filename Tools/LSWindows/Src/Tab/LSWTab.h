#pragma once

#include "../LSWWin.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CTab : public CWidget {
	public :
		CTab( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CTab();


		// == Functions.
		/**
		 * Gets the virtual client rectangle.
		 * \brief Allows parents to adjust available space for toolbars, status bars, or splitters.
		 *
		 * \param pwChild Optional child for which the virtual rect is requested.
		 * \return Returns the virtual client rectangle.
		 */
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
		virtual INT							GetCurSel() const;

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

		/**
		 * Attaches an HWND to this widget after creation.
		 * \brief Finalizes control initialization once the window handle exists.
		 *
		 * \param _hWnd The created window handle.
		 */
		virtual void						InitControl( HWND _hWnd );

		// Gets the original tab proc.
		WNDPROC								OriginalProc() const { return reinterpret_cast<WNDPROC>(m_lpOriginProc); }

		/**
		 * Gets the widget type identifier.
		 * \brief Returns one of LSW_LT_* constants.
		 *
		 * \return Returns LSW_LT_WIDGET in the base class.
		 */
		virtual uint32_t					WidgetType() const { return LSW_LT_TAB; }

		/** \brief Returns true if this is a CTab. */
		virtual bool						IsTab() const { return true; }

		/**
		 * Sets whether to show the close boxes or not.
		 *
		 * \param _bShowClose If true, close boxes are shown in the tabs.
		 */
		void								SetShowCloseBoxes( bool _bShowClose ) { m_bShowClose = _bShowClose; }

		/**
		 * Sets a tab to be checkable.
		 * 
		 * \param _iItem The index of the tab to set as checkable.
		 **/
		void								SetCheckable( int _iItem );

		/**
		 * Sets a tab as checked or not.
		 * 
		 * \param _iItem The index of the tab to set or unset as checked.
		 * \param _bChecked Checked or unchecked.
		 * \return Returns true if the given tab exists.
		 **/
		bool								SetChecked( int _iItem, bool _bChecked );

		/**
		 * Toggles the check status of a given tab.
		 * 
		 * \param _iItem The tab whose check status is to be toggled.  The tab must be marked as checkable.
		 **/
		void								ToggleCheck( int _iItem );

		/**
		 * Returns true if any tabs are checkable.
		 * 
		 * \return Returns true if any tabs are checkable.
		 **/
		bool								AnyAreCheckable() const;

		/**
		 * Checks for the given tab being checked or not.  It must also be checkable.
		 * 
		 * \param _iItem The index of the tab to check for being both checkable and checked.
		 * \return Returns true if the given tab exists, is checkable, and is checked.
		 **/
		bool								IsChecked( int _iItem ) const;

	protected :
		// == Types.
		// A tab.
		struct LSW_TAB {
			CWidget *						pwWidget;
			size_t							sTextLen = 0;
			bool							bCheckable = false;
			bool							bChecked = false;
		};

		
		// == Members.
		/** Data for each tab. */
		std::vector<LSW_TAB>				m_vTabs;
		/** Original tab function. */
		LONG_PTR							m_lpOriginProc;
		/** If true, close boxes appear in the tabs. */
		bool								m_bShowClose;


		// == Functions.
		/**
		 * Handles WM_SIZE.
		 * \brief Responds to client-area size changes.
		 *
		 * Called after the window's client size changes. Override to update layouts,
		 * reposition children, or cache new sizes.
		 *
		 * \param _wParam The requested sizing type (e.g., SIZE_RESTORED, SIZE_MINIMIZED).
		 * \param _lWidth The new client width, in pixels.
		 * \param _lHeight The new client height, in pixels.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight );

		/**
		 * Handles WM_MOVE.
		 * \brief Notified when the window is moved.
		 *
		 * \param _lX New x-position of the window (screen coordinates).
		 * \param _lY New y-position of the window (screen coordinates).
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					Move( LONG _lX, LONG _lY );

		/**
		 * Handles WM_ERASEBKGND.
		 * \brief Allows custom background erasing.
		 *
		 * \param _hDc Device context provided for erasing.
		 * \return Returns a LSW_HANDLED code.
		 */
		//virtual LSW_HANDLED					EraseBkgnd( HDC /*_hDc*/ );

		// Determines the close rectangle.
		static LSW_RECT						GetCloseRect( const LSW_RECT &_rTabRect, bool _bHasFocus );

		// The custom tab message handler.
		static LRESULT CALLBACK				CustomTabProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam );


	};

}	// namespace lsw
