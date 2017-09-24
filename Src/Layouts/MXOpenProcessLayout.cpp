#include "MXOpenProcessLayout.h"
#include "../Layouts/MXLayoutMacros.h"
#include "../Layouts/MXLayoutManager.h"
#include "../System/MXSystem.h"
#include "../Utilities/MXUtilities.h"

#include <Base/LSWBase.h>
#include <Layout/LSWLayoutManager.h>

namespace mx {

	// == Members.
	// The layout for the Open Process dialog.
	LSW_WIDGET_LAYOUT COpenProcessLayout::m_wlOpenProcessDialog[] = {
		{
			MX_OPEN_PROCESS_WINDOW,					// ltType
			MX_OPI_DIALOG,							// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			64,										// iLeft
			64,										// iTop
			271,									// dwWidth
			198,									// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | WS_THICKFRAME | DS_3DLOOK | DS_FIXEDSYS | DS_CENTER,					// dwStyle
			//WS_POPUP | WS_BORDER | WS_SYSMENU | DS_MODALFRAME | WS_CAPTION,
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,												// dwStyleEx
			MX_MAKE_WCHAR( _T_49D5B53D_Open_Process ),																									// pwcText
			_LEN_49D5B53D,							// sTextLen
			MX_OPI_NONE,							// dwParentId
		},
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_OPI_GROUP,							// wId
			L"Button",								// lpwcClass
			TRUE,									// bEnabled
			4,										// iLeft
			4,										// iTop
			263,									// dwWidth
			174,									// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | BS_GROUPBOX,																									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,																// dwStyleEx
			MX_MAKE_WCHAR( _T_2FF354B7_Select_Process ),																								// pwcText
			_LEN_2FF354B7,							// sTextLen
			MX_OPI_DIALOG,							// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_LOCK_TOP,							// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},
		{
			LSW_LT_LISTVIEW,						// ltType
			MX_OPI_LISTVIEW,						// wId
			L"SysListView32",						// lpwcClass
			TRUE,									// bEnabled
			8,										// iLeft
			13,										// iTop
			255,									// dwWidth
			110,									// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | WS_TABSTOP | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_ALIGNLEFT,		// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY | WS_EX_CLIENTEDGE | LVS_EX_FULLROWSELECT,						// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_OPI_DIALOG,							// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			MX_LOCK_TOP,							// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_OPI_BUTTON_OK,						// wId
			L"Button",								// lpwcClass
			TRUE,									// bEnabled
			4,										// iLeft
			182,									// iTop
			43,										// dwWidth
			12,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_TEXT,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,						// dwStyleEx
			MX_MAKE_WCHAR( _T_D736D92D_OK ),																	// pwcText
			_LEN_D736D92D,							// sTextLen
			MX_OPI_DIALOG,							// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			nullptr, 0,								// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			MX_FIXED_WIDTH,							// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_OPI_BUTTON_CANCEL,					// wId
			L"Button",								// lpwcClass
			TRUE,									// bEnabled
			224,									// iLeft
			182,									// iTop
			43,										// dwWidth
			12,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_TEXT,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,						// dwStyleEx
			MX_MAKE_WCHAR( _T_51BAC044_Cancel ),																// pwcText
			_LEN_51BAC044,							// sTextLen
			MX_OPI_DIALOG,							// dwParentId

			nullptr, 0,								// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			MX_FIXED_WIDTH,							// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_GROUPBOX,						// ltType
			MX_OPI_GROUPSHOW,						// wId
			L"Button",								// lpwcClass
			TRUE,									// bEnabled
			9,										// iLeft
			128,									// iTop
			253,									// dwWidth
			20,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | BS_GROUPBOX,															// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,						// dwStyleEx
			MX_MAKE_WCHAR( _T_923C763F_Show ),																	// pwcText
			_LEN_923C763F,							// sTextLen
			MX_OPI_DIALOG,							// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_OPI_RADIO_MAIN,						// wId
			L"Button",								// lpwcClass
			TRUE,									// bEnabled
			14,										// iLeft
			136,									// iTop
			29,										// dwWidth
			9,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | BS_AUTORADIOBUTTON,										// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,						// dwStyleEx
			MX_MAKE_WCHAR( _T_1F1A625A_Main ),																	// pwcText
			_LEN_1F1A625A,							// sTextLen
			MX_OPI_DIALOG,							// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			nullptr, 0,								// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			MX_FIXED_WIDTH,							// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_RADIO,							// ltType
			MX_OPI_RADIO_ALL,						// wId
			L"Button",								// lpwcClass
			TRUE,									// bEnabled
			238,									// iLeft
			136,									// iTop
			20,										// dwWidth
			9,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | BS_AUTORADIOBUTTON,										// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,						// dwStyleEx
			MX_MAKE_WCHAR( _T_0355373D_All ),																	// pwcText
			_LEN_0355373D,							// sTextLen
			MX_OPI_DIALOG,							// dwParentId

			nullptr, 0,								// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			MX_FIXED_WIDTH,							// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_CHECK,							// ltType
			MX_OPI_CHECK_RESTR,						// wId
			L"Button",								// lpwcClass
			FALSE,									// bEnabled
			9,										// iLeft
			150,									// iTop
			253,									// dwWidth
			9,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,											// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,						// dwStyleEx
			MX_MAKE_WCHAR( _T_B563255B_Open_in_Restricted_Mode__Improves_Undetectability_ ),					// pwcText
			_LEN_B563255B,							// sTextLen
			MX_OPI_DIALOG,							// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
		{
			LSW_LT_BUTTON,							// ltType
			MX_OPI_BUTTON_REFRESH,					// wId
			L"Button",								// lpwcClass
			TRUE,									// bEnabled
			9,										// iLeft
			161,									// iTop
			253,									// dwWidth
			12,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_TEXT,									// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_NOPARENTNOTIFY,						// dwStyleEx
			MX_MAKE_WCHAR( _T_AF3C9967_Refresh ),																// pwcText
			_LEN_AF3C9967,							// sTextLen
			MX_OPI_DIALOG,							// dwParentId

			MX_LOCK_LEFT,							// pcLeftSizeExp
			MX_LOCK_RIGHT,							// pcRightSizeExp
			nullptr, 0,								// pcTopSizeExp
			MX_LOCK_BOTTOM,							// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			MX_FIXED_HEIGHT,						// pcHeightSizeExp
		},
	};


	// == Functions.
	// Creates the Open Process dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
	BOOL COpenProcessLayout::CreateOpenProcessDialog( CWidget * _pwParent ) {
		std::vector<std::string> sStrings;
		std::vector<std::wstring> sStringsW;
		sStrings.resize( MX_ELEMENTS( m_wlOpenProcessDialog ) * 6 );	// Each control can have up to 6 normal strings.
		sStringsW.resize( MX_ELEMENTS( m_wlOpenProcessDialog ) * 1 );
		size_t sStringIndex = 0;
		size_t sStringWIndex = 0;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		for ( size_t I = 0; I < MX_ELEMENTS( m_wlOpenProcessDialog ); ++I ) {
			LSW_WIDGET_LAYOUT wlTemp = m_wlOpenProcessDialog[I];
			if ( wlTemp.pwcText ) {
				std::string sTemp;
				for ( size_t J = 0; J < wlTemp.sTextLen; ++J ) {
					sTemp.push_back( static_cast<char>(wlTemp.pwcText[J]) );
				}
				sStringsW[sStringWIndex] = mx::CStringDecoder::DecodeToWString( sTemp.c_str(), wlTemp.sTextLen );
				wlTemp.pwcText = sStringsW[sStringWIndex++].c_str();
			}

			if ( wlTemp.pcLeftSizeExp ) {
				sStrings[sStringIndex] = mx::CStringDecoder::DecodeToString( wlTemp.pcLeftSizeExp, wlTemp.sLeftSizeExpLen );
				wlTemp.pcLeftSizeExp = sStrings[sStringIndex++].c_str();
			}
			if ( wlTemp.pcRightSizeExp ) {
				sStrings[sStringIndex] = mx::CStringDecoder::DecodeToString( wlTemp.pcRightSizeExp, wlTemp.sRightSizeExpLen );
				wlTemp.pcRightSizeExp = sStrings[sStringIndex++].c_str();
			}
			if ( wlTemp.pcTopSizeExp ) {
				sStrings[sStringIndex] = mx::CStringDecoder::DecodeToString( wlTemp.pcTopSizeExp, wlTemp.sTopSizeExpLen );
				wlTemp.pcTopSizeExp = sStrings[sStringIndex++].c_str();
			}
			if ( wlTemp.pcBottomSizeExp ) {
				sStrings[sStringIndex] = mx::CStringDecoder::DecodeToString( wlTemp.pcBottomSizeExp, wlTemp.sBottomSizeExpLen );
				wlTemp.pcBottomSizeExp = sStrings[sStringIndex++].c_str();
			}
			if ( wlTemp.pcWidthSizeExp ) {
				sStrings[sStringIndex] = mx::CStringDecoder::DecodeToString( wlTemp.pcWidthSizeExp, wlTemp.sWidthSizeExpLen );
				wlTemp.pcWidthSizeExp = sStrings[sStringIndex++].c_str();
			}
			if ( wlTemp.pcHeightSizeExp ) {
				sStrings[sStringIndex] = mx::CStringDecoder::DecodeToString( wlTemp.pcHeightSizeExp, wlTemp.sHeightSizeExpLen );
				wlTemp.pcHeightSizeExp = sStrings[sStringIndex++].c_str();
			}
			vLayouts.push_back( wlTemp );
		}
		if ( lsw::CBase::LayoutManager()->CreateDialogX( &vLayouts[0], MX_ELEMENTS( m_wlOpenProcessDialog ), _pwParent ) ) {
			// Success.  Do stuff.
			return TRUE;
		}

		return FALSE;
	}

}	// namespace mx
