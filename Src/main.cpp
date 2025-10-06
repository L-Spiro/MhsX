
#include "MXMhsX.h"
#include <EEExpEval.h>
#include "Layouts/MXLayoutManager.h"
#include "MemHack/MXWindowMemHack.h"
#include "Utilities/MXUtilities.h"

#ifdef _DEBUG
//#define MX_XML_TEST
#endif	// #ifdef _DEBUG
#ifdef MX_XML_TEST
#include "Disassembler x86_x64/MXDisassemGen.h"
#endif	// #ifdef MX_XML_TEST

#ifdef _DEBUG
//#define MX_JSON_TEST
#endif	// #ifdef _DEBUG
#ifdef MX_JSON_TEST
#include "LSONJson.h"
#endif	// #ifdef MX_JSON_TEST

int wWinMain( HINSTANCE _hInstance, HINSTANCE _hPrevInstance, LPWSTR _lpCmdLine, int _nCmdShow ) {
#ifdef MX_XML_TEST
	mx::CDisassemGen dgGen;
	if ( dgGen.LoadXml( L"J:\\My Projects\\MHS X\\Research\\x86reference.xml" ) ) {
		dgGen.PrintTree();
		std::map<std::string, std::set<std::string>> mmMap;
		dgGen.GetContainer()->GatherElements( mmMap );
		mmMap.clear();
		dgGen.GetContainer()->GatherAttributes( mmMap );
		if ( mmMap.size() == 0 ) {
		}
		//dgGen.GenerateGroupMasks();
		//dgGen.GenerateInstExt();
		dgGen.GenerateOperandAddressFlags();
		dgGen.GenerateInstructionTable();
	}
#endif	// #ifdef MX_XML_TEST


#ifdef MX_JSON_TEST
	{
		lson::CJson jSon;
		std::vector<uint8_t> vBytes;
		mx::CFile::LoadToMemory( L"J:\\My Projects\\MHS X\\Research\\02.json", vBytes );
		vBytes.push_back( 0 );

		if ( !jSon.SetJson( reinterpret_cast<const char *>(vBytes.data()) ) ) {
			::OutputDebugStringA( "JSON FAIL\r\n" );
		}
		else {
			::OutputDebugStringA( "JSON NOT FAIL\r\n" );
		}
	}

#endif	// #ifdef MX_JSON_TEST


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
		if ( lsw::CBase::GetAccelHandler().Translate( mMsg ) ) { continue; }

		//if ( mMsg.hwnd && ::IsDialogMessageW( ::GetAncestor( mMsg.hwnd, GA_ROOT ), &mMsg ) ) { continue; }

		
		::TranslateMessage( &mMsg );
		::DispatchMessageW( &mMsg );
	}

	lsw::CBase::ShutDown();
	return static_cast<int>(mMsg.wParam);
}
