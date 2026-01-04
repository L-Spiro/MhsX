#pragma once

#include <algorithm>
#include <cstdint>
#include <vector>


namespace mx {

	// == Enumerations.
	/** Selection mode. */
	enum MX_SELECTION_MODE : uint32_t {
		MX_SM_NORMAL = 0,						/**< Normal mode. */
		MX_SM_COLUMN = 1						/**< Column mode. */
	};


	// == Types.
	/** Selection ranges. */
	struct MX_SEL_RANGE {
		uint64_t								ui64Start						= 0;						/**< The start of a selection range. */
		uint64_t								ui64Size						= 0;						/**< The size of the selection range. */
	};

	/** A Normal selection: [startAddr, endAddr] inclusive, normalized. */
	struct MX_SEL_NORMAL {
		uint64_t								ui64Start			= 0;								/**< Inclusive start address. */
		uint64_t								ui64End				= 0;								/**< Exclusive end address. */


		// == Functions.
		/** Normalize start/end so start <= end. */
		inline void								Normalize() {
			if ( ui64Start > ui64End ) { std::swap( ui64Start, ui64End ); }
		}

		/** True if a single-point selection. */
		inline bool								Empty() const { return ui64Start == ui64End; }

		/**
		 * Determines if a given address is in the selection.
		 * 
		 * \param _ui64Address The address to test for being selected.
		 * \return Returns true if there is a selection and if the given address is part of that selection.
		 **/
		inline bool								IsSelected( uint64_t _ui64Address ) {
			return _ui64Address >= ui64Start && _ui64Address < ui64End;
		}

		/**
		 * Gets the number of selected bytes.
		 * 
		 * \return Returns the number of selected bytes.
		 **/
		inline uint64_t							TotalSelected() const { return ui64End - ui64Start; }

	};

	/**
	 * Column selection anchored to a top-left address.
	 * Width is in columns (cells), height is in lines (rows). Both are >= 1 when valid.
	 * This form survives any bytes-per-row changes; visualization is realized per current layout.
	 */
	struct MX_SEL_COLUMN {
		uint64_t								ui64AnchorAddr		= 0;								/**< Absolute address of the top-left cell. */
		uint32_t								ui32Cols			= 1;								/**< Width in byte columns. */
		uint64_t								ui64Lines			= 1;								/**< Height in rows (lines). */


		// == Functions.
		/** Returns true if degenerate (treated as empty on end-gesture). */
		inline bool								Empty() const { return ui32Cols == 0 /*|| ui64Lines == 0*/; }

		/**
		 * Determines if a given address is in the selection.
		 * 
		 * \param _ui64Address The address to test for being selected.
		 * \param _ui32BytesPerRow The number of bytes in a row.
		 * \return Returns true if there is a selection and if the given address is part of that selection.
		 **/
		inline bool								IsSelected( uint64_t _ui64Address, uint32_t _ui32BytesPerRow ) {
			uint64_t ui64ThisSize = (ui64Lines * _ui32BytesPerRow) + ui32Cols;
			if ( _ui64Address < ui64AnchorAddr || _ui64Address > ui64AnchorAddr + ui64ThisSize ) { return false; }

			uint64_t ui64AddrCol = (_ui64Address - ui64AnchorAddr) % _ui32BytesPerRow;
			return ui64AddrCol < ui32Cols;
		}

		/**
		 * Gets the number of selected bytes.
		 * 
		 * \return Returns the number of selected bytes.
		 **/
		inline uint64_t							TotalSelected() const { return (ui64Lines + 1) * ui32Cols; }
	};

	/**
	 * The active selection (either Normal or Column).
	 * Stores payload for the active mode and exposes helpers shared by both.
	 */
	struct MX_SELECTION {
		MX_SELECTION_MODE						smMode				= MX_SM_NORMAL;						/**< Active selection mode. */
		bool									bHas				= false;							/**< True if there is an active selection. */
		MX_SEL_NORMAL							sn;														/**< Normal selection payload. */
		MX_SEL_COLUMN							sc;														/**< Column selection payload. */


		// == Functions.
		/**
		 * Is there an actual selection?
		 * 
		 * \return Returns true if both bHas is true and the selection region has a non-0 area.
		 **/
		inline bool								HasSelection() const {
			if ( !bHas ) { return false; }
			if ( smMode == MX_SM_NORMAL ) { return !sn.Empty(); }
			return !sc.Empty();
		}

		/**
		 * Gets the number of selected bytes.
		 * 
		 * \return Returns the number of selected bytes.
		 **/
		inline uint64_t							TotalSelected() const {
			return HasSelection() ?
				((smMode == MX_SM_NORMAL) ? sn.TotalSelected() : sc.TotalSelected()) : 0;
		}

