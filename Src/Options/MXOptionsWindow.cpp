#include "MXOptionsWindow.h"
#include "../Layouts/MXOptionsLayout.h"
#include "../Strings/MXStringDecoder.h"
#include "../Utilities/MXUtilities.h"
#include <ListBox/LSWListBox.h>

namespace mx {

	COptionsWindow::COptionsWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu ) :
		lsw::CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu ) {
	}

	// == Functions.
	// WM_INITDIALOG.
	CWidget::LSW_HANDLED COptionsWindow::InitDialog() {
		CWidget * pwGeneral = COptionsLayout::CreateGeneralPage( this );
		CWidget * pwGenSearch = COptionsLayout::CreateGeneralSearchPage( this );

		CListBox * plbBox = static_cast<CListBox *>(FindChild( COptionsLayout::MX_OI_LIST ));
		if ( plbBox ) {
			// Temp.  These strings should be on the C*Page classes.
			struct {
				const CHAR * pcText;
				size_t sLen;
			} sList[] = {
				{ _T_01940FD6_General, _LEN_01940FD6 },
				{ _T_F530FBCF_General_Search, _LEN_F530FBCF },
				{ _T_8AB28410_Hotkeys, _LEN_8AB28410 },
				{ _T_5C475EBC_Env__Variables, _LEN_5C475EBC },
				{ _T_083F0713_Programs, _LEN_083F0713 },
				{ _T_85E8BACF_Disassembler, _LEN_85E8BACF },
			};
			for ( size_t I = 0; I < MX_ELEMENTS( sList ); ++I ) {
				plbBox->AddString( mx::CStringDecoder::DecodeToString( sList[I].pcText, sList[I].sLen ).c_str() );
			}
		}
		return LSW_H_CONTINUE;
	}

	// WM_CLOSE.
	CWidget::LSW_HANDLED COptionsWindow::Close() {
		::EndDialog( Wnd(), 0 );
		return LSW_H_HANDLED;
	}

}	// namespace mx