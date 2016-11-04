#pragma once

#include <QList>
#include <QVariant>

class CTreeViewItem {
public :
	explicit CTreeViewItem( const QList<QVariant> &_lData, CTreeViewItem * _ptviParent = nullptr );
	~CTreeViewItem();


	// == Functions.
	void							AppendChild( CTreeViewItem * _ptviChild );
	CTreeViewItem *					Child( int _iRow );
    int								ChildCount() const;
    int								ColumnCount() const;
    QVariant						Data( int _iColumn ) const;
    int								Row() const;
    CTreeViewItem *					ParentItem();

protected :
	// == Members.
	QList<CTreeViewItem *>			m_lChildren;
	QList<QVariant>					m_lVal;
	CTreeViewItem *					m_ptviParent;
};
