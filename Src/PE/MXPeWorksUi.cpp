#include "MXPeWorksUi.h"
#include "../Strings/MXStringDecoder.h"
#include <cassert>
#include <QErrorMessage>
#include <QFileDialog>
#include <Windows.h>


CPe::CPe( QWidget * _pwParent ) :
	QMainWindow( _pwParent ) {
	m_mpwUi.setupUi( this );

	QObject::connect( m_mpwUi.action_Open, SIGNAL( triggered() ), this, SLOT( openPe() ) );
	QObject::connect( m_mpwUi.tabWidget, SIGNAL( tabCloseRequested( int ) ), this, SLOT( closeTab( int ) ) );
}
CPe::~CPe() {
	for ( size_t I = m_vTabs.size(); I--; ) {
		delete m_vTabs[I];
		m_vTabs[I] = nullptr;
	}
	m_vTabs.clear();
}

void CPe::openPe() {
	QString strFile;
	{
		CHAR szOpenPe[_LEN_2D715017+1];
		CHAR szExec[_LEN_2D90879D+1];
		strFile = QFileDialog::getOpenFileName( this, tr( mx::_DEC_2D715017_Open_PE_File( szOpenPe ) ),
			m_sLastFile,
			tr( mx::_DEC_2D90879D_All_Executables____exe___dll___sys___Executables____exe___Dynamic_Link_Libraries____dll___System_Files____sys_( szExec ) ) );
		::ZeroMemory( szOpenPe, sizeof( szOpenPe ) );
		::ZeroMemory( szExec, sizeof( szExec ) );
	}
	if ( strFile.isEmpty() ) {
		return;
	}
	//const wchar_t * usPath = reinterpret_cast<const wchar_t *>(strFile.utf16());
	m_sLastFile = strFile;
	/*wchar_t wDst[MAX_PATH];
	::MultiByteToWideChar( CP_UTF8, 0, strFile.toUtf8(), -1, wDst, MAX_PATH );
	::OutputDebugStringW( wDst );*/
	
	CPeTab * ptTab = new CPeTab( m_mpwUi.tabWidget );
	if ( !ptTab ) {
		QErrorMessage * pemError = new QErrorMessage( nullptr );
		if ( pemError ) {
			pemError->showMessage( "Out of memory while loading file \"" + strFile + "\"." );
			pemError->exec();
			delete pemError;
		}
		m_mpwUi.statusbar->showMessage( "Failed to load file \"" + strFile + "\"." );
		return;
	}
	if ( !ptTab->LoadFile( strFile ) ) {
		QErrorMessage * pemError = new QErrorMessage( nullptr );
		if ( pemError ) {
			pemError->showMessage( "Failed to load file \"" + strFile + "\"." );
			pemError->exec();
			delete pemError;
		}
		delete ptTab;
		m_mpwUi.statusbar->showMessage( "Failed to load file \"" + strFile + "\"." );
		return;
	}
	QFileInfo fiInfo( strFile );
	QString sFile = fiInfo.fileName();
	m_mpwUi.tabWidget->addTab( ptTab, sFile );
	m_vTabs.push_back( ptTab );
	m_mpwUi.statusbar->showMessage( "Loaded file \"" + strFile + "\"." );
}

void CPe::closeTab( int _iIndex ) {
	assert( _iIndex < m_vTabs.size() );
	delete m_mpwUi.tabWidget->widget( _iIndex );
	m_vTabs.erase( m_vTabs.begin() + _iIndex );
}

void CPe::closeEvent( QCloseEvent * _pceEvent ) {
	while ( m_vTabs.size() ) {
		closeTab( 0 );
	}
	m_mpwUi.statusbar->showMessage( "" );
}