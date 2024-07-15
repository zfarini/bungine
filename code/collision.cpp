struct VoxelBox
{
	v3i min;
	v3i max;
};

#define COLLISION_VOXEL_DIM (0.125f * 0.5f)
#define COLLISION_VOXEL_COUNT 3

v3i get_position_voxel(v3 p)
{
	v3i v;

	v.x = roundf(p.x / COLLISION_VOXEL_DIM);
	v.y = roundf(p.y / COLLISION_VOXEL_DIM);
	v.z = roundf(p.z / COLLISION_VOXEL_DIM);

	return v;
}


v3 get_closest_to_box(v3 box_min, v3 box_max, v3 p)
{
	v3 result;

	result.x = (p.x >= box_min.x && p.x <= box_max.x ? p.x : 
			   (p.x <= box_min.x ? box_min.x : box_max.x));
	result.y = (p.y >= box_min.y && p.y <= box_max.y ? p.y : 
			   (p.y <= box_min.y ? box_min.y : box_max.y));
	result.z = (p.z >= box_min.z && p.z <= box_max.z ? p.z : 
			   (p.z <= box_min.z ? box_min.z : box_max.z));
	return result;
}

b32 can_move(Entity &e, v3 p, Array<VoxelBox> colliders)
{
	VoxelBox box;

	box.min = get_position_voxel(p - e.collision_box);
	box.max = get_position_voxel(p + e.collision_box);

	for (usize i = 0; i < colliders.count; i++) {
		VoxelBox test = colliders[i];
		if (!(box.max.x < test.min.x ||
			  box.min.x > test.max.x ||
			  box.max.y < test.min.y ||
			  box.min.y > test.max.y ||
			  box.max.z < test.min.z ||
			  box.min.z > test.max.z))
			return 0;
	}
	return 1;
}

void move_entity_step(Game &game, Entity &e, v3 delta_p, Array<VoxelBox> colliders)
{
	v3 start_p = e.position;
	v3 target_p = start_p + delta_p;

	v3i first_voxel_p = get_position_voxel(start_p) - V3i(COLLISION_VOXEL_COUNT/2);
	
	float best_dist_sq = FLT_MAX;
	v3 best_p = start_p;

	for (int x = 0; x < COLLISION_VOXEL_COUNT; x++)
	for (int y = 0; y < COLLISION_VOXEL_COUNT; y++)
	for (int z = 0; z < COLLISION_VOXEL_COUNT; z++) {
		v3 voxel_p = V3(first_voxel_p + V3i(x, y, z)) * COLLISION_VOXEL_DIM;
		
		v3 min_box = voxel_p - V3(COLLISION_VOXEL_DIM * 0.5f);
		v3 max_box = voxel_p + V3(COLLISION_VOXEL_DIM * 0.5f);

		v3 p1 = get_closest_to_box(min_box, max_box, start_p);

		for (int j = 0; j < 2; j++)
		for (int i = 0; i < 3; i++)
		{
			v3 new_delta = delta_p;

			if (!j)
			{
				new_delta.e[(i+1)%3] = 0;
				new_delta.e[(i+2)%3] = 0;
			}
			else
				new_delta.e[i] = 0;

			v3 new_target = start_p + new_delta;

			v3 p2 = get_closest_to_box(min_box, max_box, new_target);

			float t = 0, ct = 0.2f;
			for (; t <= 1; t += ct)
			{
				v3 p = (1 - t) * p1 + t * p2;
				if (!can_move(e, p, colliders))
					break ;
			//	break ;
			}
			//push_cube(game.renderer, voxel_p, voxel_dim * 0.3, t == 0 ? V3(1, 1, 0) : V3(1));
			if (t == 0)
				continue;
			t -= ct;
			v3 p = (1 - t) * p1 + t * p2;
			float dist_sq = length_sq(p - target_p);
			if (dist_sq < best_dist_sq)
			{
				best_dist_sq = dist_sq;
				best_p = p;
			}
		}
	}

	e.position = best_p;
	//e.position = start_p + delta_p;
}

void move_entity(Game &game, Entity &e, v3 delta_p)
{
	v3 start_p = e.position;

	Arena *temp = begin_temp_memory();
	
	Array<VoxelBox> colliders = make_array_max<VoxelBox>(temp, game.entities.count);

	for (usize i = 0; i < game.entities.count; i++) {
		Entity &test = game.entities[i];

		if (test.id == e.id)
			continue;
		VoxelBox box;
		box.min = get_position_voxel(test.position - test.collision_box);
		box.max = get_position_voxel(test.position + test.collision_box);
		colliders.push(box);
	}


	float len = length(delta_p);
	int itr = 1 + roundf(len / COLLISION_VOXEL_DIM);

	//if (itr > 15)
	//	assert(0);
	for (int i = 0; i < itr; i++)
		move_entity_step(game, e, delta_p / itr, colliders);
	float eps = 1e-6;
	if (fabsf(e.position.x - start_p.x) < eps)
		e.dp.x = 0;
	if (fabsf(e.position.y - start_p.y) < eps)
		e.dp.y = 0;
	if (fabsf(e.position.z - start_p.z) < eps)
		e.dp.z = 0;

	float height_above_ground = 1000;
	for (usize i = 0; i < game.entities.count; i++) {
		Entity &test = game.entities[i];

		if (test.id == e.id)
			continue;
		v3 radius = e.collision_box + test.collision_box;

		float d = (e.position.z - e.collision_box.z) - (test.position.z + test.collision_box.z) + 0.001f;
		if (d >= 0 && 
			(e.position.x >= test.position.x - radius.x && e.position.x <= test.position.x + radius.x)
		&&  (e.position.y >= test.position.y - radius.y && e.position.y <= test.position.y + radius.y)
		&& d < height_above_ground) {
			height_above_ground = d;
		}
	}
	if (height_above_ground > 0.6)
		e.can_jump = false;
	if (height_above_ground < 0.2)
		e.can_jump = true;
	e.on_ground = height_above_ground < 0.2;

	end_temp_memory();
}