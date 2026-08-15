#include "entity_manager.h"
#include "../Assets/Asset_Manager.h"
#include "cglm/types.h"
#include "dyl_debug.h"
#include "platform.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>




void entity_manager_initialize(Entity_Manager* entity_manager, Dyl_Batch_Renderer* batch_renderer, Dyl_Instanced_Renderer* instance_renderer)
{
	ASSERT(entity_manager, "Cannot pass null entity manager into func");

	entity_manager->entity_count = 0;

	entity_manager->arena = arena_alloc((sizeof(Entity_Manager) * MAX_ENTITY_COUNT) * 9000);
	entity_manager->shape = arena_push(&entity_manager->arena,MAX_ENTITY_COUNT * sizeof(Shape_Params));
	if(!entity_manager->shape)
		return;


	entity_manager->batch_renderer = batch_renderer;
	entity_manager->instanced_renderer = instance_renderer;
	entity_manager->model = arena_push(&entity_manager->arena, MAX_ENTITY_COUNT * sizeof(Model*));
	Model* all_models = arena_push(&entity_manager->arena, MAX_ENTITY_COUNT * sizeof(Model));
	ASSERT(all_models, "Contiguous model storage is null");

	// 3. Point your array elements to the slots in that block
	for(size_t i = 0; i < MAX_ENTITY_COUNT; ++i)
	{
		entity_manager->model[i] = &all_models[i];
	}


	entity_manager->texture_rect = arena_push(&entity_manager->arena, MAX_ENTITY_COUNT * sizeof(vec4f));
	ASSERT(entity_manager->texture_rect, "Entity texture_rect data is null");

	entity_manager->texture = arena_push(&entity_manager->arena, MAX_ENTITY_COUNT * sizeof(Texture));
	ASSERT(entity_manager->texture_rect, "Entity texture_rect data is null");
	
	
	entity_manager->color = arena_push(&entity_manager->arena, MAX_ENTITY_COUNT * sizeof(Color));
	ASSERT(entity_manager->color, "Entity color data is null");
	
	entity_manager->size = arena_push(&entity_manager->arena, MAX_ENTITY_COUNT * sizeof(vec3f));
	ASSERT(entity_manager->size, "Entity size data is null");
	
	entity_manager->position = arena_push(&entity_manager->arena, MAX_ENTITY_COUNT * sizeof(vec3f));
	ASSERT(entity_manager->position, "Entity position data is null");
	
	entity_manager->type = arena_push(&entity_manager->arena, MAX_ENTITY_COUNT * sizeof(Entity_Type));
	ASSERT(entity_manager->type, "Entity type data is null");
	
	entity_manager->id = arena_push(&entity_manager->arena, MAX_ENTITY_COUNT * sizeof(entity_id));
	ASSERT(entity_manager->id, "Entity id data is null");
	
//	memset(entity_manager->id, 0, sizeof(uint32_t) * MAX_ENTITY_COUNT);
	
	entity_manager->component_flag = arena_push(&entity_manager->arena, MAX_ENTITY_COUNT * sizeof(u32));
	ASSERT(entity_manager->component_flag, "Entity component_flag data is null");
	entity_manager->asset = arena_push(&entity_manager->arena, MAX_ENTITY_COUNT * sizeof(Asset*));
	ASSERT(entity_manager->asset, "Entity Asset* data is null");
	entity_manager->camera = arena_push(&entity_manager->arena, MAX_ENTITY_COUNT * sizeof(Camera*));
	ASSERT(entity_manager->camera, "Entity Camera* data is null");
	entity_manager->camera_mode = arena_push(&entity_manager->arena, MAX_ENTITY_COUNT * sizeof(Entity_Camera_Mode));
	ASSERT(entity_manager->camera, "Entity Camera_Mode data is null")


	entity_manager->selected_entity_id = -1;	

	
}


