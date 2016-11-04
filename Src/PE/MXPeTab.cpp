#include "MXPeTab.h"
#include <Windows.h>


CPeTab::CPeTab( QWidget * _pwParent ) :
	QWidget( _pwParent ),
	m_psimModel( nullptr ) {
	m_mpwUi.setupUi( this );


	const QStringList qslHeaders = {
		"Field Name",
		"Value",
		"Description",
	};

	m_psimModel = new QStandardItemModel();
	m_mpwUi.treeView->setModel( m_psimModel );
	m_psimModel->setHorizontalHeaderLabels( qslHeaders );
	QHeaderView * phHeader = m_mpwUi.treeView->header();
	phHeader->setTextElideMode( Qt::TextElideMode::ElideRight );

}
CPeTab::~CPeTab() {
}

// == Functions.
bool CPeTab::LoadFile( const QString &_sPath ) {
	return m_poPeObject.LoadImageFromFile( reinterpret_cast<const wchar_t *>(_sPath.utf16()) );
}
