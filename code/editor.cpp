#define GIZMO_ROTATION_INNER_RADIUS 1.5f
#define GIZMO_ROTATION_OUTER_RADIUS 2
#define GIZMO_ROTATION_RADIUS (0.5f*(GIZMO_ROTATION_INNER_RADIUS + GIZMO_ROTATION_OUTER_RADIUS))

#define GIZMO_MAIN_AXIS_LENGTH (0.07f * 2)
#define GIZMO_OTHER_AXIS_LENGTH (GIZMO_MAIN_AXIS_LENGTH * 0.3f)
#define GIZMO_TRANSLATION_SPHERE_RADIUS (0.2f)

#define GIZMO_SCALE_BOX_RADIUS (GIZMO_MAIN_AXIS_LENGTH * 0.1f)

#define GIZMO_MINIMUM_SCALE 0.01f

#define COLLISION_MESH_SNAP_DIST 0.01f

// TODO: cleanup I don't like passing camera_ray_origin around

entity_id raycast_to_entities(Game &game, World &world, v3 camera_ray_origin, v3 camera_ray_dir,
		float &hit_t, int &mesh_index)
{
	entity_id hit_id = 0;

	v3 hit_triangle[3];

	mesh_index = -1;
	float min_t = FLT_MAX;

	for (usize i = 0; i < world.entities.count; i++) {
		Entity &e = world.entities[i];
		if (!e.scene_id)
			continue ;

		Scene &scene = get_scene_by_id(game, e.scene_id);
		if (scene.state != SCENE_LOADED)
			continue ;
		
		mat4 transform = get_entity_transform(world, e) * e.scene_transform;
		for (usize j = 0; j < scene.meshes.count; j++) {
			Mesh &mesh = scene.meshes[j];
			mat4 mesh_transform = transform * mesh.transform;

			{
				v3 x_axis = normalize((mesh_transform * V4(1, 0, 0, 0)).xyz);
				v3 y_axis = normalize((mesh_transform * V4(0, 1, 0, 0)).xyz);
				v3 z_axis = normalize((mesh_transform * V4(0, 0, 1, 0)).xyz);

				v3 a = (mesh_transform * V4(mesh.box_min, 1)).xyz;
				v3 b = (mesh_transform * V4(mesh.box_max, 1)).xyz;

				v3 c = (a + b) * 0.5f;

				x_axis *= fabsf(dot((b - a)*0.5f, x_axis));
				y_axis *= fabsf(dot((b - a)*0.5f, y_axis));
				z_axis *= fabsf(dot((b - a)*0.5f, z_axis));

				float t = ray_hit_box(camera_ray_origin, camera_ray_dir, c, x_axis, y_axis, z_axis);
				if (t < 0)
					continue ;
			}

			for (usize k = 0; k < mesh.indices.count; k += 3) {
				v3 v0 = (mesh_transform * V4(mesh.vertices[(int)mesh.indices[k+0]], 1)).xyz;
				v3 v1 = (mesh_transform * V4(mesh.vertices[(int)mesh.indices[k+1]], 1)).xyz;
				v3 v2 = (mesh_transform * V4(mesh.vertices[(int)mesh.indices[k+2]], 1)).xyz;

				float t;
				if (ray_hit_triangle(camera_ray_origin, camera_ray_dir, v0, v1, v2, &t) &&
						t < min_t) {
					min_t = t;
					hit_id = e.id;
					hit_triangle[0] = v0;
					hit_triangle[1] = v1;
					hit_triangle[2] = v2;
					mesh_index = (int)j;
				}
			}
		}
	}

	if (hit_id) {
	//	push_triangle_outline(hit_triangle[0], hit_triangle[1], hit_triangle[2], V3(0));
	}

	hit_t = min_t;
	return hit_id;
}

