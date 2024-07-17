#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#pragma comment (lib, "dxgi")
#pragma comment (lib, "dxguid")

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <dxgi1_3.h>
#include <d3dcompiler.h>
#include <dxgidebug.h>
#include <assert.h>
#include <stdint.h>
#include <cstdio>
#include <float.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#undef min
#undef max
#undef near
#undef swap

#include <ufbx.h>
#include <stb_image.h>
#include "common.h"
#include "arena.h"
#include "utils.h"
#include "math.h"
#include "platform.h"
#define DIRECT3D_DEBUG
#include "renderer.cpp"
global RenderContext g_rc;

#include "game.cpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


LRESULT win32_window_callback(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(window, message, wparam, lparam))
        return true;

	LRESULT result = 0;

	switch(message) {
		case WM_CLOSE:
			PostQuitMessage(0);
			break ;
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
	SetCursorPos(rect.left + (rect.right - rect.left) / 2, 
				 rect.top + (rect.bottom - rect.top) / 2);
}

void update_game_input(HWND window, GameInput &input, int frame)
{
	UINT button_vkcode[BUTTON_COUNT] = {};

	button_vkcode[BUTTON_CAMERA_FORWARD] 	= 'W';
	button_vkcode[BUTTON_CAMERA_BACKWARD] 	= 'S';
	button_vkcode[BUTTON_CAMERA_LEFT] 		= 'A';
	button_vkcode[BUTTON_CAMERA_RIGHT] 		= 'D';
	button_vkcode[BUTTON_CAMERA_UP] 		= 'Q';
	button_vkcode[BUTTON_CAMERA_DOWN] 		= 'E';
	button_vkcode[BUTTON_MOUSE_LEFT]		= VK_LBUTTON;
	button_vkcode[BUTTON_MOUSE_RIGHT]		= VK_RBUTTON;
	button_vkcode[BUTTON_PLAYER_FORWARD] 	= 'F';
	button_vkcode[BUTTON_PLAYER_BACKWARD] 	= 'G';
	button_vkcode[BUTTON_PLAYER_JUMP]		= VK_SPACE;
	button_vkcode[BUTTON_LEFT_SHIFT]			= VK_LSHIFT;
	for (int i = BUTTON_F1; i < BUTTON_COUNT; i++)
		button_vkcode[i] = 0x31 + (i-BUTTON_F1);

	for (int i = 0; i < BUTTON_COUNT; i++)
		input.buttons[i].was_down = input.buttons[i].is_down;

	for (int i = 0; i < BUTTON_COUNT; i++) {
		if (GetAsyncKeyState(button_vkcode[i]) & 0x8000)
			input.buttons[i].is_down = 1;
		else
			input.buttons[i].is_down = 0;
	}

	if (frame > 3) {
		POINT cursor;
		GetCursorPos(&cursor);
		v2 screen_center;
		RECT rect;
		GetClientRect(window, &rect);
		screen_center = V2(rect.left + (rect.right - rect.left) / 2, 
					rect.top + (rect.bottom - rect.top) / 2);
		input.mouse_dp = V2(cursor.x, cursor.y) - screen_center;
	}
	if (g_hide_mouse)
		move_cursor_to_window_center(window);
	else
		input.mouse_dp = {};
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

//int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
int main()
{
	{
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        
        ImGui::StyleColorsDark();
        // ImGui::StyleColorsLight();
    }
	Arena game_memory;
	{
		usize game_memory_size = GigaByte(3);
		void *data = VirtualAlloc(0, game_memory_size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
		assert(data);
		game_memory = make_arena(data, game_memory_size);
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

	RenderContext &rc = g_rc;

	Game *game = (Game *)arena_alloc(&game_memory, sizeof(*game));
	
	init_render_context(&game_memory, rc, window);

	{
        ImGui_ImplWin32_Init(window);
        ImGui_ImplDX11_Init(rc.device, rc.context);
    }
	
	GameInput game_input = {};

	float dt = 1.f / 60;
	
	b32 should_close = 0;
	int frame = 0;

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

		update_game_input(window, game_input, frame);

		ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
		
		game_update_and_render(*game, rc, &game_memory, game_input, dt);
		
		ImGui::Render();
		rc.context->OMSetRenderTargets(1, &rc.backbuffer_rtv, nullptr);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		rc.swap_chain->Present(1, 0);
		frame++;
	}

	ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}