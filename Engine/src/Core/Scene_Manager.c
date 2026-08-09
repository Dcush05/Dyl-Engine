#include "Scene_Manager.h"
#include "cglm/cam.h"
#include "dyl_debug.h"
#include "entity_manager.h"

Scene_Manager global_scene_manager;




void global_scene_manager_initialization(Dyl_Instanced_Renderer* instanced_renderer, Dyl_Batch_Renderer* batch_renderer)
{
	global_scene_manager = (Scene_Manager){0};
	global_scene_manager.arena = arena_alloc((sizeof(Scene) * INITIAL_SCENE_CAPACITY) + (sizeof(Screen) * SCREEN_COUNT));
	global_scene_manager.screen_capacity = INITIAL_SCENE_CAPACITY;
	global_scene_manager.scenes = arena_push(&global_scene_manager.arena, sizeof(Scene));
	global_scene_manager.screens = arena_push(&global_scene_manager.arena, sizeof(Screen));

	global_scene_manager.shared_instanced_renderer = instanced_renderer;
	global_scene_manager.shared_batch_renderer = batch_renderer;



	
}

ENGINE_SCENE_API u64 global_scene_manager_add(const char* name, const char* custom_scene_type, Scene_Type type)

{


	if(custom_scene_type && type != SCENE_CUSTON || (custom_scene_type == NULL && type == SCENE_CUSTON))
	{
		DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_ERROR, "Parameters are not properly matching to create a scene!");	
		return 0;
	}

	for(u64 idx = 0; idx < global_scene_manager.screen_capacity; ++idx)
	{
		if(global_scene_manager.scenes[idx].type == SCENE_NIL)
		{
			global_scene_manager.scenes[idx].name = DYL_STR_LIT(name);
			if(custom_scene_type)
				global_scene_manager.scenes[idx].custom_type_str = DYL_STR_LIT(custom_scene_type);
			global_scene_manager.scenes[idx].type = type;
			global_scene_manager.scenes[idx].id = idx + 1;
			global_scene_manager.screen_count++;
			global_scene_manager.last_scene = idx;
			return global_scene_manager.scenes[idx].id;
		}
	}
}
//void global_scene_manager_remove(const char* name);

ENGINE_SCENE_API void global_scene_manager_camera_initialization(u64 id, vec3 pos, bool relative_mouse, float window_width, float window_height)
{
	for(u64 idx = 0; idx < global_scene_manager.screen_count; ++idx)
	{
		if(global_scene_manager.scenes[idx].id == id)
		{

			camera_init(&global_scene_manager.scenes[idx].scene_camera, pos, relative_mouse, window_width, window_height);
		//	glm_perspective(glm_rad(45.0f), (float)window_width / (float)window_height, 0.1f, 100.0f, global_scene_manager.scenes[idx].projection);
			break;

		}
	}
}

ENGINE_SCENE_API void global_scene_manager_entity_manager_initialize_model_from_id(u64 id, entity_id entity_id)
{
	for(u64 idx = 0; idx < global_scene_manager.screen_count; ++idx)
	{
		if(global_scene_manager.scenes[idx].id == id)
		{
			entity_initialize_model_from_id(&global_scene_manager.scenes[idx].scene_entities,entity_id);
			break;

		}
	}

}

ENGINE_SCENE_API Scene_Id global_scene_manager_get_current_active_scene()
{
	return global_scene_manager.active_scene;
}
void global_scene_manager_make_active(u64 id)
{
	for(u64 idx = 0; idx < global_scene_manager.screen_count; ++idx)
	{
		if(global_scene_manager.scenes[idx].id == id)
		{
			global_scene_manager.scenes[idx].is_active = true;
			global_scene_manager.active_scene = global_scene_manager.scenes[idx].id;
			//NOTE: Track the count of the amount of scenes that are active
			break;
		}
	}
}



ENGINE_SCENE_API void global_scene_manager_entity_manager_initialize(u64 id)
{

	for(u64 idx = 0; idx < global_scene_manager.screen_count; ++idx)
	{
		if(global_scene_manager.scenes[idx].id == id)
		{
			entity_manager_initialize(&global_scene_manager.scenes[idx].scene_entities, global_scene_manager.shared_batch_renderer, global_scene_manager.shared_instanced_renderer);
			break;
		}

	}

}



ENGINE_SCENE_API entity_id global_scene_manager_entity_actor_create(u64 id, vec3f position, vec3f size, Color color, bool has_texture, bool is_model)
{
	for(u64 idx = 0; idx < global_scene_manager.screen_count; ++idx)
	{
		if(global_scene_manager.scenes[idx].id == id)
		{
			return entity_actor_create(&global_scene_manager.scenes[idx].scene_entities
					   ,position, size, color, has_texture, is_model);
		}

	}
}

