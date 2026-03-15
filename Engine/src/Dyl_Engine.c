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
#include <complex.h>
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
		dyl_profiler_add("frame_callback");

		dyl_profiler_add("programclose");

	#endif
	dyl_profiler_start("arena_alloc1");			
	global_arena = arena_alloc(GLOBAL_ARENA_START_SIZE * sizeof(Engine));	
	dyl_profiler_end("arena_alloc1");			
	dyl_profiler_print_func("arena_alloc1");

	dyl_profiler_start("window+renderer setup");			
	engine->window = (Dyl_Window*)arena_push(&global_arena, sizeof(Dyl_Window));
	window_initialize(engine->window, "Engine", 500,250, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
//	engine->renderer = (Renderer2D*)arena_push(&global_arena, sizeof(Renderer2D)); //set up

	
	
//	*engine->renderer = renderer_init((float)WIDTH, (float)HEIGHT, false);
	
	engine->batch_renderer = arena_push(&global_arena, sizeof(Dyl_Batch_Renderer));

	dyl_profiler_end("window+renderer setup");			
	dyl_profiler_print_func("window+renderer setup");
	engine->event = (Dyl_Event*)arena_push(&global_arena, sizeof(Dyl_Event));
	dyl_event_initalize(engine->event);
	dyl_profiler_add("entity_arena alloc + init");
	entity_arena = arena_alloc(sizeof(Entity_Manager) * MAX_ENTITY_COUNT);
	dyl_profiler_end("entity_arena alloc + init");
	dyl_profiler_print_func("entity_arena alloc + init");
	entity_manager_initialize(&engine->manager, &entity_arena);
//	exit(EXIT_SUCCESS);
	printf("hllo\n");
	Texture_Path path;
	path.path = "spritesheet.png";
	printf("%s", path.path);
	engine->texture = texture_init(path, TEXTURE_2D);
	
	Texture_Path skybox_paths = (Texture_Path){.face_paths[0] = "assets/right.jpg",.face_paths[1] = "assets/left.jpg", 
		.face_paths[2] = "assets/top.jpg",. face_paths[3] = "assets/bottom.jpg" ,.face_paths[4] = "assets/front.jpg", .face_paths[5] = "assets/back.jpg"};
	engine->sky_box_texture = texture_init(skybox_paths, TEXTURE_CUBE_MAP);
	engine->scene_camera = arena_push(&global_arena, sizeof(Camera));
	camera_init(engine->scene_camera, engine->window->window_handle,(vec3){0.5,0.5,5.0}, true,engine->window->width, engine->window->height );

	*engine->batch_renderer = dyl_batch_renderer_init(&global_arena,true,100);

	engine->instanced_renderer = arena_push(&global_arena, sizeof(Dyl_Instanced_Renderer));
	*engine->instanced_renderer = dyl_instanced_setup(&global_arena, 10, true);
	engine->model = model_init("assets/Obj/E-45-Aircraft/Aircraft.obj", &global_arena);

	dyl_instanced_renderer_initialize_mod_and_vbo(engine->instanced_renderer, &engine->model);

	engine->t_model = arena_push(&global_arena, sizeof(Model));
	*engine->t_model = model_init("assets/Obj/teapot.obj", &global_arena);
	dyl_instanced_renderer_initialize_mod_and_vbo(engine->instanced_renderer, engine->t_model);


	#if LOG_CONFIGURATION == DEBUG_LOG
		DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_WARNING,"Completed engine initialization");
	#endif


//	glm_ortho(0.0f, WIDTH, HEIGHT, 0.0F, -1.0f, 1.0f, engine->projection);	
//	engine->instanced_renderer = arena_push(&global_arena, sizeof(Dyl_Instanced_Renderer));
//	*engine->instanced_renderer = dyl_instanced_setup(5);
	
	glm_perspective(glm_rad(45.0f), (float)engine->window->width / (float)engine->window->height, 0.1f, 100.0f, engine->projection);

	memcpy(engine->instanced_renderer->projection, engine->projection, sizeof(mat4));
	engine->wireframe_mode = false;

	
}




ENGINE_API void engine_run(Engine* engine, Entity_Scene_Call_Back entity_scene_callback, Frame_Call_Back frame_callback, Event_Call_Back event_callback)
{
	ASSERT(frame_callback, "Please setup a frame call back function");

//	entity_scene_callback(engine);	 i dont think we do this every frame....

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

			camera_input(engine->scene_camera, &engine->event->event);

		}
		dyl_batch_renderer_set_view(engine->batch_renderer, &engine->scene_camera->view);

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
			for(size_t i = 0; i < 50; ++i)
			{
				for(size_t j = 0; j < 50; ++j)
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

			GLenum mode = engine->wireframe_mode ? GL_LINE : GL_FILL;
			glPolygonMode(GL_FRONT_AND_BACK, mode);
			db_sky_box_draw(engine->batch_renderer, &engine->sky_box_texture, (vec4){255,255,255,255});

			db_flush(engine->batch_renderer); 
		}
		// 2. FORCE RESET THE STATE
		glBindVertexArray(0);  // This is the most important line
		glUseProgram(0);       // Unset the batch shader
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

			
//		dyl_instanced_push_rect(engine->instanced_renderer, (vec2){1.0,1.0}, (vec2){64,64}, 0.0);


		//dyl_instanced_draw_rectangle(engine->instanced_renderer);
		dyl_instanced_push_model(engine->instanced_renderer, &engine->model,

						   (vec3){0.5, 0, 1.0}, (vec3){1.0,1.0,1.0}, 0.0f, (vec4){255,255,255});
		dyl_instanced_push_model(engine->instanced_renderer, &engine->model,
						   (vec3){1.5, 0, 1.0}, (vec3){1.0,1.0,1.0}, 0.0f, (vec4){255,255,255});
		dyl_instanced_push_model(engine->instanced_renderer, &engine->model,
						   (vec3){2.5, 0, 1.0}, (vec3){1.0,1.0,1.0}, 0.0f, (vec4){255,255,255});
		dyl_instanced_push_model(engine->instanced_renderer, &engine->model,
						   (vec3){3.5, 0, 1.0}, (vec3){1.0,1.0,1.0}, 0.0f, (vec4){255,255,255});

		dyl_instanced_push_model(engine->instanced_renderer, engine->t_model,(vec3){4.5, 0, 1.0}, (vec3){1.0,1.0,1.0}, 0.0f, (vec4){255,255,255} );







		dyl_instanced_draw(engine->instanced_renderer);

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
//	nk_sdl_shutdown();
	dyl_profiler_end("programclose");
	dyl_profiler_print_func("programclose");
	#if LOG_CONFIGURATION == DEBUG_LOG
		DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_INFO, "Program closed successfully");
	#endif

	dyl_profiler_free();
}












