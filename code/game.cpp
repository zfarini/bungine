#define _CRT_SECURE_NO_WARNINGS
#ifndef DISABLE_PREPROCESSOR
#include <stb_image.h>
#include <ufbx.h>
#include <float.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#endif
#include "common.h"
#include "arena.h"
#include "utils.h"
#include "math.h"
#include "platform.h"
#include "renderer.h"

global RenderContext *g_rc;
#define RENDERER_DEBUG
#ifdef RENDERER_DX11
#include "renderer_dx11.cpp"
#else
#include "renderer_opengl.cpp"
#endif

#include "scene.h"
#include "scene.cpp"
#include "game.h"

Entity *get_entity(World &world, entity_id id);
mat4 get_entity_transform(Entity &e);

#include "renderer.cpp"
#include "collision.cpp"
#include "world.cpp"
#include "ai.cpp"
#include "editor.cpp"

#include "generated.h"

ShadowMap create_shadow_map(int texture_width, int texture_height,
		v3 light_p, v3 light_dir, mat4 projection, v3 up = V3(0, 0, 1))
{
	ShadowMap shadow_map = {};

	shadow_map.width = texture_width;
	shadow_map.height = texture_height;
	shadow_map.light_p = light_p;
	shadow_map.light_dir = light_dir;

	shadow_map.view = lookat(shadow_map.light_p, shadow_map.light_dir, up);
	shadow_map.projection = projection;

	shadow_map.depth_texture = create_depth_texture(texture_width, texture_height);

	shadow_map.framebuffer = create_frame_buffer(true);

	bind_framebuffer_depthbuffer(shadow_map.framebuffer, shadow_map.depth_texture);
	// TODO:!!
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		assert(0);
	return shadow_map;
}

