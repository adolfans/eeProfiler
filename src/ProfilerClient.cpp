#include <stdint.h>
#include <vector>
#include "CollectedResults.h"
#include "ProfilerNetwork.h"
#include "ProfilerClient.h"
#include <sstream>
#include <WinSock2.h>

//-----------------------------------------------------------------------
Receiver::Receiver()
	:connected( false )
{

}

//-----------------------------------------------------------------------
Receiver::~Receiver()
{

}

//-----------------------------------------------------------------------
bool Receiver::ConnectTo( const char* address, int port )
{
	if( connected )
		return true;

	socket = ::socket( AF_INET, SOCK_STREAM, 0 );

	bool closeSocketAndReturnFalse = false;

	::sockaddr_in servaddr;
	memset( &servaddr, 0, sizeof(servaddr) );
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = ::inet_addr( address );
	servaddr.sin_port = ::htons( port );

	int bindResult = ::connect( socket, (::sockaddr*)&servaddr, sizeof(servaddr) );
	if( bindResult == SOCKET_ERROR )
	{
		int errorcode = errno;
		printf( "Failed to connect, errorcode: %d", (int)port, errorcode );
		closeSocketAndReturnFalse = true;
	}

	if( closeSocketAndReturnFalse )
	{
		::closesocket( socket );
		return NULL;
	}

	connected = true;

	return true;
}

//-----------------------------------------------------------------------
bool Receiver::WaitToReceivePacket( sFrameResult& result )
{
	//Try to dispatch packet

	uint32_t packetSize = 0;
	int rs = recv( socket, (char*)&packetSize, sizeof(uint32_t), 0 );

	std::vector<char> dataBuffer( packetSize );
	int resultValue = recv( socket, &dataBuffer[0], packetSize, 0 );

	//sProfilePacket packet( packetSize );
	//packet.FillData( (void*)&dataBuffer[0], packetSize );

	std::stringstream dataStream;
	dataStream << std::string( &dataBuffer[0] );

	result << dataStream;

	return true;
}

//-----------------------------------------------------------------------
bool Receiver::CloseConnection()
{
	if( connected )
	{
		shutdown( socket, SD_RECEIVE );
		closesocket( socket );
		connected = false;
	}
	return true;
}