void do_editor_op(Game &game, World &world, Editor &editor, EditorOp &op)
{
	Entity *e = get_entity(world, op.entity);
	if (e) {
		if (op.type == EDITOR_OP_TRANSLATE_ENTITY)
			e->position = op.translate.new_p;
		else if (op.type == EDITOR_OP_ROTATE_ENTITY)
			e->rotation = op.rotate.new_rot;
		else if (op.type == EDITOR_OP_SCALE_ENTITY)
			e->scale = op.scale.new_scale;
		else if (op.type == EDITOR_OP_CREATE_MESH_COLLISION_VERTEX) {
			CollisionMesh &cmesh = world.collision_meshes[world.scene_collision_mesh[e->scene_id]];
			cmesh.vertices.push(op.place_collision_vertex.pos);
		}
		else if (op.type == EDITOR_OP_DELETE_MESH_COLLISION_TRIANGLE) {
			CollisionMesh &cmesh = world.collision_meshes[world.scene_collision_mesh[e->scene_id]];
			for (int i = op.delete_collision_triangle.index + 3; i < cmesh.vertices.count; i++)
				cmesh.vertices[i - 3] = cmesh.vertices[i];
			cmesh.vertices.count -= 3;
		}
	}
	if (op.type == EDITOR_OP_PASTE_ENTITY) {
		Entity *copy_from = get_entity(world, op.paste.copy_from);
		if (copy_from) {
			// NOTE: if op.pase.id is not zero this means we are redoing this operation
			// the first time we will create then entity with any id but the second 
			// we should assign the same previous id
			Entity *copied;

			if (op.paste.id) {
				Entity p = *copy_from;
				p.id = op.paste.id;
				world.entities.push(p);
				copied = &world.entities[world.entities.count - 1];
				world.entities_id_map[p.id] = world.entities.count - 1;
			}
			else {
				Entity *p = make_entity(world);
				entity_id id = p->id;
				*p = *copy_from;
				p->id = id;
				op.paste.id = p->id;
				copied = p;
			}
			copied->position = op.paste.p;
			if (op.paste.mesh_index >= 0) {
				Scene scene = {};
				scene.id = ++game.next_scene_id;
				Scene &s = get_scene_by_id(game, copied->scene_id);
				assert(op.paste.mesh_index < s.meshes.count);
				// TODO: cleanup, @leak
				scene.meshes = make_array<Mesh>(&world.arena, 1);
				scene.meshes[0] = s.meshes[op.paste.mesh_index];
				
				copied->scene_transform = identity();

				scene.meshes[0].transform = scene.meshes[0].default_transform;

				game.scenes.push(scene);
				copied->scene_id = scene.id;
			}
		}
	}
	else if (op.type == EDITOR_OP_DELETE_ENTITY)
		remove_entity(world, op.entity);

	editor.ops.push(op);
}

void redo_editor_op(Game &game, World &world, Editor &editor)
{
	if (!editor.undos.count)
		return ;
	EditorOp op = editor.undos[editor.undos.count - 1];
	editor.undos.count--;

	do_editor_op(game, world, editor, op);
}

void undo_editor_op(Game &game, World &world, Editor &editor)
{
	if (!editor.ops.count)
		return ;
	EditorOp op = editor.ops[editor.ops.count - 1];

	editor.undos.push(op);
	editor.ops.count--;
	Entity *e = get_entity(world, op.entity);
	if (e) {
		if (op.type == EDITOR_OP_TRANSLATE_ENTITY)
			e->position = op.translate.prev_p;
		else if (op.type == EDITOR_OP_ROTATE_ENTITY)
			e->rotation = op.rotate.prev_rot;
		else if (op.type == EDITOR_OP_SCALE_ENTITY)
			e->scale = op.scale.prev_scale;
		else if (op.type == EDITOR_OP_CREATE_MESH_COLLISION_VERTEX) {
			CollisionMesh &cmesh = world.collision_meshes[world.scene_collision_mesh[e->scene_id]];
			assert(cmesh.vertices.count > 0);
			cmesh.vertices.count--;
		}
		else if (op.type == EDITOR_OP_DELETE_MESH_COLLISION_TRIANGLE) {
			CollisionMesh &cmesh = world.collision_meshes[world.scene_collision_mesh[e->scene_id]];

			cmesh.vertices.count += 3;
			for (usize i = cmesh.vertices.count - 1; i >= op.delete_collision_triangle.index + 3; i--)
				cmesh.vertices[i] = cmesh.vertices[i - 3];

			cmesh.vertices[op.delete_collision_triangle.index + 0] = op.delete_collision_triangle.v0;
			cmesh.vertices[op.delete_collision_triangle.index + 1] = op.delete_collision_triangle.v1;
			cmesh.vertices[op.delete_collision_triangle.index + 2] = op.delete_collision_triangle.v2;
		}
	}
	if (op.type == EDITOR_OP_PASTE_ENTITY)
		remove_entity(world, op.paste.id);
	else if (op.type == EDITOR_OP_DELETE_ENTITY) {
		world.entities.push(op.del.entity_data);
		world.entities_id_map[op.del.entity_data.id] = world.entities.count - 1;
	}
}

