#include <tchar.h>
#include <windows.h>
#include <winnt.h>
#include <sstream>
#include <string>
#include <map>
#include "Profiler.h"
#include "ProfilerNetwork.h"
#include "CollectedResults.h"

#define TIME_PER_SAMPLE 200			//Commit one result every 200 milliseconds

struct ProfileHandle
{
	LARGE_INTEGER startClocks;
	LARGE_INTEGER clocksBuiltUp;
	bool		bEnded;
};

static std::map<ProfileHandle*, std::string> handleToName;

ProfileHandle* CreateProfileHandle( const char* pName )
{
	ProfileHandle* handle = new ProfileHandle();
	handleToName[handle] = pName;
	return handle;
}

void ProfileBegin( ProfileHandle* handle )
{
	QueryPerformanceCounter( &handle->startClocks );
	handle->bEnded = false;
}

void ProfileEnd( ProfileHandle* handle )
{
	LARGE_INTEGER endClocks;
	QueryPerformanceCounter( &endClocks );
	handle->clocksBuiltUp.QuadPart += endClocks.QuadPart - handle->startClocks.QuadPart;
	handle->startClocks.QuadPart = endClocks.QuadPart;
	handle->bEnded = true;
}

#if 0
/*
===========================================
class SerializedProfileData
usage: serialize profileData to Json-like streams per capture
===========================================
*/
class SerializedProfileData
{
public:
	void BeginFrame( long long index, double time );
	void AddEntry( const char*, double time );
	void EndFrame();
private:
	std::string serializedData;
};

//-----------------------------------------------------------------------
void SerializedProfileData::BeginFrame( long long index, double time )
{
	char buffer[256];
	sprintf_s( &buffer[0], 256, "{\"Frame\":%d,\"Time\":%f,\"Entries\":[", (int)index, (float)time );
	serializedData += buffer;
}

//-----------------------------------------------------------------------
void SerializedProfileData::AddEntry( const char* fileName, double time )
{
	char buffer[256];
	sprintf_s( &buffer[0], 256, "{\"Name\":\"%s\",\"Time\":%f},", fileName, (float)time );
	serializedData += buffer;
}

//-----------------------------------------------------------------------
void SerializedProfileData::EndFrame()
{
	serializedData += "]}";
}

#endif

static LARGE_INTEGER		sFrequencyRT;
static double				sClocksToMilisecondsMultiplier;
static LARGE_INTEGER		sFrameBeginClocks;
static LARGE_INTEGER		sFrameClocksBuiltUp;
static long long			sClocksPerSample;

static struct ProfileInitializer
{
	ProfileInitializer();
	~ProfileInitializer(){}
} s_localInitializer;

ProfileInitializer::ProfileInitializer()
{
	QueryPerformanceFrequency( &sFrequencyRT );
	sClocksToMilisecondsMultiplier = 1.0f / ( double )sFrequencyRT.QuadPart;
	sClocksPerSample = TIME_PER_SAMPLE * sFrequencyRT.QuadPart;
}

void FrameBegin()
{
	QueryPerformanceCounter( &sFrameBeginClocks );
}


void FrameEnd()
{
	LARGE_INTEGER endClocks;
	QueryPerformanceCounter( &endClocks );
	static long long numFrame = 0;		// Number of frames captured 
	sFrameClocksBuiltUp.QuadPart += endClocks.QuadPart - sFrameBeginClocks.QuadPart;
	++ numFrame;
	if( sFrameClocksBuiltUp.QuadPart > sClocksPerSample )
	{
		sFrameResult data;
		double frameTime = (double)sFrameClocksBuiltUp.QuadPart * sClocksToMilisecondsMultiplier;
		//data.BeginFrame( numFrame, frameTime );
		data.SetNumFrames( numFrame );
		data.SetLength( frameTime );
		numFrame = 0;
		//Calculate summary
		for( auto itr = handleToName.begin();
			itr != handleToName.end();
			++ itr )
		{
			ProfileHandle* handle = itr->first;
			if( !handle->bEnded )
			{
				handle->clocksBuiltUp.QuadPart += endClocks.QuadPart - handle->startClocks.QuadPart;
				handle->startClocks.QuadPart = endClocks.QuadPart;
			}
			double timeSum = (double)itr->first->clocksBuiltUp.QuadPart * sClocksToMilisecondsMultiplier;
			data.AddEntry( itr->second.c_str(), timeSum );
		}
		std::stringstream strm;
		data >> strm;
		sProfilePacket packet( strm );
	}
}


//-----------------------------------------------------------------------
sProfileEntry::sProfileEntry()
	:next( NULL )
{

}

//-----------------------------------------------------------------------
sProfileEntry::~sProfileEntry()
{
}

//-----------------------------------------------------------------------
sProfileEntry* sProfileEntry::Next() const
{
	return next;
}

//-----------------------------------------------------------------------
const char* sProfileEntry::GetName() const
{
	return name.c_str();
}

//-----------------------------------------------------------------------
float sProfileEntry::GetTime() const
{
	return time;
}

//-----------------------------------------------------------------------
sFrameResult::sFrameResult()
	: firstEntry( NULL ),
	lastEntry( NULL )
{
}

//-----------------------------------------------------------------------
sFrameResult::~sFrameResult()
{
	for( sProfileEntry* itr = firstEntry; itr != lastEntry; )
	{
		sProfileEntry* _current = itr;
		itr = itr->Next();
		delete _current;
	}
}

//-----------------------------------------------------------------------
int sFrameResult::GetNumFrames() const 
{
	return numFrames;
}

//-----------------------------------------------------------------------
float sFrameResult::GetLength() const
{
	return m_length;
}

//-----------------------------------------------------------------------
sProfileEntry* sFrameResult::GetFirstProfileEntry() const
{
	return firstEntry;
}

//-----------------------------------------------------------------------
void sFrameResult::SetNumFrames( int numFrames )
{
	numFrames = numFrames;
}

//-----------------------------------------------------------------------
void sFrameResult::SetLength( float time )
{
	m_length = time;
}

//-----------------------------------------------------------------------
void sFrameResult::AddEntry( const char* name, float time )
{
	if( firstEntry == lastEntry == NULL )
	{
		firstEntry = lastEntry = new sProfileEntry;
		firstEntry->name = name;
		firstEntry->time = time;
	}else
	{
		lastEntry->next = new sProfileEntry;
		lastEntry->next->name = name;
		lastEntry->next->time = time;
	}
}

//-----------------------------------------------------------------------
void sFrameResult::operator >>( std::stringstream& outputStream ) const
{
	outputStream << "{\"Frame\":" << (int)GetNumFrames() << ","
		<< "\"Time\":" << (float)GetLength() <<","
		<< "\"Entries\":[";

	for( sProfileEntry* it = GetFirstProfileEntry(); it; it = it->Next() )
	{
		outputStream << "{";
		outputStream << "\"Name\":"<< "\""<<it->GetName()<<"\","
			<< "\"Time\":"<<it->GetTime();
		outputStream << "}";
		if( it->Next() )
		{
			outputStream << ",";
		}
	}

	outputStream << "]}";
}

//-----------------------------------------------------------------------
void sFrameResult::operator<<( std::stringstream& inputStream )
{
	//inputStream >> 
}