void init_render_context(RenderContext &rc)
{
#if RENDERER_OPENGL
	init_render_context_opengl(rc);
#else
	init_render_context_dx11(rc);
#endif
	rc.arena.thread_safe = true;
	rc.loaded_textures = make_array_max<Texture>(&rc.arena, 256);
	rc.debug_lines = make_array_max<v3>(&rc.arena, 5000000);

	uint32_t white_color = 0xffffffff;
	rc.white_texture = create_texture(make_cstring("__white_texture"), &white_color, 1, 1, true);
	uint32_t purple_color = 0xffff00ff;
	rc.purple_texture = create_texture(make_cstring("__purple_texture"), &purple_color, 1, 1, true);
}

void push_line(v3 a, v3 b, v3 color = V3(1))
{
	platform.render_context
->debug_lines.push(a);
	platform.render_context
->debug_lines.push(color);
	platform.render_context
->debug_lines.push(b);
	platform.render_context
->debug_lines.push(color);
}

void push_cube_outline(v3 center, v3 radius, v3 color = V3(1))
{
	v3 p00 = center + V3(-radius.x, -radius.y, -radius.z);
	v3 p01 = center + V3(+radius.x, -radius.y, -radius.z);
	v3 p02 = center + V3(+radius.x, +radius.y, -radius.z);
	v3 p03 = center + V3(-radius.x, +radius.y, -radius.z);
	v3 p10 = center + V3(-radius.x, -radius.y, +radius.z);
	v3 p11 = center + V3(+radius.x, -radius.y, +radius.z);
	v3 p12 = center + V3(+radius.x, +radius.y, +radius.z);
	v3 p13 = center + V3(-radius.x, +radius.y, +radius.z);

	push_line(p00, p01, color);
	push_line(p01, p02, color);
	push_line(p02, p03, color);
	push_line(p03, p00, color);

	push_line(p10, p11, color);
	push_line(p11, p12, color);
	push_line(p12, p13, color);
	push_line(p13, p10, color);

	push_line(p00, p10, color);
	push_line(p01, p11, color);
	push_line(p02, p12, color);
	push_line(p03, p13, color);
}

void push_box_outline(v3 center,
		v3 x_axis, v3 y_axis, v3 z_axis, v3 color = V3(1))
{
	// v3 y_axis = normalize(cross(max - min, V3(0, 0, 1)));
	// v3 x_axis = normalize(cross(y_axis, V3(0, 0, 1)));
	// v3 z_axis = normalize(cross(x_axis, y_axis));

	// v3 center = (min + max) * 0.5f;
	// v3 radius = (max - min) * 0.5f;


	// push_line(center, center + x_axis*fabsf(radius.x), V3(1, 0, 0));
	// push_line(center, center + y_axis*fabsf(radius.y), V3(0, 1, 0));
	// push_line(center, center + z_axis*fabsf(radius.z), V3(0, 0, 1));

	v3 p[8];
	int i = 0;
	/*
	   -1 -1 -1
	   1, -1 -1
	   -1 1 -1
	   1 1 -1
	   */
	for (int dz = -1; dz <= 1; dz++) {
		for (int dy = -1; dy <= 1; dy++) {
			for (int dx = -1; dx <= 1; dx++) {
				if (dx && dy && dz) {
					p[i++] = center + x_axis * dx
						+ y_axis * dy
						+ z_axis * dz;
				}
			}
		}
	}

	v3 p00 = p[0];
	v3 p01 = p[1];
	v3 p02 = p[3];
	v3 p03 = p[2];

	v3 p10 = p[4];
	v3 p11 = p[5];
	v3 p12 = p[7];
	v3 p13 = p[6]; 


	push_line(p00, p01, color);
	push_line(p01, p02, color);
	push_line(p02, p03, color);
	push_line(p03, p00, color);

	push_line(p10, p11, color);
	push_line(p11, p12, color);
	push_line(p12, p13, color);
	push_line(p13, p10, color);

	push_line(p00, p10, color);
	push_line(p01, p11, color);
	push_line(p02, p12, color);
	push_line(p03, p13, color);
}

