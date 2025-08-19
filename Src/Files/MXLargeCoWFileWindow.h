#pragma once

#include <LSWWin.h>

#include <filesystem>
#include <mutex>

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
			MX_FILE_MAP() :
				ui32Id( ++s_nInstanceCount ) {
			}


			// == Functions.
			// Open a file and create a map.
			bool												OpenFile( const std::filesystem::path &_pFile );

			// Closes the current file.
			void												Close();

			// Gets the size of the file.
			uint64_t											Size() const;

			// Map a region of the file.
			bool												MapRegion( uint64_t _ui64Offset, DWORD _dwSize );

			// Maps a full file.
			bool												MapFullRegion() { return MapRegion( 0, Size() ); }

			// Unmap.
			void												Upmap();

			// Gets the ID of this map.
			uint32_t											Id() const { return ui32Id; }


		protected :
			// == Members.
			// Size of the file.
			mutable uint64_t									ui64Size = 0;
			// Map start.
			uint64_t											ui64MapStart = UINT64_MAX;
			// The path to the file.
			std::filesystem::path								pPath;
			// The actual file handle.
			HANDLE												hFile = INVALID_HANDLE_VALUE;
			// File-mapping handle.
			HANDLE												hMap = INVALID_HANDLE_VALUE;
			// Mapped bytes.
			mutable uint8_t *									pui8MapBuffer = nullptr;
			// Mapped size.
			uint32_t											ui32MapSize = 0;
			// ID.
			uint32_t											ui32Id = 0;
			// Is the file 0-sized?
			bool												bIsEmpty = true;


			// == Functions.
			// Creates the file map.
			bool												CreateFileMap();

		private :
			// == Members.
			inline static std::atomic<size_t>					s_nInstanceCount{ 0 };
		};


		// == Members.
		MX_FILE_MAP												m_fmMainMap;									// The map for the main file being edited.
		std::vector<MX_FILE_MAP>								m_vEditedMaps;									// Mappings of edited copies.
		std::vector<size_t>										m_vActiveSegments;								// List of edited segments currently loaded into RAM.  Sorted by most-recent access.
		std::filesystem::path									m_pEditDiractory;								// Folder where edited copies go.

	};

}	// namespace mx
