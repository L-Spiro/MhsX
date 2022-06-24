
#include "MXMhsX.h"
#include <Base/LSWBase.h>
#include <Base/LSWWndClassEx.h>
#include <EEExpEval.h>
#include <Images/LSWBitmap.h>
#include "Html/MXHtml.h"
#include "Layouts/MXFoundAddressLayout.h"
#include "Layouts/MXLayoutManager.h"
#include "Layouts/MXMainWindowLayout.h"
#include "Layouts/MXOpenProcessLayout.h"
#include <MainWindow/LSWMainWindow.h>
#include "MainWindow/MXMhsMainWindow.h"
#include "MemHack/MXWindowMemHack.h"
#include "System/MXSystem.h"
#include "Utilities/MXUtilities.h"

#include <BigInt/EEBigInt.h>
#include <EEFloatX.h>
/*#include "EEExpEval.h"
#include "EEExpEvalContainer.h"
#include "EEExpEvalLexer.h"
#include "Gen/EEExpEvalParser.h"*/
//#include "Layout/LSWMainWindowLayout.h"
#include <sstream>
//#define MX_QT
#ifdef MX_QT
#include <QtWidgets/QApplication>

#else

#include <Unicode/EEUnicode.h>


#include <codecvt>
// Converts a wstring to a UTF-8 string.
std::string WStringToString2( const std::wstring &_wsIn ) {
	return std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.to_bytes( _wsIn );
}
std::string NumToUtf8( uint32_t _ui32Val ) {
	std::wstring wStr;
	wStr.push_back( (wchar_t)_ui32Val );
	return std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.to_bytes( wStr );
}


