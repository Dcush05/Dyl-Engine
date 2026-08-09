#include "Ui_Manager.h"
#include "cglm/cam.h"
#include "cglm/types.h"
#include "dyl_debug.h"
void ui_element_state_push(u64 id)
{
	for (u64 i = 0; i < global_ui_element_manager.element_state_count; i++) {
        if (global_ui_element_manager.element_state[i].id == id) {
            return;
        }
    }
	if(global_ui_element_manager.element_state_count >= ELEMENT_MAX)
		return;

	global_ui_element_manager.element_state[global_ui_element_manager.element_state_count].id = id;
	global_ui_element_manager.element_state[global_ui_element_manager.element_state_count].state = CHECK_OFF;
	global_ui_element_manager.element_state_count++;
}



bool ui_element_get_switch_state_on_id(u64 id)
{
	bool switch_state = false;
	for(u64 idx = 0; idx < ELEMENT_MAX; idx++)
	{
		if(global_ui_element_manager.element_state[idx].id == id)
		{
			switch_state = global_ui_element_manager.element_state[idx].state == CHECK_ON ? true : false;
			break;
		}
	}
	return switch_state;
}

void ui_element_set_switch_state_on_id(u64 id, bool switch_state)
{
	for(u64 idx = 0; idx < ELEMENT_MAX; idx++)
	{
		if(global_ui_element_manager.element_state[idx].id == id)
		{
			global_ui_element_manager.element_state[idx].state = switch_state == true ? CHECK_ON : CHECK_OFF;
			break;
		}
	}
}



Ui_Element_Manager global_ui_element_manager;

void global_ui_element_initialize(Dyl_Batch_Renderer* renderer,Dyl_Event* event ,float window_width, float window_height)
{
	global_ui_element_manager = (Ui_Element_Manager){0};
	global_ui_element_manager.arena = arena_alloc(sizeof(Ui_Element) * ELEMENT_MAX);
	global_ui_element_manager.vertex_arena = arena_alloc(sizeof(Vertex) * ELEMENT_MAX);
	global_ui_element_manager.elements = arena_push(&global_ui_element_manager.arena, sizeof(Ui_Element) * ELEMENT_MAX);
	vertices_setup(&global_ui_element_manager.vertices, &global_ui_element_manager.vertex_arena, ELEMENT_MAX);
	global_ui_element_manager.capacity = ELEMENT_MAX;
	global_ui_element_manager.window_width = window_width;
	global_ui_element_manager.window_height = window_height;
	global_ui_element_manager.renderer = renderer;
	global_ui_element_manager.event = event;
	global_ui_element_manager.current_focus_panel = -1;

	glm_ortho(0.0,window_width, window_height, 0.0f, -1.0f, 1.0f, global_ui_element_manager.projection);
	global_ui_element_manager.current_panel_ui_element_count = 0;

	global_ui_element_manager.font_renderer = font_renderer_init("assets/Fonts/Datatype.ttf", 65, &global_ui_element_manager.projection);
	
}
void ui_element_panel_start(vec2 position, vec2 size, Color color, u16 panel_flags)
{

	
	//TODO: This is simply just an implementation jus to get things up and running	
	Ui_Element panel;
	panel.type = UI_PANEL;

	vec2f base_pos = {position[0], position[1]};

	if (global_ui_element_manager.current_focus_panel == global_ui_element_manager.count)
	{

		if(dyl_event_key_handle(global_ui_element_manager.event, DYL_MOUSE_KEY_RBUTTON, DYL_MOUSE_KEY_RPRESS))
		{
			global_ui_element_manager.saved_new_positions[global_ui_element_manager.count] = (vec2f){
			.x = global_ui_element_manager.event->mouse_pos.x - base_pos.x,
			.y = global_ui_element_manager.event->mouse_pos.y - base_pos.y
			};
			global_ui_element_manager.current_focus_panel = -1;

		}
		color.r += 10;
		color.g += 10;
		color.b += 10;
		color.a += 80;
		
	}

	vec2f offset = global_ui_element_manager.saved_new_positions[global_ui_element_manager.count];

	panel.vertex.position[0] = base_pos.x + offset.x;
	panel.vertex.position[1] = base_pos.y + offset.y;

	panel.vertex.size[0] = size[0];
	panel.vertex.size[1] = size[1];

	panel.vertex.color[0] = color.r;
	panel.vertex.color[1] = color.g;
	panel.vertex.color[2] = color.b;
	panel.vertex.color[3] = color.a;
	panel.id = global_ui_element_manager.count + 1;

	panel.panel_flags = panel_flags;
	panel.relation_flags |= UI_PARENT_FLAG;
	panel.depth = 0;
	panel.padding = (vec2f){0};


	dyl_event_mouse_movement(global_ui_element_manager.event);
	DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_DEBUG,"mouse_position y -> %d, button y -> %f", global_ui_element_manager.event->mouse_pos.y, position[1]);

	if(global_ui_element_manager.event->mouse_pos.x >= panel.vertex.position[0] && global_ui_element_manager.event->mouse_pos.x <= panel.vertex.position[0] + size[0]
	&& global_ui_element_manager.event->mouse_pos.y >= panel.vertex.position[1] && global_ui_element_manager.event->mouse_pos.y <= panel.vertex.position[1] + size[1])
	{
		DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_INFO, "Mouse is within the bounds of the button!");

		if(dyl_event_key_handle(global_ui_element_manager.event, DYL_MOUSE_KEY_LBUTTON, DYL_MOUSE_KEY_LPRESS))
		{
			DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_INFO, "Button has been pressed");
			global_ui_element_manager.current_focus_panel = global_ui_element_manager.count;
			panel.is_pressed = true;
			

			
		}


	}else {
		if(dyl_event_key_handle(global_ui_element_manager.event, DYL_MOUSE_KEY_RBUTTON, DYL_MOUSE_KEY_RPRESS))
		{
			DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_INFO, "Button has been pressed");
			global_ui_element_manager.current_focus_panel = -1;
		}

	}


	global_ui_element_manager.parent_stack_idx[global_ui_element_manager.parent_stack_count] = global_ui_element_manager.count;
	global_ui_element_manager.parent_stack_count++;
	global_ui_element_manager.elements[global_ui_element_manager.count] = panel;
	global_ui_element_manager.count++;

}

