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
void serialize_float(FILE *fd, bool w, float &x) {
    if (w)
        fwrite(&x, sizeof(float), 1, fd);
    else
        fread(&x, sizeof(float), 1, fd);
}
void serialize_double(FILE *fd, bool w, double &x) {
    if (w)
        fwrite(&x, sizeof(double), 1, fd);
    else
        fread(&x, sizeof(double), 1, fd);
}
void serialize_mat4(FILE *fd, bool w, mat4 &x) {
    if (w)
        fwrite(&x, sizeof(mat4), 1, fd);
    else
        fread(&x, sizeof(mat4), 1, fd);
}
void serialize_quat(FILE *fd, bool w, quat &x) {
    if (w)
        fwrite(&x, sizeof(quat), 1, fd);
    else
        fread(&x, sizeof(quat), 1, fd);
}
void serialize_v3(FILE *fd, bool w, v3 &x) {
    if (w)
        fwrite(&x, sizeof(v3), 1, fd);
    else
        fread(&x, sizeof(v3), 1, fd);
}
void serialize_bool(FILE *fd, bool w, bool &x) {
    int v = x;
    if (w)
        fwrite(&v, sizeof("int"), 1, fd);
    else {
        fread(&v, sizeof("int"), 1, fd);
        x = v;
    }
}
void serialize_v2(FILE *fd, bool w, v2 &x) {
    if (w)
        fwrite(&x, sizeof(v2), 1, fd);
    else
        fread(&x, sizeof(v2), 1, fd);
}
void serialize_size_t(FILE *fd, bool w, size_t &x) {
    if (w)
        fwrite(&x, sizeof(size_t), 1, fd);
    else
        fread(&x, sizeof(size_t), 1, fd);
}
void serialize_int32_t(FILE *fd, bool w, int32_t &x) {
    if (w)
        fwrite(&x, sizeof(int32_t), 1, fd);
    else
        fread(&x, sizeof(int32_t), 1, fd);
}
void serialize_int(FILE *fd, bool w, int &x) {
    if (w)
        fwrite(&x, sizeof(int), 1, fd);
    else
        fread(&x, sizeof(int), 1, fd);
}
static StructMetaData get_struct_Constants_info();
static void imgui_edit_struct_Constants(Constants &x, const char *name,
                                        bool collapsed);
static void serialize_Constants(FILE *fd, bool w, Constants &x, Arena *arena);
static StructMetaData get_struct_SoundState_info();
static void imgui_edit_struct_SoundState(SoundState &x, const char *name,
                                         bool collapsed);
static void serialize_SoundState(FILE *fd, bool w, SoundState &x, Arena *arena);
static StructMetaData get_struct_LoadedSound_info();
static void imgui_edit_struct_LoadedSound(LoadedSound &x, const char *name,
                                          bool collapsed);
static void serialize_LoadedSound(FILE *fd, bool w, LoadedSound &x,
                                  Arena *arena);
static StructMetaData get_struct_Editor_info();
static void imgui_edit_struct_Editor(Editor &x, const char *name,
                                     bool collapsed);
static void serialize_Editor(FILE *fd, bool w, Editor &x, Arena *arena);
static StructMetaData get_struct_Camera_info();
static void imgui_edit_struct_Camera(Camera &x, const char *name,
                                     bool collapsed);
static void serialize_Camera(FILE *fd, bool w, Camera &x, Arena *arena);
static StructMetaData get_struct_CollisionTriangle_info();
static void imgui_edit_struct_CollisionTriangle(CollisionTriangle &x,
                                                const char *name,
                                                bool collapsed);
static void serialize_CollisionTriangle(FILE *fd, bool w, CollisionTriangle &x,
                                        Arena *arena);
static StructMetaData get_struct_Scene_info();
static void imgui_edit_struct_Scene(Scene &x, const char *name, bool collapsed);
static void serialize_Scene(FILE *fd, bool w, Scene &x, Arena *arena);
static StructMetaData get_struct_MeshTriangle_info();
static void imgui_edit_struct_MeshTriangle(MeshTriangle &x, const char *name,
                                           bool collapsed);
static void serialize_MeshTriangle(FILE *fd, bool w, MeshTriangle &x,
                                   Arena *arena);
static StructMetaData get_struct_SceneNode_info();
static void imgui_edit_struct_SceneNode(SceneNode &x, const char *name,
                                        bool collapsed);
static void serialize_SceneNode(FILE *fd, bool w, SceneNode &x, Arena *arena);
static StructMetaData get_struct_Bone_info();
static void imgui_edit_struct_Bone(Bone &x, const char *name, bool collapsed);
static void serialize_Bone(FILE *fd, bool w, Bone &x, Arena *arena);
static StructMetaData get_struct_Platform_info();
static void imgui_edit_struct_Platform(Platform &x, const char *name,
                                       bool collapsed);
static void serialize_Platform(FILE *fd, bool w, Platform &x, Arena *arena);
static StructMetaData get_struct_CollisionShape_info();
static void imgui_edit_struct_CollisionShape(CollisionShape &x,
                                             const char *name, bool collapsed);
static void serialize_CollisionShape(FILE *fd, bool w, CollisionShape &x,
                                     Arena *arena);
static StructMetaData get_struct_GameInput_info();
static void imgui_edit_struct_GameInput(GameInput &x, const char *name,
                                        bool collapsed);
static void serialize_GameInput(FILE *fd, bool w, GameInput &x, Arena *arena);
static StructMetaData get_struct_ConstantBuffer_info();
static void imgui_edit_struct_ConstantBuffer(ConstantBuffer &x,
                                             const char *name, bool collapsed);
static void serialize_ConstantBuffer(FILE *fd, bool w, ConstantBuffer &x,
                                     Arena *arena);
static StructMetaData get_struct_Texture_info();
static void imgui_edit_struct_Texture(Texture &x, const char *name,
                                      bool collapsed);
static void serialize_Texture(FILE *fd, bool w, Texture &x, Arena *arena);
static StructMetaData get_struct_Arena_info();
static void imgui_edit_struct_Arena(Arena &x, const char *name, bool collapsed);
static void serialize_Arena(FILE *fd, bool w, Arena &x, Arena *arena);
static StructMetaData get_struct_Game_info();
static void imgui_edit_struct_Game(Game &x, const char *name, bool collapsed);
static void serialize_Game(FILE *fd, bool w, Game &x, Arena *arena);
static StructMetaData get_struct_DepthStencilState_info();
static void imgui_edit_struct_DepthStencilState(DepthStencilState &x,
                                                const char *name,
                                                bool collapsed);
