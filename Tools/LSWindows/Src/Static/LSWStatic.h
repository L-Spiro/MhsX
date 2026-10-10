#pragma once

#include "../LSWWin.h"
#include "../Images/LSWBitmap.h"
#include "../Layout/LSWWidgetLayout.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	/**
	 * A widget class representing a static control (label).
	 **/
	class CStatic : public CWidget {
	public :
		CStatic( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		/**
		 * Determines the type of control this is.
		 * 
		 * \return Returns the specific widget type identifier.
		 **/
		virtual uint32_t					WidgetType() const { return LSW_LT_LABEL; }

		/**
		 * Returns true if this is a CStatic class.
		 * 
		 * \return Returns true to indicate this is a static widget.
		 **/
		virtual bool						IsStatic() const { return true; }

		/**
		 * Associates a new image with the static control.
		 * 
		 * \param _iType The type of image being set.
		 * \param _bImage The bitmap image to associate with the control.
		 * \return Returns true if the image was successfully set.
		 **/
		virtual bool						SetImage( INT _iType, CBitmap &_bImage );


	protected :
		// == Functions.

	private :
		/**
		 * A typedef for the parent class structure.
		 **/
		typedef CWidget						Parent;
	};
		
}	// namespace lsw
