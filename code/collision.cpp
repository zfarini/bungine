CollisionShape make_ellipsoid_shape(v3 radius)
{
	CollisionShape shape = {};

	shape.type = COLLISION_SHAPE_ELLIPSOID;
	shape.ellipsoid_radius = radius;
	return shape;
}

CollisionShape make_triangles_shape(Array<CollisionTriangle> triangles)
{
	CollisionShape shape = {};

	shape.type = COLLISION_SHAPE_TRIANGLES;
	shape.triangles = triangles;
	return shape;
}

CollisionShape make_box_shape(Arena *arena, v3 radius)
{
	Array<CollisionTriangle> triangles = make_array_max<CollisionTriangle>(arena, 12);

	v3 p00 = V3(-radius.x, -radius.y, -radius.z);
	v3 p01 = V3(+radius.x, -radius.y, -radius.z);
	v3 p02 = V3(+radius.x, +radius.y, -radius.z);
	v3 p03 = V3(-radius.x, +radius.y, -radius.z);
	v3 p10 = V3(-radius.x, -radius.y, +radius.z);
	v3 p11 = V3(+radius.x, -radius.y, +radius.z);
	v3 p12 = V3(+radius.x, +radius.y, +radius.z);
	v3 p13 = V3(-radius.x, +radius.y, +radius.z);

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

	CollisionShape shape = {};

	shape.type = COLLISION_SHAPE_TRIANGLES;
	shape.triangles = triangles;
	shape.box_radius = radius;
	return shape;
}

struct CollisionInfo
{
	v3 hit_p;
	v3 hit_normal;
	float t;
	entity_id entity;
};

#define SMALLEST_VELOCITY 0.01f
#define SLIDE_ITERATION_COUNT 4
#define SLIDE_COEFF 1.2f

void intersect_line(v3 A, v3 B, v3 dir, CollisionInfo &info)
{
	v3 D = dir;
	v3 d = normalize(B - A);
	v3 V = (A - dot(A, d) * d);
	v3 X = D - d * dot(D, d);

	float a = dot(X, X);
	float b = -2*dot(X, V);
	float c = dot(V, V) - 1;
	float delta = b * b - 4 * a * c;
	if (delta < 0 || fabsf(a) < 1e-6)
		return ;
	delta = sqrtf(delta);

	auto check_t = [&](float t) {
		if (t < 0 || t >= info.t)
			return ;
		// get closest point in line (A, B) from point t * dir
		v3 h = t * dir;
		v3 p = h - ((h - A) - d * dot(h - A, d));
		if (dot(p - A, B - A) >= 0 && length_sq(p - A) <= length_sq(B - A)) {
			info.t = t;
			info.hit_p = p;
		}
	};
	check_t((-b + delta)/(2*a));
	check_t((-b - delta)/(2*a));
}

void intersect_vertex(v3 p, v3 dir, CollisionInfo &info)
{
	float a = dot(dir, dir);
	float b = -2*dot(dir, p);
	float c = dot(p, p) - 1;
	float delta = b * b - 4 * a * c;
	if (delta < 0)
		return ;
	delta = sqrtf(delta);
	float t0 = (-b - delta)/(2*a);
	float t1 = (-b + delta)/(2*a);

	if (t1 >= 0 && t1 < t0)
		t0 = t1;

	if (t0 >= 0 && t0 <= info.t) {
		info.t = t0;
		info.hit_p = p;
	}
}

void intersect_triangle_plane(v3 v0, v3 v1, v3 v2, v3 dir, CollisionInfo &info)
{
	v3 u = v1 - v0;
	v3 v = v2 - v0;
	v3 n = cross(u, v);

	v3 normal = normalize(n);

	if (fabsf(dot(normal, dir)) < 1e-6)
		return ;

	if (dot(normal, dir) > 0)
		normal *= -1;

	float t0 = (1 + dot(v0, normal)) / (dot(normal, dir));
	float t1 = (-1 + dot(v0, normal)) / (dot(normal, dir));

	float one_over_length_n_sq = 1.f / dot(n, n);

	auto check_t = [&](float t) {
		if (t >= info.t || t < 0)
			return ;
		// find closest point to plane
		v3 p = t * dir;
		p = v0 + (p-v0)-dot(p - v0, normal)*normal;

		/*
		   P = A * u + B * v

		   cross(P, u) = B * v * u = - B * n
		   dot(cross(P, u)) = -B * dot(n, n)
		   B = -dot(cross(P, u))/dot(n, n)
		   */
		float A = dot(cross(p - v0, v), n) * one_over_length_n_sq;
		float B = -dot(cross(p - v0, u), n) * one_over_length_n_sq;

		if (A >= 0 && B >= 0 && A + B <= 1) {
			info.t = t;
			info.hit_p = p;
		}
	};
	check_t(t0);
	check_t(t1);
}

