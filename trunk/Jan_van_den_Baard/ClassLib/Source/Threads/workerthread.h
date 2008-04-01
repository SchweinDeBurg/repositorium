#ifndef _WORKERTHREAD_H_
#define _WORKERTHREAD_H
//
// workerthread.h
//
// (C) Copyright 2002 Jan van den Baard.
//     All Rights Reserved.
//
// Based on the article "Using Worker Threads" by Joseph M. Newcomer
// which I found on The Code Project (www.codeproject.com).
//
#include "event.h"
#include "../exceptions/memoryexception.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
#define THIS_FILE __FILE__
#endif

// Thread function definition.
typedef UINT ( WINAPI *LPTHREAD_PROC )( LPVOID );

// A simple worker thread class.
class ClsWorkerThread
{
	_NO_COPY( ClsWorkerThread );
public:
	// Contruction.
	ClsWorkerThread()
	{
		// Clear data handles and setup defaults.
		m_hThread = NULL;
		m_pEvents[ 0 ] = m_pEvents[ 1 ] = NULL;
		m_bRunning = FALSE;
	}

	// Destruction.
	virtual ~ClsWorkerThread()
	{
		// Destroy the thread.
		Destroy();

		// And the events.
		if ( m_pEvents[ 0 ] ) delete m_pEvents[ 0 ];
		if ( m_pEvents[ 1 ] ) delete m_pEvents[ 1 ];
	}

	// Call this function from the main thread after
	// the worker thread has exited.
	void Destroy()
	{
		// Make sure were gone...
		if ( m_hThread )
		{
			// Stop the thread if not done yet.
			m_pEvents[ 0 ]->SetEvent();

			// Wait for the thread to stop.
			::WaitForSingleObject( m_hThread, INFINITE );
			::CloseHandle( m_hThread );
			m_hThread = NULL;

			// Reset kill event.
			m_pEvents[ 0 ]->ResetEvent();
		}
	}

	// Return codes for the Wait() function.
	enum { wtRunning = 1, wtShutdown, wtError };

	// The thread should call this code at regular intervals to 
	// determine what action it should take depending on the return 
	// codes as defined above.
	//
	// wtRunning  - Thread runs, no problems.
	// wtShutdown - The thread has received a shutdown signal. Exit the
	//              thread.
	// wtError    - An error occured during the Wait() function.
	int Wait()
	{
		// Wait for the shutdown and running
		// events.
		switch ( ::WaitForMultipleObjects( 2, m_Handles, FALSE, INFINITE ))
		{
			case	WAIT_OBJECT_0:
				// Shutdown event was set.
				return wtShutdown;

			case	WAIT_OBJECT_0 + 1:
				// Running event was set.
				return wtRunning;
		}
		// Error...
		return wtError;
	}

	// Start the thread.
	BOOL Start( LPTHREAD_PROC lpfnThreadFunc, LPVOID lpParam )
	{
		// Thread already running?
		_ASSERT( m_hThread == NULL );

		// Not running by default.
		m_bRunning = FALSE;

		// Setup events in case this was not done
		// yet.
		if ( CreateEvents())
		{
			// Create the thread. Uses the CreateThread() API when not compiling
			// for multi-thread. Otherwise the _beginthreadex() routine is used.
#ifdef _MT
			m_hThread = ( HANDLE )_beginthreadex( NULL, 0, lpfnThreadFunc, lpParam, 0, ( UINT * )&m_dwThreadID );
#else
			m_hThread = CreateThread( NULL, 0, ( LPTHREAD_START_ROUTINE )lpfnThreadFunc, lpParam, 0, &m_dwThreadID );
#endif 
			if ( m_hThread )
			{
				// If this times-out the thread is up and running. If it returns
				// WAIT_OBJECT_0 the thread has exited before reaching this code.
				switch ( ::WaitForSingleObject( m_hThread, 0 ))
				{
					case	WAIT_TIMEOUT:
						m_bRunning = TRUE;
						return TRUE;

					case	WAIT_OBJECT_0:
						::CloseHandle( m_hThread );
						m_hThread = NULL;
						return FALSE;
				}
			}
		}
		return FALSE;
	}

	// Stop the thread.
	void Stop()
	{
		// Is it running?
		if ( m_bRunning && m_hThread )
		{
			// Signal the shutdown event.
			m_pEvents[ 0 ]->SetEvent();

			// Wait for the thread to die...
			::WaitForSingleObject( m_hThread, INFINITE );

			// Close the handle.
			::CloseHandle( m_hThread );

			// Reset values.
			m_pEvents[ 0 ]->ResetEvent();
			m_hThread = NULL;
			m_bRunning = FALSE;
		}
	}

	// Pause/resume.
	inline void Pause()	{ if ( m_bRunning ) m_pEvents[ 1 ]->ResetEvent(); }
	inline void Run()	{ if ( m_bRunning ) m_pEvents[ 1 ]->SetEvent();   }
	inline BOOL IsRunning() { return m_bRunning; }

protected:
	// Helper.
	BOOL CreateEvents()
	{
		// Create events. The first event is a shutdown event. The
		// second the running event. The thread runs directly after
		// creation.
		try
		{
			if ( m_pEvents[ 0 ] == NULL ) m_pEvents[ 0 ] = new ClsEvent( TRUE, FALSE );
			if ( m_pEvents[ 1 ] == NULL ) m_pEvents[ 1 ] = new ClsEvent( TRUE, TRUE  );
			m_Handles[ 0 ] = *m_pEvents[ 0 ];
			m_Handles[ 1 ] = *m_pEvents[ 1 ];
			return TRUE;
		}
		catch( ClsMemoryException & )
		{
		}
		return FALSE;
	}

	// Data.
	ClsEvent       *m_pEvents[ 2 ];
	HANDLE		m_Handles[ 2 ];
	HANDLE		m_hThread;
	DWORD		m_dwThreadID;
	BOOL		m_bRunning;
};
#endif // _WORKERTHREAD_H_