#pragma once

#include <Layout/LSWMenuLayout.h>
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace mx {

	class CStringTheoryLayout {
	public :
		// == Enumerations.
		// Control ID's.
		enum MX_STRING_THEORY_WINDOW_IDS {
			MX_STW_NONE,
			MX_STW_MAINWINDOW					= 670,
			MX_STW_INPUT_GROUP,
			MX_STW_INPUT_UTF8,
			MX_STW_UNICODE_CHARS_COMBO,
			MX_STW_UNICODE_NAME,
			MX_STW_UNICODE_4_ESCAPE,
			MX_STW_UNICODE_8_ESCAPE,
			MX_STW_RESOLVE_ESCAPES_CHECK,
			MX_STW_RESOLVE_HTML_CHECK,
			MX_STW_INPUT_IS_HEX_CHECK,
			MX_STW_INPUT_ENCODING_GROUP,
			MX_STW_INPUT_ENCODING_COMBO,
			
			MX_STW_MODIFIERS_GROUP,
			MX_STW_PREDEF_GROUP,

			MX_STW_NO_CASE,
			MX_STW_TOUPPER,
			MX_STW_TOLOWER,

			MX_STW_NO_KANA,
			MX_STW_TO_HIRAGANA,
			MX_STW_TO_KATAKANA,

			MX_STW_FULL_WIDTH,
			MX_STW_STD_DIGITS,

			MX_STW_NORMALIZE,
			MX_STW_NORMALIZEC,
			MX_STW_NORMALIZED,
			MX_STW_NORMALIZEKC,
			MX_STW_NORMALIZEKD,

			MX_STW_ENCODING_GROUP,

			MX_STW_ENCODING_COMBO,
			MX_STW_COMPOSITE,
			MX_STW_DEFAULT_CHAR,
			MX_STW_DISCARD_NONSPACING,
			MX_STW_SEP_CHARS,
			MX_STW_NO_BEST_FIT,

			MX_STW_OUTPUT_GROUP,
			MX_STW_ESCAPE_GROUP,
			MX_STW_ESPACE_ONLY_NUL,
			MX_STW_ESPACE_ALL_UNPRINTABLE,
			MX_STW_ESPACE_ONLY_NUL_AND_BS,
			MX_STW_ESPACE_ALL_UNPRINTABLE_AND_BS,
			MX_STW_ESPACE_ALL,
			MX_STW_ESPACE_STANDARD,

			MX_STW_OUTPUT_HEX_EDIT,
			MX_STW_OUTPUT_EDIT,
		};

		// Menu items
		/*enum MX_MAINWINDOW_MENU_IDS {
			MX_STW_MENU_BAR						= 780,
			MX_STW_MENU_FILE,

			MX_STW_SEARCH,
			MX_STW_TOOLS,
			MX_STW_WINDOW,
			MX_STW_HELP,

			MX_STW_FILE							= 800,
			MX_STW_OPENFILE,
			MX_STW_OPENPE,
			MX_STW_OPENRECENT,
			
		};*/


		// == Functions.
		// Creates the String Theory window.
		static CWidget *						CreateStringTheoryWindow( CWidget * _pwParent );


	protected :
		// == Members.
		// The layout for the String Theory window.
		static LSW_WIDGET_LAYOUT				m_wlMainWindow[];

		// Menu bar items for the main window.
		/*static LSW_MENU_ITEM					m_miMenuBar[];

		// File menu.
		static LSW_MENU_ITEM					m_miFileMenu[];

		// Menus.
		static LSW_MENU_LAYOUT					m_miMenus[];*/
	};

}	// namespace mx
