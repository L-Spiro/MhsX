#pragma once

#include "../MXMhsX.h"
#include "../Strings/MXSecureWString.h"
#include "MXCopyAs.h"
#include "MXSelection.h"

#include <functional>
#include <vector>


namespace mx {

	/**
	 * Class CCopySink
	 * \brief A destination for rendered clipboard data.
	 *
	 * The formatter writes sequentially with Write() and may go back to fix up a header with Patch().
	 */
	class CCopySink {
	public :
		virtual ~CCopySink() {}


		// == Functions.
		/**
		 * \brief Appends bytes to the end of the sink.
		 *
		 * \param _pvData The bytes to append.
		 * \param _sSize The number of bytes to which _pvData points.
		 * \return Returns false if memory could not be allocated.
		 */
		virtual bool											Write( const void * _pvData, size_t _sSize ) = 0;

		/**
		 * \brief Overwrites bytes that have already been written.
		 *
		 * \param _sOffset The offset of the first byte to overwrite.
		 * \param _pvData The replacement bytes.
		 * \param _sSize The number of bytes to which _pvData points.  _sOffset + _sSize must not exceed Size().
		 * \return Returns false if the range is invalid or the memory could not be accessed.
		 */
		virtual bool											Patch( size_t _sOffset, const void * _pvData, size_t _sSize ) = 0;

		/**
		 * \brief Gets the number of bytes written so far.
		 *
		 * \return Returns the number of bytes written so far.
		 */
		virtual size_t											Size() const = 0;
	};


	/**
	 * Class CCopyVectorSink
	 * \brief A CCopySink that writes into a std::vector, for data that is handed to the clipboard immediately.
	 */
	class CCopyVectorSink : public CCopySink {
	public :
		// == Functions.
		/**
		 * \brief Appends bytes to the end of the sink.
		 *
		 * \param _pvData The bytes to append.
		 * \param _sSize The number of bytes to which _pvData points.
		 * \return Returns false if memory could not be allocated.
		 */
		virtual bool											Write( const void * _pvData, size_t _sSize ) override;

		/**
		 * \brief Overwrites bytes that have already been written.
		 *
		 * \param _sOffset The offset of the first byte to overwrite.
		 * \param _pvData The replacement bytes.
		 * \param _sSize The number of bytes to which _pvData points.  _sOffset + _sSize must not exceed Size().
		 * \return Returns false if the range is invalid.
		 */
		virtual bool											Patch( size_t _sOffset, const void * _pvData, size_t _sSize ) override;

		/**
		 * \brief Gets the number of bytes written so far.
		 *
		 * \return Returns the number of bytes written so far.
		 */
		virtual size_t											Size() const override { return m_vData.size(); }

		/**
		 * \brief Gets the written bytes.
		 *
		 * \return Returns a reference to the written bytes.
		 */
		inline const std::vector<uint8_t> &						Data() const { return m_vData; }


	protected :
		// == Members.
		/** The written bytes. */
		std::vector<uint8_t>									m_vData;
	};


	/**
	 * Class CCopyGlobalSink
	 * \brief A CCopySink that writes directly into a growable moveable HGLOBAL so that large data can be passed to ::SetClipboardData()
	 *	without first being built in a second buffer.
	 */
	class CCopyGlobalSink : public CCopySink {
	public :
		CCopyGlobalSink() {}
		CCopyGlobalSink( const CCopyGlobalSink &_cgsOther ) = delete;
		~CCopyGlobalSink();


		// == Operators.
		/**
		 * \brief Copying is not allowed; the sink owns its HGLOBAL.
		 */
		CCopyGlobalSink &										operator = ( const CCopyGlobalSink &_cgsOther ) = delete;


		// == Functions.
		/**
		 * \brief Appends bytes to the end of the sink, growing the HGLOBAL as needed.
		 *
		 * \param _pvData The bytes to append.
		 * \param _sSize The number of bytes to which _pvData points.
		 * \return Returns false if memory could not be allocated or locked.
		 */
		virtual bool											Write( const void * _pvData, size_t _sSize ) override;

