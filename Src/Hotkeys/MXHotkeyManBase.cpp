#include "MXHotkeyManBase.h"
#include "../Utilities/MXUtilities.h"


namespace mx {

	// == Functions.
	// The list of hotkey handlers.
	std::vector<CHotkeyManBase::MX_HOTKEY_HANDLER> CHotkeyManBase::m_vHandlers;

	// The unique ID for each hotkey.
	LONG CHotkeyManBase::m_lId = 0;

	// The critical section for handling the handler array.
	lsw::CCriticalSection CHotkeyManBase::m_csHandlerCrit;

	// The instance count.
	uint32_t CHotkeyManBase::m_uiInstanceCount = 0;

	CHotkeyManBase::CHotkeyManBase() :
		m_lStartStopCount( 0 ) {
		lsw::CCriticalSection::CEnterCrit ecCrit( m_csHandlerCrit );
		if ( !m_uiInstanceCount++ ) {
			{
																//uiFuncParm0	pfFunc				hdName							uiId										sParms	hdParms																			hdFormattings
				CHotkeyManBase::MX_HOTKEY_HANDLER hhHandler = { 0,				Hotkey_NoAction,	{ _T_LEN_C78D6FB7_No_Action },	CHotkeyManBase::MX_F_NOP,					0 };
				m_vHandlers.push_back( hhHandler );
			}
			{
																//uiFuncParm0	pfFunc				hdName							uiId										sParms	hdParms																			hdFormattings
				CHotkeyManBase::MX_HOTKEY_HANDLER hhHandler = { 0,				Hotkey_Beep,		{ _T_LEN_16CAA8BF_Beep },		CHotkeyManBase::MX_F_BEEP,					2,		{ { _T_LEN_A48E3AB0_Frequency }, { _T_LEN_F212238A_Milliseconds } },			{ { _T_LEN_A3DC47A5__I64uHz }, { _T_LEN_32F32FE6__I64ums } } };
				m_vHandlers.push_back( hhHandler );
			}
		}
	}
	CHotkeyManBase::~CHotkeyManBase() {
		lsw::CCriticalSection::CEnterCrit ecCrit( m_csHandlerCrit );
		if ( !--m_uiInstanceCount ) {
			m_vHandlers.clear();
			m_vHandlers = std::vector<CHotkeyManBase::MX_HOTKEY_HANDLER>();
		}
	}

	// Adds a key and returns the ID or 0.
	LONG CHotkeyManBase::AddKey( INT _iCode, INT _iMod, UINT _uiScanCode, uint32_t _ui32Function,
		uint64_t _ui64Parm0, uint64_t _ui64Parm1, uint64_t _ui64Parm2,
		std::string &_sError ) {
		
		MX_HOTKEY_INSTANCE hiInstance = { 0 };
		hiInstance.phhHandler = HandlerByFunctionId( static_cast<MX_FUNCTION>(_ui32Function) );
		if ( !hiInstance.phhHandler ) {
			_sError = _DEC_S_1468A1DF_Internal_error_.c_str();
			return 0;
		}
		hiInstance.iCode = _iCode;
		hiInstance.iMod = _iMod;
		hiInstance.uiScanCode = _uiScanCode;
		hiInstance.uiParms[0] = _ui64Parm0;
		hiInstance.uiParms[1] = _ui64Parm1;
		hiInstance.uiParms[2] = _ui64Parm2;
		hiInstance.iIdentifier = NewId();
		try { m_vHotkeys.push_back( hiInstance ); }
		catch ( const std::bad_alloc /*& _eE*/ ) {
			_sError = _DEC_S_1468A1DF_Internal_error_.c_str();
			return 0;
		}
		return hiInstance.iIdentifier;
	}

	// Moves a bunch of hotkeys up in the list by ID.
	bool CHotkeyManBase::MoveUp( const std::set<INT> &_vMoveUs ) {
		size_t sLowestIdx = -1;
		for ( size_t I = 0; I < m_vHotkeys.size(); ++I ) {
			if ( _vMoveUs.count( m_vHotkeys[I].iIdentifier ) ) {
				if ( I == 0 ) {
					// The first item cannot be moved up.
					// Mark it as unmovable.
					sLowestIdx = 0;
				}
				else {
					// There is at least one item in front of this one.  If it was not marked, it can be swapped.
					if ( sLowestIdx != I - 1 ) {
						MX_HOTKEY_INSTANCE hiTmp = m_vHotkeys[I];
						m_vHotkeys[I] = m_vHotkeys[I-1];
						m_vHotkeys[I-1] = hiTmp;
					}
					else {
						// The item in front of this one could not be moved, so this one can't be moved.
						sLowestIdx = I;
					}
				}
			}
		}
		return true;
	}

	// Moves a bunch of hotkeys down in the list by ID.
	bool CHotkeyManBase::MoveDown( const std::set<INT> &_vMoveUs ) {
		size_t sLowestIdx = -1;
		for ( size_t I = m_vHotkeys.size(); I--; ) {
			if ( _vMoveUs.count( m_vHotkeys[I].iIdentifier ) ) {
				if ( I == m_vHotkeys.size() - 1 ) {
					// The last item cannot be moved back.
					// Mark it as unmovable.
					sLowestIdx = I;
				}
				else {
					// There is at least one item in front of this one.  If it was not marked, it can be swapped.
					if ( sLowestIdx != I + 1 ) {
						MX_HOTKEY_INSTANCE hiTmp = m_vHotkeys[I];
						m_vHotkeys[I] = m_vHotkeys[I+1];
						m_vHotkeys[I+1] = hiTmp;
					}
					else {
						// The item behind this one could not be moved, so this one can't be moved.
						sLowestIdx = I;
					}
				}
			}
		}
		return true;
	}

	// Gets the handler for a hotkey based off its enumeration.
	CHotkeyManBase::MX_HOTKEY_HANDLER * CHotkeyManBase::HandlerByFunctionId( MX_FUNCTION _fFunc ) {
		lsw::CCriticalSection::CEnterCrit ecCrit( m_csHandlerCrit );
		for ( size_t I = m_vHandlers.size(); I--; ) {
			if ( m_vHandlers[I].uiId == _fFunc ) { return &m_vHandlers[I]; }
		}
		return nullptr;
	}

	// Registers a hotkey handler.
	bool CHotkeyManBase::RegisterHotkeyHandler( const MX_HOTKEY_HANDLER &_hhHandler ) {
		lsw::CCriticalSection::CEnterCrit ecCrit( m_csHandlerCrit );
		try { m_vHandlers.push_back( _hhHandler ); }
		catch ( const std::bad_alloc /*& _eE*/ ) { return false;  }
		return true;
	}

	// The beeping hotkey.
	void __stdcall CHotkeyManBase::Hotkey_Beep( uint64_t /*_uiParm0*/, uint64_t _uiParm1, uint64_t _uiParm2, uint64_t /*_uiParm3*/ ) {
		::Beep( static_cast<DWORD>(_uiParm1), static_cast<DWORD>(_uiParm2) );
	}

}	// namespace mx
