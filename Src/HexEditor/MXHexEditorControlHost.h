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

	protected :
		// == Members.
		/** Overwrite Mode. */
		bool										m_bOverwrite = true;
	};

}	// namespace mx
