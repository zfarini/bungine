#include "debug.cpp"
#include "scene.cpp"
#include "game.h"
#include "collision.cpp"
#include "ai.cpp"

Entity *make_entity(Game &game, int type, Scene *scene, v3 position, v3 collision_box, mat4 scene_transform = identity())
{
	Entity e = {};

	e.id = (int)game.entities.count;
	e.type = type;
	e.position = position;
	e.scene = scene;
	e.scene_transform = scene_transform;
	e.collision_box = collision_box;
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

void render_scene(RenderContext &rc, Game &game, Scene &scene, SceneNode *node, mat4 scene_transform, mat4 node_transform,
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
		constants.player_p = game.entities[0].position;

		mat4 mesh_transform = scene_transform * node_transform * node->geometry_transform;

		//render_bones(rc, mesh.bones, mesh_transform, anim, anim_time);

		UINT32 stride = sizeof(Vertex);
		UINT32 offset = 0;
		rc.context->IASetVertexBuffers(0, 1, &mesh.vertex_buffer, &stride, &offset);

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
		render_scene(rc, game, scene, node->childs[i], scene_transform, node_transform, anim, anim_time);
}

void render_scene(RenderContext &rc, Game &game, Scene &scene, mat4 transform = identity(), Animation *anim = 0, float anim_time = 0)
{
	if (anim)
		anim_time = fmod(anim_time, anim->duration);
	render_scene(rc, game, scene, scene.root, transform, identity(), anim, anim_time);
}

void render_entities(RenderContext &rc, Game &game)
{
	for (usize i = 0; i < game.entities.count; i++) {
		Entity &e = game.entities[i];
		if (!e.scene)
			continue ;
		mat4 transform = translate(e.position) * zrotation(e.rotation.z);
		render_scene(rc, game, *e.scene, transform * e.scene_transform, e.animation, e.anim_time);
		if (game.debug_collision)
			push_cube_outline(rc, e.position, e.collision_box);
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

		rc.shadow_map = create_shadow_map(rc, 4096, 4096, 
			v3{24, 0, 24}, v3{-1, 0, -1},
			orthographic_projection(1, 75, 50, 40));

		game.mesh_render_pass = create_render_pass(rc, L"code\\shader.hlsl", "vs_main", "ps_main",
			input_layout_desc, ARRAYSIZE(input_layout_desc), sizeof(Constants));

		game.shadow_map_render_pass = create_render_pass(rc, L"code\\shader.hlsl", "vs_main", 0,
			input_layout_desc, ARRAYSIZE(input_layout_desc), sizeof(Constants));


		game.ch43		= load_scene(&game.asset_arena, rc, "data\\Ch43.fbx");
		//Scene ch06		= load_scene(&asset_arena, rc, "data\\Ch06.fbx");
		//Scene ch15		= load_scene(&asset_arena, rc, "data\\Ch15.fbx");
		//game.ch43 = load_scene(&game.asset_arena, rc, "data\\animated_pistol\\animated.fbx");
		//game.ch43 = load_scene(&game.asset_arena, rc, "data\\animated_pistol\\animated.fbx");
		//game.sponza		= load_scene(&game.asset_arena, rc, "data\\Sponza\\Sponza.fbx");
		game.cube_asset		= load_scene(&game.asset_arena, rc, "data\\cube.fbx");
		
		//game.ch43.root->local_transform = translate(0, 0, 4) * game.ch43.root->local_transform * scale(0.005f);
		//game.test_anim = load_scene(&game.asset_arena, rc, "data\\Fast Run.fbx").animations[0];

		game.entities = make_array_max<Entity>(memory, 4096);


		Entity *player = make_entity(game, EntityType_Player, &game.ch43, V3(0, 0, 2), V3(0.3, 0.3, 0.8) );

		player->scene_transform =  translate(0, 0, -player->collision_box.z) * zrotation(PI/2 + PI);
		
		Entity *enemy = make_entity(game, EntityType_Player, &game.ch43, V3(0, 3, 0.8), player->collision_box);
		enemy->scene_transform = player->scene_transform;

		//player->rotation.x = -PI/8;

		//Entity *sponza = make_entity(game, V3(0), &game.sponza);


		game.animations[ANIMATION_JUMP] = load_scene(&game.asset_arena, rc, "data\\jump.fbx").animations[0];
		game.animations[ANIMATION_SHOOT] = load_scene(&game.asset_arena, rc, "data\\shoot.fbx").animations[0];
		game.animations[ANIMATION_RUN] = load_scene(&game.asset_arena, rc, "data\\run.fbx").animations[0];
		game.animations[ANIMATION_FORWARD_GUN_WALK] = load_scene(&game.asset_arena, rc, "data\\forward_gun_walk.fbx").animations[0];
		game.animations[ANIMATION_BACKWARD_GUN_WALK] = load_scene(&game.asset_arena, rc, "data\\backward_gun_walk.fbx").animations[0];
		game.animations[ANIMATION_GUN_IDLE] = load_scene(&game.asset_arena, rc, "data\\gun_idle.fbx").animations[0];


		Entity *boxes[] = {
        	make_entity(game, EntityType_Static, &game.cube_asset, V3(0, 0, -0.5), (V3(25, 25, 0.5))),
        	make_entity(game, EntityType_Static, &game.cube_asset, V3(0, 25, 0), (V3(25, 0.5, 25))),
        	make_entity(game, EntityType_Static, &game.cube_asset, V3(10, 6, 1), (V3(5, 5.8, 0.3))),
        	make_entity(game, EntityType_Static, &game.cube_asset, V3(10, 6, 1.3), (V3(2.5, 2.5, 0.3))),
        	make_entity(game, EntityType_Static, &game.cube_asset, V3(10, 6, 1.6), (V3(1.25, 1.25, 0.3))),
        	make_entity(game, EntityType_Static, &game.cube_asset, V3(-10, 6, 1), (V3(7, 7, 0.3))),
        	make_entity(game, EntityType_Static, &game.cube_asset, V3(1, 6, 0),  (V3(4, 1, 2))),
        	make_entity(game, EntityType_Static, &game.cube_asset, V3(0, -7, 1), (V3(4, 4, 0.3))),
        	make_entity(game, EntityType_Static, &game.cube_asset, V3(0, -7, 1.6), (V3(2, 2, 0.3))),
		};
		for (int i = 0; i < ARRAY_SIZE(boxes); i++)
			boxes[i]->scene_transform = scale(boxes[i]->collision_box);
		
		game.last_camera_free_p = V3(0, 0, 3);
		game.is_initialized = 1;

	}
	//assert(0 && "render player in rest pose and check if shadow is correct");
	int window_width, window_height;
	{
		ID3D11Texture2D* backbuffer;
		rc.swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backbuffer);
		D3D11_TEXTURE2D_DESC desc;
		backbuffer->GetDesc(&desc);
		window_width = desc.Width;
		window_height = desc.Height;
	}

	if (IsDownFirstTime(input, BUTTON_F1)) {
		if (game.camera_free_mode)
			game.last_camera_free_p = game.free_camera_p;
		else
			game.free_camera_p = game.last_camera_free_p;
		game.camera_free_mode = !game.camera_free_mode;
	}
	if (IsDownFirstTime(input, BUTTON_F2)) {
		game.debug_collision = !game.debug_collision;
	}

	begin_frame(rc, rc.view, rc.projection);
	
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
      
        if (IsDown(input, BUTTON_PLAYER_JUMP) && player.can_jump)
        {
            a = a * 2;
            a += 25 * player_up;
        }
		//if (!player.on_ground)
       	a += -4 * player_up;
        a = a * (player.run ? 52 : 12);

        a -= player.dp * 3;

		{
			v3 delta_p = 0.5f * dt * dt * a + dt * player.dp;
			move_entity(game, player, delta_p);
			player.dp += a * dt;
			/*
				p' = 0.5*dt*dt*a + dt * v + p
				v' = a * dt +  v
				a
			*/
		}


		//if (!game.camera_free_mode)
        {
            v3 camera_target_p = player.position - player_forward * 2 + player_up * 0.9 + player_right * 0.5;
		
			camera_target_p = player.position - player_forward * 2 + player_up;
			game.camera_p = camera_target_p;

		 //   game.camera_p += 15 * dt * (camera_target_p - game.camera_p);
        }
		{
			Animation *new_anim;

			if (!player.on_ground)
				new_anim = &game.animations[ANIMATION_JUMP];
			else if (player.shooting)
				new_anim = &game.animations[ANIMATION_SHOOT];
			else if (player.run)
				new_anim = &game.animations[ANIMATION_RUN];
			else if (walk_backward)
				new_anim = &game.animations[ANIMATION_BACKWARD_GUN_WALK];
			else if (player.moved)
				new_anim = &game.animations[ANIMATION_FORWARD_GUN_WALK];
			else 
				new_anim = &game.animations[ANIMATION_GUN_IDLE];
			if (player.animation != new_anim) {
				player.anim_time = 0;
				player.animation = new_anim;
			}
			else 
				player.anim_time += dt;
		}

		//push_cylinder(game.renderer, player.position, 0.3, 1, V3(1, 0, 0));
	}

