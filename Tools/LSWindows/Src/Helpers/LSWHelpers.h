#pragma once

#include "../LSWWin.h"
#include "../Base/LSWBase.h"
#include "../Layout/LSWWidgetLayout.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <EEExpEval.h>
#include <hidsdi.h>
#include <string>
#include <Uxtheme.h>
#include <vector>

namespace lsw {

	struct LSW_RECT : public RECT {
		LSW_RECT() {}
		LSW_RECT( const RECT &_rRect ) { 
			left = _rRect.left;
			right = _rRect.right;
			top = _rRect.top;
			bottom = _rRect.bottom;
		}
		LSW_RECT( LONG _lLeft, LONG _lTop, LONG _lRight, LONG _lBottom ) {
			left = _lLeft;
			right = _lRight;
			top = _lTop;
			bottom = _lBottom;
		}

		
		// == Functions.
		LONG								Width() const { return right - left; }
		LONG								Height() const { return bottom - top; }
		LSW_RECT &							Zero() { left = right = top = bottom = 0; return (*this ); }
		VOID								SetWidth( LONG _lW ) { right = left + _lW; }
		VOID								SetHeight( LONG _lH ) { bottom = top + _lH; }
		POINT								UpperLeft() const { return { left, top }; }
		POINT								BottomRight() const { return { right, bottom }; }
		POINT								Center() const { return { left + (Width() >> 1), top + (Height() >> 1) }; }
		LSW_RECT							MapToDeskTop( HWND _hWnd ) const {
			LSW_RECT rTemp = (*this);
			::MapWindowPoints( _hWnd, HWND_DESKTOP, reinterpret_cast<POINT *>(&rTemp), 2 );
			return rTemp;
		}
		LSW_RECT							MapWindowPoints( HWND _hFrom, HWND _hTo ) const {
			LSW_RECT rTemp = (*this);
			::MapWindowPoints( _hFrom, _hTo, reinterpret_cast<POINT *>(&rTemp), 2 );
			return rTemp;
		}
		VOID								MoveBy( const POINT _pMove ) { MoveBy( _pMove.x, _pMove.y ); }
		VOID								MoveBy( INT _iX, INT _iY ) {
			left += _iX;
			right += _iX;
			top += _iY;
			bottom += _iY;
		}
		POINT								UpperLeftClientToScreen( HWND _hWnd ) const {
			POINT pTemp = UpperLeft();
			::ClientToScreen( _hWnd, &pTemp );
			return pTemp;
		}
		POINT								BottomRightClientToScreen( HWND _hWnd ) const {
			POINT pTemp = BottomRight();
			::ClientToScreen( _hWnd, &pTemp );
			return pTemp;
		}
		LSW_RECT							ClientToScreen( HWND _hWnd ) const {
			LSW_RECT rTemp = (*this);
			POINT pUl = rTemp.UpperLeft();
			POINT pBr = rTemp.BottomRight();
			::ClientToScreen( _hWnd, &pUl );
			::ClientToScreen( _hWnd, &pBr );
			rTemp.left = pUl.x;
			rTemp.right = pBr.x;
			rTemp.top = pUl.y;
			rTemp.bottom = pBr.y;
			return rTemp;
		}
		LSW_RECT							ScreenToClient( HWND _hWnd ) const {
			LSW_RECT rTemp = (*this);
			POINT pUl = rTemp.UpperLeft();
			POINT pBr = rTemp.BottomRight();
			::ScreenToClient( _hWnd, &pUl );
			::ScreenToClient( _hWnd, &pBr );
			rTemp.left = pUl.x;
			rTemp.right = pBr.x;
			rTemp.top = pUl.y;
			rTemp.bottom = pBr.y;
			return rTemp;
		}
		// Top and left checked inclusively.
		bool								PointIsIn_ULInclusive( INT _iX, INT _iY ) const {
			return _iX >= left && _iX < right &&
				_iY >= top && _iY < bottom;
		}
		// Top and left checked inclusively.
		bool								PointIsIn_ULInclusive( const POINT &_pPoint ) const {
			return PointIsIn_ULInclusive( _pPoint.x, _pPoint.y );
		}
		// Top, left, right, and bottom checked inclusively.
		bool								PointIsIn_FullInclusive( INT _iX, INT _iY ) const {
			return _iX >= left && _iX <= right &&
				_iY >= top && _iY <= bottom;
		}
		// Top, left, right, and bottom checked inclusively.
		bool								PointIsIn_FullInclusive( const POINT &_pPoint ) const {
			return PointIsIn_FullInclusive( _pPoint.x, _pPoint.y );
		}
		// Point must be inside the rectangle, not on any borders.
		bool								PointIsIn_Exclusive( INT _iX, INT _iY ) const {
			return _iX > left && _iX < right &&
				_iY > top && _iY < bottom;
		}
		// Point must be inside the rectangle, not on any borders.
		bool								PointIsIn_Exclusive( const POINT &_pPoint ) const {
			return PointIsIn_Exclusive( _pPoint.x, _pPoint.y );
		}
		// Calls ::PtInRect() to determine if a point is in this rectangle.
		BOOL								PtInRect( const POINT &_pPoint ) const {
			return ::PtInRect( this, _pPoint );
		}
		// Constrains this rectangle by the given rectangle (adjusts this rectangle to fit inside it).
		void								ConstrainBy( const LSW_RECT &_rRect ) {
			left = std::max( left, _rRect.left );
			top = std::max( top, _rRect.top );
			right = std::min( right, _rRect.right );
			bottom = std::min( bottom, _rRect.bottom );
		}
	};

