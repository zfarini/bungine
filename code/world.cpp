Entity *make_entity(World &world)
{
	Entity e = {};

	e.id = ++world.next_entity_id;
	world.entities.push(e);
	world.entities_id_map[e.id] = world.entities.count - 1;
	return &world.entities[world.entities.count - 1];
}

Entity *make_entity(World &world, EntityType type, SceneID scene_id, v3 position, mat4 scene_transform = identity())
{
	Entity *e = make_entity(world);

	e->type = type;
	e->position = position;
	e->scene_id = scene_id;
	e->scene_transform = scene_transform;
	e->color = V3(1);
	e->scale = V3(1);
	e->rotation = identity_quat();
	return e;
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

void remove_entity(World &world, entity_id id)
{
	assert(world.entities_id_map.count(id));

	usize index = world.entities_id_map[id];
	world.entities_id_map.erase(id);
	if (index != world.entities.count - 1) {
		world.entities[index] = world.entities[world.entities.count - 1];
		world.entities_id_map[world.entities[index].id] = index;
	}
	world.entities.count--;
}

v3 get_world_p(World &world, entity_id id)
{
	Entity *e = get_entity(world, id);
	assert(e);

	v3 position = e->position;
	while (e->parent)
	{
		Entity *p = get_entity(world, e->parent);
		assert(p);
		position += p->position;
		e = p;
	}
	return position;
}

mat4 get_entity_transform(World &world, Entity &e)
{
#if 0
	return translate(e.position) 
		* zrotation(e.rotation.z) * yrotation(e.rotation.y) * xrotation(e.rotation.x) * scale(e.scale);
#else
	return translate(get_world_p(world, e.id)) 
		* quat_to_mat(e.rotation) * scale(e.scale);
#endif
}

Animation *get_blended_animation(Arena *arena, Entity &e)
{
	if (!e.curr_anim)
		return 0;

	Animation *anim = (Animation *)arena_alloc(arena, sizeof(Animation));

	usize max_nodes_count = e.curr_anim->nodes.count;
	if (e.next_anim && e.next_anim->nodes.count > max_nodes_count)
		max_nodes_count = e.next_anim->nodes.count;

	anim->nodes = make_array<NodeAnimation>(arena, max_nodes_count);
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
			anim->nodes[j].name = e.curr_anim->nodes[j].name;
			anim->nodes[j].transform = get_animated_node_transform(*e.curr_anim, 
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
			assert(e.curr_anim->nodes[j].name == e.next_anim->nodes[j].name);
			get_animated_node_transform(*e.curr_anim, e.curr_anim->nodes[j], t1, p1, s1, q1);
			get_animated_node_transform(*e.next_anim, e.next_anim->nodes[j], t2, p2, s2, q2);

			anim->nodes[j].name = e.curr_anim->nodes[j].name;
			//t3 = 1;
			v3 p = lerp(p1, p2, t3);
			quat q = quat_lerp(q1, q2, t3);
			v3 s = lerp(s1, s2, t3);

			anim->nodes[j].transform = translate(p) * quat_to_mat(q) * scale(s);

		}
	}
	return anim;
}

void render_entities(Game &game, World &world, Camera camera, bool shadow_map_pass = false)
{
	//TODO:
	bind_constant_buffer(game.constant_buffer, 0);

	for (usize i = 0; i < world.entities.count; i++) {
		Entity &e = world.entities[i];
		if (!e.scene_id)
			continue ;
		//if (e.type == EntityType_PointLight)
		//	continue;
		mat4 entity_transform = get_entity_transform(world, e);
		mat4 scene_transform = entity_transform * e.scene_transform;

		Arena *temp = begin_temp_memory();

		Animation *anim = get_blended_animation(temp, e);
		render_scene(game, world, e.scene_id, camera, scene_transform, anim, 0, e.color, shadow_map_pass);

		end_temp_memory();
	}
}

Animation *get_animation(Game &game, String name)
{
	auto it = game.animations.find(name);
	if (it == game.animations.end())
		return 0;
	return &game.loaded_animations[it->second];
}

void update_player_animation(Game &game, World &world, Entity &e, float dt)
{
	Animation *next_anim = 0;
	e.anim_time += dt;
	e.blend_time += dt;

	Animation *jump_animation = get_animation(game, "jump");

	if (!e.curr_anim)
		e.curr_anim = get_animation(game, "gun_idle");

	if (!e.on_ground) {
		if (e.curr_anim != jump_animation
				&& e.next_anim != jump_animation
				&& !e.jumped)
			next_anim = e.next_anim;
		else
			next_anim = jump_animation;
	}
	else  if (e.shooting)
		next_anim =	get_animation(game, "shoot");
	else if (e.run)
		next_anim = get_animation(game, "run");
	else if (e.walk_backward)
		next_anim = get_animation(game, "backward_gun_walk");
	else if (e.moved)
		next_anim = get_animation(game, "forward_gun_walk");
	else 
		next_anim = get_animation(game, "gun_idle");
	if (!e.curr_anim)
		e.curr_anim = next_anim;
	else if (!next_anim)
		;
	else if (!e.next_anim) {
		if (e.curr_anim != next_anim) {
			e.next_anim = next_anim;
			e.blend_time = 0;
		}
	}
	else if (next_anim == e.next_anim) {

	}
	else if (next_anim != e.curr_anim) {
	}
	else {
		e.next_anim = 0;
		e.blend_time = 0;
	}
}

