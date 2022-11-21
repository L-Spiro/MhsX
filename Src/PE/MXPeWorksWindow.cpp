#include "MXPeWorksWindow.h"
#include "../CodePages/MXCodePages.h"
#include "../Layouts/MXLayoutMacros.h"
#include "../Layouts/MXLayoutManager.h"
#include "../Layouts/MXPeWorksLayout.h"
#include "../Strings/MXStringDecoder.h"
#include "../System/MXSystem.h"
#include "../Utilities/MXUtilities.h"
#include <Base/LSWBase.h>
#include <Base/LSWWndClassEx.h>
#include <Rebar/LSWRebar.h>
#include <ToolBar/LSWToolBar.h>
#include <TreeListView/LSWTreeListView.h>
#include <commdlg.h>
#include <strsafe.h>

#define MW_HEX_PRINT			BytesToHexWithSpaces

#define MX_PRINT_FILE_OFFSET( DST, OFF, TYPE, SECURESTRING )															\
	SECURESTRING.SecureClear();																							\
	std::swprintf( DST, L"%s (%I64u)", CUtilities::ToHex( OFF, SECURESTRING, 8 ), OFF );								\
	OFF += sizeof( TYPE )


#define MW_PE_FIELD				0
#define MW_PE_VALUE				1
#define MW_PE_OFFSET			2
#define MW_PE_BYTES				3
#define MW_PE_DESC				4

namespace mx {

	// == Members.
	// The main window class.
	ATOM CPeWorksWindow::m_aAtom = 0;

	CPeWorksWindow::CPeWorksWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) :
		lsw::CMainWindow( _wlLayout.ChangeClass( reinterpret_cast<LPCWSTR>(m_aAtom) ), _pwParent, _bCreateWidget, _hMenu, _ui64Data ) {
		//CUtilities::PrintTotalGuiObjects( GR_GDIOBJECTS );
		static const struct {
			LPCWSTR				lpwsImageName;
			DWORD				dwConst;
		} sImages[] = {
			{ L"52", MX_I_OPENFILE },
		};
		m_iImages.Create( 24, 24, ILC_COLOR32, MX_I_TOTAL, MX_I_TOTAL );

		for ( size_t I = 0; I < MX_I_TOTAL; ++I ) {
			CSecureWString wsTemp = CSystem::GetResourcesPathW();
			wsTemp += sImages[I].lpwsImageName;
			wsTemp += L".bmp";

			m_bBitmaps[sImages[I].dwConst].LoadFromFile( wsTemp.c_str(), 0, 0, LR_CREATEDIBSECTION );
			m_iImageMap[sImages[I].dwConst] = m_iImages.Add( m_bBitmaps[sImages[I].dwConst].Handle() );
		}
	}
	CPeWorksWindow::~CPeWorksWindow() {
		for ( auto I = m_vTabs.size(); I--; ) {
			delete m_vTabs[I].ppoPeObject;
			m_vTabs[I].ppoPeObject = nullptr;
			// Widget destroyed already.
		}
	}

	// == Functions.
	// Loads a file.
	bool CPeWorksWindow::LoadFile( const wchar_t * _pwcFile ) {
		CTab * ptTab = GetTab();
		if ( !ptTab ) { return false; }

		MX_TAB tTab;
		tTab.ppoPeObject = new( std::nothrow ) CPeObject();
		if ( !tTab.ppoPeObject ) { return false; }
		if ( !tTab.ppoPeObject->LoadImageFromFile( _pwcFile ) ) {
			return false;
		}
		// 
		CSecureString sLeftSizeExp;
		mx::CStringDecoder::Decode( _T_2762F803_____P__VCL, _LEN_2762F803, sLeftSizeExp );
		CSecureString sRightSizeExp;
		mx::CStringDecoder::Decode( _T_DD6DC560_____P__VCR, _LEN_DD6DC560, sRightSizeExp );
		CSecureString sTopSizeExp;
		mx::CStringDecoder::Decode( _T_340E6055_____P__VCT, _LEN_340E6055, sTopSizeExp );
		CSecureString sBottomSizeExp;
		mx::CStringDecoder::Decode( _T_C0DAD504_____P__VCB, _LEN_C0DAD504, sBottomSizeExp );
		LSW_WIDGET_LAYOUT wlLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager())->FixLayout( LSW_WIDGET_LAYOUT{
			LSW_LT_TREELISTVIEW,//LSW_LT_TREELIST,						// ltType
			static_cast<WORD>(CPeWorksLayout::MX_PEW_TAB_TREELIST_START + m_vTabs.size()),	// wId
			nullptr,								// lpwcClass
			TRUE,									// bEnabled
			TRUE,									// bActive
			0,										// iLeft
			20,										// iTop
			0,										// dwWidth
			0,										// dwHeight
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP,								// dwStyle
			0,																		// dwStyleEx
			nullptr,								// pwcText
			0,										// sTextLen
			static_cast<DWORD>(ptTab->Id()),		// dwParentId

//#if 0
			sLeftSizeExp.c_str(), 0,				// pcLeftSizeExp
			sRightSizeExp.c_str(), 0,				// pcRightSizeExp
			sTopSizeExp.c_str(), 0,					// pcTopSizeExp
			sBottomSizeExp.c_str(), 0,				// pcBottomSizeExp
			nullptr, 0,								// pcWidthSizeExp
			nullptr, 0,								// pcHeightSizeExp
//#endif
		} );
		tTab.ptWidget = static_cast<CTreeListView *>(static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager())->CreateWidget( wlLayout, ptTab, TRUE, NULL, 0 ));
		if ( !tTab.ptWidget ) {
			delete tTab.ppoPeObject;
			return false;
		}
		//std::vector<CWidget *> vNewTab;
		//CWidget::ControlSetup( tTab.ptWidget, vNewTab );
		tTab.ptWidget->InitControl( tTab.ptWidget->Wnd() );


		TCITEMW tie = { 0 };
		tie.mask = TCIF_TEXT;
		tie.pszText = const_cast<LPWSTR>(std::wcsrchr( _pwcFile, L'\\' ));
		if ( tie.pszText == NULL ) { tie.pszText = const_cast<LPWSTR>(_pwcFile); }
		else { tie.pszText++; }

		tTab.ptWidget->BeginLargeUpdate();
		if ( !FillTree( tTab ) ) {
			delete tTab.ppoPeObject;
			delete tTab.ptWidget;
			return false;
		}
		tTab.ptWidget->FinishUpdate();


		if ( ptTab->InsertItem( 0, &tie, tTab.ptWidget ) == -1 ) {
			delete tTab.ppoPeObject;
			delete tTab.ptWidget;
			return false;
		}
		
		m_vTabs.insert( m_vTabs.begin(), tTab );

		

		ForceSizeUpdate();

		return true;
	}

	// Gets the base tab control.
	CTab * CPeWorksWindow::GetTab() const {
		return const_cast<CTab *>(static_cast<const CTab *>(FindChild( CPeWorksLayout::MX_PEW_TABS )));
	}

	// WM_INITDIALOG.
	CWidget::LSW_HANDLED CPeWorksWindow::InitDialog() {
		CToolBar * plvToolBar = static_cast<CToolBar *>(FindChild( CPeWorksLayout::MX_PEW_TOOLBAR0 ));
		CRebar * plvRebar = static_cast<CRebar *>(FindChild( CPeWorksLayout::MX_PEW_REBAR0 ));

		plvToolBar->SetImageList( 0, m_iImages );
		//#define MX_TOOL_STR( TXT )					reinterpret_cast<INT_PTR>(TXT)
#define MX_TOOL_STR( TXT )						0
		const TBBUTTON bButtons[] = {
			// iBitmap							idCommand									fsState				fsStyle			bReserved	dwData	iString
			{ m_iImageMap[MX_I_OPENFILE],		CPeWorksLayout::MX_PEW_OPENPE,				TBSTATE_ENABLED,	BTNS_AUTOSIZE,	{ 0 },		0,		MX_TOOL_STR( L"Open File" ) },
		};
#undef MX_TOOL_STR

		plvToolBar->AddButtons( bButtons, MX_ELEMENTS( bButtons ) );

		plvRebar->SetImageList( m_iImages );
		{
			LSW_REBARBANDINFO riRebarInfo;
			riRebarInfo.SetColors( ::GetSysColor( COLOR_BTNTEXT ), ::GetSysColor( COLOR_BTNFACE ) );
			riRebarInfo.SetStyle( RBBS_CHILDEDGE |
			  RBBS_FIXEDBMP );
			riRebarInfo.SetChild( plvToolBar->Wnd() );
			riRebarInfo.SetChildSize( plvToolBar->GetMinBoundingRect().Width(), plvToolBar->GetMinBoundingRect().Height() );
			riRebarInfo.SetId( CPeWorksLayout::MX_PEW_TOOLBAR0 );
			plvRebar->InsertBand( -1, riRebarInfo );
		}

	   LSW_RECT rRebarRect = ClientRect( this );
	   ::MoveWindow( plvRebar->Wnd(), 0, 0, rRebarRect.Width(), plvRebar->WindowRect( this ).Height(), FALSE );

		plvRebar->UpdateRects();



		// ==== STATUS BAR ==== //
		CStatusBar * psbStatus = StatusBar();
		if ( psbStatus ) {
			const CStatusBar::LSW_STATUS_PART spParts[] = {
				// Last status message.
				//{ 450, TRUE },
				// Current process ID.
				//{ 450 + 48, TRUE },
				{ rRebarRect.Width() - psbStatus->ClientRect( this ).Height() - 32, TRUE },

				{ rRebarRect.Width() - psbStatus->ClientRect( this ).Height(), TRUE },
			};
			psbStatus->SetParts( spParts, MX_ELEMENTS( spParts ) );
		}

		ForceSizeUpdate();

		return LSW_H_CONTINUE;
	}

	// WM_COMMAND from control.
	CWidget::LSW_HANDLED CPeWorksWindow::Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc ) {
		switch ( _Id ) {
			case CPeWorksLayout::MX_PEW_OPENPE : {
				OPENFILENAMEW ofnOpenFile = { sizeof( ofnOpenFile ) };
				CSecureWString szFileName;
				szFileName.resize( 0xFFFF + 2 );
				CSecureWString wsFilter = _DEC_WS_2A6266F4_PE_Files____dll____exe__0__dll___exe_0All_Files_______0____0_0;

				ofnOpenFile.hwndOwner = Wnd();
				ofnOpenFile.lpstrFilter = wsFilter.c_str();
				ofnOpenFile.lpstrFile = &szFileName[0];
				ofnOpenFile.nMaxFile = DWORD( szFileName.size() );
				ofnOpenFile.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
				//ofnOpenFile.lpstrDefExt = L"";

				if ( ::GetOpenFileNameW( &ofnOpenFile ) ) {
					if ( !LoadFile( ofnOpenFile.lpstrFile ) ) {
					}
				}
				break;
			}
			case CPeWorksLayout::MX_PEW_COPY_FIELD : {
				CopyField( false );
				break;
			}
			case CPeWorksLayout::MX_PEW_COPY_VALUE : {
				CopyValue( false );
				break;
			}
			case CPeWorksLayout::MX_PEW_COPY_OFFSET : {
				CopyOffset( false );
				break;
			}
			case CPeWorksLayout::MX_PEW_COPY_BYTES : {
				CopyBytes( false );
				break;
			}
			case CPeWorksLayout::MX_PEW_COPY_DESC : {
				CopyDesc( false );
				break;
			}
			case CPeWorksLayout::MX_PEW_COPY_ALL : {
				CopyAll( false );
				break;
			}
			case CPeWorksLayout::MX_PEW_COPY_FIELD_VALUE : {
				CopyFieldValue( false );
				break;
			}

			case CPeWorksLayout::MX_PEW_COPY_EXPAND_SELECTED : {
				CurTabExpandSelected();
				break;
			}
			case CPeWorksLayout::MX_PEW_COPY_EXPAND_ALL : {
				CurTabExpandAll();
				break;
			}
			case CPeWorksLayout::MX_PEW_COPY_COLLAPSE_SELECTED : {
				CurTabCollapseSelected();
				break;
			}
			case CPeWorksLayout::MX_PEW_COPY_COLLAPSE_ALL : {
				CurTabCollapseAll();
				break;
			}
		}
		return LSW_H_CONTINUE;
	}

	// WM_CLOSE.
	CWidget::LSW_HANDLED CPeWorksWindow::Close() {
		
		//::EndDialog( Wnd(), 0 );
		::DestroyWindow( Wnd() );
		//CUtilities::PrintTotalGuiObjects( GR_GDIOBJECTS );
		return LSW_H_HANDLED;
	}

	// WM_ERASEBKGND.
	CWidget::LSW_HANDLED CPeWorksWindow::EraseBkgnd( HDC _hDc ) {
		return LSW_H_HANDLED;
	}

	// Gets the status bar.
	CStatusBar * CPeWorksWindow::StatusBar() {
		return static_cast<CStatusBar *>(FindChild( CPeWorksLayout::MX_PEW_STATUSBAR ));
	}

	// Gets the status bar.
	const CStatusBar * CPeWorksWindow::StatusBar() const {
		return static_cast<const CStatusBar *>(FindChild( CPeWorksLayout::MX_PEW_STATUSBAR ));
	}

	// Virtual client rectangle.  Can be used for things that need to be adjusted based on whether or not status bars, toolbars, etc. are present.
	const LSW_RECT CPeWorksWindow::VirtualClientRect( const CWidget * pwChild ) const {
		LSW_RECT rTemp = ClientRect( this );
		const CRebar * plvRebar = static_cast<const CRebar *>(FindChild( CPeWorksLayout::MX_PEW_REBAR0 ));
		if ( plvRebar ) {
			LSW_RECT rRebar = plvRebar->ClientRect( this );
			rTemp.top += rRebar.Height();
		}

		const CStatusBar * psbStatus = StatusBar();
		if ( psbStatus ) {
			LSW_RECT rStatus = psbStatus->ClientRect( this );
			rTemp.bottom -= rStatus.Height();
		}
		return rTemp;
	}

	// Prepares to create the window.  Creates the atom if necessary.
	void CPeWorksWindow::PreparePeWorks() {
		if ( !m_aAtom ) {
			lsw::CWndClassEx wceEx;
			wceEx.SetInstance( lsw::CBase::GetThisHandle() );
			WCHAR szStr[23];
			mx::CUtilities::RandomString( szStr, MX_ELEMENTS( szStr ) );
			wceEx.SetClassName( szStr );
			wceEx.SetBackgroundBrush( reinterpret_cast<HBRUSH>(CTLCOLOR_DLG + 1) );
			wceEx.SetWindPro( CWidget::WindowProc );
			m_aAtom = lsw::CBase::RegisterClassExW( wceEx.Obj() );
		}
	}

	// Closes down the PE Works library.
	void CPeWorksWindow::ShutDown() {
		// Since it was registered with lsw::CBase::RegisterClassExW(), it will
		//	be unregistered automatically.
		m_aAtom = 0;
	}

	// Fills a tree-list with the associated PE object's information.
	bool CPeWorksWindow::FillTree( MX_TAB &_tTab ) {
		lsw::CTreeListView * ptlTree = static_cast<lsw::CTreeListView *>(_tTab.ptWidget);
		// Create the header items.
		if ( !ptlTree->SetColumnText( mx::CStringDecoder::DecodeToWString( _T_LEN_D902CC4C_Field_Name ).c_str(), 0 ) ) { return false; }
		if ( !ptlTree->SetColumnWidth( 0, 230 ) ) { return false; }
		if ( !ptlTree->InsertColumn( mx::CStringDecoder::DecodeToWString( _T_LEN_DCB67730_Value ).c_str(), 450, -1 ) ) { return false; }
		if ( !ptlTree->InsertColumn( mx::CStringDecoder::DecodeToWString( _T_LEN_5EA6CFE6_Offset ).c_str(), 120, -1 ) ) { return false; }
		if ( !ptlTree->InsertColumn( mx::CStringDecoder::DecodeToWString( _T_LEN_3EF5AD3F_Raw_Bytes ).c_str(), 120, -1 ) ) { return false; }
		if ( !ptlTree->InsertColumn( mx::CStringDecoder::DecodeToWString( _T_LEN_EB78CFF1_Description ).c_str(), 850, -1 ) ) { return false; }

		//lValues << "MS-DOS Header" << "" << "" << "" << "The MS-DOS header.";
		//_T_LEN_A94A579A_MS_DOS_Header
		
		CSecureWString swsTemp;
		swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_A94A579A_MS_DOS_Header );
		TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItem( swsTemp.c_str() );
		HTREEITEM hItem = ptlTree->InsertItem( &isInsertMe );
		if ( !ptlTree->SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_A614D0B8_The_MS_DOS_header_ ).c_str(), MW_PE_DESC ) ) { return false; }
		if ( !AddDosHeader( (*ptlTree), hItem, _tTab.ppoPeObject->DosHeader(), _tTab.ppoPeObject->DosHeaderOffset() ) ) { return false; }

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_4AD9AEB4_DOS_Stub );
			isInsertMe = lsw::CTreeListView::DefaultItem( swsTemp.c_str() );
			hItem = ptlTree->InsertItem( &isInsertMe );
			if ( !ptlTree->SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_639D4FEA_The_DOS_stub_ ).c_str(), MW_PE_DESC ) ) { return false; }
			if ( !AddDosStub( (*ptlTree), hItem, _tTab.ppoPeObject->DosStub(), _tTab.ppoPeObject->DosStubOffset() ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_1E471600_COFF_Header );
			isInsertMe = lsw::CTreeListView::DefaultItem( swsTemp.c_str() );
			hItem = ptlTree->InsertItem( &isInsertMe );
			if ( !ptlTree->SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_10A8FDAA_The_Portable_Executable_signature_followed_by_the_COFF_header_ ).c_str(), MW_PE_DESC ) ) { return false; }
			if ( !AddCoffHeader( (*ptlTree), hItem, _tTab.ppoPeObject->CoffHeader(), _tTab.ppoPeObject->CoffHeaderOffset() ) ) { return false; }
		}

		{
			hItem = NULL;
			if ( _tTab.ppoPeObject->Is64() ) {
				
				{
					swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_E4E1FB09_PE_64_Header );
					isInsertMe = lsw::CTreeListView::DefaultItem( swsTemp.c_str() );
					hItem = ptlTree->InsertItem( &isInsertMe );
					if ( !ptlTree->SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_7794FFC7_The_64_bit_PE_header_ ).c_str(), MW_PE_DESC ) ) { return false; }

					if ( !AddOptHeader( (*ptlTree), hItem, _tTab.ppoPeObject->WinHeader(), (*_tTab.ppoPeObject), _tTab.ppoPeObject->ImageBase(), _tTab.ppoPeObject->WinHeaderOffset() ) ) { return false; }
					if ( !AddPe64Header( (*ptlTree), hItem, _tTab.ppoPeObject->Win64Header(), _tTab.ppoPeObject->WinXHeaderOffset() ) ) { return false; }
				}
			}
			else {
				{
					swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_6819B7C1_PE_32_Header );
					isInsertMe = lsw::CTreeListView::DefaultItem( swsTemp.c_str() );
					hItem = ptlTree->InsertItem( &isInsertMe );
					if ( !ptlTree->SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_D0AB7AAF_The_32_bit_PE_header_ ).c_str(), MW_PE_DESC ) ) { return false; }

					if ( !AddOptHeader( (*ptlTree), hItem, _tTab.ppoPeObject->WinHeader(), (*_tTab.ppoPeObject), _tTab.ppoPeObject->ImageBase(), _tTab.ppoPeObject->WinHeaderOffset() ) ) { return false; }
					if ( !AddPe32Header( (*ptlTree), hItem, _tTab.ppoPeObject->Win32Header(), _tTab.ppoPeObject->WinXHeaderOffset() ) ) { return false; }
				}
			}
			if ( !AddDataDirectories( (*ptlTree), hItem, _tTab.ppoPeObject->DataDirectory(), (*_tTab.ppoPeObject), _tTab.ppoPeObject->ImageBase(), _tTab.ppoPeObject->DataDirsOffset() ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_174C06B4_Section_Headers );
			swsTemp += L" (" + std::to_wstring( _tTab.ppoPeObject->Sections().size() ) + L")";
			isInsertMe = lsw::CTreeListView::DefaultItem( swsTemp.c_str() );
			hItem = ptlTree->InsertItem( &isInsertMe );
			if ( !ptlTree->SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_3BB9BB87_Section_headers_ ).c_str(), MW_PE_DESC ) ) { return false; }
			if ( !AddSectionHeaders( (*ptlTree), hItem, _tTab.ppoPeObject->Sections(), _tTab.ppoPeObject->ImageBase(), _tTab.ppoPeObject->SectionsOffset() ) ) { return false; }
		}

		if ( !AddExports( (*ptlTree), TVI_ROOT, (*_tTab.ppoPeObject) ) ) { return false; }

		if ( _tTab.ppoPeObject->HasImportDesc() ) {
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_CAD13458_Import_Descriptions );
			swsTemp += L" (" + std::to_wstring( _tTab.ppoPeObject->ImportDescriptor().size() ) + L")";
			isInsertMe = lsw::CTreeListView::DefaultItem( swsTemp.c_str() );
			hItem = ptlTree->InsertItem( &isInsertMe );
			if ( !ptlTree->SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_09C3C10A_Import_descriptors_ ).c_str(), MW_PE_DESC ) ) { return false; }
			uint32_t ui32Offset = _tTab.ppoPeObject->ImportDescriptorOffset();
			for ( size_t I = 0; I < _tTab.ppoPeObject->ImportDescriptor().size(); ++I ) {
				CHAR szTemp[_LEN_8FD2FEFE+16+1];
				std::sprintf( szTemp, mx::CStringDecoder::DecodeToString( _T_LEN_8FD2FEFE_IMAGE_IMPORT_DESCRIPTOR__u_ ).c_str(), static_cast<uint32_t>(I) );
				swsTemp = ee::CExpEval::StringToWString( szTemp );
				isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), ui32Offset, hItem );
				HTREEITEM hThisItem = ptlTree->InsertItem( &isInsertMe );
				CSecureString sDllName;
				_tTab.ppoPeObject->ImportDllNameByIndex( I, sDllName );
				if ( !ptlTree->SetItemText( hThisItem, ee::CExpEval::StringToWString( sDllName ).c_str(), MW_PE_VALUE ) ) { return false; }
				if ( !AddImportDesc( (*ptlTree), hThisItem, (*_tTab.ppoPeObject->ImportDescriptor()[I]), (*_tTab.ppoPeObject), ui32Offset + sizeof( MX_IMAGE_IMPORT_DESCRIPTOR ) * I ) ) { return false; }
			}
		}

		if ( _tTab.ppoPeObject->HasResourceDesc() && _tTab.ppoPeObject->ResourceDescriptor() ) {
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_A3E1BF2D_Resource_Description );
			isInsertMe = lsw::CTreeListView::DefaultItem( swsTemp.c_str() );
			hItem = ptlTree->InsertItem( &isInsertMe );
			if ( !ptlTree->SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_A84061C4_Resource_descriptor_ ).c_str(), MW_PE_DESC ) ) { return false; }
			if ( !AddResourceDesc( (*ptlTree), hItem, (*_tTab.ppoPeObject->ResourceDescriptor()), (*_tTab.ppoPeObject), _tTab.ppoPeObject->ResourceDescriptorOffset() ) ) { return false; }
		}

		if ( _tTab.ppoPeObject->HasRelocDesc() ) {
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_C5B0D729_Base_Relocations );
			swsTemp += L" (" + std::to_wstring( _tTab.ppoPeObject->BaseRelocs().size() ) + L")";
			isInsertMe = lsw::CTreeListView::DefaultItem( swsTemp.c_str() );
			hItem = ptlTree->InsertItem( &isInsertMe );
			if ( !ptlTree->SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_54921101_Base_relocations_ ).c_str(), MW_PE_DESC ) ) { return false; }
			if ( !AddReloc( (*ptlTree), hItem, _tTab.ppoPeObject->BaseRelocs(), (*_tTab.ppoPeObject) ) ) { return false; }
		}

		if ( !AddExportedFunctions( (*ptlTree), TVI_ROOT, (*_tTab.ppoPeObject) ) ) { return false; }
		if ( !AddImportedFuncs( (*ptlTree), TVI_ROOT, (*_tTab.ppoPeObject) ) ) { return false; }

		if ( _tTab.ppoPeObject->HasResourceDesc() && _tTab.ppoPeObject->ResourceDescriptor() ) {
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_6D97690D_Resources );
			isInsertMe = lsw::CTreeListView::DefaultItem( swsTemp.c_str() );
			hItem = ptlTree->InsertItem( &isInsertMe );
			if ( !ptlTree->SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_70080996_Resources_ ).c_str(), MW_PE_DESC ) ) { return false; }
			if ( !AddResourceTree( (*ptlTree), hItem, (*_tTab.ppoPeObject) ) ) { return false; }
		}


		std::vector<CPeObject::MX_EXTRACTED_RESOURCE> vResources;
		if ( _tTab.ppoPeObject->GetAllResources( vResources ) ) {
			// Failure is okay here.
			for ( size_t I = 0; I < vResources.size(); ++I ) {
				if ( vResources[I].vDirEntries.size() > 0 ) {
					switch ( vResources[I].vDirEntries[0].NameId ) {
						case static_cast<uint32_t>(reinterpret_cast<UINT_PTR>(RT_VERSION)) : {
							DecodeVersion( (*ptlTree), TVI_ROOT, vResources[I], (*_tTab.ppoPeObject) );
#if 0
							// Print version information.
							struct MX_LANG_AND_CODEPAGE {
								WORD wLanguage;
								WORD wCodePage;
							} * placTranslate;
							if ( vResources[I].vResourceData.size() ) {
								std::vector<WCHAR> vStrPrint;
								try {
									vStrPrint.resize( vResources[I].vResourceData.size() + 1 );
								}
								catch ( const std::bad_alloc /*& _eE*/ ) { continue; }
								UINT uiTranLen = 0;
								if ( ::VerQueryValueW( &vResources[I].vResourceData[0], 
									mx::CStringDecoder::DecodeToWString( _T_LEN_0B68E21B___VarFileInfo__Translation ).c_str(),
									reinterpret_cast<LPVOID *>(&placTranslate),
									&uiTranLen ) ) {

									for ( size_t J = 0; J < (uiTranLen / sizeof( MX_LANG_AND_CODEPAGE )); J++ ) {
										HRESULT hrRes = ::StringCchPrintfW( &vStrPrint[0], vStrPrint.size(),
											L"\\StringFileInfo\\%04x%04x\\FileDescription",
											placTranslate[J].wLanguage,
											placTranslate[J].wCodePage );
										if ( SUCCEEDED( hrRes ) ) {
											UINT uiFinalLen = 0;
											wchar_t * pwcFileDescription;
											if ( ::VerQueryValueW( &vResources[I].vResourceData[0],
												&vStrPrint[0],
												reinterpret_cast<LPVOID *>(&pwcFileDescription),
												&uiFinalLen ) ) {
												int iscdjhc = 0;
											}
										}
										hrRes = ::StringCchPrintfW( &vStrPrint[0], vStrPrint.size(),
											L"\\StringFileInfo\\%04x%04x\\CompanyName",
											placTranslate[J].wLanguage,
											placTranslate[J].wCodePage );
										if ( SUCCEEDED( hrRes ) ) {
											UINT uiFinalLen = 0;
											wchar_t * pwcFileDescription;
											if ( ::VerQueryValueW( &vResources[I].vResourceData[0],
												&vStrPrint[0],
												reinterpret_cast<LPVOID *>(&pwcFileDescription),
												&uiFinalLen ) ) {
												int iscdjhc = 0;
											}
										}
										hrRes = ::StringCchPrintfW( &vStrPrint[0], vStrPrint.size(),
											L"\\StringFileInfo\\%04x%04x\\FileVersion",
											placTranslate[J].wLanguage,
											placTranslate[J].wCodePage );
										if ( SUCCEEDED( hrRes ) ) {
											UINT uiFinalLen = 0;
											wchar_t * pwcFileDescription;
											if ( ::VerQueryValueW( &vResources[I].vResourceData[0],
												&vStrPrint[0],
												reinterpret_cast<LPVOID *>(&pwcFileDescription),
												&uiFinalLen ) ) {
												int iscdjhc = 0;
											}
										}
									}
								}
							}

#endif
							break;
						}
						case static_cast<uint32_t>(reinterpret_cast<UINT_PTR>(RT_MANIFEST)) : {
							DecodeManifest( (*ptlTree), TVI_ROOT, vResources[I], (*_tTab.ppoPeObject) );
							break;
						}
					}
				}
			}
		}


		return true;
	}

	// Decodes a DOS header to a given parent.
	bool CPeWorksWindow::AddDosHeader( lsw::CTreeListView &_tlTree, HTREEITEM _tiParent, const MX_DOS_HEADER &_dhHeader, uint64_t _uiStructOffset ) {
#define MW_STANDARD_DATA_PRINT( STRUCTMEMBER )																			\
	MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _dhHeader.STRUCTMEMBER, swsTemp );									\
	if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }										\
	::wsprintf( wValue, L"%u", _dhHeader.STRUCTMEMBER );																\
	if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_VALUE ) ) { return false; }											\
	ssRaw.SecureClear();																								\
	CUtilities::MW_HEX_PRINT( &_dhHeader.STRUCTMEMBER, sizeof( _dhHeader.STRUCTMEMBER ), ssRaw );						\
	if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }

		CSecureWString swsTemp;
		CSecureString ssCString;
		CSecureString ssRaw;
		WCHAR wValue[64];

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_2C7983E2_Signature );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_8758FB73_Signature_MZ__Mark_Zbikowski__ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _dhHeader.Signature, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			CUtilities::BytesToCString( &_dhHeader.Signature, sizeof( _dhHeader.Signature ), ssCString );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			CUtilities::BytesToHexWithSpaces( &_dhHeader.Signature, sizeof( _dhHeader.Signature ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_0987DADD_BytesInLastBlock );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_C0CA2E6A_The_number_of_bytes_in_the_last_block_of_the_program_that_are_actually_used__If_this_value_is_zero__then_the_entire_last_block_is_used__effectively_512__ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( BytesInLastBlock );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_EBFCAB82_BlocksInFile );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_05C91C32_Number_of_blocks_in_the_file_that_are_part_of_the_EXE_file__If_BytesInLastBlock_is_non_zero__only_that_much_of_the_last_block_is_used_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( BlocksInFile );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_5BBBF36F_NumRelocs );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_92C687C0_Number_of_relocation_entries_stored_after_the_header__May_be_zero_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( NumRelocs );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_1D508074_HeaderParagraphs );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_78057912_Number_of_paragraphs_in_the_header_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( HeaderParagraphs );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_3299877C_MinExtraParagraphs );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_49453E11_Number_of_paragraphs_of_additional_memory_that_the_program_will_need__This_is_the_equivalent_of_the_BSS_size_in_a_Unix_program_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( MinExtraParagraphs );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_613F9F98_MaxExtraParagraphs );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_B4D1192B_Maximum_number_of_paragraphs_of_additional_memory_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( MaxExtraParagraphs );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_E2D112A7_StackSegment );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_38D3097D_Relative_value_of_the_stack_segment__This_value_is_added_to_the_segment_at_which_the_program_was_loaded__and_the_result_is_used_to_initialize_the_SS_register_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( Ss );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_BB244D9C_SP );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_767AAF96_Initial_value_of_the_SP_register_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( Sp );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_2719BDCC_Checksum );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_CD12497E_Word_checksum__If_set_properly__the_16_bit_sum_of_all_words_in_the_file_should_be_zero__Usually_this_is_not_set_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( Checksum );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_0B09B747_IP );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_9055AA0B_Initial_value_of_the_IP_register_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( Ip );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_68EF0E77_CS );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_5306E6C7_Initial_value_of_the_CS_register__relative_to_the_segment_where_the_program_was_loaded_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( Cs );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_72D9B57B_OverlayNumber );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_1B543F7D_Overlay_number__A_value_of_0_implies_this_is_the_main_program_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( OverlayNumber );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_CA491992_Padding );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_101F5933_Padding_ ).c_str(), MW_PE_DESC );

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _dhHeader.Padding, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			ssCString.SecureClear();
			CUtilities::BytesToCString( &_dhHeader.Padding, sizeof( _dhHeader.Padding ), ssCString );
			_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE );
			ssRaw.SecureClear();
			CUtilities::BytesToHexWithSpaces( &_dhHeader.Padding, sizeof( _dhHeader.Padding ), ssRaw );
			_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_06EE21A8_PeOffset );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_AA2D1287_The_offset_from_the_start_of_the_file_to_the_relocation_pointer_table_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( PeOffset );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_3A62AF8F_OverlayNum );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_A20CDC05_Overlays_are_sections_of_a_program_that_remain_on_disk_until_the_program_actually_requires_them_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( OverlayNum );
		}
		
		return true;