	struct LSW_HANDLE {
		LSW_HANDLE() : hHandle( NULL ) {}
		LSW_HANDLE( HANDLE _hHandle ) : hHandle( _hHandle ) {}
		~LSW_HANDLE() {
			Reset();
		}

		LSW_HANDLE &						operator = ( HANDLE &_hHandle ) {
			Reset();
			hHandle = _hHandle;
			_hHandle = NULL;
			return (*this);
		}
		VOID								Reset() {
			if ( Valid() ) {
				::CloseHandle( hHandle );
				hHandle = NULL;
			}
		}


		// == Functions.
		BOOL								Valid() const { return hHandle && hHandle != INVALID_HANDLE_VALUE; }

		static BOOL							Valid( HANDLE _hHandle ) { return _hHandle && _hHandle != INVALID_HANDLE_VALUE; }


		HANDLE								hHandle;
	};

	struct LSW_REBARBANDINFO : REBARBANDINFOW {
		LSW_REBARBANDINFO() {
			std::memset( this, 0, sizeof( (*this) ) );
			cbSize = sizeof( REBARBANDINFOW );
		}


		// == Functions.
		LSW_REBARBANDINFO &					SetStyle( UINT _fStylemember ) {
			fMask |= RBBIM_STYLE;
			fStyle = _fStylemember;
			return (*this);
		}
		LSW_REBARBANDINFO &					SetColors( COLORREF _clrFore, COLORREF _clrBack = RGB( 0, 0, 0 ) ) {
			fMask |= RBBIM_COLORS;
			clrFore = _clrFore;
			clrBack = _clrBack;
			return (*this);
		}
		LSW_REBARBANDINFO &					SetText( LPWSTR _lpText ) {
			fMask |= RBBIM_TEXT;
			lpText = _lpText;
			cch = 0;
			return (*this);
		}
		LSW_REBARBANDINFO &					SetText( LPWSTR _lpText, UINT _cch ) {
			fMask |= RBBIM_TEXT;
			lpText = _lpText;
			cch = _cch;
			return (*this);
		}
		LSW_REBARBANDINFO &					SetImage( int _iImage ) {
			fMask |= RBBIM_IMAGE;
			iImage = _iImage;
			return (*this);
		}
		LSW_REBARBANDINFO &					SetChild( HWND _hwndChild ) {
			fMask |= RBBIM_CHILD;
			hwndChild = _hwndChild;
			return (*this);
		}
		LSW_REBARBANDINFO &					SetChildSize( UINT _cxMinChild, UINT _cyMinChild ) {
			fMask |= RBBIM_CHILDSIZE;
			cxMinChild = _cxMinChild;
			cyMinChild = _cyMinChild;
			return (*this);
		}
		LSW_REBARBANDINFO &					SetSize( UINT _cx ) {
			fMask |= RBBIM_SIZE;
			cx = _cx;
			return (*this);
		}
		LSW_REBARBANDINFO &					SetBackground( HBITMAP _hbmBack ) {
			fMask |= RBBIM_BACKGROUND;
			hbmBack = _hbmBack;
			return (*this);
		}
		LSW_REBARBANDINFO &					SetId( UINT _wID ) {
			fMask |= RBBIM_ID;
			wID = _wID;
			return (*this);
		}
	};

