
#include "MXMhsX.h"
#include <EEExpEval.h>
#include "Layouts/MXLayoutManager.h"
#include "MemHack/MXWindowMemHack.h"
#include "Utilities/MXUtilities.h"


#include <LSXml.h>


int wWinMain( HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPWSTR _lpCmdLine, int _nCmdShow ) {
	std::vector<uint8_t> vBytes;
	mx::CFile::LoadToMemory( L"J:\\My Projects\\MHS X\\Research\\x86reference.xml", vBytes );
	//mx::CFile::LoadToMemory( L"J:\\My Projects\\MHS X\\Research\\Test.xml", vBytes );
	vBytes.push_back( 0 );
	lsx::CXml xXml;
	xXml.SetXml( (const char *)(vBytes.data()) );


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
	mx::CWindowMemHack wmhMemHack;

	// Controls seconds_since_start(), milliseconds_since_start(), etc., Expression Evaluator.
	// We move it up as close to the start of the loop as possible so that these values most closely mark the actual time that meaningful execution
	//	takes place (clock() returns the time since the EXE actually started (before main() is even called), so we don't need more tickers from that
	//	time.
	// In a way, this allows (clock() - milliseconds_since_start()) to print the time it takes to initialize.
	ee::CExpEval::InitializeExpressionEvaluatorLibrary();
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
