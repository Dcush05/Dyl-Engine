#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _WIN32
	#include <windows.h>
#endif


typedef struct
{
	
	#ifdef _WIN32
		HINSTANCE h_instance;
		HINSTANCE h_prev_instance;
		PSTR lp_cmd_line;
		int cmd_show;
	#endif

}Platform;

#endif
