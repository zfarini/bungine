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


#include <cstdio>
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

#include "arena.h"
#include "utils.h"


Arena *g_stb_image_arena;


void *stb_arena_realloc(void *ptr, usize prevsize, usize newsize)
{
	void *data = arena_alloc(g_stb_image_arena, newsize);

	if (prevsize > newsize)
		prevsize = newsize;
	if (ptr)
		memcpy(data, ptr, prevsize);
	return data;
}

#define STBI_MALLOC(size) arena_alloc(g_stb_image_arena, size)
#define STBI_FREE(ptr) (void)ptr
#define STBI_REALLOC_SIZED(ptr, prevsize, newsize) stb_arena_realloc(ptr, prevsize, newsize)


#define UFBX_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <ufbx.h>
#include <stb_image.h>


struct Constants
{
	mat4 view;
	mat4 projection;
	mat4 model;
	mat4 normal_transform;
	mat4 light_transform;
	v3 camera_p;
	float diffuse_factor;
	float specular_factor;
	float specular_exponent_factor;
	int has_normal_map;
	float tmp[4 * 4 - 7];
};

#include "renderer.cpp"
#include "debug.cpp"

#include "scene.cpp"

RenderContext g_rc = {};

v3 camera_p;

struct ShadowMap
{
	ID3D11DepthStencilView *dsv;
	ID3D11ShaderResourceView *srv;
	v3 light_dir;
	v3 light_p;
	mat4 view;
	mat4 projection;
	int width, height;
};
ShadowMap shadow_map = {};

