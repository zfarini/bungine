Entity *make_entity(World &world, int type, SceneID scene_id, v3 position, CollisionShape shape, mat4 scene_transform = identity())
{
	Entity e = {};

	e.id = ++world.next_entity_id; // skip 0
	e.type = type;
	e.position = position;
	e.scene_id = scene_id;
	e.scene_transform = scene_transform;
	e.shape = shape;
	e.color = V3(1);
	e.scale = V3(1);
	e.rotation = identity_quat();
	world.entities.push(e);
	world.entities_id_map[e.id] = world.entities.count - 1;
	return &world.entities[world.entities.count - 1];
}

Entity *get_entity(World &world, entity_id id)
{
#if 0
	for (int i = 0; i < world.entities.count; i++) {
		if (world.entities[i].id == id)
			return &world.entities[i];
	}
#else
	auto it = world.entities_id_map.find(id);
	if (it != world.entities_id_map.end())
		return &world.entities[it->second];
#endif
	return 0;
}

mat4 get_entity_transform(Entity &e)
{
#if 0
	return translate(e.position) 
		* zrotation(e.rotation.z) * yrotation(e.rotation.y) * xrotation(e.rotation.x) * scale(e.scale);
#else
	return translate(e.position) 
		* quat_to_mat(e.rotation) * scale(e.scale);
#endif
}

void render_player(Game &game, World &world, Camera camera, Entity &e, bool shadow_map_pass)
{
	mat4 entity_transform = get_entity_transform(e);
	mat4 scene_transform = entity_transform * e.scene_transform;
	Arena *temp = begin_temp_memory();
	Animation *final_anim = 0;
	Animation anim = {};	
	if (e.id == world.player_id) {
		usize max_nodes_count = e.curr_anim->nodes.count;
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
			for (int j = 0; j < e.curr_anim->nodes.count; j++) {
				anim.nodes[j].name = e.curr_anim->nodes[j].name;
				anim.nodes[j].transform = get_animated_node_transform(*e.curr_anim, 
						e.curr_anim->nodes[j], fmod(e.anim_time, e.curr_anim->duration));
			}
		} else {
			float t1 = fmod(e.anim_time, e.curr_anim->duration);
			float t2 = fmod(e.blend_time, e.next_anim->duration);
			float t3 = (e.blend_time) / blend_duration;

			// t3 = powf(t3, 2);
			for (int j = 0; j < e.curr_anim->nodes.count; j++) {
				quat q1, q2;
				v3 p1, s1, p2, s2;
				assert(strings_equal(e.curr_anim->nodes[j].name, e.next_anim->nodes[j].name));
				get_animated_node_transform(*e.curr_anim, e.curr_anim->nodes[j], t1, p1, s1, q1);
				get_animated_node_transform(*e.next_anim, e.next_anim->nodes[j], t2, p2, s2, q2);

				anim.nodes[j].name = e.curr_anim->nodes[j].name;
				//t3 = 1;
				v3 p = lerp(p1, p2, t3);
				quat q = quat_lerp(q1, q2, t3);
				v3 s = lerp(s1, s2, t3);

				anim.nodes[j].transform = translate(p) * quat_to_mat(q) * scale(s);

			}
		}
		final_anim = &anim;
	}
	else
		assert(!final_anim);
	bool outline = !shadow_map_pass
	&& (e.id == world.editor_selected_entity);
	render_scene(game, 
	game.scenes[e.scene_id], camera, scene_transform, final_anim, 0, e.color, outline);


	end_temp_memory();
}

void render_entities(Game &game, World &world, Camera camera, bool shadow_map_pass = false)
{
	//TODO:

	bind_constant_buffer(game.constant_buffer, 0);

	for (usize i = 0; i < world.entities.count; i++) {
		Entity &e = world.entities[i];
		if (!e.scene_id)
			continue ;

		if (e.id == world.player_id) {
			render_player(game, world, camera, e, shadow_map_pass);
			continue ;
		}

		mat4 entity_transform = get_entity_transform(e);
		mat4 scene_transform = entity_transform * e.scene_transform;

		
		//if (e.id != world.editor_selected_entity)
	
		bool outline = !shadow_map_pass
			&& (e.id == world.editor_selected_entity);
		render_scene(game, 
			game.scenes[e.scene_id], camera, scene_transform, 0, 0, e.color, outline);


		if (game.debug_collision) {
			//	|| (game.in_editor && e.id == world.editor_selected_entity)) {
			v3 color = e.id == world.editor_selected_entity ? V3(1, 1, 0) : V3(1, 0, 0);
			if (e.shape.type == COLLISION_SHAPE_TRIANGLES) {
				for (int j = 0; j < e.shape.triangles.count; j++) {
					v3 p0 = (entity_transform * V4(e.shape.triangles[j].v0, 1)).xyz;
					v3 p1 = (entity_transform * V4(e.shape.triangles[j].v1, 1)).xyz;
					v3 p2 = (entity_transform * V4(e.shape.triangles[j].v2, 1)).xyz;

					v3 n = normalize(cross(p1 - p0, p2 - p0));
					v3 o = n * 0.01f;
					push_triangle_outline(p0 + o, p1 + o, p2 + o, color);
				}
			}
			else if (e.shape.type == COLLISION_SHAPE_ELLIPSOID) {
				push_ellipsoid_outline(e.position, e.scale * e.shape.ellipsoid_radius, color);
			}
		}
	}

}

