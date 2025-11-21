#ifndef DYL_ENGINE_H
#define DYL_ENGINE_H
#include "dyl_lib.h"
#include "renderer/Dyl_Renderer.h"
#include "Window/dyl_window.h"
#include "Events/dyl_events.h"
#include "renderer_engine_interface.h"

 

#ifdef USING_SDL
	#define SDL_MAIN_HANDLED
	#include <SDL3/SDL.h>
	#include <SDL3/SDL_main.h>
	#include <SDL3/SDL_video.h>

#endif
 


#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif


#define GLOBAL_ARENA_START_SIZE (1280 * 1280)
extern Arena global_arena;


typedef struct Engine
{
	Dyl_Window* window;
	Renderer2D* renderer;
	Dyl_Event* event;
	
}Engine;



//CALLBACK FUNCTIONS
typedef void(*Frame_Call_Back)(Engine*);
typedef void(*Event_Call_Back)(Engine*);



ENGINE_API void engine_initialize(Engine* engine);
ENGINE_API void engine_run(Engine* engine, Frame_Call_Back frame_callback, Event_Call_Back event_callback);
ENGINE_API void engine_shutdown(Engine* engine);


#endif
