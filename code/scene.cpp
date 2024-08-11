#include "scene.h"


v3 ufbx_to_v3(ufbx_vec3 v)
{
	return v3{(float)v.x, (float)v.y, (float)v.z};
}

v2 ufbx_to_v2(ufbx_vec2 v)
{
	return v2{(float)v.x, (float)v.y};
}

quat ufbx_to_quat(ufbx_quat q)
{
	return quat{(float)q.x, (float)q.y, (float)q.z, (float)q.w};
}

mat4 ufbx_to_mat4(ufbx_matrix m)
{
	mat4 result = {};

	for (int i = 0; i < 4; i++) {
		result.e[0][i] = m.cols[i].x;
		result.e[1][i] = m.cols[i].y;
		result.e[2][i] = m.cols[i].z;
	}

	result.e[3][3] = 1;

	return (result);
}

#if 0
struct stb_load_texture_data
{
	char filename[256];
	Texture *texture;
};

THREAD_WORK_FUNC(stb_load_texture_work)
{
	stb_load_texture_data *data = (stb_load_texture_data *)data;
	void *data;
	int width, height, n_channels;
	data = stbi_load(path.data, &width, &height, &n_channels, 4);
}
#endif

Texture load_texture(Arena *arena, Scene &scene, ufbx_texture *utex, bool srgb = true)
{
	assert(utex->type == UFBX_TEXTURE_FILE);

	
	stbi_set_flip_vertically_on_load(true);

	String name = make_string(arena, utex->filename.length, utex->filename.data);
	assert(name.count);

	if (name.count) {
		for (int i = 0; i < g_rc->loaded_textures.count; i++) {
			if (strings_equal(g_rc->loaded_textures[i].name, name))
				return g_rc->loaded_textures[i];
		}
	}
	void *data;
	int width, height, n_channels;
	Arena *temp = begin_temp_memory();
	if (utex->content.size) {
		data = stbi_load_from_memory((stbi_uc *)utex->content.data,
				(int)utex->content.size, &width, &height,
				&n_channels, 4);
	} else {
		int last_slash = (int)utex->filename.length - 1;

		while (last_slash >= 0 && 
				(utex->filename.data[last_slash] != '/' &&
				 utex->filename.data[last_slash] != '\\'))
			last_slash--;
		last_slash++;

		String path = concact_string(temp, scene.path, make_string(temp, utex->filename.length - last_slash, utex->filename.data + last_slash));
		path = concact_string(temp, path, make_string(temp, 1, ""));

		//add_thread_work(stb_load_texture_work, );
		data = stbi_load(path.data, &width, &height, &n_channels, 4);
		if (!data)
			printf("failed to load texture file: %s\n", path.data);
	}
	//assert(data);
	end_temp_memory();

	Texture texture = create_texture(name, data, width, height, srgb);
	if (name.count)
		g_rc->loaded_textures.push(texture);

	return texture;
}

Material load_material(Arena *arena, Scene &scene, ufbx_material *umat)
{
	Material mat = {};

	if (umat->fbx.diffuse_color.texture_enabled)
		mat.diffuse = load_texture(arena, scene, umat->fbx.diffuse_color.texture);
	if (umat->fbx.specular_color.texture_enabled)
		mat.specular = load_texture(arena, scene, umat->fbx.specular_color.texture);
	if (umat->fbx.normal_map.texture_enabled)
		mat.normal_map = load_texture(arena, scene, umat->fbx.normal_map.texture, false);
	else if (umat->fbx.bump.texture_enabled)
		mat.normal_map = load_texture(arena, scene, umat->fbx.bump.texture, false);

	if (umat->fbx.specular_exponent.texture_enabled)
		mat.specular_exponent = load_texture(arena, scene, umat->fbx.specular_exponent.texture, false);

	mat.diffuse_factor = umat->fbx.diffuse_factor.has_value ? umat->fbx.diffuse_factor.value_real : 1;
	mat.specular_factor = umat->fbx.specular_factor.has_value ? umat->fbx.specular_factor.value_real : 1;
	mat.specular_exponent_factor = umat->fbx.specular_exponent.has_value ? umat->fbx.specular_exponent.value_real : 20;	
	return mat;
}

