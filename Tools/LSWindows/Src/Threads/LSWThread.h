#pragma once

#include "../LSWWin.h"
#include "../Helpers/LSWHelpers.h"

// A sample thread-function declaration just for reference.
// typedef DWORD (WINAPI *PTHREAD_START_ROUTINE)( LPVOID lpThreadParameter );

namespace lsw {

	class CThread {
	public :
		CThread();
		~CThread();


		// == Functions.
		// Creates a thread.  The previous thread's handle is released but the thread, if any, is not stopped before
		//	this new thread is created.  If there is another thread created by this object, it should be stopped manually
		//	before using the same object to create another thread.
		// If _plCancelObj is supplied, it is assumed to point to data on the target thread which the target thread checks as a sign to exit.  Stop() is only useful
		//	if this is provided.
		BOOL								CreateThread( LPTHREAD_START_ROUTINE _lpStartAddress, LPVOID _lpParameter, volatile LONG * _plCancelObj = nullptr, LPSECURITY_ATTRIBUTES _lpsaAttribs = NULL, SIZE_T _stStackSize = 0, DWORD _dwCreationFlags = 0 );
		BOOL								CreateThread( LPTHREAD_START_ROUTINE _lpStartAddress, DWORD _dwParameter, volatile LONG * _plCancelObj = nullptr, LPSECURITY_ATTRIBUTES _lpsaAttribs = NULL, SIZE_T _stStackSize = 0, DWORD _dwCreationFlags = 0 ) {
			return CreateThread( _lpStartAddress, reinterpret_cast<LPVOID>(static_cast<UINT_PTR>(_dwParameter)), _plCancelObj, _lpsaAttribs, _stStackSize, _dwCreationFlags );
		}
		BOOL								CreateThread( LPTHREAD_START_ROUTINE _lpStartAddress, int32_t _i32Parameter, volatile LONG * _plCancelObj = nullptr, LPSECURITY_ATTRIBUTES _lpsaAttribs = NULL, SIZE_T _stStackSize = 0, DWORD _dwCreationFlags = 0 ) {
			return CreateThread( _lpStartAddress, reinterpret_cast<LPVOID>(static_cast<UINT_PTR>(_i32Parameter)), _plCancelObj, _lpsaAttribs, _stStackSize, _dwCreationFlags );
		}

		// Stops the current thread, if _plCancelObj was supplied in CreateThread().
		// It performs an InterlockedExchange() call and returns the previous value.
		LONG								Stop( LONG _lNewStopVal );

		// Waits on the current thread for the given number of milliseconds.
		DWORD								WaitFor( DWORD _dwMilliseconds = INFINITE );

		// Stops the current thread and waits for the return.
		DWORD								StopAndWait( LONG _lNewStopVal, DWORD _dwMilliseconds = INFINITE );

		// Fully resets the thread without stopping it.  Closes its internal handle, sets the ID to 0, and NULL-ifies the atom pointer.  The thread can't be
		//	stopped after these are reset, so StopAndWait() should be called first.
		VOID								Reset();

		// Exits and kills the thread.  This should only be used when all resources have been cleaned up and no more useful information can be gleaned from the thread.
		VOID								ExitThread( DWORD _dxExitCode );

		// Sets the cancellation pointer after the thread has been started.  If the pointer is valid, Stop() can later be called to set the value to which
		//	_plCancelObj points to a specific value, which is assumed to be a value that will tell the thread to stop.
		VOID								SetStopPointer( volatile LONG * _plCancelObj );

		// Gets the thread ID.
		DWORD								Id() const { return m_dwId; }



	protected :
		// == Members.
		// Handle to the thread.
		LSW_HANDLE							m_hThread;

		// Thread ID.
		DWORD								m_dwId;

		// Optional pointer to an atom that can be used to Stop() the running thread.
		// If m_plCancel is not nullptr, Stop() can be used to set a LONG on the target thread that
		//	is assumed to be used to check for stopping the thread to a given value.
		volatile LONG *						m_plCancel;
	};

}	// namespace lsw
