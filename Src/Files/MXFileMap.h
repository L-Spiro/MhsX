/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A standard file-mapping wrapper.
 */
 
#pragma once

#include "../Utilities/MXUtilities.h"
#include "MXFile.h"

#if defined( _WIN32 )
	#include <LSWWin.h>
#else
	#include <sys/mman.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <errno.h>
#endif

#include <atomic>
#include <filesystem>
#include <cstdint>
#include <cstring>


namespace mx {

	// == Platform handle aliases.
#if defined( _WIN32 )
		using MX_FILE_HANDLE				= HANDLE;
		using MX_MAP_HANDLE					= HANDLE;												// Windows mapping object handle.
		#define MX_INVALID_FILE				INVALID_HANDLE_VALUE
		#define MX_INVALID_MAP				INVALID_HANDLE_VALUE
#else
		using MX_FILE_HANDLE				= int;													// POSIX file descriptor.
		using MX_MAP_HANDLE					= int;													// Unused on POSIX; kept for symmetry.
		#define MX_INVALID_FILE				(-1)
		#define MX_INVALID_MAP				(-1)
#endif

	class CFileMap {
	public :
		CFileMap();
		~CFileMap();
		CFileMap( const CFileMap & ) = delete;
		CFileMap( CFileMap &&_fmOther ) noexcept {
			(*this) = std::move( _fmOther );
		}



		// == Types.
		class CRegionMap {
		public :
			CRegionMap() {
			}
			~CRegionMap() {
				UnMapRegion();
			}
			CRegionMap( const CRegionMap & ) = delete;
			CRegionMap( CRegionMap &&_fmOther ) noexcept { (*this) = std::move( _fmOther ); }


			// == Operators.
			/**
			 * Move assignment.
			 * \brief Transfers ownership of the mapped region (if any) from another instance and clears the source.
			 * 
			 * \param _fmOther Reference to the source CRegionMap to steal from.
			 * \return Returns a reference to this object.
			 */
			inline CRegionMap &				operator = ( CRegionMap &&_fmOther ) noexcept {
				if ( this != &_fmOther ) {
					UnMapRegion();
#define MX_FM_COPY( VAL, DEFAULT )		VAL = _fmOther.VAL; _fmOther.VAL = DEFAULT
					MX_FM_COPY( m_pbMapBuffer, nullptr );
					MX_FM_COPY( m_ui64MapStart, MAXUINT64 );
					MX_FM_COPY( m_sMapSize, 0 );
#undef MX_FM_COPY
				}
				return (*this);
			}

			CRegionMap &					operator = ( const CRegionMap & ) = delete;


			// == Functions.
			/**
			 * Maps a region of a file mapping object.
			 * \brief Calls MapViewOfFile() over the requested range, optionally for write access.
			 * 
			 * \param _hMap A valid file-mapping handle (from CreateFileMapping()).
			 * \param _ui64Offset Byte offset from the start of the file to the desired view.
			 * \param _sSize Number of bytes to map into memory.
			 * \param _bWrite TRUE to request a writable view; FALSE for read-only.
			 * \return Returns TRUE on success; FALSE on failure.
			 */
			bool							MapRegion( HANDLE _hMap, uint64_t _ui64Offset, size_t _sSize, bool _bWrite ) {
				UnMapRegion();
#if defined( _WIN32 )
				if MX_UNLIKELY( _hMap == INVALID_HANDLE_VALUE || !_sSize ) { return false; }
				m_pbMapBuffer = static_cast<uint8_t *>(::MapViewOfFile( _hMap,
					(_bWrite ? FILE_MAP_WRITE : 0) | FILE_MAP_READ,
					static_cast<DWORD>(_ui64Offset >> 32),
					static_cast<DWORD>(_ui64Offset),
					static_cast<SIZE_T>(_sSize) ));
				if ( !m_pbMapBuffer ) { return false; }
#else
				const int fd = MX_HandleToFd( _hMap );
				const int prot = PROT_READ | (_bWrite ? PROT_WRITE : 0);
				void * pv = ::mmap( nullptr, _sSize, prot, MAP_SHARED, fd, static_cast<off_t>(_ui64Offset) );
				if ( pv == MAP_FAILED ) { return false; }

				m_pbMapBuffer = static_cast<uint8_t *>(pv);
#endif
				m_ui64MapStart = _ui64Offset;
				m_sMapSize = _sSize;
				return true;
			}

			/**
			 * Unmaps the current region.
			 * \brief If a view is currently mapped, calls UnmapViewOfFile() and resets members.
			 */
			void							UnMapRegion() {
				if ( m_pbMapBuffer ) {
					// Unmap existing buffer.
#if defined( _WIN32 )
					::UnmapViewOfFile( m_pbMapBuffer );
#else
					::munmap( static_cast<void *>(m_pbMapBuffer), m_sMapSize );
#endif
					m_pbMapBuffer = nullptr;
					m_ui64MapStart = MAXUINT64;
					m_sMapSize = 0;
				}
			}