void push_triangle_outline(v3 p0, v3 p1, v3 p2, v3 color = V3(1))
{
	push_line(p0, p1, color);
	push_line(p1, p2, color);
	push_line(p2, p0, color);
}

void push_ellipsoid_outline(v3 p, v3 r, v3 color = V3(1))
{
	v3 x = V3(r.x, 0, 0);
	v3 y = V3(0, r.y, 0);
	v3 z = V3(0, 0, r.z);

	int itr_count = 60;
	float a = (2 * PI) / itr_count;

	for (int itr = 0; itr < itr_count; itr++)
	{
		float a0 = a * itr;
		float a1 = a * (itr + 1);

		v3 px0 = p + x * cosf(a0) + y * sinf(a0);
		v3 px1 = p + x * cosf(a1) + y * sinf(a1);

		v3 py0 = p + y * cosf(a0) + z * sinf(a0);
		v3 py1 = p + y * cosf(a1) + z * sinf(a1);

		v3 pz0 = p + x * cosf(a0) + z * sinf(a0);
		v3 pz1 = p + x * cosf(a1) + z * sinf(a1);

		push_line(px0, px1, color);
		push_line(py0, py1, color);
		push_line(pz0, pz1, color);
	}
}

void push_circle(v3 p, float r, v3 right_axis, v3 up_axis, v3 color = V3(1))
{
	right_axis = normalize(right_axis);
	up_axis = normalize(up_axis);

	int itr_count = 60;
	float a = (2 * PI) / itr_count;

	for (int itr = 0; itr < itr_count; itr++)
	{
		float a0 = a * itr;
		float a1 = a * (itr + 1);

		v3 p0 = p + r * (right_axis * cosf(a0) + up_axis * sinf(a0));
		v3 p1 = p + r * (right_axis * cosf(a1) + up_axis * sinf(a1));

		push_line(p0, p1, color);
	}
}

void compute_bone_transform(Array<Bone> bones, int i, Array<bool> computed)
{
	if (computed[i] || bones[i].parent == -1)
		return ;
	compute_bone_transform(bones, bones[i].parent, computed);
	bones[i].transform = bones[bones[i].parent].transform *  bones[i].transform;
	computed[i] = true;
}

void render_bones(Array<Bone> bones, mat4 transform, Animation *anim, float anim_time)
{
	if (bones.count == 0)
		return ;

	Arena *temp = begin_temp_memory();

	Array<Bone> anim_bones = clone_array(temp, bones);

	if (anim) {
		for (usize i = 0; i < bones.count; i++) {
			int index = -1;
			for (usize j = 0; j < anim->nodes.count; j++) {
				if (strings_equal(anim->nodes[j].name, anim_bones[i].name)) {
					index = (int)j;
					break ;
				}
			}
			if (index != -1) 
				anim_bones[i].transform = anim->nodes[index].transform;//get_animated_node_transform(*anim, anim->nodes[index], anim_time);
		}
	}

	Array<bool> computed = make_array<bool>(temp, anim_bones.count);
	for (int i = 0; i < anim_bones.count; i++)
		computed[i] = false;
	for (int i = 0; i < anim_bones.count; i++)
		compute_bone_transform(anim_bones, i, computed);
	for (int i = 0; i < anim_bones.count; i++)
		anim_bones[i].transform =  transform * anim_bones[i].transform;

	for (usize i = 0; i < anim_bones.count; i++) {
		v3 P = (anim_bones[i].transform * v4{0, 0, 0, 1}).xyz;

		if (anim_bones[i].parent != -1) {
			v3 parentP = (anim_bones[anim_bones[i].parent].transform * v4{0, 0, 0, 1}).xyz;
			push_line(P, parentP);
		}
	}

	end_temp_memory();
}