static void serialize_DepthStencilState(FILE *fd, bool w, DepthStencilState &x,
                                        Arena *arena);
static StructMetaData get_struct__IO_FILE_info();
static void imgui_edit_struct__IO_FILE(_IO_FILE &x, const char *name,
                                       bool collapsed);
static void serialize__IO_FILE(FILE *fd, bool w, _IO_FILE &x, Arena *arena);
static StructMetaData get_struct_VertexInputElement_info();
static void imgui_edit_struct_VertexInputElement(VertexInputElement &x,
                                                 const char *name,
                                                 bool collapsed);
static void serialize_VertexInputElement(FILE *fd, bool w,
                                         VertexInputElement &x, Arena *arena);
static StructMetaData get_struct_World_info();
static void imgui_edit_struct_World(World &x, const char *name, bool collapsed);
static void serialize_World(FILE *fd, bool w, World &x, Arena *arena);
static StructMetaData get_struct_Entity_info();
static void imgui_edit_struct_Entity(Entity &x, const char *name,
                                     bool collapsed);
static void serialize_Entity(FILE *fd, bool w, Entity &x, Arena *arena);
static StructMetaData get_struct__G_fpos64_t_info();
static void imgui_edit_struct__G_fpos64_t(_G_fpos64_t &x, const char *name,
                                          bool collapsed);
static void serialize__G_fpos64_t(FILE *fd, bool w, _G_fpos64_t &x,
                                  Arena *arena);
static StructMetaData get_struct_Shader_info();
static void imgui_edit_struct_Shader(Shader &x, const char *name,
                                     bool collapsed);
static void serialize_Shader(FILE *fd, bool w, Shader &x, Arena *arena);
static StructMetaData get_struct_GameButton_info();
static void imgui_edit_struct_GameButton(GameButton &x, const char *name,
                                         bool collapsed);
static void serialize_GameButton(FILE *fd, bool w, GameButton &x, Arena *arena);
static StructMetaData get_struct_RenderPass_info();
static void imgui_edit_struct_RenderPass(RenderPass &x, const char *name,
                                         bool collapsed);
static void serialize_RenderPass(FILE *fd, bool w, RenderPass &x, Arena *arena);
static StructMetaData get_struct_ShadowMap_info();
static void imgui_edit_struct_ShadowMap(ShadowMap &x, const char *name,
                                        bool collapsed);
static void serialize_ShadowMap(FILE *fd, bool w, ShadowMap &x, Arena *arena);
static StructMetaData get_struct_TempArena_info();
static void imgui_edit_struct_TempArena(TempArena &x, const char *name,
                                        bool collapsed);
static void serialize_TempArena(FILE *fd, bool w, TempArena &x, Arena *arena);
static StructMetaData get_struct_RenderContext_info();
static void imgui_edit_struct_RenderContext(RenderContext &x, const char *name,
                                            bool collapsed);
static void serialize_RenderContext(FILE *fd, bool w, RenderContext &x,
                                    Arena *arena);
static StructMetaData get_struct_IndexBuffer_info();
static void imgui_edit_struct_IndexBuffer(IndexBuffer &x, const char *name,
                                          bool collapsed);
static void serialize_IndexBuffer(FILE *fd, bool w, IndexBuffer &x,
                                  Arena *arena);
static StructMetaData get_struct_CollisionInfo_info();
static void imgui_edit_struct_CollisionInfo(CollisionInfo &x, const char *name,
                                            bool collapsed);
static void serialize_CollisionInfo(FILE *fd, bool w, CollisionInfo &x,
                                    Arena *arena);
static StructMetaData get_struct_FrameBuffer_info();
static void imgui_edit_struct_FrameBuffer(FrameBuffer &x, const char *name,
                                          bool collapsed);
static void serialize_FrameBuffer(FILE *fd, bool w, FrameBuffer &x,
                                  Arena *arena);
static StructMetaData get_struct_VertexInputLayout_info();
static void imgui_edit_struct_VertexInputLayout(VertexInputLayout &x,
                                                const char *name,
                                                bool collapsed);
static void serialize_VertexInputLayout(FILE *fd, bool w, VertexInputLayout &x,
                                        Arena *arena);
static StructMetaData get_struct__IO_cookie_io_functions_t_info();
static void
imgui_edit_struct__IO_cookie_io_functions_t(_IO_cookie_io_functions_t &x,
                                            const char *name, bool collapsed);
static void serialize__IO_cookie_io_functions_t(FILE *fd, bool w,
                                                _IO_cookie_io_functions_t &x,
                                                Arena *arena);
static StructMetaData get_struct_Vertex_info();
static void imgui_edit_struct_Vertex(Vertex &x, const char *name,
                                     bool collapsed);
static void serialize_Vertex(FILE *fd, bool w, Vertex &x, Arena *arena);
static StructMetaData get_struct_VertexBuffer_info();
static void imgui_edit_struct_VertexBuffer(VertexBuffer &x, const char *name,
                                           bool collapsed);
static void serialize_VertexBuffer(FILE *fd, bool w, VertexBuffer &x,
                                   Arena *arena);
static StructMetaData get_struct_RasterizerState_info();
static void imgui_edit_struct_RasterizerState(RasterizerState &x,
                                              const char *name, bool collapsed);
static void serialize_RasterizerState(FILE *fd, bool w, RasterizerState &x,
                                      Arena *arena);
static StructMetaData get_struct_ConstantBufferElement_info();
static void imgui_edit_struct_ConstantBufferElement(ConstantBufferElement &x,
                                                    const char *name,
                                                    bool collapsed);
static void serialize_ConstantBufferElement(FILE *fd, bool w,
                                            ConstantBufferElement &x,
                                            Arena *arena);
static StructMetaData get_struct_SoundPlaying_info();
static void imgui_edit_struct_SoundPlaying(SoundPlaying &x, const char *name,
                                           bool collapsed);
static void serialize_SoundPlaying(FILE *fd, bool w, SoundPlaying &x,
                                   Arena *arena);
static StructMetaData get_struct__G_fpos_t_info();
static void imgui_edit_struct__G_fpos_t(_G_fpos_t &x, const char *name,
                                        bool collapsed);
static void serialize__G_fpos_t(FILE *fd, bool w, _G_fpos_t &x, Arena *arena);
static StructMetaData get_struct_Material_info();
static void imgui_edit_struct_Material(Material &x, const char *name,
                                       bool collapsed);
