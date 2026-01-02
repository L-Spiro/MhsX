#pragma once

#include "../LSWWin.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	/**
	 * \brief A status-bar control widget.
	 *
	 * Wraps a Win32 status bar and provides helpers for managing parts and text.
	 */
	class CStatusBar : public CWidget {
	public :
		/**
		 * \brief Creates a status-bar widget.
		 *
		 * \param _wlLayout Layout information for the widget.
		 * \param _pwParent Parent widget.
		 * \param _bCreateWidget If true, the underlying window/control is created immediately.
		 * \param _hMenu Optional menu/ID handle used by the underlying Win32 create call.
		 * \param _ui64Data User-defined data associated with the widget.
		 */
		CStatusBar( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Types.
		/**
		 * \brief Describes a single status-bar part.
		 *
		 * Parts are defined using right-edge coordinates. A part may optionally lock to the
		 * right side, allowing it to remain anchored during resizing.
		 */
		struct LSW_STATUS_PART {
			/** The right-edge coordinate for the part, in client coordinates. */
			INT								iRightCoord;

			/** TRUE to lock this part to the right edge when resizing; FALSE otherwise. */
			BOOL							bLockToRight;
		};

		/**
		 * \brief Per-part state stored by the wrapper.
		 *
		 * When a status-bar part is set to SBT_OWNERDRAW, the common-control no longer
		 * stores a string for that part. This wrapper stores the part text and flags so
		 * callers can continue using GetText* / SetText* while the parent handles
		 * WM_DRAWITEM.
		 */
		struct LSW_STATUS_ITEM {
			/** The part text, stored as UTF-16. */
			std::wstring					wsText;
			/** The SB_SETTEXT high-word flags (excluding SBT_OWNERDRAW, which is forced). */
			UINT							uiType;
			/** The text color for this item. */
			COLORREF						crTextColor = ::GetSysColor( COLOR_WINDOWTEXT );
			/** The text color for this item when disabled. */
			COLORREF						crDisabledTextColor = ::GetSysColor( COLOR_GRAYTEXT );
			/** The background color for this item. */
			COLORREF						crBkColor = ::GetSysColor( COLOR_3DFACE );
		};


		// == Functions.
		/**
		 * \brief Sets the parts for the status bar.
		 *
		 * \param _vParts The parts to apply.
		 * \return TRUE if the parts were set successfully; FALSE otherwise.
		 */
		BOOL								SetParts( const std::vector<LSW_STATUS_PART> &_vParts );

		/**
		 * \brief Sets the parts for the status bar.
		 *
		 * \param _pspParts Pointer to an array of parts.
		 * \param _sTotal Total number of parts in the array.
		 * \return TRUE if the parts were set successfully; FALSE otherwise.
		 */
		BOOL								SetParts( const LSW_STATUS_PART * _pspParts, size_t _sTotal );

		/**
		 * \brief Retrieves the text for a part as ANSI.
		 *
		 * \param _iIdx The zero-based part index.
		 * \param _lpString Destination buffer for the text.
		 * \param _nMaxCount Maximum number of characters to copy (including terminator, if applicable).
		 * \return A 32-bit value containing two 16-bit values:
		 *	The low word specifies the length, in characters, of the text.
		 *	The high word specifies the type of operation used to draw the text.
		 */
		DWORD								GetTextA( INT _iIdx, LPSTR _lpString, INT _nMaxCount ) const;

		/**
		 * \brief Retrieves the text for a part as UTF-16.
		 *
		 * \param _iIdx The zero-based part index.
		 * \param _lpString Destination buffer for the text.
		 * \param _nMaxCount Maximum number of characters to copy (including terminator, if applicable).
		 * \return A 32-bit value containing two 16-bit values:
		 *	The low word specifies the length, in characters, of the text.
		 *	The high word specifies the type of operation used to draw the text.
		 */
		DWORD								GetTextW( INT _iIdx, LPWSTR _lpString, INT _nMaxCount ) const;

		/**
		 * \brief Gets the text of a part as ANSI.
		 *
		 * \param _iIdx The zero-based part index.
		 * \return The part text.
		 */
		virtual std::string					GetTextA( INT _iIdx ) const;

		/**
		 * \brief Gets the text of a part as UTF-16.
		 *
		 * \param _iIdx The zero-based part index.
		 * \return The part text.
		 */
		virtual std::wstring				GetTextW( INT _iIdx ) const;

		/**
		 * \brief Gets the text of the first part as ANSI.
		 *
		 * \return The first-part text.
		 */
		virtual std::string					GetTextA() const { return GetTextA( 0 ); }

		/**
		 * \brief Gets the text of the first part as UTF-16.
		 *
		 * \return The first-part text.
		 */
		virtual std::wstring				GetTextW() const { return GetTextW( 0 ); }

		/**
		 * \brief Gets the text length of the first part as ANSI.
		 *
		 * \return The length, in characters, of the first-part text.
		 */
		virtual INT							GetTextLengthA() const { return GetTextLengthA( 0 ); }

		/**
		 * \brief Gets the text length of the first part as UTF-16.
		 *
		 * \return The length, in characters, of the first-part text.
		 */
		virtual INT							GetTextLengthW() const { return GetTextLengthW( 0 ); }

		/**
		 * \brief Gets the text length of the given part as ANSI.
		 *
		 * \param _iIdx The zero-based part index.
		 * \return The length, in characters, of the part text.
		 */
		virtual INT							GetTextLengthA( INT _iIdx ) const;

		/**
		 * \brief Gets the text length of the given part as UTF-16.
		 *
		 * \param _iIdx The zero-based part index.
		 * \return The length, in characters, of the part text.
		 */
		virtual INT							GetTextLengthW( INT _iIdx ) const;

		/**
		 * \brief Sets the text in the specified part as ANSI, forcing SBT_OWNERDRAW.
		 *
		 * \param _iIdx The zero-based part index.
		 * \param _uiType SB_SETTEXT high-word flags (SBT_*) excluding SBT_OWNERDRAW, which is always forced.
		 * \param _lpString The text to set.
		 * \return TRUE if the text was set successfully; FALSE otherwise.
		 */
		virtual BOOL						SetTextA( INT _iIdx, UINT _uiType, LPCSTR _lpString );

		/**
		 * \brief Sets the text in the specified part as UTF-16, forcing SBT_OWNERDRAW.
		 *
		 * \param _iIdx The zero-based part index.
		 * \param _uiType SB_SETTEXT high-word flags (SBT_*) excluding SBT_OWNERDRAW, which is always forced.
		 * \param _lpString The text to set.
		 * \return TRUE if the text was set successfully; FALSE otherwise.
		 */
		virtual BOOL						SetTextW( INT _iIdx, UINT _uiType, LPCWSTR _lpString );

		/**
		 * Sets the text colors for a given item.
		 * 
		 * \param _crColorNormal The normal text color.  The default is ::GetSysColor( COLOR_WINDOWTEXT ).
		 * \param _crColorDisabled The disabled text color.  Default is ::GetSysColor( COLOR_GRAYTEXT ).
		 * \param _iIdx The index of the item whose colors are to be set.
		 **/
		void								SetItemTextColors( COLORREF _crColorNormal, COLORREF _crColorDisabled, INT _iIdx );

		/**
		 * Sets the background color for a given item.
		 * 
		 * \param _coColor The color to set.  The default is ::GetSysColor( COLOR_3DFACE ).
		 * \param _iIdx The index of the item whose background color is to be set.
		 **/
		void								SetItemBkColor( COLORREF _coColor, INT _iIdx );

		/**
		 * Sets the text colors for all items.
		 * 
		 * \param _crColorNormal The normal text color.  The default is ::GetSysColor( COLOR_WINDOWTEXT ).
		 * \param _crColorDisabled The disabled text color.  Default is ::GetSysColor( COLOR_GRAYTEXT ).
		 **/
		void								SetTextColors( COLORREF _crColorNormal, COLORREF _crColorDisabled ) {
			for ( auto I = m_vItems.size(); I--; ) {
				SetItemTextColors( _crColorNormal, _crColorDisabled, INT( I ) );
			}
		}

		/**
		 * Sets the background color for all items.
		 * 
		 * \param _coColor The background color to set.  The default is ::GetSysColor( COLOR_3DFACE ).
		 **/
		void								SetBkColors( COLORREF _coColor ) {
			for ( auto I = m_vItems.size(); I--; ) {
				SetItemBkColor( _coColor, INT( I ) );
			}
		}

		/**
		 * \brief Handles WM_DRAWITEM for this status bar.
		 *
		 * Call this from the parent window's WM_DRAWITEM handler when all parts are
		 * SBT_OWNERDRAW.
		 *
		 * \param _pDis The DRAWITEMSTRUCT pointer received from WM_DRAWITEM.
		 * \return TRUE if the item was drawn; FALSE if the message was not for this control.
		 */
		BOOL								HandleDrawItem( const DRAWITEMSTRUCT * _pDis ) const;

		/**
		 * \brief Sets the text in the first part as ANSI.
		 *
		 * \param _lpString The text to set.
		 * \return TRUE if the text was set successfully; FALSE otherwise.
		 */
		virtual BOOL						SetTextA( LPCSTR _lpString ) { return SetTextA( 0, 0, _lpString ); }

		/**
		 * \brief Sets the text in the first part as UTF-16.
		 *
		 * \param _lpwString The text to set.
		 * \return TRUE if the text was set successfully; FALSE otherwise.
		 */
		virtual BOOL						SetTextW( LPCWSTR _lpwString ) { return SetTextW( 0, 0, _lpwString ); }

		/**
		 * \brief Determines the type of control this is.
		 *
		 * \return The widget type identifier.
		 */
		virtual uint32_t					WidgetType() const { return LSW_LT_STATUSBAR; }

		/**
		 * \brief Returns true if this is a CStatusBar instance.
		 *
		 * \return True if this widget is a status bar; false otherwise.
		 */
		virtual bool						IsStatusBar() const { return true; }

		/**
		 * Gets the font used by this control.
		 * 
		 * \return Returns the font used by this control.
		 **/
		virtual HFONT						Font() const;

		/**
		 * \brief Automatically sizes parts to their text and anchors a right-locked suffix to the right edge.
		 *
		 * All parts before the first bLockToRight part are laid out from left-to-right. All parts from the first
		 * bLockToRight part to the end are laid out from right-to-left (anchored to the right edge).
		 *
		 * By default, the last non-locked part becomes a “filler” that expands to consume remaining space up to
		 * the left edge of the right-locked suffix (or to the end if no suffix exists).
		 *
		 * \param _bFillLastNonLocked If TRUE, the last non-locked part is expanded to fill remaining space.
		 * \return TRUE on success; FALSE otherwise.
		 */
		BOOL								AutoFitPartsToTextLocked( BOOL _bFillLastNonLocked = TRUE );



	protected :
		// == Members.
		/** The status-bar parts currently configured for the control. */
		std::vector<LSW_STATUS_PART>		m_vParts;
		/** Per-part text and flags used for owner-draw. */
		std::vector<LSW_STATUS_ITEM>		m_vItems;
		/** Original message handler. */
		WNDPROC								m_wpOrigProc = nullptr;
		/** Window property. */
		static WCHAR						m_szProp[2];
		/** \brief Cached SB_SETPARTS right-edge array used to skip redundant updates. */
		std::vector<INT>					m_vLastAutoFitRights;
		/** TRUE if SBT_OWNERDRAW has been asserted for all parts since the last SB_SETPARTS call. */
		BOOL								m_bAllOwnerDrawApplied = FALSE;



		// == Functions.
		/**
		 * Attaches an HWND to this widget after creation.
		 * \brief Finalizes control initialization once the window handle exists.
		 *
		 * \param _hWnd The created window handle.
		 */
		virtual void						InitControl( HWND _hWnd ) override;

		/**
		 * \brief Evaluates expressions to determine a new rectangle for the control.
		 *
		 * Called during layout/resizing to compute and apply an updated size/position.
		 */
		virtual void						EvalNewSize();

		/**
		 * Handles WM_PAINT.
		 * \brief Performs painting for the client area.
		 *
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					Paint() override;


		/**
		 * \brief Handles WM_DRAWITEM for an owner-drawn control.
		 *
		 * \param _wControlId The control identifier (DRAWITEMSTRUCT::CtlID).
		 * \param _disItem The DRAWITEMSTRUCT describing what to draw.
		 * \return LSW_HANDLED::LSW_H_HANDLED if the message was handled and drawing was performed; otherwise
		 * returns LSW_HANDLED::LSW_H_NOT_HANDLED to allow default/other handling.
		 *
		 * \note This is typically called from the parent window's WM_DRAWITEM handler.
		 *
		 * \note For SBT_OWNERDRAW status-bar parts, DRAWITEMSTRUCT::itemID is the zero-based part index.
		 * The status-bar window handle is in DRAWITEMSTRUCT::hwndItem.
		 */
		virtual LSW_HANDLED					DrawItem( WORD _wControlId, const DRAWITEMSTRUCT &_disItem ) override;

		/**
		 * Handles WM_ERASEBKGND.
		 * \brief Allows custom background erasing.
		 *
		 * \param _hDc Device context provided for erasing.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					EraseBkgnd( HDC /*_hDc*/ ) { return LSW_H_HANDLED; }

		/**
		 * \brief Draws one status-bar part the same way the common control typically does.
		 *
		 * \param _hDc The target device context.
		 * \param _rRc The part rectangle.
		 * \param _uiType The stored SB_SETTEXT flags for the part (excluding SBT_OWNERDRAW).
		 * \param _pwcText The text to draw.
		 * \param _bEnabled TRUE if enabled; FALSE otherwise.
		 * \param _sIdx The index of the item being drawn.
		 */
		void								DefaultDrawStatusPart( HDC _hDc, const RECT &_rRc, UINT _uiType, LPCWSTR _pwcText, BOOL _bEnabled, size_t _sIdx ) const;

		/**
		 * The message handler for the Status Bar (implements custom drawing).
		 * 
		 * \param _hWnd Window receiving the message.
		 * \param _uMsg Message identifier (WM_*).
		 * \param _wParam WPARAM message data.
		 * \param _lParam LPARAM message data.
		 * \return Returns an LRESULT as defined by the message semantics.
		 **/
		static LRESULT CALLBACK				StatusOverride( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam );

		/**
		 * \brief Draws the standard status-bar size grip in the lower-right corner.
		 *
		 * This routine draws a size grip only when the status-bar has the SBARS_SIZEGRIP style.
		 * If visual styles/themes are active, it draws the themed gripper (STATUS class, SP_GRIPPER).
		 * Otherwise it falls back to the classic non-themed size grip using DrawFrameControl().
		 *
		 * \param _hWnd The status-bar window handle.
		 * \param _hDc The device context to draw into.
		 * \param _rClient The full client rectangle of the status bar in the coordinate space of \p _hDc.
		 *
		 * \note This should be called after the status-bar background and panes have been drawn so the
		 * gripper is rendered on top.
		 *
		 * \note The grip rectangle size is derived from SM_CXVSCROLL and SM_CYHSCROLL to match the system’s
		 * standard grip metrics.
		 */
		static void							DrawStatusGrip( HWND _hWnd, HDC _hDc, const RECT &_rClient );

		/**
		 * \brief Measures the pixel width of a single UTF-16 text segment using DrawTextW() in DT_CALCRECT mode.
		 *
		 * This helper assumes the caller has already selected the desired font into \p _hDc.
		 *
		 * \param _hDc The device context used for measurement.
		 * \param _pwc A NUL-terminated UTF-16 string to measure. If NULL or empty, returns 0.
		 * \param _uiDrawFlags The DrawTextW() flags used for measurement. This function always ORs DT_CALCRECT
		 * into the call, so \p _uiDrawFlags should include alignment and single-line flags as desired.
		 * \return The width, in pixels, of the rendered text.
		 */
		static int							MeasureTextSegmentW( HDC _hDc, const wchar_t * _pwc, UINT _uiDrawFlags );

		/**
		 * \brief Measures the pixel width of status-bar text as it would be rendered for a pane.
		 *
		 * If tab parsing is enabled (i.e. SBT_NOTABPARSING is not set) and the string contains one or two tab
		 * characters, the status bar can display up to 3 segments (left, center, right). This helper measures
		 * each segment independently and returns the maximum segment width, which is sufficient for sizing a
		 * pane intended to contain that text without truncation for any of the three alignments.
		 *
		 * This helper assumes the caller has already selected the desired font into \p _hDc.
		 *
		 * \param _hDc The device context used for measurement.
		 * \param _pwcText A NUL-terminated UTF-16 string to measure. If NULL or empty, returns 0.
		 * \param _uiType The status-bar text type flags (SBT_*). Only SBT_NOTABPARSING and SBT_RTLREADING
		 * are considered for measurement.
		 * \return The width, in pixels, required for the widest segment of the text as rendered by a status bar.
		 */
		static int							MeasureStatusTextW( HDC _hDc, LPCWSTR _pwcText, UINT _uiType );

		/**
		 * \brief Returns the default horizontal padding used when converting measured text width into a pane width.
		 *
		 * The returned value represents extra horizontal space added to the measured text width to account for
		 * typical status-bar insets, separators, and visual breathing room so text does not touch pane edges.
		 *
		 * \return The horizontal padding, in pixels, to add to the measured text width when computing pane width.
		 */
		static int							StatusPanePaddingX();



	private :
		/** The parent type. */
		typedef CWidget						Parent;
	};

}	// namespace lsw
