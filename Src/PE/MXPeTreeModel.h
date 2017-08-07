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
		VOID						AddDosHeader( CTreeViewItem * _ptviParent, const MX_DOS_HEADER &_dhHeader );

		// Decodes a DOS stub to a given parent.
		VOID						AddDosStub( CTreeViewItem * _ptviParent, const std::vector<uint8_t> &_vStub );

		// Decodes a COFF header to a given parent.
		VOID						AddCoffHeader( CTreeViewItem * _ptviParent, const MX_COFF_HEADER &_chHeader );

		// Decodes a PE header to a given parent.
		VOID						AddOptHeader( CTreeViewItem * _ptviParent, const MX_COFF_WINDOW_OPT &_cwoHeader, uint64_t _uiImageBase );

		// Decodes a PE 32 header to a given parent.
		VOID						AddPe32Header( CTreeViewItem * _ptviParent, const MX_COFF_WINDOWS_PE32 &_cwpHeader );

		// Decodes a PE 64 header to a given parent.
		VOID						AddPe64Header( CTreeViewItem * _ptviParent, const MX_COFF_WINDOWS_PE64 &_cwpHeader );

		// Decodes an array of data directories.
		VOID						AddDataDirectories( CTreeViewItem * _ptviParent, const std::vector<MX_DATA_DIRECTORY> &_vDataDirs, uint64_t _uiImageBase );

		// Decodes a data directory.
		VOID						AddDataDirectory( CTreeViewItem * _ptviParent, const MX_DATA_DIRECTORY &_ddDir, uint64_t _uiImageBase );

		// Decodes an array of MX_IMAGE_SECTION_HEADER objects.
		VOID						AddSectionHeaders( CTreeViewItem * _ptviParent, const std::vector<MX_IMAGE_SECTION_HEADER> &_vHeaders, uint64_t _uiImageBase );

		// Decodes an MX_IMAGE_SECTION_HEADER object.
		VOID						AddSectionHeader( CTreeViewItem * _ptviParent, const MX_IMAGE_SECTION_HEADER &_ishHeader, uint64_t _uiImageBase );

	};

}	// namespace mx