uint32_t entity_create(Entity_Manager* entity_manager, uint32_t component_flag, Entity_Type type)
{
	ASSERT(entity_manager, "Cannot pass null entity manager into func");
	if(entity_manager->entity_count >= MAX_ENTITY_COUNT)
	{
		Dyl_App_Print_Log("Have reached entity limit");
		return INVALID_ENTITY;
	}

	for(size_t i = 0; i < MAX_ENTITY_COUNT; ++i)
	{
		if(entity_manager->type[i] == ENTITY_NIL)
		{
			entity_manager->component_flag[i] |= component_flag;
			entity_manager->type[i] = type;
			entity_manager->id[i] = i + 1;	
			entity_manager->entity_count++;
			return i;
		}
	}
	return INVALID_ENTITY;
}


ENGINE_ENTITY_API uint32_t entity_shape_create(Entity_Manager* entity_manager, Shape_Params shape)
{
	ASSERT(entity_manager, "Cannot pass null entity manager through func");	
	uint32_t new_entity = entity_create(entity_manager, COMP_RENDER | COMP_TRANSFORM, ENTITY_PRIMITIVE);
	ASSERT(new_entity != INVALID_ENTITY,"Unable to create new shape entity");
	entity_manager->shape[new_entity] = shape;
	return new_entity; 
}



ENGINE_ENTITY_API entity_id entity_blank_create(Entity_Manager* entity_manager, u32 flags, bool has_texture)
{
	//TODO: (Dylan) Have the user call the api and allow them to create an entity of their choice, when they are returned the id, they can then use the id to set 
	//specific things.
	//We may use optional parameters here but i dont mind multiple function calls
	//Example:
	//u32 id  = entity_blank_create(entity_manager, COMP_TEXTURE)
	//entity_set_texture_from_id(entity_manager, id, entity_texture);
}

ENGINE_ENTITY_API entity_id entity_actor_create(Entity_Manager* entity_manager, vec3f position, vec3f size, Color color, bool has_texture, bool is_model)
{
	u32 flags = (has_texture == true) ? COMP_TRANSFORM | COMP_RENDER | COMP_COLLIDER | COMP_PHYSICS | COMP_TEXTURE : COMP_TRANSFORM | COMP_RENDER | COMP_COLLIDER | COMP_PHYSICS | COMP_TEXTURE;

	if(is_model)
		flags |= COMP_MODEL;


	uint32_t new_entity = entity_create(entity_manager, flags, ENTITY_ACTOR);
	ASSERT(new_entity != INVALID_ENTITY,"Unable to create new actor entity");
	entity_manager->position[new_entity] = position;
	entity_manager->size[new_entity] = size;
	entity_manager->color[new_entity] = color;
	

	return new_entity; 
}


ENGINE_ENTITY_API entity_id entity_sky_box_create(Entity_Manager* entity_manager, const char* face_path1,
												  const char* face_path2, const char* face_path3, const char* face_path4, const char* face_path5, const char* face_path6)
{
	u32 flags = COMP_RENDER;

	uint32_t new_entity = entity_create(entity_manager, flags, ENTITY_SKYBOX);

	ASSERT(new_entity != INVALID_ENTITY,"Unable to create new actor entity");

//	entity_manager->position[new_entity] = position;
	Texture_Path skybox_paths = (Texture_Path){.face_paths[0] = DYL_STR_LIT(face_path1), .face_paths[1] = DYL_STR_LIT(face_path2), 
		.face_paths[2] = DYL_STR_LIT(face_path3),. face_paths[3] = DYL_STR_LIT(face_path4),.face_paths[4] = DYL_STR_LIT(face_path5), 
		.face_paths[5] = DYL_STR_LIT(face_path6)};

	entity_manager->texture[new_entity] = texture_init(skybox_paths, TEXTURE_CUBE_MAP, TEXTURE_NIL, true);
	return new_entity;

	
}


ENGINE_ENTITY_API void entity_set_model_from_id(Entity_Manager* manager, entity_id id, Asset* asset)
{
	ASSERT(manager, "Passing invalid manager");
	ASSERT(manager->model[id] != NULL, "Entity model slot is NULL! Allocate the model before setting its path.");
	if(!(manager->component_flag[id] & COMP_MODEL) && (manager->type[id] == ENTITY_ACTOR))
	{
		DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_ERROR, "Entity has invalid flags to attach model");
		return;
	}
	manager->asset[id] = asset;
