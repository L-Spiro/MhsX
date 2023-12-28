#include "LSWLayoutManager.h"
#include "../Base/LSWBase.h"
#include "../Button/LSWButton.h"
#include "../CheckButton/LSWCheckButton.h"
#include "../ChildWindow/LSWChildWindow.h"
#include "../ComboBox/LSWComboBox.h"
#include "../ComboBox/LSWComboBoxEx.h"
#include "../Docking/LSWDockable.h"
#include "../Edit/LSWEdit.h"
#include "../GroupBox/LSWGroupBox.h"
#include "../ListBox/LSWListBox.h"
#include "../ListView/LSWListView.h"
#include "../MainWindow/LSWMainWindow.h"
#include "../MultiSplitter/LSWMultiSplitter.h"
#include "../ProgressBar/LSWProgressBar.h"
#include "../RadioButton/LSWRadioButton.h"
#include "../Rebar/LSWRebar.h"
#include "../Splitter/LSWSplitter.h"
#include "../Static/LSWStatic.h"
#include "../StatusBar/LSWStatusBar.h"
#include "../Tab/LSWTab.h"
#include "../ToolBar/LSWToolBar.h"
#include "../ToolTip/LSWToolTip.h"
#include "../TrackBar/LSWTrackBar.h"
#include "../TreeList/LSWTreeList.h"
#include "../TreeListView/LSWTreeListView.h"
#include "../UpDown/LSWUpDown.h"


namespace lsw {

	// == Functions.
	// Creates a window with all of its controls.  Returns the main window widget.
	CWidget * CLayoutManager::CreateWindowX( const LSW_WIDGET_LAYOUT * _pwlLayouts, SIZE_T _sTotal, const LSW_MENU_LAYOUT * _pmlLayouts, SIZE_T _sTotalMenus, CWidget * _pwParent, uint64_t _ui64Data ) {
		if ( !_sTotal ) { return nullptr; }

		HMENU hMenu = NULL;
		if ( _pmlLayouts ) {
			hMenu = CreateMenu( _pmlLayouts, _sTotalMenus );
		}
		LSW_WIDGET_LAYOUT wlMainLayout = FixLayout( _pwlLayouts[0] );
		LSW_RECT rMainRect = { wlMainLayout.iLeft, wlMainLayout.iTop, static_cast<LONG>(wlMainLayout.iLeft + wlMainLayout.dwWidth), static_cast<LONG>(wlMainLayout.iTop + wlMainLayout.dwHeight) };
		::AdjustWindowRectEx( &rMainRect, wlMainLayout.dwStyle, _sTotalMenus ? TRUE : FALSE, wlMainLayout.dwStyleEx );
		wlMainLayout.iLeft = rMainRect.left;
		wlMainLayout.iTop = rMainRect.top;
		wlMainLayout.dwWidth = rMainRect.Width();
		wlMainLayout.dwHeight = rMainRect.Height();

		CWidget * pwMain = CreateWidget( wlMainLayout, _pwParent, true, hMenu, _ui64Data );
		if ( !pwMain ) { return nullptr; }

		std::vector<CWidget *> vWidgets;	// Has to be passed to the ::EnumChildWindows() calls.
		std::map<DWORD, CWidget *> mIdToPointer;
		mIdToPointer.insert_or_assign( pwMain->Id(), pwMain );
		vWidgets.push_back( pwMain );
		for ( SIZE_T I = 1; I < _sTotal; ++I ) {
			auto aTemp = mIdToPointer.find( _pwlLayouts[I].dwParentId );
			CWidget * pwParent = aTemp == mIdToPointer.end() ? nullptr : aTemp->second;

			CWidget * pwThis = CreateWidget( FixLayout( _pwlLayouts[I] ), pwParent, true, NULL, _ui64Data );
			if ( !pwThis ) {
				// Erase everything from the map and return nullptr.
				for ( auto aIt = mIdToPointer.begin(); aIt != mIdToPointer.end(); aIt++ ) {
					delete aIt->second;
				}
				return nullptr;
			}

			mIdToPointer.insert_or_assign( pwThis->Id(), pwThis );
			vWidgets.push_back( pwThis );
		}

		::EnumChildWindows( pwMain->Wnd(), CWidget::EnumChildWindows_SetFont, reinterpret_cast<LPARAM>(CBase::MessageBoxFont()) );
		CWidget::ControlSetup( pwMain, vWidgets );

		// WM_NCCREATE and WM_CREATE occer in constructors so virtual overloads don't work on them.
		// Now that construction is over and everything is ready, fake a WM_INITDIALOG.
		pwMain->InitDialog();

		return pwMain;
	}

