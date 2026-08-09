#ifndef DYL_ENGINE_H
#define DYL_ENGINE_H
#include "Core/dyl_debug_render.h"
#include "utils/dyl_base.h"
#include "Core/entity_manager.h"
#include "Renderer/Dyl_Renderer.h"
#include "Renderer/Dyl_Renderer.h"
#include "renderer/camera.h"
#include "Window/dyl_window.h"
#include "Events/dyl_events.h"
#include "Core/entity_manager.h"
#include "Core/platform.h"
#include "Assets/Asset_Manager.h"
#include "Core/Scene_Manager.h"
#include "Editor/editor_ui.h"
 

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


#define GLOBAL_ARENA_START_SIZE (1024 * 1024)
extern Arena global_arena;
extern Arena entity_arena;




typedef struct Engine
{
	Entity_Manager manager;
	Entity_Manager manager2;
	//Model model;
	mat4 projection;

	mat4 twod_proj;
	Platform platform;
	Editor editor;
//	Model* t_model;

	//Model* tree_model;
	Dyl_Window* window;
	Dyl_Batch_Renderer* batch_renderer;
	Dyl_Instanced_Renderer* instanced_renderer;
	Dyl_Instanced_Renderer* test_instanced_renderer;
	Font_Renderer* font_renderer;
	Dyl_Event* event;
	Dyl_Debug_Text_Manager* debug_text;
	Camera* scene_camera;
//	Texture texture;
	//Texture billboard;
	//Texture sky_box_texture;
	u64 frequency;
	f32 fps;
	f32 delta_time;
	f32 gpu_time;
	bool wireframe_mode;


	
}Engine;



//CALLBACK FUNCTIONS
//
typedef void(*Entity_Scene_Call_Back)(Engine*);
typedef void(*Frame_Call_Back)(Engine*);
typedef void(*Event_Call_Back)(Engine*);


ENGINE_API int engine_entry(int argc, char** argv);
ENGINE_API void engine_initialize(Engine* engine);
ENGINE_API void engine_run(Engine* engine, Entity_Scene_Call_Back entity_scene_callback, Frame_Call_Back frame_callback, Event_Call_Back event_callback);
ENGINE_API void engine_shutdown(Engine* engine);


#endif