CollisionInfo ellipsoid_intersect_triangle(v3 targetP, v3 ep, v3 er, v3 v0, v3 v1, v3 v2)
{
	CollisionInfo info = {};

	info.t = FLT_MAX;

	v3 inv_r = V3(1/er.x, 1/er.y, 1/er.z);

	//mat4 m = translate(ep) * scale(er);
	//mat4 M = scale(inv_r) * translate(-ep);

#if 0
	v3 t0 = (M * V4(v0, 1)).xyz;
	v3 t1 = (M * V4(v1, 1)).xyz;
	v3 t2 = (M * V4(v2, 1)).xyz;
	v3 dir = (M * V4(targetP, 1)).xyz;
#else
	v3 t0 = inv_r * (v0 - ep);
	v3 t1 = inv_r * (v1 - ep);
	v3 t2 = inv_r * (v2 - ep);
	v3 dir = inv_r * (targetP - ep);
#endif
	intersect_triangle_plane(t0, t1, t2, dir, info);

	intersect_vertex(t0, dir, info);
	intersect_vertex(t1, dir, info);
	intersect_vertex(t2, dir, info);

	intersect_line(t0, t1, dir, info);
	intersect_line(t1, t2, dir, info);
	intersect_line(t0, t2, dir, info);

	// NOTE: here we multiply by inverse transpose of m to get the normal transformed
	// we can't just take the difference of the transformed hit point and new center because then we will be back to the ellipsoid space
	// and the normal there is no longer just the difference like in a sphere
	info.hit_normal = inv_r * (info.t * dir - info.hit_p);//(transpose(M) * V4(info.t * dir - info.hit_p ,0)).xyz;
	info.hit_p = ep + (info.hit_p * er);

	return info;
}

CollisionInfo ellipsoid_intersect_ellipsoid(v3 targetP, v3 ep, v3 er, v3 tp, v3 tr)
{
	CollisionInfo info = {};

	info.t = FLT_MAX;

	/*
	   do a Minkowski sum on the 2 ellipsoid resulting in one with the combined radius
	   now the problem is a ray against a ellipsoid
	   bring the ray in the space where the ellipsoid is a sphere with radius 1 and origin at (0,0,0)
	   then just solve the quadratic equation where length(ray_origin + t * ray_dir) = 1
	   */
	v3 radius = er + tr;

	mat4 m = translate(tp) * scale(radius);
	mat4 M = scale(1 / radius) * translate(-tp); // world to ellipsoid space

	v3 ray_origin = (M * V4(ep, 1)).xyz;
	v3 ray_dir = (M * V4(targetP, 1)).xyz - ray_origin;

	float a = dot(ray_dir, ray_dir);
	float b = 2 * dot(ray_origin, ray_dir);
	float c = dot(ray_origin, ray_origin) - 1;

	float delta = b * b - 4 * a * c;

	if (delta < 0)
		return info;

	delta = sqrtf(delta);
	float t0 = (-b - delta) / (2 * a);
	float t1 = (-b + delta) / (2 * a);

	if (t1 >= 0 && t1 < t0)
		t0 = t1;

	if (t0 >= 0) {
		info.t = t0;
		// too lazy to think about this
		//info.hit_p = ;
		info.hit_normal = (transpose(M) * V4(ray_origin + t0*ray_dir, 0)).xyz;
	}
	return info;
}

