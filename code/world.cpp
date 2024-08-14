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
			assert(strings_equal(e.curr_anim->nodes[j].name, e.next_anim->nodes[j].name));
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

		mat4 entity_transform = get_entity_transform(world, e);
		mat4 scene_transform = entity_transform * e.scene_transform;

		Arena *temp = begin_temp_memory();

		Animation *anim = get_blended_animation(temp, e);
		render_scene(game, world, e.scene_id, camera, scene_transform, anim, 0, e.color);

		end_temp_memory();
	}
}

void update_player(Game &game, World &world, GameInput &input, float dt)
{
	// update player
	Entity *_player = get_entity(world, world.player_id);
	if (!_player)
		return ;
	Entity &player = *_player;

	b32 camera_shoot_mode = false;
	b32 walk_backward = false;
	{
		v3 player_forward = normalize(V3(cosf(world.player_camera_rotation.z), sinf(world.player_camera_rotation.z), 0));
		v3 player_up = WORLD_UP;
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
			// TODO: maybe shoot only if the animation put out the gun?
			if (game.time - player.last_gun_time > 0.3) {
				
			
				v3 dir = normalize(world.player_camera_p + 4 * player_forward
					- player.position);
				push_line(player.position, player.position + dir * 5, V3(0));
				// v3 pos = ;
				// mat4 scene_transform = identity();
				// Entity *bullet = make_entity(world, EntityType_Projectile, get_scene(game, SCENE_SPHERE), 
				// 	make_ellipsoid_shape(V3(0.1f)), scene_transform);
				// bullet->scale = V3(0.1f);
			}
		}


		//if (!player.on_ground)
		a += -40 * player_up;
		a.xy = a.xy * (player.run ? 50 : 30);
		bool jumped = false;
		if (IsDown(input, BUTTON_PLAYER_JUMP) && player.can_jump)
		{
			if (!player.pressing_jump)
				play_sound(game, game.loaded_sounds[1]);
			// a = a * 2;
			a += 200 * player_up;
			a.xy = {};
			jumped = true;
			player.pressing_jump = true;
		}
		else
			player.pressing_jump = false;
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
		}
	}
}



