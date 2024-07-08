struct RenderPass
{
	ID3D11VertexShader *vertex_shader;
	ID3D11PixelShader *pixel_shader;
	ID3D11InputLayout *input_layout;
	ID3D11RasterizerState *rasterizer_state;
	ID3D11Buffer	  *cbuffer;

	mat4 view_mat;
	mat4 projection_mat;
};

struct RenderContext
{
	ID3D11Device *device;
	ID3D11DeviceContext *context;
	IDXGISwapChain *swap_chain;
	ID3D11RenderTargetView *backbuffer_rtv;

	RenderPass *render_pass;
};