#undef MW_STANDARD_DATA_PRINT
	}

	// Decodes a DOS stub to a given parent.
	bool CPeWorksWindow::AddDosStub( lsw::CTreeListView &_tlTree, HTREEITEM _tiParent, const std::vector<uint8_t> &_vStub, uint64_t _uiStructOffset ) {
		CSecureWString swsTemp;
		CSecureString ssCString;
		CSecureString ssRaw;
		WCHAR wValue[64];

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_5AF0321B_Stub );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_0353637D_DOS_stub_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _vStub, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			CUtilities::BytesToCString( &_vStub[0], _vStub.size(), ssCString );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			CUtilities::BytesToHexWithSpaces( &_vStub[0], _vStub.size(), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		return true;
	}

	// Decodes a COFF header to a given parent.
	bool CPeWorksWindow::AddCoffHeader( lsw::CTreeListView &_tlTree, HTREEITEM _tiParent, const MX_COFF_HEADER &_chHeader, uint64_t _uiStructOffset ) {
#define MW_STANDARD_DATA_PRINT( STRUCTMEMBER )																			\
	MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _chHeader.STRUCTMEMBER, swsTemp );									\
	if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }										\
	::wsprintf( wValue, L"%u", _chHeader.STRUCTMEMBER );																\
	if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_VALUE ) ) { return false; }											\
	ssRaw.SecureClear();																								\
	CUtilities::MW_HEX_PRINT( &_chHeader.STRUCTMEMBER, sizeof( _chHeader.STRUCTMEMBER ), ssRaw );						\
	if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }

		CSecureWString swsTemp;
		CSecureString ssCString;
		CSecureString ssRaw;
		WCHAR wValue[64];

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_EE8734B0_PeSignature );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_2653BF59_The_PE_header_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _chHeader.PeSignature, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			CUtilities::BytesToCString( &_chHeader.PeSignature, sizeof( _chHeader.PeSignature ), ssCString );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			CUtilities::BytesToHexWithSpaces( &_chHeader.PeSignature, sizeof( _chHeader.PeSignature ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_DAB8E618_Machine );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_6894FFB8_Determines_for_what_machine_the_file_was_compiled_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _chHeader.Machine, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			CHAR szTemp[64];
			CUtilities::MachineTypeToString( _chHeader.Machine, szTemp );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( szTemp ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_chHeader.Machine, sizeof( _chHeader.Machine ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}
		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_63A115CA_NumberOfSections );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_1E47759C_Number_of_sections_that_are_described_at_the_end_of_the_PE_headers_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( NumberOfSections );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_52BE8619_TimeDateStamp );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_EE98A75B_32_bit_time_at_which_this_header_was_generated__DD_MM_YYYY_HH_MM_SS__ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _chHeader.TimeDateStamp, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			ssCString.SecureClear();
			CUtilities::CreateDateString( _chHeader.TimeDateStamp, ssCString );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_chHeader.TimeDateStamp, sizeof( _chHeader.TimeDateStamp ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_1A0B4ED9_PointerToSymbolTable );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_A5FA5197_The_offset_of_the_symbol_table__in_bytes__or_zero_if_no_COFF_symbol_table_exists_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( PointerToSymbolTable );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_5AE35422_NumberOfSymbols );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_7C868567_The_number_of_symbols_in_the_symbol_table_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( NumberOfSymbols );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_C171929F_SizeOfOptionalHeader );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_1E295CF8_The_size_of_the_optional_header__in_bytes_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( SizeOfOptionalHeader );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_F8F28FC8_Characteristics );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_96C80FB7_The_characteristics_of_the_image_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _chHeader.Characteristics, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			CHAR szTemp[48];
			std::sprintf( szTemp, " (%u, %s)", _chHeader.Characteristics, CUtilities::ToHex( _chHeader.Characteristics, 4 ) );

			ssCString.SecureClear();
			CUtilities::PeCharacteristicsToString( _chHeader.Characteristics, ssCString );
			ssCString.append( szTemp );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_chHeader.Characteristics, sizeof( _chHeader.Characteristics ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		return true;
#undef MW_STANDARD_DATA_PRINT
	}

	// Decodes a PE header to a given parent.
	bool CPeWorksWindow::AddOptHeader( lsw::CTreeListView &_tlTree, HTREEITEM _tiParent, const MX_COFF_WINDOW_OPT &_cwoHeader, const mx::CPeObject &_poPeObject, uint64_t _uiImageBase, uint64_t _uiStructOffset ) {
#define MW_STANDARD_DATA_PRINT( STRUCTMEMBER )																			\
	MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwoHeader.STRUCTMEMBER, swsTemp );									\
	if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }										\
	::wsprintf( wValue, L"%u", _cwoHeader.STRUCTMEMBER );																\
	if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_VALUE ) ) { return false; }											\
	ssRaw.SecureClear();																								\
	CUtilities::MW_HEX_PRINT( &_cwoHeader.STRUCTMEMBER, sizeof( _cwoHeader.STRUCTMEMBER ), ssRaw );				\
	if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }

		CSecureWString swsTemp;
		CSecureString ssCString;
		CSecureString ssRaw;
		WCHAR wValue[64];

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_C8C7C62A_Magic );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_D1140659_The_state_of_the_image_file_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwoHeader.Magic, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			CHAR szBuffer[128];
			CHAR szMagic[64];
			CUtilities::PeMagicTypeToString( _cwoHeader.Magic, szMagic );
			std::sprintf( szBuffer, "%s (%u, %s)", szMagic, _cwoHeader.Magic, CUtilities::ToHex( _cwoHeader.Magic, 4 ) );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( szBuffer ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_cwoHeader.Magic, sizeof( _cwoHeader.Magic ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_A61E71E1_MajorLinkerVersion );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_D38C901A_The_major_version_number_of_the_linker_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( MajorLinkerVersion );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_943F72F8_MinorLinkerVersion );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_F7255C27_The_minor_version_number_of_the_linker_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( MinorLinkerVersion );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_BB68F636_SizeOfCode );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_DDC027AE_The_size_of_the_code_section__in_bytes__or_the_sum_of_all_such_sections_if_there_are_multiple_code_sections_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwoHeader.SizeOfCode, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			CHAR szBuffer[32];
			ssCString.SecureClear();
			std::sprintf( szBuffer, "%u (", _cwoHeader.SizeOfCode );
			ssCString.append( szBuffer );
			CUtilities::SizeString( _cwoHeader.SizeOfCode, ssCString );
			ssCString.push_back( ')' );

			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_cwoHeader.SizeOfCode, sizeof( _cwoHeader.SizeOfCode ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_C0E5353E_SizeOfInitializedData );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_5355503B_The_size_of_the_initialized_data_section__in_bytes__or_the_sum_of_all_such_sections_if_there_are_multiple_initialized_data_sections_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwoHeader.SizeOfInitializedData, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			CHAR szBuffer[32];
			ssCString.SecureClear();
			std::sprintf( szBuffer, "%u (", _cwoHeader.SizeOfInitializedData );
			ssCString.append( szBuffer );
			CUtilities::SizeString( _cwoHeader.SizeOfInitializedData, ssCString );
			ssCString.push_back( ')' );

			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_cwoHeader.SizeOfInitializedData, sizeof( _cwoHeader.SizeOfInitializedData ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_AC11E99C_SizeOfUninitializedData );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_870326AB_The_size_of_the_uninitialized_data_section__in_bytes__or_the_sum_of_all_such_sections_if_there_are_multiple_uninitialized_data_sections_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwoHeader.SizeOfUninitializedData, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			CHAR szBuffer[32];
			ssCString.SecureClear();
			std::sprintf( szBuffer, "%u (", _cwoHeader.SizeOfUninitializedData );
			ssCString.append( szBuffer );
			CUtilities::SizeString( _cwoHeader.SizeOfUninitializedData, ssCString );
			ssCString.push_back( ')' );

			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_cwoHeader.SizeOfUninitializedData, sizeof( _cwoHeader.SizeOfUninitializedData ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_CD01EA3A_AddressOfEntryPoint );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_714C56C8_A_pointer_to_the_entry_point_function__relative_to_the_image_base_address_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwoHeader.AddressOfEntryPoint, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			ssCString.SecureClear();
			_poPeObject.RelocAddressToStringwithSection( _cwoHeader.AddressOfEntryPoint, ssCString );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_cwoHeader.AddressOfEntryPoint, sizeof( _cwoHeader.AddressOfEntryPoint ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_D793A5D3_BaseOfCode );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_57FB8D7A_A_pointer_to_the_beginning_of_the_code_section__relative_to_the_image_base_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwoHeader.BaseOfCode, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			ssCString.SecureClear();
			CUtilities::ToHex( _cwoHeader.BaseOfCode, ssCString, 8 );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_cwoHeader.BaseOfCode, sizeof( _cwoHeader.BaseOfCode ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		return true;
#undef MW_STANDARD_DATA_PRINT
	}

	// Decodes a PE 32 header to a given parent.
	bool CPeWorksWindow::AddPe32Header( lsw::CTreeListView &_tlTree, HTREEITEM _tiParent, const MX_COFF_WINDOWS_PE32 &_cwpHeader, uint64_t _uiStructOffset ) {
#define MW_STANDARD_DATA_PRINT( STRUCTMEMBER )																			\
	MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwpHeader.STRUCTMEMBER, swsTemp );									\
	if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }										\
	::wsprintf( wValue, L"%u", _cwpHeader.STRUCTMEMBER );																\
	if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_VALUE ) ) { return false; }											\
	ssRaw.SecureClear();																								\
	CUtilities::MW_HEX_PRINT( &_cwpHeader.STRUCTMEMBER, sizeof( _cwpHeader.STRUCTMEMBER ), ssRaw );				\
	if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }

		CSecureWString swsTemp;
		CSecureString ssCString;
		CSecureString ssRaw;
		WCHAR wValue[64];

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_0D756628_BaseOfData );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_E502C66C_A_pointer_to_the_beginning_of_the_data_section__relative_to_the_image_base_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwpHeader.BaseOfData, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			ssCString.SecureClear();
			CUtilities::ToHex( _cwpHeader.BaseOfData, ssCString, 8 );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_cwpHeader.BaseOfData, sizeof( _cwpHeader.BaseOfData ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_F7F61A55_ImageBase );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_A251F072_The_preferred_address_of_the_first_byte_of_the_image_when_it_is_loaded_in_memory_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwpHeader.ImageBase, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			ssCString.SecureClear();
			CUtilities::ToHex( _cwpHeader.ImageBase, ssCString, 8 );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_cwpHeader.ImageBase, sizeof( _cwpHeader.ImageBase ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_07ADF8BE_SectionAlignment );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_6EB72FFF_The_alignment_of_sections_loaded_in_memory__in_bytes_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( SectionAlignment );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_82D11DF7_FileAlignment );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_85CFFC4C_The_alignment_of_the_raw_data_of_sections_in_the_image_file__in_bytes_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( FileAlignment );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_10871E2F_MajorOperatingSystemVersion );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_D481ABDC_The_major_version_number_of_the_required_operating_system_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwpHeader.MajorOperatingSystemVersion, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			CHAR szTemp[32];
			std::sprintf( szTemp, "%u", _cwpHeader.MajorOperatingSystemVersion );
			ssCString.SecureClear();
			ssCString.append( szTemp );
			std::sprintf( szTemp, " (%u.%u = ", _cwpHeader.MajorOperatingSystemVersion, _cwpHeader.MinorOperatingSystemVersion );
			ssCString.append( szTemp );
			CUtilities::WindowsVersion( _cwpHeader.MajorOperatingSystemVersion, _cwpHeader.MinorOperatingSystemVersion, ssCString );
			ssCString.push_back( ')' );

			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_cwpHeader.MajorOperatingSystemVersion, sizeof( _cwpHeader.MajorOperatingSystemVersion ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_BE137968_MinorOperatingSystemVersion );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_E8E4D323_The_minor_version_number_of_the_required_operating_system_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( MinorOperatingSystemVersion );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_7185AF2B_MajorImageVersion );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_C3E752F1_The_major_version_number_of_the_image_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( MajorImageVersion );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_88EA5507_MinorImageVersion );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_DE88C902_The_minor_version_number_of_the_image_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( MinorImageVersion );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_CF63D31C_MajorSubsystemVersion );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_4B2D03E8_The_major_version_number_of_the_subsystem_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( MajorSubsystemVersion );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_BBD0084C_MinorSubsystemVersion );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_AFDBF601_The_minor_version_number_of_the_subsystem_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( MinorSubsystemVersion );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_0BE89003_Win32VersionValue );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_28A62C8F_This_member_is_reserved_and_must_be_0_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( Win32VersionValue );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_F49FF776_SizeOfImage );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_68F7F519_The_size_of_the_image__in_bytes__including_all_headers_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwpHeader.SizeOfImage, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }


			CHAR szTemp[32];
			std::sprintf( szTemp, "%u (", _cwpHeader.SizeOfImage );
			ssCString.SecureClear();
			ssCString.append( szTemp );
			CUtilities::SizeString( _cwpHeader.SizeOfImage, ssCString );
			ssCString.push_back( ')' );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_cwpHeader.SizeOfImage, sizeof( _cwpHeader.SizeOfImage ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_97F1426A_SizeOfHeaders );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_51FDB88D_The_combined_size_of_the_e_lfanew_member_of_IMAGE_DOS_HEADER__4_byte_signature__size_of_IMAGE_FILE_HEADER__size_of_optional_header__and_the_size_of_all_section_headers_rounded_to_a_multiple_of_the_value_specified_in_the_FileAlignment_member_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwpHeader.SizeOfHeaders, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }


			CHAR szTemp[32];
			std::sprintf( szTemp, "%u (", _cwpHeader.SizeOfHeaders );
			ssCString.SecureClear();
			ssCString.append( szTemp );
			CUtilities::SizeString( _cwpHeader.SizeOfHeaders, ssCString );
			ssCString.push_back( ')' );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_cwpHeader.SizeOfHeaders, sizeof( _cwpHeader.SizeOfHeaders ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_1F54FB2C_CheckSum );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_577C2717_The_image_file_checksum_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( CheckSum );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_2DC01B0D_Subsystem );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_E530F84F_The_subsystem_required_to_run_this_image_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwpHeader.Subsystem, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			CHAR szTemp[64];
			CHAR szBuffer[96];
			CUtilities::PeSubSystemTypeToString( _cwpHeader.Subsystem, szTemp );
			std::sprintf( szBuffer, "%s (%u)", szTemp, _cwpHeader.Subsystem );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( szBuffer ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_cwpHeader.Subsystem, sizeof( _cwpHeader.Subsystem ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_81818514_DllCharacteristics );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_569D5C69_The_DLL_characteristics_of_the_image_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwpHeader.DllCharacteristics, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }


			CHAR szTemp[48];
			std::sprintf( szTemp, " (%u, %s)", _cwpHeader.DllCharacteristics, CUtilities::ToHex( _cwpHeader.DllCharacteristics, 4 ) );
			ssCString.SecureClear();
			CUtilities::PeDllCharacteristicsToString( _cwpHeader.DllCharacteristics, ssCString );
			ssCString.append( szTemp );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_cwpHeader.DllCharacteristics, sizeof( _cwpHeader.DllCharacteristics ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_76D292D2_SizeOfStackReserve );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_976AC976_The_number_of_bytes_to_reserve_for_the_stack_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwpHeader.SizeOfStackReserve, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			CHAR szBuffer[32];
			ssCString.SecureClear();
			std::sprintf( szBuffer, "%u (", _cwpHeader.SizeOfStackReserve );
			ssCString.append( szBuffer );
			CUtilities::SizeString( _cwpHeader.SizeOfStackReserve, ssCString );
			ssCString.push_back( ')' );

			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_cwpHeader.SizeOfStackReserve, sizeof( _cwpHeader.SizeOfStackReserve ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_7A6E203E_SizeOfStackCommit );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_11985DB0_The_number_of_bytes_to_commit_for_the_stack_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwpHeader.SizeOfStackCommit, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			CHAR szBuffer[32];
			ssCString.SecureClear();
			std::sprintf( szBuffer, "%u (", _cwpHeader.SizeOfStackCommit );
			ssCString.append( szBuffer );
			CUtilities::SizeString( _cwpHeader.SizeOfStackCommit, ssCString );
			ssCString.push_back( ')' );

			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_cwpHeader.SizeOfStackCommit, sizeof( _cwpHeader.SizeOfStackCommit ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_F1DB2148_SizeOfHeapReserve );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_AECDC551_The_number_of_bytes_to_reserve_for_the_local_heap_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwpHeader.SizeOfHeapReserve, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			CHAR szBuffer[32];
			ssCString.SecureClear();
			std::sprintf( szBuffer, "%u (", _cwpHeader.SizeOfHeapReserve );
			ssCString.append( szBuffer );
			CUtilities::SizeString( _cwpHeader.SizeOfHeapReserve, ssCString );
			ssCString.push_back( ')' );

			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_cwpHeader.SizeOfHeapReserve, sizeof( _cwpHeader.SizeOfHeapReserve ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_7B7E68AF_SizeOfHeapCommit );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_212336AA_The_number_of_bytes_to_commit_for_the_local_heap_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwpHeader.SizeOfHeapCommit, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			CHAR szBuffer[32];
			ssCString.SecureClear();
			std::sprintf( szBuffer, "%u (", _cwpHeader.SizeOfHeapCommit );
			ssCString.append( szBuffer );
			CUtilities::SizeString( _cwpHeader.SizeOfHeapCommit, ssCString );
			ssCString.push_back( ')' );

			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_cwpHeader.SizeOfHeapCommit, sizeof( _cwpHeader.SizeOfHeapCommit ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_1B0208B4_LoaderFlags );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_FD06EE87_Obsolete__If_bit_0_is_set__a_breakpoint_is_invoked_before_starting_the_process___If_bit_1_is_set__a_debugger_is_invoked_on_the_process_after_it_is_loaded_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( LoaderFlags );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_5A44785A_NumberOfRvaAndSizes );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_C6FBF31C_The_number_of_directory_entries_in_the_remainder_of_the_optional_header__Each_entry_describes_a_location_and_size_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( NumberOfRvaAndSizes );
		}

		return true;
