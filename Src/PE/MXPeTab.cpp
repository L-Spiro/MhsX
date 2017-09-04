#include "MXPeTab.h"
#include "../LSWin/LSWWin.h"
#include "../Strings/MXStringDecoder.h"


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
    lRootData << _DEC_S_D902CC4C_Field_Name.c_str() <<
		_DEC_S_DCB67730_Value.c_str() <<
		_DEC_S_5EA6CFE6_Offset.c_str() <<
		_DEC_S_3EF5AD3F_Raw_Bytes.c_str() <<
		_DEC_S_EB78CFF1_Description.c_str();

	//m_psimModel = new QStandardItemModel();
	m_psimModel = new mx::CPeTreeModel( lRootData, nullptr );
	//m_mpwUi.tv->setStyleSheet( nullptr );
	m_mpwUi.tv->setModel( m_psimModel );
	
	QFont fFont = m_mpwUi.tv->font();	// Setting the style resets the font.
	m_mpwUi.tv->setAlternatingRowColors( true );
	m_mpwUi.tv->setStyleSheet( "alternate-background-color: rgb(229, 245, 255);" );
	m_mpwUi.tv->setFont( fFont );

	//int iWidth = m_mpwUi.tv->columnWidth( 0 );
	m_mpwUi.tv->setColumnWidth( 0, 208 );
	m_mpwUi.tv->setColumnWidth( 1, 450 + 5 );
	m_mpwUi.tv->setColumnWidth( 2, 122 + 5 );
	m_mpwUi.tv->setColumnWidth( 3, 102 + 5 );
	//m_mpwUi.tv->resizeColumnToContents( 0 );
	
	//m_psimModel->setHorizontalHeaderLabels( qslHeaders );
	QHeaderView * phHeader = m_mpwUi.tv->header();
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