Vertex get_ufbx_vertex(ufbx_mesh *umesh, ufbx_skin_deformer *skin, int index)
{
	Vertex v = {};

	v.position = ufbx_to_v3(ufbx_get_vertex_vec3(&umesh->vertex_position, index));
	v.normal = ufbx_to_v3(ufbx_get_vertex_vec3(&umesh->vertex_normal, index));
	if (umesh->vertex_uv.exists)
		v.uv = ufbx_to_v2(ufbx_get_vertex_vec2(&umesh->vertex_uv, index));

	if (skin) {
		uint32_t vertex = umesh->vertex_indices.data[index];

		ufbx_skin_vertex skin_vertex = skin->vertices.data[vertex];
		size_t num_weights = skin_vertex.num_weights;
		if (num_weights > MAX_BONE_WEIGHTS)
			num_weights = MAX_BONE_WEIGHTS;

		float total_weight = 0.0f;
		for (size_t i = 0; i < num_weights; i++) {
			ufbx_skin_weight skin_weight = skin->weights.data[skin_vertex.weight_begin + i];
			v.indices[i] = skin_weight.cluster_index;// |= skin_weight.cluster_index << (8 * i);
			v.weights[i] = (float)skin_weight.weight;
			total_weight += (float)skin_weight.weight;
		}

		for (size_t i = 0; i < num_weights; i++)
			v.weights[i] /= total_weight;
	}
	return v;
}

void make_bones_transform_relative_to_parent(Array<Bone> bones, usize index, Array<bool> computed)
{
	if (computed[index] || bones[index].parent == -1)
		return ;
	make_bones_transform_relative_to_parent(bones, bones[index].parent, computed);
	mat4 M = identity();
	int i = bones[index].parent;
	while (i != -1)
	{
		M = bones[i].transform * M;
		i = bones[i].parent;
	}
	bones[index].transform = inverse(M) *  bones[index].transform;
	computed[index] = true;
}