#undef MW_STANDARD_DATA_PRINT

	}

	// Decodes a PE 64 header to a given parent.
	bool CPeWorksWindow::AddPe64Header( lsw::CTreeListView &_tlTree, HTREEITEM _tiParent, const MX_COFF_WINDOWS_PE64 &_cwpHeader, uint64_t _uiStructOffset ) {
#define MW_STANDARD_DATA_PRINT( STRUCTMEMBER )																			\
	MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwpHeader.STRUCTMEMBER, swsTemp );									\
	if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }										\
	::wsprintf( wValue, L"%u", _cwpHeader.STRUCTMEMBER );																\
	if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_VALUE ) ) { return false; }											\
	ssRaw.SecureClear();																								\
	CUtilities::MW_HEX_PRINT( &_cwpHeader.STRUCTMEMBER, sizeof( _cwpHeader.STRUCTMEMBER ), ssRaw );				\
	if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }

		CSecureWString swsTemp;
		CSecureString ssCString;
		CSecureString ssRaw;
		WCHAR wValue[64];

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_F7F61A55_ImageBase );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_A251F072_The_preferred_address_of_the_first_byte_of_the_image_when_it_is_loaded_in_memory_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwpHeader.ImageBase, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			ssCString.SecureClear();
			CUtilities::ToHex( _cwpHeader.ImageBase, ssCString, 8 );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_cwpHeader.ImageBase, sizeof( _cwpHeader.ImageBase ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_07ADF8BE_SectionAlignment );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_6EB72FFF_The_alignment_of_sections_loaded_in_memory__in_bytes_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( SectionAlignment );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_82D11DF7_FileAlignment );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_85CFFC4C_The_alignment_of_the_raw_data_of_sections_in_the_image_file__in_bytes_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( FileAlignment );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_10871E2F_MajorOperatingSystemVersion );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_D481ABDC_The_major_version_number_of_the_required_operating_system_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwpHeader.MajorOperatingSystemVersion, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			CHAR szTemp[32];
			std::sprintf( szTemp, "%u", _cwpHeader.MajorOperatingSystemVersion );
			ssCString.SecureClear();
			ssCString.append( szTemp );
			std::sprintf( szTemp, " (%u.%u = ", _cwpHeader.MajorOperatingSystemVersion, _cwpHeader.MinorOperatingSystemVersion );
			ssCString.append( szTemp );
			CUtilities::WindowsVersion( _cwpHeader.MajorOperatingSystemVersion, _cwpHeader.MinorOperatingSystemVersion, ssCString );
			ssCString.push_back( ')' );

			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_cwpHeader.MajorOperatingSystemVersion, sizeof( _cwpHeader.MajorOperatingSystemVersion ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_BE137968_MinorOperatingSystemVersion );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_E8E4D323_The_minor_version_number_of_the_required_operating_system_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( MinorOperatingSystemVersion );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_7185AF2B_MajorImageVersion );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_C3E752F1_The_major_version_number_of_the_image_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( MajorImageVersion );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_88EA5507_MinorImageVersion );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_DE88C902_The_minor_version_number_of_the_image_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( MinorImageVersion );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_CF63D31C_MajorSubsystemVersion );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_4B2D03E8_The_major_version_number_of_the_subsystem_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( MajorSubsystemVersion );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_BBD0084C_MinorSubsystemVersion );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_AFDBF601_The_minor_version_number_of_the_subsystem_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( MinorSubsystemVersion );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_0BE89003_Win32VersionValue );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_28A62C8F_This_member_is_reserved_and_must_be_0_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( Win32VersionValue );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_F49FF776_SizeOfImage );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_68F7F519_The_size_of_the_image__in_bytes__including_all_headers_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwpHeader.SizeOfImage, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }


			CHAR szTemp[32];
			std::sprintf( szTemp, "%u (", _cwpHeader.SizeOfImage );
			ssCString.SecureClear();
			ssCString.append( szTemp );
			CUtilities::SizeString( _cwpHeader.SizeOfImage, ssCString );
			ssCString.push_back( ')' );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_cwpHeader.SizeOfImage, sizeof( _cwpHeader.SizeOfImage ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_97F1426A_SizeOfHeaders );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_51FDB88D_The_combined_size_of_the_e_lfanew_member_of_IMAGE_DOS_HEADER__4_byte_signature__size_of_IMAGE_FILE_HEADER__size_of_optional_header__and_the_size_of_all_section_headers_rounded_to_a_multiple_of_the_value_specified_in_the_FileAlignment_member_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwpHeader.SizeOfHeaders, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }


			CHAR szTemp[32];
			std::sprintf( szTemp, "%u (", _cwpHeader.SizeOfHeaders );
			ssCString.SecureClear();
			ssCString.append( szTemp );
			CUtilities::SizeString( _cwpHeader.SizeOfHeaders, ssCString );
			ssCString.push_back( ')' );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_cwpHeader.SizeOfHeaders, sizeof( _cwpHeader.SizeOfHeaders ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_1F54FB2C_CheckSum );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_577C2717_The_image_file_checksum_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( CheckSum );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_2DC01B0D_Subsystem );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_E530F84F_The_subsystem_required_to_run_this_image_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwpHeader.Subsystem, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			CHAR szTemp[64];
			CHAR szBuffer[96];
			CUtilities::PeSubSystemTypeToString( _cwpHeader.Subsystem, szTemp );
			std::sprintf( szBuffer, "%s (%u)", szTemp, _cwpHeader.Subsystem );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( szBuffer ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_cwpHeader.Subsystem, sizeof( _cwpHeader.Subsystem ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_81818514_DllCharacteristics );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_569D5C69_The_DLL_characteristics_of_the_image_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwpHeader.DllCharacteristics, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }


			CHAR szTemp[48];
			std::sprintf( szTemp, " (%u, %s)", _cwpHeader.DllCharacteristics, CUtilities::ToHex( _cwpHeader.DllCharacteristics, 4 ) );
			ssCString.SecureClear();
			CUtilities::PeDllCharacteristicsToString( _cwpHeader.DllCharacteristics, ssCString );
			ssCString.append( szTemp );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_cwpHeader.DllCharacteristics, sizeof( _cwpHeader.DllCharacteristics ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_76D292D2_SizeOfStackReserve );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_976AC976_The_number_of_bytes_to_reserve_for_the_stack_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwpHeader.SizeOfStackReserve, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			CHAR szBuffer[32];
			ssCString.SecureClear();
			std::sprintf( szBuffer, "%I64u (", _cwpHeader.SizeOfStackReserve );
			ssCString.append( szBuffer );
			CUtilities::SizeString( _cwpHeader.SizeOfStackReserve, ssCString );
			ssCString.push_back( ')' );

			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_cwpHeader.SizeOfStackReserve, sizeof( _cwpHeader.SizeOfStackReserve ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_7A6E203E_SizeOfStackCommit );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_11985DB0_The_number_of_bytes_to_commit_for_the_stack_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwpHeader.SizeOfStackCommit, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			CHAR szBuffer[32];
			ssCString.SecureClear();
			std::sprintf( szBuffer, "%I64u (", _cwpHeader.SizeOfStackCommit );
			ssCString.append( szBuffer );
			CUtilities::SizeString( _cwpHeader.SizeOfStackCommit, ssCString );
			ssCString.push_back( ')' );

			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_cwpHeader.SizeOfStackCommit, sizeof( _cwpHeader.SizeOfStackCommit ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_F1DB2148_SizeOfHeapReserve );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_AECDC551_The_number_of_bytes_to_reserve_for_the_local_heap_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwpHeader.SizeOfHeapReserve, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			CHAR szBuffer[32];
			ssCString.SecureClear();
			std::sprintf( szBuffer, "%I64u (", _cwpHeader.SizeOfHeapReserve );
			ssCString.append( szBuffer );
			CUtilities::SizeString( _cwpHeader.SizeOfHeapReserve, ssCString );
			ssCString.push_back( ')' );

			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_cwpHeader.SizeOfHeapReserve, sizeof( _cwpHeader.SizeOfHeapReserve ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_7B7E68AF_SizeOfHeapCommit );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_212336AA_The_number_of_bytes_to_commit_for_the_local_heap_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _cwpHeader.SizeOfHeapCommit, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			CHAR szBuffer[32];
			ssCString.SecureClear();
			std::sprintf( szBuffer, "%I64u (", _cwpHeader.SizeOfHeapCommit );
			ssCString.append( szBuffer );
			CUtilities::SizeString( _cwpHeader.SizeOfHeapCommit, ssCString );
			ssCString.push_back( ')' );

			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_cwpHeader.SizeOfHeapCommit, sizeof( _cwpHeader.SizeOfHeapCommit ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_1B0208B4_LoaderFlags );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_FD06EE87_Obsolete__If_bit_0_is_set__a_breakpoint_is_invoked_before_starting_the_process___If_bit_1_is_set__a_debugger_is_invoked_on_the_process_after_it_is_loaded_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( LoaderFlags );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_5A44785A_NumberOfRvaAndSizes );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_C6FBF31C_The_number_of_directory_entries_in_the_remainder_of_the_optional_header__Each_entry_describes_a_location_and_size_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( NumberOfRvaAndSizes );
		}

		return true;
