#include "dyl_debug_render.h"
#include "entity_manager.h"
#include <string.h>


Dyl_Debug_Text_Manager global_debug_text_manager = {0};

void dyl_debug_text_manager_init(Arena* arena)
{
	ASSERT(arena, "Arena is null");


	
	global_debug_text_manager.all_text = arena_push(arena, sizeof(Dyl_Debug_Text) * DEFAULT_DEBUG_TEXT_MAX);
	global_debug_text_manager.capacity = DEFAULT_DEBUG_TEXT_MAX;

	for(size_t i = 0; i < global_debug_text_manager.capacity; ++i)
	{
		global_debug_text_manager.all_text[i].str = DYL_STR_LIT("");
		global_debug_text_manager.all_text[i].size[0] = 0.25;
		global_debug_text_manager.all_text[i].size[1] = 0.25;
		global_debug_text_manager.all_text[i].pos[0] = 0;
		global_debug_text_manager.all_text[i].pos[1] = (0 + (global_debug_text_manager.all_text[i].size[0] * 100)) * i;

	}
	global_debug_text_manager.selected_entity_text.size[0] = 0.25;
	global_debug_text_manager.selected_entity_text.size[1] = 0.25;
	global_debug_text_manager.selected_entity_text.pos[0] = 0;
	global_debug_text_manager.selected_entity_text.pos[1] = global_debug_text_manager.all_text[DEFAULT_DEBUG_TEXT_MAX - 1].pos[1] + 100;
	global_debug_text_manager.current_selected = -1;
	global_debug_text_manager.selected_entity = (Entity_View){0};





}
void dyl_debug_text_push(u8* str)
{
	ASSERT(str, "str is null");
	if(global_debug_text_manager.count >= global_debug_text_manager.capacity)
		return;


	for(size_t i = 0; i < global_debug_text_manager.capacity; ++i)
	{
		if(global_debug_text_manager.all_text[i].str.size == 0)
		{
			//dyl_str_cpy(&manager->all_text[i].str, str, sizeof(str)); //TODO:: fix str cpy plz
			global_debug_text_manager.all_text[i].str = DYL_STR_LIT((char*)str);
			global_debug_text_manager.count++;
			break;
		}
		
	}
}


void dyl_debug_text_render(Font_Renderer* renderer)
{
	for(size_t i = 0; i < global_debug_text_manager.count; ++i)
	{
		render_text(renderer, global_debug_text_manager.all_text[i].str.string_data, global_debug_text_manager.all_text[i].pos[0], global_debug_text_manager.all_text[i].pos[1], global_debug_text_manager.all_text[i].size[0], (vec3){1.0f,0.0f,0.0f} );
		global_debug_text_manager.all_text[i].str = (Dyl_Str){0};
	}
	
	global_debug_text_manager.count = 0;
	
}


void dyl_debug_entity_select(Arena* arena ,Entity_Manager* entity_manager, Dyl_Event* event)
{
	
	static s32 last_selected_entity = 0;
	if(dyl_event_key_handle(event, DYLKEY_LSHIFT, DYL_KEY_PRESSED))
	{
		global_debug_text_manager.current_selected++;
		if(global_debug_text_manager.current_selected == entity_manager->entity_count)
			global_debug_text_manager.current_selected = 0; //NOTE: (dylan)find a better way to set it back to the first id of an entity currently in data
	}

	if(global_debug_text_manager.current_selected >= 0 && global_debug_text_manager.current_selected != entity_manager->entity_count)
	{
		global_debug_text_manager.selected_entity = entity_get_from_id(entity_manager, global_debug_text_manager.current_selected);

		if(*global_debug_text_manager.selected_entity.entity_ref.component_flag & COMP_MODEL)
		{
			//NOTE: Set last selected entity to false so it resets	
			//
			//entity_set_model_selection_from_id(entity_manager, last_selected_entity, false);
		    //entity_set_model_selection_from_id(entity_manager, global_debug_text_manager.current_selected, true);
			//entity_manager->selected_entity_id = *global_debug_text_manager.selected_entity.entity_ref.id;
			entity_set_model_selection_from_id(entity_manager, global_debug_text_manager.current_selected);
		}


		if(global_debug_text_manager.current_selected != last_selected_entity || global_debug_text_manager.selected_entity_text.str.string_data == NULL)
		{

		//	if(*global_debug_text_manager.selected_entity.entity_ref.type != ENTITY_SKYBOX)
		//	{


			Dyl_Str formatted_str = dyl_str_lit_fmt(arena, "---------Selected Entity--------\n"
															"ID: %u\n"
															"Position: (%f, %f, %f)\n"
															"Component flags(hex): %#x\n"
															"Entity Type(value): %d\n",
															*global_debug_text_manager.selected_entity.entity_ref.id, 
															(double)global_debug_text_manager.selected_entity.entity_ref.position->x, 
															(double)global_debug_text_manager.selected_entity.entity_ref.position->y,
															(double)global_debug_text_manager.selected_entity.entity_ref.position->z,
															*global_debug_text_manager.selected_entity.entity_ref.component_flag,
															*global_debug_text_manager.selected_entity.entity_ref.type);
			global_debug_text_manager.selected_entity_text.str = formatted_str;
			}
			last_selected_entity = global_debug_text_manager.current_selected;
		
			//entity_set_model_selection_from_id(entity_manager, last_selected_entity, false);


			printf("%s", (char*)global_debug_text_manager.selected_entity_text.str.string_data);

		//}
					

	}
	//FIX:
//	if(last_selected_entity - 1 > 0 && *entity_get_from_id(entity_manager, last_selected_entity - 1).entity_ref.component_flag & COMP_MODEL)
//		entity_set_model_selection_from_id(entity_manager, last_selected_entity, false);
}
void dyl_debug_entity_render(Font_Renderer* renderer)
{
	//get the selected entities data and make sure to render its selection at some point through the stencil buffer
	//
	if(global_debug_text_manager.current_selected >= 0 && global_debug_text_manager.selected_entity_text.str.size > 0)
//		printf("data");
		render_text(renderer, global_debug_text_manager.selected_entity_text.str.string_data, global_debug_text_manager.selected_entity_text.pos[0], global_debug_text_manager.selected_entity_text.pos[1], global_debug_text_manager.selected_entity_text.size[0], (vec3){1.0f,0.0f,0.0f} );


	//Rendering through stencil buffer 
	
}




