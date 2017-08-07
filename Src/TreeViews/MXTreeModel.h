#pragma once

#include "MXTreeViewItem.h"
#include <QAbstractItemModel>

class CTreeModel : public QAbstractItemModel {
Q_OBJECT

public :
    explicit CTreeModel( const void * _pvData, QObject * _poParent = nullptr );
	explicit CTreeModel( const QList<QVariant> &_lRootData, const void * _pvData, QObject * _poParent = nullptr );
    ~CTreeModel();


	// == Functions.
	QVariant					data( const QModelIndex &_miIndex, int _iRole ) const override;
    Qt::ItemFlags				flags( const QModelIndex &_miIndex ) const override;
    QVariant					headerData( int _iSection, Qt::Orientation _oOrientation,
		int _iRole = Qt::DisplayRole ) const override;
    QModelIndex					index( int _iRow, int _iColumn,
		const QModelIndex &_miParent = QModelIndex() ) const override;
    QModelIndex					parent( const QModelIndex &_miIndex ) const override;
    int							rowCount( const QModelIndex &_miParent = QModelIndex() ) const override;
    int							columnCount( const QModelIndex &_miParent = QModelIndex() ) const override;

protected :
    //virtual void				SetupModelData( const QStringList &_slIines, CTreeViewItem * _ptviParent );


	// == Members.
    CTreeViewItem *				m_ptviRootItem;
};