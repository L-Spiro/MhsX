#pragma once

#include "../Options/MXOptions.h"
#include "../MemHack/MXMemHack.h"
#include "../Strings/MXSecureString.h"
#include "../Strings/MXSecureWString.h"
#include <Layout/LSWLayoutManager.h>

using namespace lsw;

namespace mx {

	enum MX_LAYOUT_TYPES : DWORD {
		MX_MAIN_WINDOW				= LSW_LT_CUSTOM,
		MX_OPEN_PROCESS_WINDOW,
		MX_OPTIONS_WINDOW,
		MX_OPTIONS_GENERAL,
		MX_OPTIONS_GENSEARCH,
		MX_OPTIONS_SEARCHEX,
		MX_OPTIONS_OPENPROC,
		MX_OPTIONS_HOTKEYS,
		MX_FOUND_ADDRESSES,
		MX_FOUND_ADDRESSES_LISTVIEW,
		MX_EXP_EVAL,
		MX_CONVERTER,
		MX_NEW_DATA_TYPE_SEARCH,
		MX_NEW_POINTER_SEARCH,
		MX_NEW_STRING_SEARCH,
		MX_NEW_EXPRESSION_TYPE_SEARCH,
		MX_STANDARD_SUBSEARCH,
		MX_SEARCH_PROGRESS,
		MX_PE_WORKS,
		MX_STRING_THEORY_OPERATIONS,
		MX_STRING_THEORY,
		MX_FLOATING_POINT_STUDIO,
	};

	class CLayoutManager : public lsw::CLayoutManager {
	public :
		// == Functions.
		// Creates a class based on its type.
		virtual CWidget *				CreateWidget( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data );

		// Creates a pop-up menu.
		virtual bool					CreatePopupMenuEx( CWidget * _pwParent, const LSW_MENU_LAYOUT * _pmlLayout, size_t _sTotal,
			INT _iX, INT _iY );

		// Creates a copy of the given array of LSW_WIDGET_LAYOUT objects with LSW_WIDGET_LAYOUT::pwcText and the expressions.
		static void						UnencryptLayouts( const LSW_WIDGET_LAYOUT * _pwlLayouts, size_t _sTotal,
			std::vector<LSW_WIDGET_LAYOUT> &_vArray,
			std::vector<CSecureWString> &_vWStrings,
			std::vector<CSecureString> &_vStrings );

		// Removes decrypted strings from an array of LSW_WIDGET_LAYOUT objects.
		static void						CleanEncryptedStrings( std::vector<CSecureWString> &_vWStrings,
			std::vector<CSecureString> &_vStrings );

		// Removes decrypted strings.
		static void						CleanEncryptedStrings( std::vector<CSecureWString> &_vWStrings );

		// Removes decrypted strings.
		static void						CleanEncryptedStrings( std::vector<CSecureString> &_vStrings );

		// Creates a copy of a given array of LSW_MENU_ITEM objects with converted strings.
		static void						UnencryptMenuItems( const LSW_MENU_ITEM * _pmiItems, size_t _sTotal,
			std::vector<LSW_MENU_ITEM> &_vArray,
			std::vector<CSecureWString> &_vWStrings );

		// Removes decrypted strings from an array of LSW_MENU_LAYOUT objects.
		static void						UnencryptMenu( const LSW_MENU_LAYOUT * _pmlLayout, size_t _sTotal,
			std::vector<LSW_MENU_LAYOUT> &_vArray,
			std::vector<std::vector<LSW_MENU_ITEM> *> &_vMenuItems,
			std::vector<std::vector<CSecureWString> *> &_vWStrings );

		// Cleans up a decrypted menu.
		static void						CleanEncryptedMenu( std::vector<std::vector<LSW_MENU_ITEM> *> &_vMenuItems,
			std::vector<std::vector<CSecureWString> *> &_vWStrings );

		// == Members.
		// Options.
		//MX_OPTIONS *					m_poOptions;

		// MemHack object.
		//CMemHack *						m_pmMemHack;

	};

}	// namespace mx
