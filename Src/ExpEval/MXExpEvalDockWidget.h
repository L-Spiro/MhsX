#pragma once

#include <QStandardItemModel.h>
#include <QtWidgets/QDockWidget>
#include "ui_MXExp.h"

class CExpEvalDockWidget : public QDockWidget {
	Q_OBJECT

public:
	CExpEvalDockWidget( QWidget * _pwParent = nullptr );
	~CExpEvalDockWidget();

protected :
	virtual void			closeEvent( QCloseEvent * event );

private:
	Ui::DockWidget			m_dwExpEval;
	QStandardItemModel *	m_psimModel;
};
