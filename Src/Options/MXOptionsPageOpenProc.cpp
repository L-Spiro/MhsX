#include "MXOptionsPageOpenProc.h"
#include "../Layouts/MXOptionsLayout.h"
#include "../Strings/MXStringDecoder.h"
#include "../Utilities/MXUtilities.h"
#include <CheckButton/LSWCheckButton.h>
#include <Edit/LSWEdit.h>
#include <RadioButton/LSWRadioButton.h>

namespace mx {

	COptionsPageOpenProc::COptionsPageOpenProc( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		COptionsPage( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_poOptions( reinterpret_cast<MX_OPTIONS *>(_ui64Data) ) {
	}

	// == Functions.
	// Get the name of the page.
	std::wstring COptionsPageOpenProc::GetName() const {
		return _DEC_WS_49D5B53D_Open_Process;
	}

	// WM_INITDIALOG.
	CWidget::LSW_HANDLED COptionsPageOpenProc::InitDialog() {
		CCheckButton * pcbCheck = nullptr;
		pcbCheck = static_cast<CCheckButton *>(FindChild( COptionsLayout::MX_OI_OPEN_PROCESS_PROCESS ));
		if ( pcbCheck ) {
			pcbCheck->SetCheck( TRUE );
		}
		pcbCheck = static_cast<CCheckButton *>(FindChild( COptionsLayout::MX_OI_OPEN_PROCESS_PROCESSID ));
		if ( pcbCheck ) {
			pcbCheck->SetCheck( TRUE );
		}

		CheckAll();
		
		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED COptionsPageOpenProc::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {
		if ( !m_poOptions ) {
			CheckAll();
			return LSW_H_CONTINUE;
		}

		static const struct {
			WORD					wId;		// Control ID.
			DWORD					dwFlag;		// Flag to set or unset.
		} aTemp[] = {
			{ COptionsLayout::MX_OI_OPEN_PROCESS_PATH, MX_OP_SHOW_PATH },
			{ COptionsLayout::MX_OI_OPEN_PROCESS_WINDOWS, MX_OP_SHOW_WINDOWS },
			{ COptionsLayout::MX_OI_OPEN_PROCESS_CHILDWINDOWS, MX_OP_SHOW_CHILDWINDOWS },
			{ COptionsLayout::MX_OI_OPEN_PROCESS_PARENT, MX_OP_SHOW_PARENT },
			{ COptionsLayout::MX_OI_OPEN_PROCESS_X86, MX_OP_SHOW_X86 },
		};
		for ( size_t I = 0; I < MX_ELEMENTS( aTemp ); ++I ) {
			if ( aTemp[I].wId == _Id ) {
				CCheckButton * pcbCheck = static_cast<CCheckButton *>(FindChild( _Id ));
				if ( pcbCheck ) {
					if ( pcbCheck->IsChecked() ) {
						m_poOptions->dwOpenProc |= aTemp[I].dwFlag;
					}
					else {
						m_poOptions->dwOpenProc &= ~aTemp[I].dwFlag;
					}
					CheckAll();
					return LSW_H_CONTINUE;
				}
			}
		}
		switch ( _Id ) {
			case COptionsLayout::MX_OI_OPEN_PROCESS_ALL : {
				CCheckButton * pcbCheck = static_cast<CCheckButton *>(FindChild( _Id ));
				if ( pcbCheck ) {
					m_poOptions->dwOpenProc = MX_OP_SHOW_ALL;
				}
				CheckAll();
				break;
			}
			case COptionsLayout::MX_OI_OPEN_PROCESS_NONE : {
				CCheckButton * pcbCheck = static_cast<CCheckButton *>(FindChild( _Id ));
				if ( pcbCheck ) {
					m_poOptions->dwOpenProc = 0;
				}
				CheckAll();
				break;
			}
		}
		return LSW_H_CONTINUE;
	}

	// Check all buttons.
	VOID COptionsPageOpenProc::CheckAll() {
		CCheckButton * pcbCheck = nullptr;
#define MX_FLAG( BIT )		(m_poOptions ? ((m_poOptions->dwOpenProc & (BIT)) == (BIT)) : TRUE)
#define MX_CHECK( ID, TEST )																						\
		pcbCheck = static_cast<CCheckButton *>(FindChild( COptionsLayout::ID ));									\
		if ( pcbCheck ) {																							\
			pcbCheck->SetCheck( TEST );																				\
		}


		MX_CHECK( MX_OI_OPEN_PROCESS_PATH, MX_FLAG( MX_OP_SHOW_PATH ) );
		MX_CHECK( MX_OI_OPEN_PROCESS_WINDOWS, MX_FLAG( MX_OP_SHOW_WINDOWS ) );
		MX_CHECK( MX_OI_OPEN_PROCESS_CHILDWINDOWS, MX_FLAG( MX_OP_SHOW_CHILDWINDOWS ) );
		MX_CHECK( MX_OI_OPEN_PROCESS_PARENT, MX_FLAG( MX_OP_SHOW_PARENT ) );
		MX_CHECK( MX_OI_OPEN_PROCESS_X86, MX_FLAG( MX_OP_SHOW_X86 ) );

		CCheckButton * pcbChildWindows = static_cast<CCheckButton *>(FindChild( COptionsLayout::MX_OI_OPEN_PROCESS_CHILDWINDOWS ));
		if ( pcbChildWindows ) {
			pcbChildWindows->SetEnabled( MX_FLAG( MX_OP_SHOW_WINDOWS ) );
		}

		pcbCheck = static_cast<CCheckButton *>(FindChild( COptionsLayout::MX_OI_OPEN_PROCESS_ALL ));
		if ( pcbCheck ) {
			pcbCheck->SetCheck( MX_FLAG( MX_OP_SHOW_ALL ) );
		}
		pcbCheck = static_cast<CCheckButton *>(FindChild( COptionsLayout::MX_OI_OPEN_PROCESS_NONE ));
		if ( pcbCheck ) {
			pcbCheck->SetCheck( m_poOptions ? m_poOptions->dwOpenProc == 0 : FALSE );
		}
#undef MX_CHECK
#undef MX_FLAG
	}

}	// namespace mx
