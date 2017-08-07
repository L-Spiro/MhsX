#pragma once

#include "../PE/MXPeStructs.h"
#include <string>
#include <Windows.h>

namespace mx {

	// Convert a single byte to hex.  _pcString must be at least 3 characters long.
	VOID						ByteToHex( BYTE _bIn, CHAR * _pcString, BOOL _bLower = FALSE );

	// Convert a single byte to hex.
	VOID						ByteToHex( BYTE _bIn, std::string &_sString, BOOL _bLower = FALSE );

	// Convert a byte array to a hex string.  _pcString must be twice as long as the input, plus 1 character for the NULL.
	VOID						BytesToHex( const VOID * _pvIn, size_t _sLen, CHAR * _pcString, BOOL _bLower = FALSE );

	// Convert a byte array to a hex string.
	VOID						BytesToHex( const VOID * _pvIn, size_t _sLen, std::string &_sString, BOOL _bLower = FALSE );

	// Convert a byte array to a hex string.  _pcString must be 3 times as long as the input.
	VOID						BytesToHexWithSpaces( const VOID * _pvIn, size_t _sLen, CHAR * _pcString, BOOL _bLower = FALSE );

	// Convert a byte array to a hex string.
	VOID						BytesToHexWithSpaces( const VOID * _pvIn, size_t _sLen, std::string &_sString, BOOL _bLower = FALSE );

	// Is the given character printable?  If not, it should be printed as a space or question mark.
	BOOL						ByteIsPrintable( BYTE _bIn, BOOL _bPrintExtended = TRUE );

	// Convert a single byte to a printable character.  If _bPrintExtended is TRUE, characters above 127 are kept.
	CHAR						ByteToPrintable( BYTE _bIn, BOOL _bPrintExtended = TRUE, BOOL _bUseQuestionMarks = FALSE );

	// Converts a byte to a C-string value.  _pcString must be at least 5 characters long.  Returns the number of characters written.
	DWORD						ByteToCString( BYTE _bIn, CHAR * _pcString, BOOL _bLower = FALSE );

	// Converts a byte to a C-string value.  Returns the number of characters written.
	DWORD						ByteToCString( BYTE _bIn, std::string &_sString, BOOL _bLower = FALSE );

	// Convert a byte array to a C string.  Returns the number of characters written.
	DWORD						BytesToCString( const VOID * _pvIn, size_t _sLen, CHAR * _pcString, BOOL _bLower = FALSE );

	// Convert a byte array to a C string.  Returns the number of characters written.
	DWORD						BytesToCString( const VOID * _pvIn, size_t _sLen, std::string &_sString, BOOL _bLower = FALSE );

	// Converts a MX_MACHINE_TYPES enum to its string value.  _pcRet should be at least 32 characters long.
	VOID						MachineTypeToString( uint32_t _uiType, CHAR * _pcRet );

	// Creates a string with all of the MX_PE_CHARACTERISTICS bit flags set in a given value.
	VOID						PeCharacteristicsToString( uint32_t _uiVal, std::string &_sString );

	// Converts a MX_PE_MAGIC_FLAGS enum to its string value.  _pcRet should be at least 32 characters long.
	VOID						PeMagicTypeToString( uint32_t _uiType, CHAR * _pcRet );

	// Converts a MX_PE_WINDOWS_SUBSYSTEM enum to its string value.  _pcRet should be at least 32 characters long.
	VOID						PeSubSystemTypeToString( uint32_t _uiType, CHAR * _pcRet );

	// Creates a string with all of the MX_PE_DLL_CHARACTERISTICS bit flags set in a given value.
	VOID						PeDllCharacteristicsToString( uint32_t _uiVal, std::string &_sString );

	// Gets a Windows version string based on the given major and minor versions.
	VOID						WindowsVersion( uint32_t _uiMajor, uint32_t _uiMinor, std::string &_sString );

	// Creates a string that best represents the given size.
	VOID						SizeString( uint64_t _uiSize, std::string &_sString );

}	// namespace mx
