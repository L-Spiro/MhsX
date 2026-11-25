#pragma once
#include "../MXMhsX.h"
#include "../Layouts/MXFontDialogLayout.h"
#include "../Strings/MXStringDecoder.h"

#include <Fonts/LSWFonts.h>
#include <ListBox/LSWListBox.h>
#include <LSWWin.h>
#include <MainWindow/LSWMainWindow.h>

#include <vector>

using namespace lsw;

namespace mx {

	/**
	 * The Change Font dialog (View -> Font -> Change Font...).  Lists the installed fonts (optionally only those for one writing
	 *	system), their styles, and their sizes, with Strikeout and Underline effects and a sample of the result.  Typing in the edit
	 *	box above a list selects the first item starting with the text.  The dialog can be resized.
	 **/
	class CFontDialog : public lsw::CMainWindow {
	public :
		CFontDialog( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CFontDialog();


		// == Functions.
		/**
		 * Handles WM_INITDIALOG.  Fills the lists and selects the current font.
		 *
		 * \return Returns LSW_H_CONTINUE to allow default processing.
		 **/
		virtual LSW_HANDLED											InitDialog();

		/**
		 * Handles WM_CLOSE.  Closes the dialog as Cancel.
		 *
		 * \return Returns LSW_H_HANDLED.
		 **/
		virtual LSW_HANDLED											Close();

		/**
		 * Handles WM_COMMAND.  Keeps the edit boxes, lists, and sample in step, and checks and returns the font on OK.
		 *
		 * \param _wCtrlCode 0 = from menu, 1 = from accelerator; otherwise a control notification code.
		 * \param _wId The identifier for the command or control.
		 * \param _pwSrc The source control for control notifications; otherwise nullptr.
		 * \return Returns a LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED											Command( WORD _wCtrlCode, WORD _wId, CWidget * _pwSrc );

		/**
		 * Handles WM_COMMAND from a menu (Escape).
		 *
		 * \param _wId The menu command identifier.
		 * \return Returns a LSW_HANDLED code.
		 **/
		virtual LSW_HANDLED											MenuCommand( WORD _wId );

		/**
		 * Handles WM_GETMINMAXINFO.  The dialog cannot be made much smaller than it starts.
		 *
		 * \param _pmmiInfo The size limits to fill.
		 * \return Returns LSW_H_HANDLED.
		 **/
		virtual LSW_HANDLED											GetMinMaxInfo( MINMAXINFO * _pmmiInfo );


	protected :
		// == Members.
		/** The parameters. */
		CFontDialogLayout::MX_FONT_DIALOG_PARMS *					m_pfdpParms;
		/** The fonts in the Font list, in order. */
		std::vector<lsw::CFonts::LSW_SYSTEM_FONTS>					m_vFonts;
		/** The styles in the Font Style list, in order. */
		std::vector<lsw::CFonts::LSW_FONT_STYLE>					m_vStyles;
		/** The writing system whose fonts are listed. */
		lsw::CFonts::LSW_WS_ID										m_wiWritingSystem = lsw::CFonts::LSW_WS_ANY;
		/** The font of the sample. */
		HFONT														m_hSampleFont = NULL;
		/** The size of the window when it was created (the minimum size is based on it). */
		SIZE														m_sStartSize {};
		/** True while the controls are being changed by the dialog itself (their notifications are ignored). */
		bool														m_bUpdating = false;


		// == Functions.
		/**
		 * Gets a list box.
		 *
		 * \param _wId The ID of the list box.
		 * \return Returns the list box, or nullptr.
		 **/
		lsw::CListBox *												List( WORD _wId );

		/**
		 * Sets the text of a control without reacting to its change notification.
		 *
		 * \param _wId The ID of the control.
		 * \param _pwcText The text.
		 **/
		void														SetTextQuietly( WORD _wId, const wchar_t * _pwcText );

		/**
		 * Finds the first item in a list box that starts with the text of an edit box (ignoring case), or that matches it exactly.
		 *
		 * \param _wList The ID of the list box.
		 * \param _wEdit The ID of the edit box.
		 * \param _bExact If true, the whole item must match.
		 * \return Returns the index of the item, or LB_ERR.
		 **/
		INT															FindInList( WORD _wList, WORD _wEdit, bool _bExact );

		/**
		 * Fills the Font list with the fonts for the current writing system and selects a font.
		 *
		 * \param _wsSelect The name of the font to select (the first font if it is not listed).
		 **/
		void														FillFonts( const std::wstring &_wsSelect );

		/**
		 * Fills the Font Style list with the styles of the selected font and selects the style closest to the one wanted.
		 *
		 * \param _lWeight The weight wanted.
		 * \param _bItalic The italic setting wanted.
		 * \param _bSetText If true, the Font Style edit box gets the name of the style selected.
		 **/
		void														FillStyles( LONG _lWeight, BYTE _bItalic, bool _bSetText );

		/**
		 * Fills the Size list with the sizes of the selected style and selects the size in the Size edit box, if it is listed.
		 **/
		void														FillSizes();

		/**
		 * Gets the size typed in the Size edit box.
		 *
		 * \param _i32Size Receives the size in points.
		 * \return Returns true if the text is a whole number from 1 to 1638.
		 **/
		bool														GetSize( int32_t &_i32Size );

		/**
		 * Builds the font described by the controls.
		 *
		 * \param _lfFont Receives the font (its height for the DPI in the parameters).
		 * \param _i32Size Receives the size in points.
		 * \return Returns false if the size is not valid.
		 **/
		bool														BuildFont( LOGFONTW &_lfFont, int32_t &_i32Size );

		/**
		 * Shows the sample text in the font described by the controls.
		 **/
		void														UpdateSample();


	private :
		/** The parent object. */
		typedef lsw::CMainWindow									CParent;
	};

}	// namespace mx
