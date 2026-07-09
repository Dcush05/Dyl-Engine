#include "platform.h"
#include <processthreadsapi.h>
#include <profileapi.h>
#include <stddef.h>
#include <synchapi.h>
#include <winbase.h>
#include <winnt.h>


void platform_set_data(Arena* arena, Platform* platform)
{
	platform->os_str = dyl_str_lit_fmt(arena, "Operating System: %s\n", OS_TAG);
	platform->window_event_str = dyl_str_lit_fmt(arena, "Window and Event handling: %s\n", WINDOW_EVENT_TAG);
	platform->graphics_driver_str = dyl_str_lit_fmt(arena, "Graphics Driver: %s\n", glGetString(GL_VERSION));
}

void platform_set_os_performance_frequency(Platform* platform)
{
	#ifdef _WIN32
		QueryPerformanceFrequency(&platform->frequency);
	#else
		printf("Not supported by other platforms");
	#endif
}
FREQUENCY_TYPE platform_get_os_performance_frequency(Platform* platform)
{
	return platform->frequency;
}

volatile s64 global_task_count = 0;
Mutex global_mutex;



DWORD WINAPI dyl_thread_task_base(void* t)
{
	Dyl_Task* tasks = (Dyl_Task*)t;
	while(true)
	{
		for(size_t i = 0; i < MAX_TASKS; ++i)
		{
			if(tasks[i].type == TASK_ACTIVE)
			{
				tasks[i].work(tasks[i].args);
				tasks[i].type = TASK_COMPLETE;
				InterlockedDecrement64(&global_task_count);
			}
			
		}
		if(global_task_count == 0)
			break;
	}
	return TRUE;
}


//Dyl_Thread dyl_thread_create(void (*func)(void*), void* args)


void dyl_task_add(Dyl_Task tasks[], Dyl_Task task)
{
	for(size_t i = 0; i < MAX_TASKS; ++i)
	{
		if(tasks[i].type == TASK_NIL || tasks[i].type == TASK_COMPLETE)
		{
			tasks[i] = task;
			InterlockedIncrement64(&global_task_count);
			break;
		}

	}
}

Dyl_Thread dyl_thread_create(const char* thread_name)
{
	Dyl_Thread new_thread = (Dyl_Thread){0};
	new_thread.thread_name = DYL_STR_LIT(thread_name);

	#ifdef _WIN32
		printf("Creatiing windows thread");
	
	#else
		printf("Not supported by other platforms");
	#endif

	return new_thread;
}


void dyl_thread_add_task(Dyl_Thread* dthread, void(*func)(void*), void* args)
{
	Dyl_Task task;
	task.work = func;
	task.args = args;
	task.type = TASK_ACTIVE;
	task.is_completed = false;

	/*dthread.thread = CreateThread(NULL, 
								  0, 
								  dyl_thread_task_base,
								  &task, 0, 0);*/
	dyl_task_add(dthread->tasks, task);
	
}


void dyl_thread_spin(Dyl_Thread* dthread)
{

	dthread->thread = CreateThread(NULL, 
								  0, 
								  dyl_thread_task_base,
								  dthread->tasks, 0, 0);
}

extern Dyl_Thread_Pool dyl_global_thread_pool;

void dyl_thread_pool_init(u64 thread_count)
{
	dyl_global_thread_pool.thread_arena = arena_alloc(sizeof(Dyl_Thread) * thread_count);
//	dyl_global_thread_pool.threads = arena_push(&dyl_global_thread_pool.thread_arena, thread_count);
	dyl_global_thread_pool.count = thread_count;
	global_mutex = CreateMutex(NULL, FALSE, NULL);

}


void dyl_thread_pool_add(const char* thread_name)
{
	for(size_t idx = 0; idx < dyl_global_thread_pool.count; ++idx)
	{
		if(!dyl_global_thread_pool.threads[idx].is_active)
		{
			dyl_global_thread_pool.threads[idx] = dyl_thread_create(thread_name);
			dyl_global_thread_pool.in_use++;
			break;
		}
	}
}


void dyl_thread_pool_task_add(const char* thread_name, void(*func)(void*), void* args)
{
	for(size_t idx = 0; idx < dyl_global_thread_pool.count; ++idx)
	{
		if(strcmp((char*)dyl_global_thread_pool.threads->thread_name.string_data, thread_name) == 0)
		{
			dyl_thread_add_task(&dyl_global_thread_pool.threads[idx], func, args);
			dyl_global_thread_pool.threads[idx].is_active = true;
			//dyl_global_thread_pool.in_use++;
			break;
		}
	}
}

void dyl_thread_pool_sync()
{
	while(global_task_count > 0)
	{
		_mm_pause();
	}

}

void dyl_thread_pool_spin()
{
	for(size_t idx = 0; idx < dyl_global_thread_pool.count; ++idx)
	{
		if(dyl_global_thread_pool.threads[idx].is_active == true)
		{
			dyl_thread_spin(&dyl_global_thread_pool.threads[idx]);	
			dyl_global_thread_pool.in_use--;
		}
		
	}

	dyl_thread_pool_sync();
}

