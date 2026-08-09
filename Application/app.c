#include "Dyl_Engine.h"
//Entity_View view;
entity_id entity;
entity_id new_entity;
entity_id tree_entity;

entity_id tree_entity1;

entity_id tree_entity2;

entity_id tree_entity3;

entity_id tree_entity4;

entity_id tree_entity5;
vec3f entity_pos;


u64 scene_id1;
void entity_scene_initializer(Engine* engine)
{
//	view = entity_get_from_id(&engine->manager,entity_shape_create(&engine->manager, (Shape_Params){.position = {100,100}, .size = {32, 32}, .color = {255,0,255,255}}));
/*	for(int rows = 0; rows < 2; ++rows)
	{
		for(int columns = 0; columns < 2; ++columns)
		{
			entity_shape_create(&engine->manager, (Shape_Params){.position2f = {100 * columns + 16,100 * rows + 16}, .size2f = {32, 32}, .color = {255,0,255,255}});
		}
	}*/

	//entity_sky_box_create(&engine->manager, "assets/right.jpg", "assets/left.jpg", "assets/top.jpg", "assets/bottom.jpg", "assets/front.jpg", "assets/back.jpg");
//	entity_pos = (vec3f){0.5, 0.5, 3.5};


  //  entity = entity_actor_create(&engine->manager, entity_pos, (vec3f){1.0,1.0,1.0}, (Color){255,255,255,255}, false, true);
	//entity_set_model_from_id(&engine->manager, entity, global_asset_manager_get_from_name("aircraft"));

   /* new_entity = entity_actor_create(&engine->manager, (vec3f){0.5, 0.5, 8.0}, (vec3f){1.0,1.0,1.0}, (Color){255,255,255,255}, false, true);
	entity_set_model_from_id(&engine->manager, new_entity, "assets/Obj/E-45-Aircraft/Aircraft.obj", "assets/Obj/E-45-Aircraft");

	tree_entity = entity_actor_create(&engine->manager, (vec3f){0.5, 0.5, 0.0}, (vec3f){1.0,1.0,1.0}, (Color){255,255,255,255}, false, true);
	entity_set_model_from_id(&engine->manager, tree_entity, "assets/Obj/Japanese_Maple/Japanese_Maple.obj", "assets/Obj/Japanese_Maple");*/

	/*tree_entity1 = entity_actor_create(&engine->manager, (vec3f){0.8, 0.5, 0.0}, (vec3f){1.0,1.0,1.0}, (Color){255,255,255,255}, false, true);
	entity_set_model_from_id(&engine->manager, tree_entity1, "assets/Obj/Japanese_Maple/Japanese_Maple.obj", "assets/Obj/Japanese_Maple");

	tree_entity2 = entity_actor_create(&engine->manager, (vec3f){1.1, 0.5, 0.0}, (vec3f){1.0,1.0,1.0}, (Color){255,255,255,255}, false, true);
	entity_set_model_from_id(&engine->manager, tree_entity2, "assets/Obj/Japanese_Maple/Japanese_Maple.obj", "assets/Obj/Japanese_Maple");


	tree_entity3 = entity_actor_create(&engine->manager, (vec3f){1.4, 0.5, 0.0}, (vec3f){1.0,1.0,1.0}, (Color){255,255,255,255}, false, true);
	entity_set_model_from_id(&engine->manager, tree_entity3, "assets/Obj/Japanese_Maple/Japanese_Maple.obj", "assets/Obj/Japanese_Maple");

	tree_entity4 = entity_actor_create(&engine->manager, (vec3f){1.7, 0.5, 0.0}, (vec3f){1.0,1.0,1.0}, (Color){255,255,255,255}, false, true);
	entity_set_model_from_id(&engine->manager, tree_entity4, "assets/Obj/Japanese_Maple/Japanese_Maple.obj", "assets/Obj/Japanese_Maple");*/












	//new_entity = entity_actor_create(&engine->manager, (vec3f){0.5, 0.5, 2.0}, (vec3f){1.0,1.0,1.0}, (Color){255,255,255,255}, false, true);
	//entity_set_model_from_id(&engine->manager, new_entity, "assets/Obj/Japanese_Maple/Japanese_Maple.obj", "assets/Obj/Japanese_Maple");





	scene_id1 = global_scene_manager_add("main_scene", NULL, SCENE_GAMEPLAY);
	global_scene_manager_entity_manager_initialize(scene_id1);
	entity_pos = (vec3f){0.5, 0.5, 3.5};
	new_entity = global_scene_manager_entity_actor_create(scene_id1, entity_pos, (vec3f){1.0,1.0,1.0}, (Color){255,255,255,255}, false, true);
	asset_create("aircraft","assets/Obj/E-45-Aircraft/Aircraft.obj", "assets/Obj/E-45-Aircraft", ASSET_MODEL_OBJ);
	global_scene_manager_entity_manager_set_model_from_id(scene_id1,new_entity,global_asset_manager_get_from_name("aircraft"));


	//asset_create("aircraft","assets/Obj/E-45-Aircraft/Aircraft.obj", "assets/Obj/E-45-Aircraft", ASSET_MODEL_OBJ);

	global_scene_manager_entity_sky_box_create(scene_id1, "assets/right.jpg", "assets/left.jpg", "assets/top.jpg", "assets/bottom.jpg", "assets/front.jpg", "assets/back.jpg");


	/*asset_create("tree","assets/Obj/Japanese_Maple/Japanese_Maple.obj", "assets/Obj/Japanese_Maple", ASSET_MODEL_OBJ);

	tree_entity = global_scene_manager_entity_actor_create(scene_id1, entity_pos, (vec3f){1.0,1.0,1.0}, (Color){255,255,255,255}, false, true);
	global_scene_manager_entity_manager_set_model_from_id(scene_id1,tree_entity,global_asset_manager_get_from_name("tree"));*/

	global_scene_manager_make_active(scene_id1);
	global_scene_manager_entity_manager_initialize_all_models(scene_id1);


	global_scene_manager_camera_initialization(scene_id1, (vec3){0.5, 3.5, 8.0},
											true, engine->window->height, engine->window->width);


	
}


void frame_scene_callback(Engine* engine)
{
	//draw_shape2D(engine->renderer, SHAPE_RECT, .position = {100,100}, .size = {64, 64}, .color = {255,255,255,255});
	//entity_render_from_view(engine->renderer, &view);
	//entity_manager_render(engine->renderer, &engine->manager);


//	draw_shape3D(engine->renderer, SHAPE_CUBE, .position3f = {0,0,-10}, .size3f = {32,32,32}, .color = {255,0,0,255});
	
//	printf("Hello im dylan\n");
	global_scene_manager_update_by_id(scene_id1, engine->delta_time);
	global_scene_manager_render_by_id(scene_id1);
	
}

void event_scene_callback(Engine* engine)
{
	//printf("Input event stuff here plz\n");
	global_scene_manager_camera_move_by_id(scene_id1, engine->event);
	//TEST:

	if(dyl_event_key_handle(engine->event, DYLKEY_G, DYL_KEY_PRESSED))
	{
		entity_pos.z -= 1;

		
		entity_set_position3f_from_id(global_scene_manager_get_entity_manager(scene_id1), entity, entity_pos);
	}
}





