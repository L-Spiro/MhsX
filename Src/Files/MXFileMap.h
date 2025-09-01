#pragma once

#include <LSWWin.h>
#include "MXFile.h"

namespace mx {

	class CFileMap {
	public :
		CFileMap();
		~CFileMap();


		// == Operators.
		// ID-based equality.
		inline bool						operator == ( const CFileMap &_fmOther ) const {
			return Id() == _fmOther.Id();
		}

		// ID-based less-than.
		inline bool						operator < ( const CFileMap &_fmOther ) const {
			return Id() < _fmOther.Id();
		}

		// ID-based greater-than.
		inline bool						operator > ( const CFileMap &_fmOther ) const {
			return Id() > _fmOther.Id();
		}

		// Move.
		inline CFileMap &				operator = ( CFileMap &&_fmOther ) noexcept {
			if ( this != &_fmOther ) {
				Close();
#define MX_FM_COPY( VAL, DEFAULT )		VAL = _fmOther.VAL; _fmOther.VAL = DEFAULT
				MX_FM_COPY( m_ui64Id, 0 );
				MX_FM_COPY( m_hFile, INVALID_HANDLE_VALUE );
				MX_FM_COPY( m_hMap, INVALID_HANDLE_VALUE );
				MX_FM_COPY( m_pbMapBuffer, nullptr );
				MX_FM_COPY( m_dwChunkSize, static_cast<DWORD>(8 * 1024 * 1024) );
				MX_FM_COPY( m_bIsEmpty, TRUE );
				MX_FM_COPY( m_bWritable, TRUE );
				MX_FM_COPY( m_ui64Size, 0 );
				MX_FM_COPY( m_ui64MapStart, MAXUINT64 );
				MX_FM_COPY( m_dwMapSize, 0 );
#undef MX_FM_COPY

			}
			return (*this);
		}


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
		BOOL							CreateMap( const std::filesystem::path &_pFile, BOOL _bOpenForWrite, uint64_t _ui64CreationSize = 0 );

		// Closes the opened file map.
		VOID							Close();

		// Gets the size of the file.
		UINT64							Size() const;

		// Reads from the opened file.
		DWORD							Read( LPVOID _lpvBuffer, UINT64 _ui64From, DWORD _dwNumberOfBytesToRead ) const;

		// Writes to the opened file.
		DWORD							Write( LPCVOID _lpvBuffer, UINT64 _ui64From, DWORD _dwNumberOfBytesToWrite );

		// Sets the chunk size.
		inline VOID						SetChunkSize( DWORD _dwSize ) { m_dwChunkSize = _dwSize; }

		// Map a region of the file.
		BOOL							MapRegion( UINT64 _ui64Offset, DWORD _dwSize ) const;

		// Closes the current region map.
		void							UnMapRegion();

		// Gets this item's unique ID.
		inline uint64_t					Id() const { return m_ui64Id; }


	protected :
		// == Members.
		uint64_t						m_ui64Id{ ++s_aId };

		// The actual file handle.
		HANDLE							m_hFile = INVALID_HANDLE_VALUE;

		// File-mapping handle.
		HANDLE							m_hMap = INVALID_HANDLE_VALUE;

		// Mapped bytes.
		mutable PBYTE					m_pbMapBuffer = nullptr;

		// Mapping chunk size.
		DWORD							m_dwChunkSize = static_cast<DWORD>(8 * 1024 * 1024);

		// Is the file 0-sized?
		BOOL							m_bIsEmpty = TRUE;

		// Read-only or read-write?
		BOOL							m_bWritable = TRUE;

		// Size of the file.
		mutable UINT64					m_ui64Size = 0;

		// Map start.
		mutable UINT64					m_ui64MapStart = MAXUINT64;

		// Mapped size.
		mutable DWORD					m_dwMapSize = 0;


		// == Functions.
		// Creates the file map.
		BOOL							CreateFileMap();

		// Adjusts the input to the nearest mapping offset.
		UINT64							AdjustBase( UINT64 _ui64Offset ) const;

	private :
		// ID generator.
		static std::atomic<uint64_t>	s_aId;
	};

}	// namespace mx
