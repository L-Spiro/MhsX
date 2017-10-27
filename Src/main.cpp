#include "MXMhsX.h"
#include <Base/LSWBase.h>
#include <Base/LSWWndClassEx.h>
#include <Images/LSWBitmap.h>
#include "Layouts/MXLayoutManager.h"
#include "Layouts/MXMainWindowLayout.h"
#include "Layouts/MXOpenProcessLayout.h"
#include <MainWindow/LSWMainWindow.h>
#include "System/MXSystem.h"
#include "Utilities/MXUtilities.h"

/*#include "EEExpEval.h"
#include "EEExpEvalContainer.h"
#include "EEExpEvalLexer.h"
#include "Gen/EEExpEvalParser.h"*/
//#include "Layout/LSWMainWindowLayout.h"
#include <sstream>
//#include <QtWidgets/QApplication>


int wWinMain( HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPWSTR _lpCmdLine, int _nCmdShow ) {
	WCHAR szTemp[8];
	mx::CUtilities::RandomString( szTemp, MX_ELEMENTS( szTemp ) );
	lsw::CBase::Initialize( _hInstance, new mx::CLayoutManager(), szTemp );
	::ZeroMemory( szTemp, sizeof( szTemp ) );

	// Initialize the system.
	mx::CSystem::InitSystem();
	/*
	{
#define PI 3.1415926535897932384626433832795
#define e 2.7182818284590452353602874713527
		uint64_t ui64 = 0x7F3|56 &5642 ^ 53;
		double dVal = pow( 15, 41664 );
		std::istringstream sStream( "??::Width" );
		ee::CExpEvalLexer eelLexer( &sStream );
		ee::CExpEvalContainer eecContainer( &eelLexer );
		ee::CExpEvalParser eepParser( &eelLexer, &eecContainer );
		int iRet = eepParser.parse();
		ee::CExpEvalContainer::EE_RESULT rRes;
		eecContainer.Resolve( rRes );

		ee::CExpEvalContainer::EE_RESULT rRes2 = rRes;
#undef PI
#undef e
	}*/
	
	
	

	// Create the windows.
	mx::CMainWindowLayout::CreateMainWindow();
	//mx::COpenProcessLayout::CreateOpenProcessDialog( mx::CMainWindowLayout::MainWindow() );

	LSW_WIDGET_LAYOUT wlDock = {
		LSW_LT_DOCKWINDOW,						// ltType
		125,									// wId
		nullptr,								// lpwcClass
		TRUE,									// bEnabled
		TRUE,									// bActive
		4,										// iLeft
		3,										// iTop
		263,									// dwWidth
		174,									// dwHeight
		WS_POPUP | WS_SYSMENU | WS_THICKFRAME | WS_CAPTION | WS_VISIBLE,								// dwStyle
		WS_EX_TOOLWINDOW,																// dwStyleEx
		L"L. Spiro",							// pwcText
		0,										// sTextLen
		0,										// dwParentId
	};
	//lsw::CBase::LayoutManager()->CreateWindowX( &wlDock, 1, nullptr, 0 );
	lsw::CBase::LayoutManager()->CreateWidget( lsw::CLayoutManager::FixLayout( wlDock ), mx::CMainWindowLayout::MainWindow(), true, NULL );
	wlDock.wId++;
	wlDock.pwcText = L"Found Addresses";
	wlDock.iLeft = 500;
	lsw::CBase::LayoutManager()->CreateWidget( lsw::CLayoutManager::FixLayout( wlDock ), mx::CMainWindowLayout::MainWindow(), true, NULL );

	/*WCHAR szBuffer[MAX_PATH];
	::GetCurrentDirectoryW( MAX_PATH, szBuffer );*/
	//lsw::CMainWindow mwWindow( L"Agh", L"L. Spiro MHS X" );

	/*
	lsw::CBitmap bMap;
	bMap.LoadFromFile( L"Resources/98.bmp", 0, 0, LR_LOADMAP3DCOLORS );
	*/

	MSG mMsg = {};
	while ( ::GetMessageW( &mMsg, NULL, 0, 0 ) > 0 ) {
		if ( mMsg.message == WM_QUIT ) {
			break;
		}
		::TranslateMessage( &mMsg );
		::DispatchMessageW( &mMsg );
	}

	lsw::CBase::ShutDown();
	return static_cast<int>(mMsg.wParam);
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