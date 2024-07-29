#ifndef DISABLE_PREPROCESSOR
const char *get_enum_GameButtonType_str(int value)
{
	switch(value){
		case BUTTON_A: return "BUTTON_A";
		case BUTTON_B: return "BUTTON_B";
		case BUTTON_C: return "BUTTON_C";
		case BUTTON_D: return "BUTTON_D";
		case BUTTON_E: return "BUTTON_E";
		case BUTTON_F: return "BUTTON_F";
		case BUTTON_G: return "BUTTON_G";
		case BUTTON_H: return "BUTTON_H";
		case BUTTON_I: return "BUTTON_I";
		case BUTTON_J: return "BUTTON_J";
		case BUTTON_K: return "BUTTON_K";
		case BUTTON_L: return "BUTTON_L";
		case BUTTON_M: return "BUTTON_M";
		case BUTTON_N: return "BUTTON_N";
		case BUTTON_O: return "BUTTON_O";
		case BUTTON_P: return "BUTTON_P";
		case BUTTON_Q: return "BUTTON_Q";
		case BUTTON_R: return "BUTTON_R";
		case BUTTON_S: return "BUTTON_S";
		case BUTTON_T: return "BUTTON_T";
		case BUTTON_U: return "BUTTON_U";
		case BUTTON_V: return "BUTTON_V";
		case BUTTON_W: return "BUTTON_W";
		case BUTTON_X: return "BUTTON_X";
		case BUTTON_Y: return "BUTTON_Y";
		case BUTTON_Z: return "BUTTON_Z";
		case BUTTON_LEFT_CONTROL: return "BUTTON_LEFT_CONTROL";
		case BUTTON_LEFT_SHIFT: return "BUTTON_LEFT_SHIFT";
		case BUTTON_MOUSE_LEFT: return "BUTTON_MOUSE_LEFT";
		case BUTTON_MOUSE_RIGHT: return "BUTTON_MOUSE_RIGHT";
		case BUTTON_SPACE: return "BUTTON_SPACE";
		case BUTTON_F1: return "BUTTON_F1";
		case BUTTON_F2: return "BUTTON_F2";
		case BUTTON_F3: return "BUTTON_F3";
		case BUTTON_F4: return "BUTTON_F4";
		case BUTTON_F5: return "BUTTON_F5";
		case BUTTON_F6: return "BUTTON_F6";
		case BUTTON_F7: return "BUTTON_F7";
		case BUTTON_F8: return "BUTTON_F8";
		case BUTTON_F9: return "BUTTON_F9";
		case BUTTON_F10: return "BUTTON_F10";
		case BUTTON_F11: return "BUTTON_F11";
		case BUTTON_F12: return "BUTTON_F12";
		case BUTTON_COUNT: return "BUTTON_COUNT";
	}
}
const char *get_enum_ShaderType_str(int value)
{
	switch(value){
		case SHADER_TYPE_VERTEX: return "SHADER_TYPE_VERTEX";
		case SHADER_TYPE_FRAGMENT: return "SHADER_TYPE_FRAGMENT";
	}
}
const char *get_enum_InputElementType_str(int value)
{
	switch(value){
		case INPUT_ELEMENT_FLOAT: return "INPUT_ELEMENT_FLOAT";
		case INPUT_ELEMENT_SIGNED_INT: return "INPUT_ELEMENT_SIGNED_INT";
	}
}
const char *get_enum_PrimitiveType_str(int value)
{
	switch(value){
		case PRIMITIVE_TRIANGLES: return "PRIMITIVE_TRIANGLES";
		case PRIMITIVE_LINES: return "PRIMITIVE_LINES";
	}
}
const char *get_enum_VertexBufferUsage_str(int value)
{
	switch(value){
		case VERTEX_BUFFER_IMMUTABLE: return "VERTEX_BUFFER_IMMUTABLE";
		case VERTEX_BUFFER_DYNAMIC: return "VERTEX_BUFFER_DYNAMIC";
	}
}
const char *get_enum_RasterizerFillMode_str(int value)
{
	switch(value){
		case RASTERIZER_FILL_SOLID: return "RASTERIZER_FILL_SOLID";
		case RASTERIZER_FILL_WIREFRAME: return "RASTERIZER_FILL_WIREFRAME";
	}
}
const char *get_enum_RasterizerCullMode_str(int value)
{
	switch(value){
		case RASTERIZER_CULL_NONE: return "RASTERIZER_CULL_NONE";
		case RASTERIZER_CULL_FRONT: return "RASTERIZER_CULL_FRONT";
		case RASTERIZER_CULL_BACK: return "RASTERIZER_CULL_BACK";
	}
}
const char *get_enum_ConstantBufferElementType_str(int value)
{
	switch(value){
		case CONSTANT_BUFFER_ELEMENT_MAT4: return "CONSTANT_BUFFER_ELEMENT_MAT4";
		case CONSTANT_BUFFER_ELEMENT_VEC4: return "CONSTANT_BUFFER_ELEMENT_VEC4";
		case CONSTANT_BUFFER_ELEMENT_VEC3: return "CONSTANT_BUFFER_ELEMENT_VEC3";
		case CONSTANT_BUFFER_ELEMENT_VEC2: return "CONSTANT_BUFFER_ELEMENT_VEC2";
		case CONSTANT_BUFFER_ELEMENT_FLOAT: return "CONSTANT_BUFFER_ELEMENT_FLOAT";
		case CONSTANT_BUFFER_ELEMENT_INT: return "CONSTANT_BUFFER_ELEMENT_INT";
		case CONSTANT_BUFFER_ELEMENT_COUNT: return "CONSTANT_BUFFER_ELEMENT_COUNT";
	}
}
const char *get_enum_EntityType_str(int value)
{
	switch(value){
		case EntityType_Player: return "EntityType_Player";
		case EntityType_Enemy: return "EntityType_Enemy";
		case EntityType_Static: return "EntityType_Static";
	}
}
const char *get_enum_CollisionShapeType_str(int value)
{
	switch(value){
		case COLLISION_SHAPE_TRIANGLES: return "COLLISION_SHAPE_TRIANGLES";
		case COLLISION_SHAPE_ELLIPSOID: return "COLLISION_SHAPE_ELLIPSOID";
	}
}
const char *get_enum_AnimationType_str(int value)
{
	switch(value){
		case ANIMATION_JUMP: return "ANIMATION_JUMP";
		case ANIMATION_SHOOT: return "ANIMATION_SHOOT";
		case ANIMATION_RUN: return "ANIMATION_RUN";
		case ANIMATION_FORWARD_GUN_WALK: return "ANIMATION_FORWARD_GUN_WALK";
		case ANIMATION_BACKWARD_GUN_WALK: return "ANIMATION_BACKWARD_GUN_WALK";
		case ANIMATION_GUN_IDLE: return "ANIMATION_GUN_IDLE";
		case ANIMATION_COUNT: return "ANIMATION_COUNT";
	}
}
const char *get_enum_GizmoMode_str(int value)
{
	switch(value){
		case GIZMO_TRANSLATION: return "GIZMO_TRANSLATION";
		case GIZMO_SCALE: return "GIZMO_SCALE";
		case GIZMO_ROTATION: return "GIZMO_ROTATION";
	}
}
#endif
