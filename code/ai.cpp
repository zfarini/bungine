#if 0
#define AI_GRID_VOXEL_DIM (COLLISION_VOXEL_DIM * 10)
	
struct PathFindResult
{
	v3 best_p;
};

#include <set>
#include <array>
#include <map>

PathFindResult find_shorthest_path(Game &game, Entity &e, Entity &target_e)
{
	v3 target = target_e.position;
	PathFindResult result = {};

	Arena *temp = begin_temp_memory();
	Array<VoxelBox> colliders = make_array_max<VoxelBox>(temp, game.entities.count);

	for (usize i = 0; i < game.entities.count; i++) {
		Entity &test = game.entities[i];
		if (test.id == e.id || test.id == target_e.id)
			continue ;
		colliders.push(VoxelBox{get_position_voxel(test.position - test.collision_box),
								get_position_voxel(test.position + test.collision_box)});
	}

	auto get_grid_position = [](v3 p) -> v3i {
		return v3i {
			(int)roundf(p.x / AI_GRID_VOXEL_DIM),
			(int)roundf(p.y / AI_GRID_VOXEL_DIM),
			(int)roundf(p.z / AI_GRID_VOXEL_DIM)
		};
	};

	auto get_dist_to_target = [&](v3i p) {
		v3 d = (target - V3(p)*AI_GRID_VOXEL_DIM);

		int d1 = roundf(20 * length(target - V3(p) * AI_GRID_VOXEL_DIM));
		int d2 = roundf(20 * length(target - (V3(p) * AI_GRID_VOXEL_DIM + V3(0, 0, AI_GRID_VOXEL_DIM*0.4))));
		int d3 = roundf(20 * length(target - (V3(p) * AI_GRID_VOXEL_DIM - V3(0, 0, AI_GRID_VOXEL_DIM*0.4))));

		return min(d1, min(d2, d3));
	};
	std::set<std::array<int, 3>> visited;
	std::set<std::array<int, 4>> set;

	std::map<std::array<int, 3>, v3i> parent;

	std::map<std::array<int, 3>, int> f_score;
	std::map<std::array<int, 3>, int> g_score;


	v3i start_p = get_grid_position(e.position);
	v3i target_p = get_grid_position(target);

	f_score[{start_p.x, start_p.y, start_p.z}] = get_dist_to_target(start_p);
	set.insert({get_dist_to_target(start_p), start_p.x, start_p.y, start_p.z});

	v3i best_cell = start_p;
	int best_g_cost = INT_MAX;
	int itr = 0;
	while (!set.empty() && itr < 512)
	{
		auto &v = *set.begin();
		set.erase(set.begin());
		v3i p = V3i(v[1], v[2], v[3]);
		if (visited.count({p.x, p.y, p.z}))
			continue ;
		// if (p.x == target_p.x && p.y == target_p.y) {
		// 	best_cell = p;
		// 	printf("FOUND CELL!! %f\n", game.time);
		// 	break ;
		// }
		if (v[0] < best_g_cost) {
			best_g_cost = v[0];
			best_cell = p;
		}
		// if (p.x == target_p.x && p.y == target_p.y)
		// {
		// 	best_cell = p;
		//  	break ;
		// }
		visited.insert({p.x, p.y, p.z});
		//push_cube_outline(g_rc, V3(p) * AI_GRID_VOXEL_DIM, V3(AI_GRID_VOXEL_DIM) * 0.3f);

		for (int dx = -1; dx <= 1; dx++)
		for (int dy = -1; dy <= 1; dy++)
		for (int dz = -1; dz <= 1; dz++) {
			v3i new_p = p + V3i(dx, dy, dz);

			std::array<int, 3> idx = {new_p.x, new_p.y, new_p.z};
			if (visited.count(idx))
				continue ;
			if (abs(new_p.z - start_p.z) > 3 || 
			
			(!can_move(e, V3(new_p) * AI_GRID_VOXEL_DIM, colliders) &&
			 !can_move(e, V3(new_p) * AI_GRID_VOXEL_DIM + V3(0, 0, AI_GRID_VOXEL_DIM*0.4), colliders) &&
			 !can_move(e, V3(new_p) * AI_GRID_VOXEL_DIM - V3(0, 0, AI_GRID_VOXEL_DIM*0.4), colliders)))
				continue ;
			
			int new_g_score = g_score[{p.x, p.y, p.z}] + (int)roundf(10*length(V3(dx, dy, dz)));
			if (!g_score.count(idx) || new_g_score < g_score[idx]) {
				std::array<int, 4> old = {{f_score[idx], new_p.x, new_p.y, new_p.z}};
				if (set.count(old))
					set.erase(old);
				//assert(!f_score.count(idx) || f_score[idx] >= new_g_score + get_dist_to_target(new_p));
				g_score[idx] = new_g_score;
				f_score[idx] = new_g_score + get_dist_to_target(new_p);
				parent[idx] = p;
				//if (!visited.count(idx))
				set.insert({f_score[idx], new_p.x, new_p.y, new_p.z});
			}
			
		}
		itr++;
	}

	Array<v3i> path = make_array_max<v3i>(temp, 512);

	v3i p = best_cell;
	while (1) {
		if (game.debug_collision)

		push_cube_outline(g_rc, V3(p) * AI_GRID_VOXEL_DIM, V3(AI_GRID_VOXEL_DIM) * 0.45f,
			V3(0, 1, 0));
		if (!parent.count({p.x, p.y, p.z}))
			break ;
		path.push(p);
		p = parent[{p.x, p.y, p.z}];
	}

	bool clear_path = true;
	for (usize i = 2; i < path.count; i++) {
		v3i a = path[i] - path[i - 1];
		v3i b = path[1] - path[0];
		if (a.x != b.x || a.y != b.y || a.z != b.z) {
			clear_path = false;
			break ;
		}
	}


	if (path.count == 0) {
		result.best_p = target;
	}
	else {
		v3 center = V3(path[path.count - 1]) * AI_GRID_VOXEL_DIM;
		v3 box_min = center - V3(AI_GRID_VOXEL_DIM) * 0.5f;
		v3 box_max = center + V3(AI_GRID_VOXEL_DIM) * 0.5f;
		result.best_p = center + AI_GRID_VOXEL_DIM * 0.4 * normalize(V3(path[path.count - 1] - start_p));

		if (game.debug_collision)
			push_cube_outline(g_rc, result.best_p, V3(AI_GRID_VOXEL_DIM * 0.25), V3(1, 0, 0));

		
		if (path.count >= 2)
			result.best_p = get_closest_to_box(box_min, box_max, V3(path[path.count - 2]) * AI_GRID_VOXEL_DIM);
		
		//if (path[path.count -1 ].z == start_p.z)
		//	result.best_p.z = e.position.z;
	}
	end_temp_memory();
	return result;
}
#endif