	struct LSW_SELECTOBJECT {
		LSW_SELECTOBJECT( HDC _hDc, HGDIOBJ _hgdiobj, bool _bDeleteNewObjAfter = false ) :
			hDc( _hDc ),
			hCur( _hgdiobj ),
			hPrev( ::SelectObject( _hDc, _hgdiobj ) ),
			bDeleteAfter( _bDeleteNewObjAfter ) {
		}
		~LSW_SELECTOBJECT() {
			if ( bDeleteAfter ) {
				::DeleteObject( ::SelectObject( hDc, hPrev ) );
			}
			else {
				::SelectObject( hDc, hPrev );
			}
		}

		HDC									hDc;
		HGDIOBJ								hCur;
		HGDIOBJ								hPrev;
		bool								bDeleteAfter;
	};

	struct LSW_BEGINPAINT {
		LSW_BEGINPAINT( HWND _hWnd ) :
			hWnd( _hWnd ) {
			hDc = ::BeginPaint( _hWnd, &psPaint );
		}
		~LSW_BEGINPAINT() {
			if ( hDc != NULL ) {
				::EndPaint( hWnd, &psPaint );
				hDc = NULL;
			}
			hWnd = NULL;
		}


		HWND								hWnd;
		HDC									hDc;
		PAINTSTRUCT							psPaint;
	};

	struct LSW_SETTEXTCOLOR {
		LSW_SETTEXTCOLOR( HDC _hDc, COLORREF _crColor ) :
			hDc( _hDc ),
			crCur( _crColor ),
			crPrev( ::SetTextColor( _hDc, _crColor ) ) {
		}
		~LSW_SETTEXTCOLOR() {
			::SetTextColor( hDc, crPrev );
		}

		HDC									hDc;
		COLORREF							crCur;
		COLORREF							crPrev;
	};

	struct LSW_SETBKCOLOR {
		LSW_SETBKCOLOR( HDC _hDc, COLORREF _crColor ) :
			hDc( _hDc ),
			crCur( _crColor ),
			crPrev( ::SetBkColor( _hDc, _crColor ) ) {
		}
		~LSW_SETBKCOLOR() {
			::SetBkColor( hDc, crPrev );
		}

		HDC									hDc;
		COLORREF							crCur;
		COLORREF							crPrev;
	};

	struct LSW_SETBKMODE {
		LSW_SETBKMODE( HDC _hDc, int _iMode ) :
			hDc( _hDc ),
			iCur( _iMode ) {
			iPrev = ::SetBkMode( _hDc, _iMode );
		}
		~LSW_SETBKMODE() {
			::SetBkMode( hDc, iPrev );
		}

		HDC									hDc;
		int									iCur;
		int									iPrev;
	};

	struct LSW_THREAD_PRIORITY {
		LSW_THREAD_PRIORITY( int _iPriority ) :
			iPrev( ::GetThreadPriority( ::GetCurrentThread() ) ),
			iCur( _iPriority ) {
			::SetThreadPriority( ::GetCurrentThread(), _iPriority );
		}
		~LSW_THREAD_PRIORITY() {
			::SetThreadPriority( ::GetCurrentThread(), iPrev );
		}

		int									iCur;
		int									iPrev;
	};

