#include "dyl_events.h"

#include "../dyl_lib.h"
#include <SDL3/SDL_events.h>
int dyl_event_poll(Dyl_Event* event)
{
	ASSERT(event, "Passing Null event through func");


	#ifdef USING_SDL
		return SDL_PollEvent(&event->event);
	#else
		return 1;
	#endif
}