void editor_render_gizmo(World &world, Editor &editor, Gizmo &gizmo, Camera &camera)
{
	Entity *e = get_entity(world, editor.selected_entity);
	if (!e)
		return ;
	// TODO: CLEANUP this get repeated down
	v3 axis[3] = {V3(1, 0, 0), V3(0, 1, 0), V3(0, 0, 1)};

	if (gizmo.mode == GIZMO_SCALE || gizmo.mode == GIZMO_ROTATION) {
		mat4 transform = get_entity_transform(world, *e);
		for (int i = 0; i < 3; i++)
			axis[i] = normalize(V3(transform.e[0][i], transform.e[1][i], transform.e[2][i]));
	}

	float dist_to_camera = length(e->position - camera.position);
	float axis_length = GIZMO_MAIN_AXIS_LENGTH * dist_to_camera;
	float other_axis_length = GIZMO_OTHER_AXIS_LENGTH * dist_to_camera;
	float scale_box_radius =  GIZMO_SCALE_BOX_RADIUS * dist_to_camera;

	for (int i = 0; i < 3; i++) {
		v3 color = {};
		color.e[i] = 1;
		// @CLEANUP
		if (gizmo.active && (gizmo.dragging_axis == i && !gizmo.uniform_scale))
			color = V3(1, 1, 0);

		if (gizmo.mode == GIZMO_SCALE) {
			push_line(e->position, e->position + axis_length * axis[i], color);
			push_box_outline(e->position + (axis_length - scale_box_radius) * axis[i], 
				axis[0] * scale_box_radius, 
				axis[1] * scale_box_radius, 
				axis[2] * scale_box_radius, color);
		}
		else if (gizmo.mode == GIZMO_TRANSLATION) {
			push_line(e->position, e->position + axis_length * axis[i], color);
			push_ellipsoid_outline(e->position + axis_length * axis[i], V3(axis_length * GIZMO_TRANSLATION_SPHERE_RADIUS), color);

			//push_box_outline(e->position + 0.5f * axis_length * axis[i], 
			//		axis[0] * 0.5f * (i == 0 ? axis_length*(1+2*GIZMO_TRANSLATION_SPHERE_RADIUS) : other_axis_length), 
			//		axis[1] * 0.5f * (i == 1 ? axis_length*(1+2*GIZMO_TRANSLATION_SPHERE_RADIUS) : other_axis_length), 
			//		axis[2] * 0.5f * (i == 2 ? axis_length*(1+2*GIZMO_TRANSLATION_SPHERE_RADIUS) : other_axis_length), color);
		}
		else if (gizmo.mode == GIZMO_ROTATION) {
					push_circle(e->position, 
							GIZMO_ROTATION_RADIUS,
							axis[(i + 1) % 3], axis[(i + 2) % 3], color);
		}
	}
	if (gizmo.mode == GIZMO_SCALE) {
		v3 color = gizmo.active && gizmo.uniform_scale ? V3(1, 1, 0) : V3(1);
		push_box_outline(e->position,
					axis[0] * scale_box_radius, 
					axis[1] * scale_box_radius, 
					axis[2] * scale_box_radius, color);
	}
}

