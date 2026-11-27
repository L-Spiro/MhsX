#pragma once

#include "../MXMhsX.h"
#include "../Utilities/MXUtilities.h"
#include "MXInspector.h"

#include <Docking/LSWDockable.h>
#include <ListView/LSWListView.h>

using namespace lsw;

namespace mx {

	/**
	 * The Inspector: a dockable table of the values of the bytes at the caret (or the start of the selection) of the Hex Editor,
	 *	shown as many data types.  Values can be edited in place, copied, and exported.
	 **/
	class CInspectorWindow : public lsw::CDockable {
	public :
		CInspectorWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CInspectorWindow();


		// == Functions.
		/**
		 * Handles WM_INITDIALOG.  Sets up the table and starts refreshing it.
		 *
		 * \return Returns a LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED													InitDialog();

		/**
		 * Handles WM_CONTEXTMENU.  Shows the Inspector menu.
		 *
		 * \param _pwControl The control that was right-clicked.
		 * \param _iX The horizontal position of the cursor in screen coordinates, or -1 if the menu was opened with the keyboard.
		 * \param _iY The vertical position of the cursor in screen coordinates, or -1 if the menu was opened with the keyboard.
		 * \return Returns a LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED													ContextMenu( CWidget * _pwControl, INT _iX, INT _iY );

		/**
		 * Handles WM_TIMER.  Refreshes the values.
		 *
		 * \param _uiptrId The timer identifier.
		 * \param _tpProc The timer callback, if any.
		 * \return Returns a LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED													Timer( UINT_PTR _uiptrId, TIMERPROC _tpProc );

		/**
		 * Handles the private messages sent by the in-place edit box (commit and cancel).
		 *
		 * \param _uMsg The message.
		 * \param _wParam The message's WPARAM.
		 * \param _lParam The message's LPARAM.
		 * \return Returns a LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED													CustomPrivateMsg( UINT _uMsg, WPARAM _wParam, LPARAM _lParam );

		/**
		 * The WM_SIZE handler.
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
		virtual INT															PreferredDockWidth() { return 300; }

		/**
		 * Returns the desired height when first docking into a horizontal split.
		 *
		 * \return Returns the desired height in pixels.
		 **/
		virtual INT															PreferredDockHeight() { return 420; }

		/**
		 * Sets the object that supplies the data and carries out edits.  The rows are rebuilt from its settings.
		 *
		 * \param _pihHost The host.
		 **/
		void																SetHost( CInspectorHost * _pihHost );

		/**
		 * Rebuilds the rows from the settings (after they change).
		 **/
		void																Rebuild();

		/**
		 * Refreshes the values from the data at the inspected address.  Does nothing while the Inspector is hidden.
		 **/
		void																Refresh();

		/**
		 * Determines whether the keyboard focus is in the Inspector (so that Ctrl+C copies from it rather than from the hex editor).
		 *
		 * \return Returns true if the Inspector or one of its children has the focus.
		 **/
		bool																HasFocus() const;

		/**
		 * Copies the selected row ("Type<Tab>Value") to the clipboard.
		 **/
		void																CopyRow();


	protected :
		// == Enumerations.
		/** Menu commands. */
		enum MX_INSPECTOR_COMMANDS : UINT {
			MX_IC_COPY_ROW													= 100,
			MX_IC_COPY_TYPE,
			MX_IC_COPY_VALUE,
			MX_IC_COPY_COLUMN,
			MX_IC_COPY_ALL,
			MX_IC_EXPORT_CSV,
			MX_IC_FORMAT_HEX,
			MX_IC_FORMAT_DECIMAL,
			MX_IC_FORMAT_OCTAL,
			MX_IC_FORMAT_BINARY,
			MX_IC_CUSTOMIZE,
			MX_IC_ALLOW_DOCKING,
			MX_IC_GO_TO_ADDRESS,
			MX_IC_EDIT,
		};

		/** Private messages. */
		enum MX_INSPECTOR_MESSAGES : UINT {
			MX_IM_EDIT_DONE													= LSW_USER + 0x200,		/**< WPARAM: TRUE to write the value, FALSE to cancel. */
		};


