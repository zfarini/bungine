// #include <imgui/imgui.h>

// #include "common.h"
// #include "arena.h"
// #include "utils.h"
// #include "math.h"
// #include "platform.h"
// #if 0
// #define DIRECT3D_DEBUG
// #include "renderer.cpp"
// global RenderContext g_rc;
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#pragma comment (lib, "dxgi")
#pragma comment (lib, "dxguid")

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <dxgi1_3.h>
#include <d3dcompiler.h>
#include <dxgidebug.h>
#undef min
#undef max
#undef near
#undef swap
#include <assert.h>
#include <stdint.h>
#include <cstdio>
#include <float.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include <ufbx.h>
#include <stb_image.h>

#include "common.h"
#include "arena.h"
#include "utils.h"
#include "math.h"
#include "platform.h"

#include "renderer.h"

global RenderContext *g_rc;
#ifdef RENDERER_DX11
#define DIRECT3D_DEBUG
#include "renderer_dx11.cpp"
#else
#include "renderer_opengl.cpp"
#endif
#include "renderer.cpp"

#include "debug.cpp"
#include "scene.cpp"
#include "game.h"


bool ray_hit_plane(v3 ray_origin, v3 ray_dir, 
	v3 plane_normal, v3 plane_point, float *hit_t)
{
	float denom = dot(ray_dir, plane_normal);
	if (fabsf(denom) < 1e-5)
		return false;
	float t = (dot(plane_normal, plane_point) - dot(ray_origin, plane_normal)) / denom;
	if (t >= 0) {
		if (hit_t)
			*hit_t = t;
		return true;
	}
	return false;
}

float ray_hit_box(v3 ray_origin, v3 ray_dir, v3 box_center, v3 box_xaxis,
	v3 box_yaxis, v3 box_zaxis)
{
	struct  {
		v3 normal;
		v3 p;
	} planes[6];

	planes[0] = {box_xaxis, box_center + box_xaxis};
	planes[1] = {-box_xaxis, box_center - box_xaxis};
	planes[2] = {box_yaxis, box_center + box_yaxis};
	planes[3] = {-box_yaxis, box_center - box_yaxis};
	planes[4] = {box_zaxis, box_center + box_zaxis};
	planes[5] = {-box_zaxis, box_center - box_zaxis};

	float lx = length(box_xaxis);
	float ly = length(box_yaxis);
	float lz = length(box_zaxis);

	v3 xaxis = normalize(box_xaxis);
	v3 yaxis = normalize(box_yaxis);
	v3 zaxis = normalize(box_zaxis);

	float min_t = FLT_MAX;
	for (int i = 0; i < 6; i++) {
		/*
			dot(O + t * D, normal) = dot(normal, p)

			t * dot(D, normal) = dot(normal, p) - dot(O, normal) / dot(D, normal)

		*/
		float denom = dot(ray_dir, planes[i].normal);
		if (fabsf(denom) < 1e-6)
			continue ;
		float t = (dot(planes[i].normal, planes[i].p) - dot(ray_origin, planes[i].normal)) / denom;
		if (t >= 0 && t < min_t) {
			v3 p = ray_origin + t * ray_dir - box_center;
			float eps = 1e-4;
			if (dot(p, xaxis) >= -lx - eps && dot(p, xaxis) <= lx + eps &&
				dot(p, yaxis) >= -ly - eps && dot(p, yaxis) <= ly + eps &&
				dot(p, zaxis) >= -lz - eps && dot(p, zaxis) <= lz + eps)
				min_t = t;
		}
	}
	if (min_t == FLT_MAX)
		return -1;
	return min_t;
}

ShadowMap create_shadow_map(int texture_width, int texture_height,
	v3 light_p, v3 light_dir, mat4 projection, v3 up = V3(0, 0, 1))
{
	ShadowMap shadow_map = {};

// @CHECKIN
#if 0
	shadow_map.width = texture_width;
	shadow_map.height = texture_height;
	shadow_map.light_p = light_p;
	shadow_map.light_dir = light_dir;

	shadow_map.view = lookat(shadow_map.light_p, shadow_map.light_dir, up);
	shadow_map.projection = projection;

	shadow_map.depth_texture = create_depth_texture(texture_width, texture_height);
	
	shadow_map.framebuffer = create_frame_buffer();

	bind_framebuffer_depthbuffer(shadow_map.framebuffer, shadow_map.depth_texture);
	// TODO:!!
	 if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        assert(0);
#endif
	return shadow_map;
}

