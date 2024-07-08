#define MAX_BONE_WEIGHTS 4

// TODO: send indices/weights in another buffer?
struct Vertex
{
	v3 position;
	v3 normal;
	v2 uv;
	//u32 indices[MAX_BONE_WEIGHTS];
	//float weights[MAX_BONE_WEIGHTS];
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

	mat4 default_transform;
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