	struct LSW_CLIPBOARD {
		LSW_CLIPBOARD( HWND _hWnd, bool _bEmpty ) :
			bEmptied( FALSE ),
			bOpen( ::OpenClipboard( _hWnd ) ) {
			if ( bOpen && _bEmpty ) {
				bEmptied = ::EmptyClipboard();
			}
		}
		~LSW_CLIPBOARD() {
			if ( bOpen ) {
				::CloseClipboard();
			}
		}


		// == Functions.
		BOOL								SetText( const char * _pcText, size_t _stLen = -1 ) {
			if ( !_pcText || !bOpen ) { return FALSE; }
			if ( static_cast<int32_t>(_stLen) < 0 ) {
				_stLen = std::strlen( _pcText );
			}
			HGLOBAL hglbBinCopy;
			hglbBinCopy = ::GlobalAlloc( GMEM_MOVEABLE, (_stLen + 1) * sizeof( _pcText[0] ) );
			if ( !hglbBinCopy ) { return FALSE; }
			LPVOID lpvAddress = ::GlobalLock( hglbBinCopy );
			if ( !lpvAddress ) {
				::GlobalFree( hglbBinCopy );
				return FALSE;
			}
			std::memcpy( lpvAddress, _pcText, (_stLen + 1) * sizeof( _pcText[0] ) );
			char * pcText = static_cast<char *>(lpvAddress);
			pcText[_stLen] = L'\0';
			::GlobalUnlock( hglbBinCopy );

			::SetClipboardData( CF_TEXT, hglbBinCopy );
			return TRUE;
		}

		BOOL								SetText( const wchar_t * _pwcText, size_t _stLen = -1 ) {
			if ( !_pwcText || !bOpen ) {
				::OutputDebugStringA( "Failed to open.\r\n" );
				CBase::PrintError( L"Failed to open: " );
				return FALSE;
			}
			if ( static_cast<int32_t>(_stLen) < 0 ) {
				_stLen = std::wcslen( _pwcText );
			}
			HGLOBAL hglbBinCopy;
			hglbBinCopy = ::GlobalAlloc( GMEM_MOVEABLE, (_stLen + 1) * sizeof( _pwcText[0] ) );
			if ( !hglbBinCopy ) { return FALSE; }
			LPVOID lpvAddress = ::GlobalLock( hglbBinCopy );
			if ( !lpvAddress ) {
				::GlobalFree( hglbBinCopy );
				::OutputDebugStringA( "Failed to allocate.\r\n" );
				return FALSE;
			}
			std::memcpy( lpvAddress, _pwcText, (_stLen + 1) * sizeof( _pwcText[0] ) );
			wchar_t * pwcText = static_cast<wchar_t *>(lpvAddress);
			pwcText[_stLen] = L'\0';
			::GlobalUnlock( hglbBinCopy );

			::SetClipboardData( CF_UNICODETEXT, hglbBinCopy );
			return TRUE;
		}


		// == Members.
		BOOL								bOpen;
		BOOL								bEmptied;
	};

	struct LSW_KEY_FLAGS {
		union {
			DWORD							dwFlags;
			struct {
				DWORD						dwRepeatCnt : 16;
				DWORD						dwScanCode : 8;
				DWORD						dwIsExtended : 1;
				DWORD						dwReserved : 4;
				DWORD						dwContextCode : 1;
				DWORD						dwPrevState : 1;
				DWORD						dwTransitionState : 1;
			}								s;
		};
	};

	struct LSW_THEME_DATA {
		LSW_THEME_DATA( HWND _hWnd, LPCWSTR _pszClassList ) :
			hWnd( _hWnd ),
			wsClass( _pszClassList ) {
			htTheme = ::OpenThemeData( _hWnd, _pszClassList );
		}
		~LSW_THEME_DATA() {
			if ( NULL != htTheme ) {
				::CloseThemeData( htTheme );
			}
		}


		// == Functions.
		/**
		 * Gets the theme handle, creating it if necessary.
		 *
		 * \return Returns the theme handle.
		 */
		HTHEME								Handle() {
			if ( !htTheme ) {
				htTheme = ::OpenThemeData( hWnd, wsClass.c_str() );
			}
			return htTheme;
		}

