/**
 * Copyright L. Spiro 2023
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A very simple byte stream implementation.
 */


#pragma once

#include "../MXMhsX.h"
#include "../Strings/MXSecureString.h"
#include "MXUtilities.h"
#include <vector>


namespace mx {

	/**
	 * Class CStream
	 * \brief A very simple byte stream implementation.
	 *
	 * Description: A very simple byte stream implementation.
	 */
	class CStream {
	public :
		CStream( std::vector<uint8_t> &_vStream ) :
			m_vStream( _vStream ),
			m_stPos( 0 ) {
		}


		// == Functions.
		// ========
		// READING
		// ========
		/**
		 * Reads a uint8_t from the stream.
		 *
		 * \param _ui8Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		inline bool									ReadUi8( uint8_t &_ui8Value ) {
			return Read( _ui8Value );
		}

		/**
		 * Reads a uint16_t from the stream.
		 *
		 * \param _ui16Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		inline bool									ReadUi16( uint16_t &_ui16Value ) {
			return Read( _ui16Value );
		}

		/**
		 * Reads a uint32_t from the stream.
		 *
		 * \param _ui32Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		inline bool									ReadUi32( uint32_t &_ui32Value ) {
			return Read( _ui32Value );
		}

		/**
		 * Reads a uint64_t from the stream.
		 *
		 * \param _ui64Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		inline bool									ReadUi64( uint64_t &_ui64Value ) {
			return Read( _ui64Value );
		}

		/**
		 * Reads an int8_t from the stream.
		 *
		 * \param _i8Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		inline bool									ReadI8( int8_t &_i8Value ) {
			return Read( _i8Value );
		}

		/**
		 * Reads an int16_t from the stream.
		 *
		 * \param _i16Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		inline bool									ReadI16( int16_t &_i16Value ) {
			return Read( _i16Value );
		}

		/**
		 * Reads an int32_t from the stream.
		 *
		 * \param _i32Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		inline bool									ReadI32( int32_t &_i32Value ) {
			return Read( _i32Value );
		}

		/**
		 * Reads an int64_t from the stream.
		 *
		 * \param _i64Value Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		inline bool									ReadI64( int64_t &_i64Value ) {
			return Read( _i64Value );
		}

		/**
		 * Reads a float from the stream.
		 *
		 * \param _fValue Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		inline bool									ReadF32( float &_fValue ) {
			return Read( _fValue );
		}

		/**
		 * Reads a double from the stream.
		 *
		 * \param _dValue Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		inline bool									ReadF64( double &_dValue ) {
			return Read( _dValue );
		}

		/**
		 * Reads an bool from the stream.
		 *
		 * \param _bValue Holds the return value.
		 * \return Returns true if there was enough space left in the stream to read the value.
		 */
		inline bool									ReadBool( bool &_bValue ) {
			return Read<bool>( _bValue );
		}

		/**
		 * Reads a char string from the stream.
		 * 
		 * \param _sString Holds the return string.
		 * \return Returns true if there was enough space left in the stream to read the string.
		 **/
		inline bool									ReadString( CSecureString &_sString ) {
			_sString.clear();
			uint32_t ui32Len;
			if ( !ReadUi32( ui32Len ) ) { return false; }
			try {
				for ( size_t I = 0; I < ui32Len; ++I ) {
					uint8_t ui8Tmp;
					if ( !ReadUi8( ui8Tmp ) ) { return false; }
					_sString.push_back( char( ui8Tmp ) );
				}
			}
			catch( ... ) {
				return false;
			}
			return true;
		}

		/**
		 * REads a u16char_t string from the stream.
		 * 
		 * \param _wsString Holds the return string.
		 * \return Returns true if there was enough space left in the stream to write the string.
		 **/
		inline bool									ReadStringU16( CSecureWString &_wsString ) {
			CSecureString sTmp;
			if ( !ReadString( sTmp ) ) { return false; }
			DWORD dwError;
			_wsString = CUtilities::MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, sTmp, &dwError );
			return ERROR_SUCCESS == dwError;
		}


