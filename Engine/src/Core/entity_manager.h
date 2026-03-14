#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H	

#include "../utils/dyl_base.h"
#include "../renderer_engine_interface.h"
#include "../Events/dyl_events.h"
#include <stdint.h>


#define MAX_ENTITY_COUNT 512
#define INVALID_ENTITY 0xFFFFFFF
#ifdef ENGINE_EXPORTS
	#define ENGINE_ENTITY_API __declspec(dllexport)
#else
	#define ENGINE_ENTITY_API __declspec(dllimport)
#endif


typedef enum
{
	ENTITY_NIL = 0,
	ENTITY_PRIMITIVE,
	ENTITY_ACTOR,
	ENTITY_BILLBOARD,
	ENTITY_SKYBOX,
}Entity_Type;


typedef enum
{
	COMP_NIL			= 0,
	COMP_TRANSFORM		= 1 << 0,
	COMP_RENDER			= 1 << 1,
	COMP_PHYSICS		= 1 << 2,
	COMP_COLLIDER		= 1 << 3,
}Component_Flag;

typedef struct
{
	Shape_Params* shape;
//	dyl_string* name;
	vec4f* texture_rect;
	vec4f* bounding_box;
	Color* color;
	vec2f* size;
	vec2f* position;
	Entity_Type* type;
	u32* component_flag;
	u32* id;
	int entity_count;

	
}Entity_Manager;


typedef struct
{
	Entity_Manager entity_ref;
//	int* id;

}Entity_View;


typedef Entity_View* Entity;

void entity_manager_initialize(Entity_Manager* entity_manager, Arena* arena);
void entity_manager_update(Entity_Manager* entity_manager, float dt);
ENGINE_ENTITY_API void entity_manager_render(Dyl_Renderer* renderer, Entity_Manager* entity_manager);
int entity_initialize(Entity_Manager* manager, u32 component_flag, Entity_Type type);

ENGINE_ENTITY_API u32 entity_shape_create(Entity_Manager* entity_manager, Shape_Params shape);
ENGINE_ENTITY_API u32 entity_actor_create(Entity_Manager* entity_manager, vec2f position, vec2f size, Color color, bool has_texture);


Entity_View* get_all_entities_of_type(Entity_Manager* entity_manager, Entity_Type type);
Entity_View entity_get_from_type(Entity_Manager* manager, Entity_Type type); //gets the first type in the array
ENGINE_ENTITY_API Entity_View entity_get_from_id(Entity_Manager* manager, u32 id);
void entity_update_from_view(Entity entity, float dt);
ENGINE_ENTITY_API void entity_render_from_view(Dyl_Renderer* renderer, Entity entity);




#endif
