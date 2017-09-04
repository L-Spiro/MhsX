#pragma once

#include "FoundAddresses/MXFoundAddresses.h"
#include "ExpEval/MXExpEvalDockWidget.h"
#include "OpenProcess/MXOpenProcessUi.h"
#include "PE/MXPeWorksUi.h"
#include "ui_MXMhsX.h"
#include <QStandardItemModel.h>
#include <QtWidgets/QMainWindow>


// Would be called CMhsX except that Qt would generate "CMhsX" strings if it were.
class CMain : public QMainWindow {
	Q_OBJECT

public :
	CMain( QWidget *_pwParent = nullptr );
	~CMain();


	// == Functions.
	VOID						ShowExpEval();
	VOID						CloseExpEval();
	VOID						ShowFoundAddress();
	VOID						CloseFoundAddress();
	VOID						UpdateWindowChecks();


	void						closeEvent( QCloseEvent * _pceEvent );

private :
	// == Members.
	Ui::CMainClass				m_mcUi;
	CExpEvalDockWidget *		m_pdwExpEval;
	CFoundAddressesDockWidget *	m_pfaFndAddr;
	QStandardItemModel *		m_psimStoredAddressesModel;
	CPe *						m_ppPeWorksWindow;
	COp *						m_popOpenProcessWindow;


private slots :
	// == Functions.
	void						showAll();
	void						toggleExpEval();
	void						toggleFoundAddr();
	void						showPeWorks();
	void						showOpenProcess();
};