void update_enemies(Game &game, World &world, GameInput &input, float dt)
{
	Entity *player = get_entity(world, world.player_id);
	if (!player)
		return ;

	for (int i = 0; i < world.entities.count; i++) {
		Entity &e = world.entities[i];
		if (e.type != EntityType_Enemy)
			continue ;
		assert(e.ellipsoid_collision_shape);

		v3 targetP = player->position;
		v3 dP = {};
		v3 s = e.scale * e.ellipsoid_radius;

		bool jump = false;
		{
			auto target_cell = get_cell(targetP);
			auto start_cell = get_cell(e.position);

#if 0
			std::unordered_map<uint64_t, int> visited;

			Arena *temp = begin_temp_memory();

			Array<v3i> q = make_array<v3i>(temp, 500000);

			int l = 0, r = 0;
			q[r++] = start_cell;
			visited[pack_cell(q[0])] = 1;
			int itr = 0;

			int best_cell = 0;
			float best_length_sq = length_sq(e.position - targetP);

			Array<int> parent = make_array<int>(temp, q.capacity);
			parent[0] = -1;



			bool found = false;
			while (l < r && itr < 4096 && !found)
			{
				v3i cell = q[l++];

			//	render_cell(cell, 0.2f);

				auto try_cell = [&](v3i next) {
					if (!visited[pack_cell(next)] &&
							!world.occupied.count(pack_cell(next)))
					{
						v3 best = get_closest_point_in_cell(next, targetP);
						if (next.x == target_cell.x && next.y == target_cell.y && next.z == target_cell.z)
							found = true;
						if (length_sq(targetP - best) < best_length_sq) {
							best_length_sq = length_sq(targetP - best);
							best_cell = r;
						}
						assert(r < q.capacity);
						parent[r] = l - 1;
						visited[pack_cell(next)] = 1;
						q[r++] = next;
					}
				};

				for (int dx = -1; dx <= 1; dx++)
				for (int dy = -1; dy <= 1; dy++)
				{
					if (!world.occupied.count(pack_cell(V3i(cell.x + dx, cell.y + dy, cell.z - 1)))) {
						try_cell(cell + V3i(dx, dy, -1));
						continue ;
					}

					try_cell(cell + V3i(dx, dy, 0));
					try_cell(cell + V3i(dx, dy, +1));
				}
				itr++;
			}
			if (best_cell) {

				Array<v3i> path = make_array_max<v3i>(temp, 128);

				int curr = best_cell;
				while (parent[parent[curr]] != -1) {
					path.push(q[curr]);
					curr = parent[curr];
				}
				path.push(q[curr]);
				path.push(q[parent[curr]]);


				for (int i = 0; i < path.count; i++) {
					v3 color = V3(0, 1, 0);
					if (world.occupied.count(pack_cell(path[i])))
						color = V3(1, 0, 0);
					render_cell(path[i], 0.9f, V3(0, 1, 0));
				}

				//render_cell(q[0], 0.9f, V3(0, 1, 0));
				dP = get_closest_point_in_cell(path[path.count - 2], targetP) - e.position
					;
				if (path[path.count - 2].z > start_cell.z)
					jump = true;
				//dP = V3(path[path.count - 2]) * ASTART_CELL_DIM - V3(path[path.count - 1]) * ASTART_CELL_DIM;

			}
			else {
				dP = get_closest_point_in_cell(q[0], targetP) - e.position;
				LOG_DEBUG("best cell is current!");
			}
#else
			/*
				MAX_JUMP_CELL_COUNT
				START_JUMP_HEIGHT
				START_JUMP_IS_GOING_UP

				start_cell = {center_p, GOING_UP ? START_JUMP_HEIGHT : -1
					or 0 if we are on the ground}
			
				get_children: (state):
					if (state.jump == MAX_HEIGHT)
						return anything with z - 1 then substract the jump counter
					if (state.jumps >= 0 && state.jump < )
						childs.push(state.pos + v3(0, 0, 1))
			*/

			//int start_jump_height = roundf(e.height_above_ground / ASTART_CELL_DIM);
			//LOG_DEBUG("START JUMP HEIGHT: %d", start_jump_height);
			//bool is_going_down = !e.can_jump && e.dp.z < 0;

#endif

			auto cell_dist = [&](v3i a, v3i b) -> int {
				return roundf(10.f * sqrtf((b.x-a.x)*(b.x-a.x) + (b.y-a.y)*(b.y-a.y) + (b.z-a.z)*(b.z-a.z)));
			};

			int MAX_JUMP_CELL_COUNT = 5;

			State istate;
			istate.p = start_cell;
			if (!e.can_jump && e.dp.z < 0)
				istate.jump = -1;
			else {
				if (e.can_jump)
					istate.jump = 0;
				else {
					istate.jump = roundf((e.position.z - e.last_jump_z) / ASTART_CELL_DIM);
				}
				if (istate.jump < 0)
					istate.jump = 0;
			}
			LOG_DEBUG("istate jump: %d", istate.jump);

			auto get_state_childs = [&](State state, State *childs, int &count)
			{
#if 1
				bool on_ground = world.occupied.count(pack_cell(state.p + V3i(0, 0, -1))) != 0;

				if (on_ground)
					state.jump = 0;
				count = 0;

				if (state.jump == -1) {
					for (int dx = -1; dx <= 1; dx++)
					for (int dy = -1; dy <= 1; dy++) {
						State s = state;

						s.p = s.p + V3i(dx, dy, -1);
						childs[count++] = s;
					}
					return ;
				}

				// TODO: maybe allow this even if we are on the ground?
				
				if (!on_ground) {
				for (int dx = -1; dx <= 1; dx++)
				for (int dy = -1; dy <= 1; dy++) {
					State s = state;
					s.p = s.p + V3i(dx, dy, -1);
					s.jump = -1;
					childs[count++] = s;
				}
				}
				if (state.jump < MAX_JUMP_CELL_COUNT) {
					for (int dx = -1; dx <= 1; dx++)
					for (int dy = -1; dy <= 1; dy++) {
						State s = state;
						s.p = s.p + V3i(dx, dy, +1);
						s.jump++;
						childs[count++] = s;
					}
				}
				for (int dx = -1; dx <= 1; dx++)
				for (int dy = -1; dy <= 1; dy++) {
					State s = state;
					s.p = s.p + V3i(dx, dy, 0);
					if (!on_ground)
						s.jump = -1;
					else
						s.jump = 0;
					childs[count++] = s;
				}
#endif
				//count = 0;
				//for (int dx = -1; dx <= 1; dx++)
				//for (int dy = -1; dy <= 1; dy++) {
				//	if (!dx && !dy)
				//		continue ;
				//	State s = state;
				//	s.p = s.p + V3i(dx, dy, 0);
				//	childs[count++] = s;
				//}
			};

			std::unordered_map<State, int, StateHasher> visited;

			Arena *temp = begin_temp_memory();


			int itr = 0;
			float best_length_sq = length_sq(e.position - targetP);


			std::unordered_map<State, int, StateHasher> gScore, fScore;

			std::unordered_map<State, State, StateHasher> parent;
			State best_cell = istate;

			gScore[istate] = 0;
			fScore[istate] = cell_dist(istate.p, target_cell);
			istate.fscore = fScore[istate];
			best_length_sq = istate.fscore;

			std::set<State> set;
			set.insert(istate);
			visited[istate] = true;


			bool on_ground = world.occupied.count(pack_cell(istate.p + V3i(0, 0, -1))) != 0;
			LOG_DEBUG("PLAYER %d %d %d", target_cell.x, target_cell.y, target_cell.z);

			while (!set.empty() && itr < 2*8192)
			{
				auto state = *set.begin();

				set.erase(set.begin());

				//if (state.p.x == target_cell.x && state.p.y == target_cell.y &&
				//		state.p.z == target_cell.z) {
				//	LOG_DEBUG("FOUND BEST!");
				//	break ;
				//}
				//LOG_DEBUG("%d %d %d %d", state.p.x, state.p.y, state.p.z, state.fscore);
				render_cell(state.p, 0.2);

#if 1
				State childs[36];
				int child_count;

				get_state_childs(state, childs, child_count);
				for (int i = 0; i < child_count; i++) {
					if (!world.occupied.count(pack_cell(childs[i].p))) {
						if (childs[i].p.x == target_cell.x && childs[i].p.y == target_cell.y
								&& childs[i].p.z == target_cell.z)
							LOG_DEBUG("FOUND BEST 2 !!!");
	//					v3 p = get_closest_point_in_cell(childs[i].p, t
						int tentative_gScore = gScore[state] + cell_dist(childs[i].p, state.p);//+1
						if (!gScore.count(childs[i]) || tentative_gScore < gScore[childs[i]]) {
							parent[childs[i]] = state;
							gScore[childs[i]] = tentative_gScore;
							childs[i].fscore = fScore[childs[i]];

							fScore[childs[i]] = tentative_gScore + cell_dist(childs[i].p, target_cell);
							childs[i].fscore = fScore[childs[i]];
							if (!set.count(childs[i]))
								set.insert(childs[i]);

						}
						if (cell_dist(childs[i].p, target_cell) < best_length_sq) {
							best_length_sq = cell_dist(childs[i].p, target_cell);
							best_cell = childs[i];
						}
					}
				}
#endif
				itr++;
			}
			LOG_DEBUG("ITR = %d", itr);
#if 1
			if (parent.count(best_cell)) {

				Array<v3i> path = make_array_max<v3i>(temp, 1024);

				State s = best_cell;

				while (1) {
					path.push(s.p);
					if (!parent.count(s))
						break ;
					s = parent[s];
				}
				for (int i = 0; i < path.count; i++) {
					v3 color = V3(0, 1, 0);
					if (world.occupied.count(pack_cell(path[i])))
						color = V3(1, 0, 0);
					render_cell(path[i], 0.9f, V3(0, 1, 0));
				}

				//render_cell(q[0], 0.9f, V3(0, 1, 0));
				//dP = get_closest_point_in_cell(path[path.count - 2], targetP) - e.position
				//	;
				LOG_DEBUG("PATH LENGTH: %zu", path.count);
				if (path[path.count - 2].z > start_cell.z)
					jump = true;
				dP = V3(path[path.count - 2]) * ASTART_CELL_DIM - V3(path[path.count - 1]) * ASTART_CELL_DIM;
			}
			else {
				dP = get_closest_point_in_cell(start_cell, targetP) - e.position;
				LOG_DEBUG("best cell is current!");
			}
#endif

#if 0
			std::set<std::pair<int, uint64_t>> set;
			set.insert({cell_dist(start_cell, target_cell), pack_cell(start_cell)});

			std::unordered_map<uint64_t, uint64_t> parent;

			v3i best_cell = start_cell;

			int itr = 0;
			while (!set.empty() && itr < 1024) {
				v3i curr = unpack_cell(set.begin()->second);
				if (curr.x == target_cell.x && curr.y == target_cell.y && curr.z == target_cell.z)
					break ;
				set.erase(set.begin());


				for (int dx = -1; dx <= 1; dx++)
				for (int dy = -1; dy <= 1; dy++)
				{
					v3i next = curr + V3i(dx, dy, 0);
				}
				itr++;
			}
			dP.z = 0;
#else

#endif
			end_temp_memory();
		}
		//v3 dP = (player->position - e.position);
		//dP.z = 0;
		dP = normalize(dP);

		e.run = true;

		v2 D = normalize(dP.xy);
		quat target_rot = rotate_around_axis_quat(WORLD_UP, atan2(D.y, D.x));
		e.rotation = quat_lerp(e.rotation, target_rot, dt * 10);

		//if (length(dP) < 2) {
		//	dP = {};
		//	e.moved = false;
		//	e.run = false;
		//}
		//else
			e.moved = true;

		v3 a = dP;

		a += -(30) * WORLD_UP;
		a.xy = a.xy * 45;


		bool jumped = false;
		if (jump && e.can_jump)
		{
			a += 200 * WORLD_UP;
			a.xy = {};
			jumped = true;
			LOG_DEBUG("JUMPED!!");
			if (!e.pressing_jump)
				e.last_jump_z = e.position.z;

			e.pressing_jump = true;
			e.should_jump = false;
		}
		else
			e.pressing_jump = false;

		

		a -= e.dp * 3;
		{
			v3 delta_p = 0.5f * dt * dt * a + dt * e.dp;
			v3 old_p = e.position;
			move_entity(world, e, delta_p);
			if (length(e.position - old_p) < 0.1f * length(delta_p))
				e.should_jump = true;
			e.dp += a * dt;

		}
		{
			// TODO: !!! jump animation already has translation up?
			// TODO: there is a problem if we hold space
			Animation *next_anim = 0;
			e.anim_time += dt;
			e.blend_time += dt;

			if (!e.curr_anim)
				e.curr_anim = &game.animations[ANIMATION_GUN_IDLE];


			if (!e.on_ground) {
				if (e.curr_anim != &game.animations[ANIMATION_JUMP]
						&& e.next_anim != &game.animations[ANIMATION_JUMP]
						&& !jumped)
					next_anim = e.next_anim;
				else
					next_anim = &game.animations[ANIMATION_JUMP];
			}
			else  if (e.shooting)
				next_anim = &game.animations[ANIMATION_SHOOT];
			else if (e.run)
				next_anim = &game.animations[ANIMATION_RUN];
			else if (e.moved)
				next_anim = &game.animations[ANIMATION_FORWARD_GUN_WALK];
			else 
				next_anim = &game.animations[ANIMATION_GUN_IDLE];
#if 1
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
			else if (next_anim == e.next_anim)
				;
			else if (next_anim != e.curr_anim) {
				// e.curr_anim = e.next_anim;
				// e.next_anim = next_anim;
				// e.anim_time = e.blend_time;
				// e.blend_time = 0;
			}
			else {
				e.next_anim = 0;
				e.blend_time = 0;
			}
#else

			if (next_anim != player.curr_anim) {
				player.anim_time = 0;
				player.curr_anim = next_anim;
			}
#endif
		}
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


	camera = make_perspective_camera(view, 0.1f, 100, 100, (float)g_rc->window_height / g_rc->window_width);

	// float fov = 100;
	// camera.znear = 0.1f;
	// camera.zfar = 100;
	// camera.width = 2 * camera.znear * tanf(DEG2RAD * (fov / 2));
	// camera.height = camera.width *  (float)g_rc->window_height / g_rc->window_width;
	// camera.forward = -camera_z;
	// camera.right = camera_x;
	// camera.up = camera_y;

	// mat4 projection = perspective_projection(camera.znear, camera.zfar, fov, (float)g_rc->window_height / g_rc->window_width);

	// camera.position = p;
	// camera.view = view;
	// camera.projection = projection;
	return camera;
}

