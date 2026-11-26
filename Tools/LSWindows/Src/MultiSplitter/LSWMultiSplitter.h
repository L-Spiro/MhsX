#pragma once
#include "../LSWWin.h"
#include "../Docking/LSWDockTarget.h"
#include "../Layout/LSWWidgetLayout.h"
#include "../Widget/LSWWidget.h"


namespace lsw {

	class CTab;

	/**
	 * A window split into resizable areas, into which dockables can be docked.  Dockables can also be docked over each other
	 *	(LSW_AT_TAB), forming a group that shows one of them at a time, with tabs to choose between them.
	 **/
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

		/**
		 * Brings a docked widget to the front: if it is in a group of tabbed dockables, its tab is selected.
		 *
		 * \param _pwWidget The widget.
		 * \return Returns true if the widget is docked here.
		 **/
		bool								SelectDocked( const CWidget * _pwWidget );

		/**
		 * Notified when the selected tab of a tab control changes.  Changes in the tabs of groups of docked widgets are handled here
		 *	and not passed on.
		 *
		 * \param _pwTab The tab control.
		 * \param _iTab The selected tab.
		 * \param _lpnHdr The notification header.
		 **/
		virtual void						TabSelChanged( CWidget * _pwTab, int _iTab, LPNMHDR _lpnHdr );

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

		/**
		 * Called when a dockable starts being dragged.  A copy of the layout is made with the dragged widget detached from it (if it
		 *	is attached here), exactly as Detach() would leave it, and GetAttachPoint() works on that copy until EndDragPreview() is
		 *	called.  The previews then show where the widget will go once it has been taken out of its current place.
		 *
		 * \param _pwDragging The widget being dragged.
		 **/
		virtual void						BeginDragPreview( const CWidget * _pwDragging );

		/**
		 * Frees the copy of the layout made by BeginDragPreview().  GetAttachPoint() goes back to using the actual layout.
		 **/
		virtual void						EndDragPreview();

		// Determines the type of control this is.
		virtual uint32_t					WidgetType() const { return LSW_LT_MULTISPLITTER; }

		// Returns true if this is a CMultiSplitter class.
		virtual bool						IsMultiSplitter() const { return true; }

		/**
		 * To show the default cursor in the main client area (not a drag bar) of the splitter or not.
		 * 
		 * \param _bShow A boolean indicating whether to show the default cursor in t
		 * \return Returns the previous show-cursor value.
		 **/
		bool								ShowCursor( bool _bShow ) {
			auto bPrev = m_bShowCursor;
			m_bShowCursor = _bShow;
			return bPrev;
		}

