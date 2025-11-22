#include "dyl_profiler.h"
#include <stdio.h>
#include <time.h>


Dyl_Profile_Data global_profiler;
void dyl_profiler_init()
{
	for(size_t i = 0; i < MAX_PROFILED_FUNCTIONS; ++i)
	{
		global_profiler.id[i].type = PROFILE_NIL;
		global_profiler.id[i].function_names = write_string("[Uninitalized String]");
		
		global_profiler.stats[i].total_time = 0.0;
	}
}
void dyl_profiler_add(const char* tag_name)
{
	for(size_t i = 0; i < MAX_PROFILED_FUNCTIONS; ++i)
	{

		if(global_profiler.id[i].type == PROFILE_NIL)
		{
			global_profiler.id[i].function_names = write_string(tag_name);
			global_profiler.id[i].type = PROFILE_INITIALIZE;
			break;
		}
	}
}

void dyl_profiler_start(const char* function_name)
{
	
	for(size_t i = 0; i < MAX_PROFILED_FUNCTIONS; ++i)
	{
		if(global_profiler.id[i].type == PROFILE_NIL) continue;

		if(global_profiler.id[i].type == PROFILE_INITIALIZE && 
			strcmp(function_name, global_profiler.id[i].function_names->string) == 0)
		{
			global_profiler.stats[i].start_time = clock();
			break;
		}
	}
}

void dyl_profiler_end(const char* function_name)
{
	for(size_t i = 0; i < MAX_PROFILED_FUNCTIONS; ++i)
	{
		if(global_profiler.id[i].type == PROFILE_NIL) continue;
		if(strcmp(function_name, global_profiler.id[i].function_names->string) == 0)
		{
				
			global_profiler.stats[i].end_time = clock();
			global_profiler.stats[i].total_time = (double)(global_profiler.stats[i].end_time - global_profiler.stats[i].start_time) / CLOCKS_PER_SEC;
			
			break;

		}
	}

}

void dyl_profiler_print_func(const char* function_name)
{
	for(size_t i = 0; i < MAX_PROFILED_FUNCTIONS; ++i)
	{
		if(global_profiler.id[i].type == PROFILE_NIL) continue;
		if(strcmp(function_name, global_profiler.id[i].function_names->string) == 0)
		{
			printf("[Profiler]: Tag->%s, Elasped time->%f\n", global_profiler.id[i].function_names->string, global_profiler.stats[i].total_time);
			break;
		}
	}
}


void dyl_profiler_free()
{
	for(size_t i = 0; i < MAX_PROFILED_FUNCTIONS; ++i)
	{
		if(global_profiler.id[i].function_names)
			string_free(global_profiler.id[i].function_names);
		global_profiler.id[i].type = PROFILE_NIL;
		global_profiler.stats[i].start_time = 0;
		global_profiler.stats[i].end_time = 0;
		global_profiler.stats[i].total_time = 0;


	}
}
