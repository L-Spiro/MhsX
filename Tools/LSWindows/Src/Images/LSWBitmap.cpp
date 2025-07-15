#include "LSWBitmap.h"
#include "../Base/LSWBase.h"

namespace lsw {

	CBitmap::CBitmap() :
		m_hHandle( NULL ) {
	}
	CBitmap::~CBitmap() {
		Reset();
	}


	// == Functions.
	/**
	 * Loads a bitmap from a file.
	 * 
	 * \param _pcPath The image to be loaded.
	 * \param _cxDesired The width, in pixels, of the icon or cursor. If this parameter is zero and the fuLoad parameter is LR_DEFAULTSIZE, the function uses the SM_CXICON or SM_CXCURSOR system metric value to set the width. If this parameter is zero and LR_DEFAULTSIZE is not used, the function uses the actual resource width.
	 * \param _cyDesired The height, in pixels, of the icon or cursor. If this parameter is zero and the fuLoad parameter is LR_DEFAULTSIZE, the function uses the SM_CYICON or SM_CYCURSOR system metric value to set the height. If this parameter is zero and LR_DEFAULTSIZE is not used, the function uses the actual resource height.
	 * \param _fuLoad This parameter can be one or more of the following values:
	 *	LR_CREATEDIBSECTION
	 *	LR_DEFAULTCOLOR
	 *	LR_DEFAULTSIZE
	 *	LR_LOADFROMFILE (always set).
	 *	LR_LOADMAP3DCOLORS
	 *	LR_LOADTRANSPARENT
	 *	LR_MONOCHROME
	 *	LR_SHARED
	 *	LR_VGACOLOR
	 * \return Returns TRUE if the load succeeds.
	 **/
	BOOL CBitmap::LoadFromFile( const WCHAR * _pcPath, int _cxDesired, int _cyDesired, UINT _fuLoad ) {
		Reset();

		m_hHandle = ::LoadImageW( NULL, _pcPath, IMAGE_BITMAP, _cxDesired, _cyDesired, _fuLoad | LR_LOADFROMFILE );
#ifdef _DEBUG
		if ( !m_hHandle ) {
			CBase::PrintError( L"CBitmap::LoadFromFile()" );
		}
#endif	// #ifdef _DEBUG
		return m_hHandle ? TRUE : FALSE;
	}

	/**
	 * Loads an image from an embedded resource.
	 * 
	 * \param _wId The image to be loaded.
	 * \param _uiType The type of image to be loaded.
	 * \param _cxDesired The width, in pixels, of the icon or cursor. If this parameter is zero and the fuLoad parameter is LR_DEFAULTSIZE, the function uses the SM_CXICON or SM_CXCURSOR system metric value to set the width. If this parameter is zero and LR_DEFAULTSIZE is not used, the function uses the actual resource width.
	 * \param _cyDesired The height, in pixels, of the icon or cursor. If this parameter is zero and the fuLoad parameter is LR_DEFAULTSIZE, the function uses the SM_CYICON or SM_CYCURSOR system metric value to set the height. If this parameter is zero and LR_DEFAULTSIZE is not used, the function uses the actual resource height.
	 * \param _fuLoad This parameter can be one or more of the following values:
	 *	LR_CREATEDIBSECTION
	 *	LR_DEFAULTCOLOR
	 *	LR_DEFAULTSIZE
	 *	LR_LOADFROMFILE
	 *	LR_LOADMAP3DCOLORS
	 *	LR_LOADTRANSPARENT
	 *	LR_MONOCHROME
	 *	LR_SHARED
	 *	LR_VGACOLOR
	 * \return Returns TRUE if the load succeeds.
	 **/
	BOOL CBitmap::LoadFromResource( WORD _wId, UINT _uiType, int _cxDesired, int _cyDesired, UINT _fuLoad ) {
		Reset();

		m_hHandle = ::LoadImageW( CBase::GetModuleHandleW( nullptr ), MAKEINTRESOURCEW( _wId ), _uiType, _cxDesired, _cyDesired, _fuLoad );
#ifdef _DEBUG
		if ( !m_hHandle ) {
			CBase::PrintError( L"CBitmap::LoadFromResource()" );
		}
#endif	// #ifdef _DEBUG
		return m_hHandle ? TRUE : FALSE;
	}

	/**
	 * Creates a bitmap.
	 * 
	 * \param _nWidth The bitmap width, in pixels.
	 * \param _nHeight The bitmap height, in pixels.
	 * \param _cPlanes The number of color planes used by the device.
	 * \param _cBitsPerPel The number of bits required to identify the color of a single pixel.
	 * \param _lpvBits A pointer to an array of color data used to set the colors in a rectangle of pixels. Each scan line in the rectangle must be word aligned (scan lines that are not word aligned must be padded with zeros). The buffer size expected, cj, can be calculated using the formula:
	 *		cj = (((_nWidth * _cPlanes * _cBitsPerPel + 15) >> 4) << 1) * _nHeight;
	 *	If this parameter is NULL, then the contents of the new bitmap are undefined.
	 * \return Returns TRUE if the creation succeeds.
	 **/
	BOOL CBitmap::CreateBitmap( int _nWidth, int _nHeight, UINT _cPlanes, UINT _cBitsPerPel, const VOID * _lpvBits ) {
		Reset();

		m_hHandle = ::CreateBitmap( _nWidth, _nHeight, _cPlanes, _cBitsPerPel, _lpvBits );
#ifdef _DEBUG
		if ( !m_hHandle ) {
			CBase::PrintError( L"CBitmap::CreateBitmap()" );
		}
#endif	// #ifdef _DEBUG
		return m_hHandle ? TRUE : FALSE;

	}

	/**
	 * Resets the image.
	 **/
	VOID CBitmap::Reset() {
		if ( m_hHandle != NULL ) {
			::DeleteObject( m_hHandle );
			m_hHandle = NULL;
		}
	}

}	// namespace lsw