#undef MW_STANDARD_DATA_PRINT
	}

	// Decodes an array of data directories.
	bool CPeWorksWindow::AddDataDirectories( lsw::CTreeListView &_tlTree, HTREEITEM _tiParent, const std::vector<MX_DATA_DIRECTORY> &_vDataDirs, const mx::CPeObject &_poPeObject, uint64_t _uiImageBase, uint64_t _uiStructOffset ) {
		CSecureWString swsTemp;
		CSecureString ssCString;
		CSecureString ssRaw;
		WCHAR wValue[64];

		struct MX_DIRS {
			const CHAR * pcName;
			uint32_t ui32StrLen;

			const CHAR * pcDescName;
			uint32_t ui32DescStrLen;
		}
		static const aTable[MX_IMAGE_NUMBEROF_DIRECTORY_ENTRIES] = {
			{ _T_LEN_B4C7FDE2_DataDirectory_IMAGE_DIRECTORY_ENTRY_EXPORT_, _T_LEN_E00180C9_Export_directory_ },
			{ _T_LEN_207C04BE_DataDirectory_IMAGE_DIRECTORY_ENTRY_IMPORT_, _T_LEN_E9670B10_Import_directory_ },
			{ _T_LEN_E94D3397_DataDirectory_IMAGE_DIRECTORY_ENTRY_RESOURCE_, _T_LEN_43D2366B_Resource_directory_ },
			{ _T_LEN_352498F1_DataDirectory_IMAGE_DIRECTORY_ENTRY_EXCEPTION_, _T_LEN_7E617328_Exception_directory_ },
			{ _T_LEN_F1837EC1_DataDirectory_IMAGE_DIRECTORY_ENTRY_SECURITY_, _T_LEN_E318A398_Security_directory_ },
			{ _T_LEN_3918D7CF_DataDirectory_IMAGE_DIRECTORY_ENTRY_BASERELOC_, _T_LEN_3C63033E_Base_relocation_table_ },
			{ _T_LEN_35CDC213_DataDirectory_IMAGE_DIRECTORY_ENTRY_DEBUG_, _T_LEN_FC6D4DB9_Debug_directory_ },
			{ _T_LEN_B9A5B87A_DataDirectory_IMAGE_DIRECTORY_ENTRY_ARCHITECTURE_, _T_LEN_51010B52_Architecture_specific_data_ },
			{ _T_LEN_11FA74D4_DataDirectory_IMAGE_DIRECTORY_ENTRY_GLOBALPTR_, _T_LEN_2F9F4C35_Global_pointer_register_relative_virtual_address_ },
			{ _T_LEN_0A097252_DataDirectory_IMAGE_DIRECTORY_ENTRY_TLS_, _T_LEN_D218D979_Thread_local_storage_directory_ },
			{ _T_LEN_5F596E7F_DataDirectory_IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG_, _T_LEN_5FC3427C_Load_configuration_directory_ },
			{ _T_LEN_15F78447_DataDirectory_IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT_, _T_LEN_462CE6FA_Bound_import_directory_ },
			{ _T_LEN_EFE3F884_DataDirectory_IMAGE_DIRECTORY_ENTRY_IAT_, _T_LEN_ADA7D54A_Import_address_table_ },
			{ _T_LEN_261AB71E_DataDirectory_IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT_, _T_LEN_493F6470_Delay_import_table_ },
			{ _T_LEN_716D9916_DataDirectory_IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR_, _T_LEN_E80FA376_COM_descriptor_table_ },
			{ _T_LEN_B8592F2A_DataDirectory_15_, _T_LEN_F9026196_Reserved_ },
		};
		static const MX_DIRS aTableShort[MX_IMAGE_NUMBEROF_DIRECTORY_ENTRIES] = {
			{ _T_LEN_40866799_DataDirectory_EXPORT_, _T_LEN_E00180C9_Export_directory_ },
			{ _T_LEN_D43D9EC5_DataDirectory_IMPORT_, _T_LEN_E9670B10_Import_directory_ },
			{ _T_LEN_355C1DB1_DataDirectory_RESOURCE_, _T_LEN_43D2366B_Resource_directory_ },
			{ _T_LEN_E7F50C22_DataDirectory_EXCEPTION_, _T_LEN_7E617328_Exception_directory_ },
			{ _T_LEN_2D9250E7_DataDirectory_SECURITY_, _T_LEN_E318A398_Security_directory_ },
			{ _T_LEN_EBC9431C_DataDirectory_BASERELOC_, _T_LEN_3C63033E_Base_relocation_table_ },
			{ _T_LEN_A0E2E805_DataDirectory_DEBUG_, _T_LEN_FC6D4DB9_Debug_directory_ },
			{ _T_LEN_D2939E9D_DataDirectory_ARCHITECTURE_, _T_LEN_51010B52_Architecture_specific_data_ },
			{ _T_LEN_C32BE007_DataDirectory_GLOBALPTR_, _T_LEN_2F9F4C35_Global_pointer_register_relative_virtual_address_ },
			{ _T_LEN_D5802B5D_DataDirectory_TLS_, _T_LEN_D218D979_Thread_local_storage_directory_ },
			{ _T_LEN_022E7D2F_DataDirectory_LOAD_CONFIG_, _T_LEN_5FC3427C_Load_configuration_directory_ },
			{ _T_LEN_7EC1A2A0_DataDirectory_BOUND_IMPORT_, _T_LEN_462CE6FA_Bound_import_directory_ },
			{ _T_LEN_306AA18B_DataDirectory_IAT_, _T_LEN_ADA7D54A_Import_address_table_ },
			{ _T_LEN_4D2C91F9_DataDirectory_DELAY_IMPORT_, _T_LEN_493F6470_Delay_import_table_ },
			{ _T_LEN_B25F12F6_DataDirectory_COM_DESCRIPTOR_, _T_LEN_E80FA376_COM_descriptor_table_ },
			{ _T_LEN_B8592F2A_DataDirectory_15_, _T_LEN_F9026196_Reserved_ },
		};

		const MX_DIRS * pdUseMe = CUtilities::Options.bShortenEnumNames ? aTableShort : aTable;

		CHAR szTemp[64];
		for ( size_t I = 0; I < _vDataDirs.size(); ++I ) {
			swsTemp = mx::CStringDecoder::DecodeToWString( pdUseMe[I].pcName, pdUseMe[I].ui32StrLen );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( pdUseMe[I].pcDescName, pdUseMe[I].ui32DescStrLen ).c_str(), MW_PE_DESC ) ) { return false; }

			std::sprintf( szTemp, mx::CStringDecoder::DecodeToString( _T_LEN_F1D4865E_VirtualAddress__s__Size__u ).c_str(), CUtilities::ToHex( _vDataDirs[I].VirtualAddress, 8 ), _vDataDirs[I].Size );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( szTemp ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_vDataDirs[I], sizeof( _vDataDirs[I] ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }

			if ( _vDataDirs[I].VirtualAddress ) {
				AddDataDirectory( _tlTree, hItem, _vDataDirs[I], _poPeObject, _uiImageBase, _uiStructOffset );
			}

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _vDataDirs[I], swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }
		}
		::ZeroMemory( szTemp, sizeof( szTemp ) );
		
		return true;
	}

	// Decodes a data directory.
	bool CPeWorksWindow::AddDataDirectory( lsw::CTreeListView &_tlTree, HTREEITEM _tiParent, const MX_DATA_DIRECTORY &_ddDir, const mx::CPeObject &_poPeObject, uint64_t _uiImageBase, uint64_t _uiStructOffset ) {
#define MW_STANDARD_DATA_PRINT( STRUCTMEMBER )																			\
	MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _vDataDirs[I].STRUCTMEMBER, swsTemp );									\
	if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }										\
	::wsprintf( wValue, L"%u", _vDataDirs[I].STRUCTMEMBER );																\
	if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_VALUE ) ) { return false; }											\
	ssRaw.SecureClear();																								\
	CUtilities::MW_HEX_PRINT( &_vDataDirs[I].STRUCTMEMBER, sizeof( _vDataDirs[I].STRUCTMEMBER ), ssRaw );				\
	if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }

		CSecureWString swsTemp;
		CSecureString ssCString;
		CSecureString ssRaw;
		WCHAR wValue[64];

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_823DF0EC_VirtualAddress );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			//if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_212336AA_The_number_of_bytes_to_commit_for_the_local_heap_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _ddDir.VirtualAddress, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			ssCString.SecureClear();
			_poPeObject.RelocAddressToStringwithSection( _ddDir.VirtualAddress, ssCString );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_ddDir.VirtualAddress, sizeof( _ddDir.VirtualAddress ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			CHAR szBuffer[32];
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_57F28B54_Size );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			//if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_212336AA_The_number_of_bytes_to_commit_for_the_local_heap_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _ddDir.Size, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			ssCString.SecureClear();
			if ( !_ddDir.Size ) {
				std::sprintf( szBuffer, "%u", _ddDir.Size );
			}
			else {
				std::sprintf( szBuffer, "%u (", _ddDir.Size );
			}
			ssCString.append( szBuffer );
			if ( _ddDir.Size ) {
				CUtilities::SizeString( _ddDir.Size, ssCString );
				ssCString.push_back( ')' );
			}
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_ddDir.Size, sizeof( _ddDir.Size ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }

			/*
			std::string sTemp;
			if ( !_ddDir.Size ) {
				std::sprintf( szBuffer, "%u", _ddDir.Size );
			}
			else {
				std::sprintf( szBuffer, "%u (", _ddDir.Size );
			}
			sTemp.append( szBuffer );
			if ( _ddDir.Size ) {
				CUtilities::SizeString( _ddDir.Size, sTemp );
				sTemp.push_back( ')' );
			}
			CUtilities::BytesToHex( &_ddDir.Size, sizeof( _ddDir.Size ), szBufferRaw );
			MX_PRINT_FILE_OFFSET( _uiStructOffset, _ddDir.Size );
			QList<QVariant> lValues;
			lValues << "Size" << sTemp.c_str() << szOffset << szBufferRaw << "";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );*/
		}
		
		return true;
