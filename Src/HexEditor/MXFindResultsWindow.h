#pragma once

#include "../MXMhsX.h"
#include "../Strings/MXSecureWString.h"
#include "../Utilities/MXUtilities.h"
#include "MXFindResults.h"
#include "MXHexEditorInterface.h"

#include <Docking/LSWDockable.h>
#include <ListView/LSWListView.h>

#include <memory>
#include <string>
#include <vector>

using namespace lsw;

namespace mx {

	/**
	 * What the Find Results window needs from the Hex Editor: the results, the starting addresses of the views, and a way to show a
	 *	result.
	 **/
	class CFindResultsHost {
	public :
		virtual ~CFindResultsHost() {}


		// == Functions.
		/**
		 * Gets the results of the Find All searches.
		 *
		 * \return Returns the results.
		 **/
		virtual const CFindResults &										FindResultsList() const = 0;

		/**
		 * Removes every result.
		 **/
		virtual void														FindResultsClear() = 0;

		/**
		 * Shows a result: the tab of its data is made current and the result is selected.
		 *
		 * \param _pheiSource The data that was searched.
		 * \param _ui64Addr The address of the result.
		 * \param _ui64Size The size of the result.
		 **/
		virtual void														FindResultsSelect( const CHexEditorInterface * _pheiSource, uint64_t _ui64Addr, uint64_t _ui64Size ) = 0;

		/**
		 * Gets the starting address of the view of some data (the number added to the addresses shown).
		 *
		 * \param _pheiSource The data.
		 * \return Returns the starting address, or 0.
		 **/
		virtual uint64_t													FindResultsStartingAddress( const CHexEditorInterface * _pheiSource ) = 0;

		/**
		 * Gets the data of the current tab.
		 *
		 * \return Returns the data of the current tab, or nullptr.
		 **/
		virtual const CHexEditorInterface *									FindResultsCurrentSource() = 0;

		/**
		 * Shows a message in the status bar.
		 *
		 * \param _swsText The message.
		 * \param _bWarning If true, the message is a warning.
		 **/
		virtual void														FindResultsStatus( const CSecureWString &_swsText, bool _bWarning ) = 0;
	};


	/**
	 * The list of the Find Results window.  It holds no items of its own (LVS_OWNERDATA); the window supplies the text.
	 **/
	class CFindResultsListView : public lsw::CListView {
	public :
		CFindResultsListView( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		/**
		 * Supplies the text of an item (LVN_GETDISPINFOW).
		 *
		 * \param _plvdiInfo The request.
		 * \return Returns TRUE.
		 **/
		virtual BOOL														GetDispInfoNotify( NMLVDISPINFOW * _plvdiInfo );

		/**
		 * Handles LVN_ITEMCHANGED.  A newly focused item is shown in the hex editor.
		 *
		 * \param _lplvParm The notification.
		 * \return Returns a LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED													Notify_ItemChanged( LPNMLISTVIEW _lplvParm );

		/**
		 * Sorting does not apply (the results are in address order).
		 *
		 * \param _iSubItem The column clicked.
		 * \return Returns FALSE.
		 **/
		virtual BOOL														SortItems( INT /*_iSubItem*/ ) { return FALSE; }


	private :
		/** The parent object. */
		typedef lsw::CListView												CParent;
	};


	/**
	 * The Find Results window: a dockable list of the matches of Find All (Address and Value), with a map on the left that shows
	 *	where the matches are in the data.  Clicking or dragging on the map goes to the nearest match.  It likes to be docked along
	 *	the bottom of the Hex Editor.
	 **/
	class CFindResultsWindow : public lsw::CDockable {
	public :
		CFindResultsWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CFindResultsWindow();


