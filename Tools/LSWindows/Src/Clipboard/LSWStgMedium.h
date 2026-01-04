#pragma once

#include "../LSWWin.h"

#include <Ole2.h>

namespace lsw {

	/**
	 * \brief RAII wrapper around STGMEDIUM that releases via ::ReleaseStgMedium().
	 */
	class CStgMedium {
	public :
		/**
		 * \brief Constructs an empty medium.
		 */
		CStgMedium() {
			::ZeroMemory( &m_smMedium, sizeof( m_smMedium ) );
		}

		/**
		 * \brief Destroys the medium, calling ::ReleaseStgMedium() if needed.
		 */
		~CStgMedium() {
			Reset();
		}

		CStgMedium( const CStgMedium & ) = delete;
		CStgMedium &									operator = ( const CStgMedium & ) = delete;

		/**
		 * \brief Move-constructs a medium.
		 *
		 * \param _smOther The source object.
		 */
		CStgMedium( CStgMedium && _smOther ) noexcept :
			m_smMedium( _smOther.m_smMedium ) {
			::ZeroMemory( &_smOther.m_smMedium, sizeof( _smOther.m_smMedium ) );
		}


		// == Operators.
		/**
		 * \brief Move-assigns a medium.
		 *
		 * \param _smOther The source object.
		 * \return Returns (*this).
		 */
		CStgMedium &									operator = ( CStgMedium && _smOther ) noexcept {
			if ( this != &_smOther ) {
				Reset();
				m_smMedium = _smOther.m_smMedium;
				::ZeroMemory( &_smOther.m_smMedium, sizeof( _smOther.m_smMedium ) );
			}
			return (*this);
		}


		// == Functions.
		/**
		 * \brief Releases the current medium (if any) and resets it to empty.
		 */
		void											Reset() {
			if ( m_smMedium.tymed != TYMED_NULL ) {
				::ReleaseStgMedium( &m_smMedium );
				::ZeroMemory( &m_smMedium, sizeof( m_smMedium ) );
			}
		}

		/**
		 * \brief Gets a writable reference to the underlying STGMEDIUM.
		 *
		 * \return Returns a writable reference to the underlying STGMEDIUM.
		 */
		STGMEDIUM &										Medium() { return m_smMedium; }

		/**
		 * \brief Gets a read-only reference to the underlying STGMEDIUM.
		 *
		 * \return Returns a read-only reference to the underlying STGMEDIUM.
		 */
		const STGMEDIUM &								Medium() const { return m_smMedium; }

		/**
		 * \brief Gets a writable STGMEDIUM pointer for API calls.
		 *
		 * \return Returns a writable STGMEDIUM pointer.
		 */
		STGMEDIUM *										Ptr() { return &m_smMedium; }

		/**
		 * \brief Gets a read-only STGMEDIUM pointer for API calls.
		 *
		 * \return Returns a read-only STGMEDIUM pointer.
		 */
		const STGMEDIUM *								Ptr() const { return &m_smMedium; }

		/**
		 * \brief Implicit conversion to a writable STGMEDIUM pointer.
		 *
		 * \return Returns a writable STGMEDIUM pointer.
		 */
		operator STGMEDIUM * () { return &m_smMedium; }

		/**
		 * \brief Implicit conversion to a read-only STGMEDIUM pointer.
		 *
		 * \return Returns a read-only STGMEDIUM pointer.
		 */
		operator const STGMEDIUM * () const { return &m_smMedium; }

	private :
		// == Members.
		STGMEDIUM										m_smMedium{};	/**< The owned medium. */
	};

}	// namespace lsw
