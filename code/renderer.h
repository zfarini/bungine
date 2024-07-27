struct Texture
{
	#ifdef RENDERER_DX11
	ID3D11ShaderResourceView *srv;
	#endif
	String name;
	b32 valid;
};

enum ShaderType
{
	SHADER_TYPE_VERTEX,
	SHADER_TYPE_FRAGMENT,
};

struct Shader
{
	ShaderType type;

	#ifdef RENDERER_DX11
	ID3DBlob *blob;
	union {
		ID3D11VertexShader *vs;
		ID3D11PixelShader *ps;
	};
	#else
	#endif
};

struct ShaderProgram
{
	void *handle;
	Shader vs;
	Shader fs;
};

enum InputElementType
{
	INPUT_ELEMENT_FLOAT,
	INPUT_ELEMENT_SIGNED_INT,
};

struct VertexInputElement
{
	int offset;
	int count;
	InputElementType type;
	const char *name;
};

enum PrimitiveType 
{
	PRIMITIVE_TRIANGLES,
	PRIMITIVE_LINES,
};

enum VertexBufferUsage
{
	VERTEX_BUFFER_IMMUTABLE,
	VERTEX_BUFFER_DYNAMIC,
};

struct VertexBuffer
{
	usize size;
	VertexBufferUsage usage;
	usize vertex_size;

	#ifdef RENDERER_DX11
	ID3D11Buffer *buffer;
	#endif
};

struct FrameBuffer
{
	#ifdef RENDERER_DX11
	ID3D11RenderTargetView *rtv;
	ID3D11DepthStencilView *dsv;
	#endif
};

struct DepthStencilState
{
	bool enable_depth;
	#ifdef RENDERER_DX11
	ID3D11DepthStencilState *state;
	#endif
};

enum RasterizerFillMode
{
	RASTERIZER_FILL_SOLID,
	RASTERIZER_FILL_WIREFRAME
};

enum RasterizerCullMode
{
	RASTERIZER_CULL_NONE,
	RASTERIZER_CULL_FRONT,
	RASTERIZER_CULL_BACK,
};

struct RasterizerState
{
	RasterizerFillMode fillmode;
	RasterizerCullMode cullmode;
	#ifdef RENDERER_DX11
	ID3D11RasterizerState *state;
	#endif
};

struct RenderPass
{
	PrimitiveType primitive_type;

	Shader vs;
	Shader fs;
	DepthStencilState depth_stencil_state;
	RasterizerState rasterizer_state;
	#ifdef RENDERER_DX11
	ID3D11InputLayout *input_layout;
	
	#else

	#endif
};

struct RenderContext
{
	int window_width;
	int window_height;

	FrameBuffer window_framebuffer;

	#ifdef RENDERER_DX11
	HWND window;
	ID3D11Device *device;
	ID3D11DeviceContext *context;
	IDXGISwapChain *swap_chain;
	#else
	GLFWwindow *window;
	#endif

    Array<Texture> loaded_textures;

    RenderPass *render_pass;
	Texture white_texture;

	Array<v3> debug_lines;

	uintptr_t active_framebuffer_id;
	// TODO:!!!
	//GLFWwindow *window;
};

usize get_input_element_size(int type)
{
    if (type == INPUT_ELEMENT_FLOAT)
        return sizeof(float);
    else if (type == INPUT_ELEMENT_SIGNED_INT)
        return sizeof(int);
    else
        assert(0);
    return 0;
}

enum ConstantBufferElementType
{
	CONSTANT_BUFFER_ELEMENT_MAT4,
	CONSTANT_BUFFER_ELEMENT_VEC4,
	CONSTANT_BUFFER_ELEMENT_VEC3,
	CONSTANT_BUFFER_ELEMENT_VEC2,
	CONSTANT_BUFFER_ELEMENT_FLOAT,
	CONSTANT_BUFFER_ELEMENT_INT,
	CONSTANT_BUFFER_ELEMENT_COUNT
};

int get_c_type_alignement(ConstantBufferElementType type)
{
	switch (type) {
		case CONSTANT_BUFFER_ELEMENT_MAT4: return alignof(mat4);
		case CONSTANT_BUFFER_ELEMENT_VEC4: return alignof(v4);
		case CONSTANT_BUFFER_ELEMENT_VEC3: return alignof(v3);
		case CONSTANT_BUFFER_ELEMENT_VEC2: return alignof(v2);
		case CONSTANT_BUFFER_ELEMENT_FLOAT: return alignof(float);
		case CONSTANT_BUFFER_ELEMENT_INT: return alignof(int);
		default: assert(0);
	}
	return 0;
}

struct ConstantBufferElement
{
	ConstantBufferElementType type;
	int array_size;
};

struct ConstantBuffer
{
	usize size;
	ConstantBufferElement elements[64];
	int element_count;
	#ifdef RENDERER_DX11
	ID3D11Buffer *buffer;
	#endif
};