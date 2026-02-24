#include "Core/dyl_profiler.h"
#include "Core/entity_manager.h"
#include "Events/dyl_events.h"
#include "Renderer/Dyl_Renderer.h"
#include "Renderer/Shader.h"
#include "SDL3/SDL_video.h"
#include "cglm/types.h"
#include "renderer/camera.h"
#include "renderer_engine_interface.h"
#include <complex.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#define ENGINE_EXPORTS
#include "Dyl_Engine.h"
#include "Core/dyl_debug.h"
#include "Core/dyl_profiler.h"


#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_INCLUDE_COMMAND_USERDATA
#define NK_SDL3_OPENGL_IMPLEMENTATION

#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024

#include "nuklear.h"
#include "nuklear_sdl3_opengl_renderer.h"


static struct nk_context* nuk_ctx;
static struct nk_colorf nuk_color;
static struct nk_font_atlas* atlas;

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
		dyl_profiler_add("frame_callback");

		dyl_profiler_add("programclose");

	#endif
	dyl_profiler_start("arena_alloc1");			
	global_arena = arena_alloc(GLOBAL_ARENA_START_SIZE);	
	dyl_profiler_end("arena_alloc1");			
	dyl_profiler_print_func("arena_alloc1");

	dyl_profiler_start("window+renderer setup");			
	engine->window = (Dyl_Window*)arena_push(&global_arena, sizeof(Dyl_Window));
	window_initialize(engine->window, "Engine", 500,250, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
//	engine->renderer = (Renderer2D*)arena_push(&global_arena, sizeof(Renderer2D)); //set up

	
	
//	*engine->renderer = renderer_init((float)WIDTH, (float)HEIGHT, false);
	
	engine->batch_renderer = arena_push(&global_arena, sizeof(Dyl_Batch_Renderer));
	*engine->batch_renderer = dyl_batch_renderer_init(true,100);

	dyl_profiler_end("window+renderer setup");			
	dyl_profiler_print_func("window+renderer setup");
	engine->event = (Dyl_Event*)arena_push(&global_arena, sizeof(Dyl_Event));
	dyl_event_initalize(engine->event);
	entity_arena = arena_alloc(sizeof(Entity_Manager) * MAX_ENTITY_COUNT * MAX_ENTITY_COUNT);
	entity_manager_initialize(&engine->manager, &entity_arena);
	Texture_Path path;
	path.path = "spritesheet.png";
	printf("%s", path.path);
	engine->texture = texture_init(path, TEXTURE_2D);
	
	Texture_Path skybox_paths = (Texture_Path){.face_paths[0] = "assets/right.jpg",.face_paths[1] = "assets/left.jpg", 
		.face_paths[2] = "assets/top.jpg",. face_paths[3] = "assets/bottom.jpg" ,.face_paths[4] = "assets/front.jpg", .face_paths[5] = "assets/back.jpg"};
	engine->sky_box_texture = texture_init(skybox_paths, TEXTURE_CUBE_MAP);
	engine->scene_camera = arena_push(&global_arena, sizeof(Camera));
	camera_init(engine->scene_camera, engine->window->window_handle,(vec3){0.5,0.5,5.0}, true,engine->window->width, engine->window->height );

	#if LOG_CONFIGURATION == DEBUG_LOG
		DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_WARNING,"Completed engine initialization");
	#endif

	#ifdef USING_SDL
	nuk_ctx = nk_sdl_init(engine->window->window_handle);
	#endif



	#ifdef INCLUDE_CONFIGURATOR

    static struct nk_color color_table[NK_COLOR_COUNT];
    memcpy(color_table, nk_default_color_style, sizeof(color_table));
    #endif
	{
		nk_sdl_font_stash_begin(&atlas);
		nk_sdl_font_stash_end();


	}
	//glm_ortho(0.0f, WIDTH, HEIGHT, 0.0F, -1.0f, 1.0f, engine->projection);	
	
	glm_perspective(glm_rad(45.0f), (float)engine->window->width / (float)engine->window->height, 0.1f, 100.0f, engine->projection);

	engine->wireframe_mode = false;

	
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
			}
			if(dyl_event_key_press(engine->event, DYLKEY_E, DYL_KEY_PRESSED))
			{
				printf("A Key has been pressed\n");
				engine->wireframe_mode = !engine->wireframe_mode;
			}else if(dyl_event_key_press(engine->event, DYLKEY_X, DYL_KEY_PRESSED))
			{
				engine_shutdown(engine);
			}
			if(event_callback)
				event_callback(engine);
			nk_sdl_handle_event(nuk_ctx, &engine->event->event);
			nk_input_end(nuk_ctx);

			camera_input(engine->scene_camera, &engine->event->event);

		}
		dyl_batch_renderer_set_view(engine->batch_renderer, &engine->scene_camera->view);

	/*	if(nk_begin(nuk_ctx, "Hello Engine", nk_rect(0,0,250,450), NK_WINDOW_BORDER|NK_WINDOW_CLOSABLE|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
		{
			//nk_layout_row_dynamic(nuk_ctx, 20, 1);
				nk_layout_row_dynamic(nuk_ctx, 20, 1);
				static int i = 0;
				if(nk_button_label(nuk_ctx, "Create basic entity"))
				{
					printf("meow %d\n", i);
					entity_shape_create(&engine->manager, (Shape_Params){(vec2f){100 + i, 100 + i}, (vec2f){2 * i, 2 * i}, (Color){255,255,255,255}});
					i++;
				//	inventory_add(&game->data, item_search(&game->data, "Blue Gill Fish")); //testing
				}

				char buffer[MAX_BUFFER_SIZE];
				snprintf(buffer, MAX_BUFFER_SIZE, "Entity Count: %d, Max: %d", engine->manager.entity_count , MAX_ENTITY_COUNT);
				nk_layout_row_dynamic(nuk_ctx, 20, 1);
				nk_label(nuk_ctx, buffer, NK_LEFT);
		}
		nk_end(nuk_ctx);*/
		
		float dt = 1/144.0f;

		camera_update(engine->scene_camera, dt);
		window_start(engine->window);

		//draw_rectangle(engine->renderer, (vec2){100,100}, (vec2){32,32}, 0.0, (vec4){255,0,255,255});
		dyl_profiler_start("frame_callback");

		//NOTE: MY REGLAR RENDERER DOESNT WORK FOR SOME REASON FIX IT WHEN WE COME BACK if(engine->manager.entity_count < MAX_ENTITY_COUNT)
