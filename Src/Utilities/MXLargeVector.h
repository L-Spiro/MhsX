/**
 * Copyright L. Spiro 2025
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The complete set of elements is stored on disk while only a section (or cache) is kept in RAM.
 *	The disk file path is generated automatically based on the current directory, the string "lvec",
 *	and a unique ID that is incremented for each created instance. The parameter _nMaxRamItems
 *	determines how many elements remain in memory at a given time, and _nTotalSize is the overall
 *	number of elements.
 *
 * Note: Iterator support is explicitly removed to avoid repeated flushing and loading.
 */

#pragma once

#include <algorithm>
#include <atomic>
#include <filesystem>
#include <fstream>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <vector>

#if defined( _MSC_VER )
    // Microsoft Visual Studio Compiler.
    #define LGV_LIKELY( x )						( x ) [[likely]]
    #define LGV_UNLIKELY( x )					( x ) [[unlikely]]
#elif defined( __GNUC__ ) || defined( __clang__ )
    // GNU Compiler Collection (GCC) or Clang.
    #define LGV_LIKELY( x )						( __builtin_expect( !!(x), 1 ) )
    #define LGV_UNLIKELY( x )					( __builtin_expect( !!(x), 0 ) )
#else
    #error "Unsupported compiler"
#endif

#pragma optimize( "gt", on )

namespace mx {

	//------------------------------------------------------------------------------
	// large_vector: A Disk-Backed Vector with Automatic Unique File Path Generation
	//------------------------------------------------------------------------------

	/**
	 * \brief A disk–backed vector that inherits from std::vector and stores most of its data on disk.
	 *
	 * The complete set of elements is stored on disk while only a section (or cache) is kept in RAM.
	 * The disk file path is generated automatically based on the current directory, the string "lvec",
	 * and a unique ID that is incremented for each created instance. The parameter _nMaxRamItems
	 * determines how many elements remain in memory at a given time, and _nTotalSize is the overall
	 * number of elements.
	 *
	 *	Note: Iterator support is explicitly removed to avoid repeated flushing and loading.
	 *
	 * \tparam T The type of elements stored.
	 */
	template <typename T, typename Allocator = std::allocator<T>>
	class large_vector : protected std::vector<T, Allocator> {
	public :
		// Expose std::vector’s key types:
		using value_type										= typename std::vector<T, Allocator>::value_type;
		using allocator_type	 								= typename std::vector<T, Allocator>::allocator_type;
		using size_type											= typename std::vector<T, Allocator>::size_type;
		using difference_type									= typename std::vector<T, Allocator>::difference_type;
		using reference											= typename std::vector<T, Allocator>::reference;
		using const_reference									= typename std::vector<T, Allocator>::const_reference;
		using pointer											= typename std::vector<T, Allocator>::pointer;
		using const_pointer										= typename std::vector<T, Allocator>::const_pointer;

		// --- Disable Copy ---
		large_vector( const large_vector & ) = delete;
		large_vector &											operator = ( const large_vector & ) = delete;

