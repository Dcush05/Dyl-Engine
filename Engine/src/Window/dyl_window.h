#ifndef DYL_WINDOW_H
#define DYL_WINDOW_H
#include <stdint.h>
#include <stdbool.h>
#include "../dyl_lib.h"
#include <glad/glad.h>
#ifdef USING_SDL
	#include <SDL3/SDL.h>
	#include <SDL3/SDL_video.h>
	typedef SDL_Window* Window_Handle;
	typedef SDL_GLContext GL_Context_Handle;
#else
typedef void* Window_Handle;
typedef void* GL_Context_Handle;
#endif

typedef struct
{
	Window_Handle window_handle;
	GL_Context_Handle gl_context;
	unsigned long long window_flags;
	dyl_string* window_name;
	uint32_t x, y, width, height;
	bool is_window_open;
}Dyl_Window;

void window_initialize(Dyl_Window* window, const char* window_name,uint32_t x, uint32_t y, uint32_t width, uint32_t height, unsigned long long window_flags);
void window_start(Dyl_Window* window);
void window_end(Dyl_Window* window);
void window_destroy(Dyl_Window* window);


#endif
