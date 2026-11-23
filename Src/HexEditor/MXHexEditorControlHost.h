#pragma once

#include "../MXMhsX.h"

#include <StatusBar/LSWStatusBar.h>


namespace mx {

	class CHexEditorControlHost {
	public :
		CHexEditorControlHost() {
		}
		~CHexEditorControlHost() {
		}

		// == Functions.
		// Determines if Overwrite mode is enabled.
		bool										IsOverwriteMode() const { return m_bOverwrite; }

		// Toggles between Overwrite and Insert.
		virtual void								ToggleOverwriteInsert() { m_bOverwrite = !m_bOverwrite; }

		// Delete the selection.
		virtual void								DeleteSelectedOrCaret() {}

		// Deletes the character prior to the caret (backspace).
		virtual void								DeletePriorToCaret() {}

		// Determines if the control is focused or not.
		virtual bool								IsFocused() { return false; }

		// Gets the status bar.
		virtual lsw::CStatusBar *					StatusBar() { return nullptr; }

		// Gets the status bar.
		virtual const lsw::CStatusBar *				StatusBar() const { return nullptr; }

		// Sets a status-bar item’s text and "warning" status.
		virtual void								SetStatusBarText( const wchar_t * _pwcText, bool _bWarning = false, size_t _sIdx = 0, bool _bUpdateStore = true ) {}

		// Update the status-bar Character-Set part.
		virtual void								UpdateStatusBar_CharSet() {}

		// Updates the status-bar selection ranges/current position/value.
		virtual void								UpdateStatusBar_PosValue_StartSize() {}

		/**
		 * Updates the status-bar size part.
		 **/
		virtual void								UpdateStatusBar_Size() {}

		/**
		 * Determines whether typing into a file changes it as each character is typed (text, hex and binary).  Otherwise values are
		 *	collected in the edit box and written when Enter is pressed.
		 * 
		 * \return Returns true if files are edited in real time.
		 **/
		inline bool									RealTimeFileEditing() const { return m_bRealTimeFileEditing; }

		/**
		 * Sets whether typing into a file changes it as each character is typed.
		 * 
		 * \param _bEnable If true, files are edited in real time.
		 **/
		inline void									SetRealTimeFileEditing( bool _bEnable ) { m_bRealTimeFileEditing = _bEnable; }

		/**
		 * Determines whether typing text into process memory writes each character as it is typed.  Otherwise the text is collected in
		 *	the edit box and written when Enter is pressed.
		 * 
		 * \return Returns true if text is written to process memory in real time.
		 **/
		inline bool									RealTimeRamTextEditing() const { return m_bRealTimeRamTextEditing; }

		/**
		 * Sets whether typing text into process memory writes each character as it is typed.
		 * 
		 * \param _bEnable If true, text is written to process memory in real time.
		 **/
		inline void									SetRealTimeRamTextEditing( bool _bEnable ) { m_bRealTimeRamTextEditing = _bEnable; }

	protected :
		// == Members.
		/** Overwrite Mode. */
		bool										m_bOverwrite = true;
		/** Files are edited in real time. */
		bool										m_bRealTimeFileEditing = true;
		/** Text is written to process memory in real time. */
		bool										m_bRealTimeRamTextEditing = false;
	};

}	// namespace mx
