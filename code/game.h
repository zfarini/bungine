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
};

typedef usize entity_id;

struct Entity
{
	entity_id id;
	int type;
	v3 position; // world position
	v3 dp;

	v3 rotation;
	v3 scale;

	v3 color;

	b32 moved;
	b32 run;
	b32 shooting;
	b32 can_jump;
	b32 on_ground;

	CollisionShape shape;

	Scene *scene;
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


// TODO: replace this
#include <unordered_map>

enum GizmoMode
{
	GIZMO_TRANSLATION,
	GIZMO_SCALE,
	GIZMO_ROTATION
};

struct Editor
{
	bool in_gizmo;
	entity_id selected_entity;

	GizmoMode gizmo_mode;

	int dragging_axis;
	bool did_drag;
	v3 drag_p;


	int s_drag_axis;
	bool s_did_drag;
	float s_init_scale;
	float s_init_drag;

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

	//v3 editor_camera_last_p;

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
	ConstantBuffer constant_buffer;

	RenderPass debug_lines_render_pass;
	VertexBuffer debug_lines_vertex_buffer;
	ConstantBuffer debug_lines_constant_buffer;

	Arena asset_arena;
	Scene ch43, sponza, cube_asset, sphere_asset;

	Animation animations[ANIMATION_COUNT];

	b32 in_editor;
	bool debug_collision;

	int frame;
	float time;

	RasterizerState default_rasterizer_state;
	DepthStencilState default_depth_stencil_state;
	DepthStencilState disable_depth_state;

	bool show_normals;
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