		/**
		 * \brief Overwrites bytes that have already been written.
		 *
		 * \param _sOffset The offset of the first byte to overwrite.
		 * \param _pvData The replacement bytes.
		 * \param _sSize The number of bytes to which _pvData points.  _sOffset + _sSize must not exceed Size().
		 * \return Returns false if the range is invalid or the memory could not be locked.
		 */
		virtual bool											Patch( size_t _sOffset, const void * _pvData, size_t _sSize ) override;

		/**
		 * \brief Gets the number of bytes written so far.
		 *
		 * \return Returns the number of bytes written so far.
		 */
		virtual size_t											Size() const override { return m_sSize; }

		/**
		 * \brief Shrinks the HGLOBAL to the written size and transfers ownership of it to the caller.
		 *
		 * After a successful ::SetClipboardData() the system owns the handle; otherwise the caller must ::GlobalFree() it.
		 *
		 * \return Returns the HGLOBAL, or NULL if nothing was written.
		 */
		HGLOBAL													Release();


	protected :
		// == Members.
		/** The managed memory. */
		HGLOBAL													m_hGlobal = NULL;
		/** The number of bytes written. */
		size_t													m_sSize = 0;
		/** The number of bytes allocated. */
		size_t													m_sCapacity = 0;
	};


	/**
	 * Class CCopyFormatter
	 * \brief Renders selected bytes into Windows clipboard formats according to an MX_COPYAS.
	 *
	 * The bytes are pulled through a read callback in chunks, so the same code renders immediately from a live process or lazily
	 *	(during WM_RENDERFORMAT) from a file snapshot, and never needs the whole selection in RAM at once (except as required by the
	 *	destination HGLOBAL itself).
	 */
	class CCopyFormatter {
	public :
		// == Types.
		/**
		 * Reads _sSize bytes at _ui64Addr into _vDst (resizing it).  Returns false on failure.
		 */
		typedef std::function<bool ( uint64_t _ui64Addr, std::vector<uint8_t> &_vDst, size_t _sSize )>
																PfRead;

		/**
		 * Everything needed to render one Copy operation.
		 */
		struct MX_COPY_SOURCE {
			PfRead												fRead;													/**< Reads the copied bytes. */
			MX_SELECTION										sSelection;												/**< The selection that was copied. */
			uint32_t											ui32BytesPerRow			= 16;							/**< The bytes-per-row at the time of the Copy. */
			MX_COPYAS											caCopyAs;												/**< The copy parameters. */
			uint64_t											ui64Size				= 0;							/**< The size of the data source; selected ranges are clamped to it. */
		};


		// == Functions.
		/**
		 * \brief Gets the registered "BinaryData" clipboard format used for raw-byte copies.
		 *
		 * \return Returns the clipboard-format ID, or 0 on failure.
		 */
		static UINT												BinaryFormat();

		/**
		 * \brief Gets the registered "HTML Format" clipboard format.
		 *
		 * \return Returns the clipboard-format ID, or 0 on failure.
		 */
		static UINT												HtmlFormat();

		/**
		 * \brief Gets the registered "Rich Text Format" clipboard format.
		 *
		 * \return Returns the clipboard-format ID, or 0 on failure.
		 */
		static UINT												RtfFormat();

		/**
		 * \brief Gets the clipboard formats offered for a given copy type, most-preferred first.
		 *
		 * \param _caCopyAs The copy parameters.
		 * \param _vFormats Receives the clipboard-format IDs.
		 * \return Returns false on a memory or registration failure.
		 */
		static bool												Formats( const MX_COPYAS &_caCopyAs, std::vector<UINT> &_vFormats );

