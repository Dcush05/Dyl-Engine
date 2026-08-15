#include "Assets/Asset_Manager.h"
#include "Core/Ui_Manager.h"
#include "Core/dyl_debug_render.h"
#include "Core/dyl_profiler.h"
#include "Core/entity_manager.h"
#include "Core/platform.h"
#include "Editor/editor_ui.h"
#include "Events/dyl_events.h"
#include "Renderer/Dyl_Renderer.h"
#include "Renderer/Shader.h"
#include "SDL3/SDL_video.h"
#include "cglm/types.h"
#include "core/dyl_debug.h"
#include "renderer/camera.h"
#include "renderer_engine_interface.h"
#include "utils/dyl_arena.h"
#include "utils/dyl_base.h"
#include "utils/dyl_str.h"
#include "utils/dyl_types.h"
#include <complex.h>
#include <processthreadsapi.h>
#include <profileapi.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <winnt.h>
#define ENGINE_EXPORTS
#include "Dyl_Engine.h"
#include "Core/dyl_debug.h"
#include "Core/dyl_profiler.h"

#define MAX_BUFFER_SIZE 64
#define WIDTH 900
#define HEIGHT 900

Arena global_arena;
Arena entity_arena;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*NOTE:(Dylan): Implentations of the two functions(Initialization, and Run) that are running the engine. For right now I do not plan of decoupling the run function into 
separate event, update, and render functions.*/
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ENGINE_API void engine_initialize(Engine* engine)
{
	#if LOG_CONFIGURATION == DEBUG_LOG
		DYL_ENGINE_INIT_LOG();
		dyl_profiler_init();
//		dyl_profiler_add("arena_alloc1");
		dyl_profiler_add("window+renderer setup");
		dyl_profiler_add("event_initialization");
		dyl_profiler_add("model_texture_setup");
		dyl_profiler_add("scene_initialization");
		dyl_profiler_add("frame_callback");
		dyl_profiler_add("programclose");


	#endif
	
	dyl_thread_pool_init(8);


	global_arena = arena_alloc(GLOBAL_ARENA_START_SIZE * sizeof(Engine));	
	

	dyl_profiler_start("window+renderer setup");			
	engine->window = (Dyl_Window*)arena_push(&global_arena, sizeof(Dyl_Window));
	window_initialize(engine->window, "Engine", 500,250, WIDTH, HEIGHT, SDL_WINDOW_OPENGL, true, &engine->platform);
	engine->batch_renderer = arena_push(&global_arena, sizeof(Dyl_Batch_Renderer));
	*engine->batch_renderer = dyl_batch_renderer_init(&global_arena,true,100);
	engine->scene_camera = arena_push(&global_arena, sizeof(Camera));
	camera_init(engine->scene_camera, (vec3){0.5,3.5,8.0}, CAMERA_STATIC ,false, engine->window->client_width, engine->window->client_height);
	engine->instanced_renderer = arena_push(&global_arena, sizeof(Dyl_Instanced_Renderer));
	*engine->instanced_renderer = dyl_instanced_setup(&global_arena, 100, true);


	dyl_profiler_end("window+renderer setup");			
	dyl_profiler_print_func("window+renderer setup");


	dyl_profiler_start("event_initialization");
	engine->event = (Dyl_Event*)arena_push(&global_arena, sizeof(Dyl_Event));
	dyl_event_initalize(engine->event);
	dyl_profiler_end("event_initialization");
	dyl_profiler_print_func("event_initialization");
	
	dyl_profiler_add("entity_arena alloc + init");
	entity_manager_initialize(&engine->manager, engine->batch_renderer, engine->instanced_renderer);
	dyl_profiler_end("entity_arena alloc + init");
	dyl_profiler_print_func("entity_arena alloc + init");

	glm_ortho(0.0f, engine->window->client_width, engine->window->client_height, 0.0f, -1.0f, 1.0f, engine->twod_proj);	
	
	

	engine->font_renderer = arena_push(&global_arena, sizeof(Font_Renderer));
	*engine->font_renderer = font_renderer_init("assets/Fonts/vt323.ttf", 75, &engine->twod_proj);
	
	dyl_debug_text_manager_init(&global_arena);
	
	glm_perspective(glm_rad(45.0f), (float)engine->window->client_width / (float)engine->window->client_height, 0.1f, 100.0f, engine->projection);
	memcpy(engine->instanced_renderer->projection, engine->projection, sizeof(mat4));
	engine->wireframe_mode = false;


	platform_set_data(&global_arena, &engine->platform);
	platform_set_os_performance_frequency(&engine->platform);
	global_asset_manager_init();


	global_ui_element_initialize(engine->batch_renderer, engine->event, engine->window->client_width, engine->window->client_height);
	dyl_batch_renderer_set_proj2d(engine->batch_renderer, &engine->twod_proj);
	dyl_batch_renderer_set_proj3d(engine->batch_renderer, &engine->projection);


	Asset* texture = global_asset_manager_get_from_name("player_idle");
	Asset* test = global_asset_manager_get_from_name("player_idle");
	
	engine->editor = editor_create(&engine->manager);

	#if LOG_CONFIGURATION == DEBUG_LOG
		DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_WARNING,"Completed engine initialization");
	#endif
	engine->fps = 0.0f;
	engine->delta_time = 0.0f;
	engine->gpu_time = 0.0f;



	
}




