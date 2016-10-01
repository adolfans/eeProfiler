/********************************************************************
 filename:  ProfilerNetwork.h
*********************************************************************/
#ifndef __PROFILERNETWORK_H__
#define __PROFILERNETWORK_H__

#include  <vector>
#include "Profiler.h"

/*
===========================================
class: sProfilePacket
data packet being send/received using tcp socket
===========================================
*/
class sProfilePacket
{
public:
	sProfilePacket( std::stringstream& );
	sProfilePacket( int length );
	~sProfilePacket();
	void FillData( void*, int length );
	int Length() const;
	void* GetData() const;

private:
	std::vector<char> dataBuffer;
};

void SendPacket( const sProfilePacket& packet );

void BroadcastToAllClients( void* pData, int length );

void InitializeSocket();

void UninitializeSocket();

#endif