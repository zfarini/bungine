
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#ifdef RENDERER_DX11
#include "imgui/imgui_impl_dx11.cpp"
#elif RENDERER_OPENGL
#include "imgui/imgui_impl_opengl3.cpp"
#else
#error "??"
#endif


#include "imgui/imgui_demo.cpp"
#include "imgui/imgui_impl_glfw.cpp"
// #ifdef _WIN32
// #include "imgui/imgui_impl_win32.cpp"
// #else
// #endif

#include "imgui/imgui.cpp"
#include "imgui/imgui_tables.cpp"
#include "imgui/imgui_draw.cpp"
#include "imgui/imgui_widgets.cpp"

#define UFBX_IMPLEMENTATION
#include <ufbx.h>

// TODO: cleanup do I really have to include these here?
#include <math.h>
#include <float.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "common.h"
#include "math.h"
#include "platform.h"
extern Platform platform;
extern thread_local TempMemory g_temp_memory;
#include "arena.h"

function void *stb_arena_realloc(void *ptr, usize prevsize, usize newsize)
{
	void *data = arena_alloc(&g_temp_memory.arena, newsize);
    
	if (prevsize > newsize)
		prevsize = newsize;
	if (ptr)
		memcpy(data, ptr, prevsize);
	return data;
}

#define STBI_MALLOC(size) arena_alloc(&g_temp_memory.arena, size)
#define STBI_FREE(ptr) (void)ptr
#define STBI_REALLOC_SIZED(ptr, prevsize, newsize) stb_arena_realloc(ptr, prevsize, newsize)

// #define STBI_MALLOC(size) arena_alloc(&platform.temp_arena, size)
// #define STBI_FREE(ptr) (void)ptr
// #define STBI_REALLOC_SIZED(ptr, prevsize, newsize) realloc(ptr, newsize)

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


#define MA_NO_MP3
#define MA_NO_FLAC
#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>