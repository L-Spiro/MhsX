#pragma once

#include <vector>
#include <Windows.h>

namespace mx {

	class CFile {
	public :
		CFile();
		~CFile();


		// == Types.
		struct MX_FILE_ATTR {
			DWORD				dwAttr;				// The file system attribute information for a file or directory.
			UINT64				ui64CreationTime;	// File creation time.
			UINT64				ui64AccessTime;		// Last access time.
			UINT64				ui64WriteTime;		// Last write time.
			UINT64				ui64Size;			// File size.
		};


		// == Functions.
		// Opens an existing file for read or write.
		BOOL					OpenFile( LPCSTR _lpcFile, BOOL _bOpenForRead );

		// Opens an existing file for read or write.
		BOOL					OpenFile( LPCWSTR _lpwFile, BOOL _bOpenForRead );

		// Creates a file and opens for writing.
		BOOL					CreateNewFile( LPCSTR _lpcFile, BOOL _bFailIfExists );

		// Creates a file and opens for writing.
		BOOL					CreateNewFile( LPCWSTR _lpwFile, BOOL _bFailIfExists );

		// Closes the opened file.
		VOID					Close();

		// Reads from the opened file.
		DWORD					Read( LPVOID _lpvBuffer, DWORD _dwNumberOfBytesToRead ) const;

		// Writes to the opened file.
		DWORD					Write( LPCVOID _lpvBuffer, DWORD _dwNumberOfBytesToWrite );

		// Gets the size of the file.
		UINT64					Size() const;

		// Moves the file pointer from the current position and returns the new position.
		UINT64					MovePointerBy( INT64 _i64Offset );

		// Moves the file pointer to the given file position.
		UINT64					MovePointerTo( UINT64 _ui64Pos, BOOL _bFromEnd = FALSE );


		// == Statics.
		// Gets the size of a file given its path.
		static UINT64			Size( LPCSTR _lpcFile );

		// Gets the size of a file given its path.
		static UINT64			Size( LPCWSTR _lpwFile );

		// Gets the size of the file.
		static UINT64			Size( HANDLE _hFile );

		// Gets the creation time of a file.
		static UINT64			CreationTime( LPCSTR _lpcFile );

		// Gets the creation time of a file.
		static UINT64			CreationTime( LPCWSTR _lpwFile );

		// Gets the last access time of a file.
		static UINT64			LastAccessTime( LPCSTR _lpcFile );

		// Gets the last access time of a file.
		static UINT64			LastAccessTime( LPCWSTR _lpwFile );

		// Gets the last write time of a file.
		static UINT64			LastWriteTime( LPCSTR _lpcFile );

		// Gets the last write time of a file.
		static UINT64			LastWriteTime( LPCWSTR _lpwFile );

		// Gets the file attributes (flags, creation time, write time, access time, and size).
		static MX_FILE_ATTR		Attributes( LPCSTR _lpcFile );

		// Gets the file attributes (flags, creation time, write time, access time, and size).
		static MX_FILE_ATTR		Attributes( LPCWSTR _lpwFile );

		// Loads a file to memory.
		static std::vector<uint8_t> &
								LoadToMemory( LPCSTR _lpcFile, std::vector<uint8_t> &_vRet );

		// Loads a file to memory.
		static std::vector<uint8_t> &
								LoadToMemory( LPCWSTR _lpwFile, std::vector<uint8_t> &_vRet );


	protected :
		// == Members.
		// The actual file handle.
		HANDLE					m_hFile;

		// Converts a WIN32_FILE_ATTRIBUTE_DATA structure to an MX_FILE_ATTR structure.
		static MX_FILE_ATTR		Win32AttrToMxAttr( const WIN32_FILE_ATTRIBUTE_DATA &_wfadData );
	};

}	// namespace mx
