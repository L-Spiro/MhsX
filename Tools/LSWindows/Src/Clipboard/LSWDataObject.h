#pragma once

#include "../LSWWin.h"
#include "LSWEnumFormatEtc.h"

#include <functional>
#include <Ole2.h>
#include <wrl/client.h>

namespace lsw {

	/**
	 * \brief Simple IDataObject implementation with per-format render callbacks.
	 *
	 * Consumers call IDataObject::GetData() and you generate the requested STGMEDIUM on demand.
	 * This enables delayed rendering (materialization happens only when requested).
	 */
	class CDataObject : public IDataObject {
	public :
		/**
		 * \brief Signature for producing data for a format request.
		 *
		 * The callback must fill _smOut and return an HRESULT.
		 * Unless you need custom releasing semantics, set _smOut.pUnkForRelease to NULL.
		 *
		 * \param _feRequest The requested format.
		 * \param _smOut Receives the returned data medium.
		 * \return Returns S_OK on success or a COM error code on failure.
		 */
		using RenderFn = std::function<HRESULT( const FORMATETC & _feRequest, STGMEDIUM & _smOut )>;

		/**
		 * \brief Constructs an empty data object.
		 */
		CDataObject() :
			m_lRefCount( 1 ) {
		}


		// == Functions.
		/**
		 * \brief Queries for supported interfaces.
		 *
		 * \param _riid Interface id.
		 * \param _ppv Receives the interface pointer.
		 * \return Returns S_OK on success or E_NOINTERFACE if unsupported.
		 */
		HRESULT STDMETHODCALLTYPE							QueryInterface( REFIID _riid, void ** _ppv ) override {
			if ( !_ppv ) { return E_POINTER; }
			(*_ppv) = NULL;

			if ( _riid == IID_IUnknown || _riid == IID_IDataObject ) {
				(*_ppv) = static_cast<IDataObject *>(this);
				AddRef();
				return S_OK;
			}
			return E_NOINTERFACE;
		}

		/**
		 * \brief Increments the reference count.
		 *
		 * \return Returns the new reference count.
		 */
		ULONG STDMETHODCALLTYPE								AddRef() override {
			return static_cast<ULONG>( ::InterlockedIncrement( &m_lRefCount ) );
		}

		/**
		 * \brief Decrements the reference count and deletes the object when it reaches 0.
		 *
		 * \return Returns the new reference count.
		 */
		ULONG STDMETHODCALLTYPE								Release() override {
			LONG lRef = ::InterlockedDecrement( &m_lRefCount );
			if ( lRef == 0 ) {
				delete this;
				return 0;
			}
			return static_cast<ULONG>( lRef );
		}

		/**
		 * \brief Retrieves data described by FORMATETC and stores it in STGMEDIUM.
		 *
		 * \param _pfe Describes the requested format.
		 * \param _psm Receives the data.
		 * \return Returns S_OK on success or DV_E_FORMATETC if the format is not supported.
		 */
		HRESULT STDMETHODCALLTYPE							GetData( FORMATETC * _pfe, STGMEDIUM * _psm ) override {
			if ( !_pfe || !_psm ) { return E_POINTER; }
			::ZeroMemory( _psm, sizeof( (*_psm) ) );

			const SEntry * pEntry = FindBestMatch( (*_pfe) );
			if ( !pEntry ) { return DV_E_FORMATETC; }

			return pEntry->fnRender( (*_pfe), (*_psm) );
		}

		/**
		 * \brief Retrieves data directly into a caller-provided medium.
		 *
		 * \param _pfe Describes the requested format.
		 * \param _psm Caller-provided medium.
		 * \return Returns DATA_E_FORMATETC since this implementation does not support GetDataHere.
		 */
		HRESULT STDMETHODCALLTYPE							GetDataHere( FORMATETC * _pfe, STGMEDIUM * _psm ) override {
			(void)_pfe;
			(void)_psm;
			return DATA_E_FORMATETC;
		}

