#include "Core/dyl_debug_render.h"
#include "Core/dyl_profiler.h"
#include "Core/entity_manager.h"
#include "Events/dyl_events.h"
#include "Renderer/Dyl_Renderer.h"
#include "Renderer/Shader.h"
#include "SDL3/SDL_video.h"
#include "cglm/types.h"
#include "renderer/camera.h"
#include "renderer_engine_interface.h"
#include "utils/dyl_arena.h"
#include "utils/dyl_base.h"
#include "utils/dyl_str.h"
#include <complex.h>
#include <processthreadsapi.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#define ENGINE_EXPORTS
#include "Dyl_Engine.h"
#include "Core/dyl_debug.h"
#include "Core/dyl_profiler.h"


#define MAX_BUFFER_SIZE 64
#define WIDTH 900
#define HEIGHT 900

Arena global_arena;
Arena entity_arena;


ENGINE_API void engine_initialize(Engine* engine)
{
	#if LOG_CONFIGURATION == DEBUG_LOG
		DYL_ENGINE_INIT_LOG();
		dyl_profiler_init();
		dyl_profiler_add("arena_alloc1");
		dyl_profiler_add("window+renderer setup");
		dyl_profiler_add("alloc1");
		dyl_profiler_add("alloc2");
		dyl_profiler_add("alloc3");
		dyl_profiler_add("alloc4");
		dyl_profiler_add("frame_callback");
		dyl_profiler_add("programclose");

	#endif
	dyl_profiler_start("arena_alloc1");			
	global_arena = arena_alloc(GLOBAL_ARENA_START_SIZE * sizeof(Engine));	
	dyl_profiler_end("arena_alloc1");			
	dyl_profiler_print_func("arena_alloc1");

	dyl_profiler_start("window+renderer setup");			
	engine->window = (Dyl_Window*)arena_push(&global_arena, sizeof(Dyl_Window));
	window_initialize(engine->window, "Engine", 500,250, WIDTH, HEIGHT, SDL_WINDOW_OPENGL, true, &engine->platform);
	
	engine->batch_renderer = arena_push(&global_arena, sizeof(Dyl_Batch_Renderer));

	dyl_profiler_end("window+renderer setup");			
	dyl_profiler_print_func("window+renderer setup");
	dyl_profiler_start("alloc1");
	engine->event = (Dyl_Event*)arena_push(&global_arena, sizeof(Dyl_Event));
	dyl_event_initalize(engine->event);
	dyl_profiler_end("alloc1");
	dyl_profiler_print_func("alloc1");

	dyl_profiler_add("entity_arena alloc + init");
	entity_arena = arena_alloc((sizeof(Entity_Manager) * MAX_ENTITY_COUNT) * 2);
	entity_manager_initialize(&engine->manager, &entity_arena);
	dyl_profiler_end("entity_arena alloc + init");
	dyl_profiler_print_func("entity_arena alloc + init");

	printf("hllo\n");
	Texture_Path path;
	path.path = "spritesheet.png";
	printf("%s", path.path);
	engine->texture = texture_init(path, TEXTURE_2D);
	
	Texture_Path skybox_paths = (Texture_Path){.face_paths[0] = "assets/right.jpg",.face_paths[1] = "assets/left.jpg", 
		.face_paths[2] = "assets/top.jpg",. face_paths[3] = "assets/bottom.jpg" ,.face_paths[4] = "assets/front.jpg", .face_paths[5] = "assets/back.jpg"};
	engine->sky_box_texture = texture_init(skybox_paths, TEXTURE_CUBE_MAP);

	Texture_Path opath;
	opath.path = "Assets/Oshawott2.png";
	engine->billboard = texture_init(opath, TEXTURE_2D);
	

	dyl_profiler_start("alloc2");
	engine->scene_camera = arena_push(&global_arena, sizeof(Camera));
	camera_init(engine->scene_camera, (vec3){0.5,3.5,8.0}, false, engine->window->width, engine->window->height );

	dyl_profiler_end("alloc2");
	dyl_profiler_print_func("alloc2");


	dyl_profiler_start("alloc3");
	*engine->batch_renderer = dyl_batch_renderer_init(&global_arena,true,100);
	dyl_profiler_end("alloc3");

	dyl_profiler_print_func("alloc3");



	dyl_profiler_start("alloc4");
	engine->instanced_renderer = arena_push(&global_arena, sizeof(Dyl_Instanced_Renderer));
	*engine->instanced_renderer = dyl_instanced_setup(&global_arena, 10, true);

	dyl_profiler_end("alloc4");

	dyl_profiler_print_func("alloc4");

	engine->model = model_init("assets/Obj/E-45-Aircraft/Aircraft.obj", "assets/Obj/E-45-Aircraft", &global_arena);
	dyl_instanced_renderer_initialize_mod_and_vbo(engine->instanced_renderer, &engine->model);

	engine->t_model = arena_push(&global_arena, sizeof(Model));
	*engine->t_model = model_init("assets/Obj/Japanese_Maple/Japanese_Maple.obj", "assets/Obj/Japanese_Maple", &global_arena);
	dyl_instanced_renderer_initialize_mod_and_vbo(engine->instanced_renderer, engine->t_model);
	
	mat4 twod_proj;
	glm_ortho(0.0f, engine->window->width, engine->window->height, 0.0F, -1.0f, 1.0f, twod_proj);	
	
	

	engine->font_renderer = arena_push(&global_arena, sizeof(Font_Renderer));
	*engine->font_renderer = font_renderer_init("assets/Fonts/vt323.ttf", 75, &twod_proj);
	
	dyl_debug_text_manager_init(&global_arena);
	
	glm_perspective(glm_rad(45.0f), (float)engine->window->width / (float)engine->window->height, 0.1f, 100.0f, engine->projection);

	memcpy(engine->instanced_renderer->projection, engine->projection, sizeof(mat4));
	engine->wireframe_mode = false;
	#if LOG_CONFIGURATION == DEBUG_LOG
		DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_WARNING,"Completed engine initialization");
	#endif
	
}




