#include "MXPasteSpecialDialog.h"
#include "../Strings/MXStringMacros.h"
#include "../Utilities/MXUtilities.h"

#include <ListBox/LSWListBox.h>

#include <cstring>


namespace mx {

	CPasteSpecialDialog::CPasteSpecialDialog( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_ppspParms( reinterpret_cast<CPasteSpecialLayout::MX_PASTE_SPECIAL_PARMS *>(_ui64Data) ) {
	}
	CPasteSpecialDialog::~CPasteSpecialDialog() {
	}

	// == Functions.
	/**
	 * Handles WM_INITDIALOG.  Lists the clipboard formats.
	 *
	 * \return Returns LSW_H_HANDLED (the focus is set here).
	 **/
	CWidget::LSW_HANDLED CPasteSpecialDialog::InitDialog() {
		CListBox * plbList = static_cast<CListBox *>(FindChild( CPasteSpecialLayout::MX_PSI_FORMATS_LIST ));
		INT iSelect = 0;
		if ( plbList ) {
			std::vector<UINT> vFormats;
			try {
				if ( ::OpenClipboard( Wnd() ) ) {
					for ( UINT uiFmt = ::EnumClipboardFormats( 0 ); uiFmt; uiFmt = ::EnumClipboardFormats( uiFmt ) ) {
						if ( !IsHandleFormat( uiFmt ) ) { vFormats.push_back( uiFmt ); }
					}
					::CloseClipboard();
				}
				for ( auto uiFmt : vFormats ) {
					INT iIdx = plbList->AddString( FormatName( uiFmt ).c_str() );
					if ( iIdx >= 0 ) {
						plbList->SetItemData( iIdx, LPARAM( uiFmt ) );
						if ( m_ppspParms && uiFmt == m_ppspParms->uiFormat ) { iSelect = iIdx; }
					}
				}
			}
			catch ( ... ) {}
			if ( plbList->GetCount() ) {
				plbList->SetCurSel( iSelect );
			}
			plbList->SetFocus();
		}
		auto pwPaste = FindChild( CPasteSpecialLayout::MX_PSI_PASTE );
		if ( pwPaste ) { pwPaste->SetEnabled( plbList && plbList->GetCount() > 0 ); }
		CParent::InitDialog();
		return LSW_H_HANDLED;
	}

	/**
	 * Handles WM_CLOSE.  Closes the dialog as Cancel.
	 *
	 * \return Returns LSW_H_HANDLED.
	 **/
	CWidget::LSW_HANDLED CPasteSpecialDialog::Close() {
		::EndDialog( Wnd(), FALSE );
		return LSW_H_HANDLED;
	}

	/**
	 * Handles WM_COMMAND.  Paste (or a double-click on a format) reads the format; Cancel closes the dialog.
	 *
	 * \param _wCtrlCode 0 = from menu, 1 = from accelerator; otherwise a control notification code.
	 * \param _wId The identifier for the command or control.
	 * \param _pwSrc The source control for control notifications; otherwise nullptr.
	 * \return Returns a LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CPasteSpecialDialog::Command( WORD _wCtrlCode, WORD _wId, CWidget * /*_pwSrc*/ ) {
		switch ( _wId ) {
			case CPasteSpecialLayout::MX_PSI_FORMATS_LIST : {
				if ( _wCtrlCode == LBN_DBLCLK ) { PasteSelected(); }
				break;
			}
			case CPasteSpecialLayout::MX_PSI_PASTE : {
				PasteSelected();
				break;
			}
			case CPasteSpecialLayout::MX_PSI_CANCEL : {
				::EndDialog( Wnd(), FALSE );
				break;
			}
		}
		return LSW_H_HANDLED;
	}

	/**
	 * Handles WM_COMMAND from a menu (Escape).
	 *
	 * \param _wId The menu command identifier.
	 * \return Returns a LSW_HANDLED code.
	 **/
	CWidget::LSW_HANDLED CPasteSpecialDialog::MenuCommand( WORD _wId ) {
		switch ( _wId ) {
			case IDCANCEL : {}								MX_FALLTHROUGH
			case CPasteSpecialLayout::MX_PSI_CANCEL : {
				return Close();
			}
		}
		return CParent::MenuCommand( _wId );
	}

	/**
	 * Gets the name of a clipboard format.
	 *
	 * \param _uiFormat The format.
	 * \return Returns the name of the format.
	 **/
	std::wstring CPasteSpecialDialog::FormatName( UINT _uiFormat ) {
		try {
			switch ( _uiFormat ) {
				case CF_TEXT : { return _DEC_WS_9BB908F9_Text; }
				case CF_OEMTEXT : { return _DEC_WS_4B56B4A5_OEM_Text; }
				case CF_UNICODETEXT : { return _DEC_WS_C084787F_Unicode_Text; }
				case CF_SYLK : { return _DEC_WS_2653F27E_SYLK; }
				case CF_DIF : { return _DEC_WS_36D6ECE7_DIF; }
				case CF_TIFF : { return _DEC_WS_2C1FA941_TIFF_Image; }
				case CF_DIB : { return _DEC_WS_BDDDAA97_Device_Independent_Bitmap; }
				case CF_DIBV5 : { return _DEC_WS_E14F95D2_Device_Independent_Bitmap__Version_5_; }
				case CF_PENDATA : { return _DEC_WS_36308D82_Pen_Data; }
				case CF_RIFF : { return _DEC_WS_FC41EBCC_RIFF_Audio; }
				case CF_WAVE : { return _DEC_WS_58D9704F_Wave_Audio; }
				case CF_HDROP : { return _DEC_WS_9475CAF5_File_List; }
				case CF_LOCALE : { return _DEC_WS_462CC3AE_Locale; }
				case CF_DSPTEXT : { return _DEC_WS_A53E48E2_Text__Display_; }
			}
			wchar_t wcName[256];
			int iLen = ::GetClipboardFormatNameW( _uiFormat, wcName, int( std::size( wcName ) ) );
			if ( iLen > 0 ) { return std::wstring( wcName, size_t( iLen ) ); }
			std::wstring wsTmp;
			std::wstring wsRet = _DEC_WS_306383EE_Format_;
			wsRet += CUtilities::ToHex( uint64_t( _uiFormat ), wsTmp, 4 );
			return wsRet;
		}
		catch ( ... ) { return std::wstring(); }
	}

