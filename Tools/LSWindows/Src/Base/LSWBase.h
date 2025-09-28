#pragma once

#include "../LSWWin.h"
#include "../Brush/LSWBrushCache.h"

#include <map>
#include <string>

// == Macros.
#ifndef LSW_COUNT_OF
#define LSW_COUNT_OF(x)								((sizeof( x ) / sizeof( 0[x] )) / (static_cast<size_t>(!(sizeof( x ) % sizeof(0[x])))))
#define LSW_ELEMENTS( x )							LSW_COUNT_OF( x )
#endif	// #ifndef LSW_COUNT_OF

#ifndef LSW_WIN16_SCROLLBAR_MAX
#define LSW_WIN16_SCROLLBAR_MAX						0x7FFF
#endif	// #ifndef LSW_WIN16_SCROLLBAR_MAX

#ifndef LSW_WIN32_SCROLLBAR_MAX
#define LSW_WIN32_SCROLLBAR_MAX						0x7FFFFFFF
#endif	// #ifndef LSW_WIN32_SCROLLBAR_MAX


namespace lsw {

	class											CLayoutManager;

	class CBase {
	public :
		// == Functions.
		// =======================================
		// Create/Destroy.
		// =======================================
		/**
		 * Initializes process-wide UI helpers and registers framework window classes.
		 * \brief Sets up class atoms, stores the HINSTANCE and layout manager, and registers optional custom classes.
		 *
		 * Registers the dockable, splitter, multi-splitter, tree view, tree-list view, and child-window classes
		 * if names are provided. Safe to call once at startup before any windows are created.
		 *
		 * \param _hInst The module instance for this process.
		 * \param _plmLayoutMan Pointer to the layout manager used by the framework.
		 * \param _pwcDockableClassName Optional registered class name for dockable windows (nullptr to skip).
		 * \param _pwcSplitterClassName Optional registered class name for splitter windows (nullptr to skip).
		 * \param _pwcMultiSplitterClassName Optional registered class name for multi-splitter windows (nullptr to skip).
		 * \param _pwcTreeViewClassName Optional registered class name for tree-view windows (nullptr to skip).
		 * \param _pwcTreeListViewClassName Optional registered class name for tree-list view windows (nullptr to skip).
		 * \param _pwcChildWindowClassName Optional registered class name for generic child windows (nullptr to skip).
		 */
		static VOID									Initialize( HINSTANCE _hInst, CLayoutManager * _plmLayoutMan,
			const WCHAR * _pwcDockableClassName = nullptr,
			const WCHAR * _pwcSplitterClassName = nullptr,
			const WCHAR * _pwcMultiSplitterClassName = nullptr,
			const WCHAR * _pwcTreeViewClassName = nullptr,
			const WCHAR * _pwcTreeListViewClassName = nullptr,
			const WCHAR * _pwcChildWindowClassName = nullptr );

		/**
		 * Shuts down the UI helpers.
		 * \brief Frees allocated resources and unregisters state created by Initialize().
		 *
		 * Safe to call once during process shutdown after all framework windows are destroyed.
		 */
		static VOID									ShutDown();


		// =======================================
		// Windows/controls.
		// =======================================
		/**
		 * Registers a window class.
		 * \brief Thin wrapper around ::RegisterClassExW().
		 *
		 * \param _wceClss The WNDCLASSEXW structure describing the class to register.
		 * \return Returns the ATOM identifying the registered class, or 0 on failure.
		 */
		static ATOM									RegisterClassExW( const WNDCLASSEXW &_wceClss );

		/**
		 * Looks up a class ATOM by its Unicode name.
		 * \brief Retrieves the atom for a class previously registered via RegisterClassExW().
		 *
		 * \param _lpwcClass The Unicode class name to query.
		 * \return Returns the ATOM for the class, or 0 if not found.
		 */
		static ATOM									GetRegisteredClassAtom( LPCWSTR _lpwcClass );

		/**
		 * Looks up a class ATOM by its Unicode name.
		 * \brief std::wstring overload for GetRegisteredClassAtom().
		 *
		 * \param _wsClass The Unicode class name to query.
		 * \return Returns the ATOM for the class, or 0 if not found.
		 */
		static ATOM									GetRegisteredClassAtom( const std::wstring &_wsClass );

