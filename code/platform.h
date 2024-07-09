enum GameButtonType
{
	BUTTON_CAMERA_FORWARD,
	BUTTON_CAMERA_BACKWARD,
	BUTTON_CAMERA_LEFT,
	BUTTON_CAMERA_RIGHT,
	BUTTON_CAMERA_UP,
	BUTTON_CAMERA_DOWN,
	BUTTON_MOUSE_LEFT,
	BUTTON_MOUSE_RIGHT,
	BUTTON_COUNT
};

struct GameButton 
{
	b32 is_down;
	b32 was_down;
};

struct GameInput
{
	GameButton buttons[BUTTON_COUNT];
	v2 mouse_p;
	v2 last_mouse_p;
};

#define IsDown(input, button) (input.buttons[button].is_down)