#undef MW_STANDARD_DATA_PRINT
	}

	// Decodes an array of MX_IMAGE_SECTION_HEADER objects.
	bool CPeWorksWindow::AddSectionHeaders( lsw::CTreeListView &_tlTree, HTREEITEM _tiParent, const std::vector<MX_IMAGE_SECTION_HEADER> &_vHeaders, uint64_t _uiImageBase, uint64_t _uiStructOffset ) {
		CSecureWString swsTemp;
		CSecureString ssCString;
		CSecureString ssRaw;
		WCHAR wValue[64];

		for ( size_t I = 0; I < _vHeaders.size(); ++I ) {
			CHAR szTemp[16];
			::ZeroMemory( szTemp, sizeof( szTemp ) );
			std::memcpy( szTemp, _vHeaders[I].Name, 8 );
			swsTemp.SecureClear();
			swsTemp = ee::CExpEval::StringToWString( szTemp );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );

			AddSectionHeader( _tlTree, hItem, _vHeaders[I], _uiImageBase, _uiStructOffset );

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _vHeaders[I], swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }
			/*
			QList<QVariant> lValues;
			lValues << szTemp << "" << "" << "" << "";
			CTreeViewItem * ptviTemp = new CTreeViewItem( lValues, _ptviParent );
			_ptviParent->AppendChild( ptviTemp );
			AddSectionHeader( ptviTemp, _vHeaders[I], _uiImageBase, _uiStructOffset );
			_uiStructOffset += sizeof( MX_IMAGE_SECTION_HEADER );
			*/
		}
		
		return true;
	}

	// Decodes an MX_IMAGE_SECTION_HEADER object.
	bool CPeWorksWindow::AddSectionHeader( lsw::CTreeListView &_tlTree, HTREEITEM _tiParent, const MX_IMAGE_SECTION_HEADER &_ishHeader, uint64_t _uiImageBase, uint64_t _uiStructOffset ) {
#define MW_STANDARD_DATA_PRINT( STRUCTMEMBER )																			\
	MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _ishHeader.STRUCTMEMBER, swsTemp );									\
	if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }										\
	::wsprintf( wValue, L"%u", _ishHeader.STRUCTMEMBER );																\
	if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_VALUE ) ) { return false; }											\
	ssRaw.SecureClear();																								\
	CUtilities::MW_HEX_PRINT( &_ishHeader.STRUCTMEMBER, sizeof( _ishHeader.STRUCTMEMBER ), ssRaw );						\
	if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }

		CSecureWString swsTemp;
		CSecureString ssCString;
		CSecureString ssRaw;
		WCHAR wValue[64];

		{

			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_FE11D138_Name );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_E9729511_Name_of_the_section_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _ishHeader.Name, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			CHAR szTemp[16];
			::ZeroMemory( szTemp, sizeof( szTemp ) );
			std::memcpy( szTemp, _ishHeader.Name, 8 );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( szTemp ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::BytesToHexWithSpaces( &_ishHeader.Name, sizeof( _ishHeader.Name ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }

			::ZeroMemory( szTemp, sizeof( szTemp ) );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_82590091_PhysicalAddress_VirtualSize );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_687D994E_A_physical_address_or_the_total_size_of_the_section_when_loaded_into_memory__in_bytes_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _ishHeader.Misc.PhysicalAddress, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			CHAR szTemp[32];
			std::sprintf( szTemp, "%s (%u)", CUtilities::ToHex( _ishHeader.Misc.PhysicalAddress, 8 ), _ishHeader.Misc.PhysicalAddress );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( szTemp ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::BytesToHexWithSpaces( &_ishHeader.Misc.PhysicalAddress, sizeof( _ishHeader.Misc.PhysicalAddress ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }


			::ZeroMemory( szTemp, sizeof( szTemp ) );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_823DF0EC_VirtualAddress );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_52747794_The_address_of_the_first_byte_of_the_section_when_loaded_into_memory__relative_to_the_image_base_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _ishHeader.VirtualAddress, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }


			CHAR szTemp[32];
			CHAR szBuffer[48];
			std::sprintf( szBuffer, mx::CStringDecoder::DecodeToString( _T_LEN_03FE75A1__s___s_after_loading_ ).c_str(), CUtilities::ToHex( _ishHeader.VirtualAddress, 8 ),
				CUtilities::ToHex( _ishHeader.VirtualAddress + _uiImageBase, szTemp, MX_ELEMENTS( szTemp ), 0 ) );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( szBuffer ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::BytesToHexWithSpaces( &_ishHeader.VirtualAddress, sizeof( _ishHeader.VirtualAddress ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }


			::ZeroMemory( szTemp, sizeof( szTemp ) );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_22B670F2_SizeOfRawData );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_35B4F3B3_The_size_of_the_initialized_data_on_disk__in_bytes_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _ishHeader.SizeOfRawData, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			CHAR szBuffer[32];
			ssCString.SecureClear();
			std::sprintf( szBuffer, "%u (", _ishHeader.SizeOfRawData );
			ssCString.append( szBuffer );
			CUtilities::SizeString( _ishHeader.SizeOfRawData, ssCString );
			ssCString.push_back( ')' );

			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_ishHeader.SizeOfRawData, sizeof( _ishHeader.SizeOfRawData ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_26594F0E_PointerToRawData );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_BEFD1219_A_file_pointer_to_the_first_page_within_the_COFF_file_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _ishHeader.PointerToRawData, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }


			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( CUtilities::ToHex( _ishHeader.PointerToRawData, 8 ) ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::BytesToHexWithSpaces( &_ishHeader.PointerToRawData, sizeof( _ishHeader.PointerToRawData ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_DB18CB2C_PointerToRelocations );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_1951565D_A_file_pointer_to_the_beginning_of_the_relocation_entries_for_the_section_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _ishHeader.PointerToRelocations, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }


			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( CUtilities::ToHex( _ishHeader.PointerToRelocations, 8 ) ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::BytesToHexWithSpaces( &_ishHeader.PointerToRelocations, sizeof( _ishHeader.PointerToRelocations ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_2A137B15_PointerToLinenumbers );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_8F6E3689_A_file_pointer_to_the_beginning_of_the_line_number_entries_for_the_section_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _ishHeader.PointerToLinenumbers, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }


			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( CUtilities::ToHex( _ishHeader.PointerToLinenumbers, 8 ) ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::BytesToHexWithSpaces( &_ishHeader.PointerToLinenumbers, sizeof( _ishHeader.PointerToLinenumbers ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_308C6EB5_NumberOfRelocations );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_A0244534_The_number_of_relocation_entries_for_the_section_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( NumberOfRelocations );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_C187DE8C_NumberOfLinenumbers );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_BBB99A66_The_number_of_line_number_entries_for_the_section_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( NumberOfLinenumbers );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_F8F28FC8_Characteristics );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_96C80FB7_The_characteristics_of_the_image_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _ishHeader.Characteristics, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			CHAR szTemp[48];
			std::sprintf( szTemp, " (%u, %s)", _ishHeader.Characteristics, CUtilities::ToHex( _ishHeader.Characteristics, 4 ) );

			ssCString.SecureClear();
			CUtilities::PeSectionFlagsToString( _ishHeader.Characteristics, ssCString );
			ssCString.append( szTemp );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_ishHeader.Characteristics, sizeof( _ishHeader.Characteristics ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }

			::ZeroMemory( szTemp, sizeof( szTemp ) );
		}

		return true;
#undef MW_STANDARD_DATA_PRINT
	}

	// Decodes exports.
	bool CPeWorksWindow::AddExports( lsw::CTreeListView &_tlTree, HTREEITEM _tiParent, const mx::CPeObject &_poPeObject ) {
		if ( _poPeObject.ExportDescriptor() ) {
			CSecureWString swsTemp;
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_9B9AAE5F_Export_Description );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _poPeObject.ExportDescriptorOffset(), _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_903B70B6_Export_descriptor_ ).c_str(), MW_PE_DESC ) ) { return false; }

			AddExportDesc( _tlTree, hItem, _poPeObject );
		}
		
		return true;
	}

	// Decodes an MX_IMAGE_EXPORT_DIRECTORY object.
	bool CPeWorksWindow::AddExportDesc( lsw::CTreeListView &_tlTree, HTREEITEM _tiParent, const mx::CPeObject &_poPeObject ) {
#define MW_STANDARD_DATA_PRINT( STRUCTMEMBER )																									\
	MX_PRINT_FILE_OFFSET( wValue, uiOffset, _poPeObject.ExportDescriptor()->STRUCTMEMBER, swsTemp );											\
	if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }																\
	::wsprintf( wValue, L"%u", _poPeObject.ExportDescriptor()->STRUCTMEMBER );																	\
	if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_VALUE ) ) { return false; }																	\
	ssRaw.SecureClear();																														\
	CUtilities::MW_HEX_PRINT( &_poPeObject.ExportDescriptor()->STRUCTMEMBER, sizeof( _poPeObject.ExportDescriptor()->STRUCTMEMBER ), ssRaw );	\
	if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }

		CSecureWString swsTemp;
		CSecureString ssCString;
		CSecureString ssRaw;
		WCHAR wValue[64];

		uint64_t uiOffset = _poPeObject.ExportDescriptorOffset();
		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_F8F28FC8_Characteristics );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), uiOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_059CBD2E_Unused_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( Characteristics );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_52BE8619_TimeDateStamp );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), uiOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_EE98A75B_32_bit_time_at_which_this_header_was_generated__DD_MM_YYYY_HH_MM_SS__ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, uiOffset, _poPeObject.ExportDescriptor()->TimeDateStamp, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			ssCString.SecureClear();
			CUtilities::CreateDateString( _poPeObject.ExportDescriptor()->TimeDateStamp, ssCString );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_poPeObject.ExportDescriptor()->TimeDateStamp, sizeof( _poPeObject.ExportDescriptor()->TimeDateStamp ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_02E44089_MajorVersion );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), uiOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_059CBD2E_Unused_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( MajorVersion );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_B1CC01B6_MinorVersion );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), uiOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_059CBD2E_Unused_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( MinorVersion );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_FE11D138_Name );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), uiOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_FB9A87C7_Name_of_this_DLL_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, uiOffset, _poPeObject.ExportDescriptor()->TimeDateStamp, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			
			CHAR szBuffer[48];
			std::sprintf( szBuffer, "%s (", CUtilities::ToHex( _poPeObject.ExportDescriptor()->Name, 8 ) );
			ssCString.SecureClear();
			ssCString = szBuffer;
			_poPeObject.GetExportDllName( ssCString );
			ssCString.push_back( ')' );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_poPeObject.ExportDescriptor()->Name, sizeof( _poPeObject.ExportDescriptor()->Name ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }

			ZeroMemory( szBuffer, sizeof( szBuffer ) );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_6086515F_Base );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), uiOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_8132044F_The_starting_ordinal_number_for_exported_functions_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( Base );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_C6812A82_NumberOfFunctions );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), uiOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_6FDEC421_The_number_of_elements_in_the_AddressOfFunctions_array_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( NumberOfFunctions );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_DB1B0887_NumberOfNames );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), uiOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_D53E6B35_The_number_of_elements_in_the_AddressOfNames_array_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( NumberOfNames );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_930A5325_AddressOfFunctions );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), uiOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_E6E545BC_Points_to_an_array_of_function_addresses_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, uiOffset, _poPeObject.ExportDescriptor()->AddressOfFunctions, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			ssCString.SecureClear();
			_poPeObject.RelocAddressToStringwithSection( _poPeObject.ExportDescriptor()->AddressOfFunctions, ssCString );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_poPeObject.ExportDescriptor()->AddressOfFunctions, sizeof( _poPeObject.ExportDescriptor()->AddressOfFunctions ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_0B5CBD14_AddressOfNames );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), uiOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_2E242588_Points_to_an_array_of_function_names_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, uiOffset, _poPeObject.ExportDescriptor()->AddressOfNames, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			ssCString.SecureClear();
			_poPeObject.RelocAddressToStringwithSection( _poPeObject.ExportDescriptor()->AddressOfNames, ssCString );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_poPeObject.ExportDescriptor()->AddressOfNames, sizeof( _poPeObject.ExportDescriptor()->AddressOfNames ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_BE313889_AddressOfNameOrdinals );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), uiOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_018C0EF7_Points_to_an_array_of_function_ordinals_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, uiOffset, _poPeObject.ExportDescriptor()->AddressOfNameOrdinals, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			ssCString.SecureClear();
			_poPeObject.RelocAddressToStringwithSection( _poPeObject.ExportDescriptor()->AddressOfNameOrdinals, ssCString );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_poPeObject.ExportDescriptor()->AddressOfNameOrdinals, sizeof( _poPeObject.ExportDescriptor()->AddressOfNameOrdinals ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		return true;
#undef MW_STANDARD_DATA_PRINT
	}

	// Decodes an MX_IMAGE_IMPORT_DESCRIPTOR object.
	bool CPeWorksWindow::AddImportDesc( lsw::CTreeListView &_tlTree, HTREEITEM _tiParent, const MX_IMAGE_IMPORT_DESCRIPTOR &_iidDesc, const mx::CPeObject &_poPeObject, uint64_t _uiStructOffset ) {
#define MW_STANDARD_DATA_PRINT( STRUCTMEMBER )																									\
	MX_PRINT_FILE_OFFSET( wValue, uiOffset, _poPeObject.ExportDescriptor()->STRUCTMEMBER, swsTemp );											\
	if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }																\
	::wsprintf( wValue, L"%u", _poPeObject.ExportDescriptor()->STRUCTMEMBER );																	\
	if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_VALUE ) ) { return false; }																	\
	ssRaw.SecureClear();																														\
	CUtilities::MW_HEX_PRINT( &_poPeObject.ExportDescriptor()->STRUCTMEMBER, sizeof( _poPeObject.ExportDescriptor()->STRUCTMEMBER ), ssRaw );	\
	if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }

		CSecureWString swsTemp;
		CSecureString ssCString;
		CSecureString ssRaw;
		WCHAR wValue[64];

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_F8F28FC8_Characteristics );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_394E2D11_A_pointer_to_a_table_of_imported_functions_from_this_library_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _iidDesc.Characteristics, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }


			CHAR szBuffer[128];
			CHAR szTempOff1[32], szTempOff2[32];
			std::sprintf( szBuffer, mx::CStringDecoder::DecodeToString( _T_LEN_03E23EE2__s__file_offset__s_ ).c_str(),
				CUtilities::ToHex( _iidDesc.Characteristics, szTempOff1, MX_ELEMENTS( szTempOff1 ), 8 ),
				CUtilities::ToHex( _poPeObject.RelocAddressToFileOffset( _iidDesc.Characteristics ), szTempOff2, MX_ELEMENTS( szTempOff2 ), 8 ) );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( szBuffer ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_iidDesc.Characteristics, sizeof( _iidDesc.Characteristics ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }

			::ZeroMemory( szBuffer, sizeof( szBuffer ) );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_52BE8619_TimeDateStamp );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_8A8664AD_32_bit_time_at_which_the_file_was_generated__DD_MM_YYYY_HH_MM_SS__ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _iidDesc.TimeDateStamp, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			ssCString.SecureClear();
			CUtilities::CreateDateString( _iidDesc.TimeDateStamp, ssCString );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_iidDesc.TimeDateStamp, sizeof( _iidDesc.TimeDateStamp ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_C37FB7F7_ForwarderChain );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_E6BA1924_Index_into_FirstThunk_of_a_forwarded_function_or_0xFFFFFFFF_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _iidDesc.ForwarderChain, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			ssCString.SecureClear();
			CUtilities::ToHex( _iidDesc.ForwarderChain, ssCString, 8 );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_iidDesc.ForwarderChain, sizeof( _iidDesc.ForwarderChain ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_FE11D138_Name );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_AD65E47E_Relative_virtual_address_to_the_name_of_this_DLL_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _iidDesc.Name, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }


			uint32_t uiOffset;
			uint32_t uiIndex = _poPeObject.RelocAddrToRelocIndexAndOffset( _iidDesc.Name, uiOffset );
			ssCString.SecureClear();
			if ( uiIndex <= _poPeObject.SectionData().size() ) {
				CSecureString sTemp;
				while ( uiOffset < _poPeObject.SectionData()[uiIndex].vData.size() ) {
					CHAR cTemp = _poPeObject.SectionData()[uiIndex].vData[uiOffset++];
					if ( !cTemp ) { break; }
					sTemp.push_back( cTemp );
				}
				CUtilities::ToHex( _iidDesc.Name, ssCString, 8 );
				ssCString.push_back( ' ' );
				ssCString.push_back( '(' );
				ssCString.append( sTemp );
				ssCString.push_back( ')' );
			}
			else {
				CUtilities::ToHex( _iidDesc.Name, ssCString, 8 );
			}
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_iidDesc.Name, sizeof( _iidDesc.Name ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_638DD947_FirstThunk );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_8499D0FF_Relative_virtual_address_to_an_IMAGE_THUNK_DATA_union_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _iidDesc.FirstThunk, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			ssCString.SecureClear();
			CHAR szBuffer[64];
			CHAR szTempOff[32];
			std::sprintf( szBuffer, "%s (file offset %s)",
				CUtilities::ToHex( _iidDesc.FirstThunk, 8 ),
				CUtilities::ToHex( _poPeObject.RelocAddressToFileOffset( _iidDesc.FirstThunk ), szTempOff, MX_ELEMENTS( szTempOff ), 8 ) );

			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( szBuffer ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_iidDesc.FirstThunk, sizeof( _iidDesc.FirstThunk ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }

			ZeroMemory( szBuffer, sizeof( szBuffer ) );
		}
		
		return true;
