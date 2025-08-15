#pragma once

#include "../MXMhsX.h"
#include "../Utilities/MXStream.h"

#include <CriticalSection/LSWCriticalSection.h>

#include <LSONJson.h>

#include <set>
#include <string>
#include <vector>

#define MX_HOTKEY_VERSION				1
#define MX_HOTKEY_ID_BASE				0x1000


namespace mx {

	class CHotkeyManBase {
		friend class					CHotkeyManager;
	public :
		CHotkeyManBase();
		virtual ~CHotkeyManBase();


		// == Types.
		// A hotkey function.  _uiParm0 is supplied by whatever registered the function with the hotkey manager.
		typedef void (__stdcall *		PfHotkeyHandler)( uint64_t _uiParm0, uint64_t _uiParm1, uint64_t _uiParm2, uint64_t _uiParm3 );

		// A hotkey description (UTF-8).
		struct MX_HOTKEY_DESC {
			const char *				pcDesc;						// The text.
			size_t						sLen;						// Length of the text or 0 for un-encrypted 0-terminated.
		};

		// Functions.
		enum MX_FUNCTION : uint32_t {
			MX_F_NOP,
			MX_F_BEEP,
			MX_F_USER,												// New functions should start at (MX_F_USER + 0).
		};

		// A hotkey handler.
		struct MX_HOTKEY_HANDLER {
			uint64_t					uiFuncParm0;				// The first value to pass to the function.
			PfHotkeyHandler				pfFunc;						// The actual function called when the hotkey is hit.
			MX_HOTKEY_DESC				hdName;						// Name of the function.
			uint32_t					uiId;						// Function ID.
			size_t						sParms;						// Total parameters (up to 3).
			MX_HOTKEY_DESC				hdParms[3];					// The parameter descriptions.
			MX_HOTKEY_DESC				hdFormattings[3];			// Optional string to format the parameters for text.
		};

		// A hotkey instance.
		struct MX_HOTKEY_INSTANCE {
			MX_HOTKEY_HANDLER *			phhHandler;
			uint64_t					uiParms[3];					// Parameters to pass to the handler.
			INT							iCode;						// Virtual key code.
			INT							iMod;						// Key-code modifiers.
			UINT						uiScanCode;					// The scan code for printing.
			INT							iIdentifier;				// Identifier of the hotkey.  Assigned upon adding.
			bool						bSavable;					// Is it savable?
		};

		// Safely locks the hotkey manager.
		struct MX_HOTKEY_LOCK {
			MX_HOTKEY_LOCK( const CHotkeyManBase * _phmbHotkeyMan ) :
				phkmHotkeyMan( _phmbHotkeyMan ) {
				if ( phkmHotkeyMan ) { phkmHotkeyMan->Lock(); }
			}
			~MX_HOTKEY_LOCK() {
				if ( phkmHotkeyMan ) { phkmHotkeyMan->Unlock(); }
			}


		protected :
			const CHotkeyManBase *		phkmHotkeyMan;
		};


		// == Functions.
		// Loads hotkey data from a file loaded to memory.
		virtual bool					LoadFromMemory( const lson::CJsonContainer::LSON_JSON_VALUE * _pjJson, lson::CJsonContainer * _pjcContainer, CStream * _psBinary, uint32_t _uiVersion );

		// Saves hotkey data to an in-memory file image.
		virtual bool					SaveToMemory( lson::CJson::LSON_ELEMENT * _peJson, CStream * _psBinary ) const;

		// Counts the number of hotkeys the trigger on the given virtual key and modifier.
		virtual size_t					Count( INT _iCode, INT _iMod ) const {
			size_t sRet = 0;
			for ( size_t I = m_vHotkeys.size(); I--; ) {
				sRet += (m_vHotkeys[I].iCode == _iCode && m_vHotkeys[I].iMod == _iMod);
			}
			return sRet;
		}

		// Returns a boolean indicating whether the given key and modifier exist.
		virtual bool					HasKey( INT _iCode, INT _iMod ) const {
			for ( size_t I = m_vHotkeys.size(); I--; ) {
				if ( m_vHotkeys[I].iCode == _iCode && m_vHotkeys[I].iMod == _iMod ) { return true; }
			}
			return false;
		}

		// Adds a key and returns the ID or 0.
		virtual LONG					AddKey( INT _iCode, INT _iMod, UINT _uiScanCode, uint32_t _ui32Function,
											uint64_t _ui64Parm0, uint64_t _ui64Parm1, uint64_t _ui64Parm2,
											std::string &_sError );

		// Begins polling or registers the hotkeys.
		virtual bool					Start() = 0 { return false; }

		// Stops polling or unregisters the hotkeys.
		virtual bool					Stop() = 0 { return false; }

		// Removes all hotkeys.
		virtual void					Clear() { m_vHotkeys.clear(); }

		// Gets the total number of hotkeys active.
		virtual size_t					TotalHotkeys() const { return m_vHotkeys.size(); }

