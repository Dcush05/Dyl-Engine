#include "entity_manager.h"
#include "cglm/types.h"
#include "dyl_debug.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>



void entity_manager_initialize(Entity_Manager* entity_manager, Arena* arena)
{
	ASSERT(entity_manager && arena, "Cannot pass null entity manager into func");

	entity_manager->entity_count = 0;
	entity_manager->shape = (Shape_Params*)arena_push(arena,MAX_ENTITY_COUNT * sizeof(Shape_Params));
	entity_manager->texture_rect = (vec4f*)arena_push(arena, MAX_ENTITY_COUNT * sizeof(vec4f));
	entity_manager->color = (Color*)arena_push(arena, MAX_ENTITY_COUNT * sizeof(Color));
	entity_manager->size = (vec2f*)arena_push(arena, MAX_ENTITY_COUNT * sizeof(vec2f));
	entity_manager->position = (vec2f*)arena_push(arena, MAX_ENTITY_COUNT * sizeof(vec2f));
	entity_manager->type = (Entity_Type*)arena_push(arena, MAX_ENTITY_COUNT * sizeof(Entity_Type));
	entity_manager->id = (uint32_t*)arena_push(arena, MAX_ENTITY_COUNT * sizeof(uint32_t));
	memset(entity_manager->id, 0, sizeof(uint32_t) * MAX_ENTITY_COUNT);
	entity_manager->component_flag = (uint32_t*)arena_push(arena, MAX_ENTITY_COUNT * sizeof(uint32_t));
	
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
			entity_manager->id[i]++;	
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

ENGINE_ENTITY_API uint32_t entity_actor_create(Entity_Manager* entity_manager, vec2f position, vec2f size, Color color, bool has_texture)
{
	uint32_t new_entity = entity_create(entity_manager, COMP_TRANSFORM | COMP_RENDER | COMP_COLLIDER | COMP_PHYSICS , ENTITY_ACTOR);
	ASSERT(new_entity != INVALID_ENTITY,"Unable to create new actor entity");
	entity_manager->position[new_entity] = position;
	entity_manager->size[new_entity] = size;
	entity_manager->color[new_entity] = color;

	return new_entity; 
}


ENGINE_ENTITY_API Entity_View entity_get_from_id(Entity_Manager* manager, uint32_t id)
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
	return view;
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





