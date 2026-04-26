#include "dyl_debug_render.h"
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


