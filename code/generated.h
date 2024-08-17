#ifndef DISABLE_PREPROCESSOR
struct StructMemberData {
    const char *name;
    const char *type_name;
    int is_array;
    int array_size;
};
struct StructMetaData {
    const char *name;
    int member_count;
    StructMemberData members[64];
};
void serialize_quat(FILE *fd, bool w, quat &x, Arena *arena = 0) {
    if (w)
        fwrite(&x, sizeof(quat), 1, fd);
    else {
        (void)fread(&x, sizeof(quat), 1, fd);
    }
}
void serialize_v3(FILE *fd, bool w, v3 &x, Arena *arena = 0) {
    if (w)
        fwrite(&x, sizeof(v3), 1, fd);
    else {
        (void)fread(&x, sizeof(v3), 1, fd);
    }
}
void serialize_int(FILE *fd, bool w, int &x, Arena *arena = 0) {
    if (w)
        fwrite(&x, sizeof(int), 1, fd);
    else {
        (void)fread(&x, sizeof(int), 1, fd);
    }
}
void serialize_int32_t(FILE *fd, bool w, int32_t &x, Arena *arena = 0) {
    if (w)
        fwrite(&x, sizeof(int32_t), 1, fd);
    else {
        (void)fread(&x, sizeof(int32_t), 1, fd);
    }
}
void serialize_size_t(FILE *fd, bool w, size_t &x, Arena *arena = 0) {
    if (w)
        fwrite(&x, sizeof(size_t), 1, fd);
    else {
        (void)fread(&x, sizeof(size_t), 1, fd);
    }
}
void serialize_char(FILE *fd, bool w, char &x, Arena *arena = 0) {
    if (w)
        fwrite(&x, sizeof(char), 1, fd);
    else {
        (void)fread(&x, sizeof(char), 1, fd);
    }
}
void serialize_bool(FILE *fd, bool w, bool &x, Arena *arena = 0) {
    int v = x;
    if (w)
        fwrite(&v, sizeof(int), 1, fd);
    else {
        (void)fread(&v, sizeof(int), 1, fd);
        x = v;
    }
}
void serialize_v2(FILE *fd, bool w, v2 &x, Arena *arena = 0) {
    if (w)
        fwrite(&x, sizeof(v2), 1, fd);
    else {
        (void)fread(&x, sizeof(v2), 1, fd);
    }
}
void serialize_mat4(FILE *fd, bool w, mat4 &x, Arena *arena = 0) {
    if (w)
        fwrite(&x, sizeof(mat4), 1, fd);
    else {
        (void)fread(&x, sizeof(mat4), 1, fd);
    }
}
void serialize_float(FILE *fd, bool w, float &x, Arena *arena = 0) {
    if (w)
        fwrite(&x, sizeof(float), 1, fd);
    else {
        (void)fread(&x, sizeof(float), 1, fd);
    }
}
void serialize_double(FILE *fd, bool w, double &x, Arena *arena = 0) {
    if (w)
        fwrite(&x, sizeof(double), 1, fd);
    else {
        (void)fread(&x, sizeof(double), 1, fd);
    }
}
static StructMetaData get_struct_ThreadWork_info();
static void imgui_edit_struct_ThreadWork(ThreadWork &x, const char *name,
                                         bool collapsed);
static void serialize_ThreadWork(FILE *fd, bool w, ThreadWork &x, Arena *arena);
static StructMetaData get_struct_Mesh_info();
static void imgui_edit_struct_Mesh(Mesh &x, const char *name, bool collapsed);
static void serialize_Mesh(FILE *fd, bool w, Mesh &x, Arena *arena);
static StructMetaData get_struct_memory_block_info();
static void imgui_edit_struct_memory_block(memory_block &x, const char *name,
                                           bool collapsed);
static void serialize_memory_block(FILE *fd, bool w, memory_block &x,
                                   Arena *arena);
static StructMetaData get_struct_Mutex_info();
static void imgui_edit_struct_Mutex(Mutex &x, const char *name, bool collapsed);
static void serialize_Mutex(FILE *fd, bool w, Mutex &x, Arena *arena);
static StructMetaData get_struct_Material_info();
static void imgui_edit_struct_Material(Material &x, const char *name,
                                       bool collapsed);
static void serialize_Material(FILE *fd, bool w, Material &x, Arena *arena);
static StructMetaData get_struct_RenderPass_info();
static void imgui_edit_struct_RenderPass(RenderPass &x, const char *name,
                                         bool collapsed);
static void serialize_RenderPass(FILE *fd, bool w, RenderPass &x, Arena *arena);
static StructMetaData get_struct_TempMemory_info();
static void imgui_edit_struct_TempMemory(TempMemory &x, const char *name,
                                         bool collapsed);
static void serialize_TempMemory(FILE *fd, bool w, TempMemory &x, Arena *arena);
static StructMetaData get_struct_MeshTriangle_info();
static void imgui_edit_struct_MeshTriangle(MeshTriangle &x, const char *name,
                                           bool collapsed);
static void serialize_MeshTriangle(FILE *fd, bool w, MeshTriangle &x,
                                   Arena *arena);
static StructMetaData get_struct_Shader_info();
static void imgui_edit_struct_Shader(Shader &x, const char *name,
                                     bool collapsed);
static void serialize_Shader(FILE *fd, bool w, Shader &x, Arena *arena);
static StructMetaData get_struct_Arena_info();
static void imgui_edit_struct_Arena(Arena &x, const char *name, bool collapsed);
static void serialize_Arena(FILE *fd, bool w, Arena &x, Arena *arena);
static StructMetaData get_struct_VertexInputElement_info();
static void imgui_edit_struct_VertexInputElement(VertexInputElement &x,
                                                 const char *name,
                                                 bool collapsed);
static void serialize_VertexInputElement(FILE *fd, bool w,
                                         VertexInputElement &x, Arena *arena);
static StructMetaData get_struct_GameButton_info();
static void imgui_edit_struct_GameButton(GameButton &x, const char *name,
                                         bool collapsed);
static void serialize_GameButton(FILE *fd, bool w, GameButton &x, Arena *arena);
static StructMetaData get_struct_GameInput_info();
static void imgui_edit_struct_GameInput(GameInput &x, const char *name,
                                        bool collapsed);
static void serialize_GameInput(FILE *fd, bool w, GameInput &x, Arena *arena);
static StructMetaData get_struct_SoundPlaying_info();
static void imgui_edit_struct_SoundPlaying(SoundPlaying &x, const char *name,
                                           bool collapsed);
static void serialize_SoundPlaying(FILE *fd, bool w, SoundPlaying &x,
                                   Arena *arena);
static StructMetaData get_struct_Platform_info();
static void imgui_edit_struct_Platform(Platform &x, const char *name,
                                       bool collapsed);
static void serialize_Platform(FILE *fd, bool w, Platform &x, Arena *arena);
static StructMetaData get_struct_Editor_info();
static void imgui_edit_struct_Editor(Editor &x, const char *name,
                                     bool collapsed);
static void serialize_Editor(FILE *fd, bool w, Editor &x, Arena *arena);
static StructMetaData get_struct_String_info();
static void imgui_edit_struct_String(String &x, const char *name,
                                     bool collapsed);
static void serialize_String(FILE *fd, bool w, String &x, Arena *arena);
static StructMetaData get_struct_VertexInputLayout_info();
static void imgui_edit_struct_VertexInputLayout(VertexInputLayout &x,
                                                const char *name,
                                                bool collapsed);
static void serialize_VertexInputLayout(FILE *fd, bool w, VertexInputLayout &x,
                                        Arena *arena);
static StructMetaData get_struct_StringHasher_info();
static void imgui_edit_struct_StringHasher(StringHasher &x, const char *name,
                                           bool collapsed);
static void serialize_StringHasher(FILE *fd, bool w, StringHasher &x,
                                   Arena *arena);
static StructMetaData get_struct_Texture_info();
static void imgui_edit_struct_Texture(Texture &x, const char *name,
                                      bool collapsed);
static void serialize_Texture(FILE *fd, bool w, Texture &x, Arena *arena);
static StructMetaData get_struct_ShadowMap_info();
static void imgui_edit_struct_ShadowMap(ShadowMap &x, const char *name,
                                        bool collapsed);
static void serialize_ShadowMap(FILE *fd, bool w, ShadowMap &x, Arena *arena);
static StructMetaData get_struct_Animation_info();
static void imgui_edit_struct_Animation(Animation &x, const char *name,
                                        bool collapsed);
static void serialize_Animation(FILE *fd, bool w, Animation &x, Arena *arena);
static StructMetaData get_struct_VertexBuffer_info();
static void imgui_edit_struct_VertexBuffer(VertexBuffer &x, const char *name,
                                           bool collapsed);
static void serialize_VertexBuffer(FILE *fd, bool w, VertexBuffer &x,
                                   Arena *arena);
static StructMetaData get_struct_IndexBuffer_info();
static void imgui_edit_struct_IndexBuffer(IndexBuffer &x, const char *name,
                                          bool collapsed);
static void serialize_IndexBuffer(FILE *fd, bool w, IndexBuffer &x,
                                  Arena *arena);
static StructMetaData get_struct_FrameBuffer_info();
static void imgui_edit_struct_FrameBuffer(FrameBuffer &x, const char *name,
                                          bool collapsed);
static void serialize_FrameBuffer(FILE *fd, bool w, FrameBuffer &x,
                                  Arena *arena);
static StructMetaData get_struct_DepthStencilState_info();
static void imgui_edit_struct_DepthStencilState(DepthStencilState &x,
                                                const char *name,
                                                bool collapsed);
static void serialize_DepthStencilState(FILE *fd, bool w, DepthStencilState &x,
                                        Arena *arena);
static StructMetaData get_struct_StateHasher_info();
static void imgui_edit_struct_StateHasher(StateHasher &x, const char *name,
                                          bool collapsed);
static void serialize_StateHasher(FILE *fd, bool w, StateHasher &x,
                                  Arena *arena);
static StructMetaData get_struct_ProfilerBlock_info();
static void imgui_edit_struct_ProfilerBlock(ProfilerBlock &x, const char *name,
                                            bool collapsed);
static void serialize_ProfilerBlock(FILE *fd, bool w, ProfilerBlock &x,
                                    Arena *arena);
static StructMetaData get_struct_ConstantBufferElement_info();
static void imgui_edit_struct_ConstantBufferElement(ConstantBufferElement &x,
                                                    const char *name,
                                                    bool collapsed);
static void serialize_ConstantBufferElement(FILE *fd, bool w,
                                            ConstantBufferElement &x,
                                            Arena *arena);
static StructMetaData get_struct_RasterizerState_info();
static void imgui_edit_struct_RasterizerState(RasterizerState &x,
                                              const char *name, bool collapsed);
static void serialize_RasterizerState(FILE *fd, bool w, RasterizerState &x,
                                      Arena *arena);
static StructMetaData get_struct_RenderContext_info();
static void imgui_edit_struct_RenderContext(RenderContext &x, const char *name,
                                            bool collapsed);
static void serialize_RenderContext(FILE *fd, bool w, RenderContext &x,
                                    Arena *arena);
static StructMetaData get_struct_CollisionInfo_info();
static void imgui_edit_struct_CollisionInfo(CollisionInfo &x, const char *name,
                                            bool collapsed);
static void serialize_CollisionInfo(FILE *fd, bool w, CollisionInfo &x,
                                    Arena *arena);
static StructMetaData get_struct_ConstantBuffer_info();
static void imgui_edit_struct_ConstantBuffer(ConstantBuffer &x,
                                             const char *name, bool collapsed);
static void serialize_ConstantBuffer(FILE *fd, bool w, ConstantBuffer &x,
                                     Arena *arena);
static StructMetaData get_struct_Game_info();
static void imgui_edit_struct_Game(Game &x, const char *name, bool collapsed);
static void serialize_Game(FILE *fd, bool w, Game &x, Arena *arena);
static StructMetaData get_struct_Vertex_info();
static void imgui_edit_struct_Vertex(Vertex &x, const char *name,
                                     bool collapsed);
static void serialize_Vertex(FILE *fd, bool w, Vertex &x, Arena *arena);
static StructMetaData get_struct_MeshPart_info();
static void imgui_edit_struct_MeshPart(MeshPart &x, const char *name,
                                       bool collapsed);
