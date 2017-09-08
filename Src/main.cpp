#include "MXMhsX.h"
#include "LSWin/MainWindow/LSWMainWindow.h"
#include "System/MXSystem.h"

#include "EEExpEval.h"
#include "EEExpEvalContainer.h"
#include "EEExpEvalLexer.h"
#include "Gen/EEExpEvalParser.h"
#include <sstream>
//#include <QtWidgets/QApplication>


int wWinMain( HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPWSTR _lpCmdLine, int _nCmdShow ) {
	// Initialize the system.
	mx::CSystem::InitSystem();

	{
		uint64_t ui64 = 0x7F3|56 &5642 ^ 53;
		std::istringstream sStream( "0x7F3|56 &5642 ^ 53" );
		ee::CExpEvalLexer eelLexer( &sStream );
		ee::CExpEvalContainer eecContainer( &eelLexer );
		ee::CExpEvalParser eepParser( &eelLexer, &eecContainer );
		int iRet = eepParser.parse();
		ee::CExpEvalContainer::EE_RESULT rRes;
		eecContainer.Resolve( rRes );

		ee::CExpEvalContainer::EE_RESULT rRes2 = rRes;
	}

	lsw::CMainWindow mwWindow( L"Agh", L"L. Spiro MHS X" );

	MSG mMsg = {};
	while ( ::GetMessageW( &mMsg, NULL, 0, 0 ) ) {
		::TranslateMessage( &mMsg );
		::DispatchMessageW( &mMsg );
	}
}

/*
int main( int _iArgs, char * _pcArgs[] ) {
	// Initialize the system.
	mx::CSystem::InitSystem();

	QApplication aApp( _iArgs, _pcArgs );
	CMain mxWindow;
	mxWindow.show();
	return aApp.exec();
}*/


#if 0
#include <cstdio>
#include <cmath>
#include <Windows.h>
#include <WinBase.h>


float SinF_LS( float _fX ) {
    int i32I = int( _fX * (1.0f / 3.1415926535897932384626433832795f) );	// 0.31830988618379067153776752674503 = 1 / PI.
	_fX = (_fX - float( i32I ) * 3.1415926535897932384626433832795f);

	float fX2 = _fX * _fX;
	return (i32I & 1) ?
		-_fX * (float( 9.99999701976776123047e-01 ) +
			fX2 * (float( -1.66665777564048767090e-01 ) +
			fX2 * (float( 8.33255797624588012695e-03 ) +
			fX2 * (float( -1.98125766473822295666e-04 ) +
			fX2 * (float( 2.70405212177138309926e-06 ) +
			fX2 * float( -2.05329886426852681325e-08 )))))) :
		_fX * (float( 9.99999701976776123047e-01 ) +
			fX2 * (float( -1.66665777564048767090e-01 ) +
			fX2 * (float( 8.33255797624588012695e-03 ) +
			fX2 * (float( -1.98125766473822295666e-04 ) +
			fX2 * (float( 2.70405212177138309926e-06 ) +
			fX2 * float( -2.05329886426852681325e-08 ))))));
}

float SinF_LS2( float _fX ) {
    int i32I = int( _fX * (1.0f / 3.1415926535897932384626433832795f) );    // 0.31830988618379067153776752674503 = 1 / PI.
	_fX = (_fX - float( i32I ) * 3.1415926535897932384626433832795f);

	float x2 = _fX * _fX;
    return (i32I & 1) ?    
        (((((-2.05329886426852681325e-08f*x2 + 2.70405212177138309926e-06f)*x2
            - 1.98125766473822295666e-04f)*x2 + 8.33255797624588012695e-03f)*x2
            - 1.66665777564048767090e-01f)*x2 + 9.99999701976776123047e-01f)*-_fX :
        (((((-2.05329886426852681325e-08f*x2 + 2.70405212177138309926e-06f)*x2
            - 1.98125766473822295666e-04f)*x2 + 8.33255797624588012695e-03f)*x2
            - 1.66665777564048767090e-01f)*x2 + 9.99999701976776123047e-01f)*_fX;
}

