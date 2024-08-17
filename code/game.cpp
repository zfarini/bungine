#ifndef DISABLE_PREPROCESSOR
#include <stb_image.h>
#include <ufbx.h>
#include <float.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#define MA_NO_MP3
#define MA_NO_FLAC
#include <miniaudio.h>
// TODO: remove this
#include <atomic>
#define meta(...)
#endif

#include "common.h"
#include "math.h"
#include "platform.h"
Platform platform;

thread_local TempMemory g_temp_memory;

function Arena *begin_temp_memory()
{
	assert(g_temp_memory.last_used_count < ARRAY_SIZE(g_temp_memory.last_used));
	g_temp_memory.last_used[g_temp_memory.last_used_count].block = g_temp_memory.arena.block;
	if (g_temp_memory.arena.block)
		g_temp_memory.last_used[g_temp_memory.last_used_count].used = g_temp_memory.arena.block->used;
	else
		g_temp_memory.last_used[g_temp_memory.last_used_count].used = 0;
	g_temp_memory.last_used_count++;
	return &g_temp_memory.arena;
}

function void end_temp_memory()
{
	assert(g_temp_memory.last_used_count > 0);

	memory_block *block = g_temp_memory.arena.block;

	while (block != g_temp_memory.last_used[g_temp_memory.last_used_count - 1].block) {
		memory_block *next = block->next;
		// TODO: cleanup DON'T free here instead just keep the blocks around??
		platform.free_memory(block->data);
		block = next;
	}
	g_temp_memory.arena.block = block;
	if (block)
		g_temp_memory.arena.block->used =  g_temp_memory.last_used[g_temp_memory.last_used_count - 1].used;
	g_temp_memory.last_used_count--;
}

#include "arena.h"
#include "utils.h"
#include "renderer.h"



#define RENDERER_DEBUG
#ifdef RENDERER_DX11
#include "renderer_dx11.cpp"
#else
#include "renderer_opengl.cpp"
#endif

#include "scene.h"
#include "game.h"
#include "scene.cpp"
#include "pathfinding.h"

Camera make_perspective_camera(mat4 view, float znear, float zfar, float width_fov_degree, float height_over_width)
{
	Camera camera = {};

	camera.type = CAMERA_TYPE_PERSPECTIVE;
	camera.znear = znear;
	camera.zfar = zfar;
	camera.width = 2 * znear * tanf(DEG2RAD * (width_fov_degree / 2));
	camera.height = camera.width * height_over_width;
	camera.view = view;
	camera.projection = perspective_projection(znear, zfar, camera.width, camera.height);
	//TODO: remove inverse
	mat4 inv_view = inverse(view);
	camera.position = V3(inv_view.e[0][3], inv_view.e[1][3], inv_view.e[2][3]);
	camera.right	= V3(inv_view.e[0][0], inv_view.e[1][0], inv_view.e[2][0]);
	camera.up 		= V3(inv_view.e[0][1], inv_view.e[1][1], inv_view.e[2][1]);
	camera.forward	= -V3(inv_view.e[0][2], inv_view.e[1][2], inv_view.e[2][2]);

	return camera;
}
// TODO: merge this with the perspective function
Camera make_orthographic_camera(mat4 view, float znear, float zfar, float width, float height)
{
	Camera camera = {};

	camera.type = CAMERA_TYPE_ORTHOGRAPHIC;
	camera.znear = znear;
	camera.zfar = zfar;
	camera.width = width;
	camera.height = height;
	camera.view = view;
	camera.projection = orthographic_projection(znear, zfar, width, height);
	mat4 inv_view = inverse(view);
	camera.position = V3(inv_view.e[0][3], inv_view.e[1][3], inv_view.e[2][3]);
	camera.right	= V3(inv_view.e[0][0], inv_view.e[0][1], inv_view.e[0][2]);
	camera.up 		= V3(inv_view.e[1][0], inv_view.e[1][1], inv_view.e[1][2]);
	camera.forward	= V3(inv_view.e[2][0], inv_view.e[2][1], inv_view.e[2][2]);

	return camera;
}