static void serialize_MeshPart(FILE *fd, bool w, MeshPart &x, Arena *arena);
static StructMetaData get_struct_Bone_info();
static void imgui_edit_struct_Bone(Bone &x, const char *name, bool collapsed);
static void serialize_Bone(FILE *fd, bool w, Bone &x, Arena *arena);
static StructMetaData get_struct_NodeAnimation_info();
static void imgui_edit_struct_NodeAnimation(NodeAnimation &x, const char *name,
                                            bool collapsed);
static void serialize_NodeAnimation(FILE *fd, bool w, NodeAnimation &x,
                                    Arena *arena);
static StructMetaData get_struct_SceneNode_info();
static void imgui_edit_struct_SceneNode(SceneNode &x, const char *name,
                                        bool collapsed);
static void serialize_SceneNode(FILE *fd, bool w, SceneNode &x, Arena *arena);
static StructMetaData get_struct_World_info();
static void imgui_edit_struct_World(World &x, const char *name, bool collapsed);
static void serialize_World(FILE *fd, bool w, World &x, Arena *arena);
static StructMetaData get_struct_Scene_info();
static void imgui_edit_struct_Scene(Scene &x, const char *name, bool collapsed);
static void serialize_Scene(FILE *fd, bool w, Scene &x, Arena *arena);
static StructMetaData get_struct_Constants_info();
static void imgui_edit_struct_Constants(Constants &x, const char *name,
                                        bool collapsed);
static void serialize_Constants(FILE *fd, bool w, Constants &x, Arena *arena);
static StructMetaData get_struct_CollisionMesh_info();
static void imgui_edit_struct_CollisionMesh(CollisionMesh &x, const char *name,
                                            bool collapsed);
static void serialize_CollisionMesh(FILE *fd, bool w, CollisionMesh &x,
                                    Arena *arena);
static StructMetaData get_struct_CollisionShape_info();
static void imgui_edit_struct_CollisionShape(CollisionShape &x,
                                             const char *name, bool collapsed);
static void serialize_CollisionShape(FILE *fd, bool w, CollisionShape &x,
                                     Arena *arena);
static StructMetaData get_struct_Entity_info();
static void imgui_edit_struct_Entity(Entity &x, const char *name,
                                     bool collapsed);
static void serialize_Entity(FILE *fd, bool w, Entity &x, Arena *arena);
static StructMetaData get_struct_Camera_info();
static void imgui_edit_struct_Camera(Camera &x, const char *name,
                                     bool collapsed);
static void serialize_Camera(FILE *fd, bool w, Camera &x, Arena *arena);
static StructMetaData get_struct_ProfilerBlockStat_info();
static void imgui_edit_struct_ProfilerBlockStat(ProfilerBlockStat &x,
                                                const char *name,
                                                bool collapsed);
static void serialize_ProfilerBlockStat(FILE *fd, bool w, ProfilerBlockStat &x,
                                        Arena *arena);
static StructMetaData get_struct_EditorOp_info();
static void imgui_edit_struct_EditorOp(EditorOp &x, const char *name,
                                       bool collapsed);
static void serialize_EditorOp(FILE *fd, bool w, EditorOp &x, Arena *arena);
static StructMetaData get_struct_Gizmo_info();
static void imgui_edit_struct_Gizmo(Gizmo &x, const char *name, bool collapsed);
static void serialize_Gizmo(FILE *fd, bool w, Gizmo &x, Arena *arena);
static StructMetaData get_struct_LoadedSound_info();
static void imgui_edit_struct_LoadedSound(LoadedSound &x, const char *name,
                                          bool collapsed);
static void serialize_LoadedSound(FILE *fd, bool w, LoadedSound &x,
                                  Arena *arena);
static StructMetaData get_struct_SoundState_info();
static void imgui_edit_struct_SoundState(SoundState &x, const char *name,
                                         bool collapsed);
