#ifndef DYL_PROFILER_H
#define DYL_PROFILER_H
#include <time.h>
#include "../utils/dyl_base.h"

#define MAX_PROFILED_FUNCTIONS 50

typedef enum
{
	PROFILE_NIL = 0,
	PROFILE_INITIALIZE,
}Profile_Type;

typedef struct
{
	Dyl_Str function_name;
	Profile_Type type;
}Dyl_Profile_Id;


typedef struct
{
	clock_t start_time;
	clock_t end_time;
	double total_time;

}Dyl_Profile_Stats;

typedef struct
{
	Dyl_Profile_Id id[MAX_PROFILED_FUNCTIONS];
	Dyl_Profile_Stats stats[MAX_PROFILED_FUNCTIONS];
}Dyl_Profile_Data;

extern Dyl_Profile_Data global_profiler;

void dyl_profiler_init();
void dyl_profiler_add(const char* tag_name);
void dyl_profiler_start(const char* function_name);
void dyl_profiler_end(const char* function_name);
void dyl_profiler_print_func(const char* function_name);
void dyl_profiler_print_all_func();
void dyl_profiler_print_to_file_all_func();
void dyl_profiler_free();

#endif