void update_player(Game &game, World &world, GameInput &input, float dt)
{
	// update player
	Entity *_player = get_entity(world, world.player_id);
	if (!_player)
		return ;
	Entity &player = *_player;

	player.color = V3(0, 1, 1);

	b32 camera_shoot_mode = false;
	b32 walk_backward = false;
	{
		v3 player_forward = normalize(V3(cosf(world.player_camera_rotation.z), sinf(world.player_camera_rotation.z), 0));
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
		bool jumped = false;
		if (IsDown(input, BUTTON_PLAYER_JUMP) && player.can_jump)
		{
			// a = a * 2;
			a += 200 * player_up;
			a.xy = {};
			jumped = true;
		}
		//if (!player.on_ground)
		//	a.xy *= 0.7f;

		a -= player.dp * 3;

		{
			v3 delta_p = 0.5f * dt * dt * a + dt * player.dp;

			move_entity(world, player, delta_p);
			player.dp += a * dt;

		}


		//if (!game.camera_free_mode)
		// {
		//     v3 camera_target_p = player.position - player_forward * 2 + player_up * 0.9 + player_right * 0.5;

		// 	camera_target_p = player.position - player_forward * 2 + player_up;
		// 	world.camera_p = camera_target_p;

		//  //   game.camera_p += 15 * dt * (camera_target_p - game.camera_p);
		// }
		{
			// TODO: !!! jump animation already has translation up?
			// TODO: there is a problem if we hold space
			Animation *next_anim = 0;
			player.anim_time += dt;
			player.blend_time += dt;

			if (!player.curr_anim)
				player.curr_anim = &game.animations[ANIMATION_GUN_IDLE];


			if (!player.on_ground) {
				if (player.curr_anim != &game.animations[ANIMATION_JUMP]
						&& player.next_anim != &game.animations[ANIMATION_JUMP]
						&& !jumped)
					next_anim = player.next_anim;
				else
					next_anim = &game.animations[ANIMATION_JUMP];
			}
			else  if (player.shooting)
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
			else if (!next_anim)
				;
			else if (!player.next_anim) {
				if (player.curr_anim != next_anim) {
					player.next_anim = next_anim;
					player.blend_time = 0;
				}
			}
			else if (next_anim == player.next_anim)
				;
			else if (next_anim != player.curr_anim) {


				// player.curr_anim = player.next_anim;
				// player.next_anim = next_anim;
				// player.anim_time = player.blend_time;
				// player.blend_time = 0;
			}
			else {
				player.next_anim = 0;
				player.blend_time = 0;
			}
#else

			if (next_anim != player.curr_anim) {
				player.anim_time = 0;
				player.curr_anim = next_anim;
			}
#endif

			int curr_anim_idx = -1;
			int next_anim_idx = -1;
			for (int i = 0; i < ANIMATION_COUNT; i++) {
				if (player.curr_anim == &game.animations[i])
					curr_anim_idx = i;
				if (player.next_anim == &game.animations[i])
					next_anim_idx = i;
			}

			// printf("%d: %d %d %d %f %f \n", game.frame, curr_anim_idx, next_anim_idx, player.on_ground, player.height_above_ground, player.blend_time);


			// if (player.next_anim
			// && fmod(player.anim_time, player.curr_anim->duration)
			//     < 0.1f * player.curr_anim->duration) {
			//     player.curr_anim = player.next_anim;
			//     player.next_anim = 0;
			//     player.anim_time = player.blend_time;
			// }
		}
	}
}

void update_enemies(Game &game, World &world, GameInput &input, float dt)
{
	for (int i = 0; i < world.entities.count; i++)
	{
		Entity &e = world.entities[i];

		if (e.type != EntityType_Enemy)
			continue;

		Entity *player = get_entity(world, world.player_id);

		v3 dir = {};
		if (player)
			dir = player->position - e.position;

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

		move_entity(world, e, delta_p);


		e.dp += a * dt;

		if (!e.on_ground)
			e.animation = &game.animations[ANIMATION_JUMP];
		else
			e.animation = &game.animations[ANIMATION_RUN];

	//	e.rotation.z = atan2(dir.y, dir.x);
		e.anim_time += dt;
	}
}	

