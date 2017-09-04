#pragma once

#include <QMainWindow>
#include <QStandardItemModel>
#include "../ui_MXOp.h"

// Would be called COpenProcessUi except that Qt would generate "COpenProcessUi" strings if it were.
class COp : public QMainWindow {
	Q_OBJECT

public :
	COp( QWidget *_pwParent = nullptr );
	~COp();


private :
	// == Members.
	Ui::OP						m_opUi;

private slots :
	// == Functions.
	/*void						openPe();
	void						closeTab( int _iIndex );
	void						closeEvent( QCloseEvent * _pceEvent );*/
};
