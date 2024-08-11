#pragma once

#define MAX_BONE_WEIGHTS 4

typedef usize SceneID;

// TODO: send indices/weights in another buffer?
struct Vertex {
    v3 position;
    v3 normal;
    v2 uv;
    // v4 weights;
	// TODO: cleanup, don't send these with non-skinned meshes & 
	// in the case of skinned meshes send only one int/float
    float weights[MAX_BONE_WEIGHTS];
    float indices[MAX_BONE_WEIGHTS]; //[MAX_BONE_WEIGHTS];
};

// TODO: cleanup
VertexInputElement g_vertex_input_elements[] = {
    {offsetof(Vertex, position), 3, INPUT_ELEMENT_FLOAT, "POSITION"},
    {offsetof(Vertex, normal), 3, INPUT_ELEMENT_FLOAT, "NORMAL"},
    {offsetof(Vertex, uv), 2, INPUT_ELEMENT_FLOAT, "TEXCOORD"},
    {offsetof(Vertex, weights), MAX_BONE_WEIGHTS, INPUT_ELEMENT_FLOAT,
     "BLENDWEIGHT"},
    {offsetof(Vertex, indices), MAX_BONE_WEIGHTS, INPUT_ELEMENT_FLOAT,
     "BLENDINDICES"},
};

struct Material {
    Texture diffuse;
    Texture normal_map;
    Texture specular;
    Texture ambient;
    Texture specular_exponent;

    float diffuse_factor;
    float specular_factor;
    float specular_exponent_factor;
};

struct MeshPart {
    Material material;
    usize indices_count;
    usize offset;
};

struct Bone {
    String name;
    mat4 transform;
    mat4 inv_bind;
    int parent;
};

struct Mesh {
    String name;
    Array<MeshPart> parts;
    
    VertexBuffer vertex_buffer;
    IndexBuffer index_buffer;

    Array<Bone> bones;
    v3 box_min;
    v3 box_max;
    mat4 transform;
	mat4 default_transform;

    usize vertices_count;
    usize indices_count;

	// used in hit testing for now
    Array<v3> vertices;
	Array<u32> indices;
};

struct NodeAnimation {
    String name;
    Array<v3> position;
    Array<v3> scale;
    Array<quat> rotation;

    v3 const_position;
    v3 const_scale;
    quat const_rotation;
    // TODO:!!! remove this
    mat4 transform;
};

struct Animation {
    float timebegin;
    float duration;

    float frametime;

    int frame_count;

    Array<NodeAnimation> nodes;
};

struct SceneNode {
    String name;
    SceneNode *parent;
    Array<SceneNode *> childs;
    Mesh *mesh;
    mat4 local_transform;
    mat4 geometry_transform;
    int id;
    b32 skip_render;
};

struct MeshTriangle {
    v3 v0, v1, v2;
};

struct Scene {
	meta(ui: const) SceneID id;
    meta(ui) String path;
	meta(ui) String name;
	Array<Mesh> meshes;
};