//	Temp_Arena scratch = temp_arena_scratch_get(NULL, 0, sizeof(Model_Init_Args) * 200);
	//
	Model_Init_Args* args = arena_push(&manager->arena, sizeof(Model_Init_Args));
	args->model = manager->model[id];

	if(asset->model.type == ASSET_MODEL_OBJ)
	{
		args->model->model_parse_data.type = MODEL_PARAM_TINYOBJ;
		args->model->model_parse_data.tiny_data.attrib = asset->model.tiny_data.attrib;
		args->model->model_parse_data.tiny_data.materials = asset->model.tiny_data.materials;
		args->model->model_parse_data.tiny_data.shapes = asset->model.tiny_data.shapes;


	}else if(asset->model.type == ASSET_MODEL_GLTF)
	{
		args->model->model_parse_data.type = MODEL_PARAM_GLTF;
		args->model->model_parse_data.gltf_data.data = asset->model.gltf_data;
	}

	args->model->num_shapes = asset->model.shape_count;
	args->model->num_materials = asset->model.material_count;
	//	args->model = &(Model){0};
	args->file_name = (char*)asset->file_name.string_data;
	args->rel_path = (char*)asset->rel_path.string_data;
	args->arena = &manager->arena;




/*	args->model = manager->model[id];
//	args->model = &(Model){0};
	args->file_name = path;
	args->rel_path = rel_path;
	args->arena = manager->arena;*/
//	Asset_Init_Args* args = arena_push(manager->arena, sizeof(Asset_Init_Args));


	Dyl_Str thread_name = dyl_str_lit_fmt(&manager->arena, "Model: %d", id);
	dyl_thread_pool_add((char*)thread_name.string_data);
//	dyl_thread_pool_task_add((char*)thread_name.string_data, (void*)asset_create_thread_func, args);
	dyl_thread_pool_task_add((char*)thread_name.string_data, (void*)model_init, args);

//	model_init(args);



//	*manager->model[id] = *args->model;
	//model_setup(manager->model[id], manager->arena);
	//temp_arena_scratch_end(scratch);
	//dyl_instanced_renderer_initialize_mod_and_vbo(manager->instanced_renderer, manager->model[id]);

}


void entity_initialize_all_models(Entity_Manager* manager)
{
	dyl_thread_pool_spin();
	for(size_t i = 0; i < manager->entity_count; ++i)
	{
		if((manager->component_flag[i] & COMP_MODEL) && (manager->type[i] == ENTITY_ACTOR))
		{

			model_setup(manager->model[i], &manager->arena);
			dyl_instanced_renderer_initialize_mod_and_vbo(manager->instanced_renderer, manager->model[i]);

		}

	}
}

void entity_initialize_model_from_id(Entity_Manager* manager, entity_id id)
{

	dyl_thread_pool_spin();
	for(size_t i = 0; i < manager->entity_count; ++i)
	{

		if((manager->component_flag[i] & COMP_MODEL) && (manager->type[i] == ENTITY_ACTOR)
			&& manager->id[i] == id)
		{

			model_setup(manager->model[i], &manager->arena);
			dyl_instanced_renderer_initialize_mod_and_vbo(manager->instanced_renderer, manager->model[i]);
			break;

		}

	}

}

void entity_set_model_selection_from_id(Entity_Manager* manager, entity_id id)
{
	if(!(manager->component_flag[id] & COMP_MODEL) && (manager->type[id] == ENTITY_ACTOR))
	{
		DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_ERROR, "Entity has invalid flags to attach model");
		return;
	}
	//model_set_selected_model(manager->model[id], selected);
	manager->selected_entity_id = manager->id[id];
}

ENGINE_ENTITY_API void entity_set_texture2d_from_id(Entity_Manager* manager, entity_id id, const char* path)
{
	ASSERT(manager, "Passing invalid manager");
	if(!(manager->component_flag[id] & COMP_TEXTURE) && (manager->type[id] == ENTITY_ACTOR))
	{
		DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_ERROR, "Entity has invalid flags to attach model");
		return;
	}
	manager->texture[id] = texture_init((Texture_Path){.path = DYL_STR_LIT(path)}, TEXTURE_2D, TEXTURE_NIL,true);

}


