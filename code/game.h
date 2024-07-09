
struct Game
{
	v3 camera_p;
	v3 camera_rotation;

	RenderPass mesh_render_pass;
	RenderPass shadow_map_render_pass;

	Scene ch43, sponza, cube;

	Arena asset_arena;
	b32 is_initialized;
};