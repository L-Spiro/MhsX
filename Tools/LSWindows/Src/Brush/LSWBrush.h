#pragma once

#include "../LSWWin.h"
#include "../Images/LSWBitmap.h"

namespace lsw {

	class CBrush {
	public :
		CBrush();
		~CBrush();


		// == Functions.
		// The CreateSolidBrush function creates a logical brush that has the specified solid color.
		BOOL							CreateSolidBrush( COLORREF _crColor );

		// The CreatePatternBrush function creates a logical brush with the specified bitmap pattern.
		BOOL							CreatePatternBrush( const CBitmap &_bBitmap );

		// Resets the brush.
		VOID							Reset();

		// Gets the handle.
		HBRUSH							Handle() const { return m_hHandle; }


	protected :
		// == Members.
		// The brush handle.
		HBRUSH							m_hHandle;

	};

}	// namespace lsw
