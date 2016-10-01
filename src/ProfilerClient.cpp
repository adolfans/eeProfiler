#include <stdint.h>
#include <vector>
#include "CollectedResults.h"
#include "ProfilerNetwork.h"
#include "ProfilerClient.h"
#include <sstream>
#include <WinSock.h>

bool WaitToReceivePacket( int socket, sFrameResult& result )
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