static void serialize_SoundState(FILE *fd, bool w, SoundState &x, Arena *arena);
static StructMetaData get_struct_State_info();
static void imgui_edit_struct_State(State &x, const char *name, bool collapsed);
static void serialize_State(FILE *fd, bool w, State &x, Arena *arena);
static const char *get_enum_GameButtonType_str(int value) {
    switch (value) {
    case BUTTON_A:
        return "BUTTON_A";
    case BUTTON_B:
        return "BUTTON_B";
    case BUTTON_C:
        return "BUTTON_C";
    case BUTTON_D:
        return "BUTTON_D";
    case BUTTON_E:
        return "BUTTON_E";
    case BUTTON_F:
        return "BUTTON_F";
    case BUTTON_G:
        return "BUTTON_G";
    case BUTTON_H:
        return "BUTTON_H";
    case BUTTON_I:
        return "BUTTON_I";
    case BUTTON_J:
        return "BUTTON_J";
    case BUTTON_K:
        return "BUTTON_K";
    case BUTTON_L:
        return "BUTTON_L";
    case BUTTON_M:
        return "BUTTON_M";
    case BUTTON_N:
        return "BUTTON_N";
    case BUTTON_O:
        return "BUTTON_O";
    case BUTTON_P:
        return "BUTTON_P";
    case BUTTON_Q:
        return "BUTTON_Q";
    case BUTTON_R:
        return "BUTTON_R";
    case BUTTON_S:
        return "BUTTON_S";
    case BUTTON_T:
        return "BUTTON_T";
    case BUTTON_U:
        return "BUTTON_U";
    case BUTTON_V:
        return "BUTTON_V";
    case BUTTON_W:
        return "BUTTON_W";
    case BUTTON_X:
        return "BUTTON_X";
    case BUTTON_Y:
        return "BUTTON_Y";
    case BUTTON_Z:
        return "BUTTON_Z";
    case BUTTON_DELETE:
        return "BUTTON_DELETE";
    case BUTTON_LEFT_CONTROL:
        return "BUTTON_LEFT_CONTROL";
    case BUTTON_LEFT_SHIFT:
        return "BUTTON_LEFT_SHIFT";
    case BUTTON_MOUSE_LEFT:
        return "BUTTON_MOUSE_LEFT";
    case BUTTON_MOUSE_RIGHT:
        return "BUTTON_MOUSE_RIGHT";
    case BUTTON_SPACE:
        return "BUTTON_SPACE";
    case BUTTON_ESCAPE:
        return "BUTTON_ESCAPE";
    case BUTTON_F1:
        return "BUTTON_F1";
    case BUTTON_F2:
        return "BUTTON_F2";
    case BUTTON_F3:
        return "BUTTON_F3";
    case BUTTON_F4:
        return "BUTTON_F4";
    case BUTTON_F5:
        return "BUTTON_F5";
    case BUTTON_F6:
        return "BUTTON_F6";
    case BUTTON_F7:
        return "BUTTON_F7";
    case BUTTON_F8:
        return "BUTTON_F8";
    case BUTTON_F9:
        return "BUTTON_F9";
    case BUTTON_F10:
        return "BUTTON_F10";
    case BUTTON_F11:
        return "BUTTON_F11";
    case BUTTON_F12:
        return "BUTTON_F12";
    case BUTTON_COUNT:
        return "BUTTON_COUNT";
    }
    return "Enum_GameButtonType_Unknown";
}
static const char *get_enum_TextureState_str(int value) {
    switch (value) {
    case TEXTURE_STATE_UNLOADED:
        return "TEXTURE_STATE_UNLOADED";
    case TEXTURE_STATE_LOADING:
        return "TEXTURE_STATE_LOADING";
    case TEXTURE_STATE_LOADED:
        return "TEXTURE_STATE_LOADED";
    }
    return "Enum_TextureState_Unknown";
}
static const char *get_enum_LogType_str(int value) {
    switch (value) {
    case LOG_TYPE_DEBUG:
        return "LOG_TYPE_DEBUG";
    case LOG_TYPE_INFO:
        return "LOG_TYPE_INFO";
    case LOG_TYPE_WARN:
        return "LOG_TYPE_WARN";
    case LOG_TYPE_ERROR:
        return "LOG_TYPE_ERROR";
    case LOG_TYPE_FATAL:
        return "LOG_TYPE_FATAL";
    }
    return "Enum_LogType_Unknown";
}
static const char *get_enum_VertexBufferUsage_str(int value) {
    switch (value) {
    case VERTEX_BUFFER_IMMUTABLE:
        return "VERTEX_BUFFER_IMMUTABLE";
    case VERTEX_BUFFER_DYNAMIC:
        return "VERTEX_BUFFER_DYNAMIC";
    }
    return "Enum_VertexBufferUsage_Unknown";
}
static const char *get_enum_ShaderType_str(int value) {
    switch (value) {
    case SHADER_TYPE_VERTEX:
        return "SHADER_TYPE_VERTEX";
    case SHADER_TYPE_FRAGMENT:
        return "SHADER_TYPE_FRAGMENT";
    }
    return "Enum_ShaderType_Unknown";
}
static const char *get_enum_ConstantBufferElementType_str(int value) {
    switch (value) {
    case CONSTANT_BUFFER_ELEMENT_MAT4:
        return "CONSTANT_BUFFER_ELEMENT_MAT4";
    case CONSTANT_BUFFER_ELEMENT_VEC4:
        return "CONSTANT_BUFFER_ELEMENT_VEC4";
    case CONSTANT_BUFFER_ELEMENT_VEC3:
        return "CONSTANT_BUFFER_ELEMENT_VEC3";
    case CONSTANT_BUFFER_ELEMENT_VEC2:
        return "CONSTANT_BUFFER_ELEMENT_VEC2";
    case CONSTANT_BUFFER_ELEMENT_FLOAT:
        return "CONSTANT_BUFFER_ELEMENT_FLOAT";
    case CONSTANT_BUFFER_ELEMENT_INT:
        return "CONSTANT_BUFFER_ELEMENT_INT";
    case CONSTANT_BUFFER_ELEMENT_COUNT:
        return "CONSTANT_BUFFER_ELEMENT_COUNT";
    }
    return "Enum_ConstantBufferElementType_Unknown";
}
static const char *get_enum_InputElementType_str(int value) {
    switch (value) {
    case INPUT_ELEMENT_FLOAT:
        return "INPUT_ELEMENT_FLOAT";
    case INPUT_ELEMENT_SIGNED_INT:
        return "INPUT_ELEMENT_SIGNED_INT";
    }
    return "Enum_InputElementType_Unknown";
}
static const char *get_enum_RasterizerFillMode_str(int value) {
    switch (value) {
    case RASTERIZER_FILL_SOLID:
        return "RASTERIZER_FILL_SOLID";
    case RASTERIZER_FILL_WIREFRAME:
        return "RASTERIZER_FILL_WIREFRAME";
    }
    return "Enum_RasterizerFillMode_Unknown";
}
static const char *get_enum_PrimitiveType_str(int value) {
    switch (value) {
    case PRIMITIVE_TRIANGLES:
        return "PRIMITIVE_TRIANGLES";
    case PRIMITIVE_LINES:
        return "PRIMITIVE_LINES";
    }
    return "Enum_PrimitiveType_Unknown";
}
static const char *get_enum_EditorOpType_str(int value) {
    switch (value) {
    case EDITOR_OP_TRANSLATE_ENTITY:
        return "EDITOR_OP_TRANSLATE_ENTITY";
    case EDITOR_OP_ROTATE_ENTITY:
        return "EDITOR_OP_ROTATE_ENTITY";
    case EDITOR_OP_SCALE_ENTITY:
        return "EDITOR_OP_SCALE_ENTITY";
    case EDITOR_OP_PASTE_ENTITY:
        return "EDITOR_OP_PASTE_ENTITY";
    case EDITOR_OP_DELETE_ENTITY:
        return "EDITOR_OP_DELETE_ENTITY";
    case EDITOR_OP_SPAWN_ENTITY:
        return "EDITOR_OP_SPAWN_ENTITY";
    case EDITOR_OP_CREATE_MESH_COLLISION_VERTEX:
        return "EDITOR_OP_CREATE_MESH_COLLISION_VERTEX";
    case EDITOR_OP_DELETE_MESH_COLLISION_TRIANGLE:
        return "EDITOR_OP_DELETE_MESH_COLLISION_TRIANGLE";
    }
    return "Enum_EditorOpType_Unknown";
}
static const char *get_enum_RasterizerCullMode_str(int value) {
    switch (value) {
    case RASTERIZER_CULL_NONE:
        return "RASTERIZER_CULL_NONE";
    case RASTERIZER_CULL_FRONT:
        return "RASTERIZER_CULL_FRONT";
    case RASTERIZER_CULL_BACK:
        return "RASTERIZER_CULL_BACK";
    }
    return "Enum_RasterizerCullMode_Unknown";
}
static const char *get_enum_SceneState_str(int value) {
    switch (value) {
    case SCENE_UNLOADED:
        return "SCENE_UNLOADED";
    case SCENE_LOADING:
        return "SCENE_LOADING";
    case SCENE_LOADED:
        return "SCENE_LOADED";
    }
    return "Enum_SceneState_Unknown";
}
static const char *get_enum_EntityType_str(int value) {
    switch (value) {
    case EntityType_Player:
        return "EntityType_Player";
    case EntityType_Enemy:
        return "EntityType_Enemy";
    case EntityType_Static:
        return "EntityType_Static";
    case EntityType_Projectile:
        return "EntityType_Projectile";
    case EntityType_PointLight:
        return "EntityType_PointLight";
    case EntityType_Count:
        return "EntityType_Count";
    }
    return "Enum_EntityType_Unknown";
}
static const char *get_enum_CameraType_str(int value) {
    switch (value) {
    case CAMERA_TYPE_PERSPECTIVE:
        return "CAMERA_TYPE_PERSPECTIVE";
    case CAMERA_TYPE_ORTHOGRAPHIC:
        return "CAMERA_TYPE_ORTHOGRAPHIC";
    }
    return "Enum_CameraType_Unknown";
}
static const char *get_enum_EditorMode_str(int value) {
    switch (value) {
    case EDITOR_MODE_GIZMO:
        return "EDITOR_MODE_GIZMO";
    case EDITOR_MODE_COLLISION_MESH:
        return "EDITOR_MODE_COLLISION_MESH";
    }
    return "Enum_EditorMode_Unknown";
}
static const char *get_enum_GizmoMode_str(int value) {
    switch (value) {
    case GIZMO_TRANSLATION:
        return "GIZMO_TRANSLATION";
    case GIZMO_ROTATION:
        return "GIZMO_ROTATION";
    case GIZMO_SCALE:
        return "GIZMO_SCALE";
    }
    return "Enum_GizmoMode_Unknown";
}
StructMetaData get_struct_ThreadWork_info() {
    StructMetaData data = {};
    data.name = "ThreadWork";
    data.member_count = 2;
    data.members[0].name = "callback";
    data.members[0].type_name = "ThreadWorkFn*";
    data.members[1].name = "data";
    data.members[1].type_name = "void*";
    return data;
}
StructMetaData get_struct_Mesh_info() {
    StructMetaData data = {};
    data.name = "Mesh";
    data.member_count = 14;
    data.members[0].name = "name";
    data.members[0].type_name = "String";
    data.members[1].name = "parts";
    data.members[1].type_name = "Array<MeshPart>";
    data.members[2].name = "vertex_buffer";
    data.members[2].type_name = "VertexBuffer";
    data.members[3].name = "index_buffer";
    data.members[3].type_name = "IndexBuffer";
    data.members[4].name = "bones";
    data.members[4].type_name = "Array<Bone>";
    data.members[5].name = "box_min";
    data.members[5].type_name = "v3";
    data.members[6].name = "box_max";
    data.members[6].type_name = "v3";
    data.members[7].name = "transform";
    data.members[7].type_name = "mat4";
    data.members[8].name = "default_transform";
    data.members[8].type_name = "mat4";
    data.members[9].name = "vertices_count";
    data.members[9].type_name = "usize";
    data.members[10].name = "indices_count";
    data.members[10].type_name = "usize";
    data.members[11].name = "vertices";
    data.members[11].type_name = "Array<v3>";
    data.members[12].name = "indices";
    data.members[12].type_name = "Array<u32>";
    data.members[13].name = "full_vertices";
    data.members[13].type_name = "Array<Vertex>";
    return data;
}
StructMetaData get_struct_memory_block_info() {
    StructMetaData data = {};
    data.name = "memory_block";
    data.member_count = 4;
    data.members[0].name = "data";
    data.members[0].type_name = "void*";
    data.members[1].name = "used";
    data.members[1].type_name = "usize";
    data.members[2].name = "size";
    data.members[2].type_name = "usize";
    data.members[3].name = "next";
    data.members[3].type_name = "memory_block*";
    return data;
}
StructMetaData get_struct_Mutex_info() {
    StructMetaData data = {};
    data.name = "Mutex";
    data.member_count = 1;
    data.members[0].name = "int";
    data.members[0].type_name = "volatile";
    return data;
}
StructMetaData get_struct_Material_info() {
    StructMetaData data = {};
    data.name = "Material";
    data.member_count = 8;
    data.members[0].name = "diffuse";
    data.members[0].type_name = "TextureID";
    data.members[1].name = "normal_map";
    data.members[1].type_name = "TextureID";
    data.members[2].name = "specular";
    data.members[2].type_name = "TextureID";
    data.members[3].name = "ambient";
    data.members[3].type_name = "TextureID";
    data.members[4].name = "specular_exponent";
    data.members[4].type_name = "TextureID";
    data.members[5].name = "diffuse_factor";
    data.members[5].type_name = "float";
    data.members[6].name = "specular_factor";
    data.members[6].type_name = "float";
    data.members[7].name = "specular_exponent_factor";
    data.members[7].type_name = "float";
    return data;
}
StructMetaData get_struct_RenderPass_info() {
    StructMetaData data = {};
    data.name = "RenderPass";
    data.member_count = 7;
    data.members[0].name = "primitive_type";
    data.members[0].type_name = "PrimitiveType";
    data.members[1].name = "vs";
    data.members[1].type_name = "Shader";
    data.members[2].name = "fs";
    data.members[2].type_name = "Shader";
    data.members[3].name = "depth_stencil_state";
    data.members[3].type_name = "DepthStencilState";
    data.members[4].name = "rasterizer_state";
    data.members[4].type_name = "RasterizerState";
    data.members[5].name = "input_layout";
    data.members[5].type_name = "VertexInputLayout";
    data.members[6].name = "program";
    data.members[6].type_name = "uint32_t";
    return data;
}
StructMetaData get_struct_TempMemory_info() {
    StructMetaData data = {};
    data.name = "TempMemory";
    data.member_count = 1;
    data.members[0].name = "arena";
    data.members[0].type_name = "Arena";
    return data;
}
StructMetaData get_struct_MeshTriangle_info() {
    StructMetaData data = {};
    data.name = "MeshTriangle";
    data.member_count = 1;
    data.members[0].name = "v0";
    data.members[0].type_name = "v3";
    return data;
}
StructMetaData get_struct_Shader_info() {
    StructMetaData data = {};
    data.name = "Shader";
    data.member_count = 2;
    data.members[0].name = "type";
    data.members[0].type_name = "ShaderType";
    data.members[1].name = "id";
    data.members[1].type_name = "uint32_t";
    return data;
}
StructMetaData get_struct_Arena_info() {
    StructMetaData data = {};
    data.name = "Arena";
    data.member_count = 4;
    data.members[0].name = "block";
    data.members[0].type_name = "memory_block*";
    data.members[1].name = "minimum_block_size";
    data.members[1].type_name = "usize";
    data.members[2].name = "thread_safe";
    data.members[2].type_name = "b32";
    data.members[3].name = "mutex";
    data.members[3].type_name = "Mutex";
    return data;
}
StructMetaData get_struct_VertexInputElement_info() {
    StructMetaData data = {};
    data.name = "VertexInputElement";
    data.member_count = 4;
    data.members[0].name = "offset";
    data.members[0].type_name = "int";
    data.members[1].name = "count";
    data.members[1].type_name = "int";
    data.members[2].name = "type";
    data.members[2].type_name = "InputElementType";
    data.members[3].name = "char";
    data.members[3].type_name = "const";
    return data;
}
StructMetaData get_struct_GameButton_info() {
    StructMetaData data = {};
    data.name = "GameButton";
    data.member_count = 2;
    data.members[0].name = "is_down";
    data.members[0].type_name = "b32";
    data.members[1].name = "was_down";
    data.members[1].type_name = "b32";
    return data;
}
StructMetaData get_struct_GameInput_info() {
    StructMetaData data = {};
    data.name = "GameInput";
    data.member_count = 4;
    data.members[0].name = "buttons";
    data.members[0].type_name = "GameButton";
    data.members[0].is_array = 1;
    data.members[0].array_size = BUTTON_COUNT;
    data.members[1].name = "mouse_dp";
    data.members[1].type_name = "v2";
    data.members[2].name = "mouse_p";
    data.members[2].type_name = "v2";
    data.members[3].name = "last_mouse_p";
    data.members[3].type_name = "v2";
    return data;
}
StructMetaData get_struct_SoundPlaying_info() {
    StructMetaData data = {};
    data.name = "SoundPlaying";
    data.member_count = 5;
    data.members[0].name = "entity";
    data.members[0].type_name = "entity_id";
    data.members[1].name = "samples_played";
    data.members[1].type_name = "int";
    data.members[2].name = "sound";
    data.members[2].type_name = "LoadedSound*";
    data.members[3].name = "next";
    data.members[3].type_name = "SoundPlaying*";
    data.members[4].name = "prev";
    data.members[4].type_name = "SoundPlaying*";
    return data;
}
StructMetaData get_struct_Platform_info() {
    StructMetaData data = {};
    data.name = "Platform";
    data.member_count = 7;
    data.members[0].name = "render_context";
    data.members[0].type_name = "RenderContext*";
    data.members[1].name = "imgui_context";
    data.members[1].type_name = "void*";
    data.members[2].name = "add_thread_work";
    data.members[2].type_name = "AddThreadWorkFn*";
    data.members[3].name = "allocate_memory";
    data.members[3].type_name = "PlatformAllocateFn*";
    data.members[4].name = "free_memory";
    data.members[4].type_name = "PlatformFreeFn*";
    data.members[5].name = "lock_mutex";
    data.members[5].type_name = "LockMutexFn*";
    data.members[6].name = "unlock_mutex";
    data.members[6].type_name = "UnlockMutexFn*";
    return data;
}
StructMetaData get_struct_Editor_info() {
    StructMetaData data = {};
    data.name = "Editor";
    data.member_count = 9;
    data.members[0].name = "mode";
    data.members[0].type_name = "EditorMode";
    data.members[1].name = "ops";
    data.members[1].type_name = "Array<EditorOp>";
    data.members[2].name = "undos";
    data.members[2].type_name = "Array<EditorOp>";
    data.members[3].name = "gizmo";
    data.members[3].type_name = "Gizmo";
    data.members[4].name = "copy_entity_mesh";
    data.members[4].type_name = "bool";
    data.members[5].name = "selected_entity";
    data.members[5].type_name = "entity_id";
    data.members[6].name = "selected_entity_mesh";
    data.members[6].type_name = "int";
    data.members[7].name = "copied_entity";
    data.members[7].type_name = "entity_id";
    data.members[8].name = "last_camera_p";
    data.members[8].type_name = "v3";
    return data;
}
StructMetaData get_struct_String_info() {
    StructMetaData data = {};
    data.name = "String";
    data.member_count = 2;
    data.members[0].name = "data";
    data.members[0].type_name = "char*";
    data.members[1].name = "count";
    data.members[1].type_name = "usize";
    return data;
}
StructMetaData get_struct_VertexInputLayout_info() {
    StructMetaData data = {};
    data.name = "VertexInputLayout";
    data.member_count = 3;
    data.members[0].name = "elements";
    data.members[0].type_name = "VertexInputElement";
    data.members[0].is_array = 1;
    data.members[0].array_size = 64;
    data.members[1].name = "element_count";
    data.members[1].type_name = "int";
    data.members[2].name = "vertex_size";
    data.members[2].type_name = "int";
    return data;
}
StructMetaData get_struct_StringHasher_info() {
    StructMetaData data = {};
    data.name = "StringHasher";
    data.member_count = 1;
    data.members[0].name = "operator";
    data.members[0].type_name = "std::size_t";
    return data;
}
StructMetaData get_struct_Texture_info() {
    StructMetaData data = {};
    data.name = "Texture";
    data.member_count = 11;
    data.members[0].name = "opengl_id";
    data.members[0].type_name = "uint32_t";
    data.members[1].name = "id";
    data.members[1].type_name = "TextureID";
    data.members[2].name = "name";
    data.members[2].type_name = "String";
    data.members[3].name = "width";
    data.members[3].type_name = "int";
    data.members[4].name = "height";
    data.members[4].type_name = "int";
    data.members[5].name = "valid";
    data.members[5].type_name = "b32";
    data.members[6].name = "data";
    data.members[6].type_name = "void*";
    data.members[7].name = "gen_mipmaps";
    data.members[7].type_name = "b32";
    data.members[8].name = "srgb";
    data.members[8].type_name = "b32";
    data.members[9].name = "in_gpu";
    data.members[9].type_name = "b32";
    data.members[10].name = "int";
    data.members[10].type_name = "volatile";
    return data;
}
StructMetaData get_struct_ShadowMap_info() {
    StructMetaData data = {};
    data.name = "ShadowMap";
    data.member_count = 9;
    data.members[0].name = "framebuffer";
    data.members[0].type_name = "FrameBuffer";
    data.members[1].name = "light_dir";
    data.members[1].type_name = "v3";
    data.members[2].name = "light_p";
    data.members[2].type_name = "v3";
    data.members[3].name = "view";
    data.members[3].type_name = "mat4";
    data.members[4].name = "projection";
    data.members[4].type_name = "mat4";
    data.members[5].name = "tex_width";
    data.members[5].type_name = "int";
    data.members[6].name = "znear";
    data.members[6].type_name = "float";
    data.members[7].name = "width";
    data.members[7].type_name = "float";
    data.members[8].name = "depth_texture";
    data.members[8].type_name = "Texture";
    return data;
}
StructMetaData get_struct_Animation_info() {
    StructMetaData data = {};
    data.name = "Animation";
    data.member_count = 6;
    data.members[0].name = "name";
    data.members[0].type_name = "String";
    data.members[1].name = "timebegin";
    data.members[1].type_name = "float";
    data.members[2].name = "duration";
    data.members[2].type_name = "float";
    data.members[3].name = "frametime";
    data.members[3].type_name = "float";
    data.members[4].name = "frame_count";
    data.members[4].type_name = "int";
    data.members[5].name = "nodes";
    data.members[5].type_name = "Array<NodeAnimation>";
    return data;
}
StructMetaData get_struct_VertexBuffer_info() {
    StructMetaData data = {};
    data.name = "VertexBuffer";
    data.member_count = 4;
    data.members[0].name = "size";
    data.members[0].type_name = "usize";
    data.members[1].name = "usage";
    data.members[1].type_name = "VertexBufferUsage";
    data.members[2].name = "vao";
    data.members[2].type_name = "uint32_t";
    data.members[3].name = "vbo";
    data.members[3].type_name = "uint32_t";
    return data;
}
StructMetaData get_struct_IndexBuffer_info() {
    StructMetaData data = {};
    data.name = "IndexBuffer";
    data.member_count = 1;
    data.members[0].name = "ebo";
    data.members[0].type_name = "uint32_t";
    return data;
}
StructMetaData get_struct_FrameBuffer_info() {
    StructMetaData data = {};
    data.name = "FrameBuffer";
    data.member_count = 3;
    data.members[0].name = "id";
    data.members[0].type_name = "uint32_t";
    data.members[1].name = "color_texture";
    data.members[1].type_name = "Texture";
    data.members[2].name = "depth_texture";
    data.members[2].type_name = "Texture";
    return data;
}
StructMetaData get_struct_DepthStencilState_info() {
    StructMetaData data = {};
    data.name = "DepthStencilState";
    data.member_count = 1;
    data.members[0].name = "enable_depth";
    data.members[0].type_name = "bool";
    return data;
}
StructMetaData get_struct_StateHasher_info() {
    StructMetaData data = {};
    data.name = "StateHasher";
    data.member_count = 1;
    data.members[0].name = "operator";
    data.members[0].type_name = "std::size_t";
    return data;
}
StructMetaData get_struct_ProfilerBlock_info() {
    StructMetaData data = {};
    data.name = "ProfilerBlock";
    data.member_count = 3;
    data.members[0].name = "start_cycle";
    data.members[0].type_name = "uint64_t";
    data.members[1].name = "id";
    data.members[1].type_name = "uint32_t";
    data.members[2].name = "char";
    data.members[2].type_name = "const";
    return data;
}
StructMetaData get_struct_ConstantBufferElement_info() {
    StructMetaData data = {};
    data.name = "ConstantBufferElement";
    data.member_count = 2;
    data.members[0].name = "type";
    data.members[0].type_name = "ConstantBufferElementType";
    data.members[1].name = "array_size";
    data.members[1].type_name = "int";
    return data;
}
StructMetaData get_struct_RasterizerState_info() {
    StructMetaData data = {};
    data.name = "RasterizerState";
    data.member_count = 2;
    data.members[0].name = "fillmode";
    data.members[0].type_name = "RasterizerFillMode";
    data.members[1].name = "cullmode";
    data.members[1].type_name = "RasterizerCullMode";
    return data;
}
StructMetaData get_struct_RenderContext_info() {
    StructMetaData data = {};
    data.name = "RenderContext";
    data.member_count = 10;
    data.members[0].name = "arena";
    data.members[0].type_name = "Arena";
    data.members[1].name = "window_width";
    data.members[1].type_name = "int";
    data.members[2].name = "window_height";
    data.members[2].type_name = "int";
    data.members[3].name = "window_framebuffer";
    data.members[3].type_name = "FrameBuffer";
    data.members[4].name = "loaded_textures";
    data.members[4].type_name = "Array<Texture>";
    data.members[5].name = "render_pass";
    data.members[5].type_name = "RenderPass*";
    data.members[6].name = "white_texture";
    data.members[6].type_name = "TextureID";
    data.members[7].name = "debug_lines";
    data.members[7].type_name = "Array<v3>";
    data.members[8].name = "active_framebuffer_id";
    data.members[8].type_name = "uintptr_t";
    data.members[9].name = "texture_mutex";
    data.members[9].type_name = "Mutex";
    return data;
}
StructMetaData get_struct_CollisionInfo_info() {
    StructMetaData data = {};
    data.name = "CollisionInfo";
    data.member_count = 3;
    data.members[0].name = "hit_p";
    data.members[0].type_name = "v3";
    data.members[1].name = "hit_normal";
    data.members[1].type_name = "v3";
    data.members[2].name = "t";
    data.members[2].type_name = "float";
    return data;
}
StructMetaData get_struct_ConstantBuffer_info() {
    StructMetaData data = {};
    data.name = "ConstantBuffer";
    data.member_count = 4;
    data.members[0].name = "size";
    data.members[0].type_name = "usize";
    data.members[1].name = "elements";
    data.members[1].type_name = "ConstantBufferElement";
    data.members[1].is_array = 1;
    data.members[1].array_size = 64;
    data.members[2].name = "element_count";
    data.members[2].type_name = "int";
    data.members[3].name = "id";
    data.members[3].type_name = "uint32_t";
    return data;
}
StructMetaData get_struct_Game_info() {
    StructMetaData data = {};
    data.name = "Game";
    data.member_count = 35;
    data.members[0].name = "arena";
    data.members[0].type_name = "Arena";
    data.members[1].name = "is_initialized";
    data.members[1].type_name = "std::atomic_bool";
    data.members[2].name = "world";
    data.members[2].type_name = "World*";
    data.members[3].name = "shadow_map";
    data.members[3].type_name = "ShadowMap";
    data.members[4].name = "mesh_render_pass";
    data.members[4].type_name = "RenderPass";
    data.members[5].name = "shadow_map_render_pass";
    data.members[5].type_name = "RenderPass";
    data.members[6].name = "outline_render_pass";
    data.members[6].type_name = "RenderPass";
    data.members[7].name = "constant_buffer";
    data.members[7].type_name = "ConstantBuffer";
    data.members[8].name = "debug_lines_render_pass";
    data.members[8].type_name = "RenderPass";
    data.members[9].name = "debug_lines_vertex_buffer";
    data.members[9].type_name = "VertexBuffer";
    data.members[10].name = "debug_lines_constant_buffer";
    data.members[10].type_name = "ConstantBuffer";
    data.members[11].name = "asset_arena";
    data.members[11].type_name = "Arena";
    data.members[12].name = "scenes";
    data.members[12].type_name = "Array<Scene>";
    data.members[13].name = "next_scene_id";
    data.members[13].type_name = "usize";
    data.members[14].name = "in_editor";
    data.members[14].type_name = "b32";
    data.members[15].name = "debug_collision";
    data.members[15].type_name = "bool";
    data.members[16].name = "frame";
    data.members[16].type_name = "int";
    data.members[17].name = "time";
    data.members[17].type_name = "float";
    data.members[18].name = "default_rasterizer_state";
    data.members[18].type_name = "RasterizerState";
    data.members[19].name = "default_depth_stencil_state";
    data.members[19].type_name = "DepthStencilState";
    data.members[20].name = "disable_depth_state";
    data.members[20].type_name = "DepthStencilState";
    data.members[21].name = "debug_asset_fb";
    data.members[21].type_name = "FrameBuffer";
    data.members[22].name = "debug_asset_tex";
    data.members[22].type_name = "Texture";
    data.members[23].name = "sound_state";
    data.members[23].type_name = "SoundState";
    data.members[24].name = "first_playing_sound";
    data.members[24].type_name = "SoundPlaying*";
    data.members[25].name = "loaded_sounds";
    data.members[25].type_name = "LoadedSound";
    data.members[25].is_array = 1;
    data.members[25].array_size = 32;
    data.members[26].name = "animations";
    data.members[26].type_name = "std::unordered_map<String,int,StringHasher>";
    data.members[27].name = "loaded_animations";
    data.members[27].type_name = "Array<Animation>";
    data.members[28].name = "default_scene";
    data.members[28].type_name = "Scene";
    data.members[29].name = "show_normals";
    data.members[29].type_name = "bool";
    data.members[30].name = "render_bones";
    data.members[30].type_name = "bool";
    data.members[31].name = "frustum_culling";
    data.members[31].type_name = "bool";
    data.members[32].name = "master_volume";
    data.members[32].type_name = "float";
    data.members[33].name = "play_in_editor";
    data.members[33].type_name = "bool";
    data.members[34].name = "background_color";
    data.members[34].type_name = "v3";
    return data;
}
StructMetaData get_struct_Vertex_info() {
    StructMetaData data = {};
    data.name = "Vertex";
    data.member_count = 5;
    data.members[0].name = "position";
    data.members[0].type_name = "v3";
    data.members[1].name = "normal";
    data.members[1].type_name = "v3";
    data.members[2].name = "uv";
    data.members[2].type_name = "v2";
    data.members[3].name = "weights";
    data.members[3].type_name = "float";
    data.members[3].is_array = 1;
    data.members[3].array_size = 4;
    data.members[4].name = "indices";
    data.members[4].type_name = "float";
    data.members[4].is_array = 1;
    data.members[4].array_size = 4;
    return data;
}
StructMetaData get_struct_MeshPart_info() {
    StructMetaData data = {};
    data.name = "MeshPart";
    data.member_count = 3;
    data.members[0].name = "material";
    data.members[0].type_name = "Material";
    data.members[1].name = "indices_count";
    data.members[1].type_name = "usize";
    data.members[2].name = "offset";
    data.members[2].type_name = "usize";
    return data;
}
StructMetaData get_struct_Bone_info() {
    StructMetaData data = {};
    data.name = "Bone";
    data.member_count = 4;
    data.members[0].name = "name";
    data.members[0].type_name = "String";
    data.members[1].name = "transform";
    data.members[1].type_name = "mat4";
    data.members[2].name = "inv_bind";
    data.members[2].type_name = "mat4";
    data.members[3].name = "parent";
    data.members[3].type_name = "int";
    return data;
}
StructMetaData get_struct_NodeAnimation_info() {
    StructMetaData data = {};
    data.name = "NodeAnimation";
    data.member_count = 8;
    data.members[0].name = "name";
    data.members[0].type_name = "String";
    data.members[1].name = "position";
    data.members[1].type_name = "Array<v3>";
    data.members[2].name = "scale";
    data.members[2].type_name = "Array<v3>";
    data.members[3].name = "rotation";
    data.members[3].type_name = "Array<quat>";
    data.members[4].name = "const_position";
    data.members[4].type_name = "v3";
    data.members[5].name = "const_scale";
    data.members[5].type_name = "v3";
    data.members[6].name = "const_rotation";
    data.members[6].type_name = "quat";
    data.members[7].name = "transform";
    data.members[7].type_name = "mat4";
    return data;
}
StructMetaData get_struct_SceneNode_info() {
    StructMetaData data = {};
    data.name = "SceneNode";
    data.member_count = 8;
    data.members[0].name = "name";
    data.members[0].type_name = "String";
    data.members[1].name = "parent";
    data.members[1].type_name = "SceneNode*";
    data.members[2].name = "childs";
    data.members[2].type_name = "Array<SceneNode*>";
    data.members[3].name = "mesh";
    data.members[3].type_name = "Mesh*";
    data.members[4].name = "local_transform";
    data.members[4].type_name = "mat4";
    data.members[5].name = "geometry_transform";
    data.members[5].type_name = "mat4";
    data.members[6].name = "id";
    data.members[6].type_name = "int";
    data.members[7].name = "skip_render";
    data.members[7].type_name = "b32";
    return data;
}
StructMetaData get_struct_World_info() {
    StructMetaData data = {};
    data.name = "World";
    data.member_count = 17;
    data.members[0].name = "occupied";
    data.members[0].type_name = "std::unordered_map<uint64_t,bool>";
    data.members[1].name = "arena";
    data.members[1].type_name = "Arena";
    data.members[2].name = "editor";
    data.members[2].type_name = "Editor";
    data.members[3].name = "entities";
    data.members[3].type_name = "Array<Entity>";
    data.members[4].name = "entities_id_map";
    data.members[4].type_name = "std::unordered_map<entity_id,usize>";
    data.members[5].name = "scene_collision_mesh";
    data.members[5].type_name = "std::unordered_map<SceneID,int>";
    data.members[6].name = "next_entity_id";
    data.members[6].type_name = "entity_id";
    data.members[7].name = "player_camera_p";
    data.members[7].type_name = "v3";
    data.members[8].name = "player_camera_rotation";
    data.members[8].type_name = "v3";
    data.members[9].name = "player_camera_drotation";
    data.members[9].type_name = "v3";
    data.members[10].name = "editor_camera_p";
    data.members[10].type_name = "v3";
    data.members[11].name = "editor_camera_rotation";
    data.members[11].type_name = "v3";
    data.members[12].name = "player_id";
    data.members[12].type_name = "entity_id";
    data.members[13].name = "moving_box";
    data.members[13].type_name = "entity_id";
    data.members[14].name = "last_game_camera";
    data.members[14].type_name = "Camera";
    data.members[15].name = "aim_camera_transition_t";
    data.members[15].type_name = "float";
    data.members[16].name = "collision_meshes";
    data.members[16].type_name = "Array<CollisionMesh>";
    return data;
}
StructMetaData get_struct_Scene_info() {
    StructMetaData data = {};
    data.name = "Scene";
    data.member_count = 7;
    data.members[0].name = "id";
    data.members[0].type_name = "SceneID";
    data.members[1].name = "path";
    data.members[1].type_name = "String";
    data.members[2].name = "name";
    data.members[2].type_name = "String";
    data.members[3].name = "meshes";
    data.members[3].type_name = "Array<Mesh>";
    data.members[4].name = "filename";
    data.members[4].type_name = "String";
    data.members[5].name = "state";
    data.members[5].type_name = "int";
    data.members[6].name = "in_gpu";
    data.members[6].type_name = "b32";
    return data;
}
StructMetaData get_struct_Constants_info() {
    StructMetaData data = {};
    data.name = "Constants";
    data.member_count = 17;
    data.members[0].name = "view";
    data.members[0].type_name = "mat4";
    data.members[1].name = "projection";
    data.members[1].type_name = "mat4";
    data.members[2].name = "model";
    data.members[2].type_name = "mat4";
    data.members[3].name = "light_transform";
    data.members[3].type_name = "mat4";
    data.members[4].name = "bones";
    data.members[4].type_name = "mat4";
    data.members[4].is_array = 1;
    data.members[4].array_size = 96;
    data.members[5].name = "point_light_color";
    data.members[5].type_name = "v4";
    data.members[5].is_array = 1;
    data.members[5].array_size = 8;
    data.members[6].name = "point_light_position";
    data.members[6].type_name = "v4";
    data.members[6].is_array = 1;
    data.members[6].array_size = 8;
    data.members[7].name = "point_light_count";
    data.members[7].type_name = "int";
    data.members[8].name = "camera_p";
    data.members[8].type_name = "v3";
    data.members[9].name = "player_p";
    data.members[9].type_name = "v3";
    data.members[10].name = "color";
    data.members[10].type_name = "v3";
    data.members[11].name = "diffuse_factor";
    data.members[11].type_name = "float";
    data.members[12].name = "specular_factor";
    data.members[12].type_name = "float";
    data.members[13].name = "specular_exponent_factor";
    data.members[13].type_name = "float";
    data.members[14].name = "skinned";
    data.members[14].type_name = "int";
    data.members[15].name = "has_normal_map";
    data.members[15].type_name = "int";
    data.members[16].name = "show_normals";
    data.members[16].type_name = "int";
    return data;
}
StructMetaData get_struct_CollisionMesh_info() {
    StructMetaData data = {};
    data.name = "CollisionMesh";
    data.member_count = 2;
    data.members[0].name = "vertices";
    data.members[0].type_name = "Array<v3>";
    data.members[1].name = "scene";
    data.members[1].type_name = "SceneID";
    return data;
}
StructMetaData get_struct_CollisionShape_info() {
    StructMetaData data = {};
    data.name = "CollisionShape";
    data.member_count = 5;
    data.members[0].name = "collision_mesh";
    data.members[0].type_name = "CollisionMesh";
    data.members[1].name = "ellipsoid_radius";
    data.members[1].type_name = "v3";
    data.members[2].name = "ellipsoid";
    data.members[2].type_name = "bool";
    data.members[3].name = "transform";
    data.members[3].type_name = "mat4";
    data.members[4].name = "scale";
    data.members[4].type_name = "v3";
    return data;
}
StructMetaData get_struct_Entity_info() {
    StructMetaData data = {};
    data.name = "Entity";
    data.member_count = 35;
    data.members[0].name = "id";
    data.members[0].type_name = "entity_id";
    data.members[1].name = "parent";
    data.members[1].type_name = "entity_id";
    data.members[2].name = "type";
    data.members[2].type_name = "EntityType";
    data.members[3].name = "position";
    data.members[3].type_name = "v3";
    data.members[4].name = "dp";
    data.members[4].type_name = "v3";
    data.members[5].name = "rotation";
    data.members[5].type_name = "quat";
    data.members[6].name = "scale";
    data.members[6].type_name = "v3";
    data.members[7].name = "color";
    data.members[7].type_name = "v3";
    data.members[8].name = "moved";
    data.members[8].type_name = "b32";
    data.members[9].name = "run";
    data.members[9].type_name = "b32";
    data.members[10].name = "shooting";
    data.members[10].type_name = "b32";
    data.members[11].name = "can_jump";
    data.members[11].type_name = "b32";
    data.members[12].name = "on_ground";
    data.members[12].type_name = "b32";
    data.members[13].name = "pressing_jump";
    data.members[13].type_name = "b32";
    data.members[14].name = "aiming";
    data.members[14].type_name = "b32";
    data.members[15].name = "ellipsoid_collision_shape";
    data.members[15].type_name = "bool";
    data.members[16].name = "ellipsoid_radius";
    data.members[16].type_name = "v3";
    data.members[17].name = "scene_id";
    data.members[17].type_name = "SceneID";
    data.members[18].name = "scene_transform";
    data.members[18].type_name = "mat4";
    data.members[19].name = "disable_collision";
    data.members[19].type_name = "bool";
    data.members[20].name = "walk_backward";
    data.members[20].type_name = "bool";
    data.members[21].name = "jumped";
    data.members[21].type_name = "bool";
    data.members[22].name = "curr_anim";
    data.members[22].type_name = "Animation*";
    data.members[23].name = "next_anim";
    data.members[23].type_name = "Animation*";
    data.members[24].name = "anim_time";
    data.members[24].type_name = "float";
    data.members[25].name = "blend_time";
    data.members[25].type_name = "float";
    data.members[26].name = "animation";
    data.members[26].type_name = "Animation*";
    data.members[27].name = "speed";
    data.members[27].type_name = "float";
    data.members[28].name = "height_above_ground";
    data.members[28].type_name = "float";
    data.members[29].name = "point_light_scale";
    data.members[29].type_name = "float";
    data.members[30].name = "z_rot";
    data.members[30].type_name = "float";
    data.members[31].name = "last_move";
    data.members[31].type_name = "int";
    data.members[32].name = "last_gun_time";
    data.members[32].type_name = "float";
    data.members[33].name = "last_jump_z";
    data.members[33].type_name = "float";
    data.members[34].name = "should_jump";
    data.members[34].type_name = "bool";
    return data;
}
StructMetaData get_struct_Camera_info() {
    StructMetaData data = {};
    data.name = "Camera";
    data.member_count = 6;
    data.members[0].name = "type";
    data.members[0].type_name = "CameraType";
    data.members[1].name = "position";
    data.members[1].type_name = "v3";
    data.members[2].name = "view";
    data.members[2].type_name = "mat4";
    data.members[3].name = "projection";
    data.members[3].type_name = "mat4";
    data.members[4].name = "znear";
    data.members[4].type_name = "float";
    data.members[5].name = "forward";
    data.members[5].type_name = "v3";
    return data;
}
StructMetaData get_struct_ProfilerBlockStat_info() {
    StructMetaData data = {};
    data.name = "ProfilerBlockStat";
    data.member_count = 3;
    data.members[0].name = "char";
    data.members[0].type_name = "const";
    data.members[1].name = "cycle_count";
    data.members[1].type_name = "uint64_t";
    data.members[2].name = "call_count";
    data.members[2].type_name = "uint32_t";
    return data;
}
StructMetaData get_struct_EditorOp_info() {
    StructMetaData data = {};
    data.name = "EditorOp";
    data.member_count = 2;
    data.members[0].name = "type";
    data.members[0].type_name = "int";
    data.members[1].name = "entity";
    data.members[1].type_name = "entity_id";
    return data;
}
StructMetaData get_struct_Gizmo_info() {
    StructMetaData data = {};
    data.name = "Gizmo";
    data.member_count = 17;
    data.members[0].name = "mode";
    data.members[0].type_name = "GizmoMode";
    data.members[1].name = "init_position";
    data.members[1].type_name = "v3";
    data.members[2].name = "init_rotation";
    data.members[2].type_name = "quat";
    data.members[3].name = "init_scale";
    data.members[3].type_name = "v3";
    data.members[4].name = "active";
    data.members[4].type_name = "bool";
    data.members[5].name = "dragging_axis";
    data.members[5].type_name = "int";
    data.members[6].name = "did_drag";
    data.members[6].type_name = "bool";
    data.members[7].name = "uniform_scale";
    data.members[7].type_name = "bool";
    data.members[8].name = "hit_p";
    data.members[8].type_name = "v3";
    data.members[9].name = "delta_p";
    data.members[9].type_name = "v3";
    data.members[10].name = "scale";
    data.members[10].type_name = "v3";
    data.members[11].name = "delta_s";
    data.members[11].type_name = "float";
    data.members[12].name = "rotation_angle";
    data.members[12].type_name = "float";
    data.members[13].name = "rotation";
    data.members[13].type_name = "quat";
    data.members[14].name = "rotation_right_axis";
    data.members[14].type_name = "v3";
    data.members[15].name = "rotation_up_axis";
    data.members[15].type_name = "v3";
    data.members[16].name = "rotation_axis";
    data.members[16].type_name = "v3";
    return data;
}
StructMetaData get_struct_LoadedSound_info() {
    StructMetaData data = {};
    data.name = "LoadedSound";
    data.member_count = 2;
    data.members[0].name = "sample_count";
    data.members[0].type_name = "int";
    data.members[1].name = "samples";
    data.members[1].type_name = "float*";
    return data;
}
StructMetaData get_struct_SoundState_info() {
    StructMetaData data = {};
    data.name = "SoundState";
    data.member_count = 4;
    data.members[0].name = "buffer";
    data.members[0].type_name = "float*";
    data.members[1].name = "sample_count";
    data.members[1].type_name = "int";
    data.members[2].name = "read_index";
    data.members[2].type_name = "std::atomic_int32_t";
    data.members[3].name = "write_index";
    data.members[3].type_name = "std::atomic_int32_t";
    return data;
}
StructMetaData get_struct_State_info() {
    StructMetaData data = {};
    data.name = "State";
    data.member_count = 5;
    data.members[0].name = "p";
    data.members[0].type_name = "v3i";
    data.members[1].name = "jump";
    data.members[1].type_name = "int";
    data.members[2].name = "fscore";
    data.members[2].type_name = "int";
    data.members[3].name = "operator";
    data.members[3].type_name = "bool";
    data.members[4].name = "operator";
    data.members[4].type_name = "bool";
    return data;
}
static void imgui_edit_enum_GameButtonType(GameButtonType &x,
                                           const char *name) {
    const char *items[46];
    GameButtonType items_type[46];
    int curr = 0;
    items[0] = "BUTTON_A";
    items_type[0] = BUTTON_A;
    if (x == BUTTON_A)
        curr = 0;
    items[1] = "BUTTON_B";
    items_type[1] = BUTTON_B;
    if (x == BUTTON_B)
        curr = 1;
    items[2] = "BUTTON_C";
    items_type[2] = BUTTON_C;
    if (x == BUTTON_C)
        curr = 2;
    items[3] = "BUTTON_D";
    items_type[3] = BUTTON_D;
    if (x == BUTTON_D)
        curr = 3;
    items[4] = "BUTTON_E";
    items_type[4] = BUTTON_E;
    if (x == BUTTON_E)
        curr = 4;
    items[5] = "BUTTON_F";
    items_type[5] = BUTTON_F;
    if (x == BUTTON_F)
        curr = 5;
    items[6] = "BUTTON_G";
    items_type[6] = BUTTON_G;
    if (x == BUTTON_G)
        curr = 6;
    items[7] = "BUTTON_H";
    items_type[7] = BUTTON_H;
    if (x == BUTTON_H)
        curr = 7;
    items[8] = "BUTTON_I";
    items_type[8] = BUTTON_I;
    if (x == BUTTON_I)
        curr = 8;
    items[9] = "BUTTON_J";
    items_type[9] = BUTTON_J;
    if (x == BUTTON_J)
        curr = 9;
    items[10] = "BUTTON_K";
    items_type[10] = BUTTON_K;
    if (x == BUTTON_K)
        curr = 10;
    items[11] = "BUTTON_L";
    items_type[11] = BUTTON_L;
    if (x == BUTTON_L)
        curr = 11;
    items[12] = "BUTTON_M";
    items_type[12] = BUTTON_M;
    if (x == BUTTON_M)
        curr = 12;
    items[13] = "BUTTON_N";
    items_type[13] = BUTTON_N;
    if (x == BUTTON_N)
        curr = 13;
    items[14] = "BUTTON_O";
    items_type[14] = BUTTON_O;
    if (x == BUTTON_O)
        curr = 14;
    items[15] = "BUTTON_P";
    items_type[15] = BUTTON_P;
    if (x == BUTTON_P)
        curr = 15;
    items[16] = "BUTTON_Q";
    items_type[16] = BUTTON_Q;
    if (x == BUTTON_Q)
        curr = 16;
    items[17] = "BUTTON_R";
    items_type[17] = BUTTON_R;
    if (x == BUTTON_R)
        curr = 17;
    items[18] = "BUTTON_S";
    items_type[18] = BUTTON_S;
    if (x == BUTTON_S)
        curr = 18;
    items[19] = "BUTTON_T";
    items_type[19] = BUTTON_T;
    if (x == BUTTON_T)
        curr = 19;
    items[20] = "BUTTON_U";
    items_type[20] = BUTTON_U;
    if (x == BUTTON_U)
        curr = 20;
    items[21] = "BUTTON_V";
    items_type[21] = BUTTON_V;
    if (x == BUTTON_V)
        curr = 21;
    items[22] = "BUTTON_W";
    items_type[22] = BUTTON_W;
    if (x == BUTTON_W)
        curr = 22;
    items[23] = "BUTTON_X";
    items_type[23] = BUTTON_X;
    if (x == BUTTON_X)
        curr = 23;
    items[24] = "BUTTON_Y";
    items_type[24] = BUTTON_Y;
    if (x == BUTTON_Y)
        curr = 24;
    items[25] = "BUTTON_Z";
    items_type[25] = BUTTON_Z;
    if (x == BUTTON_Z)
        curr = 25;
    items[26] = "BUTTON_DELETE";
    items_type[26] = BUTTON_DELETE;
    if (x == BUTTON_DELETE)
        curr = 26;
    items[27] = "BUTTON_LEFT_CONTROL";
    items_type[27] = BUTTON_LEFT_CONTROL;
    if (x == BUTTON_LEFT_CONTROL)
        curr = 27;
    items[28] = "BUTTON_LEFT_SHIFT";
    items_type[28] = BUTTON_LEFT_SHIFT;
    if (x == BUTTON_LEFT_SHIFT)
        curr = 28;
    items[29] = "BUTTON_MOUSE_LEFT";
    items_type[29] = BUTTON_MOUSE_LEFT;
    if (x == BUTTON_MOUSE_LEFT)
        curr = 29;
    items[30] = "BUTTON_MOUSE_RIGHT";
    items_type[30] = BUTTON_MOUSE_RIGHT;
    if (x == BUTTON_MOUSE_RIGHT)
        curr = 30;
    items[31] = "BUTTON_SPACE";
    items_type[31] = BUTTON_SPACE;
    if (x == BUTTON_SPACE)
        curr = 31;
    items[32] = "BUTTON_ESCAPE";
    items_type[32] = BUTTON_ESCAPE;
    if (x == BUTTON_ESCAPE)
        curr = 32;
    items[33] = "BUTTON_F1";
    items_type[33] = BUTTON_F1;
    if (x == BUTTON_F1)
        curr = 33;
    items[34] = "BUTTON_F2";
    items_type[34] = BUTTON_F2;
    if (x == BUTTON_F2)
        curr = 34;
    items[35] = "BUTTON_F3";
    items_type[35] = BUTTON_F3;
    if (x == BUTTON_F3)
        curr = 35;
    items[36] = "BUTTON_F4";
    items_type[36] = BUTTON_F4;
    if (x == BUTTON_F4)
        curr = 36;
    items[37] = "BUTTON_F5";
    items_type[37] = BUTTON_F5;
    if (x == BUTTON_F5)
        curr = 37;
    items[38] = "BUTTON_F6";
    items_type[38] = BUTTON_F6;
    if (x == BUTTON_F6)
        curr = 38;
    items[39] = "BUTTON_F7";
    items_type[39] = BUTTON_F7;
    if (x == BUTTON_F7)
        curr = 39;
    items[40] = "BUTTON_F8";
    items_type[40] = BUTTON_F8;
    if (x == BUTTON_F8)
        curr = 40;
    items[41] = "BUTTON_F9";
    items_type[41] = BUTTON_F9;
    if (x == BUTTON_F9)
        curr = 41;
    items[42] = "BUTTON_F10";
    items_type[42] = BUTTON_F10;
    if (x == BUTTON_F10)
        curr = 42;
    items[43] = "BUTTON_F11";
    items_type[43] = BUTTON_F11;
    if (x == BUTTON_F11)
        curr = 43;
    items[44] = "BUTTON_F12";
    items_type[44] = BUTTON_F12;
    if (x == BUTTON_F12)
        curr = 44;
    items[45] = "BUTTON_COUNT";
    items_type[45] = BUTTON_COUNT;
    if (x == BUTTON_COUNT)
        curr = 45;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &curr, items, 46);
    x = items_type[curr];
}
static void imgui_edit_enum_TextureState(TextureState &x, const char *name) {
    const char *items[3];
    TextureState items_type[3];
    int curr = 0;
    items[0] = "TEXTURE_STATE_UNLOADED";
    items_type[0] = TEXTURE_STATE_UNLOADED;
    if (x == TEXTURE_STATE_UNLOADED)
        curr = 0;
    items[1] = "TEXTURE_STATE_LOADING";
    items_type[1] = TEXTURE_STATE_LOADING;
    if (x == TEXTURE_STATE_LOADING)
        curr = 1;
    items[2] = "TEXTURE_STATE_LOADED";
    items_type[2] = TEXTURE_STATE_LOADED;
    if (x == TEXTURE_STATE_LOADED)
        curr = 2;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &curr, items, 3);
    x = items_type[curr];
}
static void imgui_edit_enum_LogType(LogType &x, const char *name) {
    const char *items[5];
    LogType items_type[5];
    int curr = 0;
    items[0] = "LOG_TYPE_DEBUG";
    items_type[0] = LOG_TYPE_DEBUG;
    if (x == LOG_TYPE_DEBUG)
        curr = 0;
    items[1] = "LOG_TYPE_INFO";
    items_type[1] = LOG_TYPE_INFO;
    if (x == LOG_TYPE_INFO)
        curr = 1;
    items[2] = "LOG_TYPE_WARN";
    items_type[2] = LOG_TYPE_WARN;
    if (x == LOG_TYPE_WARN)
        curr = 2;
    items[3] = "LOG_TYPE_ERROR";
    items_type[3] = LOG_TYPE_ERROR;
    if (x == LOG_TYPE_ERROR)
        curr = 3;
    items[4] = "LOG_TYPE_FATAL";
    items_type[4] = LOG_TYPE_FATAL;
    if (x == LOG_TYPE_FATAL)
        curr = 4;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &curr, items, 5);
    x = items_type[curr];
}
static void imgui_edit_enum_VertexBufferUsage(VertexBufferUsage &x,
                                              const char *name) {
    const char *items[2];
    VertexBufferUsage items_type[2];
    int curr = 0;
    items[0] = "VERTEX_BUFFER_IMMUTABLE";
    items_type[0] = VERTEX_BUFFER_IMMUTABLE;
    if (x == VERTEX_BUFFER_IMMUTABLE)
        curr = 0;
    items[1] = "VERTEX_BUFFER_DYNAMIC";
    items_type[1] = VERTEX_BUFFER_DYNAMIC;
    if (x == VERTEX_BUFFER_DYNAMIC)
        curr = 1;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &curr, items, 2);
    x = items_type[curr];
}
static void imgui_edit_enum_ShaderType(ShaderType &x, const char *name) {
    const char *items[2];
    ShaderType items_type[2];
    int curr = 0;
    items[0] = "SHADER_TYPE_VERTEX";
    items_type[0] = SHADER_TYPE_VERTEX;
    if (x == SHADER_TYPE_VERTEX)
        curr = 0;
    items[1] = "SHADER_TYPE_FRAGMENT";
    items_type[1] = SHADER_TYPE_FRAGMENT;
    if (x == SHADER_TYPE_FRAGMENT)
        curr = 1;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &curr, items, 2);
    x = items_type[curr];
}
static void
imgui_edit_enum_ConstantBufferElementType(ConstantBufferElementType &x,
                                          const char *name) {
    const char *items[7];
    ConstantBufferElementType items_type[7];
    int curr = 0;
    items[0] = "CONSTANT_BUFFER_ELEMENT_MAT4";
    items_type[0] = CONSTANT_BUFFER_ELEMENT_MAT4;
    if (x == CONSTANT_BUFFER_ELEMENT_MAT4)
        curr = 0;
    items[1] = "CONSTANT_BUFFER_ELEMENT_VEC4";
    items_type[1] = CONSTANT_BUFFER_ELEMENT_VEC4;
    if (x == CONSTANT_BUFFER_ELEMENT_VEC4)
        curr = 1;
    items[2] = "CONSTANT_BUFFER_ELEMENT_VEC3";
    items_type[2] = CONSTANT_BUFFER_ELEMENT_VEC3;
    if (x == CONSTANT_BUFFER_ELEMENT_VEC3)
        curr = 2;
    items[3] = "CONSTANT_BUFFER_ELEMENT_VEC2";
    items_type[3] = CONSTANT_BUFFER_ELEMENT_VEC2;
    if (x == CONSTANT_BUFFER_ELEMENT_VEC2)
        curr = 3;
    items[4] = "CONSTANT_BUFFER_ELEMENT_FLOAT";
    items_type[4] = CONSTANT_BUFFER_ELEMENT_FLOAT;
    if (x == CONSTANT_BUFFER_ELEMENT_FLOAT)
        curr = 4;
    items[5] = "CONSTANT_BUFFER_ELEMENT_INT";
    items_type[5] = CONSTANT_BUFFER_ELEMENT_INT;
    if (x == CONSTANT_BUFFER_ELEMENT_INT)
        curr = 5;
    items[6] = "CONSTANT_BUFFER_ELEMENT_COUNT";
    items_type[6] = CONSTANT_BUFFER_ELEMENT_COUNT;
    if (x == CONSTANT_BUFFER_ELEMENT_COUNT)
        curr = 6;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &curr, items, 7);
    x = items_type[curr];
}
static void imgui_edit_enum_InputElementType(InputElementType &x,
                                             const char *name) {
    const char *items[2];
    InputElementType items_type[2];
    int curr = 0;
    items[0] = "INPUT_ELEMENT_FLOAT";
    items_type[0] = INPUT_ELEMENT_FLOAT;
    if (x == INPUT_ELEMENT_FLOAT)
        curr = 0;
    items[1] = "INPUT_ELEMENT_SIGNED_INT";
    items_type[1] = INPUT_ELEMENT_SIGNED_INT;
    if (x == INPUT_ELEMENT_SIGNED_INT)
        curr = 1;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &curr, items, 2);
    x = items_type[curr];
}
static void imgui_edit_enum_RasterizerFillMode(RasterizerFillMode &x,
                                               const char *name) {
    const char *items[2];
    RasterizerFillMode items_type[2];
    int curr = 0;
    items[0] = "RASTERIZER_FILL_SOLID";
    items_type[0] = RASTERIZER_FILL_SOLID;
    if (x == RASTERIZER_FILL_SOLID)
        curr = 0;
    items[1] = "RASTERIZER_FILL_WIREFRAME";
    items_type[1] = RASTERIZER_FILL_WIREFRAME;
    if (x == RASTERIZER_FILL_WIREFRAME)
        curr = 1;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &curr, items, 2);
    x = items_type[curr];
}
static void imgui_edit_enum_PrimitiveType(PrimitiveType &x, const char *name) {
    const char *items[2];
    PrimitiveType items_type[2];
    int curr = 0;
    items[0] = "PRIMITIVE_TRIANGLES";
    items_type[0] = PRIMITIVE_TRIANGLES;
    if (x == PRIMITIVE_TRIANGLES)
        curr = 0;
    items[1] = "PRIMITIVE_LINES";
    items_type[1] = PRIMITIVE_LINES;
    if (x == PRIMITIVE_LINES)
        curr = 1;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &curr, items, 2);
    x = items_type[curr];
}
static void imgui_edit_enum_EditorOpType(EditorOpType &x, const char *name) {
    const char *items[8];
    EditorOpType items_type[8];
    int curr = 0;
    items[0] = "EDITOR_OP_TRANSLATE_ENTITY";
    items_type[0] = EDITOR_OP_TRANSLATE_ENTITY;
    if (x == EDITOR_OP_TRANSLATE_ENTITY)
        curr = 0;
    items[1] = "EDITOR_OP_ROTATE_ENTITY";
    items_type[1] = EDITOR_OP_ROTATE_ENTITY;
    if (x == EDITOR_OP_ROTATE_ENTITY)
        curr = 1;
    items[2] = "EDITOR_OP_SCALE_ENTITY";
    items_type[2] = EDITOR_OP_SCALE_ENTITY;
    if (x == EDITOR_OP_SCALE_ENTITY)
        curr = 2;
    items[3] = "EDITOR_OP_PASTE_ENTITY";
    items_type[3] = EDITOR_OP_PASTE_ENTITY;
    if (x == EDITOR_OP_PASTE_ENTITY)
        curr = 3;
    items[4] = "EDITOR_OP_DELETE_ENTITY";
    items_type[4] = EDITOR_OP_DELETE_ENTITY;
    if (x == EDITOR_OP_DELETE_ENTITY)
        curr = 4;
    items[5] = "EDITOR_OP_SPAWN_ENTITY";
    items_type[5] = EDITOR_OP_SPAWN_ENTITY;
    if (x == EDITOR_OP_SPAWN_ENTITY)
        curr = 5;
    items[6] = "EDITOR_OP_CREATE_MESH_COLLISION_VERTEX";
    items_type[6] = EDITOR_OP_CREATE_MESH_COLLISION_VERTEX;
    if (x == EDITOR_OP_CREATE_MESH_COLLISION_VERTEX)
        curr = 6;
    items[7] = "EDITOR_OP_DELETE_MESH_COLLISION_TRIANGLE";
    items_type[7] = EDITOR_OP_DELETE_MESH_COLLISION_TRIANGLE;
    if (x == EDITOR_OP_DELETE_MESH_COLLISION_TRIANGLE)
        curr = 7;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &curr, items, 8);
    x = items_type[curr];
}
static void imgui_edit_enum_RasterizerCullMode(RasterizerCullMode &x,
                                               const char *name) {
    const char *items[3];
    RasterizerCullMode items_type[3];
    int curr = 0;
    items[0] = "RASTERIZER_CULL_NONE";
    items_type[0] = RASTERIZER_CULL_NONE;
    if (x == RASTERIZER_CULL_NONE)
        curr = 0;
    items[1] = "RASTERIZER_CULL_FRONT";
    items_type[1] = RASTERIZER_CULL_FRONT;
    if (x == RASTERIZER_CULL_FRONT)
        curr = 1;
    items[2] = "RASTERIZER_CULL_BACK";
    items_type[2] = RASTERIZER_CULL_BACK;
    if (x == RASTERIZER_CULL_BACK)
        curr = 2;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &curr, items, 3);
    x = items_type[curr];
}
static void imgui_edit_enum_SceneState(SceneState &x, const char *name) {
    const char *items[3];
    SceneState items_type[3];
    int curr = 0;
    items[0] = "SCENE_UNLOADED";
    items_type[0] = SCENE_UNLOADED;
    if (x == SCENE_UNLOADED)
        curr = 0;
    items[1] = "SCENE_LOADING";
    items_type[1] = SCENE_LOADING;
    if (x == SCENE_LOADING)
        curr = 1;
    items[2] = "SCENE_LOADED";
    items_type[2] = SCENE_LOADED;
    if (x == SCENE_LOADED)
        curr = 2;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &curr, items, 3);
    x = items_type[curr];
}
static void imgui_edit_enum_EntityType(EntityType &x, const char *name) {
    const char *items[6];
    EntityType items_type[6];
    int curr = 0;
    items[0] = "EntityType_Player";
    items_type[0] = EntityType_Player;
    if (x == EntityType_Player)
        curr = 0;
    items[1] = "EntityType_Enemy";
    items_type[1] = EntityType_Enemy;
    if (x == EntityType_Enemy)
        curr = 1;
    items[2] = "EntityType_Static";
    items_type[2] = EntityType_Static;
    if (x == EntityType_Static)
        curr = 2;
    items[3] = "EntityType_Projectile";
    items_type[3] = EntityType_Projectile;
    if (x == EntityType_Projectile)
        curr = 3;
    items[4] = "EntityType_PointLight";
    items_type[4] = EntityType_PointLight;
    if (x == EntityType_PointLight)
        curr = 4;
    items[5] = "EntityType_Count";
    items_type[5] = EntityType_Count;
    if (x == EntityType_Count)
        curr = 5;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &curr, items, 6);
    x = items_type[curr];
}
static void imgui_edit_enum_CameraType(CameraType &x, const char *name) {
    const char *items[2];
    CameraType items_type[2];
    int curr = 0;
    items[0] = "CAMERA_TYPE_PERSPECTIVE";
    items_type[0] = CAMERA_TYPE_PERSPECTIVE;
    if (x == CAMERA_TYPE_PERSPECTIVE)
        curr = 0;
    items[1] = "CAMERA_TYPE_ORTHOGRAPHIC";
    items_type[1] = CAMERA_TYPE_ORTHOGRAPHIC;
    if (x == CAMERA_TYPE_ORTHOGRAPHIC)
        curr = 1;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &curr, items, 2);
    x = items_type[curr];
}
static void imgui_edit_enum_EditorMode(EditorMode &x, const char *name) {
    const char *items[2];
    EditorMode items_type[2];
    int curr = 0;
    items[0] = "EDITOR_MODE_GIZMO";
    items_type[0] = EDITOR_MODE_GIZMO;
    if (x == EDITOR_MODE_GIZMO)
        curr = 0;
    items[1] = "EDITOR_MODE_COLLISION_MESH";
    items_type[1] = EDITOR_MODE_COLLISION_MESH;
    if (x == EDITOR_MODE_COLLISION_MESH)
        curr = 1;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &curr, items, 2);
    x = items_type[curr];
}
static void imgui_edit_enum_GizmoMode(GizmoMode &x, const char *name) {
    const char *items[3];
    GizmoMode items_type[3];
    int curr = 0;
    items[0] = "GIZMO_TRANSLATION";
    items_type[0] = GIZMO_TRANSLATION;
    if (x == GIZMO_TRANSLATION)
        curr = 0;
    items[1] = "GIZMO_ROTATION";
    items_type[1] = GIZMO_ROTATION;
    if (x == GIZMO_ROTATION)
        curr = 1;
    items[2] = "GIZMO_SCALE";
    items_type[2] = GIZMO_SCALE;
    if (x == GIZMO_SCALE)
        curr = 2;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &curr, items, 3);
    x = items_type[curr];
}
static void serialize_ThreadWork(FILE *fd, bool w, ThreadWork &x,
                                 Arena *arena = 0) {}