		/**
		 * \brief Constructs a large_vector.
		 *
		 * Generates a unique disk file path based on the current directory, "lvec", and a unique ID.
		 *
		 * \param _nMaxRamItems Maximum number of items to keep in RAM at one time.
		 * \param _nTotalSize Total number of elements that the vector will store.
		 */
		large_vector( size_t _nMaxRamItems, size_t _nTotalSize ) :
			std::vector<T, Allocator>(),
			m_nMaxRamItems( _nMaxRamItems ),
			m_nTotalSize( _nTotalSize ),
			m_nCurrentSectionStart( 0 ),
			m_bDirty( false ) {
			// Directory management.
			{
				std::lock_guard<std::mutex> lgLock( s_mMutex );
				if ( s_nInstanceCount == 0 ) {
					s_pBaseDirectory = std::filesystem::current_path() / "lvec";
					std::error_code ecErr;
					if ( !std::filesystem::create_directories( s_pBaseDirectory, ecErr ) && ecErr ) {
						throw std::runtime_error( "Failed to create directories for large_vector file: " + ecErr.message() );
					}
				}
				++s_nInstanceCount;
			}

			try {
				// Generate unique file path.
				uint64_t uID = s_uID.fetch_add( 1, std::memory_order_relaxed );
				m_pPathDiskFile = s_pBaseDirectory / ( "lvec" + std::to_string( uID ) + ".dat" );

				// Create/truncate and initialize.
				{
					std::ofstream ofs( m_pPathDiskFile, std::ios::binary | std::ios::trunc );
					if ( !ofs ) { throw std::runtime_error( "Failed to create disk file." ); }
					T tDefVal{};
					for ( size_t i = 0; i < m_nTotalSize; ++i ) {
						ofs.write( reinterpret_cast<const char *>( &tDefVal ), sizeof( T ) );
					}
				}

				m_ofsDisk.open( m_pPathDiskFile,
								std::ios::in | std::ios::out | std::ios::binary );
				if ( !m_ofsDisk ) {
					throw std::runtime_error( "Failed to open disk file for large_vector." );
				}

				std::vector<T, Allocator>::resize( std::min( m_nMaxRamItems, m_nTotalSize ) );
				loadSection( m_nCurrentSectionStart );
			}
			catch ( ... ) {
				--s_nInstanceCount;
				throw;
			}
		}
		large_vector( large_vector && _lvOther ) noexcept :
			std::vector<T, Allocator>( std::move( _lvOther ) ),
			m_nMaxRamItems( _lvOther.m_nMaxRamItems ),
			m_nTotalSize( _lvOther.m_nTotalSize ),
			m_nCurrentSectionStart( _lvOther.m_nCurrentSectionStart ),
			m_bDirty( _lvOther.m_bDirty ) {
			std::lock_guard<std::mutex> lgLock( s_mMutex );
			++s_nInstanceCount;

			m_pPathDiskFile = std::move( _lvOther.m_pPathDiskFile );
			_lvOther.m_pPathDiskFile.clear();
			_lvOther.m_nTotalSize = 0;

			m_ofsDisk.open( m_pPathDiskFile, std::ios::in | std::ios::out | std::ios::binary );

			m_vWriteBuffer = std::move( _lvOther.m_vWriteBuffer );
			_lvOther.m_vWriteBuffer.clear();
		}

		/**
		 * \brief Destructor that deletes the on-disk file.
		 *
		 * Any errors during file deletion are caught and ignored so that no exceptions are thrown.
		 */
		~large_vector() {
			try {
				// 1) flush buffered appends.
				flushWriteBuffer();
				// 2) flush any dirty cached section.
				try { flushCurrentSection(); } catch ( ... ) {}
				if ( m_ofsDisk.is_open() ) { m_ofsDisk.close(); }
				std::filesystem::remove( m_pPathDiskFile );
			} catch ( ... ) {}

			{
				std::lock_guard<std::mutex> lgLock( s_mMutex );
				--s_nInstanceCount;
				if ( s_nInstanceCount == 0 ) {
					std::error_code ecErr;
					std::filesystem::remove_all( s_pBaseDirectory, ecErr );
					s_uID.store( 0, std::memory_order_relaxed );
				}
			}
		}

		// --- move assignment ---
		large_vector &											operator = ( large_vector && _lvOther ) noexcept {
			if ( this != &_lvOther ) {
				// 1) Cleanup our old file.
				try {
					flushWriteBuffer();
					flushCurrentSection();
					if ( m_ofsDisk.is_open() ) { m_ofsDisk.close(); }
					std::filesystem::remove( m_pPathDiskFile );
				} catch ( ... ) {}

				// 2) Steal base state.
				std::vector<T, Allocator>::operator=( std::move( _lvOther ) );

				// 3) Steal our fields.
				m_nMaxRamItems         = _lvOther.m_nMaxRamItems;
				m_nTotalSize           = _lvOther.m_nTotalSize;
				m_nCurrentSectionStart = _lvOther.m_nCurrentSectionStart;
				m_bDirty               = _lvOther.m_bDirty;

				// 4) Steal file path.
				m_pPathDiskFile = std::move( _lvOther.m_pPathDiskFile );
				_lvOther.m_pPathDiskFile.clear();
				_lvOther.m_nTotalSize = 0;

				// 5) Reopen append handle on our new path.
				if ( m_ofsDisk.is_open() ) { m_ofsDisk.close(); }
				m_ofsDisk.open( m_pPathDiskFile, std::ios::in | std::ios::out | std::ios::binary );

				// 6) Steal the write buffer.
				m_vWriteBuffer = std::move( _lvOther.m_vWriteBuffer );
				_lvOther.m_vWriteBuffer.clear();
			}
			return (*this);
		}