Camera update_camera(Game &game, World &world, GameInput &input, float dt)
{
	Camera camera = {};

	Entity *player = get_entity(world, world.player_id);
	if (!player)
		assert(0);

#if 0
	if (game.in_editor && (ImGui::GetIO().WantCaptureMouse || !IsDown(input, BUTTON_MOUSE_LEFT)))
		input.mouse_dp = {};
#else
	if (game.in_editor && (ImGui::GetIO().WantCaptureMouse || !IsDown(input, BUTTON_MOUSE_LEFT)))
		input.mouse_dp = {};
#endif

	v3 camera_rot;
	if (game.in_editor) {
		world.editor_camera_rotation.x += -input.mouse_dp.y * dt * 0.2f;
		world.editor_camera_rotation.z += -input.mouse_dp.x * dt * 0.2f;

		if (world.editor_camera_rotation.x > PI / 2 - 0.001f)
			world.editor_camera_rotation.x = PI / 2 - 0.001f;
		if (world.editor_camera_rotation.x < -PI / 2 + 0.001f)
			world.editor_camera_rotation.x = -PI / 2 + 0.001f;
		camera_rot = world.editor_camera_rotation;
	}
	else {
		v2 mouse_dp = (input.mouse_dp) * 0.1f;
		v3 a = {-24*mouse_dp.y, 0, -20*mouse_dp.x};

		// TODO: learn the math to handle this correctly
		a -= world.player_camera_drotation * 10;
		world.player_camera_rotation += 0.5 * a * dt * dt + world.player_camera_drotation * dt;
		world.player_camera_drotation += a * dt;
		{
			if (world.player_camera_rotation.x >= PI / 2)
				world.player_camera_rotation.x = PI / 2;
			if (world.player_camera_rotation.x <= -PI / 2)
				world.player_camera_rotation.x = -PI / 2;
		}
		world.player_camera_rotation.z = fmod(world.player_camera_rotation.z, 2*PI);

		camera_rot = world.player_camera_rotation;
		camera_rot.z -= PI / 2;

		player->rotation = zrotation_quat(world.player_camera_rotation.z);

	//	player->rotation.z = world.player_camera_rotation.z;
		{
			v3 player_forward = normalize(V3(cosf(world.player_camera_rotation.z), 
						sinf(world.player_camera_rotation.z), 0));

			float o = camera_rot.x;

			float t[4] = {-PI/2, 0, PI/2.5, PI/2};
			assert(player->shape.type == COLLISION_SHAPE_ELLIPSOID);

			v3 v[4] = {
				player->position + V3(0, 0, player->shape.ellipsoid_radius.z*3),
				player->position - player_forward * 3 + V3(0, 0, player->shape.ellipsoid_radius.z * 0.5),
				player->position - player_forward * 1.5
					+ V3(0, 0, -player->shape.ellipsoid_radius.z +0.2),
				player->position - V3(0, 0, player->shape.ellipsoid_radius.z-0.1),

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

			world.player_camera_p = (A * V4(o*o*o, o*o, o, 1)).xyz;
			// float T = PI / 2;
			// float dO = 0.1f;
			// for (float O = -T; O <= T; O += dO) {
			// 	float lO = O - dO;

			// 	v3 v0 = (A * V4(lO*lO*lO, lO*lO, lO, 1)).xyz;
			// 	v3 v1 = (A * V4(O*O*O, O*O, O, 1)).xyz;

			// 	push_line(v0, v1, V3((O + T) / (T*2), 0, 0));
			// }
		}
	}

	// TODO: why does changing translation to first doesn't change anything wtf?

	mat4 camera_transform;
	if (game.in_editor)
		camera_transform = translate(world.editor_camera_p) * zrotation(camera_rot.z) * xrotation(camera_rot.x);
	else
		camera_transform = translate(world.player_camera_p) * zrotation(camera_rot.z) * xrotation(camera_rot.x) ;

	v3 camera_x = (camera_transform * v4{1, 0, 0, 0}).xyz;
	v3 camera_y = (camera_transform * v4{0, 0, 1, 0}).xyz;
	v3 camera_z = (camera_transform * v4{0, -1, 0, 0}).xyz;

#if 0
	if (game.in_editor && !ImGui::GetIO().WantCaptureKeyboard)
#else
		if (game.in_editor && !IsDown(input, BUTTON_LEFT_CONTROL)
				&& !ImGui::GetIO().WantCaptureKeyboard)
#endif
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
			world.editor_camera_p += normalize(camera_dp) * dt * 8;
		}

	mat4 rotation = {
		camera_x.x, camera_x.y, camera_x.z, 0,
		camera_y.x, camera_y.y, camera_y.z, 0,
		camera_z.x, camera_z.y, camera_z.z, 0,
		0, 0, 0, 1
	};

	v3 p = game.in_editor ? world.editor_camera_p : world.player_camera_p;
	mat4 view = rotation * translate(-p);


	float fov = 100;
	camera.znear = 0.1f;
	camera.zfar = 100;
	camera.width = 2 * camera.znear * tanf(DEG2RAD * (fov / 2));
	camera.height = camera.width *  (float)g_rc->window_height / g_rc->window_width;
	camera.forward = -camera_z;
	camera.right = camera_x;
	camera.up = camera_y;

	mat4 projection = perspective_projection(camera.znear, camera.zfar, fov, (float)g_rc->window_height / g_rc->window_width);

	camera.position = p;
	camera.view = view;
	camera.projection = projection;
	return camera;
}

