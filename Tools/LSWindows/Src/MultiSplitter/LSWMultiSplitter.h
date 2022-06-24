#pragma once
#include "../LSWWin.h"
#include "../Docking/LSWDockTarget.h"
#include "../Layout/LSWWidgetLayout.h"
#include "../Widget/LSWWidget.h"


namespace lsw {

	class CMultiSplitter : public CWidget, public CDockTarget {
	public :
		CMultiSplitter( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CMultiSplitter();


		// == Enumerations.
		// A split type.
		enum LSW_SPLIT_TYPE : DWORD {
			LSW_ST_HOR,
			LSW_ST_VER,
		};

		// == Functions.
		// Attach a widget.
		virtual bool						Attach( const LSW_DT_ATTACH &_maAttach );

		// Detaches a widget given its ID.
		virtual bool						Detach( WORD _wId );

		// Root ID.
		DWORD								RootId() const { return m_meRoot.dwId; }

		// Client rectangle.
		virtual LSW_RECT					ClientRect( const CWidget * pwChild ) const;

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
		virtual LONG						GetAttachPoint( const POINT &_pPoint, LSW_DT_ATTACH &_maAttach, INT _iSizeSuggestion,
			LSW_RECT &_rDrawRect );

		// Determines the type of control this is.
		virtual uint32_t					WidgetType() const { return LSW_LT_MULTISPLITTER; }

		// Returns true if this is a CMultiSplitter class.
		virtual bool						IsMultiSplitter() const { return true; }

	private :
		// == Types.
		// A single rectangle.
		struct LSW_MS_LAYER;
		struct LSW_MS_RECT {
			union {
				CWidget *					pqWidget;
				LSW_MS_LAYER *				pmlLayer;
			}								u;
			INT								iDist;					// Distance in either the vertical or horizontal direction.  Acts
																	//	as the location of the divider bar.
			BOOL							bContainsWidget;		// If true, inside this rectangle is a widget.  Otherwise it is an
																	//	LSW_MS_LAYER object.
			BOOL							bLockToFar;				// Locks the sizing bar to the far end of the rectangle (right or bottom).
			DWORD							dwId;					// Unique ID of this box.
			LSW_RECT						rRect;					// Rectangle of this box in client coordinates.
		};

		// A layer.  Each layer can be split X times in a single direction.
		//	If a split in the other direction occurs, another layer must be added.
		//	For example, you could have 3 vertical bars in a single layer, and if
		//	a horizontal bar is added a new layer will be created, nesting the first
		//	layer inside it.
		struct LSW_MS_LAYER {
			LSW_SPLIT_TYPE					stSplitType;
			std::vector<LSW_MS_RECT>		vRects;					// In the direction of the split type.
			LSW_RECT						rRect;					// Full bounds of the layer in client space.
		};

		// A search result.
		struct LSW_MS_LAYER_SEARCH {
			LSW_MS_LAYER *					pmlLayer;
			size_t							sIndex;
			LSW_MS_RECT *					pmrRect;
		};

		// Detach data.
		struct LSW_MS_DETACH {
			LSW_MS_LAYER *					pmlLayer;				// Layer containing the widget to remove.
			size_t							sIndex;					// Index of the widget inside the layer to remove.
			LSW_MS_LAYER *					pmlParentLayer;			// Layer containing the layer holding the widget to remove.
			size_t							sParentIndex;			// Index into the parent layer where this layer can be found.
		};


		// == Members.
		// All of the layers, in any order.
		std::vector<LSW_MS_LAYER *>			m_vLayers;
		// All of the layers, in any order, except the layer being dragged, if any.
		std::vector<LSW_MS_LAYER *>			m_vDragLayersCopy;
		// The root rectangle.
		LSW_MS_RECT							m_meRoot;
		// The copy of the root minus the widget that is being dragged.
		LSW_MS_RECT							m_meDragRootCopy;
		// Width of the drag bars.
		INT									m_iBarWidth;
		// Dragged layer.
		LSW_MS_LAYER *						m_pmlDragLayer;
		// Index into the layer for the bar being dragged.
		size_t								m_sDragBarIndex;
		// Point of the mouse click for starting the drag of a bar.
		POINT								m_pDragStart;
		// Last-drawn point.
		POINT								m_pLastPoint;
		// Last mouse-move point.
		POINT								m_pLastMouseMove;
		// Set when the mouse moves, unset when SetCursor() is called.
		bool								m_bSetCursorToggle;
		// IDs.
		static DWORD						m_dwIds;



		// == Functions.
		// Draw the XOR bar.
		void								DrawXorBar( HDC _hDc, INT _iX1, INT _iY1, INT _iWidth, INT _iHeight );

		// Draw the bar for this control.
		void								DrawBar( const POINT &_pPoint );

		// Calculates the new X coordinate of a bar from dragging.  Uses m_pmlDragLayer, m_sDragBarIndex, and m_pDragStart.
		INT									GetNewBarX( const POINT &_pCurPoint );

		// Calculates the new Y coordinate of a bar from dragging.  Uses m_pmlDragLayer, m_sDragBarIndex, and m_pDragStart.
		INT									GetNewBarY( const POINT &_pCurPoint );

		// Calculates the X delta of a bar from dragging.  Uses m_pDragStart.
		INT									GetBarXDelta( const POINT &_pCurPoint );

		// Calculates the Y delta of a bar from dragging.  Uses m_pDragStart.
		INT									GetBarYDelta( const POINT &_pCurPoint );

		// Finds a rectangle by its ID.
		bool								FindRectById( DWORD _dwId, LSW_MS_LAYER_SEARCH &_mlsRet, LSW_MS_RECT &_mrRoot );

		// Finds a rectangle by its ID.
		bool								FindRectById( DWORD _dwId, LSW_MS_LAYER * _pmlLayer, LSW_MS_LAYER_SEARCH &_mlsRet );

		// Remove a child.
		virtual void						RemoveChild( const CWidget * _pwChild );

		// Adds a layer under a given rectangle.
		LSW_MS_LAYER *						AddLayer( LSW_MS_RECT &_mrRect, CWidget * _pwWidget );

		// Fills an LSW_MS_DETACH structure with information necessary to remove a widget with the given ID.
		void								GetDetachmentInfo( WORD _wId, LSW_MS_DETACH &_mdDetach, LSW_MS_LAYER &_mlSearchLayer );

		// Calculates all of the rectangles.
		void								CalcRects();

		// Calculate rectangles.
		void								CalcRects( LSW_MS_LAYER &_mlLayer, const LSW_RECT &_rRect );

		// Calculates a single rectangle.
		LSW_RECT							CalcRect( LSW_MS_RECT &_mrRect, const LSW_RECT &_rRect, LSW_SPLIT_TYPE _stType, size_t _stTotalRectsInLayer, size_t _stRectIndexInLayer );

		// Does the given attachment type work with the split type of a layer?
		bool								SplitTypeMatchesLayerDirection( LSW_ATTACH_TYPE _atType, LSW_MS_LAYER * _plLayer );

		// Gets the layer and index of a bar being clicked.  If no bar was clicked, false is returned.
		bool								GetLayerAndBarBeingClicked( const POINT &_pClick, LSW_MS_LAYER * &_pmlLayer, size_t &_sIndex );

		// Gets the layer and index of a bar being clicked.  If no bar was clicked, false is returned.
		bool								GetLayerAndBarBeingClicked( const POINT &_pClick, LSW_MS_LAYER * &_pmlLayer, size_t &_sIndex, LSW_MS_LAYER &_mlCheckLayer );

		// Gets the rectangle for a bar index on a given layer.
		LSW_RECT							GetBarRect( LSW_MS_LAYER &_mlLayer, size_t _sIndex ) const;

		// WM_SIZE.
		virtual LSW_HANDLED					Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight );

