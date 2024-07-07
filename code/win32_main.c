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

#define DIRECT3D_DEBUG

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
			0, 0, rect.right - rect.left, rect.bottom - rect.top, 0, 0, hInstance, 0);
		assert(window);
	}

	IDXGISwapChain *swap_chain;
	ID3D11Device *device;
	ID3D11DeviceContext *device_context;
	{
		u32 flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
		
		#ifdef DIRECT3D_DEBUG
			flags |= D3D11_CREATE_DEVICE_DEBUG;
		#endif

		DXGI_SWAP_CHAIN_DESC swap_chain_desc = {
			.BufferDesc = {
				.Format = DXGI_FORMAT_R8G8B8A8_UNORM
			},
			.BufferCount = 2,
			.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.OutputWindow = window,
			.SampleDesc.Count = 1,
			.Windowed = 1,
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD
		};

		assert(SUCCEEDED(D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, flags, 0, 0, D3D11_SDK_VERSION,
			&swap_chain_desc, &swap_chain, &device, 0, &device_context)));
	}
	#ifdef DIRECT3D_DEBUG
    {
        ID3D11InfoQueue* info;
        ID3D11Device_QueryInterface(device, &IID_ID3D11InfoQueue, (void**)&info);
        ID3D11InfoQueue_SetBreakOnSeverity(info, D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        ID3D11InfoQueue_SetBreakOnSeverity(info, D3D11_MESSAGE_SEVERITY_ERROR, TRUE);
        ID3D11InfoQueue_Release(info);
    }

    // enable debug break for DXGI too
    {
        IDXGIInfoQueue* dxgiInfo;
        DXGIGetDebugInterface1(0, &IID_IDXGIInfoQueue, (void**)&dxgiInfo);
        IDXGIInfoQueue_SetBreakOnSeverity(dxgiInfo, DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        IDXGIInfoQueue_SetBreakOnSeverity(dxgiInfo, DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, TRUE);
        IDXGIInfoQueue_Release(dxgiInfo);
    }
	#endif

	ID3D11RenderTargetView *backbuffer_rtv;
	{
		ID3D11Texture2D* backbuffer;
        IDXGISwapChain1_GetBuffer(swap_chain, 0, &IID_ID3D11Texture2D, (void**)&backbuffer);
        ID3D11Device_CreateRenderTargetView(device, (ID3D11Resource*)backbuffer, NULL, &backbuffer_rtv);
        ID3D11Texture2D_Release(backbuffer);
	}

	ID3D11VertexShader *vertex_shader;
	ID3D11PixelShader *pixel_shader;
	ID3DBlob *vertex_shader_blob;
	ID3DBlob *pixel_shader_blob;
	{
		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
		
		#ifdef DIRECT3D_DEBUG
		flags |= D3DCOMPILE_DEBUG;
		#endif

		ID3DBlob *error_blob;
		if (FAILED(D3DCompileFromFile(L"code\\shader.hlsl", 0,
				D3D_COMPILE_STANDARD_FILE_INCLUDE, "vs_main", "vs_5_0",
				flags, 0, &vertex_shader_blob, &error_blob))) {
			OutputDebugStringA((char *)ID3D10Blob_GetBufferPointer(error_blob));
			assert(0);
		}
		if (FAILED(D3DCompileFromFile(L"code\\shader.hlsl", 0,
				D3D_COMPILE_STANDARD_FILE_INCLUDE, "ps_main", "ps_5_0",
				flags, 0, &pixel_shader_blob, &error_blob))) {
			OutputDebugStringA((char *)ID3D10Blob_GetBufferPointer(error_blob));
			assert(0);
		}

		ID3D11Device_CreateVertexShader(device, ID3D10Blob_GetBufferPointer(vertex_shader_blob), 
			ID3D10Blob_GetBufferSize(vertex_shader_blob), 0, &vertex_shader);
		ID3D11Device_CreatePixelShader(device, ID3D10Blob_GetBufferPointer(pixel_shader_blob), 
			ID3D10Blob_GetBufferSize(pixel_shader_blob), 0, &pixel_shader);
	}

	ID3D11RasterizerState* rasterizer_state;
    {
        D3D11_RASTERIZER_DESC desc = {
            .FillMode = D3D11_FILL_SOLID,
            .CullMode = D3D11_CULL_NONE,
            .DepthClipEnable = TRUE,
        };
        ID3D11Device_CreateRasterizerState(device, &desc, &rasterizer_state);
    }

	ID3D11Buffer* vertex_buffer;
    {
        f32 data[] = {
           0, 0, 0.5,
		   -1, 0, 0.5,
		   -1, 1, 0.5
        };

        D3D11_BUFFER_DESC desc = {
            .ByteWidth = sizeof(data),
            .Usage = D3D11_USAGE_IMMUTABLE,
            .BindFlags = D3D11_BIND_VERTEX_BUFFER,
        };

        D3D11_SUBRESOURCE_DATA initial = { .pSysMem = data };
        ID3D11Device_CreateBuffer(device, &desc, &initial, &vertex_buffer);
    }

	ID3D11InputLayout *input_layout;
	{
		D3D11_INPUT_ELEMENT_DESC desc[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            //{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, offsetof(struct Vertex, uv),       D3D11_INPUT_PER_VERTEX_DATA, 0 },
            //{ "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(struct Vertex, color),    D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

		ID3D11Device_CreateInputLayout(device, desc, ARRAYSIZE(desc),
			ID3D10Blob_GetBufferPointer(vertex_shader_blob), ID3D10Blob_GetBufferSize(vertex_shader_blob),
			&input_layout);
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

		ID3D11DeviceContext_IASetPrimitiveTopology(device_context, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		UINT32 stride = 3 * sizeof(f32);
		UINT32 offset = 0;
		ID3D11DeviceContext_IASetVertexBuffers(device_context, 0, 1, &vertex_buffer, &stride, &offset);

		ID3D11DeviceContext_IASetInputLayout(device_context, input_layout);

		RECT window_rect;
		GetClientRect(window, &window_rect);
		D3D11_VIEWPORT viewport = {
			.Width = (FLOAT)(window_rect.right - window_rect.left),
			.Height = (FLOAT)(window_rect.bottom - window_rect.top),
			.MinDepth = 0,
			.MaxDepth = 1,
		};

		ID3D11DeviceContext_RSSetState(device_context, rasterizer_state);
		ID3D11DeviceContext_RSSetViewports(device_context, 1, &viewport);

		ID3D11DeviceContext_VSSetShader(device_context, vertex_shader, 0, 0);
		ID3D11DeviceContext_PSSetShader(device_context, pixel_shader, 0, 0);

		ID3D11DeviceContext_OMSetRenderTargets(device_context, 1, &backbuffer_rtv, 0);

		ID3D11DeviceContext_Draw(device_context, 3, 0);

		IDXGISwapChain1_Present(swap_chain, 1, 0);
	}
}