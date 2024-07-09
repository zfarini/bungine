struct Texture
{
	ID3D11ShaderResourceView *data;
	String name;
};

struct ShadowMap
{
	ID3D11DepthStencilView *dsv;
	ID3D11ShaderResourceView *srv;
	v3 light_dir;
	v3 light_p;
	mat4 view;
	mat4 projection;
	int width, height;
};

struct RenderPass
{
	ID3D11VertexShader *vertex_shader;
	ID3D11PixelShader *pixel_shader;
	ID3D11InputLayout *input_layout;
	ID3D11RasterizerState *rasterizer_state;
	ID3D11Buffer	  *cbuffer;
	ID3D11SamplerState *sampler_state;
	ID3D11DepthStencilState* depth_stencil_state;

	mat4 view_mat;
	mat4 projection_mat;
	v3 camera_p;
};

struct RenderContext
{
	ID3D11Device *device;
	ID3D11DeviceContext *context;
	IDXGISwapChain *swap_chain;
	ID3D11RenderTargetView *backbuffer_rtv;
	RenderPass *render_pass;
	ID3D11DepthStencilView *depth_stencil_view;

	ID3D11ShaderResourceView *white_texture;

	Array<Texture> loaded_textures;

	ShadowMap shadow_map;
};

