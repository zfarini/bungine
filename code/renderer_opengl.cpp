// TODO: cleanup remove this
TextureID create_texture(String name, void *data, int width, int height, bool srgb = true,
		bool mipmapping = true)
{
	Texture texture = {};

	texture.width = width;
	texture.height = height;
	texture.name = name;

	uint32_t internal_format, format;

	if (srgb)
		internal_format = GL_SRGB_ALPHA;
	else
		internal_format = GL_RGBA;

	format = GL_RGBA;

	glGenTextures(1, &texture.opengl_id);

	glBindTexture(GL_TEXTURE_2D, texture.opengl_id);
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

	texture.in_gpu = true;
	texture.state = TEXTURE_STATE_LOADED;
	platform.lock_mutex(platform.render_context->texture_mutex);
	texture.id = platform.render_context->loaded_textures.count + 1;
	platform.render_context->loaded_textures.push(texture);
	platform.unlock_mutex(platform.render_context->texture_mutex);

	return texture.id;
}

// NOTE: right now I'm using the id just as an index to the array (I skip the zero entry)
Texture &get_texture(TextureID id)
{
	assert(id <= platform.render_context->loaded_textures.count);
	if (!id)
		return platform.render_context->loaded_textures[platform.render_context->purple_texture];
	return  platform.render_context->loaded_textures[id - 1];
}

void bind_texture(int index, TextureID id)
{
	Texture &texture = get_texture(id);

	if (texture.state == TEXTURE_STATE_LOADED) {
		if (!texture.in_gpu) {
			uint32_t internal_format, format;

			if (texture.srgb)
				internal_format = GL_SRGB_ALPHA;
			else
				internal_format = GL_RGBA;

			format = GL_RGBA;

			glGenTextures(1, &texture.opengl_id);

			glBindTexture(GL_TEXTURE_2D, texture.opengl_id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			if (texture.gen_mipmaps)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
						GL_LINEAR_MIPMAP_LINEAR);
			else
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
						GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, internal_format, texture.width, texture.height, 0, format,
					GL_UNSIGNED_BYTE, texture.data);

			if (texture.gen_mipmaps)
				glGenerateMipmap(GL_TEXTURE_2D);

			texture.in_gpu = true;
		}
		glActiveTexture(GL_TEXTURE0 + index);
		glBindTexture(GL_TEXTURE_2D, texture.opengl_id);
		return ;
	}
	else if (texture.state == TEXTURE_STATE_UNLOADED) {
		texture.state = TEXTURE_STATE_LOADING;
		platform.add_thread_work(load_texture_work, &texture);
	}
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, get_texture(platform.render_context->purple_texture).opengl_id);
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
		char info_log[2048];
		glGetShaderInfoLog(shader, sizeof(info_log), 0, info_log);
		LOG_ERROR("[opengl] failed to compile shader \"%.*s\": %s", str_format(filename), info_log);
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
		LOG_ERROR("[opengl] failed to link program: %s", info_log);
		assert(0);
	}

	return rp;
}

void begin_render_pass(RenderPass &rp)
{
	platform.render_context->render_pass = &rp;
	glUseProgram(rp.program);
	bind_depth_stencil_state(rp.depth_stencil_state);
	bind_rasterizer_state(rp.rasterizer_state);
}