void editor_update_gizmo(World &world, Editor &editor, Gizmo &gizmo, Camera &camera,
		v3 camera_ray_origin, v3 camera_ray_dir)
{
	Entity *e = get_entity(world, editor.selected_entity);
	if (!e) {
		gizmo.active = false;
		return ;
	}
	v3 axis[3] = {V3(1, 0, 0), V3(0, 1, 0), V3(0, 0, 1)};

	if (gizmo.mode == GIZMO_SCALE || gizmo.mode == GIZMO_ROTATION) {
		mat4 transform = get_entity_transform(world, *e);
		for (int i = 0; i < 3; i++)
			axis[i] = normalize(V3(transform.e[0][i], transform.e[1][i], transform.e[2][i]));
	}

	float dist_to_camera = length(e->position - camera.position);
	float axis_length = GIZMO_MAIN_AXIS_LENGTH * dist_to_camera;
	float other_axis_length = GIZMO_OTHER_AXIS_LENGTH * dist_to_camera;
	float scale_box_radius = GIZMO_SCALE_BOX_RADIUS * dist_to_camera;

	if (!gizmo.active) {
		float min_axis_t = FLT_MAX;
		int best_axis = -1;

		for (int i = 0; i < 3; i++) {
			float t = -1;
			if (gizmo.mode == GIZMO_ROTATION) {
				if (ray_hit_plane(camera_ray_origin, camera_ray_dir, axis[i], e->position, &t)) {
					v3 p = camera_ray_origin + t * camera_ray_dir;
					if (t < 0 
						|| length(p - e->position) < GIZMO_ROTATION_INNER_RADIUS
						|| length(p - e->position) > GIZMO_ROTATION_OUTER_RADIUS)
						continue ;
				}
			}
			else if (gizmo.mode == GIZMO_TRANSLATION) {
				t = ray_hit_box(camera_ray_origin, camera_ray_dir, e->position + 0.5f * axis_length * axis[i], 
						axis[0] * 0.5f * (i == 0 ? axis_length * (1+2*GIZMO_TRANSLATION_SPHERE_RADIUS) : other_axis_length), 
						axis[1] * 0.5f * (i == 1 ? axis_length * (1+2*GIZMO_TRANSLATION_SPHERE_RADIUS) : other_axis_length), 
						axis[2] * 0.5f * (i == 2 ? axis_length * (1+2*GIZMO_TRANSLATION_SPHERE_RADIUS) : other_axis_length));
			}
			else if (gizmo.mode == GIZMO_SCALE) {
				t = ray_hit_box(camera_ray_origin, camera_ray_dir, e->position + 
						(axis_length - scale_box_radius) 
						* axis[i],
						axis[0] * scale_box_radius, 
						axis[1] * scale_box_radius, 
						axis[2] * scale_box_radius);
			}
			if (t >= 0 && t < min_axis_t) {
				min_axis_t = t;
				best_axis = i;
			}
		}
		gizmo.uniform_scale = false;
		if (gizmo.mode == GIZMO_SCALE) {
			float t = ray_hit_box(camera_ray_origin, camera_ray_dir, e->position,
						axis[0] * scale_box_radius, 
						axis[1] * scale_box_radius, 
						axis[2] * scale_box_radius);
			if (t >= 0 && t < min_axis_t) {
				min_axis_t = t;
				gizmo.uniform_scale = true;
			}
		}
		if (min_axis_t != FLT_MAX) {
			gizmo.init_position = e->position;
			gizmo.init_rotation = e->rotation;
			gizmo.init_scale = e->scale;
			gizmo.active = true;
			gizmo.did_drag = false;
			gizmo.dragging_axis = best_axis;
		}
	}

	if (gizmo.active) {
		if (gizmo.did_drag && !v3_equal(camera.position, editor.last_camera_p))
			gizmo.did_drag = false;
		// TODO: maybe choose the best axis?
		if (gizmo.mode == GIZMO_SCALE && gizmo.uniform_scale)
			gizmo.dragging_axis = 2;

		v3 plane_normal;
		//plane_normal = cross(axis[gizmo.dragging_axis], cross(axis[gizmo.dragging_axis], camera.forward));
		//plane_normal = camera.forward;
		
		{
			// find the vector in the plane of camera forward and axis
			// that try to face camera forward and be otrhogonal to the axis
			// in other words it minimize: |dot(X, forward)| - |dot(X, axis)|
			// the vector has to be in the plane since any vector that's not can be decomposed into 2 vectors
			// one in the plane and the plane normal, and the plane normal will have dot product = 0 with both axis
			plane_normal = normalize(camera.forward - dot(axis[gizmo.dragging_axis], camera.forward) * axis[gizmo.dragging_axis] );
		}
		if (gizmo.mode == GIZMO_ROTATION)
			plane_normal = axis[gizmo.dragging_axis];

		//push_line(e->position, e->position + normalize(plane_normal) * axis_length, V3(1, 0, 1));

		float hit_t;
		if (ray_hit_plane(camera_ray_origin, camera_ray_dir, plane_normal, e->position, &hit_t)) {
			v3 hit_p = camera_ray_origin + hit_t * camera_ray_dir;

			if (!gizmo.did_drag)
				gizmo.hit_p = hit_p;

			if (gizmo.mode == GIZMO_TRANSLATION) {
				v3 delta_p = axis[gizmo.dragging_axis] * dot(hit_p - e->position, axis[gizmo.dragging_axis]);

				if (!gizmo.did_drag)
					gizmo.delta_p = delta_p;
				else 
					e->position += delta_p - gizmo.delta_p;
			}
			else if (gizmo.mode == GIZMO_SCALE) {
				float delta_s = dot(hit_p - e->position, axis[gizmo.dragging_axis]);

				if (!gizmo.did_drag) {
					gizmo.scale = e->scale;
					gizmo.delta_s = delta_s;
				}
				else {
					v3 new_scale = e->scale;
					if (gizmo.uniform_scale) {
						v3 v = gizmo.hit_p - e->position;
						float l = dot(hit_p - e->position, normalize(v));
						float s = (l >= length(v) ? 1 + (l - length(v)) : 1 / (length(v) - l + 1));
						new_scale = s * gizmo.scale;
					}
					else
						new_scale.e[gizmo.dragging_axis] = gizmo.scale.e[gizmo.dragging_axis] + delta_s - gizmo.delta_s;
					for (int i = 0; i < 3; i++)
						if (new_scale.e[gizmo.dragging_axis] < GIZMO_MINIMUM_SCALE)
							new_scale.e[gizmo.dragging_axis] = GIZMO_MINIMUM_SCALE;
					e->scale = new_scale;
				}
			}
			else if (gizmo.mode == GIZMO_ROTATION) {
				v3 p = normalize(hit_p - e->position) * GIZMO_ROTATION_RADIUS;

				push_cube_outline(e->position + p, V3(0.1), V3(0));

				v3 right_axis = axis[(gizmo.dragging_axis+1)%3];
				v3 up_axis = axis[(gizmo.dragging_axis+2)%3];

				if (gizmo.did_drag)
					right_axis = gizmo.rotation_right_axis, up_axis = gizmo.rotation_up_axis;
				else
					gizmo.rotation_axis = axis[gizmo.dragging_axis];

				p.xy = V2(dot(p, right_axis), dot(p, up_axis));

				float a = atan2(p.y, p.x);

				if (!gizmo.did_drag) {
					gizmo.did_drag = true;
					gizmo.rotation_angle = a;
					gizmo.rotation = e->rotation;
					gizmo.rotation_right_axis = right_axis;
					gizmo.rotation_up_axis = up_axis;
					gizmo.rotation_axis = axis[gizmo.dragging_axis];
				}
				else {
					e->rotation = rotate_around_axis_quat(gizmo.rotation_axis, a - gizmo.rotation_angle)
						* gizmo.rotation;
				}
			}

			if (!gizmo.did_drag)
				gizmo.did_drag = true;
		}
	}
}

