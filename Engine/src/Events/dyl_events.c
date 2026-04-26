#include "dyl_events.h"
#include <winuser.h>

#ifdef _WIN32
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param)
	{
		switch(u_msg)
		{
			case WM_CLOSE:
				PostQuitMessage(0);
			break;
			default:
				return DefWindowProc(hwnd, u_msg, w_param, l_param);
				
		}
		return 0;
	}
#endif

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
	#ifdef _WIN32
		return PeekMessage(&event->event, NULL, 0, 0, PM_REMOVE);
	#elif USING_SDL
		return SDL_PollEvent(&event->event);
	#endif
}


uint8_t dyl_key_translate(Dyl_Key_Type type)
{
	#ifdef _WIN32
		printf("No key\n");
	
	#elif USING_SDL
		switch(type)
		{
			case DYLKEY_A:
				return SDL_SCANCODE_A;
			break;
			case DYLKEY_B:
				return SDL_SCANCODE_B;
			break;
			case DYLKEY_C:
				return SDL_SCANCODE_C;
			break;
			case DYLKEY_D:
				return SDL_SCANCODE_D;
			break;
			case DYLKEY_E:
				return SDL_SCANCODE_E;
			break;
			case DYLKEY_F:
				return SDL_SCANCODE_F;
			break;
			case DYLKEY_G:
				return SDL_SCANCODE_G;
			break;
			case DYLKEY_H:
				return SDL_SCANCODE_H;
			break;
			case DYLKEY_I:
				return SDL_SCANCODE_I;
			break;
			case DYLKEY_J:
				return SDL_SCANCODE_J;
			break;
			case DYLKEY_K:
				return SDL_SCANCODE_K;
			break;
			case DYLKEY_L:
				return SDL_SCANCODE_L;
			break;
			case DYLKEY_M:
				return SDL_SCANCODE_M;
			break;
			case DYLKEY_N:
				return SDL_SCANCODE_N;
			break;
			case DYLKEY_O:
				return SDL_SCANCODE_O;
			break;
			case DYLKEY_P:
				return SDL_SCANCODE_P;
			break;
			case DYLKEY_Q:
				return SDL_SCANCODE_Q;
			break;
			case DYLKEY_R:
				return SDL_SCANCODE_R;
			break;
			case DYLKEY_S:
				return SDL_SCANCODE_S;
			break;
			case DYLKEY_T:
				return SDL_SCANCODE_T;
			break;
			case DYLKEY_U:
				return SDL_SCANCODE_U;
			break;
			case DYLKEY_V:
				return SDL_SCANCODE_V;
			break;
			case DYLKEY_W:
				return SDL_SCANCODE_W;
			break;
			case DYLKEY_X:
				return SDL_SCANCODE_X;
			break;
			case DYLKEY_Y:
				return SDL_SCANCODE_Y;
			break;
			case DYLKEY_Z:
				return SDL_SCANCODE_Z;
			break;
		}
	#endif
		
}

uint8_t dyl_event_state_translate(Dyl_Event_State state)
{
	#ifdef _WIN32
		switch(state)
		{
			case DYL_SYS_QUIT:
				return WM_QUIT;
			break;
		}

	#else
		switch(state)
		{
			case DYL_KEY_PRESSED:
				return SDL_EVENT_KEY_DOWN;
			break;
			case DYL_KEY_RELEASED:
				return SDL_EVENT_KEY_UP;
			case DYL_SYS_QUIT: 
				return SDL_EVENT_QUIT;
			break;
		}
	#endif
		
}


bool dyl_event_window_dispatch(Dyl_Event* event, Dyl_Event_State state) //NOTE:@Dylan - adjust this to return states and have the window representing the states
{

	ASSERT(event, "Passing NULL event through func");
	bool is_pressed = false;
	#ifdef _WIN32
		if(event->event.message == dyl_event_state_translate(state))
		{
			is_pressed = true;
		}
	#else
	if((uint8_t)event->event.type == dyl_event_state_translate(state))
	{
			if(event->event.type == SDL_EVENT_QUIT)
				is_pressed = true;
	}
	#endif
	return is_pressed;

}



bool dyl_event_key_press(Dyl_Event* event, Dyl_Key_Type type, Dyl_Event_State state)
{
	ASSERT(event, "Passing NULL event through func");

	bool is_pressed = false;
	#ifdef _WIN32
		
		 	
	#elif USING_SDL
		if((uint8_t)event->event.type == dyl_event_state_translate(state))
		{
			if((uint8_t)event->event.key.scancode == dyl_key_translate(type))	
			{
				event->keys[type].key_state = DYL_KEY_PRESSED;
				is_pressed = true;
			}
		}
	#endif
	return is_pressed;
}


void dyl_event_end(Dyl_Event* event)
{
	#ifdef _WIN32
		TranslateMessage(&event->event);
		DispatchMessage(&event->event);
	#elif USING_SDL
		return;
	#endif
}
