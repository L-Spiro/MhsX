#include "LSWSplitter.h"
#include "../Base/LSWBase.h"

namespace lsw {

	CSplitter::CSplitter( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu ) :
		CWidget( _wlLayout.ChangeClass( reinterpret_cast<LPCWSTR>(CBase::SplitterAtom()) ), _pwParent, _bCreateWidget, _hMenu ) {
		
	}
	CSplitter::~CSplitter() {
	}

}	// namespace lsw
