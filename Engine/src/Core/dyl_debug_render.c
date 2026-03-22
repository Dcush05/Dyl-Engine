#include "dyl_debug_render.h"

void dyl_debug_text_manager_init(Dyl_Debug_Text_Manager* manager, Arena* arena)
{
	ASSERT(manager, "Manager is null");
	ASSERT(arena, "Arena is null");


	
	*manager = (Dyl_Debug_Text_Manager){0};
	manager->all_text = arena_push(arena, sizeof(Dyl_Debug_Text) * DEFAULT_DEBUG_TEXT_MAX);
	manager->capacity = DEFAULT_DEBUG_TEXT_MAX;
	manager->count = 0;

	for(size_t i = 0; i < manager->capacity; ++i)
	{
		manager->all_text[i].str = DYL_STR_LIT("");
		manager->all_text[i].size[0] = 0.25;
		manager->all_text[i].size[1] = 0.25;
		manager->all_text[i].pos[0] = 0;
		manager->all_text[i].pos[1] = (0 + (manager->all_text[i].size[0] * 100)) * i;

	}


}
void dyl_debug_text_push(Dyl_Debug_Text_Manager* manager, u8* str)
{
	ASSERT(manager, "Manager is null");
	ASSERT(str, "str is null");
	if(manager->count >= manager->capacity)
		return;


	for(size_t i = 0; i < manager->capacity; ++i)
	{
		if(manager->all_text[i].str.size == 0)
		{
			//dyl_str_cpy(&manager->all_text[i].str, str, sizeof(str)); //TODO:: fix str cpy plz
			manager->all_text[i].str = DYL_STR_LIT((char*)str);
			manager->count++;
			break;
		}
		
	}
}


void dyl_debug_text_render(Font_Renderer* renderer ,Dyl_Debug_Text_Manager* manager)
{
	for(size_t i = 0; i < manager->count; ++i)
	{
		render_text(renderer, manager->all_text[i].str.string_data, manager->all_text[i].pos[0], manager->all_text[i].pos[1], manager->all_text[i].size[0], (vec3){1.0f,0.0f,0.0f} );
		manager->all_text[i].str = (Dyl_Str){0};
	}
	manager->count = 0;
	
}