ENGINE_ENTITY_API void entity_set_position2f_from_id(Entity_Manager* manager, entity_id id, vec2f position)
{

	//NOTE:We could also set positions of Enity billboards at some point but let focus on actors
	ASSERT(manager, "Passing invalid manager");

	if(!(manager->component_flag[id] & COMP_TRANSFORM | COMP_PHYSICS) && (manager->type[id] == ENTITY_ACTOR))
	{
		DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_ERROR, "Entity has invalid flags to attach model");
		return;
	}
	manager->position[id].x = position.x;
	manager->position[id].y = position.y;

}


ENGINE_ENTITY_API void entity_set_position3f_from_id(Entity_Manager* manager, entity_id id, vec3f position)
{
	ASSERT(manager, "Passing invalid manager");
	if(!(manager->component_flag[id] & COMP_TRANSFORM | COMP_PHYSICS) && (manager->type[id] == ENTITY_ACTOR))
	{
		DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_ERROR, "Entity has invalid flags to attach model");
		return;
	}
	manager->position[id].x = position.x;
	manager->position[id].y = position.y;
	manager->position[id].z = position.z;

//	*manager->model[id] = model_init(path, rel_path, manager->arena);
//	dyl_instanced_renderer_initialize_mod_and_vbo(manager->instanced_renderer, manager->model[id]);
	
}


ENGINE_ENTITY_API Entity_View entity_get_from_id(Entity_Manager* manager, entity_id id)
{
	Entity_View view;
	view.entity_ref.shape = &manager->shape[id];
	view.entity_ref.texture_rect = &manager->texture_rect[id];
	view.entity_ref.bounding_box = &manager->bounding_box[id];
	view.entity_ref.color = &manager->color[id];
	view.entity_ref.size = &manager->size[id];
	view.entity_ref.position = &manager->position[id];
	view.entity_ref.type = &manager->type[id];
	view.entity_ref.component_flag = &manager->component_flag[id];
	view.entity_ref.id = &manager->id[id];
	view.entity_ref.model = &manager->model[id];
	return view;
}

ENGINE_ENTITY_API void entity_attach_camera_from_id(Entity_Manager* manager, entity_id id, Entity_Camera_Mode mode, Camera* camera)
{
	for(size_d idx = 0; idx < manager->entity_count; ++idx)
	{
		if(manager->type[idx] == ENTITY_NIL || ENTITY_SKYBOX)
			continue;
		if(!(manager->component_flag[idx] & COMP_RENDER))
			continue;

		manager->camera[idx] = camera;
		manager->camera_mode[idx] = mode;


	}
}

ENGINE_ENTITY_API void entity_update_attached_camera_from_id(Entity_Manager* manager, entity_id id)
{
	for(size_d idx = 0; idx < manager->entity_count; ++idx)
	{
		if(manager->type[idx] == ENTITY_NIL || ENTITY_SKYBOX)
			continue;
		if(!(manager->component_flag[idx] & COMP_RENDER))
			continue;

		camera_set_follow_position(manager->camera[idx], (vec3){manager->position[idx].x, manager->position[idx].y - 100, manager->position[idx].z - 15});



	}

}