#ifndef DISABLE_PREPROCESSOR
#include "generated.h"
#endif

#include "sound.cpp"
#include "renderer.cpp"
#include "collision.cpp"
#include "pathfinding.cpp"
#include "world.cpp"
#include "editor.cpp"

ShadowMap create_shadow_map(int texture_width, int texture_height,
		v3 light_p, v3 light_dir, float znear, float zfar, float width, float height, v3 up = WORLD_UP)
{
	ShadowMap shadow_map = {};

	shadow_map.tex_width = texture_width;
	shadow_map.tex_height = texture_height;
	shadow_map.light_p = light_p;
	shadow_map.light_dir = light_dir;

	shadow_map.view = lookat(shadow_map.light_p, shadow_map.light_dir, up);
	shadow_map.znear = znear;
	shadow_map.zfar = zfar;
	shadow_map.width = width;
	shadow_map.height = height;
	shadow_map.projection = orthographic_projection(znear, zfar, width, height);

	shadow_map.depth_texture = get_texture(create_depth_texture(texture_width, texture_height));

	shadow_map.framebuffer = create_frame_buffer(true);

	bind_framebuffer_depthbuffer(shadow_map.framebuffer, shadow_map.depth_texture);
	// TODO:!!
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		assert(0);
	return shadow_map;
}