int wWinMain( HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPWSTR _lpCmdLine, int _nCmdShow ) {
	// Security measure.  All custom window classes have random names.  Ensure their lengths can change, but not have the same length
	//	as any other string to ensure no name collisions.  No need for true random here; % is fine enough.
#define MX_NEW_STRING( NAME, BASELEN )	std::vector<WCHAR> NAME; NAME.resize( (BASELEN) + (rand() % 4) ); mx::CUtilities::RandomString( &NAME[0], NAME.size() )
	MX_NEW_STRING( szDockable, 2 );
	MX_NEW_STRING( szSplitter, 6 );
	MX_NEW_STRING( szMSplitter, 10 );
	MX_NEW_STRING( szTreeView, 14 );
	MX_NEW_STRING( szTreeListView, 12 );
#undef MX_NEW_STRING
	lsw::CBase::Initialize( _hInstance, new mx::CLayoutManager(),
		&szDockable[0],
		&szSplitter[0],
		&szMSplitter[0],
		&szTreeView[0],
		&szTreeListView[0] );
#define MX_CLEAN_STRING( STR )	::ZeroMemory( &STR[0], STR.size() * sizeof( STR[0] ) );
	MX_CLEAN_STRING( szDockable );
	MX_CLEAN_STRING( szSplitter );
	MX_CLEAN_STRING( szMSplitter );
	MX_CLEAN_STRING( szTreeView );
	MX_CLEAN_STRING( szTreeListView );
#undef MX_CLEAN_STRING

	// Initialize the system.
	mx::CSystem::InitSystem();
	

#if 0
	::OutputDebugStringA( "\xDr\n" );
	ee::CFloatX fVal;
	double dFMax = ee::CFloatX::GetMaxForBits( 8, 24, true );
	if ( dFMax == FLT_MAX ) {
		dFMax--;
	}

	//mx::CHtml::MakeNickNameTable();
/*	ee::CUnicode::MakeNickNameTable();
	std::string sUnicode;
	ee::CUnicode::GetName( sUnicode, 2 );*/

	
	ee::CFloatX fFloat;
	const uint16_t uiExpBits = 5;
	const uint16_t uiManBits = 11;
	fFloat.uiExpBits = uiExpBits;
	fFloat.uiManBits = uiManBits;
	fFloat.uiExponent = 0x00000001 >> (uiManBits - 1);
	fFloat.uiMantissa = (0x00000001 & ((1 << (uiManBits - 1)) - 1));
	double dTemp = fFloat.AsDouble();
	double dTemp34 = dTemp;
	dTemp = ee::CFloatX::GetMaxForBits( uiExpBits, uiManBits, true );
	dTemp = ee::CFloatX::GetMinForBits( uiExpBits, uiManBits, true );	// 1.401298464e-45F
	/*fFloat.CreateInfN( fFloat.uiExpBits, fFloat.uiManBits );
	dTemp = fFloat.AsDouble();
	fFloat.CreateInfP( fFloat.uiExpBits, fFloat.uiManBits );
	dTemp = fFloat.AsDouble();
	fFloat.CreateNaN( fFloat.uiExpBits, fFloat.uiManBits );
	dTemp = fFloat.AsDouble();*/

	double dTemp2 = 3.141592653589793115997963468544185161590576171875;
	fVal.CreateFromDouble( -562.0
		//dTemp2
		//FLT_TRUE_MIN
		//2.2250738585072009e-308
		//DBL_MIN
		//FLT_MIN
		//3.14159 * 2000.135464 * 0
		//65504 + 1
							//, 8, 24 );
							, 5, 11 );
	dTemp = fVal.AsDouble();


	//generate_table();
	ee::CBigInt biInt = 16;
	for ( int H = 0; H < 2; ++H ) {
		ee::CBigInt::Multiply( biInt, 0xFF90301F );
	}
	ee::CBigInt::ShiftRight( biInt, 136 );
	std::string sTemp = biInt.ToTring( 10 );
	// 294141651099334622224 (FF209113EDEBA3C10h)
	// 147070825549667311112 (7F904889F6F5D1E08h)
	// 73535412774833655556 (3FC82444FB7AE8F04h)
	// 36767706387416827778 (1FE412227DBD74782h)
	// 18383853193708413889 (FF209113EDEBA3C1h)
	// 0x00000000001ff074 {0x78a00000, 0x00000005, 0xcccccccc, 0xcccccccc, 0xcccccccc, 0xcccccccc, 0xcccccccc, ...}
	// 23498588160 (578A00000h)
	
	uint32_t ui320 = '\x9';
	uint32_t ui321 = '\x90';
	//uint32_t ui322 = '\x901';
	//uint32_t ui323 = '\x9089';
	//uint32_t ui324 = '\x90EF2301';

	size_t sConsued;
	uint32_t ui32G0 = ee::EscapeX( "x9", 3, sConsued );
	uint32_t ui32G1 = ee::EscapeX( "xAf", 4, sConsued );
	uint32_t ui32G3 = ee::EscapeX( "x902", 5, sConsued );

	const char * pcUtf8Temp = u8"\xEF\xB9\x8C";
	// C3 AF C2 B9 C2 8C
	// ï¹
	// C3 AF C2 B9 C2 8C
	// Ã¯Â¹ÂŒ
#endif	// 0
#if 0
	//uint32_t ui32Blah = '\U0001F436';
	uint32_t ui32Blah = '\U00002019';
	uint32_t ui32Blah2 = '\u2019';
	size_t s0 = sizeof( '\U00002019' );
	size_t s1 = sizeof( '\u2019' );

	const uint32_t C = 0x0001F436;
	uint32_t H = std::floor((C - 0x10000) / 0x400) + 0xD800;
	uint32_t L = (C - 0x10000) % 0x400 + 0xDC00;

	//std::stringw
	uint32_t ui32Bleh[32];
	const wchar_t puBlehgh[] = { 0x2019, 0x0 };
	Utf16ToUnicodeRaw( ui32Bleh, (const uint16_t *)puBlehgh, 32 );

	uint32_t ui32BlehDst[32];
	Utf16ToUtf32( ui32BlehDst, (const uint16_t *)puBlehgh, 32 );

	std::string strTemp = NumToUtf8( 0x0001F436 );
	uint32_t uiBlah21212[] = { 0x00000075, 0x00000039, 0x00000045, 0x00000035, 0x00000000 };
	//{ 0x9E5, 0x0 };
	char puBlehgh212[32];
	RawUnicodeToUtf8( (uint8_t *)puBlehgh212, uiBlah21212, 32 );


	const wchar_t wPrin[] = {
		ui32Blah2 && 0xFFUL,
		(ui32Blah2 >> 8UL) && 0xFFUL,
		//0x2019,
		//0x201C,
		0x0
	};
	char wPrin32Dst[32];
	Utf16ToUnicodeRaw( ui32Bleh, (const uint16_t *)wPrin, 32 );
	RawUnicodeToUtf8( (uint8_t *)wPrin32Dst, ui32Bleh, 32 );
	OutputDebugStringW( wPrin );
	OutputDebugStringA( wPrin32Dst );
#endif
	//const char * pcUtf8Temp = u8"\xEF\xB9\x8C";
	//const wchar_t * pwcUtf16 = L"\u0041\u002B\u00E1\u0103\u01CE\u00C2\u0103";	// A+áăǎÂă
	/*
	for ( auto I = -1; I < 256; ++I ) {
		if ( !::isprint( I ) ) {
			::OutputDebugStringA( (std::to_string( I ) + "\r\n").c_str() );
		}
	}
	::OutputDebugStringA( (std::to_string( __cplusplus ) + "\r\n").c_str() );*/

	mx::CWindowMemHack wmhMemHack;
	// Create the windows.
	//mx::CMainWindowLayout::CreateMainWindow();
	//mx::COpenProcessLayout::CreateOpenProcessDialog( mx::CMainWindowLayout::MainWindow() );

	/*LSW_WIDGET_LAYOUT wlDock = {
		LSW_LT_DOCKWINDOW,						// ltType
		125,									// wId
		nullptr,								// lpwcClass
		TRUE,									// bEnabled
		TRUE,									// bActive
		154,									// iLeft
		153,									// iTop
		163,									// dwWidth
		174,									// dwHeight
		WS_VISIBLE,								// dwStyle
		0,										// dwStyleEx
		L"L. Spiro",							// pwcText
		0,										// sTextLen
		0,										// dwParentId
	};
	//lsw::CBase::LayoutManager()->CreateWindowX( &wlDock, 1, nullptr, 0 );
	//lsw::CBase::LayoutManager()->CreateWidget( lsw::CLayoutManager::FixLayout( wlDock ), mx::CMainWindowLayout::MainWindow(), true, NULL );
	wlDock.wId++;
	wlDock.pwcText = L"Found Addresses";
	wlDock.iLeft = 500;*/
	//lsw::CBase::LayoutManager()->CreateWidget( lsw::CLayoutManager::FixLayout( wlDock ), mx::CMainWindowLayout::MainWindow(), true, NULL );

	//mx::CFoundAddressLayout::CreateFoundAddressesWindow( mx::CMainWindowLayout::MainWindow() );
	/*static_cast<mx::CMhsMainWindow *>(mx::CMainWindowLayout::MainWindow())->ShowFoundAddress();
	static_cast<mx::CMhsMainWindow *>(mx::CMainWindowLayout::MainWindow())->ShowExpEval();*/

	/*WCHAR szBuffer[MAX_PATH];
	::GetCurrentDirectoryW( MAX_PATH, szBuffer );*/
	//lsw::CMainWindow mwWindow( L"Agh", L"L. Spiro MHS X" );
	//double dSin = ::sin( 3.1415926535897932384626433832795 );
	/*
	lsw::CBitmap bMap;
	bMap.LoadFromFile( L"Resources/98.bmp", 0, 0, LR_LOADMAP3DCOLORS );
	*/

	// Controls seconds_since_start(), milliseconds_since_start(), etc., Expression Evaluator.
	// We move it up as close to the start of the loop as possible so that these values most closely mark the actual time that meaningful execution
	//	takes place (clock() returns the time since the EXE actually started (before main() is even called), so we don't need more tickers from that
	//	time.
	// In a way, this allows (clock() - milliseconds_since_start()) to print the time it takes to initialize.
	ee::InitializeExpressionEvaluatorLibrary();
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
#endif	// #ifdef MX_QT

#ifdef MX_QT
int main( int _iArgs, char * _pcArgs[] ) {
	// Initialize the system.
	mx::CSystem::InitSystem();

	QApplication aApp( _iArgs, _pcArgs );
	CMain mxWindow;
	mxWindow.show();
	return aApp.exec();
}
#else
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
#endif	// #ifdef MX_QT

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