Mesh load_mesh(Arena *arena, Scene &scene, ufbx_node *unode)
{
	ufbx_mesh *umesh = unode->mesh;
	
	Mesh mesh = {};

	mesh.name = make_string(arena, unode->name.length, unode->name.data);
	mesh.parts = make_array<MeshPart>(arena, umesh->material_parts.count);

	Arena *tmp_arena = begin_temp_memory();

	Array<uint32_t> tri_indices = make_array<uint32_t>(tmp_arena, umesh->max_face_triangles * 3);

	Array<Vertex> vertices = make_array_max<Vertex>(tmp_arena, umesh->num_triangles * 3);
	Array<uint32_t> indices = make_array_max<uint32_t>(tmp_arena, umesh->num_triangles * 3);

	ufbx_skin_deformer *skin = 0;
	
	if (umesh->skin_deformers.count) {
		assert(umesh->skin_deformers.count == 1);
		skin = umesh->skin_deformers.data[0];
		assert(skin->skinning_method == UFBX_SKINNING_METHOD_LINEAR);
		//printf("%s -> bone count: %zd\n", unode->name.data, skin->clusters.count);

		mesh.bones = make_array<Bone>(arena, skin->clusters.count);
		for (usize i = 0; i < mesh.bones.count; i++) {
			ufbx_skin_cluster *cluster = skin->clusters.data[i];

			mesh.bones[i].inv_bind = ufbx_to_mat4(cluster->geometry_to_bone);
			mesh.bones[i].transform = inverse(mesh.bones[i].inv_bind);
			mesh.bones[i].parent = -1;
			mesh.bones[i].name = make_string(arena, cluster->bone_node->name.length,
					cluster->bone_node->name.data);
		//	printf("%.*s: %f %f %f\n", str_format(mesh.bones[i].name), M.e[0][3], M.e[1][3], M.e[2][0]);
			//printf("\tbone name: %.*s\n", str_format(mesh.bones[i].name));
			for (int j = 0; j < mesh.bones.count; j++) {
				if (cluster->bone_node->parent == skin->clusters.data[j]->bone_node) {
					mesh.bones[i].parent = j;
					break ;
				}
			}
			/*
				TODO: if you load the swat.fbx
				he will have 2 meshes each with their set of bones
				except the root bone will point to another bone in the other mesh
				handle this
			*/
			if (mesh.bones[i].parent == -1)
				assert(cluster->bone_node->parent->bone == 0);
		}
		Array<bool> computed = make_zero_array<bool>(tmp_arena, mesh.bones.count);
		for (usize i = 0; i < mesh.bones.count; i++)
			make_bones_transform_relative_to_parent(mesh.bones, i, computed);
	}
	uint32_t max_used_index = 0;
	for (usize part_idx = 0; part_idx < umesh->material_parts.count; part_idx++) {
		MeshPart &part = mesh.parts[part_idx];
		part.offset = indices.count;

		ufbx_mesh_part upart = umesh->material_parts.data[part_idx];
		uint32_t num_vertices = 0;

		for (usize face_idx = 0; face_idx < upart.num_faces; face_idx++) {
			ufbx_face face = umesh->faces.data[upart.face_indices.data[face_idx]];
			uint32_t num_tris = ufbx_triangulate_face(tri_indices.data, tri_indices.count, umesh, face);

			for (size_t i = 0; i < num_tris * 3; i++) {
				max_used_index = max(max_used_index, tri_indices[i]);
				indices.push(tri_indices[i]);			
			}
		}
		if (upart.face_indices.count && umesh->face_material.count) {
			// TODO: make sure this always works
			part.material = load_material(arena, scene, 
					umesh->materials.data[umesh->face_material.data[upart.face_indices.data[0]]]);
		}

		part.indices_count = indices.count - part.offset;
	}

	for (uint32_t index = 0; index < max_used_index + 1; index++)
		vertices.push(get_ufbx_vertex(umesh, skin, index));

	mesh.vertex_buffer = create_vertex_buffer(VERTEX_BUFFER_IMMUTABLE,
			vertices.count * sizeof(Vertex), vertices.data);
	mesh.index_buffer = create_index_buffer(indices.count, indices.data);


	mesh.box_min = V3(1e18);
	mesh.box_max = V3(-1e18);
	for (usize i = 0; i < indices.count; i++) {
		mesh.box_min = min(mesh.box_min, vertices[(int)indices[i]].position);
		mesh.box_max = max(mesh.box_max, vertices[(int)indices[i]].position);
	}

	mesh.vertices_count = vertices.count;
	mesh.indices_count = indices.count;
	
	mesh.vertices = make_array<v3>(arena, vertices.count);
	

	for (usize i = 0; i < vertices.count; i++) {
		mesh.vertices[i] = vertices[i].position;
	}


	mesh.indices = clone_array(arena, indices);


	// printf("INDICES COUNT: %d, VERTICES COUNT: %d, %d %d %d\n",  indices.count,  vertices.count, umesh->num_vertices,
	// 	umesh->vertex_position.values.count, umesh->vertex_normal.values.count);
	//assert(vertices.count % 3 == 0);
	//for (usize i = 0; i < vertices.count; i += 3)
	//	scene.triangles.push(MeshTriangle{vertices[i].position, vertices[i + 1].position, vertices[i + 2].position});


	end_temp_memory();
	return mesh;
}

void *ufbx_arena_realloc(void *user, void *old_ptr, size_t old_size, size_t new_size)
{
	Arena *arena = (Arena *)user;

	void *data = arena_alloc(arena, new_size);

	if (old_size > new_size)
		old_size = new_size;
	if (old_size)
		memcpy(data, old_ptr, old_size);
	return data;
}

b32 quat_equal(quat a, quat b)
{
	const float eps = 1e-9;
	return	fabsf(a.x - b.x) < eps &&
		fabsf(a.y - b.y) < eps &&
		fabsf(a.z - b.z) < eps &&
		fabsf(a.w - b.w) < eps;
}

mat4 get_animated_node_transform(Animation &anim, NodeAnimation &node, float anim_time)
{
	int frame = anim_time / anim.frametime;
	if (frame >= anim.frame_count)
		frame = anim.frame_count - 1;

	int next_frame = frame + 1 == anim.frame_count ? frame : frame + 1;
	float t = (anim_time - frame * anim.frametime) / anim.frametime;

	v3 position = node.const_position;
	quat rotation = node.const_rotation;
	v3 s = node.const_scale;

	if (node.position.count)
		position = lerp(node.position[frame], node.position[next_frame], t);
	if (node.rotation.count)
		rotation = quat_lerp(node.rotation[frame], node.rotation[next_frame], t);
	if (node.scale.count)
		s = lerp(node.scale[frame], node.scale[next_frame], t);
	return translate(position) * quat_to_mat(rotation) * scale(s);	
}

