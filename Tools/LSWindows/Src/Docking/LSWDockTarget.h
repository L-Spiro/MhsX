#pragma once

#include "../LSWWin.h"
#include "../Layout/LSWWidgetLayout.h"
#include "../Widget/LSWWidget.h"


namespace lsw {

	class CDockTarget {
	public :
		CDockTarget();
		~CDockTarget();


		// == Enumerations.
		// Attach type.
		enum LSW_ATTACH_TYPE {
			LSW_AT_LEFT,
			LSW_AT_RIGHT,
			LSW_AT_TOP,
			LSW_AT_BOTTOM,
			LSW_AT_TAB,																	/**< Layered over what is already there, with tabs to choose between them. */
		};


		// == Types.
		// Attaches a widget.
		struct LSW_DT_ATTACH {
			CWidget *						pwWidget;
			DWORD							dwId;
			LSW_ATTACH_TYPE					atAttachTo;
		};


		// == Functions.
		// Attach a widget.
		virtual bool						Attach( const LSW_DT_ATTACH &/*_maAttach*/ ) { return false; }

		// Detaches a widget given its ID.
		virtual bool						Detach( WORD /*_wId*/ ) { return false; }

		// Given a point, an LSW_DT_ATTACH structure is filled that another control could use to
		//	add a control into this splitter.  Also returned is a rectangle that can be used to
		//	draw an attach point during a drag operation.
		// In other words, a user drags a dockable control over this multi-splitter and this tells
		//	it where it can add the control and gives it a rectangle to draw.
		// The distance from the given point to the attachment point is returned.  This is always
		//	the distance to the closest attachment point, and the caller can use this to decide if
		//	attaching a control is possible.  Whatever control is being dragged should be dragged
		//	close enough to an attachment point for attachment to be possible.
		// Returned distance is squared.
		virtual LONG						GetAttachPoint( const POINT &/*_pPoint*/, LSW_DT_ATTACH &/*_maAttach*/, INT /*_iSizeSuggestion*/,
			LSW_RECT &/*_rDrawRect*/ ) { return LONG_MAX; }

		/**
		 * Called when a dockable starts being dragged.  Until EndDragPreview() is called, GetAttachPoint() should report the
		 *	attachment points as they will be once the dragged widget has been detached from this target (if it is attached to it),
		 *	since that is what happens before it is attached somewhere else.
		 *
		 * \param _pwDragging The widget being dragged.
		 **/
		virtual void						BeginDragPreview( const CWidget * /*_pwDragging*/ ) {}

		/**
		 * Called when a drag started with BeginDragPreview() ends.  GetAttachPoint() goes back to using the actual layout.
		 **/
		virtual void						EndDragPreview() {}

	};

}	// namespace lsw

