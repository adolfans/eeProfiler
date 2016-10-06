/********************************************************************
 filename:  CollectedResults.h
*********************************************************************/

#ifndef __COLLECTEDRESULTS_H__
#define __COLLECTEDRESULTS_H__

#include <string>
#include <sstream>

/*
===========================================
struct sProfileEntry
entry of task being collected in sFrameResult
===========================================
*/
struct sProfileEntry
{
	friend struct sFrameResult;
	sProfileEntry();
	~sProfileEntry();
	sProfileEntry* Next() const;
	const char* GetName() const;
	double GetTime() const;
	long GetId() const;

private:
	std::string name;
	double	time;
	sProfileEntry* next;
	long  id;
};

/*
===========================================
class sFrameResult
Frame Result received from target program
===========================================
*/
struct sFrameResult
{
public:
	sFrameResult();
	~sFrameResult();

	//Getters
	unsigned long GetNumFrames() const;
	double GetLength() const;
	double GetTimeStamp() const;

	sProfileEntry* GetFirstProfileEntry() const;

	//Setters
	void SetNumFrames( unsigned long );
	void SetLength( double time );
	void SetTimestamp( double timeStamp );
	void AddEntry( const char*, double time, long id );

	void operator >> ( std::stringstream& outputStream ) const;
	void operator << ( std::stringstream& inputStream );

private:
	sFrameResult( const sFrameResult& );
	unsigned long m_numFrames;
	double m_length;
	double m_timeStamp;
	sProfileEntry* firstEntry;
	sProfileEntry* lastEntry;
};

#endif