		//-------------------------------------------------------------------------
		// Overridden Base–Class Methods (non–iterator related)
		//-------------------------------------------------------------------------

		/**
		 * \brief Returns the total number of elements stored (on disk and in RAM).
		 *
		 * \return The total number of elements.
		 */
		size_t													size() const {
			return m_nTotalSize;
		}
    
		/**
		 * \brief Checks if the container is empty.
		 *
		 * \return True if there are no elements; false otherwise.
		 */
		bool													empty() const {
			return (m_nTotalSize == 0);
		}
    
		/**
		 * \brief Returns a pointer to the currently cached RAM section.
		 *
		 * \return Pointer to the data of the current section.
		 */
		T *														data() {
			return std::vector<T, Allocator>::data();
		}
    
		/**
		 * \brief Returns a const pointer to the currently cached RAM section.
		 *
		 * \return Const pointer to the current section's data.
		 */
		const T *												data() const {
			return std::vector<T, Allocator>::data();
		}
    
		/**
		 * \brief Provides bounds–checked access to an element.
		 *
		 * \param _nIndex The global index of the element.
		 * \return Reference to the element.
		 * \throws std::out_of_range if _nIndex is outside [0, size()).
		 */
		T &														at( size_t _nIndex ) {
			if ( _nIndex >= m_nTotalSize ) { throw std::out_of_range( "Index out of range in at()." ); }
			return (*this)[_nIndex];
		}
    
		/**
		 * \brief Provides bounds–checked, read–only access to an element.
		 *
		 * \param _nIndex The global index of the element.
		 * \return Const reference to the element.
		 * \throws std::out_of_range if _nIndex is outside [0, size()).
		 */
		const T &												at( size_t _nIndex ) const {
			if ( _nIndex >= m_nTotalSize ) { throw std::out_of_range( "Index out of range in at() const." ); }
			return const_cast<large_vector *>(this)->operator[](_nIndex);
		}
    
		//-------------------------------------------------------------------------
		// Removed Support for front() and back()
		//-------------------------------------------------------------------------
    
		T &														front() = delete;
		const T &												front() const = delete;
		T &														back() = delete;
		const T &												back() const = delete;
    
		//-------------------------------------------------------------------------
		// Overloaded Operator[] with Disk–Backed Caching Logic
		//-------------------------------------------------------------------------

		/**
		 * \brief Provides non–const access to an element using the [] operator.
		 *
		 * If the requested element (global index _nIndex) is not in the currently cached section,
		 * this function flushes the dirty section (if any) to disk and loads the required section.
		 *
		 * \param _nIndex The global index of the element.
		 * \return Reference to the element.
		 * \throws std::out_of_range if _nIndex is not valid.
		 * \throws std::runtime_error if file I/O fails.
		 */
		T &														operator [] ( size_t _nIndex ) {
			if LGV_UNLIKELY( _nIndex >= m_nTotalSize ) {
				throw std::out_of_range( "Index out of range in operator[]." );
			}
			// If _nIndex is outside the currently loaded section, flush and load the appropriate section.
			if ( _nIndex < m_nCurrentSectionStart ||
				_nIndex >= m_nCurrentSectionStart + std::vector<T, Allocator>::size() ) {
				flushWriteBuffer();
				flushCurrentSection();
				size_t nNewSectionStart = (_nIndex / m_nMaxRamItems) * m_nMaxRamItems;
				loadSection( nNewSectionStart );
			}
			m_bDirty = true;
			// Adjust _nIndex to be relative to the cached section.
			return std::vector<T, Allocator>::operator[]( _nIndex - m_nCurrentSectionStart );
		}
    
