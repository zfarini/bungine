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

int g_window_width, g_window_height;

#include "common.h"
#include "arena.h"
#include "utils.h"
#include "math.h"
#include "platform.h"
#define ENABLE_SRGB
#include "renderer.h"

RenderContext g_rc;
struct Constants
{
	mat4 view;
	mat4 projection;
	mat4 model;
	mat4 light_transform;
	mat4 bones[96];

	v3 camera_p;
	v3 player_p;
	float diffuse_factor;

	float specular_factor;
	float specular_exponent_factor;
	int skinned;
	int has_normal_map;
};

#include "renderer_opengl.cpp"
#include "renderer.cpp"

#include "game.cpp"

extern "C" const char *__asan_default_options() { return "detect_leaks=0"; }

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	(void)window;
	glViewport(0, 0, width, height);
}

v2 last_mouse_p;

void update_game_input(GLFWwindow *window, GameInput &input, int frame)
{
	int button_map[BUTTON_COUNT] = {};

	button_map[BUTTON_CAMERA_FORWARD] 	= GLFW_KEY_W;
	button_map[BUTTON_CAMERA_BACKWARD] 	= GLFW_KEY_S;
	button_map[BUTTON_CAMERA_LEFT] 		= GLFW_KEY_A;
	button_map[BUTTON_CAMERA_RIGHT]  	= GLFW_KEY_D;
	button_map[BUTTON_CAMERA_UP] 		= GLFW_KEY_Q;
	button_map[BUTTON_CAMERA_DOWN] 		= GLFW_KEY_E;
	button_map[BUTTON_PLAYER_FORWARD] 	= GLFW_KEY_F;
	button_map[BUTTON_PLAYER_BACKWARD] 	= GLFW_KEY_G;
	button_map[BUTTON_PLAYER_JUMP]		= GLFW_KEY_SPACE;
	button_map[BUTTON_LEFT_SHIFT]		= GLFW_KEY_LEFT_SHIFT;

	for (int i = BUTTON_F1; i < BUTTON_COUNT; i++)
		button_map[i] = GLFW_KEY_F1 + (i - BUTTON_F1);

	for (int i = 0; i < BUTTON_COUNT; i++) {
		input.buttons[i].was_down = input.buttons[i].is_down;
		input.buttons[i].is_down = glfwGetKey(window, button_map[i]) == GLFW_PRESS;
	}

	input.buttons[BUTTON_MOUSE_LEFT].is_down = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	input.buttons[BUTTON_MOUSE_RIGHT].is_down = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

	double mouse_x, mouse_y;
	glfwGetCursorPos(window, &mouse_x, &mouse_y);
	if (frame > 3)
		input.mouse_dp = V2(mouse_x, mouse_y) - last_mouse_p;
	last_mouse_p = V2((float)mouse_x, (float)mouse_y);
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
	#ifdef ENABLE_SRGB
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	#endif

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

    usize memory_size = GigaByte(1);
    Arena memory = make_arena(malloc(memory_size), memory_size);

	usize temp_arena_size = Megabyte(128);
	g_temp_arena.arena = make_arena(arena_alloc(&memory, temp_arena_size), temp_arena_size);

	init_render_context(&memory, g_rc);
	Game *game = (Game *)arena_alloc(&memory, sizeof(*game));


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

	GameInput game_input = {};

	// ConstantBuffer constant_buffer = create_constant_buffer(0, elems, ARRAY_SIZE(elems));
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	int frame = 0;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			break ;
		update_game_input(window, game_input, frame);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glfwGetFramebufferSize(window, &g_window_width, &g_window_height);
		game_update_and_render(*game, &memory, game_input, 1.f / 60);
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
		frame++;
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
}