		/**
		 * \brief Renders a Copy operation into one clipboard format.
		 *
		 * CF_UNICODETEXT output is NUL-terminated.  For MX_CA_WEB_PAGE and MX_CA_RICH_TEXT, CF_UNICODETEXT is the plain-text fallback
		 *	(the same text as MX_CA_HEX_EDITOR_AREAS).
		 *
		 * \param _csSource The data and parameters of the Copy operation.
		 * \param _uiFormat The clipboard format to produce.  Must be one returned by Formats() for _csSource.caCopyAs.
		 * \param _csSink Receives the rendered bytes.
		 * \param _swsMsg Receives an error message on failure.
		 * \return Returns true if the format was rendered.
		 */
		static bool												Render( const MX_COPY_SOURCE &_csSource, UINT _uiFormat, CCopySink &_csSink, CSecureWString &_swsMsg );

		/**
		 * \brief Gets the number of selected bytes, clamped to the source size.
		 *
		 * \param _csSource The data and parameters of the Copy operation.
		 * \return Returns the number of bytes that will be rendered.
		 */
		static uint64_t											TotalBytes( const MX_COPY_SOURCE &_csSource );

		/**
		 * \brief Gets the number of ranges in a selection.
		 *
		 * \param _csSource The data and parameters of the Copy operation.
		 * \return Returns 1 for a Normal selection, the number of lines for a Column selection, or 0 for no selection.
		 */
		static uint64_t											TotalRanges( const MX_COPY_SOURCE &_csSource );

		/**
		 * \brief Gets a selected range, clamped to the source size.  Ranges are returned in ascending address order.
		 *
		 * \param _csSource The data and parameters of the Copy operation.
		 * \param _ui64Idx The index of the range, less than TotalRanges().
		 * \return Returns the range, which may have a size of 0 if it lies beyond the end of the source.
		 */
		static MX_SEL_RANGE										RangeAt( const MX_COPY_SOURCE &_csSource, uint64_t _ui64Idx );


	protected :
		// == Types.
		/** Reads a contiguous address range sequentially in large chunks. */
		class CByteStream;
		/** Encodes text into a sink as UTF-16, escaped UTF-8 (HTML), or escaped ASCII (RTF). */
		class CTextOut;


		// == Functions.
		/**
		 * \brief Writes every selected byte, in order, to a sink.
		 *
		 * \param _csSource The data and parameters of the Copy operation.
		 * \param _csSink Receives the bytes.
		 * \return Returns false on a read or memory failure.
		 */
		static bool												RenderBinary( const MX_COPY_SOURCE &_csSource, CCopySink &_csSink );

		/**
		 * \brief Writes the textual form of the Copy operation.
		 *
		 * \param _csSource The data and parameters of the Copy operation.
		 * \param _toOut Receives the text.
		 * \return Returns false on a read or memory failure.
		 */
		static bool												RenderText( const MX_COPY_SOURCE &_csSource, CTextOut &_toOut );

		/**
		 * \brief Writes space-separated byte values (MX_CA_HEX_TEXT, MX_CA_DECIMAL_TEXT, MX_CA_BINARY_TEXT), one row per line.
		 *
		 * \param _csSource The data and parameters of the Copy operation.
		 * \param _toOut Receives the text.
		 * \return Returns false on a read or memory failure.
		 */
		static bool												RenderByteText( const MX_COPY_SOURCE &_csSource, CTextOut &_toOut );

		/**
		 * \brief Writes a C, Java, or Python byte-array declaration.
		 *
		 * \param _csSource The data and parameters of the Copy operation.
		 * \param _toOut Receives the text.
		 * \return Returns false on a read or memory failure.
		 */
		static bool												RenderCode( const MX_COPY_SOURCE &_csSource, CTextOut &_toOut );

		/**
		 * \brief Writes Intel HEX records (8-, 16-, or 32-bit).
		 *
		 * \param _csSource The data and parameters of the Copy operation.
		 * \param _toOut Receives the text.
		 * \return Returns false on a read or memory failure.
		 */
		static bool												RenderIntelHex( const MX_COPY_SOURCE &_csSource, CTextOut &_toOut );

		/**
		 * \brief Writes Motorola S-records (S19, S28, or S37).
		 *
		 * \param _csSource The data and parameters of the Copy operation.
		 * \param _toOut Receives the text.
		 * \return Returns false on a read or memory failure.
		 */
		static bool												RenderMotorola( const MX_COPY_SOURCE &_csSource, CTextOut &_toOut );