//TODO: Create a getter for a scene entity manager

ENGINE_SCENE_API void global_scene_manager_entity_manager_initialize_all_models(u64 id)
{

	for(u64 idx = 0; idx < global_scene_manager.screen_count; ++idx)
	{
		if(global_scene_manager.scenes[idx].id == id)
		{
			entity_initialize_all_models(&global_scene_manager.scenes[idx].scene_entities);
			break;
		}

	}

}

ENGINE_SCENE_API void global_scene_manager_entity_manager_set_model_from_id(u64 id, entity_id entity, Asset* asset)
{

	for(u64 idx = 0; idx < global_scene_manager.screen_count; ++idx)
	{
		if(global_scene_manager.scenes[idx].id == id)
		{
			entity_set_model_from_id(&global_scene_manager.scenes[idx].scene_entities, entity, asset);
			break;
		}

	}

}


ENGINE_SCENE_API entity_id global_scene_manager_entity_sky_box_create(u64 id,const char* face_path1, const char* face_path2, const char* face_path3, const char* face_path4, const char* face_path5, const char* face_path6)
{
	for(u64 idx = 0; idx < global_scene_manager.screen_count; ++idx)
	{
		if(global_scene_manager.scenes[idx].id == id)
		{
			return entity_sky_box_create(&global_scene_manager.scenes[idx].scene_entities
					   ,face_path1, face_path2, face_path3, face_path4, face_path5, face_path6);
		}

	}

}






void global_scene_manager_make_inactive(u64 id)
{
	for(u64 idx = 0; idx < global_scene_manager.screen_count; ++idx)
	{
		if(global_scene_manager.scenes[idx].id == id)
		{
			global_scene_manager.scenes[idx].is_active = false;
			break;
		}
	}
}
void global_scene_manager_update_by_name(const char* name, float dt)
{
	
}


ENGINE_SCENE_API Entity_Manager* global_scene_manager_get_entity_manager(u64 id)
{
	for(u64 idx = 0; idx < global_scene_manager.screen_count; ++idx)
	{
		if(global_scene_manager.scenes[idx].id == id)
		{
			return &global_scene_manager.scenes[idx].scene_entities;
		}
	}

}

ENGINE_SCENE_API void global_scene_manager_camera_move_by_id(u64 id, Dyl_Event* event)
{
	for(u64 idx = 0; idx < global_scene_manager.screen_count; ++idx)
	{
		if(global_scene_manager.scenes[idx].id == id && global_scene_manager.scenes[idx].is_active == true)
		{
			camera_input(&global_scene_manager.scenes[idx].scene_camera, event);

			
			//ui 
		
			//entities
			
			//entity_manager_render(&global_scene_manager.scenes[idx].scene_entities);
			break;
		}
	}


	
}

ENGINE_SCENE_API void global_scene_manager_update_by_id(u64 id, float dt)
{
	for(u64 idx = 0; idx < global_scene_manager.screen_count; ++idx)
	{
		if(global_scene_manager.scenes[idx].id == id && global_scene_manager.scenes[idx].is_active == true)
		{
		//	dyl_batch_renderer_set_proj(global_scene_manager.shared_batch_renderer, &global_scene_manager.scenes[idx].projection);
			dyl_batch_renderer_set_view(global_scene_manager.shared_batch_renderer, &global_scene_manager.scenes[idx].scene_camera.view);
			dyl_batch_renderer_set_camera_pos(global_scene_manager.shared_batch_renderer, &global_scene_manager.scenes[idx].scene_camera.camera_pos);
			dyl_instanced_renderer_set_view(global_scene_manager.shared_instanced_renderer, &global_scene_manager.scenes[idx].scene_camera.view);
			dyl_instanced_renderer_set_camera_pos(global_scene_manager.shared_instanced_renderer, &global_scene_manager.scenes[idx].scene_camera.camera_pos);
			camera_update(&global_scene_manager.scenes[idx].scene_camera, dt);
		//	global_scene_manager.shared_batch_renderer->is_ui = false;


			
			//ui 
		
			//entities
			
			//entity_manager_render(&global_scene_manager.scenes[idx].scene_entities);
			break;
		}
	}


}

ENGINE_SCENE_API void global_scene_manager_render_by_id(u64 id)
{
	for(u64 idx = 0; idx < global_scene_manager.screen_count; ++idx)
	{
		if(global_scene_manager.scenes[idx].id == id && global_scene_manager.scenes[idx].is_active == true)
		{
			//ui 
		
			//entities
			
			entity_manager_render(&global_scene_manager.scenes[idx].scene_entities);
			break;
		}
	}
//	db_flush(global_scene_manager.shared_batch_renderer);


}
void global_scene_manager_update_all(float dt);
void global_scene_manager_destroy();


