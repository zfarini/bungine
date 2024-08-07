

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#ifdef RENDERER_DX11
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#pragma comment (lib, "dxgi")
#pragma comment (lib, "dxguid")
#include <d3d11.h>
#include <dxgi1_3.h>
#include <d3dcompiler.h>
#include <dxgidebug.h>
#endif
#include <assert.h>
#include <stdint.h>
#include <cstdio>
#include <float.h>
#include <unordered_map>

#include <glad/glad.h>
#include <glad.c>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#ifdef RENDERER_OPENGL
#include "imgui/imgui_impl_opengl3.h"
#else
#include "imgui/imgui_impl_dx11.h"
#endif

#undef min
#undef max
#undef near
#undef swap

#include "game.cpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

global bool g_window_active;

LRESULT win32_window_callback(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(window, message, wparam, lparam))
        return true;

	LRESULT result = 0;

	switch(message) {
		case WM_ACTIVATE:
			if (LOWORD(wparam) == 0)
				g_window_active = false;
			else
				g_window_active = true;
			break ;
		case WM_CLOSE:
			PostQuitMessage(0);
			break ;
		case WM_MENUCHAR:
			// alt+enter disable sound
			if(LOWORD(wparam) & VK_RETURN)
				return MAKELRESULT(0, MNC_CLOSE);
		default:
			result = DefWindowProcA(window, message, wparam, lparam);
			break ;
	}

	return result;
}

b32 g_hide_mouse = 1;

void move_cursor_to_window_center(HWND window)
{
	while (ShowCursor(FALSE) >= 0)
		;
	RECT rect;
	GetClientRect(window, &rect);

	POINT cursor;
	cursor.x = rect.left + (rect.right - rect.left) / 2;
	cursor.y = rect.top + (rect.bottom - rect.top) / 2;
	ClientToScreen(window, &cursor);
	SetCursorPos(cursor.x, cursor.y);
}

v2 last_mouse_p;

void update_game_input(HWND window, GameInput &input, int frame)
{
	UINT button_vkcode[BUTTON_COUNT] = {};

	
	for (int i = 0; i < 26; i++)
		button_vkcode[BUTTON_A + i] = 'A' + i;

	button_vkcode[BUTTON_ESCAPE] 	= VK_ESCAPE;
	button_vkcode[BUTTON_SPACE] 	= VK_SPACE;
	button_vkcode[BUTTON_LEFT_SHIFT] 		= VK_LSHIFT;
	button_vkcode[BUTTON_LEFT_CONTROL] 		= VK_LCONTROL;
	button_vkcode[BUTTON_MOUSE_LEFT]		= VK_LBUTTON;
	button_vkcode[BUTTON_MOUSE_RIGHT]		= VK_RBUTTON;

	for (int i = BUTTON_F1; i < BUTTON_COUNT; i++)
		button_vkcode[i] = VK_F1 + (i-BUTTON_F1);

	for (int i = 0; i < BUTTON_COUNT; i++)
		input.buttons[i].was_down = input.buttons[i].is_down;

	if (g_window_active) {
		for (int i = 0; i < BUTTON_COUNT; i++) {
			if (GetAsyncKeyState(button_vkcode[i]) & 0x8000)
				input.buttons[i].is_down = 1;
			else
				input.buttons[i].is_down = 0;
		}
	}

	if (g_window_active)
	{
		POINT cursor;
		GetCursorPos(&cursor);
		ScreenToClient(window, &cursor);

		input.mouse_p = V2(cursor.x, cursor.y);

		if (frame > 3) {
			if (g_hide_mouse) {
				v2 screen_center;
				RECT rect;
				GetClientRect(window, &rect);
				screen_center = V2(rect.left + (rect.right - rect.left) / 2, 
							rect.top + (rect.bottom - rect.top) / 2);
				input.mouse_dp = V2(cursor.x, cursor.y) - screen_center;
			} else {
				input.mouse_dp = input.mouse_p - last_mouse_p;
			}
		}
		last_mouse_p = input.mouse_p;
		if (g_hide_mouse)
			move_cursor_to_window_center(window);

		if (IsDownFirstTime(input, BUTTON_F1)) {
			if (g_hide_mouse) {
				while (ShowCursor(TRUE) < 0)
					;
			}
			else
				move_cursor_to_window_center(window);
			g_hide_mouse = !g_hide_mouse;
		}
	}
	else
	{
		input.mouse_dp = {};
	}
}