void render_scene(RenderContext &rc, Scene &scene, mat4 transform)
{
	RenderPass &rp = *rc.render_pass;
    
	Constants constants = {};
	constants.view = rp.view_mat;
	constants.projection = rp.projection_mat;
	constants.camera_p = camera_p;
	constants.light_transform = shadow_map.projection * shadow_map.view;
	
	for (usize i = 0; i < scene.meshes.count; i++) {
			UINT32 stride = sizeof(Vertex);
			UINT32 offset = 0;
			rc.context->IASetVertexBuffers(0, 1, &scene.meshes[i].vertex_buffer, &stride, &offset);

			constants.model = transform * scene.meshes[i].default_transform;
			constants.normal_transform = inverse(transpose(constants.model));

				

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
	Arena memory_arena;
	{
		usize game_memory_size = GigaByte(6);
		void *data = VirtualAlloc(0, game_memory_size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
		assert(data);
		memory_arena = make_arena(data, game_memory_size);
	}

	Arena asset_arena = make_arena(arena_alloc(&memory_arena, GigaByte(4)), GigaByte(4));

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
	


	RenderContext &rc = g_rc;
	init_render_context(&asset_arena, rc, window);

	
	{
		shadow_map.width = 2048;
		shadow_map.height = 2048;
		shadow_map.light_p = v3{8, 0, 7};
		shadow_map.light_dir = v3{-1, 0, -0.8};

		shadow_map.view = lookat(shadow_map.light_p, shadow_map.light_dir, v3{0, 0, 1});
		shadow_map.projection = orthographic_projection(1, 100, 30, 30);

		mat4 M = shadow_map.projection * shadow_map.view;

		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width            = shadow_map.width;
		desc.Height           = shadow_map.height;
		desc.MipLevels        = 1;
		desc.ArraySize        = 1;
		desc.Format           = DXGI_FORMAT_R32_TYPELESS;
		desc.SampleDesc.Count = 1;
		desc.Usage            = D3D11_USAGE_DEFAULT;
		desc.BindFlags        = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

		ID3D11Texture2D* shadowmapDepthTexture;

		rc.device->CreateTexture2D(&desc, nullptr, &shadowmapDepthTexture);

		D3D11_DEPTH_STENCIL_VIEW_DESC shadowmapDSVdesc = {};
		shadowmapDSVdesc.Format        = DXGI_FORMAT_D32_FLOAT;
		shadowmapDSVdesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		rc.device->CreateDepthStencilView(shadowmapDepthTexture, &shadowmapDSVdesc, &shadow_map.dsv);

		D3D11_SHADER_RESOURCE_VIEW_DESC shadowmapSRVdesc = {};
		shadowmapSRVdesc.Format              = DXGI_FORMAT_R32_FLOAT;
		shadowmapSRVdesc.ViewDimension       = D3D11_SRV_DIMENSION_TEXTURE2D;
		shadowmapSRVdesc.Texture2D.MipLevels = 1;

		rc.device->CreateShaderResourceView(shadowmapDepthTexture, &shadowmapSRVdesc, &shadow_map.srv);

		shadowmapDepthTexture->Release();
	}

	D3D11_INPUT_ELEMENT_DESC input_layout_desc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,   0, offsetof(Vertex, normal),   D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,          0, offsetof(Vertex, uv),       D3D11_INPUT_PER_VERTEX_DATA, 0 },
				//{ "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(struct Vertex, color),    D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	RenderPass mesh_render_pass = create_render_pass(rc, L"code\\shader.hlsl", "vs_main", "ps_main",
		input_layout_desc, ARRAYSIZE(input_layout_desc), sizeof(Constants));

	RenderPass shadow_map_render_pass = create_render_pass(rc, L"code\\shader.hlsl", "vs_main", 0,
		input_layout_desc, ARRAYSIZE(input_layout_desc), sizeof(Constants));

	v3 camera_rotation = {};
	v2 last_mouse_p = {};
	{
		POINT cursor;
		GetCursorPos(&cursor);
		last_mouse_p = v2{(float)cursor.x, (float)cursor.y};
	}

	float dt = 1.f / 60;
	
	Scene ch43		= load_scene(&asset_arena, rc, "data\\Ch43.fbx");
	Scene ch06		= load_scene(&asset_arena, rc, "data\\Ch06.fbx");
	Scene ch15		= load_scene(&asset_arena, rc, "data\\Ch15.fbx");
	Scene sponza	= load_scene(&asset_arena, rc, "data\\Sponza\\Sponza.fbx");
	Scene cube		= load_scene(&asset_arena, rc, "data\\cube.fbx");
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
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1;

		FLOAT color[] = { 0.392f, 0.584f, 0.929f, 1.f };
		rc.context->ClearRenderTargetView(rc.backbuffer_rtv, color);

		viewport.Width = (FLOAT)shadow_map.width;
		viewport.Height = (FLOAT)shadow_map.height;
		begin_render_pass(rc, shadow_map_render_pass, viewport, 0, shadow_map.dsv, shadow_map.view, shadow_map.projection);
		{
			rc.context->ClearDepthStencilView(shadow_map.dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);

			render_scene(rc, ch43, zrotation(PI/2));
			//render_scene(rc, ch06, translate(2, 0, 0));
			//render_scene(rc, ch15, translate(-2, 0, 0));
			render_scene(rc, sponza, identity());
		}
		end_render_pass(rc, shadow_map_render_pass);
		
		viewport.Width = (FLOAT)window_width;
		viewport.Height = (FLOAT)window_height;
		
		begin_render_pass(rc, mesh_render_pass, viewport, rc.backbuffer_rtv, rc.depth_stencil_view, view, projection);
		{
			
			rc.context->PSSetShaderResources(4, 1, &shadow_map.srv);

			
			rc.context->ClearDepthStencilView(rc.depth_stencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0);
			render_scene(rc, ch43, zrotation(PI/2));
			//render_scene(rc, ch06, translate(2, 0, 0));
			//render_scene(rc, ch15, translate(-2, 0, 0));
			render_scene(rc, sponza, identity());
			render_scene(rc, cube, translate(shadow_map.light_p) * scale(0.3));
			render_scene(rc, cube, translate(shadow_map.light_p + shadow_map.light_dir) * scale(0.1));

			v3 expexted = (translate(shadow_map.light_p) * v4{0, 0, 0, 1}).xyz;

			rc.context->PSSetShaderResources(4, 0, 0);
		}
		end_render_pass(rc, mesh_render_pass);
		end_frame(rc);

		last_mouse_p = curr_mouse_p;
	}
}