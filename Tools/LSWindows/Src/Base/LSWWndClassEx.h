#pragma once

#include "../LSWWin.h"
#include "LSWBase.h"

namespace lsw {

	class CWndClassEx {
	public :
		CWndClassEx( WNDPROC _lpfnWndProc = nullptr,
			LPCWSTR _lpszClassName = nullptr,
			UINT _style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW,
			HINSTANCE _hInstance = CBase::GetThisHandle(),
			HICON _hIcon = nullptr,
			HCURSOR _hCursor = ::LoadCursor( NULL, IDC_ARROW ),	// Not ::LoadCursorW().
			HBRUSH _hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1),
			LPCWSTR _lpszMenuName = nullptr,
			HICON _hIconSm = nullptr,
			int _cbClsExtra = 0,
			int _cbWndExtra = 0
			) :
			m_wceWndClsEx( {
				sizeof( m_wceWndClsEx ),		// cbSize
				_style,
				_lpfnWndProc,
				_cbClsExtra,
				_cbWndExtra,
				_hInstance,
				_hIcon,
				_hCursor,
				_hbrBackground,
				_lpszMenuName,
				_lpszClassName,
				_hIconSm
			} ) {
		}


		// == Functions.
		const WNDCLASSEXW &							Obj() const { return m_wceWndClsEx; }
		// Style settings.
#define LSW_FLAG( FLG )								m_wceWndClsEx.style = (m_wceWndClsEx.style & ~FLG) | (_bVal ? FLG : 0)

		CWndClassEx &								SetFullHDrawOnResise( BOOL _bVal ) { LSW_FLAG( CS_HREDRAW ); return (*this); }
		CWndClassEx &								SetFullVDrawOnResise( BOOL _bVal ) { LSW_FLAG( CS_VREDRAW ); return (*this); }
		CWndClassEx &								SetRespondsToDblClick( BOOL _bVal ) { LSW_FLAG( CS_DBLCLKS ); return (*this); }
		CWndClassEx &								SetOwnsItsDc( BOOL _bVal ) { LSW_FLAG( CS_OWNDC ); return (*this); }
		CWndClassEx &								SetSharesDc( BOOL _bVal ) { LSW_FLAG( CS_CLASSDC ); return (*this); }
		CWndClassEx &								SetCanDrawOnParent( BOOL _bVal ) { LSW_FLAG( CS_PARENTDC ); return (*this); }
		CWndClassEx &								SetNoCloseButton( BOOL _bVal ) { LSW_FLAG( CS_NOCLOSE ); return (*this); }
		CWndClassEx &								SetUseBitmapForRedraw( BOOL _bVal ) { LSW_FLAG( CS_SAVEBITS ); return (*this); }
		CWndClassEx &								SetXByteAlignClient( BOOL _bVal ) { LSW_FLAG( CS_BYTEALIGNCLIENT ); return (*this); }
		CWndClassEx &								SetXByteAlignWindow( BOOL _bVal ) { LSW_FLAG( CS_BYTEALIGNWINDOW ); return (*this); }
		CWndClassEx &								SetGlobalClass( BOOL _bVal ) { LSW_FLAG( CS_GLOBALCLASS ); return (*this); }	// Allows DLL's to use your window type.
		CWndClassEx &								SetStyle( UINT _style ) { m_wceWndClsEx.style = _style; return (*this); }

#undef LSW_FLAG

		CWndClassEx &								SetWindPro( WNDPROC _lpfnWndProc ) { m_wceWndClsEx.lpfnWndProc = _lpfnWndProc; return (*this); }
		CWndClassEx &								SetClsExtra( int _cbClsExtra ) { m_wceWndClsEx.cbClsExtra = _cbClsExtra; return (*this); }
		CWndClassEx &								SetWndExtra( int _cbWndExtra ) { m_wceWndClsEx.cbWndExtra = _cbWndExtra; return (*this); }
		CWndClassEx &								SetInstance( HINSTANCE _hInstance ) { m_wceWndClsEx.hInstance = _hInstance; return (*this); }
		CWndClassEx &								SetIcon( HICON _hIcon ) { m_wceWndClsEx.hIcon = _hIcon; return (*this); }
		CWndClassEx &								SetCursor( HCURSOR _hCursor ) { m_wceWndClsEx.hCursor = _hCursor; return (*this); }
		CWndClassEx &								SetBackgroundBrush( HBRUSH _hbrBackground ) { m_wceWndClsEx.hbrBackground = _hbrBackground; return (*this); }
		CWndClassEx &								SetMenuName( LPCWSTR _lpszMenuName ) { m_wceWndClsEx.lpszMenuName = _lpszMenuName; return (*this); }
		CWndClassEx &								SetClassName( LPCWSTR _lpszClassName ) { m_wceWndClsEx.lpszClassName = _lpszClassName; return (*this); }
		CWndClassEx &								SetIconSmall( HICON _hIconSm ) { m_wceWndClsEx.hIconSm = _hIconSm; return (*this); }


	protected :
		// == Members.
		// The structure we wrap.
		WNDCLASSEXW									m_wceWndClsEx;
	};

}	// namespace lsw