	// Creates a modal dialog with all of its controls.  Returns the dialog exit value.
	INT_PTR CLayoutManager::DialogBoxX( const LSW_WIDGET_LAYOUT * _pwlLayouts, SIZE_T _sTotal, CWidget * _pwParent, uint64_t _ui64Data ) {
		LSW_DLGTEMPLATE dtTemplate;
		if ( !CreateDlgTemplate( _pwlLayouts, _sTotal, _pwParent, dtTemplate, _ui64Data ) ) {
			return -1;
		}

		INT_PTR ipRet = ::DialogBoxIndirectParamW( CBase::GetThisHandle(), 
			dtTemplate.pdtTemplate, 
			_pwParent->Wnd(), 
			CWidget::DialogProc,
			reinterpret_cast<LPARAM>(&dtTemplate.vWidgets) );
		
#if 0
		if ( ipRet == -1 ) {
			CBase::PrintError( L"DialogBoxX" );
		}
#endif	// #if 0

		DestroyDialogBoxTemplate( dtTemplate );

		return ipRet;
	}

	// Creates a modeless dialog with all of its controls.  Returns the dialog widget.
	CWidget * CLayoutManager::CreateDialogX( const LSW_WIDGET_LAYOUT * _pwlLayouts, SIZE_T _sTotal, CWidget * _pwParent, uint64_t _ui64Data ) {
		LSW_DLGTEMPLATE dtTemplate;
		if ( !CreateDlgTemplate( _pwlLayouts, _sTotal, _pwParent, dtTemplate, _ui64Data ) ) {
			return NULL;
		}

		/*HWND hWnd = */::CreateDialogIndirectParamW( CBase::GetThisHandle(), 
			dtTemplate.pdtTemplate, 
			_pwParent->Wnd(), 
			CWidget::DialogProc,
			reinterpret_cast<LPARAM>(&dtTemplate.vWidgets) );
		
#if 0
		if ( !hWnd ) {
			CBase::PrintError( L"CreateDialogX" );
		}
#endif	// #if 0

		return dtTemplate.vWidgets[0];
	}

