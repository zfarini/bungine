function Arena *begin_temp_memory()
{
	assert(g_temp_memory.last_used_count < ARRAY_SIZE(g_temp_memory.last_used));
	g_temp_memory.last_used[g_temp_memory.last_used_count].block = g_temp_memory.arena.block;
	if (g_temp_memory.arena.block)
		g_temp_memory.last_used[g_temp_memory.last_used_count].used = g_temp_memory.arena.block->used;
	else
		g_temp_memory.last_used[g_temp_memory.last_used_count].used = 0;
	g_temp_memory.last_used_count++;
	return &g_temp_memory.arena;
}

function void end_temp_memory()
{
	assert(g_temp_memory.last_used_count > 0);

	memory_block *block = g_temp_memory.arena.block;

	while (block != g_temp_memory.last_used[g_temp_memory.last_used_count - 1].block) {
		memory_block *next = block->next;
		// TODO: cleanup DON'T free here instead just keep the blocks around??
		platform.free_memory(block->data);
		block = next;
	}
	g_temp_memory.arena.block = block;
	if (block)
		g_temp_memory.arena.block->used =  g_temp_memory.last_used[g_temp_memory.last_used_count - 1].used;
	g_temp_memory.last_used_count--;
}

String::String() {
        data = 0;
        count = 0;
}

String::String(const char *cstr) {
    data = (char *)cstr;
    count = 0;
    while (cstr[count])
        count++;
}

char &String::operator[](int index) {
    assert(index >= 0 && index < count);
    return data[index];
}

bool String::operator==(const String &other) const {
    if (count != other.count)
        return false;
    for (int i = 0; i < count; i++)
        if (data[i] != other.data[i])
            return false;
    return true;
}

String make_string(Arena *arena, usize count, const char *data = 0)
{
    String s = {};

    s.data = (char *)arena_alloc(arena, count * sizeof(char));
    s.count = count;
    if (data)
        memcpy(s.data, data, count * sizeof(char));

    return s;
}

String duplicate_string(Arena *arena, String s)
{
    return make_string(arena, s.count, s.data);
}

String concact_string(Arena *arena, String a, String b)
{
    String result = make_string(arena, a.count + b.count);

    memcpy(result.data, a.data, a.count);
    memcpy(result.data + a.count, b.data, b.count);
    return result;
}

String make_zero_string(Arena *arena, String s)
{
    bool add = !s.count || s.data[s.count - 1];
    String result = make_string(arena, s.count + add);

    memcpy(result.data, s.data, s.count);
    if (add)
        result.data[result.count - 1] = 0;
    return result;
}

int find_last_occurence(String s, char c)
{
    for (int i = (int)s.count - 1; i >= 0; i--) {
        if (s[i] == c)
            return i;
    }
    return -1;
}

String substring(String s, int start, int length = -1)
{
    assert(start >= 0 && start <= s.count);
    String result;

    result.data = s.data + start;
    result.count = length == -1 ? s.count - start : length;

    return result;
}

String load_entire_file(Arena *arena, String filename) {
    Arena *temp = begin_temp_memory();

    String result = {};

    String filename_zero = make_zero_string(temp, filename);

    FILE *file = fopen(filename_zero.data, "rb");

    end_temp_memory();

    if (!file) {
        LOG_ERROR("failed to open file %.*s", str_format(filename));
        assert(0);
        return result;
    }
    fseek(file, 0, SEEK_END);
    usize size = ftell(file);
    fseek(file, 0, SEEK_SET);

    result = make_string(arena, size);

    while (size > 0) {
        size_t bytes_read = fread(result.data, 1, size, file);
        if (bytes_read <= 0)
            assert(0);
        size -= bytes_read;
    }
    return result;
}

// returns first multiple of alignement bigger than or equal to x
int align_to(int x, int alignement) {
    return alignement * ((x + alignement - 1) / alignement);
}

Camera make_perspective_camera(mat4 view, float znear, float zfar, float width_fov_degree, float height_over_width)
{
	Camera camera = {};

	camera.type = CAMERA_TYPE_PERSPECTIVE;
	camera.znear = znear;
	camera.zfar = zfar;
	camera.width = 2 * znear * tanf(DEG2RAD * (width_fov_degree / 2));
	camera.height = camera.width * height_over_width;
	camera.view = view;
	camera.projection = perspective_projection(znear, zfar, camera.width, camera.height);
	//TODO: remove inverse
	mat4 inv_view = inverse(view);
	camera.position = V3(inv_view.e[0][3], inv_view.e[1][3], inv_view.e[2][3]);
	camera.right	= V3(inv_view.e[0][0], inv_view.e[1][0], inv_view.e[2][0]);
	camera.up 		= V3(inv_view.e[0][1], inv_view.e[1][1], inv_view.e[2][1]);
	camera.forward	= -V3(inv_view.e[0][2], inv_view.e[1][2], inv_view.e[2][2]);

	return camera;
}
// TODO: merge this with the perspective function
Camera make_orthographic_camera(mat4 view, float znear, float zfar, float width, float height)
{
	Camera camera = {};

	camera.type = CAMERA_TYPE_ORTHOGRAPHIC;
	camera.znear = znear;
	camera.zfar = zfar;
	camera.width = width;
	camera.height = height;
	camera.view = view;
	camera.projection = orthographic_projection(znear, zfar, width, height);
	mat4 inv_view = inverse(view);
	camera.position = V3(inv_view.e[0][3], inv_view.e[1][3], inv_view.e[2][3]);
	camera.right	= V3(inv_view.e[0][0], inv_view.e[0][1], inv_view.e[0][2]);
	camera.up 		= V3(inv_view.e[1][0], inv_view.e[1][1], inv_view.e[1][2]);
	camera.forward	= V3(inv_view.e[2][0], inv_view.e[2][1], inv_view.e[2][2]);

	return camera;
}

