#include "LSWMultiSplitter.h"
#include "../Base/LSWBase.h"
#include "../Brush/LSWBrush.h"
#include "../Docking/LSWDockable.h"
#include "../Images/LSWBitmap.h"

namespace lsw {

	// == Members.
	// IDs.
	DWORD CMultiSplitter::m_dwIds = 0;

	CMultiSplitter::CMultiSplitter( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		CWidget( _wlLayout.ChangeClass( reinterpret_cast<LPCWSTR>(CBase::MultiSplitterAtom()) ), _pwParent, _bCreateWidget, _hMenu, _ui64Data ),
		m_iBarWidth( 4 ),
		m_pmlDragLayer( nullptr ),
		m_sDragBarIndex( 0 ),
		m_bSetCursorToggle( false )/*,
		m_iHorPos( 10 ),
		m_iVertPos( 10 )*/ {
		m_meRoot.u.pmlLayer = nullptr;
		m_meRoot.dwId = ++m_dwIds;
		m_meRoot.iDist = 500;
		m_meRoot.bContainsWidget = FALSE;
	}
	CMultiSplitter::~CMultiSplitter() {
		for ( size_t I = m_vLayers.size(); I--; ) {
			LSW_MS_LAYER * pmlTemp = m_vLayers[I];
			m_vLayers[I] = nullptr;
			delete pmlTemp;
		}
		m_vLayers.clear();


		for ( size_t I = m_vDragLayersCopy.size(); I--; ) {
			LSW_MS_LAYER * pmlTemp = m_vDragLayersCopy[I];
			m_vDragLayersCopy[I] = nullptr;
			delete pmlTemp;
		}
		m_vDragLayersCopy.clear();

		
	}

	// == Functions.
	// Attach a widget.
	bool CMultiSplitter::Attach( const LSW_DT_ATTACH &_maAttach ) {
		return Attach( _maAttach, m_meRoot );
	}

	// Detaches a widget given its ID.
	bool CMultiSplitter::Detach( WORD _wId ) {
		return Detach( _wId, m_meRoot );
	}

	// Client rectangle.
	LSW_RECT CMultiSplitter::ClientRect( const CWidget * pwChild ) const {
		for ( size_t I = 0; I < m_vLayers.size(); ++I ) {
			for ( size_t J = 0; J < m_vLayers[I]->vRects.size(); ++J ) {
				if ( m_vLayers[I]->vRects[J].bContainsWidget && m_vLayers[I]->vRects[J].u.pqWidget == pwChild ) {
					return m_vLayers[I]->vRects[J].rRect;
				}
			}
		}
		return m_meRoot.rRect;
	}

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
	LONG CMultiSplitter::GetAttachPoint( const POINT &_pPoint, LSW_DT_ATTACH &_maAttach, INT _iSizeSuggestion,
		LSW_RECT &_rDrawRect ) {
		_maAttach.pwWidget = nullptr;

		POINT pClientPoint = _pPoint;
		::ScreenToClient( Wnd(), &pClientPoint );


		// If there is nothing attached.
		if ( !m_meRoot.u.pmlLayer && !m_meRoot.bContainsWidget ) {
			// User can drag anywhere.  The whole rectangle is returned.
			_maAttach.atAttachTo = LSW_AT_LEFT;			// Ignored when you attach the first widget.
			_maAttach.dwId = RootId();
			_rDrawRect = ClientRect( this );
			_rDrawRect = _rDrawRect.MapToDeskTop( Wnd() );
			return 0;
		}

		// If only one widget is attached.
		if ( m_meRoot.u.pqWidget && m_meRoot.bContainsWidget ) {
			LONG lWinDist = LONG_MAX;
			// Check each side of the widget.
			static const LSW_ATTACH_TYPE atAttaches[] = {
				LSW_AT_LEFT,
				LSW_AT_RIGHT,
				LSW_AT_TOP,
				LSW_AT_BOTTOM,
			};
			LSW_RECT rRect = ClientRect( this );
			for ( size_t I = 0; I < LSW_ELEMENTS( atAttaches ); ++I ) {
				LSW_DT_ATTACH maAttach;
				INT iDist = 500;
				if ( atAttaches[I] == LSW_AT_LEFT || atAttaches[I] == LSW_AT_RIGHT ) {
					iDist = std::min( iDist, static_cast<INT>(rRect.Width()) );
				}
				else {
					iDist = std::min( iDist, static_cast<INT>(rRect.Height()) );
				}
				LSW_RECT rAttachrectTemp;
				LONG lDist = GetAttachPoint( pClientPoint, maAttach, rAttachrectTemp,
					rRect, rRect, iDist, RootId(), atAttaches[I], _iSizeSuggestion );
				if ( lDist < lWinDist ) {
					lWinDist = lDist;
					_maAttach = maAttach;
					_rDrawRect = rAttachrectTemp;
				}
			}
			_rDrawRect = _rDrawRect.MapToDeskTop( Wnd() );
			return lWinDist;
		}

		// If more than 1 widget is attached.
		if ( m_meRoot.u.pmlLayer && !m_meRoot.bContainsWidget ) {
			LONG lRet = GetAttachPoint( pClientPoint, _maAttach, _rDrawRect,
				(*m_meRoot.u.pmlLayer), _iSizeSuggestion );
			_rDrawRect = _rDrawRect.MapToDeskTop( Wnd() );
			return lRet;
		}

		// Can never get hit.
		return LONG_MAX;
	}

	// Draw the XOR bar.
	void CMultiSplitter::DrawXorBar( HDC _hDc, INT _iX1, INT _iY1, INT _iWidth, INT _iHeight ) {
		static WORD wDotPattern[8] = {
			0x00AA, 0x0055, 0x00AA, 0x0055,
			0x00AA, 0x0055, 0x00AA, 0x0055
		};

		CBitmap bBitmap;
		bBitmap.CreateBitmap( 8, 8, 1, 1, wDotPattern );
		CBrush bBrush;
		bBrush.CreatePatternBrush( bBitmap );
		::SetBrushOrgEx( _hDc, _iX1, _iY1, NULL );

		LSW_SELECTOBJECT soBrushOrig( _hDc, bBrush.Handle() );	// Destructor sets the original brush back.

		::PatBlt( _hDc, _iX1, _iY1, _iWidth, _iHeight, PATINVERT );
	}

	// Draw the bar for this control.
	void CMultiSplitter::DrawBar( const POINT &_pPoint ) {
		if ( !m_pmlDragLayer ) { return; }

		HDC hDc = ::GetWindowDC( Wnd() );

		LSW_RECT rCheckRect = GetBarRect( (*m_pmlDragLayer), m_sDragBarIndex );
		INT iX, iY, iWidth, iHeight;
		if ( m_pmlDragLayer->stSplitType == LSW_ST_HOR ) {
			iX = static_cast<INT>((*m_pmlDragLayer).vRects[m_sDragBarIndex].rRect.left);
			iY = GetBarYDelta( _pPoint );//GetNewBarY( _pPoint );
			iY += (*m_pmlDragLayer).vRects[m_sDragBarIndex].rRect.bottom;
			iWidth = (*m_pmlDragLayer).vRects[m_sDragBarIndex].rRect.Width();
			iHeight = m_iBarWidth;
		}
		else {
			iY = static_cast<INT>((*m_pmlDragLayer).vRects[m_sDragBarIndex].rRect.top);
			iX = GetBarXDelta( _pPoint );//GetNewBarX( _pPoint );
			iX += (*m_pmlDragLayer).vRects[m_sDragBarIndex].rRect.right;
			iWidth = m_iBarWidth;
			iHeight = (*m_pmlDragLayer).vRects[m_sDragBarIndex].rRect.Height();
		}

		DrawXorBar( hDc, iX, iY, iWidth, iHeight );

		::ReleaseDC( Wnd(), hDc );

		m_pLastPoint = _pPoint;
	}

	// Calculates the new X coordinate of a bar from dragging.  Uses m_pmlDragLayer, m_sDragBarIndex, and m_pDragStart.
	INT CMultiSplitter::GetNewBarX( const POINT &_pCurPoint ) {
		LONG lChange = GetBarXDelta( _pCurPoint );
		INT iX = static_cast<INT>(std::max( 0L, (*m_pmlDragLayer).vRects[m_sDragBarIndex].iDist + lChange ));
		return iX;
	}

	// Calculates the new Y coordinate of a bar from dragging.  Uses m_pmlDragLayer, m_sDragBarIndex, and m_pDragStart.
	INT CMultiSplitter::GetNewBarY( const POINT &_pCurPoint ) {
		LONG lChange = GetBarYDelta( _pCurPoint );
		INT iY = static_cast<INT>(std::max( 0L, (*m_pmlDragLayer).vRects[m_sDragBarIndex].iDist + lChange ));
		return iY;
	}

	// Calculates the X delta of a bar from dragging.  Uses m_pDragStart.
	INT CMultiSplitter::GetBarXDelta( const POINT &_pCurPoint ) { return _pCurPoint.x - m_pDragStart.x; }

	// Calculates the Y delta of a bar from dragging.  Uses m_pDragStart.
	INT CMultiSplitter::GetBarYDelta( const POINT &_pCurPoint ) { return _pCurPoint.y - m_pDragStart.y; }

	// Finds a rectangle by its ID.
	bool CMultiSplitter::FindRectById( DWORD _dwId, LSW_MS_LAYER_SEARCH &_mlsRet, LSW_MS_RECT &_mrRoot ) {
		if ( _mrRoot.dwId == _dwId ) {
			_mlsRet.pmlLayer = nullptr;
			_mlsRet.sIndex = 0;
			_mlsRet.pmrRect = &_mrRoot;
			return true;
		}
		return FindRectById( _dwId, _mrRoot.u.pmlLayer, _mlsRet );
	}

	// Finds a rectangle by its ID.
	bool CMultiSplitter::FindRectById( DWORD _dwId, LSW_MS_LAYER * _pmlLayer, LSW_MS_LAYER_SEARCH &_mlsRet ) {
		if ( !_pmlLayer ) { return false; }
		
		for ( size_t I = 0; I < _pmlLayer->vRects.size(); ++I ) {
			if ( _pmlLayer->vRects[I].dwId == _dwId ) {
				_mlsRet.pmlLayer = _pmlLayer;
				_mlsRet.sIndex = I;
				_mlsRet.pmrRect = &_pmlLayer->vRects[I];
				return true;
			}
			if ( !_pmlLayer->vRects[I].bContainsWidget ) {
				bool bRet = FindRectById( _dwId, _pmlLayer->vRects[I].u.pmlLayer, _mlsRet );
				if ( bRet ) { return true; }
			}
		}
		return false;
	}

	// Remove a child.
	void CMultiSplitter::RemoveChild( const CWidget * _pwChild ) {
		/*for ( size_t I = m_vLayers.size(); I--; ) {
			for ( size_t J = m_vLayers[I]->vRects.size(); J--; ) {
				if ( m_vLayers[I]->vRects[J].bContainsWidget && m_vLayers[I]->vRects[J].u.pqWidget == _pwChild ) {
					Detach( _pwChild->Id() );
				}
			}
			//if ( m_vLayers[I]->vRects.size() 
		}*/
		Detach( _pwChild->Id() );
		CWidget::RemoveChild( _pwChild );
	}

	// Adds a layer under a given rectangle.
	CMultiSplitter::LSW_MS_LAYER * CMultiSplitter::AddLayer( LSW_MS_RECT &_mrRect, CWidget * _pwWidget ) {
		// If the rectangle doesn't actually point to anything (an empty root node), then
		//	just make it point to the given widget.
		if ( !_mrRect.u.pqWidget ) {
			_mrRect.u.pqWidget = _pwWidget;
			_mrRect.bContainsWidget = TRUE;
			_mrRect.bLockToFar = FALSE;
			return nullptr;
		}
		// Otherwise, create a new layer and point the rectangle to it.
		LSW_MS_LAYER * pmlLayer = new( std::nothrow ) LSW_MS_LAYER();
		if ( pmlLayer ) {
			m_vLayers.push_back( pmlLayer );
			pmlLayer->vRects.push_back( _mrRect );
			_mrRect.bContainsWidget = FALSE;
			_mrRect.u.pmlLayer = pmlLayer;
			_mrRect.dwId = ++m_dwIds;
		}
		return pmlLayer;
	}

	// Fills an LSW_MS_DETACH structure with information necessary to remove a widget with the given ID.
	void CMultiSplitter::GetDetachmentInfo( WORD _wId, LSW_MS_DETACH &_mdDetach, LSW_MS_LAYER &_mlSearchLayer ) {
		_mdDetach.pmlLayer = _mdDetach.pmlParentLayer = nullptr;
		_mdDetach.sIndex = _mdDetach.sParentIndex = 0;
		for ( size_t I = 0; I < _mlSearchLayer.vRects.size(); ++I ) {
			if ( _mlSearchLayer.vRects[I].bContainsWidget && _mlSearchLayer.vRects[I].u.pqWidget ) {
				if ( _mlSearchLayer.vRects[I].u.pqWidget->Id() == _wId ) {
					_mdDetach.sIndex = I;
					_mdDetach.pmlLayer = &_mlSearchLayer;
					return;
				}
			}
			else if ( !_mlSearchLayer.vRects[I].bContainsWidget && _mlSearchLayer.vRects[I].u.pmlLayer ) {
				GetDetachmentInfo( _wId, _mdDetach, (*_mlSearchLayer.vRects[I].u.pmlLayer) );
				if ( _mdDetach.pmlLayer && !_mdDetach.pmlParentLayer ) {
					_mdDetach.pmlParentLayer = &_mlSearchLayer;
					_mdDetach.sParentIndex = I;
					return;
				}
			}
		}
	}

	// Calculates all of the rectangles.
	void CMultiSplitter::CalcRects() {
		::GetClientRect( Wnd(), &m_meRoot.rRect );
		if ( m_meRoot.u.pmlLayer && !m_meRoot.bContainsWidget ) {
			CalcRects( (*m_meRoot.u.pmlLayer), m_meRoot.rRect );
		}
		//::RedrawWindow( Wnd(), NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_INTERNALPAINT | RDW_ALLCHILDREN | RDW_UPDATENOW | RDW_FRAME );
		/*::InvalidateRect( Wnd(), NULL, FALSE );
		::UpdateWindow( Wnd() );*/
	}

	// Calculate rectangles.
	void CMultiSplitter::CalcRects( LSW_MS_LAYER &_mlLayer, const LSW_RECT &_rRect ) {
		LSW_RECT rCopy = _rRect;
		_mlLayer.rRect = rCopy;
		// Sort the rectangles.
		std::vector<size_t> vRects;
		vRects.reserve( _mlLayer.vRects.size() );
		// Push left/top-aligned rectangles, insert(0) the rest.
		for ( size_t I = 0; I < _mlLayer.vRects.size(); ++I ) {
			if ( _mlLayer.vRects[I].bLockToFar ) {
				vRects.insert( vRects.begin() + 0, I );
			}
			else {
				vRects.push_back( I );
			}
			//vRects.push_back( I );
		}

		for ( size_t I = 0; I < _mlLayer.vRects.size(); ++I ) {
			size_t sIdx = vRects[I];
			_mlLayer.vRects[sIdx].rRect = CalcRect( _mlLayer.vRects[sIdx], rCopy, _mlLayer.stSplitType, _mlLayer.vRects.size(), I );
			if ( !_mlLayer.vRects[sIdx].bContainsWidget && _mlLayer.vRects[sIdx].u.pmlLayer ) {
				CalcRects( (*_mlLayer.vRects[sIdx].u.pmlLayer), _mlLayer.vRects[sIdx].rRect );
			}

			if ( _mlLayer.vRects[sIdx].bLockToFar ) {
				if ( _mlLayer.stSplitType == LSW_ST_VER ) {
					rCopy.right = _mlLayer.vRects[sIdx].rRect.left - m_iBarWidth;
				}
				else {
					rCopy.bottom = _mlLayer.vRects[sIdx].rRect.top - m_iBarWidth;
				}
			}
			else {
				if ( _mlLayer.stSplitType == LSW_ST_VER ) {
					rCopy.left = _mlLayer.vRects[sIdx].rRect.right + m_iBarWidth;
				}
				else {
					rCopy.top = _mlLayer.vRects[sIdx].rRect.bottom + m_iBarWidth;
				}
			}
		}
	}

	// Calculates a single rectangle.
	LSW_RECT CMultiSplitter::CalcRect( LSW_MS_RECT &_mrRect, const LSW_RECT &_rRect, LSW_SPLIT_TYPE _stType, size_t _stTotalRectsInLayer, size_t _stRectIndexInLayer ) {
		// If only 1 rectangle in the layer, the rectangle is the full size of the layer.
		if ( _stTotalRectsInLayer == 1 || _stRectIndexInLayer == _stTotalRectsInLayer - 1 ) {
			return _rRect;
		}
		LSW_RECT rCopy = _rRect;
		
		// Otherwise the top-left coordinate is already correct.  Ensure there is a drag bar to the right or bottom.
		if ( _stType == LSW_ST_VER ) {
			// Based on how many rectangles are in the layer and which rectangle this is we can determine
			//	the max right value.  The drag bars have to be clamped into the control.
			
			if ( _mrRect.bLockToFar ) {
				LONG lMaxRight = static_cast<LONG>(_rRect.left + m_iBarWidth * ((_stTotalRectsInLayer - 1) - _stRectIndexInLayer));
				rCopy.left = std::max( lMaxRight, (rCopy.right - _mrRect.iDist) );
				rCopy.left = std::min( rCopy.left, rCopy.right );
			}
			else {
				LONG lMaxRight = static_cast<LONG>(_rRect.right - m_iBarWidth * ((_stTotalRectsInLayer - 1) - _stRectIndexInLayer));
				rCopy.right = std::min( lMaxRight, (rCopy.left + _mrRect.iDist) );
				rCopy.right = std::max( rCopy.left, rCopy.right );
			}
		}
		else {
			// Same but going down.			
			if ( _mrRect.bLockToFar ) {
				LONG lMaxBottom = static_cast<LONG>(_rRect.top + m_iBarWidth * ((_stTotalRectsInLayer - 1) - _stRectIndexInLayer));
				rCopy.top = std::max( lMaxBottom, (rCopy.bottom - _mrRect.iDist) );
				rCopy.top = std::min( rCopy.top, rCopy.top );
			}
			else {
				LONG lMaxBottom = static_cast<LONG>(_rRect.bottom - m_iBarWidth * ((_stTotalRectsInLayer - 1) - _stRectIndexInLayer));
				rCopy.bottom = std::min( lMaxBottom, (rCopy.top + _mrRect.iDist) );
				rCopy.bottom = std::max( rCopy.top, rCopy.bottom );
			}
		}
		return rCopy;
	}

	// Does the given attachment type work with the split type of a layer?
	bool CMultiSplitter::SplitTypeMatchesLayerDirection( LSW_ATTACH_TYPE _atType, LSW_MS_LAYER * _plLayer ) {
		if ( !_plLayer ) { return false; }						// A layer must be created with the given direction.
		if ( _plLayer->vRects.size() <= 1 ) { return true; }	// This layer can be made to go in any direction.
		if ( _plLayer->stSplitType == LSW_ST_HOR ) { return _atType == LSW_AT_TOP || _atType == LSW_AT_BOTTOM; }
		if ( _plLayer->stSplitType == LSW_ST_VER ) { return _atType == LSW_AT_LEFT || _atType == LSW_AT_RIGHT; }
		return false;
	}

	// Gets the layer and index of a bar being clicked.  If no bar was clicked, false is returned.
	bool CMultiSplitter::GetLayerAndBarBeingClicked( const POINT &_pClick, LSW_MS_LAYER * &_pmlLayer, size_t &_sIndex ) {
		if ( m_meRoot.u.pmlLayer && !m_meRoot.bContainsWidget ) {
			return GetLayerAndBarBeingClicked( _pClick, _pmlLayer, _sIndex, (*m_meRoot.u.pmlLayer) );
		}
		return false;
	}

	// Gets the layer and index of a bar being clicked.  If no bar was clicked, false is returned.
	bool CMultiSplitter::GetLayerAndBarBeingClicked( const POINT &_pClick, LSW_MS_LAYER * &_pmlLayer, size_t &_sIndex, LSW_MS_LAYER &_mlCheckLayer ) {
		// 2 or more sections on this layer.
		for ( size_t I = 0; I < _mlCheckLayer.vRects.size(); ++I ) {
			LSW_RECT rCheckRect = GetBarRect( _mlCheckLayer, I );

			if ( rCheckRect.PointIsIn_FullInclusive( _pClick ) ) {
				_pmlLayer = &_mlCheckLayer;
				_sIndex = I;
				return true;
			}

			if ( !_mlCheckLayer.vRects[I].bContainsWidget ) {
				bool bFound = GetLayerAndBarBeingClicked( _pClick, _pmlLayer, _sIndex, (*_mlCheckLayer.vRects[I].u.pmlLayer) );
				if ( bFound ) { return true; }
			}
		}
		return false;
	}

	// Gets the rectangle for a bar index on a given layer.
	LSW_RECT CMultiSplitter::GetBarRect( LSW_MS_LAYER &_mlLayer, size_t _sIndex ) const {
		LSW_RECT rRect;
		if ( _mlLayer.stSplitType == LSW_ST_HOR ) {
			// Horizontal bar.
			rRect.left = _mlLayer.vRects[_sIndex].rRect.left;
			rRect.top = _mlLayer.vRects[_sIndex].rRect.bottom;
			rRect.right = _mlLayer.vRects[_sIndex].rRect.right;
			rRect.bottom = rRect.top + m_iBarWidth;
		}
		else {
			// Vertical bar.
			rRect.left = _mlLayer.vRects[_sIndex].rRect.right;
			rRect.top = _mlLayer.vRects[_sIndex].rRect.top;
			rRect.right = rRect.left + m_iBarWidth;
			rRect.bottom = _mlLayer.vRects[_sIndex].rRect.bottom;
		}
		return rRect;
	}

	// WM_SIZE.
	CWidget::LSW_HANDLED CMultiSplitter::Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight ) {
		CalcRects();
		return CWidget::Size( _wParam, _lWidth, _lHeight );
	}

