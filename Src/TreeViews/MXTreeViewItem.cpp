#include "MXTreeViewItem.h"


CTreeViewItem::CTreeViewItem( const QList<QVariant> &_lData, CTreeViewItem * _ptviParent ) :
	m_ptviParent( _ptviParent ),
	m_lVal( _lData ) {
}
CTreeViewItem::~CTreeViewItem() {
	qDeleteAll( m_lChildren );
}

// == Functions.
void CTreeViewItem::AppendChild( CTreeViewItem * _ptviChild ) {
	m_lChildren.append( _ptviChild );
}

CTreeViewItem * CTreeViewItem::Child( int _iRow ) {
	return m_lChildren.value( _iRow );
}

int CTreeViewItem::ChildCount() const {
	return m_lChildren.count();
}

int CTreeViewItem::ColumnCount() const {
	return m_lVal.count();
}

QVariant CTreeViewItem::Data( int _iColumn ) const {
	return m_lVal.value( _iColumn );
}

int CTreeViewItem::Row() const {
	if ( m_ptviParent ) {
        return m_ptviParent->m_lChildren.indexOf( const_cast<CTreeViewItem *>(this) );
	}

    return 0;
}

CTreeViewItem * CTreeViewItem::ParentItem() {
	return m_ptviParent;
}
