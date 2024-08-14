#pragma once

#define WORLD_UP V3(0, 0, 1)

struct CollisionMesh {
	meta(serialize) Array<v3> vertices;
	meta(serialize) SceneID scene;
};

struct CollisionShape
{
	CollisionMesh collision_mesh;
	v3 ellipsoid_radius;
	bool ellipsoid;

	mat4 transform;
	v3 scale;
};

struct CollisionInfo
{
	v3 hit_p;
	v3 hit_normal;
	float t;
};

enum EntityType {
    EntityType_Player,
    EntityType_Enemy,
    EntityType_Static,
    EntityType_Projectile,
	EntityType_PointLight,
    EntityType_Count,
};

typedef usize entity_id;

struct Entity {
    meta(ui: const, serialize) entity_id id;

	entity_id parent;

    meta(ui, serialize) EntityType type;
    meta(ui, serialize) v3 position; // relative position
    meta(ui, serialize) v3 dp;

    meta(ui, serialize) quat rotation;
    meta(ui, serialize) v3 scale;

    meta(ui: color, serialize) v3 color;
    
    meta(ui, serialize) b32 moved;
    meta(ui, serialize) b32 run;
    meta(ui, serialize) b32 shooting;
    meta(ui, serialize) b32 can_jump;
    meta(ui, serialize) b32 on_ground;
    meta(ui, serialize) b32 pressing_jump;
    meta(ui, serialize) b32 aiming;

	meta(ui, serialize) bool ellipsoid_collision_shape;
	meta(ui, serialize) v3 ellipsoid_radius;

    meta(ui, serialize) SceneID scene_id;
	// TODO: get rid of scene_transform at some point
    meta(ui, serialize) mat4 scene_transform;

	meta(ui, serialize) bool disable_collision;

    // Animation *animation;
    Animation *curr_anim;
    Animation *next_anim;
    float anim_time;
    float blend_time;

    Animation *animation;

    float speed;

    meta(ui: const, serialize) float height_above_ground;
    
	meta(ui, serialize) float point_light_scale;

    float z_rot;
    int last_move;

    float last_gun_time;
	float last_jump_z;
};

enum AnimationType {
    ANIMATION_JUMP,
    ANIMATION_SHOOT,
    ANIMATION_RUN,
    ANIMATION_FORWARD_GUN_WALK,
    ANIMATION_BACKWARD_GUN_WALK,
    ANIMATION_GUN_IDLE,
    ANIMATION_COUNT
};

struct ShadowMap {
    FrameBuffer framebuffer;
    v3 light_dir;
    v3 light_p;
    mat4 view;
    mat4 projection;
    int tex_width, tex_height;

    float znear, zfar;
    float width, height;

    Texture depth_texture;
};

enum CameraType {
    CAMERA_TYPE_PERSPECTIVE,
    CAMERA_TYPE_ORTHOGRAPHIC,
};

struct Camera {
    CameraType type;

    v3 position;
    mat4 view;
    mat4 projection;

    float znear, zfar, width, height;
    v3 forward, right, up;
};

enum EditorMode {
	EDITOR_MODE_GIZMO,
	EDITOR_MODE_COLLISION_MESH,
};

enum EditorOpType {
    EDITOR_OP_TRANSLATE_ENTITY,
    EDITOR_OP_ROTATE_ENTITY,
    EDITOR_OP_SCALE_ENTITY,
    EDITOR_OP_PASTE_ENTITY,
    EDITOR_OP_DELETE_ENTITY,
    EDITOR_OP_SPAWN_ENTITY,

	EDITOR_OP_CREATE_MESH_COLLISION_VERTEX,
	EDITOR_OP_DELETE_MESH_COLLISION_TRIANGLE,
};

struct EditorOp {
    int type;
    entity_id entity;

    union {
        struct {
            v3 prev_p;
            v3 new_p;
        } translate;
        struct {
            quat prev_rot;
            quat new_rot;
        } rotate;
        struct {
            v3 prev_scale;
            v3 new_scale;
        } scale;
        struct {
            entity_id copy_from;
            entity_id id;
			int mesh_index;
            v3 p;
        } paste;
        struct {
            Entity entity_data;
        } del;
		struct {
			v3 pos;
		} place_collision_vertex;
		struct {
			int index;
			v3 v0, v1, v2;
		} delete_collision_triangle;
    };
};