#include <intrin.h>

bool add_thread_work(ThreadWorkFn *callback, void *data)
{
	int index;
	while (1) {
		index = thread_work_queue_occupied_index;
		if (THREAD_MASK(index + 1) == THREAD_MASK(thread_work_queue_read_index)) {
			assert(0);
			return false;
		}
		
		if (InterlockedCompareExchange((LONG *)&thread_work_queue_occupied_index, 
			index + 1, index) == index)
			break ;
	}

	ThreadWork *work = &thread_work_queue[THREAD_MASK(index)];

	work->callback = callback;
	work->data = data;

	while (InterlockedCompareExchange((LONG *)&thread_work_queue_write_index,
			index + 1, index) != index)
		;
	ReleaseSemaphore(thread_work_semaphore, 1, 0);
	return true;
}

DWORD thread_func(LPVOID param)
{
	int idx = *(DWORD *)param;

	printf("lanched thread %d\n", idx);

	while (1) {
		int entry_idx = thread_work_queue_read_index;
		_ReadWriteBarrier(); // propably not necessary
		if (entry_idx != thread_work_queue_write_index) {
			if (InterlockedCompareExchange((LONG *)&thread_work_queue_read_index, 
				entry_idx + 1, entry_idx) == entry_idx) 
			{
				ThreadWork *work = &thread_work_queue[THREAD_MASK(entry_idx)];
				work->callback(work->data);
			}
		}
		else
			WaitForSingleObject(thread_work_semaphore, INFINITE);
	}
	return 0;
}

void push_string(const char *str);

THREAD_WORK_FUNC(push_string_work)
{
	const char *str = (char *)data;
	printf("%s\n", str);
	if ((str[strlen(str) - 1] - '0') % 2 == 0)
		push_string("PUSHED FROM THREA1");
}

void push_string(const char *str)
{
	add_thread_work(push_string_work, (void *)str);
}

//typedef void game_update_and_render_fn(PlatformData &platform_data, Arena *memory, GameInput &input, float dt);