static void serialize_Mesh(FILE *fd, bool w, Mesh &x, Arena *arena = 0) {}
static void serialize_memory_block(FILE *fd, bool w, memory_block &x,
                                   Arena *arena = 0) {}
static void serialize_Mutex(FILE *fd, bool w, Mutex &x, Arena *arena = 0) {}
static void serialize_Material(FILE *fd, bool w, Material &x,
                               Arena *arena = 0) {}
static void serialize_RenderPass(FILE *fd, bool w, RenderPass &x,
                                 Arena *arena = 0) {}
static void serialize_TempMemory(FILE *fd, bool w, TempMemory &x,
                                 Arena *arena = 0) {}
static void serialize_MeshTriangle(FILE *fd, bool w, MeshTriangle &x,
                                   Arena *arena = 0) {}
static void serialize_Shader(FILE *fd, bool w, Shader &x, Arena *arena = 0) {}
static void serialize_Arena(FILE *fd, bool w, Arena &x, Arena *arena = 0) {}
static void serialize_VertexInputElement(FILE *fd, bool w,
                                         VertexInputElement &x,
                                         Arena *arena = 0) {}
static void serialize_GameButton(FILE *fd, bool w, GameButton &x,
                                 Arena *arena = 0) {}
static void serialize_GameInput(FILE *fd, bool w, GameInput &x,
                                Arena *arena = 0) {}
