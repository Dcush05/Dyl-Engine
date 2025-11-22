#include "Core/dyl_profiler.h"
#include "Events/dyl_events.h"
#include "Renderer/Dyl_Renderer.h"
#include "cglm/types.h"
#include "dyl_lib.h"
#include <complex.h>
#include <stdarg.h>
#include <stdio.h>
#define ENGINE_EXPORTS
#include "Dyl_Engine.h"
#include "Core/dyl_debug.h"
#include "Core/dyl_profiler.h"

Arena global_arena;


ENGINE_API void engine_initialize(Engine* engine)
{
	#if LOG_CONFIGURATION == DEBUG_LOG
		DYL_ENGINE_INIT_LOG();
		dyl_profiler_init();
		dyl_profiler_add("arena_alloc1");
		dyl_profiler_add("window+renderer setup");
		dyl_profiler_add("frame_callback");

		dyl_profiler_add("programclose");

		dyl_profiler_start("arena_alloc1");			
		global_arena = arena_alloc(GLOBAL_ARENA_START_SIZE);	
		dyl_profiler_end("arena_alloc1");			
		dyl_profiler_print_func("arena_alloc1");
		
	#endif

	dyl_profiler_start("window+renderer setup");			
	engine->window = (Dyl_Window*)arena_push(&global_arena, sizeof(Dyl_Window));
	window_initialize(engine->window, "Engine", 500, 500, 900, 900, SDL_WINDOW_OPENGL);
	engine->renderer = (Renderer2D*)arena_push(&global_arena, sizeof(Renderer2D)); //set up
	
	mat4 projection;
	glm_ortho(0.0f, 900,900, 0.0F, -1.0f, 1.0f, projection);	
	*engine->renderer = renderer_init(&projection);
	dyl_profiler_end("window+renderer setup");			
	dyl_profiler_print_func("window+renderer setup");
	engine->event = (Dyl_Event*)arena_push(&global_arena, sizeof(Dyl_Event));
	dyl_event_initalize(engine->event);

	#if LOG_CONFIGURATION == DEBUG_LOG
		DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_WARNING,"Completed engine initialization");
	#endif
	
	
}



ENGINE_API void engine_run(Engine* engine, Frame_Call_Back frame_callback, Event_Call_Back event_callback)
{
	printf("Meow");
	ASSERT(frame_callback, "Please setup a frame call back function");


	while(engine->window->is_window_open)
	{

		while(dyl_event_poll(engine->event))
		{

			if(dyl_event_window_dispatch(engine->event, DYL_SYS_QUIT))
			{
				#ifdef USING_SDL
					SDL_Quit();
				#endif
				engine->window->is_window_open = false;
			}
			if(dyl_event_key_press(engine->event, DYLKEY_A, DYL_KEY_PRESSED))
			{
				printf("A Key has been pressed\n");
			}
			if(event_callback)
				event_callback(engine);
		}
		window_start(engine->window);
	//	dyl_profiler_start("frame_callback");
		frame_callback(engine);
	//	dyl_profiler_end("frame_callback");
	//	dyl_profiler_print_func("frame_callback");
		window_end(engine->window);
	}
}


//-------RENDERER ENGINE INTERFACE API IMPLEMENTATION-------
ENGINE_RENDERER_API void _draw_shape2D(Renderer2D* renderer, Shape_Primitive_Type type, Shape_Params params)
{
	//printf("Param data: %f, %f, %f, %f", params.position.x, params.position.y, params.size.x, params.size.y);
	switch(type)
	{
		case SHAPE_NIL:

		break;
		case SHAPE_RECT:
			draw_rectangle(renderer, (vec2){params.position.x, params.position.y}, (vec2){params.position.x, params.position.y}, params.rotation, 
				  (vec4){params.color.r, params.color.g, params.color.b, params.color.a});
		break;
		default:
			fprintf(stderr, "You didn't properly input a shape\n");
		break;
			
	}
}

//END 



//-------APPLICATION DEBUG API IMPLEMENTATION-------
DEBUG_LOG_API void Dyl_App_Print_Log(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	char print_buffer[526];
	snprintf(print_buffer, strlen(print_buffer), "[APPLICATION LOG]: %s", format);
	vprintf(print_buffer, args);
	va_end(args);
}
//END



ENGINE_API void engine_shutdown(Engine* engine)
{
	dyl_profiler_start("programclose");
	window_destroy(engine->window);
	arena_free(&global_arena);
	dyl_profiler_end("programclose");
	dyl_profiler_print_func("programclose");
	#if LOG_CONFIGURATION == DEBUG_LOG
		DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_INFO, "Program closed successfully");
	#endif
	dyl_profiler_free();
}