void editor_push_entity_op(Game &game, World &world, Editor &editor)
{
	Entity *e = get_entity(world, editor.selected_entity);
	if (!editor.gizmo.active || !e)
		return ;

	EditorOp op = {};
	op.entity = editor.selected_entity;
	bool push = true;
	if (editor.gizmo.mode == GIZMO_TRANSLATION) {
		op.type = EDITOR_OP_TRANSLATE_ENTITY;
		op.translate.prev_p = editor.gizmo.init_position;
		op.translate.new_p = e->position;
		push &= (!v3_equal(op.translate.prev_p, op.translate.new_p));
	}
	else if (editor.gizmo.mode == GIZMO_SCALE) {
		op.type = EDITOR_OP_SCALE_ENTITY;
		op.scale.prev_scale = editor.gizmo.init_scale;
		op.scale.new_scale = e->scale;
		push &= (!v3_equal(op.scale.prev_scale, op.scale.new_scale));
	}
	else if (editor.gizmo.mode == GIZMO_ROTATION) {
		op.type = EDITOR_OP_ROTATE_ENTITY;
		op.rotate.prev_rot = editor.gizmo.init_rotation;
		op.rotate.new_rot = e->rotation;
		push &= (!quat_equal(op.rotate.prev_rot, op.rotate.new_rot));
	}
	if (push)
		do_editor_op(game, world, editor, op);
	editor.gizmo.active = false;
}

