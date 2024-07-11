#ifndef GAME_H
#define GAME_H
enum EntityType
{
	EntityType_Player,
	EntityType_Static
};

struct Entity
{
	int id;
	int type;
	v3 position; // world position
	v3 dp;
	v3 rotation;
	v3 drotation;

	v3 collision_box;

	b32 moved;
	b32 run;
	b32 shooting;
	b32 can_jump;
	b32 on_ground;


	Scene *scene;
	mat4 scene_transform;

	Animation *animation;
	float anim_time;
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

struct Game
{
	v3 camera_p;
	v3 camera_rotation;

	RenderPass mesh_render_pass;
	RenderPass shadow_map_render_pass;

	Scene ch43, sponza, cube_asset;

	Array<Entity> entities;

	Arena asset_arena;
	b32 is_initialized;
	int frame;

	Animation test_anim;

	float time;

	b32 camera_free_mode;
	b32 debug_collision;
	v3 last_camera_free_p;

	Animation animations[ANIMATION_COUNT];
};

#endif