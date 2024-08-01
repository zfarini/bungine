entity_id raycast_to_entities(Game &game, World &world, v3 ray_origin, v3 ray_dir,
		float &hit_t)
{
	entity_id hit_id = 0;
	float min_t = FLT_MAX;

	for (usize i = 0; i < world.entities.count; i++) {
		Entity &e = world.entities[i];
		if (!e.scene_id)
			continue ;

		Scene &scene = game.scenes[e.scene_id];
		mat4 transform = get_entity_transform(e) * e.scene_transform;
		for (usize j = 0; j < scene.meshes.count; j++) {
			Mesh &mesh = scene.meshes[j];
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

void do_editor_op(World &world, Editor &editor, EditorOp &op)
{
	Entity *e = get_entity(world, op.entity);
	if (e) {
		if (op.type == EDITOR_OP_TRANSLATE_ENTITY)
			e->position = op.translate.new_p;
		else if (op.type == EDITOR_OP_ROTATE_ENTITY)
			e->rotation = op.rotate.new_rot;
		else if (op.type == EDITOR_OP_SCALE_ENTITY)
			e->scale = op.scale.new_scale;
	}
	if (op.type == EDITOR_OP_PASTE_ENTITY) {
		Entity *copy_from = get_entity(world, op.paste.copy_from);
		if (copy_from) {
			// NOTE: if op.pase.id is not zero this means we are redoing this operation
			// the first time we will create then entity with any id but the second 
			// we should assign the same previous id
			if (op.paste.id) {
				Entity p = *copy_from;
				p.id = op.paste.id;
				p.position = op.paste.p;
				world.entities.push(p);
				world.entities_id_map[p.id] = world.entities.count - 1;
			}
			else {
				Entity *p = make_entity(world);
				entity_id id = p->id;
				*p = *copy_from;
				p->id = id;
				p->position = op.paste.p;
				op.paste.id = p->id;
			}
		}
	}
	else if (op.type == EDITOR_OP_DELETE_ENTITY)
		remove_entity(world, op.entity);

	editor.ops.push(op);
}

void redo_editor_op(World &world, Editor &editor)
{
	if (!editor.undos.count)
		return ;
	EditorOp op = editor.undos[editor.undos.count - 1];
	editor.undos.count--;

	do_editor_op(world, editor, op);
}

void undo_editor_op(World &world, Editor &editor)
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
	}
	if (op.type == EDITOR_OP_PASTE_ENTITY)
		remove_entity(world, op.paste.id);
	else if (op.type == EDITOR_OP_DELETE_ENTITY) {
		world.entities.push(op.del.entity_data);
		world.entities_id_map[op.del.entity_data.id] = world.entities.count - 1;
	}
}

void update_editor(Game &game, World &world, Editor &editor, GameInput &input, Camera &camera)
{
	v2 mouse_p = (input.mouse_p * V2(1.f / g_rc->window_width, 1.f / g_rc->window_height)) * 2 - V2(1);
	mouse_p.y *= -1;

	v3 ray_origin = camera.position;
	v3 ray_dir = camera.forward * camera.znear 
		+ mouse_p.x * camera.right * camera.width * 0.5f
		+ mouse_p.y * camera.up * camera.height * 0.5f;
	if (IsDown(input, BUTTON_LEFT_CONTROL)) {
		if (IsDown(input, BUTTON_T)) editor.gizmo_mode = GIZMO_TRANSLATION;
		else if (IsDown(input, BUTTON_R)) editor.gizmo_mode = GIZMO_ROTATION;
		else if (IsDown(input, BUTTON_S)) editor.gizmo_mode = GIZMO_SCALE;
	}
	float rotation_inner_radius = 1.5;
	float rotation_outer_radius = 2;
	float rotation_circle_radius = 0.5f * (rotation_inner_radius + rotation_outer_radius);
	v3 axis[3] = {
		2*V3(1, 0, 0),
		2*V3(0, 1, 0),
		2*V3(0, 0, 1)
	};

	if (editor.gizmo_mode == GIZMO_SCALE || editor.gizmo_mode == GIZMO_ROTATION) {
		Entity *e = get_entity(world, editor.selected_entity);
		if (e) { 
			mat4 transform = get_entity_transform(*e);

			for (int i = 0; i < 3; i++)
				axis[i] = V3(transform.e[0][i], transform.e[1][i], transform.e[2][i]);
			for (int i = 0; i < 3; i++)
				axis[i] = 2 * normalize(axis[i]);
		}
	}

	if (IsDown(input, BUTTON_MOUSE_RIGHT)) {
		float min_hit_t;
		entity_id hit_entity = raycast_to_entities(game, world, ray_origin, ray_dir, min_hit_t);

		if (editor.selected_entity && !editor.in_gizmo) {
			Entity *e = get_entity(world, editor.selected_entity);
			if (e) {
				float min_axis_t = FLT_MAX;
				int best_axis = -1;
				for (int i = 0; i < 3; i++) {

					float t = -1;
					if (editor.gizmo_mode == GIZMO_ROTATION) {
						if (ray_hit_plane(ray_origin, ray_dir, axis[i], e->position, &t)) {
							v3 p = ray_origin + t * ray_dir;

							if (t >= 0 && 
									length(p - e->position) > rotation_inner_radius
									&&  length(p - e->position) < rotation_outer_radius) {
							}
							else
								t = -1;
						}
					}
					else {
						t = ray_hit_box(ray_origin, ray_dir, e->position + 0.5f * axis[i], 
								axis[0] * (i == 0 ? 0.5f : 0.15f), 
								axis[1] * (i == 1 ? 0.5f : 0.15f), 
								axis[2] * (i == 2 ? 0.5f : 0.15f));
					}

					if (t >= 0 && t < min_axis_t) {
						min_axis_t = t;
						best_axis = i;
					}
				}
				if (min_axis_t != FLT_MAX) {
					//&& (hit_entity == editor.selected_entity
					//	|| min_axis_t < min_hit_t)) { 
					editor.in_gizmo = true;
					editor.did_drag = false;
					editor.dragging_axis = best_axis;
				}
				}
			}

			if (editor.in_gizmo) {

				Entity *e = get_entity(world, editor.selected_entity);
				if (e) {
					if (editor.did_drag && !v3_equal(camera.position, editor.last_camera_p))
						editor.did_drag = false;

					// TODO: actually think about this (what if the axis and camera.forward are aligned?)
					v3 plane_normal = cross(axis[editor.dragging_axis], cross(axis[editor.dragging_axis], camera.forward));
					if (editor.gizmo_mode == GIZMO_ROTATION) {
						//	plane_normal = cross(axis[editor.dragging_axis], camera.forward);
						plane_normal = axis[editor.dragging_axis];
					}

					push_line(e->position, e->position + normalize(plane_normal) * 2,
							V3(1, 0, 1));

					float hit_t;
					if (ray_hit_plane(ray_origin, ray_dir, plane_normal, e->position, &hit_t)) {
						v3 hit_p = ray_origin + hit_t * ray_dir;

						if (editor.gizmo_mode == GIZMO_TRANSLATION) {

							//push_cube_outline(hit_p, V3(0.01f), V3(0.2));
							v3 dp = normalize(axis[editor.dragging_axis]) * dot(hit_p - e->position, normalize(axis[editor.dragging_axis]));

							if (!editor.did_drag) {
								editor.did_drag = true;
								editor.p_init_drag = dp;
							}
							else
								e->position += dp - editor.p_init_drag;
						}
						else if (editor.gizmo_mode == GIZMO_SCALE) {

							float ds = dot(hit_p - e->position, normalize(axis[editor.dragging_axis]));
							//printf("DRAG: %f %f %f\n", ds, editor.s_init_drag, e->scale.e[editor.dragging_axis]);

							if (!editor.did_drag) {
								editor.did_drag = true;
								editor.s_init_drag = ds;
								editor.s_init_scale = e->scale.e[editor.dragging_axis];
							}
							else {
								v3 new_scale = e->scale;
								new_scale.e[editor.dragging_axis] = editor.s_init_scale + ds - editor.s_init_drag;
								if (new_scale.e[editor.dragging_axis] < 0.01f)
									new_scale.e[editor.dragging_axis] = 0.01f;
								e->scale = new_scale;
							}
						}
						else if (editor.gizmo_mode == GIZMO_ROTATION) {

							v3 p = normalize(hit_p - e->position) * rotation_circle_radius;


							push_cube_outline(e->position + p, V3(0.1), V3(0));

							v3 right_axis = axis[(editor.dragging_axis+1)%3];
							v3 up_axis = axis[(editor.dragging_axis+2)%3];

							if (editor.did_drag)
								right_axis = editor.r_right_axis, up_axis = editor.r_up_axis;
							else
								editor.r_axis = axis[editor.dragging_axis];

#if 1
							// transform that will take to a unit circle in xy plane?
							mat4 m = inverse(
									mat4_cols(V4(right_axis, 0),
										V4(up_axis, 0), V4(editor.r_axis, 0),
										V4(0, 0, 0, 1)));

							p = (m * V4(p, 0)).xyz;
#else
							// TODO: shouldn't this work?
							p.x = dot(p, right_axis);
							p.y = dot(p, up_axis);
#endif

							push_line(e->position, e->position + right_axis, V3(1));
							push_line(e->position, e->position + up_axis, V3(0.2));
							float a = atan2(p.y, p.x);

							if (!editor.did_drag) {
								editor.did_drag = true;
								editor.r_init_drag = a;
								editor.r_init_rot = e->rotation;
								editor.r_right_axis = right_axis;
								editor.r_up_axis = up_axis;
								editor.r_axis = axis[editor.dragging_axis];
							}
							else {
								e->rotation = rotate_around_axis_quat(editor.r_axis, a - editor.r_init_drag)
									* editor.r_init_rot;
							}
						}
						else
							assert(0);
					}
				}
			}
			else {
				//if (editor.selected_entity == hit_entity && IsDownFirstTime(input, BUTTON_MOUSE_RIGHT))
				//	editor.selected_entity = 0;
				//else
				editor.selected_entity = hit_entity;
				editor.init_entity = *get_entity(world, hit_entity);
			}
		}
		else {
			if (editor.in_gizmo && editor.did_drag) {
				EditorOp op = {};
				Entity *e = get_entity(world, editor.selected_entity);
				if (e) {
					op.entity = editor.init_entity.id;
					bool push = true;
					if (editor.gizmo_mode == GIZMO_TRANSLATION) {
						op.type = EDITOR_OP_TRANSLATE_ENTITY;
						op.translate.prev_p = editor.init_entity.position;
						op.translate.new_p = e->position;
						push &= (!v3_equal(op.translate.prev_p, op.translate.new_p));
					}
					else if (editor.gizmo_mode == GIZMO_SCALE) {
						op.type = EDITOR_OP_SCALE_ENTITY;
						op.scale.prev_scale = editor.init_entity.scale;
						op.scale.new_scale = e->scale;
						push &= (!v3_equal(op.scale.prev_scale, op.scale.new_scale));
					}
					else if (editor.gizmo_mode == GIZMO_ROTATION) {
						op.type = EDITOR_OP_ROTATE_ENTITY;
						op.rotate.prev_rot = editor.init_entity.rotation;
						op.rotate.new_rot = e->rotation;
						push &= (!quat_equal(op.rotate.prev_rot, op.rotate.new_rot));
					}
					if (push)
						do_editor_op(world, editor, op);
				}
			}
			editor.in_gizmo = 0;
			if (get_entity(world, editor.selected_entity))
				editor.init_entity = *get_entity(world, editor.selected_entity);
		}
		Entity *e = get_entity(world, editor.selected_entity);
		if (e) {

			for (int i = 0; i < 3; i++) {
				v3 color = {};
				color.e[i] = 1;
				if (editor.in_gizmo && editor.dragging_axis == i)
					color = V3(1, 1, 0);

				if (editor.gizmo_mode == GIZMO_SCALE) {
					push_line(e->position, e->position + axis[i], color);
					push_cube_outline(e->position + axis[i], V3(0.1f), color);
				}
				else if (editor.gizmo_mode == GIZMO_TRANSLATION) {
					push_line(e->position, e->position + axis[i], color);
					push_ellipsoid_outline(e->position + axis[i], V3(0.1f), color);
				}
				else {
#if 0
					for (int j = 0; j < 36; j++)
						push_circle(e->position, 
								lerp(rotation_inner_radius, rotation_outer_radius, j / 36.f)
								, axis[(i + 1) % 3], axis[(i + 2) % 3], color);
#else
					push_circle(e->position, 
							rotation_circle_radius
							, axis[(i + 1) % 3], axis[(i + 2) % 3], color);
#endif

				}
				//push_box_outline(e->position + 0.5f * axis,
				//		x_axis * (i == 0 ? 0.5f : 0.25f), 
				//		y_axis * (i == 1 ? 0.5f : 0.25f), 
				//		z_axis * (i == 2 ? 0.5f : 0.25f), color);
			}
		}

		world.editor_selected_entity = editor.selected_entity;
		editor.last_camera_p = camera.position;
	if (IsDown(input, BUTTON_LEFT_CONTROL) &&
		IsDownFirstTime(input, BUTTON_C) &&
		editor.selected_entity) {
		editor.copied_entity = editor.selected_entity;
	}
	if (IsDown(input, BUTTON_LEFT_CONTROL) &&
		IsDownFirstTime(input, BUTTON_V)) {
		Entity *e = get_entity(world, editor.copied_entity);
		if (e) {
			EditorOp op = {};
			op.type = EDITOR_OP_PASTE_ENTITY;
			op.paste.copy_from = e->id;

			float min_hit_t;
			entity_id hit_entity = raycast_to_entities(game, world, ray_origin, ray_dir, min_hit_t);
			if (hit_entity) {
				op.paste.p = ray_origin + min_hit_t * ray_dir
					+ V3(0, 0, e->scale.z);
			}else 
				op.paste.p = camera.position + camera.forward * 2
					* max(e->scale.x, max(e->scale.y, e->scale.z));
			
			do_editor_op(world, editor, op);
		}
	}


	{
		Entity *e = get_entity(world, world.editor_selected_entity);

		if (e) {
			ImGuiIO &io = ImGui::GetIO();

			ImGui::Begin("Entity");
            ImGui::ColorEdit3("color", e->color.e);
			ImGui::Text("id: %ld", e->id);
			ImGui::Text("type: %s", ENUM_STRING(EntityType, e->type));
			if (ImGui::Button("Reset scale"))
				e->scale = V3(1);
			if (ImGui::Button("Reset rotation"))
				e->rotation = identity_quat();

			if (e->id != world.player_id && ImGui::Button("delete")) {
				EditorOp op = {};
				op.entity = e->id;
				op.type = EDITOR_OP_DELETE_ENTITY;
				op.del.entity_data = *e;

				do_editor_op(world, editor, op);
				e = 0;
			}
#if 0
			begin_render_pass(game.mesh_render_pass);
			{
				FrameBuffer &fb = game.debug_asset_fb;
				bind_framebuffer(fb);

				int width = fb.color_texture.width;
				int height = fb.color_texture.height;

				set_viewport(0, 0, width, height);

				clear_framebuffer_color(fb, V4(0.3f, 0.3f, 0.3f, 1.f));
				clear_framebuffer_depth(fb, 1);

				bind_texture(4, g_rc->white_texture);
				Camera camera = {};
				camera.view = lookat(V3(0, -2, 0), V3(0, 1, 0), V3(0, 0, 1));
				camera.projection = game_camera.projection;
				camera.projection = perspective_projection(0.1, 100, 90, (float)height/width);
				camera.projection.e[1][1] *= -1;
				render_scene(game, game.scenes[e->scene_id], camera, 
					zrotation(PI/4), 0, 0, e->color);
				//render_entities(game, world, camera, false);

				Arena *temp = begin_temp_memory();

				void *data = arena_alloc(temp, sizeof(uint32_t) * width * height);
				glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

				glBindTexture(GL_TEXTURE_2D, game.debug_asset_tex.id);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA,
						GL_UNSIGNED_BYTE, data);
				ImGui::Image((void *)(intptr_t)game.debug_asset_tex.id, ImVec2(width, height));

				end_temp_memory();
			}
			end_render_pass();
#endif

			ImGui::End();
		}
	}
	if (!editor.in_gizmo && IsDown(input, BUTTON_LEFT_CONTROL) &&
		IsDownFirstTime(input, BUTTON_Z)) {
		undo_editor_op(world, editor);
	}
	if (!editor.in_gizmo && IsDown(input, BUTTON_LEFT_CONTROL) &&
		IsDownFirstTime(input, BUTTON_X)) {
		redo_editor_op(world, editor);
	}
}