		/**
		 * Handles WM_THEMECHANGED.
		 */
		void								ThemeChanged() {
			if ( NULL != htTheme ) {
				::CloseThemeData( htTheme );
				htTheme = NULL;
			}
		}


	protected :
		// == Members.
		/** The theme class. */
		std::wstring						wsClass;
		/** The Theme handle. */
		HTHEME								htTheme;
		/** The window handle. */
		HWND								hWnd;
	};

	struct LSW_RAW_INPUT_DEVICE_LIST {
		std::wstring						wsName;									/**< The device's name. */
		std::wstring						wsIdent;								/**< The device's identifier. */
		RID_DEVICE_INFO						diInfo;									/**< The device's information. */
	};

	class CHelpers {
	public :
		/**
		 * Aligns a WORD pointer to a 4-byte address.
		 *
		 * \param _lpIn The data to align.
		 * \return Returns the aligned value.
		 */
		static LPWORD						DwordAlign( LPWORD _lpIn ) {
			ULONG_PTR ulVal;

			ulVal = reinterpret_cast<ULONG_PTR>(_lpIn);
			ulVal += 3;
			ulVal >>= 2;
			ulVal <<= 2;
			return reinterpret_cast<LPWORD>(ulVal);
		}

		/**
		 * Finds a layout by its ID.
		 *
		 * \param _pwlLayouts The layouts through which to search.
		 * \param _sTotal The total number of widgets to which _pwlLayouts points.
		 * \param _dwId The ID of the widget to locate.
		 * \return Returns the found widget or nullptr.
		 */
		static LSW_WIDGET_LAYOUT *			FindLayout( LSW_WIDGET_LAYOUT * _pwlLayouts, size_t _sTotal, DWORD _dwId ) {
			for ( size_t I = 0; I < _sTotal; ++I ) {
				if ( _pwlLayouts[I].wId == _dwId ) { return &_pwlLayouts[I]; }
			}
			return nullptr;
		}

		/**
		 * Converts a message to a string.
		 *
		 * \param _wMessage The Windows message to turn into a string.
		 * \param _sRet Returned by the function after being filled with a text string containing the given Windows message.
		 * \param _bOnlyFirst Returns only the first identifier when multiple exist.
		 * \return The returned string.
		 */
		static std::string &				WindowsMessageToString( WORD _wMessage, std::string &_sRet, bool _bOnlyFirst = true );

		/**
		 * Converts MEMORY_BASIC_INFORMATION32 to MEMORY_BASIC_INFORMATION64.
		 *
		 * \param _mbiInfo The MEMORY_BASIC_INFORMATION32 object to convert.
		 * \return Returns the converted object
		 */
		static MEMORY_BASIC_INFORMATION64	MemoryBasicInformation32To64( const MEMORY_BASIC_INFORMATION32 &_mbiInfo ) {
			MEMORY_BASIC_INFORMATION64 mbiRet;
			mbiRet.AllocationBase = _mbiInfo.AllocationBase;
			mbiRet.AllocationProtect = _mbiInfo.AllocationProtect;
			mbiRet.BaseAddress = _mbiInfo.BaseAddress;
			mbiRet.Protect = _mbiInfo.Protect;
			mbiRet.RegionSize = _mbiInfo.RegionSize;
			mbiRet.State = _mbiInfo.State;
			mbiRet.Type = _mbiInfo.Type;
			return mbiRet;
		}

		/**
		 * Sets the window procedure on a window.
		 *
		 * \param _hWnd The window whose procedure is to be updated.
		 * \param _wpProc The new procedure to apply to the window.
		 * \return Returns the old procedure.
		 */
		static WNDPROC						SetWndProc( HWND _hWnd, WNDPROC _wpProc ) {
			return reinterpret_cast<WNDPROC>(::SetWindowLongPtrW( _hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(_wpProc) ));
		}