		// == Functions.
		/**
		 * Handles WM_INITDIALOG.  Sets up the list and the map and starts watching the results.
		 *
		 * \return Returns a LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED													InitDialog();

		/**
		 * Handles WM_CONTEXTMENU.  Shows the Find Results menu.
		 *
		 * \param _pwControl The control that was right-clicked.
		 * \param _iX The horizontal position of the cursor in screen coordinates, or -1 if the menu was opened with the keyboard.
		 * \param _iY The vertical position of the cursor in screen coordinates, or -1 if the menu was opened with the keyboard.
		 * \return Returns a LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED													ContextMenu( CWidget * _pwControl, INT _iX, INT _iY );

		/**
		 * Handles WM_TIMER.  Updates the list when the results change.
		 *
		 * \param _uiptrId The timer identifier.
		 * \param _tpProc The timer callback, if any.
		 * \return Returns a LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED													Timer( UINT_PTR _uiptrId, TIMERPROC _tpProc );

		/**
		 * The WM_SIZE handler.  Places the map and the list.
		 *
		 * \param _wParam The type of resizing requested.
		 * \param _lWidth The new width of the client area.
		 * \param _lHeight The new height of the client area.
		 * \return Returns a LSW_HANDLED enumeration.
		 **/
		virtual LSW_HANDLED													Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight );

		/**
		 * Returns the desired width when first docking into a vertical split.
		 *
		 * \return Returns the desired width in pixels.
		 **/
		virtual INT															PreferredDockWidth() { return 320; }

		/**
		 * Returns the desired height when first docking into a horizontal split.
		 *
		 * \return Returns the desired height in pixels.
		 **/
		virtual INT															PreferredDockHeight() { return 180; }

		/**
		 * Sets the object that supplies the results.
		 *
		 * \param _pfrhHost The host.
		 **/
		void																SetHost( CFindResultsHost * _pfrhHost );

		/**
		 * Updates the list and the map if the results changed.
		 *
		 * \param _bForce If true, they are updated even if the results did not change.
		 **/
		void																Refresh( bool _bForce = false );

		/**
		 * Supplies the text of an item of the list.
		 *
		 * \param _plvdiInfo The request.
		 **/
		void																GetDispInfo( NMLVDISPINFOW * _plvdiInfo );

		/**
		 * Called when an item of the list changes state.  A newly focused item is shown in the hex editor.
		 *
		 * \param _lplvParm The notification.
		 **/
		void																ItemChanged( LPNMLISTVIEW _lplvParm );

		/**
		 * Determines whether the keyboard focus is in the window (so that Ctrl+C copies from it rather than from the hex editor).
		 *
		 * \return Returns true if the window or one of its children has the focus.
		 **/
		bool																HasFocus() const;

		/**
		 * Copies the selected rows ("Address<Tab>Value" lines) to the clipboard.
		 **/
		void																CopySelected() { CopyRows( false ); }


	protected :
		// == Enumerations.
		/** Menu commands. */
		enum MX_FIND_RESULTS_COMMANDS : UINT {
			MX_FRC_COPY														= 100,
			MX_FRC_COPY_ALL,
			MX_FRC_SELECT_ALL,
			MX_FRC_CLEAR,
		};


		// == Types.
		/** A row of the list. */
		struct MX_ROW {
			const CFindResults::MX_FIND_SET *								pfsSet = nullptr;		/**< The set of results. */
			size_t															sSet = 0;				/**< The index of the set. */
			size_t															sMatch = 0;				/**< The index of the match in the set. */
		};


		// == Members.
		/** Supplies the results. */
		CFindResultsHost *													m_pfrhHost = nullptr;
		/** The revision of the results last shown. */
		uint64_t															m_ui64Revision = 0;
		/** Was a revision shown? */
		bool																m_bHaveRevision = false;
		/** The first row of each set of results, followed by the number of rows. */
		std::vector<size_t>													m_vSetStart;
		/** The refresh timer. */
		CUtilities::MX_TIMER												m_tRefresh;
		/** The width of the map column. */
		int																	m_iMapWidth = 24;
		/** The data shown on the map. */
		const CHexEditorInterface *											m_pheiMapSource = nullptr;
		/** For each pixel row of the map: 1 if a match is there. */
		std::vector<uint8_t>												m_vMapRows;
		/** The size of the data when the map was made. */
		uint64_t															m_ui64MapSize = 0;
		/** True when the map must be made again. */
		bool																m_bMapDirty = true;
		/** The address of the current match (shown on the map). */
		uint64_t															m_ui64MapCurrent = 0;
		/** Is there a current match? */
		bool																m_bMapCurrent = false;
		/** True while the list is being changed by the window (its changes are not shown in the hex editor). */
		bool																m_bUpdating = false;
		/** Counts the timer ticks (the values are read again every few ticks). */
		uint32_t															m_ui32Ticks = 0;
		/** True while the map is being dragged. */
		bool																m_bMapDragging = false;


