void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id,
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

void init_render_context(RenderContext &rc)
{
    {
		int flags;
		glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
			// initialize debug output
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(glDebugOutput, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
			                     nullptr, GL_TRUE);
		}
	}
    glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

Shader load_shader(String filename, ShaderType type, const char *main = "")
{
    Shader result = {};

    Arena *temp = begin_temp_memory();

    String content = load_entire_file(temp, filename);
    int gl_type;
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

    result.handle = (void *)shader;
    return result;
}

ShaderProgram create_shader_program(Shader vs, Shader fs)
{
    ShaderProgram result = {};

    result.vs = vs;
    result.fs = fs;

    uint32_t program = glCreateProgram();

	glAttachShader(program, (uintptr_t)vs.handle);
	glAttachShader(program, (uintptr_t)fs.handle);
	glLinkProgram(program);

	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		char info_log[2048];
		glGetProgramInfoLog(program, sizeof(info_log), 0, info_log);
		printf("failed to link program: %s\n", info_log);
        assert(0);
	}

    result.handle = (void *)program;
    return result;
}

void set_primitive_type(int type)
{
	g_rc.render_pass->primitive_type = type;
}

RenderPass create_render_pass(ShaderProgram program,
	VertexInputElement *input_elements, int input_element_count)
{
	RenderPass result = {};

	assert(input_element_count < ARRAY_SIZE(result.input_elements));
	result.program = program;
	result.input_element_count = input_element_count;
	for (int i = 0; i < input_element_count; i++)
		result.input_elements[i] = input_elements[i];

	return result;
}

VertexBuffer create_vertex_buffer(usize size, void *data,
    usize vertex_size, VertexInputElement *input_elements, int input_element_count)
{
    uint32_t vao;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    uint32_t vbo;
    glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    
    assert(input_element_count > 0);

    for (int i = 0; i < input_element_count; i++) {
        int type;
         if (input_elements[i].type == INPUT_ELEMENT_FLOAT)
            type = GL_FLOAT;
        else if (input_elements[i].type == INPUT_ELEMENT_SIGNED_INT)
            type = GL_INT;
        else
            assert(0);

        glVertexAttribPointer(i, input_elements[i].count,
            type, GL_FALSE, vertex_size, (void *)input_elements[i].offset);
        glEnableVertexAttribArray(i);
    }

    VertexBuffer result;
    result.handle = (void *)vao;
    return result;
}

void begin_render_pass(RenderPass &rp)
{
    g_rc.render_pass = &rp;
    glUseProgram((uintptr_t)rp.program.handle);
    set_primitive_type(PRIMITIVE_TRIANGLES);
}

void end_render_pass()
{
    g_rc.render_pass = 0;
}

void clear_color_buffer(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void clear_depth_buffer(float z)
{
    glClearDepth(z);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void draw(VertexBuffer &vb, int offset, int count)
{
    int mode = 0;
    if (g_rc.render_pass->primitive_type == PRIMITIVE_TRIANGLES)
        mode = GL_TRIANGLES;
    else if (g_rc.render_pass->primitive_type == PRIMITIVE_LINES)
        mode = GL_LINES;
    else
        assert(0);
    glDrawArrays(mode, offset, count);
}

static int get_type_alignement(ConstantBufferElement e)
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

static int get_type_size(ConstantBufferElement e)
{
    int elem_size = 0;

    if (e.type == CONSTANT_BUFFER_ELEMENT_MAT4)
        elem_size = get_type_alignement(e) * 4;
    else 
        elem_size = get_type_alignement(e);

    return elem_size;
}

ConstantBuffer create_constant_buffer(int index, ConstantBufferElement *elements, int element_count)
{
    ConstantBuffer result = {};
    assert(element_count < ARRAY_SIZE(result.elements));
    result.element_count = element_count;

    for (int i = 0; i < element_count; i++) {
        result.elements[i] = elements[i];
        result.size = align_to(result.size, get_type_alignement(elements[i]))
            + get_type_size(elements[i]) * (elements[i].array_size ? elements[i].array_size : 1);
    }

    uint32_t ubo;
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, result.size, 0, GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_UNIFORM_BUFFER, index, ubo); 

    result.handle = (void *)ubo;

    return result;
}

void update_constant_buffer(ConstantBuffer &buffer, void *data)
{
    Arena *temp = begin_temp_memory();

    char *dest = (char *)arena_alloc(temp, buffer.size);

    memset(dest, 0, buffer.size);

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

    glBindBuffer(GL_UNIFORM_BUFFER, (uintptr_t)buffer.handle);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, buffer.size, (void *)dest);
    end_temp_memory();
}
