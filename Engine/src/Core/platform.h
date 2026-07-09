#ifndef PLATFORM_H
#define PLATFORM_H


#include "../utils/dyl_str.h"


#ifdef _WIN32
	#include <windows.h>
	#define OS_TAG "Windows"
	#define WINDOW_EVENT_TAG "Win32API"
	#include <glad/glad_wgl.h>
	typedef LARGE_INTEGER FREQUENCY_TYPE;
	typedef HANDLE Thread;
	typedef HANDLE Mutex;
#elif __linux__
	#define OS_TAG "Linux"
	#define WINDOW_EVENT_TAG "SDL3"
	typedef u64 FREQUENCY_TYPE;
	typedef u64 Thread;
	typedef u64 Mutex;
#else
	#define OS_TAG "Other"
	#define WINDOW_EVENT_TAG "(Other)"
	typedef u64 FREQUENCY_TYPE;
	typedef u64 Thread;
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
	u64 thread_pool_count;
	
	

}Platform;

void platform_set_data(Arena* arena, Platform* platform);
void platform_set_os_performance_frequency(Platform* platform);
void platform_set_thread_pool_count(Platform* platform, u64 count);
FREQUENCY_TYPE platform_get_os_performance_frequency(Platform* platform);


//TODO : //////////////////////////////////////////
//OS_SPECIFIC_UTILITIES:
//Threads []
//File opening []
//Networking []
////////////////////////////////////////


#define MAX_TASKS 100




typedef enum
{
	TASK_NIL = 0,
	TASK_ACTIVE,
	TASK_COMPLETE,

}Task_Type;
typedef struct
{
	void (*work)(void*);
	void* args;
	Task_Type type;
	bool is_completed;
}Dyl_Task;






typedef struct
{
	Thread thread;
	Dyl_Task tasks[100];
	Dyl_Str thread_name;
	bool is_active;
}Dyl_Thread;


Dyl_Thread dyl_thread_create(const char* thread_name);


typedef struct
{
	Arena thread_arena;
	Dyl_Thread threads[8];
	u64 count;
	u64 in_use;
}Dyl_Thread_Pool;


Dyl_Thread_Pool dyl_global_thread_pool;
void dyl_thread_pool_init(u64 thread_count);
void dyl_thread_pool_add(const char* thread_name);

void dyl_thread_pool_task_add(const char* thread_name, void(*func)(void*), void* args);
void dyl_thread_pool_spin();


#endif
