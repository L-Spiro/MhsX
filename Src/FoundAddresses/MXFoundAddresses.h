#pragma once

#include <QStandardItemModel.h>
#include <QtWidgets/QDockWidget>
#include "ui_MXFa.h"

class CFoundAddressesDockWidget : public QDockWidget {
	Q_OBJECT

public:
	CFoundAddressesDockWidget( QWidget * _pwParent = nullptr );
	~CFoundAddressesDockWidget();

protected :
	virtual void			closeEvent( QCloseEvent * event );

private:
	Ui::FaDock				m_faFoundAddresses;
	QStandardItemModel *	m_psimModel;
};
