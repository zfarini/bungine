#define _CRT_SECURE_NO_WARNINGS
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <glad.c>
// TODO: replace this
#include <unordered_map>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#undef min
#undef max

#include "game.cpp"

extern "C" const char *__asan_default_options() { return "detect_leaks=0"; }

bool g_hide_mouse = true;

void update_game_input(GLFWwindow *window, GameInput &input, int frame)
{
	int button_map[BUTTON_COUNT] = {};
	
	for (int i = 0; i < 26; i++)
		button_map[BUTTON_A + i] = GLFW_KEY_A + i;

	button_map[BUTTON_ESCAPE] = GLFW_KEY_ESCAPE;
	button_map[BUTTON_SPACE] = GLFW_KEY_SPACE;
	button_map[BUTTON_LEFT_SHIFT] = GLFW_KEY_LEFT_SHIFT;
	button_map[BUTTON_LEFT_CONTROL] = GLFW_KEY_LEFT_CONTROL;

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

	input.last_mouse_p = input.mouse_p;
	input.mouse_p = V2(mouse_x, mouse_y);
	if (frame > 3)
		input.mouse_dp = input.mouse_p - input.last_mouse_p;

	if (IsDownFirstTime(input, TOGGLE_EDITOR_BUTTON)) {
		g_hide_mouse = !g_hide_mouse;
		glfwSetInputMode(window, GLFW_CURSOR, g_hide_mouse ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}
}

GAME_UPDATE_AND_RENDER(game_update_and_render);

int main()
{
	// TODO: change to mmap
	usize memory_size = GigaByte(4);
	Arena memory = make_arena(calloc(4, memory_size), memory_size);

	ma_device sound_device;
	{

		ma_device_config config = ma_device_config_init(ma_device_type_playback);
		config.playback.format   = ma_format_f32;
		config.playback.channels = SOUND_CHANNEL_COUNT;
		config.sampleRate        = SOUND_SAMPLE_RATE;
		config.dataCallback      = audio_write_callback;
		// @HACK: the game struct should always be allocated the first thing
		config.pUserData         = memory.data;

		if (ma_device_init(NULL, &config, &sound_device) != MA_SUCCESS)
			printf("ERROR: couldn't init sound device\n");
		else
			ma_device_start(&sound_device);
	}
	if (!glfwInit())
		assert(0);

#ifdef RENDERER_DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

	GLFWwindow *window = glfwCreateWindow(1920, 960, "game",  0, 0);
//	glfwSetWindowSize(window, 1920, 1080);
	if (!window)
		assert(0);

	glfwMakeContextCurrent(window);
	//glfwSwapInterval(0);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		assert(0);

	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	printf("OpenGL Renderer\nvendor: %s\nrenderer: %s\n", vendor, renderer);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);// Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();

	RenderContext rc = {};

	GameInput game_input = {};
	{
		double mouse_x, mouse_y;
		glfwGetCursorPos(window, &mouse_x, &mouse_y);
		game_input.mouse_p = game_input.last_mouse_p = V2(mouse_x, mouse_y);
	}

	if (g_hide_mouse)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	Platform platform = {};
	platform.render_context = &rc;
	platform.imgui_context = ImGui::GetCurrentContext();

	int frame = 0;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		update_game_input(window, game_input, frame);
		if (IsDown(game_input, BUTTON_ESCAPE))
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		glfwGetFramebufferSize(window, &rc.window_width, &rc.window_height);
		game_update_and_render(platform, &memory, game_input, 1.f / 60);

		glfwSwapBuffers(window);
		frame++;
	}

	ma_device_uninit(&sound_device);
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
}
