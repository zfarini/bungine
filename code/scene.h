#pragma once

#define MAX_BONE_WEIGHTS 4

typedef usize SceneID;

// TODO: send indices/weights in another buffer?
struct MeshVertex {
    v3 position;
    v3 normal;
    v2 uv;
    float weights[MAX_BONE_WEIGHTS];
    float indices[MAX_BONE_WEIGHTS];
};

struct Material {
    TextureID diffuse;
    TextureID normal_map;
    TextureID specular;
    TextureID ambient;
    TextureID specular_exponent;

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

    Array<MeshVertex> full_vertices;
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
    String name;

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

enum SceneState {
    SCENE_UNLOADED,
    SCENE_LOADING,
    SCENE_LOADED
};

struct Scene {
	meta(ui: const) SceneID id;
    meta(ui) String path;
	meta(ui) String name;
	Array<Mesh> meshes;

    String filename;

    int state;
    b32 in_gpu;
};