static void serialize_SoundPlaying(FILE *fd, bool w, SoundPlaying &x,
                                   Arena *arena = 0) {}
static void serialize_Platform(FILE *fd, bool w, Platform &x,
                               Arena *arena = 0) {}
static void imgui_edit_struct_Editor(Editor &x, const char *name,
                                     bool collapsed = true) {
    if (!collapsed || ImGui::CollapsingHeader(name)) {
        imgui_edit_enum_EditorMode(x.mode, "mode");
        {
            bool tmp = x.copy_entity_mesh;
            ImGui::Checkbox("copy_entity_mesh", &tmp);
            x.copy_entity_mesh = tmp;
        }
        ImGui::InputScalar("selected_entity", ImGuiDataType_U64,
                           &x.selected_entity);
        ImGui::InputInt("selected_entity_mesh", &x.selected_entity_mesh);
        ImGui::InputScalar("copied_entity", ImGuiDataType_U64,
                           &x.copied_entity);
        ImGui::InputFloat3("last_camera_p", x.last_camera_p.e);
    }
}
static void serialize_Editor(FILE *fd, bool w, Editor &x, Arena *arena = 0) {}
static void serialize_String(FILE *fd, bool w, String &x, Arena *arena = 0) {}
static void serialize_VertexInputLayout(FILE *fd, bool w, VertexInputLayout &x,
                                        Arena *arena = 0) {}
