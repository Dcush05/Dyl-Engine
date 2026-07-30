#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H	

#include "../utils/dyl_base.h"
#include "../renderer_engine_interface.h"
#include "../Events/dyl_events.h"
#include "../Assets/Asset_Manager.h"
#include <stdint.h>


#define MAX_ENTITY_COUNT 1024
#define INVALID_ENTITY 0xFFFFFFF
#ifdef ENGINE_EXPORTS
	#define ENGINE_ENTITY_API __declspec(dllexport)
#else
	#define ENGINE_ENTITY_API __declspec(dllimport)
#endif



typedef u32 entity_id;

typedef enum
{
	ENTITY_NIL = 0,
	ENTITY_PRIMITIVE,
	ENTITY_ACTOR,
	ENTITY_BILLBOARD,
	ENTITY_LIGHTING,
	ENTITY_TEXT,
	ENTITY_INANIMATE, 
	ENTITY_SKYBOX
}Entity_Type;


typedef enum
{
	COMP_NIL			= 0,
	COMP_TRANSFORM		= 1 << 0,
	COMP_RENDER			= 1 << 1,
	COMP_PHYSICS		= 1 << 2,
	COMP_COLLIDER		= 1 << 3,
	COMP_TEXTURE		= 1 << 4,
	COMP_MODEL			= 1 << 5,
}Component_Flag;



typedef struct
{

	void* model_memory_blocks[MAX_ENTITY_COUNT]; // Dedicated memory page for each entity's assets
	Dyl_Batch_Renderer* batch_renderer;
	Dyl_Instanced_Renderer* instanced_renderer;
	Model** model;
	Texture* texture;
	Shape_Params* shape;
	Dyl_Str* name;
	vec4f* texture_rect;
	vec4f* bounding_box;
	Color* color;
	vec3f* size;
	vec3f* position;
	Entity_Type* type;
	u32* component_flag;
	entity_id* id;
	Asset** asset;
	Arena arena;
	s32 entity_count;
	s32 selected_entity_id;

	
}Entity_Manager;


typedef struct
{
	Entity_Manager entity_ref;
//	int* id;

}Entity_View;


typedef Entity_View* Entity;

void entity_manager_initialize(Entity_Manager* entity_manager, Dyl_Batch_Renderer* batch_renderer, Dyl_Instanced_Renderer* instance_renderer);
void entity_manager_update(Entity_Manager* entity_manager, float dt);
int entity_initialize(Entity_Manager* manager, u32 component_flag, Entity_Type type);
Entity_View* get_all_entities_of_type(Entity_Manager* entity_manager, Entity_Type type);
Entity_View entity_get_from_type(Entity_Manager* manager, Entity_Type type); //gets the first type in the array
void entity_update_from_view(Entity entity, float dt);
void entity_set_model_selection_from_id(Entity_Manager* manager, entity_id id);

ENGINE_ENTITY_API void entity_manager_render(Entity_Manager* entity_manager);
ENGINE_ENTITY_API void entity_render_from_view(Dyl_Renderer* renderer, Entity entity);
ENGINE_ENTITY_API Entity_View entity_get_from_id(Entity_Manager* manager, u32 id);
ENGINE_ENTITY_API entity_id entity_shape_create(Entity_Manager* entity_manager, Shape_Params shape);

ENGINE_ENTITY_API void entity_set_model_from_id(Entity_Manager* manager, entity_id id, Asset* asset);

void entity_initialize_all_models(Entity_Manager* manager);
ENGINE_ENTITY_API entity_id entity_actor_create(Entity_Manager* entity_manager, vec3f position, vec3f size, Color color, bool has_texture, bool is_model);



ENGINE_ENTITY_API entity_id entity_sky_box_create(Entity_Manager* entity_manager, const char* face_path1, const char* face_path2, const char* face_path3, const char* face_path4, const char* face_path5, const char* face_path6);



//NOTE: (Dylan) Id right now is mostly for usage when the user knows that the entity will not be deinitliazed in the frame. In the future we must be able to set
//the attributes of an entity through its entity_view that the user got from the initialization step. 
//TLDR: IDX Becomes invalid when entity doesnt exist, which may lead to using an unintended entity/behavior

//TODO:(Dylan) The ID and IDX are two different concepts and should be split apart
//

//FROM ID CODE

ENGINE_ENTITY_API void entity_set_texture2d_from_id(Entity_Manager* manager, entity_id id, const char* path);
ENGINE_ENTITY_API void entity_set_position2f_from_id(Entity_Manager* manager, entity_id id, vec2f position);
ENGINE_ENTITY_API void entity_set_position3f_from_id(Entity_Manager* manager, entity_id id, vec3f position);


//FROM VIEW
ENGINE_ENTITY_API void entity_set_texture2d_from_view(Entity_Manager* manager, Entity_View* view, const char* path);
ENGINE_ENTITY_API void entity_set_position2f_from_view(Entity_Manager* manager, Entity_View* view, vec2f position);
ENGINE_ENTITY_API void entity_set_position3f_from_view(Entity_Manager* manager, Entity_View* view, vec3f position);






#endif
