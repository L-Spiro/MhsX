#pragma once

#include "../LSWWin.h"

#include <Ole2.h>

namespace lsw {

	/**
	 * \brief RAII wrapper around ::OleInitialize()/::OleUninitialize().
	 *
	 * Use this on any thread that will call OLE APIs such as ::OleSetClipboard().
	 */
	class COleInitialize {
	public :
		/**
		 * \brief Initializes OLE on the calling thread.
		 */
		COleInitialize() :
			m_hrResult( ::OleInitialize( NULL ) ),
			m_bInitialized( SUCCEEDED( m_hrResult ) ) {
		}

		/**
		 * \brief Uninitializes OLE if initialization succeeded.
		 */
		~COleInitialize() {
			if ( m_bInitialized ) {
				::OleUninitialize();
			}
		}

		COleInitialize( const COleInitialize & ) = delete;
		COleInitialize &								operator = ( const COleInitialize & ) = delete;

		/**
		 * \brief Move-constructs, transferring ownership of the initialization token.
		 *
		 * \param _oiOther The source object.
		 */
		COleInitialize( COleInitialize && _oiOther ) noexcept :
			m_hrResult( _oiOther.m_hrResult ),
			m_bInitialized( _oiOther.m_bInitialized ) {
			_oiOther.m_hrResult = E_FAIL;
			_oiOther.m_bInitialized = false;
		}


		// == Operators.
		/**
		 * \brief Move-assigns, transferring ownership of the initialization token.
		 *
		 * \param _oiOther The source object.
		 * \return Returns (*this).
		 */
		COleInitialize &								operator = ( COleInitialize && _oiOther ) noexcept {
			if ( this != &_oiOther ) {
				if ( m_bInitialized ) {
					::OleUninitialize();
				}
				m_hrResult = _oiOther.m_hrResult;
				m_bInitialized = _oiOther.m_bInitialized;

				_oiOther.m_hrResult = E_FAIL;
				_oiOther.m_bInitialized = false;
			}
			return (*this);
		}


		// == Functions.
		/**
		 * \brief Gets the HRESULT returned by ::OleInitialize().
		 *
		 * \return Returns the HRESULT returned by ::OleInitialize().
		 */
		HRESULT											Result() const { return m_hrResult; }

		/**
		 * \brief Returns true if OLE was initialized successfully.
		 *
		 * \return Returns true if OLE was initialized successfully.
		 */
		bool											Ok() const { return m_bInitialized; }

	private :
		// == Members.
		HRESULT											m_hrResult = E_FAIL;	/**< Result from ::OleInitialize(). */
		bool											m_bInitialized = false;	/**< True if ::OleUninitialize() must be called. */
	};

}	// namespace lsw
