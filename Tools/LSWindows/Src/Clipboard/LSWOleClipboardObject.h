#pragma once

#include "../LSWWin.h"
#include "LSWOleClipboard.h"

#include <utility>
#include <wrl/client.h>

namespace lsw {

	/**
	 * \brief RAII helper that acquires and releases an IDataObject from the clipboard.
	 */
	class COleClipboardObject {
	public :
		/**
		 * \brief Constructs and attempts to acquire the clipboard IDataObject.
		 */
		COleClipboardObject() {
			m_hrResult = COleClipboard::Get( m_spDataObject );
		}

		/**
		 * \brief Constructs and attempts to acquire the clipboard IDataObject.
		 *
		 * \param _hrOut Receives the HRESULT from ::OleGetClipboard().
		 */
		explicit COleClipboardObject( HRESULT & _hrOut ) {
			m_hrResult = COleClipboard::Get( m_spDataObject );
			_hrOut = m_hrResult;
		}

		/**
		 * \brief Resets the held IDataObject.
		 */
		~COleClipboardObject() {
			m_spDataObject.Reset();
		}

		COleClipboardObject( const COleClipboardObject & ) = delete;
		COleClipboardObject &							operator = ( const COleClipboardObject & ) = delete;

		/**
		 * \brief Move-constructs.
		 *
		 * \param _cOther The source object.
		 */
		COleClipboardObject( COleClipboardObject && _cOther ) noexcept :
			m_spDataObject( std::move( _cOther.m_spDataObject ) ),
			m_hrResult( _cOther.m_hrResult ) {
			_cOther.m_hrResult = E_FAIL;
		}

		/**
		 * \brief Move-assigns.
		 *
		 * \param _cOther The source object.
		 * \return Returns (*this).
		 */
		COleClipboardObject &							operator = ( COleClipboardObject && _cOther ) noexcept {
			if ( this != &_cOther ) {
				m_spDataObject = std::move( _cOther.m_spDataObject );
				m_hrResult = _cOther.m_hrResult;
				_cOther.m_hrResult = E_FAIL;
			}
			return (*this);
		}


		// == Functions.
		/**
		 * \brief Gets the HRESULT from acquisition.
		 *
		 * \return Returns the HRESULT from acquisition.
		 */
		HRESULT											Result() const { return m_hrResult; }

		/**
		 * \brief Returns true if an IDataObject is held.
		 *
		 * \return Returns true if an IDataObject is held.
		 */
		bool											Ok() const { return m_spDataObject != NULL; }

		/**
		 * \brief Gets the held IDataObject.
		 *
		 * \return Returns the held IDataObject pointer.
		 */
		IDataObject *									Get() const { return m_spDataObject.Get(); }

		/**
		 * \brief Gets the held IDataObject ComPtr.
		 *
		 * \return Returns the held IDataObject ComPtr.
		 */
		const Microsoft::WRL::ComPtr<IDataObject> &		Ptr() const { return m_spDataObject; }

	private :
		// == Members.
		Microsoft::WRL::ComPtr<IDataObject>				m_spDataObject;				/**< The held data object. */
		HRESULT											m_hrResult = E_FAIL;		/**< Result of acquisition. */
	};

}	// namespace lsw
