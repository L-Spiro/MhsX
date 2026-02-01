#pragma once

#include "../LSWWin.h"
#include "../Base/LSWBase.h"
#include "../Layout/LSWWidgetLayout.h"

#include <EEExpEval.h>

#include <algorithm>
#define _USE_MATH_DEFINES
#include <cmath>
#include <cstring>
#include <string>
#include <Uxtheme.h>
#include <vector>

#define LSW_USB_INPUTS

#ifdef LSW_USB_INPUTS
#include <hidsdi.h>
#endif	// #ifdef LSW_USB_INPUTS

namespace lsw {

	class CTabPageBase;
	class CWidget;

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
		LSW_RECT &							Zero() { left = right = top = bottom = 0; return (*this); }
		LSW_RECT &							PrepareConsume() { left = top = LONG_MAX; right = bottom = LONG_MIN; return (*this); }
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
		// Consumes the given rectangle (this rectangle is adjusted to fit the given rectangle inside it).
		void								Consume( const LSW_RECT &_rRect ) {
			left = std::min( left, _rRect.left );
			top = std::min( top, _rRect.top );
			right = std::max( right, _rRect.right );
			bottom = std::max( bottom, _rRect.bottom );
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


		// == Functions.
		VOID								Reset() {
			if ( Valid() ) {
				::CloseHandle( hHandle );
				hHandle = NULL;
			}
		}

		inline BOOL							Valid() const { return hHandle && hHandle != INVALID_HANDLE_VALUE; }

		static inline  BOOL					Valid( HANDLE _hHandle ) { return _hHandle && _hHandle != INVALID_HANDLE_VALUE; }


		// == Members.
		HANDLE								hHandle;
	};

	struct LSW_FONT {
		LSW_FONT() : hFont( NULL ), bDeleteIt( false ) {}
		LSW_FONT( HFONT _hFont ) : hFont( _hFont ), bDeleteIt( false ) {}
		~LSW_FONT() {
			Reset();
		}

		LSW_FONT &							operator = ( HFONT _hFont ) {
			Reset();
			hFont = _hFont;
			bDeleteIt = false;
			return (*this);
		}


		// == Functions.
		HFONT								CreateFontIndirectW( const LOGFONTW * _lplfFont ) {
			Reset();
			hFont = ::CreateFontIndirectW( _lplfFont );
			bDeleteIt = true;
			return hFont;
		}

		VOID								Reset() {
			if ( Valid() && bDeleteIt ) {
				::DeleteObject( hFont );
			}
			bDeleteIt = false;
			hFont = NULL;
		}

		inline BOOL							Valid() const { return Valid( hFont ); }

		static inline  BOOL					Valid( HFONT _hFont ) { return _hFont != NULL; }


		// == Members.
		HFONT								hFont = NULL;
		bool								bDeleteIt = false;
	};

	struct LSW_HPEN {
		LSW_HPEN( int _iStyle, int _cWidth, COLORREF _cColor ) : hPen( ::CreatePen( _iStyle, _cWidth, _cColor & RGB( 0xFF, 0xFF, 0xFF ) ) ) {}
		~LSW_HPEN() {
			Reset();
		}

		LSW_HPEN &							operator = ( HPEN &_hFont ) {
			Reset();
			hPen = _hFont;
			_hFont = NULL;
			return (*this);
		}


		// == Functions.
		HPEN								CreatePen( int _iStyle, int _cWidth, COLORREF _cColor ) {
			Reset();
			hPen = ::CreatePen( _iStyle, _cWidth, _cColor & RGB( 0xFF, 0xFF, 0xFF ) );
			return hPen;
		}

		VOID								Reset() {
			if ( Valid() ) {
				::DeleteObject( hPen );
				hPen = NULL;
			}
		}

		inline BOOL							Valid() const { return Valid( hPen ); }

		static inline  BOOL					Valid( HPEN _hPen ) { return _hPen != NULL; }


		// == Members.
		HPEN								hPen = NULL;
	};

	struct LSW_HMODULE {
		LSW_HMODULE() : hHandle( NULL ) {}
		LSW_HMODULE( LPCSTR _sPath ) :
			hHandle( ::LoadLibraryW( ee::CExpEval::StringToWString( _sPath ).c_str() ) ) {
		}
		LSW_HMODULE( LPCWSTR _wsPath ) :
			hHandle( ::LoadLibraryW( _wsPath ) ) {
		}
		LSW_HMODULE( const char16_t * _pu16Path ) :
			hHandle( ::LoadLibraryW( reinterpret_cast<LPCWSTR>(_pu16Path) ) ) {
		}
		~LSW_HMODULE() {
			Reset();
		}


		// == Operators.
		LSW_HMODULE &						operator = ( LSW_HMODULE && _hOther ) {
			if ( this != &_hOther ) {
				Reset();
				hHandle = _hOther.hHandle;
				_hOther.hHandle = NULL;
			}
			return (*this);
		}


		// == Functions.
		BOOL								LoadLibrary( LPCSTR _sPath ) {
			Reset();
			hHandle = ::LoadLibraryW( ee::CExpEval::StringToWString( _sPath ).c_str() );
			return hHandle != NULL;
		}

		BOOL								LoadLibrary( LPCWSTR _wsPath ) {
			Reset();
			hHandle = ::LoadLibraryW( _wsPath );
			return hHandle != NULL;
		}

		BOOL								LoadLibrary( const char16_t * _pu16Path ) {
			Reset();
			hHandle = ::LoadLibraryW( reinterpret_cast<LPCWSTR>(_pu16Path) );
			return hHandle != NULL;
		}

		inline VOID							Reset() {
			if ( Valid() ) {
				::FreeLibrary( hHandle );
				hHandle = NULL;
			}
		}

		inline BOOL							Valid() const { return hHandle != NULL; }


		// == Members.
		HMODULE								hHandle;
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
		LSW_SELECTOBJECT( HDC _hDc, HGDIOBJ _hgdioObj, bool _bDeleteNewObjAfter = false ) :
			hDc( _hDc ),
			hCur( _hgdioObj ),
			hPrev( NULL ),
			bDeleteAfter( _bDeleteNewObjAfter ) {

			if ( _hgdioObj ) {
				hPrev = ::SelectObject( _hDc, _hgdioObj );
			}
		}
		LSW_SELECTOBJECT( HDC _hDc, HGDIOBJ _hgdioObj, bool _bDeleteNewObjAfter, bool _bConditional ) :
			hDc( _bConditional ? _hDc : NULL ),
			hCur( _bConditional ? _hgdioObj : NULL ),
			hPrev( NULL ),
			bDeleteAfter( _bConditional ? _bDeleteNewObjAfter : false ) {

			if ( _hgdioObj && _bConditional ) {
				hPrev = ::SelectObject( _hDc, _hgdioObj );
			}
		}
		~LSW_SELECTOBJECT() {
			if ( hPrev ) {
				::SelectObject( hDc, hPrev );
			}
			if ( bDeleteAfter && NULL != hCur ) {
				::DeleteObject( hCur );
			}
		}