		/**
		 * Interpolates between 2 bytes.
		 *
		 * \param _bA Operand 1.
		 * \param _bB Operand 2.
		 * \param _dAmnt The amount to interpolate between the operands.
		 * \return RETURN
		 */
		static BYTE							Mix( BYTE _bA, BYTE _bB, double _dAmnt ) {
			return static_cast<BYTE>(std::round( (_bB - _bA) * _dAmnt + _bA ));
		}

		/**
		 * Interpolates between 2 bytes.
		 *
		 * \param _dA Operand 1.
		 * \param _dB Operand 2.
		 * \param _dAmnt The amount to interpolate between the operands.
		 * \return RETURN
		 */
		static double						Mix( double _dA, double _dB, double _dAmnt ) {
			return (_dB - _dA) * _dAmnt + _dA;
		}

		/**
		 * Converts from sRGB to linear.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 */
		static double						sRGBtoLinear( double _dVal ) {
			return _dVal <= 0.04045 ?
				_dVal * (1.0 / 12.92) :
				std::pow( (_dVal + 0.055) * (1.0 / 1.055), 2.4 );
		}

		/**
		 * Converts from linear to sRGB.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to sRGB space.
		 */
		static double						LinearTosRGB( double _dVal ) {
			return _dVal <= 0.0031308 ?
				_dVal * 12.92 :
				1.055 * std::pow( _dVal, 1.0 / 2.4 ) - 0.055;
		}

		/**
		 * Mixes between 2 24-bit RGB values.
		 *
		 * \param _dwColorA Operand 1.
		 * \param _dwColorB Operand 2.
		 * \param _dAmnt The amount to interpolate between the operands.
		 * \return Returns the interpolated 24-bit RGB value.
		 */
		static DWORD						MixColorRef( DWORD _dwColorA, DWORD _dwColorB, double _dAmnt ) {
			double dA = sRGBtoLinear( GetRValue( _dwColorA ) / 255.0 );
			double dB = sRGBtoLinear( GetRValue( _dwColorB ) / 255.0 );
			BYTE bR = static_cast<BYTE>(std::round( LinearTosRGB( Mix( dA, dB, _dAmnt ) ) * 255.0 ));
			dA = sRGBtoLinear( GetGValue( _dwColorA ) / 255.0 );
			dB = sRGBtoLinear( GetGValue( _dwColorB ) / 255.0 );
			BYTE bG = static_cast<BYTE>(std::round( LinearTosRGB( Mix( dA, dB, _dAmnt ) ) * 255.0 ));
			dA = sRGBtoLinear( GetBValue( _dwColorA ) / 255.0 );
			dB = sRGBtoLinear( GetBValue( _dwColorB ) / 255.0 );
			BYTE bB = static_cast<BYTE>(std::round( LinearTosRGB( Mix( dA, dB, _dAmnt ) ) * 255.0 ));
			return RGB( bR, bG, bB );
		}

		/**
		 * Gets the average character width for the font set on the given HDC.
		 *
		 * \param _hDc The device context.
		 * \return Returns the average size of upper-case and lower-case characters given the context's font.
		 */
		static SIZE							GetAveCharSize( HDC _hDc ) {
			/*
				How To Calculate Dialog Base Units with Non-System-Based Font
				http://support.microsoft.com/kb/125681
				https://www.betaarchive.com/wiki/index.php?title=Microsoft_KB_Archive/125681
			*/
			TEXTMETRICW tmTextMetric;
			::GetTextMetricsW( _hDc, &tmTextMetric );
			typedef std::wstring			strtype;
			strtype wBuffer;
			for ( auto I = 0; I < 26; ++I ) {
				wBuffer.push_back( strtype::value_type( L'A' + I ) );
				wBuffer.push_back( strtype::value_type( L'a' + I ) );
			}

			SIZE sResult;
			::GetTextExtentPoint32W( _hDc, wBuffer.c_str(), 52, &sResult );

			sResult.cx = (sResult.cx / 26 + 1) / 2;
			sResult.cy = tmTextMetric.tmHeight;
			// For MHS "string security".
			std::memset( const_cast<strtype::value_type *>(wBuffer.data()), 0, wBuffer.size() * sizeof( strtype::value_type ) );

			return sResult;
		}