	// WM_LBUTTONDOWN.
	CWidget::LSW_HANDLED CMultiSplitter::LButtonDown( DWORD /*_dwVirtKeys*/, const POINTS &_pCursorPos ) {
		// If we only have 0 or 1 child, do nothing.
		m_pmlDragLayer = nullptr;
		POINT pCurPos = { _pCursorPos.x, _pCursorPos.y };
		if ( GetLayerAndBarBeingClicked( pCurPos,
			m_pmlDragLayer, m_sDragBarIndex ) ) {
			m_pDragStart = pCurPos;

			DrawBar( pCurPos );
			::SetCapture( Wnd() );
		}
		return LSW_H_CONTINUE;
	}

	// WM_MOUSEMOVE.
	CWidget::LSW_HANDLED CMultiSplitter::MouseMove( DWORD /*_dwVirtKeys*/, const POINTS &_pCursorPos ) {
		m_pLastMouseMove = { _pCursorPos.x, _pCursorPos.y };
		m_bSetCursorToggle = true;
		if ( m_pmlDragLayer ) {
			POINT pCurPos = m_pLastMouseMove;
			DrawBar( m_pLastPoint );
			DrawBar( pCurPos );
		}

		return LSW_H_CONTINUE;
	}

	// WM_LBUTTONUP.
	CWidget::LSW_HANDLED CMultiSplitter::LButtonUp( DWORD /*_dwVirtKeys*/, const POINTS &_pCursorPos ) {
		if ( m_pmlDragLayer ) {
			DrawBar( m_pLastPoint );
			POINT pCurPos = { _pCursorPos.x, _pCursorPos.y };

			if ( m_pmlDragLayer->stSplitType == LSW_ST_HOR ) {
				//m_pmlDragLayer->vRects[m_sDragBarIndex].iDist = GetNewBarY( pCurPos );
				(*m_pmlDragLayer).vRects[m_sDragBarIndex].iDist = std::max( (*m_pmlDragLayer).vRects[m_sDragBarIndex].iDist + GetBarYDelta( pCurPos ), 0 );
				if ( ++m_sDragBarIndex < (*m_pmlDragLayer).vRects.size() ) {
					(*m_pmlDragLayer).vRects[m_sDragBarIndex].iDist = std::max( (*m_pmlDragLayer).vRects[m_sDragBarIndex].iDist - GetBarYDelta( pCurPos ), 0 );
				}
			}
			else {
				//m_pmlDragLayer->vRects[m_sDragBarIndex].iDist = GetNewBarX( pCurPos );
				(*m_pmlDragLayer).vRects[m_sDragBarIndex].iDist = std::max( (*m_pmlDragLayer).vRects[m_sDragBarIndex].iDist + GetBarXDelta( pCurPos ), 0 );
				if ( ++m_sDragBarIndex < (*m_pmlDragLayer).vRects.size() ) {
					(*m_pmlDragLayer).vRects[m_sDragBarIndex].iDist = std::max( (*m_pmlDragLayer).vRects[m_sDragBarIndex].iDist - GetBarXDelta( pCurPos ), 0 );
				}
			}
			CalcRects();
			SizeAttachments();

			m_pmlDragLayer = nullptr;
		}
		
		::ReleaseCapture();
		return LSW_H_CONTINUE;
	}