enum GizmoMode {
	GIZMO_TRANSLATION,
	GIZMO_ROTATION,
	GIZMO_SCALE
};

struct Gizmo {
	GizmoMode mode;
	v3 init_position;
	quat init_rotation;
	v3 init_scale;

	bool active;

    int dragging_axis;
	bool did_drag;
	bool uniform_scale;

	// data for the first frame we start dragging
	v3 hit_p;

	v3 delta_p;

	v3 scale;
	float delta_s;

	float rotation_angle;
	quat rotation;
	v3 rotation_right_axis;
	v3 rotation_up_axis;
	v3 rotation_axis;
};

struct Editor {
	meta(ui) EditorMode mode;

    Array<EditorOp> ops;
    Array<EditorOp> undos;

	Gizmo gizmo;

	meta(ui) bool copy_entity_mesh;

    meta(ui) entity_id selected_entity;
	meta(ui) int selected_entity_mesh;

    meta(ui) entity_id copied_entity;

    meta(ui) v3 last_camera_p;
};

struct World {
	std::unordered_map<uint64_t, bool> occupied;

    Arena arena;

    meta(ui) Editor editor;

    meta(ui, serialize) Array<Entity> entities;

    std::unordered_map<entity_id, usize> entities_id_map;
	std::unordered_map<SceneID, int> scene_collision_mesh;

    meta(serialize) entity_id next_entity_id;

    meta(serialize) v3 player_camera_p;
    meta(serialize) v3 player_camera_rotation;
    meta(serialize) v3 player_camera_drotation;

    meta(serialize) v3 editor_camera_p;
    meta(serialize) v3 editor_camera_rotation;

    meta(serialize) entity_id player_id;

    entity_id moving_box;
    // TODO: remove this
    Camera last_game_camera;

    float aim_camera_transition_t;

	meta(serialize) Array<CollisionMesh> collision_meshes;
};

struct LoadedSound
{
    int sample_count;
    float *samples;
};

struct SoundPlaying
{
    entity_id entity;
    int samples_played;
    LoadedSound *sound;
    SoundPlaying *next;
    SoundPlaying *prev;
};

struct SoundState
{
	float *buffer;
    int sample_count;

	std::atomic_int32_t read_index;
	std::atomic_int32_t write_index;
};

struct ProfilerBlockStat
{
	const char *name;
	uint64_t cycle_count;
	uint32_t call_count;
};

global ProfilerBlockStat profiler_block_stats[512];

#if 1
struct ProfilerBlock
{
	uint64_t start_cycle;
	uint32_t id;
	const char *name;

	ProfilerBlock(const char *name, uint32_t id) : name(name), id(id)
	{
		profiler_block_stats[id].name = name;
		start_cycle = rdtsc();
	}

	~ProfilerBlock()
	{
		uint64_t cycle_count = rdtsc() - start_cycle;

		assert(id < ARRAY_SIZE(profiler_block_stats));
		profiler_block_stats[id].cycle_count += cycle_count;
		profiler_block_stats[id].call_count++;
	}
};

#define PROFILE_BLOCK(name) ProfilerBlock _block_##__LINE__(name, __COUNTER__);
#define PROFILE_FUNCTION(name) ProfilerBlock _block_##__LINE__(__FUNCTION__, __COUNTER__);
#endif

struct Game {

    Arena *memory;

	// TODO: this get used in sound right now remove atomic later
    std::atomic_bool is_initialized;

    World *world;

    ShadowMap shadow_map;

    RenderPass mesh_render_pass;
    RenderPass shadow_map_render_pass;
    RenderPass outline_render_pass;
    ConstantBuffer constant_buffer;

    RenderPass debug_lines_render_pass;
    VertexBuffer debug_lines_vertex_buffer;
    ConstantBuffer debug_lines_constant_buffer;

    Arena asset_arena;

	meta(ui) Array<Scene> scenes;
	usize next_scene_id;

    Animation animations[ANIMATION_COUNT];