// int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
int main()
{
	Arena game_memory;
	{
		usize game_memory_size = GigaByte(3);
		void *data = VirtualAlloc(0, game_memory_size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
		assert(data);
		game_memory = make_arena(data, game_memory_size);
	}

	ma_device sound_device;
	{

		ma_device_config config = ma_device_config_init(ma_device_type_playback);
		config.playback.format   = ma_format_f32;
		config.playback.channels = SOUND_CHANNEL_COUNT;
		config.sampleRate        = SOUND_SAMPLE_RATE;
		config.dataCallback      = audio_write_callback;
		// @HACK: the game struct should always be allocated the first thing
		config.pUserData         = game_memory.data;

		if (ma_device_init(NULL, &config, &sound_device) != MA_SUCCESS)
			printf("ERROR: couldn't init sound device\n");
		else
			ma_device_start(&sound_device);
	}

	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO &io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		
		ImGui::StyleColorsDark();
		// ImGui::StyleColorsLight();
	}
	

	HWND window;
	{
		WNDCLASSA window_class = {};
		window_class.lpfnWndProc = win32_window_callback;
		window_class.lpszClassName = "main window class name";
		
		RegisterClassA(&window_class);

		DWORD window_style = WS_OVERLAPPEDWINDOW|WS_VISIBLE;
		
		RECT rect = {};
		rect.right = 1280;
		rect.bottom = 720;
		assert(AdjustWindowRect(&rect, window_style, 0));

		window = CreateWindowExA(0, window_class.lpszClassName, "Game", window_style,
			0, 0, rect.right - rect.left, rect.bottom - rect.top, 0, 0, 0, 0);
		assert(window);
	}
	HDC window_dc;
	{
		// Set up OpenGL context
		HDC hdc = GetDC(window);

		PIXELFORMATDESCRIPTOR pfd = {
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,
			32,                      // Color depth
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			24,                      // Depth buffer
			8,                       // Stencil buffer
			0,
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};

		int pixelFormat = ChoosePixelFormat(hdc, &pfd);
		SetPixelFormat(hdc, pixelFormat, &pfd);

		HGLRC hrc = wglCreateContext(hdc);
		wglMakeCurrent(hdc, hrc);
		assert(gladLoadGL());
		window_dc = hdc;
	}
	// Setup Platform/Renderer backends
	ImGui_ImplWin32_InitForOpenGL(window);
	ImGui_ImplOpenGL3_Init();

	RenderContext rc = {};
	//init_render_context_dx11(rc, window);

	//ImGui_ImplWin32_Init(rc.window);
	//ImGui_ImplDX11_Init(rc.device, rc.context);
	
	

	GameInput game_input = {};

	TempArena temp_arena = {};

	Platform platform = {};
	platform.render_context = &rc;
	platform.imgui_context = ImGui::GetCurrentContext();
	platform.add_thread_work = add_thread_work;

	float dt = 1.f / 60;
	
	b32 should_close = 0;
	int frame = 0;

	const char *game_dll_name = "build\\game2.dll";
	const char *game_temp_dll_name = "build\\game_temp.dll";
	game_update_and_render_fn *update_and_render = 0;
	HMODULE game_dll = 0;
	//FILETIME game_dll_last_write_time;

	RECT rect;
    GetClientRect(window, &rect);
	int window_width = rect.right - rect.left;
    int window_height = rect.bottom - rect.top;

	thread_work_semaphore = CreateSemaphoreA(0, 0, ARRAY_SIZE(thread_work_queue), 0);
	
	#define THREAD_COUNT 3
	DWORD thread_ids[THREAD_COUNT];

	for (int i = 0; i < THREAD_COUNT; i++) {
		CreateThread(0, 0, thread_func, &thread_ids[i], 0, &thread_ids[i]);
	}

	while (!should_close) {

		MSG message;
		if (PeekMessageA(&message, 0, 0, 0, PM_REMOVE)) {
			if (message.message == WM_QUIT)
				break ;
			if (message.message == WM_KEYDOWN || message.message == WM_KEYUP) {
				if (message.wParam == VK_ESCAPE)
					should_close = 1;
			}
			TranslateMessage(&message);
			DispatchMessageA(&message);
			continue;
		}
		

		// {
		// 	WIN32_FILE_ATTRIBUTE_DATA data;
		// 	GetFileAttributesExA(game_dll_name, GetFileExInfoStandard, &data);
		// 	if (!game_dll || CompareFileTime(&game_dll_last_write_time, &data.ftLastWriteTime) == -1) {
		// 		FreeLibrary(game_dll);
		// 		//if (!DeleteFile(game_temp_dll_name))
		// 		//	assert(0);
		// 		if (!CopyFile(game_dll_name, game_temp_dll_name, FALSE))
		// 			assert(0);
		// 		game_dll = LoadLibraryA(game_temp_dll_name);
		// 		assert(game_dll);
		// 		update_and_render = (game_update_and_render_fn *)GetProcAddress(game_dll, "game_update_and_render");
		// 		assert(update_and_render);
		// 		game_dll_last_write_time = data.ftLastWriteTime;
		// 	}
		// }
		// printf("%d\n", g_window_active);

		update_game_input(window, game_input, frame);


		GetClientRect(window, &rect);
		int new_width = rect.right - rect.left;
		int new_height = rect.bottom - rect.top;
		if (window_width != new_width || window_height != new_height)
			game_input.mouse_dp = {};
		window_width = new_width;
		window_height = new_height;
		rc.window_width = window_width;
		rc.window_height = window_height;

		//if (update_and_render)
		//	update_and_render(platform_data, &game_memory, game_input, dt);
		game_update_and_render(platform, &game_memory, game_input, dt);
		
		SwapBuffers(window_dc);
		frame++;
	}

#ifdef RENDERER_OPENGL
	ImGui_ImplOpenGL3_Shutdown();
#else
	ImGui_ImplDX11_Shutdown();
#endif
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

	//ma_device_uninit(&sound_device);

}