		/**
		 * \brief Determines if the object can supply data described by FORMATETC.
		 *
		 * \param _pfe Describes the requested format.
		 * \return Returns S_OK if the format is supported, otherwise DV_E_FORMATETC.
		 */
		HRESULT STDMETHODCALLTYPE							QueryGetData( FORMATETC * _pfe ) override {
			if ( !_pfe ) { return E_POINTER; }
			return FindBestMatch( (*_pfe) ) ? S_OK : DV_E_FORMATETC;
		}

		/**
		 * \brief Returns the canonical FORMATETC form (not implemented).
		 *
		 * \param _pfeIn Input format.
		 * \param _pfeOut Output format.
		 * \return Returns E_NOTIMPL.
		 */
		HRESULT STDMETHODCALLTYPE							GetCanonicalFormatEtc( FORMATETC * _pfeIn, FORMATETC * _pfeOut ) override {
			(void)_pfeIn;
			if ( !_pfeOut ) { return E_POINTER; }
			_pfeOut->ptd = NULL;
			return E_NOTIMPL;
		}

		/**
		 * \brief Sets data on the object (not implemented).
		 *
		 * \param _pfe Format description.
		 * \param _psm Data medium.
		 * \param _fRelease Indicates whether the object should take ownership of _psm.
		 * \return Returns E_NOTIMPL.
		 */
		HRESULT STDMETHODCALLTYPE							SetData( FORMATETC * _pfe, STGMEDIUM * _psm, BOOL _fRelease ) override {
			(void)_pfe;
			(void)_psm;
			(void)_fRelease;
			return E_NOTIMPL;
		}

		/**
		 * \brief Creates an enumerator for the supported formats.
		 *
		 * \param _dwDirection Enumeration direction (DATADIR_GET only).
		 * \param _ppEnum Receives the enumerator.
		 * \return Returns S_OK on success, E_NOTIMPL for unsupported directions.
		 */
		HRESULT STDMETHODCALLTYPE							EnumFormatEtc( DWORD _dwDirection, IEnumFORMATETC ** _ppEnum ) override {
			if ( !_ppEnum ) { return E_POINTER; }
			(*_ppEnum) = NULL;

			if ( _dwDirection != DATADIR_GET ) { return E_NOTIMPL; }

			std::vector<FORMATETC> vFe;
			vFe.reserve( m_vEntries.size() );
			for ( const auto & e : m_vEntries ) {
				vFe.push_back( e.fe );
			}

			CEnumFormatEtc * pEnum = new (std::nothrow) CEnumFormatEtc( vFe );
			if ( !pEnum ) { return E_OUTOFMEMORY; }

			(*_ppEnum) = pEnum;
			return S_OK;
		}

		/**
		 * \brief Establishes an advisory connection (not supported).
		 *
		 * \param _pfe Format to advise.
		 * \param _advf Advisory flags.
		 * \param _pAdvSink Sink interface.
		 * \param _pdwConnection Receives the connection token.
		 * \return Returns OLE_E_ADVISENOTSUPPORTED.
		 */
		HRESULT STDMETHODCALLTYPE							DAdvise( FORMATETC * _pfe, DWORD _advf, IAdviseSink * _pAdvSink, DWORD * _pdwConnection ) override {
			(void)_pfe;
			(void)_advf;
			(void)_pAdvSink;
			(void)_pdwConnection;
			return OLE_E_ADVISENOTSUPPORTED;
		}

		/**
		 * \brief Terminates an advisory connection (not supported).
		 *
		 * \param _dwConnection Connection token.
		 * \return Returns OLE_E_ADVISENOTSUPPORTED.
		 */
		HRESULT STDMETHODCALLTYPE							DUnadvise( DWORD _dwConnection ) override {
			(void)_dwConnection;
			return OLE_E_ADVISENOTSUPPORTED;
		}

