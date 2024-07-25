#ifndef GAME_H
#define GAME_H

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
	v3 offset;
};

struct Entity
{
	int id;
	int type;
	v3 position; // world position
	v3 dp;
	v3 rotation;
	v3 drotation;

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
};

struct Game
{
	ShadowMap shadow_map;

	v3 camera_p;
	v3 camera_rotation;

	RenderPass mesh_render_pass;
	RenderPass shadow_map_render_pass;

	RenderPass debug_lines_render_pass;
	VertexBuffer debug_lines_vertex_buffer;
	ConstantBuffer debug_lines_constant_buffer;


	Scene ch43, sponza, cube_asset, sphere_asset;

	Array<Entity> entities;

	Arena asset_arena;
	b32 is_initialized;
	int frame;

	Animation test_anim;

	float time;

	b32 camera_free_mode;
	bool debug_collision;
	v3 last_camera_free_p;

	v3 free_camera_p;

	float rot;

	Animation animations[ANIMATION_COUNT];

	ConstantBuffer constant_buffer;
};

#endif