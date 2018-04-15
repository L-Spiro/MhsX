#pragma once

#include "../Options/MXOptions.h"
#include "../MemHack/MXMemHack.h"
#include <Layout/LSWLayoutManager.h>

using namespace lsw;

namespace mx {

	enum MX_LAYOUT_TYPES : DWORD {
		MX_MAIN_WINDOW				= LSW_LT_CUSTOM,
		MX_OPEN_PROCESS_WINDOW,
		MX_OPTIONS_WINDOW,
		MX_OPTIONS_GENERAL,
		MX_OPTIONS_GENSEARCH,
		MX_OPTIONS_OPENPROC,
		MX_FOUND_ADDRESSES,
		MX_EXP_EVAL,
		MX_NEW_DATA_TYPE_SEARCH,
	};

	class CLayoutManager : public lsw::CLayoutManager {
	public :
		// == Functions.
		// Creates a class based on its type.
		virtual CWidget *				CreateWidget( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu );

		// Creates a copy of the given array of LSW_WIDGET_LAYOUT objects with LSW_WIDGET_LAYOUT::pwcText and the expressions.
		static void						UnencryptLayouts( const LSW_WIDGET_LAYOUT * _pwlLayouts, size_t _sTotal,
			std::vector<LSW_WIDGET_LAYOUT> &_vArray,
			std::vector<std::wstring> &_vWStrings,
			std::vector<std::string> &_vStrings );

		// Removes decrypted strings from an array of LSW_WIDGET_LAYOUT objects.
		static void						CleanEncryptedStrings( std::vector<std::wstring> &_vWStrings,
			std::vector<std::string> &_vStrings );

		// == Members.
		// Options.
		MX_OPTIONS *					m_poOptions;

		// Memhack object.
		CMemHack *						m_pmMemHack;

	};

}	// namespace mx
