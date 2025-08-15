#pragma once
#include "../MXMhsX.h"
#include "../Layouts/MXFoundAddressEditLayout.h"
#include "../Strings/MXSecureWString.h"
#include "../Utilities/MXUtilities.h"
#include "MXFoundAddressEditPageBase.h"

#include <LSWWin.h>

using namespace lsw;

namespace mx {

	class CFoundAddressBase;

	class CFoundAddressEditMainPage : public CFoundAddressEditPageBase {
	public :
		CFoundAddressEditMainPage( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		// Get the name of the page.
		virtual std::wstring					GetName() const;

		// WM_INITDIALOG.
		virtual LSW_HANDLED						InitDialog();

		// WM_COMMAND from control.
		virtual LSW_HANDLED						Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc );

		// Verifies the options, returning an error string in case of error.
		virtual BOOL							Verify( std::wstring &_wsError, CWidget * &_pwWidget );

		// Copies all the settings to the MX_OPTIONS structure.
		virtual BOOL							Finalize();

		// Updates the contents of page based on settings etc.
		virtual void							Update();


	protected :
		// == Members.


		// == Functions.
		// Gathers all descriptions and returns the Description string.
		CSecureWString							GatherDescriptions() const;

		// Gathers all current value strings.
		CSecureWString							GatherCurValue() const;

		// Gathers all locked-value strings.
		CSecureWString							GatherLockedValue() const;

		// Gathers all max locked-value strings.
		CSecureWString							GatherMaxLockedValue() const;

		// Gathers all address strings.
		CSecureWString							GatherAddress() const;

		// Gathers all data types.
		CUtilities::MX_DATA_TYPES				GatherDataTypes() const;

		// Gathers all lock types.
		CUtilities::MX_LOCK_TYPES				GatherLockTypes() const;

		// Gathers all locked states.
		int										GatherLockedStates() const;

		// Gathers all array lengths.
		CSecureWString							GatherArrayLens() const;

		// Gathers all array strides.
		CSecureWString							GatherArrayStrides() const;

		// Gets the lock type for a given found address.
		static LPARAM							LockType( CFoundAddressBase * _pfabFoundAddress );

		// Gets the data type for a given found address.
		static CUtilities::MX_DATA_TYPES		DataType( CFoundAddressBase * _pfabFoundAddress );

		// Converts a string to its description.
		static CSecureWString					StringToDescr( const CSecureWString &_swsString, bool _bResolveEscapes, UINT _uiCodePage );

		// Converts a float string to is description.
		static CSecureWString					FloatToDesc( const CSecureWString &_swsString, CUtilities::MX_DATA_TYPES _dtType, bool _bScentific, uint32_t &_ui32ArrayLen, uint32_t _ui32Stride, std::vector<ee::CExpEvalContainer::EE_RESULT> * _pvExps );

		// Converts a not-float string to its description.
		static CSecureWString					IntegralToDesc( const CSecureWString &_swsString, CUtilities::MX_DATA_TYPES _dtType, bool _bIsHex, uint32_t &_ui32ArrayLen, uint32_t _ui32Stride, std::vector<ee::CExpEvalContainer::EE_RESULT> * _pvExps );

	private :
		typedef CFoundAddressEditLayout			Layout;
		
	};

}	// namespace mx
