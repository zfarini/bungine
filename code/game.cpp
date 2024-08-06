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

#include <atomic>

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

SceneID get_scene(Game &game, SceneType type)
{
	assert(type > 0 && type < ARRAY_SIZE(game.scenes));
	return (SceneID)type;
}

Entity *get_entity(World &world, entity_id id);
mat4 get_entity_transform(World &world, Entity &e);
v3 get_world_p(World &world, entity_id id);

#define SOUND_CHANNEL_COUNT 2
#define SOUND_SAMPLE_RATE 48000

LoadedSound load_wav_file(Arena *arena, const char *filename)
{
    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, SOUND_CHANNEL_COUNT, SOUND_SAMPLE_RATE);

    ma_decoder decoder;
    ma_result result = ma_decoder_init_file(filename, &config, &decoder);
    if (result != MA_SUCCESS)
        assert(0);
    LoadedSound sound = {};
    //sound.samples = (float *)calloc(1, 2 * sizeof(float) * 48000 * 60);
    ma_uint64 samplesToRead = 1024;
	sound.samples = (float *)arena_alloc(arena, samplesToRead * sizeof(float) * SOUND_CHANNEL_COUNT);

    while (1) {
        ma_uint64 samplesRead = 0;
        result = ma_decoder_read_pcm_frames(&decoder, sound.samples + sound.sample_count*SOUND_CHANNEL_COUNT, 
			samplesToRead, &samplesRead);
        sound.sample_count += samplesRead;
        if (samplesRead < samplesToRead)
            break ;
		// @HACK: kinda, I'm just assuming stuff will be continuous with no random alignement jumps
		arena_alloc(arena, samplesToRead * sizeof(float) * SOUND_CHANNEL_COUNT);
    }
	printf("loaded sound %s, %d samples\n", filename, sound.sample_count);
    return sound;
}

void audio_write_callback(ma_device* device, void* output, const void* input, ma_uint32 frame_count)
{
	Game &game = *((Game *)device->pUserData);
	if (!game.is_initialized) 
		return ;

	int read_index = game.sound_state.read_index;
	int write_index = game.sound_state.write_index;
	//printf("read: %d %d %d\n", read_index, write_index, game.sound_state.sample_count);

	if (read_index == write_index)
		return ;

	if (read_index < write_index) {
		int samples_to_read = min(write_index - read_index, (int)frame_count);
		memcpy(output, game.sound_state.buffer + read_index * SOUND_CHANNEL_COUNT,
			samples_to_read * SOUND_CHANNEL_COUNT * sizeof(float));
		game.sound_state.read_index = read_index + samples_to_read;
	}
	else {
		int samples_to_copy = min((int)frame_count, game.sound_state.sample_count - read_index);
		memcpy(output, game.sound_state.buffer + read_index * SOUND_CHANNEL_COUNT,
			samples_to_copy * SOUND_CHANNEL_COUNT * sizeof(float));
		frame_count -= samples_to_copy;
		memcpy(output, game.sound_state.buffer, 
			min(write_index, (int)frame_count) * SOUND_CHANNEL_COUNT * sizeof(float));

		game.sound_state.read_index = (read_index + samples_to_copy
			+ min(write_index, (int)frame_count)) % game.sound_state.sample_count;
	}

	// for (int i = 0; i < frame_count; i++)
    // {

    //     float *out0 = (float *)output;
    //     output = (float *)output + 1;
    //     float *out1 = (float *)output;
    //     output = (float *)output + 1;

	// 	//*out0 = 0;
	// 	//*out1 = 0;
    //     // if (sample >= g_sound_file.sampleCount*2)
    //     //     sample = 0;
		
    //     // float value0 = g_sound_file.samples[sample++];
    //     // float value1 = g_sound_file.samples[sample++];
	// 	// float value = sinf(((float)sample / SOUND_SAMPLE_RATE) * 2 * 3.14159265359f * 300);
    //     // *out0 = value;
    //     // *out1 = value;
	// 	// sample++;
    // }
}

void play_sound(Game &game, LoadedSound &loaded_sound, entity_id entity = 0)
{
	SoundPlaying *sound = (SoundPlaying *)arena_alloc_zero(game.memory, sizeof(SoundPlaying));
	sound->sound = &loaded_sound;
	sound->entity = entity;

	sound->next = game.first_playing_sound;
	if (game.first_playing_sound)
		game.first_playing_sound->prev = sound;
	game.first_playing_sound = sound;
}