#if 0
	{
		Entity &e = game.entities[1];
		PathFindResult result = find_shorthest_path(game, e, game.entities[0]);
		
		v3 dir = result.best_p - e.position;

		if (length(dir) > 1)
			dir = normalize(dir);

		v3 a = normalize(V3(dir.x, dir.y, 0));

		if (dir.z > 0 && e.can_jump) {
			a = a * 2;
			a += 25 * V3(0, 0, 1);
		}

		a += -4 * V3(0, 0, 1);
		a = a * 10;
		
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
#endif

	mat4 view, projection;
	{
		v3 camera_rot;
		if (game.camera_free_mode) {
			game.camera_rotation.x += -input.mouse_dp.y * dt * 0.2f;
			game.camera_rotation.z += -input.mouse_dp.x * dt * 0.2f;

			// avoid gimbal lock
			if (game.camera_rotation.x > PI / 2 - 0.001f)
				game.camera_rotation.x = PI / 2 - 0.001f;
			if (game.camera_rotation.x < -PI / 2 + 0.001f)
				game.camera_rotation.x = -PI / 2 + 0.001f;
			camera_rot = game.camera_rotation;
		}
		else {
			v2 mouse_dp = (input.mouse_dp) * 0.1f;
			v3 a = {};

			a.z = -mouse_dp.x;
			a.x = -mouse_dp.y;

			// p' = 1/2*a*t*t + v*t + p
			// v' = a*t + v
			// a' = a

			a.z *= 20;
			a.x *= 5;
			if (camera_shoot_mode)
				a.x *= 8;
			else
				a.x *= 4;

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
        v3 player_up = V3(0, 0, 1);
        v3 player_right = normalize(cross(player_forward, player_up));
		player_right = normalize(cross(player_up, game.camera_p - player.position));
		
		if (game.camera_free_mode && IsDown(input, BUTTON_MOUSE_LEFT)) {
			game.rot += dt*5;
		}
		if (game.camera_free_mode && IsDown(input, BUTTON_MOUSE_RIGHT)) {
			game.rot -= dt*5;
		}
		printf("%f %d\n", game.rot, game.frame);

		v3 camera_p = player.position + (rotate_around_axis(player_right, camera_rot.x + game.rot) * V4(game.camera_p - player.position, 0)).xyz;

		{
			v3 p0 = player.position - player_forward * 2; 
			v3 p2 = player.position + V3(0, 0, player.collision_box.z*3);
			v3 p1 = player.position - V3(0, 0, player.collision_box.z-0.1);

			v3 a, b, c;

			float T = PI / 2;
			c = p0;
			a = (p1 + p2 - 2 * p0) / (2 * T * T);
			b = (p1-p2) / (2 * T);

			float o = camera_rot.x + game.rot;

			//o = T;
			
			//o = fmod(o + PI/2, PI) - PI/2;

			float dO = 0.1f;
			for (float O = -T; O <= T; O += dO) {
				float lO = O - dO;

				v3 v0 = a*lO*lO + b * lO + c;
				v3 v1 = a*O*O + b * O  + c;
				push_line(rc, v0, v1, V3((O + T) / (T*2), 0, 0));
			}
			camera_p = a*o*o + b * o + c;

			/*
				f(0) = p0
				f(PI/2) = p1
				f(-PI/2) = p2

				f(x) = a*x^2+b*x+p0x

				a*T^2+b*T+p0x = p1x
				a*T^2-b*T+p0x = p2x



				2*b*T = p1x - p2x

				b = (p1x - p2x) / (2*T)

				a  = (p1x + p2x - 2* p0x) / (2 * T^2)


				c = p0
			*/
		}
		//v3 camera_p = player.position - player_forward * 2 + player_up * 0.9 + player_right * 0.5;


		// if (game.camera_free_mode && IsDown(input, BUTTON_LEFT_SHIFT)) {
		// 	game.rot += dt*5;
		// }
		
		//camera_p = game.camera_p + 
	//	camera_p = xrotation(game.rot) * translate(-player.position);
 		

       
		mat4 camera_transform =  
			//translate(game.camera_p) *
			zrotation(camera_rot.z) *
			xrotation(camera_rot.x) *
			translate(camera_p);
		
		push_cube_outline(rc, camera_p, V3(1), V3(1, 0, 0));
		//push_cube_outline(rc, player.position, V3(1), V3(1, 1, 0));
		push_line(rc, camera_p, player.position);
		//push_line(rc, player.position, player.position + player_right, V3(0, 0, 1));
		//push_line(rc, player.position, game.camera_p, V3(0, 1, 0));


		if (game.camera_free_mode) {
			camera_transform = translate(game.free_camera_p) * zrotation(camera_rot.z) * xrotation(camera_rot.x);
		}
		//camera_transform = identity();

		//view = translate(-camera_p);

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
		view = rotation * translate(-p);
		projection = perspective_projection(0.1, 100, 120, (float)window_height / window_width);
	}
	rc.view = view;
	rc.projection = projection;

	
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
	game.frame++;
}