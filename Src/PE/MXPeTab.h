#pragma once

#include "../PE/MXPeObject.h"
#include <QStandardItemModel.h>
#include <QWidget.h>
#include "ui_MXPet.h"


class CPeTab : public QWidget {
	Q_OBJECT

public :
	CPeTab( QWidget *_pwParent = nullptr );
	~CPeTab();

	
	// == Functions.
	bool						LoadFile( const QString &_sPath );

private :
	// == Members.
	Ui::PeW						m_mpwUi;
	QStandardItemModel *		m_psimModel;
	mx::CPeObject				m_poPeObject;

private slots :
	// == Functions.
};