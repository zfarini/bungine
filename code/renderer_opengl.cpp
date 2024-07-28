global RenderContext *g_rc;

Texture create_texture(String name, void *data, int width, int height, bool srgb = true,
	bool mipmapping = true)
{
	Texture texture = {};

	texture.name = name;
	texture.valid = true;

	uint32_t internal_format, format;

	if (srgb)
		internal_format = GL_SRGB_ALPHA;
	else
		internal_format = GL_RGBA;

	format = GL_RGBA;

	uint32_t tex;
	g_rc->glGenTextures(1, &tex);

	g_rc->glBindTexture(GL_TEXTURE_2D, tex);
	g_rc->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	g_rc->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	if (mipmapping)
		g_rc->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				GL_LINEAR_MIPMAP_LINEAR);
	else
		g_rc->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				GL_LINEAR);
	g_rc->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	g_rc->glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format,
			GL_UNSIGNED_BYTE, data);
    if (mipmapping)
	    g_rc->glGenerateMipmap(GL_TEXTURE_2D);

	texture.id = tex;
	return texture;
}

void bind_texture(int index, Texture &texture)
{
    g_rc->glActiveTexture(GL_TEXTURE0 + index);
    g_rc->glBindTexture(GL_TEXTURE_2D, (uintptr_t)texture.id);
}

Shader load_shader(String filename, ShaderType type, const char *main = "")
{
	(void)main;

	Shader result = {};

	result.type = type;

    Arena *temp = begin_temp_memory();

    String content = load_entire_file(temp, filename);
    int gl_type = 0;
    if (type == SHADER_TYPE_VERTEX)
        gl_type = GL_VERTEX_SHADER;
    else if (type == SHADER_TYPE_FRAGMENT)
        gl_type = GL_FRAGMENT_SHADER;
    else
        assert(0);

	uint32_t shader = g_rc->glCreateShader(gl_type);
	g_rc->glShaderSource(shader, 1, &content.data, (GLint *)&content.count);

    end_temp_memory();

	g_rc->glCompileShader(shader);

	int success;
	g_rc->glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		printf("OPENGL: failed to compile shader \"%.*s\": ", str_format(filename));

		char info_log[2048];
		g_rc->glGetShaderInfoLog(shader, sizeof(info_log), 0, info_log);
		printf("%s\n", info_log);
		assert(0);
	}

    result.id = shader;
    return result;

}

RasterizerState create_rasterizer_state(RasterizerFillMode fillmode, RasterizerCullMode cullmode)
{
	RasterizerState result = {};

	result.fillmode = fillmode;
	result.cullmode = cullmode;

	return result;
}

DepthStencilState create_depth_stencil_state(bool enable_depth)
{
	DepthStencilState result = {};

	result.enable_depth = enable_depth;
	return result;
}

void bind_rasterizer_state(const RasterizerState &state)
{
	if (state.fillmode == RASTERIZER_FILL_SOLID)
		g_rc->glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (state.fillmode == RASTERIZER_FILL_WIREFRAME)
		g_rc->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (state.cullmode == RASTERIZER_CULL_NONE)
		g_rc->glDisable(GL_CULL_FACE);
	else if (state.cullmode == RASTERIZER_CULL_FRONT) {
		g_rc->glEnable(GL_CULL_FACE);
		g_rc->glCullFace(GL_FRONT);
	}
	else if (state.cullmode == RASTERIZER_CULL_BACK) {
		g_rc->glEnable(GL_CULL_FACE);
		g_rc->glCullFace(GL_BACK);
	}
}

void bind_depth_stencil_state(const DepthStencilState &state)
{
	if (state.enable_depth)
		g_rc->glEnable(GL_DEPTH_TEST);
	else
		g_rc->glDisable(GL_DEPTH_TEST);
}

VertexInputLayout create_vertex_input_layout(VertexInputElement *elements, int element_count,
		int vertex_size)
{
	VertexInputLayout result = {};

	result.vertex_size = vertex_size;
	result.element_count = element_count;
	for (int i = 0; i < element_count; i++)
		result.elements[i] = elements[i];

	return result;
}

RenderPass create_render_pass(Shader vs, Shader fs,
	PrimitiveType primitive_type, const DepthStencilState &depth_stencil_state,
		const RasterizerState &rasterizer_state, const VertexInputLayout &input_layout)
{
	RenderPass rp = {};

	rp.vs = vs;
	rp.fs = fs;
	rp.depth_stencil_state = depth_stencil_state;
	rp.rasterizer_state = rasterizer_state;
	rp.primitive_type = primitive_type;
	rp.input_layout = input_layout;
	
    rp.program = g_rc->glCreateProgram();

	g_rc->glAttachShader(rp.program, vs.id);
	g_rc->glAttachShader(rp.program, fs.id);
	g_rc->glLinkProgram(rp.program);

	int success;
	g_rc->glGetProgramiv(rp.program, GL_LINK_STATUS, &success);
	if (!success) {
		char info_log[2048];
		g_rc->glGetProgramInfoLog(rp.program, sizeof(info_log), 0, info_log);
		printf("failed to link program: %s\n", info_log);
        assert(0);
	}

	return rp;
}

