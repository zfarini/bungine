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

#include "math.h"

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
	WNDCLASSA window_class = {};

	window_class.hInstance = hInstance;
	window_class.lpfnWndProc = win32_window_callback;
	window_class.lpszClassName = "main window class name";
	

	RegisterClassA(&window_class);

	HWND window;
	{
		DWORD window_style = WS_OVERLAPPEDWINDOW|WS_VISIBLE;
		
		RECT rect = {};
		rect.right = 1280;
		rect.bottom = 720;
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

		DXGI_SWAP_CHAIN_DESC desc = {};
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferCount = 2;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.OutputWindow = window;
		desc.SampleDesc.Count = 1;
		desc.Windowed = 1;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;


		assert(SUCCEEDED(D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, flags, 0, 0, D3D11_SDK_VERSION,
			&desc, &swap_chain, &device, 0, &device_context)));
	}
	#ifdef DIRECT3D_DEBUG
    {
        ID3D11InfoQueue* info;
        device->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&info);
        info->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        info->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, TRUE);
        info->Release();
    }

    // enable debug break for DXGI too
    {
        IDXGIInfoQueue* dxgiInfo;
        DXGIGetDebugInterface1(0, __uuidof(IDXGIInfoQueue), (void**)&dxgiInfo);
        dxgiInfo->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        dxgiInfo->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, TRUE);
        dxgiInfo->Release();
    }
	#endif

	ID3D11RenderTargetView *backbuffer_rtv;
	{
		ID3D11Texture2D* backbuffer;
        swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backbuffer);
        device->CreateRenderTargetView((ID3D11Resource*)backbuffer, NULL, &backbuffer_rtv);
        backbuffer->Release();
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
			OutputDebugStringA((char *)error_blob->GetBufferPointer());
			assert(0);
		}
		if (FAILED(D3DCompileFromFile(L"code\\shader.hlsl", 0,
				D3D_COMPILE_STANDARD_FILE_INCLUDE, "ps_main", "ps_5_0",
				flags, 0, &pixel_shader_blob, &error_blob))) {
			OutputDebugStringA((char *)error_blob->GetBufferPointer());
			assert(0);
		}

		device->CreateVertexShader(vertex_shader_blob->GetBufferPointer(), 
			vertex_shader_blob->GetBufferSize(), 0, &vertex_shader);
		device->CreatePixelShader(pixel_shader_blob->GetBufferPointer(), 
			pixel_shader_blob->GetBufferSize(), 0, &pixel_shader);
	}

	ID3D11RasterizerState* rasterizer_state;
    {
        D3D11_RASTERIZER_DESC desc = {};
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_NONE;
		// TODO: enabling this clip thing that should be in range Idk why
		desc.DepthClipEnable = FALSE;
        
        device->CreateRasterizerState(&desc, &rasterizer_state);
    }

	ID3D11Buffer* vertex_buffer;
    {
        f32 data[] = {
           -1, 5, 0,
		   1, 5, 0,
		   0, 5, 1
        };

        D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = sizeof(data);
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        

        D3D11_SUBRESOURCE_DATA initial = {};
		initial.pSysMem = data;
        device->CreateBuffer(&desc, &initial, &vertex_buffer);
    }

	ID3D11InputLayout *input_layout;
	{
		D3D11_INPUT_ELEMENT_DESC desc[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            //{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, offsetof(struct Vertex, uv),       D3D11_INPUT_PER_VERTEX_DATA, 0 },
            //{ "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(struct Vertex, color),    D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

		device->CreateInputLayout(desc, ARRAYSIZE(desc),
			vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), &input_layout);
	}

	struct Constants
	{
		mat4 view;
		mat4 projection;
	};

	ID3D11Buffer* constant_buffer;
    {
        D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = sizeof(Constants);
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        device->CreateBuffer(&desc, 0, &constant_buffer);
    }

	v3 camera_p = {};
	v3 camera_rotation = {};
	v2 last_mouse_p = {};
	{
		POINT cursor;
		GetCursorPos(&cursor);
		last_mouse_p = v2{(float)cursor.x, (float)cursor.y};
	}
	float dt = 1.f / 60;
	
	//while (::ShowCursor(FALSE) >= 0);

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

		RECT window_rect;
		GetClientRect(window, &window_rect);
		int window_width = window_rect.right - window_rect.left;
		int window_height = window_rect.bottom - window_rect.top;

		// RECT clipRect = window_rect;
		// GetClientRect(window, &clipRect);
    	// ClientToScreen(window, (POINT*) &clipRect.left);
    	// ClientToScreen(window, (POINT*) &clipRect.right);
    	// ClipCursor(&clipRect);

		POINT cursor_p;
		GetCursorPos(&cursor_p);
		v2 curr_mouse_p = {(float)cursor_p.x, (float)cursor_p.y};

		{
			mat4 camera_transform = zrotation(camera_rotation.z) * xrotation(camera_rotation.x);

			v3 camera_x = (camera_transform * v4{1, 0, 0, 0}).xyz;
			v3 camera_y = (camera_transform * v4{0, 0, 1, 0}).xyz;
			v3 camera_z = (camera_transform * v4{0, -1, 0, 0}).xyz;

			v2 mouse_dp = (curr_mouse_p - last_mouse_p) * 0.1f;

			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
				camera_rotation.x += -mouse_dp.y * dt;
				camera_rotation.z += -mouse_dp.x * dt;
			}
			
			{
				v3 camera_dp = {};
				if (GetAsyncKeyState('W') & 0x8000)
					camera_dp -= camera_z;
				if (GetAsyncKeyState('S') & 0x8000)
					camera_dp += camera_z;
				if (GetAsyncKeyState('A') & 0x8000)
					camera_dp -= camera_x;
				if (GetAsyncKeyState('D') & 0x8000)
					camera_dp += camera_x;
				if (GetAsyncKeyState('Q') & 0x8000)
					camera_dp += camera_y;
				if (GetAsyncKeyState('E') & 0x8000)
					camera_dp -= camera_y;
				//camera_dp = v3{1, 0, 0};
				camera_p += normalize(camera_dp) * dt * 8;
			}

			mat4 rotation = {
				camera_x.x, camera_x.y, camera_x.z, 0,
				camera_y.x, camera_y.y, camera_y.z, 0,
				camera_z.x, camera_z.y, camera_z.z, 0,
				0, 0, 0, 1
			};

			Constants constants;

			constants.view = rotation * translate(-camera_p);
			constants.projection = perspective_projection(0.1, 100, 90, (float)window_height / window_width);

			D3D11_MAPPED_SUBRESOURCE constant_buffer_msr;
			device_context->Map(constant_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constant_buffer_msr);
			memcpy(constant_buffer_msr.pData, &constants, sizeof(constants));
			device_context->Unmap(constant_buffer, 0);
		}

		FLOAT color[] = { 0.392f, 0.584f, 0.929f, 1.f };
        device_context->ClearRenderTargetView(backbuffer_rtv, color);

		device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		UINT32 stride = 3 * sizeof(f32);
		UINT32 offset = 0;
		device_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

		device_context->IASetInputLayout(input_layout);

		D3D11_VIEWPORT viewport = {};

		viewport.Width = (FLOAT)window_width;
		viewport.Height = (FLOAT)window_height;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1;
	
		device_context->RSSetState(rasterizer_state);
		device_context->RSSetViewports(1, &viewport);

		device_context->VSSetShader(vertex_shader, 0, 0);
		device_context->VSSetConstantBuffers(0, 1, &constant_buffer);

		device_context->PSSetShader(pixel_shader, 0, 0);

		device_context->OMSetRenderTargets(1, &backbuffer_rtv, 0);

		device_context->Draw(3, 0);

		swap_chain->Present(1, 0);

		last_mouse_p = curr_mouse_p;
	}
}