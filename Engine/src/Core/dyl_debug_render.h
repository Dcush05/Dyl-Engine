#ifndef DYL_DEBUG_RENDERER_H
#define DYL_DEBUG_RENDERER_H


#include "cglm/types.h"
#include "../utils/dyl_str.h"
#include "../Renderer/Dyl_Renderer.h"
typedef struct
{

	Dyl_Str str;
	vec2 size;
	vec2 pos;

}Dyl_Debug_Text;





typedef struct
{
	Dyl_Debug_Text* all_text;
	size_d count;
	size_t capacity;

}Dyl_Debug_Text_Manager;


#define DEFAULT_DEBUG_TEXT_MAX 15

void dyl_debug_text_manager_init(Dyl_Debug_Text_Manager* manager, Arena* arena);
void dyl_debug_text_push(Dyl_Debug_Text_Manager* manager, u8* str);
void dyl_debug_text_render(Font_Renderer* renderer ,Dyl_Debug_Text_Manager* manager);


#endif