static void serialize_StringHasher(FILE *fd, bool w, StringHasher &x,
                                   Arena *arena = 0) {}
static void serialize_Texture(FILE *fd, bool w, Texture &x, Arena *arena = 0) {}
static void serialize_ShadowMap(FILE *fd, bool w, ShadowMap &x,
                                Arena *arena = 0) {}
static void serialize_Animation(FILE *fd, bool w, Animation &x,
                                Arena *arena = 0) {}
static void serialize_VertexBuffer(FILE *fd, bool w, VertexBuffer &x,
                                   Arena *arena = 0) {}
static void serialize_IndexBuffer(FILE *fd, bool w, IndexBuffer &x,
                                  Arena *arena = 0) {}
static void serialize_FrameBuffer(FILE *fd, bool w, FrameBuffer &x,
                                  Arena *arena = 0) {}
static void serialize_DepthStencilState(FILE *fd, bool w, DepthStencilState &x,
                                        Arena *arena = 0) {}
static void serialize_StateHasher(FILE *fd, bool w, StateHasher &x,
                                  Arena *arena = 0) {}
static void serialize_ProfilerBlock(FILE *fd, bool w, ProfilerBlock &x,
                                    Arena *arena = 0) {}
static void serialize_ConstantBufferElement(FILE *fd, bool w,
                                            ConstantBufferElement &x,
                                            Arena *arena = 0) {}