Array<Bone> get_animated_bones(Arena *arena, Array<Bone> bones, mat4 transform, Animation *anim, float anim_time)
{
	Array<Bone> anim_bones = clone_array(arena, bones);

	for (usize i = 0; i < bones.count; i++) {
		int index = -1;
		for (usize j = 0; j < anim->nodes.count; j++) {
			if (strings_equal(anim->nodes[j].name, anim_bones[i].name)) {
				index = (int)j;
				break ;
			}
		}
		if (index != -1) {
			anim_bones[i].transform = anim->nodes[index].transform;//get_animated_node_transform(*anim, anim->nodes[index], anim_time);
		}
	}

	Array<bool> computed = make_zero_array<bool>(arena, anim_bones.count);
	for (int i = 0; i < anim_bones.count; i++)
		compute_bone_transform(anim_bones, i, computed);
	for (int i = 0; i < anim_bones.count; i++)
		anim_bones[i].transform = transform * anim_bones[i].transform;

	return anim_bones;
}

void render_scene(Game &game, World &world, SceneID scene_id, Camera camera, mat4 transform, Animation *anim, float anim_time, v3 color, bool depth_only)
{
	if (!scene_id)
		return ;
	Scene &scene = get_scene_by_id(game, scene_id);
	if (scene.state == SCENE_UNLOADED) {
		scene.state = SCENE_LOADING;
		platform.add_thread_work(load_scene_work, &scene);
		//load_scene_work(&scene);
		return ;
	}
	else if (scene.state == SCENE_LOADED) {
		if (!scene.in_gpu) {
			for (int i = 0; i < scene.meshes.count; i++) {
				Mesh &mesh = scene.meshes[i];
				mesh.vertex_buffer = create_vertex_buffer(VERTEX_BUFFER_IMMUTABLE,
				mesh.full_vertices.count * sizeof(Vertex), mesh.full_vertices.data);
				mesh.index_buffer = create_index_buffer(mesh.indices.count, mesh.indices.data);
			}
			scene.in_gpu = true;
		}
	}
	else
		return ;

	if (anim)
		anim_time = fmod(anim_time, anim->duration);

	for (usize mesh_idx = 0; mesh_idx < scene.meshes.count; mesh_idx++) {
		Mesh &mesh = scene.meshes[mesh_idx];

		Constants constants = {};
		constants.view = camera.view;
		constants.projection = camera.projection;
		constants.camera_p = camera.position;
		constants.light_transform = game.shadow_map.projection * game.shadow_map.view;
		constants.color = color;
		constants.player_p = get_entity(*game.world, game.world->player_id)->position;
		constants.show_normals = game.show_normals;

		// TODO: cleanup
		{
			for (int i = 0; i < game.world->entities.count; i++) {
				Entity &e = game.world->entities[i];

				if (constants.point_light_count == ARRAY_SIZE(constants.point_light_color))
					break ;
				if (e.type == EntityType_PointLight) {
					int j = constants.point_light_count++;
					constants.point_light_position[j].xyz = e.position;
					constants.point_light_color[j].xyz = e.color * e.point_light_scale;
				}
			}
		}

		mat4 mesh_transform = transform * mesh.transform;
		
		constants.model = mesh_transform;
		//constants.normal_transform = inverse(transpose(constants.model));
		if (mesh.bones.count && anim) {
			constants.skinned = 1;
			Arena *temp  = begin_temp_memory();
			Array<Bone> bones = get_animated_bones(temp, mesh.bones, mesh_transform, anim, anim_time);

			for (usize j = 0; j < bones.count; j++)
				constants.bones[j] = bones[j].transform * bones[j].inv_bind;

			if (game.render_bones)
				render_bones(bones, mesh_transform, anim, anim_time);
			end_temp_memory();
		}
		// 14950
		// 14350

		// cull meshes outside of view frustum
		bool cull_mesh = false;
		
		if (!constants.skinned && game.frustum_culling)
		{
			Camera cam = camera;
			// cam = world.last_game_camera;

			v3 c[2] = {mesh.box_min, mesh.box_max};
			v3 p[8];
			mat4 to_camera = cam.view * mesh_transform;
			for (int x = 0; x < 2; x++)
			for (int y = 0; y < 2; y++)
			for (int z = 0; z < 2; z++)
				p[(x<<2)|(y<<1)|z] = (to_camera * V4(c[x].x, c[y].y, c[z].z, 1)).xyz;

			v3 frustum_points[8];

			float zfar_width, zfar_height;

			if (cam.type == CAMERA_TYPE_ORTHOGRAPHIC)
				zfar_width = cam.width, zfar_height = cam.height;
			else
				zfar_width = cam.width * (cam.zfar / cam.znear), zfar_height = cam.height * (cam.zfar /cam.znear);

			
			frustum_points[0] = V3(-0.5f * cam.width, -0.5f * cam.height, -cam.znear);
			frustum_points[1] = V3(+0.5f * cam.width, -0.5f * cam.height, -cam.znear);
			frustum_points[2] = V3(+0.5f * cam.width, +0.5f * cam.height, -cam.znear);
			frustum_points[3] = V3(-0.5f * cam.width, +0.5f * cam.height, -cam.znear);
			
			frustum_points[4] = V3(-0.5f * zfar_width, -0.5f * zfar_height, -cam.zfar);
			frustum_points[5] = V3(+0.5f * zfar_width, -0.5f * zfar_height, -cam.zfar);
			frustum_points[6] = V3(+0.5f * zfar_width, +0.5f * zfar_height, -cam.zfar);
			frustum_points[7] = V3(-0.5f * zfar_width, +0.5f * zfar_height, -cam.zfar);
			// points are counter clockwise, normal points out of the frustum
			v3 frustum_planes[6][3] = {
				{frustum_points[0], frustum_points[3], frustum_points[4]}, // left
				{frustum_points[2], frustum_points[1], frustum_points[5]}, // right
				{frustum_points[3], frustum_points[2], frustum_points[6]}, // up
				{frustum_points[1], frustum_points[0], frustum_points[4]}, // down
				{frustum_points[0], frustum_points[1], frustum_points[2]}, // front
				{frustum_points[5], frustum_points[4], frustum_points[6]}, // back
			};

			for (int i = 0; i < ARRAY_SIZE(frustum_planes); i++) {
				v3 normal = cross(frustum_planes[i][1] - frustum_planes[i][0], frustum_planes[i][2] - frustum_planes[i][0]);
				bool cull = true;
				for (int j = 0; j < ARRAY_SIZE(p); j++) {
					if (dot(p[j] - frustum_planes[i][0], normal) < 0) {
						cull = false;
						break ;
					}
				}	
				if (cull) {
					cull_mesh = true;
					break ;
				}
			}
		}

		if (!cull_mesh) {
			for (usize j = 0; j < mesh.parts.count; j++) {
				MeshPart &part = mesh.parts[j];

				if (!depth_only) {
					bind_texture(0, part.material.diffuse ? part.material.diffuse : platform.render_context->white_texture);
					bind_texture(1, part.material.specular ? part.material.specular : platform.render_context->white_texture);
					bind_texture(2, part.material.normal_map);
					bind_texture(3, part.material.specular_exponent ? part.material.specular_exponent : platform.render_context->white_texture);
				}

				constants.diffuse_factor = part.material.diffuse_factor;
				constants.specular_factor = part.material.specular_factor;
				constants.specular_exponent_factor = part.material.specular_exponent_factor;
				constants.has_normal_map = part.material.normal_map != 0;

				update_constant_buffer(game.constant_buffer, &constants);
				bind_vertex_buffer(mesh.vertex_buffer);
				bind_index_buffer(mesh.index_buffer);
				//draw((int)part.offset, (int)part.vertices_count);
				draw_indexed((int)part.offset, (int)part.indices_count);
			}
		}
	}
}