		/**
		 * \brief Creates an enumerator for advisory connections (not supported).
		 *
		 * \param _ppEnumAdvise Receives the enumerator.
		 * \return Returns OLE_E_ADVISENOTSUPPORTED.
		 */
		HRESULT STDMETHODCALLTYPE							EnumDAdvise( IEnumSTATDATA ** _ppEnumAdvise ) override {
			(void)_ppEnumAdvise;
			return OLE_E_ADVISENOTSUPPORTED;
		}

		
		// == Helpers.
		/**
		 * \brief Adds a format with a custom render callback.
		 *
		 * \param _fe The offered FORMATETC.
		 * \param _fnRender Callback that renders the data when requested.
		 * \return Returns S_OK on success, E_OUTOFMEMORY on allocation failure.
		 */
		HRESULT												AddFormat( const FORMATETC & _fe, RenderFn _fnRender ) {
			SEntry eEntry{};
			eEntry.fe = _fe;
			eEntry.fnRender = std::move( _fnRender );

			try {
				m_vEntries.push_back( std::move( eEntry ) );
			}
			catch ( ... ) { return E_OUTOFMEMORY; }

			return S_OK;
		}

		/**
		 * \brief Adds an immediate binary payload as TYMED_HGLOBAL by duplicating bytes on each request.
		 *
		 * \param _uiClipboardFormat Clipboard format id.
		 * \param _pvData Pointer to bytes.
		 * \param _stSize Number of bytes.
		 * \return Returns S_OK on success.
		 */
		HRESULT												AddBinaryCopyHGlobal( UINT _uiClipboardFormat, const void * _pvData, size_t _stSize ) {
			std::vector<uint8_t> vBytes;
			try {
				vBytes.resize( _stSize );
			}
			catch ( ... ) { return E_OUTOFMEMORY; }

			if ( _stSize ) {
				std::memcpy( vBytes.data(), _pvData, _stSize );
			}

			FORMATETC fe{};
			fe.cfFormat = static_cast<CLIPFORMAT>( _uiClipboardFormat );
			fe.dwAspect = DVASPECT_CONTENT;
			fe.lindex = -1;
			fe.tymed = TYMED_HGLOBAL;

			return AddFormat( fe, [vBytes = std::move( vBytes )]( const FORMATETC & _feRequest, STGMEDIUM & _smOut ) -> HRESULT {
				if ( (_feRequest.tymed & TYMED_HGLOBAL) == 0 ) { return DV_E_TYMED; }

				HGLOBAL hMem = COleStorage::CreateHGlobalCopy( vBytes.data(), vBytes.size() );
				if ( !hMem ) { return E_OUTOFMEMORY; }

				_smOut.tymed = TYMED_HGLOBAL;
				_smOut.hGlobal = hMem;
				_smOut.pUnkForRelease = NULL;
				return S_OK;
			} );
		}

		/**
		 * \brief Adds an immediate UTF-16 text payload as CF_UNICODETEXT by duplicating bytes on each request.
		 *
		 * \param _wsText Text to provide.
		 * \return Returns S_OK on success.
		 */
		HRESULT												AddUnicodeTextCopy( const std::wstring & _wsText ) {
			std::wstring wsText = _wsText;

			FORMATETC fe{};
			fe.cfFormat = CF_UNICODETEXT;
			fe.dwAspect = DVASPECT_CONTENT;
			fe.lindex = -1;
			fe.tymed = TYMED_HGLOBAL;

			return AddFormat( fe, [wsText = std::move( wsText )]( const FORMATETC & _feRequest, STGMEDIUM & _smOut ) -> HRESULT {
				if ( (_feRequest.tymed & TYMED_HGLOBAL) == 0 ) { return DV_E_TYMED; }

				HGLOBAL hMem = COleStorage::CreateHGlobalUnicodeText( wsText );
				if ( !hMem ) { return E_OUTOFMEMORY; }

				_smOut.tymed = TYMED_HGLOBAL;
				_smOut.hGlobal = hMem;
				_smOut.pUnkForRelease = NULL;
				return S_OK;
			} );
		}