static void serialize_Material(FILE *fd, bool w, Material &x, Arena *arena);
static StructMetaData get_struct_MeshPart_info();
static void imgui_edit_struct_MeshPart(MeshPart &x, const char *name,
                                       bool collapsed);
static void serialize_MeshPart(FILE *fd, bool w, MeshPart &x, Arena *arena);
static StructMetaData get_struct_EditorOp_info();
static void imgui_edit_struct_EditorOp(EditorOp &x, const char *name,
                                       bool collapsed);
static void serialize_EditorOp(FILE *fd, bool w, EditorOp &x, Arena *arena);
static StructMetaData get_struct_Mesh_info();
static void imgui_edit_struct_Mesh(Mesh &x, const char *name, bool collapsed);
static void serialize_Mesh(FILE *fd, bool w, Mesh &x, Arena *arena);
static StructMetaData get_struct_Animation_info();
static void imgui_edit_struct_Animation(Animation &x, const char *name,
                                        bool collapsed);
static void serialize_Animation(FILE *fd, bool w, Animation &x, Arena *arena);
static StructMetaData get_struct_ThreadWork_info();
static void imgui_edit_struct_ThreadWork(ThreadWork &x, const char *name,
                                         bool collapsed);
static void serialize_ThreadWork(FILE *fd, bool w, ThreadWork &x, Arena *arena);
static StructMetaData get_struct_NodeAnimation_info();
static void imgui_edit_struct_NodeAnimation(NodeAnimation &x, const char *name,
                                            bool collapsed);
static void serialize_NodeAnimation(FILE *fd, bool w, NodeAnimation &x,
                                    Arena *arena);
