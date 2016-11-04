#pragma once

#include <QStandardItemModel.h>
#include <QtWidgets/QMainWindow>
#include "FoundAddresses/MXFoundAddresses.h"
#include "ExpEval/MXExpEvalDockWidget.h"
#include "PE/MXPeWorksUi.h"
#include "ui_MXMhsX.h"


// Would be called CMhsX except that Qt would generate "CMhsX" strings if it were.
class CMain : public QMainWindow {
	Q_OBJECT

public :
	CMain( QWidget *_pwParent = nullptr );
	~CMain();


	// == Functions.
	void						ShowExpEval();
	void						CloseExpEval();
	void						ShowFoundAddress();
	void						CloseFoundAddress();
	void						UpdateWindowChecks();

	void						closeEvent( QCloseEvent * _pceEvent );

private :
	// == Members.
	Ui::CMainClass				m_mcUi;
	CExpEvalDockWidget *		m_pdwExpEval;
	CFoundAddressesDockWidget *	m_pfaFndAddr;
	QStandardItemModel *		m_psimStoredAddressesModel;
	CPe *						m_ppPeWorksWindow;


private slots :
	// == Functions.
	void						showAll();
	void						toggleExpEval();
	void						toggleFoundAddr();
	void						showPeWorks();
};
