#include "../MXMhsX.h"
#include "MXExpEvalDockWidget.h"

CExpEvalDockWidget::CExpEvalDockWidget( QWidget * _pwParent ) :
	QDockWidget( _pwParent ) {

	m_dwExpEval.setupUi( this );

	const QStringList qslHeaders = {
		"Expression",
		"Result",
	};

	m_psimModel = new QStandardItemModel();
	m_dwExpEval.treeView->setModel( m_psimModel );
	m_psimModel->setHorizontalHeaderLabels( qslHeaders );
	//m_psimModel->setColumnCount( 2 );
	QStandardItem * psiItem = m_psimModel->horizontalHeaderItem( 0 );

	QHeaderView * phHeader = m_dwExpEval.treeView->header();
	phHeader->setTextElideMode( Qt::TextElideMode::ElideRight );

	
	//QHeaderView * phHeader = m_dwExpEval.treeView->header();
	//m_dwExpEval.treeView->hor
	//phHeader->get
}
CExpEvalDockWidget::~CExpEvalDockWidget() {
	delete m_psimModel;
	m_psimModel = nullptr;
}

void CExpEvalDockWidget::closeEvent( QCloseEvent * event ) {
	if ( parent() ) {
		CMain * pmxParent = reinterpret_cast<CMain *>(parent());
		pmxParent->CloseExpEval();
	}
}