		// ========
		// WRITING
		// ========
		/**
		 * Writes a uint8_t to the stream.
		 *
		 * \param _ui8Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		inline bool									WriteUi8( uint8_t _ui8Value ) {
			return Write( _ui8Value );
		}

		/**
		 * Writes a uint16_t to the stream.
		 *
		 * \param _ui16Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		inline bool									WriteUi16( uint16_t _ui16Value ) {
			return Write( _ui16Value );
		}

		/**
		 * Writes a uint32_t to the stream.
		 *
		 * \param _ui32Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		inline bool									WriteUi32( uint32_t _ui32Value ) {
			return Write( _ui32Value );
		}

		/**
		 * Writes a uint64_t to the stream.
		 *
		 * \param _ui64Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		inline bool									WriteUi64( uint64_t _ui64Value ) {
			return Write( _ui64Value );
		}

		/**
		 * Writes an int8_t to the stream.
		 *
		 * \param _i8Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		inline bool									WriteI8( int8_t _i8Value ) {
			return Write( _i8Value );
		}

		/**
		 * Writes an int16_t to the stream.
		 *
		 * \param _i16Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		inline bool									WriteI16( int16_t _i16Value ) {
			return Write( _i16Value );
		}

		/**
		 * Writes an int32_t to the stream.
		 *
		 * \param _i32Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		inline bool									WriteI32( int32_t _i32Value ) {
			return Write( _i32Value );
		}

		/**
		 * Writes an int64_t to the stream.
		 *
		 * \param _i64Value The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		inline bool									WriteI64( int64_t _i64Value ) {
			return Write( _i64Value );
		}

		/**
		 * Writes a float to the stream.
		 *
		 * \param _fValue The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		inline bool									WriteF32( float _fValue ) {
			return Write( _fValue );
		}

		/**
		 * Writes a double to the stream.
		 *
		 * \param _dValue The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		inline bool									WriteF64( double _dValue ) {
			return Write( _dValue );
		}

		/**
		 * Writes an bool to the stream.
		 *
		 * \param _bValue The value to write.
		 * \return Returns true if there was enough space left in the stream to write the value.
		 */
		inline bool									WriteBool( bool _bValue ) {
			return Write<bool>( _bValue );
		}

		/**
		 * Writes a char string to the stream.
		 * 
		 * \param _sString The string to write.
		 * \return Returns true if there was enough space left in the stream to write the string.
		 **/
		inline bool									WriteString( const CSecureString &_sString ) {
			if constexpr ( sizeof( _sString.size() ) > sizeof( uint32_t ) ) {
				if ( _sString.size() & (~size_t( INT_MAX ) << 1) ) {
					if ( !WriteUi32( 0 ) ) { return false; }
					return true;
				}
			}
			if ( !WriteUi32( uint32_t( _sString.size() ) ) ) { return false; }
			for ( size_t I = 0; I < _sString.size(); ++I ) {
				if ( !WriteUi8( _sString[I] ) ) { return false; }
			}
			return true;
		}

		/**
		 * Writes a u16char_t string to the stream.
		 * 
		 * \param _wsString The string to write.
		 * \return Returns true if there was enough space left in the stream to write the string.
		 **/
		inline bool									WriteStringU16( const CSecureWString &_wsString ) {
			BOOL bErrored = FALSE;
			CHAR cDefault = '?';
			try {
				CSecureString sTmp = CUtilities::WideCharToMultiByte( CP_UTF8, WC_DEFAULTCHAR, _wsString, &cDefault, &bErrored );
				if ( bErrored ) {
					return WriteString( CSecureString() );
				}
				return WriteString( sTmp );
			}
			catch ( ... ) { return WriteString( CSecureString() ); }
		}


		// ========
		// BASE
		// ========
		/**
		 * Reads a value of the given template type.
		 *
		 * \param _tValue Holds the return value if successful.
		 * \return Returns true if there was enough space left in the stream to read the given value.
		 */
		template <typename _tType>
		inline bool									Read( _tType &_tValue ) {
			if ( (m_vStream.size() - m_stPos) >= sizeof( _tType ) ) {
				_tValue = (*reinterpret_cast<_tType *>(&m_vStream.data()[m_stPos]));
				m_stPos += sizeof( _tType );
				return true;
			}
			return false;
		}

		/**
		 * Writes a value of the given template type.
		 *
		 * \param _tValue Holds the return value if successful.
		 * \return Returns true if there was enough space left in the stream to read the given value.
		 */
		template <typename _tType>
		inline bool									Write( _tType _tValue ) {
			if ( m_stPos + sizeof( _tType ) > m_vStream.size() ) {
				m_vStream.resize( m_stPos + sizeof( _tType ) );
			}
			(*reinterpret_cast<_tType *>(&m_vStream.data()[m_stPos])) = _tValue;
			m_stPos += sizeof( _tType );
			return true;
		}



	protected :
		// == Members.
		/** The vector object. */
		std::vector<uint8_t> &						m_vStream;
		/** Our position within the vector. */
		size_t										m_stPos;
	};

}	// namespace mx