	// WM_SETCURSOR.
	CWidget::LSW_HANDLED CMultiSplitter::SetCursor( CWidget * /*_pwControl*/, WORD /*_wHitTest*/, WORD /*_wIdent*/ ) {
		HCURSOR hCursor = NULL;
		if ( m_bSetCursorToggle ) {
			LSW_MS_LAYER * pmlDragLayer = nullptr;
			size_t sDragBarIdx;
			if ( GetLayerAndBarBeingClicked( m_pLastMouseMove,
				pmlDragLayer, sDragBarIdx ) ) {
				switch ( pmlDragLayer->stSplitType ) {
					case LSW_ST_HOR : {
						hCursor = ::LoadCursor( NULL, IDC_SIZENS );
						break;
					}
					case LSW_ST_VER : {
						hCursor = ::LoadCursor( NULL, IDC_SIZEWE );
						break;
					}
				}
			}
		}
		::SetCursor( hCursor );
		m_bSetCursorToggle = false;
		return LSW_H_CONTINUE;
	}

	// Size all attachments.
	void CMultiSplitter::SizeAttachments() const {
		for ( size_t I = 0; I < m_vLayers.size(); ++I ) {
			for ( size_t J = 0; J < m_vLayers[I]->vRects.size(); ++J ) {
				if ( m_vLayers[I]->vRects[J].bContainsWidget && m_vLayers[I]->vRects[J].u.pqWidget ) {
					::SetWindowPos( m_vLayers[I]->vRects[J].u.pqWidget->Wnd(), NULL,
						m_vLayers[I]->vRects[J].rRect.left,
						m_vLayers[I]->vRects[J].rRect.top,
						m_vLayers[I]->vRects[J].rRect.Width(),
						m_vLayers[I]->vRects[J].rRect.Height(),
						SWP_NOACTIVATE | SWP_NOZORDER | SWP_DRAWFRAME | SWP_NOSENDCHANGING );
				}
			}
		}
	}