		// Gets a hotkey instance by index.
		virtual MX_HOTKEY_INSTANCE &	Get( size_t _sIdx ) { return m_vHotkeys[_sIdx]; }

		// Resets the state back to default.
		virtual void					Reset() { Clear(); }

		// Deletes a key by its ID.
		virtual bool					DeleteById( INT _iId ) {
			for ( size_t I = m_vHotkeys.size(); I--; ) {
				if ( m_vHotkeys[I].iIdentifier == _iId ) {
					m_vHotkeys.erase( m_vHotkeys.begin() + I );
					return true;
				}
			}
			return false;
		}

		// Moves a bunch of hotkeys up in the list by ID.
		virtual bool					MoveUp( const std::set<INT> &_vMoveUs );

		// Moves a bunch of hotkeys down in the list by ID.
		virtual bool					MoveDown( const std::set<INT> &_vMoveUs );

		// Gets the first hotkey with the given key combination.
		virtual const MX_HOTKEY_INSTANCE *
										Get( INT _iCode, INT _iMod ) {
			size_t sIdx = 0;
			return Get( _iCode, _iMod, sIdx );
		}

		// Gets the first hotkey with the given key combination.
		virtual const MX_HOTKEY_INSTANCE *
										Get( INT _iCode, INT _iMod, size_t &_sIdx ) const {
			for ( size_t I = m_vHotkeys.size(); I--; ) {
				if ( m_vHotkeys[I].iCode == _iCode && m_vHotkeys[I].iMod == _iMod ) {
					_sIdx = I;
					return &m_vHotkeys[I];
				}
			}
			return nullptr;
		}

		// Gets a hotkey by its ID.
		virtual const MX_HOTKEY_INSTANCE *
										GetById( INT _iId ) const {
			for ( size_t I = m_vHotkeys.size(); I--; ) {
				if ( m_vHotkeys[I].iIdentifier == _iId ) {
					return &m_vHotkeys[I];
				}
			}
			return nullptr;
		}

		// Locks for updating.
		virtual void					Lock() const { m_csCrit.EnterCriticalSection(); }

		// Unlocks.
		virtual void					Unlock() const { m_csCrit.LeaveCriticalSection(); }

		// Copies the object.
		virtual bool					Copy( const CHotkeyManBase &_hmbOther ) = 0 { m_vHotkeys = _hmbOther.m_vHotkeys; return true; }

		// Execute by index.
		void							ExecuteByindex( size_t _sIdx ) {
			if ( _sIdx < m_vHotkeys.size() ) {
				m_vHotkeys[_sIdx].phhHandler->pfFunc( m_vHotkeys[_sIdx].phhHandler->uiFuncParm0, m_vHotkeys[_sIdx].uiParms[0], m_vHotkeys[_sIdx].uiParms[1], m_vHotkeys[_sIdx].uiParms[2] );
			}
		}

		// Gets a new ID for a hotkey.
		static INT						NewId() { return ::InterlockedAdd( &m_lId, 1 ) + MX_HOTKEY_ID_BASE; }

		// Gets the handler for a hotkey based off its enumeration.
		static MX_HOTKEY_HANDLER *		HandlerByFunctionId( MX_FUNCTION _fFunc );

		// Registers a hotkey handler.
		static bool						RegisterHotkeyHandler( const MX_HOTKEY_HANDLER &_hhHandler );


	protected :
		// == Members.
		// The list of hotkeys.
		std::vector<MX_HOTKEY_INSTANCE>	m_vHotkeys;
		// The start/stop count.
		LONG							m_lStartStopCount;
		// The critical section for this manager.
		lsw::CCriticalSection			m_csCrit;
		// The list of hotkey handlers.
		static std::vector<CHotkeyManBase::MX_HOTKEY_HANDLER>
										m_vHandlers;
		/*static CHotkeyManBase::MX_HOTKEY_HANDLER
										m_hhHandlers[];*/
		// The unique ID for each hotkey.
		static LONG						m_lId;
		// The critical section for handling the handler array.
		static lsw::CCriticalSection	m_csHandlerCrit;
		// The instance count.
		static uint32_t					m_uiInstanceCount;


		// == Functions.
		// Utility to be used by the inheriting classes for their own bookkeeping of the start/stop count.
		LONG							IncStartStopCount() { return ::InterlockedIncrement( &m_lStartStopCount ); /* Returns post-increment. */ }

		// Utility to be used by the inheriting classes for their own bookkeeping of the start/stop count.
		LONG							DecStartStopCount() { return ::InterlockedDecrement( &m_lStartStopCount ); /* Returns post-decrement. */ }

		// The no-action hotkey.
		static void __stdcall			Hotkey_NoAction( uint64_t /*_uiParm0*/, uint64_t /*_uiParm1*/, uint64_t /*_uiParm2*/, uint64_t /*_uiParm3*/ ) {}

		// The beeping hotkey.
		static void __stdcall			Hotkey_Beep( uint64_t /*_uiParm0*/, uint64_t _uiParm1, uint64_t _uiParm2, uint64_t /*_uiParm3*/ );
	};

}	// namespace mx
