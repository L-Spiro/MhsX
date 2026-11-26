#pragma once

#include "../LSWWin.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	/**
	 * A tab control.  The tab strip is laid out and drawn here (the native control keeps track of the tabs, the selection, and the
	 *	keyboard), so that tabs can have their own widths or fill the control, and so that hovering does not cause flicker.
	 **/
	class CTab : public CWidget {
	public :
		CTab( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CTab();


		// == Enumerations.
		/** How wide the tabs are. */
		enum LSW_TAB_WIDTH_MODE {
			LSW_TWM_TEXT,																	/**< Each tab is as wide as its own text (the default).  If they do not all fit, they share the width equally. */
			LSW_TWM_FILL,																	/**< The tabs fill the width of the control, sharing it equally (some may be 1 pixel wider so that they fit exactly). */
		};


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

		/**
		 * Gets the width a tab needs for its text (the text plus the padding on both sides, not counting a close box or check box).
		 *
		 * \param _iTab The index of the tab.
		 * \return Returns the width in pixels.
		 **/
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
		 * Sets item text.
		 *
		 * \param _iItem The index of the tab.
		 * \param _pwcText The new text.
		 * \return Returns true if successful.
		 **/
		bool								SetItemTextW( int _iItem, LPCWSTR _pwcText );

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
		void								SetShowCloseBoxes( bool _bShowClose ) { m_bShowClose = _bShowClose; UpdateLayout(); }

		/**
		 * Sets how wide the tabs are.
		 *
		 * \param _twmMode LSW_TWM_TEXT for tabs as wide as their text, or LSW_TWM_FILL for tabs that fill the control.
		 **/
		void								SetWidthMode( LSW_TAB_WIDTH_MODE _twmMode ) { m_twmWidthMode = _twmMode; UpdateLayout(); }

		/**
		 * Gets how wide the tabs are.
		 *
		 * \return Returns LSW_TWM_TEXT or LSW_TWM_FILL.
		 **/
		LSW_TAB_WIDTH_MODE					WidthMode() const { return m_twmWidthMode; }

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

		/**
		 * \brief Hit-tests the tab control at client coordinates and returns the hovered tab index.
		 *
		 * Calls TabCtrl_HitTest() (TCM_HITTEST) and optionally filters results to only count hits that are on the tab item
		 * itself (TCHT_ONITEM). Returns -1 when the mouse is not over any tab item.
		 *
		 * \param _iX Client X coordinate.
		 * \param _iY Client Y coordinate.
		 * \return The 0-based tab index under the cursor, or -1 if none.
		 */
		int									HitTestTab( int _iX, int _iY ) const {
			TCHITTESTINFO htiInfo{};
			htiInfo.pt.x = _iX;
			htiInfo.pt.y = _iY;

			const int iTab = int(::SendMessageW( Wnd(), TCM_HITTEST, 0, reinterpret_cast<LPARAM>(reinterpret_cast<TC_HITTESTINFO *>(&htiInfo)) ));
			if ( iTab < 0 ) { return -1; }

			// Optional: ignore edge cases if you only want "on the tab item".
			if ( (htiInfo.flags & TCHT_ONITEM) == 0 ) { return -1; }

			return iTab;
		}

		/**
		 * \brief Sets the current "hot" (mouse-over) tab index, updating state only when it changes.
		 *
		 * When the hot tab changes, this function updates internal state and triggers any necessary UI refresh.
		 * Typical uses include invalidating the control for custom hover rendering and/or notifying the parent.
		 *
		 * \param _iNewHotTab The new 0-based hot tab index, or -1 for "no hot tab."
		 */
		void								SetHotTab( int _iNewHotTab );

		/**
		 * Handles WM_ERASEBKGND.
		 * \brief Allows custom background erasing.
		 *
		 * \param _hDc Device context provided for erasing.
		 * \return Returns a LSW_HANDLED code.
		 */
		//virtual LSW_HANDLED					EraseBkgnd( HDC /*_hDc*/ ) { return m_vTabs.size() ? LSW_H_HANDLED : LSW_H_CONTINUE; }


	protected :
		// == Types.
		// A tab.
		struct LSW_TAB {
			CWidget *						pwWidget;										/**< The page shown by the tab. */
			size_t							sTextLen = 0;									/**< The length of the tab's text. */
			bool							bCheckable = false;								/**< The tab has a check box. */
			bool							bChecked = false;								/**< The check box is checked. */
			RECT							rRect = {};										/**< Where the tab is (client coordinates, not including the extra size of a selected tab). */
		};

		
		// == Members.
		/** Data for each tab. */
		std::vector<LSW_TAB>				m_vTabs;
		/** Original tab function. */
		LONG_PTR							m_lpOriginProc = 0;
		/** THe hot tab. */
		int									m_iHotTab = -1;
		/** If true, close boxes appear in the tabs. */
		bool								m_bShowClose = true;
		/** If true we are tracking mouse events. */
		bool								m_bTrackMouseLeave = false;
		/** If true we are tracking the mouse leaving the non-client parts (the close boxes and check boxes). */
		bool								m_bTrackNcLeave = false;
		/** How wide the tabs are. */
		LSW_TAB_WIDTH_MODE					m_twmWidthMode = LSW_TWM_TEXT;
		/** The top of the row of tabs. */
		int									m_iRowTop = 0;
		/** The bottom of the row of tabs (where the page area begins). */
		int									m_iRowBottom = 0;


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
		 * \brief Begins tracking for WM_MOUSELEAVE so we can clear the hot tab when the mouse exits the tab control.
		 *
		 * Uses TrackMouseEvent() with TME_LEAVE. Call this when receiving WM_MOUSEMOVE, but only once per hover session.
		 * WM_MOUSELEAVE will reset the tracking flag, allowing tracking to be re-armed on the next WM_MOUSEMOVE.
		 */
		void								BeginMouseLeaveTracking();

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

		/**
		 * Works out where every tab goes (from the width mode, the text, and the size of the control) and redraws the tabs.
		 **/
		void								UpdateLayout();

		/**
		 * Gets the font used for the tab text.
		 *
		 * \return Returns the control's font, or the default GUI font.
		 **/
		HFONT								TabFont() const;

		/**
		 * Gets the extra width a tab needs for its close box or check box.
		 *
		 * \param _iTab The index of the tab.
		 * \return Returns the extra width in pixels, or 0 if the tab has neither.
		 **/
		int									BoxWidth( INT _iTab ) const;

		/**
		 * Gets the rectangle in which the native control keeps a tab (not where it is shown).
		 *
		 * \param _iTab The index of the tab.
		 * \return Returns the native rectangle.
		 **/
		RECT								NativeItemRect( INT _iTab ) const;

		/**
		 * Gets the tab at a point.
		 *
		 * \param _pPoint The point in client coordinates.
		 * \return Returns the index of the tab, or -1.
		 **/
		INT									TabFromPoint( const POINT &_pPoint ) const;

		/**
		 * Converts the mouse position of a mouse message to the matching place in the native control's own layout, so that it
		 *	selects the tab that is shown under the mouse.
		 *
		 * \param _lParam The client coordinates from the mouse message.
		 * \return Returns the coordinates to give the native control.
		 **/
		LPARAM								ToNativePoint( LPARAM _lParam ) const;

		/**
		 * Gets the rectangle a tab is drawn in (a selected tab is larger).
		 *
		 * \param _iTab The index of the tab.
		 * \param _bSelected Whether the tab is selected.
		 * \return Returns the rectangle in client coordinates.
		 **/
		RECT								DrawRect( INT _iTab, bool _bSelected ) const;

		/**
		 * Redraws one tab (without erasing, so there is no flicker).
		 *
		 * \param _iTab The index of the tab, or -1 to do nothing.
		 **/
		void								InvalidateTab( INT _iTab );

		/**
		 * Draws the whole control: the background, the page area and its border, and the row of tabs.
		 *
		 * \param _hDc The device context.
		 * \param _rClient The client rectangle.
		 **/
		void								PaintTabs( HDC _hDc, const RECT &_rClient );

		/**
		 * Keeps painting off the child windows and the pages that cover the control.
		 *
		 * \param _hDc The device context to clip.
		 **/
		void								ExcludeCovered( HDC _hDc ) const;

		// The custom tab message handler.
		static LRESULT CALLBACK				CustomTabProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam );


	};

}	// namespace lsw
