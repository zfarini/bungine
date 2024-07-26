#include "debug.cpp"
#include "scene.cpp"
#include "game.h"
#include "collision.cpp"
#include "ai.cpp"

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

			bind_texture(0, part.material.diffuse.valid ? part.material.diffuse : g_rc.white_texture);
			bind_texture(1, part.material.specular.valid ? part.material.specular : g_rc.white_texture);
			bind_texture(2, part.material.normal_map);
			bind_texture(3, part.material.specular_exponent.valid ? part.material.specular_exponent : g_rc.white_texture);
			
			constants.diffuse_factor = part.material.diffuse_factor;
			constants.specular_factor = part.material.specular_factor;
			constants.specular_exponent_factor = part.material.specular_exponent_factor;
			constants.has_normal_map = part.material.normal_map.valid;

			update_constant_buffer(game.constant_buffer, &constants);
			draw(mesh.vertex_buffer, (int)part.offset, (int)part.vertices_count);
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

#include "world.cpp"


void game_update_and_render(Game &game, Arena *memory, GameInput &input, float dt)
{
	if (!game.is_initialized) {
		arena_alloc(memory, sizeof(game));
		//memory->used += sizeof(game);

		//game.world = (World *)arena_alloc(memory, sizeof(World));
		//memset(game.world, 0, sizeof(*game.world));
		game.world = new World(); // TODO: using unordered_map for now
		World &world = *game.world;

		world.arena = make_arena(arena_alloc(memory, Megabyte(64)), Megabyte(64));

		game.asset_arena = make_arena(arena_alloc(memory, Megabyte(256)), Megabyte(256));
		
		game.mesh_render_pass = create_render_pass(
			create_shader_program(
				load_shader(make_cstring("vertex.glsl"), SHADER_TYPE_VERTEX),
				load_shader(make_cstring("fragment.glsl"), SHADER_TYPE_FRAGMENT)),
			g_vertex_input_elements, ARRAY_SIZE(g_vertex_input_elements));

		game.shadow_map_render_pass = create_render_pass(
			create_shader_program(
				load_shader(make_cstring("vertex.glsl"), SHADER_TYPE_VERTEX),
				load_shader(make_cstring("shadow_map_fs.glsl"), SHADER_TYPE_FRAGMENT)),
			g_vertex_input_elements, ARRAY_SIZE(g_vertex_input_elements));

		{
			VertexInputElement input_elements[] = {
				{0, 3, INPUT_ELEMENT_FLOAT, "POSITION"},
				{sizeof(v3), 3, INPUT_ELEMENT_FLOAT, "COLOR"},
			};

			game.debug_lines_render_pass = create_render_pass(
				create_shader_program(
					load_shader(make_cstring("debug_lines_vs.glsl"), SHADER_TYPE_VERTEX),
					load_shader(make_cstring("debug_lines_fs.glsl"), SHADER_TYPE_FRAGMENT)),
				input_elements, ARRAY_SIZE(input_elements));
			game.debug_lines_vertex_buffer = create_vertex_buffer(g_rc.debug_lines.capacity * sizeof(v3),
				0, 2 * sizeof(v3), input_elements, ARRAY_SIZE(input_elements));

			ConstantBufferElement elems[] = {
				{CONSTANT_BUFFER_ELEMENT_MAT4},
			};
			game.debug_lines_constant_buffer = create_constant_buffer(1, elems, ARRAY_SIZE(elems));
		}

		game.shadow_map = create_shadow_map(4096, 4096, 
			V3(24, 0, 24), V3(-1, 0, -1),
			orthographic_projection(1, 75, 50, 40));
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
		game.constant_buffer = create_constant_buffer(0, elems, ARRAY_SIZE(elems));

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
		// 	for (int i = 0; i < node.position.count; i++)
		// 		node.position[i].y -= 50;
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
		for (int i = 0; i < ARRAY_SIZE(boxes); i++)
			boxes[i]->scene_transform = scale(boxes[i]->shape.box_radius);
		
		Entity *player = make_entity(world, EntityType_Player, &game.ch43, V3(0, 0, 8), make_ellipsoid_shape(V3(0.55f, 0.55f, 0.95f)));
		world.player_id = player->id;
		//player->scene = &game.sphere_asset;
		//player->scene = 0;
		//player->shape.ellipsoid_radius = V3(0.2, 0.2, 0.2); 
		//player->scene_transform =  scale(player->shape.ellipsoid_radius);
		player->scene_transform =  translate(0, 0, -player->shape.ellipsoid_radius.z) * zrotation(3*PI/2) * scale(V3(1.1));
		player->color = V3(0.2, 0.8, 0.8);

		world.last_camera_free_p = V3(0, 0, 3);

		game.is_initialized = 1;
	}

	World &world = *game.world;

	if (IsDownFirstTime(input, BUTTON_F1)) {
		game.in_editor = !game.in_editor;

		if (game.in_editor)
			world.free_camera_p = world.last_camera_free_p;
		else
			world.last_camera_free_p = world.free_camera_p;
	}

	begin_render_frame();
		
	update_player(game, world, input, dt);
	update_enemies(game, world, input, dt);

	Camera game_camera = update_camera(game, world, input, dt);
	if (game.in_editor && IsDown(input, BUTTON_MOUSE_RIGHT))
	{
   	 	int window_width, window_height;
    	get_window_framebuffer_dimension(window_width, window_height);
		v2 mouse_p = (input.mouse_p * V2(1.f / window_width, 1.f / window_height)) * 2 - V2(1);
		mouse_p.y *= -1;
		printf("%f %f\n", mouse_p.x, mouse_p.y);
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

			mat4 scene_transform = translate(e.position) * zrotation(e.rotation.z)
				* e.scene_transform;
			for (usize j = 0; j < e.scene->meshes.count; j++) {
				Mesh &mesh = e.scene->meshes[j];
				mat4 mesh_transform = scene_transform * mesh.transform;

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
		world.editor_selected_entity = hit_id;
	}
	
	push_cube_outline(game.shadow_map.light_p, V3(0.3));
	push_line(game.shadow_map.light_p, game.shadow_map.light_p + 0.5 * game.shadow_map.light_dir);

	begin_render_pass(game.shadow_map_render_pass);
	{
		set_viewport(0, 0, game.shadow_map.width, game.shadow_map.height);
		bind_frame_buffer(game.shadow_map.framebuffer);
		clear_depth_buffer(1);
		render_entities(game, world, Camera{game.shadow_map.light_p, game.shadow_map.view, game.shadow_map.projection});
	}
	end_render_pass();

	begin_render_pass(game.mesh_render_pass);
	{
		int window_width, window_height;
		get_window_framebuffer_dimension(window_width, window_height);
		set_viewport(0, 0, window_width, window_height);

		bind_window_framebuffer();
		clear_color_buffer(0.392f, 0.584f, 0.929f, 1.f);
		clear_depth_buffer(1);
		bind_texture(4, game.shadow_map.depth_texture);
		render_entities(game, world, game_camera);
	}
	end_render_pass();

	push_line(V3(0), V3(5, 0, 0.01), V3(1, 0, 0));
	push_line(V3(0), V3(0, 5, 0.01), V3(0, 1, 0));
	push_line(V3(0), V3(0, 0, 5), V3(0, 0, 1));

	begin_render_pass(game.debug_lines_render_pass);
	{
		set_primitive_type(PRIMITIVE_LINES);
		update_vertex_buffer(game.debug_lines_vertex_buffer, (int)g_rc.debug_lines.count * sizeof(v3),
			g_rc.debug_lines.data);
		mat4 mvp = game_camera.projection * game_camera.view;
		bind_constant_buffer(game.debug_lines_constant_buffer);
		update_constant_buffer(game.debug_lines_constant_buffer, &mvp);
		draw(game.debug_lines_vertex_buffer, 0, (int)(g_rc.debug_lines.count / 2));
	}
	end_render_pass();


	{
		ImGuiIO &io = ImGui::GetIO();
		ImGui::Begin("debug");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::Checkbox("debug collission", &game.debug_collision);
        ImGui::End();
	}

	{
		Entity *e = get_entity(world, world.editor_selected_entity);

		if (e) {
			ImGuiIO &io = ImGui::GetIO();
			ImGui::Begin("Entity");
			ImGui::InputFloat("X", &e->position.x);
			ImGui::InputFloat("Y", &e->position.y);
			ImGui::InputFloat("Z", &e->position.z);
			ImGui::End();
		}
	}
	end_render_frame();

	game.time += dt;
	game.frame++;
}