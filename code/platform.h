#pragma once

enum GameButtonType {
    BUTTON_A,
    BUTTON_B,
    BUTTON_C,
    BUTTON_D,
    BUTTON_E,
    BUTTON_F,
    BUTTON_G,
    BUTTON_H,
    BUTTON_I,
    BUTTON_J,
    BUTTON_K,
    BUTTON_L,
    BUTTON_M,
    BUTTON_N,
    BUTTON_O,
    BUTTON_P,
    BUTTON_Q,
    BUTTON_R,
    BUTTON_S,
    BUTTON_T,
    BUTTON_U,
    BUTTON_V,
    BUTTON_W,
    BUTTON_X,
    BUTTON_Y,
    BUTTON_Z,

	BUTTON_DELETE,
    BUTTON_LEFT_CONTROL,
    BUTTON_LEFT_SHIFT,
    BUTTON_MOUSE_LEFT,
    BUTTON_MOUSE_RIGHT,
    BUTTON_SPACE,
    BUTTON_ESCAPE,
    BUTTON_F1,
    BUTTON_F2,
    BUTTON_F3,
    BUTTON_F4,
    BUTTON_F5,
    BUTTON_F6,
    BUTTON_F7,
    BUTTON_F8,
    BUTTON_F9,
    BUTTON_F10,
    BUTTON_F11,
    BUTTON_F12,
    BUTTON_COUNT
};

#define BUTTON_CAMERA_FORWARD BUTTON_W
#define BUTTON_CAMERA_BACKWARD BUTTON_S
#define BUTTON_CAMERA_LEFT BUTTON_A
#define BUTTON_CAMERA_RIGHT BUTTON_D
#define BUTTON_CAMERA_UP BUTTON_Q
#define BUTTON_CAMERA_DOWN BUTTON_E
#define BUTTON_PLAYER_FORWARD BUTTON_F
#define BUTTON_PLAYER_BACKWARD BUTTON_G
#define BUTTON_PLAYER_JUMP BUTTON_SPACE
#define TOGGLE_EDITOR_BUTTON BUTTON_F1

struct GameButton {
    b32 is_down;
    b32 was_down;
};

struct GameInput {
    GameButton buttons[BUTTON_COUNT];
    v2 mouse_dp;
    v2 mouse_p;
    v2 last_mouse_p;
};

#define IsDown(input, button) (input.buttons[button].is_down)
#define IsKeyboardButton(button) (button != BUTTON_MOUSE_LEFT && button != BUTTON_MOUSE_RIGHT)
#define WasDown(input, button) (input.buttons[button].was_down)
#define IsDownFirstTime(input, button)                                         \
    (IsDown(input, button) && !WasDown(input, button))


#define THREAD_WORK_FUNC(name) void name(void *data)

typedef THREAD_WORK_FUNC(ThreadWorkFn);

struct ThreadWork {
	ThreadWorkFn *callback;
	void *data;
};



global ThreadWork thread_work_queue[256];
#define THREAD_MASK(x) ((x) & (ARRAY_SIZE(thread_work_queue) - 1))
global volatile int thread_work_queue_read_index;
global volatile int thread_work_queue_write_index;
global volatile int thread_work_queue_occupied_index;
global void *thread_work_semaphore;

typedef bool AddThreadWorkFn(ThreadWorkFn *callback, void *data);

typedef void *PlatformAllocateFn(usize size);
typedef void PlatformFreeFn(void *);

struct RenderContext;

struct Platform {
    RenderContext *render_context;
    void *imgui_context;

    AddThreadWorkFn *add_thread_work;

    PlatformAllocateFn *allocate_memory;
    PlatformFreeFn *free_memory;

    Arena temp_arena;
    struct {
        memory_block *block;
		usize used;
    } temp_arena_last_used[16];
    int temp_arena_last_used_count;
};

global Platform platform;

