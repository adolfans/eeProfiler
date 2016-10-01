
#ifndef __PROFILER_H__
#define __PROFILER_H__

struct ProfileHandle;

ProfileHandle* CreateProfileHandle( const char* pName );

void ProfileBegin( ProfileHandle* );
void ProfileEnd( ProfileHandle* );

void FrameBegin();
void FrameEnd();

bool InitializeServer();

#endif