		/**
		 * Handles WM_ERASEBKGND.
		 * \brief Allows custom background erasing.
		 *
		 * \param _hDc Device context provided for erasing.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					EraseBkgnd( HDC /*_hDc*/ ) { return LSW_H_HANDLED; }

		/**
		 * Handles WM_PAINT.
		 * \brief Performs painting for the client area.
		 *
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					Paint();


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

		/** A group of widgets docked over each other, with a tab control to choose which one is shown. */
		struct LSW_MS_GROUP {
			CTab *							ptTab = nullptr;		/**< The tab control, which is what occupies the group's rectangle. */
			std::vector<CWidget *>			vMembers;				/**< The widgets in the group, in the order of their tabs.  They are
																		 children of the splitter, shown over the tab control's page area. */
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
		/** If true, m_meDragRootCopy and m_vDragLayersCopy hold the layout to use for drag previews (see BeginDragPreview()). */
		bool								m_bDragPreview = false;
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
		// Show the default cursor?
		bool								m_bShowCursor = false;
		/** The groups of tabbed widgets. */
		std::vector<LSW_MS_GROUP>			m_vGroups;
		/** Set while the splitter is being destroyed (groups are then left as they are). */
		bool								m_bDestroying = false;
		// IDs.
		static DWORD						m_dwIds;
		/** The control ID given to the next group's tab control. */
		static WORD							m_wNextGroupId;



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

		/**
		 * Detaches a widget from a layout tree given its ID.  Used both for the actual layout and for the copy used for drag
		 *	previews, so that the copy always ends up exactly as the actual layout will.  Rectangles are not recalculated.
		 *
		 * \param _wId The ID of the widget to detach.
		 * \param _mrRoot The root of the tree.
		 * \param _vLayers All of the layers in the tree.  Layers removed from the tree are deleted and removed from this list.
		 * \return Returns true if the widget was found and detached.
		 **/
		bool								DetachFromTree( WORD _wId, LSW_MS_RECT &_mrRoot, std::vector<LSW_MS_LAYER *> &_vLayers );

		/**
		 * Makes a deep copy of a layer and all of the layers under it.
		 *
		 * \param _mlSrc The layer to copy.
		 * \param _vLayers Receives every layer created (they belong to the caller even if the copy fails).
		 * \return Returns the copy of _mlSrc, or nullptr on failure.
		 **/
		static LSW_MS_LAYER *				CloneLayer( const LSW_MS_LAYER &_mlSrc, std::vector<LSW_MS_LAYER *> &_vLayers );

		/**
		 * Gets the root to use for finding attachment points: the drag-preview copy during a drag, otherwise the actual root.
		 *
		 * \return Returns the root of the layout used for attachment points.
		 **/
		const LSW_MS_RECT &					AttachRoot() const { return m_bDragPreview ? m_meDragRootCopy : m_meRoot; }

		/**
		 * Copies the layer tree with a widget removed so that it can be dragged elsewhere (into m_meDragRootCopy and
		 *	m_vDragLayersCopy), and calculates the rectangles of the copy.
		 *
		 * \param _pwDragging The widget being dragged.  If it is not attached here, the copy is the same as the actual layout.
		 * \return Returns true if the copy was made.
		 **/
		bool								CopyLayerTreeForDrag( const CWidget * _pwDragging );

		/**
		 * Determines whether widgets can be docked over a widget as tabs (it is a dockable or a group of them).
		 *
		 * \param _pwWidget The widget in a rectangle.
		 * \return Returns true if a dockable can be tabbed over it.
		 **/
		bool								CanTabOnto( const CWidget * _pwWidget ) const;

		/**
		 * Gets the attachment point for docking over the widget in a rectangle as a tab.
		 *
		 * \param _pPoint The point (client coordinates).
		 * \param _maAttach Receives the attachment.
		 * \param _rAttachRect Receives the rectangle to draw (client coordinates).
		 * \param _mrRect The rectangle.
		 * \return Returns the squared distance from the point to the center of the rectangle, or LONG_MAX if nothing can be tabbed
		 *	there.
		 **/
		LONG								GetTabAttachPoint( const POINT &_pPoint, LSW_DT_ATTACH &_maAttach, LSW_RECT &_rAttachRect,
			const LSW_MS_RECT &_mrRect ) const;

		/**
		 * Docks a widget over the widget in a rectangle, making a group if there is not one there yet.
		 *
		 * \param _maAttach The attachment (LSW_AT_TAB).
		 * \param _mrRect The rectangle.
		 * \return Returns true if the widget was docked.
		 **/
		bool								AttachAsTab( const LSW_DT_ATTACH &_maAttach, LSW_MS_RECT &_mrRect );

		/**
		 * Finds the group whose tab control is a widget.
		 *
		 * \param _pwTab The widget.
		 * \return Returns the index of the group, or SIZE_MAX.
		 **/
		size_t								FindGroup( const CWidget * _pwTab ) const;

		/**
		 * Finds the group that contains a widget.
		 *
		 * \param _wId The ID of the widget.
		 * \param _sMember Receives the index of the widget in the group.
		 * \return Returns the index of the group, or SIZE_MAX.
		 **/
		size_t								FindGroupOfMember( WORD _wId, size_t &_sMember ) const;

		/**
		 * Creates an empty group (with its tab control).
		 *
		 * \return Returns the index of the new group, or SIZE_MAX on failure.
		 **/
		size_t								CreateGroup();

		/**
		 * Adds a widget to a group and shows it.
		 *
		 * \param _sGroup The index of the group.
		 * \param _pwWidget The widget.
		 * \return Returns true if it was added.
		 **/
		bool								AddToGroup( size_t _sGroup, CWidget * _pwWidget );

		/**
		 * Removes a widget from the group it is in.  A group left with one widget is replaced by that widget.
		 *
		 * \param _wId The ID of the widget.
		 * \return Returns true if the widget was in a group.
		 **/
		bool								RemoveFromGroup( WORD _wId );

		/**
		 * Finds the rectangle that holds a widget.
		 *
		 * \param _pwWidget The widget.
		 * \param _mrRoot The root of the tree to search.
		 * \return Returns the rectangle, or nullptr.
		 **/
		static LSW_MS_RECT *				FindCellByWidget( const CWidget * _pwWidget, LSW_MS_RECT &_mrRoot );

		/**
		 * Finds the rectangle that holds a widget in a layer.
		 *
		 * \param _pwWidget The widget.
		 * \param _mlLayer The layer to search.
		 * \return Returns the rectangle, or nullptr.
		 **/
		static LSW_MS_RECT *				FindCellByWidget( const CWidget * _pwWidget, LSW_MS_LAYER &_mlLayer );

		/**
		 * Moves the tab control of each group to its rectangle (the widgets in a group size themselves from ClientRect()).
		 **/
		void								SizeGroups();
	};

}	// namespace lsw
