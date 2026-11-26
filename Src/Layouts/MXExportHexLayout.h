#pragma once
#include "../MXMhsX.h"
#include "../HexEditor/MXCopyAs.h"

#include <Layout/LSWWidgetLayout.h>
#include <Widget/LSWWidget.h>

#include <string>

using namespace lsw;

namespace mx {

	/**
	 * The layout of the Export Hex dialog (File -> Export Hex...), which writes the file (or the selection) to a text file in one of
	 *	the Copy As formats.
	 **/
	class CExportHexLayout {
	public :
		// == Enumerations.
		/** Control IDs. */
		enum MX_EXPORT_HEX_IDS {
			MX_EHI_NONE,
			MX_EHI_DIALOG						= 6300,
			MX_EHI_FILE_LABEL,
			MX_EHI_FILE_EDIT,
			MX_EHI_BROWSE,
			MX_EHI_TYPE_LABEL,
			MX_EHI_TYPE_COMBO,
			MX_EHI_BPR_LABEL,
			MX_EHI_BPR_EDIT,
			MX_EHI_BPR_UPDOWN,
			MX_EHI_OPTIONS_GROUP,
			MX_EHI_RANGE_LABEL,
			MX_EHI_RANGE_ALL,
			MX_EHI_RANGE_SEL,
			MX_EHI_START_LABEL,
			MX_EHI_START_ZERO,
			MX_EHI_START_RANGE,
			MX_EHI_START_CUSTOM,
			MX_EHI_START_EDIT,
			MX_EHI_UNITS_LABEL,
			MX_EHI_UNITS_BYTES,
			MX_EHI_UNITS_WORDS,
			MX_EHI_SPACES,
			MX_EHI_LINEFEEDS,

			MX_EHI_EXPORT,
			MX_EHI_CANCEL,
		};

		/** Where Intel HEX and Motorola addresses start. */
		enum MX_START_MODE {
			MX_SM_ZERO,												/**< The first exported byte is at address 0. */
			MX_SM_RANGE,											/**< Each byte is at its own (displayed) address. */
			MX_SM_CUSTOM,											/**< The first exported byte is at ui64CustomStart. */
		};


		// == Types.
		/** Parameters passed to and returned by the dialog. */
		struct MX_EXPORT_HEX_PARMS {
			std::wstring						wsSourceName;						/**< The name of what is being exported (shown in the title). */
			std::wstring						wsPath;								/**< In/out: the file to write. */
			uint32_t							ui32Type = 0;						/**< In/out: the index of the export type (see Types()). */
			uint32_t							ui32BytesPerRow = 16;				/**< In/out: the bytes per row. */
			bool								bHasSelection = false;				/**< Whether there is a selection that can be exported. */
			bool								bSelection = false;					/**< In/out: export the selection rather than everything. */
			MX_START_MODE						smStart = MX_SM_ZERO;				/**< In/out: where Intel HEX and Motorola addresses start. */
			uint64_t							ui64CustomStart = 0;				/**< In/out: the custom start address. */
			bool								bHexInput = true;					/**< Whether the custom start address is read as hexadecimal. */
			bool								bWords = false;						/**< In/out: Intel HEX and Motorola addresses are in words. */
			bool								bSpaces = true;						/**< In/out: Hex Text puts spaces between bytes. */
			bool								bLineFeeds = true;					/**< In/out: Hex Text puts each row on its own line. */
		};

		/** One export type. */
		struct MX_EXPORT_TYPE {
			MX_COPY_AS							caFormat;							/**< The Copy As format that produces it. */
			const char *						pcName;								/**< The encrypted name shown in the list. */
			size_t								sNameLen;							/**< The length of pcName. */
			const char *						pcExt;								/**< The encrypted file extension (with its '.'). */
			size_t								sExtLen;							/**< The length of pcExt. */
		};


		// == Functions.
		/**
		 * Shows the Export Hex dialog.  Makes an in-memory copy of the LSW_WIDGET_LAYOUT's so it can decode strings etc.
		 *
		 * \param _pwParent The parent window.
		 * \param _ehpParms The defaults on input and the choices made on output.
		 * \return Returns TRUE if the user pressed Export.
		 **/
		static DWORD							CreateExportHexDialog( CWidget * _pwParent, MX_EXPORT_HEX_PARMS &_ehpParms );

		/**
		 * Gets the export types, in the order in which they are listed.
		 *
		 * \param _sTotal Receives the number of types.
		 * \return Returns the types.
		 **/
		static const MX_EXPORT_TYPE *			Types( size_t &_sTotal );

		/**
		 * Determines whether an export type is made of address records (Intel HEX or Motorola), which use the start-address and
		 *	address-unit options.
		 *
		 * \param _caFormat The Copy As format.
		 * \return Returns true for the Intel HEX and Motorola formats.
		 **/
		static bool								IsRecordFormat( MX_COPY_AS _caFormat );


	protected :
		// == Members.
		/** The layout of the Export Hex dialog. */
		static LSW_WIDGET_LAYOUT				m_wlExportHexDialog[];
		/** The export types. */
		static const MX_EXPORT_TYPE				m_etTypes[];
	};

}	// namespace mx
