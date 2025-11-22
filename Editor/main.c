#include "dyl_engine.h"


void frame_scene_callback(Engine* engine)
{
	draw_shape2D(engine->renderer, SHAPE_RECT, .position = {100,100}, .size = {64, 64}, .color = {255,255,255,255});
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
	engine_run(&engine, frame_scene_callback, event_scene_callback);
	engine_shutdown(&engine);

}
