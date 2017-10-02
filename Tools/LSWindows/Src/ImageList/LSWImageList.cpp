#include "LSWImageList.h"

namespace lsw {

	CImageList::CImageList() :
		m_iHandle( NULL ) {
	}
	CImageList::~CImageList() {
	}


	// == Functions.
	// Creates the image list.
	BOOL CImageList::Create( int _cx, int _cy, UINT _flags, int _cInitial, int _cGrow ) {
		Reset();
		m_iHandle = ::ImageList_Create( _cx, _cy, _flags, _cInitial, _cGrow );
		return m_iHandle ? TRUE : FALSE;
	}

	// Creates an image list from the specified bitmap.
	BOOL CImageList::LoadImageW( HINSTANCE _hi, LPCWSTR _lpbmp, int _cx, int _cGrow, COLORREF _crMask, UINT _uType, UINT _uFlags ) {
		Reset();
		m_iHandle = ::ImageList_LoadImageW( _hi, _lpbmp, _cx, _cGrow, _crMask, _uType, _uFlags );
		return m_iHandle ? TRUE : FALSE;
	}

	// Adds an image or images to an image list.  Returns the index of the first new image if successful, or -1 otherwise.
	//	Be sure to use the DeleteObject function to delete hbmImage and hbmMask after the function returns.
	int CImageList::Add( HBITMAP _hbmImage, HBITMAP _hbmMask ) {
		if ( !m_iHandle ) { return -1; }
		return ::ImageList_Add( m_iHandle, _hbmImage, _hbmMask );
	}

	// Adds an image or images to an image list, generating a mask from the specified bitmap.  Returns the index of the first new image if successful, or -1 otherwise.
	//	Bitmaps with color depth greater than 8bpp are not supported. Be sure to use the DeleteObject function to delete hbmImage after the function returns.
	int CImageList::AddMasked( HBITMAP _hbmImage, COLORREF _crMask ) {
		if ( !m_iHandle ) { return -1; }
		return ::ImageList_AddMasked( m_iHandle, _hbmImage, _crMask );
	}

	// Draws an image list item in the specified device context.
	BOOL CImageList::Draw( int _i, HDC _hdcDst, int _x, int _y, UINT _fStyle ) {
		if ( !m_iHandle ) { return FALSE; }
		return ::ImageList_Draw( m_iHandle, _i, _hdcDst, _x, _y, _fStyle );
	}

	// Reset the list (free all resources and start over).
	VOID CImageList::Reset() {
		if ( m_iHandle ) {
			::ImageList_Destroy( m_iHandle );
			m_iHandle = NULL;
		}
	}

}	// namespace lsw