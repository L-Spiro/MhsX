#pragma once
#include "../LSWWin.h"
#include "../Layout/LSWWidgetLayout.h"
#include "../Widget/LSWWidget.h"


namespace lsw {

	class CMultiSplitter : public CWidget {
	public :
		CMultiSplitter( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL );
		~CMultiSplitter();


		// == Enumerations.
		// A split type.
		enum LSW_SPLIT_TYPE : DWORD {
			LSW_ST_HOR,
			LSW_ST_VER,
		};

		// Attach type.
		enum LSW_ATTACH_TYPE {
			LSW_AT_LEFT,
			LSW_AT_RIGHT,
			LSW_AT_TOP,
			LSW_AT_BOTTOM,
		};

		// == Types.
		// Attaches a widget.
		struct LSW_MS_ATTACH {
			CWidget *						pwWidget;
			DWORD							dwId;
			LSW_ATTACH_TYPE					atAttachTo;
		};


		// == Functions.
		// Attach a widget.
		bool								Attach( const LSW_MS_ATTACH &_maAttach );

		// Root ID.
		DWORD								RootId() const { return m_meRoot.dwId; }

		// Client rectangle.
		virtual const LSW_RECT				ClientRect( const CWidget * pwChild ) const;


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


		// == Members.
		// All of the layers, in any order.
		std::vector<LSW_MS_LAYER *>			m_vLayers;

		// The root rectangle.
		LSW_MS_RECT							m_meRoot;

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

		// Finds a rectangle by its ID.
		bool								FindRectById( DWORD _dwId, LSW_MS_LAYER_SEARCH &_mlsRet );

		// Finds a rectangle by its ID.
		bool								FindRectById( DWORD _dwId, LSW_MS_LAYER * _pmlLayer, LSW_MS_LAYER_SEARCH &_mlsRet );

		// Remove a child.
		virtual void						RemoveChild( const CWidget * _pwChild );

		// Adds a layer under a given rectangle.
		LSW_MS_LAYER *						AddLayer( LSW_MS_RECT &_mrRect, CWidget * _pwWidget );

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

		// Size all attachments.
		void								SizeAttachments() const;
	};

}	// namespace lsw
