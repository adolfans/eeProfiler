#include <WinSock.h>
#include <stdio.h>
#include <set>
#include "Mutex.h"
#include "ProfilerNetwork.h"

#define DEFAULTPORT0 55553

static std::set<int>	clientSet;
static Mutex*			s_clientsMutex;

void AddClientFd( int clientFd )
{
	s_clientsMutex->Lock();
	clientSet.insert( clientFd );
	s_clientsMutex->Unlock();
}

void SendToAllClients( void* data, int length )
{
	s_clientsMutex->Lock();
	for( auto itr = clientSet.begin();
		itr != clientSet.end();	)
	{
		int clientFd = (*itr);
		int result = send( clientFd, (char*)data, length, 0 );
		if( result == SOCKET_ERROR )
		{
			closesocket( clientFd );
			itr = clientSet.erase( itr );
		}else
		{
			++ itr; 
		}
	}
	s_clientsMutex->Unlock();
}

static int listenFd;
static DWORD listeningThreadId;
static DWORD broadcastThreadId;
static HANDLE listeningThreadHandle;
static HANDLE broadcastThreadHandle;

DWORD WINAPI ListeningThread( LPVOID p );
//DWORD WINAPI BroadcastThread( LPVOID p );

bool InitializeServer( /*const char* ip, const char* port */ )
{
	listenFd = socket( AF_INET, SOCK_STREAM, 0 );
	
	bool closeSocketAndReturnFalse = false;

	sockaddr_in servaddr;
	memset( &servaddr, 0, sizeof(servaddr) );
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl( INADDR_ANY );
	unsigned short defaultPort = DEFAULTPORT0;
	servaddr.sin_port = htons( defaultPort );

	int bindResult = bind( listenFd, (sockaddr*)&servaddr, sizeof(servaddr) );
	while( bindResult == SOCKET_ERROR )
	{
		int errorcode = errno;
		printf( "Failed to bind on port: %d, errorcode: %d", (int)defaultPort, errorcode );
		defaultPort++;
		servaddr.sin_port = htons( defaultPort );
		bindResult = bind( listenFd, (sockaddr*)&servaddr, sizeof(servaddr) );
	}

	if( !bindResult )
	{
		printf( "Profile server bind on port: %d", (int)defaultPort );
	}

	if( listen( listenFd, SOMAXCONN ) == SOCKET_ERROR  )
	{
		printf( "Listen failed with error: %ld\n", WSAGetLastError() );
		closeSocketAndReturnFalse = true;
	}

	if( closeSocketAndReturnFalse )
	{
		closesocket( listenFd );
		return false;
	}

	//Initialize Async threads
	listeningThreadHandle = CreateThread( NULL, 0, ListeningThread, NULL, STACK_SIZE_PARAM_IS_A_RESERVATION, (DWORD*)&listeningThreadId );
	//broadcastThreadHandle = CreateThread( NULL, 0, BroadcastThread, NULL, STACK_SIZE_PARAM_IS_A_RESERVATION, (DWORD*)&broadcastThreadId );

	s_clientsMutex = new Mutex;

	return true;
}

void BroadcastToAllClients( void* pData, int length )
{
	SendToAllClients( pData, length );
	return;
}

DWORD WINAPI ListeningThread( LPVOID p )
{
	sockaddr_in clientAddr;
	int sizeCli = sizeof( sockaddr_in );
	while( true )
	{
		int clientFd = accept( listenFd, (sockaddr*)&clientAddr, &sizeCli );
		if( clientFd == INVALID_SOCKET )
		{
			return 0;
		}else
		{
			AddClientFd( clientFd );
		}
	}
	return 0;
}

void InitializeSocket()
{
	//windows only
	WSADATA _wsaData;
	if ( WSAStartup( MAKEWORD( 2, 2 ), &_wsaData ) != 0) {
		printf( "Failed to start network %ld\n", WSAGetLastError() );
	}
}

void UninitializeSocket()
{
	WSACleanup();
}