struct Texture
{
	void *handle;
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
	void *handle;
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
	int type;
	const char *name;
};

enum PrimitiveType 
{
	PRIMITIVE_TRIANGLES,
	PRIMITIVE_LINES,
};

struct VertexBuffer
{
	void *handle;
};

struct FrameBuffer
{
	void *handle;
};

struct RenderPass
{
	int primitive_type;
	ShaderProgram program;
	
	VertexInputElement input_elements[32];
	int input_element_count;
};


struct RenderContext
{
    void *data;
    Array<Texture> loaded_textures;

    RenderPass *render_pass;
	Texture white_texture;

	uintptr_t active_framebuffer_id;
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

struct ConstantBufferElement
{
	int type;
	int array_size;
};

struct ConstantBuffer
{
	void *handle;
	usize size;
	ConstantBufferElement elements[64];
	int element_count;
};

int get_constant_buffer_element_size(int type)
{
	int size[] = {
		sizeof(mat4),
		sizeof(v4),
		sizeof(v3),
		sizeof(v2),
		sizeof(float),
		sizeof(int)
	};
	static_assert(ARRAY_SIZE(size) == CONSTANT_BUFFER_ELEMENT_COUNT);
	return size[type];
}


int get_constant_buffer_element_alignement(int type)
{ 
	int align[] = {
		alignof(mat4),
		alignof(v4),
		alignof(v3),
		alignof(v2),
		alignof(float),
		alignof(int)
	};
	static_assert(ARRAY_SIZE(align) == CONSTANT_BUFFER_ELEMENT_COUNT);
	return align[type];
}