		/**
		 * Gets the size of a given string printed with the given HDC.
		 *
		 * \param _wsString The string whose length in pixels is to be found.
		 * \param _hDc The device context.
		 * \return Returns the size of te given text in the given device context's font.
		 */
		static SIZE							GetTextSize( const std::wstring &_wsString, HDC _hDc ) {
			SIZE sResult;
			::GetTextExtentPoint32W( _hDc, _wsString.c_str(), static_cast<int>(_wsString.size()), &sResult );
			return sResult;
		}

		/**
		 * Gets the size of a given string printed with the given HDC.
		 *
		 * \param _sString The string whose length in pixels is to be found.
		 * \param _hDc The device context.
		 * \return Returns the size of te given text in the given device context's font.
		 */
		static SIZE							GetTextSize( const std::string &_sString, HDC _hDc ) {
			SIZE sResult;
			::GetTextExtentPoint32A( _hDc, _sString.c_str(), static_cast<int>(_sString.size()), &sResult );
			return sResult;
		}

		/**
		 * Gathers the menu ID's into an array.
		 *
		 * \param _hMenu The menu whose ID's are to be gathered.
		 * \return Returns a vector containing all of the menu ID's, or -1 for sub menus.
		 */
		static std::vector<int>				GetMenuItemIDs( HMENU _hMenu ) {
			int iCnt = ::GetMenuItemCount( _hMenu );
			std::vector<int> vRet;
			for ( int I = 0; I < iCnt; ++I ) {
				vRet.push_back( ::GetMenuItemID( _hMenu, I ) );
			}
			return vRet;
		}

		/**
		 * Removes double separators and separators at the ends of menus.
		 *
		 * \param _hMenu The menu to sanitize.
		 */
		static void							SanitizeMenuSeparators( HMENU _hMenu ) {
			std::vector<int> vIds = GetMenuItemIDs( _hMenu );
			while ( vIds.size() && vIds[vIds.size()-1] == 0 ) {
				// Separator at the end of the menu.
				::DeleteMenu( _hMenu, static_cast<UINT>(vIds.size() - 1), MF_BYPOSITION );
				vIds.pop_back();
			}
			// Remove double separators.
			if ( vIds.size() > 1 ) {
				for ( auto I = vIds.size(); --I >= 1; ) {
					if ( vIds[I] == 0 && vIds[I-1] == 0 ) {
						::DeleteMenu( _hMenu, static_cast<UINT>(I), MF_BYPOSITION );
						vIds.erase( vIds.begin() + I++ );
					}
				}
			}
		}

		/**
		 * Fully expands the PATH enviroment variable into an array of paths.
		 *
		 * \return Returns a vector of paths.
		 */
		template <typename _tType>
		static std::vector<_tType>			ExpandPATH() {
			_tType tTmp, tReplacement;
			tTmp.push_back( _tType::value_type( '%' ) );
			tTmp.push_back( _tType::value_type( 'P' ) );
			tTmp.push_back( _tType::value_type( 'A' ) );
			tTmp.push_back( _tType::value_type( 'T' ) );
			tTmp.push_back( _tType::value_type( 'H' ) );
			tTmp.push_back( _tType::value_type( '%' ) );
			size_t stStart, stEnd;
			while ( FindEnvVariableInString<_tType>( tTmp, stStart, stEnd, tReplacement ) ) {
				tTmp.replace( stStart, stEnd - stStart + 1, tReplacement );
			}
			// Remove quotes.
			for ( auto I = tTmp.size(); I--; ) {
				if ( tTmp[I] == _tType::value_type( '"' ) ) {
					tTmp.erase( tTmp.begin() + I );
				}
			}
			return ee::CExpEval::Tokenize<_tType>( tTmp, _tType::value_type( ';' ), false );
		}

