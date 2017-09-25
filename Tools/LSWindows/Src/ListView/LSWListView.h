#pragma once

#include "../LSWWin.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	class CListView : public CWidget {
	public :
		CListView( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true );


		// == Functions.
		// Inserts a column at the given index.
		INT									InsertColumn( INT _iIndex, const CHAR * _pcText, INT _iFormat = LVCFMT_LEFT );

		// Inserts a column at the given index.
		INT									InsertColumn( INT _iIndex, const WCHAR * _pwcText, INT _iFormat = LVCFMT_LEFT );

		// Adds a column with the given text.
		INT									AddColumn( const CHAR * _pcText, INT _iFormat = LVCFMT_LEFT );

		// Adds a column with the given text.
		INT									AddColumn( const WCHAR * _pwcText, INT _iFormat = LVCFMT_LEFT );

		// Sets the width of a column.
		BOOL								SetColumnWidth( INT _iCol, INT _iWidth );


	protected :
		// == Members.
		// The number of columns.
		SIZE_T								m_sColumns;


	private :
		typedef CWidget						Parent;
	};

}	// namespace lsw