static const char *get_enum_SceneType_str(int value) {
    switch (value) {
    case SCENE_PLAYER:
        return "SCENE_PLAYER";
    case SCENE_CUBE:
        return "SCENE_CUBE";
    case SCENE_SPHERE:
        return "SCENE_SPHERE";
    case SCENE_TEST:
        return "SCENE_TEST";
    }
    return "Enum_SceneType_Unknown";
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
    }
    return "Enum_EditorOpType_Unknown";
}
static const char *get_enum_GizmoMode_str(int value) {
    switch (value) {
    case GIZMO_TRANSLATION:
        return "GIZMO_TRANSLATION";
    case GIZMO_SCALE:
        return "GIZMO_SCALE";
    case GIZMO_ROTATION:
        return "GIZMO_ROTATION";
    }
    return "Enum_GizmoMode_Unknown";
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
static const char *get_enum_InputElementType_str(int value) {
    switch (value) {
    case INPUT_ELEMENT_FLOAT:
        return "INPUT_ELEMENT_FLOAT";
    case INPUT_ELEMENT_SIGNED_INT:
        return "INPUT_ELEMENT_SIGNED_INT";
    }
    return "Enum_InputElementType_Unknown";
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
    case EntityType_Count:
        return "EntityType_Count";
    }
    return "Enum_EntityType_Unknown";
}
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
static const char *get_enum_RasterizerFillMode_str(int value) {
    switch (value) {
    case RASTERIZER_FILL_SOLID:
        return "RASTERIZER_FILL_SOLID";
    case RASTERIZER_FILL_WIREFRAME:
        return "RASTERIZER_FILL_WIREFRAME";
    }
    return "Enum_RasterizerFillMode_Unknown";
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
static const char *get_enum_CollisionShapeType_str(int value) {
    switch (value) {
    case COLLISION_SHAPE_TRIANGLES:
        return "COLLISION_SHAPE_TRIANGLES";
    case COLLISION_SHAPE_ELLIPSOID:
        return "COLLISION_SHAPE_ELLIPSOID";
    }
    return "Enum_CollisionShapeType_Unknown";
}
static const char *get_enum_AnimationType_str(int value) {
    switch (value) {
    case ANIMATION_JUMP:
        return "ANIMATION_JUMP";
    case ANIMATION_SHOOT:
        return "ANIMATION_SHOOT";
    case ANIMATION_RUN:
        return "ANIMATION_RUN";
    case ANIMATION_FORWARD_GUN_WALK:
        return "ANIMATION_FORWARD_GUN_WALK";
    case ANIMATION_BACKWARD_GUN_WALK:
        return "ANIMATION_BACKWARD_GUN_WALK";
    case ANIMATION_GUN_IDLE:
        return "ANIMATION_GUN_IDLE";
    case ANIMATION_COUNT:
        return "ANIMATION_COUNT";
    }
    return "Enum_AnimationType_Unknown";
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
static const char *get_enum_CameraType_str(int value) {
    switch (value) {
    case CAMERA_TYPE_PERSPECTIVE:
        return "CAMERA_TYPE_PERSPECTIVE";
    case CAMERA_TYPE_ORTHOGRAPHIC:
        return "CAMERA_TYPE_ORTHOGRAPHIC";
    }
    return "Enum_CameraType_Unknown";
}
StructMetaData get_struct_Constants_info() {
    StructMetaData data = {};
    data.name = "Constants";
    data.member_count = 14;
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
    data.members[5].name = "camera_p";
    data.members[5].type_name = "v3";
    data.members[6].name = "player_p";
    data.members[6].type_name = "v3";
    data.members[7].name = "color";
    data.members[7].type_name = "v3";
    data.members[8].name = "diffuse_factor";
    data.members[8].type_name = "float";
    data.members[9].name = "specular_factor";
    data.members[9].type_name = "float";
    data.members[10].name = "specular_exponent_factor";
    data.members[10].type_name = "float";
    data.members[11].name = "skinned";
    data.members[11].type_name = "int";
    data.members[12].name = "has_normal_map";
    data.members[12].type_name = "int";
    data.members[13].name = "show_normals";
    data.members[13].type_name = "int";
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
StructMetaData get_struct_Editor_info() {
    StructMetaData data = {};
    data.name = "Editor";
    data.member_count = 18;
    data.members[0].name = "ops";
    data.members[0].type_name = "Array<EditorOp>";
    data.members[1].name = "undos";
    data.members[1].type_name = "Array<EditorOp>";
    data.members[2].name = "init_entity";
    data.members[2].type_name = "Entity";
    data.members[3].name = "in_gizmo";
    data.members[3].type_name = "bool";
    data.members[4].name = "selected_entity";
    data.members[4].type_name = "entity_id";
    data.members[5].name = "copied_entity";
    data.members[5].type_name = "entity_id";
    data.members[6].name = "gizmo_mode";
    data.members[6].type_name = "GizmoMode";
    data.members[7].name = "dragging_axis";
    data.members[7].type_name = "int";
    data.members[8].name = "did_drag";
    data.members[8].type_name = "bool";
    data.members[9].name = "p_init_drag";
    data.members[9].type_name = "v3";
    data.members[10].name = "s_init_scale";
    data.members[10].type_name = "float";
    data.members[11].name = "s_init_drag";
    data.members[11].type_name = "float";
    data.members[12].name = "r_init_rot";
    data.members[12].type_name = "quat";
    data.members[13].name = "r_init_drag";
    data.members[13].type_name = "float";
    data.members[14].name = "r_right_axis";
    data.members[14].type_name = "v3";
    data.members[15].name = "r_up_axis";
    data.members[15].type_name = "v3";
    data.members[16].name = "r_axis";
    data.members[16].type_name = "v3";
    data.members[17].name = "last_camera_p";
    data.members[17].type_name = "v3";
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
StructMetaData get_struct_CollisionTriangle_info() {
    StructMetaData data = {};
    data.name = "CollisionTriangle";
    data.member_count = 3;
    data.members[0].name = "v0";
    data.members[0].type_name = "v3";
    data.members[1].name = "v1";
    data.members[1].type_name = "v3";
    data.members[2].name = "v2";
    data.members[2].type_name = "v3";
    return data;
}
StructMetaData get_struct_Scene_info() {
    StructMetaData data = {};
    data.name = "Scene";
    data.member_count = 7;
    data.members[0].name = "nodes";
    data.members[0].type_name = "Array<SceneNode>";
    data.members[1].name = "meshes";
    data.members[1].type_name = "Array<Mesh>";
    data.members[2].name = "root";
    data.members[2].type_name = "SceneNode*";
    data.members[3].name = "animations";
    data.members[3].type_name = "Array<Animation>";
    data.members[4].name = "path";
    data.members[4].type_name = "String";
    data.members[5].name = "triangles";
    data.members[5].type_name = "Array<MeshTriangle>";
    data.members[6].name = "is_loaded";
    data.members[6].type_name = "int";
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
StructMetaData get_struct_Platform_info() {
    StructMetaData data = {};
    data.name = "Platform";
    data.member_count = 4;
    data.members[0].name = "render_context";
    data.members[0].type_name = "void*";
    data.members[1].name = "imgui_context";
    data.members[1].type_name = "void*";
    data.members[2].name = "temp_arena";
    data.members[2].type_name = "TempArena";
    data.members[3].name = "add_thread_work";
    data.members[3].type_name = "AddThreadWorkFn*";
    return data;
}
StructMetaData get_struct_CollisionShape_info() {
    StructMetaData data = {};
    data.name = "CollisionShape";
    data.member_count = 7;
    data.members[0].name = "type";
    data.members[0].type_name = "CollisionShapeType";
    data.members[1].name = "triangles";
    data.members[1].type_name = "Array<CollisionTriangle>";
    data.members[2].name = "ellipsoid_radius";
    data.members[2].type_name = "v3";
    data.members[3].name = "box_radius";
    data.members[3].type_name = "v3";
    data.members[4].name = "transform";
    data.members[4].type_name = "mat4";
    data.members[5].name = "scale";
    data.members[5].type_name = "v3";
    data.members[6].name = "entity";
    data.members[6].type_name = "entity_id";
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
StructMetaData get_struct_Texture_info() {
    StructMetaData data = {};
    data.name = "Texture";
    data.member_count = 6;
    data.members[0].name = "id";
    data.members[0].type_name = "uint32_t";
    data.members[1].name = "name";
    data.members[1].type_name = "String";
    data.members[2].name = "width";
    data.members[2].type_name = "int";
    data.members[3].name = "height";
    data.members[3].type_name = "int";
    data.members[4].name = "valid";
    data.members[4].type_name = "b32";
    data.members[5].name = "state";
    data.members[5].type_name = "int";
    return data;
}
StructMetaData get_struct_Arena_info() {
    StructMetaData data = {};
    data.name = "Arena";
    data.member_count = 3;
    data.members[0].name = "data";
    data.members[0].type_name = "void*";
    data.members[1].name = "used";
    data.members[1].type_name = "usize";
    data.members[2].name = "size";
    data.members[2].type_name = "usize";
    return data;
}
StructMetaData get_struct_Game_info() {
    StructMetaData data = {};
    data.name = "Game";
    data.member_count = 30;
    data.members[0].name = "memory";
    data.members[0].type_name = "Arena*";
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
    data.members[12].type_name = "Scene";
    data.members[12].is_array = 1;
    data.members[12].array_size = 16;
    data.members[13].name = "animations";
    data.members[13].type_name = "Animation";
    data.members[13].is_array = 1;
    data.members[13].array_size = ANIMATION_COUNT;
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
    data.members[26].name = "show_normals";
    data.members[26].type_name = "bool";
    data.members[27].name = "render_bones";
    data.members[27].type_name = "bool";
    data.members[28].name = "frustum_culling";
    data.members[28].type_name = "bool";
    data.members[29].name = "master_volume";
    data.members[29].type_name = "float";
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
StructMetaData get_struct__IO_FILE_info() {
    StructMetaData data = {};
    data.name = "_IO_FILE";
    data.member_count = 12;
    data.members[0].name = "_flags";
    data.members[0].type_name = "int";
    data.members[1].name = "_IO_read_ptr";
    data.members[1].type_name = "char*";
    data.members[2].name = "_IO_read_end";
    data.members[2].type_name = "char*";
    data.members[3].name = "_IO_read_base";
    data.members[3].type_name = "char*";
    data.members[4].name = "_IO_write_base";
    data.members[4].type_name = "char*";
    data.members[5].name = "_IO_write_ptr";
    data.members[5].type_name = "char*";
    data.members[6].name = "_IO_write_end";
    data.members[6].type_name = "char*";
    data.members[7].name = "_IO_buf_base";
    data.members[7].type_name = "char*";
    data.members[8].name = "_IO_buf_end";
    data.members[8].type_name = "char*";
    data.members[9].name = "_IO_save_base";
    data.members[9].type_name = "char*";
    data.members[10].name = "_IO_backup_base";
    data.members[10].type_name = "char*";
    data.members[11].name = "_IO_save_end";
    data.members[11].type_name = "char*";
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
StructMetaData get_struct_World_info() {
    StructMetaData data = {};
    data.name = "World";
    data.member_count = 15;
    data.members[0].name = "arena";
    data.members[0].type_name = "Arena";
    data.members[1].name = "editor";
    data.members[1].type_name = "Editor";
    data.members[2].name = "entities";
    data.members[2].type_name = "Array<Entity>";
    data.members[3].name = "entities_id_map";
    data.members[3].type_name = "std::unordered_map<entity_id,usize>";
    data.members[4].name = "next_entity_id";
    data.members[4].type_name = "entity_id";
    data.members[5].name = "player_camera_p";
    data.members[5].type_name = "v3";
    data.members[6].name = "player_camera_rotation";
    data.members[6].type_name = "v3";
    data.members[7].name = "player_camera_drotation";
    data.members[7].type_name = "v3";
    data.members[8].name = "editor_camera_p";
    data.members[8].type_name = "v3";
    data.members[9].name = "editor_camera_rotation";
    data.members[9].type_name = "v3";
    data.members[10].name = "editor_selected_entity";
    data.members[10].type_name = "entity_id";
    data.members[11].name = "player_id";
    data.members[11].type_name = "entity_id";
    data.members[12].name = "moving_box";
    data.members[12].type_name = "entity_id";
    data.members[13].name = "last_game_camera";
    data.members[13].type_name = "Camera";
    data.members[14].name = "aim_camera_transition_t";
    data.members[14].type_name = "float";
    return data;
}
StructMetaData get_struct_Entity_info() {
    StructMetaData data = {};
    data.name = "Entity";
    data.member_count = 28;
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
    data.members[15].name = "shape";
    data.members[15].type_name = "CollisionShape";
    data.members[16].name = "scene_id";
    data.members[16].type_name = "SceneID";
    data.members[17].name = "scene_transform";
    data.members[17].type_name = "mat4";
    data.members[18].name = "curr_anim";
    data.members[18].type_name = "Animation*";
    data.members[19].name = "next_anim";
    data.members[19].type_name = "Animation*";
    data.members[20].name = "anim_time";
    data.members[20].type_name = "float";
    data.members[21].name = "blend_time";
    data.members[21].type_name = "float";
    data.members[22].name = "animation";
    data.members[22].type_name = "Animation*";
    data.members[23].name = "speed";
    data.members[23].type_name = "float";
    data.members[24].name = "height_above_ground";
    data.members[24].type_name = "float";
    data.members[25].name = "z_rot";
    data.members[25].type_name = "float";
    data.members[26].name = "last_move";
    data.members[26].type_name = "int";
    data.members[27].name = "last_gun_time";
    data.members[27].type_name = "float";
    return data;
}
StructMetaData get_struct__G_fpos64_t_info() {
    StructMetaData data = {};
    data.name = "_G_fpos64_t";
    data.member_count = 2;
    data.members[0].name = "__pos";
    data.members[0].type_name = "__off64_t";
    data.members[1].name = "__state";
    data.members[1].type_name = "__mbstate_t";
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
StructMetaData get_struct_TempArena_info() {
    StructMetaData data = {};
    data.name = "TempArena";
    data.member_count = 3;
    data.members[0].name = "arena";
    data.members[0].type_name = "Arena";
    data.members[1].name = "last_used";
    data.members[1].type_name = "usize";
    data.members[1].is_array = 1;
    data.members[1].array_size = 16;
    data.members[2].name = "last_used_count";
    data.members[2].type_name = "int";
    return data;
}
StructMetaData get_struct_RenderContext_info() {
    StructMetaData data = {};
    data.name = "RenderContext";
    data.member_count = 8;
    data.members[0].name = "window_width";
    data.members[0].type_name = "int";
    data.members[1].name = "window_height";
    data.members[1].type_name = "int";
    data.members[2].name = "window_framebuffer";
    data.members[2].type_name = "FrameBuffer";
    data.members[3].name = "loaded_textures";
    data.members[3].type_name = "Array<Texture>";
    data.members[4].name = "render_pass";
    data.members[4].type_name = "RenderPass*";
    data.members[5].name = "white_texture";
    data.members[5].type_name = "Texture";
    data.members[6].name = "debug_lines";
    data.members[6].type_name = "Array<v3>";
    data.members[7].name = "active_framebuffer_id";
    data.members[7].type_name = "uintptr_t";
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
StructMetaData get_struct__IO_cookie_io_functions_t_info() {
    StructMetaData data = {};
    data.name = "_IO_cookie_io_functions_t";
    data.member_count = 4;
    data.members[0].name = "read";
    data.members[0].type_name = "cookie_read_function_t*";
    data.members[1].name = "write";
    data.members[1].type_name = "cookie_write_function_t*";
    data.members[2].name = "seek";
    data.members[2].type_name = "cookie_seek_function_t*";
    data.members[3].name = "close";
    data.members[3].type_name = "cookie_close_function_t*";
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
StructMetaData get_struct__G_fpos_t_info() {
    StructMetaData data = {};
    data.name = "_G_fpos_t";
    data.member_count = 2;
    data.members[0].name = "__pos";
    data.members[0].type_name = "__off_t";
    data.members[1].name = "__state";
    data.members[1].type_name = "__mbstate_t";
    return data;
}
StructMetaData get_struct_Material_info() {
    StructMetaData data = {};
    data.name = "Material";
    data.member_count = 8;
    data.members[0].name = "diffuse";
    data.members[0].type_name = "Texture";
    data.members[1].name = "normal_map";
    data.members[1].type_name = "Texture";
    data.members[2].name = "specular";
    data.members[2].type_name = "Texture";
    data.members[3].name = "ambient";
    data.members[3].type_name = "Texture";
    data.members[4].name = "specular_exponent";
    data.members[4].type_name = "Texture";
    data.members[5].name = "diffuse_factor";
    data.members[5].type_name = "float";
    data.members[6].name = "specular_factor";
    data.members[6].type_name = "float";
    data.members[7].name = "specular_exponent_factor";
    data.members[7].type_name = "float";
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
StructMetaData get_struct_Mesh_info() {
    StructMetaData data = {};
    data.name = "Mesh";
    data.member_count = 10;
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
    data.members[8].name = "vertices_count";
    data.members[8].type_name = "usize";
    data.members[9].name = "indices_count";
    data.members[9].type_name = "usize";
    return data;
}
StructMetaData get_struct_Animation_info() {
    StructMetaData data = {};
    data.name = "Animation";
    data.member_count = 5;
    data.members[0].name = "timebegin";
    data.members[0].type_name = "float";
    data.members[1].name = "duration";
    data.members[1].type_name = "float";
    data.members[2].name = "frametime";
    data.members[2].type_name = "float";
    data.members[3].name = "frame_count";
    data.members[3].type_name = "int";
    data.members[4].name = "nodes";
    data.members[4].type_name = "Array<NodeAnimation>";
    return data;
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
static void imgui_edit_enum_SceneType(SceneType &x, const char *name) {
    const char *items[4];
    items[0] = "SCENE_PLAYER";
    items[1] = "SCENE_CUBE";
    items[2] = "SCENE_SPHERE";
    items[3] = "SCENE_TEST";
    int type = x;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &type, items, 4);
    x = (SceneType)type;
}
static void imgui_edit_enum_EditorOpType(EditorOpType &x, const char *name) {
    const char *items[6];
    items[0] = "EDITOR_OP_TRANSLATE_ENTITY";
    items[1] = "EDITOR_OP_ROTATE_ENTITY";
    items[2] = "EDITOR_OP_SCALE_ENTITY";
    items[3] = "EDITOR_OP_PASTE_ENTITY";
    items[4] = "EDITOR_OP_DELETE_ENTITY";
    items[5] = "EDITOR_OP_SPAWN_ENTITY";
    int type = x;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &type, items, 6);
    x = (EditorOpType)type;
}
static void imgui_edit_enum_GizmoMode(GizmoMode &x, const char *name) {
    const char *items[3];
    items[0] = "GIZMO_TRANSLATION";
    items[1] = "GIZMO_SCALE";
    items[2] = "GIZMO_ROTATION";
    int type = x;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &type, items, 3);
    x = (GizmoMode)type;
}
static void imgui_edit_enum_PrimitiveType(PrimitiveType &x, const char *name) {
    const char *items[2];
    items[0] = "PRIMITIVE_TRIANGLES";
    items[1] = "PRIMITIVE_LINES";
    int type = x;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &type, items, 2);
    x = (PrimitiveType)type;
}
static void imgui_edit_enum_VertexBufferUsage(VertexBufferUsage &x,
                                              const char *name) {
    const char *items[2];
    items[0] = "VERTEX_BUFFER_IMMUTABLE";
    items[1] = "VERTEX_BUFFER_DYNAMIC";
    int type = x;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &type, items, 2);
    x = (VertexBufferUsage)type;
}
static void imgui_edit_enum_ShaderType(ShaderType &x, const char *name) {
    const char *items[2];
    items[0] = "SHADER_TYPE_VERTEX";
    items[1] = "SHADER_TYPE_FRAGMENT";
    int type = x;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &type, items, 2);
    x = (ShaderType)type;
}
static void imgui_edit_enum_InputElementType(InputElementType &x,
                                             const char *name) {
    const char *items[2];
    items[0] = "INPUT_ELEMENT_FLOAT";
    items[1] = "INPUT_ELEMENT_SIGNED_INT";
    int type = x;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &type, items, 2);
    x = (InputElementType)type;
}
static void imgui_edit_enum_TextureState(TextureState &x, const char *name) {
    const char *items[3];
    items[0] = "TEXTURE_STATE_UNLOADED";
    items[1] = "TEXTURE_STATE_LOADING";
    items[2] = "TEXTURE_STATE_LOADED";
    int type = x;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &type, items, 3);
    x = (TextureState)type;
}
static void imgui_edit_enum_EntityType(EntityType &x, const char *name) {
    const char *items[5];
    items[0] = "EntityType_Player";
    items[1] = "EntityType_Enemy";
    items[2] = "EntityType_Static";
    items[3] = "EntityType_Projectile";
    items[4] = "EntityType_Count";
    int type = x;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &type, items, 5);
    x = (EntityType)type;
}
static void imgui_edit_enum_GameButtonType(GameButtonType &x,
                                           const char *name) {
    const char *items[45];
    items[0] = "BUTTON_A";
    items[1] = "BUTTON_B";
    items[2] = "BUTTON_C";
    items[3] = "BUTTON_D";
    items[4] = "BUTTON_E";
    items[5] = "BUTTON_F";
    items[6] = "BUTTON_G";
    items[7] = "BUTTON_H";
    items[8] = "BUTTON_I";
    items[9] = "BUTTON_J";
    items[10] = "BUTTON_K";
    items[11] = "BUTTON_L";
    items[12] = "BUTTON_M";
    items[13] = "BUTTON_N";
    items[14] = "BUTTON_O";
    items[15] = "BUTTON_P";
    items[16] = "BUTTON_Q";
    items[17] = "BUTTON_R";
    items[18] = "BUTTON_S";
    items[19] = "BUTTON_T";
    items[20] = "BUTTON_U";
    items[21] = "BUTTON_V";
    items[22] = "BUTTON_W";
    items[23] = "BUTTON_X";
    items[24] = "BUTTON_Y";
    items[25] = "BUTTON_Z";
    items[26] = "BUTTON_LEFT_CONTROL";
    items[27] = "BUTTON_LEFT_SHIFT";
    items[28] = "BUTTON_MOUSE_LEFT";
    items[29] = "BUTTON_MOUSE_RIGHT";
    items[30] = "BUTTON_SPACE";
    items[31] = "BUTTON_ESCAPE";
    items[32] = "BUTTON_F1";
    items[33] = "BUTTON_F2";
    items[34] = "BUTTON_F3";
    items[35] = "BUTTON_F4";
    items[36] = "BUTTON_F5";
    items[37] = "BUTTON_F6";
    items[38] = "BUTTON_F7";
    items[39] = "BUTTON_F8";
    items[40] = "BUTTON_F9";
    items[41] = "BUTTON_F10";
    items[42] = "BUTTON_F11";
    items[43] = "BUTTON_F12";
    items[44] = "BUTTON_COUNT";
    int type = x;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &type, items, 45);
    x = (GameButtonType)type;
}
static void imgui_edit_enum_RasterizerFillMode(RasterizerFillMode &x,
                                               const char *name) {
    const char *items[2];
    items[0] = "RASTERIZER_FILL_SOLID";
    items[1] = "RASTERIZER_FILL_WIREFRAME";
    int type = x;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &type, items, 2);
    x = (RasterizerFillMode)type;
}
static void imgui_edit_enum_RasterizerCullMode(RasterizerCullMode &x,
                                               const char *name) {
    const char *items[3];
    items[0] = "RASTERIZER_CULL_NONE";
    items[1] = "RASTERIZER_CULL_FRONT";
    items[2] = "RASTERIZER_CULL_BACK";
    int type = x;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &type, items, 3);
    x = (RasterizerCullMode)type;
}
static void imgui_edit_enum_CollisionShapeType(CollisionShapeType &x,
                                               const char *name) {
    const char *items[2];
    items[0] = "COLLISION_SHAPE_TRIANGLES";
    items[1] = "COLLISION_SHAPE_ELLIPSOID";
    int type = x;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &type, items, 2);
    x = (CollisionShapeType)type;
}
static void imgui_edit_enum_AnimationType(AnimationType &x, const char *name) {
    const char *items[7];
    items[0] = "ANIMATION_JUMP";
    items[1] = "ANIMATION_SHOOT";
    items[2] = "ANIMATION_RUN";
    items[3] = "ANIMATION_FORWARD_GUN_WALK";
    items[4] = "ANIMATION_BACKWARD_GUN_WALK";
    items[5] = "ANIMATION_GUN_IDLE";
    items[6] = "ANIMATION_COUNT";
    int type = x;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &type, items, 7);
    x = (AnimationType)type;
}
static void
imgui_edit_enum_ConstantBufferElementType(ConstantBufferElementType &x,
                                          const char *name) {
    const char *items[7];
    items[0] = "CONSTANT_BUFFER_ELEMENT_MAT4";
    items[1] = "CONSTANT_BUFFER_ELEMENT_VEC4";
    items[2] = "CONSTANT_BUFFER_ELEMENT_VEC3";
    items[3] = "CONSTANT_BUFFER_ELEMENT_VEC2";
    items[4] = "CONSTANT_BUFFER_ELEMENT_FLOAT";
    items[5] = "CONSTANT_BUFFER_ELEMENT_INT";
    items[6] = "CONSTANT_BUFFER_ELEMENT_COUNT";
    int type = x;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &type, items, 7);
    x = (ConstantBufferElementType)type;
}
static void imgui_edit_enum_CameraType(CameraType &x, const char *name) {
    const char *items[2];
    items[0] = "CAMERA_TYPE_PERSPECTIVE";
    items[1] = "CAMERA_TYPE_ORTHOGRAPHIC";
    int type = x;
    if (ImGui::CollapsingHeader(name))
        ImGui::ListBox(name, &type, items, 2);
    x = (CameraType)type;
}
static void serialize_Constants(FILE *fd, bool w, Constants &x,
                                Arena *arena = 0) {}
