#pragma once

#include "../LSWWin.h"
#include "../Base/LSWGlobalAlloc.h"
#include "../Widget/LSWWidget.h"
#include "LSWMenuLayout.h"

#include <map>
#include <vector>

namespace lsw {

	class CLayoutManager {
	public :
		// == Types.
		// A DLGTEMPLATE object and helper objects.
		struct LSW_DLGTEMPLATE {
			DLGTEMPLATE *				pdtTemplate;			// Created with CGlobalAlloc.
			std::vector<CWidget *>		vWidgets;
			std::map<DWORD, CWidget *>	mIdToPointer;
			CGlobalAlloc				gaAlloc;
		};

		// == Functions.
		// Creates a window with all of its controls.  Returns the main window widget.
		CWidget *						CreateWindowX( const LSW_WIDGET_LAYOUT * _pwlLayouts, SIZE_T _sTotal, const LSW_MENU_LAYOUT * _pmlLayouts = nullptr, SIZE_T _sTotalMenus = 0, CWidget * _pwParent = nullptr, uint64_t _ui64Data = 0 );

		// Creates a modal dialog with all of its controls.  Returns the dialog exit value.
		INT_PTR							DialogBoxX( const LSW_WIDGET_LAYOUT * _pwlLayouts, SIZE_T _sTotal, CWidget * _pwParent, uint64_t _ui64Data );

		// Creates a modeless dialog with all of its controls.  Returns the dialog widget.
		CWidget *						CreateDialogX( const LSW_WIDGET_LAYOUT * _pwlLayouts, SIZE_T _sTotal, CWidget * _pwParent, uint64_t _ui64Data );

		// Creates a DLGTEMPLATE structure and helper objects given an array of LSW_WIDGET_LAYOUT objects.
		BOOL							CreateDlgTemplate( const LSW_WIDGET_LAYOUT * _pwlLayouts, SIZE_T _sTotal, CWidget * _pwParent, LSW_DLGTEMPLATE &_dtTemplate, uint64_t _ui64Data );

		// Destroys an LSW_DLGTEMPLATE object created for use by DialogBoxX().
		VOID							DestroyDialogBoxTemplate( LSW_DLGTEMPLATE &_dtTemplate );

		// Creates a class based on its type.
		virtual CWidget *				CreateWidget( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data );

		// Creates a menu given a menu layout.
		HMENU							CreateMenu( const LSW_MENU_LAYOUT &_mlLayout );

		// Creates menus given menu layouts.
		HMENU							CreateMenu( const LSW_MENU_LAYOUT * _pmlLayouts, SIZE_T _sTotal );

		// Creates a pop-upmenu given a menu layout.
		HMENU							CreatePopupMenu( const LSW_MENU_LAYOUT &_mlLayout );

		// Creates a pop-upmenu given menu layouts.
		HMENU							CreatePopupMenu( const LSW_MENU_LAYOUT * _pmlLayouts, SIZE_T _sTotal );

		// Takes the given layout and produces a copy with certain things changed as necessary.  For example, if
		//	the control type is LSW_LT_DOCKWINDOW, the class name is changed to lsw::CBase::DockableAtom().
		// In other cases this fills in missing information, so if you pass a nullptr control class name, the common name will be filled automatically here, etc.
		static LSW_WIDGET_LAYOUT		FixLayout( const LSW_WIDGET_LAYOUT &_wlLayout );


	protected :
		// == Functions.
		// Converts an array of LSW_WIDGET_LAYOUT structures to a DLGTEMPLATE structure and an array of following DLGITEMTEMPLATE structures.
		SIZE_T							LayoutToDialogTemplate( const LSW_WIDGET_LAYOUT * _pwlLayouts, SIZE_T _sTotal, DLGTEMPLATE * _pdtTemplate, const std::vector<CWidget *> &_vWidgets );

		// Converts an LSW_WIDGET_LAYOUT structure to a DLGITEMTEMPLATE structure.  Pass nullptr to get the size to allocate.
		SIZE_T							LayoutToItemTemplate( const LSW_WIDGET_LAYOUT &_wlLayout, DLGITEMTEMPLATE * _pdittTemplate, CWidget * _pwCreationData );

		// Writes a string to a given pointer formatted for use with DLGITEMTEMPLATE objects.
		SIZE_T							ItemTemplateString( const WCHAR * _pwcString, WORD * _pwDest );

		// Appends a menu item to a menu.
		BOOL							AppendMenuItem( HMENU _hMenu, const LSW_MENU_ITEM &_miItem );

	};

}	// namespace lsw
