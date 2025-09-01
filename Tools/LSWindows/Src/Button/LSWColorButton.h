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

		/**
		 * Returns true if this is a CColorButton class.
		 * 
		 * \return Returns true.
		 **/
		virtual bool								IsColorButton() const { return true; }

		/**
		 * Sets the current color.
		 * 
		 * \param _crColor The color to set.
		 **/
		inline void									SetColor( const COLORREF &_crColor ) { m_crColor = _crColor; }

		/**
		 * Gets the current color.
		 * 
		 * \return Returns the current color.
		 **/
		inline COLORREF								GetColor() const { return m_crColor; }

		/**
		 * Sets the color history.
		 * 
		 * \param _vColors A vector containing the colors to set.  Should be sized to 16.
		 **/
		inline void									SetColors( const std::vector<COLORREF> &_vColors ) {
			auto sSize = std::clamp<size_t>( _vColors.size(), 0, LSW_ELEMENTS( m_acrCust ) );
			std::memcpy( m_acrCust, _vColors.data(), sSize * sizeof( COLORREF ) );
		}

		/**
		 * Gets the history colors.
		 * 
		 * \return Returns the history colors.
		 **/
		inline std::vector<COLORREF>				GetColors() const {
			std::vector<COLORREF> vRet;
			try {
				vRet.resize( LSW_ELEMENTS( m_acrCust ) );
				std::memcpy( vRet.data(), m_acrCust, sizeof( m_acrCust ) );
			}
			catch ( ... ) {}
			return vRet;
		}

		/**
		 * Determines if a color has been selected.
		 * 
		 * \return Returns TRUE if a color has been selected.
		 **/
		inline BOOL									ColorPicked() const { return m_bColorWasSelected; }


	protected :
		// == Members.
		COLORREF									m_acrCust[16] = {						/**< The 16 history colors. */
			RGB( 255, 255, 255 ), RGB( 192, 192, 192 ), RGB( 128, 128, 128 ), RGB( 0, 0, 0 ),
			RGB( 255, 0, 0 ), RGB( 255, 128, 0 ), RGB( 255, 255, 0 ), RGB( 128, 255, 0 ),
			RGB( 0, 255, 0 ), RGB( 0, 255, 128 ), RGB( 0, 255, 255 ), RGB( 0, 128, 255 ),
			RGB( 0, 0, 255 ), RGB( 128, 0, 255 ), RGB( 255, 0, 255 ), RGB( 255, 0, 128 )
		};
		HFONT										m_hFont = NULL;							/**< The text font. */
		COLORREF									m_crColor = RGB( 0xFF, 0xFF, 0xFF );	/**< The current face color of the button. */
		WCHAR										m_wsText[256];							/**< Cached window text used during painting. */
		BOOL										m_bHot = FALSE;							/**< Hover flag (mouse inside client area). */
		BOOL										m_bPressed = FALSE;						/**< Pressed flag (mouse down inside). */
		BOOL										m_bFocus = FALSE;						/**< Focus flag. */
		BOOL										m_bColorWasSelected = FALSE;			/**< Set to TRUE if a color is chosen. */


		// == Functions.
		// Setting the HWND after the control has been created.
		virtual void								InitControl( HWND _hWnd );

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