static void serialize_SoundState(FILE *fd, bool w, SoundState &x,
                                 Arena *arena = 0) {}
static void serialize_LoadedSound(FILE *fd, bool w, LoadedSound &x,
                                  Arena *arena = 0) {}
static void imgui_edit_struct_Editor(Editor &x, const char *name,
                                     bool collapsed = true) {
    if (!collapsed || ImGui::CollapsingHeader(name)) {
        {
            bool tmp = x.in_gizmo;
            ImGui::Checkbox("in_gizmo", &tmp);
            x.in_gizmo = tmp;
        }
        ImGui::Text("selected_entity = %lu", x.selected_entity);
        ImGui::Text("copied_entity = %lu", x.copied_entity);
        imgui_edit_enum_GizmoMode(x.gizmo_mode, "gizmo_mode");
        ImGui::InputInt("dragging_axis", &x.dragging_axis);
        {
            bool tmp = x.did_drag;
            ImGui::Checkbox("did_drag", &tmp);
            x.did_drag = tmp;
        }
        ImGui::InputFloat3("p_init_drag", x.p_init_drag.e);
        ImGui::InputFloat("s_init_scale", &x.s_init_scale);
        ImGui::InputFloat("s_init_drag", &x.s_init_drag);
        ImGui::InputFloat4("r_init_rot", x.r_init_rot.e);
        ImGui::InputFloat("r_init_drag", &x.r_init_drag);
        ImGui::InputFloat3("r_right_axis", x.r_right_axis.e);
        ImGui::InputFloat3("r_up_axis", x.r_up_axis.e);
        ImGui::InputFloat3("r_axis", x.r_axis.e);
        ImGui::InputFloat3("last_camera_p", x.last_camera_p.e);
    }
}
static void serialize_Editor(FILE *fd, bool w, Editor &x, Arena *arena = 0) {}
static void serialize_Camera(FILE *fd, bool w, Camera &x, Arena *arena = 0) {}
static void imgui_edit_struct_CollisionTriangle(CollisionTriangle &x,
                                                const char *name,
                                                bool collapsed = true) {
    if (!collapsed || ImGui::CollapsingHeader(name)) {
        ImGui::InputFloat3("v0", x.v0.e);
        ImGui::InputFloat3("v1", x.v1.e);
        ImGui::InputFloat3("v2", x.v2.e);
    }
}
static void serialize_CollisionTriangle(FILE *fd, bool w, CollisionTriangle &x,
                                        Arena *arena = 0) {
    serialize_v3(fd, w, x.v0);
    serialize_v3(fd, w, x.v1);
    serialize_v3(fd, w, x.v2);
}
static void serialize_Scene(FILE *fd, bool w, Scene &x, Arena *arena = 0) {}
static void serialize_MeshTriangle(FILE *fd, bool w, MeshTriangle &x,
                                   Arena *arena = 0) {}