		/**
		 * \brief Provides const access to an element using the [] operator.
		 *
		 * \param _nIndex The global index of the element.
		 * \return Const reference to the element.
		 */
		const T &												operator [] ( size_t _nIndex ) const {
			return const_cast<large_vector *>(this)->operator[](_nIndex);
		}
    
		//-------------------------------------------------------------------------
		// Disable Iterator Support
		//-------------------------------------------------------------------------
    
		// The following iterator functions are explicitly deleted to prevent unintended
		// iteration that would trigger constant flushing/loading of disk sections.
		typename std::vector<T, Allocator>::iterator			begin() = delete;
		typename std::vector<T, Allocator>::iterator			end() = delete;
		typename std::vector<T, Allocator>::const_iterator		begin() const = delete;
		typename std::vector<T, Allocator>::const_iterator		end() const = delete;
		typename std::vector<T, Allocator>::const_iterator		cbegin() const = delete;
		typename std::vector<T, Allocator>::const_iterator		cend() const = delete;

		//-------------------------------------------------------------------------
		// Modification Functions: push_back, pop_back, insert
		//-------------------------------------------------------------------------
    
		/**
		 * \brief Appends an element to the end of the vector.
		 *
		 * Flushes the current in–RAM cache to disk, then appends _tElem at the end of the file.
		 *
		 * \param _tElem The element to append.
		 * \throws std::runtime_error if file I/O fails.
		 */
		void													push_back( const T & _tElem ) {
			// Ensure cache integrity before we buffer.
			flushCurrentSection();

			// Buffer the element.
			m_vWriteBuffer.push_back( _tElem );
			++m_nTotalSize;
			// Flush in bulk when buffer fills.
			if ( m_vWriteBuffer.size() >= m_nMaxRamItems ) {
				flushWriteBuffer();
			}
		}

		/**
		 * \brief Appends an array of elements to the end of the vector.
		 *
		 * Flushes the current in–RAM cache to disk, then appends _nCount elements from _pArray in a single operation.
		 *
		 * \param _pArray Pointer to the first element of the array to append.
		 * \param _nCount Number of elements to append.
		 * \throws std::runtime_error if file I/O fails.
		 */
		void													push_back( const T * _pArray, size_t _nCount ) {
			flushCurrentSection();

			// if it all fits in the buffer, accumulate.
			size_t sSpace = m_nMaxRamItems - m_vWriteBuffer.size();
			if ( _nCount <= sSpace ) {
				m_vWriteBuffer.insert( m_vWriteBuffer.end(),
					_pArray, _pArray + _nCount );
				m_nTotalSize += _nCount;
				if ( m_vWriteBuffer.size() >= m_nMaxRamItems ) {
					flushWriteBuffer();
				}
			}
			else {
				// Flush existing buffer.
				flushWriteBuffer();
				// Write large block directly.
				m_ofsDisk.seekp( m_nTotalSize * sizeof( T ), std::ios::beg );
				m_ofsDisk.write(
					reinterpret_cast<const char *>( _pArray ),
					_nCount * sizeof( T )
				);
				m_nTotalSize += _nCount;
			}
		}
    