#undef MW_STANDARD_DATA_PRINT
	}

	// Decodes an MX_IMAGE_IMPORT_DESCRIPTOR object.
	bool CPeWorksWindow::AddResourceDesc( lsw::CTreeListView &_tlTree, HTREEITEM _tiParent, const MX_IMAGE_RESOURCE_DIRECTORY &_irdDesc, const mx::CPeObject &_poPeObject, uint64_t _uiStructOffset ) {
#define MW_STANDARD_DATA_PRINT( STRUCTMEMBER )																									\
	MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _irdDesc.STRUCTMEMBER, swsTemp );															\
	if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }																\
	::wsprintf( wValue, L"%u", _irdDesc.STRUCTMEMBER );																							\
	if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_VALUE ) ) { return false; }																	\
	ssRaw.SecureClear();																														\
	CUtilities::MW_HEX_PRINT( &_irdDesc.STRUCTMEMBER, sizeof( _irdDesc.STRUCTMEMBER ), ssRaw );	\
	if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }

		CSecureWString swsTemp;
		CSecureString ssCString;
		CSecureString ssRaw;
		WCHAR wValue[64];
		
		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_F8F28FC8_Characteristics );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_059CBD2E_Unused_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( Characteristics );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_52BE8619_TimeDateStamp );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_3FBE666B_32_bit_time_at_which_this_file_was_generated__DD_MM_YYYY_HH_MM_SS__ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _irdDesc.TimeDateStamp, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }

			ssCString.SecureClear();
			CUtilities::CreateDateString( _irdDesc.TimeDateStamp, ssCString );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_irdDesc.TimeDateStamp, sizeof( _irdDesc.TimeDateStamp ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_02E44089_MajorVersion );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_3F42F050_Major_version_of_the_resource_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( MajorVersion );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_B1CC01B6_MinorVersion );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_9749C186_Minor_version_of_the_resource_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( MinorVersion );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_D3BC7C71_NumberOfNamedEntries );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_2544B610_Number_of_entries_with_names_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( NumberOfNamedEntries );
		}

		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_B86CDFB9_NumberOfIdEntries );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_48C20C6B_Number_of_entries_with_ID_values_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MW_STANDARD_DATA_PRINT( NumberOfIdEntries );
		}
		
		return true;
#undef MW_STANDARD_DATA_PRINT
	}

	// Decodes the relocation table.
	bool CPeWorksWindow::AddReloc( lsw::CTreeListView &_tlTree, HTREEITEM _tiParent, const std::vector<CPeObject::MX_BASE_RELOC> &_vRelocs, const mx::CPeObject &_poPeObject ) {
#define MW_STANDARD_DATA_PRINT( STRUCTMEMBER )																									\
	MX_PRINT_FILE_OFFSET( wValue, uiOffset, _vRelocs[I].pibrReloc->STRUCTMEMBER, swsTemp );														\
	if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }																\
	::wsprintf( wValue, L"%u", _vRelocs[I].pibrReloc->STRUCTMEMBER );																			\
	if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_VALUE ) ) { return false; }																	\
	ssRaw.SecureClear();																														\
	CUtilities::MW_HEX_PRINT( &_vRelocs[I].pibrReloc->STRUCTMEMBER, sizeof( _vRelocs[I].pibrReloc->STRUCTMEMBER ), ssRaw );						\
	if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }

		CSecureWString swsTemp;
		CSecureString ssCString;
		CSecureString ssRaw;
		WCHAR wValue[64];
		uint64_t uiOffset = 0;

		for ( size_t I = 0; I < _vRelocs.size(); ++I ) {
			{
				uiOffset = _vRelocs[I].ui64FileOffset;
				swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_823DF0EC_VirtualAddress );
				TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), uiOffset, _tiParent );
				HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
				if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_70039ABB_Virtual_address_of_the_start_of_the_chunk_of_relocation_offsets_ ).c_str(), MW_PE_DESC ) ) { return false; }

				MX_PRINT_FILE_OFFSET( wValue, uiOffset, _vRelocs[I].pibrReloc->VirtualAddress, swsTemp );
				if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }


				ssCString.SecureClear();
				_poPeObject.RelocAddressToStringwithSection( _vRelocs[I].pibrReloc->VirtualAddress, ssCString );
				if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
				ssRaw.SecureClear();
				CUtilities::MW_HEX_PRINT( &_vRelocs[I].pibrReloc->VirtualAddress, sizeof( _vRelocs[I].pibrReloc->VirtualAddress ), ssRaw );
				if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
			}

			{
				WCHAR swzTemp[128];
				int iLen = ::wsprintf( swzTemp, _DEC_WS_31984B07_SizeOfBlock___u_relocations_.c_str(), _vRelocs[I].uiTotal );
				swsTemp = swzTemp;
				ZeroMemory( swzTemp, sizeof( swzTemp[0] ) * iLen );
				TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), uiOffset, _tiParent );
				HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
				if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_D862F717_Size_of_the_chunk_ ).c_str(), MW_PE_DESC ) ) { return false; }

				MW_STANDARD_DATA_PRINT( SizeOfBlock );

				AddReloc( _tlTree, hItem, _vRelocs[I], _poPeObject, uiOffset );
			}
		}
		
		return true;
#undef MW_STANDARD_DATA_PRINT
	}

	// Decodes the relocation offsets.
	bool CPeWorksWindow::AddReloc( lsw::CTreeListView &_tlTree, HTREEITEM _tiParent, const CPeObject::MX_BASE_RELOC &_brRelocs, const mx::CPeObject &_poPeObject, uint64_t _uiStructOffset ) {
		WCHAR wValue[64];
		CSecureWString swsTemp;
		CSecureString ssCString;
		CSecureString ssRaw;
		for ( uint32_t I = 0; I < _brRelocs.uiTotal; ++I ) {
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_95CFA75D_TypeOffset );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), _uiStructOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_D2C02962_Type_and_offset_ ).c_str(), MW_PE_DESC ) ) { return false; }

			MX_PRINT_FILE_OFFSET( wValue, _uiStructOffset, _brRelocs.puiOffsets[I], swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }


			ssCString.SecureClear();
			CUtilities::PeRelocBaseToString( _brRelocs.puiOffsets[I] >> 12, ssCString );
			ssCString += " (";
			CUtilities::ToUnsigned( _brRelocs.puiOffsets[I] >> 12, ssCString );
			ssCString += "), ";
			_poPeObject.RelocAddressToStringwithSection( _brRelocs.pibrReloc->VirtualAddress + (_brRelocs.puiOffsets[I] & 0xFFF), ssCString );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_brRelocs.puiOffsets[I], sizeof( _brRelocs.puiOffsets[I] ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		return true;
	}

	// Decodes exported functions.
	bool CPeWorksWindow::AddExportedFunctions( lsw::CTreeListView &_tlTree, HTREEITEM _tiParent, const mx::CPeObject &_poPeObject ) {
		std::vector<CPeObject::MX_EXPORT> vList;
		_poPeObject.GetExports( vList );

		if ( vList.size() ) {
			WCHAR wValue[64];
			CSecureWString swsTemp;
			{
				swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_23932DDC_Exports );
				swsTemp += L" (" + std::to_wstring( vList.size() ) + L")";

				TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), -1, _tiParent );
				HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
				if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_70039ABB_Virtual_address_of_the_start_of_the_chunk_of_relocation_offsets_ ).c_str(), MW_PE_DESC ) ) { return false; }
				if ( !AddExportedFunctions( _tlTree, hItem, _poPeObject, vList ) ) { return false; }
			}
		}

		return true;
	}

	// Decodes exported functions.
	bool CPeWorksWindow::AddExportedFunctions( lsw::CTreeListView &_tlTree, HTREEITEM _tiParent, const mx::CPeObject &_poPeObject, const std::vector<CPeObject::MX_EXPORT> &_vList ) {
		WCHAR wValue[64];
		CSecureWString swsTemp;
		CSecureString ssCString;
		CSecureString ssRaw;

		for ( size_t I = 0; I < _vList.size(); ++I ) {
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_9E965BE5_Address__Ordinal__Name );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), -1, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );

			CHAR szHex0[32], szU0[32];
			CHAR szHex1[32], szU1[32];
			CHAR szHex2[32], szU2[32];
			CHAR szOffset[128];
			std::snprintf( szOffset, MX_ELEMENTS( szOffset ), mx::CStringDecoder::DecodeToString( _T_LEN_7D6FFEED__s___s____s___s____s___s_ ).c_str(),
				CUtilities::ToHex( _vList[I].ui64FileOffsetAddr, szHex0, MX_ELEMENTS( szHex0 ), 8 ), CUtilities::ToUnsigned( _vList[I].ui64FileOffsetAddr, szU0, MX_ELEMENTS( szU0 ) ),
				CUtilities::ToHex( _vList[I].ui64FileOffsetOrd, szHex1, MX_ELEMENTS( szHex1 ), 8 ), CUtilities::ToUnsigned( _vList[I].ui64FileOffsetOrd, szU1, MX_ELEMENTS( szU1 ) ),
				CUtilities::ToHex( _vList[I].ui64FileOffsetName, szHex2, MX_ELEMENTS( szHex2 ), 8 ), CUtilities::ToUnsigned( _vList[I].ui64FileOffsetName, szU2, MX_ELEMENTS( szU2 ) ) );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( szOffset ).c_str(), MW_PE_OFFSET ) ) { return false; }



			ssCString.SecureClear();
			CUtilities::ToHex( _vList[I].uiAddress, ssCString, _poPeObject.Is64() ? 16 : 8 );
			ssCString.append( ", " );
			CUtilities::ToUnsigned( _vList[I].uiOrdinal, ssCString );
			ssCString.append( ", " );
			ssCString += _vList[I].sName;
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }

			ssRaw.SecureClear();
			CUtilities::BytesToHex( &_vList[I].uiAddress, sizeof( _vList[I].uiAddress ), ssRaw );
			ssRaw.push_back( ' ' );
			CUtilities::BytesToHex( &_vList[I].uiOrdinal, sizeof( _vList[I].uiOrdinal ), ssRaw );
			ssRaw.push_back( ' ' );
			CUtilities::BytesToHexWithSpaces( _vList[I].sName.c_str(), _vList[I].sName.size(), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		return true;
	}

	// Decodes all imports.
	bool CPeWorksWindow::AddImportedFuncs( lsw::CTreeListView &_tlTree, HTREEITEM _tiParent, const mx::CPeObject &_poPeObject ) {
		if ( _poPeObject.ImportDescriptor().size() > 1 ) {
			CSecureWString swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_B728D480_Imports );
			swsTemp += L" (" + std::to_wstring( _poPeObject.ImportDescriptor().size() ) + L")";
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), -1, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_F799DAB6_Function_imports_ ).c_str(), MW_PE_DESC ) ) { return false; }
			for ( size_t I = 0; I < _poPeObject.ImportDescriptor().size() - 1; ++I ) {
				if ( !AddImportedFuncs( _tlTree, hItem, I, _poPeObject ) ) { return false; }
			}
		}

		return true;
	}

	// Decodes imports from a given import directory.
	bool CPeWorksWindow::AddImportedFuncs( lsw::CTreeListView &_tlTree, HTREEITEM _tiParent, uint32_t _uiImportDir, const mx::CPeObject &_poPeObject ) {
		if ( _uiImportDir == _poPeObject.ImportDescriptor().size() - 1 ) { return true; }
		{
			std::vector<CPeObject::MX_IMPORT> vImports;
			_poPeObject.GetImportsFromImportDescByIndex( _uiImportDir, vImports );

			CSecureString ssTemp;
			_poPeObject.ImportDllNameByIndex( _uiImportDir, ssTemp );
			ssTemp += " (";
			CUtilities::ToUnsigned( vImports.size(), ssTemp );
			ssTemp += " functions)";
			CSecureWString swsTemp = ee::CExpEval::StringToWString( ssTemp );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), -1, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
			
			for ( size_t I = 0; I < vImports.size(); ++I ) {
				AddImport( _tlTree, hItem, vImports[I], _poPeObject );
			}
		}
		return true;
	}

	// Decodes an import.
	bool CPeWorksWindow::AddImport( lsw::CTreeListView &_tlTree, HTREEITEM _tiParent, const CPeObject::MX_IMPORT &_iImport, const mx::CPeObject &_poPeObject ) {
		uint64_t uiOffset = _iImport.ui64FileOffset;
		{
			CSecureWString swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_3A0BEF98_Ordinal__Name );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), uiOffset, _tiParent );
			HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );

			WCHAR wValue[64];
			MX_PRINT_FILE_OFFSET( wValue, uiOffset, _iImport.uiOrdinal, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }


			
			CSecureString ssCString;
			CUtilities::ToHex( _iImport.uiOrdinal, ssCString, 4 );
			ssCString += std::string( ", " ) + _iImport.sName;
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }


			CSecureString ssRaw;
			if ( _iImport.bBound ) {
				uint8_t uiBuf[32];
				uint32_t uiSize = _poPeObject.Is64() ? sizeof( uint64_t ) : sizeof( uint32_t );
				_poPeObject.ReadRelocBytes( _poPeObject.FileOffsetToRelocAddress( _iImport.ui64FileOffset ), uiBuf, uiSize );
				CUtilities::BytesToHex( uiBuf, uiSize, ssRaw );
			}
			else {
				CUtilities::BytesToHex( &_iImport.uiOrdinal, sizeof( _iImport.uiOrdinal ), ssRaw );
				ssRaw += ' ';
				CUtilities::BytesToHexWithSpaces( _iImport.sName.c_str(), _iImport.sName.size(), ssRaw );
			}
			
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
		}

		return true;
	}

	// Decodes the resource tree.
	bool CPeWorksWindow::AddResourceTree( lsw::CTreeListView &_tlTree, HTREEITEM _tiParent, const mx::CPeObject &_poPeObject ) {
		if ( !_poPeObject.ResourceDescriptor() ) { return true; }
		uint32_t uiOffset;
		uint32_t uiIndex = _poPeObject.RelocAddrToRelocIndexAndOffset( _poPeObject.DataDirectory()[MX_IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress, uiOffset );
		return AddResourceTree( _tlTree, _tiParent, _poPeObject.ResourceDescriptor(), uiIndex, _poPeObject, 0 );
	}

	// Decodes the resource tree.
	bool CPeWorksWindow::AddResourceTree( lsw::CTreeListView &_tlTree, HTREEITEM _tiParent, const MX_IMAGE_RESOURCE_DIRECTORY * _pirdDesc, uint32_t _uiSectionIndex, const mx::CPeObject &_poPeObject, uint32_t _uiLevel ) {
		uint32_t uiTotal = _pirdDesc->NumberOfNamedEntries + _pirdDesc->NumberOfIdEntries;

		const MX_IMAGE_RESOURCE_DIRECTORY_ENTRY * pirdeEntry = reinterpret_cast<const MX_IMAGE_RESOURCE_DIRECTORY_ENTRY *>(_pirdDesc + 1);
		size_t sEndAddr = reinterpret_cast<size_t>(&_poPeObject.SectionData()[_uiSectionIndex].vData[0]) + _poPeObject.SectionData()[_uiSectionIndex].vData.size();
		for ( uint32_t I = 0; I < uiTotal; ++I, ++pirdeEntry ) {
			// Check bounds.
			if ( reinterpret_cast<size_t>(pirdeEntry + 1) > sEndAddr ) {
				// Trying to read past the section.  No way to continue.
				break;
			}
			if ( !AddResourceTree( _tlTree, _tiParent, pirdeEntry, _uiSectionIndex, _poPeObject, _uiLevel ) ) { return false; }
		}

		return true;
	}

	// Decodes the resource tree.
	bool CPeWorksWindow::AddResourceTree( lsw::CTreeListView &_tlTree, HTREEITEM _tiParent, const MX_IMAGE_RESOURCE_DIRECTORY_ENTRY * _pirdeEntry, uint32_t _uiSectionIndex, const mx::CPeObject &_poPeObject, uint32_t _uiLevel ) {
#define MW_STANDARD_DATA_PRINT( STRUCTMEMBER )																									\
	MX_PRINT_FILE_OFFSET( wValue, uiOffset, pirdResDir->STRUCTMEMBER, swsTemp );																\
	if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }																\
	::wsprintf( wValue, L"%u", pirdResDir->STRUCTMEMBER );																						\
	if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_VALUE ) ) { return false; }																	\
	ssRaw.SecureClear();																														\
	CUtilities::MW_HEX_PRINT( &pirdResDir->STRUCTMEMBER, sizeof( pirdResDir->STRUCTMEMBER ), ssRaw );											\
	if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }


		uint64_t uiOffset = reinterpret_cast<size_t>(_pirdeEntry) - reinterpret_cast<size_t>(&_poPeObject.SectionData()[_uiSectionIndex].vData[0]);
		uiOffset += _poPeObject.SectionData()[_uiSectionIndex].ui64FileOffset;
		HTREEITEM hItem = NULL;

		WCHAR wValue[64];
		CSecureWString swsTemp;
		CSecureString ssCString;
		CSecureString ssRaw;
		{
			swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_D531786A_NameId__Data );
			TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), uiOffset, _tiParent );
			hItem = _tlTree.InsertItem( &isInsertMe );
			if ( !_tlTree.SetItemText( hItem, ((_pirdeEntry->NameId & 0x80000000) ?
				mx::CStringDecoder::DecodeToWString( _T_LEN_BA1310C1_Name_and_data_ ) :
				mx::CStringDecoder::DecodeToWString( _T_LEN_151FBE53_ID_and_data_ )).c_str(), MW_PE_DESC ) ) { return false; }

			
			ssCString.SecureClear();
			if ( _pirdeEntry->NameId & 0x80000000 ) {
				uiOffset = (_pirdeEntry->NameId & 0x7FFFFFFF) + _poPeObject.SectionData()[_uiSectionIndex].ui64FileOffset;
				ssCString += '\"';
				_poPeObject.GetSizedWideString( (_pirdeEntry->NameId & 0x7FFFFFFF) + _poPeObject.SectionData()[_uiSectionIndex].ui64RelocAddress, ssCString );
				ssCString += "\" (";
				CUtilities::ToHex( _pirdeEntry->NameId, ssCString, 8 );
				ssCString += ')';
			}
			else {
				uint16_t uiId = _pirdeEntry->NameId & 0xFFFF;
				if ( _uiLevel > 0 || !CUtilities::ResourceTypeToString( uiId, ssCString ) ) {
					CUtilities::ToUnsigned( uiId, ssCString );
				}
				else {
					ssCString += " (";
					CUtilities::ToUnsigned( uiId, ssCString );
					ssCString += ')';
				}
			}
			ssCString += ", ";
			CUtilities::ToHex( _pirdeEntry->Data, ssCString, 8 );

			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }


			ssRaw.SecureClear();
			CUtilities::MW_HEX_PRINT( &_pirdeEntry->NameId, sizeof( _pirdeEntry->NameId ), ssRaw );
			ssRaw += ',';
			ssRaw += ' ';
			CUtilities::MW_HEX_PRINT( &_pirdeEntry->Data, sizeof( _pirdeEntry->Data ), ssRaw );
			if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }


			MX_PRINT_FILE_OFFSET( wValue, uiOffset, MX_IMAGE_RESOURCE_DIRECTORY_ENTRY, swsTemp );
			if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }
		}

		if ( (_pirdeEntry->Data & 0x80000000) ) {
			// Points to an MX_IMAGE_RESOURCE_DIRECTORY.
			size_t sOffset = _pirdeEntry->Data & 0x7FFFFFFF;
			if ( sOffset + sizeof( MX_IMAGE_RESOURCE_DIRECTORY ) <= _poPeObject.SectionData()[_uiSectionIndex].vData.size() ) {
				const MX_IMAGE_RESOURCE_DIRECTORY * pirdResDir = reinterpret_cast<const MX_IMAGE_RESOURCE_DIRECTORY *>(&_poPeObject.SectionData()[_uiSectionIndex].vData[sOffset]);
				//AddResourceDesc( ptviTemp, (*pirdResDir), _poPeObject, sOffset + _poPeObject.SectionData()[_uiSectionIndex].ui64FileOffset );
				AddResourceTree( _tlTree, hItem, pirdResDir, _uiSectionIndex, _poPeObject, _uiLevel + 1 );
			}
		}
		else {
			size_t sOffset = _pirdeEntry->Data;
			if ( sOffset + sizeof( MX_IMAGE_RESOURCE_DATA_ENTRY ) <= _poPeObject.SectionData()[_uiSectionIndex].vData.size() ) {
				const MX_IMAGE_RESOURCE_DATA_ENTRY * pirdResDir = reinterpret_cast<const MX_IMAGE_RESOURCE_DATA_ENTRY *>(&_poPeObject.SectionData()[_uiSectionIndex].vData[sOffset]);
				uiOffset = _poPeObject.SectionData()[_uiSectionIndex].ui64FileOffset + sOffset;


				{
					swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_B4404A69_OffsetToData );
					TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), uiOffset, _tiParent );
					HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
					if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_805143DD_Data_pointer_ ).c_str(), MW_PE_DESC ) ) { return false; }

					MX_PRINT_FILE_OFFSET( wValue, uiOffset, pirdResDir->OffsetToData, swsTemp );
					if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }


					ssCString.SecureClear();
					_poPeObject.RelocAddressToStringwithSection( pirdResDir->OffsetToData, ssCString );
					if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
					ssRaw.SecureClear();
					CUtilities::MW_HEX_PRINT( &pirdResDir->OffsetToData, sizeof( pirdResDir->OffsetToData ), ssRaw );
					if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
				}

				{
					swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_57F28B54_Size );
					TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), sOffset, _tiParent );
					HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
					if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_469DC0E0_Size_of_the_resource_ ).c_str(), MW_PE_DESC ) ) { return false; }

					MW_STANDARD_DATA_PRINT( Size );
				}

				{
					swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_6CD26DB4_Code_page_ );
					TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), sOffset, _tiParent );
					HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
					
					MX_PRINT_FILE_OFFSET( wValue, uiOffset, pirdResDir->OffsetToData, swsTemp );
					if ( !_tlTree.SetItemText( hItem, wValue, MW_PE_OFFSET ) ) { return false; }


					ssCString.SecureClear();
					CSecureString sDesc;
					if ( CCodePages::CodePageToString( pirdResDir->CodePage, ssCString, sDesc ) ) {
						ssCString += " (";
						CUtilities::ToUnsigned( pirdResDir->CodePage, ssCString );
						ssCString += ')';
					}
					else {
						CUtilities::ToUnsigned( pirdResDir->CodePage, ssCString );
						sDesc = mx::CStringDecoder::DecodeToString( _T_LEN_6CD26DB4_Code_page_ );
					}
					if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssCString ).c_str(), MW_PE_VALUE ) ) { return false; }
					if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( sDesc ).c_str(), MW_PE_DESC ) ) { return false; }

					

					ssRaw.SecureClear();
					CUtilities::MW_HEX_PRINT( &pirdResDir->CodePage, sizeof( pirdResDir->CodePage ), ssRaw );
					if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( ssRaw ).c_str(), MW_PE_BYTES ) ) { return false; }
				}

				{
					swsTemp = mx::CStringDecoder::DecodeToWString( _T_LEN_B4D48994_Reserved );
					TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), sOffset, _tiParent );
					HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
					if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_F9026196_Reserved_ ).c_str(), MW_PE_DESC ) ) { return false; }

					MW_STANDARD_DATA_PRINT( Reserved );
				}

			}
		}

		return true;