		/**
		 * \brief Adds a delayed binary format that produces TYMED_HGLOBAL on demand.
		 *
		 * \param _uiClipboardFormat Clipboard format id.
		 * \param _fnCreate Callback that returns an HGLOBAL containing the full payload.
		 * \return Returns S_OK on success.
		 *
		 * \note The returned HGLOBAL must be GMEM_MOVEABLE. Ownership transfers to the receiver on success.
		 */
		HRESULT												AddDelayedBinaryHGlobal( UINT _uiClipboardFormat, std::function<HGLOBAL()> _fnCreate ) {
			FORMATETC fe{};
			fe.cfFormat = static_cast<CLIPFORMAT>( _uiClipboardFormat );
			fe.dwAspect = DVASPECT_CONTENT;
			fe.lindex = -1;
			fe.tymed = TYMED_HGLOBAL;

			return AddFormat( fe, [_fnCreate = std::move( _fnCreate )]( const FORMATETC & _feRequest, STGMEDIUM & _smOut ) -> HRESULT {
				if ( (_feRequest.tymed & TYMED_HGLOBAL) == 0 ) { return DV_E_TYMED; }

				HGLOBAL hMem = _fnCreate ? _fnCreate() : NULL;
				if ( !hMem ) { return E_FAIL; }

				_smOut.tymed = TYMED_HGLOBAL;
				_smOut.hGlobal = hMem;
				_smOut.pUnkForRelease = NULL;
				return S_OK;
			} );
		}

		/**
		 * \brief Adds a delayed stream format (TYMED_ISTREAM) on demand.
		 *
		 * \param _uiClipboardFormat Clipboard format id.
		 * \param _fnCreate Callback that creates an IStream for the payload.
		 * \return Returns S_OK on success.
		 *
		 * \note The callback should return an AddRef'd IStream; this helper detaches it into STGMEDIUM.
		 */
		HRESULT												AddDelayedIStream( UINT _uiClipboardFormat, std::function<HRESULT( Microsoft::WRL::ComPtr<IStream> & )> _fnCreate ) {
			FORMATETC fe{};
			fe.cfFormat = static_cast<CLIPFORMAT>( _uiClipboardFormat );
			fe.dwAspect = DVASPECT_CONTENT;
			fe.lindex = -1;
			fe.tymed = TYMED_ISTREAM;

			return AddFormat( fe, [_fnCreate = std::move( _fnCreate )]( const FORMATETC & _feRequest, STGMEDIUM & _smOut ) -> HRESULT {
				if ( (_feRequest.tymed & TYMED_ISTREAM) == 0 ) { return DV_E_TYMED; }

				Microsoft::WRL::ComPtr<IStream> spStream;
				HRESULT hr = _fnCreate ? _fnCreate( spStream ) : E_FAIL;
				if ( FAILED( hr ) ) { return hr; }
				if ( !spStream ) { return E_FAIL; }

				_smOut.tymed = TYMED_ISTREAM;
				_smOut.pstm = spStream.Detach();
				_smOut.pUnkForRelease = NULL;
				return S_OK;
			} );
		}

	private :
		struct SEntry {
			FORMATETC										fe{};
			RenderFn										fnRender;
		};

		static bool											Compatible( const FORMATETC & _feOffered, const FORMATETC & _feRequest ) {
			if ( _feOffered.cfFormat != _feRequest.cfFormat ) { return false; }
			if ( _feOffered.dwAspect != _feRequest.dwAspect ) { return false; }
			if ( _feRequest.lindex != -1 && _feOffered.lindex != _feRequest.lindex ) { return false; }
			if ( (_feOffered.tymed & _feRequest.tymed) == 0 ) { return false; }
			return true;
		}

		const SEntry *										FindBestMatch( const FORMATETC & _feRequest ) const {
			for ( const auto & e : m_vEntries ) {
				if ( Compatible( e.fe, _feRequest ) ) { return &e; }
			}
			return NULL;
		}

		~CDataObject() = default;


		// == Members.
		LONG												m_lRefCount = 1;	/**< Reference count. */
		std::vector<SEntry>									m_vEntries;			/**< Offered formats. */
	};

}	// namespace lsw
