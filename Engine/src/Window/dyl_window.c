#include "dyl_window.h"
#include <stdint.h>

#ifdef USING_SDL
#include <SDL3/SDL.h>
#endif



void window_initialize(Dyl_Window* window, const char* window_name, u32 x, u32 y, u32 width, u32 height, unsigned long long window_flags)
{
	ASSERT(window, "Window pointer is NULL");
	window->x = x;
	window->y = y;
	window->width = width;
	window->height = height;
	window->window_flags = window_flags;
	window->is_window_open = true;
	window->window_name = DYL_STR_LIT(window_name);
	#ifdef USING_SDL	
	
		if (SDL_Init(SDL_INIT_VIDEO) == 0) {
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not initialize SDL: %s", SDL_GetError());
			return;
		}
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG | SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
		
		window->window_handle = SDL_CreateWindow((char*)window->window_name.string_data, window->width, window->height, window->window_flags);
		SDL_SetWindowPosition(window->window_handle, window->x, window->y);
		ASSERT(window->window_handle, "Unable to create window: %s", SDL_GetError);
		window->gl_context = SDL_GL_CreateContext(window->window_handle);
		ASSERT(window->gl_context, "Unable to create OpenGl context: %s", SDL_GetError);
		ASSERT(gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress), "Unable to initialize Opengl");
		SDL_GL_SetSwapInterval(1);


	#endif
}

void window_start(Dyl_Window* window)
{
	#ifdef USING_SDL
		SDL_GL_MakeCurrent(window->window_handle, window->gl_context);
	#endif

	//TODO: Please make a more uniformed way of sending GPU commands
	//glViewport(0,0,window->width, window->height);

	glClearColor(1.0,1.0,1.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void window_end(Dyl_Window* window)
{
	
	#ifdef USING_SDL
		SDL_GL_SwapWindow(window->window_handle);
	#endif

}

void window_destroy(Dyl_Window* window)
{
	ASSERT(window, "Unable to destroy window");
	window->x = 0;
	window->y = 0;
	window->width = 0;
	window->height = 0;
	window->window_flags = 0;
	
	#ifdef USING_SDL
	SDL_DestroyWindow(window->window_handle);
	#else
		if(window->window_handle)
		{
			free(window->window_handle);
			window->window_handle = NULL;
		}
	#endif
		
}