		// == Types.
		/** A row of the table. */
		struct MX_ROW {
			CInspector::MX_INSPECTOR_TYPE									itType = CInspector::MX_IT_BINARY;	/**< The data type. */
			std::wstring													wsValue;				/**< The text shown in the Value column. */
			size_t															sBytes = 0;				/**< The number of bytes the value uses. */
			bool															bValid = false;			/**< Is there a value? */
		};


		// == Members.
		/** Supplies the data and carries out edits. */
		CInspectorHost *													m_pihHost = nullptr;
		/** The rows. */
		std::vector<MX_ROW>													m_vRows;
		/** The data last inspected. */
		CInspectorHost::MX_INSPECTOR_SOURCE									m_isSource;
		/** Is m_isSource valid? */
		bool																m_bSourceValid = false;
		/** The refresh timer. */
		CUtilities::MX_TIMER												m_tRefresh;
		/** The in-place edit box, or NULL. */
		HWND																m_hEdit = NULL;
		/** Identifies the current edit box in the messages it posts. */
		uint32_t															m_ui32EditId = 0;
		/** The row being edited. */
		int																	m_iEditRow = -1;
		/** The text of the value when editing began (nothing is written if it is not changed). */
		std::wstring														m_wsEditOriginal;
		/** The address of the value being edited. */
		uint64_t															m_ui64EditAddress = 0;
		/** The number of bytes the value being edited used when editing began. */
		size_t																m_sEditBytes = 0;
		/** The column that was right-clicked (0 = Type, 1 = Value). */
		int																	m_iContextColumn = 1;
		/** The row that was selected before the last click (a click on the value of the selected row edits it). */
		int																	m_iSelBeforeClick = -1;


		// == Functions.
		/**
		 * Gets the list view.
		 *
		 * \return Returns the list view, or nullptr.
		 **/
		CListView *															ListView();

		/**
		 * Gets the selected row.
		 *
		 * \return Returns the index of the selected row, or -1.
		 **/
		int																	SelectedRow();

		/**
		 * Gets the text of a cell as shown.
		 *
		 * \param _iRow The row.
		 * \param _iColumn The column (0 = Type, 1 = Value).
		 * \return Returns the text of the cell.
		 **/
		std::wstring														CellText( int _iRow, int _iColumn );

		/**
		 * Begins editing the value of a row in place.
		 *
		 * \param _iRow The row.
		 **/
		void																BeginEdit( int _iRow );

		/**
		 * Ends editing in place.
		 *
		 * \param _bCommit If true, the value typed is written.
		 **/
		void																EndEdit( bool _bCommit );

		/**
		 * Puts text on the clipboard.
		 *
		 * \param _wsText The text.
		 * \return Returns true if the text was put on the clipboard.
		 **/
		bool																SetClipboardText( const std::wstring &_wsText );

		/**
		 * Exports the table to a CSV file (UTF-8, comma-separated).
		 **/
		void																ExportCsv();

		/**
		 * Shows the Customize dialog and applies the settings chosen.
		 **/
		void																Customize();

		/**
		 * Shows a message in the status bar of the host.
		 *
		 * \param _swsText The message.
		 * \param _bWarning If true, the message is a warning.
		 **/
		void																Status( const CSecureWString &_swsText, bool _bWarning );

		/**
		 * The subclass procedure of the list view (clicks and keys that edit and copy).
		 *
		 * \param _hWnd The list view.
		 * \param _uMsg The message.
		 * \param _wParam The message's WPARAM.
		 * \param _lParam The message's LPARAM.
		 * \param _uIdSubclass The subclass ID.
		 * \param _dwRefData The CInspectorWindow.
		 * \return Returns the result of the message.
		 **/
		static LRESULT CALLBACK												ListSubclassProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam, UINT_PTR _uIdSubclass, DWORD_PTR _dwRefData );

		/**
		 * The subclass procedure of the in-place edit box (Enter writes, Escape cancels, losing the focus cancels).
		 *
		 * \param _hWnd The edit box.
		 * \param _uMsg The message.
		 * \param _wParam The message's WPARAM.
		 * \param _lParam The message's LPARAM.
		 * \param _uIdSubclass The subclass ID.
		 * \param _dwRefData The CInspectorWindow.
		 * \return Returns the result of the message.
		 **/
		static LRESULT CALLBACK												EditSubclassProc( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam, UINT_PTR _uIdSubclass, DWORD_PTR _dwRefData );


	private :
		/** The parent class. */
		typedef lsw::CDockable												CParent;
	};

}	// namespace mx