void ui_element_panel_end()
{
	u64 panel_idx = global_ui_element_manager.parent_stack_idx[global_ui_element_manager.parent_stack_count];
	if(global_ui_element_manager.elements[panel_idx].type != UI_PANEL)
		return;

	vertices_push(&global_ui_element_manager.vertices, global_ui_element_manager.elements[panel_idx].vertex);
	global_ui_element_manager.elements[panel_idx].depth = 0;
	global_ui_element_manager.parent_stack_count--;
//	global_ui_element_manager.count--;

}





void ui_element_text_start(const char* text, vec2 position, float scale, Color color)
{
	
	vec2 pos;
	pos[0] = position[0];
	pos[1] = position[1];

	for(u64 idx = 0; idx < global_ui_element_manager.count; ++idx)
	{
		if(global_ui_element_manager.elements[idx].relation_flags & UI_PARENT_FLAG)
		{
			
			pos[0] += global_ui_element_manager.elements[idx].vertex.position[0];
			pos[1] += global_ui_element_manager.elements[idx].vertex.position[1];
			global_ui_element_manager.elements[idx].next_element_idx = idx + 1;
			global_ui_element_manager.elements[idx].child_element_idx = idx + 1;
			global_ui_element_manager.elements[idx].depth++;
			if(global_ui_element_manager.elements[idx].type == UI_PANEL)
			{
				global_ui_element_manager.current_panel_ui_element_count = global_ui_element_manager.elements[idx].depth + 1;
				if(global_ui_element_manager.elements[idx].panel_flags & UI_PANEL_NAME_FLAG)
				{
					pos[0] = global_ui_element_manager.elements[idx].vertex.position[0] + 10;
					pos[1] = global_ui_element_manager.elements[idx].vertex.position[1] + 10;
				}
			}
			//TODO: Make it so that you automatically adjust the font size by half based on if the parent is not a panel-based element (ex. buttons, rects, switches)
			//??????


		}else if(global_ui_element_manager.elements[idx].type == UI_BUTTON)
		{
			pos[0] = global_ui_element_manager.elements[idx].vertex.position[0] + global_ui_element_manager.elements[idx].padding.x;
			pos[1] = global_ui_element_manager.elements[idx].vertex.position[1] + global_ui_element_manager.elements[idx].padding.y;
			global_ui_element_manager.elements[idx].depth++;
			global_ui_element_manager.elements[idx].relation_flags |= UI_PARENT_FLAG;
		}else if(global_ui_element_manager.elements[idx].type == UI_RECT)
		{
			//NOTE: Account for the inner rect idx for checkbox
				if(global_ui_element_manager.elements[idx - 1].type == UI_CHECKBOX)
				{
					pos[0] = global_ui_element_manager.elements[idx - 1].vertex.position[0] + global_ui_element_manager.elements[idx].padding.x;
					pos[1] = global_ui_element_manager.elements[idx - 1].vertex.position[1] + global_ui_element_manager.elements[idx].padding.y;
					global_ui_element_manager.elements[idx - 1].depth++;
					global_ui_element_manager.elements[idx - 1].relation_flags |= UI_PARENT_FLAG;

				}
			}
	}

	Ui_Text text_ui;
	text_ui.text = DYL_STR_LIT(text);
	text_ui.vertex.position[0] = pos[0];
	text_ui.vertex.position[1] = pos[1];
	text_ui.vertex.color[0] = color.r;
	text_ui.vertex.color[1] = color.g;
	text_ui.vertex.color[2] = color.b;
	text_ui.vertex.color[3] = color.a;
	text_ui.scale = scale;
	
	text_ui.id = global_ui_element_manager.text_element_count + 1;
	global_ui_element_manager.text_elements[global_ui_element_manager.text_element_count] = text_ui;
	global_ui_element_manager.text_element_count++;
	
}

