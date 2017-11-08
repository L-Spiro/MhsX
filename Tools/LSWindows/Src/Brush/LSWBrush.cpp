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
	// The CreateSolidBrush function creates a logical brush that has the specified solid color.
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

	// The CreatePatternBrush function creates a logical brush with the specified bitmap pattern.
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

	// Resets the brush.
	VOID CBrush::Reset() {
		if ( m_hHandle ) {
			::DeleteObject( m_hHandle );
			m_hHandle = NULL;
		}
	}

}	// namespace lsw
