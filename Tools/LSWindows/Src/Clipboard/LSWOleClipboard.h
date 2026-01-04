#pragma once

#include "../LSWWin.h"

#include <wrl/client.h>

//#include <Windows.h>
//#include <Ole2.h>
//#include <ObjIdl.h>
//
//
//#include <cstdint>
//#include <vector>
//#include <string>
//#include <functional>
//#include <utility>
//#include <new>
//#include <cstring>

namespace lsw {

	/**
	 * \brief Convenience wrapper for OLE clipboard operations.
	 */
	class COleClipboard {
	public :
		// == Functions.
		/**
		 * \brief Sets an IDataObject onto the clipboard.
		 *
		 * \param _spObject The object to place on the clipboard.
		 * \return Returns an HRESULT indicating success or failure.
		 */
		static HRESULT										Set( const Microsoft::WRL::ComPtr<IDataObject> & _spObject ) {
			return ::OleSetClipboard( _spObject.Get() );
		}

		/**
		 * \brief Sets an IDataObject onto the clipboard.
		 *
		 * \param _pObject The object to place on the clipboard.
		 * \return Returns an HRESULT indicating success or failure.
		 */
		static HRESULT										Set( IDataObject * _pObject ) {
			return ::OleSetClipboard( _pObject );
		}

		/**
		 * \brief Retrieves the current clipboard IDataObject.
		 *
		 * \param _spOut Receives the clipboard IDataObject.
		 * \return Returns an HRESULT indicating success or failure.
		 */
		static HRESULT										Get( Microsoft::WRL::ComPtr<IDataObject> & _spOut ) {
			_spOut.Reset();
			IDataObject * pObj = NULL;
			HRESULT hr = ::OleGetClipboard( &pObj );
			if ( SUCCEEDED( hr ) && pObj ) {
				_spOut.Attach( pObj );
			}
			return hr;
		}

		/**
		 * \brief Forces the clipboard to render any delayed formats and become independent of the source app.
		 *
		 * \return Returns an HRESULT indicating success or failure.
		 */
		static HRESULT										Flush() {
			return ::OleFlushClipboard();
		}

		/**
		 * \brief Returns true if the given IDataObject is the current clipboard object.
		 *
		 * \param _pObject The object to test.
		 * \return Returns true if _pObject is current.
		 */
		static bool											IsCurrent( IDataObject * _pObject ) {
			return ::OleIsCurrentClipboard( _pObject ) == S_OK;
		}

		/**
		 * \brief Returns true if the given IDataObject is the current clipboard object.
		 *
		 * \param _spObject The object to test.
		 * \return Returns true if _spObject is current.
		 */
		static bool											IsCurrent( const Microsoft::WRL::ComPtr<IDataObject> & _spObject ) {
			return ::OleIsCurrentClipboard( _spObject.Get() ) == S_OK;
		}
	};

}	// namespace lsw
