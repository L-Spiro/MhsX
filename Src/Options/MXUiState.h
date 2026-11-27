#pragma once

#include "../MXMhsX.h"
#include "../HexEditor/MXInspector.h"
#include "../Strings/MXSecureWString.h"

#include <Docking/LSWDockable.h>
#include <LSONJson.h>
#include <MultiSplitter/LSWMultiSplitter.h>

#include <string>
#include <utility>
#include <vector>

namespace mx {

	/**
	 * Where the windows of MHS X are and how they are arranged (window positions, what is docked where, splitter sizes, which
	 *	dockables are open), plus settings of the windows that are saved with them (recent files, Inspector settings).  Saved in the
	 *	settings JSON file under "Ui".
	 **/
	class CUiState {
	public :
		// == Types.
		/** A dockable of a window. */
		struct MX_DOCKABLE {
			WORD															wId = 0;				/**< The control ID of the dockable. */
			bool															bOpen = true;			/**< Is it open? */
			bool															bFloating = false;		/**< Is it floating? */
			lsw::LSW_RECT													rWindow = lsw::LSW_RECT( 0, 0, 0, 0 );	/**< Its window rectangle when floating (screen coordinates). */
		};

		/** A window. */
		struct MX_WINDOW {
			bool															bValid = false;			/**< Was the window saved? */
			lsw::LSW_RECT													rNormal = lsw::LSW_RECT( 0, 0, 0, 0 );	/**< The window rectangle when not maximized or minimized. */
			bool															bMaximized = false;		/**< Is the window maximized? */
			bool															bHasLayout = false;		/**< Was the arrangement of the splitter saved? */
			lsw::CMultiSplitter::LSW_MS_LAYOUT_NODE							lnLayout;				/**< The arrangement of the splitter. */
			std::vector<MX_DOCKABLE>										vDockables;				/**< The dockables. */


			// == Functions.
			/**
			 * Finds a saved dockable.
			 *
			 * \param _wId The control ID of the dockable.
			 * \return Returns the saved dockable, or nullptr.
			 **/
			const MX_DOCKABLE *												Dockable( WORD _wId ) const {
				for ( const auto & dThis : vDockables ) {
					if ( dThis.wId == _wId ) { return &dThis; }
				}
				return nullptr;
			}

			/**
			 * Determines whether a dockable should be open.
			 *
			 * \param _wId The control ID of the dockable.
			 * \param _bDefault Whether it is open when nothing was saved.
			 * \return Returns true if the dockable should be open.
			 **/
			bool															IsOpen( WORD _wId, bool _bDefault ) const {
				const MX_DOCKABLE * pdThis = Dockable( _wId );
				return pdThis ? pdThis->bOpen : _bDefault;
			}
		};

		/** A dockable that a window can have: its control ID and the dockable (nullptr if it is closed). */
		typedef std::pair<WORD, lsw::CDockable *>							MX_KNOWN_DOCKABLE;


		// == Members.
		/** The MHS X main window. */
		MX_WINDOW															wMain;
		/** The Hex Editor. */
		MX_WINDOW															wHexEditor;
		/** The files most recently opened in the Hex Editor, most recent first. */
		std::vector<CSecureWString>											vRecentFiles;
		/** Were Inspector settings loaded? */
		bool																bHaveInspector = false;
		/** The Inspector settings. */
		CInspector::MX_SETTINGS												sInspector;
		/** The most recent files kept. */
		static constexpr size_t												MX_MAX_RECENT_FILES = 16;


		// == Functions.
		/**
		 * Adds a file to the top of the recent files (moving it if it is already there).
		 *
		 * \param _wsPath The full path of the file.
		 **/
		void																AddRecentFile( const std::wstring &_wsPath );

		/**
		 * Removes a file from the recent files.
		 *
		 * \param _wsPath The full path of the file.
		 **/
		void																RemoveRecentFile( const std::wstring &_wsPath );

		/**
		 * Saves the state as a "Ui" member of a JSON object.
		 *
		 * \param _peRoot The JSON object.
		 * \return Returns true if the state was added.
		 **/
		bool																Save( lson::CJson::LSON_ELEMENT * _peRoot ) const;

		/**
		 * Loads the state from the "Ui" member of the root object of a JSON file.  Values that are missing keep their current values.
		 *
		 * \param _pjJson The JSON file.
		 * \return Returns true if the state was found.
		 **/
		bool																Load( lson::CJson * _pjJson );

		/**
		 * Captures the state of a window: its position, the arrangement of its splitter, and which of its dockables are open, docked,
		 *	or floating (and where).
		 *
		 * \param _pwWindow The window.
		 * \param _pmsSplitter Its splitter, or nullptr.
		 * \param _vDockables The dockables the window can have.
		 * \param _wWindow Receives the state.
		 **/
		static void															CaptureWindow( lsw::CWidget * _pwWindow, const lsw::CMultiSplitter * _pmsSplitter,
			const std::vector<MX_KNOWN_DOCKABLE> &_vDockables, MX_WINDOW &_wWindow );

		/**
		 * Moves a window to its saved position (if the position is still on a monitor).  A window that was maximized is maximized
		 *	once it is shown.
		 *
		 * \param _pwWindow The window.
		 * \param _wWindow The saved state.
		 **/
		static void															RestorePlacement( lsw::CWidget * _pwWindow, const MX_WINDOW &_wWindow );