		/**
		 * True if selection is limited to a single logical row (for Normal) or single row (for Column).
		 * Used for conversion logic.
		 *
		 * \param _ui32BytesPerRow Current bytes-per-row.
		 * \return Returns true if the selection occupies exactly one logical row.
		 */
		inline bool								IsSingleRow( uint32_t _ui32BytesPerRow ) const {
			if ( smMode == MX_SM_COLUMN ) {
				return sc.ui64Lines == 1;
			}
			const uint64_t ui64StartLine = sn.ui64Start / _ui32BytesPerRow;
			const uint64_t ui64EndLine   = sn.ui64End   / _ui32BytesPerRow;
			return ui64StartLine == ui64EndLine;
		}

		/**
		 * Determines if a given address is in the selection.
		 * 
		 * \param _ui64Address The address to test for being selected.
		 * \param _ui32BytesPerRow The number of bytes in a row.
		 * \return Returns true if there is a selection and if the given address is part of that selection.
		 **/
		inline bool								IsSelected( uint64_t _ui64Address, uint32_t _ui32BytesPerRow ) {
			if ( !bHas ) { return false; }
			if ( smMode == MX_SM_NORMAL ) {
				return sn.IsSelected( _ui64Address );
			}
			return sc.IsSelected( _ui64Address, _ui32BytesPerRow );
		}

		/**
		 * Gathers the selected ranges.  Addresses will be sorted in descanding order (higher addresses first).
		 * 
		 * \param _ui32BytesPerRow The number of bytes in a row.
		 * \param _vSelections Holds the returned selection ranges sorted from high-to-low.
		 * \return Returns true if allocating all of the selection ranges succeeded or there is no selection.  False always indicates a memory failure.
		 **/
		inline bool								GatherSelected_HighToLow( uint32_t _ui32BytesPerRow, std::vector<MX_SEL_RANGE> &_vSelections ) {
			_vSelections.clear();
			if ( !HasSelection() ) { return true; }
			try {
				if ( smMode == MX_SM_NORMAL ) {	
					// Known not to be empty.
					// There is only one selection range.
					MX_SEL_RANGE srRange = { .ui64Start = sn.ui64Start, .ui64Size = sn.ui64End - sn.ui64Start };
					_vSelections.push_back( srRange );
					return true;
				}

				// Column Mode.
				// Known not to be empty.
				for ( size_t I = sc.ui64Lines + 1; I--; ) {
					MX_SEL_RANGE srRange = { .ui64Start = sc.ui64AnchorAddr + I * _ui32BytesPerRow, .ui64Size = sc.ui32Cols };
					_vSelections.push_back( srRange );
				}
				return true;
			}
			catch ( ... ) { return false; }
		 }

		/**
		 * Gathers the selected ranges.  Addresses will be sorted in ascanding order (lower addresses first).
		 * 
		 * \param _ui32BytesPerRow The number of bytes in a row.
		 * \param _vSelections Holds the returned selection ranges sorted from low-to-high.
		 * \return Returns true if allocating all of the selection ranges succeeded or there is no selection.  False always indicates a memory failure.
		 **/
		inline bool								GatherSelected_LowToHigh( uint32_t _ui32BytesPerRow, std::vector<MX_SEL_RANGE> &_vSelections ) {
			_vSelections.clear();
			if ( !HasSelection() ) { return true; }
			try {
				if ( smMode == MX_SM_NORMAL ) {	
					// Known not to be empty.
					// There is only one selection range.
					MX_SEL_RANGE srRange = { .ui64Start = sn.ui64Start, .ui64Size = sn.ui64End - sn.ui64Start };
					_vSelections.push_back( srRange );
					return true;
				}

				// Column Mode.
				// Known not to be empty.
				for ( size_t I = 0; I <= sc.ui64Lines; ++I ) {
					MX_SEL_RANGE srRange = { .ui64Start = sc.ui64AnchorAddr + I * _ui32BytesPerRow, .ui64Size = sc.ui32Cols };
					_vSelections.push_back( srRange );
				}
				return true;
			}
			catch ( ... ) { return false; }
		 }

