#define AI_GRID_VOXEL_DIM (COLLISION_VOXEL_DIM * 10)
	
struct PathFindResult
{
	v3 best_p;
};

PathFindResult find_shorthest_path(Game &game, Entity &e, v3 target)
{
	PathFindResult result = {};

	v3i start_p = {
		(int)roundf(e.position.x / AI_GRID_VOXEL_DIM),
		(int)roundf(e.position.y / AI_GRID_VOXEL_DIM),
		(int)roundf(e.position.z / AI_GRID_VOXEL_DIM)
	};

	v3i target_p = {
		(int)roundf(target.x / AI_GRID_VOXEL_DIM),
		(int)roundf(target.y / AI_GRID_VOXEL_DIM),
		(int)roundf(target.z / AI_GRID_VOXEL_DIM)
	};

	v3i max_search_dim = {
		51, 51, 5
	};

	v3i half_dim = V3i(max_search_dim.x/2, max_search_dim.y/2, max_search_dim.z/2);
	v3i min_cell = -half_dim;
	v3i max_cell =  half_dim;

	
	v3i dim_bits = {
		(int)ceilf(log2f(max_search_dim.x)),
		(int)ceilf(log2f(max_search_dim.y)),
		(int)ceilf(log2f(max_search_dim.z))
	};

	auto index_to_cell = [&](int idx) {
		int x = idx & ((1 << dim_bits.x) - 1);
		int y = (idx >> dim_bits.x) & ((1 << dim_bits.y) - 1);
		int z = (idx >> (dim_bits.x + dim_bits.y)) & ((1 << dim_bits.z) - 1);
		return V3i(x, y, z) - half_dim;
	};

	auto cell_to_index = [&](v3i cell) {
		cell = cell + half_dim;
		return cell.x | (cell.y << dim_bits.x) | (cell.z << (dim_bits.x + dim_bits.y));
	};

	int cell_count = (max_cell.x - min_cell.x + 1) * (max_cell.y - min_cell.y + 1) * (max_cell.z - min_cell.z + 1);
	cell_count = (1 << (dim_bits.x + dim_bits.y + dim_bits.z));


	Arena *temp = begin_temp_memory();
	Array<b32> visited = make_zero_array<b32>(temp, cell_count);
	Array<int> queue = make_array<int>(temp, 1000*cell_count);

	Array<VoxelBox> colliders = make_array_max<VoxelBox>(temp, game.entities.count);
	for (usize i = 0; i < game.entities.count; i++) {
		Entity &test = game.entities[i];

		if (test.id != e.id)
			colliders.push(VoxelBox{get_position_voxel(test.position - test.collision_box),
									get_position_voxel(test.position + test.collision_box)});
	}
	
	int l = 0, r = 0;

	queue[r++] = cell_to_index(V3i(0));
	visited[queue[0]] = 1;

	Array<int> parent = make_array<int>(temp, cell_count);
	Array<int> dist = make_zero_array<int>(temp, cell_count);

	parent[queue[0]] = -1;
	dist[queue[0]] = 1;

	auto in_range = [&](v3i p) {
		return p.x >= min_cell.x && p.x <= max_cell.x &&
				p.y >= min_cell.y && p.y <= max_cell.y &&
				p.z >= min_cell.z && p.z <= max_cell.z;
	};

	while (l < r)
	{
		int parent_idx = queue[l++];
		v3i curr = index_to_cell(parent_idx);

		//if (curr.x == target_p.x && curr.y == target_p.y)
		//	break ;

		for (int dx = -1; dx <= 1; dx++)
		for (int dy = -1; dy <= 1; dy++)
		for (int dz = -1; dz <= 1; dz++) {
			v3i p = curr + V3i(dx, dy, dz);

			int d = dist[parent_idx] + 10 + 100*((dx!=0)+(dy!=0)+(dz!=0) > 1);

			if (in_range(p) &&
				(!visited[cell_to_index(p)] || d < dist[cell_to_index(p)]) && 
				can_move(e, V3(start_p + p) * AI_GRID_VOXEL_DIM, colliders))
			{
				parent[cell_to_index(p)] = parent_idx;
				visited[cell_to_index(p)] = 1;
				dist[cell_to_index(p)] = d;
				queue[r++] = cell_to_index(p);
			}
		}
	}

	Array<int> in_path = make_zero_array<int>(temp, cell_count);
	v3i dP = target_p - start_p;
	dP.z = 0;
	bool moved = false;
	if (!in_range(dP) || !visited[cell_to_index(dP)]) {
		float best_dist_sq = FLT_MAX;
		int best = queue[0];
		for (int i = 0; i < cell_count; i++) {
			if (!visited[i]) continue ;
			float d = length_sq((V3(start_p + index_to_cell(i))) * AI_GRID_VOXEL_DIM - target);
	
			if (d < best_dist_sq) {
				best_dist_sq = d;
				best = i;
			}
		}
		dP = index_to_cell(best);
	}
	if (in_range(dP) && visited[cell_to_index(dP)])
	{
		int curr = cell_to_index(dP);

		int last_move = -1;
		while (1)
		{
			in_path[curr] = 1;
			if (dist[curr] == 1)
				break;
			int best = -1;
			int best_dist = INT_MAX;
			for (int dx = -1; dx <= 1; dx++)
			for (int dy = -1; dy <= 1; dy++)
			for (int dz = -1; dz <= 1; dz++) {
				v3i p = index_to_cell(curr) + V3i(dx, dy, dz);
				if (in_range(p) && visited[cell_to_index(p)]
				&& dist[cell_to_index(p)] < best_dist) {
					best = cell_to_index(p);
					best_dist = dist[cell_to_index(p)];
				}
			}
			if (best == -1)
				break ;
			if (dist[best] != 1)
				last_move = best;
			curr = best;
		}
		if (last_move != -1) {
			v3 P = V3(start_p + index_to_cell(last_move)) * AI_GRID_VOXEL_DIM;

			result.best_p = P;

		}
	}
	// if (!moved) {
	// 	e.animation = &game.animations[ANIMATION_GUN_IDLE];
	// 	//e.anim_time = 0;
	// }
	
	for (int x = min_cell.x; x <= max_cell.x; x++)
	for (int y = min_cell.y; y <= max_cell.y; y++)
	for (int z = min_cell.z; z <= max_cell.z; z++) {
		v3i p = V3i(x, y, z);
		v3 pos = V3(start_p + p) * AI_GRID_VOXEL_DIM;
		b32 can = can_move(e, pos, colliders);
		b32 vis = visited[cell_to_index(p)];

		v3 color = V3(1);
		if (vis)
			color = V3(1, 1, 0);
		if (in_path[cell_to_index(p)])
			color = V3(0, 1, 0);
		else
			continue ;
		push_cube_outline(g_rc, pos, V3(AI_GRID_VOXEL_DIM*0.4f), color);
	}

	end_temp_memory();

	return result;
}