void get_animated_node_transform(Animation &anim, NodeAnimation &node, float anim_time,
		v3 &position, v3 &s, quat &rotation)
{
	int frame = anim_time / anim.frametime;
	if (frame >= anim.frame_count)
		frame = anim.frame_count - 1;

	int next_frame = frame + 1 == anim.frame_count ? frame : frame + 1;
	float t = (anim_time - frame * anim.frametime) / anim.frametime;

	position = node.const_position;
	rotation = node.const_rotation;
	s = node.const_scale;

	if (node.position.count)
		position = lerp(node.position[frame], node.position[next_frame], t);
	if (node.rotation.count)
		rotation = quat_lerp(node.rotation[frame], node.rotation[next_frame], t);
	if (node.scale.count)
		s = lerp(node.scale[frame], node.scale[next_frame], t);
}

Animation load_animation(Arena *arena, ufbx_scene *uscene, ufbx_anim_stack *stack)
{
	Arena *temp = begin_temp_memory();

	Animation anim = {};

	anim.timebegin = stack->time_begin;
	anim.duration = stack->time_end - stack->time_begin;
	int target_framerate = 60;

	anim.frame_count = (u32)(anim.duration * target_framerate);
	if (anim.frame_count < 2)
		anim.frame_count = 2;
	anim.frametime = anim.duration / (anim.frame_count - 1);

	Array<v3> position = make_array<v3>(temp, anim.frame_count);
	Array<quat> rotation = make_array<quat>(temp, anim.frame_count);
	Array<v3> scale = make_array<v3>(temp, anim.frame_count);

	anim.nodes = make_array_max<NodeAnimation>(arena, uscene->nodes.count);
	for (usize j = 0; j < uscene->nodes.count; j++) {
		ufbx_node *node = uscene->nodes.data[j];
		if (node->is_root)
			continue ;
		NodeAnimation node_anim = {};
		node_anim.name = make_string(arena, (usize)node->name.length, (char *)node->name.data);
		//printf("\tanim node name: %.*s\n", str_format(node_anim.name));
		b32 const_position = true;
		b32 const_scale = true;
		b32 const_rotation = true;

		for (usize frame = 0; frame < anim.frame_count; frame++) {
			float t = anim.timebegin + frame * anim.frametime;
			ufbx_transform transform = ufbx_evaluate_transform(stack->anim, node, t);

			position[frame] = ufbx_to_v3(transform.translation);
			rotation[frame] = ufbx_to_quat(transform.rotation);
			scale[frame] = ufbx_to_v3(transform.scale);

			if (frame) {
				// TODO: this is done in lerp anyway
				// Negated quaternions are equivalent, but interpolating between ones of different
				// polarity takes a the longer path, so flip the quaternion if necessary.
				if (dot(rotation[frame], rotation[frame - 1]) < 0.0f)
					rotation[frame] = -rotation[frame];

				if (!v3_equal(position[frame - 1], position[frame]))
					const_position = false;
				if (!quat_equal(rotation[frame - 1], rotation[frame]))
					const_rotation = false;
				if (!v3_equal(scale[frame - 1], scale[frame]))
					const_scale = false;
			}

		}
		//printf("\tP: %d, R: %d, S: %d\n", const_position ? 1 : 0,
		//	const_rotation ? 1 : 0, const_scale ? 1 : 0);

		if (const_position)
			node_anim.const_position = position[0];
		else
			node_anim.position = clone_array(arena, position);
		if (const_rotation)
			node_anim.const_rotation = rotation[0];
		else
			node_anim.rotation = clone_array(arena, rotation);
		if (const_scale)
			node_anim.const_scale = scale[0];
		else
			node_anim.scale = clone_array(arena, scale);

		anim.nodes.push(node_anim);
	}

	end_temp_memory();

	return anim;
}