#define S(type, fd, w, value) do {\
	if (w) fwrite(&value, sizeof(type), 1, fd); \
	else fread(&value, sizeof(type), 1, fd); \
	} while (0)

#define serialize_int(...) S(int, __VA_ARGS__)
#define serialize_b32(...) S(b32, __VA_ARGS__)
#define serialize_float(...) S(float, __VA_ARGS__)
#define serialize_usize(...) S(usize, __VA_ARGS__)


void serialize(FILE *fd, bool w, v3 &v)
{
	for (int i = 0; i < 3; i++)
		serialize_float(fd, w, v.e[i]);
}
void serialize(FILE *fd, bool w, quat &q)
{
	for (int i = 0; i < 4; i++)
		serialize_float(fd, w, q.e[i]);
}

void serialize(Arena *arena, FILE *fd, bool w, CollisionShape &shape)
{
	if (!w)
		shape = {};

	serialize_int(fd, w, shape.type);
	if (w) {
		serialize_usize(fd, w, shape.triangles.count);
		for (int i = 0; i < shape.triangles.count; i++) {
			serialize(fd, w, shape.triangles[i].v0);
			serialize(fd, w, shape.triangles[i].v1);
			serialize(fd, w, shape.triangles[i].v2);
		}
	}
	else {
		usize count;
		serialize_usize(fd, w, count);
		shape.triangles = make_array<CollisionTriangle>(arena, count);
		for (int i = 0; i < count; i++) {
			CollisionTriangle t;
			serialize(fd, w, t.v0);
			serialize(fd, w, t.v1);
			serialize(fd, w, t.v2);
			shape.triangles[i] = t;
		}
	}
	serialize(fd, w, shape.ellipsoid_radius);
}


void serialize(FILE *fd, bool w, mat4 &m)
{
	for (int i = 0; i < 16; i++)
		serialize_float(fd, w, m.e[i/4][i%4]);
}

void serialize(Arena *arena, FILE *fd, bool w, Entity &e)
{
	serialize_usize(fd, w, e.id);
	serialize_int(fd, w, e.type);
	serialize(fd, w, e.position);
	serialize(fd, w, e.dp);
	serialize(fd, w, e.rotation);
	serialize(fd, w, e.scale);
	serialize(fd, w, e.color);

	serialize_b32(fd, w, e.moved);
	serialize_b32(fd, w, e.run);
	serialize_b32(fd, w, e.shooting);
	serialize_b32(fd, w, e.can_jump);
	serialize_b32(fd, w, e.on_ground);

	serialize(arena, fd, w, e.shape);

	serialize_usize(fd, w, e.scene_id);
	serialize(fd, w, e.scene_transform);
	serialize_float(fd, w, e.height_above_ground);
}

void serialize(FILE *fd, bool w, World &world)
{
	if (w) {
		serialize_usize(fd, w, world.entities.count);
		for (int i = 0; i < world.entities.count; i++)
			serialize(&world.arena, fd, w, world.entities[i]);
	}
	else {
		usize count;
		serialize_usize(fd, w, count);
		world.entities = make_array_max<Entity>(&world.arena, 4096);
		for (int i = 0; i < count; i++) {
			Entity e = {};
			serialize(&world.arena, fd, w, e);
			world.entities.push(e);
		}

	}
	serialize_usize(fd, w, world.next_entity_id);
	serialize(fd, w, world.player_camera_p);
	serialize(fd, w, world.player_camera_rotation);
	serialize(fd, w, world.player_camera_drotation);
	serialize(fd, w, world.editor_camera_p);
	serialize(fd, w, world.editor_camera_rotation);
	serialize_usize(fd, w, world.player_id);
}

#undef S
