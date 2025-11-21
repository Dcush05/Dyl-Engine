#include "Events/dyl_events.h"
#include "Renderer/Dyl_Renderer.h"
#include "cglm/types.h"
#include "dyl_lib.h"
#include <complex.h>
#define ENGINE_EXPORTS
#include "Dyl_Engine.h"

Arena global_arena;


ENGINE_API void engine_initialize(Engine* engine)
{
	global_arena = arena_alloc(GLOBAL_ARENA_START_SIZE);	
	engine->window = (Dyl_Window*)arena_push(&global_arena, sizeof(Dyl_Window));
	window_initialize(engine->window, "Engine", 500, 500, 900, 900, SDL_WINDOW_OPENGL);
	engine->renderer = (Renderer2D*)arena_push(&global_arena, sizeof(Renderer2D)); //set up
	mat4 projection;
	glm_ortho(0.0f, 900,900, 0.0F, -1.0f, 1.0f, projection);	
	*engine->renderer = renderer_init(&projection);
	engine->event = (Dyl_Event*)arena_push(&global_arena, sizeof(Dyl_Event));
	dyl_event_initalize(engine->event);
	
	
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
		frame_callback(engine);
		window_end(engine->window);
	}
}


//RENDERER ENGINE INTERFACE API IMPLEMENTATION
//
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
ENGINE_API void engine_shutdown(Engine* engine)
{
	window_destroy(engine->window);
	arena_free(&global_arena);
}











