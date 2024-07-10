#include "scene.cpp"
#include "debug.cpp"
#include "game.h"

Entity *make_entity(Game &game, v3 position, Scene *scene, mat4 scene_transform = identity())
{
	Entity e = {};

	e.position = position;
	e.scene = scene;
	e.scene_transform = scene_transform;

	game.entities.push(e);
	return &game.entities[game.entities.count - 1];
}

void compute_bone_transform(Array<Bone> bones, int i, Array<bool> computed)
{
	if (computed[i] || bones[i].parent == -1)
		return ;
	compute_bone_transform(bones, bones[i].parent, computed);
	bones[i].transform = bones[bones[i].parent].transform *  bones[i].transform;
	computed[i] = true;
}

void render_bones(RenderContext &rc, Array<Bone> bones, mat4 transform, Animation *anim, float anim_time)
{
	if (bones.count == 0)
		return ;

	Arena *temp = begin_temp_memory();

	Array<Bone> anim_bones = clone_array(temp, bones);

	if (anim) {
		for (usize i = 0; i < bones.count; i++) {
			int index = -1;
			for (usize j = 0; j < anim->nodes.count; j++) {
				if (strings_equal(anim->nodes[j].name, anim_bones[i].name)) {
					index = (int)j;
					break ;
				}
			}
			if (index != -1) 
				anim_bones[i].transform = get_animated_node_transform(*anim, anim->nodes[index], anim_time);
		}
	}

	Array<bool> computed = make_array<bool>(temp, anim_bones.count);
	for (int i = 0; i < anim_bones.count; i++)
		computed[i] = false;
	for (int i = 0; i < anim_bones.count; i++)
		compute_bone_transform(anim_bones, i, computed);
	for (int i = 0; i < anim_bones.count; i++)
		anim_bones[i].transform = translate(-2, 0, 0) * transform * anim_bones[i].transform;

	for (usize i = 0; i < anim_bones.count; i++) {
		v3 P = (anim_bones[i].transform * v4{0, 0, 0, 1}).xyz;

		if (anim_bones[i].parent != -1) {
			v3 parentP = (anim_bones[anim_bones[i].parent].transform * v4{0, 0, 0, 1}).xyz;
			push_line(rc, P, parentP);
		}
	}

	end_temp_memory();
}

Array<Bone> get_animated_bones(Arena *arena, Array<Bone> bones, mat4 transform, Animation *anim, float anim_time)
{
	Array<Bone> anim_bones = clone_array(arena, bones);

	for (usize i = 0; i < bones.count; i++) {
		int index = -1;
		for (usize j = 0; j < anim->nodes.count; j++) {
			if (strings_equal(anim->nodes[j].name, anim_bones[i].name)) {
				index = (int)j;
				break ;
			}
		}
		if (index != -1) {
			anim_bones[i].transform = get_animated_node_transform(*anim, anim->nodes[index], anim_time);
		}
		//anim_bones[i].transform = inverse(bones[i].transform);
		//anim_bones[i].transform = transform * anim_bones[i].transform;
	}

	Array<bool> computed = make_zero_array<bool>(arena, anim_bones.count);
	for (int i = 0; i < anim_bones.count; i++)
		compute_bone_transform(anim_bones, i, computed);
	for (int i = 0; i < anim_bones.count; i++)
		anim_bones[i].transform = transform * anim_bones[i].transform;
	return anim_bones;
}

void render_scene(RenderContext &rc, Scene &scene, SceneNode *node, mat4 scene_transform, mat4 node_transform,
	Animation *anim, float anim_time)
{
	if (node->skip_render)
		return ;

	mat4 local_transform = node->local_transform;
	if (anim) {
		//assert(anim->nodes.count == scene.nodes.count);
		//local_transform = ;
		//local_transform = get_animated_node_transform(*anim, anim->nodes[node->id], anim_time);

		for (usize j = 0; j < anim->nodes.count; j++) {
			if (strings_equal(anim->nodes[j].name, node->name)) {
				local_transform = get_animated_node_transform(*anim, anim->nodes[j], anim_time);
				break ;
			}
		}
	}
	node_transform = node_transform * local_transform;
		
	if (node->mesh) {
		Mesh &mesh = *node->mesh;

		RenderPass &rp = *rc.render_pass;
		Constants constants = {};
		constants.view = rp.view_mat;
		constants.projection = rp.projection_mat;
		constants.camera_p = rp.camera_p;
		constants.light_transform = rc.shadow_map.projection * rc.shadow_map.view;
			
		mat4 mesh_transform = scene_transform * node_transform * node->geometry_transform;

		render_bones(rc, mesh.bones, mesh_transform, anim, anim_time);

		UINT32 stride = sizeof(Vertex);
		UINT32 offset = 0;
		rc.context->IASetVertexBuffers(0, 1, &mesh.vertex_buffer, &stride, &offset);

		constants.model = mesh_transform;
		//constants.normal_transform = inverse(transpose(constants.model));
		if (mesh.bones.count) {
			constants.skinned = 1;
			Arena *temp  = begin_temp_memory();
			Array<Bone> bones = get_animated_bones(temp, mesh.bones, mesh_transform, anim, anim_time);
			for (usize j = 0; j < bones.count; j++)
				constants.bones[j] = bones[j].transform * bones[j].inv_bind;
			end_temp_memory();
		}

		for (usize j = 0; j < mesh.parts.count; j++) {
			MeshPart &part = mesh.parts[j];

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

			copy_data_to_gpu_buffer(rc, rp.cbuffer, &constants, sizeof(constants));	
			rc.context->Draw((UINT)part.vertices_count, (UINT)part.offset);
		}
	}

	for (usize i = 0; i < node->childs.count; i++)
		render_scene(rc, scene, node->childs[i], scene_transform, node_transform, anim, anim_time);
}