		/**
		 * \brief Writes the selection as a single line of Base64.
		 *
		 * \param _csSource The data and parameters of the Copy operation.
		 * \param _toOut Receives the text.
		 * \return Returns false on a read or memory failure.
		 */
		static bool												RenderBase64( const MX_COPY_SOURCE &_csSource, CTextOut &_toOut );

		/**
		 * \brief Writes the selection as a uuencoded file.
		 *
		 * \param _csSource The data and parameters of the Copy operation.
		 * \param _toOut Receives the text.
		 * \return Returns false on a read or memory failure.
		 */
		static bool												RenderUuEncoding( const MX_COPY_SOURCE &_csSource, CTextOut &_toOut );

		/**
		 * \brief Writes values of a primitive type (MX_CA_UINT8 through MX_CA_PCM32), space-separated with one row per line.
		 *
		 * \param _csSource The data and parameters of the Copy operation.
		 * \param _toOut Receives the text.
		 * \return Returns false on a read or memory failure.
		 */
		static bool												RenderValues( const MX_COPY_SOURCE &_csSource, CTextOut &_toOut );

		/**
		 * \brief Writes the selection the way the hex editor displays it: address, left area, and right area.
		 *
		 * Rows are aligned to the bytes-per-row exactly as in the view.  Cells outside of the selection are blank.
		 *
		 * \param _csSource The data and parameters of the Copy operation.
		 * \param _toOut Receives the text.
		 * \param _bNewLineAfterLast If true, the last row is also followed by a new line (as required inside HTML and RTF).
		 * \return Returns false on a read or memory failure.
		 */
		static bool												RenderHexEditorAreas( const MX_COPY_SOURCE &_csSource, CTextOut &_toOut, bool _bNewLineAfterLast );

		/**
		 * \brief Writes CF_HTML (UTF-8 with the Version/StartHTML/EndHTML/StartFragment/EndFragment header).
		 *
		 * \param _csSource The data and parameters of the Copy operation.
		 * \param _csSink Receives the bytes.
		 * \return Returns false on a read or memory failure.
		 */
		static bool												RenderHtml( const MX_COPY_SOURCE &_csSource, CCopySink &_csSink );

		/**
		 * \brief Writes Rich Text Format.
		 *
		 * \param _csSource The data and parameters of the Copy operation.
		 * \param _csSink Receives the bytes.
		 * \return Returns false on a read or memory failure.
		 */
		static bool												RenderRtf( const MX_COPY_SOURCE &_csSource, CCopySink &_csSink );

		/**
		 * \brief Gets the primitive data format that corresponds to an MX_CA_UINT8-through-MX_CA_PCM32 copy type.
		 *
		 * \param _caFormat The copy type.
		 * \param _dfFormat Receives the corresponding data format.
		 * \return Returns false if _caFormat is not a primitive-value copy type.
		 */
		static bool												ValueFormat( MX_COPY_AS _caFormat, MX_DATA_FMT &_dfFormat );

		/**
		 * \brief Gets the number of bytes consumed by one value of a data format.
		 *
		 * \param _dfFormat The data format.
		 * \param _csCharSet The character set (MX_DF_CHAR in Unicode consumes 2 bytes).
		 * \return Returns the number of bytes per value.
		 */
		static uint32_t											StrideForFormat( MX_DATA_FMT _dfFormat, CCharSets::MX_CHAR_SETS _csCharSet );

		/**
		 * \brief Gets the fixed text width of one value of a data format, used to keep the Hex Editor Areas columns aligned.
		 *
		 * \param _dfFormat The data format.
		 * \param _ui32FloatDigits The number of floating-point digits.
		 * \return Returns the width in characters.
		 */
		static uint32_t											WidthForFormat( MX_DATA_FMT _dfFormat, uint32_t _ui32FloatDigits );

