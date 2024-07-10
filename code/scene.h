#define MAX_BONE_WEIGHTS 4

// TODO: send indices/weights in another buffer?
struct Vertex
{
	v3 position;
	v3 normal;
	v2 uv;
	int indices[MAX_BONE_WEIGHTS];
	float weights[MAX_BONE_WEIGHTS];
};

struct Material
{
	Texture diffuse;
	Texture normal_map;
	Texture specular;
	Texture ambient;
	Texture specular_exponent;

	float diffuse_factor;
	float specular_factor;
	float specular_exponent_factor;
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
	mat4 inv_bind;
	int parent;
};

struct Mesh
{
	String name;
	Array<MeshPart> parts;
	ID3D11Buffer *vertex_buffer;
	Array<Bone> bones;
};

struct NodeAnimation
{
	String name;
	Array<v3> position;
	Array<v3> scale;
	Array<quat> rotation;

	v3 const_position;
	v3 const_scale;
	quat const_rotation;
};

struct Animation
{
	float timebegin;
	float duration;

	float frametime;

	int frame_count;

	Array<NodeAnimation> nodes;
};

struct SceneNode
{
	String name;
	SceneNode *parent;
	Array<SceneNode *> childs;
	Mesh *mesh;
	mat4 local_transform;
	mat4 geometry_transform;
};

struct Scene
{
	Array<SceneNode> nodes;
	Array<Mesh> meshes;

	SceneNode *root;
	Array<Animation> animations;
	String path;
};