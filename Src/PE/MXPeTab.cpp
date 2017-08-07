#include "MXPeTab.h"
#include <Windows.h>


CPeTab::CPeTab( QWidget * _pwParent ) :
	QWidget( _pwParent ),
	m_psimModel( nullptr ) {
	m_mpwUi.setupUi( this );


	/*const QStringList qslHeaders = {
		"Field Name",
		"Value",
		"Description",
	};*/

	QList<QVariant> lRootData;
    lRootData << "Field Name" << "Value" << "Raw Bytes" << "Description";

	//m_psimModel = new QStandardItemModel();
	m_psimModel = new mx::CPeTreeModel( lRootData, nullptr );
	//m_mpwUi.treeView->setStyleSheet( nullptr );
	m_mpwUi.treeView->setModel( m_psimModel );
	//int iWidth = m_mpwUi.treeView->columnWidth( 0 );
	m_mpwUi.treeView->setColumnWidth( 0, 208 );
	m_mpwUi.treeView->setColumnWidth( 1, 450 );
	m_mpwUi.treeView->setColumnWidth( 2, 102 );
	//m_mpwUi.treeView->resizeColumnToContents( 0 );

	//m_psimModel->setHorizontalHeaderLabels( qslHeaders );
	QHeaderView * phHeader = m_mpwUi.treeView->header();
	phHeader->setTextElideMode( Qt::TextElideMode::ElideRight );

}
CPeTab::~CPeTab() {
	delete m_psimModel;
	m_psimModel = nullptr;
}

// == Functions.
BOOL CPeTab::LoadFile( const QString &_sPath ) {
	if ( !m_poPeObject.LoadImageFromFile( reinterpret_cast<const wchar_t *>(_sPath.utf16()) ) ) {
		return FALSE;
	}
	if ( m_psimModel->CreateTree( m_poPeObject ) ) {
		//m_mpwUi.treeView->resizeColumnToContents( 0 );
		return TRUE;
	}
	return FALSE;
}
