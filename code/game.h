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

    meta(ui) bool walk_backward;
    meta(ui) bool jumped;

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
	bool should_jump;
};

// enum AnimationType {
//     ANIMATION_JUMP,
//     ANIMATION_SHOOT,
//     ANIMATION_RUN,
//     ANIMATION_FORWARD_GUN_WALK,
//     ANIMATION_BACKWARD_GUN_WALK,
//     ANIMATION_GUN_IDLE,
//     ANIMATION_COUNT
// };

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

    CollisionMesh cmesh;
    SceneID cmesh_scene;
};

struct World {
	std::unordered_map<uint64_t, bool> occupied;

    Arena arena;

    meta(ui) Editor editor;

    meta(ui, serialize) Array<Entity> entities;

    std::unordered_map<entity_id, usize> entities_id_map;

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

struct Game {
    Arena arena;

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

    std::unordered_map<String, int, StringHasher> animations;
    Array<Animation> loaded_animations;

	std::unordered_map<SceneID, CollisionMesh> collision_meshes;

	Scene default_scene;

    meta(ui) bool show_normals;
    meta(ui) bool render_bones;
    meta(ui) bool frustum_culling;
    meta(ui) float master_volume;

	bool play_in_editor;
    meta(ui: color) v3 background_color;

};

// TODO: cleanup
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

Entity *get_entity(World &world, entity_id id);
mat4 get_entity_transform(World &world, Entity &e);
v3 get_world_p(World &world, entity_id id);