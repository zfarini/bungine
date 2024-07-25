#include "debug.cpp"
#include "scene.cpp"
#include "game.h"
#include "collision.cpp"
#include "ai.cpp"

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

Entity *make_entity(Game &game, int type, Scene *scene, v3 position, CollisionShape shape, mat4 scene_transform = identity())
{
	Entity e = {};

	e.id = (int)game.entities.count;
	e.type = type;
	e.position = position;
	e.scene = scene;
	e.scene_transform = scene_transform;
	e.shape = shape;
	e.color = V3(1);
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
		constants.player_p = game.entities[0].position;
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
			draw(mesh.vertex_buffer, part.offset, part.vertices_count);
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

void render_entities(Game &game, Camera camera)
{
	bind_constant_buffer(game.constant_buffer);

	for (usize i = 0; i < game.entities.count; i++) {
		Entity &e = game.entities[i];
		if (!e.scene)
			continue ;
		Arena *temp = begin_temp_memory();
		Animation *final_anim = 0;
		Animation anim = {};	
		if (!i) {
			int max_nodes_count = e.curr_anim->nodes.count;
			if (e.next_anim && e.next_anim->nodes.count > max_nodes_count)
				max_nodes_count = e.next_anim->nodes.count;

			anim.nodes = make_array<NodeAnimation>(temp, max_nodes_count);
			if (e.next_anim)
				assert(e.curr_anim->nodes.count == e.next_anim->nodes.count);

			float blend_duration = e.next_anim ? e.next_anim->duration*0.2f : 0;
			if (e.blend_time > blend_duration && e.next_anim) {
				e.curr_anim = e.next_anim;
				e.anim_time = e.blend_time;
				e.next_anim = 0;
			}
			
			if (!e.next_anim) {
				assert(e.curr_anim);
				for (int i = 0; i < e.curr_anim->nodes.count; i++) {
					anim.nodes[i].name = e.curr_anim->nodes[i].name;
					anim.nodes[i].transform = get_animated_node_transform(*e.curr_anim, 
						e.curr_anim->nodes[i], fmod(e.anim_time, e.curr_anim->duration));
				}
			} else {
				float t1 = e.anim_time;
				float t2 = e.blend_time;
				float t3 = (e.blend_time) / blend_duration;

				for (int i = 0; i < e.curr_anim->nodes.count; i++) {
					quat q1, q2;
					v3 p1, s1, p2, s2;
					assert(strings_equal(e.curr_anim->nodes[i].name, e.next_anim->nodes[i].name));
					get_animated_node_transform(*e.curr_anim, e.curr_anim->nodes[i], t1, p1, s1, q1);
					get_animated_node_transform(*e.next_anim, e.next_anim->nodes[i], t2, p2, s2, q2);

					anim.nodes[i].name = e.curr_anim->nodes[i].name;
					//t3 = 1;
					v3 p = lerp(p1, p2, t3);
					quat q = quat_lerp(q1, q2, t3);
					v3 s = lerp(s1, s2, t3);

					anim.nodes[i].transform = translate(p) * quat_to_mat(q) * scale(s);

				}
			}
			final_anim = &anim;
		}
		else
			assert(!final_anim);

		mat4 transform = translate(e.position) * zrotation(e.rotation.z);
		render_scene(game, *e.scene, camera, transform * e.scene_transform, final_anim, 0, e.color);

		if (game.debug_collision) {
			v3 d = e.position;
			if (e.shape.type == COLLISION_SHAPE_TRIANGLES) {
				for (int j = 0; j < e.shape.triangles.count; j++) {
					v3 n = normalize(cross(e.shape.triangles[j].v1 - e.shape.triangles[j].v0,
						e.shape.triangles[j].v2 - e.shape.triangles[j].v0));
					v3 o = n * 0.01f;
					push_triangle_outline(e.shape.triangles[j].v0 + d + o,
											e.shape.triangles[j].v1 + d + o,
											e.shape.triangles[j].v2 + d + o, V3(1, 0, 0));
				}
			}
			else if (e.shape.type == COLLISION_SHAPE_ELLIPSOID) {
				push_ellipsoid_outline(d, e.shape.ellipsoid_radius, V3(1, 0, 0));
			}
		}
		end_temp_memory();
	}
}

void update_player(Game &game, GameInput &input, float dt)
{
	// update player
	b32 camera_shoot_mode = false;
	b32 walk_backward = false;
	Entity &player = game.entities[0];
	{
        v3 player_forward = normalize(V3(cosf(player.rotation.z), sinf(player.rotation.z), 0));
        v3 player_up = V3(0, 0, 1);
        v3 player_right = normalize(cross(player_forward, player_up));
        v3 a = {};

        bool forward = false;

        if (IsDown(input, BUTTON_PLAYER_FORWARD))
		{
            a += player_forward;
            forward = true;
        }
		if (IsDown(input, BUTTON_PLAYER_BACKWARD))
        {
            a -= player_forward;
            walk_backward = true;
        }
        a = normalize(a);

        player.moved = forward;
        player.run = input.buttons[BUTTON_LEFT_SHIFT].is_down && forward;
        if (!IsDown(input, BUTTON_MOUSE_LEFT) || player.run || player.moved)
            player.shooting = false;
        else
        {
            player.shooting = true;
            player.run = false;
            player.moved = false;
            a = {};

        }
      
        
		//if (!player.on_ground)
       	a += -40 * player_up;
        a.xy = a.xy * (player.run ? 50 : 30);
		if (IsDown(input, BUTTON_PLAYER_JUMP) && player.can_jump)
        {
           // a = a * 2;
            a += 200 * player_up;
			a.xy = {};
		}
		//if (!player.on_ground)
		//	a.xy *= 0.7f;

        a -= player.dp * 3;

		{
			v3 delta_p = 0.5f * dt * dt * a + dt * player.dp;
	
			move_entity(game, player, delta_p);
			player.dp += a * dt;
			
		}


		//if (!game.camera_free_mode)
        {
            v3 camera_target_p = player.position - player_forward * 2 + player_up * 0.9 + player_right * 0.5;
		
			camera_target_p = player.position - player_forward * 2 + player_up;
			game.camera_p = camera_target_p;

		 //   game.camera_p += 15 * dt * (camera_target_p - game.camera_p);
        }
		{
			// TODO: !!! jump animation already has translation up?
			Animation *next_anim = 0;

			if (!player.on_ground)
				next_anim = &game.animations[ANIMATION_JUMP];
			else if (player.shooting)
				next_anim = &game.animations[ANIMATION_SHOOT];
			else if (player.run)
				next_anim = &game.animations[ANIMATION_RUN];
			else if (walk_backward)
				next_anim = &game.animations[ANIMATION_BACKWARD_GUN_WALK];
			else if (player.moved)
				next_anim = &game.animations[ANIMATION_FORWARD_GUN_WALK];
			else 
				next_anim = &game.animations[ANIMATION_GUN_IDLE];
			
			#if 1
			if (!player.curr_anim)
				player.curr_anim = next_anim;
			else if (!player.next_anim) {
				if (player.curr_anim != next_anim) {
					player.next_anim = next_anim;
					player.blend_time = 0;
				}
			}
			else if (next_anim == player.next_anim)
				;
			else if (next_anim != player.curr_anim) {
				//printf("MULTIPLE ANIMATION BUFFERED (skipping to next_anim) %d\n", game.frame);
				player.curr_anim = player.next_anim;
				player.next_anim = next_anim;
				player.anim_time = player.blend_time;
				player.blend_time = 0;
			}
			else {
				player.next_anim = 0;
			}
			#else

			if (next_anim != player.curr_anim) {
				player.anim_time = 0;
				player.curr_anim = next_anim;
			}
			#endif

			player.anim_time += dt;
			player.blend_time += dt;
		}
	}
}

void update_enemies(Game &game, GameInput &input, float dt)
{
	for (int i = 0; i < game.entities.count; i++)
	{
		Entity &e = game.entities[i];

		if (e.type != EntityType_Enemy)
			continue;


		v3 dir = game.entities[0].position - e.position;

		if (length(dir) > 1)
			dir = normalize(dir);

		v3 a = normalize(V3(dir.x, dir.y, 0));

		a += -40 * V3(0, 0, 1);
		a.xy = a.xy * e.speed;

		if (dir.z > 0 && e.can_jump) {
			a += 200 * V3(0, 0, 1);
			a.xy *= 0.3f;
		}

		a -= e.dp * 3;

		v3 delta_p = 0.5f * dt * dt * a + dt * e.dp;

		move_entity(game, e, delta_p);

		
		e.dp += a * dt;

		if (!e.on_ground)
			e.animation = &game.animations[ANIMATION_JUMP];
		else
			e.animation = &game.animations[ANIMATION_RUN];

		e.rotation.z = atan2(dir.y, dir.x);
		e.anim_time += dt;
	}
}	

Camera update_camera(Game &game, GameInput &input, float dt)
{
	if (IsDownFirstTime(input, BUTTON_F2)) {
		if (game.camera_free_mode)
			game.last_camera_free_p = game.free_camera_p;
		else
			game.free_camera_p = game.last_camera_free_p;
		game.camera_free_mode = !game.camera_free_mode;
	}

    Entity &player = game.entities[0];

    if (!g_hide_mouse)
        input.mouse_dp = {};

    v3 camera_rot;
    if (game.camera_free_mode) {
        game.camera_rotation.x += -input.mouse_dp.y * dt * 0.2f;
        game.camera_rotation.z += -input.mouse_dp.x * dt * 0.2f;

        if (game.camera_rotation.x > PI / 2 - 0.001f)
            game.camera_rotation.x = PI / 2 - 0.001f;
        if (game.camera_rotation.x < -PI / 2 + 0.001f)
            game.camera_rotation.x = -PI / 2 + 0.001f;
        camera_rot = game.camera_rotation;
    }
    else {
        v2 mouse_dp = (input.mouse_dp) * 0.1f;
        v3 a = {-24*mouse_dp.y, 0, -20*mouse_dp.x};
        
        a -= player.drotation * 10;
        player.rotation += 0.5 * a * dt * dt + player.drotation * dt;
        player.drotation += a * dt;
        {
            if (player.rotation.x >= PI / 2)
                player.rotation.x = PI / 2;
            if (player.rotation.x <= -PI / 2)
                player.rotation.x = -PI / 2;
        }
        camera_rot = player.rotation;
        camera_rot.z -= PI / 2;
    }	
    v3 player_forward = normalize(V3(cosf(player.rotation.z), sinf(player.rotation.z), 0));
	v3 camera_p;
    {
        float o = camera_rot.x;
        
        float t[4] = {-PI/2, 0, PI/2.5, PI/2};
        v3 v[4] = {
                    player.position + V3(0, 0, player.shape.ellipsoid_radius.z*3),
                    player.position - player_forward * 3 + V3(0, 0, player.shape.ellipsoid_radius.z * 0.5),
                    player.position - player_forward * 1.5
                    + V3(0, 0, -player.shape.ellipsoid_radius.z +0.2),
                    player.position - V3(0, 0, player.shape.ellipsoid_radius.z-0.1),
                    
        };

        //for (int i = 0; i < 4; i++)
        //	push_cube_outline(v[i], V3(0.3));
        
        mat4 M;
        mat4 V;

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                M.e[i][j] = 1;
                for (int k = 0; k < 3 - i; k++)
                    M.e[i][j] *= t[j];

                if (i == 3)
                    V.e[i][j] = 0;
                else
                    V.e[i][j] = v[j].e[i];
            }
        }
        
        M = inverse(M);

        mat4 A = V * M;

        camera_p = (A * V4(o*o*o, o*o, o, 1)).xyz;
        // float T = PI / 2;
        // float dO = 0.1f;
        // for (float O = -T; O <= T; O += dO) {
        // 	float lO = O - dO;

        // 	v3 v0 = (A * V4(lO*lO*lO, lO*lO, lO, 1)).xyz;
        // 	v3 v1 = (A * V4(O*O*O, O*O, O, 1)).xyz;

        // 	push_line(v0, v1, V3((O + T) / (T*2), 0, 0));
        // }
    }
	// TODO: why does changing translation to first doesn't change anything wtf?
	mat4 camera_transform =  translate(camera_p)*zrotation(camera_rot.z) * xrotation(camera_rot.x) ;

    if (game.camera_free_mode)
        camera_transform = translate(game.free_camera_p) * zrotation(camera_rot.z) * xrotation(camera_rot.x);
    
    v3 camera_x = (camera_transform * v4{1, 0, 0, 0}).xyz;
    v3 camera_y = (camera_transform * v4{0, 0, 1, 0}).xyz;
    v3 camera_z = (camera_transform * v4{0, -1, 0, 0}).xyz;

    if (game.camera_free_mode)
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
        game.free_camera_p += normalize(camera_dp) * dt * 8;
    }

    mat4 rotation = {
        camera_x.x, camera_x.y, camera_x.z, 0,
        camera_y.x, camera_y.y, camera_y.z, 0,
        camera_z.x, camera_z.y, camera_z.z, 0,
        0, 0, 0, 1
    };

    v3 p = game.camera_free_mode ? game.free_camera_p : camera_p;
    mat4 view = rotation * translate(-p);

    int window_width, window_height;
    get_window_framebuffer_dimension(window_width, window_height);
    mat4 projection = perspective_projection(0.1, 100, 100, (float)window_height / window_width);

	Camera camera = {};
	camera.position = p;
	camera.view = view;
	camera.projection = projection;
	return camera;
}

