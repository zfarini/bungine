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
	BUTTON_PLAYER_FORWARD,
	BUTTON_PLAYER_BACKWARD,
	BUTTON_PLAYER_JUMP,
	BUTTON_LEFT_SHIFT,

	BUTTON_F1, BUTTON_F2, BUTTON_F3, BUTTON_F4, BUTTON_F5,
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
	v2 mouse_dp;
	//v2 last_mouse_p;
};

#define IsDown(input, button) (input.buttons[button].is_down)
#define WasDown(input, button) (input.buttons[button].was_down)
#define IsDownFirstTime(input, button) (IsDown(input, button) && !WasDown(input, button))