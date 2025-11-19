#include "dyl_events.h"




void dyl_event_initalize(Dyl_Event* event)
{
	ASSERT(event, "Passing Null event through func");
	for(size_t i = 0; i < DYL_KEY_AMOUNT; ++i)
	{
		event->keys[i].key_state = DYL_KEY_RELEASED;
	}
	int key_pos = 0;
	event->keys[key_pos].key_type = DYLKEY_NONE;
	key_pos++;
	event->keys[key_pos].key_type = DYLKEY_A;
	key_pos++;
	event->keys[key_pos].key_type = DYLKEY_B;
	key_pos++;
	event->keys[key_pos].key_type = DYLKEY_C;
	key_pos++;
	event->keys[key_pos].key_type = DYLKEY_D;
	key_pos++;
	event->keys[key_pos].key_type = DYLKEY_E;
	key_pos++;
	event->keys[key_pos].key_type = DYLKEY_F;
	key_pos++;
	event->keys[key_pos].key_type = DYLKEY_G;
	key_pos++;
	event->keys[key_pos].key_type = DYLKEY_H;
	key_pos++;
	event->keys[key_pos].key_type = DYLKEY_I;
	key_pos++;
	event->keys[key_pos].key_type = DYLKEY_J;
	key_pos++;
	event->keys[key_pos].key_type = DYLKEY_K;
	key_pos++;
	event->keys[key_pos].key_type = DYLKEY_L;
	key_pos++;
	event->keys[key_pos].key_type = DYLKEY_M;
	key_pos++;
	event->keys[key_pos].key_type = DYLKEY_N;
	key_pos++;
	event->keys[key_pos].key_type = DYLKEY_O;
	key_pos++;
	event->keys[key_pos].key_type = DYLKEY_P;
	key_pos++;
	event->keys[key_pos].key_type = DYLKEY_Q;
	key_pos++;
	event->keys[key_pos].key_type = DYLKEY_R;
	key_pos++;
	event->keys[key_pos].key_type = DYLKEY_T;
	key_pos++;
	event->keys[key_pos].key_type = DYLKEY_U;
	key_pos++;
	event->keys[key_pos].key_type = DYLKEY_V;
	key_pos++;
	event->keys[key_pos].key_type = DYLKEY_W;
	key_pos++;
	event->keys[key_pos].key_type = DYLKEY_X;
	key_pos++;
	event->keys[key_pos].key_type = DYLKEY_Y;
	key_pos++;
	event->keys[key_pos].key_type = DYLKEY_Z;
	key_pos++;
}
int dyl_event_poll(Dyl_Event* event)
{
	ASSERT(event, "Passing Null event through func");
	#ifdef USING_SDL
		return SDL_PollEvent(&event->event);
	#else
		return 1;
	#endif
}


uint8_t dyl_key_translate(Dyl_Key_Type type)
{
	#ifdef USING_SDL
	switch(type)
	{
		case DYLKEY_A:
			return SDL_SCANCODE_A;
		break;
		case DYLKEY_X:
			return SDL_SCANCODE_X;
		break;
	}
	#else
		return 1;
	#endif
		
}

uint8_t dyl_key_state_translate(Dyl_Key_State state)
{
	#ifdef USING_SDL
	switch(state)
	{
		case DYL_KEY_PRESSED:
			return SDL_EVENT_KEY_DOWN;
		break;
		case DYL_KEY_RELEASED:
			return SDL_EVENT_KEY_UP;
		break;
	}
	#else
		return 1;
	#endif
		
}






bool dyl_event_key_press(Dyl_Event* event, Dyl_Key_Type type, Dyl_Key_State state)
{
	ASSERT(event, "Passing NULL event through func");

	bool is_pressed = false;
	#ifdef USING_SDL
		if((uint8_t)event->event.type == dyl_key_state_translate(state))
		{
			if((uint8_t)event->event.key.scancode == dyl_key_translate(type))	
			{
				event->keys[type].key_state = DYL_KEY_PRESSED;
				is_pressed = true;
			}
		}
		 	
	#else
	#endif
	return is_pressed;
}
