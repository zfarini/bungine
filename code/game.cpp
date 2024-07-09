#include "scene.cpp"
#include "debug.cpp"
#include "game.h"

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

void render_scene(RenderContext &rc, Scene &scene, mat4 transform)
{
	RenderPass &rp = *rc.render_pass;
    
	Constants constants = {};
	constants.view = rp.view_mat;
	constants.projection = rp.projection_mat;
	constants.camera_p = rp.camera_p;
	constants.light_transform = rc.shadow_map.projection * rc.shadow_map.view;
	
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

void game_update_and_render(Game &game, RenderContext &rc, Arena *memory, GameInput &input, float dt)
{
	if (!game.is_initialized) {
		arena_alloc(memory, sizeof(game));
		//memory->used += sizeof(game);


		D3D11_INPUT_ELEMENT_DESC input_layout_desc[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,   0, offsetof(Vertex, normal),   D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,          0, offsetof(Vertex, uv),       D3D11_INPUT_PER_VERTEX_DATA, 0 },
					//{ "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(struct Vertex, color),    D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		
		g_temp_arena.arena = make_arena(arena_alloc(memory, GigaByte(1)), GigaByte(1));
		game.asset_arena = make_arena(arena_alloc(memory, GigaByte(1)), GigaByte(1));


		rc.shadow_map = create_shadow_map(rc, 2048, 2048, 
			v3{8, 0, 7}, v3{-1, 0, -0.8},
			orthographic_projection(1, 100, 30, 30));

		game.mesh_render_pass = create_render_pass(rc, L"code\\shader.hlsl", "vs_main", "ps_main",
			input_layout_desc, ARRAYSIZE(input_layout_desc), sizeof(Constants));

		game.shadow_map_render_pass = create_render_pass(rc, L"code\\shader.hlsl", "vs_main", 0,
			input_layout_desc, ARRAYSIZE(input_layout_desc), sizeof(Constants));

		game.ch43		= load_scene(&game.asset_arena, rc, "data\\Ch43.fbx");
		//Scene ch06		= load_scene(&asset_arena, rc, "data\\Ch06.fbx");
		//Scene ch15		= load_scene(&asset_arena, rc, "data\\Ch15.fbx");
		game.sponza		= load_scene(&game.asset_arena, rc, "data\\Sponza\\Sponza.fbx");
		game.cube		= load_scene(&game.asset_arena, rc, "data\\cube.fbx");
		game.is_initialized = 1;
	}

	int window_width, window_height;
	{
		ID3D11Texture2D* backbuffer;
		rc.swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backbuffer);
		D3D11_TEXTURE2D_DESC desc;
		backbuffer->GetDesc(&desc);
		window_width = desc.Width;
		window_height = desc.Height;
	}

	mat4 view, projection;
	{
		v2 mouse_dp = (input.mouse_p - input.last_mouse_p) * 0.2f;
		if (IsDown(input, BUTTON_MOUSE_LEFT)) {
			game.camera_rotation.x += -mouse_dp.y * dt;
			game.camera_rotation.z += -mouse_dp.x * dt;
		}
		mat4 camera_transform =  zrotation(game.camera_rotation.z) * xrotation(game.camera_rotation.x);

		v3 camera_x = (camera_transform * v4{1, 0, 0, 0}).xyz;
		v3 camera_y = (camera_transform * v4{0, 0, 1, 0}).xyz;
		v3 camera_z = (camera_transform * v4{0, -1, 0, 0}).xyz;


		

		{
			v3 camera_dp = {};
			if (IsDown(input, BUTTON_CAMERA_FORWARD))
				camera_dp -= camera_z;
			if (IsDown(input, BUTTON_CAMERA_BACKWARD))
				camera_dp += camera_z;
			if (IsDown(input, BUTTON_CAMERA_LEFT))
				camera_dp -= camera_x;
			if (IsDown(input, BUTTON_CAMERA_RIGHT))
				camera_dp += camera_x;
			if (IsDown(input, BUTTON_CAMERA_UP))
				camera_dp += camera_y;
			if (IsDown(input, BUTTON_CAMERA_DOWN))
				camera_dp -= camera_y;
			game.camera_p += normalize(camera_dp) * dt * 8;
		}

		mat4 rotation = {
			camera_x.x, camera_x.y, camera_x.z, 0,
			camera_y.x, camera_y.y, camera_y.z, 0,
			camera_z.x, camera_z.y, camera_z.z, 0,
			0, 0, 0, 1
		};

		view = rotation * translate(-game.camera_p);
		projection = perspective_projection(0.1, 100, 90, (float)window_height / window_width);
	}

	begin_frame(rc);
	
	D3D11_VIEWPORT viewport = {};
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;

	FLOAT color[] = { 0.392f, 0.584f, 0.929f, 1.f };
	rc.context->ClearRenderTargetView(rc.backbuffer_rtv, color);

	viewport.Width = (FLOAT)rc.shadow_map.width;
	viewport.Height = (FLOAT)rc.shadow_map.height;
	begin_render_pass(rc, game.shadow_map_render_pass, viewport, 0, rc.shadow_map.dsv, rc.shadow_map.view, rc.shadow_map.projection, rc.shadow_map.light_p);
	{
		rc.context->ClearDepthStencilView(rc.shadow_map.dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);

		render_scene(rc, game.ch43, zrotation(PI/2));
		//render_scene(rc, ch06, translate(2, 0, 0));
		//render_scene(rc, ch15, translate(-2, 0, 0));
		render_scene(rc, game.sponza, identity());
	}
	end_render_pass(rc, game.shadow_map_render_pass);
	
	viewport.Width = (FLOAT)window_width;
	viewport.Height = (FLOAT)window_height;
	
	begin_render_pass(rc, game.mesh_render_pass, viewport, rc.backbuffer_rtv, rc.depth_stencil_view, view, projection, game.camera_p);
	{
		rc.context->PSSetShaderResources(4, 1, &rc.shadow_map.srv);

		rc.context->ClearDepthStencilView(rc.depth_stencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0);
		render_scene(rc, game.ch43, zrotation(PI/2));
		//render_scene(rc, ch06, translate(2, 0, 0));
		//render_scene(rc, ch15, translate(-2, 0, 0));
		render_scene(rc, game.sponza, identity());
		render_scene(rc, game.cube, translate(rc.shadow_map.light_p) * scale(0.3));
		render_scene(rc, game.cube, translate(rc.shadow_map.light_p + rc.shadow_map.light_dir) * scale(0.1));

		v3 expexted = (translate(rc.shadow_map.light_p) * v4{0, 0, 0, 1}).xyz;

		rc.context->PSSetShaderResources(4, 0, 0);
	}
	end_render_pass(rc, game.mesh_render_pass);
	end_frame(rc);
}