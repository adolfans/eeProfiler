#include "ProfilerNetwork.h"
#include "stdint.h"
#include <sstream>

sProfilePacket::sProfilePacket( std::stringstream& dataStream )
{
	std::string _tmpStr;
	dataStream >> _tmpStr;

	dataBuffer.resize( sizeof(uint32_t) + _tmpStr.length() + 1 );
	*&dataBuffer[0] = (uint32_t)_tmpStr.length() + 1;

	strcpy( &dataBuffer[sizeof(uint32_t)], _tmpStr.c_str() );
}

sProfilePacket::sProfilePacket( int length )
{
	dataBuffer.resize(  sizeof(uint32_t) + length );
	*&dataBuffer[0] = (uint32_t)length;
}

sProfilePacket::~sProfilePacket()
{

}

void sProfilePacket::FillData( void* pData, int length )
{
	memcpy( &dataBuffer[sizeof(uint32_t)], pData, length );
}

//-----------------------------------------------------------------------
int sProfilePacket::Length() const
{
	return dataBuffer.size();
}

void* sProfilePacket::GetData() const
{
	return (void*)&dataBuffer[0];
}

void SendPacket( const sProfilePacket& packet )
{
	BroadcastToAllClients( packet.GetData(), packet.Length() );
}