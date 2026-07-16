#include "dyl_window.h"
#include <profileapi.h>

#ifdef USING_SDL
#include <SDL3/SDL.h>
#endif



/*#if defined (__WIN32)
	static HWND CreateOpenGLWindow(char* title, int x, int y, int width, int height, BYTE type, DWORD flags)
	{
		int pf;
		HDC hdc;
		HWND hwnd;
		WNDCLASS wc;
		PIXELFORMATDESCRIPTOR pfd;
		static HINSTANCE h_instance = 0;

		if(!h_instance)
		{
			h_instance = GetModuleHandle(NULL);
			wc.style = CS_OWNDC;
	//		wc.lpfnWndProc   = (WNDPROC)WindowProc;
			wc.cbClsExtra    = 0;
			wc.cbWndExtra    = 0;
			wc.hInstance     = h_instance;
			wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
			wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
			wc.hbrBackground = NULL;
			wc.lpszMenuName  = NULL;
			wc.lpszClassName = "OpenGL";
			

		}
		

		return hwnd;
		
	}
#endif*/


void window_initialize(Dyl_Window* window, const char* window_name, u32 x, u32 y, u32 width, u32 height, unsigned long long window_flags, bool enable_vsync, Platform* platform)
{
	
	ASSERT(window, "Window pointer is NULL");
	window->x = x;
	window->y = y;
	window->width = width;
	window->height = height;
	window->window_flags = window_flags;
	window->is_window_open = true;
	window->window_name = DYL_STR_LIT(window_name);
	window->enable_vsync = enable_vsync;

	#ifdef _WIN32
		LPCTSTR CLASS_NAME = "Dyl Engine Window";
		WNDCLASS window_class = {0};
		window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		window_class.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
		window_class.hbrBackground = NULL;
		window_class.lpfnWndProc = WindowProc;
		window_class.hInstance = platform->h_instance;
		window_class.lpszClassName = CLASS_NAME;
		RegisterClass(&window_class);


		//DUMMY WINDOW
		HWND fake_wnd = CreateWindowEx(0,"core", "Fake Window", WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0,0,1,1, NULL, NULL, platform->h_instance, NULL);
		HDC fake_dc = GetDC(fake_wnd);
		PIXELFORMATDESCRIPTOR fake_pfd;
		memset(&fake_pfd, 0, sizeof(fake_pfd));
		fake_pfd.nSize = sizeof(fake_pfd);
		fake_pfd.nVersion = 1;
		fake_pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		fake_pfd.iPixelType = PFD_TYPE_RGBA;
		fake_pfd.cColorBits = 32;
		fake_pfd.cAlphaBits = 8;
		fake_pfd.cDepthBits = 24;
		
		int fake_pf = ChoosePixelFormat(fake_dc, &fake_pfd);
		if(!fake_pf)
		{
			MessageBox(NULL, "ChoosePixelFormat() failed cannot find suitable format.", "Error", MB_OK);
			ASSERT(1, "Failed");
		}
		int fake_spf = SetPixelFormat(fake_dc, fake_pf, &fake_pfd);
		if(!fake_spf)
		{
			MessageBox(NULL, "SetPixelFormat() failed cannot find suitable format.", "Error", MB_OK);
			ASSERT(1, "Failed");
		}	
		DescribePixelFormat(fake_dc, fake_pf, sizeof(PIXELFORMATDESCRIPTOR), &fake_pfd);
		
		//DUMMY CTX
		HGLRC fake_rc = wglCreateContext(fake_dc);
		if(!fake_rc)
		{
			MessageBox(NULL, "wglCreateContext() failed", "ERROR", MB_OK);
			ASSERT(1, "Failed");
		}
		
		if(!wglMakeCurrent(fake_dc, fake_rc))
		{
			MessageBox(NULL, "wglMakeCurrent() failed", "ERROR", MB_OK);
			ASSERT(1, "Failed");
		}

		gladLoadWGL(fake_dc);	

		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(fake_rc);
		ReleaseDC(fake_wnd, fake_dc);
		DestroyWindow(fake_wnd);

		window->window_handle = CreateWindowEx(0, CLASS_NAME,(LPCSTR)window->window_name.string_data, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 
										 window->x, window->y, window->width, window->height, NULL, NULL, platform->h_instance, NULL);

		if(!window->window_handle)
		{
			fprintf(stderr, "ERROR: Unable to setup window\n");
			exit(0);
		}
		
		window->device_context = GetDC(window->window_handle);
		const int pixelAttribs[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_ALPHA_BITS_ARB, 8,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
		WGL_SAMPLES_ARB, 4,
		0
		};
		int pfid = 0;
		UINT num_formats = 0;
		const bool status = wglChoosePixelFormatARB(window->device_context, pixelAttribs, NULL, 1, &pfid, &num_formats);
		if(!status || !num_formats)
		{
			MessageBox(NULL, "wglChoosePixelFormatARB() failed.", "ERROR", MB_OK);
			ASSERT(0, "Failed");
		}


		PIXELFORMATDESCRIPTOR pfd;
		memset(&pfd, 0, sizeof(pfd));
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		DescribePixelFormat(window->device_context, pfid, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
		int spf = SetPixelFormat(window->device_context, pfid, &pfd);
		if(!spf)
		{
			MessageBox(NULL, "SetPixelFormat() failed cannot find suitable format.", "Error", MB_OK);
			ASSERT(1, "Failed");
		}	


		const int major_min = 4, minor_min = 5;
		const int contextAttribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, major_min,
		WGL_CONTEXT_MINOR_VERSION_ARB, minor_min,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB, 
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
		0
		};

		window->gl_context = wglCreateContextAttribsARB(window->device_context, 0, contextAttribs);
		if(!window->gl_context)
		{
			MessageBox(NULL, "Unable to setup window gl_context :(.", "ERROR", MB_OK );
			ASSERT(0, "Failed");
		}
		ASSERT(wglMakeCurrent(window->device_context, window->gl_context), "Unable to create non dummy gl_context");
		gladLoadGL();
		#if 0
			while(1)
			{
				ShowWindow(window->window_handle, platform->cmd_show);
			}
		#endif
		wglSwapIntervalEXT(window->enable_vsync);

		ShowWindow(window->window_handle, platform->cmd_show);


	#else	
	
		if (SDL_Init(SDL_INIT_VIDEO) == 0) {
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not initialize SDL: %s", SDL_GetError());
			return;
		}
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG | SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
		
		window->window_handle = SDL_CreateWindow((char*)window->window_name.string_data, window->width, window->height, window->window_flags);
		SDL_SetWindowPosition(window->window_handle, window->x, window->y);
		ASSERT(window->window_handle, "Unable to create window: %s", SDL_GetError);
		window->gl_context = SDL_GL_CreateContext(window->window_handle);
		ASSERT(window->gl_context, "Unable to create OpenGl context: %s", SDL_GetError);
		ASSERT(gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress), "Unable to initialize Opengl");
		window->enable_vsync = enable_vsync;
		SDL_GL_SetSwapInterval(enable_vsync);
	
	#endif
}