	// Given a layer, index, side, and size (as a suggestion), generate a center point and a possible docking rectangle for the given side.
	void CMultiSplitter::GenerateCenterPointAndAttachmentRect( const LSW_MS_LAYER &_mlLayer, size_t _sIndex, LSW_ATTACH_TYPE _atAttachType, INT _iSizeSuggestion,
		POINT &_pCenterPoint, LSW_RECT &_rAttachRect ) {
		return GenerateCenterPointAndAttachmentRect( _mlLayer.rRect, _mlLayer.vRects[_sIndex].rRect, _mlLayer.vRects[_sIndex].iDist, _atAttachType, _iSizeSuggestion,
		_pCenterPoint, _rAttachRect );
	}

	// Given a layer, index, side, and size (as a suggestion), generate a center point and a possible docking rectangle for the given side.
	void CMultiSplitter::GenerateCenterPointAndAttachmentRect( const LSW_RECT &_rLayerRect, const LSW_RECT &_rRect, INT _iDist, LSW_ATTACH_TYPE _atAttachType, INT _iSizeSuggestion,
		POINT &_pCenterPoint, LSW_RECT &_rAttachRect ) {
		switch ( _atAttachType ) {
			case LSW_AT_LEFT : {
				_rAttachRect.left = _rAttachRect.right = _rRect.left;
				_rAttachRect.top = _rRect.top;
				_rAttachRect.bottom = _rRect.bottom;
				_pCenterPoint = _rAttachRect.Center();
				LONG lAdjust = std::min( _iSizeSuggestion, _iDist );
				_rAttachRect.right += lAdjust;
				break;
			}
			case LSW_AT_RIGHT : {
				_rAttachRect.left = _rAttachRect.right = _rRect.right;
				_rAttachRect.top = _rRect.top;
				_rAttachRect.bottom = _rRect.bottom;
				_pCenterPoint = _rAttachRect.Center();
				_rAttachRect.right = std::min( _rAttachRect.left + _iSizeSuggestion, _rLayerRect.right );
				_rAttachRect.left = std::max( 0L, _rAttachRect.right - _iSizeSuggestion );
				break;
			}
			case LSW_AT_TOP : {
				_rAttachRect.top = _rAttachRect.bottom = _rRect.top;
				_rAttachRect.left = _rRect.left;
				_rAttachRect.right = _rRect.right;
				_pCenterPoint = _rAttachRect.Center();
				LONG lAdjust = std::min( _iSizeSuggestion, _iDist );
				_rAttachRect.bottom += lAdjust;
				break;
			}
			case LSW_AT_BOTTOM : {
				_rAttachRect.top = _rAttachRect.bottom = _rRect.bottom;
				_rAttachRect.left = _rRect.left;
				_rAttachRect.right = _rRect.right;
				_pCenterPoint = _rAttachRect.Center();
				_rAttachRect.bottom = std::min( _rAttachRect.top + _iSizeSuggestion, _rLayerRect.bottom );
				_rAttachRect.top = std::max( 0L, _rAttachRect.bottom - _iSizeSuggestion );
				break;
			}
		}
	}

