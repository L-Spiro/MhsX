#pragma once

#include "../ExpEval/MXExpEvalWindow.h"
#include "../FoundAddresses/MXFoundAddressesWindow.h"
#include "../MemHack/MXMemHack.h"

#include <MainWindow/LSWMainWindow.h>
#include <MultiSplitter/LSWMultiSplitter.h>
#include <ImageList/LSWImageList.h>
#include <Images/LSWBitmap.h>
#include <ListView/LSWListView.h>

using namespace lsw;

namespace lsw {
	class CStatusBar;
}

namespace mx {
	
	class CMhsMainWindow : public lsw::CMainWindow {
	public :
		CMhsMainWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL );
		~CMhsMainWindow();


		// == Enumerations.
		// Images.
		enum MX_IMAGES {
			MX_I_OPENPROCESS,
			MX_I_OPENFORDEBUG,

			MX_I_ADDENTRY,
			MX_I_OPENENTRY,
			MX_I_SAVEENTRY,

			MX_I_DELETEENTRY,
			MX_I_DELETEALL,
			MX_I_LOCK,
			MX_I_UNLOCK,

			MX_I_EDIT,

			MX_I_OPTIONS,

			MX_I_TOTAL,
		};


		// == Functions.
		// WM_INITDIALOG.
		virtual LSW_HANDLED					InitDialog();

		// WM_COMMAND from control.
		virtual LSW_HANDLED					Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc );

		// WM_COMMAND from menu.
		virtual LSW_HANDLED					MenuCommand( WORD _Id );

		// WM_ERASEBKGND.
		virtual LSW_HANDLED					EraseBkgnd( HDC _hDc );

		// WM_NCDESTROY.
		virtual LSW_HANDLED					NcDestroy();

		// Gets the status bar.
		CStatusBar *						StatusBar();

		// Gets the status bar.
		const CStatusBar *					StatusBar() const;

		// Gets the list view.
		CListView *							MainListView();

		// Gets the list view.
		const CListView *					MainListView() const;

		// Gets the multi-splitter.
		CMultiSplitter *					MultiSplitter();

		// Gets the multi-splitter.
		const CMultiSplitter *				MultiSplitter() const;

		// Virtual client rectangle.  Can be used for things that need to be adjusted based on whether or not status bars, toolbars, etc. are present.
		virtual const LSW_RECT				VirtualClientRect( const CWidget * pwChild ) const;

		// Shows the Found Address dockable.
		void								ShowFoundAddress();

		// Shows the Expression Evaluator dockable.
		void								ShowExpEval();


	protected :
		// == Members.
		// Image list.
		CImageList							m_iImages;

		// Images.
		CBitmap								m_bBitmaps[MX_I_TOTAL];

		// Image mapping.
		INT									m_iImageMap[MX_I_TOTAL];

		// The MHS object.
		CMemHack *							m_pmhMemHack;

		// The Found Address dockable.
		CFoundAddressesWindow *				m_pfaFoundAddresses;

		// The Expression Evaluator dockable.
		CExpEvalWindow *					m_eeExpEval;


		// == Functions.
		// Remove a child.
		virtual void						RemoveChild( const CWidget * _pwChild );

		// Update the "Window" checks.
		void								UpdateWindowChecks();

	};

}	// namespace mx
