/********************************************************************
 filename:  ProfilerClient.h
*********************************************************************/
#ifndef __PROFILERCLIENT_H__
#define __PROFILERCLIENT_H__

#include "CollectedResults.h"

struct Receiver
{
public:
	Receiver();
	~Receiver();

	bool ConnectTo( const char* address, int port );
	// Receive one result
	// Note: this function will block calling thread until connection is reset
	// or one result being received
	bool WaitToReceivePacket( sFrameResult& result );
	bool CloseConnection();
private:
	int socket;
	bool connected;
};

#endif