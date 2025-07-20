#pragma once

#include "../MXMhsX.h"
#include "../Utilities/MXUtilities.h"

#include <Docking/LSWDockable.h>
#include <ImageList/LSWImageList.h>
#include <Images/LSWBitmap.h>
#include <ListView/LSWListView.h>

using namespace lsw;

namespace mx {
	
	class CFoundAddressesWindow : public lsw::CDockable {
	public :
		CFoundAddressesWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CFoundAddressesWindow();


		// == Enumerations.
		// Images.
		enum MX_IMAGES {
			MX_I_SEARCH,
			MX_I_SUBSEARCH,
			MX_I_ADD,

			MX_I_TOTAL
		};

		// Timer.
		enum MX_TIMER {
			MX_T_UPDATE_LIST				= 0x3D00
		};

		// Menu.
		enum MX_MENU {
			MX_M_CONTEXT_MENU				= 0x3E00
		};

		


		// == Functions.
		// WM_INITDIALOG.
		virtual LSW_HANDLED					InitDialog();

		// WM_COMMAND from control.
		virtual LSW_HANDLED					Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc );

		// WM_COMMAND from menu.
		virtual LSW_HANDLED					MenuCommand( WORD _Id );

		// Virtual client rectangle.  Can be used for things that need to be adjusted based on whether or not status bars, toolbars, etc. are present.
		virtual const LSW_RECT				VirtualClientRect( const CWidget * pwChild ) const;

		// Gets a pointer to the list view.
		CListView *							ListView();

		// Gets a pointer to the list view.
		const CListView *					ListView() const;

		// Updates the header column after a new search.
		void								UpdateHeaders( CUtilities::MX_SEARCH_TYPES _stType );

		// Adds the found addresses to the list.
		void								AddFoundAddresses();

		// Sets the update speed.
		void								SetUpdateSpeed( uint32_t _ui32Speed );

		// Gets the total number of selected addresses.
		size_t								TotalSelected() const;

		// Adds the selected addresses to the main list.
		bool								AddSelectedAddresses() const;

		// Copies the selected texts' address lines.
		void								CopySelectedAddresses() const;

		// Copies the selected texts' value lines.
		void								CopySelectedValues() const;

		// Copies the selected texts' current value lines if any.
		void								CopySelectedCurValues() const;

		// Copies the selected rows' text.
		void								CopySelectedText() const;

		// Gets the index of the current-value header or -1.
		int									CurValIndex() const;

		// Returns the desired width when first docking into a vertical split.
		virtual INT							PreferredDockWidth() { return 300; }

		// Returns the desired height when first docking into a horizontal split.
		virtual INT							PreferredDockHeight() { return 200; }


	protected :
		// == Members.
		// Image list.
		CImageList							m_iImages;

		// Images.
		CBitmap								m_bBitmaps[MX_I_TOTAL];

		// Image mapping.
		INT									m_iImageMap[MX_I_TOTAL];

		// The main window.
		class CMhsMainWindow *				m_pmmwMhsWindow;

		// Header sizes.
		std::vector<INT>					m_vHeaderWidths;

		// Timer.
		UINT_PTR							m_uiptrUpdateListTimer;

		// Timer update speed.
		UINT								m_uiUpdateSpeed;


		// == Functions.
		// WM_TIMER.
		virtual LSW_HANDLED					Timer( UINT_PTR _uiptrId, TIMERPROC _tpProc );

		// WM_CONTEXTMENU.
		virtual LSW_HANDLED					ContextMenu( CWidget * _pwControl, INT _iX, INT _iY );

		// WM_NOTIFY->NM_DBLCLK on this item (if LSW_HANDLED::LSW_H_CONTINUE, message is passed to owning window).
		virtual LSW_HANDLED					DblClk( const LPNMITEMACTIVATE _phHdr );
	};

}	// namespace mx
