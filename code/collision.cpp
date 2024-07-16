#if 0
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

#endif

mat4 gM;
int t_idx;

v3 hit_normal = V3(0);
v3 inter;

float intersect_line(v3 l0, v3 l1, v3 ln, v3 targetP, v3 normal, float &t)
{
	//if (dot(ln, targetP) > 0)
	//	return FLT_MAX;

	v3 A = l0;
	v3 B = l1;
	v3 D = targetP;
	v3 d = normalize(B - A);
	v3 V = -(-A + dot(A, d) * d);
	v3 X = D - d * dot(D, d);

	float a = dot(X, X);
	float b = -2*dot(X, V);
	float c = dot(V, V) - 1;
	float delta = b * b - 4 * a * c;
	if (delta < 0 || fabsf(a) < 1e-6)
		return t;
	delta = sqrtf(delta);
	float t0 = (-b + delta)/(2*a);
	float t1 = (-b - delta)/(2*a);
	

	auto check_t = [&](float T) {
		if (T < 0 || T > t) return ;

		v3 p = T * targetP;
		v3 v = d * dot(p - A, d);
		p = p - ((p - A) - v);
		//push_cube_outline(g_rc, (gM * V4(p, 1)).xyz, V3(0.1), V3(0, 1, 1));
		//push_cube_outline(g_rc, (gM * V4(l0, 1)).xyz, V3(0.1), V3(0));
		//push_cube_outline(g_rc, (gM * V4(l1, 1)).xyz, V3(0.1), V3(0));
		//printf("BLA: %f\n", fabsf(length(p - (A + d * dot(p - A, d)))));

		if (dot(p - l0, l1 - l0) >= 0
		&& length_sq(p - l0) <= length_sq(l1 - l0)) {
			t = T;
			hit_normal = T * targetP - p;
			inter = (gM * V4(p, 1)).xyz;

			//hit_normal = normal;
			//if (dot(targetP, hit_normal) > 0)
			//	hit_normal *= -1;
			//assert(0);
		}
	};
	check_t(t0);
	check_t(t1);
	return t;
}

typedef unsigned int uint32;
#define in(a) ((uint32&) a)
bool checkPointInTriangle(const v3& point,
const v3& pa,const v3& pb, const v3& pc)
{
	v3 e10=pb-pa;
	v3 e20=pc-pa;
	float a = dot(e10, e10);
	float b = dot(e10, e20);
	float c = dot(e20, e20);
	float ac_bb=(a*c)-(b*b);
	v3 vp = point - pa;
	float d = dot(vp, e10);
	float e = dot(vp, e20);
	float x = (d*c)-(e*b);
	float y = (e*a)-(d*b);
	float z = x+y-ac_bb;
	return (( in(z)& ~(in(x)|in(y)) ) & 0x80000000);
}
#undef in

float intersect_vertex(v3 p, v3 targetP, v3 normal, float &t)
{
	v3 dir = targetP;
	float a = dot(dir, dir);
	float b = -2*dot(dir, p);
	float c = dot(p, p) - 1;
	float delta = b * b - 4 * a * c;
	if (delta < 0)
		return t;
	delta = sqrtf(delta);
	float t0 = (-b + delta)/(2*a);
	float t1 = (-b - delta)/(2*a);

	if (t0 >= 0 && t0 <= t) {
		t = t0;
		hit_normal = (targetP*t) - p;
		inter = (gM * V4(p, 1)).xyz;
	}
	if (t1 >= 0 && t1 <= t) {
		t = t1;
		hit_normal = (targetP*t) - p;
		inter = (gM * V4(p, 1)).xyz;
	}
	return t;
}

