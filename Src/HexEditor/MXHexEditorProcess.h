#pragma once

#include "../MXMhsX.h"
#include "../MemHack/MXProcess.h"
#include "MXHexEditorInterface.h"


namespace mx {

	class CHexEditorProcess : public CHexEditorInterface {
	public :
		CHexEditorProcess();
		virtual ~CHexEditorProcess();


		// == Functions.
		// Sets the target process.
		bool								SetProcess( DWORD _dwId, CProcess::MX_OPEN_PROC_MODE _opmMode, DWORD _dwFlags );

		// Gets the process object directly.
		const CProcess &					Process() const { return m_pProcess; }

		// Resets the object back to scratch.
		void								Reset();

		// Read from the given data stream.
		virtual bool						Read( uint64_t _ui64Addr, CBuffer &_bDst, size_t _sSize ) const;

		// Does the given data stream allow insertion and deletion?  Processes return true.
		virtual bool						OverwriteOnly() const { return true; }

		// Is the given data stream read-only?
		virtual bool						ReadOnly() const { return !m_pProcess.Writeable(); }

		// Insert bytes at a given address.
		virtual bool						Insert( uint64_t /*_ui64Addr*/, const void * /*_pvSrc*/, size_t /*_sSize*/ ) { return false; }

		// Overwrite bytes at a given address.
		virtual bool						Overwrite( uint64_t _ui64Addr, const void * _pvSrc, size_t _sSize );

		// Delets bytes at a given address.
		virtual bool						Delete( uint64_t _ui64Addr, uint64_t _ui64Size );

		// Gets the size of the data source.
		virtual uint64_t					Size() const;

		// Gets the interface type.
		virtual MX_HEX_EDITOR_TYPES			Type() const { return MX_HET_PROCESS; }

		// Default starting address.
		virtual uint64_t					DefaultAddress() const override;

		// Does the given range of addresses contain anything real-time?
		virtual bool						RangeContainsRealTime( uint64_t _ui64Start, uint64_t _ui64Total ) const override;


	protected :
		// == Members.
		// The target process.
		CProcess							m_pProcess;
		
	};

}	// namespace mx