void game_update_and_render(Game &game, Arena *memory, GameInput &input, float dt)
{
	if (!game.is_initialized) {
		arena_alloc(memory, sizeof(game));
		//memory->used += sizeof(game);


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

		game.entities = make_array_max<Entity>(memory, 4096);


		Entity *player = make_entity(game, EntityType_Player, &game.ch43, V3(0, 0, 8), make_ellipsoid_shape(V3(0.55f, 0.55f, 0.95f)));
		//player->scene = &game.sphere_asset;
		//player->scene = 0;
		//player->shape.ellipsoid_radius = V3(0.2, 0.2, 0.2); 
		//player->scene_transform =  scale(player->shape.ellipsoid_radius);
		player->scene_transform =  translate(0, 0, -player->shape.ellipsoid_radius.z) * zrotation(3*PI/2) * scale(V3(1.1));
		player->color = V3(0.2, 0.8, 0.8);
		
		//Entity *sh = make_entity(game, EntityType_Player, &game.sphere_asset, V3(0, 0, 2), V3(0.3, 0.3, 0.8) );
		
		// for (int i = 0; i < 2; i++)
		// 	for (int j = 0; j < 2; j++) {
		// 	Entity *enemy = make_entity(game, EntityType_Enemy, &game.ch43, V3(i * 2, j * 2, 6), player->shape);
		// 	enemy->scene_transform = player->scene_transform;
		// 	enemy->speed = ((float)rand() / RAND_MAX)*50 + 20;
		// }

		//player->rotation.x = -PI/8;

		//Entity *sponza = make_entity(game, V3(0), &game.sponza);


		game.animations[ANIMATION_JUMP] = load_scene(&game.asset_arena, "data/jump.fbx").animations[0];
		game.animations[ANIMATION_SHOOT] = load_scene(&game.asset_arena, "data/shoot.fbx").animations[0];
		game.animations[ANIMATION_RUN] = load_scene(&game.asset_arena, "data/run.fbx").animations[0];
		game.animations[ANIMATION_FORWARD_GUN_WALK] = load_scene(&game.asset_arena, "data/forward_gun_walk.fbx").animations[0];
		game.animations[ANIMATION_BACKWARD_GUN_WALK] = load_scene(&game.asset_arena, "data/backward_gun_walk.fbx").animations[0];
		game.animations[ANIMATION_GUN_IDLE] = load_scene(&game.asset_arena, "data/gun_idle.fbx").animations[0];

		// {
		// 	//@HACK
		// 	auto &jump = game.animations[ANIMATION_JUMP];
		// 	auto &node = jump.nodes[0];
		// 	for (int i = 0; i < node.position.count; i++)
		// 		node.position[i].y -= 50;
		// }

		Entity *boxes[] = {
        	make_entity(game, EntityType_Static, &game.cube_asset, V3(0, 0, -0.5), make_box_shape(memory, V3(25, 25, 0.5))),
        	make_entity(game, EntityType_Static, &game.cube_asset, V3(0, 25, 0), make_box_shape(memory, (V3(25, 0.5, 25)))),
        	make_entity(game, EntityType_Static, &game.cube_asset, V3(10, 6, 1), make_box_shape(memory, (V3(5, 5.8, 0.3)))),
        	make_entity(game, EntityType_Static, &game.cube_asset, V3(10, 6, 1.3), make_box_shape(memory, (V3(2.5, 2.5, 0.3)))),
        	make_entity(game, EntityType_Static, &game.cube_asset, V3(10, 6, 1.6), make_box_shape(memory, (V3(1.25, 1.25, 0.3)))),
        	make_entity(game, EntityType_Static, &game.cube_asset, V3(-10, 6, 1), make_box_shape(memory, (V3(7, 7, 0.3)))),
        	make_entity(game, EntityType_Static, &game.cube_asset, V3(1, 6, 0),  make_box_shape(memory, (V3(4, 1, 2)))),
        	make_entity(game, EntityType_Static, &game.cube_asset, V3(0, -7, 1), make_box_shape(memory, (V3(4, 4, 0.3)))),
        	make_entity(game, EntityType_Static, &game.cube_asset, V3(0, -7, 1.6), make_box_shape(memory, (V3(2, 2, 0.3)))),

			make_entity(game, EntityType_Static, &game.cube_asset, V3(7, -7, 0.1), make_box_shape(memory, (V3(3, 3, 0.1)))),

		};
		for (int i = 0; i < ARRAY_SIZE(boxes); i++)
			boxes[i]->scene_transform = scale(boxes[i]->shape.box_radius);
		
		game.last_camera_free_p = V3(0, 0, 3);

		game.is_initialized = 1;
	}

	begin_render_frame();
		
	update_player(game, input, dt);
	update_enemies(game, input, dt);

	Camera game_camera = update_camera(game, input, dt);
	
	push_cube_outline(game.shadow_map.light_p, V3(0.3));
	push_line(game.shadow_map.light_p, game.shadow_map.light_p + 0.5 * game.shadow_map.light_dir);

	begin_render_pass(game.shadow_map_render_pass);
	{
		set_viewport(0, 0, game.shadow_map.width, game.shadow_map.height);
		bind_frame_buffer(game.shadow_map.framebuffer);
		clear_depth_buffer(1);
		render_entities(game, Camera{game.shadow_map.light_p, game.shadow_map.view, game.shadow_map.projection});
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
		render_entities(game, game_camera);
	}
	end_render_pass();

	push_line(V3(0), V3(5, 0, 0.01), V3(1, 0, 0));
	push_line(V3(0), V3(0, 5, 0.01), V3(0, 1, 0));
	push_line(V3(0), V3(0, 0, 5), V3(0, 0, 1));

	begin_render_pass(game.debug_lines_render_pass);
	{
		set_primitive_type(PRIMITIVE_LINES);
		update_vertex_buffer(game.debug_lines_vertex_buffer, g_rc.debug_lines.count * sizeof(v3),
			g_rc.debug_lines.data);
		mat4 mvp = game_camera.projection * game_camera.view;
		bind_constant_buffer(game.debug_lines_constant_buffer);
		update_constant_buffer(game.debug_lines_constant_buffer, &mvp);
		draw(game.debug_lines_vertex_buffer, 0, g_rc.debug_lines.count / 2);
	}
	end_render_pass();


	{
		ImGuiIO &io = ImGui::GetIO();
		ImGui::Begin("debug");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::Checkbox("debug collission", &game.debug_collision);
        ImGui::End();
	}
	end_render_frame();

	game.time += dt;
	game.frame++;
}