void ui_element_rect_start(vec2 position, vec2 size, Color color)
{
	vec2 pos;
	pos[0] = position[0];
	pos[1] = position[1];
	u8 relation_flags = 0;

	for(u64 idx = 0; idx < global_ui_element_manager.count; ++idx)
	{
		if(global_ui_element_manager.elements[idx].relation_flags & UI_PARENT_FLAG)
		{
			pos[0] += global_ui_element_manager.elements[idx].vertex.position[0];
			pos[1] += global_ui_element_manager.elements[idx].vertex.position[1];
			global_ui_element_manager.elements[idx].next_element_idx = idx + 1;
			global_ui_element_manager.elements[idx].child_element_idx = idx + 1;
			global_ui_element_manager.elements[idx].depth++;
			global_ui_element_manager.current_panel_ui_element_count = global_ui_element_manager.elements[idx].depth + 1;
			relation_flags |= UI_CHILD_FLAG;
		}
	}
	

	Ui_Element rect;
	rect.type = UI_RECT;
	rect.vertex.position[0] = pos[0];
	rect.vertex.position[1] = pos[1];
	rect.vertex.size[0] = size[0];
	rect.vertex.size[1] = size[1];
	rect.vertex.color[0] = color.r;
	rect.vertex.color[1] = color.b;
	rect.vertex.color[2] = color.g;
	rect.vertex.color[3] = color.a;
	rect.relation_flags = relation_flags;
	rect.id = global_ui_element_manager.count + 1;
	rect.padding = (vec2f){0};
	global_ui_element_manager.elements[global_ui_element_manager.count] = rect;
	global_ui_element_manager.count++;

	


}
void ui_element_rect_end()
{
	if(global_ui_element_manager.elements[global_ui_element_manager.count - 1].type != UI_RECT)
		return;

	vertices_push(&global_ui_element_manager.vertices, global_ui_element_manager.elements[global_ui_element_manager.count - 1].vertex);
	global_ui_element_manager.count--;
	global_ui_element_manager.current_panel_ui_element_count--;
}




