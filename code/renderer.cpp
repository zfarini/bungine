void init_render_context(Arena *arena, RenderContext &rc, Platform &platform)
{
#if RENDERER_OPENGL
	init_render_context_opengl(rc, platform);
#else
	init_render_context_dx11(rc, platform);
#endif
	rc.loaded_textures = make_array_max<Texture>(arena, 256);
	rc.debug_lines = make_array_max<v3>(arena, 4 * 500000);

	uint32_t white_color = 0xffffffff;
	rc.white_texture = create_texture(make_cstring("__white_texture"), &white_color, 1, 1, true);
}

void push_line(v3 a, v3 b, v3 color = V3(1))
{
	g_rc->debug_lines.push(a);
	g_rc->debug_lines.push(color);
	g_rc->debug_lines.push(b);
	g_rc->debug_lines.push(color);
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
		anim_bones[i].transform = translate(-2, 0, 0) * transform * anim_bones[i].transform;

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


void render_scene(Game &game, Scene &scene, Camera camera, SceneNode *node, mat4 scene_transform, mat4 node_transform,
		Animation *anim, float anim_time, v3 color)
{
	if (node->skip_render)
		return ;

	mat4 local_transform = node->local_transform;
	if (anim) {
		for (usize j = 0; j < anim->nodes.count; j++) {
			if (strings_equal(anim->nodes[j].name, node->name)) {
				local_transform = anim->nodes[j].transform;//get_animated_node_transform(*anim, anim->nodes[j], anim_time);
				break ;
			}
		}
	}
	node_transform = node_transform * local_transform;

	if (node->mesh) {
		Mesh &mesh = *node->mesh;

		Constants constants = {};
		constants.view = camera.view;
		constants.projection = camera.projection;
		constants.camera_p = camera.position;
		constants.light_transform = game.shadow_map.projection * game.shadow_map.view;
		constants.color = color;
		constants.player_p = get_entity(*game.world, game.world->player_id)->position;
		constants.show_normals = game.show_normals;

		mat4 mesh_transform = scene_transform * node_transform * node->geometry_transform;

		constants.model = mesh_transform;
		//constants.normal_transform = inverse(transpose(constants.model));
		if (mesh.bones.count && anim) {
			constants.skinned = 1;
			Arena *temp  = begin_temp_memory();
			Array<Bone> bones = get_animated_bones(temp, mesh.bones, mesh_transform, anim, anim_time);
			for (usize j = 0; j < bones.count; j++)
				constants.bones[j] = bones[j].transform * bones[j].inv_bind;
			end_temp_memory();
		}

		for (usize j = 0; j < mesh.parts.count; j++) {
			MeshPart &part = mesh.parts[j];

			bind_texture(0, part.material.diffuse.valid ? part.material.diffuse : g_rc->white_texture);
			bind_texture(1, part.material.specular.valid ? part.material.specular : g_rc->white_texture);
			bind_texture(2, part.material.normal_map);
			bind_texture(3, part.material.specular_exponent.valid ? part.material.specular_exponent : g_rc->white_texture);

			constants.diffuse_factor = part.material.diffuse_factor;
			constants.specular_factor = part.material.specular_factor;
			constants.specular_exponent_factor = part.material.specular_exponent_factor;
			constants.has_normal_map = part.material.normal_map.valid;

			update_constant_buffer(game.constant_buffer, &constants);
			bind_vertex_buffer(mesh.vertex_buffer);
			draw((int)part.offset, (int)part.vertices_count);
		}
	}

	for (usize i = 0; i < node->childs.count; i++)
		render_scene(game, scene, camera, node->childs[i], scene_transform, node_transform, anim, anim_time, color);
}

void render_scene(Game &game, Scene &scene, Camera camera, mat4 transform, Animation *anim, float anim_time, v3 color,
		bool outline = false)
{
	/*
		clear stencil
		render scene (set stencil to some value X)
		render scene again but scale it along normals and
			use some static color as fragment shader
			and only render if the stencil value is not equal to X
	*/
	if (anim)
		anim_time = fmod(anim_time, anim->duration);

	if (!outline) {
		render_scene(game, scene, camera, scene.root, transform, identity(), anim, anim_time, color);
		return ;
	}

	glEnable(GL_STENCIL_TEST);    
	glStencilMask(0xFF);
	glClear(GL_STENCIL_BUFFER_BIT);

	glStencilFunc(GL_ALWAYS, 250, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	render_scene(game, scene, camera, scene.root, transform, identity(), anim, anim_time, color);

	glStencilFunc(GL_NOTEQUAL, 250, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilMask(0x00);
	
	end_render_pass();
	begin_render_pass(game.outline_render_pass);
	render_scene(game, scene, camera, scene.root, transform, identity(), anim, anim_time, color);
	end_render_pass();
	begin_render_pass(game.mesh_render_pass);

	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}