		HDC									hDc;
		HGDIOBJ								hCur;
		HGDIOBJ								hPrev;
		bool								bDeleteAfter;
	};

	struct LSW_HDC {
		LSW_HDC( HWND _hWnd ) :
			hWnd( _hWnd ),
			hDc( ::GetDC( _hWnd ) ) {
		}
		~LSW_HDC() {
			if ( hDc ) { ::ReleaseDC( hWnd, hDc ); }
		}

		HWND								hWnd;
		HDC									hDc;
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

	/** Message-only window. */
	struct LSW_MESSAGE_WINDOW {
		LSW_MESSAGE_WINDOW() {}
		~LSW_MESSAGE_WINDOW() {
			Destroy();
		}


		// == Functions.
		/**
		 * Creates the message-only window.
		 * 
		 * \return Returns the handle to the created window.
		 **/
		HWND								Create() {
			Destroy();

			hWnd = ::CreateWindowExW(
				0, L"STATIC", nullptr, 0,
				0, 0, 0, 0,
				HWND_MESSAGE, nullptr,
				::GetModuleHandleW( nullptr ), nullptr );
			return hWnd;
		}

		/**
		 * Destroys the message-only window.
		 **/
		void								Destroy() {
			if ( hWnd ) {
				::DestroyWindow( hWnd );
				hWnd = NULL;
			}
		}

	private :
		// == Members.
		HWND								hWnd = NULL;							/**< The handle to the the window. */
	};

	/** A wrapper around ::GlobalLock()/::GlobalUnlock() for Windows. */
	struct LSW_GLOBALLOCK {
		LSW_GLOBALLOCK( HGLOBAL _gLockMe ) :
			gLockMe( _gLockMe ) {
			if ( gLockMe ) {
				lpvData = ::GlobalLock( gLockMe );
			}
		}
		~LSW_GLOBALLOCK() {
			Unlock();
		}


		// == Functions.
		/**
		 * Gets a pointer to the memory pointer returned by ::GlobalLock().
		 * 
		 * \return Returns the pointer to the memory pointer returned by ::GlobalLock().
		 **/
		inline LPVOID						Data() { return lpvData; }

		/**
		 * Unlocks the memory.
		 **/
		inline void							Unlock() {
			if ( gLockMe && lpvData ) {
				::GlobalUnlock( gLockMe );
				gLockMe = NULL;
				lpvData = NULL;
			}
		}

	private :
		// == Members.
		HGLOBAL								gLockMe = NULL;							/**< The handle we manage. */
		LPVOID								lpvData = NULL;							/**< The pointer to the data. */
	};

