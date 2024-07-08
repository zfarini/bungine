#include "scene.h"

v3 ufbx_vec3_to_v3(ufbx_vec3 v)
{
	return v3{(float)v.x, (float)v.y, (float)v.z};
}

v2 ufbx_vec2_to_v2(ufbx_vec2 v)
{
	return v2{(float)v.x, (float)v.y};
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

Texture load_texture(RenderContext &rc, ufbx_texture *utex, bool srgb = true)
{
	assert(utex->type == UFBX_TEXTURE_FILE);

	void *data;
	int width, height, n_channels;

	stbi_set_flip_vertically_on_load(true);

	String name = StringDup(utex->filename.data);

	if (name.size) {
		for (int i = 0; i < rc.loaded_textures.count; i++) {
			if (rc.loaded_textures[i].name == name)
				return rc.loaded_textures[i];
		}
	}

	if (utex->content.size) {
		data = stbi_load_from_memory((stbi_uc *)utex->content.data,
				(int)utex->content.size, &width, &height,
				&n_channels, 4);
	} else {
		data = stbi_load(utex->filename.data, &width, &height, &n_channels, 4);
	}
	assert(data);

	Texture tex;
	tex.name = name;
	tex.data = create_texture(rc, data, width, height, srgb);

	if (name.size)
		rc.loaded_textures.push(tex);

	stbi_image_free(data);
	return tex;
}

Material load_material(RenderContext &rc, ufbx_material *umat)
{
	Material mat = {};

	//printf("Material: %s\n", umat->name.data);
	#if 0
	for (int tex_id = 0; tex_id < umat->textures.count; tex_id++)
	{
		ufbx_material_texture utex = umat->textures.data[tex_id];

		String name = String(utex.material_prop.data);

		if (name == String("DiffuseColor"))
			mat.diffuse = load_texture(rc, utex.texture);
		else if (name == String("NormalMap") || name == String("Bump"))
			mat.normal_map = load_texture(rc, utex.texture, false);
		else if (name == String("SpecularColor"))
			mat.specular = load_texture(rc, utex.texture);
		else if (name == String("ShininessExponent"))
			mat.specular_exponent = load_texture(rc, utex.texture, false); 
		//	printf("\t%.*s\n", str_format(name));
	}
	#else

	if (umat->fbx.diffuse_color.texture_enabled)
		mat.diffuse = load_texture(rc, umat->fbx.diffuse_color.texture);
	if (umat->fbx.specular_color.texture_enabled)
		mat.specular = load_texture(rc, umat->fbx.specular_color.texture);
	if (umat->fbx.normal_map.texture_enabled)
		mat.normal_map = load_texture(rc, umat->fbx.normal_map.texture, false);
	else if (umat->fbx.bump.texture_enabled)
		mat.normal_map = load_texture(rc, umat->fbx.bump.texture, false);

	if (umat->fbx.specular_exponent.texture_enabled)
		mat.specular_exponent = load_texture(rc, umat->fbx.specular_exponent.texture, false);

	mat.diffuse_factor = umat->fbx.diffuse_factor.has_value ? umat->fbx.diffuse_factor.value_real : 1;
	mat.specular_factor = umat->fbx.specular_factor.has_value ? umat->fbx.specular_factor.value_real : 1;
	mat.specular_exponent_factor = umat->fbx.specular_exponent.has_value ? umat->fbx.specular_exponent.value_real : 20;	
	#endif
	return mat;
}

Mesh load_mesh(RenderContext &rc, ufbx_node *unode)
{
	ufbx_mesh *umesh = unode->mesh;

	Mesh mesh = {};

	mesh.default_transform = ufbx_to_mat4(unode->geometry_to_world);
	mesh.parts = make_array<MeshPart>(umesh->material_parts.count);

    Array<uint32_t> tri_indices = make_array<uint32_t>(umesh->max_face_triangles * 3);

	Array<Vertex> vertices = make_array_max<Vertex>(umesh->num_triangles * 3);

	for (usize part_idx = 0; part_idx < umesh->material_parts.count; part_idx++) {
		MeshPart &part = mesh.parts[part_idx];
		part.offset = vertices.count;

		ufbx_mesh_part upart = umesh->material_parts.data[part_idx];
		uint32_t num_vertices = 0;

		for (usize face_idx = 0; face_idx < upart.num_faces; face_idx++) {
			ufbx_face face = umesh->faces.data[upart.face_indices.data[face_idx]];
			uint32_t num_tris = ufbx_triangulate_face(tri_indices.data, tri_indices.count, umesh, face);
			
			for (size_t i = 0; i < num_tris * 3; i++) {
            	uint32_t index = tri_indices[i];

            	Vertex v;

            	v.position = ufbx_vec3_to_v3(ufbx_get_vertex_vec3(&umesh->vertex_position, index));
            	v.normal = ufbx_vec3_to_v3(ufbx_get_vertex_vec3(&umesh->vertex_normal, index));
            	v.uv = ufbx_vec2_to_v2(ufbx_get_vertex_vec2(&umesh->vertex_uv, index));
	
				vertices.push(v);
			}
		}
		if (upart.face_indices.count && umesh->face_material.count) {
			// TODO: make sure this always works
			part.material = load_material(rc, 
					umesh->materials.data[umesh->face_material.data[upart.face_indices.data[0]]]);
		}

		part.vertices_count = vertices.count - part.offset;
	}

	mesh.vertex_buffer = create_vertex_buffer(rc, vertices.count * sizeof(Vertex), vertices.data);

	return mesh;
}

Scene load_scene(RenderContext &rc, const char *filename)
{
	Scene scene = {};

	ufbx_load_opts opts = {};

	opts.target_axes.right = UFBX_COORDINATE_AXIS_POSITIVE_X;
	opts.target_axes.up = UFBX_COORDINATE_AXIS_POSITIVE_Z;
	opts.target_axes.front = UFBX_COORDINATE_AXIS_POSITIVE_Y;
	opts.target_unit_meters = 1;
		
	ufbx_error error;
	ufbx_scene *uscene = ufbx_load_file(filename, &opts, &error);
	if (!uscene) {
		fprintf(stderr, "Failed to load %s: %s\n", filename, error.description.data);
		exit(1);
	}

	scene.meshes = make_array_max<Mesh>(uscene->meshes.count);

	for (size_t i = 0; i < uscene->nodes.count; i++) {
		ufbx_node *unode = uscene->nodes.data[i];
		if (unode->is_root)
			continue;
		if (unode->mesh)
			scene.meshes.push(load_mesh(rc, unode));
	}

	ufbx_free_scene(uscene);
	return scene;
}