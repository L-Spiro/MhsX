#pragma once

#include "../LSWWin.h"
#include "../Images/LSWBitmap.h"

namespace lsw {

	/**
	 * Class CBrush
	 * \brief RAII wrapper for a GDI brush handle (HBRUSH).
	 *
	 * Provides creation helpers for solid and pattern brushes and ensures
	 * the handle is released on destruction or when Reset() is called.
	 */
	class CBrush {
	public :
		CBrush();
		~CBrush();


		// == Functions.
		/**
		 * Creates a solid-color brush.
		 * \brief Wraps ::CreateSolidBrush() and stores the resulting HBRUSH on success.
		 *
		 * Any previously owned handle is released before storing the new one.
		 *
		 * \param _crColor The COLORREF specifying the solid color (0x00BBGGRR).
		 * \return Returns TRUE on success; otherwise FALSE.
		 */
		BOOL							CreateSolidBrush( COLORREF _crColor );

		/**
		 * Creates a pattern brush from a bitmap.
		 * \brief Wraps ::CreatePatternBrush() using the given CBitmap.
		 *
		 * Any previously owned handle is released before storing the new one.
		 * The pattern alignment follows the current brush origin.
		 *
		 * \param _bBitmap The source bitmap used to define the pattern.
		 * \return Returns TRUE on success; otherwise FALSE.
		 */
		BOOL							CreatePatternBrush( const CBitmap &_bBitmap );

		/**
		 * Resets the brush to an empty state.
		 * \brief Releases the owned HBRUSH and sets the internal handle to NULL.
		 */
		VOID							Reset();

		/**
		 * Gets the underlying brush handle.
		 * \brief Accessor for the managed HBRUSH.
		 *
		 * \return Returns the HBRUSH, or nullptr if none.
		 */
		HBRUSH							Handle() const { return m_hHandle; }


	protected :
		// == Members.
		/**
		 * The managed brush handle.
		 * \brief Owned HBRUSH, or nullptr when not set.
		 */
		HBRUSH							m_hHandle;

	};

}	// namespace lsw
