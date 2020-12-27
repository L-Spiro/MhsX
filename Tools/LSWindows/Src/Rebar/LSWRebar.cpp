#include "LSWRebar.h"

namespace lsw {

	CRebar::CRebar( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		CWidget( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
	}


	// == Functions.
	// Sets the image list.
	BOOL CRebar::SetImageList( const CImageList &_ilList ) {
		if ( !Wnd() ) { return FALSE; }
		REBARINFO rbInfo = { sizeof( rbInfo ) };
		rbInfo.fMask = RBIM_IMAGELIST;
		rbInfo.himl = _ilList.Handle();
		return static_cast<BOOL>(::SendMessageW( Wnd(), RB_SETBARINFO, 0L, reinterpret_cast<LPARAM>(&rbInfo) ));
	}

	// Inserts a band.
	BOOL CRebar::InsertBand( INT _iIndex, const REBARBANDINFOW &_rbiBand ) {
		if ( !Wnd() ) { return FALSE; }
		REBARBANDINFOW rbiTemp = _rbiBand;
		rbiTemp.cbSize = sizeof( rbiTemp );
		return static_cast<BOOL>(::SendMessageW( Wnd(), RB_INSERTBANDW, _iIndex, reinterpret_cast<LPARAM>(&rbiTemp) ));
	}

	// Inserts a band.
	BOOL CRebar::InsertBand( INT _iIndex, const REBARBANDINFOA &_rbiBand ) {
		if ( !Wnd() ) { return FALSE; }
		REBARBANDINFOA rbiTemp = _rbiBand;
		rbiTemp.cbSize = sizeof( rbiTemp );
		return static_cast<BOOL>(::SendMessageA( Wnd(), RB_INSERTBANDA, _iIndex, reinterpret_cast<LPARAM>(&rbiTemp) ));
	}

	// Sets a band's info.
	BOOL CRebar::SetBand( INT _iIndex, const REBARBANDINFOW &_rbiBand ) {
		if ( !Wnd() ) { return FALSE; }
		REBARBANDINFOW rbiTemp = _rbiBand;
		rbiTemp.cbSize = sizeof( rbiTemp );
		return static_cast<BOOL>(::SendMessageW( Wnd(), RB_SETBANDINFOW, _iIndex, reinterpret_cast<LPARAM>(&rbiTemp) ));
	}

	// Sets a band's info.
	BOOL CRebar::SetBand( INT _iIndex, const REBARBANDINFOA &_rbiBand ) {
		if ( !Wnd() ) { return FALSE; }
		REBARBANDINFOA rbiTemp = _rbiBand;
		rbiTemp.cbSize = sizeof( rbiTemp );
		return static_cast<BOOL>(::SendMessageA( Wnd(), RB_SETBANDINFOA, _iIndex, reinterpret_cast<LPARAM>(&rbiTemp) ));
	}

	// Gets the total band count.
	UINT CRebar::GetBandCount() const {
		if ( !Wnd() ) { return 0; }
		return static_cast<UINT>(::SendMessageW( Wnd(), RB_GETBANDCOUNT, 0L, 0L ));
	}

	// Gets the row count.
	UINT CRebar::GetRowCount() const {
		if ( !Wnd() ) { return 0; }
		return static_cast<UINT>(::SendMessageW( Wnd(), RB_GETROWCOUNT, 0L, 0L ));
	}

}	// namespace lsw