	/** A wrapper around cliboard access for Windows. */
	struct LSW_CLIPBOARD {
		LSW_CLIPBOARD( HWND _hWnd = NULL, bool _bEmpty = true ) {
			if ( NULL == _hWnd ) {
				_hWnd = mwWindow.Create();
			}
			bOpen = ::OpenClipboard( _hWnd ) != FALSE;
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
		/**
		 * Gets data of a given format.  Call within a try/catch block.  An exception indicates a memory-allocation failure when resizing _vData.
		 * 
		 * \param	_uiFormat The format of the data to attempt to retreive from the clipboard.
		 * \param	_vData Holds the returned vector of data.
		 * \return	Returns true if data of the given format was found in the clipboard.
		 * \throws	std::bad_alloc on allocation error.
		 **/
		template <typename _tType = std::vector<uint8_t>>
		bool								GetData( UINT _uiFormat, _tType &_vData ) {
			static_assert( sizeof( _tType::value_type ) == 1, "Compile-time assertion failed: sizeof( _tType::value_type ) must equal 1." );
			if ( !bOpen ) { return false; }
			if ( !::IsClipboardFormatAvailable( _uiFormat ) ) { return false; }

			HGLOBAL hMem = ::GetClipboardData( _uiFormat );
			if ( hMem ) {
				_vData.resize( ::GlobalSize( hMem ) );
				LSW_GLOBALLOCK glLock( hMem );
				if ( !glLock.Data() ) { return false; }
				std::memcpy( _vData.data(), glLock.Data(), _vData.size() );
				return true;
			}
			return false;
		}

		/**
		 * Sets data in a given format.  Call within a try/catch block.  An exception indicates a memory-allocation failure when allocating the global memory buffer.
		 * 
		 * \param	_uiFormat Format of the data to set.
		 * \param	_vData The actual data to set.
		 * \return	Returns true if the clipboard is open and the write succeeds.
		 * \throws	std::bad_alloc on allocation error.
		 **/
		template <typename _tType = std::vector<uint8_t>>
		bool								SetData( UINT _uiFormat, const _tType &_vData ) {
			if ( !bOpen ) { return false; }

			HGLOBAL hglbBinCopy;
			hglbBinCopy = ::GlobalAlloc( GMEM_MOVEABLE, _vData.size() );
			if ( !hglbBinCopy ) { throw std::bad_alloc(); }
			{
				LSW_GLOBALLOCK glLock( hglbBinCopy );
				if ( !glLock.Data() ) {
					glLock.Unlock();
					::GlobalFree( hglbBinCopy );
					return false;
				}
				std::memcpy( glLock.Data(), _vData.data(), _vData.size() );
			}
			if ( NULL == ::SetClipboardData( _uiFormat, hglbBinCopy ) ) {
				// It wasn't transferred.
				::GlobalFree( hglbBinCopy );
				return false;
			}
			return true;
		}

		/**
		 * \brief Advertises a clipboard format for delayed rendering.
		 *
		 * Calls ::SetClipboardData( _uiFormat, NULL ) to register the format without providing data yet.
		 * You must later supply the data when WM_RENDERFORMAT / WM_RENDERALLFORMATS is received.
		 *
		 * \param _uiFormat The clipboard format to register.
		 * \return Returns true if the format was registered; false otherwise.
		 *
		 * \note The clipboard must be open and owned (typically after ::EmptyClipboard()).
		 */
		bool								SetDeferredData( UINT _uiFormat ) {
			return NULL != ::SetClipboardData( _uiFormat, NULL );
		}

		/**
		 * Sets the clipboard text in 8-bit characters.  Can be data rather than a NULL-terminated text string, but a NULL character is appended to the end.
		 * 
		 * \param _pcText The data/string to copy to the clipboard.
		 * \param _stLen The number of bytes to which _pcText points.
		 * \return Returns TRUE if the clipboard is opened and memory could be allocated and locked.
		 **/
		bool								SetText( const char * _pcText, size_t _stLen = -1 ) {
			if ( !_pcText || !bOpen ) {
				::OutputDebugStringA( "Failed to open clipboard.\r\n" );
				CBase::PrintError( L"Failed to open clipboard: " );
				return false;
			}
			if ( static_cast<int32_t>(_stLen) < 0 ) {
				_stLen = std::strlen( _pcText );
			}
			HGLOBAL hglbBinCopy;
			hglbBinCopy = ::GlobalAlloc( GMEM_MOVEABLE, (_stLen + 1) * sizeof( _pcText[0] ) );
			if ( !hglbBinCopy ) { return false; }
			{
				LSW_GLOBALLOCK glLock( hglbBinCopy );
				if ( !glLock.Data() ) {
					::GlobalFree( hglbBinCopy );
					return false;
				}
				std::memcpy( glLock.Data(), _pcText, _stLen * sizeof( _pcText[0] ) );
				char * pcText = static_cast<char *>(glLock.Data());
				pcText[_stLen] = L'\0';
			}

			if ( NULL == ::SetClipboardData( CF_TEXT, hglbBinCopy ) ) {
				// It wasn't transferred.
				::GlobalFree( hglbBinCopy );
				return false;
			}
			return true;
		}

		/**
		 * Sets the clipboard text in 16-bit characters.  Can be data rather than a NULL-terminated text string, but a NULL character is appended to the end.
		 * 
		 * \param _pwcText The data/string to copy to the clipboard.
		 * \param _stLen The number of wchar_t characters to which _pwcText points.
		 * \return Returns TRUE if the clipboard is opened and memory could be allocated and locked.
		 **/
		bool								SetText( const wchar_t * _pwcText, size_t _stLen = -1 ) {
			if ( !_pwcText || !bOpen ) {
				::OutputDebugStringA( "Failed to open clipboard.\r\n" );
				CBase::PrintError( L"Failed to open clipboard: " );
				return false;
			}
			if ( static_cast<int32_t>(_stLen) < 0 ) {
				_stLen = std::wcslen( _pwcText );
			}
			HGLOBAL hglbBinCopy;
			hglbBinCopy = ::GlobalAlloc( GMEM_MOVEABLE, (_stLen + 1) * sizeof( _pwcText[0] ) );
			if ( !hglbBinCopy ) { return false; }
			{
				LSW_GLOBALLOCK glLock( hglbBinCopy );
				if ( !glLock.Data() ) {
					::GlobalFree( hglbBinCopy );
					::OutputDebugStringA( "Failed to allocate.\r\n" );
					return false;
				}
				std::memcpy( glLock.Data(), _pwcText, _stLen * sizeof( _pwcText[0] ) );
				wchar_t * pwcText = static_cast<wchar_t *>(glLock.Data());
				pwcText[_stLen] = L'\0';
				::GlobalUnlock( hglbBinCopy );
			}

			::EmptyClipboard();
			if ( NULL == ::SetClipboardData( CF_UNICODETEXT, hglbBinCopy ) ) {
				// It wasn't transferred.
				::GlobalFree( hglbBinCopy );
				return false;
			}
			return true;
		}


	protected :
		LSW_MESSAGE_WINDOW					mwWindow;								/**< Message window if no winder was provided when opening the clipboard. */
		bool								bOpen = false;							/**< Is the clipboard open? */
		bool								bEmptied = false;						/**< Has the clipboard been emptied? */
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

	struct LSW_KEY {
		DWORD								dwScanCode;								/**< The key scancode. */
		BYTE								bKeyCode;								/**< The key virtual key code. */
		BYTE								bKeyModifier;							/**< The key modifier code. */


		// == Functions.
		/**
		 * Fills out the structure based on the WPARAM and LPARAM passed to WM_KEYDOWN.
		 * 
		 * \param _wpParm The virtual-key code of the nonsystem key.
		 * \param _lpParm The repeat count, scan code, extended-key flag, context code, previous key-state flag, and transition-state flag, as seen in LSW_KEY_FLAGS.
		 * \param _bAllowKeyCombos If true, VK_SHIFT, VK_CONTROL, and VK_MENU keys update only bKeyModifier, and subsequent calls are required to set the other values.
		 **/
		VOID								MakeFromKeyDown( WPARAM _wpParm, LPARAM _lpParm, bool _bAllowKeyCombos ) {
			bool bExtended = (_lpParm & (KF_EXTENDED << 16));
			if ( _bAllowKeyCombos ) {
				if ( static_cast<BYTE>(_wpParm) == VK_SHIFT ) {
					bKeyModifier = BYTE( ::MapVirtualKeyW( (_lpParm >> 16) & 0xFF, MAPVK_VSC_TO_VK_EX ) );
					return;
				}
				if ( static_cast<BYTE>(_wpParm) == VK_CONTROL ) {
					bKeyModifier = bExtended ? VK_RCONTROL : VK_LCONTROL;
					return;
				}
				if ( static_cast<BYTE>(_wpParm) == VK_MENU ) {
					bKeyModifier = bExtended ? VK_RMENU : VK_LMENU;
					return;
				}
			}
			bKeyCode = static_cast<BYTE>(_wpParm);
			dwScanCode = static_cast<DWORD>(_lpParm);
		}
	};

	struct LSW_THEME_DATA {
		LSW_THEME_DATA( HWND _hWnd, LPCWSTR _pszClassList ) :
			hWnd( _hWnd ),
			wsClass( _pszClassList ) {
			htTheme = ::OpenThemeData( _hWnd, _pszClassList );
		}
		LSW_THEME_DATA( HWND _hWnd, LPCWSTR _pszClassList, bool _bConditional ) :
			hWnd( _bConditional ? _hWnd : NULL ),
			wsClass( _pszClassList ) {
			htTheme = _bConditional ? ::OpenThemeData( _hWnd, _pszClassList ) : NULL;
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
			if ( !htTheme && hWnd ) {
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
		HANDLE								hDevice;								/**< A handle to the device. */
		RID_DEVICE_INFO						diInfo;									/**< The device's information. */
		std::vector<uint8_t>				vPreparsedData;							/**< The device's preparsed data.  Cast .data() to PHIDP_PREPARSED_DATA. */
#ifdef LSW_USB_INPUTS
		PHIDP_PREPARSED_DATA				pdPreparsedData;						/**< The device's preparsed data. */
#else
		void *								pdPreparsedData;
#endif	// #ifdef LSW_USB_INPUTS
	};

	struct LSW_HID_HANLE {
		LSW_HID_HANLE( const wchar_t * _pwcDeviceId ) {
			hHandle = ::CreateFileW( _pwcDeviceId,
				GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING, 0,
				NULL );
			bOpened = hHandle != NULL;
		}
		~LSW_HID_HANLE() {
			if ( hHandle ) {
				::CloseHandle( hHandle );
			}
		}

		HANDLE								hHandle;
		BOOL								bOpened;
	};

	struct LSW_HDEVNOTIFY {
		LSW_HDEVNOTIFY() :
			hNotify( NULL ) {
		};
		~LSW_HDEVNOTIFY() {
			Reset();
		}


		// == Functions.
		/**
		 * Registers the device or type of device for which a window will receive notifications.
		 * 
		 * \param _hRecipient A handle to the window or service that will receive device events for the devices specified in the NotificationFilter parameter. The same window handle can be used in multiple calls to RegisterDeviceNotification.  Services can specify either a window handle or service status handle.
		 * \param _lpvNotificationFilter A pointer to a block of data that specifies the type of device for which notifications should be sent. This block always begins with the DEV_BROADCAST_HDR structure. The data following this header is dependent on the value of the dbch_devicetype member, which can be DBT_DEVTYP_DEVICEINTERFACE or DBT_DEVTYP_HANDLE.
		 * \param _dwFlags This parameter can be DEVICE_NOTIFY_WINDOW_HANDLE or DEVICE_NOTIFY_SERVICE_HANDLE, and DEVICE_NOTIFY_ALL_INTERFACE_CLASSES.
		 * \return Returns TRUE if the call to ::RegisterDeviceNotificationW() does not return NULL.
		 **/
		BOOL								RegisterDeviceNot( HANDLE _hRecipient, LPVOID _lpvNotificationFilter, DWORD _dwFlags = DEVICE_NOTIFY_WINDOW_HANDLE ) {
			Reset();
			hNotify = ::RegisterDeviceNotificationW( _hRecipient, _lpvNotificationFilter, _dwFlags );
			return hNotify == NULL ? FALSE : TRUE;
		}

		/**
		 * Resets the handle.
		 **/
		void								Reset() {
			if ( NULL != hNotify ) {
				if ( ::UnregisterDeviceNotification( hNotify ) ) {
					hNotify = NULL;
				}
			}
		}


		// == Members.
		/** GUID for all USB serial host PnP drivers. */
		static const GUID					s_gUsbPnPDevices;


	protected :
		// == Members.
		HDEVNOTIFY							hNotify;
	};

	struct LSW_WINDOW_PLACEMENT {
		~LSW_WINDOW_PLACEMENT() {
			if ( NULL != hMenu ) {
				// Window was destroyed before the menu was restored, leaving the menu without an owner to destroy it.
				::DestroyMenu( hMenu );
				hMenu = NULL;
			}
		}


		// == Members.
		/** The menu, if menu-hiding is used. */
		HMENU								hMenu = NULL;
		/** The window style before going into full-screen mode. */
		LONG								lWindowStyle = 0;
		/** The window placement. */
		WINDOWPLACEMENT						wpPlacement;
		/** If true, we are in borderless mode. */
		bool								bInBorderless = false;
		/** If true, this object is performing a size operation on the given window. */
		bool								bIsSizing = false;


		// == Functions.
		/**
		 * Enters borderless mode.  The restoration information is saved to the structure.
		 * 
		 * \param _hWnd The window to affect.
		 * \return Returns true if full-screening the given window succeeds.
		 **/
		bool								EnterBorderless( HWND _hWnd, bool _bHideMenu ) {
			if ( bInBorderless ) { return true; }
			LONG lStyle = ::GetWindowLongW( _hWnd, GWL_STYLE );
			if ( !lStyle ) { return false; }

			HMONITOR hMon = ::MonitorFromWindow( _hWnd, MONITOR_DEFAULTTOPRIMARY );
			if ( NULL == hMon ) { return false; }
			MONITORINFO miMonInfo = { sizeof( miMonInfo ) };
			if ( ::GetMonitorInfoW( hMon, &miMonInfo ) && ::GetWindowPlacement( _hWnd, &wpPlacement ) ) {
				bIsSizing = true;
				if ( _bHideMenu && hMenu == NULL ) {
					hMenu = ::GetMenu( _hWnd );
					::SetMenu( _hWnd, NULL );
				}
				::SetWindowLongW( _hWnd, GWL_STYLE,
					(lStyle & ~WS_OVERLAPPEDWINDOW) | WS_POPUP );
				::SetWindowPos( _hWnd, HWND_TOP,
					miMonInfo.rcMonitor.left, miMonInfo.rcMonitor.top,
					miMonInfo.rcMonitor.right - miMonInfo.rcMonitor.left,
					miMonInfo.rcMonitor.bottom - miMonInfo.rcMonitor.top,
					SWP_NOOWNERZORDER | SWP_FRAMECHANGED );
				bIsSizing = false;
				lWindowStyle = lStyle;
				bInBorderless = true;
				while ( ::ShowCursor( FALSE ) >= 0 ) {}
				return true;
			}
			return false;
		}

		/**
		 * Leaves borderless and returns to normal.
		 * 
		 * \param _hWnd The window to affect.
		 * \return Returns true if the window was returned to normal.
		 **/
		bool								LeaveBorderless( HWND _hWnd ) {
			if ( !bInBorderless ) { return true; }
			bInBorderless = false;
			bIsSizing = true;
			if ( hMenu != NULL ) {
				::SetMenu( _hWnd, hMenu );
				hMenu = NULL;
			}
			if ( !::SetWindowLongW( _hWnd, GWL_STYLE,
                  lWindowStyle | WS_OVERLAPPEDWINDOW ) ) {
				bInBorderless = true;
				bIsSizing = false;
				return false;
			}
			if ( !::SetWindowPlacement( _hWnd, &wpPlacement ) ) { bInBorderless = true; bIsSizing = false; return false; }
			
			if ( !::SetWindowPos( _hWnd, NULL,
				wpPlacement.rcNormalPosition.left, wpPlacement.rcNormalPosition.top,
				wpPlacement.rcNormalPosition.right - wpPlacement.rcNormalPosition.left,
				wpPlacement.rcNormalPosition.bottom - wpPlacement.rcNormalPosition.top,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
				SWP_NOOWNERZORDER | SWP_FRAMECHANGED ) ) {
				bIsSizing = false;
				bInBorderless = true;
				return false;
			}
			::ShowWindow( _hWnd, SW_NORMAL );
			while ( ::ShowCursor( TRUE ) < 0 ) {}
			bIsSizing = false;
			bInBorderless = false;
			return true;
		}
	};

	struct LSW_FILE_MAP_VIEW {
		LSW_FILE_MAP_VIEW( HANDLE _hFileMappingObject, uint64_t _ui64FileOffset, size_t _dwNumberOfBytesToMap, DWORD _dwDesiredAccess ) :
			pvBuffer( ::MapViewOfFile( _hFileMappingObject, _dwDesiredAccess, DWORD( _ui64FileOffset >> 32 ), DWORD( _ui64FileOffset ), _dwNumberOfBytesToMap ) ) {
		}
		~LSW_FILE_MAP_VIEW() {
			if ( pvBuffer != NULL ) {
				::UnmapViewOfFile( pvBuffer );
			}
		}


		// == Members.
		/** The mapped address for access to the mapped region. */
		LPVOID								pvBuffer;
	};

	struct LSW_TIMER {
		LSW_TIMER( HWND _hWnd, UINT_PTR _nIDEvent, UINT _uElapse, TIMERPROC _lpTimerFunc ) :
			hWnd( _hWnd ),
			uiptrEvent( _nIDEvent ),
			uiTime( _uElapse ),
			lpTimerFunc( _lpTimerFunc ) {
			uiptrActiveId = ::SetTimer( _hWnd, _nIDEvent, _uElapse, _lpTimerFunc );
		}
		LSW_TIMER() {}
		~LSW_TIMER() {
			Stop();
		}


		// == Functions.
		/**
		 * Starts the timer.
		 * 
		 * \param _hWnd A handle to the window to be associated with the timer. This window must be owned by the calling thread. If a NULL value for _hWnd is passed in along with an _nIDEvent of an existing timer, that timer will be replaced in the same way that an existing non-NULL _hWnd timer will be.
		 * \param _nIDEvent A nonzero timer identifier. If the _hWnd parameter is NULL, and the _nIDEvent does not match an existing timer then it is ignored and a new timer ID is generated. If the _hWnd parameter is not NULL and the window specified by _hWnd already has a timer with the value _nIDEvent, then the existing timer is replaced by the new timer. When Start replaces a timer, the timer is reset. Therefore, a message will be sent after the current time-out value elapses, but the previously set time-out value is ignored. If the call is not intended to replace an existing timer, _nIDEvent should be 0 if the _hWnd is NULL.
		 * \param _uElapse The time-out value, in milliseconds.
		 *	If _uElapse is less than USER_TIMER_MINIMUM (0x0000000A), the timeout is set to USER_TIMER_MINIMUM. If _uElapse is greater than USER_TIMER_MAXIMUM (0x7FFFFFFF), the timeout is set to USER_TIMER_MAXIMUM
		 * \param _lpTimerFunc A pointer to the function to be notified when the time-out value elapses. If _lpTimerFunc is NULL, the system posts a WM_TIMER message to the application queue. The hwnd member of the message's MSG structure contains the value of the hWnd parameter.
		 * \return Returns the ID of the created timer.
		 **/
		UINT_PTR							Start( HWND _hWnd, UINT_PTR _nIDEvent, UINT _uElapse, TIMERPROC _lpTimerFunc ) {
			Stop();
			hWnd = _hWnd;
			uiptrEvent = _nIDEvent;
			uiTime = _uElapse;
			lpTimerFunc = _lpTimerFunc;
			uiptrActiveId = ::SetTimer( hWnd, uiptrEvent, uiTime, lpTimerFunc );
			return uiptrActiveId;
		}

		/**
		 * Restarts an existing timer.
		 * 
		 * \param _uElapse The new duration.
		 * \return Returns the ID of the created timer.
		 **/
		UINT_PTR							ReStart( UINT _uElapse ) {
			uiTime = _uElapse;
			uiptrActiveId = ::SetTimer( hWnd, uiptrEvent, uiTime, lpTimerFunc );
			return uiptrActiveId;
		}

		/**
		 * Stops the timer.
		 * 
		 * \return If the function succeeds, the return value is nonzero. If the function fails, the return value is zero. To get extended error information, call GetLastError.
		 **/
		BOOL								Stop() {
			if ( uiptrActiveId ) {
				if ( ::KillTimer( hWnd, uiptrActiveId ) ) {
					uiptrActiveId = 0;
					return TRUE;
				}
				return FALSE;
			}
			return TRUE;
		}


		// == Members.
		/** The associated window. */
		HWND								hWnd = NULL;
		/** The ID. */
		UINT_PTR							uiptrEvent = 0;
		/** The time. */
		UINT								uiTime = 0;
		/** The timer function. */
		TIMERPROC							lpTimerFunc = nullptr;
		/** Active ID. */
		UINT_PTR							uiptrActiveId = 0;
	};

	class CHelpers {
	public :
		// == Enumerations.
		/** Mask for DrawRectOutlineMasked(). */
		enum LSW_RECT_SIDE {
			LSW_RS_TOP		= 1U << 0,
			LSW_RS_LEFT		= 1U << 1,
			LSW_RS_BOTTOM	= 1U << 2,
			LSW_RS_RIGHT	= 1U << 3,
		};


		// == Types.
		/** Location of a found menu item. */
		struct LSW_MENU_LOC {
			HMENU							hMenu;					/**< The HMENU that directly owns the item. */
			UINT							uPos;					/*< The zero-based position within hMenu. */
		};

		// == Functions.
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
		 * \return Returns the mixed value.
		 */
		static BYTE							Mix( BYTE _bA, BYTE _bB, double _dAmnt ) {
			return static_cast<BYTE>(std::round( (_bB - _bA) * _dAmnt + _bA ));
		}

		/**
		 * Interpolates between 2 values.
		 *
		 * \param _dA Operand 1.
		 * \param _dB Operand 2.
		 * \param _dAmnt The amount to interpolate between the operands.
		 * \return Returns the mixed value.
		 */
		static double						Mix( double _dA, double _dB, double _dAmnt ) {
			return (_dB - _dA) * _dAmnt + _dA;
		}

		/**
		 * Interpolates between 2 values.
		 *
		 * \param _fA Operand 1.
		 * \param _fB Operand 2.
		 * \param _fAmnt The amount to interpolate between the operands.
		 * \return Returns the mixed value.
		 */
		static float						Mix( float _fA, float _fB, float _fAmnt ) {
			return (_fB - _fA) * _fAmnt + _fA;
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
		 * Converts from sRGB to linear.
		 *
		 * \param _fVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 */
		static float						sRGBtoLinear( float _fVal ) {
			return _fVal <= 0.04045f ?
				_fVal * (1.0f / 12.92f) :
				std::pow( (_fVal + 0.055f) * (1.0f / 1.055f), 2.4f );
		}

		/**
		 * Converts from linear to sRGB.
		 *
		 * \param _fVal The value to convert.
		 * \return Returns the value converted to sRGB space.
		 */
		static float						LinearTosRGB( float _fVal ) {
			return _fVal <= 0.0031308f ?
				_fVal * 12.92f :
				1.055f * std::pow( _fVal, 1.0f / 2.4f ) - 0.055f;
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
		 * Mixes between 2 24-bit RGB values.
		 *
		 * \param _bRedA Operand 1 (red).
		 * \param _bRedB Operand 2 (red).
		 * \param _bGreenA Operand 1 (green).
		 * \param _bGreenB Operand 2 (green).
		 * \param _bBlueA Operand 1 (blue).
		 * \param _bBlueB Operand 2 (blue).
		 * \param _fAmnt The amount to interpolate between the operands.
		 * \return Returns the interpolated 24-bit RGB value.
		 */
		static DWORD						MixColorRef( BYTE _bRedA, BYTE _bRedB,
			BYTE _bGreenA, BYTE _bGreenB,
			BYTE _bBlueA, BYTE _bBlueB, float _fAmnt ) {
			float dA = sRGBtoLinear( _bRedA / 255.0f );
			float dB = sRGBtoLinear( _bRedB / 255.0f );
			BYTE bR = static_cast<BYTE>(std::round( LinearTosRGB( Mix( dA, dB, _fAmnt ) ) * 255.0f ));
			dA = sRGBtoLinear( _bGreenA / 255.0f );
			dB = sRGBtoLinear( _bGreenB / 255.0f );
			BYTE bG = static_cast<BYTE>(std::round( LinearTosRGB( Mix( dA, dB, _fAmnt ) ) * 255.0f ));
			dA = sRGBtoLinear( _bBlueA / 255.0f );
			dB = sRGBtoLinear( _bBlueB / 255.0f );
			BYTE bB = static_cast<BYTE>(std::round( LinearTosRGB( Mix( dA, dB, _fAmnt ) ) * 255.0f ));
			return RGB( bR, bG, bB );
		}

		/**
		 * Mixes between 2 24-bit RGB values without gamma-correction.
		 *
		 * \param _bRedA Operand 1 (red).
		 * \param _bRedB Operand 2 (red).
		 * \param _bGreenA Operand 1 (green).
		 * \param _bGreenB Operand 2 (green).
		 * \param _bBlueA Operand 1 (blue).
		 * \param _bBlueB Operand 2 (blue).
		 * \param _fAmnt The amount to interpolate between the operands.
		 * \return Returns the interpolated 24-bit RGB value.
		 */
		static DWORD						MixColorRef_NosRGB( BYTE _bRedA, BYTE _bRedB,
			BYTE _bGreenA, BYTE _bGreenB,
			BYTE _bBlueA, BYTE _bBlueB, float _fAmnt ) {
			float dA = _bRedA / 255.0f;
			float dB = _bRedB / 255.0f;
			BYTE bR = static_cast<BYTE>(std::round( Mix( dA, dB, _fAmnt ) * 255.0f ));
			dA = _bGreenA / 255.0f;
			dB = _bGreenB / 255.0f;
			BYTE bG = static_cast<BYTE>(std::round( Mix( dA, dB, _fAmnt ) * 255.0f ));
			dA = _bBlueA / 255.0f;
			dB = _bBlueB / 255.0f;
			BYTE bB = static_cast<BYTE>(std::round( Mix( dA, dB, _fAmnt ) * 255.0f ));
			return RGB( bR, bG, bB );
		}

		/**
		 * \brief Chooses black or white text for best contrast against a given background color.
		 *
		 * \param _crBack Background color to evaluate.
		 * \return Returns RGB( 0, 0, 0 ) or RGB( 255, 255, 255 ) for best legibility.
		 */
		static inline COLORREF				ContrastingTextColor( COLORREF _crBack ) {
			const int iY = ( (GetRValue( _crBack ) * 299) + (GetGValue( _crBack ) * 587) + (GetBValue( _crBack ) * 114) ) / 1000;
			return iY >= 140 ? RGB( 0, 0, 0 ) : RGB( 255, 255, 255 );
		}

		/**
		 * Draw a 1-pixel outline around a rectangle according to a side mask.
		 *
		 * \brief
		 * Renders individual 1-pixel strokes for the rectangle's top, left, bottom, and/or right
		 * edges based on a 4-bit mask. Each bit enables one side: bit 0 = top, bit 1 = left,
		 * bit 2 = bottom, bit 3 = right. Uses FillRect with a solid brush for pixel-exact lines.
		 *
		 * \param _hDc The destination device context.
		 * \param _rcRect The target rectangle in client coordinates.
		 * \param _uiMask The side mask: LSW_RS_TOP(1), LSW_RS_LEFT(2), LSW_RS_BOTTOM(4), LSW_RS_RIGHT(8).
		 * \param _crColor The outline color (COLORREF).
		 */
		static void							DrawRectOutlineMasked( HDC _hDc, const RECT &_rcRect, UINT _uiMask, COLORREF _crColor ) {
			// Nothing to draw if empty.
			if ( _rcRect.right <= _rcRect.left || _rcRect.bottom <= _rcRect.top || !_uiMask ) { return; }
			

			RECT rTmp;
			auto hBrush = lsw::CBase::BrushCache().Brush( static_cast<COLORREF>((_crColor) & RGB( 0xFF, 0xFF, 0xFF )) );
			if ( !hBrush ) { return; }

			// Top: [left, top, right, top+1).
			if ( _uiMask & LSW_RS_TOP ) {
				rTmp.left = _rcRect.left;
				rTmp.top = _rcRect.top;
				rTmp.right = _rcRect.right;
				rTmp.bottom = _rcRect.top + 1;
				::FillRect( _hDc, &rTmp, hBrush );
			}

			// Left: [left, top, left+1, bottom).
			if ( _uiMask & LSW_RS_LEFT ) {
				rTmp.left = _rcRect.left;
				rTmp.top = _rcRect.top;
				rTmp.right = _rcRect.left + 1;
				rTmp.bottom = _rcRect.bottom;
				::FillRect( _hDc, &rTmp, hBrush );
			}

			// Bottom: [left, bottom-1, right, bottom).
			if ( _uiMask & LSW_RS_BOTTOM ) {
				rTmp.left = _rcRect.left;
				rTmp.top = _rcRect.bottom - 1;
				rTmp.right = _rcRect.right;
				rTmp.bottom = _rcRect.bottom;
				::FillRect( _hDc, &rTmp, hBrush );
			}

			// Right: [right-1, top, right, bottom).
			if ( _uiMask & LSW_RS_RIGHT ) {
				rTmp.left = _rcRect.right - 1;
				rTmp.top = _rcRect.top;
				rTmp.right = _rcRect.right;
				rTmp.bottom = _rcRect.bottom;
				::FillRect( _hDc, &rTmp, hBrush );
			}
		}

		/**
		 * \brief Draws a single-pixel-thick line on an HDC.
		 *
		 * Uses MoveToEx()/LineTo() with a solid pen of width 1. The final endpoint
		 * pixel is explicitly set because LineTo() does not draw the last pixel.
		 *
		 * \param _hDc Target device context.
		 * \param _i32X0 Starting X coordinate.
		 * \param _i32Y0 Starting Y coordinate.
		 * \param _i32X1 Ending X coordinate.
		 * \param _i32Y1 Ending Y coordinate.
		 * \param _crColor COLORREF of the line color.
		 */
		static inline void					DrawLineSinglePixel_Inclusive( HDC _hDc,
			int32_t _i32X0, int32_t _i32Y0,
			int32_t _i32X1, int32_t _i32Y1,
			COLORREF _crColor ) {
			if( !_hDc ) { return; }

			LSW_HPEN hPen( PS_SOLID, 1, _crColor & RGB( 0xFF, 0xFF, 0xFF ) );
			if( !hPen.hPen ) { return; }

			LSW_SELECTOBJECT soPen( _hDc, hPen.hPen );
			POINT ptOld;
			::MoveToEx( _hDc, _i32X0, _i32Y0, &ptOld );
			::LineTo( _hDc, _i32X1, _i32Y1 );

			// Explicitly set the final endpoint pixel so the line is inclusive.
			::SetPixel( _hDc, _i32X1, _i32Y1, _crColor & RGB( 0xFF, 0xFF, 0xFF ) );
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
		 * Measure a set of characters and return the maximum advance width.
		 *
		 * \param _hDc The HDC on which to measure the characters. The caller is responsible for selecting the desired font.
		 * \param _pwszChars A null-terminated string containing the characters to measure.
		 * \return Returns the maximum pixel width among all characters in the string when rendered to the given HDC.
		 */
		static int							MeasureMax( HDC _hDc, const wchar_t * _pwszChars ) {
			SIZE sThis {};
			int iMax = 0;
			for ( const wchar_t * pwcP = _pwszChars; *pwcP; ++pwcP ) {
				::GetTextExtentPoint32W( _hDc, pwcP, 1, &sThis );
				if ( sThis.cx > iMax ) { iMax = sThis.cx; }
			}
			return iMax;
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
		 * Maps a given scan code to a into a virtual-key code as text.
		 *
		 * \param _uiKey The scan code to return as a string.
		 * \return Returns the text string representing the given scan code.
		 */
		static std::wstring					ScanCodeToString( UINT _uiKey ) {
			std::wstring wsRet;
			WCHAR szBufText[64];
			if ( ::GetKeyNameTextW( _uiKey, szBufText, LSW_ELEMENTS( szBufText ) ) ) {
				wsRet += szBufText;
			}
			else {
				// Fallbacks: function keys or hex VK.
				if ( _uiKey >= VK_F1 && _uiKey <= VK_F24 ) {
					wsRet = L"F" + std::to_wstring( _uiKey - VK_F1 + 1 );
				}
				else {
					wchar_t wfb[16] = {};
					::wsprintfW( wfb, L"VK_%02X", static_cast<unsigned>(_uiKey) );
					wsRet = wfb;
				}
			}

			return wsRet;
		}

		/**
		 * Converts a modifier to text.
		 *
		 * \param _iMod The VK_ modifier to convert to a string.
		 * \param _wsResult Holds the result of the conversion.
		 * \param _bIgnoreLeftRight If true, left and right Shift, Control, and Alt are considered indistinguishable.
		 * \return Returns true if the conversion to text was successful.
		 */
		static bool							ModifierToString( INT _iMod, std::wstring &_wsResult, bool _bIgnoreLeftRight ) {
			if ( _bIgnoreLeftRight ) {
				if ( _iMod == VK_LSHIFT || _iMod == VK_RSHIFT ) {
					_iMod = VK_SHIFT;
				}
			}
			UINT uiKey = ::MapVirtualKeyW( _iMod, MAPVK_VK_TO_VSC_EX );
			UINT uiExt = (_iMod == VK_LWIN || _iMod == VK_RWIN) ? KF_EXTENDED : 0;
			if ( !_bIgnoreLeftRight ) {
				uiExt |= (_iMod == VK_RCONTROL) ? KF_EXTENDED : 0;
				uiExt |= (_iMod == VK_RMENU) ? KF_EXTENDED : 0;
			}
			_wsResult += ScanCodeToString( (uiKey | uiExt) << 16 );
			return true;
		}

		/**
		 * Creates a string from the current key configuration.
		 * 
		 * \param _kKey The key to print.
		 * \param _bIgnoreLeftRight If true, left/right variations of VK_MENU and VK_CONTROL are ignored.
		 * \return Returns a wide-character string containing the key in text.
		 **/
		static std::wstring					ToString( const LSW_KEY &_kKey, bool _bIgnoreLeftRight ) {
			std::wstring wTmp;
			if ( _kKey.bKeyModifier ) {
				CHelpers::ModifierToString( _kKey.bKeyModifier, wTmp, _bIgnoreLeftRight );
				wTmp += L"+";
			}
			wTmp += CHelpers::ScanCodeToString( UINT( _kKey.dwScanCode ) );
			return wTmp;
		}

		/**
		 * \brief Converts an ACCEL into a printable shortcut using ScanCodeToString() and ModifierToString().
		 *
		 * Handles both FVIRTKEY (VK-based) and character accelerators. Modifiers come from fVirt.
		 */
		static std::wstring					AccelToPrintable( const ACCEL &_aAccelerator, bool _bIgnoreLeftRight = true ) {
			std::wstring wsOut;

			if ( _aAccelerator.fVirt & FCONTROL ) {
				if ( !wsOut.empty() ) { wsOut += L"+"; }
				ModifierToString( VK_CONTROL, wsOut, _bIgnoreLeftRight );
			}
			if ( _aAccelerator.fVirt & FALT ) {
				if ( !wsOut.empty() ) { wsOut += L"+"; }
				ModifierToString( VK_MENU, wsOut, _bIgnoreLeftRight );
			}
			if ( _aAccelerator.fVirt & FSHIFT ) {
				if ( !wsOut.empty() ) { wsOut += L"+"; }
				ModifierToString( VK_SHIFT, wsOut, _bIgnoreLeftRight );
			}

			if ( !wsOut.empty() ) { wsOut += L"+"; }

			if ( _aAccelerator.fVirt & FVIRTKEY ) {
				UINT uiScan = ::MapVirtualKeyW( _aAccelerator.key, MAPVK_VK_TO_VSC_EX );
				UINT uiExt  = (uiScan & 0x0100) ? KF_EXTENDED : 0;
				std::wstring wsKey = ScanCodeToString( (uiScan | uiExt) << 16 );
				wsOut += wsKey;
			}
			else {
				// Character accelerator (ASCII). Show the character itself.
				wchar_t wcTmp = static_cast<wchar_t>(_aAccelerator.key);
				// Win32, locale-aware:
				::CharUpperBuffW( &wcTmp, 1 );
				wchar_t wszTmpBuffer[2] = { wcTmp, 0 };
				wsOut += wcTmp;
			}

			return wsOut;
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
		 * \brief Recursively find a menu item by command ID anywhere under _hMenu.
		 *
		 * \param _hMenu The root to search (e.g., GetMenu( hWnd )).
		 * \param _uId The command ID to find.
		 * \param _mlOut Locates the owning HMENU and position on success.
		 * \return Returns true if found.
		 */
		static bool							FindMenuItemById( HMENU _hMenu, UINT _uId, LSW_MENU_LOC &_mlOut ) {
			const int iCount = ::GetMenuItemCount( _hMenu );
			for ( int I = 0; I < iCount; ++I ) {
				const UINT uId = ::GetMenuItemID( _hMenu, I );
				if ( uId == _uId ) {
					_mlOut.hMenu = _hMenu;
					_mlOut.uPos = static_cast<UINT>(I);
					return true;
				}
				// If this position is a popup, recurse.
				if ( uId == static_cast<UINT>(-1) ) {
					if ( HMENU hSub = ::GetSubMenu( _hMenu, I ) ) {
						if ( FindMenuItemById( hSub, _uId, _mlOut ) ) { return true; }
					}
				}
			}
			return false;
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
		 * \param _sString The string in which to seek %...%.
		 * \param _stStart Holds the returned start position of the %...% section.
		 * \param _stEnd Holds the returned end position of the %...% section.
		 * \param _sReplacement Holds the replacement string, if true is returned.
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
		 * Gets an input device's preparsed data.
		 *
		 * \param _hHandle The handle to the device.
		 * \return Returns the device's preparsed data.
		 */
		static std::vector<uint8_t>			GetRawInputDevicePreparsedData( HANDLE _hHandle );

		/**
		 * Gathers all of the raw nput devices of a given type into an array.
		 *
		 * \param _dwType The type of device to gather.
		 * \return Returns a vector containing the raw input devices of the given type.
		 */
		static std::vector<LSW_RAW_INPUT_DEVICE_LIST>
											GatherRawInputDevices( DWORD _dwType );

		/**
		 * Registers raw input devices.
		 *
		 * \param _pridDevices An array of devices to register.
		 * \param _uiNumDevices The number of devices to which _uiNumDevices points.
		 * \return Returns TRUE if registration succeeded.
		 */
		static bool							RegisterRawInputDevices( const RAWINPUTDEVICE * _pridDevices, UINT _uiNumDevices );

		/**
		 * Gets the raw input data from a WM_INPUT message.
		 *
		 * \param _hRawInput The handle provided by WM_INPUT.
		 * \return Returns an array of the gathered input data.
		 */
		static std::vector<uint8_t>			GetRawInputData_Input( HRAWINPUT _hRawInput );

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

		/**
		 * Gets the desktop task-bar top.
		 * 
		 * \return Returns the top of the taskbar.
		 */
		static LONG							TaskBarTop() {
			LSW_RECT rDesktop;
			rDesktop.Zero();
			if ( ::SystemParametersInfoW( SPI_GETWORKAREA, 0, &rDesktop, 0 ) ) {
				LSW_RECT rTmp;
				if ( ::GetWindowRect( ::GetDesktopWindow(), &rTmp ) ) {
					// If docked to the top.
					if ( rTmp.top != rDesktop.top ) {
						return std::max( rTmp.top, rDesktop.top );
					}
					if ( rTmp.bottom != rDesktop.bottom ) {
						// Docked to the bottom.
						return std::min( rTmp.bottom, rDesktop.bottom );
					}
					return rTmp.top;
				}
			}
			return 0;
		}

		/**
		 * Gets the desktop task-bar bottom.
		 * 
		 * \return Returns the bottom of the taskbar.
		 */
		static LONG							TaskBarBottom() {
			LSW_RECT rDesktop;
			rDesktop.Zero();
			if ( ::SystemParametersInfoW( SPI_GETWORKAREA, 0, &rDesktop, 0 ) ) {
				LSW_RECT rTmp;
				if ( ::GetWindowRect( ::GetDesktopWindow(), &rTmp ) ) {
					// If docked to the bottom.
					if ( rTmp.bottom != rDesktop.bottom ) {
						return std::max( rTmp.bottom, rDesktop.bottom );
					}
					if ( rTmp.top != rDesktop.top ) {
						// Docked to the top.
						return std::min( rTmp.top, rDesktop.top );
					}
					return rTmp.bottom;
				}
			}
			return 0;
		}

		/**
		 * Gets the desktop task-bar left.
		 * 
		 * \return Returns the left of the taskbar.
		 */
		static LONG							TaskBarLeft() {
			LSW_RECT rDesktop;
			rDesktop.Zero();
			if ( ::SystemParametersInfoW( SPI_GETWORKAREA, 0, &rDesktop, 0 ) ) {
				LSW_RECT rTmp;
				if ( ::GetWindowRect( ::GetDesktopWindow(), &rTmp ) ) {
					// If docked to the left.
					if ( rTmp.left != rDesktop.left ) {
						return std::max( rTmp.left, rDesktop.left );
					}
					if ( rTmp.right != rDesktop.right ) {
						// Docked to the right.
						return std::min( rTmp.right, rDesktop.right );
					}
					return rTmp.left;
				}
			}
			return 0;
		}

		/**
		 * Gets the desktop task-bar right.
		 * 
		 * \return Returns the right of the taskbar.
		 */
		static LONG							TaskBarRight() {
			LSW_RECT rDesktop;
			rDesktop.Zero();
			if ( ::SystemParametersInfoW( SPI_GETWORKAREA, 0, &rDesktop, 0 ) ) {
				LSW_RECT rTmp;
				if ( ::GetWindowRect( ::GetDesktopWindow(), &rTmp ) ) {
					// If docked to the right.
					if ( rTmp.right != rDesktop.right ) {
						return std::max( rTmp.right, rDesktop.right );
					}
					if ( rTmp.left != rDesktop.left ) {
						// Docked to the left.
						return std::min( rTmp.left, rDesktop.left );
					}
					return rTmp.right;
				}
			}
			return 0;
		}

		/**
		 * Gets the taskbar rectange.
		 *
		 * \return Returns the taskbar rectangle.
		 */
		static LSW_RECT						TaskBarRect() {
			return LSW_RECT( TaskBarLeft(), TaskBarTop(), TaskBarRight(), TaskBarBottom() );
		}

		/**
		 * Takes an array of CTabPageBase's and fits them into a CTab control, additionally resizing the parent window by the same amount as the tab control is resized.  The tabs are named according to the return value of each CTabPageBase's GetName() method.
		 * 
		 * \param _pptpbPages The pages to insert into the tab control.
		 * \param _stTotal The number of pages to which to _pptpbPages points.
		 * \param _pwTab The tab control into which to insert the pages.
		 * \param _pwParent The parent to resize along with the tab control.
		 * \param _bFitWindow If TRUE, the window is resized to fit the tab control snuggly, otherwise it is reized by the same amount as the tab control, which leaves any extra space around the tab control as-is.
		 **/
		static void							FillTabAndFitWindow( CTabPageBase ** _pptpbPages, size_t _stTotal, CWidget * _pwTab, CWidget * _pwParent, bool _bFitWindow );

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
