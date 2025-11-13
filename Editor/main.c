#include "dyl_engine.h"


int main(void)
{
	//TTEST
	Engine engine;
	engine_initialize(&engine);
	engine_run(&engine, NULL);
	engine_shutdown(&engine);

}