			/**
			 * Gets the start offset of the current map.
			 * \brief Returns the file-relative byte offset where the mapped view begins.
			 * 
			 * \return Returns the starting byte offset, or MAXUINT64 if no view is mapped.
			 */
			inline uint64_t					Start() const { return m_ui64MapStart; }

			/**
			 * Gets the size of the current map.
			 * \brief Returns the number of bytes currently mapped into memory.
			 * 
			 * \return Returns the mapped-byte count; 0 if no view is mapped.
			 */
			inline size_t					Size() const { return m_sMapSize; }

			/**
			 * Gets a writable pointer to the mapped bytes.
			 * \brief Provides direct access to the mapped memory region.
			 * 
			 * \return Returns a pointer to the first mapped byte; nullptr if unmapped.
			 */
			inline uint8_t *				Data() { return m_pbMapBuffer; }

			/**
			 * Gets a read-only pointer to the mapped bytes.
			 * \brief Provides const access to the mapped memory region.
			 * 
			 * \return Returns a const pointer to the first mapped byte; nullptr if unmapped.
			 */
			inline const uint8_t *			Data() const { return m_pbMapBuffer; }


		private :
			// == Members.
			mutable uint8_t *				m_pbMapBuffer = nullptr;								/**< Pointer to the mapped view (nullptr if unmapped). */
			mutable uint64_t				m_ui64MapStart = MAXUINT64;								/**< File-relative start offset of the mapped view. */
			mutable size_t					m_sMapSize = 0;											/**< Size in bytes of the mapped view. */
		};


		// == Operators.
		/**
		 * Equality operator (ID-based).
		 * \brief Compares two CFileMap instances by their unique identifiers.
		 * 
		 * \param _fmOther The other file map to compare against.
		 * \return Returns true if both objects have the same Id().
		 */
		inline bool							operator == ( const CFileMap &_fmOther ) const {
			return Id() == _fmOther.Id();
		}

		/**
		 * Less-than operator (ID-based).
		 * \brief Orders two CFileMap instances by their unique identifiers.
		 * 
		 * \param _fmOther The other file map to compare against.
		 * \return Returns true if this Id() is less than the other Id().
		 */
		inline bool							operator < ( const CFileMap &_fmOther ) const {
			return Id() < _fmOther.Id();
		}

		/**
		 * Greater-than operator (ID-based).
		 * \brief Orders two CFileMap instances by their unique identifiers.
		 * 
		 * \param _fmOther The other file map to compare against.
		 * \return Returns true if this Id() is greater than the other Id().
		 */
		inline bool							operator > ( const CFileMap &_fmOther ) const {
			return Id() > _fmOther.Id();
		}

		/**
		 * Move assignment.
		 * \brief Transfers ownership of OS handles and state from another CFileMap; the source is reset.
		 * 
		 * \param _fmOther Reference to the source CFileMap to steal from.
		 * \return Returns a reference to this object.
		 */
		inline CFileMap &					operator = ( CFileMap &&_fmOther ) noexcept {
			if ( this != &_fmOther ) {
				Close();
#define MX_FM_COPY( VAL, DEFAULT )		VAL = _fmOther.VAL; _fmOther.VAL = DEFAULT
				MX_FM_COPY( m_ui64Id, 0 );
				MX_FM_COPY( m_hFile, INVALID_HANDLE_VALUE );
				MX_FM_COPY( m_hMap, INVALID_HANDLE_VALUE );
				MX_FM_COPY( m_dwChunkSize, static_cast<uint32_t>(8 * 1024 * 1024) );
				MX_FM_COPY( m_bIsEmpty, TRUE );
				MX_FM_COPY( m_bWritable, TRUE );
				MX_FM_COPY( m_ui64Size, 0 );
#undef MX_FM_COPY
				m_rmMap = std::move( _fmOther.m_rmMap );
			}
			return (*this);
		}

		
		CFileMap &							operator = ( const CFileMap & ) = delete;


		// == Functions.
		/**
		 * Creates (or opens) a file and its mapping.
		 * \brief If _ui64CreationSize != 0, the file is (re)created and resized to that many bytes.
		 *
		 * \param _pFile Path to the file.
		 * \param _bOpenForWrite TRUE to open for write when not creating; ignored when creating (write is forced).
		 * \param _ui64CreationSize If non-zero, (re)create the file and set its size to this value.
		 * \return Returns TRUE on success.
		 */
		bool								CreateMap( const std::filesystem::path &_pFile, bool _bOpenForWrite, uint64_t _ui64CreationSize = 0 );

		/**
		 * Closes the file and mapping.
		 * \brief Releases any open handles and unmaps any active view.
		 */
		void								Close();

		/**
		 * Gets the file size in bytes.
		 * \brief Returns the cached size if available; may query the OS when needed.
		 * 
		 * \return Returns the size of the underlying file.
		 */
		uint64_t							Size() const;