		/**
		 * \brief Removes the last element from the vector.
		 *
		 * Flushes the current cache to disk and then truncates the file to remove the last element.
		 *
		 * \throws std::runtime_error if the vector is empty or file I/O fails.
		 */
		void													pop_back() {
			if ( m_nTotalSize == 0 ) { throw std::runtime_error( "pop_back called on empty vector." ); }
			flushWriteBuffer();
			flushCurrentSection();
			m_ofsDisk.close();
			--m_nTotalSize;
			std::filesystem::resize_file( m_pPathDiskFile, m_nTotalSize * sizeof( T ) );
			m_ofsDisk.open( m_pPathDiskFile, std::ios::in | std::ios::out | std::ios::binary );
			if ( m_nCurrentSectionStart >= m_nTotalSize ) {
				m_nCurrentSectionStart = (m_nTotalSize > 0)
					? (((m_nTotalSize - 1) / m_nMaxRamItems) * m_nMaxRamItems)
					: 0;
			}
			loadSection( m_nCurrentSectionStart );
			m_ofsDisk.seekp( m_nTotalSize * sizeof( T ), std::ios::beg );
		}

		/**
		 * \brief Removes a specified number of elements from the back of the vector.
		 *
		 * Flushes the current in–RAM cache, subtracts _nCount from m_nTotalSize (if valid),
		 * truncates the disk file accordingly, and reloads the current cache if necessary.
		 *
		 * \param _nCount The number of items to pop.
		 * \throws std::runtime_error if _nCount exceeds the current size.
		 */
		void													pop_back( size_t _nCount ) {
			if ( _nCount > m_nTotalSize ) { throw std::runtime_error( "pop_back called with too many items." ); }
			flushWriteBuffer();
			flushCurrentSection();
			m_ofsDisk.close();
			m_nTotalSize -= _nCount;
			std::filesystem::resize_file( m_pPathDiskFile, m_nTotalSize * sizeof( T ) );
			m_ofsDisk.open( m_pPathDiskFile, std::ios::in | std::ios::out | std::ios::binary );
			if ( m_nCurrentSectionStart >= m_nTotalSize ) {
				m_nCurrentSectionStart = (m_nTotalSize > 0)
					? (((m_nTotalSize - 1) / m_nMaxRamItems) * m_nMaxRamItems)
					: 0;
			}
			loadSection( m_nCurrentSectionStart );
			m_ofsDisk.seekp( m_nTotalSize * sizeof( T ), std::ios::beg );
		}
    
		/**
		 * \brief Inserts an element at the specified index.
		 *
		 * Flushes the current cache to disk, then shifts elements in the file to make room for _tElem.
		 * The file is updated and m_nTotalSize increases by one. If the insertion occurs within the
		 * current cached section, the cache is reloaded.
		 *
		 * \param _nIndex The global index at which to insert the element.
		 * \param _tElem The element to insert.
		 * \throws std::out_of_range if _nIndex is greater than the current size.
		 * \throws std::runtime_error if file I/O fails.
		 */
		void													insert( size_t _nIndex, const T & _tElem ) {
			if ( _nIndex > m_nTotalSize ) { throw std::out_of_range( "Insert index out of range." ); }

			flushWriteBuffer();
			flushCurrentSection();

			m_ofsDisk.close();
			size_t sNewSize = m_nTotalSize + 1;
			std::filesystem::resize_file( m_pPathDiskFile, sNewSize * sizeof( T ) );
			m_ofsDisk.open( m_pPathDiskFile, std::ios::in | std::ios::out | std::ios::binary );

			std::vector<T> vBlock;
			vBlock.reserve( m_nMaxRamItems );
			for ( size_t nShiftStart = _nIndex; nShiftStart < m_nTotalSize; nShiftStart += m_nMaxRamItems ) {
				size_t nChunk = std::min( m_nMaxRamItems, m_nTotalSize - nShiftStart );
				size_t nReadPos = m_nTotalSize - nShiftStart - nChunk;

				// Read chunk.
				m_ofsDisk.seekg( nReadPos * sizeof( T ), std::ios::beg );
				vBlock.resize( nChunk );
				m_ofsDisk.read( reinterpret_cast<char *>(vBlock.data()), nChunk * sizeof( T ) );

				// Write chunk shifted +1.
				m_ofsDisk.seekp( (nReadPos + 1) * sizeof( T ), std::ios::beg );
				m_ofsDisk.write( reinterpret_cast<const char *>(vBlock.data()), nChunk * sizeof( T ) );
			}

			m_ofsDisk.seekp( _nIndex * sizeof( T ), std::ios::beg );
			m_ofsDisk.write( reinterpret_cast<const char *>(&_tElem), sizeof( T ) );

			m_nTotalSize = sNewSize;
			if ( _nIndex >= m_nCurrentSectionStart &&
				 _nIndex < m_nCurrentSectionStart + std::vector<T,Allocator>::size() ) {
				loadSection( m_nCurrentSectionStart );
			}

			m_ofsDisk.seekp( m_nTotalSize * sizeof( T ), std::ios::beg );
		}

