#pragma once

#include "../TreeViews/MXTreeModel.h"
#include "MXPeObject.h"

namespace mx {

	class CPeTreeModel : public CTreeModel {
	public :
		explicit CPeTreeModel( const void * _pvData, QObject * _poParent = nullptr );
		explicit CPeTreeModel( const QList<QVariant> &_lRootData, const void * _pvData, QObject * _poParent = nullptr );
		~CPeTreeModel();

		// == Functions.
		// Create the tree given a mx::CPeObject object.
		BOOL						CreateTree( const mx::CPeObject &_poPeObject );


	protected :
		// == Functions.
		// Decodes a DOS header to a given parent.
		VOID						AddDosHeader( CTreeViewItem * _ptviParent, const MX_DOS_HEADER &_dhHeader, uint64_t _uiStructOffset );

		// Decodes a DOS stub to a given parent.
		VOID						AddDosStub( CTreeViewItem * _ptviParent, const std::vector<uint8_t> &_vStub, uint64_t _uiStructOffset );

		// Decodes a COFF header to a given parent.
		VOID						AddCoffHeader( CTreeViewItem * _ptviParent, const MX_COFF_HEADER &_chHeader, uint64_t _uiStructOffset );

		// Decodes a PE header to a given parent.
		VOID						AddOptHeader( CTreeViewItem * _ptviParent, const MX_COFF_WINDOW_OPT &_cwoHeader, const mx::CPeObject &_poPeObject, uint64_t _uiImageBase, uint64_t _uiStructOffset );

		// Decodes a PE 32 header to a given parent.
		VOID						AddPe32Header( CTreeViewItem * _ptviParent, const MX_COFF_WINDOWS_PE32 &_cwpHeader, uint64_t _uiStructOffset );

		// Decodes a PE 64 header to a given parent.
		VOID						AddPe64Header( CTreeViewItem * _ptviParent, const MX_COFF_WINDOWS_PE64 &_cwpHeader, uint64_t _uiStructOffset );

		// Decodes an array of data directories.
		VOID						AddDataDirectories( CTreeViewItem * _ptviParent, const std::vector<MX_DATA_DIRECTORY> &_vDataDirs, const mx::CPeObject &_poPeObject, uint64_t _uiImageBase, uint64_t _uiStructOffset );

		// Decodes a data directory.
		VOID						AddDataDirectory( CTreeViewItem * _ptviParent, const MX_DATA_DIRECTORY &_ddDir, const mx::CPeObject &_poPeObject, uint64_t _uiImageBase, uint64_t _uiStructOffset );

		// Decodes an array of MX_IMAGE_SECTION_HEADER objects.
		VOID						AddSectionHeaders( CTreeViewItem * _ptviParent, const std::vector<MX_IMAGE_SECTION_HEADER> &_vHeaders, uint64_t _uiImageBase, uint64_t _uiStructOffset );

		// Decodes an MX_IMAGE_SECTION_HEADER object.
		VOID						AddSectionHeader( CTreeViewItem * _ptviParent, const MX_IMAGE_SECTION_HEADER &_ishHeader, uint64_t _uiImageBase, uint64_t _uiStructOffset );

		// Decodes exports.
		VOID						AddExports( CTreeViewItem * _ptviParent, const mx::CPeObject &_poPeObject );

		// Decodes an MX_IMAGE_EXPORT_DIRECTORY object.
		VOID						AddExportDesc( CTreeViewItem * _ptviParent, const mx::CPeObject &_poPeObject );

		// Decodes exported functions.
		VOID						AddExportedFunctions( CTreeViewItem * _ptviParent, const mx::CPeObject &_poPeObject );

		// Decodes exported functions.
		VOID						AddExportedFunctions( CTreeViewItem * _ptviParent, const mx::CPeObject &_poPeObject, const std::vector<CPeObject::MX_EXPORT> &_vList );

		// Decodes an MX_IMAGE_IMPORT_DESCRIPTOR object.
		VOID						AddImportDesc( CTreeViewItem * _ptviParent, const MX_IMAGE_IMPORT_DESCRIPTOR &_iidDesc, const mx::CPeObject &_poPeObject, uint64_t _uiStructOffset );

		// Decodes all imports.
		VOID						AddImportedFuncs( CTreeViewItem * _ptviParent, const mx::CPeObject &_poPeObject );

		// Decodes imports from a given import directory.
		VOID						AddImportedFuncs( CTreeViewItem * _ptviParent, uint32_t _uiImportDir, const mx::CPeObject &_poPeObject );

		// Decodes an import.
		VOID						AddImport( CTreeViewItem * _ptviParent, const CPeObject::MX_IMPORT &_iImport, const mx::CPeObject &_poPeObject );

		// Decodes an MX_IMAGE_IMPORT_DESCRIPTOR object.
		VOID						AddResourceDesc( CTreeViewItem * _ptviParent, const MX_IMAGE_RESOURCE_DIRECTORY &_irdDesc, const mx::CPeObject &_poPeObject, uint64_t _uiStructOffset );

		// Decodes the resource tree.
		VOID						AddResourceTree( CTreeViewItem * _ptviParent, const mx::CPeObject &_poPeObject );

		// Decodes the resource tree.
		VOID						AddResourceTree( CTreeViewItem * _ptviParent, const MX_IMAGE_RESOURCE_DIRECTORY * _pirdDesc, uint32_t _uiSectionIndex, const mx::CPeObject &_poPeObject, uint32_t _uiLevel );

		// Decodes the resource tree.
		VOID						AddResourceTree( CTreeViewItem * _ptviParent, const MX_IMAGE_RESOURCE_DIRECTORY_ENTRY * _pirdeEntry, uint32_t _uiSectionIndex, const mx::CPeObject &_poPeObject, uint32_t _uiLevel );

		// Decodes the relocation table.
		VOID						AddReloc( CTreeViewItem * _ptviParent, const std::vector<CPeObject::MX_BASE_RELOC> &_vRelocs, const mx::CPeObject &_poPeObject );

		// Decodes the relocation offsets.
		VOID						AddReloc( CTreeViewItem * _ptviParent, const CPeObject::MX_BASE_RELOC &_brRelocs, const mx::CPeObject &_poPeObject, uint64_t _uiStructOffset );

	};

}	// namespace mx