#undef MW_STANDARD_DATA_PRINT
	}

	// Decodes an RT_VERSION resource.
	bool CPeWorksWindow::DecodeVersion( lsw::CTreeListView &_tlTree, HTREEITEM _tiParent, const CPeObject::MX_EXTRACTED_RESOURCE &_erResource, const mx::CPeObject &_poPeObject ) {
		CSecureWString swsTemp;
		
		// Print version information.
		struct MX_LANG_AND_CODEPAGE {
			WORD wLanguage;
			WORD wCodePage;
		} * placTranslate;
		if ( _erResource.vResourceData.size() ) {
			{
				swsTemp = CreateResourceNameString( _erResource, _poPeObject );
				TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), -1, _tiParent );
				_tiParent = _tlTree.InsertItem( &isInsertMe );
				//if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_805143DD_Data_pointer_ ).c_str(), MW_PE_DESC ) ) { return false; }
			}
			
			UINT uiTranLen = 0;
			if ( ::VerQueryValueW( &_erResource.vResourceData[0], 
				mx::CStringDecoder::DecodeToWString( _T_LEN_0B68E21B___VarFileInfo__Translation ).c_str(),
				reinterpret_cast<LPVOID *>(&placTranslate),
				&uiTranLen ) ) {

				for ( size_t J = 0; J < (uiTranLen / sizeof( MX_LANG_AND_CODEPAGE )); J++ ) {
					UINT uiFinalLen = 0;
					wchar_t * pwcResult;
					swsTemp.SecureClear();
					swsTemp = CreateStringFileInfoString( placTranslate[J].wLanguage, placTranslate[J].wCodePage, _T_LEN_A6E8F47C_Comments );
					if ( ::VerQueryValueW( &_erResource.vResourceData[0],
						swsTemp.c_str(),
						reinterpret_cast<LPVOID *>(&pwcResult),
						&uiFinalLen ) ) {
						TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), -1, _tiParent );
						HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
						if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_5E06B2C2_Additional_information_that_should_be_displayed_for_diagnostic_purposes_ ).c_str(), MW_PE_DESC ) ) { return false; }
						if ( !_tlTree.SetItemText( hItem, pwcResult, MW_PE_VALUE ) ) { return false; }
					}
					swsTemp.SecureClear();
					swsTemp = CreateStringFileInfoString( placTranslate[J].wLanguage, placTranslate[J].wCodePage, _T_LEN_3115C37D_CompanyName );
					if ( ::VerQueryValueW( &_erResource.vResourceData[0],
						swsTemp.c_str(),
						reinterpret_cast<LPVOID *>(&pwcResult),
						&uiFinalLen ) ) {
						TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), -1, _tiParent );
						HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
						if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_50BF2458_Company_that_produced_the_file___for_example____Microsoft_Corporation___or___Standard_Microsystems_Corporation__Inc____This_string_is_required_ ).c_str(), MW_PE_DESC ) ) { return false; }
						if ( !_tlTree.SetItemText( hItem, pwcResult, MW_PE_VALUE ) ) { return false; }
					}
					swsTemp.SecureClear();
					swsTemp = CreateStringFileInfoString( placTranslate[J].wLanguage, placTranslate[J].wCodePage, _T_LEN_B91EDAA1_FileDescription );
					if ( ::VerQueryValueW( &_erResource.vResourceData[0],
						swsTemp.c_str(),
						reinterpret_cast<LPVOID *>(&pwcResult),
						&uiFinalLen ) ) {
						TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), -1, _tiParent );
						HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
						if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_56BE8472_File_description_to_be_presented_to_users__This_string_may_be_displayed_in_a_list_box_when_the_user_is_choosing_files_to_install___for_example____Keyboard_Driver_for_AT_Style_Keyboards____This_string_is_required_ ).c_str(), MW_PE_DESC ) ) { return false; }
						if ( !_tlTree.SetItemText( hItem, pwcResult, MW_PE_VALUE ) ) { return false; }
					}
					swsTemp.SecureClear();
					swsTemp = CreateStringFileInfoString( placTranslate[J].wLanguage, placTranslate[J].wCodePage, _T_LEN_B719A4B4_FileVersion );
					if ( ::VerQueryValueW( &_erResource.vResourceData[0],
						swsTemp.c_str(),
						reinterpret_cast<LPVOID *>(&pwcResult),
						&uiFinalLen ) ) {
						TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), -1, _tiParent );
						HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
						if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_C2754D48_Version_number_of_the_file___for_example____3_10___or___5_00_RC2____This_string_is_required_ ).c_str(), MW_PE_DESC ) ) { return false; }
						if ( !_tlTree.SetItemText( hItem, pwcResult, MW_PE_VALUE ) ) { return false; }
					}
					swsTemp.SecureClear();
					swsTemp = CreateStringFileInfoString( placTranslate[J].wLanguage, placTranslate[J].wCodePage, _T_LEN_4828FEFB_InternalName );
					if ( ::VerQueryValueW( &_erResource.vResourceData[0],
						swsTemp.c_str(),
						reinterpret_cast<LPVOID *>(&pwcResult),
						&uiFinalLen ) ) {
						TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), -1, _tiParent );
						HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
						if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_3851A6C7_Internal_name_of_the_file__if_one_exists___for_example__a_module_name_if_the_file_is_a_dynamic_link_library__If_the_file_has_no_internal_name__this_string_should_be_the_original_filename__without_extension__This_string_is_required_ ).c_str(), MW_PE_DESC ) ) { return false; }
						if ( !_tlTree.SetItemText( hItem, pwcResult, MW_PE_VALUE ) ) { return false; }
					}
					swsTemp.SecureClear();
					swsTemp = CreateStringFileInfoString( placTranslate[J].wLanguage, placTranslate[J].wCodePage, _T_LEN_F319DD29_LegalCopyright );
					if ( ::VerQueryValueW( &_erResource.vResourceData[0],
						swsTemp.c_str(),
						reinterpret_cast<LPVOID *>(&pwcResult),
						&uiFinalLen ) ) {
						TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), -1, _tiParent );
						HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
						if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_0D1FCEC4_Copyright_notices_that_apply_to_the_file__This_should_include_the_full_text_of_all_notices__legal_symbols__copyright_dates__and_so_on__This_string_is_optional_ ).c_str(), MW_PE_DESC ) ) { return false; }
						if ( !_tlTree.SetItemText( hItem, pwcResult, MW_PE_VALUE ) ) { return false; }
					}
					swsTemp.SecureClear();
					swsTemp = CreateStringFileInfoString( placTranslate[J].wLanguage, placTranslate[J].wCodePage, _T_LEN_65795EB1_LegalTrademarks );
					if ( ::VerQueryValueW( &_erResource.vResourceData[0],
						swsTemp.c_str(),
						reinterpret_cast<LPVOID *>(&pwcResult),
						&uiFinalLen ) ) {
						TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), -1, _tiParent );
						HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
						if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_5BD3D97F_Trademarks_and_registered_trademarks_that_apply_to_the_file__This_should_include_the_full_text_of_all_notices__legal_symbols__trademark_numbers__and_so_on__This_string_is_optional_ ).c_str(), MW_PE_DESC ) ) { return false; }
						if ( !_tlTree.SetItemText( hItem, pwcResult, MW_PE_VALUE ) ) { return false; }
					}
					swsTemp.SecureClear();
					swsTemp = CreateStringFileInfoString( placTranslate[J].wLanguage, placTranslate[J].wCodePage, _T_LEN_208ABFC4_OriginalFilename );
					if ( ::VerQueryValueW( &_erResource.vResourceData[0],
						swsTemp.c_str(),
						reinterpret_cast<LPVOID *>(&pwcResult),
						&uiFinalLen ) ) {
						TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), -1, _tiParent );
						HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
						if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_5572F2DA_Original_name_of_the_file__not_including_a_path__This_information_enables_an_application_to_determine_whether_a_file_has_been_renamed_by_a_user__The_format_of_the_name_depends_on_the_file_system_for_which_the_file_was_created__This_string_is_required_ ).c_str(), MW_PE_DESC ) ) { return false; }
						if ( !_tlTree.SetItemText( hItem, pwcResult, MW_PE_VALUE ) ) { return false; }
					}
					swsTemp.SecureClear();
					swsTemp = CreateStringFileInfoString( placTranslate[J].wLanguage, placTranslate[J].wCodePage, _T_LEN_A9B70282_PrivateBuild );
					if ( ::VerQueryValueW( &_erResource.vResourceData[0],
						swsTemp.c_str(),
						reinterpret_cast<LPVOID *>(&pwcResult),
						&uiFinalLen ) ) {
						TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), -1, _tiParent );
						HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
						if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_4F883DC5_Information_about_a_private_version_of_the_file___for_example____Built_by_TESTER1_on___TESTBED____This_string_should_be_present_only_if_VS_FF_PRIVATEBUILD_is_specified_in_the_fileflags_parameter_of_the_root_block_ ).c_str(), MW_PE_DESC ) ) { return false; }
						if ( !_tlTree.SetItemText( hItem, pwcResult, MW_PE_VALUE ) ) { return false; }
					}
					swsTemp.SecureClear();
					swsTemp = CreateStringFileInfoString( placTranslate[J].wLanguage, placTranslate[J].wCodePage, _T_LEN_D5C9130A_ProductName );
					if ( ::VerQueryValueW( &_erResource.vResourceData[0],
						swsTemp.c_str(),
						reinterpret_cast<LPVOID *>(&pwcResult),
						&uiFinalLen ) ) {
						TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), -1, _tiParent );
						HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
						if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_580B70A2_Name_of_the_product_with_which_the_file_is_distributed__This_string_is_required_ ).c_str(), MW_PE_DESC ) ) { return false; }
						if ( !_tlTree.SetItemText( hItem, pwcResult, MW_PE_VALUE ) ) { return false; }
					}
					swsTemp.SecureClear();
					swsTemp = CreateStringFileInfoString( placTranslate[J].wLanguage, placTranslate[J].wCodePage, _T_LEN_268A34A0_ProductVersion );
					if ( ::VerQueryValueW( &_erResource.vResourceData[0],
						swsTemp.c_str(),
						reinterpret_cast<LPVOID *>(&pwcResult),
						&uiFinalLen ) ) {
						TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), -1, _tiParent );
						HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
						if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_8CF8F15D_Version_of_the_product_with_which_the_file_is_distributed___for_example____3_10___or___5_00_RC2____This_string_is_required_ ).c_str(), MW_PE_DESC ) ) { return false; }
						if ( !_tlTree.SetItemText( hItem, pwcResult, MW_PE_VALUE ) ) { return false; }
					}
					swsTemp.SecureClear();
					swsTemp = CreateStringFileInfoString( placTranslate[J].wLanguage, placTranslate[J].wCodePage, _T_LEN_BB227717_SpecialBuild );
					if ( ::VerQueryValueW( &_erResource.vResourceData[0],
						swsTemp.c_str(),
						reinterpret_cast<LPVOID *>(&pwcResult),
						&uiFinalLen ) ) {
						TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), -1, _tiParent );
						HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
						if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_1BF0CE20_Text_that_specifies_how_this_version_of_the_file_differs_from_the_standard_version___for_example____Private_build_for_TESTER1_solving_mouse_problems_on_M250_and_M250E_computers____This_string_should_be_present_only_if_VS_FF_SPECIALBUILD_is_specified_in_the_fileflags_parameter_of_the_root_block_ ).c_str(), MW_PE_DESC ) ) { return false; }
						if ( !_tlTree.SetItemText( hItem, pwcResult, MW_PE_VALUE ) ) { return false; }
					}
				}
			}
		}
		return true;
	}

	// Creates a StringFileInfo\LAND_CODEPAGE\%S string.
	CSecureWString CPeWorksWindow::CreateStringFileInfoString( WORD _wLang, WORD _wCodePage, const char * _pcEncodedString, size_t _sLen ) {
		std::vector<wchar_t> vBuffer;
		try { vBuffer.resize( _T_MAX_LEN * 3 ); }
		catch ( const std::bad_alloc /*& _eE*/ ) { return CSecureWString(); }

		CSecureWString swsFormat = _DEC_WS_E8883E36___StringFileInfo + _DEC_WS_CE7D2016____04x_04x__ + mx::CStringDecoder::DecodeToWString( _pcEncodedString, _sLen );
		if ( SUCCEEDED( ::StringCchPrintfW( &vBuffer[0], vBuffer.size(),
			(_DEC_WS_E8883E36___StringFileInfo + _DEC_WS_CE7D2016____04x_04x__ + mx::CStringDecoder::DecodeToWString( _pcEncodedString, _sLen )).c_str(),
			_wLang,
			_wCodePage ) ) ) {
			CSecureWString swRet = &vBuffer[0];
			for ( size_t I = vBuffer.size(); I--; ) {
				vBuffer[I] = 0;
			}
			return swRet;
		}
		return CSecureWString();
	}

	// Decodes an RT_MANIFEST resource.
	bool CPeWorksWindow::DecodeManifest( lsw::CTreeListView &_tlTree, HTREEITEM _tiParent, const CPeObject::MX_EXTRACTED_RESOURCE &_erResource, const mx::CPeObject &_poPeObject ) {
		if ( _erResource.vResourceData.size() ) {
			CSecureWString swsTemp;
			{
				swsTemp = CreateResourceNameString( _erResource, _poPeObject );
				TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), -1, _tiParent );
				_tiParent = _tlTree.InsertItem( &isInsertMe );
				//if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_805143DD_Data_pointer_ ).c_str(), MW_PE_DESC ) ) { return false; }
			}
			swsTemp.SecureClear();
			std::vector<CSecureString> vLines = CUtilities::StringToLines( std::string( reinterpret_cast<const char *>(&_erResource.vResourceData[0]), _erResource.vResourceData.size() ) );
			for ( size_t I = 0; I < vLines.size(); ++I ) {

				TVINSERTSTRUCTW isInsertMe = lsw::CTreeListView::DefaultItemLParam( swsTemp.c_str(), -1, _tiParent );
				HTREEITEM hItem = _tlTree.InsertItem( &isInsertMe );
				//if ( !_tlTree.SetItemText( hItem, mx::CStringDecoder::DecodeToWString( _T_LEN_50BF2458_Company_that_produced_the_file___for_example____Microsoft_Corporation___or___Standard_Microsystems_Corporation__Inc____This_string_is_required_ ).c_str(), MW_PE_DESC ) ) { return false; }
				if ( !_tlTree.SetItemText( hItem, ee::CExpEval::StringToWString( vLines[I] ).c_str(), MW_PE_VALUE ) ) { return false; }
			}
		}
		return true;

	}

	// Creates a resource name string given a directory stack.
	CSecureWString CPeWorksWindow::CreateResourceNameString( const CPeObject::MX_EXTRACTED_RESOURCE &_erResource, const mx::CPeObject &_poPeObject ) {
		CSecureString ssCString;

		ssCString.SecureClear();
		for ( size_t I = 0; I < _erResource.vDirEntries.size(); ++I ) {
			if ( _erResource.vDirEntries[I].NameId & 0x80000000 ) {
				uint32_t uiOffset;
				uint32_t uiIndex = _poPeObject.RelocAddrToRelocIndexAndOffset( _poPeObject.DataDirectory()[MX_IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress, uiOffset );
				//uiOffset = (_erResource.vDirEntries[I].NameId & 0x7FFFFFFF) + _poPeObject.SectionData()[_uiSectionIndex].ui64FileOffset;
				ssCString += '\"';
				_poPeObject.GetSizedWideString( (_erResource.vDirEntries[I].NameId & 0x7FFFFFFF) + _poPeObject.SectionData()[uiIndex].ui64RelocAddress, ssCString );
				ssCString += "\" (";
				CUtilities::ToHex( _erResource.vDirEntries[I].NameId, ssCString, 8 );
				ssCString += ')';
			}
			else {
				uint16_t uiId = _erResource.vDirEntries[I].NameId & 0xFFFF;
				if ( I > 0 || !CUtilities::ResourceTypeToString( uiId, ssCString ) ) {
					CUtilities::ToUnsigned( uiId, ssCString );
				}
				else {
					ssCString += " (";
					CUtilities::ToUnsigned( uiId, ssCString );
					ssCString += ')';
				}
			}
			ssCString += ", ";
			CUtilities::ToHex( _erResource.vDirEntries[I].Data, ssCString, 8 );

			ssCString += "\\";
		}
		return ee::CExpEval::StringToWString( ssCString );
	}

	// WM_CONTEXTMENU.
	CWidget::LSW_HANDLED CPeWorksWindow::ContextMenu( CWidget * _pwControl, INT _iX, INT _iY ) {
		if ( _pwControl->Id() == CPeWorksLayout::MX_PEW_TABS ) {
			bool bExpSel = CurTabAnySelectedHasUnexpandedChildren();
			bool bExpAll = CurTabAnyHasUnexpandedChildren();
			bool bColSel = CurTabAnySelectedHasExpandedChildren();
			bool bColAll = CurTabAnyHasExpandedChildren();
			LSW_MENU_ITEM miMenuBar[] = {
				//bIsSeperator	dwId													bCheckable	bChecked	bEnabled	lpwcText, stTextLen												bSkip
				{ FALSE,		CPeWorksLayout::MX_PEW_COPY_FIELD,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_05239A67_Copy__Field, _LEN_05239A67 ),			FALSE },
				{ FALSE,		CPeWorksLayout::MX_PEW_COPY_VALUE,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_43A1870B_Copy__Value, _LEN_43A1870B ),			FALSE },
				{ FALSE,		CPeWorksLayout::MX_PEW_COPY_OFFSET,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_EF323132_Copy__Offset, _LEN_EF323132 ),			FALSE },
				{ FALSE,		CPeWorksLayout::MX_PEW_COPY_BYTES,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_C928AB0C_Copy__Raw_Bytes, _LEN_C928AB0C ),		FALSE },
				{ FALSE,		CPeWorksLayout::MX_PEW_COPY_DESC,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_03939D2C_Copy__Desc, _LEN_03939D2C ),			FALSE },
				{ FALSE,		CPeWorksLayout::MX_PEW_COPY_ALL,						FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_9B7D368F_Copy_A_ll, _LEN_9B7D368F ),			FALSE },
				{ FALSE,		CPeWorksLayout::MX_PEW_COPY_FIELD_VALUE,				FALSE,		FALSE,		TRUE,		MW_MENU_TXT( _T_E9B5F487_Copy_F_ield__Value, _LEN_E9B5F487 ),	FALSE },
				{ TRUE,			0,														FALSE,		FALSE,		TRUE,		nullptr,														FALSE },
				{ FALSE,		CPeWorksLayout::MX_PEW_COPY_EXPAND_SELECTED,			FALSE,		FALSE,		bExpSel,	MW_MENU_TXT( _T_BBEDF334_Expand_Selec_ted, _LEN_BBEDF334 ),		FALSE },
				{ FALSE,		CPeWorksLayout::MX_PEW_COPY_EXPAND_ALL,					FALSE,		FALSE,		bExpAll,	MW_MENU_TXT( _T_465B3E6C_E_xpand_All, _LEN_465B3E6C ),			FALSE },
				{ FALSE,		CPeWorksLayout::MX_PEW_COPY_COLLAPSE_SELECTED,			FALSE,		FALSE,		bColSel,	MW_MENU_TXT( _T_89E206F0_C_ollapse_Selected, _LEN_89E206F0 ),	FALSE },
				{ FALSE,		CPeWorksLayout::MX_PEW_COPY_COLLAPSE_ALL,				FALSE,		FALSE,		bColAll,	MW_MENU_TXT( _T_99D9F557__Collapse_All, _LEN_99D9F557 ),		FALSE },
			};

			const LSW_MENU_LAYOUT miMenus[] = {
				{
					MX_M_CONTEXT_MENU,
					0,
					0,
					MX_ELEMENTS( miMenuBar ),
					miMenuBar
				},
				/*{
					MX_MWMI_MENU_FILE,
					MX_MWMI_MENU_BAR,
					MX_MWMI_FILE,
					MX_ELEMENTS( m_miFileMenu ),
					m_miFileMenu
				},*/
			};
			mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
			plmLayout->CreatePopupMenuEx( this, miMenus, MX_ELEMENTS( miMenus ), _iX, _iY );
		}
		return lsw::CMainWindow::ContextMenu( _pwControl, _iX, _iY );
	}

	// Informs tha a child tab closed one of it headers.
	void CPeWorksWindow::ChildTabClosed( CWidget * _pwChild, INT _iTab ) {
		//CTab * ptTab = reinterpret_cast<CTab *>(_pwChild);
		if ( _iTab < m_vTabs.size() ) {
			delete m_vTabs[_iTab].ppoPeObject;
			//delete m_vTabs[_iTab].ptWidget;	// Deleted by the tab.
			m_vTabs.erase( m_vTabs.begin() + _iTab );
		}
	}

	// Copies all of the given field in the current tab.
	bool CPeWorksWindow::CopyAny( INT _iIdx, bool _bCopyHidden ) {
		CTab * ptTab = GetTab();
		if ( !ptTab ) { return false; }

		int iSel = ptTab->GetCurSel();
		if ( iSel >= m_vTabs.size() ) { return false; }
		if ( !m_vTabs[iSel].ptWidget ) { return false; }
		std::vector<HTREEITEM> vItems;
		m_vTabs[iSel].ptWidget->GatherSelected( vItems, _bCopyHidden );
		CSecureWString swsString, swsTmp;
		for ( size_t I = 0; I < vItems.size(); ++I ) {
			if ( !m_vTabs[iSel].ptWidget->GetItemText( vItems[I], _iIdx, swsTmp ) ) { return false; }
			if ( swsString.size() ) {
				swsString.push_back( L'\r' );
				swsString.push_back( L'\n' );
			}
			swsString.append( swsTmp );
		}

		if ( swsString.size() ) {
			lsw::LSW_CLIPBOARD cbClipBoard( Wnd(), true );
			cbClipBoard.SetText( swsString.c_str(), swsString.size() );
		}
		return true;
	}

	// Copies all of the Field fields in the current tab.
	bool CPeWorksWindow::CopyField( bool _bCopyHidden ) {
		return CopyAny( MW_PE_FIELD, _bCopyHidden );
	}

	// Copies all of the Value fields in the current tab.
	bool CPeWorksWindow::CopyValue( bool _bCopyHidden ) {
		return CopyAny( MW_PE_VALUE, _bCopyHidden );
	}

	// Copies all of the Offset fields in the current tab.
	bool CPeWorksWindow::CopyOffset( bool _bCopyHidden ) {
		return CopyAny( MW_PE_OFFSET, _bCopyHidden );
	}

	// Copies all of the Raw Bytes fields in the current tab.
	bool CPeWorksWindow::CopyBytes( bool _bCopyHidden ) {
		return CopyAny( MW_PE_BYTES, _bCopyHidden );
	}

	// Copies all of the Description fields in the current tab.
	bool CPeWorksWindow::CopyDesc( bool _bCopyHidden ) {
		return CopyAny( MW_PE_DESC, _bCopyHidden );
	}

	// Copies all of the fields in the current tab.
	bool CPeWorksWindow::CopyAll( bool _bCopyHidden ) {
		CTab * ptTab = GetTab();
		if ( !ptTab ) { return false; }

		if ( ptTab->GetCurSel() >= m_vTabs.size() ) { return false; }
		if ( !m_vTabs[ptTab->GetCurSel()].ptWidget ) { return false; }
		std::vector<HTREEITEM> vItems;
		m_vTabs[ptTab->GetCurSel()].ptWidget->GatherSelected( vItems, _bCopyHidden );
		CSecureWString swsString, swsTmp;
		for ( size_t I = 0; I < vItems.size(); ++I ) {
			if ( swsString.size() ) {
				swsString.push_back( L'\r' );
				swsString.push_back( L'\n' );
			}
			if ( !m_vTabs[ptTab->GetCurSel()].ptWidget->GetItemText( vItems[I], MW_PE_FIELD, swsTmp ) ) { return false; }
			swsString.append( swsTmp );

			swsString.push_back( L'\t' );
			if ( !m_vTabs[ptTab->GetCurSel()].ptWidget->GetItemText( vItems[I], MW_PE_VALUE, swsTmp ) ) { return false; }
			swsString.append( swsTmp );

			swsString.push_back( L'\t' );
			if ( !m_vTabs[ptTab->GetCurSel()].ptWidget->GetItemText( vItems[I], MW_PE_OFFSET, swsTmp ) ) { return false; }
			swsString.append( swsTmp );

			swsString.push_back( L'\t' );
			if ( !m_vTabs[ptTab->GetCurSel()].ptWidget->GetItemText( vItems[I], MW_PE_BYTES, swsTmp ) ) { return false; }
			swsString.append( swsTmp );

			swsString.push_back( L'\t' );
			if ( !m_vTabs[ptTab->GetCurSel()].ptWidget->GetItemText( vItems[I], MW_PE_DESC, swsTmp ) ) { return false; }
			swsString.append( swsTmp );
		}

		if ( swsString.size() ) {
			lsw::LSW_CLIPBOARD cbClipBoard( Wnd(), true );
			cbClipBoard.SetText( swsString.c_str(), swsString.size() );
		}
		return true;
	}

	// Copies all of the Field and Value fields in the format "Field: Format".
	bool CPeWorksWindow::CopyFieldValue( bool _bCopyHidden ) {
		CTab * ptTab = GetTab();
		if ( !ptTab ) { return false; }

		if ( ptTab->GetCurSel() >= m_vTabs.size() ) { return false; }
		if ( !m_vTabs[ptTab->GetCurSel()].ptWidget ) { return false; }
		std::vector<HTREEITEM> vItems;
		m_vTabs[ptTab->GetCurSel()].ptWidget->GatherSelected( vItems, _bCopyHidden );
		CSecureWString swsString, swsTmp;
		for ( size_t I = 0; I < vItems.size(); ++I ) {
			if ( swsString.size() ) {
				swsString.push_back( L'\r' );
				swsString.push_back( L'\n' );
			}
			if ( !m_vTabs[ptTab->GetCurSel()].ptWidget->GetItemText( vItems[I], MW_PE_FIELD, swsTmp ) ) { return false; }
			swsString.append( swsTmp );

			swsString.push_back( L':' );
			swsString.push_back( L'\t' );
			if ( !m_vTabs[ptTab->GetCurSel()].ptWidget->GetItemText( vItems[I], MW_PE_VALUE, swsTmp ) ) { return false; }
			swsString.append( swsTmp );
		}

		if ( swsString.size() ) {
			lsw::LSW_CLIPBOARD cbClipBoard( Wnd(), true );
			cbClipBoard.SetText( swsString.c_str(), swsString.size() );
		}
		return true;
	}

}	// namespace mx


#undef MX_PRINT_FILE_OFFSET
#undef MW_PE_FIELD
#undef MW_PE_VALUE
#undef MW_PE_OFFSET
#undef MW_PE_BYTES
#undef MW_PE_DESC

#undef MW_HEX_PRINT
