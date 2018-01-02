#pragma once

#include "../LSWWin.h"
#include "../ImageList/LSWImageList.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CRebar : public CWidget {
	public :
		CRebar( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL );


		// == Functions.
		// Sets the image list.
		BOOL								SetImageList( const CImageList &_ilList );

		// Inserts a band.
		BOOL								InsertBand( INT _iIndex, const REBARBANDINFOW &_rbiBand );

		// Inserts a band.
		BOOL								InsertBand( INT _iIndex, const REBARBANDINFOA &_rbiBand );

		// Sets a band's info.
		BOOL								SetBand( INT _iIndex, const REBARBANDINFOW &_rbiBand );

		// Sets a band's info.
		BOOL								SetBand( INT _iIndex, const REBARBANDINFOA &_rbiBand );

		// Gets the total band count.
		UINT								GetBandCount() const;

		// Gets the row count.
		UINT								GetRowCount() const;


	protected :


	private :
		//typedef CWidget						Parent;
	};

}	// namespace lsw
