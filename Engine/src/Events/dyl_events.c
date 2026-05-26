#include "dyl_events.h"
#include <stdbool.h>


//TODO: FIX MOUSE INPUT


#ifdef _WIN32

	void hide_and_confine_mouse(HWND hwnd)
	{
		ShowCursor(true);
		RECT rect;
		GetClientRect(hwnd, &rect);
		MapWindowPoints(hwnd, NULL, (POINT*)&rect, 2);
		ClipCursor(&rect);
		SetCapture(hwnd);
	}

	void restore_mouse()
	{
		ShowCursor(true);
		ClipCursor(NULL);
		ReleaseCapture();
	}

	LRESULT CALLBACK WindowProc(HWND hwnd, UINT u_msg, WPARAM w_param, LPARAM l_param)
	{
		switch(u_msg)
		{
			case WM_CLOSE:
				PostQuitMessage(0);
			break;
			case WM_ACTIVATE:
			//	if(LOWORD(w_param) == WA_INACTIVE)
			//		restore_mouse();
		//		else
		//			hide_and_confine_mouse(hwnd);	
			break;
			case WM_LBUTTONDOWN:
		//		SetCapture(hwnd);
			break;
			case WM_LBUTTONUP:
		//		ReleaseCapture();
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
	event->keys[key_pos].key_type = DYL_MOUSE_KEY_LBUTTON;
	key_pos++;
	event->keys[key_pos].key_type = DYL_MOUSE_KEY_RBUTTON;
	key_pos++;
	event->keys[key_pos].key_type = DYL_MOUSE_KEY_MBUTTON;
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
		switch(type)
		{
			case DYLKEY_A:
				return 0x41;
			break;
			case DYLKEY_B:
				return 0x42;
			break;
			case DYLKEY_C:
				return 0x43;
			break;
			case DYLKEY_D:
				return 0x44;
			break;
			case DYLKEY_E:
				return 0x45;
			break;
			case DYLKEY_F:
				return 0x46;
			break;
			case DYLKEY_G:
				return 0x47;
			break;
			case DYLKEY_H:
				return 0x48;
			break;
			case DYLKEY_I:
				return 0x49;
			break;
			case DYLKEY_J:
				return 0x4A;
			break;
			case DYLKEY_K:
				return 0x4B;
			break;
			case DYLKEY_L:
				return 0x4C;
			break;
			case DYLKEY_M:

				return 0x4D;
			break;
			case DYLKEY_N:
				return 0x4E;
			break;
			case DYLKEY_O:
				return 0x4F;
			break;
			case DYLKEY_P:
				return 0x50;
			break;
			case DYLKEY_Q:
				return 0x51;
			break;
			case DYLKEY_R:
				return 0x52;
			break;
			case DYLKEY_S:
				return 0x53;
			break;
			case DYLKEY_T:
				return 0x54;
			break;
			case DYLKEY_U:
				return 0x55;
			break;
			case DYLKEY_V:
				return 0x56;
			break;
			case DYLKEY_W:
				return 0x57;
			break;
			case DYLKEY_X:
				return 0x58;
			break;
			case DYLKEY_Y:
				return 0x59;
			break;
			case DYLKEY_Z:
				return 0x5A;
			break;
			case DYL_MOUSE_KEY_LBUTTON:
				return VK_LBUTTON;
			break;
			case DYL_MOUSE_KEY_RBUTTON:
				return VK_RBUTTON;
			break;
			case DYL_MOUSE_KEY_MBUTTON:
				return VK_RBUTTON;
			break;

		}
	
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
			case DYL_KEY_PRESSED:
				return WM_KEYDOWN;
			case DYL_KEY_RELEASED:
				return WM_KEYUP;
			case DYL_MOUSE_KEY_LPRESS:
				return WM_LBUTTONDOWN;
			case DYL_MOUSE_KEY_RPRESS:
				return WM_RBUTTONDOWN;
			case DYL_MOUSE_KEY_LRELEASED:
				return WM_LBUTTONUP;
			case DYL_MOUSE_KEY_RRELEASED:
				return WM_RBUTTONUP;
			case DYL_MOUSE_MOVEMENT:
				return WM_MOUSEMOVE;
			case DYL_SYS_QUIT:
				return WM_QUIT;
		}

	#else
		switch(state)
		{
			case DYL_KEY_PRESSED:
				return SDL_EVENT_KEY_DOWN;
			case DYL_KEY_RELEASED:
				return SDL_EVENT_KEY_UP;
			case DYL_SYS_QUIT: 
				return SDL_EVENT_QUIT;
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
			if(event->event.message == WM_QUIT)
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

bool dyl_event_key_handle(Dyl_Event* event, Dyl_Key_Type type, Dyl_Event_State state)
{
	ASSERT(event, "Passing NULL event through func");

	bool is_active = false;
	#ifdef _WIN32

		if((uint8_t)event->event.message == dyl_event_state_translate(state))
		{
			if((uint8_t)event->event.wParam == dyl_key_translate(type))	
			{
				event->keys[type].key_state = state;
				is_active = true;
			//	printf("Key press has been initialized\n");
			}
		}
		
		 	
	#elif USING_SDL
		if((uint8_t)event->event.type == dyl_event_state_translate(state))
		{
			if((uint8_t)event->event.key.scancode == dyl_key_translate(type))	
			{
				event->keys[type].key_state = DYL_KEY_PRESSED;
				is_active = true;
			}
		}
	#endif

	event->keys[type].is_active = is_active;
	return is_active;
}

bool dyl_event_mouse_movement(Dyl_Event* event)
{
	ASSERT(event, "Passing NULL event through func");

	bool is_moved = false;
	#ifdef _WIN32

		if((uint8_t)event->event.message == dyl_event_state_translate(DYL_MOUSE_MOVEMENT))
		{
			u64 mouse_x = event->event.pt.x;
			u64 mouse_y = event->event.pt.y;
			/*LPPOINT pos;
			GetCursorPos(pos);
			wprintf(L"Get Cursor pos: %lld %lld", pos->x, pos->y);*/
			event->mouse_pos = (vec2i){mouse_x, mouse_y};
			is_moved = true;

		}
		
		 	
	#elif USING_SDL
		/*if((uint8_t)event->event.type == dyl_event_state_translate(state))
		{
			if((uint8_t)event->event.key.scancode == dyl_key_translate(type))	
			{
				event->keys[type].key_state = DYL_KEY_PRESSED;
				is_pressed = true;
			}
		}*/
	#endif
	return is_moved;
}





void dyl_event_end(Dyl_Event* event)
{
	#ifdef _WIN32
		for(size_t i = 0; i < DYL_KEY_AMOUNT; ++i)
		{
		//	if(!event->keys[i].is_active)
		//		event->keys[i].key_state = DYL_KEY_RELEASED;
		}
		TranslateMessage(&event->event);
		DispatchMessage(&event->event);
	#elif USING_SDL
		return;
	#endif
}
