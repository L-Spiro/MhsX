#pragma once
#include "../MXMhsX.h"
#include "../MemHack/MXMemHack.h"
#include "../Options/MXOptions.h"

#include <EEExpEval.h>
#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

using namespace lsw;

namespace mx {

	class CNumberInputLayout {
	public :
		// == Enumerations.
		// Control ID's.
		enum MX_NUMBER_INPUT_IDS {
			MX_NI_NONE,
			MX_NI_DIALOG						= 4600,
			MX_NI_LABEL,
			MX_NI_COMBO,

			MX_NI_CANCEL,
			MX_NI_OK,
		};


		// == Types.
		struct									MX_NUMBER_DIALOG_PARMS;
		typedef bool (*							PfVerifyFunc)( void *, const MX_NUMBER_DIALOG_PARMS & );

		/** Parameters to pass to the dialog. */
		struct MX_NUMBER_DIALOG_PARMS {
			CSecureWString						swsHeaderText;						/**< The text for the dialog. */
			CSecureWString						swsLabelText;						/**< The text for the label. */
			CSecureWString						swsDefaultText;						/**< The default text in the combo box. */
			CSecureWString						swsExpression;						/**< Upon return, this holds the expression that was entered so that it can be added to the history. */
			std::vector<CUtilities::MX_COMBO_ENTRY>
												vComboItems;						/**< The combo-box entries */
			ee::CExpEvalContainer::EE_RESULT	rResult;							/**< Holds the returned result.  It can never return an object type, but if the return type is a string it will be copied to psStringReturn. */
			std::string *						psStringReturn = nullptr;			/**< If rResult.ncType is EE_NC_OBJECT and this is not NULL, it will be filled with the string result of the entered expression. */
			CWidget *							pwThis = nullptr;					/**< Set by this object when calling pfVerifyFunc.  Do not set externally. */

			PfVerifyFunc						pfVerifyFunc = nullptr;				/**< The function to verify that the input is valid. Called when the user hits OK. */
			void *								pvVerifyParm = nullptr;				/**< The parameter to pass to the verification function. */
			bool								bHexInput = false;					/**< Default to hex numeric interpretation? */
		};


		// == Functions.
		// Creates the Number Input dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
		static DWORD							CreateNumberInputDialog( CWidget * _pwParent, MX_NUMBER_DIALOG_PARMS &_ndpParms );


	protected :
		// == Members.
		// The layout for the Number Input dialog.
		static LSW_WIDGET_LAYOUT				m_wlNumberInputDialog[];
	};

}	// namespace mx
