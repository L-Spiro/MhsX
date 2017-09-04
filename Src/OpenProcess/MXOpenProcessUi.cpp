#include "MXOpenProcessUi.h"
#include "../Strings/MXStringDecoder.h"


COp::COp( QWidget * _pwParent ) :
	QMainWindow( _pwParent ) {
	m_opUi.setupUi( this );

	const QStringList qslHeaders = {
		_DEC_S_49A0210A_Process.c_str(),
		_DEC_S_C0D8DDA3_Process_ID.c_str(),
		_DEC_S_2C5ABC07_Windows.c_str(),
	};

	QHeaderView * phHeader = m_opUi.tv->header();
	//phHeader->set
	//m_opUi.tv->set( qslHeaders );
}
COp::~COp() {
}