ENGINE_API void engine_run(Engine* engine, Entity_Scene_Call_Back entity_scene_callback, Frame_Call_Back frame_callback, Event_Call_Back event_callback)
{
	ASSERT(frame_callback, "Please setup a frame call back function");

	dyl_profiler_add("scene_initialization");

	global_scene_manager_initialization(engine->instanced_renderer, engine->batch_renderer);
	entity_scene_callback(engine);	

	dyl_profiler_end("scene_initialization");

	bool test = false;

	dyl_profiler_print_func("scene_initialization");

	#ifdef _WIN32
		LARGE_INTEGER start;
		LARGE_INTEGER end;
		QueryPerformanceCounter(&start);
	#endif

	int current_scene = 0;
	int frame_count = 0;
	float fps_timer = 0.0f;

	while(engine->window->is_window_open)
	{
		while(dyl_event_poll(engine->event))
		{
			if(dyl_event_window_dispatch(engine->event, DYL_SYS_QUIT))
			{
				engine->window->is_window_open = false;
				engine_shutdown(engine);
			}
			if(dyl_event_key_handle(engine->event, DYLKEY_E, DYL_KEY_PRESSED))
			{
				printf("A Key has been pressed\n");
				engine->wireframe_mode = !engine->wireframe_mode;
			}else if(dyl_event_key_handle(engine->event, DYLKEY_X, DYL_KEY_PRESSED))
			{
				engine_shutdown(engine);
			}else if(dyl_event_key_handle(engine->event, DYLKEY_H, DYL_KEY_PRESSED))
			{
				engine->window->enable_vsync = !engine->window->enable_vsync;
				window_set_vsync(engine->window, engine->window->enable_vsync);	
			}else if(dyl_event_key_handle(engine->event, DYLKEY_TAB, DYL_KEY_PRESSED)) //HOTFIX: SHADER HOT RELOADING
			{
				shader_programs_all_create(&engine->batch_renderer->shaders);
				shader_programs_all_create(&engine->instanced_renderer->shaders);
			}
			if(dyl_event_mouse_movement(engine->event))
			{
				printf("Mouse position is: %d, %d\n", engine->event->mouse_pos.x, engine->event->mouse_pos.y);
			}
			if(event_callback)
				event_callback(engine);

			dyl_debug_entity_select(&global_arena, &engine->manager, engine->event);
			dyl_event_end(engine->event);

		}

		window_start(engine->window);

		dyl_profiler_start("frame_callback");
		GLenum mode = engine->wireframe_mode ? GL_LINE : GL_FILL;
		glPolygonMode(GL_FRONT_AND_BACK, mode);

		dyl_debug_text_push(engine->platform.os_str.string_data);
		dyl_debug_text_push(engine->platform.window_event_str.string_data);
		dyl_debug_text_push(engine->platform.graphics_driver_str.string_data);

		Dyl_Str current_fps_debug_txt = dyl_str_lit_fmt(&global_arena, "FPS: %f", engine->fps);
		dyl_debug_text_push(current_fps_debug_txt.string_data);
		Dyl_Str current_dt_debug_txt = dyl_str_lit_fmt(&global_arena, "Elasped Time: %f", engine->delta_time);
		dyl_debug_text_push(current_dt_debug_txt.string_data);

		Dyl_Str vertices_sent_debug_txt = dyl_str_lit_fmt(&global_arena, "Vertices (batch): %d/%d", engine->batch_renderer->object_data.vertices.vertex_count,
											   engine->batch_renderer->object_data.vertices.capacity);
		dyl_debug_text_push(vertices_sent_debug_txt.string_data);
		Dyl_Str instanced_vertices_sent_debug_txt = dyl_str_lit_fmt(&global_arena, "Triangles (instanced): %d", engine->instanced_renderer->triangle_count);
		dyl_debug_text_push(instanced_vertices_sent_debug_txt.string_data);
		engine->gpu_time = (float)engine->instanced_renderer->time_elasped / 1000000000;


		Dyl_Str gpu_time_txt = dyl_str_lit_fmt(&global_arena, "GPU Time(instanced): %f", engine->gpu_time);
		dyl_debug_text_push(gpu_time_txt.string_data);


				
			
			
		//editor_events(&engine->editor);	
		frame_callback(engine);
		editor_construct(&engine->editor);
		dyl_debug_text_render(engine->font_renderer);
		dyl_debug_entity_render(engine->font_renderer);
		db_flush(engine->batch_renderer);

		#ifdef _WIN32
			QueryPerformanceCounter(&end);
			engine->delta_time = ((float)(end.QuadPart - start.QuadPart) / engine->platform.frequency.QuadPart);
			start = end;
			frame_count++;
			fps_timer += engine->delta_time;
			if(fps_timer >= 1.0)
			{
				engine->fps = frame_count;
				frame_count = 0;
				fps_timer -= 1.0f;
			}
		#endif

		window_end(engine->window);

		dyl_profiler_end("frame_callback");
		dyl_profiler_print_func("frame_callback");


	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*NOTE:(Dylan): Basic draw api functions to just rendering shapes on the screen without adding tot he entity manager, as of right now it is dead code*/
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


ENGINE_RENDERER_API void _draw_shape2D(Dyl_Batch_Renderer* renderer, Shape_Primitive_Type type, Shape_Params params)
{
	
	switch(type)
	{
		case SHAPE_NIL:

		break;
		case SHAPE_RECT:
			db_rectangle_draw(renderer, (vec2){params.position2f.x, params.position2f.y}, (vec2){params.position2f.x, params.position2f.y}, params.rotation, 
				  (vec4){params.color.r, params.color.g, params.color.b, params.color.a});
		break;
		default:
			fprintf(stderr, "You didn't properly input a shape\n");
		break;
			
	}
}

ENGINE_RENDERER_API void _draw_shape3D(Dyl_Batch_Renderer* renderer, Shape_Primitive_Type type, Shape_Params params)
{
	//printf("Param data: %f, %f, %f, %f", params.position.x, params.position.y, params.size.x, params.size.y);
	switch(type)
	{
		case SHAPE_NIL:

		break;
		case SHAPE_CUBE:
			db_cube_draw(renderer, (vec3){params.position3f.x, params.position3f.y, params.position3f.z}, 
			 (vec3){params.size3f.x, params.size3f.y, params.size3f.z}, params.rotation, 
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
//	db_destroy(engine->batch_renderer);
	arena_free(&global_arena);
	arena_free(&entity_arena);
	dyl_profiler_end("programclose");
	dyl_profiler_print_func("programclose");
	#if LOG_CONFIGURATION == DEBUG_LOG
		DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_INFO, "Program closed successfully");
	#endif

	dyl_profiler_free();
	#ifdef USING_SDL
		SDL_Quit();
	#else
		ExitProcess(0);
	#endif
}












