#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H
#ifdef ENGINE_EXPORTS
	#define ENGINE_SCENE_API __declspec(dllexport)
#else
	#define ENGINE_SCENE_API __declspec(dllimport)
#endif




#include "../utils/dyl_str.h"
#include "entity_manager.h"
#include "../renderer/camera.h"

#define INITIAL_SCENE_CAPACITY 20
#define SCREEN_COUNT 10


typedef enum
{

	SCENE_NIL = 0,
	SCENE_MENU,
	SCENE_GAMEPLAY,
	SCENE_LOADING,
	SCENE_CUSTON

}Scene_Type;


typedef enum
{
	SCREEN_NIL,
	SCREEN_STATIC_OR_SPLASH,
	SCREEN_MAIN,
	SCREEN_MENU,
	SCREEN_SETTINGS,
	SCREEN_CUSTOM
}Screen_Type;



typedef struct
{
	Dyl_Str name;
	Dyl_Str type;
	u64 id;
}Screen;


typedef struct
{

	u64 screen_ids[SCREEN_COUNT];
	Entity_Manager scene_entities;
	Dyl_Str name;
	Dyl_Str custom_type_str;
	mat4 projection;
	u64 id;
	u64 next_scene_id;
	Camera scene_camera;
	Scene_Type type;
	bool is_active;

}Scene;


typedef struct
{

	Dyl_Instanced_Renderer* shared_instanced_renderer;
	Dyl_Batch_Renderer* shared_batch_renderer;
	Arena arena;
	Screen* screens;
	Scene* scenes;
	u64 screen_capacity;
	u64 screen_count;
	u64 scene_count;
	u64 last_scene;
	u64 first_scene;
	u64 active_scene;
	bool is_editor_mode;
}Scene_Manager;

extern Scene_Manager global_scene_manager;


typedef u64 Scene_Id;


void global_scene_manager_initialization(Dyl_Instanced_Renderer* instanced_renderer, Dyl_Batch_Renderer* batch_renderer);
ENGINE_SCENE_API Scene_Id global_scene_manager_add(const char* name, const char* custom_scene_type /* CAN BE NULL MAKE SURE WE ARE PROPERLY HANDLING THIS
							  */, Scene_Type type );
ENGINE_SCENE_API Scene_Id global_scene_manager_get_current_active_scene();
void global_scene_manager_remove(const char* name);
//TODO: IN THE FUTURE WE SHOULD APPEND TO A MULTITUDE OF SCENE CAMERAS INSTEAD OF ONE CAMERA PER SCENE
ENGINE_SCENE_API void global_scene_manager_camera_initialization(u64 id, vec3 pos, bool relative_mouse, float window_width, float window_height);

ENGINE_SCENE_API void global_scene_manager_make_active(u64 id);

ENGINE_SCENE_API void global_scene_manager_entity_manager_initialize(u64 id);
ENGINE_SCENE_API void global_scene_manager_entity_manager_initialize_all_models(u64 id);

ENGINE_SCENE_API void global_scene_manager_entity_manager_initialize_model_from_id(u64 id, entity_id entity_id);

ENGINE_SCENE_API entity_id global_scene_manager_entity_actor_create(u64 id, vec3f position, vec3f size, Color color, bool has_texture, bool is_model);

ENGINE_SCENE_API void global_scene_manager_entity_manager_set_model_from_id(u64 id, entity_id entity, Asset* asset);

ENGINE_SCENE_API Entity_Manager* global_scene_manager_get_entity_manager(u64 id);

ENGINE_SCENE_API entity_id global_scene_manager_entity_sky_box_create(u64 id,const char* face_path1, const char* face_path2, const char* face_path3, const char* face_path4, const char* face_path5, const char* face_path6);

ENGINE_SCENE_API void global_scene_manager_camera_move_by_id(u64 id, Dyl_Event* event);


ENGINE_SCENE_API void global_scene_manager_update_by_id(u64 id, float dt);
void global_scene_manager_update_by_name(const char* name, float dt);
void global_scene_manager_update_all(float dt);
ENGINE_SCENE_API void global_scene_manager_render_by_id(u64 id);
void global_scene_manager_destroy();





#endif
