#pragma once

#include "../LSWWin.h"

namespace lsw {

	class CImageList {
	public :
		CImageList();
		~CImageList();


		// == Functions.
		// Gets the handle.
		HIMAGELIST							Handle() const { return m_iHandle; }

		// Creates the image list.
		BOOL								Create( int _cx, int _cy, UINT _flags, int _cInitial, int _cGrow );

		// Creates an image list from the specified bitmap.
		BOOL								LoadImageW( HINSTANCE _hi, LPCWSTR _lpbmp, int _cx, int _cGrow, COLORREF _crMask, UINT _uType, UINT _uFlags );

		// Retrieves the number of images in an image list.
		int									GetImageCount() const { return m_iHandle ? ::ImageList_GetImageCount( m_iHandle ) : 0; }

		// Adds an image or images to an image list.  Returns the index of the first new image if successful, or -1 otherwise.
		//	Be sure to use the DeleteObject function to delete hbmImage and hbmMask after the function returns.
		int									Add( HBITMAP _hbmImage, HBITMAP _hbmMask = NULL );

		// Adds an image or images to an image list, generating a mask from the specified bitmap.  Returns the index of the first new image if successful, or -1 otherwise.
		//	Bitmaps with color depth greater than 8bpp are not supported. Be sure to use the DeleteObject function to delete hbmImage after the function returns.
		int									AddMasked( HBITMAP _hbmImage, COLORREF _crMask );

		// Draws an image list item in the specified device context.
		BOOL								Draw( int _i, HDC _hdcDst, int _x, int _y, UINT _fStyle );

		// Removes an image from an image list.
		BOOL								Remove( int _i ) { return m_iHandle ? ::ImageList_Remove( m_iHandle, _i ) : FALSE; }

		// Reset the list (free all resources and start over).
		VOID								Reset();


	protected :
		// == Members.
		// The image list handle.
		HIMAGELIST							m_iHandle;
	};

}	// namespace lsw
