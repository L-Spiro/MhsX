#pragma once

#include "../LSWWin.h"
#include "../Layout/LSWWidgetLayout.h"
#include "../Widget/LSWWidget.h"
#include "LSWButton.h"

namespace lsw {

	class CColorButton : public CButton {
	public :
		CColorButton( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		virtual ~CColorButton();


		// == Functions.
		// Determines the type of control this is.
		virtual uint32_t							WidgetType() const { return LSW_LT_COLORBUTTON; }

		// Returns true if this is a CColorButton class.
		virtual bool								IsColorButton() const { return true; }


	protected :
		// == Members.
		COLORREF									m_acrCust[16] = {						/**< The 16 history colors. */
			RGB( 255, 255, 255 ), RGB( 192, 192, 192 ), RGB( 128, 128, 128 ), RGB( 0, 0, 0 ),
			RGB( 255, 0, 0 ), RGB( 255, 128, 0 ), RGB( 255, 255, 0 ), RGB( 128, 255, 0 ),
			RGB( 0, 255, 0 ), RGB( 0, 255, 128 ), RGB( 0, 255, 255 ), RGB( 0, 128, 255 ),
			RGB( 0, 0, 255 ), RGB( 128, 0, 255 ), RGB( 255, 0, 255 ), RGB( 255, 0, 128 )
		};

		COLORREF									m_crColor;								/**< The current face color of the button. */
		WCHAR										m_wsText[256];							/**< Cached window text used during painting. */
		BOOL										m_bHot;									/**< Hover flag (mouse inside client area). */
		BOOL										m_bPressed;								/**< Pressed flag (mouse down inside). */
		BOOL										m_bFocus;								/**< Focus flag. */


		// == Functions.
		// Setting the HWND after the control has been created.
		virtual void								InitControl( HWND _hWnd );

		/**
		 * \brief Chooses black or white text for best contrast against a given background color.
		 *
		 * \param _crBack Background color to evaluate.
		 * \return Returns RGB( 0, 0, 0 ) or RGB( 255, 255, 255 ) for best legibility.
		 */
		inline COLORREF								ContrastingTextColor( COLORREF _crBack ) {
			const int iY = ( (GetRValue( _crBack ) * 299) + (GetGValue( _crBack ) * 587) + (GetBValue( _crBack ) * 114) ) / 1000;
			return iY >= 140 ? RGB( 0, 0, 0 ) : RGB( 255, 255, 255 );
		}

		/**
		 * \brief Draws the colored button face, border, text, and focus cues.
		 *
		 * \param _hDcPaint The HDC obtained during painting.
		 * \param _rcClient Client rectangle of the button.
		 */
		inline void									DrawColorButton( HDC _hDcPaint, const RECT &_rcClient );

		/**
		 * \brief Opens the system ChooseColor dialog and updates the given color on success.
		 *
		 * \param _hOwner Owner window for the modal dialog.
		 * \param _crIo In/out color. Supplies initial color; receives the chosen color when TRUE is returned.
		 * \return Returns TRUE if the user selected a color; otherwise FALSE.
		 */
		inline BOOL									DoPickColor( COLORREF &_crIo );

		/**
		 * \brief Subclass procedure for the color-picker button.
		 *
		 * \param _hWnd The subclassed button HWND.
		 * \param _uiMsg The message identifier.
		 * \param _wParam The WPARAM for the message.
		 * \param _lParam The LPARAM for the message.
		 * \param _uId Subclass ID used when installing the subclass.
		 * \param _dwRef Reference data passed to SetWindowSubclass (holds SColorButtonState *).
		 * \return Returns an LRESULT per message semantics.
		 */
		static LRESULT CALLBACK						ColorButtonSubclassProc( HWND _hWnd, UINT _uiMsg, WPARAM _wParam, LPARAM _lParam, UINT_PTR _uId, DWORD_PTR _dwRef );

	private :
		typedef CButton								Parent;
	};
		
}	// namespace lsw
