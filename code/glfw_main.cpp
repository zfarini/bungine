#include <stb_image.h>
#include <ufbx.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <fcntl.h>

#include <iostream>
#include <map>
#include <vector>
//#include <unistd.h>

#include "glad.c"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#undef min
#undef max

#include "common.h"
#include "arena.h"
#include "utils.h"
#include "math.h"
#include "platform.h"
#include "renderer.h"

RenderContext g_rc;

#include "renderer_opengl.cpp"
#include "renderer.cpp"

#include "game.cpp"

extern "C" const char *__asan_default_options() { return "detect_leaks=0"; }

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	(void)window;
	glViewport(0, 0, width, height);
}

int main()
{
	if (!glfwInit())
		assert(0);

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow *window = glfwCreateWindow(600, 400, "game", 0, 0);
	if (!window)
		assert(0);

	glfwMakeContextCurrent(window);
	// TODO: test with this, a lot of stuff break (mouse, jump..)
	//glfwSwapInterval(0);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		assert(0);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    usize memory_size = Megabyte(128);
    Arena memory = make_arena(malloc(memory_size), memory_size);

	usize temp_arena_size = Megabyte(16);
	g_temp_arena.arena = make_arena(arena_alloc(&memory, temp_arena_size), temp_arena_size);

	init_render_context(g_rc);

    Shader vertex_shader = load_shader(make_cstring("vertex.glsl"), SHADER_TYPE_VERTEX);
	Shader fragment_shader = load_shader(make_cstring("fragment.glsl"), SHADER_TYPE_FRAGMENT);
	ShaderProgram shader_program = create_shader_program(vertex_shader, fragment_shader);

	RenderPass render_pass = create_render_pass(shader_program, g_vertex_input_elements, ARRAY_SIZE(g_vertex_input_elements));

	struct Constants
	{
		mat4 view;
		mat4 projection;
		mat4 model;
	};
	ConstantBufferElement elems[] = {
		{CONSTANT_BUFFER_ELEMENT_MAT4},
		{CONSTANT_BUFFER_ELEMENT_MAT4},
		{CONSTANT_BUFFER_ELEMENT_MAT4},
	};

	// ConstantBuffer constant_buffer = create_constant_buffer(0, elems, ARRAY_SIZE(elems));


	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		clear_color_buffer(0.2f, 0.5f, 0.7f, 1.f);
		clear_depth_buffer(1.f);
		//game_update_and_render(game, game_input, dt, window);
        // begin_render_pass(render_pass);
		// {
		// 	Constants constants = {};
		// 	float x = sinf(glfwGetTime()) * 0.5f + 0.5f;
		// 	constants.color = V3(x, x, x);
		// 	update_constant_buffer(constant_buffer, &constants);
		// 	draw(vertex_buffer, 0, 3);
        // }
		// end_render_pass();
	
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
}