void update_sound(Game &game, World &world)
{
	if (!game.first_playing_sound)
		return ;

	SoundState &state = game.sound_state;

	int frames_to_write = 1;
	int fps = 60;
	int max_samples_to_write =  ((SOUND_SAMPLE_RATE * frames_to_write) / fps);


	int write_index = state.write_index;
	int read_index = state.read_index;
	//printf("write %d %d %d\n", read_index, write_index, state.sample_count);

	int can_write = 0;
	if (read_index <= write_index)
		can_write = read_index + state.sample_count - write_index; 
	else
		can_write = read_index - write_index;
	
	can_write -= 1;

	max_samples_to_write = min(max_samples_to_write, can_write);

	if (max_samples_to_write <= 0)
		return ;

	Entity *player = get_entity(world, world.player_id);
	v3 player_forward = normalize(V3(cosf(world.player_camera_rotation.z), sinf(world.player_camera_rotation.z), 0));
	v3 player_up = V3(0, 0, 1);
	v3 player_right = normalize(cross(player_forward, player_up));
		
	int index = write_index;
	for (int sample = 0; sample < max_samples_to_write; sample++) {
		for (int i = 0; i < SOUND_CHANNEL_COUNT; i++)
			state.buffer[index * SOUND_CHANNEL_COUNT + i] = 0;
		index++;
		if (index == state.sample_count)
			index = 0;
	}
	//printf("INIT JUMP SOUND %d %d\n", max_samples_to_write, game.first_playing_sound->sound->sample_count);

	for (SoundPlaying *playing_sound = game.first_playing_sound; 
		playing_sound;) {

		Entity *e = get_entity(world, playing_sound->entity);
		float volume[2] = {1, 1};
		if (e) {
			v3 to_e = normalize(e->position - world.player_camera_p);
			float x = dot(to_e, player_right);
			float y = dot(to_e, player_forward);
			float a = fabsf(atan2(y, x));
			a = a / PI;
			volume[0] = a;
			volume[1] = 1 - a;

			float dist = 1 - logf(length(e->position - world.player_camera_p)) / 5;
			if (dist < 0)
				dist = 0;
			volume[0] *= dist;
			volume[1] *= dist;
		}
		
		int samples_to_write = min(max_samples_to_write, 
		playing_sound->sound->sample_count - playing_sound->samples_played);

		index = write_index;
		for (int sample = 0; sample < samples_to_write; sample++) {
			for (int i = 0; i < SOUND_CHANNEL_COUNT; i++) {
				state.buffer[index * SOUND_CHANNEL_COUNT + i] += 
					playing_sound->sound->samples[playing_sound->samples_played
						* SOUND_CHANNEL_COUNT + i] * volume[i] * game.master_volume;
			}
			playing_sound->samples_played++;
			index++;
			if (index == state.sample_count)
				index = 0;
		}
		SoundPlaying *next = playing_sound->next;
		if (playing_sound->samples_played == playing_sound->sound->sample_count) {
			if (next)
				next->prev = playing_sound->prev;
			if (playing_sound->prev)
				playing_sound->prev->next = next;
			if (game.first_playing_sound == playing_sound)
				game.first_playing_sound = next;
		}
		playing_sound = next;
	}
	//printf("finished? %d\n", game.first_playing_sound == 0);
	//printf("END JUMP SOUND\n");
	int new_write_index = (write_index + max_samples_to_write) % state.sample_count;
	state.write_index = new_write_index;
}


#include "generated.h"

#include "renderer.cpp"
#include "collision.cpp"
#include "world.cpp"
#include "ai.cpp"
#include "editor.cpp"


ShadowMap create_shadow_map(int texture_width, int texture_height,
		v3 light_p, v3 light_dir, float znear, float zfar, float width, float height, v3 up = V3(0, 0, 1))
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
		world.editor.ops = make_array_max<EditorOp>(&world.arena, 8192);
		world.editor.undos = make_array_max<EditorOp>(&world.arena, 8192);

		game.asset_arena = make_arena(arena_alloc(memory, Megabyte(256)), Megabyte(256));

		game.default_rasterizer_state = create_rasterizer_state(RASTERIZER_FILL_SOLID, RASTERIZER_CULL_NONE);
		game.default_depth_stencil_state = create_depth_stencil_state(true);
		game.disable_depth_state = create_depth_stencil_state(false);
		game.wireframe_rasterizer_state = create_rasterizer_state(RASTERIZER_FILL_WIREFRAME, RASTERIZER_CULL_NONE);


		//game.default_rasterizer_state = game.wireframe_rasterizer_state;

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
					load_shader(make_cstring("code/debug_line_shader.hlsl"), SHADER_TYPE_VERTEX, "vs_main"),
					load_shader(make_cstring("code/debug_line_shader.hlsl"), SHADER_TYPE_FRAGMENT, "ps_main"),
#else
					load_shader(make_cstring("shaders/debug_lines_vs.glsl"), SHADER_TYPE_VERTEX),
					load_shader(make_cstring("shaders/debug_lines_fs.glsl"), SHADER_TYPE_FRAGMENT),
