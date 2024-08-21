#define _CRT_SECURE_NO_WARNINGS
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glad.c>
#include <math.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>

#include "game.cpp"

#ifdef PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef min
#undef max
#undef near
#undef far
#else
extern "C" const char *__asan_default_options() { return "detect_leaks=0"; }
#endif

#define THREAD_COUNT 3

void fatal_error(const char* message)
{
	LOG_FATAL("%s", message);

#ifdef PLATFORM_WIN32
	MessageBox(0, message, 0, MB_OK|MB_ICONERROR);
#elif defined PLATFORM_LINUX
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "zenity --error --no-wrap --text=\"%s\"", message);
    system(cmd);
#endif

    exit(1);
}

global bool g_hide_mouse = true;

void update_game_input(GLFWwindow *window, GameInput &input, int frame)
{
	int button_map[BUTTON_COUNT] = {};
	
	for (int i = 0; i < 26; i++)
		button_map[BUTTON_A + i] = GLFW_KEY_A + i;

	button_map[BUTTON_ESCAPE] = GLFW_KEY_ESCAPE;
	button_map[BUTTON_SPACE] = GLFW_KEY_SPACE;
	button_map[BUTTON_LEFT_SHIFT] = GLFW_KEY_LEFT_SHIFT;
	button_map[BUTTON_LEFT_CONTROL] = GLFW_KEY_LEFT_CONTROL;
	button_map[BUTTON_DELETE] = GLFW_KEY_DELETE;

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

int get_thread_id()
{
	return (int)(GetCurrentThreadId());
}

// returns the old value of ptr
int compare_exchange(volatile int *ptr, int new_value, int old_value)
{
#ifdef PLATFORM_WIN32
	return InterlockedCompareExchange((LONG *)ptr, new_value, old_value);
#else
#error "todo"
#endif
}

void lock_mutex(Mutex &mutex)
{
	int id = get_thread_id();
	if (mutex.value == id)
		return ;
	while (compare_exchange(&mutex.value, id, 0))
		;
}

void unlock_mutex(Mutex &mutex)
{
	//if (!mutex.value)
	//	return ;
	int id = get_thread_id();
	assert(compare_exchange(&mutex.value, 0, id) == id);
}

void release_semaphore(void *semaphore)
{
	ReleaseSemaphore(semaphore, 1, 0);
}

#define THREAD_MASK(x) ((x) & (ARRAY_SIZE(thread_work_queue) - 1))
ThreadWork thread_work_queue[256];
volatile int thread_work_queue_read_index;
volatile int thread_work_queue_write_index;
volatile int thread_work_queue_occupied_index;
void *thread_work_semaphore;

bool add_thread_work(ThreadWorkFn *callback, void *data)
{
	int index;
	while (1) {
		index = thread_work_queue_occupied_index;
		if (THREAD_MASK(index + 1) == THREAD_MASK(thread_work_queue_read_index)) {
			assert(0);
			return false;
		}
		
		if (compare_exchange(&thread_work_queue_occupied_index, index + 1, index) == index)
			break ;
	}

	ThreadWork *work = &thread_work_queue[THREAD_MASK(index)];

	work->callback = callback;
	work->data = data;

	while (compare_exchange(&thread_work_queue_write_index, index + 1, index) != index)
		;
	release_semaphore(thread_work_semaphore);
	return true;
}

#ifdef PLATFORM_WIN32
DWORD thread_func(LPVOID param)
{
	int idx = *(DWORD *)param;

	while (1) {
		int entry_idx = thread_work_queue_read_index;
		_ReadWriteBarrier(); // propably not necessary
		if (entry_idx != thread_work_queue_write_index) {
			if (compare_exchange(&thread_work_queue_read_index, entry_idx + 1, entry_idx) == entry_idx) 
			{
				ThreadWork *work = &thread_work_queue[THREAD_MASK(entry_idx)];
				work->callback(work->data);
			}
		}
		else
			WaitForSingleObject(thread_work_semaphore, INFINITE);
	}
	return 0;
}
#else
#error "todo"
#endif

void set_working_directory()
{
	Arena *temp = begin_temp_memory();

#ifdef PLATFORM_WIN32
	char exe_path[MAX_PATH];
    if (GetModuleFileNameA(NULL, exe_path, MAX_PATH) == 0)
        fatal_error("failed to get executable full path");

	String path = exe_path;
	path = substring(path, 0, find_last_occurence(path, '\\'));
	path = substring(path, 0, find_last_occurence(path, '\\'));

	if (SetCurrentDirectoryA(make_zero_string(temp, path).data) == 0)
		fatal_error("failed to set working directory");
#else
#error "todo"
#endif
	end_temp_memory();
}

int main()
{
	platform.allocate_memory = (PlatformAllocateFn *)malloc;
	platform.free_memory = (PlatformFreeFn *)free;
	platform.lock_mutex = lock_mutex;
	platform.unlock_mutex = unlock_mutex;
	platform.add_thread_work = add_thread_work;

	set_working_directory();

	Game game = {};

	ma_device sound_device;
	{

		ma_device_config config = ma_device_config_init(ma_device_type_playback);
		config.playback.format   = ma_format_f32;
		config.playback.channels = SOUND_CHANNEL_COUNT;
		config.sampleRate        = SOUND_SAMPLE_RATE;
		config.dataCallback      = audio_write_callback;
		config.pUserData         = &game;

		if (ma_device_init(NULL, &config, &sound_device) != MA_SUCCESS)
			LOG_ERROR("failed to init sound device");
		else
			ma_device_start(&sound_device);
	}
	if (!glfwInit())
		fatal_error("failed to init glfw");

#ifdef RENDERER_DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

	GLFWwindow *window = glfwCreateWindow(1600, 900, "game",  0, 0);
//	glfwSetWindowSize(window, 1920, 1080);
	if (!window)
		fatal_error("failed to create window");

	glfwMakeContextCurrent(window);
	//glfwSwapInterval(0);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		fatal_error("failed to init OpenGL loader");

	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	LOG_INFO("OpenGL backend");
	LOG_INFO("Vendor: %s", vendor);
	LOG_INFO("Renderer: %s", renderer);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	GameInput game_input = {};
	{
		double mouse_x, mouse_y;
		glfwGetCursorPos(window, &mouse_x, &mouse_y);
		game_input.mouse_p = game_input.last_mouse_p = V2(mouse_x, mouse_y);
	}

	if (g_hide_mouse)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	RenderContext render_context = {};
	{
		platform.render_context = &render_context;
		platform.imgui_context = ImGui::GetCurrentContext();
	}

	LOG_INFO("lanching %d threads", THREAD_COUNT);

#ifdef PLATFORM_WIN32
	thread_work_semaphore = CreateSemaphoreA(0, 0, ARRAY_SIZE(thread_work_queue), 0);
	DWORD thread_ids[THREAD_COUNT];
	for (int i = 0; i < THREAD_COUNT; i++)
		CreateThread(0, 0, thread_func, &thread_ids[i], 0, &thread_ids[i]);
#else
	#error "todo"
#endif

	int frame = 0;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		update_game_input(window, game_input, frame);
		if (IsDown(game_input, BUTTON_ESCAPE))
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		glfwGetFramebufferSize(window, &render_context.window_width, &render_context.window_height);
		// TODO: https://gafferongames.com/post/fix_your_timestep
		float dt = 1.f / 60;
		game_update_and_render(game, game_input, dt);

		glfwSwapBuffers(window);
		frame++;
	}

	ma_device_uninit(&sound_device);
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
}