    b32 in_editor;
    meta(ui) bool debug_collision;

    meta(ui) int frame;
    meta(ui) float time;

    RasterizerState default_rasterizer_state, wireframe_rasterizer_state;
    DepthStencilState default_depth_stencil_state;
    DepthStencilState disable_depth_state;

    FrameBuffer debug_asset_fb;
    Texture debug_asset_tex;

    SoundState sound_state;
    
    SoundPlaying *first_playing_sound;
    LoadedSound loaded_sounds[32];

	Scene default_scene;

    meta(ui) bool show_normals;
    meta(ui) bool render_bones;
    meta(ui) bool frustum_culling;
    meta(ui) float master_volume;

	bool play_in_editor;
};

struct Constants {
    mat4 view;
    mat4 projection;
    mat4 model;
    mat4 light_transform;
    mat4 bones[96];

	v4 point_light_color[8];
	v4 point_light_position[8];
	int point_light_count;

    v3 camera_p;
    v3 player_p;
    v3 color;
    float diffuse_factor;
    float specular_factor;
    float specular_exponent_factor;
    int skinned;
    int has_normal_map;
    int show_normals;
};

#define ASTART_CELL_DIM (0.5f)

v3i get_cell(v3 p)
{
	v3i res;

	res.x = roundf(p.x / (ASTART_CELL_DIM));
	res.y = roundf(p.y / (ASTART_CELL_DIM));
	res.z = roundf(p.z / (ASTART_CELL_DIM));
	return res;
}

const int MAX_CELL_POW = 10;
const int MAX_CELL = (1 << MAX_CELL_POW);

uint64_t pack_cell(v3i c)
{
	assert(abs(c.x) < MAX_CELL/2 && abs(c.y) < MAX_CELL/2 && abs(c.z) < MAX_CELL/2);
	return ((uint64_t)(c.x + MAX_CELL/2)) | ((c.y + MAX_CELL/2) << MAX_CELL_POW) | ((c.z+ MAX_CELL/2) << (MAX_CELL_POW*2));
}
v3i unpack_cell(uint64_t x)
{
	v3i res;

	res.x = (x >> 0) & (MAX_CELL - 1);
	res.y = (x >> MAX_CELL_POW) & (MAX_CELL - 1);
	res.z = (x >> (MAX_CELL_POW * 2)) & (MAX_CELL - 1);
	return res - V3i(MAX_CELL/2, MAX_CELL/2, MAX_CELL/2);
};

v3 get_closest_point_in_cell(v3i cell, v3 p)
{
	v3 box_min = V3(cell) * ASTART_CELL_DIM - 0.5f * V3(ASTART_CELL_DIM);
	v3 box_max = V3(cell) * ASTART_CELL_DIM + 0.5f * V3(ASTART_CELL_DIM);

	v3 result;
	result.x = (p.x >= box_min.x && p.x <= box_max.x ? p.x : 
			   (p.x <= box_min.x ? box_min.x : box_max.x));
	result.y = (p.y >= box_min.y && p.y <= box_max.y ? p.y : 
			   (p.y <= box_min.y ? box_min.y : box_max.y));
	result.z = (p.z >= box_min.z && p.z <= box_max.z ? p.z : 
			   (p.z <= box_min.z ? box_min.z : box_max.z));
	return result;
}

void render_cell(v3i x, float s = 1, v3 color = V3(1, 1, 0))
{
	push_cube_outline(V3(x) * ASTART_CELL_DIM, V3(ASTART_CELL_DIM*0.5f * s), color);
}

struct State
{
	v3i p;
	// -1 => can't jump unless i'm on the ground
	// 0 
	int jump;
	//bool operator<(State& rhs) const 
	//{
	//   return pack_cell(p) < pack_cell(rhs.p);
	//}
	bool operator==(const State &rhs) const
	{
		return p.x == rhs.p.x && p.y == rhs.p.y && p.z == rhs.p.z && jump == rhs.jump;
	}
};

struct StateHasher
{
  std::size_t operator()(const State& k) const
  {
	  return pack_cell(k.p) | ((uint64_t)(k.jump + 1) << (3 * MAX_CELL_POW));
  }
};