bool ui_element_button_start(vec2 position, vec2 size, Color default_color, Color pressed_color)
{
	vec2 pos;
	pos[0] = position[0];
	pos[1] = position[1];
	u8 relation_flags = 0;

	for(u64 idx = 0; idx < global_ui_element_manager.count; ++idx)
	{
		if(global_ui_element_manager.elements[idx].relation_flags & UI_PARENT_FLAG)
		{
			pos[0] += global_ui_element_manager.elements[idx].vertex.position[0];
			pos[1] += global_ui_element_manager.elements[idx].vertex.position[1];
			global_ui_element_manager.elements[idx].next_element_idx = idx + 1;
			global_ui_element_manager.elements[idx].child_element_idx = idx + 1;
			global_ui_element_manager.elements[idx].depth++;
			global_ui_element_manager.current_panel_ui_element_count = global_ui_element_manager.elements[idx].depth + 1;
			relation_flags |= UI_CHILD_FLAG;
		}
	}
	

	Ui_Element button;
	button.type = UI_BUTTON;
	button.vertex.position[0] = pos[0];
	button.vertex.position[1] = pos[1];
	button.vertex.size[0] = size[0];
	button.vertex.size[1] = size[1];
	button.is_pressed = false;
	button.relation_flags = relation_flags;
	button.id = global_ui_element_manager.count + 1;
	button.padding = (vec2f){0};

	


	//Checking whether or not the mouse posiion is within the buttons rect
	dyl_event_mouse_movement(global_ui_element_manager.event);
	DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_DEBUG,"mouse_position y -> %d, button y -> %f", global_ui_element_manager.event->mouse_pos.y, pos[1]);
	float world_mouse_pos_y = global_ui_element_manager.window_height - global_ui_element_manager.event->mouse_pos.y;

	if(global_ui_element_manager.event->mouse_pos.x >= pos[0] && global_ui_element_manager.event->mouse_pos.x <= pos[0] + size[0]
	&& global_ui_element_manager.event->mouse_pos.y >= pos[1] && global_ui_element_manager.event->mouse_pos.y <= pos[1] + size[1])
	{
		DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_INFO, "Mouse is within the bounds of the button!");

		if(dyl_event_key_handle(global_ui_element_manager.event, DYL_MOUSE_KEY_LBUTTON, DYL_MOUSE_KEY_LPRESS))
		{
			DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_INFO, "Button has been pressed");
			button.is_pressed = true;
			global_ui_element_manager.current_focus_panel = -1;
		}

	}
	if(!button.is_pressed)
	{
		button.vertex.color[0] = default_color.r;
		button.vertex.color[1] = default_color.g;
		button.vertex.color[2] = default_color.b;
		button.vertex.color[3] = default_color.a;
	}else{
		button.vertex.color[0] = pressed_color.r;
		button.vertex.color[1] = pressed_color.g;
		button.vertex.color[2] = pressed_color.b;
		button.vertex.color[3] = pressed_color.a;
	}

	
	global_ui_element_manager.elements[global_ui_element_manager.count] = button;
	global_ui_element_manager.count++;
	return button.is_pressed; 



		







}
void ui_element_button_end()
{
	if(global_ui_element_manager.elements[global_ui_element_manager.count - 1].type != UI_BUTTON)
		return;

	vertices_push(&global_ui_element_manager.vertices, global_ui_element_manager.elements[global_ui_element_manager.count - 1].vertex);
	global_ui_element_manager.count--;
	global_ui_element_manager.current_panel_ui_element_count--;
}
//NOTE: checked_color and unchecked_color

void ui_element_checkbox_start(vec2 position, vec2 size, Color default_color, Color checked_color, Color unchecked_color, bool* state)
{
	//The outter rect	
	vec2 pos;
	pos[0] = position[0];
	pos[1] = position[1];
	u8 relation_flags = 0;

	for(u64 idx = 0; idx < global_ui_element_manager.count; ++idx)
	{
		if(global_ui_element_manager.elements[idx].relation_flags & UI_PARENT_FLAG)
		{
			pos[0] += global_ui_element_manager.elements[idx].vertex.position[0];
			pos[1] += global_ui_element_manager.elements[idx].vertex.position[1];
			global_ui_element_manager.elements[idx].next_element_idx = idx + 1;
			global_ui_element_manager.elements[idx].child_element_idx = idx + 1;
			global_ui_element_manager.elements[idx].depth++;
			global_ui_element_manager.current_panel_ui_element_count = global_ui_element_manager.elements[idx].depth + 1;
			relation_flags |= UI_CHILD_FLAG;
		}
	}
	

	Ui_Element inner;
	inner.type = UI_CHECKBOX;
	inner.vertex.size[0] = size[0] / 2;
	inner.vertex.size[1] = size[1] / 2;
	inner.vertex.position[0] = pos[0] + (size[0] - inner.vertex.size[0]) / 2;
	inner.vertex.position[1] = pos[1] + (size[1] - inner.vertex.size[1]) / 2;
	inner.vertex.color[0] = unchecked_color.r;
	inner.vertex.color[1] = unchecked_color.g;
	inner.vertex.color[2] = unchecked_color.b;
	inner.vertex.color[3] = unchecked_color.a;
	inner.relation_flags = relation_flags;
	inner.id = global_ui_element_manager.count + 1;
	inner.padding = (vec2f){0};
	ui_element_state_push(inner.id);
//	bool current_state = ui_element_get_switch_state_on_id(inner.id);





	if(global_ui_element_manager.event->mouse_pos.x >= inner.vertex.position[0] && global_ui_element_manager.event->mouse_pos.x <= inner.vertex.position[0] + 
		inner.vertex.size[0]
	&& global_ui_element_manager.event->mouse_pos.y >= inner.vertex.position[1] && global_ui_element_manager.event->mouse_pos.y <= inner.vertex.position[1] + 
		inner.vertex.size[1])
	{
		DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_INFO, "Mouse is within the bounds of the button!");

		if(dyl_event_key_handle(global_ui_element_manager.event, DYL_MOUSE_KEY_LBUTTON, DYL_MOUSE_KEY_LPRESS))
		{
			DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_INFO, "Button has been pressed");
			*state = !(*state);
            ui_element_set_switch_state_on_id(inner.id, *state);
			global_ui_element_manager.current_focus_panel = -1;
			
		}

	}

	if(*state)
	{
		inner.vertex.color[0] = checked_color.r;
		inner.vertex.color[1] = checked_color.g;
		inner.vertex.color[2] = checked_color.b;
		inner.vertex.color[3] = checked_color.a;
	}else{
		inner.vertex.color[0] = unchecked_color.r;
		inner.vertex.color[1] = unchecked_color.g;
		inner.vertex.color[2] = unchecked_color.b;
		inner.vertex.color[3] = unchecked_color.a;
	}

	global_ui_element_manager.elements[global_ui_element_manager.count] = inner;
	global_ui_element_manager.count++;
	ui_element_rect_start(position, size, default_color);
	DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_INFO, "STATE IS %d", *state );






}
void ui_element_checkbox_end()
{

	ui_element_rect_end();
	if(global_ui_element_manager.elements[global_ui_element_manager.count - 1].type != UI_CHECKBOX)
		return;

	vertices_push(&global_ui_element_manager.vertices, global_ui_element_manager.elements[global_ui_element_manager.count - 1].vertex);
	global_ui_element_manager.count--;
	global_ui_element_manager.current_panel_ui_element_count--;


}
void ui_element_set_padding(vec2f pad)
{
	if(global_ui_element_manager.elements[global_ui_element_manager.count - 1].id == 0)
		return;
	global_ui_element_manager.elements[global_ui_element_manager.count - 1].padding = pad;

}
void ui_element_set_margin(vec2f margin)
{
	if(global_ui_element_manager.elements[global_ui_element_manager.count - 1].id == 0)
		return;
	global_ui_element_manager.elements[global_ui_element_manager.count - 1].margin = margin;

}