void editor_edit_collision_mesh(Game &game, GameInput &input, World &world, Editor &editor, Camera &camera, v3 camera_ray_origin,
		v3 camera_ray_dir)
{
	float min_hit_t;
	int hit_entity_mesh_index;
	entity_id hit_entity = raycast_to_entities(game, world, camera_ray_origin, camera_ray_dir, 
			min_hit_t, hit_entity_mesh_index);

	Entity *e = get_entity(world, hit_entity);

	if (!e || !e->scene_id)
		return ;

	mat4 to_world = get_entity_transform(world, *e) * e->scene_transform;
	mat4 to_object = inverse(to_world);

	if (!world.scene_collision_mesh.count(e->scene_id)) {
		CollisionMesh cmesh = {};
		// TODO: cleanup
		cmesh.vertices = make_array_max<v3>(&world.arena, 3 * 1000);
		cmesh.scene = e->scene_id;
		world.collision_meshes.push(cmesh);
		world.scene_collision_mesh[e->scene_id] = (int)(world.collision_meshes.count - 1);
	}

	CollisionMesh &cmesh = world.collision_meshes[world.scene_collision_mesh[e->scene_id]];

	v3 hit_p = camera_ray_origin + min_hit_t * camera_ray_dir;

	float dist_to_snap = COLLISION_MESH_SNAP_DIST * length(hit_p - camera.position);

	if (IsDownFirstTime(input, BUTTON_MOUSE_RIGHT)) {
		hit_p = (to_object * V4(hit_p, 1)).xyz;

		float closest_dist = FLT_MAX;
		int best_p = 0;
		for (int i = 0; i < cmesh.vertices.count; i++) {
			float dist = length_sq(cmesh.vertices[i] - hit_p);
			if (dist < closest_dist) {
				closest_dist = dist;
				best_p = i;
			}
		}
		if (closest_dist < dist_to_snap * dist_to_snap)
			hit_p = cmesh.vertices[best_p];
		//cmesh.vertices.push(hit_p);
		EditorOp op = {};
		op.type = EDITOR_OP_CREATE_MESH_COLLISION_VERTEX;
		op.entity = hit_entity;
		op.place_collision_vertex.pos = hit_p;

		do_editor_op(game, world, editor, op);

		hit_p = (to_world * V4(hit_p, 1)).xyz;
	}

	int hit_triangle = -1;
	{
		float min_t = FLT_MAX;
		for (int i = 0; i + 2 < cmesh.vertices.count; i += 3) {
			v3 v0 = (to_world * V4(cmesh.vertices[i + 0], 1)).xyz;
			v3 v1 = (to_world * V4(cmesh.vertices[i + 1], 1)).xyz;
			v3 v2 = (to_world * V4(cmesh.vertices[i + 2], 1)).xyz;

			float hit_t;
			if (ray_hit_triangle(camera_ray_origin, camera_ray_dir, v0, v1, v2, &hit_t)
					&& hit_t < min_t) {
				min_t = hit_t;
				hit_triangle = i;
			}
		}
	}

	if (IsDownFirstTime(input, BUTTON_DELETE)) {
		if (hit_triangle != -1) {
			EditorOp op = {};
			op.type = EDITOR_OP_DELETE_MESH_COLLISION_TRIANGLE;
			op.entity = hit_entity;
			op.delete_collision_triangle.index = hit_triangle;
			op.delete_collision_triangle.v0 = cmesh.vertices[hit_triangle + 0]; 
			op.delete_collision_triangle.v1 = cmesh.vertices[hit_triangle + 1]; 
			op.delete_collision_triangle.v2 = cmesh.vertices[hit_triangle + 2]; 
			do_editor_op(game, world, editor, op);
		}
	}

	push_cube_outline(hit_p, V3(dist_to_snap), V3(1, 1, 0));
	for (usize i = (cmesh.vertices.count/3)*3; i < cmesh.vertices.count; i++)
		push_cube_outline((to_world * V4(cmesh.vertices[i], 1)).xyz, V3(dist_to_snap), V3(1, 0, 0));

	for (usize i = 0; i + 2 < cmesh.vertices.count; i += 3) {
		v3 color = i == hit_triangle ? V3(1, 1, 0) : V3(1, 0, 0);
		push_triangle_outline((to_world * V4(cmesh.vertices[i + 0], 1)).xyz,
					(to_world * V4(cmesh.vertices[i + 1], 1)).xyz,
					(to_world * V4(cmesh.vertices[i + 2], 1)).xyz, color);
	}
}

