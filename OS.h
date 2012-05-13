#include <GTK++.h>

#ifdef _WINDOWS
	#include "Win.h"
#else
	#include "Linux.h"
	#include <sys/time.h>

	inline long GetTickCount()
	{
		timeval tv;
		gettimeofday(&tv, NULL);
		return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
	}

	inline unsigned char GetRValue(int Colour)
	{
		return Colour & 0xFF;
	}

	inline unsigned char GetGValue(int Colour)
	{
		return (Colour >> 8) & 0xFF;
	}

	inline unsigned char GetBValue(int Colour)
	{
		return (Colour >> 16) & 0xFF;
	}
#endif

extern GTKGLWindow *hwnd;
