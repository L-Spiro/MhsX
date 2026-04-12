#include "MXNumberInputLayout.h"
#include "../Layouts/MXLayoutMacros.h"
#include "../Strings/MXStringDecoder.h"
#include "../System/MXSystem.h"
#include "MXLayoutManager.h"

#include <Base/LSWBase.h>


namespace mx {

#define MX_NUMBER_DLG_W										170
#define MX_NUMBER_DLG_H										170

	// == Members.
	// The layout for the New Data-Type Search dialog.
	LSW_WIDGET_LAYOUT CNumberInputLayout::m_wlNumberInputDialog[] = {
		{
			MX_NEW_DATA_TYPE_SEARCH,						// ltType
			MX_NI_DIALOG,									// wId
			nullptr,										// lpwcClass
			TRUE,											// bEnabled
			FALSE,											// bActive
			0,												// iLeft
			0,												// iTop
			MX_NUMBER_DLG_W,								// dwWidth
			MX_NUMBER_DLG_H,								// dwHeight
			WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | DS_3DLOOK | DS_FIXEDSYS | DS_MODALFRAME | DS_CENTER,					// dwStyle
			WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,												// dwStyleEx
			nullptr,										// pwcText
			0,												// sTextLen
			MX_NI_NONE,										// dwParentId
		},
	};

	// == Functions.
	// Creates the New Data-Type Search dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
	DWORD CNumberInputLayout::CreateNumberInputDialog( CWidget * _pwParent, MX_NUMBER_DIALOG_PARMS &_ndpParms ) {
		std::vector<CSecureString> sStrings;
		std::vector<CSecureWString> sStringsW;
		std::vector<LSW_WIDGET_LAYOUT> vLayouts;
		CLayoutManager::UnencryptLayouts( m_wlNumberInputDialog, std::size( m_wlNumberInputDialog ),
			vLayouts,
			sStringsW,
			sStrings );
		
		mx::CLayoutManager * plmLayout = static_cast<mx::CLayoutManager *>(lsw::CBase::LayoutManager());
		INT_PTR ipProc = plmLayout->DialogBoxX( &vLayouts[0], std::size( m_wlNumberInputDialog ), _pwParent, reinterpret_cast<uint64_t>(&_ndpParms) );
		CLayoutManager::CleanEncryptedStrings( sStringsW, sStrings );
		if ( ipProc != 0 ) {
			// Success.  Do stuff.
			return TRUE;
		}

		return FALSE;
	}

}	// namespace mx
