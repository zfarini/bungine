#define COBJMACROS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <dxgi1_3.h>
#include <d3dcompiler.h>
#include <dxgidebug.h>
#include <assert.h>
#include <stdint.h>

#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#pragma comment (lib, "dxgi")
#pragma comment (lib, "dxguid")


typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef float f32;
typedef double f64;
typedef i32 b32;

LRESULT win32_window_callback(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
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

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	WNDCLASSA window_class = {
		.hInstance = hInstance,
		.lpfnWndProc = win32_window_callback,
		.lpszClassName = "main window class name",
	};

	RegisterClassA(&window_class);

	HWND window;
	{
		DWORD window_style = WS_OVERLAPPEDWINDOW|WS_VISIBLE;
		RECT rect = {.right = 1280, .bottom = 720};
		assert(AdjustWindowRect(&rect, window_style, 0));

		window = CreateWindowExA(0, window_class.lpszClassName, "Game", window_style,
			CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, 0, 0, hInstance, 0);
		assert(window);
	}

	IDXGISwapChain *swap_chain;
	ID3D11Device *device;
	ID3D11DeviceContext *device_context;
	{
		u32 flags = D3D11_CREATE_DEVICE_SINGLETHREADED|D3D11_CREATE_DEVICE_DEBUG;

		DXGI_SWAP_CHAIN_DESC swap_chain_desc = {
			.BufferDesc = {
				.Format = DXGI_FORMAT_R8G8B8A8_UNORM
			},
			.BufferCount = 1,
			.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.OutputWindow = window,
			.SampleDesc.Count = 1,
			.Windowed = 1
		};

		assert(SUCCEEDED(D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, flags, 0, 0, D3D11_SDK_VERSION,
			&swap_chain_desc, &swap_chain, &device, 0, &device_context)));
	}

	ID3D11RenderTargetView *backbuffer_rtv;
	{
		ID3D11Texture2D* backbuffer;
        IDXGISwapChain1_GetBuffer(swap_chain, 0, &IID_ID3D11Texture2D, (void**)&backbuffer);
        ID3D11Device_CreateRenderTargetView(device, (ID3D11Resource*)backbuffer, NULL, &backbuffer_rtv);
        ID3D11Texture2D_Release(backbuffer);
	}

	b32 should_close = 0;
	while (!should_close) {
		MSG message;
		if (PeekMessageA(&message, 0, 0, 0, PM_REMOVE)) {
			if (message.message == WM_QUIT)
				break ;
			if (message.message == WM_KEYDOWN) {
				if (message.wParam == VK_ESCAPE)
					should_close = 1;
			}
			TranslateMessage(&message);
			DispatchMessageA(&message);
			continue;
		}

		FLOAT color[] = { 0.392f, 0.584f, 0.929f, 1.f };
        ID3D11DeviceContext_ClearRenderTargetView(device_context, backbuffer_rtv, color);

		IDXGISwapChain1_Present(swap_chain, 1, 0);
	}
}