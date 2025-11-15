#ifndef DYL_EVENTS_H
#define DYL_EVENTS_H




#ifdef USING_SDL
	#include <SDL3/SDL_events.h>
	typedef SDL_Event Event_Handle;
#else
	typedef void* Event_Handle;
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
	DYL_KEY_AMOUNT,

	

}Dyl_Key_Type;

typedef enum
{
	DYL_KEY_PRESSED,
	DYL_KEY_RELEASED
}Dyl_Key_State;


typedef struct
{
	Dyl_Key_Type key_type;
	Dyl_Key_State key_state;
}Dyl_Key;
typedef struct
{
	Event_Handle event;
	Dyl_Key keys[DYL_KEY_AMOUNT];
}Dyl_Event;


void dyl_event_initalize(Dyl_Event* event);
int dyl_event_poll(Dyl_Event* event);


#endif