		/**
		 * Helps expand %VAR% environment variables inside a string.  Gets the range of the first %...% encountered and returns the string that should be there.
		 *
		 * \param PARM The string in which to seek %...%.
		 * \param PARM Holds the returned start position of the %...% section.
		 * \param PARM Holds the returned end position of the %...% section.
		 * \param PARM Holds the replacement string, if true is returned.
		 * \return Returns true if a %...% value was found and expanded.
		 */
		template <typename _tType>
		static bool							FindEnvVariableInString( const _tType &_sString, size_t &_stStart, size_t &_stEnd, _tType &_sReplacement ) {
			_tType sTmp;
			_tType tExpanded;
			tExpanded.resize( 0xFFFF + 2 );
			for ( size_t I = 0; I < _sString.size(); ++I ) {
				if ( _sString[I] == _tType::value_type( '%' ) ) {
					sTmp.clear();
					for ( size_t J = I + 1; J < _sString.size(); ++J ) {
						if ( _sString[J] == _tType::value_type( '%' ) ) {
							DWORD dwSize = 0;
							if constexpr ( sizeof( _tType::value_type ) == sizeof( char ) ) {
								dwSize = ::GetEnvironmentVariableA( reinterpret_cast<LPCSTR>(sTmp.data()), reinterpret_cast<LPSTR>(tExpanded.data()), static_cast<DWORD>(tExpanded.size()) );
							}
							else if constexpr ( sizeof( _tType::value_type ) == sizeof( wchar_t ) ) {
								dwSize = ::GetEnvironmentVariableW( reinterpret_cast<LPCWSTR>(sTmp.data()), reinterpret_cast<LPWSTR>(tExpanded.data()), static_cast<DWORD>(tExpanded.size()) );
							}
							if ( dwSize ) {
								_stStart = I;
								_stEnd = J;
								_sReplacement.assign( tExpanded, 0, dwSize );
								return true;
							}
							else {
								I = J - 1;
								break;
							}
						}
						else {
							sTmp.push_back( _sString[J] );
						}
					}
				}
			}
			return false;
		}

		/**
		 * Gets a list of input devices.
		 *
		 * \return Returns a vector of input devices.
		 */
		static std::vector<RAWINPUTDEVICELIST>
											GetRawInputDeviceList();

		/**
		 * Gets an input device's name.
		 *
		 * \param _hHandle The handle to the device.
		 * \param _wsIdentString Contains the identifier string for the device.
		 * \return Returns a wstring containing the name of the device.
		 */
		static std::wstring					GetRawInputDeviceName( HANDLE _hHandle, std::wstring &_wsIdentString );

		/**
		 * Gets an input device's information.
		 *
		 * \param _hHandle The handle to the device.
		 * \return Returns the device's information.
		 */
		static RID_DEVICE_INFO				GetRawInputDeviceInformation( HANDLE _hHandle );

		/**
		 * Gathers all of the raw nput devices of a given type into an array.
		 *
		 * \param _dwType The type of device to gather.
		 * \return Returns a vector containing the raw input devices of the given type.
		 */
		static std::vector<LSW_RAW_INPUT_DEVICE_LIST>
											GatherRawInputDevices( DWORD _dwType );

		/**
		 * Gets the desktop space minus the taskbar.
		 *
		 * \return RETURN
		 */
		static LSW_RECT						UsableDesktopRect() {
			LSW_RECT rDesktop;
			rDesktop.Zero();
			if ( ::SystemParametersInfoW( SPI_GETWORKAREA, 0, &rDesktop, 0 ) ) {
				/*LSW_RECT rTmp;
				if ( ::GetWindowRect( ::GetDesktopWindow(), &rTmp ) ) {
					return rDesktop;
				}*/
			}
			return rDesktop;
		}


	protected :
		// == Types.
#ifdef _DEBUG
		struct LSW_MESSAGE {
			uint32_t						uiId;
			const char *					pcName;
		};
#endif	// #ifdef _DEBUG


		// == Members.
#ifdef _DEBUG
		static LSW_MESSAGE					m_mMessages[];
#endif	// #ifdef _DEBUG
	};

}	// namespace lsw
