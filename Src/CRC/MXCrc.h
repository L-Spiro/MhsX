/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A standard CRC class.
 */


#pragma once
#include <cinttypes>


namespace mx {

	/**
	 * Class CCrc
	 * \brief A standard 32-bit cyclic redundancy check calculator.
	 *
	 * Description: A standard 32-bit cyclic redundancy check calculator.
	 */
	class CCrc {
	public :
		// == Functions.
		/**
		 * Calculate the standard 32-bit CRC value for a given set of bytes.
		 *
		 * \param _pui8Data The data for which to calculate a CRC value.
		 * \param _uiptrLen Length of the data.
		 * \return Returns the standard 32-bit CRC value for the given data
		 */
		static uint32_t 				GetCrc( const uint8_t * _pui8Data, uintptr_t _uiptrLen );

		/**
		 * Calculate the alternative 32-bit CRC value for a given set of bytes.
		 *
		 * \param _pui8Data The data for which to calculate a CRC value.
		 * \param _uiptrLen Length of the data.
		 * \return Returns the alternative 32-bit CRC value for the given data
		 */
		static uint32_t 				GetAltCrc( const uint8_t * _pui8Data, uintptr_t _uiptrLen );

		/**
		 * Initialize the tables.
		 */
		static void 					Init();

		/**
		 * Calculates the CRC of a given byte, modifying the previous CRC value in-place.  Must call Init() first.
		 *
		 * \param _pui32Table The table to use.
		 * \param _ui8Val The byte to add to the CRC calculation.
		 * \param _ui32Crc The CRC to update.
		 */
		static void 					GetCrc( const uint32_t * _pui32Table, uint8_t _ui8Val, uint32_t &_ui32Crc );

		/**
		 * Gets the standard table.
		 * 
		 * \return Returns a pointer to the standard CRC table.
		 **/
		static inline const uint32_t *	StdTable() { return m_ui32StdTable; }

		/**
		 * Gets the alternative table.
		 * 
		 * \return Returns a pointer to the alternative CRC table.
		 **/
		static inline const uint32_t *	AltTable() { return m_ui32AltTable; }


	protected :
		// == Members.
		/**
		 * Standard table of codes.
		 */
		static uint32_t					m_ui32StdTable[256];

		/**
		 * Alternative table of codes.
		 */
		static uint32_t					m_ui32AltTable[256];

		/**
		 * Has the table been initialized?
		 */
		static bool						m_bInit;


		// == Functions.
		/**
		 * Initialize a table with a key.
		 *
		 * \param _pui32Table The table to initialize
		 * \param _ui32Key The key to use for initialization.
		 */
		static void 					Init( uint32_t * _pui32Table, uint32_t _ui32Key );

		
	};

}	// namespace mx