		/**
		 * Reads from the opened file.
		 * \brief Copies data starting at a given file offset into a caller-provided buffer, using a temporary or provided region map.
		 * 
		 * \param _lpvBuffer Destination buffer to receive the bytes.
		 * \param _ui64From File-relative byte offset to begin reading.
		 * \param _dwNumberOfBytesToRead Number of bytes to read into the destination buffer.
		 * \param _prmMap Optional pointer to a CRegionMap to reuse; if nullptr, a transient map is used internally.
		 * \return Returns the number of bytes actually read; 0 on failure.
		 */
		uint32_t							Read( LPVOID _lpvBuffer, uint64_t _ui64From, uint32_t _dwNumberOfBytesToRead, CRegionMap * _prmMap = nullptr ) const;

		/**
		 * Writes to the opened file.
		 * \brief Copies data from a caller-provided buffer into the file at a given offset, using a temporary or provided region map.
		 * 
		 * \param _lpvBuffer Source buffer containing the bytes to write.
		 * \param _ui64From File-relative byte offset to begin writing.
		 * \param _dwNumberOfBytesToWrite Number of bytes to write from the source buffer.
		 * \param _prmMap Optional pointer to a CRegionMap to reuse; if nullptr, a transient map is used internally.
		 * \return Returns the number of bytes actually written; 0 on failure.
		 */
		uint32_t							Write( LPCVOID _lpvBuffer, uint64_t _ui64From, uint32_t _dwNumberOfBytesToWrite, CRegionMap * _prmMap = nullptr );

		/**
		 * Sets the preferred chunk size for mapping operations.
		 * \brief Controls the maximum view size used when mapping regions for Read()/Write().
		 * 
		 * \param _ui32Size Desired chunk size in bytes.
		 */
		inline void							SetChunkSize( uint32_t _ui32Size ) { m_dwChunkSize = _ui32Size; }

		/**
		 * Maps a region of the file into memory.
		 * \brief Ensures a view covering the requested range is available, using _prmMap if provided.
		 * 
		 * \param _ui64Offset File-relative byte offset where the view should begin.
		 * \param _sSize Minimum number of bytes that must be accessible from the view.
		 * \param _prmMap Optional pointer to a CRegionMap to receive/reuse the mapping; if nullptr, uses an internal map.
		 * \return Returns TRUE on success; FALSE on failure.
		 */
		bool								MapRegion( uint64_t _ui64Offset, size_t _sSize, CRegionMap * _prmMap = nullptr ) const;

		/**
		 * Unmaps any currently mapped region.
		 * \brief Convenience wrapper that clears the active region map associated with this file map.
		 */
		void								UnMapRegion();

		/**
		 * Gets this object's unique identifier.
		 * \brief Returns a monotonically assigned ID used for stable ordering and comparisons.
		 * 
		 * \return Returns the 64-bit unique ID for this CFileMap.
		 */
		inline uint64_t						Id() const { return m_ui64Id; }

		/**
		 * Is the opened file read-only?
		 * 
		 * \return Returns true if the file is read-only.
		 **/
		bool								ReadOnly() const;

		/**
		 * Was the file opened for write?
		 * 
		 * \return Returns true if the file was opened for writing.
		 **/
		inline bool							Writeable() const { return m_bWritable; }


	protected :
		// == Members.
		uint64_t							m_ui64Id{ ++s_aId };									/**< Unique identifier for this instance. */
		std::filesystem::path				m_pFilePath;											/**< The file path. */
		MX_FILE_HANDLE						m_hFile = MX_INVALID_FILE;								/**< OS file handle (HANDLE or fd). */
		MX_MAP_HANDLE						m_hMap = MX_INVALID_MAP;								/**< File-mapping object handle. */
		mutable CRegionMap					m_rmMap;												/**< Reusable region map. */
		uint32_t							m_dwChunkSize = static_cast<uint32_t>(8 * 1024 * 1024);	/**< Mapping chunk size in bytes. */
		bool								m_bIsEmpty = true;										/**< Flag indicating whether the file is empty. */
		bool								m_bWritable = false;									/**< Flag indicating writable access. */
		mutable uint64_t					m_ui64Size = 0;											/**< Cached file size in bytes. */


		// == Functions.
		/**
		 * Creates the file-mapping object for the currently opened file.
		 * \brief Initializes m_hMap from m_hFile using CreateFileMapping(); does not map a view.
		 * 
		 * \return Returns TRUE on success; FALSE on failure (m_hMap remains INVALID_HANDLE_VALUE).
		 */
		bool								CreateFileMap();

		/**
		 * Adjusts a requested file offset down to a valid mapping base.
		 * \brief Aligns _ui64Offset to the system allocation granularity for MapViewOfFile().
		 * 
		 * \param _ui64Offset Requested file-relative byte offset for mapping.
		 * \return Returns the aligned base offset suitable for MapViewOfFile().
		 */
		uint64_t							AdjustBase( uint64_t _ui64Offset ) const;

	private :
		// ID generator.
		static std::atomic<uint64_t>		s_aId;
	};

}	// namespace mx
