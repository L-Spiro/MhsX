#include "LSWMultiSplitter.h"
#include "../Base/LSWBase.h"
#include "../Brush/LSWBrush.h"
#include "../Images/LSWBitmap.h"

namespace lsw {

	// == Members.
	// IDs.
	DWORD CMultiSplitter::m_dwIds = 0;

	CMultiSplitter::CMultiSplitter( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu ) :
		CWidget( _wlLayout.ChangeClass( reinterpret_cast<LPCWSTR>(CBase::MultiSplitterAtom()) ), _pwParent, _bCreateWidget, _hMenu ),
		m_iBarWidth( 4 ),
		m_pmlDragLayer( nullptr ),
		m_sDragBarIndex( 0 )/*,
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
	}

	// == Functions.
	// Attach a widget.
	bool CMultiSplitter::Attach( const LSW_MS_ATTACH &_maAttach ) {
		if ( !_maAttach.pwWidget ) { return false; }
		LSW_MS_LAYER_SEARCH mlsSer;
		if ( !FindRectById( _maAttach.dwId, mlsSer ) ) { return false; }


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

		// The call to SplitTypeMatchesLayerDirection() ensures that the current type of splitter is either
		//	"either one" or the same as what we st below.
		// If it is "either one" then it will be set properly below, and if it is already set to a direction
		//	then the below code keeps it on the same direction.
		// It's easier to redundantly set it here than to check for special cases.
		if ( _maAttach.atAttachTo == LSW_AT_LEFT || _maAttach.atAttachTo == LSW_AT_RIGHT ) {
			mlsSer.pmlLayer->stSplitType = LSW_ST_VER;
		}
		else {
			mlsSer.pmlLayer->stSplitType = LSW_ST_HOR;
		}
		if ( _maAttach.atAttachTo == LSW_AT_BOTTOM || _maAttach.atAttachTo == LSW_AT_RIGHT ) {
			// If added to the bottom or right, insert after the current index.
			++sInsertIdx;
		}
		LSW_MS_RECT rTemp;
		rTemp.dwId = ++m_dwIds;
		rTemp.bContainsWidget = TRUE;
		rTemp.iDist = 200;
		rTemp.u.pqWidget = _maAttach.pwWidget;
		mlsSer.pmlLayer->vRects.insert( mlsSer.pmlLayer->vRects.begin() + sInsertIdx, rTemp );

		CalcRects();
		return true;
	}

	// Client rectangle.
	const LSW_RECT CMultiSplitter::ClientRect( const CWidget * pwChild ) const {
		for ( size_t I = 0; I < m_vLayers.size(); ++I ) {
			for ( size_t J = 0; J < m_vLayers[I]->vRects.size(); ++J ) {
				if ( m_vLayers[I]->vRects[J].bContainsWidget && m_vLayers[I]->vRects[J].u.pqWidget == pwChild ) {
					return m_vLayers[I]->vRects[J].rRect;
				}
			}
		}
		return m_meRoot.rRect;
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
			iY = GetNewBarY( _pPoint );
			iY += (*m_pmlDragLayer).vRects[m_sDragBarIndex].rRect.top;
			iWidth = (*m_pmlDragLayer).vRects[m_sDragBarIndex].rRect.Width();
			iHeight = m_iBarWidth;
		}
		else {
			iY = static_cast<INT>((*m_pmlDragLayer).vRects[m_sDragBarIndex].rRect.top);
			iX = GetNewBarX( _pPoint );
			iX += (*m_pmlDragLayer).vRects[m_sDragBarIndex].rRect.left;
			iWidth = m_iBarWidth;
			iHeight = (*m_pmlDragLayer).vRects[m_sDragBarIndex].rRect.Height();
		}

		DrawXorBar( hDc, iX, iY, iWidth, iHeight );

		ReleaseDC( Wnd(), hDc );

		m_pLastPoint = _pPoint;
	}

	// Calculates the new X coordinate of a bar from dragging.  Uses m_pmlDragLayer, m_sDragBarIndex, and m_pDragStart.
	INT CMultiSplitter::GetNewBarX( const POINT &_pCurPoint ) {
		LONG lChange = _pCurPoint.x - m_pDragStart.x;
		INT iX = static_cast<INT>(std::max( 0L, (*m_pmlDragLayer).vRects[m_sDragBarIndex].iDist + lChange ));
		return iX;
	}

	// Calculates the new Y coordinate of a bar from dragging.  Uses m_pmlDragLayer, m_sDragBarIndex, and m_pDragStart.
	INT CMultiSplitter::GetNewBarY( const POINT &_pCurPoint ) {
		LONG lChange = _pCurPoint.y - m_pDragStart.y;
		INT iY = static_cast<INT>(std::max( 0L, (*m_pmlDragLayer).vRects[m_sDragBarIndex].iDist + lChange ));
		return iY;
	}

	// Finds a rectangle by its ID.
	bool CMultiSplitter::FindRectById( DWORD _dwId, LSW_MS_LAYER_SEARCH &_mlsRet ) {
		if ( m_meRoot.dwId == _dwId ) {
			_mlsRet.pmlLayer = nullptr;
			_mlsRet.sIndex = 0;
			_mlsRet.pmrRect = &m_meRoot;
			return true;
		}
		return FindRectById( _dwId, m_meRoot.u.pmlLayer, _mlsRet );
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
		for ( size_t I = m_vLayers.size(); I--; ) {
			for ( size_t J = m_vLayers[I]->vRects.size(); J--; ) {
				if ( m_vLayers[I]->vRects[J].bContainsWidget && m_vLayers[I]->vRects[J].u.pqWidget == _pwChild ) {
					// TODO: A real command that correctly removes widgets.
					m_vLayers[I]->vRects.erase( m_vLayers[I]->vRects.begin() + J );
				}
			}
			//if ( m_vLayers[I]->vRects.size() 
		}
		CWidget::RemoveChild( _pwChild );
	}

	// Adds a layer under a given rectangle.
	CMultiSplitter::LSW_MS_LAYER * CMultiSplitter::AddLayer( LSW_MS_RECT &_mrRect, CWidget * _pwWidget ) {
		// If the rectangle doesn't actually point to anything (an empty root node), then
		//	just make it point to the given widget.
		if ( !_mrRect.u.pqWidget ) {
			_mrRect.u.pqWidget = _pwWidget;
			_mrRect.bContainsWidget = TRUE;
			return nullptr;
		}
		// Otherwise, create a new layer and point the rectangle to it.
		LSW_MS_LAYER * pmlLayer = new LSW_MS_LAYER();
		if ( pmlLayer ) {
			m_vLayers.push_back( pmlLayer );
			pmlLayer->vRects.push_back( _mrRect );
			_mrRect.bContainsWidget = FALSE;
			_mrRect.u.pmlLayer = pmlLayer;
			_mrRect.dwId = ++m_dwIds;
		}
		return pmlLayer;
	}

	// Calculates all of the rectangles.
	void CMultiSplitter::CalcRects() {
		::GetClientRect( Wnd(), &m_meRoot.rRect );
		if ( m_meRoot.u.pmlLayer && !m_meRoot.bContainsWidget ) {
			CalcRects( (*m_meRoot.u.pmlLayer), m_meRoot.rRect );
		}
	}

	// Calculate rectangles.
	void CMultiSplitter::CalcRects( LSW_MS_LAYER &_mlLayer, const LSW_RECT &_rRect ) {
		LSW_RECT rCopy = _rRect;
		for ( size_t I = 0; I < _mlLayer.vRects.size(); ++I ) {
			_mlLayer.vRects[I].rRect = CalcRect( _mlLayer.vRects[I], rCopy, _mlLayer.stSplitType, _mlLayer.vRects.size(), I );
			if ( !_mlLayer.vRects[I].bContainsWidget && _mlLayer.vRects[I].u.pmlLayer ) {
				CalcRects( (*_mlLayer.vRects[I].u.pmlLayer), _mlLayer.vRects[I].rRect );
			}

			if ( _mlLayer.stSplitType == LSW_ST_VER ) {
				rCopy.left = _mlLayer.vRects[I].rRect.right + m_iBarWidth;
			}
			else {
				rCopy.top = _mlLayer.vRects[I].rRect.bottom + m_iBarWidth;
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
			LONG lMaxRight = static_cast<LONG>(_rRect.right - m_iBarWidth * ((_stTotalRectsInLayer - 1) - _stRectIndexInLayer));
			rCopy.right = std::min( lMaxRight, (rCopy.left + _mrRect.iDist) );
			rCopy.right = std::max( rCopy.left, rCopy.right );
		}
		else {
			// Same but going down.
			LONG lMaxBottom = static_cast<LONG>(_rRect.bottom - m_iBarWidth * ((_stTotalRectsInLayer - 1) - _stRectIndexInLayer));
			rCopy.bottom = std::min( lMaxBottom, (rCopy.top + _mrRect.iDist) );
			rCopy.bottom = std::max( rCopy.top, rCopy.bottom );
		}
		return rCopy;
	}

	// Does the given attachment type work with the split type of a layer?
	bool CMultiSplitter::SplitTypeMatchesLayerDirection( LSW_ATTACH_TYPE _atType, LSW_MS_LAYER * _plLayer ) {
		if ( !_plLayer ) { return false; }						// A layer must be create with the given direction.
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
		// If only 1 rectangle on this layer there is no drag bar (the rectangle covers the full surface).
		if ( _mlCheckLayer.vRects.size() <= 1 ) { return false; }

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
		return LSW_H_CONTINUE;
	}

	// WM_LBUTTONDOWN.
	CWidget::LSW_HANDLED CMultiSplitter::LButtonDown( DWORD _dwVirtKeys, const POINTS &_pCursorPos ) {
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
	CWidget::LSW_HANDLED CMultiSplitter::MouseMove( DWORD _dwVirtKeys, const POINTS &_pCursorPos ) {
		if ( m_pmlDragLayer ) {
			POINT pCurPos = { _pCursorPos.x, _pCursorPos.y };
			DrawBar( m_pLastPoint );
			DrawBar( pCurPos );
		}

		return LSW_H_CONTINUE;
	}

	// WM_LBUTTONUP.
	CWidget::LSW_HANDLED CMultiSplitter::LButtonUp( DWORD _dwVirtKeys, const POINTS &_pCursorPos ) {
		if ( m_pmlDragLayer ) {
			DrawBar( m_pLastPoint );
			POINT pCurPos = { _pCursorPos.x, _pCursorPos.y };

			if ( m_pmlDragLayer->stSplitType == LSW_ST_HOR ) {
				m_pmlDragLayer->vRects[m_sDragBarIndex].iDist = GetNewBarY( pCurPos );
			}
			else {
				m_pmlDragLayer->vRects[m_sDragBarIndex].iDist = GetNewBarX( pCurPos );
			}
			CalcRects();
			SizeAttachments();

			m_pmlDragLayer = nullptr;
		}
		
		::ReleaseCapture();
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

}	// namespace lsw
