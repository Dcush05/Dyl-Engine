#ifndef PLATFORM_H
#define PLATFORM_H


#include "../utils/dyl_str.h"


#ifdef _WIN32
	#include <windows.h>
	#define OS_TAG "Windows"
	#define WINDOW_EVENT_TAG "Win32API"
	#include <glad/glad_wgl.h>
	typedef LARGE_INTEGER FREQUENCY_TYPE;
#elif __linux__
	#define OS_TAG "Linux"
	#define WINDOW_EVENT_TAG "SDL3"
	typedef u64 FREQUENCY_TYPE;
#else
	#define OS_TAG "Other"
	#define WINDOW_EVENT_TAG "(Other)"
	typedef u64 FREQUENCY_TYPE;
#endif




typedef struct
{
	
	#ifdef _WIN32
		HINSTANCE h_instance;
		HINSTANCE h_prev_instance;
		PSTR lp_cmd_line;
		int cmd_show;
	#endif
	FREQUENCY_TYPE frequency;
	Dyl_Str os_str;
	Dyl_Str window_event_str;
	Dyl_Str graphics_driver_str;
	
	

}Platform;

void platform_set_data(Arena* arena, Platform* platform);
void platform_set_os_performance_frequency(Platform* platform);
FREQUENCY_TYPE platform_get_os_performance_frequency(Platform* platform);


#endif