float ellipsoid_intersect_triangle(v3 targetP, v3 ep, v3 ex, v3 ey, v3 ez, v3 t0, v3 t1, v3 t2)
{
	hit_normal = {};

	float t = FLT_MAX;

	mat4 m = translate(ep) * mat4_cols(V4(ex, 0), V4(ey, 0), V4(ez, 0), V4(0, 0, 0, 1));
	mat4 M = inverse(m);

	v3 Mt0 = (M * V4(t0, 1)).xyz;
	v3 Mt1 = (M * V4(t1, 1)).xyz;
	v3 Mt2 = (M * V4(t2, 1)).xyz;
	v3 MtargetP = (M * V4(targetP, 1)).xyz;

	v3 org_normal = normalize(cross(t1 - t0, t2 - t0));

	v3 tn = normalize(cross(Mt1 - Mt0, Mt2 - Mt0));

	gM = m;
	intersect_vertex(Mt0, MtargetP, normalize(cross(org_normal, t2 - t1)), t);
	intersect_vertex(Mt1, MtargetP, normalize(cross(org_normal, t0 - t2)), t);
	intersect_vertex(Mt2, MtargetP, normalize(cross(org_normal, t1 - t0)), t);

	intersect_line(Mt0, Mt1, tn, MtargetP, normalize(cross(tn, Mt0 - Mt1)), t);
	//push_line(g_rc, t0, t1, V3(0));
	//push_line(g_rc, (t0 + t1)*0.5f, (t0 + t1)*0.5f + normalize(cross(org_normal, t0 - t1)), V3(0, 0, 1));

	intersect_line(Mt1, Mt2, tn, MtargetP, normalize(cross(tn, Mt1 - Mt2)), t);
	intersect_line(Mt0, Mt2, tn, MtargetP, normalize(cross(tn, Mt2 - Mt0)), t);
#if 1
	if (fabsf(dot(tn, MtargetP)) > 1e-6) {
		v3 normal = tn;
		//v3 normal = normalize(cross(Mt1 - Mt0, Mt2 - Mt0));
		v3 dir = MtargetP;

		//if (dot(normal, dir) > 0)
		//	normal = -normal;

		if (dot(normal, dir) > 0)
			normal *= -1;

		float T0 = (1 + dot(Mt0, normal)) / (dot(normal, dir));
		float T1 = (-1 + dot(Mt0, normal)) / (dot(normal, dir));

		v3 P0 = (m * V4(T0 * dir, 1)).xyz;
		v3 P1 = (m * V4(T1 * dir + normalize(dir), 1)).xyz;


		//push_line(g_rc, ep, P0, V3(1, 0, 0));
		

		v3 u = Mt1 - Mt0;
		v3 v = Mt2 - Mt0;
		v3 n = cross(u, v);

		auto check_t = [&](float T) {
			if (T >= t || T < 0)
				return ;
			v3 p = T * dir;
			p = Mt0 + (p-Mt0)-dot(p - Mt0, normal)*normal;

			//printf("V: %f\n", fabsf(dot(p - Mt0, normal)));

			//if (fabsf(dot(T * dir + normal - Mt0, normal)) < fabsf(dot(p - Mt0, normal)))
			//	p = T * dir + normal;
			

			//p = p - Mt0;
			float B = -dot(cross(p - Mt0, u), n) / dot(n, n);
			float A = dot(cross(p - Mt0, v), n) / dot(n, n);
			float C = 1 - A - B;
			//printf("%f %f %f %f %f\n", dot(p, normal), T, A, B, C);

			if (A >= 0 && B >= 0 && C >= 0) {
			//if (checkPointInTriangle(p, Mt0, Mt1, Mt2)) {
			//	push_line(g_rc, ep, (m*V4(p, 1)).xyz, V3(1, 1, 0));
		//	push_cube_outline(g_rc, (m*V4(p, 1)).xyz, V3(0.3));
				t = T;
				hit_normal = T * dir - p;
				inter = (gM * V4(p, 1)).xyz;
			}
		};
		check_t(T0);
		check_t(T1);
		//if (t != FLT_MAX)
		//	return t;
	}
#endif

	//if (t != FLT_MAX)
	//	push_line(g_rc, ep, ep + t * (targetP - ep), V3(0));
	//printf("t = %f\n", t);
	//if (t > 1)
	//	t = -1;

	//if (t != FLT_MAX)
	//hit_normal = (transpose(gM) * V4(hit_normal,0)).xyz;
	hit_normal = (transpose(M) * V4(hit_normal,0)).xyz;

	return t;
}

struct Triangle
{
	v3 t0, t1, t2;
};

#if 1
v3 ex = V3(0.6, 0, 0);
v3 ey = V3(0, 0.6, 0);
v3 ez = V3(0, 0, 0.9);
#else
v3 ex = V3(1, 0, 0);
v3 ey = V3(0, 1, 0);
v3 ez = V3(0, 0, 1);
#endif

Array<Triangle> ground;

