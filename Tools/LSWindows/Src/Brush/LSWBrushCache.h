#pragma once

#include "../LSWWin.h"

#include <unordered_map>

namespace lsw {

	/**
	 * \brief Brush cache.
	 *
	 * Description: Creates and reuses solid brushes for given colors.
	 */
	class CBrushCache {
	public :
		/**
		 * \brief Constructs an empty brush cache.
		 *
		 * Description: Initializes the internal storage with no brushes pre-created. The cache
		 * is not thread-safe; synchronize externally if accessed from multiple threads.
		 */
		CBrushCache() {
		}

		/**
		 * \brief Destroys the brush cache and releases all brushes.
		 *
		 * Description: Calls Reset() to delete all cached brush objects. After destruction,
		 * any HBRUSH previously returned by Brush( COLORREF ) must no longer be used.
		 */
		~CBrushCache() {
			Reset();
		}


		// == Functions.
		/**
		 * \brief Gets (or creates) a solid brush for a color.
		 *
		 * Description: Returns a cached HBRUSH corresponding to the specified COLORREF. If no
		 * brush exists yet, one is created via CreateSolidBrush( COLORREF ) and stored for reuse.
		 * The returned brush is owned by the cache; callers must not delete it.
		 *
		 * \param _crColor The RGB color for which a solid brush is requested.
		 * \return Returns an HBRUSH suitable for painting with the requested color.
		 *
		 * \remark Ownership: Do not call DeleteObject() on the returned brush. The cache manages
		 * its lifetime and will free it in Reset() or the destructor.
		 */
		HBRUSH											Brush( COLORREF _crColor ) {
			const auto it = m_mBrushes.find( _crColor );
			if ( it != m_mBrushes.end() ) { return it->second; }
			HBRUSH hBr = ::CreateSolidBrush( _crColor );
			m_mBrushes[_crColor] = hBr;
			return hBr;
		}

		/**
		 * \brief Deletes all cached brushes and clears the cache.
		 *
		 * Description: Iterates through the cache and calls DeleteObject() on each stored HBRUSH,
		 * then removes all entries. After this call, any previously obtained HBRUSH from Brush()
		 * becomes invalid and must not be used.
		 */
		void											Reset() {
			for ( auto & it : m_mBrushes ) {
				::DeleteObject( it.second );
			}
			m_mBrushes.clear();
		}

	private :
		std::unordered_map<COLORREF,HBRUSH>				m_mBrushes;
	};

}	// namespace lsw