		/**
		 * Gets the ATOM for dockable windows.
		 * \brief Accessor for the dockable window class atom.
		 *
		 * \return Returns the ATOM for dockable windows.
		 */
		static ATOM									DockableAtom() { return m_aDockable; }

		/**
		 * Gets the ATOM for splitter windows.
		 * \brief Accessor for the splitter window class atom.
		 *
		 * \return Returns the ATOM for splitter windows.
		 */
		static ATOM									SplitterAtom() { return m_aSplitter; }

		/**
		 * Gets the ATOM for multi-splitter windows.
		 * \brief Accessor for the multi-splitter window class atom.
		 *
		 * \return Returns the ATOM for multi-splitter windows.
		 */
		static ATOM									MultiSplitterAtom() { return m_aMultiSplitter; }

		/**
		 * Gets the ATOM for tree view windows.
		 * \brief Accessor for the tree-view window class atom.
		 *
		 * \return Returns the ATOM for tree-view windows.
		 */
		static ATOM									TreeListAtom() { return m_aTreeView; }

		/**
		 * Gets the ATOM for tree-list view windows.
		 * \brief Accessor for the tree-list view window class atom.
		 *
		 * \return Returns the ATOM for tree-list view windows.
		 */
		static ATOM									TreeListViewAtom() { return m_aTreeListView; }

		/**
		 * Gets the ATOM for child windows.
		 *
		 * \return Returns the ATOM for child windows.
		 */
		static ATOM									ChildWindowAtom() { return m_aChildWindow; }

		/**
		 * Gets the string name for TreeListViews.
		 * 
		 * \return Returns the registerd string for the LSW_LT_TREELISTVIEW class.
		 **/
		static const std::wstring &					TreeListViewString() { return m_wsTreeListViewName; }

		/**
		 * \brief Set 64-bit scrollbar info (wrapper that scales into Win32 SCROLLINFO).
		 *
		 * Description: Maintains a virtual 64-bit scroll range [0.._ui64Max] and position _ui64Pos,
		 *  scaling them to a smaller physical range for Win32 scrollbars. For ÅgnormalÅh ranges
		 *  (<= 0x7FFFFFFF) it passes values straight through. For larger ranges it maps into a
		 *  0..0x7FFF space and computes nPos proportionally.
		 *
		 * \param _hWnd The window owning the scrollbar.
		 * \param _iBar SB_VERT or SB_HORZ.
		 * \param _uiMask Standard SIF_* mask (e.g., SIF_ALL).
		 * \param _ui64Max The virtual 64-bit maximum (inclusive end position).
		 * \param _ui64Pos The virtual 64-bit current position.
		 * \param _iPage The page size in Ågvirtual unitsÅh (commonly rows visible).
		 * \param _bRedraw TRUE to redraw the scrollbar.
		 * \return Returns non-zero on success.
		 */
		static BOOL									SetScrollInfo64( HWND _hWnd, int _iBar, UINT _uiMask,
			uint64_t _ui64Max, uint64_t _ui64Pos, int _iPage, BOOL _bRedraw );

		/**
		 * \brief Get 64-bit scrollbar position (wrapper around GetScrollInfo).
		 *
		 * Description: Converts the physical 32/16-bit thumb position back into the virtual
		 *  64-bit space. Handles both SIF_POS and SIF_TRACKPOS. Special-cases the end of range
		 *  to compensate for integer division truncation.
		 *
		 * \param _hWnd The window owning the scrollbar.
		 * \param _iBar SB_VERT or SB_HORZ.
		 * \param _uiMask Either SIF_POS or SIF_TRACKPOS (optionally OR with SIF_PAGE; we add it).
		 * \param _ui64Max The virtual 64-bit maximum (inclusive end position).
		 * \return Returns the virtual 64-bit position.
		 */
		static uint64_t								GetScrollPos64( HWND _hWnd, int _iBar, UINT _uiMask, uint64_t _ui64Max );


		// =======================================
		// Process functions.
		// =======================================
		/**
		 * Gets a module handle by name (Unicode).
		 * \brief Thin wrapper around ::GetModuleHandleW().
		 *
		 * \param _lpModuleName The Unicode module name, or nullptr for the calling process.
		 * \return Returns the module handle on success, or nullptr on failure.
		 */
		static HMODULE								GetModuleHandleW( LPCWSTR _lpModuleName );