static void serialize_RasterizerState(FILE *fd, bool w, RasterizerState &x,
                                      Arena *arena = 0) {}
static void serialize_RenderContext(FILE *fd, bool w, RenderContext &x,
                                    Arena *arena = 0) {}
static void serialize_CollisionInfo(FILE *fd, bool w, CollisionInfo &x,
                                    Arena *arena = 0) {}
static void serialize_ConstantBuffer(FILE *fd, bool w, ConstantBuffer &x,
                                     Arena *arena = 0) {}
static void imgui_edit_struct_Game(Game &x, const char *name,
                                   bool collapsed = true) {
    if (!collapsed || ImGui::CollapsingHeader(name)) {
        if (ImGui::CollapsingHeader("scenes")) {
            for (int i = 0; i < x.scenes.count; i++) {
                char elem_name[32];
                snprintf(elem_name, sizeof(elem_name), "scenes[%d]", i);
                imgui_edit_struct_Scene(x.scenes.data[i], elem_name, true);
            }
        }
        {
            bool tmp = x.debug_collision;
            ImGui::Checkbox("debug_collision", &tmp);
            x.debug_collision = tmp;
        }
        ImGui::InputInt("frame", &x.frame);
        ImGui::InputFloat("time", &x.time);
        {
            bool tmp = x.show_normals;
            ImGui::Checkbox("show_normals", &tmp);
            x.show_normals = tmp;
        }
        {
            bool tmp = x.render_bones;
            ImGui::Checkbox("render_bones", &tmp);
            x.render_bones = tmp;
        }
        {
            bool tmp = x.frustum_culling;
            ImGui::Checkbox("frustum_culling", &tmp);
            x.frustum_culling = tmp;
        }
        ImGui::InputFloat("master_volume", &x.master_volume);
        ImGui::ColorEdit3("background_color", x.background_color.e);
    }
}
static void serialize_Game(FILE *fd, bool w, Game &x, Arena *arena = 0) {}
static void serialize_Vertex(FILE *fd, bool w, Vertex &x, Arena *arena = 0) {}
static void serialize_MeshPart(FILE *fd, bool w, MeshPart &x,
                               Arena *arena = 0) {}