static void serialize_SceneNode(FILE *fd, bool w, SceneNode &x,
                                Arena *arena = 0) {}
static void serialize_Bone(FILE *fd, bool w, Bone &x, Arena *arena = 0) {}
static void serialize_Platform(FILE *fd, bool w, Platform &x,
                               Arena *arena = 0) {}
static void imgui_edit_struct_CollisionShape(CollisionShape &x,
                                             const char *name,
                                             bool collapsed = true) {
    if (!collapsed || ImGui::CollapsingHeader(name)) {
        imgui_edit_enum_CollisionShapeType(x.type, "type");
        if (ImGui::CollapsingHeader("triangles")) {
            for (int i = 0; i < x.triangles.count; i++) {
                char elem_name[32];
                snprintf(elem_name, sizeof(elem_name), "triangles[%d]", i);
                imgui_edit_struct_CollisionTriangle(x.triangles.data[i],
                                                    elem_name, true);
            }
        }
        ImGui::InputFloat3("ellipsoid_radius", x.ellipsoid_radius.e);
    }
}
static void serialize_CollisionShape(FILE *fd, bool w, CollisionShape &x,
                                     Arena *arena = 0) {
    {
        int tmp = x.type;
        serialize_int(fd, w, tmp);
        if (!w)
            x.type = (CollisionShapeType)tmp;
    }
    if (w) {
        serialize_size_t(fd, w, x.triangles.capacity);
        serialize_size_t(fd, w, x.triangles.count);
        for (int i = 0; i < x.triangles.count; i++)
            serialize_CollisionTriangle(fd, w, x.triangles.data[i], arena);
    } else {
        assert(arena);
        usize capacity, count;
        serialize_size_t(fd, w, capacity);
        serialize_size_t(fd, w, count);
        x.triangles = make_array_max<CollisionTriangle>(arena, capacity);
        for (int i = 0; i < count; i++) {
            CollisionTriangle y = {};
            serialize_CollisionTriangle(fd, w, y, arena);
            x.triangles.push(y);
        }
    }
    serialize_v3(fd, w, x.ellipsoid_radius);
}
static void serialize_GameInput(FILE *fd, bool w, GameInput &x,
                                Arena *arena = 0) {}