		/**
		 * \brief Inserts an array of elements at the specified index.
		 *
		 * Flushes buffered push_back(), resizes the file, shifts the tail in blocks,
		 * then writes the new range in one go.
		 *
		 * \param _nIndex The global index at which to insert.
		 * \param _pArray Pointer to the first element to insert.
		 * \param _nCount Number of elements to insert.
		 * \throws std::out_of_range if _nIndex is greater than the current size.
		 */
		void													insert( size_t _nIndex, const T * _pArray, size_t _nCount ) {
			if ( _nIndex > m_nTotalSize ) { throw std::out_of_range( "Insert index out of range." ); }

			flushWriteBuffer();
			flushCurrentSection();

			m_ofsDisk.close();
			size_t sNewSize = m_nTotalSize + _nCount;
			std::filesystem::resize_file( m_pPathDiskFile, sNewSize * sizeof( T ) );
			m_ofsDisk.open( m_pPathDiskFile, std::ios::in | std::ios::out | std::ios::binary );

			std::vector<T> vBlock;
			vBlock.reserve( m_nMaxRamItems );
			for ( size_t nShiftStart = _nIndex; nShiftStart < m_nTotalSize; nShiftStart += m_nMaxRamItems ) {
				size_t nChunk = std::min( m_nMaxRamItems, m_nTotalSize - nShiftStart );
				size_t nReadPos = m_nTotalSize - nShiftStart - nChunk;

				m_ofsDisk.seekg( nReadPos * sizeof( T ), std::ios::beg );
				vBlock.resize( nChunk );
				m_ofsDisk.read( reinterpret_cast<char *>(vBlock.data()), nChunk * sizeof( T ) );

				m_ofsDisk.seekp( (nReadPos + _nCount) * sizeof( T ), std::ios::beg );
				m_ofsDisk.write( reinterpret_cast<const char *>(vBlock.data()), nChunk * sizeof( T ) );
			}

			// Write new block.
			m_ofsDisk.seekp( _nIndex * sizeof( T ), std::ios::beg );
			m_ofsDisk.write( reinterpret_cast<const char *>(_pArray), _nCount * sizeof( T ) );

			m_nTotalSize = sNewSize;
			if ( _nIndex >= m_nCurrentSectionStart &&
				 _nIndex < m_nCurrentSectionStart + std::vector<T,Allocator>::size() ) {
				loadSection( m_nCurrentSectionStart );
			}

			m_ofsDisk.seekp( m_nTotalSize * sizeof( T ), std::ios::beg );
		}

