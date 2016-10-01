// ProfilerServer.cpp : Defines the entry point for the console application.
//
#include "tchar.h"
#include "ProfilerNetwork.h"
#include "Profiler.h"
#include <time.h>
#include <windows.h>
#define  TEST_TIME_LENGTH	(1000*30)		//30 seconds
int _tmain(int argc, _TCHAR* argv[])
{
	//Fake profile information to test monitor
	InitializeSocket();
	srand( ( unsigned int )time(NULL) );

	int sleptTimeBuiltUp = 0;
	for( ; ; )
	{
		FrameBegin();
		printf( "FrameBegin\n" );
		//sleep for 5~8 milliseconds
		int _s1 = rand() % 3 + 5;
		Sleep( (DWORD)( _s1 ) );
		static ProfileHandle* _some = CreateProfileHandle( "test" );
		ProfileBegin( _some );
		printf( "ProfileBegin(\"test\")\n" );
		//sleep for 5~8 milliseconds
		int _s2 = rand() % 3 + 5;
		Sleep( (DWORD)( _s2 ) );
		ProfileEnd( _some );
		printf( "ProfileEnd\n" );
		//sleep for 5~9 milliseconds
		int _s3 = rand() % 3 + 6;
		Sleep( (DWORD)( _s3 ) );
		FrameEnd();
		printf( "FrameEnd\n" );
		sleptTimeBuiltUp += _s1 + _s2 + _s3;
		if( sleptTimeBuiltUp > TEST_TIME_LENGTH )
		{
			break;
		}
	}

	UninitializeSocket();
	return 0;
}

