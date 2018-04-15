#include "MXSearcher.h"
#include "../System/MXSystem.h"

#include <algorithm>


namespace mx {

	// == Functions.
	// Fill a list of chunks.
	bool CSearcher::GenerateChunks( CAddressChunkList &_aclList, uint64_t _ui64MaxLen, uint64_t _ui64Start, uint64_t _ui64End ) {
		if ( !_ui64MaxLen ) { return false; }
		uint64_t aStart = _ui64Start;
		MEMORY_BASIC_INFORMATION64 mbiInfo;
		uint64_t ui64TotalAreaAdded = 0;

		while ( aStart < _ui64End ) {
			if ( !m_ppProcess->VirtualQueryEx( aStart, &mbiInfo ) ) { break; }
			if ( mbiInfo.BaseAddress > _ui64End ) { break; }

			// Clamp start.
			if ( mbiInfo.BaseAddress < aStart ) {
				uint64_t ui64Dif = aStart - mbiInfo.BaseAddress;
				mbiInfo.BaseAddress += ui64Dif;
				mbiInfo.RegionSize -= ui64Dif;
			}
			// Clamp end.
			if ( mbiInfo.BaseAddress + mbiInfo.RegionSize > _ui64End ) {
				uint64_t ui64Dif = (mbiInfo.BaseAddress + mbiInfo.RegionSize) - _ui64End;
				mbiInfo.RegionSize -= ui64Dif;
			}

			// Move the cursor to the next address.
			aStart += mbiInfo.RegionSize;

			if ( mbiInfo.State & MEM_FREE ) { continue; }
			if ( mbiInfo.Protect & PAGE_NOACCESS ) { continue; }

			// Add the current chunk in separate chunks, each a maximum of _ui64MaxLen bytes.
			uint64_t ui64ChunkEnd = mbiInfo.BaseAddress + mbiInfo.RegionSize;
			while ( mbiInfo.BaseAddress < ui64ChunkEnd ) {
				uint64_t ui64Size = std::min( mbiInfo.RegionSize, _ui64MaxLen );
				_aclList.Add( mbiInfo.BaseAddress, ui64Size );
				mbiInfo.BaseAddress += ui64Size;
				ui64TotalAreaAdded += ui64Size;
			}

		}

		//SYSTEM_INFO lpvEnd = CSystem::GetSystemInfo();

		return true;
	}

}	// namespace mx
