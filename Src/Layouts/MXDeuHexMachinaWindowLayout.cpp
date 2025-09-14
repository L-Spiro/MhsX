#include "MXDeuHexMachinaWindowLayout.h"
#include "../HexEditor/MXDeusHexMachinaWindow.h"
#include "../Utilities/MXUtilities.h"
#include "../Strings/MXStringDecoder.h"
#include "MXLayoutMacros.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>
//#include <Base/LSWWndClassEx.h>
//#include <Layout/LSWLayoutManager.h>

namespace mx {

#define MX_W_W				1460
#define MX_W_H				940
	// == Members.
	// The layout for the PE Works window.
	LSW_WIDGET_LAYOUT CDeusHexMachinaLayout::m_wlMainWindow[] = {
		{
			MX_DEUS_HEX_MACHINA,							// ltType
			MX_W_MAINWINDOW,								// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			TRUE,											// bActive
			155,											// iLeft
			150,											// iTop
			MX_W_W,											// dwWidth
			MX_W_H,											// dwHeight
			WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | DS_3DLOOK | DS_SETFONT,				// dwStyle
			WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,														// dwStyleEx
			MX_MAKE_WCHAR( _T_BF7FD5A0_Deus_Hex_Machina ),	// pwcText
			_LEN_BF7FD5A0,									// sTextLen
			MX_W_NONE,										// dwParentId
			/*
			MX_PARENT_VCLEFT,						// pcLeftSizeExp
			MX_PARENT_VCRIGHT,						// pcRightSizeExp
			MX_PARENT_VCTOP,						// pcTopSizeExp
			MX_PARENT_VCBOTTOM,						// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
			*/
		},
		{
			LSW_LT_STATUSBAR,								// ltType
			MX_W_STATUSBAR,									// wId
			STATUSCLASSNAMEW,								// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			0,												// iLeft
			0,												// iTop
			0,												// dwWidth
			0,												// dwHeight
			WS_VISIBLE | WS_CHILD | SBARS_SIZEGRIP,			// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_W_MAINWINDOW,								// dwParentId

			nullptr, 0,										// pcLeftSizeExp
			nullptr, 0,										// pcRightSizeExp
			nullptr, 0,										// pcTopSizeExp
			nullptr, 0,										// pcBottomSizeExp
			nullptr, 0,										// pcWidthSizeExp
			nullptr, 0,										// pcHeightSizeExp
		},
#if 1
		{
			LSW_LT_REBAR,									// ltType
			MX_W_REBAR0,									// wId
			REBARCLASSNAMEW,								// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			0,												// iLeft
			0,												// iTop
			m_wlMainWindow[0].dwWidth,						// dwWidth
			150,											// dwHeight
			WS_VISIBLE | WS_BORDER | WS_CHILD | WS_CLIPCHILDREN |
				WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NOPARENTALIGN |
				RBS_VARHEIGHT | RBS_BANDBORDERS,			// dwStyle
			0,//WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_TOOLWINDOW,										// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_W_MAINWINDOW,								// dwParentId

			MX_PARENT_CLEFT,//MX_LOCK_LEFT,					// pcLeftSizeExp
			MX_PARENT_CRIGHT,								// pcRightSizeExp
			MX_PARENT_CTOP,									// pcTopSizeExp
			nullptr, 0,										// pcBottomSizeExp
			nullptr, 0,										// pcWidthSizeExp
			MX_FIXED_HEIGHT,								// pcHeightSizeExp
		},
		{
			LSW_LT_TOOLBAR,									// ltType
			MX_W_TOOLBAR0,									// wId
			TOOLBARCLASSNAMEW,								// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			0,												// iLeft
			0,												// iTop
			0,												// dwWidth
			0,												// dwHeight
			WS_CHILD | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | WS_CLIPCHILDREN |
				WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NORESIZE | WS_VISIBLE | CCS_ADJUSTABLE,		// dwStyle
			0,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_W_MAINWINDOW,								// dwParentId
		},
#endif
		{
			LSW_LT_TAB,										// ltType
			MX_W_TABS,										// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			0,												// iLeft
			0,												// iTop
			MX_W_W,											// dwWidth
			MX_W_H,											// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | TCS_HOTTRACK,				// dwStyle
			WS_EX_ACCEPTFILES,								// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_W_MAINWINDOW,								// dwParentId

			MX_PARENT_VCLEFT,								// pcLeftSizeExp
			MX_PARENT_VCRIGHT,								// pcRightSizeExp
			MX_PARENT_VCTOP,								// pcTopSizeExp
			MX_PARENT_VCBOTTOM,								// pcBottomSizeExp
			nullptr, 0,										// pcWidthSizeExp
			nullptr, 0,										// pcHeightSizeExp
		},
		/*{
			LSW_LT_TREELIST,						// ltType
			MX_W_TESTTREELIST,					// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			FALSE,									// bActive
			0,										// iLeft
			0,										// iTop
			255,									// dwWidth
			110,									// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP,								// dwStyle
			0,																		// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			MX_W_MAINWINDOW,						// dwParentId

			MX_PARENT_VCLEFT,						// pcLeftSizeExp
			MX_PARENT_VCRIGHT,						// pcRightSizeExp
			MX_PARENT_VCTOP,						// pcTopSizeExp
			MX_PARENT_VCBOTTOM,						// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
		},*/
	};

#undef MX_W_W
#undef MX_W_H

