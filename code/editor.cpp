entity_id raycast_to_entities(World &world, v3 ray_origin, v3 ray_dir,
		float &hit_t)
{
	entity_id hit_id = 0;
	float min_t = FLT_MAX;

	for (usize i = 0; i < world.entities.count; i++) {
		Entity &e = world.entities[i];
		if (!e.scene)
			continue ;

		mat4 transform = get_entity_transform(e) * e.scene_transform;
		for (usize j = 0; j < e.scene->meshes.count; j++) {
			Mesh &mesh = e.scene->meshes[j];
			mat4 mesh_transform = transform * mesh.transform;

			v3 x_axis = normalize((mesh_transform * V4(1, 0, 0, 0)).xyz);
			v3 y_axis = normalize((mesh_transform * V4(0, 1, 0, 0)).xyz);
			v3 z_axis = normalize((mesh_transform * V4(0, 0, 1, 0)).xyz);

			v3 a = (mesh_transform * V4(mesh.box_min, 1)).xyz;
			v3 b = (mesh_transform * V4(mesh.box_max, 1)).xyz;

			v3 c = (a + b) * 0.5f;

			x_axis *= fabsf(dot((b - a)*0.5f, x_axis));
			y_axis *= fabsf(dot((b - a)*0.5f, y_axis));
			z_axis *= fabsf(dot((b - a)*0.5f, z_axis));


			//push_box_outline(c, x_axis, y_axis, z_axis);
			float t = ray_hit_box(ray_origin, ray_dir, c, x_axis, y_axis, z_axis);
			if (t >= 0 && t < min_t) {
				hit_id = e.id;
				min_t = t;
			}

			//ray_hit_box();
		}
	}
	hit_t = min_t;
	return hit_id;
}

void update_editor(World &world, Editor &editor, GameInput &input, Camera &camera)
{
	v2 mouse_p = (input.mouse_p * V2(1.f / g_rc->window_width, 1.f / g_rc->window_height)) * 2 - V2(1);
	mouse_p.y *= -1;

	v3 ray_origin = camera.position;
	v3 ray_dir = camera.forward * camera.znear 
		+ mouse_p.x * camera.right * camera.width * 0.5f
		+ mouse_p.y * camera.up * camera.height * 0.5f;

	if (IsDown(input, BUTTON_MOUSE_RIGHT)) {
		float min_hit_t;
		entity_id hit_entity = raycast_to_entities(world, ray_origin, ray_dir, min_hit_t);

		v3 x_axis = 2*V3(1, 0, 0);
		v3 y_axis = 2*V3(0, 1, 0);
		v3 z_axis = 2*V3(0, 0, 1);

		if (editor.selected_entity && !editor.in_gizmo) {
			Entity *e = get_entity(world, editor.selected_entity);
			if (e) {
				float min_axis_t = FLT_MAX;
				int best_axis = -1;
				for (int i = 0; i < 3; i++) {
					v3 axis = i == 0 ? x_axis : (i == 1 ? y_axis : z_axis);

					float t = ray_hit_box(ray_origin, ray_dir, e->position + 0.5f * axis, 
							x_axis * (i == 0 ? 0.5f : 0.25f), 
							y_axis * (i == 1 ? 0.5f : 0.25f), 
							z_axis * (i == 2 ? 0.5f : 0.25f));

					if (t >= 0 && t < min_axis_t) {
						min_axis_t = t;
						best_axis = i;
					}
				}
				if (min_axis_t != FLT_MAX
						&& (hit_entity == editor.selected_entity
							|| min_axis_t < min_hit_t)) { 
					editor.in_gizmo = true;
					editor.did_drag = false;
					editor.dragging_axis = best_axis;
				}
			}
		}

		if (editor.in_gizmo) {
			Entity *e = get_entity(world, editor.selected_entity);
			if (e) {
				v3 axis = editor.dragging_axis == 0 ? x_axis : (editor.dragging_axis == 1 ? y_axis : z_axis);

				// TODO: actually think about this (what if the axis and camera.forward are aligned?)
				v3 plane_normal = cross(axis, cross(axis, camera.forward));

				push_line(e->position, e->position + normalize(plane_normal) * 2,
						V3(1, 0, 1));

				float hit_t;
				if (ray_hit_plane(ray_origin, ray_dir, plane_normal, e->position, &hit_t)) {
					v3 hit_p = ray_origin + hit_t * ray_dir;
					//push_cube_outline(hit_p, V3(0.01f), V3(0.2));
					v3 dp = normalize(axis) * dot(hit_p - e->position, normalize(axis));

					if (!editor.did_drag) {
						editor.did_drag = true;
						editor.drag_p = dp;
					}
					else
						e->position += dp - editor.drag_p;
				}
			}
		}
		else {
			//if (editor.selected_entity == hit_entity && IsDownFirstTime(input, BUTTON_MOUSE_RIGHT))
			//	editor.selected_entity = 0;
			//else
			editor.selected_entity = hit_entity;
		}
	}
	else {
		editor.in_gizmo = 0;
	}
	Entity *e = get_entity(world, editor.selected_entity);
	if (e) {
		v3 x_axis = 2*V3(1, 0, 0);
		v3 y_axis = 2*V3(0, 1, 0);
		v3 z_axis = 2*V3(0, 0, 1);
		for (int i = 0; i < 3; i++) {
			v3 axis = i == 0 ? x_axis : (i == 1 ? y_axis : z_axis);
			v3 color = {};
			color.e[i] = 1;
			if (editor.in_gizmo && editor.dragging_axis == i)
				color = V3(1, 1, 0);
			push_line(e->position, e->position + axis, color);

			push_box_outline(e->position + 0.5f * axis,
					x_axis * (i == 0 ? 0.5f : 0.25f), 
					y_axis * (i == 1 ? 0.5f : 0.25f), 
					z_axis * (i == 2 ? 0.5f : 0.25f), color);
		}
	}

	world.editor_selected_entity = editor.selected_entity;
}