ENGINE_ENTITY_API void entity_manager_render(Entity_Manager* entity_manager)
{
	for(size_d idx = 0; idx < entity_manager->entity_count; ++idx)
	{
		if(entity_manager->type[idx] == ENTITY_NIL)
			continue;

		if(!(entity_manager->component_flag[idx] & COMP_RENDER))
			continue;

		if(entity_manager->type[idx] == ENTITY_SKYBOX)
		{
			db_sky_box_draw(entity_manager->batch_renderer, &entity_manager->texture[idx], (vec4){255,255,255,255});
		}
		
	}


	printf("dyl_instanced_draw called: instance_count=%d current_model=%p vao=%u triangles=%zu\n",
    entity_manager->instanced_renderer->instance_count,
    entity_manager->instanced_renderer->current_model,
    entity_manager->instanced_renderer->current_model ? entity_manager->instanced_renderer->current_model->mesh.m_vao : 0,
    entity_manager->instanced_renderer->current_model ? entity_manager->instanced_renderer->current_model->num_triangles : 0);
	for(size_d idx = 0; idx < entity_manager->entity_count; ++idx)
	{
		if(entity_manager->type[idx] == ENTITY_NIL)
			continue;

		if(!(entity_manager->component_flag[idx] & COMP_RENDER))
			continue;

		if(entity_manager->type[idx] == ENTITY_ACTOR)
		{
			if(entity_manager->component_flag[idx] & COMP_MODEL)
			{
				Model* model = entity_manager->model[idx];

				bool is_selected = (entity_manager->selected_entity_id == (s32)entity_manager->id[idx]);
				if(entity_manager->camera_mode[idx] != CAMERA_MODE_NIL)
				{
					dyl_batch_renderer_set_view(entity_manager->batch_renderer, &entity_manager->camera[idx]->view);
					dyl_batch_renderer_set_camera_pos(entity_manager->batch_renderer, &entity_manager->camera[idx]->follow_pos);
					dyl_instanced_renderer_set_view(entity_manager->instanced_renderer, &entity_manager->camera[idx]->view);
					dyl_instanced_renderer_set_camera_pos(entity_manager->instanced_renderer, &entity_manager->camera[idx]->follow_pos);
					

				}
				dyl_instanced_push_model(entity_manager->instanced_renderer, model, (vec3){entity_manager->position[idx].x,
				entity_manager->position[idx].y, entity_manager->position[idx].z},(vec3){entity_manager->size[idx].x, 
							 entity_manager->size[idx].y, entity_manager->size[idx].z}, 0.0, (vec4){entity_manager->color[idx].r,
							 entity_manager->color[idx].g, entity_manager->color[idx].b, entity_manager->color[idx].a}, is_selected);

				//printf("Model should be rendering here muahaha");

			}

		}		
	}


	dyl_instanced_draw(entity_manager->instanced_renderer);
	

	db_flush(entity_manager->batch_renderer);	

	//entity_manager->selected_entity_id = -1;

//	entity_manager->instanced_renderer->instance_count = 0;
//	entity_manager->instanced_renderer->batch_start_idx = 0;
//	entity_manager->instanced_renderer->current_model = NULL;
}



/*ENGINE_ENTITY_API void entity_render_from_view(Renderer2D* renderer, Entity entity)
{
	ASSERT(renderer, "Cannot pass null renderer through func");
	ASSERT(entity, "Cannot pass null entity through func");

	if(*entity->entity_ref.type == ENTITY_PRIMITIVE)
	{
		draw_rectangle(renderer, (vec2){
			entity->entity_ref.shape->position2f.x, entity->entity_ref.shape->position2f.y
		}, (vec2){entity->entity_ref.shape->size2f.x, entity->entity_ref.shape->size2f.y}, entity->entity_ref.shape->rotation, (vec4){entity->entity_ref.shape->color.r,
				 entity->entity_ref.shape->color.g, entity->entity_ref.shape->color.b, entity->entity_ref.shape->color.b});

	}
}*/
/*ENGINE_ENTITY_API void entity_manager_render(Renderer2D* renderer, Entity_Manager* entity_manager)
{
	ASSERT(renderer, "Cannot pass null renderer through func");
	ASSERT(entity_manager, "Cannot pass null entity manager through func");

	for(size_t i = 0; i < MAX_ENTITY_COUNT; ++i)
	{
		if(entity_manager->type[i] == ENTITY_NIL)
			continue;


		if(entity_manager->type[i] == ENTITY_PRIMITIVE)
		{
			draw_rectangle(renderer, (vec2){entity_manager->shape[i].position2f.x, entity_manager->shape[i].position2f.y},
				  (vec2){entity_manager->shape[i].size2f.x, entity_manager->shape[i].size2f.y}, entity_manager->shape[i].rotation, 
				  (vec4){entity_manager->shape[i].color.r, entity_manager->shape[i].color.g, entity_manager->shape[i].color.b,
				  entity_manager->shape[i].color.a});
		}
	}
}*/





