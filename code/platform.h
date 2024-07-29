#pragma once

enum GameButtonType
{
	BUTTON_A, BUTTON_B, BUTTON_C, BUTTON_D, BUTTON_E, BUTTON_F, BUTTON_G, BUTTON_H, BUTTON_I,
	BUTTON_J, BUTTON_K, BUTTON_L, BUTTON_M, BUTTON_N, BUTTON_O, BUTTON_P, BUTTON_Q, BUTTON_R,
	BUTTON_S, BUTTON_T, BUTTON_U, BUTTON_V, BUTTON_W, BUTTON_X, BUTTON_Y, BUTTON_Z,

	BUTTON_LEFT_CONTROL,
	BUTTON_LEFT_SHIFT,
	BUTTON_MOUSE_LEFT,
	BUTTON_MOUSE_RIGHT,
	BUTTON_SPACE,
	BUTTON_F1, BUTTON_F2, BUTTON_F3, BUTTON_F4, BUTTON_F5, BUTTON_F6, 
	BUTTON_F7, BUTTON_F8, BUTTON_F9, BUTTON_F10, BUTTON_F11, BUTTON_F12,
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

struct GameButton 
{
	b32 is_down;
	b32 was_down;
};

struct GameInput
{
	GameButton buttons[BUTTON_COUNT];
	v2 mouse_dp;
	v2 mouse_p;
	v2 last_mouse_p;
};

#define IsDown(input, button) (input.buttons[button].is_down)
#define WasDown(input, button) (input.buttons[button].was_down)
#define IsDownFirstTime(input, button) (IsDown(input, button) && !WasDown(input, button))


#ifdef RENDERER_OPENGL
#define GL_FUNCTIONS(X) \
    X(PFNGLENABLEPROC,                   glEnable                   ) \
    X(PFNGLDISABLEPROC,                  glDisable                  ) \
    X(PFNGLBLENDFUNCPROC,                glBlendFunc                ) \
    X(PFNGLVIEWPORTPROC,                 glViewport                 ) \
    X(PFNGLCLEARCOLORPROC,               glClearColor               ) \
    X(PFNGLCLEARPROC,                    glClear                    ) \
    X(PFNGLDRAWARRAYSPROC,               glDrawArrays               ) \
    X(PFNGLCREATEBUFFERSPROC,            glCreateBuffers            ) \
    X(PFNGLNAMEDBUFFERSTORAGEPROC,       glNamedBufferStorage       ) \
    X(PFNGLBINDVERTEXARRAYPROC,          glBindVertexArray          ) \
    X(PFNGLCREATEVERTEXARRAYSPROC,       glCreateVertexArrays       ) \
    X(PFNGLVERTEXARRAYATTRIBBINDINGPROC, glVertexArrayAttribBinding ) \
    X(PFNGLVERTEXARRAYVERTEXBUFFERPROC,  glVertexArrayVertexBuffer  ) \
    X(PFNGLVERTEXARRAYATTRIBFORMATPROC,  glVertexArrayAttribFormat  ) \
    X(PFNGLENABLEVERTEXARRAYATTRIBPROC,  glEnableVertexArrayAttrib  ) \
    X(PFNGLCREATESHADERPROGRAMVPROC,     glCreateShaderProgramv     ) \
    X(PFNGLGETPROGRAMIVPROC,             glGetProgramiv             ) \
    X(PFNGLGETPROGRAMINFOLOGPROC,        glGetProgramInfoLog        ) \
    X(PFNGLGENPROGRAMPIPELINESPROC,      glGenProgramPipelines      ) \
    X(PFNGLUSEPROGRAMSTAGESPROC,         glUseProgramStages         ) \
    X(PFNGLBINDPROGRAMPIPELINEPROC,      glBindProgramPipeline      ) \
    X(PFNGLPROGRAMUNIFORMMATRIX2FVPROC,  glProgramUniformMatrix2fv  ) \
    X(PFNGLBINDTEXTUREUNITPROC,          glBindTextureUnit          ) \
    X(PFNGLCREATETEXTURESPROC,           glCreateTextures           ) \
    X(PFNGLTEXTUREPARAMETERIPROC,        glTextureParameteri        ) \
    X(PFNGLTEXTURESTORAGE2DPROC,         glTextureStorage2D         ) \
    X(PFNGLTEXTURESUBIMAGE2DPROC,        glTextureSubImage2D        ) \
    X(PFNGLDEBUGMESSAGECALLBACKPROC,     glDebugMessageCallback     ) \
    X(PFNGLGETINTEGERVPROC,     		 glGetIntegerv     			) \
    X(PFNGLDEBUGMESSAGECONTROLPROC,      glDebugMessageControl     	) \
    X(PFNGLCREATESHADERPROC,      		 glCreateShader     		) \
    X(PFNGLSHADERSOURCEPROC,      		 glShaderSource     		) \
    X(PFNGLCOMPILESHADERPROC,      		 glCompileShader     		) \
    X(PFNGLGETSHADERIVPROC,      		 glGetShaderiv     			) \
    X(PFNGLGETSHADERINFOLOGPROC,      	 glGetShaderInfoLog     	) \
    X(PFNGLACTIVETEXTUREPROC,      	 	 glActiveTexture     		) \
    X(PFNGLBINDTEXTUREPROC,      	 	 glBindTexture     			) \
    X(PFNGLGENTEXTURESPROC,      	 	 glGenTextures     			) \
    X(PFNGLTEXIMAGE2DPROC,      	 	 glTexImage2D				) \
    X(PFNGLGENERATEMIPMAPPROC,      	 glGenerateMipmap			) \
    X(PFNGLTEXPARAMETERIPROC,      	 	 glTexParameteri     		) \
    X(PFNGLCLEARDEPTHPROC,      	 	 glClearDepth     			) \
    X(PFNGLBINDFRAMEBUFFERPROC,      	 glBindFrameBuffer     		) \
    X(PFNGLCULLFACEPROC,      	 		 glCullFace     			) \
    X(PFNGLCREATEPROGRAMPROC,      	 	 glCreateProgram     		) \
    X(PFNGLATTACHSHADERPROC,      	 	 glAttachShader     		) \
    X(PFNGLLINKPROGRAMPROC,      	 	 glLinkProgram     			) \
    X(PFNGLUSEPROGRAMPROC,      	 	 glUseProgram     			) \
    X(PFNGLGENVERTEXARRAYSPROC,      	 glGenVertexArrays     		) \
	X(PFNGLGENBUFFERSPROC,      	 	 glGenBuffers     			) \
	X(PFNGLBINDBUFFERPROC,      	 	 glBindBuffer     			) \
    X(PFNGLBINDBUFFERBASEPROC,      	 glBindBufferBase     		) \
    X(PFNGLBUFFERDATAPROC,      	 	 glBufferData     			) \
    X(PFNGLBUFFERSUBDATAPROC,      	 	 glBufferSubData     		) \
    X(PFNGLVERTEXATTRIBPOINTERPROC,      glVertexAttribPointer     	) \
    X(PFNGLENABLEVERTEXATTRIBARRAYPROC,  glEnableVertexAttribArray  ) \
    X(PFNGLPOLYGONMODEPROC,      	 	 glPolygonMode     			) 
#endif

struct Platform {
	void *render_context;
	void *imgui_context;
	TempArena temp_arena;
#if RENDERER_OPENGL
	GLFWwindow *window;
#endif
};

#define GAME_UPDATE_AND_RENDER(name) void name(Platform &platform, Arena *memory, GameInput &input, float dt)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render_fn);
