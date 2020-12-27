#pragma once

#include "../MXMhsX.h"
#include "../Strings/MXStringDecoder.h"
#include "MXPeObject.h"
#include "MXPeStructs.h"
#include <ImageList/LSWImageList.h>
#include <Images/LSWBitmap.h>
#include <LSWWin.h>
#include <MainWindow/LSWMainWindow.h>
#include <StatusBar/LSWStatusBar.h>
#include <Tab/LSWTab.h>
#include <TreeList/LSWTreeList.h>

using namespace lsw;

namespace mx {

	class CPeWorksWindow : public lsw::CMainWindow {
	public :
		CPeWorksWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CPeWorksWindow();


		// == Enumerations.
		// Images.
		enum MX_IMAGES {
			MX_I_OPENFILE,

			MX_I_TOTAL,
		};


		// == Functions.
		// Loads a file.
		bool								LoadFile( const wchar_t * _pwcFile );

		// Gets the base tab control.
		CTab *								GetTab() const;

		// WM_INITDIALOG.
		virtual LSW_HANDLED					InitDialog();

		// WM_COMMAND from control.
		virtual LSW_HANDLED					Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc );

		// WM_COMMAND from menu.
		virtual LSW_HANDLED					MenuCommand( WORD _Id ) { return Command( 0, _Id, nullptr ); }

		// WM_CLOSE.
		virtual LSW_HANDLED					Close();

		// WM_ERASEBKGND.
		virtual LSW_HANDLED					EraseBkgnd( HDC _hDc );

		// Translate a child's tooltip text.
		virtual std::wstring				TranslateTooltip( const std::string &_sText ) { 
			return CStringDecoder::DecodeToWString( _sText.c_str(), _sText.size() );
		}

		// Gets the status bar.
		CStatusBar *						StatusBar();

		// Gets the status bar.
		const CStatusBar *					StatusBar() const;

		// Virtual client rectangle.  Can be used for things that need to be adjusted based on whether or not status bars, toolbars, etc. are present.
		virtual const LSW_RECT				VirtualClientRect( const CWidget * pwChild ) const;

		// Prepares to create the window.  Creates the atom if necessary.
		static void							PreparePeWorks();

		// Closes down the PE Works library.
		static void							ShutDown();


	protected :
		// == Types.
		// A tab.
		struct MX_TAB {
			CTreeList *						ptWidget;				// The CTreeList pointer.
			CPeObject *						ppoPeObject;			// The PE file.
		};

		// == Members.
		// Image list.
		CImageList							m_iImages;
		// Images.
		CBitmap								m_bBitmaps[MX_I_TOTAL];
		// Image mapping.
		INT									m_iImageMap[MX_I_TOTAL];
		// Tabs.
		std::vector<MX_TAB>					m_vTabs;

		// The main window class.
		static ATOM							m_aAtom;


		// == Functions.
		// Fills a tree-list with the accosiated PE object's information.
		bool								FillTree( MX_TAB &_tTab );

		// Decodes a DOS header to a given parent.
		bool								AddDosHeader( lsw::CTreeList &_tlTree, HTREEITEM _tiParent, const MX_DOS_HEADER &_dhHeader, uint64_t _uiStructOffset );

		// Decodes a DOS stub to a given parent.
		bool								AddDosStub( lsw::CTreeList &_tlTree, HTREEITEM _tiParent, const std::vector<uint8_t> &_vStub, uint64_t _uiStructOffset );

		// Decodes a COFF header to a given parent.
		bool								AddCoffHeader( lsw::CTreeList &_tlTree, HTREEITEM _tiParent, const MX_COFF_HEADER &_chHeader, uint64_t _uiStructOffset );

		// Decodes a PE header to a given parent.
		bool								AddOptHeader( lsw::CTreeList &_tlTree, HTREEITEM _tiParent, const MX_COFF_WINDOW_OPT &_cwoHeader, const mx::CPeObject &_poPeObject, uint64_t _uiImageBase, uint64_t _uiStructOffset );

		// Decodes a PE 32 header to a given parent.
		bool								AddPe32Header( lsw::CTreeList &_tlTree, HTREEITEM _tiParent, const MX_COFF_WINDOWS_PE32 &_cwpHeader, uint64_t _uiStructOffset );

		// Decodes a PE 64 header to a given parent.
		bool								AddPe64Header( lsw::CTreeList &_tlTree, HTREEITEM _tiParent, const MX_COFF_WINDOWS_PE64 &_cwpHeader, uint64_t _uiStructOffset );

		// Decodes an array of data directories.
		bool								AddDataDirectories( lsw::CTreeList &_tlTree, HTREEITEM _tiParent, const std::vector<MX_DATA_DIRECTORY> &_vDataDirs, const mx::CPeObject &_poPeObject, uint64_t _uiImageBase, uint64_t _uiStructOffset );

		// Decodes a data directory.
		bool								AddDataDirectory( lsw::CTreeList &_tlTree, HTREEITEM _tiParent, const MX_DATA_DIRECTORY &_ddDir, const mx::CPeObject &_poPeObject, uint64_t _uiImageBase, uint64_t _uiStructOffset );

