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
	dyl_event_initalize(engine->event);
	
	
}

ENGINE_API void engine_run(Engine* engine, Frame_Call_Back callback)
{
	printf("Meow");
//	ASSERT(callback, "Please setup a frame call back function");
	while(engine->window->is_window_open)
	{

		while(dyl_event_poll(engine->event))
		{
			if(dyl_event_key_press(engine->event, DYLKEY_A, DYL_KEY_PRESSED))
			{
				printf("A Key has been pressed\n");
			}
			if(dyl_event_key_press(engine->event, DYLKEY_X, DYL_KEY_PRESSED))
			{
				printf("X key has been pressed, leaving application....\n");
				engine_shutdown(engine);
				exit(0);
			}
		}
		window_start(engine->window);
		//	callback(engine);
		window_end(engine->window);
	}
}
ENGINE_API void engine_shutdown(Engine* engine)
{
	window_destroy(engine->window);
	arena_free(&global_arena);
}








