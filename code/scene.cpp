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

Mesh load_mesh(RenderContext &rc, ufbx_node *unode)
{
	ufbx_mesh *umesh = unode->mesh;

	Mesh mesh = {};

	mesh.default_transform = ufbx_to_mat4(unode->geometry_to_world);
	mesh.parts = make_darray<MeshPart>(umesh->material_parts.count);

    DArray<uint32_t> tri_indices = make_darray<uint32_t>(umesh->max_face_triangles * 3);

	DArray<Vertex> vertices = make_darray<Vertex>(umesh->num_triangles * 3);
	usize vertices_count = 0;

	for (usize part_idx = 0; part_idx < umesh->material_parts.count; part_idx++) {
		MeshPart &part = mesh.parts[part_idx];
		part.offset = vertices_count;

		ufbx_mesh_part upart = umesh->material_parts.data[part_idx];
		uint32_t num_vertices = 0;

		for (usize face_idx = 0; face_idx < upart.num_faces; face_idx++) {
			ufbx_face face = umesh->faces.data[upart.face_indices.data[face_idx]];
			uint32_t num_tris = ufbx_triangulate_face(tri_indices.data, tri_indices.count, umesh, face);
			
			for (size_t i = 0; i < num_tris * 3; i++) {
            	uint32_t index = tri_indices[i];

            	Vertex &v = vertices[vertices_count++];

            	v.position = ufbx_vec3_to_v3(ufbx_get_vertex_vec3(&umesh->vertex_position, index));
            	v.normal = ufbx_vec3_to_v3(ufbx_get_vertex_vec3(&umesh->vertex_normal, index));
            	v.uv = ufbx_vec2_to_v2(ufbx_get_vertex_vec2(&umesh->vertex_uv, index));
        	}
		}

		part.vertices_count = vertices_count - part.offset;
	}

	mesh.vertex_buffer = create_vertex_buffer(rc, vertices_count * sizeof(Vertex), vertices.data);

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

	scene.meshes = make_darray<Mesh>(uscene->meshes.count);
	int mesh_idx = 0;

	for (size_t i = 0; i < uscene->nodes.count; i++) {
		ufbx_node *unode = uscene->nodes.data[i];
		if (unode->is_root) continue;

		if (unode->mesh) {
			scene.meshes[mesh_idx++] = load_mesh(rc, unode);
		}
	}

	ufbx_free_scene(uscene);
	return scene;
}