	// Menu bar items for the main window.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miMenuBar[] = {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_M_MENU_FILE,									FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_854A6B8B__File, _LEN_854A6B8B ) },
		{ FALSE,		MX_M_MENU_EDIT,									FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_88E08381__Edit, _LEN_88E08381 ) },
		{ FALSE,		MX_M_MENU_SELECT,								FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_CC9D9D8D__Select, _LEN_CC9D9D8D ) },
		{ FALSE,		MX_M_MENU_FIND,									FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_9CF72232_Fin_d, _LEN_9CF72232 ) },
		{ FALSE,		MX_M_MENU_VIEW,									FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_F728F615__View, _LEN_F728F615 ) },
		{ FALSE,		MX_M_MENU_FORMAT,								FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_DA97462D_F_ormat, _LEN_DA97462D ) },
		{ FALSE,		MX_M_MENU_SCRIPTS,								FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_85EAC004_Scri_pts, _LEN_85EAC004 ) },
		{ FALSE,		MX_M_MENU_TEMPLATES,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_6ED47C07_Temp_lates, _LEN_6ED47C07 ) },
		{ FALSE,		MX_M_MENU_TOOLS,								FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_8D2E41E6__Tools, _LEN_8D2E41E6 ) },
		//{ FALSE,		MX_M_MENU_WINDOW,								FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_854A6B8B__File, _LEN_854A6B8B ) },
		{ FALSE,		MX_M_MENU_HELP,									FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_01520137__Help, _LEN_01520137 ) },
	};

	// File menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miFileMenu[] {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_M_FILE_NEW,									FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_08F2F449__New, _LEN_08F2F449 ) },
		{ FALSE,		MX_M_FILE_OPENFILE,								FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_2BEA22F9__Open_File, _LEN_2BEA22F9 ) },
		{ FALSE,		MX_M_FILE_OPENCURPROCESS,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_3E14AB9A_Open__Current_Process, _LEN_3E14AB9A ) },
		{ FALSE,		MX_M_FILE_OPENFILE,								FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_364A9A57_Open__Process, _LEN_364A9A57 ) },
		{ FALSE,		MX_M_FILE_OPENRECENT,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_EA79B404_Open__Recent, _LEN_EA79B404 ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_FILE_SAVE,									FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_5CB36745__Save, _LEN_5CB36745 ) },
		{ FALSE,		MX_M_FILE_SAVE_AS,								FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_034D4A63_Save__As___, _LEN_034D4A63 ) },
		{ FALSE,		MX_M_FILE_SAVE_A_COPY,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_EE6562A1_Save_a_Cop_y___, _LEN_EE6562A1 ) },
		{ FALSE,		MX_M_FILE_SAVE_SELECTION,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_B78BF6D5_Save_Selec_tion___, _LEN_B78BF6D5 ) },
		{ FALSE,		MX_M_FILE_SAVE_ALL,								FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_CD75926E_Sa_ve_All, _LEN_CD75926E ) },
		{ FALSE,		MX_M_FILE_CLOSE,								FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_74D51E55__Close, _LEN_74D51E55 ) },
		{ FALSE,		MX_M_FILE_CLOSE_ALL,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_1D146047_C_lose_All, _LEN_1D146047 ) },
		{ FALSE,		MX_M_FILE_REVERT_REFRESH,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_1E3CFF84_Revert_Refres_h, _LEN_1E3CFF84 ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_FILE_SPECIAL,								FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_2EF51FCB_Spec_ial, _LEN_2EF51FCB ) },
		{ FALSE,		MX_M_FILE_IMPORT_HEX,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_CE0CAA04_I_mport_Hex___, _LEN_CE0CAA04 ) },
		{ FALSE,		MX_M_FILE_EXPORT_HEX,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_33C7BDA7__Export_Hex___, _LEN_33C7BDA7 ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_FILE_EXIT,									FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_6ADD8CE5_E_xit, _LEN_6ADD8CE5 ) },
	};

	// File -> New menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miFileNewMenu[] = {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_M_FILE_NEW_TEXT_FILE,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_E8E120C8_New__Text_File, _LEN_E8E120C8 ) },
		{ FALSE,		MX_M_FILE_NEW_HEX_FILE,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_99EE2388_New__Hex_File, _LEN_99EE2388 ) },
		{ FALSE,		MX_M_FILE_NEW_UNICODE_FILE,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_D9722ACF_New__Unicode_File, _LEN_D9722ACF ) },
		{ FALSE,		MX_M_FILE_NEW_UTF8_FILE,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_D9129A81_New_UTF__8_File, _LEN_D9129A81 ) },
		{ FALSE,		MX_M_FILE_NEW_C_FILE,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_AA94A061_New__C_C___File, _LEN_AA94A061 ) },
		{ FALSE,		MX_M_FILE_NEW_HTML_FILE,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_92AFDE30_New_HT_ML_File, _LEN_92AFDE30 ) },
		{ FALSE,		MX_M_FILE_NEW_PHP_FILE,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_F7042DA0_New_PHP_File, _LEN_F7042DA0 ) },
		{ FALSE,		MX_M_FILE_NEW_XML_FILE,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_86EC20E3_New__XML_File, _LEN_86EC20E3 ) },
	};

	 // File -> Special menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miFileSpecialMenu[] = {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_M_FILE_RENAME_FILE,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_C5BD4991__Rename_File___, _LEN_C5BD4991 ) },
		{ FALSE,		MX_M_FILE_DELETE_FILE,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_73ECC2A6__Delete_File, _LEN_73ECC2A6 ) },
	};

	// Edit menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miEditMenu[] = {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_M_EDIT_UNDO,									FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_EAA5E669__Undo, _LEN_EAA5E669 ) },
		{ FALSE,		MX_M_EDIT_REDO,									FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_7B273131__Redo, _LEN_7B273131 ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_EDIT_CUT,									FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_6E69B475_Cu_t, _LEN_6E69B475 ) },
		{ FALSE,		MX_M_EDIT_COPY,									FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_446FE619__Copy, _LEN_446FE619 ) },
		{ FALSE,		MX_M_EDIT_COPY_AS_,								FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_F479063A_Cop_y_As, _LEN_F479063A ) },
		{ FALSE,		MX_M_EDIT_PASTE,								FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_FB82E709__Paste, _LEN_FB82E709 ) },
		{ FALSE,		MX_M_EDIT_PASTE_SPECIAL,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_5C3FAC38_Paste_Spec_ial___, _LEN_5C3FAC38 ) },
		{ FALSE,		MX_M_EDIT_DELETE,								FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_3DBE79B1_Delete, _LEN_3DBE79B1 ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_EDIT_INSERT_OVERWRITE,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_CB4B4FA5_Insert_O_verwrite, _LEN_CB4B4FA5 ) },
		{ FALSE,		MX_M_EDIT_INSERT_COLOR,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_B57D707F_Insert_Co_lor___, _LEN_B57D707F ) },
		{ FALSE,		MX_M_EDIT_INSERT_DATE,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_BB3A17D9_I_nsert_Data_Time, _LEN_BB3A17D9 ) },
		{ FALSE,		MX_M_EDIT_SET_FILE_SIZE,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_90E4EE8E_Set_File_Si_ze___, _LEN_90E4EE8E ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_EDIT_READ_ONLY,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_03A5AEDB_Read__Only, _LEN_03A5AEDB ) },
		{ FALSE,		MX_M_EDIT_KEEP_FILE_TIME,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_381FB8E7__Keep_File_Time, _LEN_381FB8E7 ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_EDIT_PROPERTIES,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_CB94BF68_Prop_erties, _LEN_CB94BF68 ) },
	};

	// Edit -> Copy As menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miEditCopyAsMenu[] = {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_M_EDIT_COPY_AS_HEX,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_2F29967F__Copy_as_Hex_Text, _LEN_2F29967F ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_EDIT_COPY_AS_DECIMAL,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_1E422E7D_Copy_as__Decimal_Text, _LEN_1E422E7D ) },
		{ FALSE,		MX_M_EDIT_COPY_AS_BINARY,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_83F240E0_Copy_as_B_inary_Text, _LEN_83F240E0 ) },
		{ FALSE,		MX_M_EDIT_COPY_AS_C,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_CA6D9CC2_Copy_as_C_C_ode, _LEN_CA6D9CC2 ) },
		{ FALSE,		MX_M_EDIT_COPY_AS_JAVA,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_887AD3E5_Copy_as__Java_Code, _LEN_887AD3E5 ) },
		{ FALSE,		MX_M_EDIT_COPY_AS_PYTHON,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_EB7A8579_Copy_as__Python_Code, _LEN_EB7A8579 ) },
		{ FALSE,		MX_M_EDIT_COPY_AS_8_BIT_HEX,					FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_2CCFF31F_Copy_as_Intel__8_Bit_Hex_Code, _LEN_2CCFF31F ) },
		{ FALSE,		MX_M_EDIT_COPY_AS_16_BIT_HEX,					FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_EA9F97C5_Copy_as_Intel__16_Bit_Hex_Code, _LEN_EA9F97C5 ) },
		{ FALSE,		MX_M_EDIT_COPY_AS_32_BIT_HEX,					FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_5BB5A94F_Copy_as_Intel__32_Bit_Hex_Code, _LEN_5BB5A94F ) },
		{ FALSE,		MX_M_EDIT_COPY_AS_MOTOROLA_S19,					FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_F0ED12D9_Copy_as_Motorola_S1_9_Records, _LEN_F0ED12D9 ) },
		{ FALSE,		MX_M_EDIT_COPY_AS_MOTOROLA_S28,					FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_4E1EF640_Copy_as_Motorola_S_28_Records, _LEN_4E1EF640 ) },
		{ FALSE,		MX_M_EDIT_COPY_AS_MOTOROLA_S37,					FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_6043708A_Copy_as_Motorola_S3_7_Records, _LEN_6043708A ) },
		{ FALSE,		MX_M_EDIT_COPY_AS_HEX_EDITOR_AREAS,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_DD2A433C_Copy_as_Hex_Edi_tor_Areas, _LEN_DD2A433C ) },
		{ FALSE,		MX_M_EDIT_COPY_AS_WEB_PAGE,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_F20B9059_Copy_as_Web_Page___HTML_, _LEN_F20B9059 ) },
		{ FALSE,		MX_M_EDIT_COPY_AS_RICH_TEXT,					FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_BDA0AEC0_Copy_as_Rich_Text_Format___RTF_, _LEN_BDA0AEC0 ) },
		{ FALSE,		MX_M_EDIT_COPY_AS_BASE64,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_887B0947_Copy_as__Base64, _LEN_887B0947 ) },
		{ FALSE,		MX_M_EDIT_COPY_AS_UUENCODING,					FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_43198F35_Copy_as__Uuencoding, _LEN_43198F35 ) },
	};

	// Edit -> Insert/Overwrite.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miEditInsertOverwriteMenu[] = {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_M_EDIT_INSERT_FILE,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_899D8058_Insert__File___, _LEN_899D8058 ) },
		{ FALSE,		MX_M_EDIT_INSERT_BYTES,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_09EEA57F_Insert__Bytes___, _LEN_09EEA57F ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_EDIT_OVERWRITE_FILE,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_9BAF9C05__Overwrite_File___, _LEN_9BAF9C05 ) },
		{ FALSE,		MX_M_EDIT_OVERWRITE_BYTES,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_B6216D76_O_verwrite_Bytes___, _LEN_B6216D76 ) },
	};

	// Select menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miSelectMenu[] = {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_M_SELECT_SELECT_ALL,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_0D2831F4_Select__All, _LEN_0D2831F4 ) },
		{ FALSE,		MX_M_SELECT_WORD,								FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_8175DD3D_Select__Word, _LEN_8175DD3D ) },
		{ FALSE,		MX_M_SELECT_LINE,								FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_93901CDA_Select__Line, _LEN_93901CDA ) },
		{ FALSE,		MX_M_SELECT_SELECT_RANGE,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_2ED4F2F2__Select_Range___, _LEN_2ED4F2F2 ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_SELECT_EXPAND_SELECTION,					FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_48180F4F_E_xpand_Selection, _LEN_48180F4F ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_SELECT_REVSERSE_SELECTION,					FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_D0352186__Reverse_Selection, _LEN_D0352186 ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_SELECT_MARK_SELECTION_START,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_3AD86671__Mark_Selection_Start, _LEN_3AD86671 ) },
		{ FALSE,		MX_M_SELECT_MARK_SELECTION_END,					FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_235B2698_Mark_Selection__End, _LEN_235B2698 ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_SELECT_COLUMN_MODE,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_B29A2EA2__Column_Mode, _LEN_B29A2EA2 ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_SELECT_MOVE_SELECTION_BACK,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_F1AEC748_Move_Selection__Back, _LEN_F1AEC748 ) },
		{ FALSE,		MX_M_SELECT_MOVE_SELECTION_FORWARD,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_A3D4C6A9_Move_Selection__Forward, _LEN_A3D4C6A9 ) },
	};

	// Find menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miFindMenu[] = {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_M_FIND_FIND,									FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_6E0D9949__Find___, _LEN_6E0D9949 ) },
		{ FALSE,		MX_M_FIND_FIND_NEXT,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_F6024D9D_Find__Next, _LEN_F6024D9D ) },
		{ FALSE,		MX_M_FIND_FIND_PREV,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_E0E59780_Find__Previous, _LEN_E0E59780 ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_FIND_REPLACE,								FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_0477761F__Replace, _LEN_0477761F ) },
		{ FALSE,		MX_M_FIND_REPLACE_NEXT,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_6ADBABDA_Rep_lace_Next, _LEN_6ADBABDA ) },
		{ FALSE,		MX_M_FIND_REPLACE_PREV,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_9EEEB248_Replace_Previo_us, _LEN_9EEEB248 ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_FIND_FIND_STRINGS,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_813C2014_F_ind_Strings___, _LEN_813C2014 ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_FIND_FIND_IN_FILES,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_F593DB67_Fin_d_in_Files___, _LEN_F593DB67 ) },
		{ FALSE,		MX_M_FIND_REPLACE_IN_FILES,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_3CAB5A45_R_eplace_in_Files___, _LEN_3CAB5A45 ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_FIND_GOTO,									FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_E37279C2__Goto___, _LEN_E37279C2 ) },
		{ FALSE,		MX_M_FIND_GOTO_AGAIN,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_9367A5D7_G_oto_Again, _LEN_9367A5D7 ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_FIND_ADD_BOOKMARK,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_CB091DF0__Add_Edit_Bookmark, _LEN_CB091DF0 ) },
		{ FALSE,		MX_M_FIND_TOGGLE_BOOKMARK,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_88AE7C0F_Toggle__Bookmark, _LEN_88AE7C0F ) },
		{ FALSE,		MX_M_FIND_NEXT_BOOKMARK,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_C0CDF736_Ne_xt_Bookmark, _LEN_C0CDF736 ) },
		{ FALSE,		MX_M_FIND_PREV_BOOKMARK,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_3D5BB1EB_Pre_vious_Bookmark, _LEN_3D5BB1EB ) },
		{ FALSE,		MX_M_FIND_CLEAR_BOOKMARKS,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_B340003C__Clear_All_Bookmarks, _LEN_B340003C ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_FIND_JUMP_TO_TEMPLATE_VARIABLE,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_FB0FF0BC__Jump_To_Template_Variable, _LEN_FB0FF0BC ) },
		{ FALSE,		MX_M_FIND_JUMP_TO_MATCHING_BRACE,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_12591B76_Jump_To__Matching_Brace_Tag, _LEN_12591B76 ) },
	};

	// View menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miViewMenu[] = {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_M_VIEW_EDIT_AS_,								FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_530B68DF_E_dit_As, _LEN_530B68DF ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_VIEW_FONT,									FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_D9415549__Font, _LEN_D9415549 ) },
		{ FALSE,		MX_M_VIEW_CHARACTER_SET,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_FEB84FB6__Character_Set, _LEN_FEB84FB6 ) },
		{ FALSE,		MX_M_VIEW_ADDRESSES,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_AAFBF8FA__Addresses, _LEN_AAFBF8FA ) },
		{ FALSE,		MX_M_VIEW_LINE_WIDTH,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_E1526D62__Line_Width, _LEN_E1526D62 ) },
		{ FALSE,		MX_M_VIEW_GROUP_BY,								FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_1A28DD4F_Group__By, _LEN_1A28DD4F ) },
		{ FALSE,		MX_M_VIEW_DIVISION_LINES,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_3618A863_Di_vision_Lines, _LEN_3618A863 ) },
		{ FALSE,		MX_M_VIEW_LEFT_AREA,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_A1177424_Le_ft_Area, _LEN_A1177424 ) },
		{ FALSE,		MX_M_VIEW_RIGHT_AREA,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_2D68134B__Right_Area, _LEN_2D68134B ) },
		{ FALSE,		MX_M_VIEW_HIGHLIGHTING,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_7BB34DB6__Highlighting, _LEN_7BB34DB6 ) },
		{ FALSE,		MX_M_VIEW_RULER,								FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_05A83FE3_R_uler, _LEN_05A83FE3 ) },
		{ FALSE,		MX_M_VIEW_MINI_MAP,								FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_8073DEAF_Mini_Ma_p, _LEN_8073DEAF ) },
		{ FALSE,		MX_M_VIEW_STATUS_BAR,							FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_0AF8B5FC__Status_Bar, _LEN_0AF8B5FC ) },
		{ FALSE,		MX_M_VIEW_ENDIAN,								FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_6D380769__Endian, _LEN_6D380769 ) },
	};

	// View -> Edit As menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miViewEditAsMenu[] = {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_M_VIEW_EDIT_AS_TEXT,							TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_325EFA5C__Text, _LEN_325EFA5C ) },
		{ FALSE,		MX_M_VIEW_EDIT_AS_HEX,							TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_9CC0956A__Hex, _LEN_9CC0956A ) },
		{ FALSE,		MX_M_VIEW_EDIT_AS_BINARY,						TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_AD13BBF2_B_inary, _LEN_AD13BBF2 ) },
		{ FALSE,		MX_M_VIEW_EDIT_AS_SCRIPT,						TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_9BEEF077__Script, _LEN_9BEEF077 ) },
		{ FALSE,		MX_M_VIEW_EDIT_AS_TEMPLATE,						TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_1CD64D3B__Template, _LEN_1CD64D3B ) },
		{ FALSE,		MX_M_VIEW_EDIT_AS_EBCDIC,						TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_48C55E0B_E_BCDIC, _LEN_48C55E0B ) },
		{ FALSE,		MX_M_VIEW_EDIT_AS_UNICODE,						TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_DB5BE63A__Unicode, _LEN_DB5BE63A ) },
		{ FALSE,		MX_M_VIEW_EDIT_AS_UTF8,							TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_4524B804_UTF__8, _LEN_4524B804 ) },
		{ FALSE,		MX_M_VIEW_EDIT_AS_PROCESS,						TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_8EF74ACC__Process, _LEN_8EF74ACC ) },
		{ FALSE,		MX_M_VIEW_EDIT_AS_CODE,							TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_7EC06D03__Code, _LEN_7EC06D03 ) },
		{ FALSE,		MX_M_VIEW_EDIT_AS_TAGGED,						TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_7EE33C3D_Ta_gged, _LEN_7EE33C3D ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_VIEW_EDIT_AS_TOGGLE,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_711E6EA5__Toggle_Edit_As_Hex, _LEN_711E6EA5 ) },
	};

	// View -> Font menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miViewFontMenu[] = {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_M_VIEW_FONT_CHANGE_FONT,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_E06803BF_Change__Font___, _LEN_E06803BF ) },
		{ FALSE,		MX_M_VIEW_FONT_LINE_SPACING,					FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_E2B38391_Set__Line_Spacing___, _LEN_E2B38391 ) },
		{ FALSE,		MX_M_VIEW_FONT_USE_DEFAULT_FONT,				TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_6FF83190__Use_Default_Hex_Editor_Font, _LEN_6FF83190 ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_VIEW_FONT_ENLARGE_FONT,					FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_5E30F3F6__Enlarge_Font, _LEN_5E30F3F6 ) },
		{ FALSE,		MX_M_VIEW_FONT_SHRINK_FONT,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_97DD88C0__Shrink_Font, _LEN_97DD88C0 ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_VIEW_FONT_RESET_FONT_SIZE,					FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_D138DECB_Reset_Font_Si_ze, _LEN_D138DECB ) },
		{ FALSE,		MX_M_VIEW_FONT_RESET_FONT,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_557A8BF4__Reset_Font, _LEN_557A8BF4 ) },
	};

	// View -> Character Set menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miViewCharSetMenu[] = {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_M_VIEW_CHAR_SET_ASCII,						TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_470FBFA1_ASCII_A_NSI, _LEN_470FBFA1 ) },
		{ FALSE,		MX_M_VIEW_CHAR_SET_UNICODE,						TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_DB5BE63A__Unicode, _LEN_DB5BE63A ) },
		{ FALSE,		MX_M_VIEW_CHAR_SET_UTF8,						TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_4524B804_UTF__8, _LEN_4524B804 ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_VIEW_CHAR_SET_CODEPAGE,					FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_C677D49F__Code_Page___, _LEN_C677D49F ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_VIEW_CHAR_SET_USE_DEFAULT_CHAR_SET,		FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_D138DECB_Reset_Font_Si_ze, _LEN_D138DECB ) },
	};

	// View -> Addresses menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miViewAddressesMenu[] = {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_M_VIEW_ADDRESSES_SHOW_LINE_NUMBERS_ADDRESSES,TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_AA1A566C_Show_Line_Numbers_Addr_esses, _LEN_AA1A566C ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_VIEW_ADDRESSES_DISPLAY_FORMATS,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_95DC5454__Display_Format, _LEN_95DC5454 ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_VIEW_ADDRESSES_SET_STARTING_ADDRESS,		FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_E47140EC_Set_St_arting_Address___, _LEN_E47140EC ) },
	};

	// View -> Addresses -> Display Format menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miViewAddressesDisplayMenu[] = {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_M_VIEW_ADDRESSES_DISPLAY_FORMATS_BYTE_HEX,	TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_F8B0A24D_Byte_Number___Hex_, _LEN_F8B0A24D ) },
		{ FALSE,		MX_M_VIEW_ADDRESSES_DISPLAY_FORMATS_BYTE_DEC,	TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_A88B8F34_Byte_Number___Decimal_, _LEN_A88B8F34 ) },
		{ FALSE,		MX_M_VIEW_ADDRESSES_DISPLAY_FORMATS_BYTE_OCT,	TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_113603DC_Byte_Number___Octal_, _LEN_113603DC ) },
		{ FALSE,		MX_M_VIEW_ADDRESSES_DISPLAY_FORMATS_LINE_DEC,	TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_FFAAE152__Line_Number__Decimal_, _LEN_FFAAE152 ) },
		{ FALSE,		MX_M_VIEW_ADDRESSES_DISPLAY_FORMATS_SHORT_HEX,	TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_18B5D2F2_S_hort_Number__Hex_, _LEN_18B5D2F2 ) },
		{ FALSE,		MX_M_VIEW_ADDRESSES_DISPLAY_FORMATS_SHORT_DEC,	TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_C1493B68_Sho_rt_Number__Decimal_, _LEN_C1493B68 ) },
	};

	// View -> Line Width menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miViewLineWidthMenu[] = {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_M_VIEW_LINE_WIDTH_4_BYTES,					TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_6A7D8A9E_Fixed__4_Bytes, _LEN_6A7D8A9E ) },
		{ FALSE,		MX_M_VIEW_LINE_WIDTH_8_BYTES,					TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_ADDDE0EA_Fixed__8_Bytes, _LEN_ADDDE0EA ) },
		{ FALSE,		MX_M_VIEW_LINE_WIDTH_12_BYTES,					TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_D4A90CCA_Fixed__12_Bytes, _LEN_D4A90CCA ) },
		{ FALSE,		MX_M_VIEW_LINE_WIDTH_16_BYTES,					TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_20E628D9_Fixed__16_Bytes, _LEN_20E628D9 ) },
		{ FALSE,		MX_M_VIEW_LINE_WIDTH_20_BYTES,					TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_CDB91A00_Fixed__20_Bytes, _LEN_CDB91A00 ) },
		{ FALSE,		MX_M_VIEW_LINE_WIDTH_40_BYTES,					TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_0BD61387_Fixed__40_Bytes, _LEN_0BD61387 ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_VIEW_LINE_WIDTH_AUTO,						TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_1753251A__Auto_Width, _LEN_1753251A ) },
		{ FALSE,		MX_M_VIEW_LINE_WIDTH_CUSTOM,					FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_D6D1EDA2__Custom_Width___, _LEN_D6D1EDA2 ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_VIEW_LINE_WIDTH_DEFAULT,					TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_5B534A5A_Use_Default_Line_Width_for_____Hex___, _LEN_5B534A5A ) },
	};

	// View -> Group By menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miViewGroupByMenu[] = {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_M_VIEW_GROUP_BY_BYTE,						TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_7DDCC05B__Byte__8_bits_, _LEN_7DDCC05B ) },
		{ FALSE,		MX_M_VIEW_GROUP_BY_SHORT,						TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_64F47C4E__Short__16_bits_, _LEN_64F47C4E ) },
		{ FALSE,		MX_M_VIEW_GROUP_BY_INT,							TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_A47F6744__Int__32_bits_, _LEN_A47F6744 ) },
		{ FALSE,		MX_M_VIEW_GROUP_BY_INT64,						TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_94E24188__Int64__64_bits_, _LEN_94E24188 ) },
		{ FALSE,		MX_M_VIEW_GROUP_BY_CUSTOM,						TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_932B6BAC__Custom___, _LEN_932B6BAC ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_VIEW_GROUP_BY_SWAP,						TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_F54BB4DD_S_wap_Little_Endian_Bytes_by_Group, _LEN_F54BB4DD ) },
	};

	// View -> Division Lines menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miViewDivisionLinesMenu[] = {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_M_VIEW_DIVISION_LINES_NONE,					TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_76455D54__None, _LEN_76455D54 ) },
		{ FALSE,		MX_M_VIEW_DIVISION_LINES_1_BYTE,				TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_2350C298__1_Byte, _LEN_2350C298 ) },
		{ FALSE,		MX_M_VIEW_DIVISION_LINES_2_BYTES,				TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_D4119E22__2_Bytes, _LEN_D4119E22 ) },
		{ FALSE,		MX_M_VIEW_DIVISION_LINES_4_BYTES,				TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_B7C1AB18__4_Bytes, _LEN_B7C1AB18 ) },
		{ FALSE,		MX_M_VIEW_DIVISION_LINES_8_BYTES,				TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_7061C16C__8_Bytes, _LEN_7061C16C ) },
		{ FALSE,		MX_M_VIEW_DIVISION_LINES_CUSTOM,				TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_932B6BAC__Custom___, _LEN_932B6BAC ) },
		{ FALSE,		MX_M_VIEW_DIVISION_LINES_SET_STARTING,			TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_3646976F_Set_Starting_Division__Offset___, _LEN_3646976F ) },
	};

	// View -> Left Area menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miViewLeftAreaMenu[] = {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_M_VIEW_LEFT_AREA_HEX,						TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_9CC0956A__Hex, _LEN_9CC0956A ) },
		{ FALSE,		MX_M_VIEW_LEFT_AREA_CHAR,						TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_852B0A04__Char, _LEN_852B0A04 ) },
		{ FALSE,		MX_M_VIEW_LEFT_AREA_OCTAL,						TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_F81D67E4__Octal, _LEN_F81D67E4 ) },
		{ FALSE,		MX_M_VIEW_LEFT_AREA_BINARY,						TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_418F9E48__Binary, _LEN_418F9E48 ) },
		{ FALSE,		MX_M_VIEW_LEFT_AREA_DECIMAL,					TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_5217E1C9__Decimal, _LEN_5217E1C9 ) },
	};

	// View -> Right Area menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miViewRightAreaMenu[] = {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_M_VIEW_RIGHT_AREA_HEX,						TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_9CC0956A__Hex, _LEN_9CC0956A ) },
		{ FALSE,		MX_M_VIEW_RIGHT_AREA_CHAR,						TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_852B0A04__Char, _LEN_852B0A04 ) },
		{ FALSE,		MX_M_VIEW_RIGHT_AREA_OCTAL,						TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_F81D67E4__Octal, _LEN_F81D67E4 ) },
		{ FALSE,		MX_M_VIEW_RIGHT_AREA_BINARY,					TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_418F9E48__Binary, _LEN_418F9E48 ) },
		{ FALSE,		MX_M_VIEW_RIGHT_AREA_DECIMAL,					TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_5217E1C9__Decimal, _LEN_5217E1C9 ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_VIEW_RIGHT_AREA_HIDE,						TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_AD4C96B0__Hide, _LEN_AD4C96B0 ) },
	};

	// View -> Highlighting menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miViewHighlightMenu[] = {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_M_VIEW_HIGHLIGHTING_LINEFEED_CHARS,			TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_128A6EB8__Linefeed_Characters, _LEN_128A6EB8 ) },
		{ FALSE,		MX_M_VIEW_HIGHLIGHTING_ALPHA_CHARS,				TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_105BA7AD__Alphanumeric_Characters, _LEN_105BA7AD ) },
		{ FALSE,		MX_M_VIEW_HIGHLIGHTING_CONTROL_CHARS,			TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_0E674527__Control_Characters, _LEN_0E674527 ) },
		{ FALSE,		MX_M_VIEW_HIGHLIGHTING_NON_ASCII,				TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_B7878680_N_on_ASCII_Characters, _LEN_B7878680 ) },
		{ FALSE,		MX_M_VIEW_HIGHLIGHTING_ZEROES,					TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_62707C03__Zeros, _LEN_62707C03 ) },
		{ FALSE,		MX_M_VIEW_HIGHLIGHTING_POINTERS,				TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_4887B201__Pointers, _LEN_4887B201 ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_VIEW_HIGHLIGHTING_AUTO_HIGHLIGHT_SEL,		TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_488682EC_Auto__Highlight_Selection, _LEN_488682EC ) },
		{ FALSE,		MX_M_VIEW_HIGHLIGHTING_AUTO_HIGHLIGHT_OPS,		TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_7D4510D2_Auto_Hightlight_Options, _LEN_7D4510D2 ) },
	};

	// View -> Ruler menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miViewRulerMenu[] = {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_M_VIEW_RULER_SHOW,							TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_8A9236B5_Show__Ruler, _LEN_8A9236B5 ) },
		{ FALSE,		MX_M_VIEW_RULER_SHOW_LABELS,					TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_38AD007B_Show__Labels, _LEN_38AD007B ) },
		{ FALSE,		MX_M_VIEW_RULER_SHOW_ARROWS,					TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_57CBE724_Show__Arrows, _LEN_57CBE724 ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_VIEW_RULER_DISPLAY_FMT,					FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_95DC5454__Display_Format, _LEN_95DC5454 ) },
	};

	// View -> Ruler -> Display Format menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miViewRulerFmtMenu[] = {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_M_VIEW_RULER_DISPLAY_FMT_HEX,				TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_9CC0956A__Hex, _LEN_9CC0956A ) },
		{ FALSE,		MX_M_VIEW_RULER_DISPLAY_FMT_DEC,				TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_5217E1C9__Decimal, _LEN_5217E1C9 ) },
	};

	// View -> Mini Map menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miViewMiniMapMenu[] = {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_M_VIEW_MINI_MAP_SHOW,						TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_D5B24A37__Show_Mini_Map, _LEN_D5B24A37 ) },
		{ FALSE,		MX_M_VIEW_MINI_MAP_OPTIONS,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_88AE254B_Mini_Map__Options___, _LEN_88AE254B ) },
	};

	// View -> Status Bar menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miViewStatusBarMenu[] = {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled
		{ FALSE,		MX_M_VIEW_STATUS_BAR_FILE_POS_FORMAT,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_12D71705_File__Position_Format, _LEN_12D71705 ) },
		{ FALSE,		MX_M_VIEW_STATUS_BAR_FILE_SIZE_FORMAT,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_0A21046B_File__Size_Format, _LEN_0A21046B ) },
		{ FALSE,		MX_M_VIEW_STATUS_BAR_SEL_SIZE_FORMAT,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_A0AF5D5A_S_election_Size_Format, _LEN_A0AF5D5A ) },
		{ FALSE,		MX_M_VIEW_STATUS_BAR_BYTE_VALUE_FORMAT,			FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_75C0E37F_Byte__Value_Format, _LEN_75C0E37F ) },
	};

	// View -> Status Bar -> File Position Format menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miViewStatusBarFilePosMenu[] = {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled
		{ FALSE,		MX_M_VIEW_STATUS_BAR_POS_FMT_BYTE_HEX,			TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_F8B0A24D_Byte_Number___Hex_, _LEN_F8B0A24D ) },
		{ FALSE,		MX_M_VIEW_STATUS_BAR_POS_FMT_BYTE_DEC,			TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_A88B8F34_Byte_Number___Decimal_, _LEN_A88B8F34 ) },
		{ FALSE,		MX_M_VIEW_STATUS_BAR_POS_FMT_BYTE_DEC_HEX,		TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_75D47148_Byte_Number__Decimal_He_x_, _LEN_75D47148 ) },
		{ FALSE,		MX_M_VIEW_STATUS_BAR_POS_FMT_BYTE_OCT,			TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_113603DC_Byte_Number___Octal_, _LEN_113603DC ) },
		{ FALSE,		MX_M_VIEW_STATUS_BAR_POS_FMT_LINE_DEC,			TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_FFAAE152__Line_Number__Decimal_, _LEN_FFAAE152 ) },
		{ FALSE,		MX_M_VIEW_STATUS_BAR_POS_FMT_SHORT_HEX,			TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_18B5D2F2_S_hort_Number__Hex_, _LEN_18B5D2F2 ) },
		{ FALSE,		MX_M_VIEW_STATUS_BAR_POS_FMT_SHORT_DEC,			TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_C1493B68_Sho_rt_Number__Decimal_, _LEN_C1493B68 ) },
	};

	// View -> Status Bar -> File Size Format menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miViewStatusBarFileSizeMenu[] = {
		
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled
		{ FALSE,		MX_M_VIEW_STATUS_BAR_FILE_SIZE_FMT_BYTE_HEX,	TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_F8B0A24D_Byte_Number___Hex_, _LEN_F8B0A24D ) },
		{ FALSE,		MX_M_VIEW_STATUS_BAR_FILE_SIZE_FMT_BYTE_DEC,	TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_A88B8F34_Byte_Number___Decimal_, _LEN_A88B8F34 ) },
		{ FALSE,		MX_M_VIEW_STATUS_BAR_FILE_SIZE_FMT_BYTE_DEC_HEX,TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_75D47148_Byte_Number__Decimal_He_x_, _LEN_75D47148 ) },
		{ FALSE,		MX_M_VIEW_STATUS_BAR_FILE_SIZE_FMT_BYTE_OCT,	TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_113603DC_Byte_Number___Octal_, _LEN_113603DC ) },
		{ FALSE,		MX_M_VIEW_STATUS_BAR_FILE_SIZE_FMT_LINE_DEC,	TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_FFAAE152__Line_Number__Decimal_, _LEN_FFAAE152 ) },
		{ FALSE,		MX_M_VIEW_STATUS_BAR_FILE_SIZE_FMT_SHORT_HEX,	TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_18B5D2F2_S_hort_Number__Hex_, _LEN_18B5D2F2 ) },
		{ FALSE,		MX_M_VIEW_STATUS_BAR_FILE_SIZE_FMT_SHORT_DEC,	TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_C1493B68_Sho_rt_Number__Decimal_, _LEN_C1493B68 ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_VIEW_STATUS_BAR_FILE_SIZE_FMT_COMMAS,		TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_97D96D25_Include_Com_mas, _LEN_97D96D25 ) },
	};

	// View -> Status Bar -> Selection Size Format menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miViewStatusBarSelSizeMenu[] = {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled
		{ FALSE,		MX_M_VIEW_STATUS_BAR_SEL_SIZE_FMT_BYTE_HEX,		TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_F8B0A24D_Byte_Number___Hex_, _LEN_F8B0A24D ) },
		{ FALSE,		MX_M_VIEW_STATUS_BAR_SEL_SIZE_FMT_BYTE_DEC,		TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_A88B8F34_Byte_Number___Decimal_, _LEN_A88B8F34 ) },
		{ FALSE,		MX_M_VIEW_STATUS_BAR_SEL_SIZE_FMT_BYTE_DEC_HEX,	TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_75D47148_Byte_Number__Decimal_He_x_, _LEN_75D47148 ) },
		{ FALSE,		MX_M_VIEW_STATUS_BAR_SEL_SIZE_FMT_BYTE_OCT,		TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_113603DC_Byte_Number___Octal_, _LEN_113603DC ) },
		{ FALSE,		MX_M_VIEW_STATUS_BAR_SEL_SIZE_FMT_LINE_DEC,		TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_FFAAE152__Line_Number__Decimal_, _LEN_FFAAE152 ) },
		{ FALSE,		MX_M_VIEW_STATUS_BAR_SEL_SIZE_FMT_SHORT_HEX,	TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_18B5D2F2_S_hort_Number__Hex_, _LEN_18B5D2F2 ) },
		{ FALSE,		MX_M_VIEW_STATUS_BAR_SEL_SIZE_FMT_SHORT_DEC,	TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_C1493B68_Sho_rt_Number__Decimal_, _LEN_C1493B68 ) },
	};

	// View -> Status Bar -> Byte Value Format menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miViewStatusBarByteValueMenu[] = {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled
		{ FALSE,		MX_M_VIEW_STATUS_BAR_BYTE_VAL_FMT_DEC,			TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_5217E1C9__Decimal, _LEN_5217E1C9 ) },
		{ FALSE,		MX_M_VIEW_STATUS_BAR_BYTE_VAL_FMT_HEX,			TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_9CC0956A__Hex, _LEN_9CC0956A ) },
		{ FALSE,		MX_M_VIEW_STATUS_BAR_BYTE_VAL_FMT_DEC_HEX,		TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_6CDE7BC0_De_cimal_Hex, _LEN_6CDE7BC0 ) },
		{ FALSE,		MX_M_VIEW_STATUS_BAR_BYTE_VAL_FMT_DEC_HEX_BIN,	TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_AF2D9A1F_Decimal_Hex__Binary, _LEN_AF2D9A1F ) },
		{ FALSE,		MX_M_VIEW_STATUS_BAR_BYTE_VAL_FMT_NONE,			TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_76455D54__None, _LEN_76455D54 ) },
	};

	// View -> Endian menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miViewEndianMenu[] = {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled
		{ FALSE,		MX_M_VIEW_ENDIAN_LE,							TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_89DF1A9E__Little_Endian__Intel_, _LEN_89DF1A9E ) },
		{ FALSE,		MX_M_VIEW_ENDIAN_BE,							TRUE,		FALSE,		TRUE,		MW_MENU_TXT( _T_2381E8C6__Big_Endian__Motorola_, _LEN_2381E8C6 ) },
		{ TRUE,			0,												FALSE,		FALSE,		TRUE,		nullptr },
		{ FALSE,		MX_M_VIEW_ENDIAN_TOGGLE,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_8CE449C7__Toggle_Endian, _LEN_8CE449C7 ) },
	};

	// Format menu.
	LSW_MENU_ITEM CDeusHexMachinaLayout::m_miFormatMenu[] = {
		//bIsSeperator	dwId											bCheckable	bChecked	bEnabled	
		{ FALSE,		MX_M_FIND_FIND,									FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_6E0D9949__Find___, _LEN_6E0D9949 ) },
	};

	// Menus.
	LSW_MENU_LAYOUT CDeusHexMachinaLayout::m_miMenus[] = {
		{
			MX_M_MENU_BAR,
			0,
			0,
			MX_ELEMENTS( m_miMenuBar ),
			m_miMenuBar
		},

		{
			MX_M_FILE,
			MX_M_MENU_BAR,
			MX_M_MENU_FILE,
			MX_ELEMENTS( m_miFileMenu ),
			m_miFileMenu
		},
		{
			MX_M_NEW,
			MX_M_FILE,
			MX_M_FILE_NEW,
			MX_ELEMENTS( m_miFileNewMenu ),
			m_miFileNewMenu
		},
		{
			MX_M_SPECIAL,
			MX_M_FILE,
			MX_M_FILE_SPECIAL,
			MX_ELEMENTS( m_miFileSpecialMenu ),
			m_miFileSpecialMenu
		},

		{
			MX_M_EDIT,
			MX_M_MENU_BAR,
			MX_M_MENU_EDIT,
			MX_ELEMENTS( m_miEditMenu ),
			m_miEditMenu
		},
		{
			MX_M_COPY_AS,
			MX_M_EDIT,
			MX_M_EDIT_COPY_AS_,
			MX_ELEMENTS( m_miEditCopyAsMenu ),
			m_miEditCopyAsMenu
		},
		{
			MX_M_INSERT_OVERWRITE,
			MX_M_EDIT,
			MX_M_EDIT_INSERT_OVERWRITE,
			MX_ELEMENTS( m_miEditInsertOverwriteMenu ),
			m_miEditInsertOverwriteMenu
		},

		{
			MX_M_SELECT,
			MX_M_MENU_BAR,
			MX_M_MENU_SELECT,
			MX_ELEMENTS( m_miSelectMenu ),
			m_miSelectMenu
		},

		{
			MX_M_FIND,
			MX_M_MENU_BAR,
			MX_M_MENU_FIND,
			MX_ELEMENTS( m_miFindMenu ),
			m_miFindMenu
		},

		{
			MX_M_VIEW,
			MX_M_MENU_BAR,
			MX_M_MENU_VIEW,
			MX_ELEMENTS( m_miViewMenu ),
			m_miViewMenu
		},
		{
			MX_M_EDIT_AS,
			MX_M_VIEW,
			MX_M_VIEW_EDIT_AS_,
			MX_ELEMENTS( m_miViewEditAsMenu ),
			m_miViewEditAsMenu
		},
		{
			MX_M_FONT,
			MX_M_VIEW,
			MX_M_VIEW_FONT,
			MX_ELEMENTS( m_miViewFontMenu ),
			m_miViewFontMenu
		},
		{
			MX_M_CHAR_SET,
			MX_M_VIEW,
			MX_M_VIEW_CHARACTER_SET,
			MX_ELEMENTS( m_miViewCharSetMenu ),
			m_miViewCharSetMenu
		},
		{
			MX_M_ADDRESSES,
			MX_M_VIEW,
			MX_M_VIEW_ADDRESSES,
			MX_ELEMENTS( m_miViewAddressesMenu ),
			m_miViewAddressesMenu
		},
		{
			MX_M_ADDRESSES_DISPLAY_FORMATS,
			MX_M_ADDRESSES,
			MX_M_VIEW_ADDRESSES_DISPLAY_FORMATS,
			MX_ELEMENTS( m_miViewAddressesDisplayMenu ),
			m_miViewAddressesDisplayMenu
		},
		{
			MX_M_LINE_WIDTH,
			MX_M_VIEW,
			MX_M_VIEW_LINE_WIDTH,
			MX_ELEMENTS( m_miViewLineWidthMenu ),
			m_miViewLineWidthMenu
		},
		{
			MX_M_GROUP_BY,
			MX_M_VIEW,
			MX_M_VIEW_GROUP_BY,
			MX_ELEMENTS( m_miViewGroupByMenu ),
			m_miViewGroupByMenu
		},
		{
			MX_M_DIVISION_LINES,
			MX_M_VIEW,
			MX_M_VIEW_DIVISION_LINES,
			MX_ELEMENTS( m_miViewDivisionLinesMenu ),
			m_miViewDivisionLinesMenu
		},
		{
			MX_M_LEFT_AREA,
			MX_M_VIEW,
			MX_M_VIEW_LEFT_AREA,
			MX_ELEMENTS( m_miViewLeftAreaMenu ),
			m_miViewLeftAreaMenu
		},
		{
			MX_M_RIGHT_AREA,
			MX_M_VIEW,
			MX_M_VIEW_RIGHT_AREA,
			MX_ELEMENTS( m_miViewRightAreaMenu ),
			m_miViewRightAreaMenu
		},
		{
			MX_M_HIGHLIGHTING,
			MX_M_VIEW,
			MX_M_VIEW_HIGHLIGHTING,
			MX_ELEMENTS( m_miViewHighlightMenu ),
			m_miViewHighlightMenu
		},
		{
			MX_M_RULER,
			MX_M_VIEW,
			MX_M_VIEW_RULER,
			MX_ELEMENTS( m_miViewRulerMenu ),
			m_miViewRulerMenu
		},
		{
			MX_M_RULER_FMT,
			MX_M_RULER,
			MX_M_VIEW_RULER_DISPLAY_FMT,
			MX_ELEMENTS( m_miViewRulerFmtMenu ),
			m_miViewRulerFmtMenu
		},
		{
			MX_M_MINI_MAP,
			MX_M_VIEW,
			MX_M_VIEW_MINI_MAP,
			MX_ELEMENTS( m_miViewMiniMapMenu ),
			m_miViewMiniMapMenu
		},
		{
			MX_M_STATUS_BAR,
			MX_M_VIEW,
			MX_M_VIEW_STATUS_BAR,
			MX_ELEMENTS( m_miViewStatusBarMenu ),
			m_miViewStatusBarMenu
		},
		{
			MX_M_STATUS_BAR_FILE_POS_FMT,
			MX_M_STATUS_BAR,
			MX_M_VIEW_STATUS_BAR_FILE_POS_FORMAT,
			MX_ELEMENTS( m_miViewStatusBarFilePosMenu ),
			m_miViewStatusBarFilePosMenu
		},
		{
			MX_M_STATUS_BAR_FILE_SIZE_FMT,
			MX_M_STATUS_BAR,
			MX_M_VIEW_STATUS_BAR_FILE_SIZE_FORMAT,
			MX_ELEMENTS( m_miViewStatusBarFileSizeMenu ),
			m_miViewStatusBarFileSizeMenu
		},
		{
			MX_M_STATUS_BAR_SEL_SIZE_FMT,
			MX_M_STATUS_BAR,
			MX_M_VIEW_STATUS_BAR_SEL_SIZE_FORMAT,
			MX_ELEMENTS( m_miViewStatusBarSelSizeMenu ),
			m_miViewStatusBarSelSizeMenu
		},
		{
			MX_M_STATUS_BAR_BYTE_VALUE_FMT,
			MX_M_STATUS_BAR,
			MX_M_VIEW_STATUS_BAR_BYTE_VALUE_FORMAT,
			MX_ELEMENTS( m_miViewStatusBarByteValueMenu ),
			m_miViewStatusBarByteValueMenu
		},
		{
			MX_M_ENDIAN,
			MX_M_VIEW,
			MX_M_VIEW_ENDIAN,
			MX_ELEMENTS( m_miViewEndianMenu ),
			m_miViewEndianMenu
		},
	};

	// == Functions.
	// Creates the PE Works window.
	CWidget * CDeusHexMachinaLayout::CreateDeusHexMachinaWindow( CWidget * _pwParent ) {
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		CLayoutManager::UnencryptLayouts( m_wlMainWindow, MX_ELEMENTS( m_wlMainWindow ),
			vLayouts,
			sStringsW,
			sStrings );

		std::vector<LSW_MENU_LAYOUT> vMenus;
		std::vector<std::vector<LSW_MENU_ITEM> *> vMenuItems;
		std::vector<std::vector<CSecureWString> *> vMenuStrings;
		CLayoutManager::UnencryptMenu( m_miMenus, MX_ELEMENTS( m_miMenus ),
			vMenus,
			vMenuItems,
			vMenuStrings );


		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		CDeusHexMachinaWindow::PrepareWindow();
		CWidget * pwThis = plmLayout->CreateWindowX( &vLayouts[0], MX_ELEMENTS( m_wlMainWindow ),
			vMenus.size() ? &vMenus[0] : nullptr, vMenus.size(),
			_pwParent );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		CLayoutManager::CleanEncryptedMenu( vMenuItems, vMenuStrings );

		return pwThis;
	}

}	// namespace mx
