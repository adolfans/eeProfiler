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
	float GetTime() const;

private:
	std::string name;
	float	time;
	sProfileEntry* next;
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
	int GetNumFrames() const;
	float GetLength() const;
	sProfileEntry* GetFirstProfileEntry() const;

	//Setters
	void SetNumFrames( int );
	void SetLength( float time );
	void AddEntry( const char*, float time );

	void operator >> ( std::stringstream& outputStream ) const;
	void operator << ( std::stringstream& inputStream );

private:
	int numFrames;
	float m_length;
	sProfileEntry* firstEntry;
	sProfileEntry* lastEntry;
};

#endif