		// == Functions.
		/**
		 * Gets the list view.
		 *
		 * \return Returns the list view, or nullptr.
		 **/
		CListView *															ListView();

		/**
		 * Gets the map control.
		 *
		 * \return Returns the map control, or nullptr.
		 **/
		CWidget *															Map();

		/**
		 * Gets the total number of rows.
		 *
		 * \return Returns the number of results.
		 **/
		size_t																TotalRows() const { return m_vSetStart.size() ? m_vSetStart.back() : 0; }

		/**
		 * Finds the set and match of a row.
		 *
		 * \param _sRow The row.
		 * \param _rRow Receives the set and match.
		 * \return Returns true if the row exists.
		 **/
		bool																GetRow( size_t _sRow, MX_ROW &_rRow ) const;

		/**
		 * Gets the text of a cell.
		 *
		 * \param _sRow The row.
		 * \param _iColumn The column (0 = Address, 1 = Value).
		 * \param _wsText Receives the text.
		 * \return Returns true if the row exists.
		 **/
		bool																CellText( size_t _sRow, int _iColumn, std::wstring &_wsText );

		/**
		 * Gets the focused row.
		 *
		 * \return Returns the focused row, or -1.
		 **/
		int																	FocusedRow();

		/**
		 * Selects and focuses a row (showing it in the hex editor).
		 *
		 * \param _sRow The row.
		 **/
		void																GoToRow( size_t _sRow );

		/**
		 * Shows a row's match in the hex editor.
		 *
		 * \param _sRow The row.
		 **/
		void																ShowRow( size_t _sRow );

		/**
		 * Copies rows ("Address<Tab>Value" lines) to the clipboard.
		 *
		 * \param _bAll If true, every row is copied; otherwise the selected rows.
		 **/
		void																CopyRows( bool _bAll );

		/**
		 * Places the map and the list.
		 **/
		void																LayOut();

		/**
		 * Decides which data the map shows (that of the focused row, the current tab, or the first set), and marks the map for
		 *	rebuilding if it changed.
		 **/
		void																UpdateMapSource();

		/**
		 * Makes the map: marks the pixel rows that have matches.
		 *
		 * \param _iHeight The height of the bar in pixels.
		 **/
		void																BuildMap( int _iHeight );

		/**
		 * Gets the rectangle of the bar of the map (the part above the count).
		 *
		 * \param _rClient The client rectangle of the map.
		 * \return Returns the rectangle of the bar.
		 **/
		RECT																MapBarRect( const RECT &_rClient ) const;

		/**
		 * Draws the map.
		 *
		 * \param _hDc The device context.
		 * \param _rClient The client rectangle of the map.
		 **/
		void																PaintMap( HDC _hDc, const RECT &_rClient );

		/**
		 * Goes to the match nearest the address at a point on the map.
		 *
		 * \param _iY The vertical position in the map's client area.
		 **/
		void																MapClick( int _iY );

		/**
		 * Gets the text shown below the map (the number of results).
		 *
		 * \return Returns the text.
		 **/
		std::wstring														CountText() const;

		/**
		 * The window procedure of the list (a click on the focused row shows it again; Enter shows the focused row).
		 *
		 * \param _hWnd The list.
		 * \param _uMsg The message.
		 * \param _wParam The message's WPARAM.
		 * \param _lParam The message's LPARAM.
		 * \param _uIdSubclass The ID of the subclass.
		 * \param _dwRefData The window (CFindResultsWindow *).
		 * \return Returns the result of the message.
		 **/
		static LRESULT CALLBACK												ListSubclassProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam, UINT_PTR _uIdSubclass, DWORD_PTR _dwRefData );

		/**
		 * The window procedure of the map.
		 *
		 * \param _hWnd The map.
		 * \param _uMsg The message.
		 * \param _wParam The message's WPARAM.
		 * \param _lParam The message's LPARAM.
		 * \param _uIdSubclass The ID of the subclass.
		 * \param _dwRefData The window (CFindResultsWindow *).
		 * \return Returns the result of the message.
		 **/
		static LRESULT CALLBACK												MapSubclassProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam, UINT_PTR _uIdSubclass, DWORD_PTR _dwRefData );


	private :
		/** The parent object. */
		typedef lsw::CDockable												CParent;
	};

}	// namespace mx