void ui_element_render()
{

//	dyl_batch_renderer_set_proj2d(global_ui_element_manager.renderer, &global_ui_element_manager.projection);
//	dyl_batch_renderer_set_shader_tag(global_ui_element_manager.renderer, SHADER_UI);
	//
	global_ui_element_manager.renderer->is_ui = true;
	global_ui_element_manager.renderer->is_rounded_rect = true;
	for(u64 idx = 0; idx < global_ui_element_manager.vertices.vertex_count; ++idx)
	{
	//	global_ui_element_manager.renderer->is_ui = true;
	/*	if(global_ui_element_manager.elements[idx].type == UI_PANEL)
			global_ui_element_manager.renderer->is_rounded_rect = true;
		else
			global_ui_element_manager.renderer->is_rounded_rect = false;*/
		db_rectangle_draw(global_ui_element_manager.renderer, (vec2){global_ui_element_manager.vertices.vertices[idx].position[0], 
			global_ui_element_manager.vertices.vertices[idx].position[1]},
					(vec2){global_ui_element_manager.vertices.vertices[idx].size[0], global_ui_element_manager.vertices.vertices[idx].size[1]}, 0.0, (vec4){global_ui_element_manager.vertices.vertices[idx].color[0], 
					global_ui_element_manager.vertices.vertices[idx].color[1], global_ui_element_manager.vertices.vertices[idx].color[2]
					,global_ui_element_manager.vertices.vertices[idx].color[3]});

	}
	db_flush(global_ui_element_manager.renderer);
	u64 left_over_text_count = global_ui_element_manager.count;
	for(u64 idx = 0; idx < global_ui_element_manager.text_element_count; ++idx)
	{
	//	global_ui_element_manager.renderer->is_ui = true;
			render_text(&global_ui_element_manager.font_renderer, global_ui_element_manager.text_elements[idx].text.string_data, 
			 global_ui_element_manager.text_elements[idx].vertex.position[0], 
			 global_ui_element_manager.text_elements[idx].vertex.position[1],
				global_ui_element_manager.text_elements[idx].scale, (vec3){global_ui_element_manager.text_elements[idx].vertex.color[0],
						global_ui_element_manager.text_elements[idx].vertex.color[1],
						global_ui_element_manager.text_elements[idx].vertex.color[2],
			 });
		

				
	}

	global_ui_element_manager.vertices.vertices[global_ui_element_manager.vertices.vertex_count] = (Vertex){0};
	global_ui_element_manager.vertices.vertex_count = 0;
	global_ui_element_manager.count = 0;
	global_ui_element_manager.text_element_count = 0;
//	global_ui_element_manager.element_state_count = 0;
	global_ui_element_manager.current_panel_ui_element_count = 0;

}