	// Given a point, an LSW_DT_ATTACH structure is filled that another control could use to
	//	add a control into this splitter.  Also returned is a rectangle that can be used to
	//	draw an attach point during a drag operation.
	// In other words, a user drags a dockable control over this multi-splitter and this tells
	//	it where it can add the control and gives it a rectangle to draw.
	// The distance from the given point to the attachment point is returned.  This is always
	//	the distance to the closest attachment point, and the caller can use this to decide if
	//	attaching a control is possible.  Whatever control is being dragged should be dragged
	//	close enough to an attachment point for attachment to be possible.
	LONG CMultiSplitter::GetAttachPoint( const POINT &_pPoint, LSW_DT_ATTACH &_maAttach, LSW_RECT &_rAttachRect,
		const LSW_MS_LAYER &_mlLayer, INT _iSizeSuggestion ) {
		LONG lWinDist = LONG_MAX;
		static const LSW_ATTACH_TYPE atAttaches[] = {
			LSW_AT_LEFT,
			LSW_AT_RIGHT,
			LSW_AT_TOP,
			LSW_AT_BOTTOM,
		};
		for ( size_t I = 0; I < _mlLayer.vRects.size(); ++I ) {
			for ( size_t J = 0; J < LSW_ELEMENTS( atAttaches ); ++J ) {
				LSW_DT_ATTACH maAttach;
				LSW_RECT rAttachRectTemp;
				LONG lDist = GetAttachPoint( _pPoint, maAttach, rAttachRectTemp,
					_mlLayer, I, atAttaches[J], _iSizeSuggestion );
				if ( lDist < lWinDist ) {
					lWinDist = lDist;
					_maAttach = maAttach;
					_rAttachRect = rAttachRectTemp;
				}
			}
			if ( !_mlLayer.vRects[I].bContainsWidget && _mlLayer.vRects[I].u.pmlLayer ) {
				LSW_DT_ATTACH maAttach;
				LSW_RECT rAttachRectTemp;
				LONG lDist = GetAttachPoint( _pPoint, maAttach, rAttachRectTemp,
					(*_mlLayer.vRects[I].u.pmlLayer), _iSizeSuggestion );
				if ( lDist < lWinDist ) {
					lWinDist = lDist;
					_maAttach = maAttach;
					_rAttachRect = rAttachRectTemp;
				}
			}
		}
		return lWinDist;
	}

