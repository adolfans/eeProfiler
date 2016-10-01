/********************************************************************
 filename:  ProfilerClient.h
*********************************************************************/
#ifndef __PROFILERCLIENT_H__
#define __PROFILERCLIENT_H__

#include "CollectedResults.h"

// Receive one result
// Note: this function will block calling thread until connection is reset
// or one result being received
bool WaitToReceivePacket( int socket, sFrameResult& result );

#endif