entity_id raycast_to_entities(Game &game, World &world, v3 ray_origin, v3 ray_dir,
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


				//push_box_outline(c, x_axis, y_axis, z_axis);
				float t = ray_hit_box(ray_origin, ray_dir, c, x_axis, y_axis, z_axis);
				if (t < 0)
					continue ;
			}

			// TODO: cleanup, is there a better way to do this?
			for (usize t = 0; t < mesh.indices.count; t += 3) {
				v3 v0 = (mesh_transform * V4(mesh.vertices[(int)mesh.indices[t+0]], 1)).xyz;
				v3 v1 = (mesh_transform * V4(mesh.vertices[(int)mesh.indices[t+1]], 1)).xyz;
				v3 v2 = (mesh_transform * V4(mesh.vertices[(int)mesh.indices[t+2]], 1)).xyz;

				v3 u = v1 - v0;
				v3 v = v2 - v0;
				v3 normal = cross(u, v);

				float hit_t;
				if (ray_hit_triangle(ray_origin, ray_dir, v0, v1, v2, &hit_t) &&
						hit_t < min_t) {
					min_t = hit_t;
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
			for (int i = cmesh.vertices.count - 1; i >= op.delete_collision_triangle.index + 3; i--)
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

void update_editor(Game &game, World &world, Editor &editor, GameInput &input, Camera &camera)
{
	{
		Entity *e = get_entity(world, world.editor_selected_entity);

		if (e) {
			ImGuiIO &io = ImGui::GetIO();

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


			//const char * items[EntityType_Count];
			//for (int i = 0; i < EntityType_Count; i++)
			//	items[i] = ENUM_STRING(EntityType, i);
			//int type = e->type;
			//ImGui::ListBox("listbox", &type, items, EntityType_Count, 3);
			//e->type = (EntityType) type;

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

				bind_texture(4, platform.render_context
->white_texture);
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
	if (!game.in_editor)
		return ;

	

	v2 mouse_p = (input.mouse_p * V2(1.f / platform.render_context
->window_width, 1.f / platform.render_context
->window_height)) * 2 - V2(1);
	mouse_p.y *= -1;

	v3 ray_origin = camera.position;
	v3 ray_dir = camera.forward * camera.znear 
		+ mouse_p.x * camera.right * camera.width * 0.5f
		+ mouse_p.y * camera.up * camera.height * 0.5f;
	if (IsDown(input, BUTTON_LEFT_CONTROL)) {
		if (IsDown(input, BUTTON_T)) editor.gizmo_mode = GIZMO_TRANSLATION;
		else if (IsDown(input, BUTTON_R)) editor.gizmo_mode = GIZMO_ROTATION;
		else if (IsDown(input, BUTTON_S)) editor.gizmo_mode = GIZMO_SCALE;
		
		if (IsDownFirstTime(input, BUTTON_Q))  {

			editor.edit_collision_mesh = !editor.edit_collision_mesh;
			if (editor.edit_collision_mesh)
				editor.in_gizmo = 0;
		}
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
			mat4 transform = get_entity_transform(world, *e);

			for (int i = 0; i < 3; i++)
				axis[i] = V3(transform.e[0][i], transform.e[1][i], transform.e[2][i]);
			for (int i = 0; i < 3; i++)
				axis[i] = 2 * normalize(axis[i]);
		}
	}

	if (!editor.edit_collision_mesh && IsDown(input, BUTTON_MOUSE_RIGHT)) {
		float min_hit_t;
		int hit_entity_mesh_index;
		entity_id hit_entity = raycast_to_entities(game, world, ray_origin, ray_dir, min_hit_t, hit_entity_mesh_index);

		if (editor.selected_entity && !editor.in_gizmo && !editor.edit_collision_mesh) {
			Entity *e = get_entity(world, editor.selected_entity);
			if (e) {
				float min_axis_t = FLT_MAX;

				int best_axis = -1;

				float axis_dim = 0.07f;
				float other_dim = axis_dim * 0.3f;

				axis_dim *= length(e->position - camera.position);
				other_dim *= length(e->position - camera.position);

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
					else if (editor.gizmo_mode == GIZMO_TRANSLATION) {
						t = ray_hit_box(ray_origin, ray_dir, e->position + 0.5f * axis_dim * axis[i], 
								axis[0] * (i == 0 ? axis_dim : other_dim), 
								axis[1] * (i == 1 ? axis_dim : other_dim), 
								axis[2] * (i == 2 ? axis_dim : other_dim));
					}
					else if (editor.gizmo_mode == GIZMO_SCALE) {
						float radius = 0.1 * axis_dim;
						t = ray_hit_box(ray_origin, ray_dir, e->position + (axis_dim - 0.5f * radius) * axis[i], 
								axis[0] * radius, 
								axis[1] * radius, 
								axis[2] * radius);
					}

					if (t >= 0 && t < min_axis_t) {
						min_axis_t = t;
						best_axis = i;
					}
				}
				editor.uniform_scale = false;
				if (editor.gizmo_mode == GIZMO_SCALE) {
					float radius = 0.1 * axis_dim;
					float t = ray_hit_box(ray_origin, ray_dir, e->position,
							axis[0] * radius, 
							axis[1] * radius, 
							axis[2] * radius);
					if (t >= 0 && t < min_axis_t) {
						min_axis_t = t;
						editor.uniform_scale = true;
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
				
					v3 plane_normal;

					if (editor.gizmo_mode == GIZMO_SCALE && editor.uniform_scale)
						editor.dragging_axis = 2;

					{
						plane_normal = cross(axis[editor.dragging_axis], cross(axis[editor.dragging_axis], camera.forward));
						if (editor.gizmo_mode == GIZMO_ROTATION)
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
								editor.s_init_drag_p = hit_p;
								editor.s_init_scale = e->scale;//e->scale.e[editor.dragging_axis];
							}
							else {
								v3 new_scale = e->scale;
								if (editor.uniform_scale) {

									v3 v = editor.s_init_drag_p - e->position;

									float l = dot(hit_p - e->position, normalize(v));

									float s;
									if (l >= length(v))
										s = 1 + (l - length(v));
									else
										s = 1 / (length(v) - l + 1);

									new_scale = s * editor.s_init_scale;
								}
								else {
									new_scale.e[editor.dragging_axis] = editor.s_init_scale.e[editor.dragging_axis] + ds - editor.s_init_drag;
									if (new_scale.e[editor.dragging_axis] < 0.01f)
										new_scale.e[editor.dragging_axis] = 0.01f;
								}
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
				editor.selected_entity_mesh = hit_entity_mesh_index;
				Entity *e = get_entity(world, hit_entity);
				if (e)
					editor.init_entity = *e;
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
						do_editor_op(game, world, editor, op);
				}
			}
			editor.in_gizmo = 0;
			if (get_entity(world, editor.selected_entity))
				editor.init_entity = *get_entity(world, editor.selected_entity);
	}
	if (editor.edit_collision_mesh) {
		
		float min_hit_t;
		int hit_entity_mesh_index;
		entity_id hit_entity = raycast_to_entities(game, world, ray_origin, ray_dir, min_hit_t, hit_entity_mesh_index);

		Entity *e = get_entity(world, hit_entity);

		if (e && e->scene_id) {

			mat4 to_world = get_entity_transform(world, *e) * e->scene_transform;
			mat4 to_object = inverse(to_world);

			if (!world.scene_collision_mesh.count(e->scene_id)) {
				CollisionMesh cmesh = {};
				// TODO: cleanup
				cmesh.vertices = make_array_max<v3>(&world.arena, 3 * 1000);
				cmesh.scene = e->scene_id;
				world.collision_meshes.push(cmesh);
				world.scene_collision_mesh[e->scene_id] = world.collision_meshes.count - 1;
			}

			CollisionMesh &cmesh = world.collision_meshes[world.scene_collision_mesh[e->scene_id]];

			v3 hit_p = ray_origin + min_hit_t * ray_dir;

			float dist_to_snap = 0.01f * length(hit_p - camera.position);
			float size = dist_to_snap;

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
					if (ray_hit_triangle(ray_origin, ray_dir, v0, v1, v2, &hit_t)
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

			push_cube_outline(hit_p, V3(size), V3(1, 1, 0));
			for (int i = (cmesh.vertices.count/3)*3; i < cmesh.vertices.count; i++)
				push_cube_outline((to_world * V4(cmesh.vertices[i], 1)).xyz, V3(size), V3(1, 0, 0));

			for (int i = 0; i + 2 < cmesh.vertices.count; i += 3) {
				v3 color = i == hit_triangle ? V3(1, 1, 0) : V3(1, 0, 0);
				push_triangle_outline((to_world * V4(cmesh.vertices[i + 0], 1)).xyz,
							(to_world * V4(cmesh.vertices[i + 1], 1)).xyz,
							(to_world * V4(cmesh.vertices[i + 2], 1)).xyz, color);
			}
		}
	}
	if (!editor.edit_collision_mesh)
	{
		Entity *e = get_entity(world, editor.selected_entity);
		if (e) {

			float axis_dim = 0.07f;
			float other_dim = axis_dim * 0.3f;
			axis_dim  *= length(e->position - camera.position);
			other_dim *= length(e->position - camera.position);

			for (int i = 0; i < 3; i++) {
				v3 color = {};
				color.e[i] = 1;
				if (editor.in_gizmo && 
						(editor.dragging_axis == i && !editor.uniform_scale))
					color = V3(1, 1, 0);

				if (editor.gizmo_mode == GIZMO_SCALE) {
					push_line(e->position, e->position + axis_dim * axis[i], color);
					//push_cube_outline(e->position + axis_dim * axis[i], V3(axis_dim * 0.2f), color);

				}
				else if (editor.gizmo_mode == GIZMO_TRANSLATION) {
					push_line(e->position, e->position + axis_dim * axis[i], color);
					push_ellipsoid_outline(e->position + axis_dim * axis[i], V3(axis_dim * 0.2f), color);
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
				//push_box_outline(e->position + 0.5f * axis[i],
				//		axis[0] * (i == 0 ? 0.5f : 0.25f), 
				//		axis[1] * (i == 1 ? 0.5f : 0.25f), 
				//		axis[2] * (i == 2 ? 0.5f : 0.25f), color);

				if (editor.gizmo_mode == GIZMO_SCALE) {

					//push_cube_outline(e->position + axis_dim * axis[i], V3(axis_dim * 0.2f), color);
					float radius = 0.1 * axis_dim;
					push_box_outline(e->position + (axis_dim - 0.5f * radius) * axis[i], 
							axis[0] * radius, 
							axis[1] * radius, 
							axis[2] * radius, color);
				}
				else if (editor.gizmo_mode == GIZMO_TRANSLATION) {
					push_box_outline(e->position + 0.5f * axis_dim * axis[i], 
							axis[0] * (i == 0 ? axis_dim : other_dim), 
							axis[1] * (i == 1 ? axis_dim : other_dim), 
							axis[2] * (i == 2 ? axis_dim : other_dim), color);
				}
			}
			if (editor.gizmo_mode == GIZMO_SCALE) {
				float radius = 0.1 * axis_dim;
				v3 color = editor.in_gizmo && editor.uniform_scale ? V3(1, 1, 0) : V3(1);
				push_box_outline(e->position,
							axis[0] * radius, 
							axis[1] * radius, 
							axis[2] * radius, color);
			}
		}
	}

	world.editor_selected_entity = editor.selected_entity;
	editor.last_camera_p = camera.position;
	if (IsDown(input, BUTTON_LEFT_CONTROL) &&
		IsDownFirstTime(input, BUTTON_C) &&
		editor.selected_entity) {
		editor.copied_entity = editor.selected_entity;
		editor.copy_entity_mesh = IsDown(input, BUTTON_LEFT_SHIFT);
	}
	if (IsDown(input, BUTTON_LEFT_CONTROL) &&
		IsDownFirstTime(input, BUTTON_V)) {
		Entity *copy_from = get_entity(world, editor.copied_entity);
		if (copy_from) {
			EditorOp op = {};
			op.type = EDITOR_OP_PASTE_ENTITY;
			op.paste.copy_from = copy_from->id;
			if (editor.copy_entity_mesh)
				op.paste.mesh_index = editor.selected_entity_mesh;
			else
				op.paste.mesh_index = -1;

			float min_hit_t;
			int hit_mesh_index;
			entity_id hit_entity = raycast_to_entities(game, world, ray_origin, ray_dir, min_hit_t, hit_mesh_index);
			if (hit_entity) {
				op.paste.p = ray_origin + min_hit_t * ray_dir
					+ V3(0, 0, copy_from->scale.z);
			}else 
				op.paste.p = camera.position + camera.forward * 2
					* max(copy_from->scale.x, max(copy_from->scale.y, copy_from->scale.z));
			
			do_editor_op(game, world, editor, op);
		}
	}

	if (!editor.in_gizmo && IsDown(input, BUTTON_LEFT_CONTROL) &&
		IsDownFirstTime(input, BUTTON_Z)) {
		undo_editor_op(game, world, editor);
	}
	if (!editor.in_gizmo && IsDown(input, BUTTON_LEFT_CONTROL) &&
		IsDownFirstTime(input, BUTTON_X)) {
		redo_editor_op(game, world, editor);
	}
}