		/**
		 * \brief Formats one value (not MX_DF_CHAR).
		 *
		 * \param _dfFormat The data format.
		 * \param _pui8Data Points to at least StrideForFormat() bytes.
		 * \param _bBigEndian If true, multi-byte values are read as big-endian.
		 * \param _ui32FloatDigits The number of floating-point digits.
		 * \param _wsOut Receives the text (replacing its contents).
		 * \return Returns false on a memory failure.
		 */
		static bool												FormatValue( MX_DATA_FMT _dfFormat, const uint8_t * _pui8Data, bool _bBigEndian, uint32_t _ui32FloatDigits, std::wstring &_wsOut );

		/**
		 * \brief Formats a row address exactly as the address gutter does.
		 *
		 * \param _caAddr The captured address-gutter settings.
		 * \param _ui64RowAddr The data address of the start of the row.
		 * \param _ui32BytesPerRow The bytes-per-row.
		 * \param _wsOut Receives the text (replacing its contents).
		 * \return Returns false on a memory failure.
		 */
		static bool												FormatAddress( const MX_COPY_ADDR &_caAddr, uint64_t _ui64RowAddr, uint32_t _ui32BytesPerRow, std::wstring &_wsOut );

		/**
		 * \brief Assembles an unsigned value from bytes.
		 *
		 * \param _pui8Data The bytes.
		 * \param _sSize The number of bytes (1 to 8).
		 * \param _bBigEndian If true, the bytes are big-endian.
		 * \return Returns the assembled value.
		 */
		static uint64_t											Assemble( const uint8_t * _pui8Data, size_t _sSize, bool _bBigEndian );
	};


	/**
	 * Class CCopyCapture
	 * \brief A private copy of the bytes of a Copy operation, for data sources whose contents can change after the Copy (processes).
	 *
	 * Only the selected bytes are stored.  Rendering reads them back through CCopyFormatter, so a capture renders exactly as the live
	 *	source would have at the time of the Copy.
	 */
	class CCopyCapture {
	public :
		// == Functions.
		/**
		 * \brief Reads every selected byte from the source and stores it, replacing any previous capture.
		 *
		 * \param _csSource The data and parameters of the Copy operation.  _csSource.fRead is used only during this call.
		 * \param _swsMsg Receives an error message on failure.
		 * \return Returns false on a read or memory failure, in which case the capture is empty.
		 */
		bool													Capture( const CCopyFormatter::MX_COPY_SOURCE &_csSource, CSecureWString &_swsMsg );

		/**
		 * \brief Renders the captured Copy operation into one clipboard format.
		 *
		 * \param _uiFormat The clipboard format to produce.
		 * \param _csSink Receives the rendered bytes.
		 * \param _swsMsg Receives an error message on failure.
		 * \return Returns false if nothing is captured or rendering fails.
		 */
		bool													Render( UINT _uiFormat, CCopySink &_csSink, CSecureWString &_swsMsg ) const;

		/**
		 * \brief Discards the captured bytes.
		 */
		void													Reset();

		/**
		 * \brief Determines whether anything is captured.
		 *
		 * \return Returns true if a Copy operation is held.
		 */
		inline bool												Empty() const { return !m_csSource.sSelection.HasSelection(); }


	protected :
		// == Members.
		/** The non-empty selected ranges, in ascending address order. */
		std::vector<MX_SEL_RANGE>								m_vRanges;
		/** The offset into m_vData of each range in m_vRanges. */
		std::vector<uint64_t>									m_vOffsets;
		/** The captured bytes, range after range. */
		std::vector<uint8_t>									m_vData;
		/** The Copy parameters (fRead is not kept). */
		CCopyFormatter::MX_COPY_SOURCE							m_csSource;


		// == Functions.
		/**
		 * \brief Reads captured bytes.  Bytes that were not selected read as 0.
		 *
		 * \param _ui64Addr The address of the first byte to read.
		 * \param _vDst Receives the bytes.
		 * \param _sSize The number of bytes to read.
		 * \return Returns false on a memory failure.
		 */
		bool													Read( uint64_t _ui64Addr, std::vector<uint8_t> &_vDst, size_t _sSize ) const;
	};

}	// namespace mx