void end_render_pass()
{
	platform.render_context->render_pass = 0;
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

// TODO: does this require a vao to be bound?
IndexBuffer create_index_buffer(usize size, uint32_t *indices)
{
	IndexBuffer result = {};

	glGenBuffers(1, &result.ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(uint32_t), indices, GL_STATIC_DRAW);

	return result;
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
	auto &layout = platform.render_context->render_pass->input_layout;

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

// TODO: this assumes a vao was already bound
void bind_index_buffer(IndexBuffer &ib)
{
	assert(platform.render_context->render_pass);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib.ebo);
}

void draw(int offset, int vertices_count)
{
	int mode = 0;
	if (platform.render_context->render_pass->primitive_type == PRIMITIVE_TRIANGLES)
		mode = GL_TRIANGLES;
	else if (platform.render_context->render_pass->primitive_type == PRIMITIVE_LINES)
		mode = GL_LINES;
	else
		assert(0);
	glDrawArrays(mode, offset, vertices_count);
}

void draw_indexed(int offset, int indices_count)
{
	int mode = 0;
	if (platform.render_context->render_pass->primitive_type == PRIMITIVE_TRIANGLES)
		mode = GL_TRIANGLES;
	else if (platform.render_context->render_pass->primitive_type == PRIMITIVE_LINES)
		mode = GL_LINES;
	else
		assert(0);
	glDrawElements(mode, indices_count, GL_UNSIGNED_INT, (void *)offset);
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

//https://registry.khronos.org/OpenGL/specs/gl/glspec45.core.pdf#page=159
ConstantBuffer create_constant_buffer(Array<ConstantBufferElement> elements)
{
	ConstantBuffer result = {};

	assert(elements.count < ARRAY_SIZE(result.elements));
	int offset = 0;
	for (int i = 0; i < elements.count; i++) {
		result.elements[i] = elements[i];

		offset = align_to(offset, get_type_alignement(elements[i]));
		if (elements[i].array_size) {
			// TODO: this is likely wrong, it doesn't work with v3 array but 
			// the wiki says some implementation get it wrong
			int stride = align_to(get_type_size(elements[i]), sizeof(v4));
			offset += stride * elements[i].array_size;
			offset = align_to(offset, get_type_alignement(elements[i]));
		}
		else
			offset += get_type_size(elements[i]);
	}

	result.element_count = (int)elements.count;
	result.size = offset;

	glGenBuffers(1, &result.id);
	glBindBuffer(GL_UNIFORM_BUFFER, result.id);
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
	glBufferSubData(GL_UNIFORM_BUFFER, 0, buffer.size, (void *)dest);
}

void bind_constant_buffer(ConstantBuffer &cbuffer, int index)
{
	glBindBufferBase(GL_UNIFORM_BUFFER, index, cbuffer.id); 
}

void begin_render_frame()
{
	platform.render_context->debug_lines.count = 0;
	//#ifdef _WIN32
	//ImGui_ImplWin32_NewFrame();
	//#else
	ImGui_ImplGlfw_NewFrame();
	//#endif
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();
}

void end_render_frame()
{
	bind_framebuffer(platform.render_context->window_framebuffer);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void APIENTRY gl_debug_output(GLenum source, GLenum type, unsigned int id,
		GLenum severity, GLsizei length,
		const char *message, const void *userParam) {
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
		return;

	const char *s_source = "", *s_type = "", *s_severity = "";
	
	switch (source) {
		case GL_DEBUG_SOURCE_API: s_source = "API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: s_source = "Window System"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: s_source = "Shader Compiled"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY: s_source = "Third Party"; break;
		case GL_DEBUG_SOURCE_APPLICATION: s_source = "Application"; break;
		case GL_DEBUG_SOURCE_OTHER: s_source = "Other"; break;
	}

	switch (type) {
		case GL_DEBUG_TYPE_ERROR: s_type = "Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: s_type = "Deprecated Behaviour"; break ;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: s_type = "Undefined Behaviour"; break;
		case GL_DEBUG_TYPE_PORTABILITY: s_type = "Portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE: s_type = "Performance"; break;
		case GL_DEBUG_TYPE_MARKER: s_type = "Marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP: s_type = "Push Group"; break;
		case GL_DEBUG_TYPE_POP_GROUP: s_type = "Pop Group"; break;
		case GL_DEBUG_TYPE_OTHER: s_type = "Other"; break;
	}

	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH: s_severity = "high"; break;
		case GL_DEBUG_SEVERITY_MEDIUM: s_severity = "meduim"; break;
		case GL_DEBUG_SEVERITY_LOW: s_severity = "low"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: s_severity = "notification";break;
	}

	LOG_WARN("[opengl debug layer (id: %d, source: %s, type: %s, severity: %s)]: %s",
			id, s_source, s_type, s_severity, message);
}

void init_render_context_opengl(RenderContext &rc)
{
#ifndef _WIN32
	int flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(gl_debug_output, 0);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
				0, GL_TRUE);
	}
#endif
	rc.window_framebuffer.id = 0;
	glEnable(GL_FRAMEBUFFER_SRGB);
	// TODO: cleanup
	glLineWidth(1.5f);
}

// TODO: merge this with create_texture
TextureID create_depth_texture(int width, int height)
{
	Texture texture = {};

	glGenTextures(1, &texture.opengl_id);
	glBindTexture(GL_TEXTURE_2D, texture.opengl_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height,
			0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	// TODO: cleanup, give the ability to change these
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	
	platform.lock_mutex(platform.render_context->texture_mutex);

	texture.id = platform.render_context->loaded_textures.count + 1;
	texture.state = TEXTURE_STATE_LOADED;
	texture.in_gpu = true;
	platform.render_context->loaded_textures.push(texture);

	platform.unlock_mutex(platform.render_context->texture_mutex);

	return texture.id;
}

FrameBuffer create_frame_buffer(bool depth_only = false, bool read = false)
{
	FrameBuffer result = {};
	uint32_t fbo;

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (!depth_only) {
		if (read)
			glReadBuffer(GL_COLOR_ATTACHMENT0);
		GLenum buffers[] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, buffers);
	}

	//glBindFramebuffer(GL_FRAMEBUFFER, platform.render_context.active_framebuffer_id);

	result.id = fbo;
	return result;
}

void bind_framebuffer_depthbuffer(FrameBuffer &framebuffer, Texture &texture)
{
	framebuffer.depth_texture = texture;
	bind_framebuffer(framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
			texture.opengl_id, 0);
}

void bind_framebuffer_color(FrameBuffer &framebuffer, Texture &texture)
{
	framebuffer.color_texture = texture;
	bind_framebuffer(framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
			texture.opengl_id, 0);
}
