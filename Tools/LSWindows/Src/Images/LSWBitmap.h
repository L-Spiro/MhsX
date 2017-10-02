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

		// Resets the image.
		VOID							Reset();


	protected :
		// == Members.
		// The bitmap handle.
		HANDLE							m_hHandle;
	};

}	// namespace lsw
