#include "platform.h"
#include <profileapi.h>


void platform_set_data(Arena* arena, Platform* platform)
{
	platform->os_str = dyl_str_lit_fmt(arena, "Operating System: %s\n", OS_TAG);
	platform->window_event_str = dyl_str_lit_fmt(arena, "Window and Event handling: %s\n", WINDOW_EVENT_TAG);
	platform->graphics_driver_str = dyl_str_lit_fmt(arena, "Graphics Driver: %s\n", glGetString(GL_VERSION));
}

void platform_set_os_performance_frequency(Platform* platform)
{
	#ifdef _WIN32
		QueryPerformanceFrequency(&platform->frequency);
	#else
		printf("Not supported by other platforms");
	#endif
}
FREQUENCY_TYPE platform_get_os_performance_frequency(Platform* platform)
{
	return platform->frequency;
}