	// Creates a DLGTEMPLATE structure and helper objects given an array of LSW_WIDGET_LAYOUT objects.
	BOOL CLayoutManager::CreateDlgTemplate( const LSW_WIDGET_LAYOUT * _pwlLayouts, SIZE_T _sTotal, CWidget * _pwParent, LSW_DLGTEMPLATE &_dtTemplate, uint64_t _ui64Data ) {
		_dtTemplate.pdtTemplate = nullptr;
		if ( !_sTotal ) { return TRUE; }

		LSW_WIDGET_LAYOUT wlMainLayout = FixLayout( _pwlLayouts[0] );
		LSW_RECT rMainRect = { wlMainLayout.iLeft, wlMainLayout.iTop, static_cast<LONG>(wlMainLayout.iLeft + wlMainLayout.dwWidth), static_cast<LONG>(wlMainLayout.iTop + wlMainLayout.dwHeight) };
		::AdjustWindowRect( &rMainRect, wlMainLayout.dwStyle, FALSE );
		wlMainLayout.iLeft = rMainRect.left;
		wlMainLayout.iTop = rMainRect.top;
		wlMainLayout.dwWidth = rMainRect.Width();
		wlMainLayout.dwHeight = rMainRect.Height();


		CWidget * pwMain = CreateWidget( wlMainLayout, _pwParent, false, NULL, _ui64Data );
		if ( !pwMain ) { return FALSE; }
		_dtTemplate.mIdToPointer.insert_or_assign( pwMain->Id(), pwMain );
		_dtTemplate.vWidgets.push_back( pwMain );
		for ( SIZE_T I = 1; I < _sTotal; ++I ) {
			auto aTemp = _dtTemplate.mIdToPointer.find( _pwlLayouts[I].dwParentId );
			CWidget * pwParent = aTemp == _dtTemplate.mIdToPointer.end() ? nullptr : aTemp->second;
			// Parent here cannot be NULL.
			if ( !pwParent ) { pwParent = pwMain; }

			CWidget * pwThis = CreateWidget( FixLayout( _pwlLayouts[I] ), pwParent, false, NULL, _ui64Data );
			if ( !pwThis ) {
				DestroyDialogBoxTemplate( _dtTemplate );
				return FALSE;
			}

			_dtTemplate.mIdToPointer.insert_or_assign( pwThis->Id(), pwThis );
			_dtTemplate.vWidgets.push_back( pwThis );
		}

		// The controls are linked to each other, just not attached to any windows.
		// Figure out how much memory to allocate.
		SIZE_T sSize = LayoutToDialogTemplate( _pwlLayouts, _sTotal, nullptr, _dtTemplate.vWidgets );
		// Allocate.
		//CGlobalAlloc gaAlloc( GMEM_ZEROINIT, sSize );
		_dtTemplate.gaAlloc.Alloc( GMEM_ZEROINIT, sSize );
		_dtTemplate.pdtTemplate = reinterpret_cast<DLGTEMPLATE *>(_dtTemplate.gaAlloc.Lock());
		LayoutToDialogTemplate( _pwlLayouts, _sTotal, _dtTemplate.pdtTemplate, _dtTemplate.vWidgets );
		_dtTemplate.gaAlloc.Unlock();

		return TRUE;
	}

	// Destroys an LSW_DLGTEMPLATE object created for use by DialogBoxX().
	VOID CLayoutManager::DestroyDialogBoxTemplate( LSW_DLGTEMPLATE &_dtTemplate ) {
		for ( size_t J = _dtTemplate.vWidgets.size(); J--; ) {
			delete _dtTemplate.vWidgets[J];
		}
	}

	// Creates a class based on its type.
	CWidget * CLayoutManager::CreateWidget( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) {
		switch ( _wlLayout.ltType ) {
			case LSW_LT_BUTTON : { return new CButton( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ); }
			case LSW_LT_CHECK : { return new CCheckButton( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ); }
			case LSW_LT_CHILDWINDOW : { return new CChildWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ); }
			case LSW_LT_COMBOBOX : { return new CComboBox( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ); }
			case LSW_LT_COMBOBOXEX : { return new CComboBoxEx( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ); }
			case LSW_LT_DOCKWINDOW : { return new CDockable( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ); }
			case LSW_LT_EDIT : { return new CEdit( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ); }
			case LSW_LT_GROUPBOX : { return new CGroupBox( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ); }
			case LSW_LT_LABEL : { return new CStatic( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ); }
			case LSW_LT_LISTBOX : { return new CListBox( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ); }
			case LSW_LT_LISTVIEW : { return new CListView( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ); }
			case LSW_LT_MAINWINDOW : { return new CMainWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ); }
			case LSW_LT_MULTISPLITTER : { return new CMultiSplitter( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ); }
			case LSW_LT_PROGRESSBAR : { return new CProgressBar( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ); }
			case LSW_LT_RADIO : { return new CRadioButton( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ); }
			case LSW_LT_REBAR : { return new CRebar( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ); }
			case LSW_LT_SPLITTER : { return new CSplitter( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ); }
			case LSW_LT_STATUSBAR : { return new CStatusBar( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ); }
			case LSW_LT_TAB : { return new CTab( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ); }
			case LSW_LT_TOOLBAR : { return new CToolBar( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ); }
			case LSW_LT_TOOLTIP : { return new CToolTip( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ); }
			
			case LSW_LT_TRACKBAR : { return new CTrackBar( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ); }