#endif
					PRIMITIVE_LINES, game.default_depth_stencil_state, game.default_rasterizer_state,
					line_input_layout);

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
		
#if 1
		game.scenes[SCENE_TEST] = load_scene(&game.asset_arena, "Z:\\3dGame4\\data\\parking\\source\\ZM Car Park\\zma_carpark_b2.obj");
#else
#endif
		game.scenes[SCENE_CUBE] = load_scene(&game.asset_arena, "data/cube.fbx");
		game.scenes[SCENE_SPHERE] = load_scene(&game.asset_arena, "data/sphere.fbx");

#if 1
		game.scenes[SCENE_PLAYER] = load_scene(&game.asset_arena, "data/Ybot.fbx");
		game.animations[ANIMATION_JUMP] = load_scene(&game.asset_arena, "data/jump.fbx").animations[0];
		game.animations[ANIMATION_SHOOT] = load_scene(&game.asset_arena, "data/shoot.fbx").animations[0];
		game.animations[ANIMATION_SHOOT].duration *= 0.6;
		game.animations[ANIMATION_RUN] = load_scene(&game.asset_arena, "data/run.fbx").animations[0];
		game.animations[ANIMATION_FORWARD_GUN_WALK] = load_scene(&game.asset_arena, "data/forward_gun_walk.fbx").animations[0];
		game.animations[ANIMATION_BACKWARD_GUN_WALK] = load_scene(&game.asset_arena, "data/backward_gun_walk.fbx").animations[0];
		game.animations[ANIMATION_GUN_IDLE] = load_scene(&game.asset_arena, "data/gun_idle.fbx").animations[0];
#else
		game.scenes[SCENE_PLAYER] = load_scene(&game.asset_arena, "data/Swat.fbx");
		game.animations[ANIMATION_JUMP] = load_scene(&game.asset_arena, "data/SwatRifleJump.fbx").animations[0];
		game.animations[ANIMATION_SHOOT] = load_scene(&game.asset_arena, "data/SwatRiffleShoot.fbx").animations[0];
		game.animations[ANIMATION_RUN] = load_scene(&game.asset_arena, "data/SwatRunning.fbx").animations[0];
		game.animations[ANIMATION_FORWARD_GUN_WALK] = load_scene(&game.asset_arena, "data/SwatRifleWalk.fbx").animations[0];
		game.animations[ANIMATION_BACKWARD_GUN_WALK] = load_scene(&game.asset_arena, "data/SwatBackwardRifleWalk.fbx").animations[0];
		game.animations[ANIMATION_GUN_IDLE] = load_scene(&game.asset_arena, "data/SwatRifleIdle.fbx").animations[0];
