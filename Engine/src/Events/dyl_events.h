#ifndef DYL_EVENTS_H
#define DYL_EVENTS_H
#include "../utils/dyl_base.h"


#include <stdbool.h>
#ifdef _WIN32
	#include "../Window/dyl_window.h"	
	#include <windowsx.h>
	typedef MSG Event_Handle;
#else
	#include <SDL3/SDL_events.h>
	typedef SDL_Event Event_Handle;
#endif


typedef enum
{
	DYLKEY_NONE = 0,
	DYLKEY_A,
	DYLKEY_B,
	DYLKEY_C,
	DYLKEY_D,
	DYLKEY_E,
	DYLKEY_F,
	DYLKEY_G,
	DYLKEY_H,
	DYLKEY_I,
	DYLKEY_J,
	DYLKEY_K,
	DYLKEY_L,
	DYLKEY_M,
	DYLKEY_N,
	DYLKEY_O,
	DYLKEY_P,
	DYLKEY_Q,
	DYLKEY_R,
	DYLKEY_S,
	DYLKEY_T,
	DYLKEY_U,
	DYLKEY_V,
	DYLKEY_W,
	DYLKEY_X,
	DYLKEY_Y,
	DYLKEY_Z,
	DYLKEY_SPACE,
	DYLKEY_TAB,
	DYLKEY_LSHIFT,
	DYLKEY_LCTRL,
	DYL_MOUSE_KEY_LBUTTON,
	DYL_MOUSE_KEY_RBUTTON,
	DYL_MOUSE_KEY_MBUTTON,

	DYL_KEY_AMOUNT,

	

}Dyl_Key_Type;

typedef enum
{
	DYL_KEY_PRESSED,
	DYL_KEY_RELEASED,
	DYL_MOUSE_KEY_LPRESS,
	DYL_MOUSE_KEY_RPRESS,
	DYL_MOUSE_KEY_LRELEASED,
	DYL_MOUSE_KEY_RRELEASED,
	DYL_MOUSE_MOVEMENT,
	DYL_SYS_QUIT,
	DYL_SYS_RESIZE,
}Dyl_Event_State;


typedef struct
{
	Dyl_Key_Type key_type;
	Dyl_Event_State key_state;
	bool is_active;
}Dyl_Key;
typedef struct
{

	Dyl_Key keys[DYL_KEY_AMOUNT];
	Event_Handle event;
	vec2i mouse_pos;
	vec2i mouse_delta;

}Dyl_Event;


void dyl_event_initalize(Dyl_Event* event);
int dyl_event_poll(Dyl_Event* event);
bool dyl_event_window_dispatch(Dyl_Event* event, Dyl_Event_State state);
__declspec(dllexport) bool dyl_event_key_handle(Dyl_Event* event, Dyl_Key_Type type, Dyl_Event_State state);
uint8_t dyl_event_state_translate(Dyl_Event_State state);
bool dyl_event_mouse_movement(Dyl_Event* event);
void dyl_event_end(Dyl_Event* event);


#endif