void game_update_and_render(Game &game, GameInput &input, float dt)
{
	//ImGui::SetCurrentContext((ImGuiContext *)platform.imgui_context);

	//Game &game = *((Game *)memory->data);

	if (!game.is_initialized) {
        stbi_set_flip_vertically_on_load(true);

        game.asset_arena.thread_safe = true;

		init_render_context(*platform.render_context);



		game.world = new World(); // TODO: cleanup using unordered_map for now
		World &world = *game.world;

		world.editor.ops = make_array_max<EditorOp>(&world.arena, 8192);
		world.editor.undos = make_array_max<EditorOp>(&world.arena, 8192);


		game.default_rasterizer_state = create_rasterizer_state(RASTERIZER_FILL_SOLID, RASTERIZER_CULL_NONE);
		game.default_depth_stencil_state = create_depth_stencil_state(true);
		game.disable_depth_state = create_depth_stencil_state(false);
		game.wireframe_rasterizer_state = create_rasterizer_state(RASTERIZER_FILL_WIREFRAME, RASTERIZER_CULL_NONE);


		//game.default_rasterizer_state = game.wireframe_rasterizer_state;

		VertexInputLayout input_layout = create_vertex_input_layout(g_vertex_input_elements, 
				ARRAY_SIZE(g_vertex_input_elements), sizeof(Vertex));

		game.mesh_render_pass = create_render_pass(
#ifdef RENDERER_DX11
				load_shader("code\\shader.hlsl", SHADER_TYPE_VERTEX, "vs_main"),
				load_shader("code\\shader.hlsl", SHADER_TYPE_FRAGMENT, "ps_main"),
#else
				load_shader("shaders/vertex.glsl", SHADER_TYPE_VERTEX),
				load_shader("shaders/fragment.glsl", SHADER_TYPE_FRAGMENT),
#endif
				PRIMITIVE_TRIANGLES, game.default_depth_stencil_state, game.default_rasterizer_state,
				input_layout);

		game.outline_render_pass = create_render_pass(
				load_shader("shaders/outline_vertex.glsl", SHADER_TYPE_VERTEX),
				load_shader("shaders/outline_fragment.glsl", SHADER_TYPE_FRAGMENT),
				PRIMITIVE_TRIANGLES, game.default_depth_stencil_state, game.default_rasterizer_state,
				input_layout);

		game.shadow_map_render_pass = create_render_pass(
				load_shader("shaders/vertex.glsl", SHADER_TYPE_VERTEX),
				load_shader("shaders/shadow_map_fs.glsl", SHADER_TYPE_FRAGMENT),
				PRIMITIVE_TRIANGLES, game.default_depth_stencil_state, game.default_rasterizer_state,
				input_layout);

		game.shadow_map = create_shadow_map(4096, 4096, 
				V3(24, 0, 24), V3(-1, 0, -1),
				1, 75, 50, 40);
		{
			VertexInputElement input_elements[] = {
				{0, 3, INPUT_ELEMENT_FLOAT, "POSITION"},
				{sizeof(v3), 3, INPUT_ELEMENT_FLOAT, "COLOR"},
			};
			VertexInputLayout line_input_layout = create_vertex_input_layout(input_elements, ARRAY_SIZE(input_elements),
					sizeof(v3) * 2);

			game.debug_lines_render_pass = create_render_pass(
#ifdef RENDERER_DX11
					load_shader("code/debug_line_shader.hlsl", SHADER_TYPE_VERTEX, "vs_main"),
					load_shader("code/debug_line_shader.hlsl", SHADER_TYPE_FRAGMENT, "ps_main"),
#else
					load_shader("shaders/debug_lines_vs.glsl", SHADER_TYPE_VERTEX),
					load_shader("shaders/debug_lines_fs.glsl", SHADER_TYPE_FRAGMENT),
#endif
					PRIMITIVE_LINES, game.default_depth_stencil_state, game.default_rasterizer_state,
					line_input_layout);

			game.debug_lines_vertex_buffer = create_vertex_buffer(VERTEX_BUFFER_DYNAMIC,
					platform.render_context->debug_lines.capacity * sizeof(v3));

			ConstantBufferElement elems[] = {
				{CONSTANT_BUFFER_ELEMENT_MAT4},
			};
			game.debug_lines_constant_buffer = create_constant_buffer(make_array<ConstantBufferElement>(elems, ARRAY_SIZE(elems)));
		}

		//perspective_projection(1, 75, 120, 1));

		{
			StructMetaData constant_metadata = get_struct_Constants_info();
			Arena *temp = begin_temp_memory();

			auto elems = make_array<ConstantBufferElement>(temp, constant_metadata.member_count);
			
			// TODO: cleanup
			for (int i = 0; i < constant_metadata.member_count; i++) {
				
				elems[i] = {};
				bool found = false;
				for (int j = 0; j < CONSTANT_BUFFER_ELEMENT_COUNT; j++) {
					const char *type_name = get_constant_buffer_element_typename((ConstantBufferElementType)j);

					int k = 0;
					while (constant_metadata.members[i].type_name[k]
							== type_name[k] && type_name[k])
						k++;
					if (!constant_metadata.members[i].type_name[k] && !type_name[k]) {
						elems[i].type = (ConstantBufferElementType)j;
						found = true;
						break ;
					}
				}
				if (!found) {
					LOG_ERROR("in constant buffer elements, couldn't find type for %s", constant_metadata.members[i].type_name);
					assert(0);
				}
				if (constant_metadata.members[i].is_array)
					elems[i].array_size = constant_metadata.members[i].array_size;
			}

			game.constant_buffer = create_constant_buffer(elems);
			end_temp_memory();
		}
		game.scenes = make_array_max<Scene>(&game.arena, 1024);
		game.loaded_animations = make_array_max<Animation>(&game.arena, 128);


		load_scene(&game.asset_arena, game, "data/parking/zma_carpark_b2.obj");
		load_scene(&game.asset_arena, game, "data/Sponza/Sponza.fbx");
		load_scene(&game.asset_arena, game, "data/cube.fbx");
		load_scene(&game.asset_arena, game, "data/sphere.fbx");
		//load_scene(&game.asset_arena, game, "data/wood-crates/source/BoxPack1.fbx");
		//load_scene(&game.asset_arena, game, "data/PrivacyFencePack/PrivacyFencePack.fbx");
		load_scene(&game.asset_arena, game, "data/Ybot.fbx");

		load_animation(&game.asset_arena, game, "data/jump.fbx");
		load_animation(&game.asset_arena, game, "data/shoot.fbx");
		load_animation(&game.asset_arena, game, "data/run.fbx");
		load_animation(&game.asset_arena, game, "data/forward_gun_walk.fbx");
		load_animation(&game.asset_arena, game, "data/backward_gun_walk.fbx");
		load_animation(&game.asset_arena, game, "data/gun_idle.fbx");

		FILE *fd = fopen("world.bin", "rb");
		if (!fd) {
			world.entities = make_array_max<Entity>(&world.arena, 4096);

			Entity *ground = make_entity(world, EntityType_Static, get_scene_id_by_name(game, "cube.fbx"), V3(0, 0, -0.5));
			ground->scale = V3(100, 100, 0.5);
			ground->color = V3(0.3f);

			Entity *player = make_entity(world, EntityType_Player, get_scene_id_by_name(game, "Ybot.fbx"),
					V3(0, 0, 4));
			player->ellipsoid_radius = V3(0.55f, 0.55f, 0.95f);
			player->ellipsoid_collision_shape = true;
			player->scene_transform = translate(0, 0, -player->ellipsoid_radius.z) * zrotation(3*PI/2) * scale(V3(1.1));
			player->color = V3(0, 1, 1);

			world.player_id = player->id;
			world.editor_camera_p = V3(0, 0, 3);

			world.collision_meshes = make_array_max<CollisionMesh>(&world.arena, game.scenes.capacity);
		}
		else {
			serialize_World(fd, false, world, &world.arena);
			for (int i = 0; i < world.entities.count; i++)
				world.entities_id_map[world.entities[i].id] = i;
			for (int i = 0; i < world.collision_meshes.count; i++)
				world.scene_collision_mesh[world.collision_meshes[i].scene] = i;

			fclose(fd);
		}
		
		// {
		// 	game.debug_asset_fb = create_frame_buffer(false, true);

		// 	Texture texture = get_texture(create_texture(make_cstring("Debug Asset"), 0, 128, 128,
		// 			true, false));
		// 	Texture depth = get_texture(create_depth_texture(texture.width, texture.height));
		// 	bind_framebuffer_color(game.debug_asset_fb, texture);
		// 	bind_framebuffer_depthbuffer(game.debug_asset_fb, depth);

		// 	game.debug_asset_tex = create_texture(make_cstring("debug asset temp"),
		// 				0, texture.width, texture.height, 1, 0);
		// 	// TODO:
		// 	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER)
		// 			== GL_FRAMEBUFFER_COMPLETE);
		// }

		game.sound_state.sample_count = SOUND_SAMPLE_RATE*10;
		game.sound_state.buffer = (float *)arena_alloc_zero(&game.arena, game.sound_state.sample_count * SOUND_CHANNEL_COUNT * sizeof(float));

		// TODO: cleanup
		game.loaded_sounds[0] = load_wav_file(&game.arena, "data/music.wav");
		game.loaded_sounds[1] = load_wav_file(&game.arena, "data/jump.wav");

		game.master_volume = 1;
		game.frustum_culling = true;

		game.is_initialized = 1;
		game.background_color = V3(0.392f, 0.584f, 0.929f);
	}

	//if (game.frame == 0)
	//	play_sound(game, game.loaded_sounds[0], 0);

	World &world = *game.world;

	if (IsDownFirstTime(input, TOGGLE_EDITOR_BUTTON))
		game.in_editor = !game.in_editor;

	if (ImGui::GetIO().WantCaptureKeyboard) {
		//for (int i = 0; i < BUTTON_COUNT; i++)
		//	if (IsKeyboardButton(i))
		//		input.buttons[i].is_down = false;
	}

	begin_render_frame();

	for (int i = 0; i < ARRAY_SIZE(profiler_block_stats); i++)
		profiler_block_stats[i] = {};
	std::unordered_map<uint64_t, bool> occupied;

	int itr_count = 0;
	for (int i = 0; i < world.entities.count; i++) {
		Entity &e = world.entities[i];
		if (e.disable_collision || e.ellipsoid_collision_shape || !world.scene_collision_mesh.count(e.scene_id))
			continue ;
		
		CollisionMesh cmesh = world.collision_meshes[world.scene_collision_mesh[e.scene_id]];

		mat4 transform = get_entity_transform(world, e) * e.scene_transform;
		for (int j = 0; j + 2 < cmesh.vertices.count; j += 3) {
			v3 p0 = (transform * V4(cmesh.vertices[j + 0], 1)).xyz;
			v3 p1 = (transform * V4(cmesh.vertices[j + 1], 1)).xyz;
			v3 p2 = (transform * V4(cmesh.vertices[j + 2], 1)).xyz;

			v3i c1 = get_cell(p0);
			v3i c2 = get_cell(p1);
			v3i c3 = get_cell(p2);

			int d_u = roundf(length(p1 - p0) / ASTART_CELL_DIM);
			int d_v = roundf(length(p2 - p0) / ASTART_CELL_DIM);

			for (int u = 0; u <= d_u; u++) {
				float U = (u * ASTART_CELL_DIM) / length(p1 - p0);
				if (U > 1)
					break ;
				for (int v = 0; v <= d_v; v++) {
						float V = (v * ASTART_CELL_DIM) / length(p2 - p0);
						if (U + V > 1)
							break ;
						v3 p = p0 + (p1 - p0) * U + (p2 - p0) * V;
						v3i cell = get_cell(p);

						occupied[pack_cell(cell)] = true;
						itr_count++;
						//push_cube_outline(V3(get_cell(p)) * ASTART_CELL_DIM, V3(ASTART_CELL_DIM*0.4f), V3(1, 1, 0));
				}
			}
		}
	}
	world.occupied = occupied;

	for (auto [x, y] : occupied) {
		//auto p = unpack_cell(x);
		//push_cube_outline(V3(unpack_cell(x)) * ASTART_CELL_DIM, V3(ASTART_CELL_DIM*0.4f), V3(1, 1, 0));
	}

	if (!game.in_editor || game.play_in_editor) {
		update_player(game, world, input, dt);
		update_enemies(game, world, input, dt);
	}
	Camera game_camera = update_camera(game, world, input, dt);

	if (!game.in_editor)
		world.last_game_camera = game_camera;


	update_editor(game, world, world.editor, input, game_camera);
	if (game.in_editor) {
		
		Camera c = world.last_game_camera;

		v3 p[4] = {
			c.position + c.forward * c.znear - c.right * c.width * 0.5f - c.up * c.height * 0.5f,
			c.position + c.forward * c.znear + c.right * c.width * 0.5f - c.up * c.height * 0.5f,
			c.position + c.forward * c.znear + c.right * c.width * 0.5f + c.up * c.height * 0.5f,
			c.position + c.forward * c.znear - c.right * c.width * 0.5f + c.up * c.height * 0.5f,
		};
		push_line(c.position, p[0]);
		push_line(c.position, p[1]);
		push_line(c.position, p[2]);
		push_line(c.position, p[3]);
		push_line(p[0], p[1]);
		push_line(p[1], p[2]);
		push_line(p[2], p[3]);
		push_line(p[3], p[0]);
	}
	push_cube_outline(game.shadow_map.light_p, V3(0.3));
	push_line(game.shadow_map.light_p, game.shadow_map.light_p + 0.5 * game.shadow_map.light_dir);

	begin_render_pass(game.shadow_map_render_pass);
	{
		set_viewport(0, 0, game.shadow_map.tex_width, game.shadow_map.tex_height);
		bind_framebuffer(game.shadow_map.framebuffer);
		clear_framebuffer_depth(game.shadow_map.framebuffer, 1);
		render_entities(game, world, make_orthographic_camera(game.shadow_map.view, game.shadow_map.znear,
			game.shadow_map.zfar, game.shadow_map.width, game.shadow_map.height), true);
	}
	end_render_pass();

	begin_render_pass(game.mesh_render_pass);
	{
		set_viewport(0, 0, platform.render_context->window_width, platform.render_context->window_height);

		bind_framebuffer(platform.render_context->window_framebuffer);
		clear_framebuffer_color(platform.render_context->window_framebuffer, V4(game.background_color, 1));
		clear_framebuffer_depth(platform.render_context->window_framebuffer, 1);

		bind_texture(4, game.shadow_map.depth_texture.id);
		render_entities(game, world, game_camera, false);

	

		// clear_framebuffer_depth(platform.render_context->window_framebuffer, 1);
		// bind_rasterizer_state(game.wireframe_rasterizer_state);
		// render_entities(game, world, game_camera, false);
	}
	end_render_pass();

	// push_line(V3(0), V3(5, 0, 0.01), V3(1, 0, 0));
	// push_line(V3(0), V3(0, 5, 0.01), V3(0, 1, 0));
	// push_line(V3(0), V3(0, 0, 5), V3(0, 0, 1));

	begin_render_pass(game.debug_lines_render_pass);
	{
		// TODO:!!!
		clear_framebuffer_depth(platform.render_context->window_framebuffer, 1);
		update_vertex_buffer(game.debug_lines_vertex_buffer, (int)platform.render_context->debug_lines.count * sizeof(v3),
				platform.render_context->debug_lines.data);
		mat4 mvp = game_camera.projection * game_camera.view;
		bind_constant_buffer(game.debug_lines_constant_buffer, 1);
		update_constant_buffer(game.debug_lines_constant_buffer, &mvp);
		bind_vertex_buffer(game.debug_lines_vertex_buffer);
		draw(0, (int)(platform.render_context->debug_lines.count / 2));
	}
	end_render_pass();


	{
		ImGuiIO &io = ImGui::GetIO();
		ImGui::Begin("debug");
		ImGui::Text("average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::Text("resolution: %dx%d", platform.render_context->window_width, platform.render_context->window_height);

		ImGui::Text("entity count: %ld", world.entities.count);
		if (ImGui::Button("new cube")) {
			Entity *entity = make_entity(world, EntityType_Static,
					get_scene_id_by_name(game, "cube.fbx"), game_camera.position
					+ game_camera.forward * 4);
			world.editor.selected_entity = entity->id;
		}
		if (ImGui::Button("new sphere")) {
			Entity *entity = make_entity(world, EntityType_Static,
					get_scene_id_by_name(game, "sphere.fbx"), game_camera.position
					+ game_camera.forward * 4);
			entity->ellipsoid_radius = V3(1);
			entity->ellipsoid_collision_shape = true;
			world.editor.selected_entity = entity->id;
		}

		if (ImGui::Button("save world")) {
			FILE *fd = fopen("world.bin", "wb");
			if (!fd)
				assert(0);
			serialize_World(fd, true, world);
			fclose(fd);
		}

		ImGui::Checkbox("play in editor", &game.play_in_editor);

		if (get_entity(world, world.editor.copied_entity))
			ImGui::Text("copying entity %ld", world.editor.copied_entity);
		imgui_edit_struct_Game(game, "game");
		imgui_edit_struct_World(world, "world");


		ImGui::End();
	}
	//ImGui::ShowDemoWindow();

	end_render_frame();

	update_sound(game, world);

	////for (int i = 0; i < ARRAY_SIZE(profiler_block_stats); i++)
	//{
	//	ProfilerBlockStat &stat = profiler_block_stats[i];
	//	if (!stat.name)
	//		break ;
	//	LOG_DEBUG("profiler block %s: (%zu cycles, %u calls)", stat.name, stat.cycle_count, stat.call_count);
	//}

	game.time += dt;
	// @HACK
	if (game.frame == 0 || !game.in_editor)
		ImGui::SetWindowFocus(NULL);
	game.frame++;
}