void update_player(Game &game, World &world, GameInput &input, float dt)
{
	Entity *_player = get_entity(world, world.player_id);
	if (!_player)
		return ;
	Entity &player = *_player;

	v3 player_forward = normalize(V3(cosf(world.player_camera_rotation.z), sinf(world.player_camera_rotation.z), 0));
	v3 player_up = WORLD_UP;
	v3 player_right = normalize(cross(player_forward, player_up));
	v3 a = {};

	bool forward = false;

	player.walk_backward = false;
	if (IsDown(input, BUTTON_PLAYER_FORWARD))
	{
		a += player_forward;
		forward = true;
	}
	if (IsDown(input, BUTTON_PLAYER_BACKWARD))
	{
		a -= player_forward;
		player.walk_backward = true;
	}
	a = normalize(a);

	player.moved = forward;
	player.run = input.buttons[BUTTON_LEFT_SHIFT].is_down && forward;
	if (!IsDown(input, BUTTON_MOUSE_LEFT) || player.moved)
		player.shooting = false;
	else
	{
		player.shooting = true;
		player.run = false;
		player.moved = false;
		a = {};
	}

	if (!IsDown(input, BUTTON_MOUSE_RIGHT) || player.moved) {
		if (player.aiming)
			world.aim_camera_transition_t = 0;
		player.aiming = 0;
	} else {
		if (!player.aiming)
			world.aim_camera_transition_t = 0;
		player.aiming = 1;
	}

	if (player.shooting) {
		
	}

	//if (!player.on_ground)
	a += -40 * player_up;
	a.xy = a.xy * (player.run ? 50 : 30);
	player.jumped = false;
	if (IsDown(input, BUTTON_PLAYER_JUMP) && player.can_jump)
	{
		if (!player.pressing_jump)
			play_sound(game, game.loaded_sounds[1]);
		// a = a * 2;
		a += 200 * player_up;
		a.xy = {};
		player.jumped = true;
		player.pressing_jump = true;
	}
	else
		player.pressing_jump = false;

	a -= player.dp * 3;
	{
		v3 delta_p = 0.5f * dt * dt * a + dt * player.dp;

		move_entity(game, world, player, delta_p);
		player.dp += a * dt;
	}

	update_player_animation(game, world, player, dt);
	
}

void update_enemies(Game &game, World &world, GameInput &input, float dt)
{
	Entity *player = get_entity(world, world.player_id);
	if (!player)
		return ;

	for (int entity_i = 0; entity_i < world.entities.count; entity_i++) {
		Entity &e = world.entities[entity_i];
		if (e.type != EntityType_Enemy)
			continue ;
		assert(e.ellipsoid_collision_shape);

		v3 s = e.scale * e.ellipsoid_radius;

		bool jump = false;

		v3 dP = find_path_astar(world, e, player->position);
		if (dP.z) {
			jump = true;
			dP.z = 0;
		}

		dP = normalize(dP);

		e.run = true;
		e.moved = true;

		v2 D = normalize(dP.xy);
		quat target_rot = rotate_around_axis_quat(WORLD_UP, atan2(D.y, D.x));
		e.rotation = quat_lerp(e.rotation, target_rot, dt * 10);


		v3 a = dP;

		a += -(40) * WORLD_UP;
		a.xy = a.xy * 50;

		e.jumped = false;
		if (e.should_jump)
			jump = 1;
		if (jump && e.can_jump)
		{
			a += 200 * WORLD_UP;
			a.xy = {}; // TODO: !! check this
			if (!e.pressing_jump)
				e.last_jump_z = e.position.z;

			e.pressing_jump = true;
			e.should_jump = false;
			e.jumped = true;
		}
		else
			e.pressing_jump = false;

		a -= e.dp * 3;
		{
			v3 delta_p = 0.5f * dt * dt * a + dt * e.dp;
			v3 old_p = e.position;
			move_entity(game, world, e, delta_p);
			if (length(e.position - old_p) < 0.1f * length(delta_p))
				e.should_jump = true;
			e.dp += a * dt;

		}
		update_player_animation(game, world, e, dt);
		
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
			v3 player_right = normalize(cross(player_forward, V3(0, 0, 1)));
			float o = camera_rot.x;

			float t[4] = {-PI/2, 0, PI/2.5, PI/2};
			assert(player->ellipsoid_collision_shape);

			v3 player_p = get_world_p(world, player->id);

			v3 v[4] = {
				player_p + V3(0, 0, player->ellipsoid_radius.z*3),
				player_p - player_forward * 3 + V3(0, 0, player->ellipsoid_radius.z * 0.5),
				player_p - player_forward * 1.5
					+ V3(0, 0, -player->ellipsoid_radius.z +0.2),
				player_p - V3(0, 0, player->ellipsoid_radius.z-0.1),

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

			v3 target_camera_p = (A * V4(o*o*o, o*o, o, 1)).xyz;
#if 1
			if (player->aiming)
				target_camera_p += player_right * 1.f + player_forward * 0.9 + V3(0, 0, 0.7);
			else
				target_camera_p += player_right * 0.3f;
			float transition_time = 1.5f;
			if (world.aim_camera_transition_t < transition_time)
				world.player_camera_p += (15 + 40 * world.aim_camera_transition_t/transition_time) * dt * (target_camera_p - world.player_camera_p);
			else
				world.player_camera_p +=  (15 + 40) * dt * (target_camera_p - world.player_camera_p);

			world.aim_camera_transition_t = min(transition_time, dt + world.aim_camera_transition_t);
#else
			world.player_camera_p = target_camera_p;
#endif
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
		// TODO: cleanup
		if (!IsDown(input, BUTTON_LEFT_CONTROL) && !WasDown(input, BUTTON_LEFT_CONTROL))
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

	camera = make_perspective_camera(view, 0.1f, 100, 100, (float)platform.render_context->window_height / platform.render_context->window_width);

	return camera;
}