		// WM_LBUTTONDOWN.
		virtual LSW_HANDLED					LButtonDown( DWORD _dwVirtKeys, const POINTS &_pCursorPos );

		// WM_MOUSEMOVE.
		virtual LSW_HANDLED					MouseMove( DWORD _dwVirtKeys, const POINTS &_pCursorPos );

		// WM_LBUTTONUP.
		virtual LSW_HANDLED					LButtonUp( DWORD _dwVirtKeys, const POINTS &_pCursorPos );

		// WM_SETCURSOR.
		virtual LSW_HANDLED					SetCursor( CWidget * _pwControl, WORD _wHitTest, WORD _wIdent );

		// Size all attachments.
		void								SizeAttachments() const;

		// Given a layer, index, side, and size (as a suggestion), generate a center point and a possible docking rectangle for the given side.
		void								GenerateCenterPointAndAttachmentRect( const LSW_MS_LAYER &_mlLayer, size_t _sIndex, LSW_ATTACH_TYPE _atAttachType, INT _iSizeSuggestion,
			POINT &_pCenterPoint, LSW_RECT &_rAttachRect );

		// Given a layer, index, side, and size (as a suggestion), generate a center point and a possible docking rectangle for the given side.
		void								GenerateCenterPointAndAttachmentRect( const LSW_RECT &_rLayerRect, const LSW_RECT &_rRect, INT _iDist, LSW_ATTACH_TYPE _atAttachType, INT _iSizeSuggestion,
			POINT &_pCenterPoint, LSW_RECT &_rAttachRect );

