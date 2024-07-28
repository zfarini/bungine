#include <ufbx.h>
#include <stb_image.h>
#include <float.h>
#include <cstdio>
#include <cstdlib>
#include <X11/Xlib.h>
#include <EGL/egl.h>
#include <GL/glcorearb.h>
#include <unistd.h>
#include <cassert>
#include <cstring>
#include <X11/cursorfont.h>

#define OPENGL_DEBUG

#include "common.h"
#include "arena.h"
#include "utils.h"
#include "math.h"
#include "platform.h"
#include "renderer.h"
#include "renderer_opengl.cpp"
#include "renderer.cpp"

#include "game.cpp"

void fatal_error(const char *message)
{
	fprintf(stderr, "%s\n", message);
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "zenity --error --no-wrap --text=\"%s\"", message);
    system(cmd);
	exit(1);
}

int main()
{
    Display* x_display = XOpenDisplay(NULL);
	if (!x_display)
		fatal_error("Cannot open X display");

	XSetWindowAttributes attributes = {};
	attributes.event_mask = StructureNotifyMask;

    int width = 800;
    int height = 600;
    Window window = XCreateWindow(
        x_display, DefaultRootWindow(x_display),
        0, 0, width, height, 
        0, CopyFromParent, InputOutput, CopyFromParent, CWEventMask,
        &attributes);
	if (!window)
		fatal_error("Failed to create X window");

	// set title
    XStoreName(x_display, window, "game");

	Atom WM_PROTOCOLS = XInternAtom(x_display, "WM_PROTOCOLS", False);
    Atom WM_DELETE_WINDOW = XInternAtom(x_display , "WM_DELETE_WINDOW", False);
    XSetWMProtocols(x_display, window, &WM_DELETE_WINDOW, 1);


    EGLDisplay display;
	{
        display = eglGetDisplay((NativeDisplayType)x_display);
		if (display == EGL_NO_DISPLAY)
			fatal_error("Failed to get EGL display");

		EGLint major, minor;
        if (!eglInitialize(display, &major, &minor))
			fatal_error("Failed to initalize EGL display");
		if (major < 1 || (major == 1 && minor < 5))
			fatal_error("EGL version 1.5 or higher required");
		printf("EGL VERSION: %d.%d\n", major, minor);
	}

	if (!eglBindAPI(EGL_OPENGL_API))
		fatal_error("Failed to select OpenGL API for EGL");

	EGLConfig config;
    {
        EGLint attr[] = {
            EGL_SURFACE_TYPE,      EGL_WINDOW_BIT,
            EGL_CONFORMANT,        EGL_OPENGL_BIT,
            EGL_RENDERABLE_TYPE,   EGL_OPENGL_BIT,
            EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,

            EGL_RED_SIZE,      8,
            EGL_GREEN_SIZE,    8,
            EGL_BLUE_SIZE,     8,
            EGL_DEPTH_SIZE,   24,
            EGL_STENCIL_SIZE,  8,

            // uncomment for multisampled framebuffer
            //EGL_SAMPLE_BUFFERS, 1,
            //EGL_SAMPLES,        4, // 4x MSAA

            EGL_NONE,
        };

        EGLint count;
        if (!eglChooseConfig(display, attr, &config, 1, &count) || count != 1)
            fatal_error("Failed to choose EGL config");
    }

	EGLSurface surface;
    {
        EGLint attr[] = {
#if 0
            EGL_GL_COLORSPACE, EGL_GL_COLORSPACE_LINEAR,
#else
            EGL_GL_COLORSPACE, EGL_GL_COLORSPACE_SRGB,
#endif
			EGL_RENDER_BUFFER, EGL_BACK_BUFFER,
            EGL_NONE,
        };

        surface = eglCreateWindowSurface(display, config, window, attr);
        if (surface == EGL_NO_SURFACE)
            fatal_error("Failed to create EGL surface");
    }

    // create EGL context
    EGLContext context;
    {
        EGLint attr[] = {
            EGL_CONTEXT_MAJOR_VERSION, 4,
            EGL_CONTEXT_MINOR_VERSION, 2,
            EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
#ifndef OPENGL_DEBUG
            // ask for debug context for non "Release" builds
            // this is so we can enable debug callback
            EGL_CONTEXT_OPENGL_DEBUG, EGL_TRUE,
#endif
            EGL_NONE,
        };

        context = eglCreateContext(display, config, EGL_NO_CONTEXT, attr);
        if (context == EGL_NO_CONTEXT)
			fatal_error("Failed to create EGL context, OpenGL 4.2 is possibly not supported");
    }

    if (!eglMakeCurrent(display, surface, surface, context))
		fatal_error("Failed to make EGL context current");

	// vsync
	int vsync = 1;
	if (!eglSwapInterval(display, vsync)) {
		// TODO: this shouldn't be fatal
		fatal_error("Failed to set vsync for EGL");
		vsync = 0;
		fprintf(stderr, "failed to enable vsync\n");
	}

	RenderContext rc = {};

	rc.x_display = x_display;
	rc.display = display;
	rc.surface = surface;
	rc.context = context;
	rc.window = window;

#define X(type, name) do {\
	rc.name = (type)eglGetProcAddress(#name); \
	if (!rc.name) \
		fatal_error("Failed to load OpenGL function "#name"\n");\
} while (0);
    GL_FUNCTIONS(X)
#undef X

	init_render_context_opengl(rc);

	XSelectInput(x_display, window, KeyPressMask | KeyReleaseMask
			|ButtonPressMask|ButtonReleaseMask|PointerMotionMask);

	// show window
    XMapWindow(x_display, window);


	 //XDefineCursor(_glfw.x11.display, window->x11.handle,
     //                 _glfw.x11.hiddenCursorHandle);

	Platform platform = {};
	platform.render_context = &rc;

	GameInput game_input = {};
	usize game_memory_size = GigaByte(2);
	// TODO: use mmap
	Arena game_memory = make_arena(malloc(game_memory_size), game_memory_size);

	int button_xcode[BUTTON_COUNT] = {};
	button_xcode[BUTTON_CAMERA_FORWARD] 	= 0x19;
	button_xcode[BUTTON_CAMERA_BACKWARD] 	= 0x27;
	button_xcode[BUTTON_CAMERA_LEFT] 		= 0x26;
	button_xcode[BUTTON_CAMERA_RIGHT] 		= 0x28;
	button_xcode[BUTTON_CAMERA_UP] 			= 0x18;
	button_xcode[BUTTON_CAMERA_DOWN] 		= 0x1a;
	button_xcode[BUTTON_PLAYER_FORWARD] 	= 0x29;
	button_xcode[BUTTON_PLAYER_BACKWARD] 	= 0x2a;
	button_xcode[BUTTON_PLAYER_JUMP]		= 0x41;
	button_xcode[BUTTON_LEFT_SHIFT]			= 0x32;

	//button_vkcode[BUTTON_MOUSE_LEFT]		= VK_LBUTTON;
	//button_vkcode[BUTTON_MOUSE_RIGHT]		= VK_RBUTTON;

	for (int i = BUTTON_F1; i < BUTTON_COUNT; i++)
		button_xcode[i] = 0x43 + (i-BUTTON_F1);

	bool should_close = false;

	v2 last_mouse_p = {};

	int frame = 0;

	bool hide_mouse = true;

	while (!should_close) {
		game_input.mouse_dp = {};
		for (int i = 0; i < BUTTON_COUNT; i++)
			game_input.buttons[i].was_down = game_input.buttons[i].is_down;
		
		while (XPending(x_display)) {
			XEvent event;
			XNextEvent(x_display, &event);
			if (event.type == ClientMessage) {
				if (event.xclient.message_type == WM_PROTOCOLS) {
                    Atom protocol = event.xclient.data.l[0];
					if (protocol == WM_DELETE_WINDOW)
						should_close = true;
				}
			}
			else if (event.type == KeyPress || event.type == KeyRelease) {
				for (int i = 0; i < BUTTON_COUNT; i++)
					if (button_xcode[i] == event.xkey.keycode) {
						game_input.buttons[i].is_down = event.type == KeyPress;
						break ;
					}
        	    printf( "KeyPress: %x\n", event.xkey.keycode );
        	    if (event.xkey.keycode == 0x09)
					should_close = true;
        	}
			else if (event.type == ButtonPress || event.type == ButtonRelease)
			{
				if (event.xbutton.button == 1)
					game_input.buttons[BUTTON_MOUSE_LEFT].is_down = event.type == ButtonPress;
				else if (event.xbutton.button == 3)
					game_input.buttons[BUTTON_MOUSE_RIGHT].is_down = event.type == ButtonPress;
        	    //printf( "Mouse KeyPress: %x\n", event.xbutton.button );
			}
			else if (event.type == MotionNotify)
			{
				game_input.mouse_p = V2(event.xmotion.x, event.xmotion.y);
				if (frame > 3) {
					game_input.mouse_dp = game_input.mouse_p - last_mouse_p;
				}
				last_mouse_p = game_input.mouse_p;
				//printf("mouse p: %d %d\n", event.xmotion.x, event.xmotion.y);
			}
		}
		if (game_input.buttons[BUTTON_F1].is_down)
			hide_mouse = !hide_mouse;
		if (hide_mouse) {
		}
		else {

		}

		XWindowAttributes attr;
        Status status = XGetWindowAttributes(x_display, window, &attr);
		width = attr.width;
		height = attr.height;

		float dt = 1.f / 60;
		game_update_and_render(platform, &game_memory, game_input, dt);

		// TODO: handle when window is minimized 
		if (!eglSwapBuffers(display, surface))
			fatal_error("Failed to swap OpenGL buffers");
		frame++;
	}
}
