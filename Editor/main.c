#include "dyl_engine.h"

Entity_View view;
void entity_scene_initializer(Engine* engine)
{
	view = entity_get_from_id(&engine->manager,entity_shape_create(&engine->manager, (Shape_Params){.position = {100,100}, .size = {32, 32}, .color = {255,0,255,255}}));
	for(int i = 0; i < 1022; ++i)
	{
		entity_shape_create(&engine->manager, (Shape_Params){.position = {100,100}, .size = {32, 32}, .color = {255,0,255,255}});
	}
	
}


void frame_scene_callback(Engine* engine)
{
	draw_shape2D(engine->renderer, SHAPE_RECT, .position = {100,100}, .size = {64, 64}, .color = {255,255,255,255});
	//entity_render_from_view(engine->renderer, &view);
	entity_manager_render(engine->renderer, &engine->manager);
	
//	printf("Hello im dylan\n");
}

void event_scene_callback(Engine* engine)
{
	//printf("Input event stuff here plz\n");
}
int main(void)
{
	//TTEST
	Engine engine;
	Dyl_App_Print_Log("Hello world i am %d\n", 5);
	engine_initialize(&engine);
	engine_run(&engine, entity_scene_initializer, frame_scene_callback, event_scene_callback);
	engine_shutdown(&engine);

}