void update_editor(Game &game, World &world, Editor &editor, GameInput &input, Camera &camera)
{
	{
		Entity *e = get_entity(world, editor.selected_entity);

		if (e) {
			ImGui::Begin("Entity");

			if (ImGui::Button("Reset scale"))
				e->scale = V3(1);
			if (ImGui::Button("Reset rotation"))
				e->rotation = identity_quat();
			if (e->id != world.player_id && ImGui::Button("delete")) {
				EditorOp op = {};
				op.entity = e->id;
				op.type = EDITOR_OP_DELETE_ENTITY;
				op.del.entity_data = *e;
				do_editor_op(game, world, editor, op);
				e = 0;
			}
			if (e)
				imgui_edit_struct_Entity(*e, "selected entity", false);

			ImGui::End();
		}
	}

	if (!game.in_editor)
		return ;

	v3 camera_ray_origin, camera_ray_dir;

	{
		v2 mouse_p = (input.mouse_p * V2(1.f / platform.render_context
->window_width, 1.f / platform.render_context
->window_height)) * 2 - V2(1);
		mouse_p.y *= -1;
		camera_ray_origin = camera.position;
		camera_ray_dir = camera.forward * camera.znear 
			+ mouse_p.x * camera.right * camera.width * 0.5f
			+ mouse_p.y * camera.up * camera.height * 0.5f;
	}

	if (IsDown(input, BUTTON_LEFT_CONTROL) && IsDownFirstTime(input, BUTTON_Q))
		editor.mode = (editor.mode == EDITOR_MODE_COLLISION_MESH ? EDITOR_MODE_GIZMO : EDITOR_MODE_COLLISION_MESH);

	switch (editor.mode) {
		case EDITOR_MODE_GIZMO: {
			if (IsDown(input, BUTTON_LEFT_CONTROL)) {
				if (IsDown(input, BUTTON_T)) editor.gizmo.mode = GIZMO_TRANSLATION;
				else if (IsDown(input, BUTTON_R)) editor.gizmo.mode = GIZMO_ROTATION;
				else if (IsDown(input, BUTTON_S)) editor.gizmo.mode = GIZMO_SCALE;
			}

			if (IsDown(input, BUTTON_MOUSE_RIGHT)) {
				editor_update_gizmo(world, editor, editor.gizmo, camera, camera_ray_origin, camera_ray_dir);

				if (!editor.gizmo.active) {
					// @CLEANUP: frame late
					float min_hit_t;
					int hit_entity_mesh_index;
					entity_id hit_entity = raycast_to_entities(game, world, camera_ray_origin, camera_ray_dir, min_hit_t, hit_entity_mesh_index);
					editor.selected_entity = hit_entity;
					editor.selected_entity_mesh = hit_entity_mesh_index;
				}
			}
			else
				editor_push_entity_op(game, world, editor);
			editor_render_gizmo(world, editor, editor.gizmo, camera);
			break ;
		}
		case EDITOR_MODE_COLLISION_MESH: {
			editor_edit_collision_mesh(game, input, world, editor, camera, camera_ray_origin, camera_ray_dir);
			break ;
		}
	}

	if (IsDown(input, BUTTON_LEFT_CONTROL) && IsDownFirstTime(input, BUTTON_C) && editor.selected_entity) {
		editor.copied_entity = editor.selected_entity;
		editor.copy_entity_mesh = IsDown(input, BUTTON_LEFT_SHIFT);
	}

	if (IsDown(input, BUTTON_LEFT_CONTROL) && IsDownFirstTime(input, BUTTON_V)) {
		Entity *copy_from = get_entity(world, editor.copied_entity);
		if (copy_from) {
			EditorOp op = {};
			op.type = EDITOR_OP_PASTE_ENTITY;
			op.paste.copy_from = copy_from->id;
			op.paste.mesh_index = (editor.copy_entity_mesh ? editor.selected_entity_mesh : -1);

			float min_hit_t;
			int hit_mesh_index;
			entity_id hit_entity = raycast_to_entities(game, world, camera_ray_origin, 
					camera_ray_dir, min_hit_t, hit_mesh_index);
			if (hit_entity)
				op.paste.p = camera_ray_origin + min_hit_t * camera_ray_dir + V3(0, 0, copy_from->scale.z);
			else 
				op.paste.p = camera.position + camera.forward * 2 * copy_from->scale;
			
			do_editor_op(game, world, editor, op);
		}
	}

	if (IsDown(input, BUTTON_LEFT_CONTROL) && IsDownFirstTime(input, BUTTON_Z))
		undo_editor_op(game, world, editor);
	if (IsDown(input, BUTTON_LEFT_CONTROL) && IsDownFirstTime(input, BUTTON_X))
		redo_editor_op(game, world, editor);

	editor.last_camera_p = camera.position;
}
