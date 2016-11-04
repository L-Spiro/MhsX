#include "MXMhsX.h"
#include "Strings\MXStringDecoder.h"

CMain::CMain( QWidget * _pwParent ) :
	QMainWindow( _pwParent ),
	m_pdwExpEval( nullptr ),
	m_pfaFndAddr( nullptr ),
	m_ppPeWorksWindow( nullptr ) {
	m_mcUi.setupUi( this );


	// background-color: qlineargradient(spread:pad, x1:0, y1:1, x2:1, y2:0, stop:0 rgba(168, 168, 168, 255), stop:0.25 rgba(192, 192, 192, 255), stop:1 rgba(224, 224, 224, 255));

	/*QString sStyle = this->styleSheet();
	const char * pcClasses[] = {
		"QMainWindow"
	};
	QString sNewStyle;
	for ( uint32_t I = 0; I < sizeof( pcClasses ) / sizeof( pcClasses[0] ); ++I ) {
		sNewStyle += pcClasses[I];
		sNewStyle += " { ";
		sNewStyle += sStyle;
		sNewStyle += " } ";
	}*/
	/*this->setStyleSheet( sNewStyle );
	m_pdwExpEval->setStyleSheet( sNewStyle );*/
	ShowExpEval();
	ShowFoundAddress();

	QObject::connect( m_mcUi.actionShow_All, SIGNAL( triggered() ), this, SLOT( showAll() ) );
	QObject::connect( m_mcUi.action_Expression_Evaluator, SIGNAL( triggered() ), this, SLOT( toggleExpEval() ) );
	QObject::connect( m_mcUi.action_Found_Addresses, SIGNAL( triggered() ), this, SLOT( toggleFoundAddr() ) );
	QObject::connect( m_mcUi.action_PE_Works, SIGNAL( triggered() ), this, SLOT( showPeWorks() ) );


	const QStringList qslHeaders = {
		"Description",
		"Address",
		"Current Value",
		"Value When Locked",
		"Type",
	};

	m_psimStoredAddressesModel = new QStandardItemModel();
	m_mcUi.treeView->setModel( m_psimStoredAddressesModel );
	m_psimStoredAddressesModel->setHorizontalHeaderLabels( qslHeaders );
	QHeaderView * phHeader = m_mcUi.treeView->header();
	phHeader->setTextElideMode( Qt::TextElideMode::ElideRight );
	
	
	char szBuffer[_T_MAX_LEN];
	m_mcUi.statusBar->showMessage( tr( mx::CStringDecoder::Decode( _T_4F39CC88_Ready_, _LEN_4F39CC88, szBuffer ) ) );
	//m_mcUi.statusBar->showMessage( tr( "Ready." ) );

	//setWindowTitle(QApplication::translate("CMainClass", mx::CStringDecoder::Decode( _T_0B4C2B0E_L__Spiro_Memory_Hacking_Software_X, _LEN_0B4C2B0E, szBuffer ), 0));
	//setWindowTitle(QApplication::translate("CMainClass", "Poop Memory Hacking Software X", 0));
	//setWindowTitle(QApplication::translate("CMainClass", "L. Spiro Memory Hacking Software X", 0));

	::memset( szBuffer, 0, sizeof( szBuffer ) );	// Don’t leave decoded strings on the stack.
}

CMain::~CMain() {
	delete m_pdwExpEval;
	m_pdwExpEval = nullptr;

	delete m_psimStoredAddressesModel;
	m_psimStoredAddressesModel = nullptr;
}

// == Functions.
void CMain::ShowExpEval() {
	if ( !m_pdwExpEval ) {
		m_pdwExpEval = new CExpEvalDockWidget( this );
		addDockWidget( Qt::LeftDockWidgetArea, m_pdwExpEval );
	}
	if ( m_pdwExpEval ) {
		m_pdwExpEval->show();
	}
	UpdateWindowChecks();
}

void CMain::CloseExpEval() {
	if ( m_pdwExpEval ) {
		m_pdwExpEval->setHidden( true );
	}
	UpdateWindowChecks();
}

void CMain::UpdateWindowChecks() {
	m_mcUi.action_Expression_Evaluator->setChecked( m_pdwExpEval ? !m_pdwExpEval->isHidden() : false );
	m_mcUi.action_Found_Addresses->setChecked( m_pfaFndAddr ? !m_pfaFndAddr->isHidden() : false );
}

void CMain::closeEvent( QCloseEvent * _pceEvent ) {
	delete m_ppPeWorksWindow;
	m_ppPeWorksWindow = nullptr;
}

void CMain::ShowFoundAddress() {
	if ( !m_pfaFndAddr ) {
		m_pfaFndAddr = new CFoundAddressesDockWidget( this );
		addDockWidget( Qt::LeftDockWidgetArea, m_pfaFndAddr );
	}
	if ( m_pfaFndAddr ) {
		m_pfaFndAddr->show();
	}
	UpdateWindowChecks();
}

void CMain::CloseFoundAddress() {
	if ( m_pfaFndAddr ) {
		m_pfaFndAddr->setHidden( true );
	}
	UpdateWindowChecks();
}

void CMain::showAll() {
	ShowExpEval();
	ShowFoundAddress();
}

void CMain::toggleExpEval() {
	if ( !m_pdwExpEval || m_pdwExpEval->isHidden() ) {
		ShowExpEval();
	}
	else {
		CloseExpEval();
	}
}

void CMain::toggleFoundAddr() {
	if ( !m_pfaFndAddr || m_pfaFndAddr->isHidden() ) {
		ShowFoundAddress();
	}
	else {
		CloseFoundAddress();
	}
}

void CMain::showPeWorks() {
	if ( !m_ppPeWorksWindow ) {
		m_ppPeWorksWindow = new CPe( nullptr );
	}
	if ( m_ppPeWorksWindow ) {
		m_ppPeWorksWindow->show();
	}
}