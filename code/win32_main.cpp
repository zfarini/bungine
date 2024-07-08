#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <dxgi1_3.h>
#include <d3dcompiler.h>
#include <dxgidebug.h>
#include <assert.h>
#include <stdint.h>

#undef min
#undef max
#undef near
#undef swap

#include <ufbx.h>
#include <stb_image.h>

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
typedef size_t usize;

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


#include "utils.h"



struct Constants
{
	mat4 view;
	mat4 projection;
	mat4 model;
	mat4 normal_transform;
	v3 camera_p;
	float diffuse_factor;
	float specular_factor;
	float specular_exponent_factor;
	int has_normal_map;
	float tmp[4 * 4 - 7];
};

#include "renderer.cpp"
#include "scene.cpp"

v3 camera_p;

void render_scene(RenderContext &rc, Scene &scene, mat4 transform)
{
	RenderPass &rp = *rc.render_pass;
    
	for (usize i = 0; i < scene.meshes.count; i++) {
			UINT32 stride = sizeof(Vertex);
			UINT32 offset = 0;
			rc.context->IASetVertexBuffers(0, 1, &scene.meshes[i].vertex_buffer, &stride, &offset);

			Constants constants;
			constants.view = rp.view_mat;
			constants.projection = rp.projection_mat;
			constants.model = transform * scene.meshes[i].default_transform;
			constants.normal_transform = inverse(transpose(constants.model));
			constants.camera_p = camera_p;

				

			for (usize j = 0; j < scene.meshes[i].parts.count; j++) {
				MeshPart &part = scene.meshes[i].parts[j];

				if (part.material.diffuse.data)
					rc.context->PSSetShaderResources(0, 1, &part.material.diffuse.data);
				else
					rc.context->PSSetShaderResources(0, 1, &rc.white_texture);
				if (part.material.specular.data)
					rc.context->PSSetShaderResources(1, 1, &part.material.specular.data);
				else
					rc.context->PSSetShaderResources(1, 1, &rc.white_texture);
				rc.context->PSSetShaderResources(2, 1, &part.material.normal_map.data);
				if (part.material.specular_exponent.data)
					rc.context->PSSetShaderResources(3, 1, &part.material.specular_exponent.data);
				else
					rc.context->PSSetShaderResources(3, 1, &rc.white_texture);

				constants.diffuse_factor = part.material.diffuse_factor;
				constants.specular_factor = part.material.specular_factor;
				constants.specular_exponent_factor = part.material.specular_exponent_factor;
				constants.has_normal_map = part.material.normal_map.data != 0;

				copy_constants_to_gpu(rc, rp.cbuffer, &constants, sizeof(constants));	
				rc.context->Draw((UINT)part.vertices_count, (UINT)part.offset);
			}
	}
}

//int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
int main()
{

	WNDCLASSA window_class = {};

	window_class.hInstance = 0;
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
			0, 0, rect.right - rect.left, rect.bottom - rect.top, 0, 0, 0, 0);
		assert(window);
	}
	
	RenderContext rc = {};

	init_render_context(rc, window);

	D3D11_INPUT_ELEMENT_DESC input_layout_desc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,   0, offsetof(Vertex, normal),   D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT,          0, offsetof(Vertex, uv),       D3D11_INPUT_PER_VERTEX_DATA, 0 },
				//{ "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(struct Vertex, color),    D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	RenderPass mesh_render_pass = create_render_pass(rc, L"code\\shader.hlsl", "vs_main", "ps_main",
		input_layout_desc, ARRAYSIZE(input_layout_desc), sizeof(Constants));

	v3 camera_rotation = {};
	v2 last_mouse_p = {};
	{
		POINT cursor;
		GetCursorPos(&cursor);
		last_mouse_p = v2{(float)cursor.x, (float)cursor.y};
	}

	float dt = 1.f / 60;
	
	Scene ch43 = load_scene(rc, "data\\Ch43.fbx");
	Scene ch06 = load_scene(rc, "data\\Ch06.fbx");
	Scene ch15 = load_scene(rc, "data\\Ch15.fbx");
	Scene sponza = load_scene(rc, "data\\Sponza\\Sponza.fbx");
	//while (::ShowCursor(FALSE) >= 0);

	b32 should_close = 0;
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

		mat4 view, projection;
		{
			mat4 camera_transform =  zrotation(camera_rotation.z) * xrotation(camera_rotation.x);

			v3 camera_x = (camera_transform * v4{1, 0, 0, 0}).xyz;
			v3 camera_y = (camera_transform * v4{0, 0, 1, 0}).xyz;
			v3 camera_z = (camera_transform * v4{0, -1, 0, 0}).xyz;

			v2 mouse_dp = (curr_mouse_p - last_mouse_p) * 0.2f;

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

			view = rotation * translate(-camera_p);
			projection = perspective_projection(0.1, 100, 90, (float)window_height / window_width);
		}

		begin_frame(rc);
		
		D3D11_VIEWPORT viewport = {};

		viewport.Width = (FLOAT)window_width;
		viewport.Height = (FLOAT)window_height;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1;
		
		begin_render_pass(rc, mesh_render_pass, viewport, rc.backbuffer_rtv, view, projection);
		{
			render_scene(rc, ch43, identity());
			render_scene(rc, ch06, translate(2, 0, 0));
			render_scene(rc, ch15, translate(-2, 0, 0));
			render_scene(rc, sponza, translate(0, 0, 0));
		}
		end_render_pass(rc, mesh_render_pass);

		end_frame(rc);

		last_mouse_p = curr_mouse_p;
	}
}