	// Given a point, an LSW_DT_ATTACH structure is filled that another control could use to
	//	add a control into this splitter.  Also returned is a rectangle that can be used to
	//	draw an attach point during a drag operation.
	// In other words, a user drags a dockable control over this multi-splitter and this tells
	//	it where it can add the control and gives it a rectangle to draw.
	// The distance from the given point to the attachment point is returned.  This is always
	//	the distance to the closest attachment point, and the caller can use this to decide if
	//	attaching a control is possible.  Whatever control is being dragged should be dragged
	//	close enough to an attachment point for attachment to be possible.
	LONG CMultiSplitter::GetAttachPoint( const POINT &_pPoint, LSW_DT_ATTACH &_maAttach, LSW_RECT &_rAttachRect,
		const LSW_MS_LAYER &_mlLayer, size_t _sIndex, LSW_ATTACH_TYPE _atAttachType, INT _iSizeSuggestion ) {
		return GetAttachPoint( _pPoint, _maAttach, _rAttachRect,
			_mlLayer.rRect, _mlLayer.vRects[_sIndex].rRect, _mlLayer.vRects[_sIndex].iDist, _mlLayer.vRects[_sIndex].dwId,
			_atAttachType, _iSizeSuggestion );
	}

	// Given a point, an LSW_DT_ATTACH structure is filled that another control could use to
	//	add a control into this splitter.  Also returned is a rectangle that can be used to
	//	draw an attach point during a drag operation.
	// In other words, a user drags a dockable control over this multi-splitter and this tells
	//	it where it can add the control and gives it a rectangle to draw.
	// The distance from the given point to the attachment point is returned.  This is always
	//	the distance to the closest attachment point, and the caller can use this to decide if
	//	attaching a control is possible.  Whatever control is being dragged should be dragged
	//	close enough to an attachment point for attachment to be possible.
	LONG CMultiSplitter::GetAttachPoint( const POINT &_pPoint, LSW_DT_ATTACH &_maAttach, LSW_RECT &_rAttachRect,
		const LSW_RECT &_rLayerRect, const LSW_RECT &_rRect, INT _iDist, DWORD _dwId, LSW_ATTACH_TYPE _atAttachType, INT _iSizeSuggestion ) {
		POINT pCenterPoint;
		GenerateCenterPointAndAttachmentRect( _rLayerRect, _rRect, _iDist, _atAttachType, _iSizeSuggestion,
			pCenterPoint, _rAttachRect );
		_maAttach.atAttachTo = _atAttachType;
		_maAttach.dwId = _dwId;
		_maAttach.pwWidget = nullptr;
		LONG lX = _pPoint.x - pCenterPoint.x;
		LONG lY = _pPoint.y - pCenterPoint.y;
		return lX * lX + lY * lY;
	}

