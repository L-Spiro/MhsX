#include "MXCopyFormatter.h"
#include "../Strings/MXStringDecoder.h"
#include "../Strings/MXStringMacros.h"
#include "MXHexEditorControl.h"

#include <Float16/EEFloat16.h>

#include <algorithm>
#include <cstring>
#include <format>


namespace mx {

	// == Constants.
	/** The number of bytes fetched from the data source at a time. */
	static constexpr size_t										MX_COPY_CHUNK				= 1024 * 1024;

	/** The number of encoded bytes CCopyFormatter::CTextOut buffers before passing them to its sink. */
	static constexpr size_t										MX_TEXT_BUFFER				= 64 * 1024;

	/** The digits used for hexadecimal output. */
	static constexpr char										MX_HEX_DIGITS[]				= "0123456789ABCDEF";


	// == Types.
	/**
	 * Class CCopyFormatter::CByteStream
	 * \brief Reads a contiguous address range sequentially, fetching it from the data source in large chunks.
	 */
	class CCopyFormatter::CByteStream {
	public :
		CByteStream( const MX_COPY_SOURCE &_csSource, uint64_t _ui64Start, uint64_t _ui64Size ) :
			m_csSource( _csSource ),
			m_ui64Next( _ui64Start ),
			m_ui64Left( _ui64Size ) {
		}


		// == Functions.
		/**
		 * \brief Gets the next block of contiguous bytes and advances past them.
		 *
		 * \param _pui8Data Receives a pointer to the bytes, valid until the next call.
		 * \param _sMax The maximum number of bytes to return.
		 * \return Returns the number of bytes in the block, or 0 at the end of the range or on failure (see Failed()).
		 */
		size_t													NextBlock( const uint8_t * &_pui8Data, size_t _sMax = SIZE_MAX ) {
			if ( m_sPos == m_vBuffer.size() ) {
				if ( !Fill() ) { return 0; }
			}
			size_t sAvail = std::min( m_vBuffer.size() - m_sPos, _sMax );
			_pui8Data = &m_vBuffer[m_sPos];
			m_sPos += sAvail;
			return sAvail;
		}

		/**
		 * \brief Reads an exact number of bytes.
		 *
		 * \param _pui8Dst The destination.
		 * \param _sSize The number of bytes to read.
		 * \return Returns false if fewer than _sSize bytes remain or the source could not be read.
		 */
		bool													Read( uint8_t * _pui8Dst, size_t _sSize ) {
			while ( _sSize ) {
				const uint8_t * pui8Src = nullptr;
				size_t sGot = NextBlock( pui8Src, _sSize );
				if ( !sGot ) { return false; }
				std::memcpy( _pui8Dst, pui8Src, sGot );
				_pui8Dst += sGot;
				_sSize -= sGot;
			}
			return true;
		}

		/**
		 * \brief Gets the number of bytes not yet consumed.
		 *
		 * \return Returns the number of bytes left in the range.
		 */
		inline uint64_t											Remaining() const { return m_ui64Left + (m_vBuffer.size() - m_sPos); }

		/**
		 * \brief Determines whether a read from the data source failed.
		 *
		 * \return Returns true if a read failed.
		 */
		inline bool												Failed() const { return m_bFailed; }


	protected :
		// == Members.
		/** The data source. */
		const MX_COPY_SOURCE &									m_csSource;
		/** The buffered bytes. */
		std::vector<uint8_t>									m_vBuffer;
		/** The read position within m_vBuffer. */
		size_t													m_sPos = 0;
		/** The address of the next chunk to fetch. */
		uint64_t												m_ui64Next;
		/** The number of bytes not yet fetched. */
		uint64_t												m_ui64Left;
		/** Set when a read fails. */
		bool													m_bFailed = false;


		// == Functions.
		/**
		 * \brief Fetches the next chunk into m_vBuffer.
		 *
		 * \return Returns false at the end of the range or on failure.
		 */
		bool													Fill() {
			if ( !m_ui64Left || m_bFailed ) { return false; }
			const size_t sChunk = size_t( std::min<uint64_t>( m_ui64Left, MX_COPY_CHUNK ) );
			try {
				if ( !m_csSource.fRead || !m_csSource.fRead( m_ui64Next, m_vBuffer, sChunk ) || m_vBuffer.size() < sChunk ) {
					m_bFailed = true;
					return false;
				}
				m_vBuffer.resize( sChunk );
			}
			catch ( ... ) {
				m_bFailed = true;
				return false;
			}
			m_sPos = 0;
			m_ui64Next += sChunk;
			m_ui64Left -= sChunk;
			return true;
		}
	};


	/**
	 * Class CCopyFormatter::CTextOut
	 * \brief Encodes text into a sink as UTF-16 (CF_UNICODETEXT), escaped UTF-8 (CF_HTML), or escaped ASCII (RTF), buffering the output.
	 *
	 * Errors are sticky: after any failure every call returns false, so callers may check Ok() once per line.
	 */
	class CCopyFormatter::CTextOut {
	public :
		// == Enumerations.
		/** The output encoding. */
		enum MX_MODE {
			MX_M_UTF16,											/**< UTF-16LE text with CRLF new lines. */
			MX_M_HTML,											/**< UTF-8 with &, <, and > escaped, and CRLF new lines. */
			MX_M_RTF,											/**< ASCII with \, {, and } escaped, \uN? for other characters, and \par new lines. */
		};


		CTextOut( CCopySink &_csSink, MX_MODE _mMode ) :
			m_csSink( _csSink ),
			m_mMode( _mMode ) {
		}


		// == Functions.
		/**
		 * \brief Writes one character.
		 *
		 * \param _wcChar The character to write.
		 * \return Returns false if an error has occurred.
		 */
		bool													Put( wchar_t _wcChar ) {
			if ( !m_bOk ) { return false; }
			try {
				switch ( m_mMode ) {
					case MX_M_UTF16 : {
						const uint16_t ui16Char = static_cast<uint16_t>(_wcChar);
						m_vBuffer.push_back( static_cast<uint8_t>(ui16Char) );
						m_vBuffer.push_back( static_cast<uint8_t>(ui16Char >> 8) );
						break;
					}
					case MX_M_HTML : {
						switch ( _wcChar ) {
							case L'&' : { AppendRaw( "&amp;" ); break; }
							case L'<' : { AppendRaw( "&lt;" ); break; }
							case L'>' : { AppendRaw( "&gt;" ); break; }
							default : { AppendUtf8( static_cast<uint32_t>(static_cast<uint16_t>(_wcChar)) ); }
						}
						break;
					}
					case MX_M_RTF : {
						const uint16_t ui16Char = static_cast<uint16_t>(_wcChar);
						if ( ui16Char == L'\\' || ui16Char == L'{' || ui16Char == L'}' ) {
							m_vBuffer.push_back( '\\' );
							m_vBuffer.push_back( static_cast<uint8_t>(ui16Char) );
						}
						else if ( ui16Char < 0x80 ) {
							m_vBuffer.push_back( static_cast<uint8_t>(ui16Char) );
						}
						else {
							// \uN takes a signed 16-bit value, followed by a one-character fallback for readers that do not understand it.
							AppendRaw( std::format( "\\u{}?", static_cast<int16_t>(ui16Char) ).c_str() );
						}
						break;
					}
				}
			}
			catch ( ... ) { m_bOk = false; return false; }
			return FlushIfFull();
		}

		/**
		 * \brief Writes characters.
		 *
		 * \param _pwcText The characters to write.
		 * \param _sLen The number of characters to which _pwcText points.
		 * \return Returns false if an error has occurred.
		 */
		bool													Put( const wchar_t * _pwcText, size_t _sLen ) {
			for ( size_t I = 0; I < _sLen; ++I ) {
				if ( !Put( _pwcText[I] ) ) { return false; }
			}
			return m_bOk;
		}

		/**
		 * \brief Writes a string.
		 *
		 * \param _wsText The string to write.
		 * \return Returns false if an error has occurred.
		 */
		bool													Put( const std::wstring &_wsText ) { return Put( _wsText.data(), _wsText.size() ); }

		/**
		 * \brief Writes a NUL-terminated ASCII string, escaped like any other text.
		 *
		 * \param _pcText The string to write.
		 * \return Returns false if an error has occurred.
		 */
		bool													Put( const char * _pcText ) {
			while ( (*_pcText) ) {
				if ( !Put( static_cast<wchar_t>(static_cast<uint8_t>((*_pcText++))) ) ) { return false; }
			}
			return m_bOk;
		}

		/**
		 * \brief Writes a run of spaces.
		 *
		 * \param _sTotal The number of spaces to write.
		 * \return Returns false if an error has occurred.
		 */
		bool													Spaces( size_t _sTotal ) {
			while ( _sTotal-- ) {
				if ( !Put( L' ' ) ) { return false; }
			}
			return m_bOk;
		}

		/**
		 * \brief Ends the current line.
		 *
		 * \return Returns false if an error has occurred.
		 */
		bool													NewLine() {
			if ( !m_bOk ) { return false; }
			if ( m_mMode == MX_M_RTF ) {
				return Raw( "\\par\r\n" );
			}
			return Put( L'\r' ) && Put( L'\n' );
		}

		/**
		 * \brief Writes markup without escaping (for HTML and RTF framing).  In UTF-16 mode each byte becomes one character.
		 *
		 * \param _pcText The NUL-terminated ASCII markup to write.
		 * \return Returns false if an error has occurred.
		 */
		bool													Raw( const char * _pcText ) {
			if ( !m_bOk ) { return false; }
			if ( m_mMode == MX_M_UTF16 ) { return Put( _pcText ); }
			try {
				AppendRaw( _pcText );
			}
			catch ( ... ) { m_bOk = false; return false; }
			return FlushIfFull();
		}

		/**
		 * \brief Passes all buffered bytes to the sink.
		 *
		 * \return Returns false if an error has occurred.
		 */
		bool													Flush() {
			if ( !m_bOk ) { return false; }
			if ( m_vBuffer.size() ) {
				if ( !m_csSink.Write( m_vBuffer.data(), m_vBuffer.size() ) ) {
					m_bOk = false;
					return false;
				}
				m_vBuffer.clear();
			}
			return true;
		}

		/**
		 * \brief Gets the total number of bytes produced so far, including buffered bytes.
		 *
		 * \return Returns the byte offset of the next byte to be written.
		 */
		inline size_t											Written() const { return m_csSink.Size() + m_vBuffer.size(); }

		/**
		 * \brief Determines whether every operation so far has succeeded.
		 *
		 * \return Returns true if no error has occurred.
		 */
		inline bool												Ok() const { return m_bOk; }


	protected :
		// == Members.
		/** The destination. */
		CCopySink &												m_csSink;
		/** The output encoding. */
		MX_MODE													m_mMode;
		/** Encoded bytes not yet passed to m_csSink. */
		std::vector<uint8_t>									m_vBuffer;
		/** A high surrogate waiting for its low surrogate (MX_M_HTML only). */
		uint16_t												m_ui16HighSurrogate = 0;
		/** Cleared on the first error. */
		bool													m_bOk = true;


		// == Functions.
		/**
		 * \brief Appends bytes as-is.  Call within a try/catch block.
		 *
		 * \param _pcText The NUL-terminated bytes to append.
		 */
		void													AppendRaw( const char * _pcText ) {
			while ( (*_pcText) ) {
				m_vBuffer.push_back( static_cast<uint8_t>((*_pcText++)) );
			}
		}