static void serialize_ConstantBuffer(FILE *fd, bool w, ConstantBuffer &x,
                                     Arena *arena = 0) {}
static void serialize_Texture(FILE *fd, bool w, Texture &x, Arena *arena = 0) {}
static void serialize_Arena(FILE *fd, bool w, Arena &x, Arena *arena = 0) {}
static void imgui_edit_struct_Game(Game &x, const char *name,
                                   bool collapsed = true) {
    if (!collapsed || ImGui::CollapsingHeader(name)) {
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
    }
}
static void serialize_Game(FILE *fd, bool w, Game &x, Arena *arena = 0) {}
static void serialize_DepthStencilState(FILE *fd, bool w, DepthStencilState &x,
                                        Arena *arena = 0) {}
static void serialize__IO_FILE(FILE *fd, bool w, _IO_FILE &x,
                               Arena *arena = 0) {}
static void serialize_VertexInputElement(FILE *fd, bool w,
                                         VertexInputElement &x,
                                         Arena *arena = 0) {}
static void imgui_edit_struct_World(World &x, const char *name,
                                    bool collapsed = true) {
    if (!collapsed || ImGui::CollapsingHeader(name)) {
        imgui_edit_struct_Editor(x.editor, "editor");
        if (ImGui::CollapsingHeader("entities")) {
            for (int i = 0; i < x.entities.count; i++) {
                char elem_name[32];
                snprintf(elem_name, sizeof(elem_name), "entities[%d]", i);
                imgui_edit_struct_Entity(x.entities.data[i], elem_name, true);
            }
        }
        ImGui::Text("editor_selected_entity = %lu", x.editor_selected_entity);
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
}
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
        imgui_edit_struct_CollisionShape(x.shape, "shape");
        ImGui::Text("scene_id = %lu", x.scene_id);
        ImGui::InputFloat("height_above_ground", &x.height_above_ground);
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
    serialize_CollisionShape(fd, w, x.shape, arena);
    serialize_size_t(fd, w, x.scene_id);
    serialize_mat4(fd, w, x.scene_transform);
    serialize_float(fd, w, x.height_above_ground);
}
static void serialize__G_fpos64_t(FILE *fd, bool w, _G_fpos64_t &x,
                                  Arena *arena = 0) {}