//		camera_update(engine->scene_camera, 0.01667);
		//FIX:bruv it was because of incorrect comparison of the strings in the shader uniform caching code

	//	printf("%f, %f, %f", engine->scene_camera->camera_pos[0], engine->scene_camera->camera_pos[1],engine->scene_camera->camera_pos[2]);
	//	renderer_set_view(engine->renderer, &engine->scene_camera->view);	

		//frame_callback(engine);
	//	db_rectangle_draw(engine->batch_renderer, (vec2){100,132},
				
	//				(vec2){32,32}, 1.0, (vec4){1.0, 1.0, 0.5, 0.5});
	//	db_rectangle_draw(engine->batch_renderer, (vec2){100,164},
					
				//	(vec2){32,32}, 1.0, (vec4){1.0, 1.0, 0.5, 0.5});
	//	db_rectangle_draw(engine->batch_renderer, (vec2){132,100},
	//				(vec2){32,32}, 0.0, (vec4){255,100,255,255});
	//	db_rectangle_draw(engine->batch_renderer, (vec2){132, 164},
				//	(vec2){32,32}, 1.0, (vec4){1.0, 1.0, 0.5, 0.5});

		dyl_batch_renderer_set_shader_tag(engine->batch_renderer, SHADER_DYNAMIC);	
		//db_cube_draw(engine->batch_renderer, (vec3){0,0,-0.5}, (vec3){1.0,1.0,1.0}, 0.0f, (vec4){255,0,255,255});
		static int frame_count;
		frame_count += 3;
		for(size_t i = 0; i < 50; ++i)
		{
			for(size_t j = 0; j < 50; ++j)
			{
//				db_rectangle_draw(engine->batch_renderer, (vec2){i * 32, j * 32},
//				(vec2){64, 64}, 0.5 * frame_count, (vec4){255,255,0,255});
			//	draw_rectangle(engine->renderer, (vec2){i * 32, j * 32},
			//	(vec2){64, 64}, 0.5 * frame_count, (vec4){255,255,0,255});
			

				db_cube_draw(engine->batch_renderer, (vec3){i / 1.0, (j) / 1.0,-0.5}, (vec3){1.0,1.0,1.0}, 0.0f, (vec4){255,0,255,255});
		//		db_texture_draw(engine->batch_renderer, &engine->texture, (vec4){0,0,32,32}, (vec2){i * 32, j * 32}, (vec2){32,32}, 0.0, (vec4){255,255,255,255} );

			}
		}

		//draw_texture(engine->renderer, &engine->texture, (vec4){0,0,32,32}, (vec2){100,100}, (vec2){32,32}, 0.0, (vec4){255,255,255,255});
		
		//printf("%d %d\n", engine->texture.width, engine->texture.height);
	    // db_texture_draw(engine->batch_renderer, &engine->texture, (vec4){0,0,32,32}, (vec2){100,100}, (vec2){32,32}, 0.0, (vec4){255,255,255,255} );
		//dyl_batch_renderer_set_shader_tag(engine->batch_renderer, SHADER_SPRITE);


		//fprintf(stdout, "Vertices sent: %zu/%zu\n", engine->batch_renderer->vertex_data.vertex_count, engine->batch_renderer->vertex_data.capacity);

		db_cube_texture_draw(engine->batch_renderer, &engine->texture, (vec4){0,0,32,32}
					   ,(vec3){-0.5,0,1.0}, (vec3){1.0,1.0,1.0}, 0.0f, (vec4){255,255,255,255});
		db_cube_texture_draw(engine->batch_renderer, &engine->texture, (vec4){0,0,32,32}
					   ,(vec3){0.5,0,1.0}, (vec3){1.0,1.0,1.0}, 0.0f, (vec4){255,255,255,255});
		db_cube_texture_draw(engine->batch_renderer, &engine->texture, (vec4){0,0,32,32}
					   ,(vec3){1.5,0,1.0}, (vec3){1.0,1.0,1.0}, 0.0f, (vec4){255,255,255,255});

		GLenum mode = engine->wireframe_mode ? GL_LINE : GL_FILL;
		glPolygonMode(GL_FRONT_AND_BACK, mode);





		db_sky_box_draw(engine->batch_renderer, &engine->sky_box_texture, (vec4){255,255,255,255});
		db_flush(engine->batch_renderer); //separate flush for our sky box cubemap
		
		
		

			//	entity_manager_render(engine->renderer, &engine->manager);
	//	nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);
	  //  frame_callback(engine);
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
	db_destroy(engine->batch_renderer);

	arena_free(&global_arena);
	arena_free(&entity_arena);
//	nk_sdl_shutdown();
	dyl_profiler_end("programclose");
	dyl_profiler_print_func("programclose");
	#if LOG_CONFIGURATION == DEBUG_LOG
		DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_INFO, "Program closed successfully");
	#endif

	dyl_profiler_free();
}












