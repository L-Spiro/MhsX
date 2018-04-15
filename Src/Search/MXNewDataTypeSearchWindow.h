#pragma once
#include "../MXMhsX.h"
#include "../Options/MXOptions.h"
#include <ListBox/LSWListBox.h>
#include <LSWWin.h>
#include <MainWindow/LSWMainWindow.h>
#include <string>

using namespace lsw;

namespace mx {

	class CNewDataTypeSearchWindow : public lsw::CMainWindow {
	public :
		CNewDataTypeSearchWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, MX_OPTIONS * _poOptions, bool _bCreateWidget = true, HMENU _hMenu = NULL );


		// == Functions.
		// WM_INITDIALOG.
		virtual LSW_HANDLED					InitDialog();

		// WM_COMMAND from control.
		virtual LSW_HANDLED					Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc );

		// WM_CLOSE.
		virtual LSW_HANDLED					Close();


	protected :
		// == Types.
		struct MX_SEARCH_WIN_DATA {
			uint32_t						uiDataType;
			uint32_t						uiEvalType;
			std::wstring					wsLValText;
			std::wstring					wsRValText;
			std::vector<std::wstring>		vLValHistory;
			std::vector<std::wstring>		vRValHistory;

			std::wstring					wsFromText;
			std::wstring					wsToText;
			std::vector<std::wstring>		vFromHistory;
			std::vector<std::wstring>		vToHistory;
		};


		// == Members.
		MX_OPTIONS *						m_poOptions;

		static MX_SEARCH_WIN_DATA			m_swdData;


		// == Functions.
		// Saves the current dialog values to an MX_SEARCH_WIN_DATA structure.
		void								SaveDialogData( MX_SEARCH_WIN_DATA &_swdData );
	};

}	// namespace mx

