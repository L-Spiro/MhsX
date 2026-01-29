#pragma once

#include "../LSWWin.h"

#include <Ole2.h>
#include <vector>

namespace lsw {

	/**
	 * \brief Enumerates FORMATETC entries for IDataObject::EnumFormatEtc().
	 *
	 * \note This assumes FORMATETC::ptd is NULL (typical). If you use non-NULL ptd, you must deep-copy it.
	 */
	class CEnumFormatEtc : public IEnumFORMATETC {
	public :
		/**
		 * \brief Constructs an enumerator over the given format list.
		 *
		 * \param _vFormats The formats to enumerate.
		 */
		CEnumFormatEtc( const std::vector<FORMATETC> & _vFormats ) :
			m_lRefCount( 1 ),
			m_vFormats( _vFormats ),
			m_stIndex( 0 ) {
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

			if ( _riid == IID_IUnknown || _riid == IID_IEnumFORMATETC ) {
				(*_ppv) = static_cast<IEnumFORMATETC *>(this);
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
			return static_cast<ULONG>(::InterlockedIncrement( &m_lRefCount ));
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
			return static_cast<ULONG>(lRef);
		}

		/**
		 * \brief Retrieves the next celt elements in the enumeration sequence.
		 *
		 * \param _celt Number of elements requested.
		 * \param _rgelt Receives the elements.
		 * \param _pceltFetched Receives the number of elements returned.
		 * \return Returns S_OK if all requested elements were returned, otherwise S_FALSE.
		 */
		HRESULT STDMETHODCALLTYPE							Next( ULONG _celt, FORMATETC * _rgelt, ULONG * _pceltFetched ) override {
			if ( !_rgelt ) { return E_POINTER; }
			if ( _celt != 1 && !_pceltFetched ) { return E_POINTER; }

			ULONG uiFetched = 0;
			while ( uiFetched < _celt && m_stIndex < m_vFormats.size() ) {
				_rgelt[uiFetched] = m_vFormats[m_stIndex];
				++m_stIndex;
				++uiFetched;
			}

			if ( _pceltFetched ) { (*_pceltFetched) = uiFetched; }
			return (uiFetched == _celt) ? S_OK : S_FALSE;
		}

		/**
		 * \brief Skips over the specified number of elements.
		 *
		 * \param _celt Number of elements to skip.
		 * \return Returns S_OK if all elements were skipped, otherwise S_FALSE.
		 */
		HRESULT STDMETHODCALLTYPE							Skip( ULONG _celt ) override {
			const size_t stRemain = (m_stIndex < m_vFormats.size()) ? (m_vFormats.size() - m_stIndex) : 0;
			const size_t stSkip = (static_cast<size_t>(_celt) < stRemain) ? static_cast<size_t>(_celt) : stRemain;
			m_stIndex += stSkip;
			return (stSkip == _celt) ? S_OK : S_FALSE;
		}

		/**
		 * \brief Resets the enumeration to the beginning.
		 *
		 * \return Returns S_OK.
		 */
		HRESULT STDMETHODCALLTYPE							Reset() override {
			m_stIndex = 0;
			return S_OK;
		}

		/**
		 * \brief Creates a new enumerator with the same state as the current one.
		 *
		 * \param _ppEnum Receives the cloned enumerator.
		 * \return Returns S_OK on success.
		 */
		HRESULT STDMETHODCALLTYPE							Clone( IEnumFORMATETC ** _ppEnum ) override {
			if ( !_ppEnum ) { return E_POINTER; }
			(*_ppEnum) = NULL;

			CEnumFormatEtc * pNew = new (std::nothrow) CEnumFormatEtc( m_vFormats );
			if ( !pNew ) { return E_OUTOFMEMORY; }
			pNew->m_stIndex = m_stIndex;

			(*_ppEnum) = pNew;
			return S_OK;
		}

	private :
		~CEnumFormatEtc() = default;


		// == Members.
		LONG												m_lRefCount = 1;	/**< Reference count. */
		std::vector<FORMATETC>								m_vFormats;			/**< Formats to enumerate. */
		size_t												m_stIndex = 0;		/**< Current enumeration index. */
	};

}	// namespace lsw
