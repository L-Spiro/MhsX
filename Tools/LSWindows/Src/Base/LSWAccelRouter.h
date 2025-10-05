#pragma once

#include "../LSWWin.h"
#include <unordered_map>

namespace lsw {

	/**
	 * \class CAccelRouter
	 * \brief Associates accelerator tables with specific HWNDs and routes messages to the nearest registered ancestor.
	 *
	 * There is always a blank line after the \brief.
	 *
	 * Register a table per HWND (top-level frame, child editor view, tool windows, etc.).
	 * On each message, ask the router for the correct HACCEL based on msg.hwnd. It ascends
	 * the ancestor chain until it finds a registered window.
	 */
	class CAccelRouter {
	public :
		CAccelRouter() {}
		~CAccelRouter() {
			for ( auto & kv : m_mOwned ) {
				::DestroyAcceleratorTable( kv.second );
			}
		}


		// == Functions.
		/**
		 * Registers an existing accelerator handle with a target window.
		 *
		 * \param _hWnd The HWND that owns/uses this accelerator table.
		 * \param _hAccel The accelerator handle to associate.
		 */
		void												RegisterHandle( HWND _hWnd, HACCEL _hAccel ) {
			m_mHwndToAccel[_hWnd] = _hAccel;
		}

		/**
		 * Builds a new accelerator table from an ACCEL array and registers it to a window.
		 * The router will destroy this table automatically when destroyed or when Unregister() replaces it.
		 *
		 * \param _hWnd The HWND to associate.
		 * \param _paAccels Pointer to ACCEL array.
		 * \param _iCount Number of entries in _paAccels.
		 * \return Returns the created HACCEL or nullptr on failure.
		 */
		HACCEL												CreateAndRegister( HWND _hWnd, const ACCEL * _paAccels, int _iCount ) {
			Unregister( _hWnd );
			HACCEL hNew = ::CreateAcceleratorTableW( const_cast<ACCEL *>(_paAccels), _iCount );
			if ( hNew ) {
				m_mHwndToAccel[_hWnd] = hNew;
				m_mOwned[_hWnd] = hNew;
			}
			return hNew;
		}

		/**
		 * Unregisters any accelerator associated with a window. Destroys it if owned by the router.
		 *
		 * \param _hWnd The HWND to disassociate.
		 */
		void												Unregister( HWND _hWnd ) {
			if ( auto itOwned = m_mOwned.find( _hWnd ); itOwned != m_mOwned.end() ) {
				::DestroyAcceleratorTable( itOwned->second );
				m_mOwned.erase( itOwned );
			}
			m_mHwndToAccel.erase( _hWnd );
		}

		/**
		 * Resets the object fully.
		 **/
		void												Reset() {
			for ( auto & kv : m_mOwned ) {
				::DestroyAcceleratorTable( kv.second );
			}
			m_mOwned.clear();
			m_mHwndToAccel.clear();
		}

		/**
		 * Finds the accelerator table to use for a given window by searching up the ancestor chain.
		 *
		 * \param _hFrom The starting HWND (usually msg.hwnd; may be nullptr).
		 * \param _phTarget Receives the HWND that owns the accelerator (can be nullptr).
		 * \return Returns the matching HACCEL or nullptr if none found.
		 */
		HACCEL												ResolveFor( HWND _hFrom, HWND * _phTarget ) const {
			HWND hCur = _hFrom ? _hFrom : ::GetActiveWindow();
			while ( hCur ) {
				if ( auto it = m_mHwndToAccel.find( hCur ); it != m_mHwndToAccel.end() ) {
					if ( _phTarget ) { (*_phTarget) = hCur; }
					return it->second;
				}
				// Prefer owner chain for popups, then parent chain for children.
				HWND hOwner = ::GetWindow( hCur, GW_OWNER );
				hCur = hOwner ? hOwner : ::GetParent( hCur );
			}
			if ( _phTarget ) { (*_phTarget) = NULL; }
			return NULL;
		}

		/**
		 * Translates a message using the accelerator associated with its window chain.
		 * Call this once per message before TranslateMessage/DispatchMessage.
		 *
		 * \param _mMsg The message to translate.
		 * \return Returns TRUE if an accelerator was translated (message consumed).
		 */
		BOOL												Translate( MSG & _mMsg ) const {
			HWND hTarget = nullptr;
			if ( HACCEL hAccel = ResolveFor( _mMsg.hwnd, &hTarget ) ) {
				if ( ::TranslateAcceleratorW( hTarget, hAccel, &_mMsg ) ) {
					return TRUE;
				}
			}
			return FALSE;
		}

	private :
		std::unordered_map<HWND, HACCEL> m_mHwndToAccel;	/**< All associations we manage (owned or external). */
		std::unordered_map<HWND, HACCEL> m_mOwned;			/**< Subset that we created and must destroy. */
	};

}	// namespace lsw