		// Given a point, an LSW_DT_ATTACH structure is filled that another control could use to
		//	add a control into this splitter.  Also returned is a rectangle that can be used to
		//	draw an attach point during a drag operation.
		// In other words, a user drags a dockable control over this multi-splitter and this tells
		//	it where it can add the control and gives it a rectangle to draw.
		// The distance from the given point to the attachment point is returned.  This is always
		//	the distance to the closest attachment point, and the caller can use this to decide if
		//	attaching a control is possible.  Whatever control is being dragged should be dragged
		//	close enough to an attachment point for attachment to be possible.
		LONG								GetAttachPoint( const POINT &_pPoint, LSW_DT_ATTACH &_maAttach, LSW_RECT &_rAttachRect,
			const LSW_MS_LAYER &_mlLayer, INT _iSizeSuggestion );

		// Given a point, an LSW_DT_ATTACH structure is filled that another control could use to
		//	add a control into this splitter.  Also returned is a rectangle that can be used to
		//	draw an attach point during a drag operation.
		// In other words, a user drags a dockable control over this multi-splitter and this tells
		//	it where it can add the control and gives it a rectangle to draw.
		// The distance from the given point to the attachment point is returned.  This is always
		//	the distance to the closest attachment point, and the caller can use this to decide if
		//	attaching a control is possible.  Whatever control is being dragged should be dragged
		//	close enough to an attachment point for attachment to be possible.
		LONG								GetAttachPoint( const POINT &_pPoint, LSW_DT_ATTACH &_maAttach, LSW_RECT &_rAttachRect,
			const LSW_MS_LAYER &_mlLayer, size_t _sIndex, LSW_ATTACH_TYPE _atAttachType, INT _iSizeSuggestion );

		// Given a point, an LSW_DT_ATTACH structure is filled that another control could use to
		//	add a control into this splitter.  Also returned is a rectangle that can be used to
		//	draw an attach point during a drag operation.
		// In other words, a user drags a dockable control over this multi-splitter and this tells
		//	it where it can add the control and gives it a rectangle to draw.
		// The distance from the given point to the attachment point is returned.  This is always
		//	the distance to the closest attachment point, and the caller can use this to decide if
		//	attaching a control is possible.  Whatever control is being dragged should be dragged
		//	close enough to an attachment point for attachment to be possible.
		LONG								GetAttachPoint( const POINT &_pPoint, LSW_DT_ATTACH &_maAttach, LSW_RECT &_rAttachRect,
			const LSW_RECT &_rLayerRect, const LSW_RECT &_rRect, INT _iDist, DWORD _dwId, LSW_ATTACH_TYPE _atAttachType, INT _iSizeSuggestion );

		// Gets the parent layer given a layer pointer.
		LSW_MS_LAYER *						GetParentLayer( LSW_MS_LAYER * _pmlLayer, size_t &_sindex );

		// Gets the parent layer given a layer pointer.
		LSW_MS_LAYER *						GetParentLayerRecursive( LSW_MS_LAYER * _pmlCheckLayer, LSW_MS_LAYER * _pmlLayer, size_t &_sindex );

		// Attach a widget to the given root.
		bool								Attach( const LSW_DT_ATTACH &_maAttach, LSW_MS_RECT &_mrRoot );

		// Detaches a widget from the given root given its ID.
		bool								Detach( WORD _wId, LSW_MS_RECT &_mrRoot );

		// Copies the layer tree with a specific layer removed so that it can be dragged elsewhere.
		bool								CopyLayerTreeForDrag();
	};

}	// namespace lsw