		/**
		 * Gets a module handle by name (ANSI).
		 * \brief Thin wrapper around ::GetModuleHandleA().
		 *
		 * \param _lpModuleName The ANSI module name, or nullptr for the calling process.
		 * \return Returns the module handle on success, or nullptr on failure.
		 */
		static HMODULE								GetModuleHandleA( LPCSTR _lpModuleName );

		/**
		 * Gets the module handle for this executable.
		 * \brief Accessor for the HINSTANCE provided to Initialize().
		 *
		 * \return Returns the HMODULE for this process.
		 */
		static HMODULE								GetThisHandle() { return m_hInstance; }


		// =======================================
		// Layouts.
		// =======================================
		/**
		 * Gets the layout manager.
		 * \brief Accessor for the process-wide CLayoutManager.
		 *
		 * \return Returns the layout manager pointer, or nullptr if not initialized.
		 */
		static CLayoutManager *						LayoutManager() { return m_plmLayoutMan; }


		// =======================================
		// Error-related.
		// =======================================
		/**
		 * Prints a formatted error message.
		 * \brief Logs a message with the provided text and Win32 error code.
		 *
		 * If _dwError is UINT_MAX, the current ::GetLastError() value is used.
		 *
		 * \param _pwcText A brief description of the failing operation.
		 * \param _dwError The Win32 error code to print, or UINT_MAX to query ::GetLastError().
		 */
		static VOID									PrintError( LPCWSTR _pwcText, DWORD _dwError = UINT_MAX );

		/**
		 * Appends formatted error text to a string.
		 * \brief Extends _wsRet with _pwcText and the given Win32 error description.
		 *
		 * If _dwError is UINT_MAX, the current ::GetLastError() value is used.
		 *
		 * \param _pwcText A brief description of the failing operation.
		 * \param _wsRet Destination string to which the formatted error is appended.
		 * \param _dwError The Win32 error code to append, or UINT_MAX to query ::GetLastError().
		 * \return Returns the reference to _wsRet for chaining.
		 */
		static std::wstring							AppendError( LPCWSTR _pwcText, std::wstring &_wsRet, DWORD _dwError = UINT_MAX );

		/**
		 * Displays an error message box (Unicode).
		 * \brief Shows a message box with MB_ICONERROR using a Unicode title and message.
		 *
		 * \param _hWnd Owner window handle (may be nullptr).
		 * \param _pwcMsg The Unicode message to display.
		 * \param _pwcTitle The Unicode title (defaults to L"Error").
		 */
		static VOID									MessageBoxError( HWND _hWnd, LPCWSTR _pwcMsg, LPCWSTR _pwcTitle = L"Error" );

		/**
		 * Displays an error message box (ANSI).
		 * \brief Shows a message box with MB_ICONERROR using an ANSI title and message.
		 *
		 * \param _hWnd Owner window handle (may be nullptr).
		 * \param _pcMsg The ANSI message to display.
		 * \param _pcTitle The ANSI title (defaults to "Error").
		 */
		static VOID									MessageBoxError( HWND _hWnd, LPCSTR _pcMsg, LPCSTR _pcTitle = "Error" );


		// =======================================
		// Prompting.
		// =======================================
		/**
		 * Shows an informational OK prompt (ANSI).
		 * \brief Displays MB_ICONINFORMATION with OK, or OK/Cancel if requested.
		 *
		 * \param _hWnd Owner window handle (may be nullptr).
		 * \param _pcMsg The ANSI message to display.
		 * \param _pcTitle The ANSI title to display.
		 * \param _bIncludeCancel True to show OK/Cancel, false for OK only.
		 * \return Returns true if the user accepted (IDOK), otherwise false.
		 */
		static bool									PromptOk( HWND _hWnd, LPCSTR _pcMsg, LPCSTR _pcTitle, bool _bIncludeCancel = true );

		/**
		 * Shows an informational OK prompt (Unicode).
		 * \brief Displays MB_ICONINFORMATION with OK, or OK/Cancel if requested.
		 *
		 * \param _hWnd Owner window handle (may be nullptr).
		 * \param _pwcMsg The Unicode message to display.
		 * \param _pwcTitle The Unicode title to display.
		 * \param _bIncludeCancel True to show OK/Cancel, false for OK only.
		 * \return Returns true if the user accepted (IDOK), otherwise false.
		 */
		static bool									PromptOk( HWND _hWnd, LPCWSTR _pwcMsg, LPCWSTR _pwcTitle, bool _bIncludeCancel = true );