CollisionInfo move_entity(World &world, Entity &e, v3 delta_p, Array<CollisionShape> shapes)
{
	CollisionInfo first_hit = {};

	v3 e_radius = e.scale * e.shape.ellipsoid_radius;

	for (int itr = 0; itr < SLIDE_ITERATION_COUNT; itr++) {
		if (length_sq(delta_p) < SMALLEST_VELOCITY*SMALLEST_VELOCITY)
			break ;

		CollisionInfo hit_info = {};
		hit_info.t = FLT_MAX;
		entity_id hit_entity = 0;

		for (int i = 0; i < shapes.count; i++) {
			CollisionInfo info;

			if (shapes[i].type == COLLISION_SHAPE_TRIANGLES) {
				for (int j = 0; j < shapes[i].triangles.count; j++) {
					v3 p0 = (shapes[i].transform * V4(shapes[i].triangles[j].v0, 1)).xyz;
					v3 p1 = (shapes[i].transform * V4(shapes[i].triangles[j].v1, 1)).xyz;
					v3 p2 = (shapes[i].transform * V4(shapes[i].triangles[j].v2, 1)).xyz;

					info = ellipsoid_intersect_triangle(e.position + delta_p, e.position, e_radius,
							p0, p1, p2);
					info.entity = shapes[i].entity;
					if (info.t < hit_info.t)
						hit_info = info;
				}
			}
			else if (shapes[i].type == COLLISION_SHAPE_ELLIPSOID) {
				info = ellipsoid_intersect_ellipsoid(e.position + delta_p, e.position, e_radius,
						V3(shapes[i].transform.e[0][3],
							shapes[i].transform.e[1][3],
							shapes[i].transform.e[2][3]), 
						shapes[i].ellipsoid_radius
						* shapes[i].scale);
				info.entity = shapes[i].entity;
				if (info.t < hit_info.t)
					hit_info = info;
			}
			else
				assert(0);
		}

		if (!itr)
			first_hit = hit_info;

		// if we can move the full distance we have to check if we will be at least SMALLEST_VELOCITY away from the closest object
		if (hit_info.t >= 1) {
			if (hit_info.t != FLT_MAX && length_sq(delta_p * (hit_info.t - 1)) < SMALLEST_VELOCITY * SMALLEST_VELOCITY)
				e.position += normalize(delta_p) * (length(delta_p) - (SMALLEST_VELOCITY - length(delta_p * (hit_info.t - 1))));
			else
				e.position += delta_p;
			break ;
		}

		if (length_sq(delta_p*hit_info.t) >= SMALLEST_VELOCITY*SMALLEST_VELOCITY)
			e.position += normalize(delta_p) * (length(delta_p)*hit_info.t - SMALLEST_VELOCITY);

		v3 normal = normalize(hit_info.hit_normal);

		v3 old_delta_p = delta_p;
		delta_p *= (1 - hit_info.t);
		delta_p = delta_p - normal*dot(normal, delta_p)*SLIDE_COEFF;
	}

	return first_hit;
}

void move_entity(World &world, Entity &e, v3 delta_p)
{
	assert(e.shape.type == COLLISION_SHAPE_ELLIPSOID);

	Arena *temp = begin_temp_memory();

	Array<CollisionShape> shapes = make_array_max<CollisionShape>(temp, world.entities.count + 64);

	for (int i = 0; i < world.entities.count; i++) {
		Entity &test = world.entities[i];

		if (test.id == e.id)
			continue ;

		CollisionShape shape = test.shape;
		shape.transform = get_entity_transform(world, test);

		if (shape.type == COLLISION_SHAPE_ELLIPSOID)
			shape.scale = test.scale;

		shape.entity = test.id;
		shapes.push(shape);
	}
	// {
	// 	CollisionShape shape = {};
	// 	shape.type = COLLISION_SHAPE_ELLIPSOID;
	// 	shape.ellipsoid_radius = e.shape.ellipsoid_radius;
	// 	shape.offset = V3(0, 3, 1);
	// 	shapes.push(shape);
	// }

	//v3 old_p = e.position;
	v3 rel_p = e.position;

	e.position = get_world_p(world, e.id);
	v3 start_p = e.position;

	move_entity(world, e, V3(delta_p.x, delta_p.y, 0), shapes);

	int itr = 1 + roundf(fabsf(delta_p.z) / (SMALLEST_VELOCITY*3.5f));
	for (int i = 0; i < itr; i++)
		move_entity(world, e, V3(0, 0, delta_p.z / itr), shapes);

	if (fabsf(e.position.x - start_p.x) < 1e-7)
		e.dp.x = 0;
	if (fabsf(e.position.y - start_p.y) < 1e-7)
		e.dp.y = 0;
	if (fabsf(e.position.z - start_p.z) < 1e-7)
		e.dp.z = 0;


	v3 save_p = e.position;
	// try to move down to find the ground of the entity
	CollisionInfo collision = move_entity(world, e, V3(0, 0, -1), shapes);
	e.position = save_p;

	// TODO: this could be negative (imagine a sloped wall and I'm sliding its not necesarry that the bottom
	//	of the ellipsoid will hit first)
	float height_above_ground = e.position.z - collision.hit_p.z - e.scale.z*e.shape.ellipsoid_radius.z
		- SMALLEST_VELOCITY;
	if (collision.t < 0 || collision.t == FLT_MAX)
		height_above_ground = 10000;
	if (height_above_ground < 0)
		height_above_ground = 0;
	e.height_above_ground = height_above_ground;

	if (e.height_above_ground > 0.6)
		e.can_jump = false;
	e.on_ground = e.height_above_ground < 0.01f;
	if (e.on_ground)
		e.can_jump = true;

	if (e.type == EntityType_Static)
		e.position = rel_p + e.position - start_p;
	else {
		// Entity *parent = get_entity(world, collision.entity);
		// if (parent) {
		// 	e.parent = parent->id;
		// 	e.position -= parent->position;
		// }
		// else
		// 	e.parent = 0;
	}
	end_temp_memory();
}