static void serialize_Shader(FILE *fd, bool w, Shader &x, Arena *arena = 0) {}
static void serialize_GameButton(FILE *fd, bool w, GameButton &x,
                                 Arena *arena = 0) {}
static void serialize_RenderPass(FILE *fd, bool w, RenderPass &x,
                                 Arena *arena = 0) {}
static void serialize_ShadowMap(FILE *fd, bool w, ShadowMap &x,
                                Arena *arena = 0) {}
static void serialize_TempArena(FILE *fd, bool w, TempArena &x,
                                Arena *arena = 0) {}
static void serialize_RenderContext(FILE *fd, bool w, RenderContext &x,
                                    Arena *arena = 0) {}
static void serialize_IndexBuffer(FILE *fd, bool w, IndexBuffer &x,
                                  Arena *arena = 0) {}
static void serialize_CollisionInfo(FILE *fd, bool w, CollisionInfo &x,
                                    Arena *arena = 0) {}
static void serialize_FrameBuffer(FILE *fd, bool w, FrameBuffer &x,
                                  Arena *arena = 0) {}
static void serialize_VertexInputLayout(FILE *fd, bool w, VertexInputLayout &x,
                                        Arena *arena = 0) {}
static void serialize__IO_cookie_io_functions_t(FILE *fd, bool w,
                                                _IO_cookie_io_functions_t &x,
                                                Arena *arena = 0) {}
static void serialize_Vertex(FILE *fd, bool w, Vertex &x, Arena *arena = 0) {}
static void serialize_VertexBuffer(FILE *fd, bool w, VertexBuffer &x,
                                   Arena *arena = 0) {}
static void serialize_RasterizerState(FILE *fd, bool w, RasterizerState &x,
                                      Arena *arena = 0) {}
static void serialize_ConstantBufferElement(FILE *fd, bool w,
                                            ConstantBufferElement &x,
                                            Arena *arena = 0) {}
static void serialize_SoundPlaying(FILE *fd, bool w, SoundPlaying &x,
                                   Arena *arena = 0) {}
static void serialize__G_fpos_t(FILE *fd, bool w, _G_fpos_t &x,
                                Arena *arena = 0) {}
static void serialize_Material(FILE *fd, bool w, Material &x,
                               Arena *arena = 0) {}
static void serialize_MeshPart(FILE *fd, bool w, MeshPart &x,
                               Arena *arena = 0) {}
static void serialize_EditorOp(FILE *fd, bool w, EditorOp &x,
                               Arena *arena = 0) {}
static void serialize_Mesh(FILE *fd, bool w, Mesh &x, Arena *arena = 0) {}
static void serialize_Animation(FILE *fd, bool w, Animation &x,
                                Arena *arena = 0) {}
static void serialize_ThreadWork(FILE *fd, bool w, ThreadWork &x,
                                 Arena *arena = 0) {}
static void serialize_NodeAnimation(FILE *fd, bool w, NodeAnimation &x,
                                    Arena *arena = 0) {}
#endif
