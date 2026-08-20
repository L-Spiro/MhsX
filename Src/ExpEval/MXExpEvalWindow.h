#pragma once

#include "../MXMhsX.h"
#include "../Utilities/MXUtilities.h"

#include <ComboBox/LSWComboBox.h>
#include <Docking/LSWDockable.h>
#include <Edit/LSWEdit.h>
#include <ImageList/LSWImageList.h>
#include <Images/LSWBitmap.h>
#include <ListView/LSWListView.h>

using namespace lsw;

namespace mx {
	
	class CExpEvalWindow : public lsw::CDockable {
	public :
		CExpEvalWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CExpEvalWindow();


		// == Enumerations.
		// Images.
		enum MX_IMAGES {
			MX_I_ADD,
			MX_I_REMOVE,
			MX_I_SCIENTIFIC_NOTATION,
			MX_I_EXT_OUTPUT,

			MX_I_TOTAL
		};


		// == Functions.
		// WM_INITDIALOG.
		virtual LSW_HANDLED					InitDialog();

		// WM_COMMAND from control.
		virtual LSW_HANDLED					Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc );

		/**
		 * The WM_SIZE handler.
		 *
		 * \param _wParam The type of resizing requested.
		 * \param _lWidth The new width of the client area.
		 * \param _lHeight The new height of the client area.
		 * \return Returns a LSW_HANDLED enumeration.
		 */
		virtual LSW_HANDLED					Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight ) {
			lsw::CWidget::LSW_SETREDRAW srRedraw( this, false, true, RDW_INVALIDATE |
				RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN );
			return lsw::CDockable::Size( _wParam, _lWidth, _lHeight );
		}

		// Virtual client rectangle.  Can be used for things that need to be adjusted based on whether or not status bars, toolbars, etc. are present.
		virtual const LSW_RECT				VirtualClientRect( const CWidget * _pwChild ) const;

		// Gets a pointer to the list view.
		CListView *							ListView();

		// Gets a pointer to the input edit box.
		CEdit *								InputEdit();

		// Gets a pointer to the output combo box.
		CComboBox *							OutputCombo();

		// Gets a pointer to the result edit.
		CEdit *								Edit();

		// Updates the text result.
		void								UpdateResult();

		// Adds an expression to the list.
		void								AddExpression( const char * _pcExp );

		// Removes the selected expressions from the list.
		void								RemoveSelectedExpressions();

		// Sets the update speed.
		void								SetUpdateSpeed( uint32_t _ui32Speed );

		// Returns the desired width when first docking into a vertical split.
		virtual INT							PreferredDockWidth() { return 300; }

		// Returns the desired height when first docking into a horizontal split.
		virtual INT							PreferredDockHeight() { return 320; }


	protected :
		// == Members.
		// Image list.
		CImageList							m_iImages;

		// Images.
		CBitmap								m_bBitmaps[MX_I_TOTAL];

		// Image mapping.
		INT									m_iImageMap[MX_I_TOTAL];

		// Timer.
		CUtilities::MX_TIMER				m_tUpdateListTimer;

		// List-update timer update speed.
		UINT								m_uiUpdateSpeed = 100;

		// Decoding mode.
		CUtilities::MX_DECODING				m_dDecoding = CUtilities::MX_D_STANDARD;

		// The array of expressions.
		std::vector<CExpression *>			m_vExpressions;


		// == Functions.
		// WM_CTLCOLORSTATIC.
		virtual LSW_HANDLED					CtlColorStatic( HDC _hDc, CWidget * _pwControl, HBRUSH &_hBrush );

		// WM_TIMER.
		virtual LSW_HANDLED					Timer( UINT_PTR _uiptrId, TIMERPROC _tpProc );

	};

}	// namespace mx