		// Decodes an array of MX_IMAGE_SECTION_HEADER objects.
		bool								AddSectionHeaders( lsw::CTreeList &_tlTree, HTREEITEM _tiParent, const std::vector<MX_IMAGE_SECTION_HEADER> &_vHeaders, uint64_t _uiImageBase, uint64_t _uiStructOffset );

		// Decodes an MX_IMAGE_SECTION_HEADER object.
		bool								AddSectionHeader( lsw::CTreeList &_tlTree, HTREEITEM _tiParent, const MX_IMAGE_SECTION_HEADER &_ishHeader, uint64_t _uiImageBase, uint64_t _uiStructOffset );

		// Decodes exports.
		bool								AddExports( lsw::CTreeList &_tlTree, HTREEITEM _tiParent, const mx::CPeObject &_poPeObject );

		// Decodes an MX_IMAGE_EXPORT_DIRECTORY object.
		bool								AddExportDesc( lsw::CTreeList &_tlTree, HTREEITEM _tiParent, const mx::CPeObject &_poPeObject );

		// Decodes an MX_IMAGE_IMPORT_DESCRIPTOR object.
		bool								AddImportDesc( lsw::CTreeList &_tlTree, HTREEITEM _tiParent, const MX_IMAGE_IMPORT_DESCRIPTOR &_iidDesc, const mx::CPeObject &_poPeObject, uint64_t _uiStructOffset );

		// Decodes an MX_IMAGE_IMPORT_DESCRIPTOR object.
		bool								AddResourceDesc( lsw::CTreeList &_tlTree, HTREEITEM _tiParent, const MX_IMAGE_RESOURCE_DIRECTORY &_irdDesc, const mx::CPeObject &_poPeObject, uint64_t _uiStructOffset );

		// Decodes the relocation table.
		bool								AddReloc( lsw::CTreeList &_tlTree, HTREEITEM _tiParent, const std::vector<CPeObject::MX_BASE_RELOC> &_vRelocs, const mx::CPeObject &_poPeObject );

		// Decodes the relocation offsets.
		bool								AddReloc( lsw::CTreeList &_tlTree, HTREEITEM _tiParent, const CPeObject::MX_BASE_RELOC &_brRelocs, const mx::CPeObject &_poPeObject, uint64_t _uiStructOffset );

		// Decodes exported functions.
		bool								AddExportedFunctions( lsw::CTreeList &_tlTree, HTREEITEM _tiParent, const mx::CPeObject &_poPeObject );

		// Decodes exported functions.
		bool								AddExportedFunctions( lsw::CTreeList &_tlTree, HTREEITEM _tiParent, const mx::CPeObject &_poPeObject, const std::vector<CPeObject::MX_EXPORT> &_vList );

		// Decodes all imports.
		bool								AddImportedFuncs( lsw::CTreeList &_tlTree, HTREEITEM _tiParent, const mx::CPeObject &_poPeObject );

		// Decodes imports from a given import directory.
		bool								AddImportedFuncs( lsw::CTreeList &_tlTree, HTREEITEM _tiParent, uint32_t _uiImportDir, const mx::CPeObject &_poPeObject );

		// Decodes an import.
		bool								AddImport( lsw::CTreeList &_tlTree, HTREEITEM _tiParent, const CPeObject::MX_IMPORT &_iImport, const mx::CPeObject &_poPeObject );

		// Decodes the resource tree.
		bool								AddResourceTree( lsw::CTreeList &_tlTree, HTREEITEM _tiParent, const mx::CPeObject &_poPeObject );

		// Decodes the resource tree.
		bool								AddResourceTree( lsw::CTreeList &_tlTree, HTREEITEM _tiParent, const MX_IMAGE_RESOURCE_DIRECTORY * _pirdDesc, uint32_t _uiSectionIndex, const mx::CPeObject &_poPeObject, uint32_t _uiLevel );

		// Decodes the resource tree.
		bool								AddResourceTree( lsw::CTreeList &_tlTree, HTREEITEM _tiParent, const MX_IMAGE_RESOURCE_DIRECTORY_ENTRY * _pirdeEntry, uint32_t _uiSectionIndex, const mx::CPeObject &_poPeObject, uint32_t _uiLevel );

		// Decodes an RT_VERSION resource.
		bool								DecodeVersion( lsw::CTreeList &_tlTree, HTREEITEM _tiParent, const CPeObject::MX_EXTRACTED_RESOURCE &_erResource, const mx::CPeObject &_poPeObject );

		// Creates a StringFileInfo\LAND_CODEPAGE\%S string.
		CSecureWString						CreateStringFileInfoString( WORD _wLang, WORD _wCodePage, const char * _pcEncodedString, size_t _sLen );

		// Decodes an RT_MANIFEST resource.
		bool								DecodeManifest( lsw::CTreeList &_tlTree, HTREEITEM _tiParent, const CPeObject::MX_EXTRACTED_RESOURCE &_erResource, const mx::CPeObject &_poPeObject );

		// Creates a resource name string given a directory stack.
		CSecureWString						CreateResourceNameString( const CPeObject::MX_EXTRACTED_RESOURCE &_erResource, const mx::CPeObject &_poPeObject );
	};

}	// namespace mx
