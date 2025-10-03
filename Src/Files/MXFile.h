#pragma once

#include <LSWWin.h>

#include <filesystem>
#include <vector>

namespace mx {

	class CFile {
	public :
		CFile();
		~CFile();


		// == Enumerations.
		/**
		 * \brief Deletes all files in a directory. Directories are left intact.
		 *
		 * If _fMode is MX_FDM_RECURSIVE, files in subdirectories are also removed.
		 * Symlinks are removed as files. Errors are ignored per-entry; the function
		 * returns the number of successfully deleted files.
		 */
		enum MX_FILE_DELETE_MODE {
			MX_FDM_NON_RECURSIVE = 0,				/**< Only delete files directly under _pDir. */
			MX_FDM_RECURSIVE						/**< Delete files under _pDir and all subdirectories. */
		};


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
		DWORD					Read( LPVOID _lpvBuffer, DWORD _dwNumberOfBytesToRead, DWORD * _pdwBytesRead = nullptr ) const;

		// Writes to the opened file.
		DWORD					Write( LPCVOID _lpvBuffer, DWORD _dwNumberOfBytesToWrite, DWORD * _pdwBytesWritten = nullptr );

		// Gets the size of the file.
		UINT64					Size() const;

		// Moves the file pointer from the current position and returns the new position.
		UINT64					MovePointerBy( INT64 _i64Offset ) const;

		// Moves the file pointer to the given file position.
		UINT64					MovePointerTo( UINT64 _ui64Pos, BOOL _bFromEnd = FALSE ) const;

		// Read-only?
		bool					ReadOnly() const;


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

		// Determines if the given path represents an existing file (not folder).
		static bool				IsFile( LPCSTR _lpcFile );

		// Determines if the given path represents an existing file (not folder).
		static bool				IsFile( LPCWSTR _lpwFile );

		// Determines if the given path represents an existing folder (not file).
		static bool				IsFolder( LPCSTR _lpcFolder );

		// Determines if the given path represents an existing folder (not file).
		static bool				IsFolder( LPCWSTR _lpwFolder );

		/**
		 * Determines whether the currently opened data stream is read-only.
		 *
		 * \return Returns true if the file cannot be written to due to the file's read-only attribute
		 * or the volume being write-protected; false otherwise.
		 */
		static bool				ReadOnly( const std::filesystem::path _pPath );

		/**
		 * Finds files/folders in a given directory.
		 * 
		 * \param _pcFolderPath		The path to the directory to search.
		 * \param _pcSearchString	A wildcard search string to find only certain files/folders.
		 * \param _bIncludeFolders	If true, folders are included in the return.
		 * \param _vResult			The return array.  Found files and folders are appended to the array.
		 * \return					Returns _vResult.
		 **/
		static std::vector<std::wstring> &
								FindFiles( const wchar_t * _pcFolderPath, const wchar_t * _pcSearchString, bool _bIncludeFolders, std::vector<std::wstring> &_vResult );

		/**
		 * Delets all files of a given type in a given folder.
		 * 
		 * \param _pcFolderPath		The directory in which to delete files.
		 * \param _pcType			The file type to delete.
		 * \param _bIncludeFolders	Whether to include subfolders or not.
		 * \return					Returns the number of files deleted.
		 **/
		static size_t			DeleteFiles( const wchar_t * _pcFolderPath, const wchar_t * _pcType, bool _bIncludeFolders );

		/**
		 * \brief Deletes files under the given directory, optionally recursively.
		 *
		 * \param _pDir				Directory whose files should be deleted.
		 * \param _fMode			Deletion mode (recursive or not).
		 * \return					Count of files successfully deleted.
		 */
		static size_t			EraseFilesInDirectory( const std::filesystem::path &_pDir, MX_FILE_DELETE_MODE _fMode = MX_FDM_NON_RECURSIVE );

		/**
		 * \brief Safely deletes a directory tree (non-throwing).
		 *
		 * Ensures the target exists and is a real directory (or a directory symlink),
		 * refuses to operate on root/suspicious paths, normalizes permissions
		 * (clears read-only) before removal, and never follows directory symlinks.
		 *
		 * \param _pDir The directory to delete.
		 * \return Returns true if the directory (or link) was removed or did not exist; false on failure.
		 */
		static bool				DeleteDirectorySafely( const std::filesystem::path &_pDir );

		/**
		 * \brief Safely creates a directory (and parents) if needed (non-throwing).
		 *
		 * Validates the target path, refuses root/suspicious paths, rejects any parent
		 * component that is a symlink (to avoid traversal through links), and creates
		 * all missing components. If the target already exists and is a directory, this
		 * returns true. Uses error_code overloads (no exceptions).
		 *
		 * \param _pDir				The directory to create.
		 * \return					Returns true if the directory exists by the end (created or pre-existing); false on failure.
		 */
		static bool				MakeDirectorySafely( const std::filesystem::path &_pDir );


	protected :
		// == Members.
		// The actual file handle.
		HANDLE					m_hFile;
		// The file path.
		std::filesystem::path	m_pFilePath;

		// Converts a WIN32_FILE_ATTRIBUTE_DATA structure to an MX_FILE_ATTR structure.
		static MX_FILE_ATTR		Win32AttrToMxAttr( const WIN32_FILE_ATTRIBUTE_DATA &_wfadData );
	};

}	// namespace mx
