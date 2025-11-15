#include "Events/dyl_events.h"
#include "dyl_lib.h"
#define ENGINE_EXPORTS
#include "Dyl_Engine.h"

Arena global_arena;


ENGINE_API void engine_initialize(Engine* engine)
{
	global_arena = arena_alloc(GLOBAL_ARENA_START_SIZE);	
	engine->window = (Dyl_Window*)arena_push(&global_arena, sizeof(Dyl_Window));
	window_initialize(engine->window, "Engine", 500, 500, 900,900, SDL_WINDOW_OPENGL);
	engine->renderer = (Renderer2D*)arena_push(&global_arena, sizeof(Renderer2D)); //set up
	engine->event = (Dyl_Event*)arena_push(&global_arena, sizeof(Dyl_Event));
	
	
}

ENGINE_API void engine_run(Engine* engine, Frame_Call_Back callback)
{
	printf("Meow");
//	ASSERT(callback, "Please setup a frame call back function");
	while(engine->window->is_window_open)
	{

		//WHEN SETTING UP OUR EVENTS SYSTEM USE SDL_WAITEVENT NOT POLLEVENT
		while(dyl_event_poll(engine->event))
		{

		}
		window_start(engine->window);
		//	callback(engine);
		window_end(engine->window);
	}
}
ENGINE_API void engine_shutdown(Engine* engine)
{
	printf("No imp\n");
	window_destroy(engine->window);
	arena_free(&global_arena);
}








