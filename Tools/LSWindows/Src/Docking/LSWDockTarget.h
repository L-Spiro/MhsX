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

	};

}	// namespace lsw