void render_scene(RenderContext &rc, Scene &scene, mat4 transform = identity(), Animation *anim = 0, float anim_time = 0)
{
	if (anim)
		anim_time = fmod(anim_time, anim->duration);
	render_scene(rc, scene, scene.root, transform, identity(), anim, anim_time);
}

void render_entities(RenderContext &rc, Game &game)
{
	for (usize i = 0; i < game.entities.count; i++) {
		Entity &e = game.entities[i];
		if (!e.scene)
			continue ;
		mat4 transform = translate(e.position);
		render_scene(rc, *e.scene, transform * e.scene_transform, e.animation, e.anim_time);
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
			{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,          0, offsetof(Vertex, weights),       D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDINDICES", 0, DXGI_FORMAT_R32_SINT,          0, offsetof(Vertex, indices),       D3D11_INPUT_PER_VERTEX_DATA, 0 },

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
		//game.ch43 = load_scene(&game.asset_arena, rc, "data\\animated_pistol\\animated.fbx");
		//game.ch43 = load_scene(&game.asset_arena, rc, "data\\animated_pistol\\animated.fbx");
		game.sponza		= load_scene(&game.asset_arena, rc, "data\\Sponza\\Sponza.fbx");
		game.cube		= load_scene(&game.asset_arena, rc, "data\\cube.fbx");
		
		//game.ch43.root->local_transform = translate(0, 0, 4) * game.ch43.root->local_transform * scale(0.005f);
		game.test_anim = load_scene(&game.asset_arena, rc, "data\\Fast Run.fbx").animations[0];

		game.entities = make_array_max<Entity>(memory, 4096);


		Entity *player = make_entity(game, V3(0), &game.ch43);
		player->animation = &game.test_anim;

		Entity *sponza = make_entity(game, V3(0), &game.sponza);
			
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
	//	if (IsDown(input, BUTTON_MOUSE_LEFT)) {
			game.camera_rotation.x += -input.mouse_dp.y * dt * 0.2f;
			game.camera_rotation.z += -input.mouse_dp.x * dt * 0.2f;
	//	}
		// avoid gimbal lock
		if (game.camera_rotation.x > PI / 2 - 0.001f)
			game.camera_rotation.x = PI / 2 - 0.001f;
		if (game.camera_rotation.x < -PI / 2 + 0.001f)
			game.camera_rotation.x = -PI / 2 + 0.001f;
		
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

	for (usize i = 0; i < game.entities.count; i++)
		game.entities[i].anim_time = game.time;

	begin_frame(rc, view, projection);
	
	push_line(rc, V3(0), V3(5, 0, 0), V3(1, 0, 0));
	push_line(rc, V3(0), V3(0, 5, 0), V3(0, 1, 0));
	push_line(rc, V3(0), V3(0, 0, 5), V3(0, 0, 1));

	FLOAT color[] = { 0.392f, 0.584f, 0.929f, 1.f };
	rc.context->ClearRenderTargetView(rc.backbuffer_rtv, color);

	D3D11_VIEWPORT viewport = {};
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.Width = (FLOAT)rc.shadow_map.width;
	viewport.Height = (FLOAT)rc.shadow_map.height;


	begin_render_pass(rc, game.shadow_map_render_pass, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 
		viewport, 0, rc.shadow_map.dsv, rc.shadow_map.view, rc.shadow_map.projection, rc.shadow_map.light_p);
	{
		rc.context->ClearDepthStencilView(rc.shadow_map.dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);
		render_entities(rc, game);
	}
	end_render_pass(rc);
#if 1
	begin_render_pass(rc, game.mesh_render_pass, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		rc.window_viewport, rc.backbuffer_rtv, rc.depth_stencil_view, view, projection, game.camera_p);
	{
		rc.context->PSSetShaderResources(4, 1, &rc.shadow_map.srv);

		rc.context->ClearDepthStencilView(rc.depth_stencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0);
		render_entities(rc, game);

		v3 expexted = (translate(rc.shadow_map.light_p) * v4{0, 0, 0, 1}).xyz;

		ID3D11ShaderResourceView *null_srv = 0;
		rc.context->PSSetShaderResources(4, 1, &null_srv);
	}
	end_render_pass(rc);
	#endif

	push_cube_outline(rc, rc.shadow_map.light_p, V3(0.3));
	push_line(rc, rc.shadow_map.light_p, rc.shadow_map.light_p + 0.5 * rc.shadow_map.light_dir);

	end_frame(rc);

	game.time += dt;
}