void begin_render_pass(RenderPass &rp)
{
	g_rc->render_pass = &rp;
	g_rc->glUseProgram(rp.program);
	bind_depth_stencil_state(rp.depth_stencil_state);
	bind_rasterizer_state(rp.rasterizer_state);
}

void end_render_pass()
{
	g_rc->render_pass = 0;
}

void set_viewport(float top_left_x, float top_left_y, float width, float height)
{
	g_rc->glViewport(top_left_x, top_left_y, width, height);
}

void bind_framebuffer(FrameBuffer &framebuffer)
{
	// TODO: only bind if its not the active one (eh maybe opengl already handle this?)
	g_rc->glBindFrameBuffer(GL_FRAMEBUFFER, framebuffer.id);
}

void clear_framebuffer_color(FrameBuffer &framebuffer, v4 color)
{
	bind_framebuffer(framebuffer);
	g_rc->glClearColor(color.x, color.y, color.z, color.w);
	g_rc->glClear(GL_COLOR_BUFFER_BIT);
}

void clear_framebuffer_depth(FrameBuffer &framebuffer, float depth)
{
	bind_framebuffer(framebuffer);
	g_rc->glClearDepth(depth);
	g_rc->glClear(GL_DEPTH_BUFFER_BIT);
}

VertexBuffer create_vertex_buffer(VertexBufferUsage usage, usize size, void *data = 0)
{
	VertexBuffer result = {};

	result.usage = usage;
	result.size = size;

	g_rc->glGenVertexArrays(1, &result.vao);
	g_rc->glBindVertexArray(result.vao);

	g_rc->glGenBuffers(1, &result.vbo);
	g_rc->glBindBuffer(GL_ARRAY_BUFFER, result.vbo);
	if (data)
		g_rc->glBufferData(GL_ARRAY_BUFFER, size, data, usage == VERTEX_BUFFER_IMMUTABLE ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);

	return result;
}

void update_vertex_buffer(VertexBuffer &vb, int size, void *data)
{
	assert(vb.usage == VERTEX_BUFFER_DYNAMIC);
	g_rc->glBindVertexArray(vb.vao); // TODO: do I need this call?
    g_rc->glBindBuffer(GL_ARRAY_BUFFER, vb.vbo);
    g_rc->glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
}

void bind_vertex_buffer(VertexBuffer &vb)
{
	auto &layout = g_rc->render_pass->input_layout;

	g_rc->glBindVertexArray(vb.vao);
	g_rc->glBindBuffer(GL_ARRAY_BUFFER, vb.vbo);
	// TODO: this doesn't have to be done every frame
	for (int i = 0; i < layout.element_count; i++) {
        int type = 0;
         if (layout.elements[i].type == INPUT_ELEMENT_FLOAT)
            type = GL_FLOAT;
        else if (layout.elements[i].type == INPUT_ELEMENT_SIGNED_INT)
            type = GL_INT;
        else
            assert(0);

        g_rc->glVertexAttribPointer(i, layout.elements[i].count,
            type, GL_FALSE, (GLsizei) layout.vertex_size, (void *)layout.elements[i].offset);
        g_rc->glEnableVertexAttribArray(i);
    }
}

void draw(int offset, int vertices_count)
{
	int mode = 0;
    if (g_rc->render_pass->primitive_type == PRIMITIVE_TRIANGLES)
        mode = GL_TRIANGLES;
    else if (g_rc->render_pass->primitive_type == PRIMITIVE_LINES)
        mode = GL_LINES;
    else
        assert(0);
	g_rc->glDrawArrays(mode, offset, vertices_count);
}

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

int get_type_alignement(ConstantBufferElement e)
{
    int align = 0;
    if (e.array_size > 0)
        align = 4 * alignof(float);
    else if (e.type == CONSTANT_BUFFER_ELEMENT_VEC2)
        align = 2 * alignof(float);
    else if (e.type == CONSTANT_BUFFER_ELEMENT_VEC3 || e.type == CONSTANT_BUFFER_ELEMENT_VEC4
        || e.type == CONSTANT_BUFFER_ELEMENT_MAT4)
        align = 4 * alignof(float);
    else if (e.type == CONSTANT_BUFFER_ELEMENT_INT || e.type == CONSTANT_BUFFER_ELEMENT_FLOAT)
        align = alignof(float);
    else
        assert(0);
    return align;
}

int get_type_size(ConstantBufferElement e)
{
    return get_constant_buffer_element_size(e.type);
}

