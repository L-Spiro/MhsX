#pragma once

#include <QMainWindow>
#include <QStandardItemModel>
#include "../ui_MXPew.h"
#include "MXPeTab.h"

// Would be called CPeWorksUi except that Qt would generate "CPeWorksUi" strings if it were.
class CPe : public QMainWindow {
	Q_OBJECT

public :
	CPe( QWidget *_pwParent = nullptr );
	~CPe();


private :
	// == Members.
	Ui::PeWin					m_mpwUi;
	QString						m_sLastFile;
	std::vector<CPeTab *>		m_vTabs;

private slots :
	// == Functions.
	void						openPe();
	void						closeTab( int _iIndex );
};