		/**
		 * \brief Appends a UTF-16 code unit as UTF-8, joining surrogate pairs.  Call within a try/catch block.
		 *
		 * \param _ui32Unit The UTF-16 code unit.
		 */
		void													AppendUtf8( uint32_t _ui32Unit ) {
			if ( _ui32Unit >= 0xD800 && _ui32Unit <= 0xDBFF ) {
				if ( m_ui16HighSurrogate ) { AppendCodePoint( 0xFFFD ); }
				m_ui16HighSurrogate = static_cast<uint16_t>(_ui32Unit);
				return;
			}
			if ( _ui32Unit >= 0xDC00 && _ui32Unit <= 0xDFFF ) {
				if ( m_ui16HighSurrogate ) {
					AppendCodePoint( 0x10000 + ((uint32_t( m_ui16HighSurrogate ) - 0xD800) << 10) + (_ui32Unit - 0xDC00) );
					m_ui16HighSurrogate = 0;
				}
				else {
					AppendCodePoint( 0xFFFD );
				}
				return;
			}
			if ( m_ui16HighSurrogate ) {
				AppendCodePoint( 0xFFFD );
				m_ui16HighSurrogate = 0;
			}
			AppendCodePoint( _ui32Unit );
		}

		/**
		 * \brief Appends a code point as UTF-8.  Call within a try/catch block.
		 *
		 * \param _ui32Cp The code point.
		 */
		void													AppendCodePoint( uint32_t _ui32Cp ) {
			if ( _ui32Cp < 0x80 ) {
				m_vBuffer.push_back( static_cast<uint8_t>(_ui32Cp) );
			}
			else if ( _ui32Cp < 0x800 ) {
				m_vBuffer.push_back( static_cast<uint8_t>(0xC0 | (_ui32Cp >> 6)) );
				m_vBuffer.push_back( static_cast<uint8_t>(0x80 | (_ui32Cp & 0x3F)) );
			}
			else if ( _ui32Cp < 0x10000 ) {
				m_vBuffer.push_back( static_cast<uint8_t>(0xE0 | (_ui32Cp >> 12)) );
				m_vBuffer.push_back( static_cast<uint8_t>(0x80 | ((_ui32Cp >> 6) & 0x3F)) );
				m_vBuffer.push_back( static_cast<uint8_t>(0x80 | (_ui32Cp & 0x3F)) );
			}
			else {
				m_vBuffer.push_back( static_cast<uint8_t>(0xF0 | (_ui32Cp >> 18)) );
				m_vBuffer.push_back( static_cast<uint8_t>(0x80 | ((_ui32Cp >> 12) & 0x3F)) );
				m_vBuffer.push_back( static_cast<uint8_t>(0x80 | ((_ui32Cp >> 6) & 0x3F)) );
				m_vBuffer.push_back( static_cast<uint8_t>(0x80 | (_ui32Cp & 0x3F)) );
			}
		}

		/**
		 * \brief Flushes once enough bytes are buffered.
		 *
		 * \return Returns false if an error has occurred.
		 */
		inline bool												FlushIfFull() {
			if ( m_vBuffer.size() >= MX_TEXT_BUFFER ) { return Flush(); }
			return m_bOk;
		}
	};


	/**
	 * Class CCopyFormatter::CUtf8ToUtf16Sink
	 * \brief Converts UTF-8 written to it into UTF-16 in another sink (for the text form of HTML and RTF).
	 *
	 * Size() reports UTF-8 bytes consumed.
	 */
	class CCopyFormatter::CUtf8ToUtf16Sink : public CCopySink {
	public :
		CUtf8ToUtf16Sink( CCopySink &_csDst ) :
			m_csDst( _csDst ) {
		}


		// == Functions.
		/**
		 * \brief Converts UTF-8 bytes and appends them to the destination as UTF-16.  Sequences may be split across calls.
		 *
		 * \param _pvData The UTF-8 bytes.
		 * \param _sSize The number of bytes to which _pvData points.
		 * \return Returns false if memory could not be allocated or the destination failed.
		 */
		virtual bool											Write( const void * _pvData, size_t _sSize ) override {
			const uint8_t * pui8Src = static_cast<const uint8_t *>(_pvData);
			try {
				m_vOut.clear();
				m_vOut.reserve( _sSize * 2 );
				for ( size_t I = 0; I < _sSize; ++I ) {
					const uint8_t ui8Byte = pui8Src[I];
					if ( m_ui32Need ) {
						if ( (ui8Byte & 0xC0) == 0x80 ) {
							m_ui32Cp = (m_ui32Cp << 6) | (ui8Byte & 0x3F);
							if ( --m_ui32Need == 0 ) { Emit( m_ui32Cp ); }
							continue;
						}
						// A truncated sequence; this byte starts something new.
						m_ui32Need = 0;
						Emit( 0xFFFD );
					}
					if ( ui8Byte < 0x80 ) { Emit( ui8Byte ); }
					else if ( (ui8Byte & 0xE0) == 0xC0 ) { m_ui32Cp = ui8Byte & 0x1F; m_ui32Need = 1; }
					else if ( (ui8Byte & 0xF0) == 0xE0 ) { m_ui32Cp = ui8Byte & 0x0F; m_ui32Need = 2; }
					else if ( (ui8Byte & 0xF8) == 0xF0 ) { m_ui32Cp = ui8Byte & 0x07; m_ui32Need = 3; }
					else { Emit( 0xFFFD ); }
				}
			}
			catch ( ... ) { return false; }
			if ( m_vOut.size() && !m_csDst.Write( m_vOut.data(), m_vOut.size() ) ) { return false; }
			m_sSize += _sSize;
			return true;
		}

		/**
		 * \brief Gets the number of UTF-8 bytes written so far.
		 *
		 * \return Returns the number of UTF-8 bytes written so far.
		 */
		virtual size_t											Size() const override { return m_sSize; }

		/**
		 * \brief Completes the conversion, replacing an unfinished trailing sequence with U+FFFD.
		 *
		 * \return Returns false if memory could not be allocated or the destination failed.
		 */
		bool													Finish() {
			if ( !m_ui32Need ) { return true; }
			m_ui32Need = 0;
			try {
				m_vOut.clear();
				Emit( 0xFFFD );
			}
			catch ( ... ) { return false; }
			return m_csDst.Write( m_vOut.data(), m_vOut.size() );
		}


	protected :
		// == Members.
		/** The UTF-16 destination. */
		CCopySink &												m_csDst;
		/** UTF-16LE bytes produced by the current Write(). */
		std::vector<uint8_t>									m_vOut;
		/** The number of UTF-8 bytes written. */
		size_t													m_sSize = 0;
		/** The code point being assembled. */
		uint32_t												m_ui32Cp = 0;
		/** The number of continuation bytes still expected. */
		uint32_t												m_ui32Need = 0;


		// == Functions.
		/**
		 * \brief Appends a code point to m_vOut as UTF-16LE.  Call within a try/catch block.
		 *
		 * \param _ui32Cp The code point.  Surrogates and values above U+10FFFF become U+FFFD.
		 */
		void													Emit( uint32_t _ui32Cp ) {
			if ( (_ui32Cp >= 0xD800 && _ui32Cp <= 0xDFFF) || _ui32Cp > 0x10FFFF ) { _ui32Cp = 0xFFFD; }
			if ( _ui32Cp >= 0x10000 ) {
				_ui32Cp -= 0x10000;
				PushUnit( 0xD800 + (_ui32Cp >> 10) );
				PushUnit( 0xDC00 + (_ui32Cp & 0x3FF) );
				return;
			}
			PushUnit( _ui32Cp );
		}

		/**
		 * \brief Appends one UTF-16 code unit to m_vOut, little-endian.  Call within a try/catch block.
		 *
		 * \param _ui32Unit The code unit.
		 */
		void													PushUnit( uint32_t _ui32Unit ) {
			m_vOut.push_back( static_cast<uint8_t>(_ui32Unit) );
			m_vOut.push_back( static_cast<uint8_t>(_ui32Unit >> 8) );
		}
	};


	/**
	 * Class CCopyFormatter::CCountSink
	 * \brief Discards what is written to it, counting the bytes.
	 */
	class CCopyFormatter::CCountSink : public CCopySink {
	public :
		// == Functions.
		/**
		 * \brief Counts bytes without storing them.
		 *
		 * \param _pvData Unused.
		 * \param _sSize The number of bytes written.
		 * \return Returns true.
		 */
		virtual bool											Write( const void * /*_pvData*/, size_t _sSize ) override {
			m_sSize += _sSize;
			return true;
		}

		/**
		 * \brief Gets the number of bytes written so far.
		 *
		 * \return Returns the number of bytes written so far.
		 */
		virtual size_t											Size() const override { return m_sSize; }


	protected :
		// == Members.
		/** The number of bytes written. */
		size_t													m_sSize = 0;
	};

	// == CCopyVectorSink functions.
	/**
	 * \brief Appends bytes to the end of the sink.
	 *
	 * \param _pvData The bytes to append.
	 * \param _sSize The number of bytes to which _pvData points.
	 * \return Returns false if memory could not be allocated.
	 */
	bool CCopyVectorSink::Write( const void * _pvData, size_t _sSize ) {
		try {
			const uint8_t * pui8Data = static_cast<const uint8_t *>(_pvData);
			m_vData.insert( m_vData.end(), pui8Data, pui8Data + _sSize );
			return true;
		}
		catch ( ... ) { return false; }
	}



	// == CCopyGlobalSink functions.
	CCopyGlobalSink::~CCopyGlobalSink() {
		if ( m_hGlobal ) {
			::GlobalFree( m_hGlobal );
			m_hGlobal = NULL;
		}
	}

	/**
	 * \brief Appends bytes to the end of the sink, growing the HGLOBAL as needed.
	 *
	 * \param _pvData The bytes to append.
	 * \param _sSize The number of bytes to which _pvData points.
	 * \return Returns false if memory could not be allocated or locked.
	 */
	bool CCopyGlobalSink::Write( const void * _pvData, size_t _sSize ) {
		if ( !_sSize ) { return true; }
		if ( _sSize > SIZE_MAX - m_sSize ) { return false; }
		const size_t sNeeded = m_sSize + _sSize;
		if ( sNeeded > m_sCapacity ) {
			// Grow geometrically to keep the number of reallocations (and copies) logarithmic.
			size_t sNewCap = std::max<size_t>( sNeeded, std::max<size_t>( m_sCapacity + (m_sCapacity >> 1), 64 * 1024 ) );
			HGLOBAL hNew = m_hGlobal ? ::GlobalReAlloc( m_hGlobal, sNewCap, GMEM_MOVEABLE ) : ::GlobalAlloc( GMEM_MOVEABLE, sNewCap );
			if ( !hNew && sNewCap != sNeeded ) {
				// Try again with only what is required.
				sNewCap = sNeeded;
				hNew = m_hGlobal ? ::GlobalReAlloc( m_hGlobal, sNewCap, GMEM_MOVEABLE ) : ::GlobalAlloc( GMEM_MOVEABLE, sNewCap );
			}
			if ( !hNew ) { return false; }
			m_hGlobal = hNew;
			m_sCapacity = sNewCap;
		}
		uint8_t * pui8Dst = static_cast<uint8_t *>(::GlobalLock( m_hGlobal ));
		if ( !pui8Dst ) { return false; }
		std::memcpy( pui8Dst + m_sSize, _pvData, _sSize );
		::GlobalUnlock( m_hGlobal );
		m_sSize = sNeeded;
		return true;
	}


	/**
	 * \brief Shrinks the HGLOBAL to the written size and transfers ownership of it to the caller.
	 *
	 * After a successful ::SetClipboardData() the system owns the handle; otherwise the caller must ::GlobalFree() it.
	 *
	 * \return Returns the HGLOBAL, or NULL if nothing was written.
	 */
	HGLOBAL CCopyGlobalSink::Release() {
		if ( !m_hGlobal || !m_sSize ) { return NULL; }
		if ( m_sCapacity != m_sSize ) {
			// Formats such as "BinaryData" take their size from ::GlobalSize(), so trim the slack.
			HGLOBAL hNew = ::GlobalReAlloc( m_hGlobal, m_sSize, GMEM_MOVEABLE );
			if ( hNew ) { m_hGlobal = hNew; m_sCapacity = m_sSize; }
		}
		HGLOBAL hRet = m_hGlobal;
		m_hGlobal = NULL;
		m_sSize = m_sCapacity = 0;
		return hRet;
	}