static void serialize_Bone(FILE *fd, bool w, Bone &x, Arena *arena = 0) {}
static void serialize_NodeAnimation(FILE *fd, bool w, NodeAnimation &x,
                                    Arena *arena = 0) {}
static void serialize_SceneNode(FILE *fd, bool w, SceneNode &x,
                                Arena *arena = 0) {}
static void imgui_edit_struct_World(World &x, const char *name,
                                    bool collapsed = true) {
    if (!collapsed || ImGui::CollapsingHeader(name)) {
        imgui_edit_struct_Editor(x.editor, "editor", true);
        if (ImGui::CollapsingHeader("entities")) {
            for (int i = 0; i < x.entities.count; i++) {
                char elem_name[32];
                snprintf(elem_name, sizeof(elem_name), "entities[%d]", i);
                imgui_edit_struct_Entity(x.entities.data[i], elem_name, true);
            }
        }
    }
}
static void serialize_World(FILE *fd, bool w, World &x, Arena *arena = 0) {
    if (w) {
        serialize_size_t(fd, w, x.entities.capacity);
        serialize_size_t(fd, w, x.entities.count);
        for (int i = 0; i < x.entities.count; i++)
            serialize_Entity(fd, w, x.entities.data[i], arena);
    } else {
        assert(arena);
        usize capacity, count;
        serialize_size_t(fd, w, capacity);
        serialize_size_t(fd, w, count);
        x.entities = make_array_max<Entity>(arena, capacity);
        for (int i = 0; i < count; i++) {
            Entity y = {};
            serialize_Entity(fd, w, y, arena);
            x.entities.push(y);
        }
    }
    serialize_size_t(fd, w, x.next_entity_id);
    serialize_v3(fd, w, x.player_camera_p);
    serialize_v3(fd, w, x.player_camera_rotation);
    serialize_v3(fd, w, x.player_camera_drotation);
    serialize_v3(fd, w, x.editor_camera_p);
    serialize_v3(fd, w, x.editor_camera_rotation);
    serialize_size_t(fd, w, x.player_id);
    if (w) {
        serialize_size_t(fd, w, x.collision_meshes.capacity);
        serialize_size_t(fd, w, x.collision_meshes.count);
        for (int i = 0; i < x.collision_meshes.count; i++)
            serialize_CollisionMesh(fd, w, x.collision_meshes.data[i], arena);
    } else {
        assert(arena);
        usize capacity, count;
        serialize_size_t(fd, w, capacity);
        serialize_size_t(fd, w, count);
        x.collision_meshes = make_array_max<CollisionMesh>(arena, capacity);
        for (int i = 0; i < count; i++) {
            CollisionMesh y = {};
            serialize_CollisionMesh(fd, w, y, arena);
            x.collision_meshes.push(y);
        }
    }
}
static void imgui_edit_struct_Scene(Scene &x, const char *name,
                                    bool collapsed = true) {
    if (!collapsed || ImGui::CollapsingHeader(name)) {
        ImGui::Text("id = %lu", x.id);
        ImGui::Text("path: \"%.*s\"", (int)x.path.count, x.path.data);
        ImGui::Text("name: \"%.*s\"", (int)x.name.count, x.name.data);
    }
}
static void serialize_Scene(FILE *fd, bool w, Scene &x, Arena *arena = 0) {}
static void serialize_Constants(FILE *fd, bool w, Constants &x,
                                Arena *arena = 0) {}
static void serialize_CollisionMesh(FILE *fd, bool w, CollisionMesh &x,
                                    Arena *arena = 0) {
    if (w) {
        serialize_size_t(fd, w, x.vertices.capacity);
        serialize_size_t(fd, w, x.vertices.count);
        for (int i = 0; i < x.vertices.count; i++)
            serialize_v3(fd, w, x.vertices.data[i], arena);
    } else {
        assert(arena);
        usize capacity, count;
        serialize_size_t(fd, w, capacity);
        serialize_size_t(fd, w, count);
        x.vertices = make_array_max<v3>(arena, capacity);
        for (int i = 0; i < count; i++) {
            v3 y = {};
            serialize_v3(fd, w, y, arena);
            x.vertices.push(y);
        }
    }
    serialize_size_t(fd, w, x.scene);
}
static void serialize_CollisionShape(FILE *fd, bool w, CollisionShape &x,
                                     Arena *arena = 0) {}
static void imgui_edit_struct_Entity(Entity &x, const char *name,
                                     bool collapsed = true) {
    if (!collapsed || ImGui::CollapsingHeader(name)) {
        ImGui::Text("id = %lu", x.id);
        imgui_edit_enum_EntityType(x.type, "type");
        ImGui::InputFloat3("position", x.position.e);
        ImGui::InputFloat3("dp", x.dp.e);
        ImGui::InputFloat4("rotation", x.rotation.e);
        ImGui::InputFloat3("scale", x.scale.e);
        ImGui::ColorEdit3("color", x.color.e);
        {
            bool tmp = x.moved;
            ImGui::Checkbox("moved", &tmp);
            x.moved = tmp;
        }
        {
            bool tmp = x.run;
            ImGui::Checkbox("run", &tmp);
            x.run = tmp;
        }
        {
            bool tmp = x.shooting;
            ImGui::Checkbox("shooting", &tmp);
            x.shooting = tmp;
        }
        {
            bool tmp = x.can_jump;
            ImGui::Checkbox("can_jump", &tmp);
            x.can_jump = tmp;
        }
        {
            bool tmp = x.on_ground;
            ImGui::Checkbox("on_ground", &tmp);
            x.on_ground = tmp;
        }
        {
            bool tmp = x.pressing_jump;
            ImGui::Checkbox("pressing_jump", &tmp);
            x.pressing_jump = tmp;
        }
        {
            bool tmp = x.aiming;
            ImGui::Checkbox("aiming", &tmp);
            x.aiming = tmp;
        }
        {
            bool tmp = x.ellipsoid_collision_shape;
            ImGui::Checkbox("ellipsoid_collision_shape", &tmp);
            x.ellipsoid_collision_shape = tmp;
        }
        ImGui::InputFloat3("ellipsoid_radius", x.ellipsoid_radius.e);
        ImGui::InputScalar("scene_id", ImGuiDataType_U64, &x.scene_id);
        {
            bool tmp = x.disable_collision;
            ImGui::Checkbox("disable_collision", &tmp);
            x.disable_collision = tmp;
        }
        {
            bool tmp = x.walk_backward;
            ImGui::Checkbox("walk_backward", &tmp);
            x.walk_backward = tmp;
        }
        {
            bool tmp = x.jumped;
            ImGui::Checkbox("jumped", &tmp);
            x.jumped = tmp;
        }
        ImGui::InputFloat("height_above_ground", &x.height_above_ground);
        ImGui::InputFloat("point_light_scale", &x.point_light_scale);
    }
}
static void serialize_Entity(FILE *fd, bool w, Entity &x, Arena *arena = 0) {
    serialize_size_t(fd, w, x.id);
    {
        int tmp = x.type;
        serialize_int(fd, w, tmp);
        if (!w)
            x.type = (EntityType)tmp;
    }
    serialize_v3(fd, w, x.position);
    serialize_v3(fd, w, x.dp);
    serialize_quat(fd, w, x.rotation);
    serialize_v3(fd, w, x.scale);
    serialize_v3(fd, w, x.color);
    serialize_int32_t(fd, w, x.moved);
    serialize_int32_t(fd, w, x.run);
    serialize_int32_t(fd, w, x.shooting);
    serialize_int32_t(fd, w, x.can_jump);
    serialize_int32_t(fd, w, x.on_ground);
    serialize_int32_t(fd, w, x.pressing_jump);
    serialize_int32_t(fd, w, x.aiming);
    serialize_bool(fd, w, x.ellipsoid_collision_shape);
    serialize_v3(fd, w, x.ellipsoid_radius);
    serialize_size_t(fd, w, x.scene_id);
    serialize_mat4(fd, w, x.scene_transform);
    serialize_bool(fd, w, x.disable_collision);
    serialize_float(fd, w, x.height_above_ground);
    serialize_float(fd, w, x.point_light_scale);
}
static void serialize_Camera(FILE *fd, bool w, Camera &x, Arena *arena = 0) {}
static void serialize_ProfilerBlockStat(FILE *fd, bool w, ProfilerBlockStat &x,
                                        Arena *arena = 0) {}
static void serialize_EditorOp(FILE *fd, bool w, EditorOp &x,
                               Arena *arena = 0) {}
static void serialize_Gizmo(FILE *fd, bool w, Gizmo &x, Arena *arena = 0) {}
static void serialize_LoadedSound(FILE *fd, bool w, LoadedSound &x,
                                  Arena *arena = 0) {}
static void serialize_SoundState(FILE *fd, bool w, SoundState &x,
                                 Arena *arena = 0) {}
static void serialize_State(FILE *fd, bool w, State &x, Arena *arena = 0) {}
#endif
