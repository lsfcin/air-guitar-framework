#ifndef __Win32Timer_H__
#define __Win32Timer_H__

#define NOMINMAX 
#include <windows.h>

#define FREQUENCY_RESAMPLE_RATE 200

class win32timer
{
private:
    DWORD mStartTick;
	LONGLONG mLastTime;
    LARGE_INTEGER mStartTime;
    LARGE_INTEGER mFrequency;

    DWORD mProcMask;
    DWORD mSysMask;

    HANDLE mThread;

    DWORD mQueryCount;
public:    
    void reset();
    unsigned long getMilliseconds();
    unsigned long getMicroseconds();
	unsigned long getNanoseconds();
};

#endif