	// == CCopyFormatter functions.
	/**
	 * \brief Gets the registered "BinaryData" clipboard format used for raw-byte copies.
	 *
	 * \return Returns the clipboard-format ID, or 0 on failure.
	 */
	UINT CCopyFormatter::BinaryFormat() {
		static const UINT uiFormat = ::RegisterClipboardFormatW( L"BinaryData" );
		return uiFormat;
	}

	/**
	 * \brief Gets the registered "HTML Format" clipboard format.
	 *
	 * \return Returns the clipboard-format ID, or 0 on failure.
	 */
	UINT CCopyFormatter::HtmlFormat() {
		static const UINT uiFormat = ::RegisterClipboardFormatW( L"HTML Format" );
		return uiFormat;
	}

	/**
	 * \brief Gets the registered "Rich Text Format" clipboard format.
	 *
	 * \return Returns the clipboard-format ID, or 0 on failure.
	 */
	UINT CCopyFormatter::RtfFormat() {
		static const UINT uiFormat = ::RegisterClipboardFormatW( L"Rich Text Format" );
		return uiFormat;
	}

	/**
	 * \brief Gets the clipboard formats offered for a given copy type, most-preferred first.
	 *
	 * \param _caCopyAs The copy parameters.
	 * \param _vFormats Receives the clipboard-format IDs.
	 * \return Returns false on a memory or registration failure.
	 */
	bool CCopyFormatter::Formats( const MX_COPYAS &_caCopyAs, std::vector<UINT> &_vFormats ) {
		try {
			_vFormats.clear();
			switch ( _caCopyAs.caCopyFmt ) {
				case MX_CA_BINARY : {
					if ( !BinaryFormat() ) { return false; }
					_vFormats.push_back( BinaryFormat() );
					// The raw bytes as text, so that plain-text editors can paste them.  Windows synthesizes the other text formats.
					_vFormats.push_back( _caCopyAs.csCharSet == CCharSets::MX_CS_UNICODE ? CF_UNICODETEXT : CF_TEXT );
					break;
				}
				case MX_CA_WEB_PAGE : {
					if ( !HtmlFormat() ) { return false; }
					_vFormats.push_back( HtmlFormat() );
					_vFormats.push_back( CF_UNICODETEXT );
					break;
				}
				case MX_CA_RICH_TEXT : {
					if ( !RtfFormat() ) { return false; }
					_vFormats.push_back( RtfFormat() );
					_vFormats.push_back( CF_UNICODETEXT );
					break;
				}
				default : {
					_vFormats.push_back( CF_UNICODETEXT );
				}
			}
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * \brief Renders a Copy operation into one clipboard format.
	 *
	 * CF_TEXT and CF_UNICODETEXT output is NUL-terminated.  For MX_CA_WEB_PAGE, CF_UNICODETEXT holds the HTML document (without
	 *	the CF_HTML header); for MX_CA_RICH_TEXT it holds the RTF markup.  For MX_CA_BINARY, CF_TEXT and CF_UNICODETEXT hold the raw bytes (readers stop at the first 0).
	 *
	 * \param _csSource The data and parameters of the Copy operation.
	 * \param _uiFormat The clipboard format to produce.  Must be one returned by Formats() for _csSource.caCopyAs.
	 * \param _csSink Receives the rendered bytes.
	 * \param _swsMsg Receives an error message on failure.
	 * \return Returns true if the format was rendered.
	 */
	bool CCopyFormatter::Render( const MX_COPY_SOURCE &_csSource, UINT _uiFormat, CCopySink &_csSink, CSecureWString &_swsMsg ) {
		try {
			bool bRet = false;
			const MX_COPY_AS caFmt = _csSource.caCopyAs.caCopyFmt;
			if ( caFmt == MX_CA_BINARY && _uiFormat == BinaryFormat() ) {
				bRet = RenderBinary( _csSource, _csSink );
			}
			else if ( caFmt == MX_CA_WEB_PAGE && _uiFormat == HtmlFormat() ) {
				bRet = RenderHtml( _csSource, _csSink );
			}
			else if ( caFmt == MX_CA_RICH_TEXT && _uiFormat == RtfFormat() ) {
				bRet = RenderRtf( _csSource, _csSink );
			}
			else if ( caFmt == MX_CA_BINARY && (_uiFormat == CF_TEXT || _uiFormat == CF_UNICODETEXT) ) {
				bRet = RenderBinaryText( _csSource, _csSink, _uiFormat == CF_UNICODETEXT );
			}
			else if ( caFmt == MX_CA_WEB_PAGE && _uiFormat == CF_UNICODETEXT ) {
				// Plain-text editors receive the HTML document itself.
				CUtf8ToUtf16Sink uusSink( _csSink );
				CTextOut toOut( uusSink, CTextOut::MX_M_HTML );
				size_t sFragStart, sFragEnd;
				const uint8_t ui8Nul = 0;
				bRet = RenderHtmlDocument( _csSource, toOut, sFragStart, sFragEnd ) && toOut.Flush() && uusSink.Write( &ui8Nul, 1 ) && uusSink.Finish();
			}
			else if ( caFmt == MX_CA_RICH_TEXT && _uiFormat == CF_UNICODETEXT ) {
				// Plain-text editors receive the RTF markup itself.
				CUtf8ToUtf16Sink uusSink( _csSink );
				bRet = RenderRtf( _csSource, uusSink ) && uusSink.Finish();
			}
			else if ( caFmt != MX_CA_BINARY && _uiFormat == CF_UNICODETEXT ) {
				CTextOut toOut( _csSink, CTextOut::MX_M_UTF16 );
				bRet = RenderText( _csSource, toOut ) && toOut.Put( L'\0' ) && toOut.Flush();
			}
			else {
				_swsMsg = _DEC_WS_1468A1DF_Internal_error_;
				return false;
			}
			if ( !bRet ) {
				_swsMsg = _DEC_WS_F90B4E11_Failed_to_place_the_data_on_the_clipboard_;
			}
			return bRet;
		}
		catch ( ... ) {
			_swsMsg = _DEC_WS_F39F91A5_Out_of_memory_;
			return false;
		}
	}

	/**
	 * \brief Gets the number of selected bytes, clamped to the source size.
	 *
	 * \param _csSource The data and parameters of the Copy operation.
	 * \return Returns the number of bytes that will be rendered.
	 */
	uint64_t CCopyFormatter::TotalBytes( const MX_COPY_SOURCE &_csSource ) {
		const uint64_t ui64Ranges = TotalRanges( _csSource );
		if ( _csSource.sSelection.smMode == MX_SM_NORMAL || !ui64Ranges ) {
			return ui64Ranges ? RangeAt( _csSource, 0 ).ui64Size : 0;
		}
		// Every Column line is the same width except those clipped by the end of the source.
		const MX_SEL_RANGE srFirst = RangeAt( _csSource, 0 );
		const MX_SEL_RANGE srLast = RangeAt( _csSource, ui64Ranges - 1 );
		if ( srLast.ui64Size == srFirst.ui64Size ) { return srFirst.ui64Size * ui64Ranges; }
		uint64_t ui64Total = 0;
		for ( uint64_t I = 0; I < ui64Ranges; ++I ) {
			ui64Total += RangeAt( _csSource, I ).ui64Size;
		}
		return ui64Total;
	}

	/**
	 * \brief Gets the number of ranges in a selection.
	 *
	 * \param _csSource The data and parameters of the Copy operation.
	 * \return Returns 1 for a Normal selection, the number of lines for a Column selection, or 0 for no selection.
	 */
	uint64_t CCopyFormatter::TotalRanges( const MX_COPY_SOURCE &_csSource ) {
		if ( !_csSource.sSelection.HasSelection() ) { return 0; }
		if ( _csSource.sSelection.smMode == MX_SM_NORMAL ) { return 1; }
		return _csSource.sSelection.sc.ui64Lines + 1;
	}

	/**
	 * \brief Gets a selected range, clamped to the source size.  Ranges are returned in ascending address order.
	 *
	 * \param _csSource The data and parameters of the Copy operation.
	 * \param _ui64Idx The index of the range, less than TotalRanges().
	 * \return Returns the range, which may have a size of 0 if it lies beyond the end of the source.
	 */
	MX_SEL_RANGE CCopyFormatter::RangeAt( const MX_COPY_SOURCE &_csSource, uint64_t _ui64Idx ) {
		MX_SEL_RANGE srRange;
		const MX_SELECTION &sSel = _csSource.sSelection;
		if ( sSel.smMode == MX_SM_NORMAL ) {
			srRange.ui64Start = sSel.sn.ui64Start;
			srRange.ui64Size = sSel.sn.ui64End - sSel.sn.ui64Start;
		}
		else {
			const uint32_t ui32Bpr = _csSource.ui32BytesPerRow ? _csSource.ui32BytesPerRow : 16;
			srRange.ui64Start = sSel.sc.ui64AnchorAddr + _ui64Idx * ui32Bpr;
			srRange.ui64Size = std::min( ui32Bpr, sSel.sc.ui32Cols );
		}
		// Clamp to the source.
		if ( srRange.ui64Start >= _csSource.ui64Size ) {
			srRange.ui64Size = 0;
		}
		else {
			srRange.ui64Size = std::min( srRange.ui64Size, _csSource.ui64Size - srRange.ui64Start );
		}
		return srRange;
	}

	/**
	 * \brief Writes every selected byte, in order, to a sink.
	 *
	 * \param _csSource The data and parameters of the Copy operation.
	 * \param _csSink Receives the bytes.
	 * \return Returns false on a read or memory failure.
	 */
	bool CCopyFormatter::RenderBinary( const MX_COPY_SOURCE &_csSource, CCopySink &_csSink ) {
		const uint64_t ui64Ranges = TotalRanges( _csSource );
		for ( uint64_t I = 0; I < ui64Ranges; ++I ) {
			const MX_SEL_RANGE srRange = RangeAt( _csSource, I );
			CByteStream bsStream( _csSource, srRange.ui64Start, srRange.ui64Size );
			const uint8_t * pui8Data = nullptr;
			while ( size_t sGot = bsStream.NextBlock( pui8Data ) ) {
				if ( !_csSink.Write( pui8Data, sGot ) ) { return false; }
			}
			if ( bsStream.Failed() ) { return false; }
		}
		return true;
	}

	/**
	 * \brief Writes every selected byte, in order, followed by a NUL terminator, as the text form of a raw copy.
	 *
	 * \param _csSource The data and parameters of the Copy operation.
	 * \param _csSink Receives the bytes.
	 * \param _bWide If true, the bytes are UTF-16 (CF_UNICODETEXT) and the terminator is a whole 2-byte character; otherwise the
	 *	terminator is 1 byte (CF_TEXT).
	 * \return Returns false on a read or memory failure.
	 */
	bool CCopyFormatter::RenderBinaryText( const MX_COPY_SOURCE &_csSource, CCopySink &_csSink, bool _bWide ) {
		if ( !RenderBinary( _csSource, _csSink ) ) { return false; }
		static const uint8_t ui8Zeros[3] = {};
		if ( !_bWide ) { return _csSink.Write( ui8Zeros, 1 ); }
		// An odd byte count leaves half a character; complete it so the terminator is aligned.
		return _csSink.Write( ui8Zeros, (_csSink.Size() & 1) ? 3 : 2 );
	}

	/**
	 * \brief Writes the textual form of the Copy operation.
	 *
	 * \param _csSource The data and parameters of the Copy operation.
	 * \param _toOut Receives the text.
	 * \return Returns false on a read or memory failure.
	 */
	bool CCopyFormatter::RenderText( const MX_COPY_SOURCE &_csSource, CTextOut &_toOut ) {
		switch ( _csSource.caCopyAs.caCopyFmt ) {
			case MX_CA_HEX_TEXT : {}						MX_FALLTHROUGH
			case MX_CA_DECIMAL_TEXT : {}					MX_FALLTHROUGH
			case MX_CA_BINARY_TEXT : { return RenderByteText( _csSource, _toOut ); }
			case MX_CA_C_CODE : {}							MX_FALLTHROUGH
			case MX_CA_JAVA_CODE : {}						MX_FALLTHROUGH
			case MX_CA_PYTHON_CODE : { return RenderCode( _csSource, _toOut ); }
			case MX_CA_INTEL_8 : {}							MX_FALLTHROUGH
			case MX_CA_INTEL_16 : {}						MX_FALLTHROUGH
			case MX_CA_INTEL_32 : { return RenderIntelHex( _csSource, _toOut ); }
			case MX_CA_MOTOROLA_S19 : {}					MX_FALLTHROUGH
			case MX_CA_MOTOROLA_S28 : {}					MX_FALLTHROUGH
			case MX_CA_MOTOROLA_S37 : { return RenderMotorola( _csSource, _toOut ); }
			case MX_CA_HEX_EDITOR_AREAS : {}				MX_FALLTHROUGH
			case MX_CA_WEB_PAGE : {}						MX_FALLTHROUGH
			case MX_CA_RICH_TEXT : { return RenderHexEditorAreas( _csSource, _toOut, false ); }
			case MX_CA_BASE64 : { return RenderBase64( _csSource, _toOut ); }
			case MX_CA_UUENCODING : { return RenderUuEncoding( _csSource, _toOut ); }
			default : { return RenderValues( _csSource, _toOut ); }
		}
	}

	/**
	 * \brief Writes space-separated byte values (MX_CA_HEX_TEXT, MX_CA_DECIMAL_TEXT, MX_CA_BINARY_TEXT), one row per line.
	 *
	 * \param _csSource The data and parameters of the Copy operation.
	 * \param _toOut Receives the text.
	 * \return Returns false on a read or memory failure.
	 */
	bool CCopyFormatter::RenderByteText( const MX_COPY_SOURCE &_csSource, CTextOut &_toOut ) {
		const MX_COPY_AS caFmt = _csSource.caCopyAs.caCopyFmt;
		const uint32_t ui32Bpr = _csSource.ui32BytesPerRow ? _csSource.ui32BytesPerRow : 16;
		const uint64_t ui64Ranges = TotalRanges( _csSource );
		bool bFirstLine = true;
		wchar_t wcCell[9];
		for ( uint64_t I = 0; I < ui64Ranges; ++I ) {
			const MX_SEL_RANGE srRange = RangeAt( _csSource, I );
			if ( !srRange.ui64Size ) { continue; }
			CByteStream bsStream( _csSource, srRange.ui64Start, srRange.ui64Size );
			uint64_t ui64Col = 0;
			const uint8_t * pui8Data = nullptr;
			while ( size_t sGot = bsStream.NextBlock( pui8Data ) ) {
				for ( size_t J = 0; J < sGot; ++J ) {
					// Each row (and each Column-selection line) starts a new line of text.
					if ( ui64Col == 0 ) {
						if ( !bFirstLine && !_toOut.NewLine() ) { return false; }
						bFirstLine = false;
					}
					else if ( !_toOut.Put( L' ' ) ) { return false; }

					uint8_t ui8Val = pui8Data[J];
					switch ( caFmt ) {
						case MX_CA_DECIMAL_TEXT : {
							// Left-aligned in a 3-character cell: "76 " then the separator.
							size_t sLen = 0;
							if ( ui8Val >= 100 ) { wcCell[sLen++] = wchar_t( L'0' + ui8Val / 100 ); }
							if ( ui8Val >= 10 ) { wcCell[sLen++] = wchar_t( L'0' + (ui8Val / 10) % 10 ); }
							wcCell[sLen++] = wchar_t( L'0' + ui8Val % 10 );
							while ( sLen < 3 ) { wcCell[sLen++] = L' '; }
							if ( !_toOut.Put( wcCell, 3 ) ) { return false; }
							break;
						}
						case MX_CA_BINARY_TEXT : {
							for ( size_t K = 0; K < 8; ++K ) {
								wcCell[7-K] = wchar_t( L'0' + (ui8Val & 1) );
								ui8Val >>= 1;
							}
							if ( !_toOut.Put( wcCell, 8 ) ) { return false; }
							break;
						}
						default : {
							wcCell[0] = wchar_t( MX_HEX_DIGITS[ui8Val>>4] );
							wcCell[1] = wchar_t( MX_HEX_DIGITS[ui8Val&0xF] );
							if ( !_toOut.Put( wcCell, 2 ) ) { return false; }
						}
					}
					if ( ++ui64Col == ui32Bpr ) { ui64Col = 0; }
				}
			}
			if ( bsStream.Failed() ) { return false; }
		}
		return _toOut.Ok();
	}

	/**
	 * \brief Writes a C, Java, or Python byte-array declaration.
	 *
	 * \param _csSource The data and parameters of the Copy operation.
	 * \param _toOut Receives the text.
	 * \return Returns false on a read or memory failure.
	 */
	bool CCopyFormatter::RenderCode( const MX_COPY_SOURCE &_csSource, CTextOut &_toOut ) {
		const MX_COPY_AS caFmt = _csSource.caCopyAs.caCopyFmt;
		const uint32_t ui32Bpr = _csSource.ui32BytesPerRow ? _csSource.ui32BytesPerRow : 16;
		const uint64_t ui64Total = TotalBytes( _csSource );

		// Header.
		switch ( caFmt ) {
			case MX_CA_C_CODE : {
				if ( !_toOut.Put( std::format( "unsigned char hexData[{}] = {{", ui64Total ).c_str() ) ) { return false; }
				break;
			}
			case MX_CA_JAVA_CODE : {
				if ( !_toOut.Put( "byte hexData[] = {" ) ) { return false; }
				break;
			}
			default : {
				if ( !_toOut.Put( "buffer = b''.join([" ) ) { return false; }
			}
		}

		const uint64_t ui64Ranges = TotalRanges( _csSource );
		uint64_t ui64Idx = 0;
		wchar_t wcHex[4] = { L'\\', L'x', 0, 0 };
		for ( uint64_t I = 0; I < ui64Ranges; ++I ) {
			const MX_SEL_RANGE srRange = RangeAt( _csSource, I );
			CByteStream bsStream( _csSource, srRange.ui64Start, srRange.ui64Size );
			const uint8_t * pui8Data = nullptr;
			while ( size_t sGot = bsStream.NextBlock( pui8Data ) ) {
				for ( size_t J = 0; J < sGot; ++J, ++ui64Idx ) {
					const bool bLineStart = (ui64Idx % ui32Bpr) == 0;
					wcHex[2] = wchar_t( MX_HEX_DIGITS[pui8Data[J]>>4] );
					wcHex[3] = wchar_t( MX_HEX_DIGITS[pui8Data[J]&0xF] );
					switch ( caFmt ) {
						case MX_CA_C_CODE : {}				MX_FALLTHROUGH
						case MX_CA_JAVA_CODE : {
							if ( bLineStart ) {
								if ( ui64Idx && !_toOut.Put( L',' ) ) { return false; }
								if ( !_toOut.NewLine() ) { return false; }
								if ( !_toOut.Put( caFmt == MX_CA_C_CODE ? "\t" : "   " ) ) { return false; }
							}
							else if ( !_toOut.Put( ", " ) ) { return false; }
							if ( caFmt == MX_CA_JAVA_CODE && !_toOut.Put( "(byte)" ) ) { return false; }
							if ( !_toOut.Put( "0x" ) || !_toOut.Put( &wcHex[2], 2 ) ) { return false; }
							break;
						}
						default : {
							if ( bLineStart ) {
								if ( ui64Idx && !_toOut.Put( "'," ) ) { return false; }
								if ( !_toOut.NewLine() || !_toOut.Put( "    b'" ) ) { return false; }
							}
							if ( !_toOut.Put( wcHex, 4 ) ) { return false; }
						}
					}
				}
			}
			if ( bsStream.Failed() ) { return false; }
		}

		// Footer.
		if ( caFmt == MX_CA_PYTHON_CODE ) {
			if ( ui64Idx && !_toOut.Put( "'," ) ) { return false; }
			return _toOut.NewLine() && _toOut.Put( "])" );
		}
		return _toOut.NewLine() && _toOut.Put( "};" );
	}

	/**
	 * \brief Writes Intel HEX records (8-, 16-, or 32-bit).
	 *
	 * \param _csSource The data and parameters of the Copy operation.
	 * \param _toOut Receives the text.
	 * \return Returns false on a read or memory failure.
	 */
	bool CCopyFormatter::RenderIntelHex( const MX_COPY_SOURCE &_csSource, CTextOut &_toOut ) {
		const MX_COPY_AS caFmt = _csSource.caCopyAs.caCopyFmt;
		const uint32_t ui32Bpr = _csSource.ui32BytesPerRow ? _csSource.ui32BytesPerRow : 16;
		const size_t sMaxRecord = std::min<size_t>( ui32Bpr, 255 );
		const uint64_t ui64Bias = _csSource.caCopyAs.caAddress.ui64Bias;

		// Writes one record: ":LLAAAATT<data>CC".
		auto WriteRecord = [&]( uint8_t _ui8Type, uint16_t _ui16Addr, const uint8_t * _pui8Data, size_t _sSize ) -> bool {
			uint8_t ui8Sum = static_cast<uint8_t>(_sSize + (_ui16Addr >> 8) + (_ui16Addr & 0xFF) + _ui8Type);
			char szHead[10];
			szHead[0] = ':';
			szHead[1] = MX_HEX_DIGITS[(_sSize>>4)&0xF];		szHead[2] = MX_HEX_DIGITS[_sSize&0xF];
			szHead[3] = MX_HEX_DIGITS[(_ui16Addr>>12)&0xF];	szHead[4] = MX_HEX_DIGITS[(_ui16Addr>>8)&0xF];
			szHead[5] = MX_HEX_DIGITS[(_ui16Addr>>4)&0xF];	szHead[6] = MX_HEX_DIGITS[_ui16Addr&0xF];
			szHead[7] = MX_HEX_DIGITS[_ui8Type>>4];			szHead[8] = MX_HEX_DIGITS[_ui8Type&0xF];
			szHead[9] = '\0';
			if ( !_toOut.Put( szHead ) ) { return false; }
			wchar_t wcByte[2];
			for ( size_t I = 0; I < _sSize; ++I ) {
				ui8Sum = static_cast<uint8_t>(ui8Sum + _pui8Data[I]);
				wcByte[0] = wchar_t( MX_HEX_DIGITS[_pui8Data[I]>>4] );
				wcByte[1] = wchar_t( MX_HEX_DIGITS[_pui8Data[I]&0xF] );
				if ( !_toOut.Put( wcByte, 2 ) ) { return false; }
			}
			const uint8_t ui8Check = static_cast<uint8_t>(0x100 - ui8Sum);
			wcByte[0] = wchar_t( MX_HEX_DIGITS[ui8Check>>4] );
			wcByte[1] = wchar_t( MX_HEX_DIGITS[ui8Check&0xF] );
			return _toOut.Put( wcByte, 2 ) && _toOut.NewLine();
		};

		// The upper address bits most recently announced with a type-02 or type-04 record.
		uint64_t ui64CurUpper = UINT64_MAX;
		std::vector<uint8_t> vRecord;
		try {
			vRecord.resize( sMaxRecord );
		}
		catch ( ... ) { return false; }

		const uint64_t ui64Ranges = TotalRanges( _csSource );
		for ( uint64_t I = 0; I < ui64Ranges; ++I ) {
			const MX_SEL_RANGE srRange = RangeAt( _csSource, I );
			CByteStream bsStream( _csSource, srRange.ui64Start, srRange.ui64Size );
			uint64_t ui64Addr = srRange.ui64Start + ui64Bias;
			while ( bsStream.Remaining() ) {
				// Records never cross a 64-kilobyte boundary.
				const size_t sThis = size_t( std::min<uint64_t>( std::min<uint64_t>( sMaxRecord, bsStream.Remaining() ), 0x10000 - (ui64Addr & 0xFFFF) ) );
				if ( !bsStream.Read( vRecord.data(), sThis ) ) { return false; }

				const uint64_t ui64Upper = ui64Addr >> 16;
				if ( caFmt != MX_CA_INTEL_8 && ui64Upper != ui64CurUpper ) {
					uint8_t ui8Ext[2];
					if ( caFmt == MX_CA_INTEL_16 ) {
						// Extended segment address: the segment is the address bits 19..4 (only the 64-kilobyte-aligned part is used).
						const uint16_t ui16Seg = static_cast<uint16_t>((ui64Upper & 0xF) << 12);
						ui8Ext[0] = static_cast<uint8_t>(ui16Seg >> 8);
						ui8Ext[1] = static_cast<uint8_t>(ui16Seg);
						if ( !WriteRecord( 0x02, 0, ui8Ext, 2 ) ) { return false; }
					}
					else {
						// Extended linear address: the upper 16 bits of a 32-bit address.
						ui8Ext[0] = static_cast<uint8_t>(ui64Upper >> 8);
						ui8Ext[1] = static_cast<uint8_t>(ui64Upper);
						if ( !WriteRecord( 0x04, 0, ui8Ext, 2 ) ) { return false; }
					}
					ui64CurUpper = ui64Upper;
				}
				if ( !WriteRecord( 0x00, static_cast<uint16_t>(ui64Addr), vRecord.data(), sThis ) ) { return false; }
				ui64Addr += sThis;
			}
			if ( bsStream.Failed() ) { return false; }
		}
		// End-of-file record.  No new line after it.
		return _toOut.Put( ":00000001FF" );
	}

	/**
	 * \brief Writes Motorola S-records (S19, S28, or S37).
	 *
	 * \param _csSource The data and parameters of the Copy operation.
	 * \param _toOut Receives the text.
	 * \return Returns false on a read or memory failure.
	 */
	bool CCopyFormatter::RenderMotorola( const MX_COPY_SOURCE &_csSource, CTextOut &_toOut ) {
		const MX_COPY_AS caFmt = _csSource.caCopyAs.caCopyFmt;
		const uint32_t ui32Bpr = _csSource.ui32BytesPerRow ? _csSource.ui32BytesPerRow : 16;
		const uint64_t ui64Bias = _csSource.caCopyAs.caAddress.ui64Bias;
		// S1/S9 use 16-bit addresses, S2/S8 24-bit, S3/S7 32-bit.
		const size_t sAddrBytes = caFmt == MX_CA_MOTOROLA_S19 ? 2 : (caFmt == MX_CA_MOTOROLA_S28 ? 3 : 4);
		const char cDataType = char( '0' + (sAddrBytes - 1) );
		const char cTermType = char( '0' + (11 - sAddrBytes) );
		// The byte count includes the address and checksum and must fit in one byte.
		const size_t sMaxRecord = std::min<size_t>( ui32Bpr, 255 - sAddrBytes - 1 );

		// Writes one record: "S<type><count><address><data><checksum>".
		auto WriteRecord = [&]( char _cType, size_t _sAddrBytes, uint64_t _ui64Addr, const uint8_t * _pui8Data, size_t _sSize ) -> bool {
			const size_t sCount = _sSize + _sAddrBytes + 1;
			uint8_t ui8Sum = static_cast<uint8_t>(sCount);
			wchar_t wcByte[2] = { L'S', wchar_t( _cType ) };
			if ( !_toOut.Put( wcByte, 2 ) ) { return false; }
			auto PutByte = [&]( uint8_t _ui8Val ) -> bool {
				wcByte[0] = wchar_t( MX_HEX_DIGITS[_ui8Val>>4] );
				wcByte[1] = wchar_t( MX_HEX_DIGITS[_ui8Val&0xF] );
				return _toOut.Put( wcByte, 2 );
			};
			if ( !PutByte( static_cast<uint8_t>(sCount) ) ) { return false; }
			for ( size_t I = _sAddrBytes; I--; ) {
				const uint8_t ui8Val = static_cast<uint8_t>(_ui64Addr >> (I * 8));
				ui8Sum = static_cast<uint8_t>(ui8Sum + ui8Val);
				if ( !PutByte( ui8Val ) ) { return false; }
			}
			for ( size_t I = 0; I < _sSize; ++I ) {
				ui8Sum = static_cast<uint8_t>(ui8Sum + _pui8Data[I]);
				if ( !PutByte( _pui8Data[I] ) ) { return false; }
			}
			return PutByte( static_cast<uint8_t>(~ui8Sum) );
		};

		// Header record (S0) carrying "HDR" at address 0.  S0 always uses a 16-bit address.
		static const uint8_t ui8Header[] = { 'H', 'D', 'R' };
		if ( !WriteRecord( '0', 2, 0, ui8Header, sizeof( ui8Header ) ) || !_toOut.NewLine() ) { return false; }

		std::vector<uint8_t> vRecord;
		try {
			vRecord.resize( sMaxRecord );
		}
		catch ( ... ) { return false; }

		const uint64_t ui64Ranges = TotalRanges( _csSource );
		for ( uint64_t I = 0; I < ui64Ranges; ++I ) {
			const MX_SEL_RANGE srRange = RangeAt( _csSource, I );
			CByteStream bsStream( _csSource, srRange.ui64Start, srRange.ui64Size );
			uint64_t ui64Addr = srRange.ui64Start + ui64Bias;
			while ( bsStream.Remaining() ) {
				const size_t sThis = size_t( std::min<uint64_t>( sMaxRecord, bsStream.Remaining() ) );
				if ( !bsStream.Read( vRecord.data(), sThis ) ) { return false; }
				if ( !WriteRecord( cDataType, sAddrBytes, ui64Addr, vRecord.data(), sThis ) || !_toOut.NewLine() ) { return false; }
				ui64Addr += sThis;
			}
			if ( bsStream.Failed() ) { return false; }
		}
		// Termination record with a start address of 0.  No new line after it.
		return WriteRecord( cTermType, sAddrBytes, 0, nullptr, 0 );
	}

	/**
	 * \brief Writes the selection as a single line of Base64.
	 *
	 * \param _csSource The data and parameters of the Copy operation.
	 * \param _toOut Receives the text.
	 * \return Returns false on a read or memory failure.
	 */
	bool CCopyFormatter::RenderBase64( const MX_COPY_SOURCE &_csSource, CTextOut &_toOut ) {
		static constexpr char szAlphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		uint8_t ui8Group[3];
		size_t sInGroup = 0;
		wchar_t wcOut[4];
		auto EmitGroup = [&]( size_t _sBytes ) -> bool {
			const uint32_t ui32Bits = (uint32_t( ui8Group[0] ) << 16) | (uint32_t( _sBytes > 1 ? ui8Group[1] : 0 ) << 8) | uint32_t( _sBytes > 2 ? ui8Group[2] : 0 );
			wcOut[0] = wchar_t( szAlphabet[(ui32Bits>>18)&0x3F] );
			wcOut[1] = wchar_t( szAlphabet[(ui32Bits>>12)&0x3F] );
			wcOut[2] = _sBytes > 1 ? wchar_t( szAlphabet[(ui32Bits>>6)&0x3F] ) : L'=';
			wcOut[3] = _sBytes > 2 ? wchar_t( szAlphabet[ui32Bits&0x3F] ) : L'=';
			return _toOut.Put( wcOut, 4 );
		};

		const uint64_t ui64Ranges = TotalRanges( _csSource );
		for ( uint64_t I = 0; I < ui64Ranges; ++I ) {
			const MX_SEL_RANGE srRange = RangeAt( _csSource, I );
			CByteStream bsStream( _csSource, srRange.ui64Start, srRange.ui64Size );
			const uint8_t * pui8Data = nullptr;
			while ( size_t sGot = bsStream.NextBlock( pui8Data ) ) {
				for ( size_t J = 0; J < sGot; ++J ) {
					ui8Group[sInGroup++] = pui8Data[J];
					if ( sInGroup == 3 ) {
						if ( !EmitGroup( 3 ) ) { return false; }
						sInGroup = 0;
					}
				}
			}
			if ( bsStream.Failed() ) { return false; }
		}
		if ( sInGroup ) { return EmitGroup( sInGroup ); }
		return _toOut.Ok();
	}

	/**
	 * \brief Writes the selection as a uuencoded file.
	 *
	 * \param _csSource The data and parameters of the Copy operation.
	 * \param _toOut Receives the text.
	 * \return Returns false on a read or memory failure.
	 */
	bool CCopyFormatter::RenderUuEncoding( const MX_COPY_SOURCE &_csSource, CTextOut &_toOut ) {
		// 0 is written as '`' rather than ' ' so that lines never end in significant spaces.
		auto UuChar = []( uint32_t _ui32Val ) -> wchar_t {
			_ui32Val &= 0x3F;
			return _ui32Val ? wchar_t( L' ' + _ui32Val ) : L'`';
		};
		uint8_t ui8Line[45];
		size_t sInLine = 0;
		wchar_t wcOut[4];
		auto EmitLine = [&]() -> bool {
			if ( !_toOut.Put( UuChar( uint32_t( sInLine ) ) ) ) { return false; }
			for ( size_t I = 0; I < sInLine; I += 3 ) {
				const uint32_t ui32Bits = (uint32_t( ui8Line[I] ) << 16) |
					(uint32_t( I + 1 < sInLine ? ui8Line[I+1] : 0 ) << 8) |
					uint32_t( I + 2 < sInLine ? ui8Line[I+2] : 0 );
				wcOut[0] = UuChar( ui32Bits >> 18 );
				wcOut[1] = UuChar( ui32Bits >> 12 );
				wcOut[2] = UuChar( ui32Bits >> 6 );
				wcOut[3] = UuChar( ui32Bits );
				if ( !_toOut.Put( wcOut, 4 ) ) { return false; }
			}
			sInLine = 0;
			return _toOut.NewLine();
		};

		if ( !_toOut.Put( "begin 644 " ) ) { return false; }
		if ( !_toOut.Put( _csSource.caCopyAs.wsName.size() ? _csSource.caCopyAs.wsName : std::wstring( L"data.bin" ) ) || !_toOut.NewLine() ) { return false; }

		const uint64_t ui64Ranges = TotalRanges( _csSource );
		for ( uint64_t I = 0; I < ui64Ranges; ++I ) {
			const MX_SEL_RANGE srRange = RangeAt( _csSource, I );
			CByteStream bsStream( _csSource, srRange.ui64Start, srRange.ui64Size );
			const uint8_t * pui8Data = nullptr;
			while ( size_t sGot = bsStream.NextBlock( pui8Data ) ) {
				for ( size_t J = 0; J < sGot; ++J ) {
					ui8Line[sInLine++] = pui8Data[J];
					if ( sInLine == std::size( ui8Line ) && !EmitLine() ) { return false; }
				}
			}
			if ( bsStream.Failed() ) { return false; }
		}
		if ( sInLine && !EmitLine() ) { return false; }
		// A zero-length line, then "end".
		return _toOut.Put( L'`' ) && _toOut.NewLine() && _toOut.Put( "end" );
	}

	/**
	 * \brief Writes values of a primitive type (MX_CA_UINT8 through MX_CA_PCM32), space-separated with one row per line.
	 *
	 * \param _csSource The data and parameters of the Copy operation.
	 * \param _toOut Receives the text.
	 * \return Returns false on a read or memory failure.
	 */
	bool CCopyFormatter::RenderValues( const MX_COPY_SOURCE &_csSource, CTextOut &_toOut ) {
		MX_DATA_FMT dfFmt;
		if ( !ValueFormat( _csSource.caCopyAs.caCopyFmt, dfFmt ) ) { return false; }
		const uint32_t ui32Bpr = _csSource.ui32BytesPerRow ? _csSource.ui32BytesPerRow : 16;
		const uint32_t ui32Stride = StrideForFormat( dfFmt, _csSource.caCopyAs.csCharSet );
		// Values run consecutively through each range (none are lost at row ends); lines break every row's worth of values.
		const uint64_t ui64PerLine = std::max<uint64_t>( ui32Bpr / ui32Stride, 1 );

		std::wstring wsTmp;
		uint8_t ui8Value[8];
		bool bFirstLine = true;
		const uint64_t ui64Ranges = TotalRanges( _csSource );
		for ( uint64_t I = 0; I < ui64Ranges; ++I ) {
			const MX_SEL_RANGE srRange = RangeAt( _csSource, I );
			CByteStream bsStream( _csSource, srRange.ui64Start, srRange.ui64Size );
			uint64_t ui64Col = 0;
			// A trailing partial value is dropped.
			while ( bsStream.Remaining() >= ui32Stride ) {
				if ( !bsStream.Read( ui8Value, ui32Stride ) ) { return false; }
				if ( ui64Col == 0 ) {
					if ( !bFirstLine && !_toOut.NewLine() ) { return false; }
					bFirstLine = false;
				}
				else if ( !_toOut.Put( L' ' ) ) { return false; }
				if ( !FormatValue( dfFmt, ui8Value, _csSource.caCopyAs.bBigEndian, _csSource.caCopyAs.ui32FloatDigits, wsTmp ) ) { return false; }
				if ( !_toOut.Put( wsTmp ) ) { return false; }
				if ( ++ui64Col == ui64PerLine ) { ui64Col = 0; }
			}
			if ( bsStream.Failed() ) { return false; }
		}
		return _toOut.Ok();
	}

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
	bool CCopyFormatter::RenderHexEditorAreas( const MX_COPY_SOURCE &_csSource, CTextOut &_toOut, bool _bNewLineAfterLast ) {
		const MX_COPYAS &caParms = _csSource.caCopyAs;
		const uint32_t ui32Bpr = _csSource.ui32BytesPerRow ? _csSource.ui32BytesPerRow : 16;
		const CCharSets::MX_CHAR_SET &csSet = CCharSets::m_csSets[caParms.csCharSet];

		// Per-area layout, mirroring CHexEditorControl::GetTextRectForIndex().
		/** The layout of one area (left or right). */
		struct MX_AREA {
			MX_DATA_FMT										dfFmt;					/**< The area's data format. */
			uint32_t										ui32Stride;				/**< Bytes per value. */
			uint32_t										ui32GroupSize;			/**< Bytes per group (a multiple of ui32Stride). */
			uint32_t										ui32Width;				/**< Characters per value. */
		};
		auto MakeArea = [&]( MX_DATA_FMT _dfFmt ) -> MX_AREA {
			MX_AREA aArea;
			aArea.dfFmt = _dfFmt;
			aArea.ui32Stride = StrideForFormat( _dfFmt, caParms.csCharSet );
			aArea.ui32GroupSize = (caParms.ui32GroupSize && _dfFmt != MX_DF_CHAR && caParms.ui32GroupSize >= aArea.ui32Stride &&
				caParms.ui32GroupSize % aArea.ui32Stride == 0) ? caParms.ui32GroupSize : aArea.ui32Stride;
			aArea.ui32Width = WidthForFormat( _dfFmt, caParms.ui32FloatDigits );
			return aArea;
		};
		const MX_AREA aLeft = MakeArea( caParms.dfLeft );
		const MX_AREA aRight = MakeArea( caParms.dfRight );

		std::vector<uint8_t> vWindow;							// Cached source bytes.
		uint64_t ui64WindowStart = 0;
		std::vector<uint8_t> vMask;								// 1 for each selected byte of the current row.
		std::wstring wsLine, wsTmp;
		try {
			vMask.resize( ui32Bpr );
		}
		catch ( ... ) { return false; }

		// Appends one area of the current row to wsLine.
		auto AppendArea = [&]( const MX_AREA &_aArea, const uint8_t * _pui8Row, size_t _sAvail ) -> bool {
			int32_t i32Continuation = 0;						// Bytes still belonging to the previous multi-byte character.
			for ( uint32_t J = 0; J < ui32Bpr; J += _aArea.ui32Stride ) {
				const bool bFits = J + _aArea.ui32Stride <= _sAvail && J + _aArea.ui32Stride <= ui32Bpr;
				bool bSel = bFits;
				for ( uint32_t K = 0; bSel && K < _aArea.ui32Stride; ++K ) { bSel = vMask[J+K] != 0; }

				if ( _aArea.dfFmt == MX_DF_CHAR ) {
					if ( i32Continuation > 0 ) {
						// The display leaves these cells empty.
						i32Continuation -= int32_t( _aArea.ui32Stride );
						continue;
					}
					if ( !bSel ) {
						wsLine.push_back( L' ' );
						continue;
					}
					wchar_t wcChar = L'.';
					const uint32_t ui32Len = csSet.pfDisplayable( &_pui8Row[J], _sAvail - J, wcChar );
					if ( wcChar >= 0x80 && csSet.bHideOver127 ) { wcChar = L'.'; }
					wsLine.push_back( wcChar );
					if ( ui32Len > _aArea.ui32Stride ) { i32Continuation = int32_t( ui32Len - _aArea.ui32Stride ); }
					continue;
				}

				if ( J ) {
					// Spaces separate groups; values inside a group touch only for the fixed-width digit formats.
					if ( J % _aArea.ui32GroupSize == 0 ) {
						wsLine.append( caParms.ui32SpacesBetweenGroups, L' ' );
					}
					else if ( _aArea.dfFmt != MX_DF_HEX && _aArea.dfFmt != MX_DF_OCT && _aArea.dfFmt != MX_DF_BIN ) {
						wsLine.push_back( L' ' );
					}
				}
				if ( !bSel ) {
					wsLine.append( _aArea.ui32Width, L' ' );
					continue;
				}
				if ( !FormatValue( _aArea.dfFmt, &_pui8Row[J], caParms.bBigEndian, caParms.ui32FloatDigits, wsTmp ) ) { return false; }
				if ( wsTmp.size() < _aArea.ui32Width ) { wsLine.append( _aArea.ui32Width - wsTmp.size(), L' ' ); }
				wsLine.append( wsTmp );
			}
			return true;
		};

		bool bFirstRow = true;
		// Emits the row at _ui64Row using the selected bytes in vMask.
		auto EmitRow = [&]( uint64_t _ui64Row ) -> bool {
			try {
				// Fetch the row (clipped to the end of the source) through a window of many rows.
				const size_t sAvail = size_t( std::min<uint64_t>( ui32Bpr, _csSource.ui64Size - _ui64Row ) );
				if ( _ui64Row < ui64WindowStart || _ui64Row + sAvail > ui64WindowStart + vWindow.size() ) {
					const uint64_t ui64Want = std::min<uint64_t>( uint64_t( std::max<size_t>( MX_COPY_CHUNK / ui32Bpr, 1 ) ) * ui32Bpr, _csSource.ui64Size - _ui64Row );
					if ( !_csSource.fRead || !_csSource.fRead( _ui64Row, vWindow, size_t( ui64Want ) ) || vWindow.size() < ui64Want ) { return false; }
					ui64WindowStart = _ui64Row;
				}
				const uint8_t * pui8Row = &vWindow[size_t( _ui64Row - ui64WindowStart )];

				wsLine.clear();
				if ( caParms.bShowAddress ) {
					if ( !FormatAddress( caParms.caAddress, _ui64Row, ui32Bpr, wsTmp ) ) { return false; }
					wsLine.append( wsTmp );
					wsLine.append( L"  " );
				}
				if ( caParms.bShowLeft ) {
					if ( !AppendArea( aLeft, pui8Row, sAvail ) ) { return false; }
				}
				if ( caParms.bShowRight ) {
					if ( caParms.bShowLeft ) { wsLine.append( L"  " ); }
					if ( !AppendArea( aRight, pui8Row, sAvail ) ) { return false; }
					wsLine.push_back( L' ' );
				}
			}
			catch ( ... ) { return false; }

			if ( !bFirstRow && !_toOut.NewLine() ) { return false; }
			bFirstRow = false;
			return _toOut.Put( wsLine );
		};

		uint64_t ui64CurRow = UINT64_MAX;
		const uint64_t ui64Ranges = TotalRanges( _csSource );
		for ( uint64_t I = 0; I < ui64Ranges; ++I ) {
			const MX_SEL_RANGE srRange = RangeAt( _csSource, I );
			const uint64_t ui64End = srRange.ui64Start + srRange.ui64Size;
			for ( uint64_t ui64Addr = srRange.ui64Start; ui64Addr < ui64End; ) {
				// Data rows are aligned to 0; the starting-address bias only affects the printed address.
				const uint64_t ui64Row = ui64Addr / ui32Bpr * ui32Bpr;
				if ( ui64Row != ui64CurRow ) {
					if ( ui64CurRow != UINT64_MAX && !EmitRow( ui64CurRow ) ) { return false; }
					ui64CurRow = ui64Row;
					std::fill( vMask.begin(), vMask.end(), uint8_t( 0 ) );
				}
				const uint64_t ui64RowEnd = std::min( ui64Row + ui32Bpr, ui64End );
				std::fill( vMask.begin() + size_t( ui64Addr - ui64Row ), vMask.begin() + size_t( ui64RowEnd - ui64Row ), uint8_t( 1 ) );
				ui64Addr = ui64RowEnd;
			}
		}
		if ( ui64CurRow != UINT64_MAX && !EmitRow( ui64CurRow ) ) { return false; }
		if ( _bNewLineAfterLast && !bFirstRow && !_toOut.NewLine() ) { return false; }
		return _toOut.Ok();
	}

	/**
	 * \brief Writes CF_HTML (UTF-8 with the Version/StartHTML/EndHTML/StartFragment/EndFragment header).
	 *
	 * The document is rendered into memory first so that the header offsets are known before anything is written; the source is
	 *	read only once, so data that changes underneath (process memory) cannot make the offsets wrong.
	 *
	 * \param _csSource The data and parameters of the Copy operation.
	 * \param _csSink Receives the bytes.
	 * \return Returns false on a read or memory failure.
	 */
	bool CCopyFormatter::RenderHtml( const MX_COPY_SOURCE &_csSource, CCopySink &_csSink ) {
		CCopyVectorSink cvsDoc;
		size_t sFragStart, sFragEnd;
		{
			CTextOut toDoc( cvsDoc, CTextOut::MX_M_HTML );
			if ( !RenderHtmlDocument( _csSource, toDoc, sFragStart, sFragEnd ) || !toDoc.Flush() ) { return false; }
		}

		// Every number has 10 digits, so the header length does not depend on the values.
		auto MakeHeader = []( size_t _sStartHtml, size_t _sEndHtml, size_t _sStartFrag, size_t _sEndFrag ) {
			return std::format( "Version:1.0\r\nStartHTML:{:010}\r\nEndHTML:{:010}\r\nStartFragment:{:010}\r\nEndFragment:{:010}\r\n",
				_sStartHtml, _sEndHtml, _sStartFrag, _sEndFrag );
		};
		std::string sHeader;
		try {
			const size_t sHeaderLen = MakeHeader( 0, 0, 0, 0 ).size();
			sHeader = MakeHeader( sHeaderLen, sHeaderLen + cvsDoc.Size(), sHeaderLen + sFragStart, sHeaderLen + sFragEnd );
		}
		catch ( ... ) { return false; }

		// A terminating NUL (outside of EndHTML) for readers that treat the data as a C string.
		const uint8_t ui8Nul = 0;
		return _csSink.Write( sHeader.data(), sHeader.size() ) &&
			_csSink.Write( cvsDoc.Data().data(), cvsDoc.Data().size() ) &&
			_csSink.Write( &ui8Nul, 1 );
	}

	/**
	 * \brief Writes the HTML document "<html><body><pre>...</pre></body></html>" (UTF-8, escaped).
	 *
	 * \param _csSource The data and parameters of the Copy operation.
	 * \param _toOut Receives the text.  Must be in HTML mode.
	 * \param _sFragmentStart Receives the offset (relative to the first byte written here) of the start of the fragment ("<pre>").
	 * \param _sFragmentEnd Receives the offset (relative to the first byte written here) just past the end of the fragment ("</pre>").
	 * \return Returns false on a read or memory failure.
	 */
	bool CCopyFormatter::RenderHtmlDocument( const MX_COPY_SOURCE &_csSource, CTextOut &_toOut, size_t &_sFragmentStart, size_t &_sFragmentEnd ) {
		const size_t sBase = _toOut.Written();
		if ( !_toOut.Raw( "<html>\r\n<body>\r\n" ) ) { return false; }
		_sFragmentStart = _toOut.Written() - sBase;
		if ( !_toOut.Raw( "<pre>\r\n" ) ) { return false; }
		if ( !RenderHexEditorAreas( _csSource, _toOut, true ) ) { return false; }
		if ( !_toOut.Raw( "</pre>" ) ) { return false; }
		_sFragmentEnd = _toOut.Written() - sBase;
		return _toOut.Raw( "\r\n</body>\r\n</html>\r\n" );
	}

	/**
	 * \brief Writes Rich Text Format.
	 *
	 * \param _csSource The data and parameters of the Copy operation.
	 * \param _csSink Receives the bytes.
	 * \return Returns false on a read or memory failure.
	 */
	bool CCopyFormatter::RenderRtf( const MX_COPY_SOURCE &_csSource, CCopySink &_csSink ) {
		CTextOut toOut( _csSink, CTextOut::MX_M_RTF );
		if ( !toOut.Raw( "{\\rtf1\\ansi\\ansicpg1252\\deff0\\deflang1033{\\fonttbl{\\f0\\fmodern\\fprq1\\fcharset0 Courier New;}}\r\n"
			"\\viewkind4\\uc1\\pard\\f0\\fs20 " ) ) { return false; }
		if ( !RenderHexEditorAreas( _csSource, toOut, true ) ) { return false; }
		if ( !toOut.Raw( "}" ) || !toOut.Flush() ) { return false; }
		const uint8_t ui8Nul = 0;
		return _csSink.Write( &ui8Nul, 1 );
	}

	/**
	 * \brief Gets the primitive data format that corresponds to an MX_CA_UINT8-through-MX_CA_PCM32 copy type.
	 *
	 * \param _caFormat The copy type.
	 * \param _dfFormat Receives the corresponding data format.
	 * \return Returns false if _caFormat is not a primitive-value copy type.
	 */
	bool CCopyFormatter::ValueFormat( MX_COPY_AS _caFormat, MX_DATA_FMT &_dfFormat ) {
		switch ( _caFormat ) {
			case MX_CA_UINT8 : { _dfFormat = MX_DF_UINT8; return true; }
			case MX_CA_INT8 : { _dfFormat = MX_DF_INT8; return true; }
			case MX_CA_UINT16 : { _dfFormat = MX_DF_UINT16; return true; }
			case MX_CA_INT16 : { _dfFormat = MX_DF_INT16; return true; }
			case MX_CA_UINT32 : { _dfFormat = MX_DF_UINT32; return true; }
			case MX_CA_INT32 : { _dfFormat = MX_DF_INT32; return true; }
			case MX_CA_UINT64 : { _dfFormat = MX_DF_UINT64; return true; }
			case MX_CA_INT64 : { _dfFormat = MX_DF_INT64; return true; }
			case MX_CA_P16 : { _dfFormat = MX_DF_POINTER16; return true; }
			case MX_CA_P32 : { _dfFormat = MX_DF_POINTER32; return true; }
			case MX_CA_P64 : { _dfFormat = MX_DF_POINTER64; return true; }
			case MX_CA_FLOAT10 : { _dfFormat = MX_DF_FLOAT10; return true; }
			case MX_CA_FLOAT11 : { _dfFormat = MX_DF_FLOAT11; return true; }
			case MX_CA_FLOAT16 : { _dfFormat = MX_DF_FLOAT16; return true; }
			case MX_CA_FLOAT32 : { _dfFormat = MX_DF_FLOAT32; return true; }
			case MX_CA_FLOAT64 : { _dfFormat = MX_DF_FLOAT64; return true; }
			case MX_CA_BFLOAT : { _dfFormat = MX_DF_BFLOAT16; return true; }
			case MX_CA_PCM8 : { _dfFormat = MX_DF_PCM8; return true; }
			case MX_CA_PCM16 : { _dfFormat = MX_DF_PCM16; return true; }
			case MX_CA_PCM24 : { _dfFormat = MX_DF_PCM24; return true; }
			case MX_CA_PCM32 : { _dfFormat = MX_DF_PCM32; return true; }
			default : { return false; }
		}
	}

	/**
	 * \brief Gets the number of bytes consumed by one value of a data format.
	 *
	 * \param _dfFormat The data format.
	 * \param _csCharSet The character set (MX_DF_CHAR in Unicode consumes 2 bytes).
	 * \return Returns the number of bytes per value.
	 */
	uint32_t CCopyFormatter::StrideForFormat( MX_DATA_FMT _dfFormat, CCharSets::MX_CHAR_SETS _csCharSet ) {
		switch ( _dfFormat ) {
			case MX_DF_CHAR : { return (_csCharSet == CCharSets::MX_CS_UNICODE) ? 2 : 1; }
			case MX_DF_UINT16 : {}							MX_FALLTHROUGH
			case MX_DF_INT16 : {}							MX_FALLTHROUGH
			case MX_DF_POINTER16 : {}						MX_FALLTHROUGH
			case MX_DF_FLOAT10 : {}							MX_FALLTHROUGH
			case MX_DF_FLOAT11 : {}							MX_FALLTHROUGH
			case MX_DF_BFLOAT16 : {}						MX_FALLTHROUGH
			case MX_DF_FLOAT16 : {}							MX_FALLTHROUGH
			case MX_DF_PCM16 : { return 2; }
			case MX_DF_PCM24 : { return 3; }
			case MX_DF_UINT32 : {}							MX_FALLTHROUGH
			case MX_DF_INT32 : {}							MX_FALLTHROUGH
			case MX_DF_POINTER32 : {}						MX_FALLTHROUGH
			case MX_DF_FLOAT32 : {}							MX_FALLTHROUGH
			case MX_DF_PCM32 : { return 4; }
			case MX_DF_UINT64 : {}							MX_FALLTHROUGH
			case MX_DF_INT64 : {}							MX_FALLTHROUGH
			case MX_DF_POINTER64 : {}						MX_FALLTHROUGH
			case MX_DF_FLOAT64 : { return 8; }
			default : { return 1; }
		}
	}

	/**
	 * \brief Gets the fixed text width of one value of a data format, used to keep the Hex Editor Areas columns aligned.
	 *
	 * \param _dfFormat The data format.
	 * \param _ui32FloatDigits The number of floating-point digits.
	 * \return Returns the width in characters.
	 */
	uint32_t CCopyFormatter::WidthForFormat( MX_DATA_FMT _dfFormat, uint32_t _ui32FloatDigits ) {
		switch ( _dfFormat ) {
			case MX_DF_HEX : { return 2; }
			case MX_DF_DEC : {}								MX_FALLTHROUGH
			case MX_DF_OCT : {}								MX_FALLTHROUGH
			case MX_DF_UINT8 : { return 3; }
			case MX_DF_BIN : { return 8; }
			case MX_DF_CHAR : { return 1; }
			case MX_DF_INT8 : {}							MX_FALLTHROUGH
			case MX_DF_POINTER16 : { return 4; }
			case MX_DF_UINT16 : { return 5; }
			case MX_DF_INT16 : { return 6; }
			case MX_DF_POINTER32 : { return 8; }
			case MX_DF_UINT32 : { return 10; }
			case MX_DF_INT32 : { return 11; }
			case MX_DF_POINTER64 : { return 16; }
			case MX_DF_UINT64 : {}							MX_FALLTHROUGH
			case MX_DF_INT64 : { return 20; }
			case MX_DF_PCM8 : {}							MX_FALLTHROUGH
			case MX_DF_PCM16 : {}							MX_FALLTHROUGH
			case MX_DF_PCM24 : {}							MX_FALLTHROUGH
			case MX_DF_PCM32 : { return _ui32FloatDigits + 3; }		// "-1." + digits.
			default : { return _ui32FloatDigits + 7; }				// Sign, '.', and "e+308".
		}
	}

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
	bool CCopyFormatter::FormatValue( MX_DATA_FMT _dfFormat, const uint8_t * _pui8Data, bool _bBigEndian, uint32_t _ui32FloatDigits, std::wstring &_wsOut ) {
		try {
			const uint8_t ui8Val = (*_pui8Data);
			switch ( _dfFormat ) {
				case MX_DF_HEX : {
					_wsOut.assign( 2, L'0' );
					_wsOut[0] = wchar_t( MX_HEX_DIGITS[ui8Val>>4] );
					_wsOut[1] = wchar_t( MX_HEX_DIGITS[ui8Val&0xF] );
					break;
				}
				case MX_DF_DEC : { _wsOut = std::format( L"{:03}", ui8Val ); break; }
				case MX_DF_OCT : { _wsOut = std::format( L"{:03o}", ui8Val ); break; }
				case MX_DF_BIN : { _wsOut = std::format( L"{:08b}", ui8Val ); break; }
				case MX_DF_UINT8 : { _wsOut = std::to_wstring( ui8Val ); break; }
				case MX_DF_INT8 : { _wsOut = std::to_wstring( static_cast<int8_t>(ui8Val) ); break; }
				case MX_DF_UINT16 : { _wsOut = std::to_wstring( static_cast<uint16_t>(Assemble( _pui8Data, 2, _bBigEndian )) ); break; }
				case MX_DF_INT16 : { _wsOut = std::to_wstring( static_cast<int16_t>(Assemble( _pui8Data, 2, _bBigEndian )) ); break; }
				case MX_DF_UINT32 : { _wsOut = std::to_wstring( static_cast<uint32_t>(Assemble( _pui8Data, 4, _bBigEndian )) ); break; }
				case MX_DF_INT32 : { _wsOut = std::to_wstring( static_cast<int32_t>(Assemble( _pui8Data, 4, _bBigEndian )) ); break; }
				case MX_DF_UINT64 : { _wsOut = std::to_wstring( Assemble( _pui8Data, 8, _bBigEndian ) ); break; }
				case MX_DF_INT64 : { _wsOut = std::to_wstring( static_cast<int64_t>(Assemble( _pui8Data, 8, _bBigEndian )) ); break; }
				case MX_DF_POINTER16 : { _wsOut = std::format( L"{:04X}", Assemble( _pui8Data, 2, _bBigEndian ) ); break; }
				case MX_DF_POINTER32 : { _wsOut = std::format( L"{:08X}", Assemble( _pui8Data, 4, _bBigEndian ) ); break; }
				case MX_DF_POINTER64 : { _wsOut = std::format( L"{:016X}", Assemble( _pui8Data, 8, _bBigEndian ) ); break; }
				case MX_DF_FLOAT10 : {
					ee::CFloatX fFloat;
					fFloat.CreateFromBits( Assemble( _pui8Data, 2, _bBigEndian ), 5, 6, true, false );
					_wsOut = std::format( L"{1:.{0}g}", _ui32FloatDigits, fFloat.AsDouble() );
					break;
				}
				case MX_DF_FLOAT11 : {
					ee::CFloatX fFloat;
					fFloat.CreateFromBits( Assemble( _pui8Data, 2, _bBigEndian ), 5, 7, true, false );
					_wsOut = std::format( L"{1:.{0}g}", _ui32FloatDigits, fFloat.AsDouble() );
					break;
				}
				case MX_DF_BFLOAT16 : {
					ee::CFloatX fFloat;
					fFloat.CreateFromBits( Assemble( _pui8Data, 2, _bBigEndian ), 8, 8, true, true );
					_wsOut = std::format( L"{1:.{0}g}", _ui32FloatDigits, fFloat.AsDouble() );
					break;
				}
				case MX_DF_FLOAT16 : {
					ee::CFloat16 fFloat( static_cast<uint16_t>(Assemble( _pui8Data, 2, _bBigEndian )) );
					_wsOut = std::format( L"{1:.{0}g}", _ui32FloatDigits, double( fFloat ) );
					break;
				}
				case MX_DF_FLOAT32 : {
					const uint32_t ui32Bits = static_cast<uint32_t>(Assemble( _pui8Data, 4, _bBigEndian ));
					float fVal;
					std::memcpy( &fVal, &ui32Bits, sizeof( fVal ) );
					_wsOut = std::format( L"{1:.{0}g}", _ui32FloatDigits, fVal );
					break;
				}
				case MX_DF_FLOAT64 : {
					const uint64_t ui64Bits = Assemble( _pui8Data, 8, _bBigEndian );
					double dVal;
					std::memcpy( &dVal, &ui64Bits, sizeof( dVal ) );
					_wsOut = std::format( L"{1:.{0}g}", _ui32FloatDigits, dVal );
					break;
				}
				case MX_DF_PCM8 : {
					// Unsigned 8-bit PCM, centered on 128.
					const double dV = std::clamp( (static_cast<int32_t>(ui8Val) - 128) / 127.0, -1.0, 1.0 );
					_wsOut = std::format( L"{1:.{0}f}", _ui32FloatDigits, dV );
					break;
				}
				case MX_DF_PCM16 : {
					const double dV = std::clamp( double( static_cast<int16_t>(Assemble( _pui8Data, 2, _bBigEndian )) ) / 32767.0, -1.0, 1.0 );
					_wsOut = std::format( L"{1:.{0}f}", _ui32FloatDigits, dV );
					break;
				}
				case MX_DF_PCM24 : {
					// Shift into the top of an int32_t to sign-extend.
					const int32_t i32Val = static_cast<int32_t>(static_cast<uint32_t>(Assemble( _pui8Data, 3, _bBigEndian )) << 8);
					const double dV = std::clamp( double( i32Val ) / 2147483392.0, -1.0, 1.0 );
					_wsOut = std::format( L"{1:.{0}f}", _ui32FloatDigits, dV );
					break;
				}
				case MX_DF_PCM32 : {
					const double dV = std::clamp( double( static_cast<int32_t>(Assemble( _pui8Data, 4, _bBigEndian )) ) / 2147483647.0, -1.0, 1.0 );
					_wsOut = std::format( L"{1:.{0}f}", _ui32FloatDigits, dV );
					break;
				}
				default : {
					_wsOut.assign( 1, L'.' );
				}
			}
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * \brief Formats a row address exactly as the address gutter does.
	 *
	 * \param _caAddr The captured address-gutter settings.
	 * \param _ui64RowAddr The data address of the start of the row.
	 * \param _ui32BytesPerRow The bytes-per-row.
	 * \param _wsOut Receives the text (replacing its contents).
	 * \return Returns false on a memory failure.
	 */
	bool CCopyFormatter::FormatAddress( const MX_COPY_ADDR &_caAddr, uint64_t _ui64RowAddr, uint32_t _ui32BytesPerRow, std::wstring &_wsOut ) {
		try {
			const uint64_t ui64Byte = _caAddr.ui64Bias + _ui64RowAddr;
			const uint64_t ui64Line = _ui64RowAddr / (_ui32BytesPerRow ? _ui32BytesPerRow : 16);
			uint64_t ui64Val = ui64Byte;
			uint32_t ui32Base = 16;
			const auto afFmt = static_cast<CHexEditorControl::MX_ADDRESS_FMT>(_caAddr.ui32Format);
			switch ( afFmt ) {
				case CHexEditorControl::MX_AF_BYTES_DEC : { ui32Base = 10; break; }
				case CHexEditorControl::MX_AF_BYTES_OCT : { ui32Base = 8; break; }
				case CHexEditorControl::MX_AF_LINE_NUMBER : { ui64Val = ui64Line + 1; ui32Base = 10; break; }
				case CHexEditorControl::MX_AF_SHORT_HEX : { ui64Val = ui64Byte >> 1; break; }
				case CHexEditorControl::MX_AF_SHORT_DEC : { ui64Val = ui64Byte >> 1; ui32Base = 10; break; }
				default : {}
			}

			// Digits.
			_wsOut.clear();
			do {
				const uint32_t ui32D = static_cast<uint32_t>(ui64Val % ui32Base);
				_wsOut.push_back( ui32D < 10 ? wchar_t( L'0' + ui32D ) : wchar_t( (_caAddr.bLowercaseHex ? L'a' : L'A') + ui32D - 10 ) );
				ui64Val /= ui32Base;
			} while ( ui64Val );
			std::reverse( _wsOut.begin(), _wsOut.end() );

			// Hex/octal pad with '0'; decimal pads with spaces.
			if ( _wsOut.size() < _caAddr.ui32MinDigits ) {
				_wsOut.insert( _wsOut.begin(), _caAddr.ui32MinDigits - _wsOut.size(), ui32Base == 10 ? L' ' : L'0' );
			}

			// ':' every 4 hex digits.
			if ( ui32Base == 16 && _caAddr.bShowColonIn && _wsOut.size() > 4 ) {
				std::wstring wsGrouped;
				size_t sCnt = 0;
				for ( size_t I = _wsOut.size(); I--; ) {
					wsGrouped.push_back( _wsOut[I] );
					if ( (++sCnt % 4) == 0 && I != 0 ) { wsGrouped.push_back( L':' ); }
				}
				std::reverse( wsGrouped.begin(), wsGrouped.end() );
				_wsOut.swap( wsGrouped );
			}
			if ( _caAddr.bShowColonAfter ) { _wsOut.push_back( L':' ); }
			if ( _caAddr.bShowTypeSpec ) {
				if ( ui32Base == 16 ) { _wsOut.push_back( L'h' ); }
				else if ( ui32Base == 8 ) { _wsOut.push_back( L'o' ); }
			}
			if ( _caAddr.bUseShortSuffixW && (afFmt == CHexEditorControl::MX_AF_SHORT_HEX || afFmt == CHexEditorControl::MX_AF_SHORT_DEC) ) {
				_wsOut.push_back( L'w' );
			}
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * \brief Assembles an unsigned value from bytes.
	 *
	 * \param _pui8Data The bytes.
	 * \param _sSize The number of bytes (1 to 8).
	 * \param _bBigEndian If true, the bytes are big-endian.
	 * \return Returns the assembled value.
	 */
	uint64_t CCopyFormatter::Assemble( const uint8_t * _pui8Data, size_t _sSize, bool _bBigEndian ) {
		uint64_t ui64Val = 0;
		for ( size_t I = 0; I < _sSize; ++I ) {
			const uint64_t ui64Byte = _pui8Data[_bBigEndian ? I : (_sSize - 1 - I)];
			ui64Val = (ui64Val << 8) | ui64Byte;
		}
		return ui64Val;
	}


	// == CCopyCapture functions.
	/**
	 * \brief Reads every selected byte from the source and stores it, replacing any previous capture.
	 *
	 * \param _csSource The data and parameters of the Copy operation.  _csSource.fRead is used only during this call.
	 * \param _swsMsg Receives an error message on failure.
	 * \return Returns false on a read or memory failure, in which case the capture is empty.
	 */
	bool CCopyCapture::Capture( const CCopyFormatter::MX_COPY_SOURCE &_csSource, CSecureWString &_swsMsg ) {
		Reset();
		try {
			const uint64_t ui64Total = CCopyFormatter::TotalBytes( _csSource );
			if ( ui64Total > SIZE_MAX ) {
				_swsMsg = _DEC_WS_F39F91A5_Out_of_memory_;
				return false;
			}
			m_vData.reserve( size_t( ui64Total ) );

			std::vector<uint8_t> vChunk;
			const uint64_t ui64Ranges = CCopyFormatter::TotalRanges( _csSource );
			for ( uint64_t I = 0; I < ui64Ranges; ++I ) {
				const MX_SEL_RANGE srRange = CCopyFormatter::RangeAt( _csSource, I );
				if ( !srRange.ui64Size ) { continue; }
				m_vRanges.push_back( srRange );
				m_vOffsets.push_back( m_vData.size() );
				for ( uint64_t ui64Done = 0; ui64Done < srRange.ui64Size; ) {
					const size_t sChunk = size_t( std::min<uint64_t>( srRange.ui64Size - ui64Done, MX_COPY_CHUNK ) );
					if ( !_csSource.fRead || !_csSource.fRead( srRange.ui64Start + ui64Done, vChunk, sChunk ) || vChunk.size() < sChunk ) {
						Reset();
						_swsMsg = _DEC_WS_6C7C47A0_Failed_to_read_the_selected_data_;
						return false;
					}
					m_vData.insert( m_vData.end(), vChunk.begin(), vChunk.begin() + sChunk );
					ui64Done += sChunk;
				}
			}
			m_csSource = _csSource;
			m_csSource.fRead = nullptr;
			return true;
		}
		catch ( ... ) {
			Reset();
			_swsMsg = _DEC_WS_F39F91A5_Out_of_memory_;
			return false;
		}
	}

	/**
	 * \brief Renders the captured Copy operation into one clipboard format.
	 *
	 * \param _uiFormat The clipboard format to produce.
	 * \param _csSink Receives the rendered bytes.
	 * \param _swsMsg Receives an error message on failure.
	 * \return Returns false if nothing is captured or rendering fails.
	 */
	bool CCopyCapture::Render( UINT _uiFormat, CCopySink &_csSink, CSecureWString &_swsMsg ) const {
		if ( Empty() ) {
			_swsMsg = _DEC_WS_1468A1DF_Internal_error_;
			return false;
		}
		try {
			CCopyFormatter::MX_COPY_SOURCE csSource = m_csSource;
			csSource.fRead = [this]( uint64_t _ui64Addr, std::vector<uint8_t> &_vDst, size_t _sSize ) {
				return Read( _ui64Addr, _vDst, _sSize );
			};
			return CCopyFormatter::Render( csSource, _uiFormat, _csSink, _swsMsg );
		}
		catch ( ... ) {
			_swsMsg = _DEC_WS_F39F91A5_Out_of_memory_;
			return false;
		}
	}

	/**
	 * \brief Discards the captured bytes.
	 */
	void CCopyCapture::Reset() {
		m_vRanges = std::vector<MX_SEL_RANGE>();
		m_vOffsets = std::vector<uint64_t>();
		m_vData = std::vector<uint8_t>();
		m_csSource = CCopyFormatter::MX_COPY_SOURCE();
	}

	/**
	 * \brief Reads captured bytes.  Bytes that were not selected read as 0.
	 *
	 * \param _ui64Addr The address of the first byte to read.
	 * \param _vDst Receives the bytes.
	 * \param _sSize The number of bytes to read.
	 * \return Returns false on a memory failure.
	 */
	bool CCopyCapture::Read( uint64_t _ui64Addr, std::vector<uint8_t> &_vDst, size_t _sSize ) const {
		try {
			_vDst.assign( _sSize, 0 );
		}
		catch ( ... ) { return false; }
		const uint64_t ui64End = _ui64Addr + _sSize;
		// Find the first range that ends after _ui64Addr.  Ranges are ascending and do not overlap.
		auto aIt = std::upper_bound( m_vRanges.begin(), m_vRanges.end(), _ui64Addr,
			[]( uint64_t _ui64Val, const MX_SEL_RANGE &_srRange ) { return _ui64Val < _srRange.ui64Start; } );
		if ( aIt != m_vRanges.begin() ) { --aIt; }
		for ( ; aIt != m_vRanges.end() && (*aIt).ui64Start < ui64End; ++aIt ) {
			const uint64_t ui64RangeEnd = (*aIt).ui64Start + (*aIt).ui64Size;
			const uint64_t ui64From = std::max( _ui64Addr, (*aIt).ui64Start );
			const uint64_t ui64To = std::min( ui64End, ui64RangeEnd );
			if ( ui64From >= ui64To ) { continue; }
			const uint64_t ui64Src = m_vOffsets[size_t( aIt - m_vRanges.begin() )] + (ui64From - (*aIt).ui64Start);
			std::memcpy( _vDst.data() + size_t( ui64From - _ui64Addr ), m_vData.data() + size_t( ui64Src ), size_t( ui64To - ui64From ) );
		}
		return true;
	}

}	// namespace mx
