#include "MXTreeModel.h"


CTreeModel::CTreeModel( const void * _pvData, QObject * _poParent )
    : QAbstractItemModel( _poParent ) {
    QList<QVariant> lRootData;
    lRootData << "Title" << "Value";
    m_ptviRootItem = new CTreeViewItem( lRootData );
    //SetupModelData( _sData.split( QString( "\n" ) ), m_ptviRootItem );
}
CTreeModel::CTreeModel( const QList<QVariant> &_lRootData, const void * _pvData, QObject * _poParent )
	: QAbstractItemModel( _poParent ) {
	m_ptviRootItem = new CTreeViewItem( _lRootData );
	//SetupModelData( _sData.split( QString( "\n" ) ), m_ptviRootItem );
}

CTreeModel::~CTreeModel() {
    delete m_ptviRootItem;
	m_ptviRootItem = nullptr;
}

QVariant CTreeModel::data( const QModelIndex &_miIndex, int _iRow ) const {
    if ( !_miIndex.isValid() ) {
        return QVariant();
	}

    if ( _iRow != Qt::DisplayRole ) {
        return QVariant();
	}

    CTreeViewItem * ptviItem = static_cast<CTreeViewItem*>(_miIndex.internalPointer());
    return ptviItem->Data( _miIndex.column() );
}

Qt::ItemFlags CTreeModel::flags( const QModelIndex &_miIndex ) const {
    if ( !_miIndex.isValid() ) {
        return 0;
	}
    return QAbstractItemModel::flags( _miIndex );
}

QVariant CTreeModel::headerData( int _iSection, Qt::Orientation _oOrientation,
	int _iRole ) const {
    if ( _oOrientation == Qt::Horizontal && _iRole == Qt::DisplayRole ) {
        return m_ptviRootItem->Data( _iSection );
	}

    return QVariant();
}

QModelIndex CTreeModel::index( int _iRow, int _iColumn,
		const QModelIndex &_miParent ) const {
    if ( !hasIndex( _iRow, _iColumn, _miParent ) ) {
        return QModelIndex();
	}

    CTreeViewItem * ptviParentItem;

    if ( !_miParent.isValid() ) {
        ptviParentItem = m_ptviRootItem;
	}
    else {
        ptviParentItem = static_cast<CTreeViewItem *>(_miParent.internalPointer());
	}

    CTreeViewItem * ptviChildItem = ptviParentItem->Child(_iRow );
    if ( ptviChildItem ) {
        return createIndex( _iRow, _iColumn, ptviChildItem );
	}
    else {
        return QModelIndex();
	}
}

QModelIndex CTreeModel::parent(const QModelIndex &_miIndex ) const {
    if ( !_miIndex.isValid() ) {
        return QModelIndex();
	}

    CTreeViewItem * ptviChildItem = static_cast<CTreeViewItem *>(_miIndex.internalPointer());
    CTreeViewItem * ptviParentItem = ptviChildItem->ParentItem();

    if ( ptviParentItem == m_ptviRootItem ) {
        return QModelIndex();
	}

    return createIndex( ptviParentItem->Row(), 0, ptviParentItem );
}

int CTreeModel::rowCount( const QModelIndex &_miParent ) const {
    CTreeViewItem * ptivParentItem;
    if ( _miParent.column() > 0 ) {
        return 0;
	}

    if ( !_miParent.isValid() ) {
        ptivParentItem = m_ptviRootItem;
	}
    else {
        ptivParentItem = static_cast<CTreeViewItem *>(_miParent.internalPointer());
	}

    return ptivParentItem->ChildCount();
}

int CTreeModel::columnCount( const QModelIndex &_miParent ) const {
    if ( _miParent.isValid() ) {
        return static_cast<CTreeViewItem *>(_miParent.internalPointer())->ColumnCount();
	}
    else {
        return m_ptviRootItem->ColumnCount();
	}
}