void window_set_vsync(Dyl_Window* window, bool enable_vsync)
{
	window->enable_vsync = enable_vsync;

	#ifdef _WIN32
		wglSwapIntervalEXT(window->enable_vsync);
	#else
		SDL_GL_SetSwapInterval(window->enable_vsync);
	#endif
}


void window_start(Dyl_Window* window)
{
	#ifdef _WIN32

//		QueryPerformanceCounter(window->start_time);
		wglMakeCurrent(window->device_context, window->gl_context);
	#else
		SDL_GL_MakeCurrent(window->window_handle, window->gl_context);
	#endif

	//TODO: Please make a more uniformed way of sending GPU commands
	//glViewport(0,0,window->width, window->height);

	glClearColor(0.0,0.0,0.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


}

void window_end(Dyl_Window* window)
{
	
	#ifdef _WIN32

	//	QueryPerformanceCounter(window->end_time);
	//	window->elasped_time = window->start_time - window->end_time;
	//	window->fps = window->elasped_time / window->frequency;

		SwapBuffers(window->device_context);
	#else
		SDL_GL_SwapWindow(window->window_handle);
	#endif

}

void window_destroy(Dyl_Window* window)
{
	ASSERT(window, "Unable to destroy window");
	window->x = 0;
	window->y = 0;
	window->width = 0;
	window->height = 0;
	window->window_flags = 0;
	
	#ifdef _WIN32
		
	/*	if(window->window_handle)
		{
			free(window->window_handle);
			window->window_handle = NULL;
		}*/
		wglMakeCurrent(NULL, NULL);
		if(window->gl_context)
			wglDeleteContext(window->gl_context);

		if(window->device_context)
			ReleaseDC(window->window_handle, window->device_context);

		if(window->window_handle)
			DestroyWindow(window->window_handle);
	#elif USING_SDL
		SDL_DestroyWindow(window->window_handle);
	#endif
		
}