float SinF_G( float _fX ) {
	int i32I = int( _fX * (1.0f / 3.1415926535897932384626433832795f) );	// 0.31830988618379067153776752674503 = 1 / PI.
	_fX = (_fX - float( i32I ) * 3.1415926535897932384626433832795f);

	float x2 = _fX * _fX;
    return (i32I & 1) ?
        (((((-2.05342856289746600727e-08f*x2 + 2.70405218307799040084e-06f)*x2
            - 1.98125763417806681909e-04f)*x2 + 8.33255814755188010464e-03f)*x2
            - 1.66665772196961623983e-01f)*x2 + 9.99999707044156546685e-01f)*-_fX :
        (((((-2.05342856289746600727e-08f*x2 + 2.70405218307799040084e-06f)*x2
            - 1.98125763417806681909e-04f)*x2 + 8.33255814755188010464e-03f)*x2
            - 1.66665772196961623983e-01f)*x2 + 9.99999707044156546685e-01f)*_fX;

}


int main()
{
	const unsigned int ui32Total = 75000000;
	const double dLow = -3.1415926535897932384626433832795 * 1.0;
	const double dHigh = 3.1415926535897932384626433832795 * 1.0;
	const double dH = (dHigh - dLow) / ui32Total;
    
	double dError = 0.0;
	double dMaxError = 0.0;
	double dMaxVal = 0.0;
	float fVal = dHigh;
	unsigned int iCount = 0;
	//for ( unsigned int I = 0; I < ui32Total; ++I ) {
	for ( unsigned int & iTest0 = (*reinterpret_cast<unsigned int *>(&fVal)); fVal >= 0.0f; --iTest0 ) {
		double dI = fVal;//dLow + I * dH;
		double dSin = static_cast<double>(::sin( dI ));
		double dSinAcc = SinF_LS( dI );
		double dThisError = static_cast<double>(::fabs( dSin - dSinAcc ));
		dMaxError = dThisError > dMaxError ? dThisError : dMaxError;
		dMaxVal = dSinAcc > dMaxVal ? dSinAcc : dMaxVal;
		dError += dThisError;
		++iCount;


		dI = -fVal;//dLow + I * dH;
		dSin = static_cast<double>(::sin( dI ));
		dSinAcc = SinF_LS( dI );
		dThisError = static_cast<double>(::fabs( dSin - dSinAcc ));
		dMaxError = dThisError > dMaxError ? dThisError : dMaxError;
		dMaxVal = dSinAcc > dMaxVal ? dSinAcc : dMaxVal;
		dError += dThisError;
		++iCount;
	}
  
	char szBuffer[512];
	sprintf( szBuffer, "Max: %.33f Avg: %.33f Max Val: %.33f %u\r\n", dMaxError, dError / iCount, dMaxVal, iCount );
	OutputDebugStringA( szBuffer );
    
    
	dError = 0.0;
	dMaxError = 0.0;
	dMaxVal = 0.0;
	fVal = dHigh;
	iCount = 0;
	for ( unsigned int & iTest0 = (*reinterpret_cast<unsigned int *>(&fVal)); fVal >= 0.0f; --iTest0 ) {
		double dI = fVal;//dLow + I * dH;
		double dSin = static_cast<double>(::sin( dI ));
		double dSinAcc = SinF_G( dI );
		double dThisError = static_cast<double>(::fabs( dSin - dSinAcc ));
		dMaxError = dThisError > dMaxError ? dThisError : dMaxError;
		dMaxVal = dSinAcc > dMaxVal ? dSinAcc : dMaxVal;
		dError += dThisError;
		++iCount;


		dI = -fVal;//dLow + I * dH;
		dSin = static_cast<double>(::sin( dI ));
		dSinAcc = SinF_G( dI );
		dThisError = static_cast<double>(::fabs( dSin - dSinAcc ));
		dMaxError = dThisError > dMaxError ? dThisError : dMaxError;
		dMaxVal = dSinAcc > dMaxVal ? dSinAcc : dMaxVal;
		dError += dThisError;
		++iCount;
	}
  
	sprintf( szBuffer, "Max: %.33f Avg: %.33f Max Val: %.33f %u\r\n", dMaxError, dError / iCount, dMaxVal, iCount );
	OutputDebugStringA( szBuffer );

	return 0;
}
#endif



/*int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMainWindow w;
    w.setMenuBar(new QMenuBar);
    w.menuBar()->addMenu("File");
	w.setStyleSheet("QMainWindow { background-color: black; }");
 
    w.show();
 
    return a.exec();
}*/