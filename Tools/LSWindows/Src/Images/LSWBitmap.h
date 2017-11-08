#pragma once

#include "../LSWWin.h"

namespace lsw {

	class CBitmap {
	public :
		CBitmap();
		~CBitmap();


		// == Functions.
		// Loads a bitmap from a file.
		BOOL							LoadFromFile( const WCHAR * _pcPath, int _cxDesired, int _cyDesired, UINT _fuLoad );

		// Creates a bitmap.
		BOOL							CreateBitmap( int _nWidth, int _nHeight, UINT _cPlanes, UINT _cBitsPerPel, const VOID * _lpvBits );

		// Resets the image.
		VOID							Reset();

		// Gets the handle.
		HBITMAP							Handle() const { return static_cast<HBITMAP>(m_hHandle); }


	protected :
		// == Members.
		// The bitmap handle.
		HANDLE							m_hHandle;
	};

}	// namespace lsw
