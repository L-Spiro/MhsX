#pragma once

#include <LSWWin.h>

#include <filesystem>

namespace mx {

	class CLargeCoWFileWindow {
	public :
		CLargeCoWFileWindow();
		~CLargeCoWFileWindow();


		// == Functions.
		// Opens a file for editing.  Maps the first sector to memory.
		bool													OpenFile( const std::filesystem::path &_pFile );

		// Resets the object back to scratch.
		void													Reset();


	protected :
		// == Types.
		// A single file map with a single file view.
		struct MX_FILE_MAP {
			// == Functions.
			// Open a file and create a map.
			bool												OpenFile( const std::filesystem::path &_pFile );

			// Closes the current file.
			void												Close();

			// Gets the size of the file.
			uint64_t											Size() const;


		protected :
			// == Members.
			// The path to the file.
			std::filesystem::path								pPath;
			// The actual file handle.
			HANDLE												hFile = INVALID_HANDLE_VALUE;
			// File-mapping handle.
			HANDLE												hMap = INVALID_HANDLE_VALUE;
			// Mapped bytes.
			mutable PBYTE										pbMapBuffer = nullptr;
			// Is the file 0-sized?
			BOOL												bIsEmpty = TRUE;
			// Size of the file.
			mutable uint64_t									ui64Size = 0;
			// Map start.
			mutable uint64_t									ui64MapStart = UINT64_MAX;
			// Mapped size.
			mutable uint32_t									ui32MapSize = 0;


			// == Functions.
			// Creates the file map.
			BOOL												CreateFileMap();
		};


		// == Members.

	};

}	// namespace mx