void compute_bone_transform(Array<Bone> bones, int i, Array<bool> computed)
{
	if (computed[i] || bones[i].parent == -1)
		return ;
	compute_bone_transform(bones, bones[i].parent, computed);
	bones[i].transform = bones[bones[i].parent].transform *  bones[i].transform;
	computed[i] = true;
}

void render_bones(Array<Bone> bones, mat4 transform, Animation *anim, float anim_time)
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
				anim_bones[i].transform = anim->nodes[index].transform;//get_animated_node_transform(*anim, anim->nodes[index], anim_time);
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
			push_line(P, parentP);
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
			anim_bones[i].transform = anim->nodes[index].transform;//get_animated_node_transform(*anim, anim->nodes[index], anim_time);
		}
	}

	Array<bool> computed = make_zero_array<bool>(arena, anim_bones.count);
	for (int i = 0; i < anim_bones.count; i++)
		compute_bone_transform(anim_bones, i, computed);
	for (int i = 0; i < anim_bones.count; i++)
		anim_bones[i].transform = transform * anim_bones[i].transform;
	return anim_bones;
}

void render_scene(Game &game, Scene &scene, Camera camera, SceneNode *node, mat4 scene_transform, mat4 node_transform,
	Animation *anim, float anim_time, v3 color)
{
	if (node->skip_render)
		return ;

	mat4 local_transform = node->local_transform;
	if (anim) {
		for (usize j = 0; j < anim->nodes.count; j++) {
			if (strings_equal(anim->nodes[j].name, node->name)) {
				local_transform = anim->nodes[j].transform;//get_animated_node_transform(*anim, anim->nodes[j], anim_time);
				break ;
			}
		}
	}
	node_transform = node_transform * local_transform;
		
	if (node->mesh) {
		Mesh &mesh = *node->mesh;

		Constants constants = {};
		constants.view = camera.view;
		constants.projection = camera.projection;
		constants.camera_p = camera.position;
		constants.light_transform = game.shadow_map.projection * game.shadow_map.view;
		constants.color = color;

		mat4 mesh_transform = scene_transform * node_transform * node->geometry_transform;

		constants.model = mesh_transform;
		//constants.normal_transform = inverse(transpose(constants.model));
		if (mesh.bones.count && anim) {
			constants.skinned = 1;
			Arena *temp  = begin_temp_memory();
			Array<Bone> bones = get_animated_bones(temp, mesh.bones, mesh_transform, anim, anim_time);
			for (usize j = 0; j < bones.count; j++)
				constants.bones[j] = bones[j].transform * bones[j].inv_bind;
			end_temp_memory();
		}

		for (usize j = 0; j < mesh.parts.count; j++) {
			MeshPart &part = mesh.parts[j];

			bind_texture(0, part.material.diffuse.valid ? part.material.diffuse : g_rc->white_texture);
			bind_texture(1, part.material.specular.valid ? part.material.specular : g_rc->white_texture);
			bind_texture(2, part.material.normal_map);
			bind_texture(3, part.material.specular_exponent.valid ? part.material.specular_exponent : g_rc->white_texture);
			
			constants.diffuse_factor = part.material.diffuse_factor;
			constants.specular_factor = part.material.specular_factor;
			constants.specular_exponent_factor = part.material.specular_exponent_factor;
			constants.has_normal_map = part.material.normal_map.valid;

			update_constant_buffer(game.constant_buffer, &constants);
			bind_vertex_buffer(mesh.vertex_buffer);
			draw((int)part.offset, (int)part.vertices_count);
		}
	}

	for (usize i = 0; i < node->childs.count; i++)
		render_scene(game, scene, camera, node->childs[i], scene_transform, node_transform, anim, anim_time, color);
}