void move_entity2(Game &game, Entity &e, v3 delta_p)
{
	Arena *temp = begin_temp_memory();

	//ex *= 0.25f, ey *= 0.25f, ez *= 0.25f;
	e.scene_transform = scale(length(ex), length(ey), length(ez));

	Array<Triangle> triangles = make_array_max<Triangle>(temp, game.entities.count * 12 + ground.count + 64);


	for (int i = 0; i < game.entities.count; i++) {
		Entity &test = game.entities[i];

		if (test.id == e.id)
			continue ;
		v3 center = test.position;
		v3 radius = test.collision_box;
		v3 p00 = center + V3(-radius.x, -radius.y, -radius.z);
		v3 p01 = center + V3(+radius.x, -radius.y, -radius.z);
		v3 p02 = center + V3(+radius.x, +radius.y, -radius.z);
		v3 p03 = center + V3(-radius.x, +radius.y, -radius.z);
		v3 p10 = center + V3(-radius.x, -radius.y, +radius.z);
		v3 p11 = center + V3(+radius.x, -radius.y, +radius.z);
		v3 p12 = center + V3(+radius.x, +radius.y, +radius.z);
		v3 p13 = center + V3(-radius.x, +radius.y, +radius.z);

		//bottom
		triangles.push({p00, p02, p01});
		triangles.push({p00, p03, p02});
		//up
		triangles.push({p10, p11, p12});
		triangles.push({p10, p12, p13});

		//left
		triangles.push({p00, p10, p13});
		triangles.push({p00, p13, p03});
		//right
		triangles.push({p01, p12, p11});
		triangles.push({p01, p02, p12});
		
		//front
		triangles.push({p00, p01, p11});
		triangles.push({p00, p11, p10});
		//back
		triangles.push({p03, p12, p02});
		triangles.push({p03, p13, p12});
	}
	triangles.push({V3(0, 2, 0), V3(2, 2, 0), V3(0,4, 2)});
	triangles.push({V3(2, 2, 0), V3(0, 4, 2), V3(2, 4, 2)});
	for (int i = 0; i < ground.count; i++) {
		triangles.push(ground[i]);
	}

	//triangles.push({V3(12, -8, 0), V3(8, -12, 4), V3(-8, -12, 4)});
	//delta_p.z = 0;
	v3 normal = {};
	v3 start_p = e.position;

	for (int itr = 0; itr < 4; itr++) {
		if (length(delta_p) < 1e-6)
			break ;
		bool no_collision = true;
		float t = 1;
		float min_t = 1;
		v3 intersection = {};
		for (int i = 0; i < triangles.count; i++) {
			t_idx = i;
			float tt = ellipsoid_intersect_triangle(e.position + delta_p, e.position, 
				ex, ey, ez, triangles[i].t0, triangles[i].t1, triangles[i].t2);
			//if (tt >= 0 && tt <= 1) {
				push_triangle_outline(g_rc, triangles[i].t0, triangles[i].t1, triangles[i].t2,
				V3(1, 0, 0));
			//	printf("%f\n", tt);
			//	push_cube_outline(g_rc, triangles[i].t0, V3(5), V3(0));
			//}
			if (tt >= 0 && tt != FLT_MAX && tt < 1) {
				if (tt < min_t) {
					min_t = tt;
					t = min(t, tt);
					//printf("%f\n", t);
					normal = hit_normal;
					intersection = inter;
					no_collision = false;
				}
			}
			//break ;
		}
	//	t = max(0.f, t - 0.05f);

		//printf("t = %f\n", t);
		v3 dest = e.position + delta_p;

		if (no_collision) {
			e.position += delta_p;
			break;
		}
		
		float c = 0.025f;
		if (length(delta_p)*t >= c) {
			e.position += normalize(delta_p) * (length(delta_p)*t - c);
		}

		//push_line(g_rc, e.position, e.position + normalize(delta_p), V3(1, 0, 0));

		normal = normalize(normal);

		v3 slide_normal = normalize(e.position - intersection);

		//push_line(g_rc, e.position, e.position + slide_normal, V3(1, 1, 0));

		v3 old_delta_p = delta_p;
		delta_p *= (1 - t);
		delta_p = delta_p - normal*dot(normal,  delta_p)*1.2;

		if (length(delta_p) < c)
			break ;
		if (dot(normalize(delta_p), normalize(old_delta_p)) < -0.9f)
			break ;
		
	}
	{
		e.can_jump = true;
	}
	end_temp_memory();

	//if (length(e.position - start_p) / length(delta_p) < 0.05f)
	//	e.dp = {};

	//push_cube_outline(g_rc, e.position+ ex, V3(0.01), V3(0));
}

void move_entity(Game &game, Entity &e, v3 delta_p)
{
	//if (length(V3(0, 0, delta_p.z)) > 20 * length(V3(delta_p.x, delta_p.y, 0)))
	//	delta_p.xy = {};

	//printf("T: %f %f\n", length(V3(delta_p.x, delta_p.y, 0)), length(V3(0, 0, delta_p.z)));
//	if (game.frame > 60*5)
//		delta_p.x = 0;
	v3 prev = e.position;
	move_entity2(game, e, V3(delta_p.x, delta_p.y, 0));
	move_entity2(game, e, V3(0, 0, delta_p.z));
	printf("%f %f %f\n", e.position.z, prev.z, delta_p.z);

	if (fabsf(e.position.z - prev.z) > 0.3*fabsf(delta_p.z))
		e.on_ground = false;
	else
		e.on_ground = true;
	//e.can_jump = e.on_ground;
	//move_entity2(game, e, V3(delta_p.x, 0, 0));
	//move_entity2(game, e, V3(0, delta_p.y, 0));
	//if (delta_p.z > 0)

	//push_ellipsoid_outline(g_rc, e.position, ex, ey, ez);
}