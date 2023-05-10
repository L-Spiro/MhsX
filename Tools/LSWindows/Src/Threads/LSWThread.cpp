#include "LSWThread.h"

namespace lsw {

	CThread::CThread() :
		m_plCancel( nullptr ),
		m_dwId( 0 ) {
	}
	CThread::~CThread() {
	}

	// == Functions.
	// Creates a thread.  The previous thread's handle is released but the thread, if any, is not stopped before
	//	this new thread is created.  If there is another thread created by this object, it should be stopped manually
	//	before using the same object to create another thread.
	// If _plCancelObj is supplied, it is assumed to point to data on the target thread which the target thread checks as a sign to exit.  Stop() is only useful
	//	if this is provided.
	BOOL CThread::CreateThread( LPTHREAD_START_ROUTINE _lpStartAddress, LPVOID _lpParameter, volatile LONG * _plCancelObj, LPSECURITY_ATTRIBUTES _lpsaAttribs, SIZE_T _stStackSize, DWORD _dwCreationFlags ) {
		Reset();
		
		m_plCancel = _plCancelObj;

		m_hThread.hHandle = ::CreateThread( _lpsaAttribs, _stStackSize, _lpStartAddress, _lpParameter, _dwCreationFlags, &m_dwId );
		return m_hThread.Valid();
	}

	// Stops the current thread, if _plCancelObj was supplied in CreateThread().
	// It performs an InterlockedExchange() call and returns the previous value.
	LONG CThread::Stop( LONG _lNewStopVal ) {
		if ( m_plCancel && m_hThread.Valid() ) {
			return ::InterlockedExchange( m_plCancel, _lNewStopVal );
		}
		return 0;
	}

	// Waits on the current thread for the given number of milliseconds.
	DWORD CThread::WaitFor( DWORD _dwMilliseconds ) {
		if ( m_hThread.Valid() ) {
			return ::WaitForSingleObject( m_hThread.hHandle, _dwMilliseconds );
		}
		return WAIT_FAILED;
	}

	// Stops the current thread and waits for the return.
	DWORD CThread::StopAndWait( LONG _lNewStopVal, DWORD _dwMilliseconds ) {
		Stop( _lNewStopVal );
		DWORD dwRet;
		while ( (dwRet = WaitFor( _dwMilliseconds )) == WAIT_TIMEOUT ) {}
		return dwRet;
	}

	// Fully resets the thread without stopping it.  Closes its internal handle, sets the ID to 0, and NULL-ifies the atom pointer.  The thread can't be
	//	stopped after these are reset, so StopAndWait() should be called first.
	VOID CThread::Reset() {
		// Doesn't stop the thread.  Just keeps resources allocated correctly.
		//	But if this is the last reference to the previous thread, it will no longer be accessible after this.
		m_hThread.Reset();
		m_plCancel = nullptr;
		m_dwId = 0;
	}

	// Exits and kills the thread.  This should only be used when all resources have been cleaned up and no more useful information can be gleaned from the thread.
	VOID CThread::ExitThread( DWORD /*_dxExitCode*/ ) {
		if ( m_dwId || m_hThread.Valid() ) {
			DWORD dwId = m_dwId;
			Reset();
			::ExitThread( dwId );
		}
	}

	// Sets the cancellation pointer after the thread has been started.  If the pointer is valid, Stop() can later be called to set the value to which
	//	_plCancelObj points to a specific value, which is assumed to be a value that will tell the thread to stop.
	VOID CThread::SetStopPointer( volatile LONG * _plCancelObj ) {
		if ( m_hThread.Valid() ) {
			m_plCancel = _plCancelObj;
		}
	}

}	// namespace lsw
