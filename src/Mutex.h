/********************************************************************
 filename:  Mutex.h
*********************************************************************/
#ifndef __MUTEX_H__
#define __MUTEX_H__

/*
===========================================
class Mutex
This is a lock.
===========================================
*/
#ifdef WIN32
#include <windows.h>
class Mutex
{
public:
	inline Mutex();
	inline~Mutex();

	inline void Lock();
	inline void Unlock();

private:
	HANDLE win32MutexHandle;
};


inline Mutex::Mutex()
{
	win32MutexHandle = CreateMutex( NULL, FALSE, NULL );
}

inline Mutex::~Mutex()
{
	CloseHandle( win32MutexHandle );
}

inline void Mutex::Lock()
{
	DWORD rsult = WaitForSingleObject( win32MutexHandle, INFINITE );
	if( rsult != WAIT_OBJECT_0 )
	{
		printf( "Failed on WaitForSingleObject" );
	}
}

inline void Mutex::Unlock()
{
	ReleaseMutex( win32MutexHandle );
}

#endif
#endif