void render_scene(Game &game, Scene &scene, Camera camera, mat4 transform, Animation *anim, float anim_time, v3 color)
{
	if (anim)
		anim_time = fmod(anim_time, anim->duration);
	render_scene(game, scene, camera, scene.root, transform, identity(), anim, anim_time, color);
}

mat4 get_entity_transform(Entity &e)
{
    return translate(e.position) 
        * zrotation(e.rotation.z) * yrotation(e.rotation.y) * xrotation(e.rotation.x) * scale(e.scale);
}

#include "collision.cpp"
#include "world.cpp"
#include "ai.cpp"

extern "C" void game_update_and_render(PlatformData &platform_data, Arena *memory, GameInput &input, float dt)
{
	g_rc = (RenderContext *)platform_data.render_context;
	ImGui::SetCurrentContext((ImGuiContext *)platform_data.imgui_context);
	g_temp_arena = platform_data.temp_arena;

	Game &game = *((Game *)memory->data);
	if (!game.is_initialized) {


		assert(memory->used == 0);
		arena_alloc(memory, sizeof(game));

		usize temp_arena_size = Megabyte(128);
		g_temp_arena->arena = make_arena(arena_alloc(memory, temp_arena_size), temp_arena_size);


		init_render_context(memory, *g_rc);
		//memory->used += sizeof(game);

		//game.world = (World *)arena_alloc(memory, sizeof(World));
		//memset(game.world, 0, sizeof(*game.world));
		game.world = new World(); // TODO: using unordered_map for now
		World &world = *game.world;

		world.arena = make_arena(arena_alloc(memory, Megabyte(64)), Megabyte(64));

		game.asset_arena = make_arena(arena_alloc(memory, Megabyte(256)), Megabyte(256));
		
		game.default_rasterizer_state = create_rasterizer_state(RASTERIZER_FILL_SOLID, RASTERIZER_CULL_NONE);
		game.default_depth_stencil_state = create_depth_stencil_state(true);
		game.disable_depth_state = create_depth_stencil_state(false);

		game.mesh_render_pass = create_render_pass(
			load_shader(make_cstring("code/shader.hlsl"), SHADER_TYPE_VERTEX, "vs_main"),
			load_shader(make_cstring("code/shader.hlsl"), SHADER_TYPE_FRAGMENT, "ps_main"),
			make_array<VertexInputElement>(g_vertex_input_elements, ARRAY_SIZE(g_vertex_input_elements)),
			PRIMITIVE_TRIANGLES, game.default_depth_stencil_state, game.default_rasterizer_state);

		// @CHECKIN
		#if 0
		game.shadow_map_render_pass = create_render_pass(
			create_shader_program(
				load_shader(make_cstring("vertex.glsl"), SHADER_TYPE_VERTEX),
				load_shader(make_cstring("shadow_map_fs.glsl"), SHADER_TYPE_FRAGMENT)),
			g_vertex_input_elements, ARRAY_SIZE(g_vertex_input_elements));

		game.shadow_map = create_shadow_map(4096, 4096, 
			V3(24, 0, 24), V3(-1, 0, -1),
			orthographic_projection(1, 75, 50, 40));
		#endif
		{
			VertexInputElement input_elements[] = {
				{0, 3, INPUT_ELEMENT_FLOAT, "POSITION"},
				{sizeof(v3), 3, INPUT_ELEMENT_FLOAT, "COLOR"},
			};

			game.debug_lines_render_pass = create_render_pass(
				load_shader(make_cstring("code/debug_line_shader.hlsl"), SHADER_TYPE_VERTEX, "vs_main"),
				load_shader(make_cstring("code/debug_line_shader.hlsl"), SHADER_TYPE_FRAGMENT, "ps_main"),
				make_array<VertexInputElement>(input_elements, ARRAY_SIZE(input_elements)),
				PRIMITIVE_LINES, game.disable_depth_state, game.default_rasterizer_state);

			game.debug_lines_vertex_buffer = create_vertex_buffer(VERTEX_BUFFER_DYNAMIC,
				2 * sizeof(v3), g_rc->debug_lines.capacity * sizeof(v3));

			ConstantBufferElement elems[] = {
				{CONSTANT_BUFFER_ELEMENT_MAT4},
			};
			game.debug_lines_constant_buffer = create_constant_buffer(make_array<ConstantBufferElement>(elems, ARRAY_SIZE(elems)));
		}

			//perspective_projection(1, 75, 120, 1));

		ConstantBufferElement elems[] = {
			{CONSTANT_BUFFER_ELEMENT_MAT4},
			{CONSTANT_BUFFER_ELEMENT_MAT4},
			{CONSTANT_BUFFER_ELEMENT_MAT4},
			{CONSTANT_BUFFER_ELEMENT_MAT4},
			{CONSTANT_BUFFER_ELEMENT_MAT4, 96},
			{CONSTANT_BUFFER_ELEMENT_VEC3},
			{CONSTANT_BUFFER_ELEMENT_VEC3},
			{CONSTANT_BUFFER_ELEMENT_VEC3},
			{CONSTANT_BUFFER_ELEMENT_FLOAT},
			{CONSTANT_BUFFER_ELEMENT_FLOAT},
			{CONSTANT_BUFFER_ELEMENT_FLOAT},
			{CONSTANT_BUFFER_ELEMENT_INT},
			{CONSTANT_BUFFER_ELEMENT_INT},
		};
		game.constant_buffer = create_constant_buffer(make_array<ConstantBufferElement>(elems, ARRAY_SIZE(elems)));

		game.ch43		= load_scene(&game.asset_arena, "data/YBot.fbx");
		//Scene ch06		= load_scene(&asset_arena, "data\\Ch06.fbx");
		//Scene ch15		= load_scene(&asset_arena, "data\\Ch15.fbx");
		//game.ch43 = load_scene(&game.asset_arena, "data\\animated_pistol\\animated.fbx");
		//game.ch43 = load_scene(&game.asset_arena, "data\\animated_pistol\\animated.fbx");
		//game.sponza		= load_scene(&game.asset_arena, rc, "data\\Sponza\\Sponza.fbx");
		game.cube_asset		= load_scene(&game.asset_arena, "data/cube.fbx");
		game.sphere_asset	= load_scene(&game.asset_arena, "data/sphere.fbx");

		//game.ch43.root->local_transform = translate(0, 0, 4) * game.ch43.root->local_transform * scale(0.005f);
		//game.test_anim = load_scene(&game.asset_arena, rc, "data\\Fast Run.fbx").animations[0];

		game.animations[ANIMATION_JUMP] = load_scene(&game.asset_arena, "data/jump.fbx").animations[0];
		game.animations[ANIMATION_SHOOT] = load_scene(&game.asset_arena, "data/shoot.fbx").animations[0];
		game.animations[ANIMATION_RUN] = load_scene(&game.asset_arena, "data/run.fbx").animations[0];
		game.animations[ANIMATION_FORWARD_GUN_WALK] = load_scene(&game.asset_arena, "data/forward_gun_walk.fbx").animations[0];
		game.animations[ANIMATION_BACKWARD_GUN_WALK] = load_scene(&game.asset_arena, "data/backward_gun_walk.fbx").animations[0];
		game.animations[ANIMATION_GUN_IDLE] = load_scene(&game.asset_arena, "data/gun_idle.fbx").animations[0];

		world.entities = make_array_max<Entity>(&world.arena, 4096);


		
		
		//Entity *sh = make_entity(game, EntityType_Player, &game.sphere_asset, V3(0, 0, 2), V3(0.3, 0.3, 0.8) );
		
		// for (int i = 0; i < 2; i++)
		// 	for (int j = 0; j < 2; j++) {
		// 	Entity *enemy = make_entity(game, EntityType_Enemy, &game.ch43, V3(i * 2, j * 2, 6), player->shape);
		// 	enemy->scene_transform = player->scene_transform;
		// 	enemy->speed = ((float)rand() / RAND_MAX)*50 + 20;
		// }

		//player->rotation.x = -PI/8;

		//Entity *sponza = make_entity(game, V3(0), &game.sponza);



		// {
		// 	//@HACK
		// 	auto &jump = game.animations[ANIMATION_JUMP];
		// 	auto &node = jump.nodes[0];
		// 	for (int i = 0; i < (int)(node.position.count*0.7; i++)
		// 		node.position[i].y -= 30;
		// }

		Entity *boxes[] = {
        	make_entity(world, EntityType_Static, &game.cube_asset, V3(0, 0, -0.5), make_box_shape(memory, V3(25, 25, 0.5))),
        	make_entity(world, EntityType_Static, &game.cube_asset, V3(0, 25, 0), make_box_shape(memory, (V3(25, 0.5, 25)))),
        	make_entity(world, EntityType_Static, &game.cube_asset, V3(10, 6, 1), make_box_shape(memory, (V3(5, 5.8, 0.3)))),
        	make_entity(world, EntityType_Static, &game.cube_asset, V3(10, 6, 1.3), make_box_shape(memory, (V3(2.5, 2.5, 0.3)))),
        	make_entity(world, EntityType_Static, &game.cube_asset, V3(10, 6, 1.6), make_box_shape(memory, (V3(1.25, 1.25, 0.3)))),
        	make_entity(world, EntityType_Static, &game.cube_asset, V3(-10, 6, 1), make_box_shape(memory, (V3(7, 7, 0.3)))),
        	make_entity(world, EntityType_Static, &game.cube_asset, V3(1, 6, 0),  make_box_shape(memory, (V3(4, 1, 2)))),
        	make_entity(world, EntityType_Static, &game.cube_asset, V3(0, -7, 1), make_box_shape(memory, (V3(4, 4, 0.3)))),
        	make_entity(world, EntityType_Static, &game.cube_asset, V3(0, -7, 1.6), make_box_shape(memory, (V3(2, 2, 0.3)))),

			make_entity(world, EntityType_Static, &game.cube_asset, V3(7, -7, 0.1), make_box_shape(memory, (V3(3, 3, 0.1)))),

		};
		for (int i = 0; i < ARRAY_SIZE(boxes); i++) {
			//boxes[i]->scene_transform = scale();
			boxes[i]->scale = boxes[i]->shape.box_radius;
			boxes[i]->shape = make_box_shape(memory, V3(1));
			
			float r = (float)rand() / RAND_MAX;
			float g = (float)rand() / RAND_MAX;
			float b = (float)rand() / RAND_MAX;

			boxes[i]->color = V3(r, g, b);

		}
		Entity *player = make_entity(world, EntityType_Player, &game.ch43, V3(0, 0, 8), make_ellipsoid_shape(V3(0.55f, 0.55f, 0.95f)));
		world.player_id = player->id;
		//player->scene = &game.sphere_asset;
		//player->scene = 0;
		//player->shape.ellipsoid_radius = V3(0.2, 0.2, 0.2); 
		//player->scene_transform =  scale(player->shape.ellipsoid_radius);
		player->scene_transform =  translate(0, 0, -player->shape.ellipsoid_radius.z) * zrotation(3*PI/2) * scale(V3(1.1));
		player->color = V3(0.2, 0.8, 0.8);

		world.editor_camera_p = V3(0, 0, 3);



		game.is_initialized = 1;
	}

	World &world = *game.world;

	if (IsDownFirstTime(input, BUTTON_F1)) {
		game.in_editor = !game.in_editor;
	}

	begin_render_frame();
	
	if (!game.in_editor) {
		update_player(game, world, input, dt);
		update_enemies(game, world, input, dt);
	}

	Camera game_camera = update_camera(game, world, input, dt);
	if (game.in_editor)
	{

		v2 mouse_p = (input.mouse_p * V2(1.f / g_rc->window_width, 1.f / g_rc->window_height)) * 2 - V2(1);
		mouse_p.y *= -1;
		v3 ray_origin = game_camera.position;
		v3 ray_dir = game_camera.forward * game_camera.znear 
			+ mouse_p.x * game_camera.right * game_camera.width * 0.5f
			+ mouse_p.y * game_camera.up * game_camera.height * 0.5f;

		entity_id hit_id = 0;

		float min_t = FLT_MAX;
		for (usize i = 0; i < world.entities.count; i++) {
			Entity &e = world.entities[i];
			if (!e.scene)
				continue ;

			mat4 transform = get_entity_transform(e) * e.scene_transform;
			for (usize j = 0; j < e.scene->meshes.count; j++) {
				Mesh &mesh = e.scene->meshes[j];
				mat4 mesh_transform = transform * mesh.transform;

				v3 x_axis = normalize((mesh_transform * V4(1, 0, 0, 0)).xyz);
				v3 y_axis = normalize((mesh_transform * V4(0, 1, 0, 0)).xyz);
				v3 z_axis = normalize((mesh_transform * V4(0, 0, 1, 0)).xyz);

				v3 a = (mesh_transform * V4(mesh.box_min, 1)).xyz;
				v3 b = (mesh_transform * V4(mesh.box_max, 1)).xyz;

				v3 c = (a + b) * 0.5f;

				x_axis *= fabsf(dot((b - a)*0.5f, x_axis));
				y_axis *= fabsf(dot((b - a)*0.5f, y_axis));
				z_axis *= fabsf(dot((b - a)*0.5f, z_axis));


				//push_box_outline(c, x_axis, y_axis, z_axis);
				float t = ray_hit_box(ray_origin, ray_dir, c, x_axis, y_axis, z_axis);
				if (t >= 0 && t < min_t) {
					hit_id = e.id;
					min_t = t;
				}
				
				//ray_hit_box();
			}
		}
		bool should_switch = true;
		{
			Entity *e = get_entity(world, world.editor_selected_entity);
			if (e) {
				mat4 transform = get_entity_transform(*e);

				v3 axis[3];

				for (int i = 0; i < 3; i++) {
					//axis[i] = V3(transform.e[0][i], transform.e[1][i], transform.e[2][i]);
					axis[i] = {};
					axis[i].e[i] = 1;
					axis[i] = normalize(axis[i]) * 2;
				}
				


				if (game.dragging_axis == 0) {
					game.dragging_axis = 0;
					game.drag_p = {};
					game.did_drag = false;
					game.drag_org_camera_p = game_camera.position;

					float start_t = min_t;
					min_t = FLT_MAX;
					int best = 0;
					for (int i = 0; i < 3; i++) {
						float t = ray_hit_box(ray_origin, ray_dir, e->position + 0.5f * axis[i], 
							axis[0] * (i == 0 ? 0.5f : 0.25f),
							axis[1] * (i == 1 ? 0.5f : 0.25f), 
							axis[2] * (i == 2 ? 0.5f : 0.25f));
						if (t >= 0 && t < min_t) {
							best = i + 1;
							min_t = t;
						}
					}
					if (best)
						game.dragging_axis = best;
					if (hit_id != world.editor_selected_entity && min_t < start_t)
						should_switch = false;
				}
				

				if (IsDown(input, BUTTON_MOUSE_RIGHT)) {
					int a = game.dragging_axis - 1;

					v3 plane_normal = -ray_dir;
					plane_normal = -cross(axis[a], cross(axis[a], game_camera.position - e->position));
					//plane_normal = game_camera.position - e->position;
					float hit_t;
					if (ray_hit_plane(ray_origin, ray_dir, plane_normal, e->position, &hit_t)) {
						v3 hit_p = ray_origin + hit_t * ray_dir;
						//push_cube_outline(hit_p, V3(0.01f), V3(0.2));
						v3 dp = normalize(axis[a]) * dot(hit_p - e->position, normalize(axis[a]));
					

						if (!game.did_drag) {
							game.did_drag = true;
							game.drag_p = dp;
						}
						else
							e->position += dp - game.drag_p;
					}
					//push_line(e->position, e->position + normalize(plane_normal) * 2);
					
				}
				else {
					game.dragging_axis = 0;
					game.drag_p = {};
					game.did_drag = false;
				}

				for (int i = 0; i < 3; i++) {
					v3 c = game.dragging_axis == i + 1 ? V3(1, 1, 0) : V3(0);
					// push_box_outline(e->position + 0.5f * axis[i],
					// 		axis[0] * (i == 0 ? 0.5f : 0.25f),
					// 		axis[1] * (i == 1 ? 0.5f : 0.25f), 
					// 		axis[2] * (i == 2 ? 0.5f : 0.25f)
					// 		, c);
					v3 color = {};
					color.e[i] = 1;
					if (game.dragging_axis == i + 1)
						color = V3(1, 0, 1);
					push_line(e->position, e->position + axis[i], color);
				}
			}
		}
		if (should_switch && IsDownFirstTime(input, BUTTON_MOUSE_RIGHT))
			world.editor_selected_entity = hit_id;
	}
	
	
	push_cube_outline(game.shadow_map.light_p, V3(0.3));
	push_line(game.shadow_map.light_p, game.shadow_map.light_p + 0.5 * game.shadow_map.light_dir);

	// @CHECKIN
	#if 0
	begin_render_pass(game.shadow_map_render_pass);
	{
		set_viewport(0, 0, game.shadow_map.width, game.shadow_map.height);
		bind_frame_buffer(game.shadow_map.framebuffer);
		clear_depth_buffer(1);
		render_entities(game, world, Camera{game.shadow_map.light_p, game.shadow_map.view, game.shadow_map.projection});
	}
	end_render_pass();
	#endif

	begin_render_pass(game.mesh_render_pass);
	{
		set_viewport(0, 0, g_rc->window_width, g_rc->window_height);

		bind_framebuffer(g_rc->window_framebuffer);
		clear_framebuffer_color(g_rc->window_framebuffer, V4(0.392f, 0.584f, 0.929f, 1.f));
		clear_framebuffer_depth(g_rc->window_framebuffer, 1);
		// @CHECKIN
		//bind_texture(4, game.shadow_map.depth_texture);
		render_entities(game, world, game_camera);
	}
	end_render_pass();

	// push_line(V3(0), V3(5, 0, 0.01), V3(1, 0, 0));
	// push_line(V3(0), V3(0, 5, 0.01), V3(0, 1, 0));
	// push_line(V3(0), V3(0, 0, 5), V3(0, 0, 1));
	// @CHECKIN
	begin_render_pass(game.debug_lines_render_pass);
	{
		//glDisable(GL_DEPTH_TEST);
		update_vertex_buffer(game.debug_lines_vertex_buffer, (int)g_rc->debug_lines.count * sizeof(v3),
			g_rc->debug_lines.data);
		mat4 mvp = game_camera.projection * game_camera.view;
		bind_constant_buffer(game.debug_lines_constant_buffer);
		update_constant_buffer(game.debug_lines_constant_buffer, &mvp);
		bind_vertex_buffer(game.debug_lines_vertex_buffer);;
		draw(0, (int)(g_rc->debug_lines.count / 2));
		//glEnable(GL_DEPTH_TEST);
	}
	end_render_pass();


	{
		ImGuiIO &io = ImGui::GetIO();
		ImGui::Begin("debug");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::Text("resolution: %dx%d", g_rc->window_width, g_rc->window_height);
		ImGui::Checkbox("debug collission", &game.debug_collision);
        ImGui::End();
	}

	{
		Entity *e = get_entity(world, world.editor_selected_entity);

		if (e) {
			ImGuiIO &io = ImGui::GetIO();

			v3 r = e->rotation * RAD2DEG;
			
			ImGui::Begin("Entity");
			ImGui::InputFloat("X", &e->position.x);
			ImGui::InputFloat("Y", &e->position.y);
			ImGui::InputFloat("Z", &e->position.z);
			ImGui::InputFloat("RX", &r.x);
			ImGui::InputFloat("RY", &r.y);
			ImGui::InputFloat("RZ", &r.z);
			ImGui::InputFloat("SX", &e->scale.x);
			ImGui::InputFloat("SY", &e->scale.y);
			ImGui::InputFloat("SZ", &e->scale.z);
			ImGui::End();

			e->rotation = r * DEG2RAD;
		}
	}
	end_render_frame();

	game.time += dt;
	if (game.frame == 0)
		ImGui::SetWindowFocus(NULL);
	game.frame++;
}