		/**
		 * Shows a Yes/No question prompt (ANSI).
		 * \brief Displays MB_ICONQUESTION with Yes and No buttons.
		 *
		 * \param _hWnd Owner window handle (may be nullptr).
		 * \param _pcMsg The ANSI question text.
		 * \param _pcTitle The ANSI title to display.
		 * \return Returns true if the user chose Yes (IDYES), otherwise false.
		 */
		static bool									PromptYesNo( HWND _hWnd, LPCSTR _pcMsg, LPCSTR _pcTitle );

		/**
		 * Shows a Yes/No question prompt (Unicode).
		 * \brief Displays MB_ICONQUESTION with Yes and No buttons.
		 *
		 * \param _hWnd Owner window handle (may be nullptr).
		 * \param _pwcMsg The Unicode question text.
		 * \param _pwcTitle The Unicode title to display.
		 * \return Returns true if the user chose Yes (IDYES), otherwise false.
		 */
		static bool									PromptYesNo( HWND _hWnd, LPCWSTR _pwcMsg, LPCWSTR _pwcTitle );


		// =======================================
		// Fonts/Drawing.
		// =======================================
		/**
		 * Gets the default message-box font.
		 * \brief Accessor for the font used by message dialogs.
		 *
		 * \return Returns the HFONT for message boxes.
		 */
		static HFONT								MessageBoxFont() { return m_hMessageFont; }

		/**
		 * Gets the default status-bar font.
		 * \brief Accessor for the font used by status bars.
		 *
		 * \return Returns the HFONT for status bars.
		 */
		static HFONT								StatusBarFont() { return m_hStatusFont; }

		/**
		 * Gets the non-client metrics.
		 * \brief Returns the cached NONCLIENTMETRICSW filled at initialization.
		 *
		 * \return Returns the NONCLIENTMETRICSW structure with current metrics.
		 */
		static NONCLIENTMETRICSW					NonClientMetrics() { return m_ncmNonClientMetrics; }

		/**
		 * Gets the brush cache.
		 * 
		 * \return Returns a reference to the brush cache
		 **/
		static CBrushCache &						BrushCache() { return m_bcBrushes; }


		// =======================================
		// Debug.
		// =======================================
		/**
		 * Converts a message ID to its textual name.
		 * \brief Writes a human-readable symbolic name for a WM_* message into _sName.
		 *
		 * \param _wMessage The message ID (e.g., WM_PAINT).
		 * \param _sName Output string receiving the message name.
		 */
		static VOID									MessageToText( WORD _wMessage, std::string &_sName );


	protected :
		// == Types.
		struct WNDCLASSEXW_ATOM : public WNDCLASSEXW {
			ATOM									aAtom;
		};

		// == Members.
		// Registered classes.
		static std::map<std::wstring, WNDCLASSEXW_ATOM>
													m_mClasses;

		// This HINSTANCE.
		static HINSTANCE							m_hInstance;

		// The layout manager.
		static CLayoutManager *						m_plmLayoutMan;

		// Non-client metrics.
		static NONCLIENTMETRICSW					m_ncmNonClientMetrics;

		// Message-box font.
		static HFONT								m_hMessageFont;

		// Status font.
		static HFONT								m_hStatusFont;

		static CBrushCache							m_bcBrushes;							/**< Cache of brushes not created via CBrush.  These are intended to be created once and then remain alive throughout the life of the program, being destroyed only at shut-down. */

		// The dockable class.
		static ATOM									m_aDockable;

		// The splitter class.
		static ATOM									m_aSplitter;

		// The multi-splitter class.
		static ATOM									m_aMultiSplitter;

		// The tree view class.
		static ATOM									m_aTreeView;

		// The tree-list view class.
		static ATOM									m_aTreeListView;

		/** The child-window class. */
		static ATOM									m_aChildWindow;
	
		/** the TreeListView name. */
		static std::wstring							m_wsTreeListViewName;
	};

}	// namespace lsw
