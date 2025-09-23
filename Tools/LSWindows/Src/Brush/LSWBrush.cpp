#include "LSWBrush.h"
#ifdef _DEBUG
#include "../Base/LSWBase.h"
#endif	// #ifdef _DEBUG

namespace lsw {

	CBrush::CBrush() :
		m_hHandle( NULL ) {
	}
	CBrush::~CBrush() {
		Reset();
	}

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
	BOOL CBrush::CreateSolidBrush( COLORREF _crColor ) {
		Reset();
		m_hHandle = ::CreateSolidBrush( _crColor );
#ifdef _DEBUG
		if ( !m_hHandle ) {
			CBase::PrintError( L"CBrush::CreateSolidBrush()" );
		}
#endif	// #ifdef _DEBUG
		return m_hHandle ? TRUE : FALSE;
	}

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
	BOOL CBrush::CreatePatternBrush( const CBitmap &_bBitmap ) {
		Reset();
		m_hHandle = ::CreatePatternBrush( _bBitmap.Handle() );
#ifdef _DEBUG
		if ( !m_hHandle ) {
			CBase::PrintError( L"CBrush::CreatePatternBrush()" );
		}
#endif	// #ifdef _DEBUG
		return m_hHandle ? TRUE : FALSE;
	}

	/**
	 * Resets the brush to an empty state.
	 * \brief Releases the owned HBRUSH and sets the internal handle to NULL.
	 */
	VOID CBrush::Reset() {
		if ( m_hHandle ) {
			::DeleteObject( m_hHandle );
			m_hHandle = NULL;
		}
	}

}	// namespace lsw