ConstantBuffer create_constant_buffer(Array<ConstantBufferElement> elements)
{
	ConstantBuffer result = {};

	assert(elements.count < ARRAY_SIZE(result.elements));
	int offset = 0;
    for (int i = 0; i < elements.count; i++) {
        result.elements[i] = elements[i];
 
        offset = align_to(offset, get_type_alignement(elements[i]))
            + get_type_size(elements[i]) * (elements[i].array_size ? elements[i].array_size : 1);
    }

	result.element_count = elements.count;
	result.size = offset;

	g_rc->glGenBuffers(1, &result.id);
    g_rc->glBindBuffer(GL_UNIFORM_BUFFER, result.id);
	// TODO: why do we do this?
    g_rc->glBufferData(GL_UNIFORM_BUFFER, result.size, 0, GL_DYNAMIC_DRAW);

	return result;
}

void update_constant_buffer(ConstantBuffer &buffer, void *data)
{
	char dest[sizeof(mat4) * 512] = {};

	assert(buffer.size < sizeof(dest));

    int cstruct_offset = 0;
    int ubo_offset = 0;

    for (int i = 0; i < buffer.element_count; i++) {
        ConstantBufferElement e = buffer.elements[i];

        ubo_offset = align_to(ubo_offset, get_type_alignement(e));
        cstruct_offset = align_to(cstruct_offset, get_constant_buffer_element_alignement(e.type));

        int count = e.array_size ? e.array_size : 1;
        for (int j = 0; j < count; j++) {
            memcpy(dest + ubo_offset, (char *)data + cstruct_offset, get_constant_buffer_element_size(e.type));
            ubo_offset += get_type_size(e);
            cstruct_offset += get_constant_buffer_element_size(e.type);
        }
    }
    //memcpy(dest, data, sizeof(Constants));

    g_rc->glBindBuffer(GL_UNIFORM_BUFFER, buffer.id);
	// TODO: subData?
    g_rc->glBufferSubData(GL_UNIFORM_BUFFER, 0, buffer.size, (void *)dest);
}

void bind_constant_buffer(ConstantBuffer &cbuffer, int index)
{
    g_rc->glBindBufferBase(GL_UNIFORM_BUFFER, index, cbuffer.id); 
}

void begin_render_frame()
{
	XWindowAttributes attr;
    Status status = XGetWindowAttributes(g_rc->x_display, g_rc->window, &attr);
	g_rc->window_width = attr.width;
	g_rc->window_height = attr.height;

	g_rc->debug_lines.count = 0;
}

void end_render_frame()
{
}

void APIENTRY gl_debug_output(GLenum source, GLenum type, unsigned int id,
		GLenum severity, GLsizei length,
		const char *message, const void *userParam) {
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
		return;
	
	printf("---------------\nOPENGL Debug: (%d): %s\n", id, message);
	printf("Sorce: ");
	switch (source) {
		case GL_DEBUG_SOURCE_API: printf("API"); break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: printf("Window System"); break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: printf("Shader Compiled"); break;
		case GL_DEBUG_SOURCE_THIRD_PARTY: printf("Third Party"); break;
		case GL_DEBUG_SOURCE_APPLICATION: printf("Application"); break;
		case GL_DEBUG_SOURCE_OTHER: printf("Other"); break;
	}
	printf("\nType: ");

	switch (type) {
		case GL_DEBUG_TYPE_ERROR: printf("Error"); break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: printf("Deprecated Behaviour"); break ;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: printf("Undefined Behaviour"); break;
		case GL_DEBUG_TYPE_PORTABILITY: printf("Portability"); break;
		case GL_DEBUG_TYPE_PERFORMANCE: printf("Performance"); break;
		case GL_DEBUG_TYPE_MARKER: printf("Marker"); break;
		case GL_DEBUG_TYPE_PUSH_GROUP: printf("Push Group"); break;
		case GL_DEBUG_TYPE_POP_GROUP: printf("Pop Group"); break;
		case GL_DEBUG_TYPE_OTHER: printf("Other"); break;
	}
	printf("\nSeverity: ");

	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH: printf("high"); break;
		case GL_DEBUG_SEVERITY_MEDIUM: printf("meduim"); break;
		case GL_DEBUG_SEVERITY_LOW: printf("low"); break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: printf("notification"); break;
	}
	printf("\n\n");
}

void init_render_context_opengl(RenderContext &rc)
{
#ifdef OPENGL_DEBUG
	int flags;
	rc.glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    	rc.glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		rc.glEnable(GL_DEBUG_OUTPUT);
		rc.glDebugMessageCallback(gl_debug_output, nullptr);
		rc.glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
			                     nullptr, GL_TRUE);
	}
#endif
	rc.window_framebuffer.id = 0;
	rc.glEnable(GL_FRAMEBUFFER_SRGB);
}