Animation load_animation(Arena *arena, Game &game, const char *filename)
{
	Scene scene = {};

	Arena *temp = begin_temp_memory();

	ufbx_load_opts opts = {};

	opts.target_axes.right = UFBX_COORDINATE_AXIS_POSITIVE_X;
	opts.target_axes.up = UFBX_COORDINATE_AXIS_POSITIVE_Z;
	opts.target_axes.front = UFBX_COORDINATE_AXIS_POSITIVE_Y;
	opts.target_unit_meters = 1;
	opts.temp_allocator.allocator.realloc_fn = ufbx_arena_realloc;
	opts.temp_allocator.allocator.user = temp;
	opts.result_allocator.allocator.realloc_fn = ufbx_arena_realloc;
	opts.result_allocator.allocator.user = temp;

	ufbx_error error;
	ufbx_scene *uscene = ufbx_load_file(filename, &opts, &error);
	if (!uscene) {
		fprintf(stderr, "Failed to load animation %s: %s\n", filename, error.description.data);
		exit(1);
	}
	assert(uscene->anim_stacks.count);
	return load_animation(arena, uscene, uscene->anim_stacks.data[0]);
}

Scene *load_scene(Arena *arena, Game &game, const char *filename)
{
	Scene scene = {};

	Arena *temp = begin_temp_memory();
	{
		int slash = -1;

		for (int i = 0; filename[i]; i++) {
			if (filename[i] == '/' || filename[i] == '\\')
				slash = i;
		}
		// TODO: cleanup
		int len = strlen(filename);
		scene.path = make_string(arena, slash + 1, filename);
		scene.name = make_string(arena, len - (slash + 1), filename + slash + 1);
	}

	ufbx_load_opts opts = {};

	opts.target_axes.right = UFBX_COORDINATE_AXIS_POSITIVE_X;
	opts.target_axes.up = UFBX_COORDINATE_AXIS_POSITIVE_Z;
	opts.target_axes.front = UFBX_COORDINATE_AXIS_POSITIVE_Y;
	opts.target_unit_meters = 1;
	//TODO: this needs to use thread local storage
	opts.temp_allocator.allocator.realloc_fn = ufbx_arena_realloc;
	opts.temp_allocator.allocator.user = temp;
	opts.result_allocator.allocator.realloc_fn = ufbx_arena_realloc;
	opts.result_allocator.allocator.user = temp;
	opts.generate_missing_normals = true;
	opts.load_external_files = true;
	//opts.obj_merge_objects = true;
	//TODO: loading the car parking model with this option crashes the gpu driver
	//opts.obj_merge_groups = true;

	ufbx_error error;
	ufbx_scene *uscene = ufbx_load_file(filename, &opts, &error);
	if (!uscene) {
		fprintf(stderr, "Failed to load %s: %s\n", filename, error.description.data);
		exit(1);
	}


	usize total_num_triangles = 0;
	for (size_t i = 0; i < uscene->nodes.count; i++)
		if (uscene->nodes.data[i]->mesh)
			total_num_triangles += uscene->nodes.data[i]->mesh->num_triangles;
	
	printf("loading scene %s (%zd meshes, %zd triangles)\n", filename, uscene->meshes.count, total_num_triangles);

	mat4 root_transform = ufbx_to_mat4(uscene->root_node->node_to_parent);

	scene.meshes = make_array_max<Mesh>(arena, uscene->meshes.count);

	for (usize i = 0; i < uscene->nodes.count; i++) {
		if (uscene->nodes.data[i]->mesh) {
			Mesh mesh = load_mesh(arena, scene, uscene->nodes.data[i]);
			mesh.transform = ufbx_to_mat4(uscene->nodes.data[i]->geometry_to_world);

			double center[3] = {};
			for (usize j = 0; j < mesh.vertices.count; j++)
				for (int k = 0; k < 3; k++)
					center[k] += mesh.vertices[j].e[k];
			for (int k = 0; k < 3; k++)
				center[k] /= mesh.vertices.count;

			mesh.default_transform = root_transform * translate(-V3(center[0], center[1], center[2]));

			scene.meshes.push(mesh);
		}
	}

	end_temp_memory();
	scene.id = ++game.next_scene_id;
	game.scenes.push(scene);
	return &game.scenes[game.scenes.count - 1];
}