	// Gets the parent layer given a layer pointer.
	CMultiSplitter::LSW_MS_LAYER * CMultiSplitter::GetParentLayer( LSW_MS_LAYER * _pmlLayer, size_t &_sindex ) {
		if ( !m_meRoot.bContainsWidget && m_meRoot.u.pmlLayer ) {
			return GetParentLayerRecursive( m_meRoot.u.pmlLayer, _pmlLayer, _sindex );
		}
		return nullptr;
	}

	// Gets the parent layer given a layer pointer.
	CMultiSplitter::LSW_MS_LAYER * CMultiSplitter::GetParentLayerRecursive( LSW_MS_LAYER * _pmlCheckLayer, LSW_MS_LAYER * _pmlLayer, size_t &_sindex ) {
		for ( size_t I = 0; I < _pmlCheckLayer->vRects.size(); ++I ) {
			if ( !_pmlCheckLayer->vRects[I].bContainsWidget && _pmlCheckLayer->vRects[I].u.pmlLayer ) {
				if ( _pmlCheckLayer->vRects[I].u.pmlLayer == _pmlLayer ) {
					_sindex = I;
					return _pmlCheckLayer;
				}
				LSW_MS_LAYER * pmlTemp = GetParentLayerRecursive( _pmlCheckLayer->vRects[I].u.pmlLayer, _pmlLayer, _sindex );
				if ( pmlTemp ) {
					return pmlTemp;
				}
			}
		}
		return nullptr;
	}

