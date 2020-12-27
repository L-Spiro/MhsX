#pragma once

#include "../LSWWin.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CStatusBar : public CWidget {
	public :
		CStatusBar( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Types.
		// A part.
		struct LSW_STATUS_PART {
			INT								iRightCoord;
			BOOL							bLockToRight;
		};


		// == Functions.
		// Add parts.
		BOOL								SetParts( const std::vector<LSW_STATUS_PART> &_vParts );

		// Add parts.
		BOOL								SetParts( const LSW_STATUS_PART * _pspParts, size_t _sTotal );

		// Retrieves the text from the specified part of a status window.
		//	Returns a 32-bit value that consists of two 16-bit values. The low word specifies the length, in characters, of the text. The high word specifies the type of operation used to draw the text.
		DWORD								GetTextA( INT _iIdx, LPSTR _lpString, INT _nMaxCount ) const;

		// Retrieves the text from the specified part of a status window.
		//	Returns a 32-bit value that consists of two 16-bit values. The low word specifies the length, in characters, of the text. The high word specifies the type of operation used to draw the text.
		DWORD								GetTextW( INT _iIdx, LPWSTR _lpString, INT _nMaxCount ) const;

		// Retrieves the text from the specified part of a status window.
		virtual std::string					GetTextA( INT _iIdx ) const;

		// Retrieves the text from the specified part of a status window.
		virtual std::wstring				GetTextW( INT _iIdx ) const;

		// Gets the window text.
		virtual std::string					GetTextA() const { return GetTextA( 0 ); }

		// Gets the window text.
		virtual std::wstring				GetTextW() const { return GetTextW( 0 ); }

		// Gets the length of the text in the first part of the status bar.
		virtual INT							GetTextLengthA() const { return GetTextLengthA( 0 ); }

		// Gets the length of the text in the first part of the status bar.
		virtual INT							GetTextLengthW() const { return GetTextLengthW( 0 ); }

		// Gets the length of the text in the gicen part of the status bar.
		virtual INT							GetTextLengthA( INT _iIdx ) const;

		// Gets the length of the text in the gicen part of the status bar.
		virtual INT							GetTextLengthW( INT _iIdx ) const;

		// Sets the text in the specified part of a status window.
		virtual BOOL						SetTextA( BYTE _iIdx, BYTE _bDrawingOp, LPCSTR _lpString );

		// Sets the text in the specified part of a status window.
		virtual BOOL						SetTextW( BYTE _iIdx, BYTE _bDrawingOp, LPCWSTR _lpString );

		// Sets the text in the first part of a status window.
		virtual BOOL						SetTextA( LPCSTR _lpString ) { return SetTextA( 0, 0, _lpString ); }

		// Sets the text in the first part of a status window.
		virtual BOOL						SetTextW( LPCWSTR _lpwString ) { return SetTextW( 0, 0, _lpwString ); }

		// Determines the type of control this is.
		virtual uint32_t					WidgetType() const { return LSW_LT_STATUSBAR; }

		// Returns true if this is a CStatusBar class.
		virtual bool						IsStatusBar() const { return true; }


	protected :
		// == Members.
		// The parts.
		std::vector<LSW_STATUS_PART>		m_vParts;

		// == Functions.
		// Evaluates expressions to determine a new rectangle for the control.
		virtual void						EvalNewSize();

	private :
		typedef CWidget						Parent;
	};

}	// namespace lsw
