#pragma once

enum TextureState {
    TEXTURE_STATE_UNLOADED,
    TEXTURE_STATE_LOADING,
    TEXTURE_STATE_LOADED,
};

struct Texture {
#ifdef RENDERER_DX11
    ID3D11ShaderResourceView *srv;
#else
    uint32_t id;
#endif
    String name;
    int width;
    int height;
    b32 valid; // TODO: remove this


    int state;
};

enum ShaderType {
    SHADER_TYPE_VERTEX,
    SHADER_TYPE_FRAGMENT,
};

struct Shader {
    ShaderType type;

#ifdef RENDERER_DX11
    ID3DBlob *blob;
    union {
        ID3D11VertexShader *vs;
        ID3D11PixelShader *ps;
    };
#else
    uint32_t id;
#endif
};

enum InputElementType {
    INPUT_ELEMENT_FLOAT,
    INPUT_ELEMENT_SIGNED_INT,
};

struct VertexInputElement {
    int offset;
    int count;
    InputElementType type;
    const char *name;
};

enum PrimitiveType {
    PRIMITIVE_TRIANGLES,
    PRIMITIVE_LINES,
};

enum VertexBufferUsage {
    VERTEX_BUFFER_IMMUTABLE,
    VERTEX_BUFFER_DYNAMIC,
};

struct VertexBuffer {
    usize size;
    VertexBufferUsage usage;

#ifdef RENDERER_DX11
    ID3D11Buffer *buffer;
#else
    uint32_t vao;
    uint32_t vbo;
#endif
};

struct IndexBuffer {
    #ifdef RENDERER_OPENGL
    uint32_t ebo;
    #else
    #endif
};

struct FrameBuffer {
#ifdef RENDERER_DX11
    ID3D11RenderTargetView *rtv;
    ID3D11DepthStencilView *dsv;
#else
    uint32_t id;

#endif
    Texture color_texture;
    Texture depth_texture;
};

struct DepthStencilState {
    bool enable_depth;
#ifdef RENDERER_DX11
    ID3D11DepthStencilState *state;
#endif
};

enum RasterizerFillMode { RASTERIZER_FILL_SOLID, RASTERIZER_FILL_WIREFRAME };

enum RasterizerCullMode {
    RASTERIZER_CULL_NONE,
    RASTERIZER_CULL_FRONT,
    RASTERIZER_CULL_BACK,
};

struct RasterizerState {
    RasterizerFillMode fillmode;
    RasterizerCullMode cullmode;
#ifdef RENDERER_DX11
    ID3D11RasterizerState *state;
#endif
};

struct VertexInputLayout {
    VertexInputElement elements[64];
    int element_count;
    int vertex_size;
#ifdef RENDERER_DX11
    ID3D11InputLayout *input_layout
#endif
};

struct RenderPass {
    PrimitiveType primitive_type;

    Shader vs;
    Shader fs;
    DepthStencilState depth_stencil_state;
    RasterizerState rasterizer_state;
    VertexInputLayout input_layout;

    uint32_t program;
};

struct RenderContext {
    int window_width;
    int window_height;

    FrameBuffer window_framebuffer;

#ifdef RENDERER_DX11
    HWND window;
    ID3D11Device *device;
    ID3D11DeviceContext *context;
    IDXGISwapChain *swap_chain;
#else
//     GLFWwindow *window;
// #define X(type, name) type name;
//     GL_FUNCTIONS(X)
// #undef X
#endif

    Array<Texture> loaded_textures;

    RenderPass *render_pass;
    Texture white_texture;

    Array<v3> debug_lines;

    uintptr_t active_framebuffer_id;
    // TODO:!!!
    // GLFWwindow *window;
};

usize get_input_element_size(int type) {
    if (type == INPUT_ELEMENT_FLOAT)
        return sizeof(float);
    else if (type == INPUT_ELEMENT_SIGNED_INT)
        return sizeof(int);
    else
        assert(0);
    return 0;
}

// TODO:cleanup maybe generate some enum for all struct declared and be able to query metadata
// using the id, then we can just use it here
enum ConstantBufferElementType {
    CONSTANT_BUFFER_ELEMENT_MAT4,
    CONSTANT_BUFFER_ELEMENT_VEC4,
    CONSTANT_BUFFER_ELEMENT_VEC3,
    CONSTANT_BUFFER_ELEMENT_VEC2,
    CONSTANT_BUFFER_ELEMENT_FLOAT,
    CONSTANT_BUFFER_ELEMENT_INT,
    CONSTANT_BUFFER_ELEMENT_COUNT
};

const char *get_constant_buffer_element_typename(ConstantBufferElementType type) {
	switch (type) {
    case CONSTANT_BUFFER_ELEMENT_MAT4:
		return "mat4";
    case CONSTANT_BUFFER_ELEMENT_VEC4:
		return "v4";
    case CONSTANT_BUFFER_ELEMENT_VEC3:
		return "v3";
    case CONSTANT_BUFFER_ELEMENT_VEC2:
		return "v2";
    case CONSTANT_BUFFER_ELEMENT_FLOAT:
		return "float";
    case CONSTANT_BUFFER_ELEMENT_INT:
		return "int";
    default:
        assert(0);
    }
	return "";
}

int get_c_type_alignement(ConstantBufferElementType type) {
    switch (type) {
    case CONSTANT_BUFFER_ELEMENT_MAT4:
        return alignof(mat4);
    case CONSTANT_BUFFER_ELEMENT_VEC4:
        return alignof(v4);
    case CONSTANT_BUFFER_ELEMENT_VEC3:
        return alignof(v3);
    case CONSTANT_BUFFER_ELEMENT_VEC2:
        return alignof(v2);
    case CONSTANT_BUFFER_ELEMENT_FLOAT:
        return alignof(float);
    case CONSTANT_BUFFER_ELEMENT_INT:
        return alignof(int);
    default:
        assert(0);
    }
    return 0;
}

struct ConstantBufferElement {
    ConstantBufferElementType type;
    int array_size;
};

struct ConstantBuffer {
    usize size;
    ConstantBufferElement elements[64];
    int element_count;
#ifdef RENDERER_DX11
    ID3D11Buffer *buffer;
#else
    uint32_t id;
#endif
};
