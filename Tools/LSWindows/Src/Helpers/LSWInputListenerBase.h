#include "../LSWWin.h"
#include "../Widget/LSWWidget.h"

#pragma once


namespace lsw {

	class CInputListenerBase {
	public :
		CInputListenerBase();


		// == Enumerations.
		/** The listening state. */
		enum LSW_LISTENING_STATE : uint32_t {
			LSW_LS_WAITING,
			LSW_LS_LISTENING,
			LSW_LS_SUCCESS,
			LSW_LS_FAILURE,
		};


		// == Functions.
		/**
		 * Starts listening for keyboard input.
		 *
		 * \param _pwControl The control that will be listening for keyboard input.
		 * \return Returns true if beginning the keyboard isten was successful.
		 */
		virtual bool								BeginListening_Keyboard( CWidget * _pwControl );

		/**
		 * Gets the text for the "Listening" state.
		 *
		 * \return Returns the text to be used during the listening state.
		 */
		virtual const wchar_t *						ListeningText() const { return L""; }

		/**
		 * Gets the default text for the listenable controls.  This text is used after listening fails.
		 *
		 * \return Returns the text string to be used after failed listening.
		 */
		virtual const wchar_t *						DefaultText() const { return L""; }

		/**
		 * Determines if the left and right Shift, Control, and Alt keys should be non-distinguishable.
		 *
		 * \return Return true to make no distinction between the left and right Shift, Control, and Alt keys.
		 */
		virtual bool								IgnoringLeftRight() { return false; }

		/**
		 * Detrmines if key cominations are allowed, such as Shift+Key, Alt+Key, or Control+Key.  If false, Shift, Control, and Alt
		 *	can be used as keys by themselves.
		 *
		 * \return Return false if Sift, Control, and Alt should be possible keys by themselves, or true if Shift, Control, and Alt should be
		 *	used as modifiers for other keys.
		 */
		virtual bool								AllowKeyCombos() { return true; }

		/**
		 * Called after focus on the control is lost or a key has been registered.
		 *
		 * \param _pwControl The control used for listening.
		 * \param _bSuccess Set to true if the call is due to a key being registered.
		 * \return Returns true if the listening state is LSW_LS_LISTENING and the old control procedure was restored.
		 */
		virtual bool								StopListening_Keyboard( CWidget * _pwControl, bool _bSuccess );


	protected :
		//== Members.
		/** Button original procedure. */
		WNDPROC										m_wpOrigProc;
		/** The listening state. */
		LSW_LISTENING_STATE							m_lsListenState;
		/** The result after listening success. */
		LSW_KEY										m_kResult;
		/** Window property. */
		static WCHAR								m_szProp[2];


		// == Functions.
		/**
		 * Window procedure.
		 *
		 * \param _hWnd The window parameter.
		 * \param _uMsg The message parameter.
		 * \param _wParam The WPARAM.
		 * \param _lParam The LPARAM.
		 * \return Returns the result of the message-handler.
		 */
		static LRESULT CALLBACK						ControlOverride( HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam );

	};

}	// namespace lsw
