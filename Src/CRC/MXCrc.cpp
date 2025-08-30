/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A standard CRC class.
 */


#include "MXCrc.h"

namespace mx {

	// == Members.
	/**
	 * Standard table of codes.
	 */
	uint32_t CCrc::m_ui32StdTable[256] = { 0 };

	/**
	 * Alternative table of codes.
	 */
	uint32_t CCrc::m_ui32AltTable[256] = { 0 };

	/**
	 * Has the table been initialized?
	 */
	bool CCrc::m_bInit = false;

	// == Functions.
	/**
	 * Calculate the standard 32-bit CRC value for a given set of bytes.
	 *
	 * \param _pui8Data The data for which to calculate a CRC value.
	 * \param _uiptrLen Length of the data.
	 * \return Returns the standard 32-bit CRC value for the given data
	 */
	uint32_t CCrc::GetCrc( const uint8_t * _pui8Data, uintptr_t _uiptrLen ) {
		Init();

		uint32_t ui32Crc = UINT32_MAX;
		for ( uintptr_t I = 0; I < _uiptrLen; ++I ) {
			GetCrc( m_ui32StdTable, (*_pui8Data++), ui32Crc );
		}

		return ~ui32Crc;
	}

	/**
	 * Calculate the alternative 32-bit CRC value for a given set of bytes.
	 *
	 * \param _pui8Data The data for which to calculate a CRC value.
	 * \param _uiptrLen Length of the data.
	 * \return Returns the alternative 32-bit CRC value for the given data
	 */
	uint32_t CCrc::GetAltCrc( const uint8_t * _pui8Data, uintptr_t _uiptrLen ) {
		Init();

		uint32_t ui32Crc = UINT32_MAX;
		_pui8Data += _uiptrLen;
		for ( uintptr_t I = 0; I < _uiptrLen; ++I ) {
			GetCrc( m_ui32AltTable, (*--_pui8Data), ui32Crc );
		}

		return ~ui32Crc;
	}

	/**
	 * Initialize the table.
	 */
	void CCrc::Init() {
		if ( !m_bInit ) {
			Init( m_ui32StdTable, 0xEDB88320 );
			Init( m_ui32AltTable, 0x04C11DB7 );
			m_bInit = true;
		}		
	}

	/**
	 * Calculates the CRC of a given byte, modifying the previous CRC value in-place.  Must call Init() first.
	 *
	 * \param _pui32Table The table to use.
	 * \param _ui8Val The byte to add to the CRC calculation.
	 * \param _ui32Crc The CRC to update.
	 */
	void CCrc::GetCrc( const uint32_t * _pui32Table, uint8_t _ui8Val, uint32_t &_ui32Crc ) {
		_ui32Crc = (_ui32Crc >> 8) ^ _pui32Table[_ui8Val^(_ui32Crc&0xFF)];
	}

	/**
	 * Initialize a table with a key.
	 *
	 * \param _pui32Table The table to initialize
	 * \param _ui32Key The key to use for initialization.
	 */
	void CCrc::Init( uint32_t * _pui32Table, uint32_t _ui32Key ) {
		for ( uint32_t I = 0; I < 256; ++I ) {
			uint32_t ui32Crc = I;
			for ( uint32_t J = 8; J--; ) {
				if ( ui32Crc & 1 ) {
					ui32Crc = (ui32Crc >> 1) ^ _ui32Key;
				}
				else {
					ui32Crc >>= 1;
				}
			}
			_pui32Table[I] = ui32Crc;
		}
	}

}	// namespace mx
