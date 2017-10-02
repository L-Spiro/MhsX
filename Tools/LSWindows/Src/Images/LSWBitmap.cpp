#include "LSWBitmap.h"
#ifdef _DEBUG
#include "../Base/LSWBase.h"
#endif	// #ifdef _DEBUG

namespace lsw {

	CBitmap::CBitmap() :
		m_hHandle( NULL ) {
	}
	CBitmap::~CBitmap() {
		Reset();
	}


	// == Functions.
	// Loads a bitmap from a file.
	BOOL CBitmap::LoadFromFile( const WCHAR * _pcPath, int _cxDesired, int _cyDesired, UINT _fuLoad ) {
		Reset();

		m_hHandle = LoadImageW( NULL, _pcPath, IMAGE_BITMAP, _cxDesired, _cyDesired, _fuLoad | LR_LOADFROMFILE );
#ifdef _DEBUG
		if ( !m_hHandle ) {
			CBase::PrintError( L"CBitmap::LoadFromFile()" );
		}
#endif	// #ifdef _DEBUG
		return m_hHandle ? TRUE : FALSE;
	}

	// Resets the image.
	VOID CBitmap::Reset() {
		if ( m_hHandle != NULL ) {
			::DeleteObject( m_hHandle );
			m_hHandle = NULL;
		}
	}

}	// namespace lsw