	// Attach a widget to the given root.
	bool CMultiSplitter::Attach( const LSW_DT_ATTACH &_maAttach, LSW_MS_RECT &_mrRoot ) {
 		if ( !_maAttach.pwWidget ) { return false; }
		LSW_MS_LAYER_SEARCH mlsSer;
		if ( !FindRectById( _maAttach.dwId, mlsSer, _mrRoot ) ) { return false; }


		// If adding it in the same direction as what is already on the container (or
		//	if the container has 0 or 1 items and thus has no direction yet), add it to
		//	the container array.
		bool bCanAddToExistingContainer = SplitTypeMatchesLayerDirection( _maAttach.atAttachTo, mlsSer.pmlLayer );

		if ( !bCanAddToExistingContainer ) {
			mlsSer.pmlLayer = AddLayer( (*mlsSer.pmrRect), _maAttach.pwWidget );
			if ( !mlsSer.pmlLayer ) {
				// The rectangle pointed to nothing, so the widget was just added to it and nothing
				//	else needs to be done.  This happens on the root rectangle the first time a
				//	widget is added.
				return true;
			}
			// Otherwise mlsSer.pmlLayer points to the layer that was just added.
			mlsSer.sIndex = 0;
		}

		// If added to the top or left, insert before the current index.
		size_t sInsertIdx = mlsSer.sIndex;
		LSW_MS_RECT rTemp;
		rTemp.iDist = 200;

		// The call to SplitTypeMatchesLayerDirection() ensures that the current type of splitter is either
		//	"either one" or the same as what we st below.
		// If it is "either one" then it will be set properly below, and if it is already set to a direction
		//	then the below code keeps it on the same direction.
		// It's easier to redundantly set it here than to check for special cases.
		if ( _maAttach.atAttachTo == LSW_AT_LEFT || _maAttach.atAttachTo == LSW_AT_RIGHT ) {
			mlsSer.pmlLayer->stSplitType = LSW_ST_VER;
			if ( _maAttach.pwWidget->IsDockable() ) {
				rTemp.iDist = static_cast<CDockable *>(_maAttach.pwWidget)->PreferredDockWidth();
			}
		}
		else {
			mlsSer.pmlLayer->stSplitType = LSW_ST_HOR;
			if ( _maAttach.pwWidget->IsDockable() ) {
				rTemp.iDist = static_cast<CDockable *>(_maAttach.pwWidget)->PreferredDockHeight();
			}
		}
		
		if ( _maAttach.atAttachTo == LSW_AT_BOTTOM || _maAttach.atAttachTo == LSW_AT_RIGHT ) {
			// If added to the bottom or right, insert after the current index.
			++sInsertIdx;
			rTemp.bLockToFar = TRUE;
		}
		else {
			rTemp.bLockToFar = FALSE;
		}
		
		rTemp.dwId = ++m_dwIds;
		rTemp.bContainsWidget = TRUE;
		
		
		rTemp.u.pqWidget = _maAttach.pwWidget;
		mlsSer.pmlLayer->vRects.insert( mlsSer.pmlLayer->vRects.begin() + sInsertIdx, rTemp );

		CalcRects();
		return true;
	}

	// Detaches a widget from the given root given its ID.
	bool CMultiSplitter::Detach( WORD _wId, LSW_MS_RECT &_mrRoot ) {
		if ( _mrRoot.bContainsWidget && _mrRoot.u.pqWidget ) {
			if ( _mrRoot.u.pqWidget->Id() == _wId ) {
				// It is just the root node and nothing else.
				_mrRoot.bContainsWidget = FALSE;
				_mrRoot.u.pqWidget = nullptr;
				CalcRects();
				ForceSizeUpdate();
				return true;
			}
		}
		if ( !_mrRoot.bContainsWidget && _mrRoot.u.pmlLayer ) {
			LSW_MS_DETACH mdDetachInfo;
			GetDetachmentInfo( _wId, mdDetachInfo, (*_mrRoot.u.pmlLayer) );
			if ( mdDetachInfo.pmlLayer ) {
				// Something was found.
				mdDetachInfo.pmlLayer->vRects.erase( mdDetachInfo.pmlLayer->vRects.begin() + mdDetachInfo.sIndex );
				
				if ( mdDetachInfo.pmlLayer->vRects.size() == 0 ) {
					// The layer is now empty.  Collapse it down if possible.
					for ( size_t I = m_vLayers.size(); I--; ) {
						if ( m_vLayers[I] == mdDetachInfo.pmlLayer ) {
							LSW_MS_LAYER * pmlTemp = m_vLayers[I];
							m_vLayers[I] = nullptr;
							delete pmlTemp;
							m_vLayers.erase( m_vLayers.begin() + I );
						}
					}
					
					if ( mdDetachInfo.pmlParentLayer ) {
						mdDetachInfo.pmlParentLayer->vRects.erase( mdDetachInfo.pmlParentLayer->vRects.begin() + mdDetachInfo.sParentIndex );
					}
				}
				else if ( /*mdDetachInfo.pmlParentLayer && */mdDetachInfo.pmlLayer->vRects.size() == 1 && !mdDetachInfo.pmlLayer->vRects[0].bContainsWidget ) {
					LSW_MS_LAYER * pmlPrev = mdDetachInfo.pmlLayer->vRects[0].u.pmlLayer;
					(*mdDetachInfo.pmlLayer) = (*mdDetachInfo.pmlLayer->vRects[0].u.pmlLayer);
					for ( size_t I = m_vLayers.size(); I--; ) {
						if ( m_vLayers[I] == pmlPrev ) {
							LSW_MS_LAYER * pmlTemp = m_vLayers[I];
							m_vLayers[I] = nullptr;
							delete pmlTemp;
							m_vLayers.erase( m_vLayers.begin() + I );
						}
					}
					// This layer is a sub-layer and contains only a reference to another layer.  This layer is not needed.
					//mdDetachInfo.pmlParentLayer->vRects[mdDetachInfo.sParentIndex].

				}
				CalcRects();
				ForceSizeUpdate();
				return true;
			}
		}
		
		return false;
	}

	// Copies the layer tree with a specific layer removed so that it can be dragged elsewhere.
	bool CMultiSplitter::CopyLayerTreeForDrag() {

		return true;
	}

}	// namespace lsw