		 /**
		 * Gathers a part of the selected ranges.  Addresses will be grouped into chunks of _ui64GroupSize, and you can request an index into those groupings to be returned via _ui64GroupIdx.
		 *	This prevents long freezes or memory failures when millions of rows have been selected in column mode.
		 *	Addresses will be sorted in descanding order (higher addresses first).
		 * 
		 * \param _ui32BytesPerRow The number of bytes in a row.
		 * \param _vSelections Holds the returned selection ranges sorted from high-to-low.
		 * \param _ui64GroupSize The size of each group to gather.
		 * \param _ui64GroupIdx The index of the group to gather.
		 * \return Returns true if allocating all of the selection ranges succeeded or there is no selection.  False always indicates a memory failure.
		 **/
		inline bool								GatherSelected_HighToLow( uint32_t _ui32BytesPerRow, std::vector<MX_SEL_RANGE> &_vSelections, uint64_t _ui64GroupSize, uint64_t _ui64GroupIdx ) {
			_vSelections.clear();
			if ( !HasSelection() ) { return true; }
			try {
				if ( !_ui64GroupSize ) { return false; }
				if ( smMode == MX_SM_NORMAL ) {
					if ( _ui64GroupIdx != 0 ) { return true; }
					// Known not to be empty.
					// There is only one selection range.
					MX_SEL_RANGE srRange = { .ui64Start = sn.ui64Start, .ui64Size = sn.ui64End - sn.ui64Start };
					_vSelections.push_back( srRange );
					return true;
				}

				// Column Mode.
				// Known not to be empty.
				
				uint64_t ui64Total = sc.ui64Lines + 1;
				uint64_t ui64TotalGroups = (ui64Total % _ui64GroupSize == 0) ? (ui64Total / _ui64GroupSize) : (ui64Total / _ui64GroupSize + 1);
				if ( _ui64GroupIdx >= ui64TotalGroups ) { return true; }
				uint64_t ui64NewTotal = std::min( _ui64GroupSize, ui64Total - _ui64GroupSize * _ui64GroupIdx );
				uint64_t ui64Offset = ui64Total - _ui64GroupSize * _ui64GroupIdx - ui64NewTotal;
				for ( size_t I = ui64NewTotal; I--; ) {
					MX_SEL_RANGE srRange = { .ui64Start = sc.ui64AnchorAddr + (I + ui64Offset) * _ui32BytesPerRow, .ui64Size = sc.ui32Cols };
					_vSelections.push_back( srRange );
				}
				return true;
			}
			catch ( ... ) { return false; }
		 }

		/**
		 * Gathers a part of the selected ranges.  Addresses will be grouped into chunks of _ui64GroupSize, and you can request an index into those groupings to be returned via _ui64GroupIdx.
		 *	This prevents long freezes or memory failures when millions of rows have been selected in column mode.
		 *	Addresses will be sorted in ascanding order (lower addresses first).
		 * 
		 * \param _ui32BytesPerRow The number of bytes in a row.
		 * \param _vSelections Holds the returned selection ranges sorted from low-to-high.
		 * \param _ui32BytesPerRow The number of bytes in a row.
		 * \param _ui32BytesPerRow The number of bytes in a row.
		 * \return Returns true if allocating all of the selection ranges succeeded or there is no selection.  False always indicates a memory failure.
		 **/
		inline bool								GatherSelected_LowToHigh( uint32_t _ui32BytesPerRow, std::vector<MX_SEL_RANGE> &_vSelections, uint64_t _ui64GroupSize, uint64_t _ui64GroupIdx ) {
			_vSelections.clear();
			if ( !HasSelection() ) { return true; }
			try {
				if ( !_ui64GroupSize ) { return false; }
				if ( smMode == MX_SM_NORMAL ) {
					if ( _ui64GroupIdx != 0 ) { return true; }
					// Known not to be empty.
					// There is only one selection range.
					MX_SEL_RANGE srRange = { .ui64Start = sn.ui64Start, .ui64Size = sn.ui64End - sn.ui64Start };
					_vSelections.push_back( srRange );
					return true;
				}

				// Column Mode.
				// Known not to be empty.
				uint64_t ui64Total = sc.ui64Lines + 1;
				uint64_t ui64TotalGroups = (ui64Total % _ui64GroupSize == 0) ? (ui64Total / _ui64GroupSize) : (ui64Total / _ui64GroupSize + 1);
				if ( _ui64GroupIdx >= ui64TotalGroups ) { return true; }
				uint64_t ui64NewTotal = std::min( _ui64GroupSize, ui64Total - _ui64GroupSize * _ui64GroupIdx );
				uint64_t ui64Offset = _ui64GroupSize * _ui64GroupIdx;
				for ( size_t I = 0; I <= ui64NewTotal; ++I ) {
					MX_SEL_RANGE srRange = { .ui64Start = sc.ui64AnchorAddr + (I + ui64Offset) * _ui32BytesPerRow, .ui64Size = sc.ui32Cols };
					_vSelections.push_back( srRange );
				}
				return true;
			}
			catch ( ... ) { return false; }
		 }

		/**
		 * Gets the lower-left address of the selection.  Lower refers to the address, which means visually this is the upper-left address.
		 * 
		 * \return Returns the lower-left selection address or 0.
		 **/
		inline uint64_t							LowerLeftAddress() const {
			if ( !HasSelection() ) { return 0; }
			if ( smMode == MX_SM_NORMAL ) { return sn.ui64Start; }
			return sc.ui64AnchorAddr;
		}

		/**
		 * Gets the upper-right address of the selection.  Upper refers to the address, which means visually this is the lower-left address.
		 * 
		 * \param _ui32BytesPerRow The number of bytes in a row.
		 * \return Returns the upper-right selection address or 0.
		 **/
		inline uint64_t							UpperRightAddress( uint32_t _ui32BytesPerRow ) const {
			if ( !HasSelection() ) { return 0; }
			if ( smMode == MX_SM_NORMAL ) { return sn.ui64End; }
			return sc.ui64AnchorAddr + (sc.ui64Lines * _ui32BytesPerRow) + sc.ui32Cols;
		}
	};

}	// namespace mx