extern "C" GAME_UPDATE_AND_RENDER(game_update_and_render)
{
	g_rc = (RenderContext *)platform.render_context;
	g_temp_arena = &platform.temp_arena;
	ImGui::SetCurrentContext((ImGuiContext *)platform.imgui_context);

	Game &game = *((Game *)memory->data);
	if (!game.is_initialized) {

		assert(memory->used == 0);
		arena_alloc_zero(memory, sizeof(game));

		init_render_context(memory, *g_rc, platform);

		usize temp_arena_size = Megabyte(128);
		g_temp_arena->arena = make_arena(arena_alloc(memory, temp_arena_size), temp_arena_size);


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


		VertexInputLayout input_layout = create_vertex_input_layout(g_vertex_input_elements, 
				ARRAY_SIZE(g_vertex_input_elements), sizeof(Vertex));

		game.mesh_render_pass = create_render_pass(
#ifdef RENDERER_DX11
				load_shader(make_cstring("code\\shader.hlsl"), SHADER_TYPE_VERTEX, "vs_main"),
				load_shader(make_cstring("code\\shader.hlsl"), SHADER_TYPE_FRAGMENT, "ps_main"),
#else
				load_shader(make_cstring("shaders/vertex.glsl"), SHADER_TYPE_VERTEX),
				load_shader(make_cstring("shaders/fragment.glsl"), SHADER_TYPE_FRAGMENT),
#endif
				PRIMITIVE_TRIANGLES, game.default_depth_stencil_state, game.default_rasterizer_state,
				input_layout);

		game.outline_render_pass = create_render_pass(
				load_shader(make_cstring("shaders/outline_vertex.glsl"), SHADER_TYPE_VERTEX),
				load_shader(make_cstring("shaders/outline_fragment.glsl"), SHADER_TYPE_FRAGMENT),
				PRIMITIVE_TRIANGLES, game.default_depth_stencil_state, game.default_rasterizer_state,
				input_layout);

		game.shadow_map_render_pass = create_render_pass(
				load_shader(make_cstring("shaders/vertex.glsl"), SHADER_TYPE_VERTEX),
				load_shader(make_cstring("shaders/shadow_map_fs.glsl"), SHADER_TYPE_FRAGMENT),
				PRIMITIVE_TRIANGLES, game.default_depth_stencil_state, game.default_rasterizer_state,
				input_layout);

		game.shadow_map = create_shadow_map(4096, 4096, 
				V3(24, 0, 24), V3(-1, 0, -1),
				orthographic_projection(1, 75, 50, 40));
		{
			VertexInputElement input_elements[] = {
				{0, 3, INPUT_ELEMENT_FLOAT, "POSITION"},
				{sizeof(v3), 3, INPUT_ELEMENT_FLOAT, "COLOR"},
			};
			VertexInputLayout input_layout = create_vertex_input_layout(input_elements, ARRAY_SIZE(input_elements),
					sizeof(v3) * 2);

			game.debug_lines_render_pass = create_render_pass(
#ifdef RENDERER_DX11
					load_shader(make_cstring("code/debug_line_shader.hlsl"), SHADER_TYPE_VERTEX, "vs_main"),
					load_shader(make_cstring("code/debug_line_shader.hlsl"), SHADER_TYPE_FRAGMENT, "ps_main"),
#else
					load_shader(make_cstring("shaders/debug_lines_vs.glsl"), SHADER_TYPE_VERTEX),
					load_shader(make_cstring("shaders/debug_lines_fs.glsl"), SHADER_TYPE_FRAGMENT),
#endif
					PRIMITIVE_LINES, game.default_depth_stencil_state, game.default_rasterizer_state,
					input_layout);

			game.debug_lines_vertex_buffer = create_vertex_buffer(VERTEX_BUFFER_DYNAMIC,
					g_rc->debug_lines.capacity * sizeof(v3));

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
			{CONSTANT_BUFFER_ELEMENT_INT},
		};
		game.constant_buffer = create_constant_buffer(make_array<ConstantBufferElement>(elems, ARRAY_SIZE(elems)));

		game.scenes[1] = load_scene(&game.asset_arena, "data/YBot.fbx");
		game.scenes[2] = load_scene(&game.asset_arena, "data/cube.fbx");
		game.scenes[3] = load_scene(&game.asset_arena, "data/sphere.fbx");

		game.ch43 = 1;
		game.cube_asset = 2;
		game.sphere_asset = 3;

		game.animations[ANIMATION_JUMP] = load_scene(&game.asset_arena, "data/jump.fbx").animations[0];
		game.animations[ANIMATION_SHOOT] = load_scene(&game.asset_arena, "data/shoot.fbx").animations[0];
		game.animations[ANIMATION_RUN] = load_scene(&game.asset_arena, "data/run.fbx").animations[0];
		game.animations[ANIMATION_FORWARD_GUN_WALK] = load_scene(&game.asset_arena, "data/forward_gun_walk.fbx").animations[0];
		game.animations[ANIMATION_BACKWARD_GUN_WALK] = load_scene(&game.asset_arena, "data/backward_gun_walk.fbx").animations[0];
		game.animations[ANIMATION_GUN_IDLE] = load_scene(&game.asset_arena, "data/gun_idle.fbx").animations[0];



		FILE *fd = fopen("world.bin", "rb");
		if (!fd) {

			world.entities = make_array_max<Entity>(&world.arena, 4096);

			Entity *boxes[] = {
				make_entity(world, EntityType_Static, game.cube_asset, V3(0, 0, -0.5), make_box_shape(memory, V3(25, 25, 0.5))),
				make_entity(world, EntityType_Static, game.cube_asset, V3(0, 25, 0), make_box_shape(memory, (V3(25, 0.5, 25)))),
				make_entity(world, EntityType_Static, game.cube_asset, V3(10, 6, 1), make_box_shape(memory, (V3(5, 5.8, 0.3)))),
				make_entity(world, EntityType_Static, game.cube_asset, V3(10, 6, 1.3), make_box_shape(memory, (V3(2.5, 2.5, 0.3)))),
				make_entity(world, EntityType_Static, game.cube_asset, V3(10, 6, 1.6), make_box_shape(memory, (V3(1.25, 1.25, 0.3)))),
				make_entity(world, EntityType_Static, game.cube_asset, V3(-10, 6, 1), make_box_shape(memory, (V3(7, 7, 0.3)))),
				make_entity(world, EntityType_Static, game.cube_asset, V3(1, 6, 0),  make_box_shape(memory, (V3(4, 1, 2)))),
				make_entity(world, EntityType_Static, game.cube_asset, V3(0, -7, 1), make_box_shape(memory, (V3(4, 4, 0.3)))),
				make_entity(world, EntityType_Static, game.cube_asset, V3(0, -7, 1.6), make_box_shape(memory, (V3(2, 2, 0.3)))),

				make_entity(world, EntityType_Static, game.cube_asset, V3(7, -7, 0.1), make_box_shape(memory, (V3(3, 3, 0.1)))),

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
			Entity *player = make_entity(world, EntityType_Player, game.ch43, V3(0, 0, 8), make_ellipsoid_shape(V3(0.55f, 0.55f, 0.95f)));
			world.player_id = player->id;
			//player->scene = &game.sphere_asset;
			//player->scene = 0;
			//player->shape.ellipsoid_radius = V3(0.2, 0.2, 0.2); 
			//player->scene_transform =  scale(player->shape.ellipsoid_radius);
			player->scene_transform = translate(0, 0, -player->shape.ellipsoid_radius.z) * zrotation(3*PI/2) * scale(V3(1.1));
			player->color = V3(0.2, 0.8, 0.8);

			world.editor_camera_p = V3(0, 0, 3);
		}
		else {
			serialize(fd, false, world);
			for (int i = 0; i < world.entities.count; i++)
				world.entities_id_map[world.entities[i].id] = i;
			fclose(fd);
		}

		{
			game.debug_asset_fb = create_frame_buffer(false, true);

			Texture texture = create_texture(make_cstring("Debug Asset"), 0, 128, 128,
					true, false);
			Texture depth = create_depth_texture(texture.width, texture.height);
			bind_framebuffer_color(game.debug_asset_fb, texture);
			bind_framebuffer_depthbuffer(game.debug_asset_fb, depth);
			// TODO:
			assert(glCheckFramebufferStatus(GL_FRAMEBUFFER)
					== GL_FRAMEBUFFER_COMPLETE);
		}

		game.is_initialized = 1;
	}

	World &world = *game.world;



	if (IsDownFirstTime(input, TOGGLE_EDITOR_BUTTON))
		game.in_editor = !game.in_editor;

	begin_render_frame();

	if (!game.in_editor) {
		update_player(game, world, input, dt);
		update_enemies(game, world, input, dt);
	}

	Camera game_camera = update_camera(game, world, input, dt);

	if (game.in_editor)
		update_editor(game, world, world.editor, input, game_camera);

	push_cube_outline(game.shadow_map.light_p, V3(0.3));
	push_line(game.shadow_map.light_p, game.shadow_map.light_p + 0.5 * game.shadow_map.light_dir);

	begin_render_pass(game.shadow_map_render_pass);
	{
		set_viewport(0, 0, game.shadow_map.width, game.shadow_map.height);
		bind_framebuffer(game.shadow_map.framebuffer);
		clear_framebuffer_depth(game.shadow_map.framebuffer, 1);
		render_entities(game, world, Camera{game.shadow_map.light_p, game.shadow_map.view, game.shadow_map.projection},
					true);
	}
	end_render_pass();

	begin_render_pass(game.mesh_render_pass);
	{
		set_viewport(0, 0, g_rc->window_width, g_rc->window_height);

		bind_framebuffer(g_rc->window_framebuffer);
		clear_framebuffer_color(g_rc->window_framebuffer, V4(0.392f, 0.584f, 0.929f, 1.f));
		clear_framebuffer_depth(g_rc->window_framebuffer, 1);

		bind_texture(4, game.shadow_map.depth_texture);
		render_entities(game, world, game_camera, false);
	}
	end_render_pass();

	// push_line(V3(0), V3(5, 0, 0.01), V3(1, 0, 0));
	// push_line(V3(0), V3(0, 5, 0.01), V3(0, 1, 0));
	// push_line(V3(0), V3(0, 0, 5), V3(0, 0, 1));

	begin_render_pass(game.debug_lines_render_pass);
	{
		// TODO:!!!
		clear_framebuffer_depth(g_rc->window_framebuffer, 1);
		update_vertex_buffer(game.debug_lines_vertex_buffer, (int)g_rc->debug_lines.count * sizeof(v3),
				g_rc->debug_lines.data);
		mat4 mvp = game_camera.projection * game_camera.view;
		bind_constant_buffer(game.debug_lines_constant_buffer, 1);
		update_constant_buffer(game.debug_lines_constant_buffer, &mvp);
		bind_vertex_buffer(game.debug_lines_vertex_buffer);
		draw(0, (int)(g_rc->debug_lines.count / 2));
	}
	end_render_pass();


	{
		ImGuiIO &io = ImGui::GetIO();
		ImGui::Begin("debug");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::Text("resolution: %dx%d", g_rc->window_width, g_rc->window_height);
		ImGui::Checkbox("debug collission", &game.debug_collision);
		ImGui::Checkbox("show normals", &game.show_normals);
		ImGui::Checkbox("show bones", &game.render_bones);
		ImGui::Text("entity count: %ld", world.entities.count);
		if (ImGui::Button("new cube")) {
			Entity *entity = make_entity(world, EntityType_Static,
					2, game_camera.position
					+ game_camera.forward * 4, make_box_shape(&world.arena, V3(1)));
			world.editor.selected_entity = entity->id;
		}
		if (ImGui::Button("new sphere")) {
			Entity *entity = make_entity(world, EntityType_Static,
					3, game_camera.position
					+ game_camera.forward * 4, make_ellipsoid_shape(V3(1)));
			world.editor.selected_entity = entity->id;
		}

		if (ImGui::Button("save world")) {
			FILE *fd = fopen("world.bin", "wb");
			if (!fd)
				assert(0);
			serialize(fd, true, world);
			fclose(fd);
		}

		if (get_entity(world, world.editor.copied_entity))
			ImGui::Text("copying entity %ld", world.editor.copied_entity);

		ImGui::End();
	}

	{
		Entity *e = get_entity(world, world.editor_selected_entity);

		if (e) {
			ImGuiIO &io = ImGui::GetIO();

			ImGui::Begin("Entity");
            ImGui::ColorEdit3("color", e->color.e);
			ImGui::Text("id: %ld", e->id);
			ImGui::Text("type: %s", ENUM_STRING(EntityType, e->type));
			if (ImGui::Button("Reset scale"))
				e->scale = V3(1);
			if (ImGui::Button("Reset rotation"))
				e->rotation = identity_quat();

			if (ImGui::Button("delete")) {
				if (e->id != world.player_id) {
					int index = world.entities_id_map[e->id];
					int e_id = e->id;
					assert(index >= 0 && index < world.entities.count);
					assert(e == &world.entities[index]);
					world.entities_id_map.erase(e->id);

					if (index != world.entities.count - 1) {
						world.entities[index] = world.entities[world.entities.count - 1];
						world.entities_id_map[world.entities[index].id] = index;
					}
					world.entities.count--;
				}
			}
			begin_render_pass(game.mesh_render_pass);
			{
				FrameBuffer &fb = game.debug_asset_fb;
				bind_framebuffer(fb);

				int width = fb.color_texture.width;
				int height = fb.color_texture.height;

				set_viewport(0, 0, width, height);

				clear_framebuffer_color(fb, V4(0.3f, 0.3f, 0.3f, 1.f));
				clear_framebuffer_depth(fb, 1);

				bind_texture(4, g_rc->white_texture);
				Camera camera = {};
				camera.view = lookat(V3(0, -2, 0), V3(0, 1, 0), V3(0, 0, 1));
				camera.projection = game_camera.projection;
				camera.projection = perspective_projection(0.1, 100, 90, (float)height/width);
				camera.projection.e[1][1] *= -1;
				render_scene(game, game.scenes[e->scene_id], camera, 
					zrotation(PI/4), 0, 0, e->color);
				//render_entities(game, world, camera, false);

				Arena *temp = begin_temp_memory();

				void *data = arena_alloc(temp, sizeof(uint32_t) * width * height);
				glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

				Texture tex = create_texture(make_cstring("tmp"),
						data, width, height, 1, 0);

				ImGui::Image((void *)(intptr_t)tex.id, ImVec2(width, height));

				// TODO:
				//glDeleteTextures(1, &tex.id);

				end_temp_memory();
			}
			end_render_pass();

			ImGui::End();
		}
	}
	end_render_frame();

	game.time += dt;
	if (game.frame == 0)
		ImGui::SetWindowFocus(NULL);
	game.frame++;
}