ENGINE_API void engine_run(Engine* engine, Entity_Scene_Call_Back entity_scene_callback, Frame_Call_Back frame_callback, Event_Call_Back event_callback)
{
	ASSERT(frame_callback, "Please setup a frame call back function");

	entity_scene_callback(engine);	

	while(engine->window->is_window_open)
	{
		
		dyl_batch_renderer_set_proj(engine->batch_renderer,&engine->projection);
		

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
			}

			if(dyl_event_mouse_movement(engine->event))
			{
				printf("Mouse position is: %d, %d\n", engine->event->mouse_pos.x, engine->event->mouse_pos.y);
			}
			if(event_callback)
				event_callback(engine);

			camera_input(engine->scene_camera, engine->event);
			dyl_event_end(engine->event);

		}
		dyl_batch_renderer_set_view(engine->batch_renderer, &engine->scene_camera->view);

		dyl_batch_renderer_set_camera_pos(engine->batch_renderer, &engine->scene_camera->camera_pos);

		dyl_instanced_renderer_set_view(engine->instanced_renderer, &engine->scene_camera->view);

	
		float dt = 1/144.0f;

		camera_update(engine->scene_camera, dt);
		window_start(engine->window);

		dyl_profiler_start("frame_callback");
		if(1)
		{
			dyl_batch_renderer_set_shader_tag(engine->batch_renderer, SHADER_DYNAMIC);	
			static int frame_count;
			frame_count += 3;
			for(size_t i = 0; i < 5; ++i)
			{
				for(size_t j = 0; j < 5; ++j)
				{
					db_cube_draw(engine->batch_renderer, (vec3){i / 1.0, (j) / 1.0,-0.5}, (vec3){1.0,1.0,1.0}, 0.0f, (vec4){255,0,255,255});

				}
			}

			db_cube_texture_draw(engine->batch_renderer, &engine->texture, (vec4){0,0,32,32}
						   ,(vec3){-0.5,0,1.0}, (vec3){1.0,1.0,1.0}, 0.0f, (vec4){255,255,255,255});
			db_cube_texture_draw(engine->batch_renderer, &engine->texture, (vec4){0,0,32,32}
						   ,(vec3){0.5,0,1.0}, (vec3){1.0,1.0,1.0}, 0.0f, (vec4){255,255,255,255});
			db_cube_texture_draw(engine->batch_renderer, &engine->texture, (vec4){0,0,32,32}
						   ,(vec3){1.5,0,1.0}, (vec3){1.0,1.0,1.0}, 0.0f, (vec4){255,255,255,255});
			db_rectangle_draw(engine->batch_renderer, (vec2){-1.0, 1.0}, (vec2){1.0, 1.0}, 0, (vec4){255,0,0,255});

			db_billboard_draw(engine->batch_renderer, &engine->billboard, (vec4){0,0,32,32}, (vec3){-2.0, 1.0, 1.0}, (vec2){1.0,1.0}, 0.0f, (vec4){255,255,255,255});

			db_plane_draw(engine->batch_renderer, (vec3){-16, 0.0, -16.0}, (vec2){32,32}, 0.0, (vec4){128,128,128,255});

			//	db_light_cube(engine->batch_renderer, (vec3){1.0, 6.0, -6.0}, (vec3){1.0, 1.0, 1.0}, (vec4){255,0,0,255}, (vec4){255,200,200,255}, 0.1f, 0.5f, LIGHTING_SPECULAR, engine->scene_camera->camera_pos);

			GLenum mode = engine->wireframe_mode ? GL_LINE : GL_FILL;
			glPolygonMode(GL_FRONT_AND_BACK, mode);
			db_sky_box_draw(engine->batch_renderer, &engine->sky_box_texture, (vec4){255,255,255,255});
			Dyl_Str vertices_sent_debug_txt = dyl_str_lit_fmt(&global_arena, "Vertices (batch): %d/%d", engine->batch_renderer->object_data.vertices.vertex_count,
												   engine->batch_renderer->object_data.vertices.capacity);

			dyl_debug_text_push(vertices_sent_debug_txt.string_data);
	

			

			db_flush(engine->batch_renderer); 
			
		}
			dyl_instanced_push_model(engine->instanced_renderer, &engine->model,
									   (vec3){0.5, 0.5, 3.5}, (vec3){1.0,1.0,1.0}, 0.0f, (vec4){255,255,255, 255});
			dyl_instanced_push_model(engine->instanced_renderer, engine->t_model,(vec3){4.5, 0, 3.5}, (vec3){1.0,1.0,1.0}, 0.0f, (vec4){255,255,255, 255} );

			//		dyl_instanced_push_model(engine->instanced_renderer, engine->tree_model,(vec3){5.0, 0, 4.5}, (vec3){1.0,1.0,1.0}, 0.0f, (vec4){255,255,255, 255} );
			//
			Dyl_Str instanced_vertices_sent_debug_txt = dyl_str_lit_fmt(&global_arena, "Triangles (instanced): %d", engine->instanced_renderer->triangle_count);

			dyl_debug_text_push(instanced_vertices_sent_debug_txt.string_data);
	


			dyl_instanced_draw(engine->instanced_renderer);

				
//		dyl_instanced_push_rect(engine->instanced_renderer, (vec2){1.0,1.0}, (vec2){64,64}, 0.0);


		//dyl_instanced_draw_rectangle(engine->instanced_renderer);
		
		dyl_debug_text_render(engine->font_renderer);

		


		frame_callback(engine);
		window_end(engine->window);
		dyl_profiler_end("frame_callback");
		dyl_profiler_print_func("frame_callback");


	}
}


//-------RENDERER ENGINE INTERFACE API IMPLEMENTATION-------
ENGINE_RENDERER_API void _draw_shape2D(Dyl_Batch_Renderer* renderer, Shape_Primitive_Type type, Shape_Params params)
{
	//printf("Param data: %f, %f, %f, %f", params.position.x, params.position.y, params.size.x, params.size.y);
	
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