#endif

		FILE *fd = fopen("world.bin", "rb");
		if (!fd) {

			world.entities = make_array_max<Entity>(&world.arena, 4096);

			Entity *boxes[] = {
				make_entity(world, EntityType_Static, get_scene(game, SCENE_CUBE), V3(0, 0, -0.5), make_box_shape(memory, V3(100, 100, 0.5))),

				
				//make_entity(world, EntityType_Static, get_scene(game, SCENE_CUBE), V3(0, 0, -0.5), make_box_shape(memory, V3(25, 25, 0.5))),
				// make_entity(world, EntityType_Static, get_scene(game, SCENE_CUBE), V3(0, 25, 0), make_box_shape(memory, (V3(25, 0.5, 25)))),
				// make_entity(world, EntityType_Static, get_scene(game, SCENE_CUBE), V3(10, 6, 1), make_box_shape(memory, (V3(5, 5.8, 0.3)))),
				// make_entity(world, EntityType_Static, get_scene(game, SCENE_CUBE), V3(10, 6, 1.3), make_box_shape(memory, (V3(2.5, 2.5, 0.3)))),
				// make_entity(world, EntityType_Static, get_scene(game, SCENE_CUBE), V3(10, 6, 1.6), make_box_shape(memory, (V3(1.25, 1.25, 0.3)))),
				// make_entity(world, EntityType_Static, get_scene(game, SCENE_CUBE), V3(-10, 6, 1), make_box_shape(memory, (V3(7, 7, 0.3)))),
				// make_entity(world, EntityType_Static, get_scene(game, SCENE_CUBE), V3(1, 6, 0),  make_box_shape(memory, (V3(4, 1, 2)))),
				// make_entity(world, EntityType_Static, get_scene(game, SCENE_CUBE), V3(0, -7, 1), make_box_shape(memory, (V3(4, 4, 0.3)))),
				// make_entity(world, EntityType_Static, get_scene(game, SCENE_CUBE), V3(0, -7, 1.6), make_box_shape(memory, (V3(2, 2, 0.3)))),

				//make_entity(world, EntityType_Static, get_scene(game, SCENE_CUBE), V3(7, -7, 0.1), make_box_shape(memory, (V3(3, 3, 0.1)))),
			};
			world.moving_box = 7;

			for (int i = 0; i < ARRAY_SIZE(boxes); i++) {
				//boxes[i]->scene_transform = scale();
				boxes[i]->scale = boxes[i]->shape.box_radius;
				boxes[i]->shape = make_box_shape(memory, V3(1));

				float r = (float)rand() / RAND_MAX;
				float g = (float)rand() / RAND_MAX;
				float b = (float)rand() / RAND_MAX;

				boxes[i]->color = V3(r, g, b);

			}
			Entity *player = make_entity(world, EntityType_Player, get_scene(game, SCENE_PLAYER), V3(0, 0, 8), make_ellipsoid_shape(V3(0.55f, 0.55f, 0.95f)));
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

		Entity *test = make_entity(world, EntityType_Static, get_scene(game, SCENE_TEST), V3(0, 0, 0.1f), make_box_shape(memory, V3(0)));
		test->rotation = rotate_around_axis_quat(V3(1, 0, 0), PI/2);

		{
			game.debug_asset_fb = create_frame_buffer(false, true);

			Texture texture = create_texture(make_cstring("Debug Asset"), 0, 128, 128,
					true, false);
			Texture depth = create_depth_texture(texture.width, texture.height);
			bind_framebuffer_color(game.debug_asset_fb, texture);
			bind_framebuffer_depthbuffer(game.debug_asset_fb, depth);

			game.debug_asset_tex = create_texture(make_cstring("debug asset temp"),
						0, texture.width, texture.height, 1, 0);
			// TODO:
			assert(glCheckFramebufferStatus(GL_FRAMEBUFFER)
					== GL_FRAMEBUFFER_COMPLETE);
		}

		game.sound_state.sample_count = SOUND_SAMPLE_RATE*10;
		game.sound_state.buffer = (float *)arena_alloc_zero(memory, game.sound_state.sample_count * SOUND_CHANNEL_COUNT * sizeof(float));

		game.loaded_sounds[0] = load_wav_file(memory, "data/music.wav");
		game.loaded_sounds[1] = load_wav_file(memory, "data/jump.wav");

		game.master_volume = 1;
		game.frustum_culling = true;

		game.is_initialized = 1;
	}
	game.memory = memory;


	if (game.frame == 0) {
		//play_sound(game, game.loaded_sounds[0], 12);
	}

	World &world = *game.world;

	if (IsDownFirstTime(input, TOGGLE_EDITOR_BUTTON))
		game.in_editor = !game.in_editor;

	begin_render_frame();

	// TODO: decide weither this should update before or after player

	if (!game.in_editor) {
		update_player(game, world, input, dt);
		update_enemies(game, world, input, dt);



		//Entity *e = get_entity(world, world.moving_box);
		//if (e)
		//	e->position.z = 20 + sinf(game.time) * 20;
	}
	Camera game_camera = update_camera(game, world, input, dt);

	if (!game.in_editor)
		world.last_game_camera = game_camera;

	if (game.in_editor) {
		update_editor(game, world, world.editor, input, game_camera);
		
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
		set_viewport(0, 0, g_rc->window_width, g_rc->window_height);

		bind_framebuffer(g_rc->window_framebuffer);
		clear_framebuffer_color(g_rc->window_framebuffer, V4(0.392f, 0.584f, 0.929f, 1.f));
		clear_framebuffer_depth(g_rc->window_framebuffer, 1);

		bind_texture(4, game.shadow_map.depth_texture);
		render_entities(game, world, game_camera, false);

		// clear_framebuffer_depth(g_rc->window_framebuffer, 1);
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
		//clear_framebuffer_depth(g_rc->window_framebuffer, 1);
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
		ImGui::SliderFloat("master volume", &game.master_volume, 0, 1);
		ImGui::Checkbox("debug collission", &game.debug_collision);
		ImGui::Checkbox("show normals", &game.show_normals);
		ImGui::Checkbox("show bones", &game.render_bones);
		ImGui::Checkbox("frustum culling", &game.frustum_culling);

		ImGui::Text("entity count: %ld", world.entities.count);
		if (ImGui::Button("new cube")) {
			Entity *entity = make_entity(world, EntityType_Static,
					get_scene(game, SCENE_CUBE), game_camera.position
					+ game_camera.forward * 4, make_box_shape(&world.arena, V3(1)));
			world.editor.selected_entity = entity->id;
		}
		if (ImGui::Button("new sphere")) {
			Entity *entity = make_entity(world, EntityType_Static,
					get_scene(game, SCENE_SPHERE), game_camera.position
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

	
	end_render_frame();

	update_sound(game, world);

	game.time += dt;
	if (game.frame == 0)
		ImGui::SetWindowFocus(NULL);
	game.frame++;
}