	/**
	 * Determines whether a clipboard format is held as a GDI object or another handle that is not global memory.
	 *
	 * \param _uiFormat The format.
	 * \return Returns true if the format has no bytes to read.
	 **/
	bool CPasteSpecialDialog::IsHandleFormat( UINT _uiFormat ) {
		switch ( _uiFormat ) {
			case CF_BITMAP : {}								MX_FALLTHROUGH
			case CF_METAFILEPICT : {}						MX_FALLTHROUGH
			case CF_PALETTE : {}							MX_FALLTHROUGH
			case CF_ENHMETAFILE : {}						MX_FALLTHROUGH
			case CF_OWNERDISPLAY : {}						MX_FALLTHROUGH
			case CF_DSPBITMAP : {}							MX_FALLTHROUGH
			case CF_DSPMETAFILEPICT : {}					MX_FALLTHROUGH
			case CF_DSPENHMETAFILE : { return true; }
		}
		// Private and GDI-object ranges can hold any kind of handle.
		if ( _uiFormat >= CF_PRIVATEFIRST && _uiFormat <= CF_PRIVATELAST ) { return true; }
		if ( _uiFormat >= CF_GDIOBJFIRST && _uiFormat <= CF_GDIOBJLAST ) { return true; }
		return false;
	}

	/**
	 * Reads the bytes of a clipboard format.  Text formats end at their terminators.
	 *
	 * \param _hWnd The window that opens the clipboard.
	 * \param _uiFormat The format.
	 * \param _vBytes Receives the bytes.
	 * \return Returns true if the format was read.
	 **/
	bool CPasteSpecialDialog::ReadFormat( HWND _hWnd, UINT _uiFormat, std::vector<uint8_t> &_vBytes ) {
		_vBytes.clear();
		if ( IsHandleFormat( _uiFormat ) ) { return false; }
		if ( !::OpenClipboard( _hWnd ) ) { return false; }
		bool bRet = false;
		try {
			HANDLE hData = ::GetClipboardData( _uiFormat );
			if ( hData ) {
				const SIZE_T stSize = ::GlobalSize( hData );
				const void * pvData = stSize ? ::GlobalLock( hData ) : nullptr;
				if ( pvData ) {
					size_t sLen = size_t( stSize );
					// Text is cut at its terminator (the memory block can be larger than the text).
					if ( _uiFormat == CF_TEXT || _uiFormat == CF_OEMTEXT || _uiFormat == CF_DSPTEXT ) {
						const void * pvEnd = std::memchr( pvData, 0, sLen );
						if ( pvEnd ) { sLen = size_t( static_cast<const uint8_t *>(pvEnd) - static_cast<const uint8_t *>(pvData) ); }
					}
					else if ( _uiFormat == CF_UNICODETEXT ) {
						const wchar_t * pwcText = static_cast<const wchar_t *>(pvData);
						size_t sChars = sLen / sizeof( wchar_t ), I = 0;
						while ( I < sChars && pwcText[I] ) { ++I; }
						sLen = I * sizeof( wchar_t );
					}
					_vBytes.assign( static_cast<const uint8_t *>(pvData), static_cast<const uint8_t *>(pvData) + sLen );
					::GlobalUnlock( hData );
					bRet = true;
				}
			}
		}
		catch ( ... ) {
			_vBytes.clear();
			bRet = false;
		}
		::CloseClipboard();
		return bRet;
	}

	/**
	 * Reads the selected format and closes the dialog.
	 **/
	void CPasteSpecialDialog::PasteSelected() {
		CListBox * plbList = static_cast<CListBox *>(FindChild( CPasteSpecialLayout::MX_PSI_FORMATS_LIST ));
		if ( !plbList || !m_ppspParms ) { return; }
		INT iSel = plbList->GetCurSel();
		if ( iSel < 0 ) { return; }
		const UINT uiFmt = UINT( plbList->GetItemData( iSel ) );
		if ( !ReadFormat( Wnd(), uiFmt, m_ppspParms->vBytes ) ) {
			lsw::CBase::MessageBoxError( Wnd(), _DEC_WS_3F01E5A0_The_clipboard_data_could_not_be_read_.c_str(), _DEC_WS_DC533F38_Paste_Special.c_str() );
			return;
		}
		if ( m_ppspParms->vBytes.empty() ) {
			lsw::CBase::MessageBoxError( Wnd(), _DEC_WS_3AF29F46_There_is_nothing_to_paste_.c_str(), _DEC_WS_DC533F38_Paste_Special.c_str() );
			return;
		}
		m_ppspParms->uiFormat = uiFmt;
		::EndDialog( Wnd(), TRUE );
	}

}	// namespace mx
