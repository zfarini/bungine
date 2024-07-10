
struct Entity
{
	v3 position; // world position

	Scene *scene;
	mat4 scene_transform;

	Animation *animation;
	float anim_time;
};

struct Game
{
	v3 camera_p;
	v3 camera_rotation;

	RenderPass mesh_render_pass;
	RenderPass shadow_map_render_pass;

	Scene ch43, sponza, cube;

	Array<Entity> entities;

	Arena asset_arena;
	b32 is_initialized;

	Animation test_anim;
	
	float time;
};