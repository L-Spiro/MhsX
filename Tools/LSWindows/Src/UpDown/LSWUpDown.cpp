#include "LSWUpDown.h"


namespace lsw {

	CUpDown::CUpDown( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		Parent( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
	}

	// == Functions.
	/**
	 * Setting the HWND after the control has been created.
	 * 
	 * \param _hWnd Handle to the window.
	 **/
	void CUpDown::InitControl( HWND /*_hWnd*/ ) {
	}

	/**
	 * Gets the radix base (10 or 16).
	 * 
	 * \return Returns the current radix base.
	 **/
	uint32_t CUpDown::GetBase() const {
		if ( m_hWnd == NULL ) { return 10; }
		return static_cast<uint32_t>(::SendMessageW( m_hWnd, UDM_GETBASE, 0, 0 ));
	}

	/**
	 * Gets the position.
	 * 
	 * \return Returns the current position of the up-down control.
	 **/
	int32_t CUpDown::GetPos() const {
		if ( m_hWnd == NULL ) { return 0; }
		BOOL bSuccess = FALSE;
		int32_t i32Ret = static_cast<int32_t>(::SendMessageW( m_hWnd, UDM_GETPOS32, 0, reinterpret_cast<LPARAM>(&bSuccess) ));
		if ( !bSuccess ) { return 0; }
		return i32Ret;
	}

	/**
	 * Gets the buddy control.
	 * 
	 * \return Returns a pointer to the buddy control widget.
	 **/
	CWidget * CUpDown::GetBuddy() const {
		if ( m_hWnd == NULL ) { return nullptr; }
		HWND hBuddy = reinterpret_cast<HWND>(::SendMessageW( m_hWnd, UDM_GETBUDDY, 0, 0 ));
		if ( hBuddy != NULL ) {
			return LSW_WIN2CLASS( hBuddy );
		}
		return nullptr;
	}

	/**
	 * Gets the range.
	 * 
	 * \param _i32Lower A reference to an integer that receives the lower limit of the range.
	 * \param _i32Upper A reference to an integer that receives the upper limit of the range.
	 **/
	void CUpDown::GetRange( int32_t &_i32Lower, int32_t &_i32Upper ) const {
		if ( m_hWnd == NULL ) {
			_i32Lower = _i32Upper = 0;
			return;
		}
		::SendMessageW( m_hWnd, UDM_GETRANGE32, reinterpret_cast<WPARAM>(&_i32Lower), reinterpret_cast<LPARAM>(&_i32Upper) );
	}

	/**
	 * Gets the range.
	 * 
	 * \return Returns the range of the control.
	 **/
	int64_t CUpDown::GetRange() const {
		int32_t i32Lo, i32Hi;
		GetRange( i32Lo, i32Hi );
		return (int64_t( i32Hi ) << 32ULL) | int64_t( i32Lo );
	}

	/**
	 * Determines if the control is using Unicode or not.
	 * 
	 * \return Returns TRUE if the control is using Unicode, or FALSE otherwise.
	 **/
	BOOL CUpDown::GetUnicodeFormat() const {
		if ( m_hWnd == NULL ) { return false; }
		return ::SendMessageW( m_hWnd, UDM_GETUNICODEFORMAT, 0, 0 ) != 0;
	}

	/**
	 * Sets the base radix (10 or 16) and returns the previous radix.
	 * 
	 * \param _ui32Base The new radix base to set.
	 * \return Returns the previous radix base.
	 **/
	uint32_t CUpDown::SetBase( uint32_t _ui32Base ) {
		if ( m_hWnd == NULL ) { return 10; }
		return static_cast<uint32_t>(::SendMessageW( m_hWnd, UDM_SETBASE, static_cast<WPARAM>(_ui32Base), 0 ));
	}

	/**
	 * Sets the buddy control and returns the previous buddy control.
	 * 
	 * \param _pwBuddy A pointer to the new buddy control widget.
	 * \return Returns a pointer to the previous buddy control widget.
	 **/
	CWidget * CUpDown::SetBuddy( CWidget * _pwBuddy ) {
		if ( m_hWnd == NULL ) { return nullptr; }
		HWND hNew = _pwBuddy ? _pwBuddy->Wnd() : NULL;
		HWND hBuddy = reinterpret_cast<HWND>(::SendMessageW( m_hWnd, UDM_SETBUDDY, reinterpret_cast<WPARAM>(hNew), 0 ));
		if ( hBuddy != NULL ) {
			return LSW_WIN2CLASS( hBuddy );
		}
		return nullptr;
	}

	/**
	 * Sets the position and returns the previous position.
	 * 
	 * \param _i32NewPos The new position to set.
	 * \return Returns the previous position.
	 **/
	int32_t CUpDown::SetPos( int32_t _i32NewPos ) {
		if ( m_hWnd == NULL ) { return 0; }
		return static_cast<uint32_t>(::SendMessageW( m_hWnd, UDM_SETPOS32, 0, static_cast<LPARAM>(_i32NewPos) ));
	}

	/**
	 * Sets the range.
	 * 
	 * \param _i32Lower The new lower limit for the range.
	 * \param _i32Upper The new upper limit for the range.
	 **/
	void CUpDown::SetRange( int32_t _i32Lower, int32_t _i32Upper ) {
		if ( m_hWnd != NULL ) {
			::SendMessageW( m_hWnd, UDM_SETRANGE32, static_cast<WPARAM>(_i32Lower), static_cast<LPARAM>(_i32Upper) );
		}
	}

	/**
	 * Sets to use Unicode (TRUE) or ASCII (FALSE) and returns the previous Unicode setting.
	 * 
	 * \param _bUseUnicode TRUE to use Unicode format, FALSE to use ASCII format.
	 * \return Returns the previous Unicode setting.
	 **/
	BOOL CUpDown::SetUnicodeFormat( BOOL _bUseUnicode ) {
		if ( m_hWnd == NULL ) { return FALSE; }
		return static_cast<BOOL>(::SendMessageW( m_hWnd, UDM_SETUNICODEFORMAT, static_cast<WPARAM>(_bUseUnicode), 0 ));
	}

}	// namespace lsw
