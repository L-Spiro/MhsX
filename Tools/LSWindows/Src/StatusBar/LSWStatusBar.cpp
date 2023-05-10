#include "LSWStatusBar.h"
#include <strsafe.h>

namespace lsw {

	CStatusBar::CStatusBar( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
	}

	// == Functions.
	// Add parts.
	BOOL CStatusBar::SetParts( const std::vector<LSW_STATUS_PART> &_vParts ) {
		if ( !_vParts.size() ) { return TRUE; }
		return SetParts( &_vParts[0], _vParts.size() );
	}

	// Add parts.
	BOOL CStatusBar::SetParts( const LSW_STATUS_PART * _pspParts, size_t _sTotal ) {
		m_vParts.clear();
		m_vParts.reserve( _sTotal );
		std::vector<INT> vInts;
		vInts.reserve( _sTotal );
		for ( size_t I = 0; I < _sTotal; ++I ) {
			vInts.push_back( _pspParts[I].iRightCoord );
			m_vParts.push_back( _pspParts[I] );
		}
		if ( !vInts.size() ) { return TRUE; }
		return static_cast<BOOL>(::SendMessageW( Wnd(), SB_SETPARTS, static_cast<WPARAM>(vInts.size()), reinterpret_cast<LPARAM>(&vInts[0]) ));
	}

	// Retrieves the text from the specified part of a status window.
	//	Returns a 32-bit value that consists of two 16-bit values. The low word specifies the length, in characters, of the text. The high word specifies the type of operation used to draw the text.
	DWORD CStatusBar::GetTextA( INT _iIdx, LPSTR _lpString, INT _nMaxCount ) const {
		DWORD dwLen = GetTextLengthA( _iIdx ) + 1;
		if ( static_cast<INT>(dwLen) <= _nMaxCount ) {
			return static_cast<DWORD>(::SendMessageA( Wnd(), SB_GETTEXTA, static_cast<WPARAM>(_iIdx), reinterpret_cast<LPARAM>(_lpString) ));
		}
		CHAR * pcTemp = new( std::nothrow ) CHAR[dwLen];
		DWORD dwRet = static_cast<DWORD>(::SendMessageA( Wnd(), SB_GETTEXTA, static_cast<WPARAM>(_iIdx), reinterpret_cast<LPARAM>(pcTemp) ));
		::StringCbCopyNA( _lpString, _nMaxCount * sizeof( _lpString[0] ), pcTemp, dwLen * sizeof( pcTemp[0] ) );
		delete [] pcTemp;
		return dwRet;
	}

	// Retrieves the text from the specified part of a status window.
	//	Returns a 32-bit value that consists of two 16-bit values. The low word specifies the length, in characters, of the text. The high word specifies the type of operation used to draw the text.
	DWORD CStatusBar::GetTextW( INT _iIdx, LPWSTR _lpString, INT _nMaxCount ) const {
		DWORD dwLen = GetTextLengthA( _iIdx ) + 1;
		if ( static_cast<INT>(dwLen) <= _nMaxCount ) {
			return static_cast<DWORD>(::SendMessageW( Wnd(), SB_GETTEXTW, static_cast<WPARAM>(_iIdx), reinterpret_cast<LPARAM>(_lpString) ));
		}
		WCHAR * pwcTemp = new( std::nothrow ) WCHAR[dwLen];
		DWORD dwRet = static_cast<DWORD>(::SendMessageW( Wnd(), SB_GETTEXTW, static_cast<WPARAM>(_iIdx), reinterpret_cast<LPARAM>(pwcTemp) ));
		::StringCbCopyNW( _lpString, _nMaxCount * sizeof( _lpString[0] ), pwcTemp, dwLen * sizeof( pwcTemp[0] ) );
		delete [] pwcTemp;
		return dwRet;
	}

	// Retrieves the text from the specified part of a status window.
	std::string CStatusBar::GetTextA( INT _iIdx ) const {
		DWORD dwLen = GetTextLengthA( _iIdx ) + 1;
		CHAR * pcTemp = new( std::nothrow ) CHAR[dwLen];
		::SendMessageA( Wnd(), SB_GETTEXTA, static_cast<WPARAM>(_iIdx), reinterpret_cast<LPARAM>(pcTemp) );
		std::string sRet = pcTemp;
		delete [] pcTemp;
		return sRet;
	}

	// Retrieves the text from the specified part of a status window.
	std::wstring CStatusBar::GetTextW( INT _iIdx ) const {
		DWORD dwLen = GetTextLengthA( _iIdx ) + 1;
		WCHAR * pwcTemp = new( std::nothrow ) WCHAR[dwLen];
		::SendMessageW( Wnd(), SB_GETTEXTW, static_cast<WPARAM>(_iIdx), reinterpret_cast<LPARAM>(pwcTemp) );
		std::wstring sRet = pwcTemp;
		delete [] pwcTemp;
		return sRet;
	}

	// Gets the length of the text in the gicen part of the status bar.
	INT CStatusBar::GetTextLengthA( INT _iIdx ) const {
		if ( !Wnd() ) { return 0; }
		return LOWORD( static_cast<DWORD>(::SendMessageA( Wnd(), SB_GETTEXTLENGTHA, static_cast<WPARAM>(_iIdx), 0L )) );
	}

	// Gets the length of the text in the gicen part of the status bar.
	INT CStatusBar::GetTextLengthW( INT _iIdx ) const {
		if ( !Wnd() ) { return 0; }
		return LOWORD( static_cast<DWORD>(::SendMessageW( Wnd(), SB_GETTEXTLENGTHW, static_cast<WPARAM>(_iIdx), 0L )) );
	}

	// Sets the text in the specified part of a status window.
	BOOL CStatusBar::SetTextA( BYTE _iIdx, BYTE _bDrawingOp, LPCSTR _lpString ) {
		if ( !Wnd() ) { return FALSE; }
		return static_cast<BOOL>(::SendMessageA( Wnd(), SB_SETTEXTA, static_cast<WPARAM>(MAKEWORD( _iIdx, _bDrawingOp )), reinterpret_cast<LPARAM>(_lpString) ));
	}

	// Sets the text in the specified part of a status window.
	BOOL CStatusBar::SetTextW( BYTE _iIdx, BYTE _bDrawingOp, LPCWSTR _lpString ) {
		if ( !Wnd() ) { return FALSE; }
		return static_cast<BOOL>(::SendMessageW( Wnd(), SB_SETTEXTW, static_cast<WPARAM>(MAKEWORD( _iIdx, _bDrawingOp )), reinterpret_cast<LPARAM>(_lpString) ));
	}

	// Evaluates expressions to determine a new rectangle for the control.
	void CStatusBar::EvalNewSize() {
		::SendMessageW( Wnd(), WM_SIZE, 0L, 0L );
		/*::GetWindowRect( Wnd(), &m_rRect );
		::GetClientRect( Wnd(), &m_rClientRect );*/

		if ( m_pwParent && m_vParts.size() ) {
			LONG lChange = m_pwParent->WindowRect( this ).Width() - m_pwParent->StartRect().Width();
			std::vector<INT> vInts;
			vInts.reserve( m_vParts.size() );
			for ( size_t I = 0; I < m_vParts.size(); ++I ) {
				INT iPos = m_vParts[I].bLockToRight ? (m_vParts[I].iRightCoord + lChange) : m_vParts[I].iRightCoord;
				vInts.push_back( iPos );
			}
			::SendMessageW( Wnd(), SB_SETPARTS, static_cast<WPARAM>(vInts.size()), reinterpret_cast<LPARAM>(&vInts[0]) );
		}
	}

}	// namespace lsw
