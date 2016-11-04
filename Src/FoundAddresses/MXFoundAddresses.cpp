#include "../MXMhsX.h"
#include "MXFoundAddresses.h"

CFoundAddressesDockWidget::CFoundAddressesDockWidget( QWidget * _pwParent ) :
	QDockWidget( _pwParent ) {

	m_faFoundAddresses.setupUi( this );

	const QStringList qslHeaders = {
		"Address",
		"Value",
	};

	m_psimModel = new QStandardItemModel();
	m_faFoundAddresses.treeView->setModel( m_psimModel );
	m_psimModel->setHorizontalHeaderLabels( qslHeaders );
	QStandardItem * psiItem = m_psimModel->horizontalHeaderItem( 0 );

	QHeaderView * phHeader = m_faFoundAddresses.treeView->header();
	phHeader->setTextElideMode( Qt::TextElideMode::ElideRight );
}
CFoundAddressesDockWidget::~CFoundAddressesDockWidget() {
	delete m_psimModel;
	m_psimModel = nullptr;
}

void CFoundAddressesDockWidget::closeEvent( QCloseEvent * event ) {
	if ( parent() ) {
		CMain * pmxParent = reinterpret_cast<CMain *>(parent());
		pmxParent->CloseFoundAddress();
	}
}
