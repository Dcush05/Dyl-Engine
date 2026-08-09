#include "editor_ui.h"
#include "../core/Scene_Manager.h"
#include "../core/dyl_debug.h"




Editor editor_create(Entity_Manager* manager)
{
	Editor editor;
	editor.entities = manager;
	editor.interface = (Entity_Interface){0};
	editor.interface.position = (vec3f){0};
	editor.interface.size = (vec3f){0};
	editor.arena = arena_alloc(sizeof(Dyl_Str) * 100000);
	return editor;
}


#define EDITOR_TEXT_SCALE 0.20

#define EDITOR_PLUS_COLOR (Color){128,255,128,255}
#define EDITOR_MINUS_COLOR (Color){255,128,128,255}


void editor_events(Editor* editor)
{

}
void editor_construct(Editor* editor)
{
		
		ui_element_panel_start((vec2){100,250}, (vec2){256, 512}, (Color){256,256,256,100}, UI_PANEL_NAME_FLAG | UI_PANEL_MOVABLE_FLAG);

			ui_element_text_start("Entity Add", (vec2){0,0}, 0.3, (Color){0,0,0,255});
			if(ui_element_button_start((vec2){15, 100}, 
						   (vec2){32,32}, (Color){0,255,0,255}, 
						   (Color){255,0,0,255}))
			{
				Scene_Id scene = global_scene_manager_get_current_active_scene();
				entity_id id = global_scene_manager_entity_actor_create(scene, editor->interface.position, editor->interface.size, editor->interface.color, editor->interface.has_texture, editor->interface.is_model);

				if(id == INVALID_ENTITY)
				{
					DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_ERROR, "Unable to create entity");	
				}else{
				//	entity_set_model_from_id(editor->entities, id, global_asset_manager_get_from_name("aircraft"));
					//asset_create("","assets/Obj/Japanese_Maple/Japanese_Maple.obj", "assets/Obj/Japanese_Maple", ASSET_MODEL_OBJ);

					global_scene_manager_entity_manager_set_model_from_id(scene, id,global_asset_manager_get_from_name("aircraft"));
					//entity_initialize_all_models(editor->entities);
			//					global_scene_manager_entity_manager_initialize_all_models(scene);
					global_scene_manager_entity_manager_initialize_model_from_id(scene, id);

					DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_DEBUG, "Created entity from ui!");	

				}	
				editor->interface = (Entity_Interface){0};

					
			}

			
				ui_element_set_padding((vec2f){0,-25});
				ui_element_text_start("Create entity", (vec2){100,100},EDITOR_TEXT_SCALE ,(Color){255,0,0,255});
			
			ui_element_button_end();

			ui_element_checkbox_start((vec2){15, 140}, (vec2){32,32},
							 (Color){0,255,255,255}, (Color){255,0,0,255}, (Color){128,128,128,255}, &editor->interface.is_model);

				ui_element_set_padding(INTERFACE_TEXT_PADDING);
				ui_element_text_start("Model", (vec2){100,100}, EDITOR_TEXT_SCALE, (Color){255,0,0,255});

			ui_element_checkbox_end();


			ui_element_checkbox_start((vec2){15, 200}, (vec2){32,32},
							 (Color){0,255,255,255}, (Color){255,0,0,255}, (Color){128,128,128,255}, &editor->interface.has_texture);

				ui_element_set_padding(INTERFACE_TEXT_PADDING);
				ui_element_text_start("Texture", (vec2){100,100},EDITOR_TEXT_SCALE, (Color){255,0,0,255});

			ui_element_checkbox_end();

			//---POSITION SET UI---


			//Temp_Arena arena = temp_arena_scratch_get(NULL, 0, sizeof(Dyl_Str) * 1000);
			Dyl_Str text = {0};
			if(ui_element_button_start((vec2){15, 280}, (vec2){32,32}, EDITOR_PLUS_COLOR, (Color){255,255,255,255}))
			{
				editor->interface.position.x += 0.01;
				DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_DEBUG, "YAY BUTTON HAS BEEN PRESSED");	

			}

				Dyl_Str text_x = dyl_str_lit_fmt(&editor->arena, "Pos x: %.2f", editor->interface.position.x);
				ui_element_set_padding((vec2f){0, -20});
				ui_element_text_start((char*)text_x.string_data, (vec2){0,0}, EDITOR_TEXT_SCALE, (Color){255,255,255,255});
			ui_element_button_end();


			if(ui_element_button_start((vec2){90, 280}, (vec2){32,32}, EDITOR_PLUS_COLOR, (Color){255,255,255,255}))
			{
				editor->interface.position.y += 0.01;
				DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_DEBUG, "YAY BUTTON HAS BEEN PRESSED");	
			}
				Dyl_Str text_y = dyl_str_lit_fmt(&editor->arena, "Pos y: %.2f", editor->interface.position.y);
				ui_element_set_padding((vec2f){0, -20});
				ui_element_text_start((char*)text_y.string_data, (vec2){0,0}, EDITOR_TEXT_SCALE, (Color){255,255,255,255});
			ui_element_button_end();

			if(ui_element_button_start((vec2){175, 280}, (vec2){32,32}, EDITOR_PLUS_COLOR, (Color){255,255,255,255}))
			{
				editor->interface.position.z += 0.01;

				DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_DEBUG, "YAY BUTTON HAS BEEN PRESSED");	
			}
				Dyl_Str text_z = dyl_str_lit_fmt(&editor->arena, "Pos z: %.2f", editor->interface.position.z);
				ui_element_set_padding((vec2f){0, -20});
				ui_element_text_start((char*)text_z.string_data, (vec2){0,0},EDITOR_TEXT_SCALE,  (Color){255,255,255,255});
			ui_element_button_end();
			if(ui_element_button_start((vec2){15, 312}, (vec2){32,32}, EDITOR_MINUS_COLOR, (Color){255,255,255,255}))
			{
				editor->interface.position.x -= 0.01;

				DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_DEBUG, "YAY BUTTON HAS BEEN PRESSED");	
			}
			ui_element_button_end();
			if(ui_element_button_start((vec2){90, 312}, (vec2){32,32}, EDITOR_MINUS_COLOR, (Color){255,255,255,255}))
			{
				editor->interface.position.y -= 0.01;

				DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_DEBUG, "YAY BUTTON HAS BEEN PRESSED");	
			}
			ui_element_button_end();
			if(ui_element_button_start((vec2){175, 312}, (vec2){32,32}, EDITOR_MINUS_COLOR, (Color){255,255,255,255}))
			{
				editor->interface.position.z -= 0.01;

				DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_DEBUG, "YAY BUTTON HAS BEEN PRESSED");	
			}
			ui_element_button_end();
			


			

			//---COLOR SET UI---
			if(ui_element_button_start((vec2){15, 375}, (vec2){25,25}, EDITOR_PLUS_COLOR, (Color){255,255,255,255}))
			{
				editor->interface.color.r += 1;
				if(editor->interface.color.r >= 255)
					editor->interface.color.g = 255;


				DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_DEBUG, "YAY BUTTON HAS BEEN PRESSED");	

			}

				Dyl_Str r = dyl_str_lit_fmt(&editor->arena, "r: %d", editor->interface.color.r);
				ui_element_set_padding((vec2f){0, -20});
				ui_element_text_start((char*)r.string_data, (vec2){0,0}, EDITOR_TEXT_SCALE, (Color){255,0,0,255});
			ui_element_button_end();


			if(ui_element_button_start((vec2){55, 375}, (vec2){25,25}, EDITOR_PLUS_COLOR, (Color){255,255,255,255}))
			{
				editor->interface.color.g += 1;
				if(editor->interface.color.g >= 255)
					editor->interface.color.g = 255;

				DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_DEBUG, "YAY BUTTON HAS BEEN PRESSED");	
			}
				Dyl_Str g = dyl_str_lit_fmt(&editor->arena, "g: %d", editor->interface.color.g);
				ui_element_set_padding((vec2f){0, -20});
				ui_element_text_start((char*)g.string_data, (vec2){0,0}, EDITOR_TEXT_SCALE, (Color){0,255,0,255});
			ui_element_button_end();

			if(ui_element_button_start((vec2){95, 375}, (vec2){25,25}, EDITOR_PLUS_COLOR, (Color){255,255,255,255}))
			{
				editor->interface.color.b += 1;
				if(editor->interface.color.b >= 255)
					editor->interface.color.b = 255;



				DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_DEBUG, "YAY BUTTON HAS BEEN PRESSED");	
			}
				Dyl_Str b = dyl_str_lit_fmt(&editor->arena, "b: %d", editor->interface.color.b);
				ui_element_set_padding((vec2f){0, -20});
				ui_element_text_start((char*)b.string_data, (vec2){0,0},EDITOR_TEXT_SCALE,  (Color){0,0,255,255});
			ui_element_button_end();
			if(ui_element_button_start((vec2){135, 375}, (vec2){25,25}, EDITOR_PLUS_COLOR, (Color){255,255,255,255}))
			{
				editor->interface.color.a += 1;
				if(editor->interface.color.a >= 255)
					editor->interface.color.a = 255;



				DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_DEBUG, "YAY BUTTON HAS BEEN PRESSED");	
			}
				Dyl_Str a = dyl_str_lit_fmt(&editor->arena, "a: %d", editor->interface.color.a);
				ui_element_set_padding((vec2f){0, -20});
				ui_element_text_start((char*)a.string_data, (vec2){0,0},EDITOR_TEXT_SCALE,  (Color){255,255,255,255});
			ui_element_button_end();





			if(ui_element_button_start((vec2){15, 400}, (vec2){25,25}, EDITOR_MINUS_COLOR, (Color){255,255,255,255}))
			{
				editor->interface.color.r -= 1;
				if(editor->interface.color.r <= 0)
					editor->interface.color.r = 0;



				DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_DEBUG, "YAY BUTTON HAS BEEN PRESSED");	
			}
			ui_element_button_end();
			if(ui_element_button_start((vec2){55, 400}, (vec2){25,25}, EDITOR_MINUS_COLOR, (Color){255,255,255,255}))
			{
				editor->interface.color.g -= 1;
				if(editor->interface.color.g <= 0)
					editor->interface.color.g = 0;



				DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_DEBUG, "YAY BUTTON HAS BEEN PRESSED");	
			}
			ui_element_button_end();
			if(ui_element_button_start((vec2){95, 400}, (vec2){25,25}, EDITOR_MINUS_COLOR, (Color){255,255,255,255}))
			{

				editor->interface.color.b -= 1;
				if(editor->interface.color.b <= 0)
					editor->interface.color.b = 0;



				DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_DEBUG, "YAY BUTTON HAS BEEN PRESSED");	
			}
			ui_element_button_end();
			if(ui_element_button_start((vec2){135, 400}, (vec2){25,25}, EDITOR_MINUS_COLOR, (Color){255,255,255,255}))
			{

				editor->interface.color.a -= 1;
				if(editor->interface.color.a <= 0)
					editor->interface.color.a = 0;

				DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_DEBUG, "YAY BUTTON HAS BEEN PRESSED");	
			}
			ui_element_button_end();
			
			if(ui_element_button_start((vec2){15, 450}, (vec2){25,25}, EDITOR_PLUS_COLOR, (Color){255,255,255,255}))
			{
				editor->interface.size.x += 0.1;
				if(editor->interface.size.x >= 1.0)
					editor->interface.size.x = 1.0;


				DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_DEBUG, "YAY BUTTON HAS BEEN PRESSED");	

			}

				Dyl_Str x_size = dyl_str_lit_fmt(&editor->arena, "xs: %.2f", editor->interface.size.x);
				ui_element_set_padding((vec2f){0, -20});
				ui_element_text_start((char*)x_size.string_data, (vec2){0,0}, EDITOR_TEXT_SCALE, (Color){255,255,255,255});
			ui_element_button_end();


			if(ui_element_button_start((vec2){75, 450}, (vec2){25,25}, EDITOR_PLUS_COLOR, (Color){255,255,255,255}))
			{
				editor->interface.size.y += 0.01;
				if(editor->interface.size.y >= 1.0)
					editor->interface.size.y = 1.0;




				DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_DEBUG, "YAY BUTTON HAS BEEN PRESSED");	
			}
				Dyl_Str y_size = dyl_str_lit_fmt(&editor->arena, "ys: %.2f", editor->interface.size.y);
				ui_element_set_padding((vec2f){0, -20});
				ui_element_text_start((char*)y_size.string_data, (vec2){0,0}, EDITOR_TEXT_SCALE, (Color){255,255,255,255});
			ui_element_button_end();

			if(ui_element_button_start((vec2){135, 450}, (vec2){25,25}, EDITOR_PLUS_COLOR, (Color){255,255,255,255}))
			{
				editor->interface.size.z += 0.01;
				if(editor->interface.size.z >= 1.0)
					editor->interface.size.z = 1.0;



				DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_DEBUG, "YAY BUTTON HAS BEEN PRESSED");	
			}
				Dyl_Str z_size = dyl_str_lit_fmt(&editor->arena, "zs: %.2f", editor->interface.size.z);
				ui_element_set_padding((vec2f){0, -20});
				ui_element_text_start((char*)z_size.string_data, (vec2){0,0},EDITOR_TEXT_SCALE,  (Color){255,255,255,255});
			ui_element_button_end();
			if(ui_element_button_start((vec2){15, 475}, (vec2){25,25}, EDITOR_MINUS_COLOR, (Color){255,255,255,255}))
			{
				editor->interface.size.x -= 0.1;
				if(editor->interface.size.x <= 0.1)
					editor->interface.size.x = 0.1;

				DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_DEBUG, "YAY BUTTON HAS BEEN PRESSED");	
			}
			ui_element_button_end();
			if(ui_element_button_start((vec2){75, 475}, (vec2){25,25}, EDITOR_MINUS_COLOR, (Color){255,255,255,255}))
			{
				editor->interface.size.y -= 0.1;
				if(editor->interface.size.y <= 0.1)
					editor->interface.size.y = 0.1;



				DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_DEBUG, "YAY BUTTON HAS BEEN PRESSED");	
			}
			ui_element_button_end();
			if(ui_element_button_start((vec2){135, 475}, (vec2){100,100}, EDITOR_MINUS_COLOR, (Color){255,255,255,255}))
			{

				editor->interface.size.z -= 0.1;
				if(editor->interface.size.z <= 0.1)
					editor->interface.size.z = 0.1;



				DYL_ENGINE_PRINT_LOG(DYL_ENGINE_LOG_DEBUG, "YAY BUTTON HAS BEEN PRESSED");	
			}
			ui_element_button_end();





			

			
		ui_element_panel_end();	
		ui_element_render();
		arena_reset(&editor->arena);

}


