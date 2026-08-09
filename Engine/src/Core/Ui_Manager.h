#ifndef UI_MANAGER_H
#define UI_MANAGER_H



#include "../utils/dyl_types.h"
#include "../renderer/Dyl_Renderer.h"
#include "../Events/dyl_events.h"

typedef enum
{
	UI_NIL = 0,
	UI_TEXT,
	UI_BUTTON,
	UI_PANEL,
	UI_CONTAINER,
	UI_RECT,
	UI_CHECKBOX

}Ui_Type;


//PRMITIVES



/*typedef struct
{

	vec
}Dyl_UI_Rect;*/




#define UI_BUTTON_HOVER_FLAG			(1 << 1)
#define UI_BUTTON_PRESSED_FLAG			(1 << 2)
#define UI_BUTTON_RELEASED_FLAG			(1 << 3)

#define UI_PANEL_NAME_FLAG				(1 << 4)
#define UI_PANEL_TRANSPARENT_GLOSS_FLAG	(1 << 5)
#define UI_PANEL_SCROLLABLE_FLAG		(1 << 6)
#define UI_PANEL_MOVABLE_FLAG			(1 << 7)
#define UI_PANEL_RESIZEABLE_FLAG		(1 << 8)
#define UI_PANELL_NON_ROUNDED_RECTS		(1 << 9)

#define UI_PARENT_FLAG					(1 << 10)
#define UI_CHILD_FLAG					(1 << 11)
#define UI_SIBLING_FLAG					(1 << 12)



typedef struct
{
	//Vertex_Data vertices;
	
	Vertex vertex;
	//NOTE(dylan): We could have flags that allows us to see the name of the panel
	
	Ui_Type type;

	vec2f padding;
	vec2f margin;

	s32 parent_element_idx;
	s32 child_element_idx;
	s32 next_element_idx;
	s32 id;
	s32 depth;
	u16 button_state_flags;	
	u16 relation_flags;
	u16 panel_flags;
	bool is_pressed;


}Ui_Element;


typedef struct
{
	Vertex vertex;
	Dyl_Str text;
	u64 id;
	float scale;
}Ui_Text;



#define ELEMENT_MAX 512


typedef enum
{
	CHECK_OFF,
	CHECK_ON,
	CHECK_NEUTRAL 
}Check_State;


typedef struct
{
	Check_State state;
	u64 id;
}Ui_Element_State_Data;

typedef struct
{

	Font_Renderer font_renderer;
	Ui_Text text_elements[ELEMENT_MAX];
	Ui_Element_State_Data element_state[ELEMENT_MAX];
	vec2f saved_new_positions[ELEMENT_MAX];
	u64 parent_stack_idx[ELEMENT_MAX];
	mat4 projection;	
	Arena arena;
	Arena vertex_arena;
	Vertex_Data vertices;
	Ui_Element* elements;
	Dyl_Batch_Renderer* renderer;
	Dyl_Event* event;
	u64 count;
	u64 capacity;
	u64 element_state_count;
	u64 parent_stack_count;
	u64 text_element_count;
	s32 current_focus_panel;
	s32 current_panel_ui_element_count;
	float window_width;
	float window_height;

	
}Ui_Element_Manager;

extern Ui_Element_Manager global_ui_element_manager;

void global_ui_element_initialize(Dyl_Batch_Renderer* renderer,Dyl_Event* event ,float window_width, float window_height);
void ui_element_panel_start(vec2 position, vec2 size, Color color, u16 panel_flags);
void ui_element_panel_end();

//USAGE:


void ui_element_rect_start(vec2 position, vec2 size, Color color);
void ui_element_rect_end();

void ui_element_text_start(const char* text, vec2 position, float scale, Color color);
void ui_element_text_end();
bool ui_element_button_start(vec2 position, vec2 size, Color default_color, Color pressed_color);
void ui_element_button_end();


void ui_element_checkbox_start(vec2 position, vec2 size, Color default_color, Color checked_color, Color unchecked_color, bool* state);
void ui_element_checkbox_end();


void ui_element_set_padding(vec2f pad);
void ui_element_set_margin(vec2f margin);


void ui_element_render();



#endif
