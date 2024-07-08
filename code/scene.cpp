#define MAX_BONE_WEIGHTS 4

// TODO: send indices/weights in another buffer?
struct Vertex
{
	v3 position;
	v3 normal;
	v2 uv;
	u32 indices[MAX_BONE_WEIGHTS];
	float weights[MAX_BONE_WEIGHTS];
};

struct Texture
{
	ID3D11Texture2D *data;
};

struct Material
{
	Texture diffuse;
	Texture normal_map;
	Texture specular;
	Texture ambient;
};

struct MeshPart
{
	Material material;
	usize vertices_count;
	usize offset;
};

struct Bone
{
	String name;	
	mat4 transform;
};

struct Mesh
{
	DArray<MeshPart> parts;
	ID3D11Buffer *vertex_buffer;

	DArray<Bone> bones;
};

struct Animation
{
	float timebegin;
	float framerate;

	u32 frame_count;

	v3 *position;
	v3 *scale;
	quat *rotation;

	DArray<String> bone_names; 
};

struct Scene
{
	DArray<Mesh> meshes;
	DArray<Animation> animations;
};

v3 ufbx_vec3_to_v3(ufbx_vec3 v)
{
	return v3{(float)v.x, (float)v.y, (float)v.z};
}

v2 ufbx_vec2_to_v2(ufbx_vec2 v)
{
	return v2{(float)v.x, (float)v.y};
}

ID3D11Buffer *create_vertex_buffer(usize size, void *data = 0)
{
	ID3D11Buffer *buffer;

	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = (UINT)size;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	if (data) {
		D3D11_SUBRESOURCE_DATA initial = {};
		initial.pSysMem = data;
		g_device->CreateBuffer(&desc, &initial, &buffer);
	}
	else
		g_device->CreateBuffer(&desc, 0, &buffer);
	return buffer;
}

Mesh load_mesh(ufbx_mesh *umesh)
{
	Mesh mesh = {};

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

	mesh.vertex_buffer = create_vertex_buffer(vertices_count * sizeof(Vertex), vertices.data);

	return mesh;
}

Scene load_scene(const char *filename)
{
	Scene scene = {};

	ufbx_load_opts opts = {};
	ufbx_error error;
	ufbx_scene *uscene = ufbx_load_file(filename, &opts, &error);
	if (!uscene) {
		fprintf(stderr, "Failed to load %s: %s\n", filename, error.description.data);
		exit(1);
	}

	scene.meshes = make_darray<Mesh>(uscene->meshes.count);

	for (usize i = 0; i < uscene->meshes.count; i++) {
		scene.meshes[i] = load_mesh(uscene->meshes.data[i]);
	}

	ufbx_free_scene(uscene);
	return scene;
}