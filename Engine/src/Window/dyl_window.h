#ifndef DYL_WINDOW_H
#define DYL_WINDOW_H
#include "../Core/platform.h"
#include <stdint.h>
#include <stdbool.h>
#include "../utils/dyl_base.h"
#include <glad/glad.h>
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <glad/glad_wgl.h>
	#pragma comment(lib, "user32.lib")
	#pragma comment(lib, "gdi32.lib")
	typedef HANDLE Window_Handle;
	typedef HGLRC GL_Context_Handle;
	typedef HDC Device_Context;	
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#elif USING_SDL
	#include <SDL3/SDL.h>
	#include <SDL3/SDL_video.h>
	typedef SDL_Window* Window_Handle;
	typedef SDL_GLContext GL_Context_Handle;
	typedef void* Device_Context;
#endif





typedef struct
{

	unsigned long long window_flags;
	Dyl_Str window_name;
	Window_Handle window_handle;
	GL_Context_Handle gl_context;
	Device_Context device_context;
	u32 x, y, width, height;
	bool is_window_open;
	bool enable_vsync;
}Dyl_Window;

void window_initialize(Dyl_Window* window, const char* window_name,uint32_t x, u32 y, u32 width, u32 height, unsigned long long window_flags, bool enable_vsync, Platform* platform);
void window_set_vsync(Dyl_Window* window, bool enable_vsync);
void window_start(Dyl_Window* window);
void window_end(Dyl_Window* window);
void window_destroy(Dyl_Window* window);


#endif
