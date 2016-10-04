#include <tchar.h>
#include <windows.h>
#include <winnt.h>
#include <sstream>
#include <string>
#include <map>
#include "Profiler.h"
#include "ProfilerNetwork.h"
#include "CollectedResults.h"

#define TIME_PER_SAMPLE 1000			//Commit one result every 1000 milliseconds

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

static LARGE_INTEGER		sFrequencyRT;
static double				sClocksToMilisecondsMultiplier;
static LARGE_INTEGER		sFrameBeginClocks;
static LARGE_INTEGER		sFrameClocksBuiltUp;
static long long			sClocksPerSample;
static LARGE_INTEGER		sClocksSinceInitialization;

static struct ProfileInitializer
{
	ProfileInitializer();
	~ProfileInitializer(){}
} s_localInitializer;

//-----------------------------------------------------------------------
ProfileInitializer::ProfileInitializer()
{
	QueryPerformanceFrequency( &sFrequencyRT );
	sClocksToMilisecondsMultiplier = 1000.0 / ( double )sFrequencyRT.QuadPart;
	sClocksPerSample = ( long long )( (double)TIME_PER_SAMPLE * (double)sFrequencyRT.QuadPart * 0.001 );
	sFrameClocksBuiltUp.QuadPart = 0;
	QueryPerformanceCounter( &sClocksSinceInitialization );
}

//-----------------------------------------------------------------------
void FrameBegin()
{
	QueryPerformanceCounter( &sFrameBeginClocks );
}

//-----------------------------------------------------------------------
void FrameEnd()
{
	LARGE_INTEGER endClocks;
	QueryPerformanceCounter( &endClocks );
	static unsigned long numFrame = 0;		// Number of frames captured 
	sFrameClocksBuiltUp.QuadPart += endClocks.QuadPart - sFrameBeginClocks.QuadPart;
	++ numFrame;
	if( sFrameClocksBuiltUp.QuadPart > sClocksPerSample )
	{
		sFrameResult data;
		double frameTime = (double)sFrameClocksBuiltUp.QuadPart * sClocksToMilisecondsMultiplier;
		double timeStamp = (double)( endClocks.QuadPart - sClocksSinceInitialization.QuadPart ) *  sClocksToMilisecondsMultiplier;
		data.SetNumFrames( numFrame );
		data.SetLength( frameTime );
		data.SetTimestamp( timeStamp );
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
		SendPacket( packet );
		sFrameClocksBuiltUp.QuadPart = 0;
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
double sProfileEntry::GetTime() const
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
	for( sProfileEntry* itr = firstEntry; itr; )
	{
		sProfileEntry* _current = itr;
		itr = itr->Next();
		delete _current;
	}
}

//-----------------------------------------------------------------------
unsigned long sFrameResult::GetNumFrames() const 
{
	return m_numFrames;
}

//-----------------------------------------------------------------------
double sFrameResult::GetLength() const
{
	return m_length;
}

//-----------------------------------------------------------------------
double sFrameResult::GetTimeStamp() const
{
	return m_timeStamp;
}

//-----------------------------------------------------------------------
sProfileEntry* sFrameResult::GetFirstProfileEntry() const
{
	return firstEntry;
}

//-----------------------------------------------------------------------
void sFrameResult::SetNumFrames( unsigned long numFrames )
{
	m_numFrames = numFrames;
}

//-----------------------------------------------------------------------
void sFrameResult::SetLength( double time )
{
	m_length = time;
}

//-----------------------------------------------------------------------
void sFrameResult::SetTimestamp( double timeStamp )
{
	m_timeStamp = timeStamp;
}

//-----------------------------------------------------------------------
void sFrameResult::AddEntry( const char* name, double time )
{
	if( lastEntry == NULL )
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
	outputStream << "{\"Frame\":" << (unsigned long)GetNumFrames() << ","
		<< "\"Time\":" << (double)GetLength() <<","
		<< "\"TimeStamp\":" << (double)m_timeStamp <<","
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


/*
{
"Frame":-858993460,
"Time":200.017,
"Entries":
[
{
"Name":"test",
"Time":64.1617
}
]
}
*/
}

//-----------------------------------------------------------------------
void sFrameResult::operator<<( std::stringstream& inputStream )
{
	inputStream.seekg(0, std::ios_base::end);
	std::streamoff bufferSize = inputStream.tellg();
	inputStream.seekg(0, std::ios_base::beg);
	std::vector<char> buffer( (unsigned int)bufferSize + 1 );
	for( sProfileEntry* itr = firstEntry; itr; )
	{
		sProfileEntry* _current = itr;
		itr = itr->Next();
		delete _current;
	}
	firstEntry = NULL;
	lastEntry = NULL;
	//Try to deserialize property
	for(;;)
	{
		inputStream.getline( &buffer[0], bufferSize, '\"');
		inputStream.getline( &buffer[0], bufferSize, '\"');
		std::string PropertyName( &buffer[0] );
		if( PropertyName == "Frame" )
		{
			inputStream.getline( &buffer[0], bufferSize, ':' );
			unsigned long _frm = 0;
			inputStream >> _frm;
			this->SetNumFrames( _frm );
		}else if( PropertyName == "Time" )
		{
			inputStream.getline( &buffer[0], bufferSize, ':' );
			double _time = 0;
			inputStream >> _time;
			this->SetLength( _time );
		}else if( PropertyName == "TimeStamp" )
		{
			inputStream.getline( &buffer[0], bufferSize, ':' );
			double _timeStamp = 0;
			inputStream >> _timeStamp;
			this->SetTimestamp( _timeStamp );
		}else if( PropertyName == "Entries" )
		{
			inputStream.getline( &buffer[0], bufferSize, ':' );
			inputStream.getline( &buffer[0], bufferSize, '[');
			for(;;)
			{
				inputStream.getline( &buffer[0], bufferSize, '{' );
				double _entryTime = 0;
				std::string _entryName;
				for(;;)
				{
					inputStream.getline( &buffer[0], bufferSize, '\"' );
					inputStream.getline( &buffer[0], bufferSize, '\"' );
					std::string entityPropery = &buffer[0];
					if( entityPropery == "Name" )
					{
						inputStream.getline( &buffer[0], bufferSize, '\"' );
						inputStream.getline( &buffer[0], bufferSize, '\"' );
						_entryName = &buffer[0];
					}else if( entityPropery == "Time" )
					{
						inputStream.getline( &buffer[0], bufferSize, ':' );
						inputStream >> _entryTime;
					}
					char nextCharacter;
					inputStream >> nextCharacter;
					if( nextCharacter == ',' )
						continue;
					else
					{
						std::streamoff currentPos = inputStream.tellg();
						inputStream.seekg( currentPos - 1, std::ios_base::beg);
						break;
					}
				}
				this->AddEntry( _entryName.c_str(), _entryTime );
				inputStream.getline( &buffer[0], bufferSize, '}' );
				char nextCharacter;
				inputStream >> nextCharacter;
				if( nextCharacter == ',' )
					continue;
				else
				{
					std::streamoff currentPos = inputStream.tellg();
					inputStream.seekg( currentPos - 1, std::ios_base::beg);
					break;
				}
			}
			inputStream.getline( &buffer[0], bufferSize, ']' );
		}else
		{

		}
		char nextCharacter;
		inputStream >> nextCharacter;
		if( nextCharacter == ',' )
			continue;
		else
		{
			std::streamoff currentPos = inputStream.tellg();
			inputStream.seekg( currentPos - 1, std::ios_base::beg);
			break;
		}
	}
	
}