		/**
		 * \brief Erases a number of elements from the front of the vector.
		 *
		 * Flushes any buffered push_back() data and the dirty cache, shifts the remaining
		 * elements in blocks down to index 0, truncates the file, resets the cache, and
		 * repositions the disk handle for future appends.
		 *
		 * \param _nCount The number of elements to remove from the front.
		 * \throws std::runtime_error if _nCount exceeds the current size.
		 */
		void													erase_front( size_t _nCount ) {
			if ( _nCount > m_nTotalSize ) { throw std::runtime_error( "erase_front count too large." ); }

			flushWriteBuffer();
			flushCurrentSection();

			// 2) Compute new size
			size_t sNewSize = m_nTotalSize - _nCount;

			// 3) Shift tail in blocks
			std::vector<T>	vBlock;
			vBlock.reserve( m_nMaxRamItems );
			for ( size_t nSrc = _nCount; nSrc < m_nTotalSize; nSrc += m_nMaxRamItems ) {
				size_t sChunk = std::min( m_nMaxRamItems, m_nTotalSize - nSrc );

				m_ofsDisk.seekg( nSrc * sizeof( T ), std::ios::beg );
				vBlock.resize( sChunk );
				m_ofsDisk.read( reinterpret_cast<char *>( vBlock.data() ), sChunk * sizeof( T ) );

				m_ofsDisk.seekp( (nSrc - _nCount) * sizeof( T ), std::ios::beg );
				m_ofsDisk.write( reinterpret_cast<const char *>( vBlock.data() ), sChunk * sizeof( T ) );
			}

			m_ofsDisk.close();
			std::filesystem::resize_file( m_pPathDiskFile, sNewSize * sizeof( T ) );
			m_ofsDisk.open( m_pPathDiskFile, std::ios::in | std::ios::out | std::ios::binary );

			m_nCurrentSectionStart = 0;
			m_nTotalSize           = sNewSize;
			size_t sLoad           = std::min( m_nMaxRamItems, m_nTotalSize );
			std::vector<T,Allocator>::resize( sLoad );
			if ( sLoad ) {
				loadSection( 0 );
			}

			m_ofsDisk.seekp( m_nTotalSize * sizeof( T ), std::ios::beg );
		}


    
		//-------------------------------------------------------------------------
		// Custom Member Functions: resize() and reserve()
		//-------------------------------------------------------------------------
    
		/**
		 * \brief Resizes the vector to contain _nNewSize elements.
		 *
		 * Flushes the current in–RAM cache to disk and adjusts the on–disk file size:
		 * - If _nNewSize is larger than the current size, appends default–constructed elements.
		 * - If smaller, truncates the file.
		 * Updates m_nTotalSize and reloads the current in–RAM cache if necessary.
		 *
		 * \param _nNewSize The new total number of elements.
		 */
		void													resize( size_t _nNewSize ) {
			flushWriteBuffer();
			flushCurrentSection();

			if ( _nNewSize > m_nTotalSize ) {
				// Expand file: append default-initialized elements via our open append handle.
				m_ofsDisk.seekp( m_nTotalSize * sizeof( T ), std::ios::beg );
				T tDef{};
				for ( size_t i = m_nTotalSize; i < _nNewSize; ++i ) {
					m_ofsDisk.write( reinterpret_cast<const char *>( &tDef ), sizeof( T ) );
				}
			}
			else if ( _nNewSize < m_nTotalSize ) {
				// Truncate the file on disk
				m_ofsDisk.close();
				std::filesystem::resize_file( m_pPathDiskFile, _nNewSize * sizeof( T ) );
				
				m_ofsDisk.open( m_pPathDiskFile, std::ios::in | std::ios::out | std::ios::binary );

				// If our cache start is now past EOF, reload the last block
				if ( m_nCurrentSectionStart >= _nNewSize ) {
					m_nCurrentSectionStart = (_nNewSize > 0)
						? (((_nNewSize - 1) / m_nMaxRamItems) * m_nMaxRamItems)
						: 0;
				}
			}

			// Update size and resize the RAM cache
			m_nTotalSize = _nNewSize;
			size_t nLoad = (m_nTotalSize > m_nCurrentSectionStart)
				? std::min( m_nMaxRamItems, m_nTotalSize - m_nCurrentSectionStart )
				: 0;
			std::vector<T, Allocator>::resize( nLoad );

			loadSection( m_nCurrentSectionStart );

			// Reposition append pointer to new EOF
			m_ofsDisk.seekp( m_nTotalSize * sizeof( T ), std::ios::beg );
		}
    
		/**
		 * \brief Reserves storage. For large_vector, reserve() does nothing.
		 *
		 * \param _nNewCapacity The requested capacity (unused).
		 */
		void													reserve( size_t /*_nNewCapacity*/ ) {
			// Do nothing.
		}

		//-------------------------------------------------------------------------
		// Disk I/O: Flushing and Loading Sections
		//-------------------------------------------------------------------------
	