			case LSW_LT_TREELIST : { return new CTreeList( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ); }
			case LSW_LT_TREELISTVIEW : { return new CTreeListView( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ); }
			case LSW_LT_UPDOWN : { return new CUpDown( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ); }
			case LSW_LT_WIDGET : { return new CWidget( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ); }
		}
		return nullptr;
	}

	// Creates a menu given a menu layout.
	HMENU CLayoutManager::CreateMenu( const LSW_MENU_LAYOUT &_mlLayout ) {
		HMENU hMenu = ::CreateMenu();
		for ( SIZE_T I = 0; I < _mlLayout.stTotalMenuItems; ++I ) {
			if ( !AppendMenuItem( hMenu, _mlLayout.pmiItems[I] ) ) {
				::DestroyMenu( hMenu );
				return NULL;
			}
		}
		return hMenu;
	}

	// Creates menus given menu layouts.
	HMENU CLayoutManager::CreateMenu( const LSW_MENU_LAYOUT * _pmlLayouts, SIZE_T _sTotal ) {
		std::map<DWORD, HMENU> mIdToMenu;
		HMENU hMenu = NULL;
		for ( SIZE_T I = 0; I < _sTotal; ++I ) {
			HMENU hTemp = CreateMenu( _pmlLayouts[I] );
			if ( !hTemp ) {
				for ( auto J = mIdToMenu.begin(); J != mIdToMenu.end(); ++J ) {
					::DestroyMenu( J->second );
				}
				return NULL;
			}
			if ( !hMenu ) {
				hMenu = hTemp;
			}

			// Parent it.
			HMENU hParent = NULL;
			if ( _pmlLayouts[I].dwParentMenuId ) {
				auto aTemp = mIdToMenu.find( _pmlLayouts[I].dwParentMenuId );
				hParent = aTemp == mIdToMenu.end() ? NULL : aTemp->second;
			}
			if ( hParent ) {
				// Add it as a child to the item.
				MENUITEMINFOW miSub = { sizeof( MENUITEMINFOW ) };
				miSub.fMask = MIIM_SUBMENU;
				miSub.hSubMenu = hTemp;
				::SetMenuItemInfoW( hParent, _pmlLayouts[I].dwParentMenuItemId, FALSE, &miSub );
			}

			mIdToMenu.insert_or_assign( _pmlLayouts[I].dwId, hTemp );
		}

		return hMenu;
	}

	// Creates a pop-upmenu given a menu layout.
	HMENU CLayoutManager::CreatePopupMenu( const LSW_MENU_LAYOUT &_mlLayout ) {
		HMENU hMenu = ::CreatePopupMenu();
		for ( SIZE_T I = 0; I < _mlLayout.stTotalMenuItems; ++I ) {
			if ( !AppendMenuItem( hMenu, _mlLayout.pmiItems[I] ) ) {
				::DestroyMenu( hMenu );
				return NULL;
			}
		}
		return hMenu;
	}

	// Creates a pop-upmenu given menu layouts.
	HMENU CLayoutManager::CreatePopupMenu( const LSW_MENU_LAYOUT * _pmlLayouts, SIZE_T _sTotal ) {
		std::map<DWORD, HMENU> mIdToMenu;
		HMENU hMenu = NULL;
		for ( SIZE_T I = 0; I < _sTotal; ++I ) {
			HMENU hTemp = I == 0 ? CreatePopupMenu( _pmlLayouts[I] ) : CreateMenu( _pmlLayouts[I] );
			if ( !hTemp ) {
				for ( auto J = mIdToMenu.begin(); J != mIdToMenu.end(); ++J ) {
					::DestroyMenu( J->second );
				}
				return NULL;
			}
			if ( !hMenu ) {
				hMenu = hTemp;
			}

			// Parent it.
			HMENU hParent = NULL;
			if ( _pmlLayouts[I].dwParentMenuId ) {
				auto aTemp = mIdToMenu.find( _pmlLayouts[I].dwParentMenuId );
				hParent = aTemp == mIdToMenu.end() ? NULL : aTemp->second;
			}
			if ( hParent ) {
				// Add it as a child to the item.
				MENUITEMINFOW miSub = { sizeof( MENUITEMINFOW ) };
				miSub.fMask = MIIM_SUBMENU;
				miSub.hSubMenu = hTemp;
				::SetMenuItemInfoW( hParent, _pmlLayouts[I].dwParentMenuItemId, FALSE, &miSub );
			}

			mIdToMenu.insert_or_assign( _pmlLayouts[I].dwId, hTemp );
		}

		return hMenu;
	}

	// Takes the given layout and produces a copy with certain things changed as necessary.  For example, if
	//	the control type is LSW_LT_DOCKWINDOW, the class name is changed to lsw::CBase::DockableAtom().
	// In other cases this fills in missing information, so if you pass a nullptr control class name, the common name will be filled automatically here, etc.
	LSW_WIDGET_LAYOUT CLayoutManager::FixLayout( const LSW_WIDGET_LAYOUT &_wlLayout ) {
		LSW_WIDGET_LAYOUT wlCopy = _wlLayout;
		// This case is always handled.
		/*if ( wlCopy.ltType == LSW_LT_DOCKWINDOW ) {
			wlCopy.lpwcClass = reinterpret_cast<LPCWSTR>(lsw::CBase::DockableAtom());
		}*/
		if ( !wlCopy.lpwcClass ) {
			const struct {
				LSW_LAYOUTTYPES					ltType;
				LPCWSTR							lpwsClass;
			} aStruct [] = {
				{ LSW_LT_BUTTON, WC_BUTTONW },
				{ LSW_LT_CHECK, WC_BUTTONW },
				{ LSW_LT_CHILDWINDOW, reinterpret_cast<LPCWSTR>(lsw::CBase::ChildWindowAtom()) },
				{ LSW_LT_COMBOBOX, WC_COMBOBOXW },
				{ LSW_LT_COMBOBOXEX, WC_COMBOBOXEXW },
				{ LSW_LT_DOCKWINDOW, reinterpret_cast<LPCWSTR>(lsw::CBase::DockableAtom()) },
				{ LSW_LT_EDIT, WC_EDITW },
				{ LSW_LT_GROUPBOX, WC_BUTTONW },
				{ LSW_LT_LISTVIEW, WC_LISTVIEWW },
				{ LSW_LT_LISTBOX, WC_LISTBOXW },
				{ LSW_LT_LABEL, WC_STATICW },
				{ LSW_LT_MULTISPLITTER, reinterpret_cast<LPCWSTR>(lsw::CBase::MultiSplitterAtom()) },
				{ LSW_LT_PROGRESSBAR, PROGRESS_CLASSW },
				{ LSW_LT_RADIO, WC_BUTTONW },
				{ LSW_LT_REBAR, REBARCLASSNAMEW },				
				{ LSW_LT_STATUSBAR, STATUSCLASSNAMEW },
				{ LSW_LT_TAB, WC_TABCONTROLW },
				{ LSW_LT_TRACKBAR, TRACKBAR_CLASS },
				{ LSW_LT_TOOLBAR, TOOLBARCLASSNAMEW },
				{ LSW_LT_TOOLTIP, TOOLTIPS_CLASSW },
				{ LSW_LT_TREEVIEW, WC_TREEVIEWW },
				{ LSW_LT_UPDOWN, UPDOWN_CLASSW },
			};
			for ( size_t I = 0; I < LSW_ELEMENTS( aStruct ); ++I ) {
				if ( wlCopy.ltType == aStruct[I].ltType ) {
					wlCopy.lpwcClass = aStruct[I].lpwsClass;
					break;
				}
			}
		}
		return wlCopy;
	}

	// Converts an array of LSW_WIDGET_LAYOUT structures to a DLGTEMPLATE structure and an array of following DLGITEMTEMPLATE structures.
	SIZE_T CLayoutManager::LayoutToDialogTemplate( const LSW_WIDGET_LAYOUT * _pwlLayouts, SIZE_T _sTotal, DLGTEMPLATE * _pdtTemplate, const std::vector<CWidget *> &_vWidgets ) {
		uint8_t * pui8Temp = reinterpret_cast<uint8_t *>(_pdtTemplate);
		SIZE_T sRet = sizeof( DLGTEMPLATE );
		// For now, always set the font.
		DWORD dwStyle = _pwlLayouts[0].dwStyle | DS_SETFONT;
		if ( _pdtTemplate ) {
			_pdtTemplate->style = dwStyle;
			_pdtTemplate->dwExtendedStyle = _pwlLayouts[0].dwStyleEx;
			_pdtTemplate->cdit = static_cast<WORD>(_sTotal - 1);
			_pdtTemplate->x = static_cast<short>(_pwlLayouts[0].iLeft);
			_pdtTemplate->y = static_cast<short>(_pwlLayouts[0].iTop);
			_pdtTemplate->cx = static_cast<short>(_pwlLayouts[0].dwWidth);
			_pdtTemplate->cy = static_cast<short>(_pwlLayouts[0].dwHeight);

		}
		WORD * pwTemp = reinterpret_cast<WORD *>(pui8Temp + sRet);
#define LSW_WRITE_WORD( VAL )	if ( _pdtTemplate ) { (*pwTemp++) = static_cast<WORD>(VAL); } sRet += sizeof( WORD )
		// No menu.
		LSW_WRITE_WORD( 0 );
		
		// Dialog class.  If input is, null, use default.
		if ( !_pwlLayouts[0].lpwcClass ) {
			LSW_WRITE_WORD( 0 );
		}
		else {
			pwTemp = reinterpret_cast<WORD *>(pui8Temp + sRet);
			sRet += ItemTemplateString( _pwlLayouts[0].lpwcClass, _pdtTemplate ? pwTemp : nullptr );
		}

		// Dialog title.
		pwTemp = reinterpret_cast<WORD *>(pui8Temp + sRet);
		sRet += ItemTemplateString( _pwlLayouts[0].pwcText, _pdtTemplate ? pwTemp : nullptr );

		// Optional font.
		if ( dwStyle & DS_SETFONT ) {
			// Get the system default.
			NONCLIENTMETRICSW ncmMetrics = { sizeof( ncmMetrics ) };
			::SystemParametersInfoW( SPI_GETNONCLIENTMETRICS, ncmMetrics.cbSize, &ncmMetrics, 0 );
			//HFONT hFont = ::CreateFontIndirectW( &ncmMetrics.lfMessageFont );

			pwTemp = reinterpret_cast<WORD *>(pui8Temp + sRet);
			LSW_WRITE_WORD( ncmMetrics.lfMessageFont.lfHeight );
			pwTemp = reinterpret_cast<WORD *>(pui8Temp + sRet);
			sRet += ItemTemplateString( ncmMetrics.lfMessageFont.lfFaceName, _pdtTemplate ? pwTemp : nullptr );
		}

		sRet = reinterpret_cast<SIZE_T>(CHelpers::DwordAlign( reinterpret_cast<LPWORD>(sRet) ));

		// All other controls.
		for ( SIZE_T I = 1; I < _sTotal; ++I ) {
			size_t sBackup = sRet;
			sRet = reinterpret_cast<SIZE_T>(CHelpers::DwordAlign( reinterpret_cast<LPWORD>(sRet) ));
			DLGITEMTEMPLATE * pditTemp = reinterpret_cast<DLGITEMTEMPLATE *>(pui8Temp + sRet);
			size_t sAddMe = LayoutToItemTemplate( FixLayout( _pwlLayouts[I] ), _pdtTemplate ? pditTemp : nullptr, _vWidgets[I] );
			if ( !sAddMe ) {
				if ( _pdtTemplate ) { --_pdtTemplate->cdit; }
				sRet = sBackup;
			}
			else {
				sRet += sAddMe;
			}
		}

		return reinterpret_cast<SIZE_T>(CHelpers::DwordAlign( reinterpret_cast<LPWORD>(sRet) ));
#undef LSW_WRITE_WORD
	}

	// Converts an LSW_WIDGET_LAYOUT structure to a DLGITEMTEMPLATE structure.  Pass nullptr to get the size to allocate.
	SIZE_T CLayoutManager::LayoutToItemTemplate( const LSW_WIDGET_LAYOUT &_wlLayout, DLGITEMTEMPLATE * _pdittTemplate, CWidget * /*_pwCreationData*/ ) {
		if ( _wlLayout.ltType == LSW_LT_TOOLTIP ) { return 0; }
		uint8_t * pui8Temp = reinterpret_cast<uint8_t *>(_pdittTemplate);
		SIZE_T sRet = sizeof( DLGITEMTEMPLATE );
		if ( _pdittTemplate ) {
			_pdittTemplate->style = _wlLayout.dwStyle;
			_pdittTemplate->dwExtendedStyle = _wlLayout.dwStyleEx;
			_pdittTemplate->id = _wlLayout.wId;
			_pdittTemplate->x = static_cast<short>(_wlLayout.iLeft);
			_pdittTemplate->y = static_cast<short>(_wlLayout.iTop);
			_pdittTemplate->cx = static_cast<short>(_wlLayout.dwWidth);
			_pdittTemplate->cy = static_cast<short>(_wlLayout.dwHeight);
		}
		WORD * pwTemp = reinterpret_cast<WORD *>(pui8Temp + sRet);
		sRet += ItemTemplateString( _wlLayout.lpwcClass, _pdittTemplate ? pwTemp : nullptr );

		pwTemp = reinterpret_cast<WORD *>(pui8Temp + sRet);
		sRet += ItemTemplateString( _wlLayout.pwcText, _pdittTemplate ? pwTemp : nullptr );

		// No creation data.
		if ( _pdittTemplate ) {
			pwTemp = reinterpret_cast<WORD *>(pui8Temp + sRet);
			(*pwTemp++) = 0;
		}
		sRet += sizeof( WORD );
		/*
		if ( _pdittTemplate ) {
			pwTemp = reinterpret_cast<WORD *>(pui8Temp + sRet);
			(*pwTemp++) = sizeof( _pwCreationData ) + sizeof( WORD );	// Size of the pointer is correct.
			std::memcpy( pwTemp, _pwCreationData, sizeof( _pwCreationData ) );
		}
		sRet += sizeof( _pwCreationData ) + sizeof( WORD );*/
		return sRet;//reinterpret_cast<SIZE_T>(CHelpers::DwordAlign( reinterpret_cast<LPWORD>(sRet) ));
	}

	// Writes a string to a given pointer formatted for use with DLGITEMTEMPLATE objects.
	SIZE_T CLayoutManager::ItemTemplateString( const WCHAR * _pwcString, WORD * _pwDest ) {
		SIZE_T sRet = 0;
		if ( _pwcString ) {
			while ( (*_pwcString) ) {
				if ( _pwDest ) {
					(*_pwDest++) = (*_pwcString);
				}

				sRet += sizeof( WORD );
				++_pwcString;
			}
		}
		// The NULL terminator.
		if ( _pwDest ) {
			(*_pwDest) = L'\0';
		}
		sRet += sizeof( WORD );
		return sRet;
	}

	// Appends a menu item to a menu.
	BOOL CLayoutManager::AppendMenuItem( HMENU _hMenu, const LSW_MENU_ITEM &_miItem ) {
		if ( _miItem.bSkip ) { return TRUE; }
		if ( _miItem.bIsSeperator ) {
			return ::AppendMenuW( _hMenu, MF_SEPARATOR, 0, NULL );
		}
		UINT uiFlags = MF_STRING;
		if ( _miItem.bCheckable ) {
			uiFlags |= _miItem.bChecked ? MF_CHECKED : MF_UNCHECKED;
		}
		uiFlags |= _miItem.bEnabled ? MF_ENABLED : MF_GRAYED;
		return ::AppendMenuW( _hMenu, uiFlags, _miItem.dwId, _miItem.lpwcText );
	}

}	// namespace lsw
