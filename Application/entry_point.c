#include "app.c"

#ifdef _WIN32
#pragma message("Compiling for x86 Windows systems")
#define entry_point int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) 
#include <consoleapi.h>


void open_console(void)
{

	if (AttachConsole(ATTACH_PARENT_PROCESS))
    {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
        freopen("CONIN$", "r", stdin);

        printf("Attached to parent console\n");
    }
}

#else
#pragma message("Compiling for x86 non Windows systems")
#define entry_point int main()
#endif


entry_point
{
	Engine engine;


	#ifdef _WIN32
		engine.platform.h_instance = hInstance; 
		engine.platform.h_prev_instance = hPrevInstance;
		engine.platform.lp_cmd_line = lpCmdLine;
		engine.platform.cmd_show = nCmdShow;
	#endif
	open_console();
	Dyl_App_Print_Log("Hello world i am %d\n", 5);
	engine_initialize(&engine);
	engine_run(&engine, entity_scene_initializer, frame_scene_callback, event_scene_callback);
	engine_shutdown(&engine);

}