		/**
		 * Rearranges a window's splitter and dockables as saved.  The dockables that should be open must already exist.
		 *
		 * \param _pmsSplitter The splitter.
		 * \param _wWindow The saved state.
		 * \param _vFixed Widgets docked in the splitter that are not dockables (such as the main view).
		 * \param _vDockables The dockables the window can have.
		 **/
		static void															RestoreLayout( lsw::CMultiSplitter * _pmsSplitter, const MX_WINDOW &_wWindow,
			const std::vector<lsw::CWidget *> &_vFixed, const std::vector<MX_KNOWN_DOCKABLE> &_vDockables );


	protected :
		// == Functions.
		/**
		 * Saves a window.
		 *
		 * \param _wWindow The window.
		 * \param _sName The name of the JSON member.
		 * \param _peParent The JSON object to which to add it.
		 **/
		static void															SaveWindow( const MX_WINDOW &_wWindow, const std::string &_sName, lson::CJson::LSON_ELEMENT * _peParent );

		/**
		 * Saves a node of the arrangement of a splitter.
		 *
		 * \param _lnNode The node.
		 * \param _sName The name of the JSON member.
		 * \param _peParent The JSON object or array to which to add it.
		 **/
		static void															SaveNode( const lsw::CMultiSplitter::LSW_MS_LAYOUT_NODE &_lnNode, const std::string &_sName,
			lson::CJson::LSON_ELEMENT * _peParent );

		/**
		 * Saves the Inspector settings.
		 *
		 * \param _sSettings The settings.
		 * \param _peParent The JSON object to which to add them.
		 **/
		static void															SaveInspector( const CInspector::MX_SETTINGS &_sSettings, lson::CJson::LSON_ELEMENT * _peParent );

		/**
		 * Loads a window.
		 *
		 * \param _pjvWindow The JSON object.
		 * \param _pjcContainer The JSON container.
		 * \param _wWindow Receives the window.
		 * \return Returns true if the window was loaded.
		 **/
		static bool															LoadWindow( const lson::CJsonContainer::LSON_JSON_VALUE * _pjvWindow, lson::CJsonContainer * _pjcContainer,
			MX_WINDOW &_wWindow );

		/**
		 * Loads a node of the arrangement of a splitter.
		 *
		 * \param _pjvNode The JSON object.
		 * \param _pjcContainer The JSON container.
		 * \param _lnNode Receives the node.
		 * \param _sDepth How deep the node is (deeply nested data is refused).
		 * \return Returns true if the node was loaded.
		 **/
		static bool															LoadNode( const lson::CJsonContainer::LSON_JSON_VALUE * _pjvNode, lson::CJsonContainer * _pjcContainer,
			lsw::CMultiSplitter::LSW_MS_LAYOUT_NODE &_lnNode, size_t _sDepth );

		/**
		 * Loads the Inspector settings.
		 *
		 * \param _pjvInspector The JSON object.
		 * \param _pjcContainer The JSON container.
		 * \param _sSettings Receives the settings.
		 * \return Returns true if the settings were loaded.
		 **/
		static bool															LoadInspector( const lson::CJsonContainer::LSON_JSON_VALUE * _pjvInspector, lson::CJsonContainer * _pjcContainer,
			CInspector::MX_SETTINGS &_sSettings );

		/**
		 * Reads a number member of a JSON object.
		 *
		 * \param _pjvObject The JSON object.
		 * \param _pjcContainer The JSON container.
		 * \param _sName The name of the member.
		 * \param _dValue Receives the value.
		 * \return Returns true if the member is a number.
		 **/
		static bool															GetNumber( const lson::CJsonContainer::LSON_JSON_VALUE * _pjvObject, lson::CJsonContainer * _pjcContainer,
			const std::string &_sName, double &_dValue );

		/**
		 * Reads a true/false member of a JSON object.
		 *
		 * \param _pjvObject The JSON object.
		 * \param _pjcContainer The JSON container.
		 * \param _sName The name of the member.
		 * \param _bValue Receives the value.
		 * \return Returns true if the member is true or false.
		 **/
		static bool															GetBool( const lson::CJsonContainer::LSON_JSON_VALUE * _pjvObject, lson::CJsonContainer * _pjcContainer,
			const std::string &_sName, bool &_bValue );

		/**
		 * Reads a string value.
		 *
		 * \param _jvValue The JSON value.
		 * \param _pjcContainer The JSON container.
		 * \param _wsValue Receives the value.
		 * \return Returns true if the value is a string.
		 **/
		static bool															GetString( const lson::CJsonContainer::LSON_JSON_VALUE &_jvValue, lson::CJsonContainer * _pjcContainer,
			std::wstring &_wsValue );

		/**
		 * Adds a rectangle to a JSON object (as Left, Top, Right, and Bottom).
		 *
		 * \param _rRect The rectangle.
		 * \param _peParent The JSON object.
		 **/
		static void															SaveRect( const lsw::LSW_RECT &_rRect, lson::CJson::LSON_ELEMENT * _peParent );

		/**
		 * Reads a rectangle from a JSON object (Left, Top, Right, and Bottom).
		 *
		 * \param _pjvObject The JSON object.
		 * \param _pjcContainer The JSON container.
		 * \param _rRect Receives the rectangle.
		 * \return Returns true if the rectangle was read.
		 **/
		static bool															LoadRect( const lson::CJsonContainer::LSON_JSON_VALUE * _pjvObject, lson::CJsonContainer * _pjcContainer,
			lsw::LSW_RECT &_rRect );

		/**
		 * Determines whether enough of a window rectangle is on a monitor for the window to be put there.
		 *
		 * \param _rRect The window rectangle (screen coordinates).
		 * \return Returns true if the rectangle has a reasonable size and its title area is on a monitor.
		 **/
		static bool															IsOnScreen( const lsw::LSW_RECT &_rRect );
	};

}	// namespace mx
