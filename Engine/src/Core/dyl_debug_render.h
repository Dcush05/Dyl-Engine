#ifndef DYL_DEBUG_RENDERER_H
#define DYL_DEBUG_RENDERER_H


#include "cglm/types.h"
#include "../utils/dyl_str.h"
#include "../Renderer/Dyl_Renderer.h"
#include "entity_manager.h"


typedef struct
{

	Dyl_Str str;
	vec2 size;
	vec2 pos;

}Dyl_Debug_Text;

typedef struct
{
	Arena formatted_str_arena;
	Dyl_Debug_Text selected_entity_text;
	Entity_View selected_entity;
	Dyl_Debug_Text* all_text;
	size_d count;
	size_d capacity;
	s32 current_selected;

}Dyl_Debug_Text_Manager;


extern Dyl_Debug_Text_Manager global_debug_text_manager;


#define DEFAULT_DEBUG_TEXT_MAX 15


void dyl_debug_text_manager_init(Arena* arena);
void dyl_debug_text_push(u8* str);
void dyl_debug_text_render(Font_Renderer* renderer);



void dyl_debug_entity_select(Arena* arena ,Entity_Manager* entity_manager, Dyl_Event* event);
void dyl_debug_entity_render(Font_Renderer* renderer);


#endif
