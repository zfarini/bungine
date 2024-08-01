#pragma once

enum EntityType
{
	EntityType_Player,
	EntityType_Enemy,
	EntityType_Static
};

enum CollisionShapeType
{
	COLLISION_SHAPE_TRIANGLES,
	COLLISION_SHAPE_ELLIPSOID,
};

struct CollisionTriangle
{
	v3 v0, v1, v2;
};

struct CollisionShape
{
	int type;
	Array<CollisionTriangle> triangles;
	v3 ellipsoid_radius;
	// TODO: both of these are bad
	v3 box_radius;
	mat4 transform;
	v3 scale;
};

typedef usize entity_id;
typedef usize SceneID;


struct Entity
{
	entity_id id;
	int type;
	v3 position; // world position
	v3 dp;

	quat rotation;
	v3 scale;

	v3 color;

	b32 moved;
	b32 run;
	b32 shooting;
	b32 can_jump;
	b32 on_ground;

	CollisionShape shape;

	SceneID scene_id;
	mat4 scene_transform;

	//Animation *animation;

	Animation *curr_anim;
	Animation *next_anim;
	float anim_time;
	float blend_time;

	Animation *animation;

	float speed;

	float height_above_ground;
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
	int width, height;
	Texture depth_texture;
};

struct Camera
{
	v3 position;
	mat4 view;
	mat4 projection;

	float znear, zfar, width, height;
	v3 forward, right, up;
};

enum GizmoMode
{
	GIZMO_TRANSLATION,
	GIZMO_SCALE,
	GIZMO_ROTATION
};

enum EditorOpType
{
	EDITOR_OP_TRANSLATE_ENTITY,
	EDITOR_OP_ROTATE_ENTITY,
	EDITOR_OP_SCALE_ENTITY,
	EDITOR_OP_PASTE_ENTITY,
	EDITOR_OP_DELETE_ENTITY,
	EDITOR_OP_SPAWN_ENTITY,
};

struct EditorOp
{
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
			v3 p;
		} paste;
		struct {
			Entity entity_data;
		} del;
	};
};

struct Editor
{
	Array<EditorOp> ops;
	Array<EditorOp> undos;

	Entity init_entity;

	bool in_gizmo;
	entity_id selected_entity;
	entity_id copied_entity;

	GizmoMode gizmo_mode;

	int dragging_axis;
	bool did_drag;

	v3 p_init_drag;

	float s_init_scale;
	float s_init_drag;

	quat r_init_rot;
	float r_init_drag;
	v3 r_right_axis;
	v3 r_up_axis;
	v3 r_axis;

	v3 last_camera_p;
};

struct World
{
	Arena arena;

	Editor editor;

	Array<Entity> entities;

	std::unordered_map<entity_id, usize> entities_id_map;

	entity_id next_entity_id;

	v3 player_camera_p;
	v3 player_camera_rotation;
	v3 player_camera_drotation;

	v3 editor_camera_p;
	v3 editor_camera_rotation;

	entity_id editor_selected_entity;
	entity_id player_id;
};

struct Game
{
	b32 is_initialized;

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

	SceneID ch43, sponza, cube_asset, sphere_asset;
	Scene scenes[16];

	Animation animations[ANIMATION_COUNT];

	b32 in_editor;
	bool debug_collision;

	int frame;
	float time;

	RasterizerState default_rasterizer_state;
	DepthStencilState default_depth_stencil_state;
	DepthStencilState disable_depth_state;

	FrameBuffer debug_asset_fb;
	Texture debug_asset_tex;

	bool show_normals;
	bool render_bones;
};

struct Constants
{
	mat4 view;
	mat4 projection;
	mat4 model;
	mat4 light_transform;
	mat4 bones[96];

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
