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
	glGenTextures(1, &tex);

	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	if (mipmapping)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				GL_LINEAR_MIPMAP_LINEAR);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format,
			GL_UNSIGNED_BYTE, data);
	if (mipmapping)
		glGenerateMipmap(GL_TEXTURE_2D);

	texture.id = tex;
	return texture;
}

void bind_texture(int index, Texture &texture)
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, (uintptr_t)texture.id);
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

	uint32_t shader = glCreateShader(gl_type);
	glShaderSource(shader, 1, &content.data, (GLint *)&content.count);

	end_temp_memory();

	glCompileShader(shader);

	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		printf("OPENGL: failed to compile shader \"%.*s\": ", str_format(filename));

		char info_log[2048];
		glGetShaderInfoLog(shader, sizeof(info_log), 0, info_log);
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
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (state.fillmode == RASTERIZER_FILL_WIREFRAME)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (state.cullmode == RASTERIZER_CULL_NONE)
		glDisable(GL_CULL_FACE);
	else if (state.cullmode == RASTERIZER_CULL_FRONT) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
	}
	else if (state.cullmode == RASTERIZER_CULL_BACK) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
}

void bind_depth_stencil_state(const DepthStencilState &state)
{
	if (state.enable_depth)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
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

	rp.program = glCreateProgram();

	glAttachShader(rp.program, vs.id);
	glAttachShader(rp.program, fs.id);
	glLinkProgram(rp.program);

	int success;
	glGetProgramiv(rp.program, GL_LINK_STATUS, &success);
	if (!success) {
		char info_log[2048];
		glGetProgramInfoLog(rp.program, sizeof(info_log), 0, info_log);
		printf("failed to link program: %s\n", info_log);
		assert(0);
	}

	return rp;
}

void begin_render_pass(RenderPass &rp)
{
	g_rc->render_pass = &rp;
	glUseProgram(rp.program);
	bind_depth_stencil_state(rp.depth_stencil_state);
	bind_rasterizer_state(rp.rasterizer_state);
}

void end_render_pass()
{
	g_rc->render_pass = 0;
}

void set_viewport(float top_left_x, float top_left_y, float width, float height)
{
	glViewport(top_left_x, top_left_y, width, height);
}

void bind_framebuffer(FrameBuffer &framebuffer)
{
	// TODO: only bind if its not the active one (eh maybe opengl already handle this?)
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);
}

void clear_framebuffer_color(FrameBuffer &framebuffer, v4 color)
{
	bind_framebuffer(framebuffer);
	glClearColor(color.x, color.y, color.z, color.w);
	glClear(GL_COLOR_BUFFER_BIT);
}

void clear_framebuffer_depth(FrameBuffer &framebuffer, float depth)
{
	bind_framebuffer(framebuffer);
	glClearDepth(depth);
	glClear(GL_DEPTH_BUFFER_BIT);
}

VertexBuffer create_vertex_buffer(VertexBufferUsage usage, usize size, void *data = 0)
{
	VertexBuffer result = {};

	result.usage = usage;
	result.size = size;

	glGenVertexArrays(1, &result.vao);
	glBindVertexArray(result.vao);

	glGenBuffers(1, &result.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, result.vbo);
	if (data)
		glBufferData(GL_ARRAY_BUFFER, size, data, usage == VERTEX_BUFFER_IMMUTABLE ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);

	return result;
}

void update_vertex_buffer(VertexBuffer &vb, int size, void *data)
{
	assert(vb.usage == VERTEX_BUFFER_DYNAMIC);
	glBindVertexArray(vb.vao); // TODO: do I need this call?
	glBindBuffer(GL_ARRAY_BUFFER, vb.vbo);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
}

void bind_vertex_buffer(VertexBuffer &vb)
{
	auto &layout = g_rc->render_pass->input_layout;

	glBindVertexArray(vb.vao);
	glBindBuffer(GL_ARRAY_BUFFER, vb.vbo);
	// TODO: this doesn't have to be done every frame
	for (int i = 0; i < layout.element_count; i++) {
		int type = 0;
		if (layout.elements[i].type == INPUT_ELEMENT_FLOAT)
			type = GL_FLOAT;
		else if (layout.elements[i].type == INPUT_ELEMENT_SIGNED_INT)
			type = GL_INT;
		else
			assert(0);

		glVertexAttribPointer(i, layout.elements[i].count,
				type, GL_FALSE, (GLsizei) layout.vertex_size, (void *)layout.elements[i].offset);
		glEnableVertexAttribArray(i);
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
	glDrawArrays(mode, offset, vertices_count);
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

	glGenBuffers(1, &result.id);
	glBindBuffer(GL_UNIFORM_BUFFER, result.id);
	// TODO: why do we do this?
	glBufferData(GL_UNIFORM_BUFFER, result.size, 0, GL_DYNAMIC_DRAW);

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

	glBindBuffer(GL_UNIFORM_BUFFER, buffer.id);
	// TODO: subData?
	glBufferSubData(GL_UNIFORM_BUFFER, 0, buffer.size, (void *)dest);
}

void bind_constant_buffer(ConstantBuffer &cbuffer, int index)
{
	glBindBufferBase(GL_UNIFORM_BUFFER, index, cbuffer.id); 
}

void begin_render_frame()
{
	glfwGetFramebufferSize(g_rc->window, &g_rc->window_width, &g_rc->window_height);
	g_rc->debug_lines.count = 0;
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();
}

void end_render_frame()
{
	bind_framebuffer(g_rc->window_framebuffer);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

void init_render_context_opengl(RenderContext &rc, Platform &platform)
{
	rc.window = platform.window;
#ifdef RENDERER_DEBUG
	int flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(gl_debug_output, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
				nullptr, GL_TRUE);
	}
#endif
	rc.window_framebuffer.id = 0;
	glEnable(GL_FRAMEBUFFER_SRGB);
}

// TODO: merge this with create_texture
Texture create_depth_texture(int width, int height)
{
	Texture result = {};

	uint32_t texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height,
			0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	result.id = texture;
	result.valid = true;
	result.name = make_cstring("shadow map depth texture");
	return result;
}

FrameBuffer create_frame_buffer()
{
	FrameBuffer result = {};
	uint32_t fbo;

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	//glBindFramebuffer(GL_FRAMEBUFFER, g_rc.active_framebuffer_id);

	result.id = fbo;
	return result;
}

void bind_framebuffer_depthbuffer(FrameBuffer &framebuffer, Texture &texture)
{
	bind_framebuffer(framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
			texture.id, 0);
	glLineWidth(1.5f);
}