		/**
		 * \brief Flushes the current in–RAM section to disk.
		 *
		 * If the current section has been modified, its contents are written
		 * to the disk file at the proper offset using the persistent fstream.
		 */
		void													flushCurrentSection() {
			if ( m_bDirty ) {
				m_ofsDisk.clear();
				m_ofsDisk.seekp( m_nCurrentSectionStart * sizeof( T ), std::ios::beg );
				if ( !m_ofsDisk ) { throw std::runtime_error( "Failed to seek for flushCurrentSection()." ); }
				size_t sWriteLen = std::vector<T, Allocator>::size() * sizeof( T );
				m_ofsDisk.write(
					reinterpret_cast<const char *>( std::vector<T, Allocator>::data() ),
					sWriteLen
				);
				if ( !m_ofsDisk ) { throw std::runtime_error( "Failed to write in flushCurrentSection()." ); }
				m_bDirty = false;
			}
		}

		/**
		 * \brief Loads a section from disk into the in–RAM cache.
		 *
		 * The cache is populated with the section starting at _nSectionStart,
		 * by seeking and reading directly from the persistent fstream.
		 *
		 * \param _nSectionStart The starting global index of the section to load.
		 */
		void													loadSection( size_t _nSectionStart ) {
			m_nCurrentSectionStart = _nSectionStart;
			size_t nItemsToLoad = std::min( m_nMaxRamItems, m_nTotalSize - m_nCurrentSectionStart );
			std::vector<T, Allocator>::resize( nItemsToLoad );
			m_ofsDisk.seekg( m_nCurrentSectionStart * sizeof( T ), std::ios::beg );
			if ( !m_ofsDisk ) {
				throw std::runtime_error( "Failed to seek for loadSection()." );
			}
			m_ofsDisk.read(
				reinterpret_cast<char *>( std::vector<T, Allocator>::data() ),
				nItemsToLoad * sizeof( T )
			);
			if ( !m_ofsDisk ) {
				throw std::runtime_error( "Failed to read in loadSection()." );
			}

			m_bDirty = false;
		}

		/**
		 * \brief Flushes any buffered push_back() operations to disk.
		 *
		 * Writes the entire buffer in one go, then clears it.
		 */
		void													flushWriteBuffer() {
			if ( !m_vWriteBuffer.empty() ) {
				// write buffered data at its proper offset
				m_ofsDisk.seekp(
					( m_nTotalSize - m_vWriteBuffer.size() ) * sizeof( T ),
					std::ios::beg
				);
				m_ofsDisk.write(
					reinterpret_cast<const char *>( m_vWriteBuffer.data() ),
					m_vWriteBuffer.size() * sizeof( T )
				);
				m_vWriteBuffer.clear();
				// reposition at EOF for future appends
				m_ofsDisk.seekp( m_nTotalSize * sizeof( T ), std::ios::beg );
			}
		}

    
	private :
		mutable std::fstream									m_ofsDisk;				/**< Single handle for fast append. */
		std::vector<T>											m_vWriteBuffer;			/**< Buffered push_back() elements. */
		std::filesystem::path									m_pPathDiskFile;		/**< Generated disk file path based on the current directory, "lvec", and a unique ID. */
		size_t													m_nMaxRamItems;         /**< Maximum number of items to keep in RAM. */
		size_t													m_nTotalSize;           /**< Total number of elements stored. */
		size_t													m_nCurrentSectionStart; /**< Global index marking the start of the current RAM section. */
		bool													m_bDirty;               /**< True if the current section has been modified. */
    
		/** A static atomic counter for generating unique IDs for each instance. */
		inline static std::atomic<uint64_t>						s_uID{ 0 };

		// Static variables for instance counting and directory management.
		inline static std::atomic<size_t>						s_nInstanceCount{ 0 };
		inline static std::mutex								s_mMutex;
		inline static std::filesystem::path						s_pBaseDirectory;


		// == Functions.
		
	};

}	// namespace mx

#pragma optimize( "", on )
