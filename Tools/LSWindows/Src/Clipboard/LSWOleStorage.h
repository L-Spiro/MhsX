#pragma once

#include "../LSWWin.h"

#include <Ole2.h>
#include <string>
#include <wrl/client.h>

namespace lsw {

	/**
	 * \brief Helpers for creating common clipboard storage types.
	 */
	class COleStorage {
	public :
		// == Functions.
		/**
		 * \brief Creates a GMEM_MOVEABLE HGLOBAL holding a copy of the given bytes.
		 *
		 * \param _pvData Pointer to bytes to copy.
		 * \param _stSize Number of bytes to copy.
		 * \return Returns the created HGLOBAL, or NULL on failure.
		 */
		static HGLOBAL									CreateHGlobalCopy( const void * _pvData, size_t _stSize ) {
			HGLOBAL hMem = ::GlobalAlloc( GMEM_MOVEABLE, static_cast<SIZE_T>(_stSize) );
			if ( !hMem ) { return NULL; }

			void * pvDst = ::GlobalLock( hMem );
			if ( !pvDst ) {
				::GlobalFree( hMem );
				return NULL;
			}

			if ( _stSize ) {
				std::memcpy( pvDst, _pvData, _stSize );
			}

			::GlobalUnlock( hMem );
			return hMem;
		}

		/**
		 * \brief Creates a GMEM_MOVEABLE HGLOBAL holding CF_UNICODETEXT (NUL-terminated).
		 *
		 * \param _wsText The text to copy.
		 * \return Returns the created HGLOBAL, or NULL on failure.
		 */
		static HGLOBAL									CreateHGlobalUnicodeText( const std::wstring & _wsText ) {
			const size_t stLen = _wsText.size();
			const size_t stBytes = (stLen + 1) * sizeof( wchar_t );

			HGLOBAL hMem = ::GlobalAlloc( GMEM_MOVEABLE, static_cast<SIZE_T>(stBytes) );
			if ( !hMem ) { return NULL; }

			void * pvDst = ::GlobalLock( hMem );
			if ( !pvDst ) {
				::GlobalFree( hMem );
				return NULL;
			}

			if ( stLen ) {
				std::memcpy( pvDst, _wsText.data(), stLen * sizeof( wchar_t ) );
			}
			static_cast<wchar_t *>(pvDst)[stLen] = L'\0';

			::GlobalUnlock( hMem );
			return hMem;
		}

		/**
		 * \brief Creates an IStream backed by an HGLOBAL.
		 *
		 * \param _hGlobal Global memory handle.
		 * \param _bDeleteOnRelease If true, the stream frees _hGlobal when released.
		 * \param _spStream Receives the created stream (Reset on entry).
		 * \return Returns an HRESULT indicating success or failure.
		 */
		static HRESULT									CreateStreamOnHGlobal( HGLOBAL _hGlobal, bool _bDeleteOnRelease, Microsoft::WRL::ComPtr<IStream> & _spStream ) {
			_spStream.Reset();
			IStream * pStream = NULL;
			HRESULT hr = ::CreateStreamOnHGlobal( _hGlobal, _bDeleteOnRelease ? TRUE : FALSE, &pStream );
			if ( SUCCEEDED( hr ) ) {
				_spStream.Attach( pStream );
			}
			return hr;
		}
	};

}	// namespace lsw
