#ifndef DYL_DEBUG_H
#define DYL_DEBUG_H
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdarg.h>


#define NO_DEBUG_LOG 0
#define DEBUG_LOG 1
#define LOG_CONFIGURATION NO_DEBUG_LOG


#define DYL_ENGINE_LOG_ERROR 0x1
#define DYL_ENGINE_LOG_INFO 0x2
#define DYL_ENGINE_LOG_WARNING 0x3
#define DYL_ENGINE_LOG_DEBUG 0x4


static clock_t dyl_log_start_time;
#define DYL_ENGINE_INIT_LOG() \
				(dyl_log_start_time = clock())\
	
#define DYL_ENGINE_PRINT_LOG(level,fmt, ...) \
	do{ \
		if(level == DYL_ENGINE_LOG_INFO)  \
		{ \
			fprintf(stderr, "[ENGINE (INFO) LOG %.3fs @%s:%s:%d]: " fmt "\n", \
					(double)(clock() - dyl_log_start_time) / CLOCKS_PER_SEC, \
					__FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
	    }else if(level == DYL_ENGINE_LOG_ERROR){ \
			fprintf(stderr, "[ENGINE (ERROR) LOG %.3fs @%s:%s:%d]: " fmt "\n", \
					(double)(clock() - dyl_log_start_time) / CLOCKS_PER_SEC, \
					__FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
		  }else if(level == DYL_ENGINE_LOG_WARNING){ \
				fprintf(stderr, "[ENGINE (WARNING) LOG %.3fs @%s:%s:%d]: " fmt "\n", \
						(double)(clock() - dyl_log_start_time) / CLOCKS_PER_SEC, \
						__FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
			}else if(level == DYL_ENGINE_LOG_DEBUG){ \
				fprintf(stderr, "[ENGINE (DEBUG) LOG %.3fs @%s:%s:%d]: " fmt "\n", \
						(double)(clock() - dyl_log_start_time) / CLOCKS_PER_SEC, \
						__FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
			}\
	}while(0)
	
#ifdef ENGINE_EXPORTS
	#define DEBUG_LOG_API  __declspec(dllexport)
#else
	#define DEBUG_LOG_API __declspec(dllimport)	
#endif

DEBUG_LOG_API void Dyl_App_Print_Log(const char* format, ...);


#endif
