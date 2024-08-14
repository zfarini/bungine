# 1 "code/game.cpp"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 450 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "code/game.cpp" 2
# 16 "code/game.cpp"
# 1 "code/common.h" 1








typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef float f32;
typedef double f64;
typedef i32 b32;
typedef size_t usize;
# 17 "code/game.cpp" 2
# 1 "code/arena.h" 1


struct Arena
{
 void *data;
 usize used;
 usize size;
};

static Arena make_arena(void *memory, usize size)
{
 Arena arena = {};

 arena.data = memory;
 arena.size = size;
 return arena;
}



static void *_arena_alloc(const char *filename, const char *func, int line, Arena *arena, usize size)
{
 if (!size)
  return 0;
 int alignement = 16;

 int misalign = ((uintptr_t)((char *)arena->data + arena->used)) & (alignement - 1);
 misalign = misalign ? alignement - misalign : 0;

 if (arena->used + size + misalign > arena->size) {
  printf("%s: %s:%d: failed to allocate %zd bytes (used: %zd, size: %zd, left: %zd, misalign: %d)\n", filename, func, line, size,
   arena->used, arena->size, arena->size - arena->used, misalign);
  assert(0);
  return 0;
 }


 void *ptr = (char *)arena->data + arena->used + misalign;
 arena->used += size + misalign;
 return ptr;
}

static void *arena_alloc_zero(Arena *arena, usize size)
{
 void *mem = _arena_alloc("code/arena.h", __func__, 45, arena, size);
 memset(mem, 0, size);
 return mem;
}

struct TempArena {
 Arena arena;
 usize last_used[16];
 int last_used_count;
};

static TempArena *g_temp_arena;

static Arena *begin_temp_memory()
{
 assert(g_temp_arena->last_used_count < (sizeof(g_temp_arena->last_used) / sizeof(*g_temp_arena->last_used)));
 g_temp_arena->last_used[g_temp_arena->last_used_count++] = g_temp_arena->arena.used;
 return &g_temp_arena->arena;
}

static void end_temp_memory()
{
 assert(g_temp_arena->last_used_count > 0);
 g_temp_arena->arena.used = g_temp_arena->last_used[--g_temp_arena->last_used_count];
}
# 18 "code/game.cpp" 2
# 1 "code/utils.h" 1




template <typename T> T min(T a, T b) { return (a < b ? a : b); }

template <typename T> T max(T a, T b) { return (a > b ? a : b); }

template <typename T> void swap(T &a, T &b) {
    T tmp = a;

    a = b;
    b = tmp;
}

template <typename T> struct Array {
    T *data;
    usize count;
    usize capacity;

    T &operator[](int index) {
        assert(index >= 0 && index < count);
        return data[index];
    }

    T &operator[](usize index) {
        assert(index < count);
        return data[index];
    }

    void push(const T &value) {
        assert(count < capacity);
        data[count++] = value;
    }

    T &back() {
        assert(count);
        return data[count - 1];
    }
};

template <typename T> Array<T> make_array_max(Arena *arena, usize capacity) {
    Array<T> result;

    result.count = 0;
    result.capacity = capacity;
    result.data = (T *)_arena_alloc("code/utils.h", __func__, 47, arena, capacity * sizeof(T));
    return result;
}

template <typename T>
Array<T> make_array(Arena *arena, usize count, const T *data = 0) {
    Array<T> result = make_array_max<T>(arena, count);

    result.count = count;
    if (data && count)
        memcpy(result.data, data, count * sizeof(T));
    return result;
}

template <typename T> Array<T> make_zero_array(Arena *arena, usize count) {
    Array<T> result = make_array<T>(arena, count);
    memset(result.data, 0, sizeof(T) * count);

    return result;
}

template <typename T> Array<T> make_array(T *data, usize count) {
    Array<T> result;

    result.capacity = result.count = count;
    result.data = data;
    return result;
}


template <typename T> Array<T> clone_array(Arena *arena, Array<T> &array) {
    Array<T> result;

    result.data = (T *)_arena_alloc("code/utils.h", __func__, 80, arena, array.capacity * sizeof(T));
    memcpy(result.data, array.data, sizeof(T) * array.count);
    result.count = array.count;
    result.capacity = array.capacity;
    return result;
}







typedef Array<char> String;

String make_string(Arena *arena, usize count, const char *data = 0) {
    return make_array<char>(arena, count, data);
}
b32 strings_equal(const String &a, const String &b) {
    if (a.count != b.count)
        return false;
    for (usize i = 0; i < a.count; i++)
        if (a.data[i] != b.data[i])
            return false;
    return true;
}

String make_cstring(const char *cstr) {
    usize len = 0;
    while (cstr[len])
        len++;
    String s;
    s.data = (char *)cstr;
    s.capacity = len + 1;
    s.count = len;
    return s;
}

String concact_string(Arena *arena, String a, String b) {
    String result = make_string(arena, a.count + b.count);

    memcpy(result.data, a.data, a.count);
    memcpy(result.data + a.count, b.data, b.count);
    return result;
}


String load_entire_file(Arena *arena, String filename) {
    assert(filename.count < filename.capacity &&
           filename.data[filename.count] == 0);
    String result = {};

    FILE *file = fopen(filename.data, "rb");
    if (!file) {
        printf("failed to open file %.*s\n", (int)filename.count, filename.data);
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


int align_to(int x, int alignement) {
    return alignement * ((x + alignement - 1) / alignement);
}

enum LogType {
 LOG_TYPE_DEBUG,
 LOG_TYPE_INFO,
 LOG_TYPE_WARN,
 LOG_TYPE_ERROR,
 LOG_TYPE_FATAL
};



void _log(LogType log_type, const char *fmt, va_list args1, va_list args2)
{
 static FILE *log_file = fopen("log.txt", "w");

 const char *log_type_str[] = {
  "debug",
  "info",
  "warn",
  "error",
  "fatal"
 };
 const char *log_type_color[] = {
  "\033[032m",
  "\033[035m",
  "\033[033m",
  "\033[031m",
  "\033[031m",
 };
 printf("%s[%s]:\033[0m ", log_type_color[log_type], log_type_str[log_type]);
 vprintf(fmt, args1);
 printf("\n");

 fprintf(log_file, "[%s]: ", log_type_str[log_type]);
 vfprintf(log_file, fmt, args2);
 fprintf(log_file, "\n");
}
# 204 "code/utils.h"
void LOG_DEBUG(const char *fmt, ...) { va_list args1, args2; va_start(args1, fmt); va_start(args2, fmt); _log(LOG_TYPE_DEBUG, fmt, args1, args2); va_end(args1); va_end(args2); };
void LOG_INFO(const char *fmt, ...) { va_list args1, args2; va_start(args1, fmt); va_start(args2, fmt); _log(LOG_TYPE_INFO, fmt, args1, args2); va_end(args1); va_end(args2); };
void LOG_WARN(const char *fmt, ...) { va_list args1, args2; va_start(args1, fmt); va_start(args2, fmt); _log(LOG_TYPE_WARN, fmt, args1, args2); va_end(args1); va_end(args2); };
void LOG_ERROR(const char *fmt, ...) { va_list args1, args2; va_start(args1, fmt); va_start(args2, fmt); _log(LOG_TYPE_ERROR, fmt, args1, args2); va_end(args1); va_end(args2); };
void LOG_FATAL(const char *fmt, ...) { va_list args1, args2; va_start(args1, fmt); va_start(args2, fmt); _log(LOG_TYPE_FATAL, fmt, args1, args2); va_end(args1); va_end(args2); };


uint64_t rdtsc() {
 return __rdtsc();






}
# 19 "code/game.cpp" 2
# 1 "code/math.h" 1






int sign(float x) {
    if (x < 0)
        return -1;
    else if (x > 0)
        return 1;
    return 0;
}


union v2 {
    struct {
        float x, y;
    };
    struct {
        float u, v;
    };
    float e[2];
};

v2 V2(float x) { return v2{x, x}; }
v2 V2(float x, float y) { return v2{x, y}; }

v2 operator+(v2 a, v2 b) { return v2{a.x + b.x, a.y + b.y}; }

v2 operator-(v2 a, v2 b) { return v2{a.x - b.x, a.y - b.y}; }

v2 operator*(v2 a, float b) { return v2{a.x * b, a.y * b}; }

v2 operator*(float a, v2 b) { return v2{a * b.x, a * b.y}; }

v2 operator*(v2 a, v2 b) { return v2{a.x * b.x, a.y * b.y}; }

v2 operator/(v2 a, float b) {
    float inv = 1.f / b;
    return v2{a.x * inv, a.y * inv};
}

v2 &operator+=(v2 &a, v2 b) { return a = a + b; }

v2 &operator-=(v2 &a, v2 b) { return a = a - b; }

v2 &operator*=(v2 &a, v2 b) { return a = a * b; }

v2 &operator*=(v2 &a, float b) { return a = a * b; }

v2 &operator/=(v2 &a, float b) { return a = a / b; }

float dot(v2 a, v2 b) { return a.x * b.x + a.y * b.y; }

float length_sq(v2 a) { return dot(a, a); }

float length(v2 a) { return sqrtf(dot(a, a)); }

v2 normalize(v2 a) {
    float len = length_sq(a);


    if (len < 1e-9)
        return {};
    else
        return a / sqrtf(len);
}


union v3 {
    struct {
        float x, y, z;
    };
    struct {
        float r, g, b;
    };
    struct {
        float u, v, w;
    };
    v2 xy;
    float e[3];
};

v3 V3(float x) { return v3{x, x, x}; }

v3 V3(float x, float y, float z) {
    v3 result;

    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}

v3 operator+(v3 a, v3 b) { return v3{a.x + b.x, a.y + b.y, a.z + b.z}; }

v3 operator-(v3 a, v3 b) { return v3{a.x - b.x, a.y - b.y, a.z - b.z}; }

v3 operator*(v3 a, float b) { return v3{a.x * b, a.y * b, a.z * b}; }

v3 operator*(float a, v3 b) { return v3{a * b.x, a * b.y, a * b.z}; }

v3 operator/(v3 a, float b) {
    float inv = 1.f / b;
    return v3{a.x * inv, a.y * inv, a.z * inv};
}

v3 operator*(v3 a, v3 b) { return v3{a.x * b.x, a.y * b.y, a.z * b.z}; }

v3 operator-(v3 a) { return v3{-a.x, -a.y, -a.z}; }

v3 &operator+=(v3 &a, v3 b) { return a = a + b; }

v3 &operator-=(v3 &a, v3 b) { return a = a - b; }

v3 &operator*=(v3 &a, v3 b) { return a = a * b; }

v3 &operator*=(v3 &a, float b) { return a = a * b; }

v3 &operator/=(v3 &a, float b) { return a = a / b; }

v3 operator/(float a, v3 b) { return v3{a / b.x, a / b.y, a / b.z}; }

float dot(v3 a, v3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

float length_sq(v3 a) { return dot(a, a); }

float length(v3 a) { return sqrtf(dot(a, a)); }

v3 normalize(v3 a) {
    float len = length_sq(a);

    if (len < 1e-9)
        return {};
    else
        return a / sqrtf(len);
}

v3 cross(v3 a, v3 b) {
    return v3{a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
              a.x * b.y - a.y * b.x};
}

v3 min(v3 a, v3 b) { return V3(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z)); }

v3 max(v3 a, v3 b) { return V3(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z)); }

bool v3_equal(v3 a, v3 b, float eps = 1e-6) {
    return fabsf(a.x - b.x) < eps && fabsf(a.y - b.y) < eps &&
           fabsf(a.z - b.z) < eps;
}

union v3i {
    struct {
        int x, y, z;
    };
    int e[3];
};

v3i V3i(int x, int y, int z) { return v3i{x, y, z}; }

v3i V3i(int x) { return v3i{x, x, x}; }

v3 V3(v3i v) { return v3{(float)v.x, (float)v.y, (float)v.z}; }

v3i operator+(v3i a, v3i b) { return v3i{a.x + b.x, a.y + b.y, a.z + b.z}; }

v3i operator-(v3i a, v3i b) { return v3i{a.x - b.x, a.y - b.y, a.z - b.z}; }

v3i operator-(v3i a) { return v3i{-a.x, -a.y, -a.z}; }

v3i max(v3i a, v3i b) {
    return V3i(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
}

v3i min(v3i a, v3i b) {
    return V3i(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
}


union v4 {
    struct {
        float x, y, z, w;
    };
    struct {
        float r, g, b, a;
    };
    v3 xyz;
    float e[4];
};

v4 V4(float x, float y, float z, float w) {
    v4 v;

    v.x = x, v.y = y, v.z = z, v.w = w;
    return v;
}

v4 V4(v3 xyz, float w) {
    v4 v;

    v.xyz = xyz;
    v.w = w;
    return v;
}

v4 operator+(v4 a, v4 b) {
    return v4{a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

v4 operator-(v4 a, v4 b) {
    return v4{a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

v4 operator*(v4 a, float b) { return v4{a.x * b, a.y * b, a.z * b, a.w * b}; }

float dot(v4 a, v4 b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }

v4 operator-(v4 a) { return v4{-a.x, -a.y, -a.z, -a.w}; }

float length(v4 a) { return sqrtf(dot(a, a)); }


union mat4 {
    float e[4][4];
    float v[16];
};

mat4 mat4_rows(v4 r0, v4 r1, v4 r2, v4 r3) {
    mat4 m;

    m.e[0][0] = r0.e[0], m.e[0][1] = r0.e[1], m.e[0][2] = r0.e[2],
    m.e[0][3] = r0.e[3];
    m.e[1][0] = r1.e[0], m.e[3][1] = r1.e[1], m.e[1][2] = r1.e[2],
    m.e[1][3] = r1.e[3];
    m.e[2][0] = r2.e[0], m.e[2][1] = r2.e[1], m.e[2][2] = r2.e[2],
    m.e[2][3] = r2.e[3];
    m.e[3][0] = r3.e[0], m.e[1][1] = r3.e[1], m.e[3][2] = r3.e[2],
    m.e[3][3] = r3.e[3];
    return m;
}

mat4 mat4_cols(v4 c0, v4 c1, v4 c2, v4 c3) {
    mat4 m;

    m.e[0][0] = c0.e[0], m.e[1][0] = c0.e[1], m.e[2][0] = c0.e[2],
    m.e[3][0] = c0.e[3];
    m.e[0][1] = c1.e[0], m.e[1][1] = c1.e[1], m.e[2][1] = c1.e[2],
    m.e[3][1] = c1.e[3];
    m.e[0][2] = c2.e[0], m.e[1][2] = c2.e[1], m.e[2][2] = c2.e[2],
    m.e[3][2] = c2.e[3];
    m.e[0][3] = c3.e[0], m.e[1][3] = c3.e[1], m.e[2][3] = c3.e[2],
    m.e[3][3] = c3.e[3];
    return m;
}

mat4 transpose(mat4 a) {
    mat4 b;

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            b.e[i][j] = a.e[j][i];
    return b;
}

mat4 operator*(mat4 a, mat4 b) {
    mat4 c = {};

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 4; k++)
                c.e[i][j] += a.e[i][k] * b.e[k][j];
    return c;
}

v4 operator*(mat4 a, v4 b) {
    v4 c;

    c.e[0] = a.e[0][0] * b.e[0] + a.e[0][1] * b.e[1] + a.e[0][2] * b.e[2] +
             a.e[0][3] * b.e[3];
    c.e[1] = a.e[1][0] * b.e[0] + a.e[1][1] * b.e[1] + a.e[1][2] * b.e[2] +
             a.e[1][3] * b.e[3];
    c.e[2] = a.e[2][0] * b.e[0] + a.e[2][1] * b.e[1] + a.e[2][2] * b.e[2] +
             a.e[2][3] * b.e[3];
    c.e[3] = a.e[3][0] * b.e[0] + a.e[3][1] * b.e[1] + a.e[3][2] * b.e[2] +
             a.e[3][3] * b.e[3];

    return c;
}

mat4 scale(float a) {
    mat4 S = {};

    S.e[0][0] = S.e[1][1] = S.e[2][2] = a;
    S.e[3][3] = 1;
    return S;
}

mat4 scale(float x, float y, float z) {
    mat4 S = {};

    S.e[0][0] = x;
    S.e[1][1] = y;
    S.e[2][2] = z;
    S.e[3][3] = 1;
    return S;
}

mat4 scale(v3 v) { return scale(v.x, v.y, v.z); }

mat4 identity() { return scale(1); }

mat4 translate(float x, float y, float z) {
    mat4 T = identity();

    T.e[0][3] = x;
    T.e[1][3] = y;
    T.e[2][3] = z;
    return T;
}

mat4 translate(v3 t) { return translate(t.x, t.y, t.z); }

mat4 operator*(float a, mat4 b) {
    mat4 c;

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            c.e[i][j] = a * b.e[i][j];
    return c;
}

mat4 zrotation(float a) {
    float c = cosf(a);
    float s = sinf(a);

    return {c, -s, 0, 0, s, c, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
}

mat4 xrotation(float a) {
    float c = cosf(a);
    float s = sinf(a);

    return {1, 0, 0, 0, 0, c, -s, 0, 0, s, c, 0, 0, 0, 0, 1};
}

mat4 yrotation(float a) {
    float c = cosf(a);
    float s = sinf(a);

    return {c, 0, s, 0, 0, 1, 0, 0, -s, 0, c, 0, 0, 0, 0, 1};
}

mat4 rotate_around_axis(v3 u, float a) {
    u = normalize(u);
    float c = cosf(a);
    float s = sinf(a);

    return {c + u.x * u.x * (1 - c),
            u.x * u.y * (1 - c) - u.z * s,
            u.x * u.z * (1 - c) + u.y * s,
            0,
            u.y * u.x * (1 - c) + u.z * s,
            c + u.y * u.y * (1 - c),
            u.y * u.z * (1 - c) - u.x * s,
            0,
            u.z * u.x * (1 - c) - u.y * s,
            u.z * u.y * (1 - c) + u.x * s,
            c + u.z * u.z * (1 - c),
            0,
            0,
            0,
            0,
            1};
}

mat4 perspective_projection(float znear, float zfar, float width, float height) {
    return {znear * 2 / width,
            0,
            0,
            0,
            0,
            znear * 2 / height,
            0,
            0,






            0,
            0,
            (zfar + znear) / (znear - zfar),
            2 * znear * zfar / (znear - zfar),

            0,
            0,
            -1,
            0};
}

mat4 orthographic_projection(float znear, float zfar, float width, float height) {
    return {2.f / width,
            0,
            0,
            0,
            0,
            2.f / height,
            0,
            0,






            0,
            0,
            -2.f / (zfar - znear),
            (-znear - zfar) / (zfar - znear),

            0,
            0,
            0,
            1};
}

mat4 lookat(v3 position, v3 dir, v3 up) {
    v3 z_axis = normalize(-dir);


    v3 x_axis = normalize(cross(up, z_axis));
    v3 y_axis = normalize(cross(z_axis, x_axis));

    mat4 transform = {
        x_axis.x, x_axis.y, x_axis.z, 0, y_axis.x, y_axis.y, y_axis.z, 0,
        z_axis.x, z_axis.y, z_axis.z, 0, 0, 0, 0, 1};

    return transform * translate(-position);
}

using quat = v4;

quat Quat(float x, float y, float z, float w) { return {x, y, z, w}; }

quat operator*(quat a, quat b) {
    return {
        a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
        a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
        a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
        a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
    };
}

mat4 quat_to_mat(quat a) {
    float x = a.x;
    float y = a.y;
    float z = a.z;
    float w = a.w;

    mat4 matrix;

    float wx, wy, wz, xx, yy, yz, xy, xz, zz;

    xx = x * x;
    xy = x * y;
    xz = x * z;
    yy = y * y;
    zz = z * z;
    yz = y * z;

    wx = w * x;
    wy = w * y;
    wz = w * z;

    matrix.v[0] = 1.0f - 2.0f * (yy + zz);
    matrix.v[4] = 2.0f * (xy - wz);
    matrix.v[8] = 2.0f * (xz + wy);
    matrix.v[12] = 0.0;

    matrix.v[1] = 2.0f * (xy + wz);
    matrix.v[5] = 1.0f - 2.0f * (xx + zz);
    matrix.v[9] = 2.0f * (yz - wx);
    matrix.v[13] = 0.0;

    matrix.v[2] = 2.0f * (xz - wy);
    matrix.v[6] = 2.0f * (yz + wx);
    matrix.v[10] = 1.0f - 2.0f * (xx + yy);
    matrix.v[14] = 0.0;

    matrix.v[3] = 0;
    matrix.v[7] = 0;
    matrix.v[11] = 0;
    matrix.v[15] = 1;

    return transpose(matrix);
}

quat quat_lerp(quat a, quat b, float t) {
    float l2 = dot(a, b);
    if (l2 < 0.0f)
        b = -b;
    v4 c;
    c.x = a.x - t * (a.x - b.x);
    c.y = a.y - t * (a.y - b.y);
    c.z = a.z - t * (a.z - b.z);
    c.w = a.w - t * (a.w - b.w);
    return c * (1.f / length(c));
}

mat4 inverse(mat4 matrix) {
    const float *m = (float *)matrix.e;

    float t0 = m[10] * m[15];
    float t1 = m[14] * m[11];
    float t2 = m[6] * m[15];
    float t3 = m[14] * m[7];
    float t4 = m[6] * m[11];
    float t5 = m[10] * m[7];
    float t6 = m[2] * m[15];
    float t7 = m[14] * m[3];
    float t8 = m[2] * m[11];
    float t9 = m[10] * m[3];
    float t10 = m[2] * m[7];
    float t11 = m[6] * m[3];
    float t12 = m[8] * m[13];
    float t13 = m[12] * m[9];
    float t14 = m[4] * m[13];
    float t15 = m[12] * m[5];
    float t16 = m[4] * m[9];
    float t17 = m[8] * m[5];
    float t18 = m[0] * m[13];
    float t19 = m[12] * m[1];
    float t20 = m[0] * m[9];
    float t21 = m[8] * m[1];
    float t22 = m[0] * m[5];
    float t23 = m[4] * m[1];

    mat4 out_matrix;
    float *o = (float *)out_matrix.e;

    o[0] = (t0 * m[5] + t3 * m[9] + t4 * m[13]) -
           (t1 * m[5] + t2 * m[9] + t5 * m[13]);
    o[1] = (t1 * m[1] + t6 * m[9] + t9 * m[13]) -
           (t0 * m[1] + t7 * m[9] + t8 * m[13]);
    o[2] = (t2 * m[1] + t7 * m[5] + t10 * m[13]) -
           (t3 * m[1] + t6 * m[5] + t11 * m[13]);
    o[3] = (t5 * m[1] + t8 * m[5] + t11 * m[9]) -
           (t4 * m[1] + t9 * m[5] + t10 * m[9]);

    float d = 1.0f / (m[0] * o[0] + m[4] * o[1] + m[8] * o[2] + m[12] * o[3]);

    o[0] = d * o[0];
    o[1] = d * o[1];
    o[2] = d * o[2];
    o[3] = d * o[3];
    o[4] = d * ((t1 * m[4] + t2 * m[8] + t5 * m[12]) -
                (t0 * m[4] + t3 * m[8] + t4 * m[12]));
    o[5] = d * ((t0 * m[0] + t7 * m[8] + t8 * m[12]) -
                (t1 * m[0] + t6 * m[8] + t9 * m[12]));
    o[6] = d * ((t3 * m[0] + t6 * m[4] + t11 * m[12]) -
                (t2 * m[0] + t7 * m[4] + t10 * m[12]));
    o[7] = d * ((t4 * m[0] + t9 * m[4] + t10 * m[8]) -
                (t5 * m[0] + t8 * m[4] + t11 * m[8]));
    o[8] = d * ((t12 * m[7] + t15 * m[11] + t16 * m[15]) -
                (t13 * m[7] + t14 * m[11] + t17 * m[15]));
    o[9] = d * ((t13 * m[3] + t18 * m[11] + t21 * m[15]) -
                (t12 * m[3] + t19 * m[11] + t20 * m[15]));
    o[10] = d * ((t14 * m[3] + t19 * m[7] + t22 * m[15]) -
                 (t15 * m[3] + t18 * m[7] + t23 * m[15]));
    o[11] = d * ((t17 * m[3] + t20 * m[7] + t23 * m[11]) -
                 (t16 * m[3] + t21 * m[7] + t22 * m[11]));
    o[12] = d * ((t14 * m[10] + t17 * m[14] + t13 * m[6]) -
                 (t16 * m[14] + t12 * m[6] + t15 * m[10]));
    o[13] = d * ((t20 * m[14] + t12 * m[2] + t19 * m[10]) -
                 (t18 * m[10] + t21 * m[14] + t13 * m[2]));
    o[14] = d * ((t18 * m[6] + t23 * m[14] + t15 * m[2]) -
                 (t22 * m[14] + t14 * m[2] + t19 * m[6]));
    o[15] = d * ((t22 * m[10] + t16 * m[2] + t21 * m[6]) -
                 (t20 * m[6] + t23 * m[10] + t17 * m[2]));

    return out_matrix;
}

template <typename T> T lerp(const T a, const T b, float t) {
    return (1 - t) * a + t * b;
}

bool ray_hit_plane(v3 ray_origin, v3 ray_dir, v3 plane_normal, v3 plane_point,
                   float *hit_t) {
    float denom = dot(ray_dir, plane_normal);
    if (fabsf(denom) < 1e-5)
        return false;
    float t = (dot(plane_normal, plane_point) - dot(ray_origin, plane_normal)) /
              denom;
    if (t >= 0) {
        if (hit_t)
            *hit_t = t;
        return true;
    }
    return false;
}

bool ray_hit_triangle(v3 ray_origin, v3 ray_dir, v3 v0, v3 v1, v3 v2, float *hit_t)
{
 v3 u = v1 - v0;
 v3 v = v2 - v0;
 v3 normal = cross(u, v);

 float t;
 if (!ray_hit_plane(ray_origin, ray_dir, normal, v0, &t))
  return false;

 float one_over_length_n_sq = 1.f/dot(normal, normal);
 v3 p = ray_origin + t * ray_dir - v0;
 float alpha = dot(cross(p, v), normal) * one_over_length_n_sq;
 float beta = -dot(cross(p, u), normal) * one_over_length_n_sq;

 if (alpha >= 0 && beta >= 0 && alpha + beta <= 1) {
  if (hit_t)
   *hit_t = t;
  return true;
 }
 return false;
}

void push_cube_outline(v3 p, v3 r, v3 color);

float ray_hit_box(v3 ray_origin, v3 ray_dir, v3 box_center, v3 box_xaxis,
                  v3 box_yaxis, v3 box_zaxis) {
    struct {
        v3 normal;
        v3 p;
    } planes[6];

    planes[0] = {box_xaxis, box_center + box_xaxis};
    planes[1] = {-box_xaxis, box_center - box_xaxis};
    planes[2] = {box_yaxis, box_center + box_yaxis};
    planes[3] = {-box_yaxis, box_center - box_yaxis};
    planes[4] = {box_zaxis, box_center + box_zaxis};
    planes[5] = {-box_zaxis, box_center - box_zaxis};

    float lx = length(box_xaxis);
    float ly = length(box_yaxis);
    float lz = length(box_zaxis);

    v3 xaxis = normalize(box_xaxis);
    v3 yaxis = normalize(box_yaxis);
    v3 zaxis = normalize(box_zaxis);

    float min_t = FLT_MAX;
    for (int i = 0; i < 6; i++) {







        float denom = dot(ray_dir, planes[i].normal);
        if (fabsf(denom) < 1e-6)
            continue;
        float t = (dot(planes[i].normal, planes[i].p) -
                   dot(ray_origin, planes[i].normal)) /
                  denom;
        if (t >= 0 && t < min_t) {
            v3 p = ray_origin + t * ray_dir - box_center;
            float eps = 1e-4;
            if (dot(p, xaxis) >= -lx - eps && dot(p, xaxis) <= lx + eps &&
                dot(p, yaxis) >= -ly - eps && dot(p, yaxis) <= ly + eps &&
                dot(p, zaxis) >= -lz - eps && dot(p, zaxis) <= lz + eps)
                min_t = t;
        }
    }
    if (min_t == FLT_MAX)
        return -1;
    return min_t;
}

quat rotate_around_axis_quat(v3 axis, float a) {
    axis = normalize(axis);
    float s = sinf(a / 2);
    float c = cosf(a / 2);
    return V4(axis * s, c);
}

quat zrotation_quat(float a) { return rotate_around_axis_quat(V3(0, 0, 1), a); }

quat identity_quat() { return Quat(0, 0, 0, 1); }
# 20 "code/game.cpp" 2
# 1 "code/platform.h" 1


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
# 64 "code/platform.h"
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
# 146 "code/platform.h"
typedef void ThreadWorkFn(void *data);

struct ThreadWork {
 ThreadWorkFn *callback;
 void *data;
};

static ThreadWork thread_work_queue[256];

static volatile int thread_work_queue_read_index;
static volatile int thread_work_queue_write_index;
static volatile int thread_work_queue_occupied_index;
static void *thread_work_semaphore;

typedef bool AddThreadWorkFn(ThreadWorkFn *callback, void *data);

struct Platform {
    void *render_context;
    void *imgui_context;
    TempArena temp_arena;

    AddThreadWorkFn *add_thread_work;
};



typedef void game_update_and_render_fn(Platform &platform, Arena *memory, GameInput &input, float dt);
# 21 "code/game.cpp" 2
# 1 "code/renderer.h" 1


enum TextureState {
    TEXTURE_STATE_UNLOADED,
    TEXTURE_STATE_LOADING,
    TEXTURE_STATE_LOADED,
};

struct Texture {



    uint32_t id;

    String name;
    int width;
    int height;
    b32 valid;


    int state;
};

enum ShaderType {
    SHADER_TYPE_VERTEX,
    SHADER_TYPE_FRAGMENT,
};

struct Shader {
    ShaderType type;
# 39 "code/renderer.h"
    uint32_t id;

};

enum InputElementType {
    INPUT_ELEMENT_FLOAT,
    INPUT_ELEMENT_SIGNED_INT,
};

struct VertexInputElement {
    int offset;
    int count;
    InputElementType type;
    const char *name;
};

enum PrimitiveType {
    PRIMITIVE_TRIANGLES,
    PRIMITIVE_LINES,
};

enum VertexBufferUsage {
    VERTEX_BUFFER_IMMUTABLE,
    VERTEX_BUFFER_DYNAMIC,
};

struct VertexBuffer {
    usize size;
    VertexBufferUsage usage;




    uint32_t vao;
    uint32_t vbo;

};

struct IndexBuffer {

    uint32_t ebo;


};

struct FrameBuffer {




    uint32_t id;


    Texture color_texture;
    Texture depth_texture;
};

struct DepthStencilState {
    bool enable_depth;



};

enum RasterizerFillMode { RASTERIZER_FILL_SOLID, RASTERIZER_FILL_WIREFRAME };

enum RasterizerCullMode {
    RASTERIZER_CULL_NONE,
    RASTERIZER_CULL_FRONT,
    RASTERIZER_CULL_BACK,
};

struct RasterizerState {
    RasterizerFillMode fillmode;
    RasterizerCullMode cullmode;



};

struct VertexInputLayout {
    VertexInputElement elements[64];
    int element_count;
    int vertex_size;



};

struct RenderPass {
    PrimitiveType primitive_type;

    Shader vs;
    Shader fs;
    DepthStencilState depth_stencil_state;
    RasterizerState rasterizer_state;
    VertexInputLayout input_layout;

    uint32_t program;
};

struct RenderContext {
    int window_width;
    int window_height;

    FrameBuffer window_framebuffer;
# 158 "code/renderer.h"
    Array<Texture> loaded_textures;

    RenderPass *render_pass;
    Texture white_texture;

    Array<v3> debug_lines;

    uintptr_t active_framebuffer_id;
};

usize get_input_element_size(int type) {
    if (type == INPUT_ELEMENT_FLOAT)
        return sizeof(float);
    else if (type == INPUT_ELEMENT_SIGNED_INT)
        return sizeof(int);
    else
        assert(0);
    return 0;
}



enum ConstantBufferElementType {
    CONSTANT_BUFFER_ELEMENT_MAT4,
    CONSTANT_BUFFER_ELEMENT_VEC4,
    CONSTANT_BUFFER_ELEMENT_VEC3,
    CONSTANT_BUFFER_ELEMENT_VEC2,
    CONSTANT_BUFFER_ELEMENT_FLOAT,
    CONSTANT_BUFFER_ELEMENT_INT,
    CONSTANT_BUFFER_ELEMENT_COUNT
};

const char *get_constant_buffer_element_typename(ConstantBufferElementType type) {
 switch (type) {
    case CONSTANT_BUFFER_ELEMENT_MAT4:
  return "mat4";
    case CONSTANT_BUFFER_ELEMENT_VEC4:
  return "v4";
    case CONSTANT_BUFFER_ELEMENT_VEC3:
  return "v3";
    case CONSTANT_BUFFER_ELEMENT_VEC2:
  return "v2";
    case CONSTANT_BUFFER_ELEMENT_FLOAT:
  return "float";
    case CONSTANT_BUFFER_ELEMENT_INT:
  return "int";
    default:
        assert(0);
    }
 return "";
}

int get_c_type_alignement(ConstantBufferElementType type) {
    switch (type) {
    case CONSTANT_BUFFER_ELEMENT_MAT4:
        return alignof(mat4);
    case CONSTANT_BUFFER_ELEMENT_VEC4:
        return alignof(v4);
    case CONSTANT_BUFFER_ELEMENT_VEC3:
        return alignof(v3);
    case CONSTANT_BUFFER_ELEMENT_VEC2:
        return alignof(v2);
    case CONSTANT_BUFFER_ELEMENT_FLOAT:
        return alignof(float);
    case CONSTANT_BUFFER_ELEMENT_INT:
        return alignof(int);
    default:
        assert(0);
    }
    return 0;
}

struct ConstantBufferElement {
    ConstantBufferElementType type;
    int array_size;
};

struct ConstantBuffer {
    usize size;
    ConstantBufferElement elements[64];
    int element_count;



    uint32_t id;

};
# 22 "code/game.cpp" 2


static RenderContext *g_rc;




# 1 "code/renderer_opengl.cpp" 1
Texture create_texture(String name, void *data, int width, int height, bool srgb = true,
  bool mipmapping = true)
{
 Texture texture = {};

 texture.width = width;
 texture.height = height;
 texture.name = name;
 texture.valid = true;

 uint32_t internal_format, format;

 if (srgb)
  internal_format = GL_SRGB_ALPHA;
 else
  internal_format = GL_RGBA;

 format = GL_RGBA;

 uint32_t tex;
 glGenTextures(1, &tex);

 glBindTexture(GL_TEXTURE_2D, tex);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
 if (mipmapping)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
    GL_LINEAR_MIPMAP_LINEAR);
 else
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
    GL_LINEAR);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

 glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format,
   GL_UNSIGNED_BYTE, data);
 if (mipmapping)
  glGenerateMipmap(GL_TEXTURE_2D);

 texture.id = tex;
 return texture;
}

void bind_texture(int index, Texture &texture)
{
 glActiveTexture(GL_TEXTURE0 + index);
 glBindTexture(GL_TEXTURE_2D, (uintptr_t)texture.id);
}

Shader load_shader(String filename, ShaderType type, const char *main = "")
{
 (void)main;

 Shader result = {};

 result.type = type;

 Arena *temp = begin_temp_memory();

 String content = load_entire_file(temp, filename);
 int gl_type = 0;
 if (type == SHADER_TYPE_VERTEX)
  gl_type = GL_VERTEX_SHADER;
 else if (type == SHADER_TYPE_FRAGMENT)
  gl_type = GL_FRAGMENT_SHADER;
 else
  assert(0);

 uint32_t shader = glCreateShader(gl_type);
 glShaderSource(shader, 1, &content.data, (GLint *)&content.count);

 end_temp_memory();

 glCompileShader(shader);

 int success;
 glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
 if (!success) {
  char info_log[2048];
  glGetShaderInfoLog(shader, sizeof(info_log), 0, info_log);
  LOG_ERROR("[opengl] failed to compile shader \"%.*s\": %s", (int)filename.count, filename.data, info_log);
  assert(0);
 }

 result.id = shader;
 return result;

}

RasterizerState create_rasterizer_state(RasterizerFillMode fillmode, RasterizerCullMode cullmode)
{
 RasterizerState result = {};

 result.fillmode = fillmode;
 result.cullmode = cullmode;

 return result;
}

DepthStencilState create_depth_stencil_state(bool enable_depth)
{
 DepthStencilState result = {};

 result.enable_depth = enable_depth;
 return result;
}

void bind_rasterizer_state(const RasterizerState &state)
{
 if (state.fillmode == RASTERIZER_FILL_SOLID)
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
 else if (state.fillmode == RASTERIZER_FILL_WIREFRAME)
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

 if (state.cullmode == RASTERIZER_CULL_NONE)
  glDisable(GL_CULL_FACE);
 else if (state.cullmode == RASTERIZER_CULL_FRONT) {
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
 }
 else if (state.cullmode == RASTERIZER_CULL_BACK) {
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
 }
}

void bind_depth_stencil_state(const DepthStencilState &state)
{
 if (state.enable_depth)
  glEnable(GL_DEPTH_TEST);
 else
  glDisable(GL_DEPTH_TEST);
}

VertexInputLayout create_vertex_input_layout(VertexInputElement *elements, int element_count,
  int vertex_size)
{
 VertexInputLayout result = {};

 result.vertex_size = vertex_size;
 result.element_count = element_count;
 for (int i = 0; i < element_count; i++)
  result.elements[i] = elements[i];

 return result;
}

RenderPass create_render_pass(Shader vs, Shader fs,
  PrimitiveType primitive_type, const DepthStencilState &depth_stencil_state,
  const RasterizerState &rasterizer_state, const VertexInputLayout &input_layout)
{
 RenderPass rp = {};

 rp.vs = vs;
 rp.fs = fs;
 rp.depth_stencil_state = depth_stencil_state;
 rp.rasterizer_state = rasterizer_state;
 rp.primitive_type = primitive_type;
 rp.input_layout = input_layout;

 rp.program = glCreateProgram();

 glAttachShader(rp.program, vs.id);
 glAttachShader(rp.program, fs.id);
 glLinkProgram(rp.program);

 int success;
 glGetProgramiv(rp.program, GL_LINK_STATUS, &success);
 if (!success) {
  char info_log[2048];
  glGetProgramInfoLog(rp.program, sizeof(info_log), 0, info_log);
  LOG_ERROR("[opengl] failed to link program: %s", info_log);
  assert(0);
 }

 return rp;
}

void begin_render_pass(RenderPass &rp)
{
 g_rc->render_pass = &rp;
 glUseProgram(rp.program);
 bind_depth_stencil_state(rp.depth_stencil_state);
 bind_rasterizer_state(rp.rasterizer_state);
}

void end_render_pass()
{
 g_rc->render_pass = 0;
}

void set_viewport(float top_left_x, float top_left_y, float width, float height)
{
 glViewport(top_left_x, top_left_y, width, height);
}

void bind_framebuffer(FrameBuffer &framebuffer)
{

 glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);
}

void clear_framebuffer_color(FrameBuffer &framebuffer, v4 color)
{
 bind_framebuffer(framebuffer);
 glClearColor(color.x, color.y, color.z, color.w);
 glClear(GL_COLOR_BUFFER_BIT);
}

void clear_framebuffer_depth(FrameBuffer &framebuffer, float depth)
{
 bind_framebuffer(framebuffer);
 glClearDepth(depth);
 glClear(GL_DEPTH_BUFFER_BIT);
}


IndexBuffer create_index_buffer(usize size, uint32_t *indices)
{
 IndexBuffer result = {};

 glGenBuffers(1, &result.ebo);
 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, result.ebo);
 glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(uint32_t), indices, GL_STATIC_DRAW);

 return result;
}

VertexBuffer create_vertex_buffer(VertexBufferUsage usage, usize size, void *data = 0)
{
 VertexBuffer result = {};

 result.usage = usage;
 result.size = size;

 glGenVertexArrays(1, &result.vao);
 glBindVertexArray(result.vao);

 glGenBuffers(1, &result.vbo);
 glBindBuffer(GL_ARRAY_BUFFER, result.vbo);
 if (data)
  glBufferData(GL_ARRAY_BUFFER, size, data, usage == VERTEX_BUFFER_IMMUTABLE ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);

 return result;
}

void update_vertex_buffer(VertexBuffer &vb, int size, void *data)
{
 assert(vb.usage == VERTEX_BUFFER_DYNAMIC);
 glBindVertexArray(vb.vao);
 glBindBuffer(GL_ARRAY_BUFFER, vb.vbo);
 glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
}

void bind_vertex_buffer(VertexBuffer &vb)
{
 auto &layout = g_rc->render_pass->input_layout;

 glBindVertexArray(vb.vao);
 glBindBuffer(GL_ARRAY_BUFFER, vb.vbo);

 for (int i = 0; i < layout.element_count; i++) {
  int type = 0;
  if (layout.elements[i].type == INPUT_ELEMENT_FLOAT)
   type = GL_FLOAT;
  else if (layout.elements[i].type == INPUT_ELEMENT_SIGNED_INT)
   type = GL_INT;
  else
   assert(0);

  glVertexAttribPointer(i, layout.elements[i].count,
    type, GL_FALSE, (GLsizei) layout.vertex_size, (void *)layout.elements[i].offset);
  glEnableVertexAttribArray(i);
 }
}


void bind_index_buffer(IndexBuffer &ib)
{
 assert(g_rc->render_pass);
 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib.ebo);
}

void draw(int offset, int vertices_count)
{
 int mode = 0;
 if (g_rc->render_pass->primitive_type == PRIMITIVE_TRIANGLES)
  mode = GL_TRIANGLES;
 else if (g_rc->render_pass->primitive_type == PRIMITIVE_LINES)
  mode = GL_LINES;
 else
  assert(0);
 glDrawArrays(mode, offset, vertices_count);
}

void draw_indexed(int offset, int indices_count)
{
 int mode = 0;
 if (g_rc->render_pass->primitive_type == PRIMITIVE_TRIANGLES)
  mode = GL_TRIANGLES;
 else if (g_rc->render_pass->primitive_type == PRIMITIVE_LINES)
  mode = GL_LINES;
 else
  assert(0);
 glDrawElements(mode, indices_count, GL_UNSIGNED_INT, (void *)offset);
}

int get_constant_buffer_element_size(int type)
{
 int size[] = {
  sizeof(mat4),
  sizeof(v4),
  sizeof(v3),
  sizeof(v2),
  sizeof(float),
  sizeof(int)
 };
 static_assert((sizeof(size) / sizeof(*size)) == CONSTANT_BUFFER_ELEMENT_COUNT);
 return size[type];
}


int get_constant_buffer_element_alignement(int type)
{
 int align[] = {
  alignof(mat4),
  alignof(v4),
  alignof(v3),
  alignof(v2),
  alignof(float),
  alignof(int)
 };
 static_assert((sizeof(align) / sizeof(*align)) == CONSTANT_BUFFER_ELEMENT_COUNT);
 return align[type];
}

int get_type_alignement(ConstantBufferElement e)
{
 int align = 0;
 if (e.array_size > 0)
  align = 4 * alignof(float);
 else if (e.type == CONSTANT_BUFFER_ELEMENT_VEC2)
  align = 2 * alignof(float);
 else if (e.type == CONSTANT_BUFFER_ELEMENT_VEC3 || e.type == CONSTANT_BUFFER_ELEMENT_VEC4
   || e.type == CONSTANT_BUFFER_ELEMENT_MAT4)
  align = 4 * alignof(float);
 else if (e.type == CONSTANT_BUFFER_ELEMENT_INT || e.type == CONSTANT_BUFFER_ELEMENT_FLOAT)
  align = alignof(float);
 else
  assert(0);
 return align;
}

int get_type_size(ConstantBufferElement e)
{
 return get_constant_buffer_element_size(e.type);
}


ConstantBuffer create_constant_buffer(Array<ConstantBufferElement> elements)
{
 ConstantBuffer result = {};

 assert(elements.count < (sizeof(result.elements) / sizeof(*result.elements)));
 int offset = 0;
 for (int i = 0; i < elements.count; i++) {
  result.elements[i] = elements[i];

  offset = align_to(offset, get_type_alignement(elements[i]));
  if (elements[i].array_size) {


   int stride = align_to(get_type_size(elements[i]), sizeof(v4));
   offset += stride * elements[i].array_size;
   offset = align_to(offset, get_type_alignement(elements[i]));
  }
  else
   offset += get_type_size(elements[i]);
 }

 result.element_count = (int)elements.count;
 result.size = offset;

 glGenBuffers(1, &result.id);
 glBindBuffer(GL_UNIFORM_BUFFER, result.id);

 glBufferData(GL_UNIFORM_BUFFER, result.size, 0, GL_DYNAMIC_DRAW);

 return result;
}

void update_constant_buffer(ConstantBuffer &buffer, void *data)
{
 char dest[sizeof(mat4) * 512] = {};

 assert(buffer.size < sizeof(dest));

 int cstruct_offset = 0;
 int ubo_offset = 0;

 for (int i = 0; i < buffer.element_count; i++) {
  ConstantBufferElement e = buffer.elements[i];

  ubo_offset = align_to(ubo_offset, get_type_alignement(e));
  cstruct_offset = align_to(cstruct_offset, get_constant_buffer_element_alignement(e.type));

  int count = e.array_size ? e.array_size : 1;
  for (int j = 0; j < count; j++) {
   memcpy(dest + ubo_offset, (char *)data + cstruct_offset, get_constant_buffer_element_size(e.type));
   ubo_offset += get_type_size(e);
   cstruct_offset += get_constant_buffer_element_size(e.type);
  }
 }


 glBindBuffer(GL_UNIFORM_BUFFER, buffer.id);
 glBufferSubData(GL_UNIFORM_BUFFER, 0, buffer.size, (void *)dest);
}

void bind_constant_buffer(ConstantBuffer &cbuffer, int index)
{
 glBindBufferBase(GL_UNIFORM_BUFFER, index, cbuffer.id);
}

void begin_render_frame()
{
 g_rc->debug_lines.count = 0;



 ImGui_ImplGlfw_NewFrame();

 ImGui_ImplOpenGL3_NewFrame();
 ImGui::NewFrame();
}

void end_render_frame()
{
 bind_framebuffer(g_rc->window_framebuffer);
 ImGui::Render();
 ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void APIENTRY gl_debug_output(GLenum source, GLenum type, unsigned int id,
  GLenum severity, GLsizei length,
  const char *message, const void *userParam) {
 if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
  return;

 const char *s_source = "", *s_type = "", *s_severity = "";

 switch (source) {
  case GL_DEBUG_SOURCE_API: s_source = "API"; break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM: s_source = "Window System"; break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER: s_source = "Shader Compiled"; break;
  case GL_DEBUG_SOURCE_THIRD_PARTY: s_source = "Third Party"; break;
  case GL_DEBUG_SOURCE_APPLICATION: s_source = "Application"; break;
  case GL_DEBUG_SOURCE_OTHER: s_source = "Other"; break;
 }

 switch (type) {
  case GL_DEBUG_TYPE_ERROR: s_type = "Error"; break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: s_type = "Deprecated Behaviour"; break ;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: s_type = "Undefined Behaviour"; break;
  case GL_DEBUG_TYPE_PORTABILITY: s_type = "Portability"; break;
  case GL_DEBUG_TYPE_PERFORMANCE: s_type = "Performance"; break;
  case GL_DEBUG_TYPE_MARKER: s_type = "Marker"; break;
  case GL_DEBUG_TYPE_PUSH_GROUP: s_type = "Push Group"; break;
  case GL_DEBUG_TYPE_POP_GROUP: s_type = "Pop Group"; break;
  case GL_DEBUG_TYPE_OTHER: s_type = "Other"; break;
 }

 switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH: s_severity = "high"; break;
  case GL_DEBUG_SEVERITY_MEDIUM: s_severity = "meduim"; break;
  case GL_DEBUG_SEVERITY_LOW: s_severity = "low"; break;
  case GL_DEBUG_SEVERITY_NOTIFICATION: s_severity = "notification";break;
 }

 LOG_WARN("[opengl debug layer (id: %d, source: %s, type: %s, severity: %s)]: %s",
   id, s_source, s_type, s_severity, message);
}

void init_render_context_opengl(RenderContext &rc, Platform &platform)
{

 int flags;
 glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
 if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(gl_debug_output, 0);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
    0, GL_TRUE);
 }

 rc.window_framebuffer.id = 0;
 glEnable(GL_FRAMEBUFFER_SRGB);

 glLineWidth(1.5f);
}


Texture create_depth_texture(int width, int height)
{
 Texture result = {};

 uint32_t texture;

 glGenTextures(1, &texture);
 glBindTexture(GL_TEXTURE_2D, texture);
 glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height,
   0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

 result.id = texture;
 result.valid = true;
 result.name = make_cstring("shadow map depth texture");
 return result;
}

FrameBuffer create_frame_buffer(bool depth_only = false, bool read = false)
{
 FrameBuffer result = {};
 uint32_t fbo;

 glGenFramebuffers(1, &fbo);
 glBindFramebuffer(GL_FRAMEBUFFER, fbo);
 glDrawBuffer(GL_NONE);
 glReadBuffer(GL_NONE);

 if (!depth_only) {
  if (read)
   glReadBuffer(GL_COLOR_ATTACHMENT0);
  GLenum buffers[] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, buffers);
 }



 result.id = fbo;
 return result;
}

void bind_framebuffer_depthbuffer(FrameBuffer &framebuffer, Texture &texture)
{
 framebuffer.depth_texture = texture;
 bind_framebuffer(framebuffer);
 glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
   texture.id, 0);
}

void bind_framebuffer_color(FrameBuffer &framebuffer, Texture &texture)
{
 framebuffer.color_texture = texture;
 bind_framebuffer(framebuffer);
 glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
   texture.id, 0);
}
# 30 "code/game.cpp" 2


# 1 "code/scene.h" 1




typedef usize SceneID;


struct Vertex {
    v3 position;
    v3 normal;
    v2 uv;



    float weights[4];
    float indices[4];
};


VertexInputElement g_vertex_input_elements[] = {
    {offsetof(Vertex, position), 3, INPUT_ELEMENT_FLOAT, "POSITION"},
    {offsetof(Vertex, normal), 3, INPUT_ELEMENT_FLOAT, "NORMAL"},
    {offsetof(Vertex, uv), 2, INPUT_ELEMENT_FLOAT, "TEXCOORD"},
    {offsetof(Vertex, weights), 4, INPUT_ELEMENT_FLOAT,
     "BLENDWEIGHT"},
    {offsetof(Vertex, indices), 4, INPUT_ELEMENT_FLOAT,
     "BLENDINDICES"},
};

struct Material {
    Texture diffuse;
    Texture normal_map;
    Texture specular;
    Texture ambient;
    Texture specular_exponent;

    float diffuse_factor;
    float specular_factor;
    float specular_exponent_factor;
};

struct MeshPart {
    Material material;
    usize indices_count;
    usize offset;
};

struct Bone {
    String name;
    mat4 transform;
    mat4 inv_bind;
    int parent;
};

struct Mesh {
    String name;
    Array<MeshPart> parts;

    VertexBuffer vertex_buffer;
    IndexBuffer index_buffer;

    Array<Bone> bones;
    v3 box_min;
    v3 box_max;
    mat4 transform;
 mat4 default_transform;

    usize vertices_count;
    usize indices_count;


    Array<v3> vertices;
 Array<u32> indices;
};

struct NodeAnimation {
    String name;
    Array<v3> position;
    Array<v3> scale;
    Array<quat> rotation;

    v3 const_position;
    v3 const_scale;
    quat const_rotation;

    mat4 transform;
};

struct Animation {
    float timebegin;
    float duration;

    float frametime;

    int frame_count;

    Array<NodeAnimation> nodes;
};

struct SceneNode {
    String name;
    SceneNode *parent;
    Array<SceneNode *> childs;
    Mesh *mesh;
    mat4 local_transform;
    mat4 geometry_transform;
    int id;
    b32 skip_render;
};

struct MeshTriangle {
    v3 v0, v1, v2;
};

struct Scene {
 meta(ui: const) SceneID id;
    meta(ui) String path;
 meta(ui) String name;
 Array<Mesh> meshes;
};
# 33 "code/game.cpp" 2
# 1 "code/game.h" 1




struct CollisionMesh {
 meta(serialize) Array<v3> vertices;
 meta(serialize) SceneID scene;
};

struct CollisionShape
{
 CollisionMesh collision_mesh;
 v3 ellipsoid_radius;
 bool ellipsoid;

 mat4 transform;
 v3 scale;
};

struct CollisionInfo
{
 v3 hit_p;
 v3 hit_normal;
 float t;
};

enum EntityType {
    EntityType_Player,
    EntityType_Enemy,
    EntityType_Static,
    EntityType_Projectile,
 EntityType_PointLight,
    EntityType_Count,
};

typedef usize entity_id;

struct Entity {
    meta(ui: const, serialize) entity_id id;

 entity_id parent;

    meta(ui, serialize) EntityType type;
    meta(ui, serialize) v3 position;
    meta(ui, serialize) v3 dp;

    meta(ui, serialize) quat rotation;
    meta(ui, serialize) v3 scale;

    meta(ui: color, serialize) v3 color;

    meta(ui, serialize) b32 moved;
    meta(ui, serialize) b32 run;
    meta(ui, serialize) b32 shooting;
    meta(ui, serialize) b32 can_jump;
    meta(ui, serialize) b32 on_ground;
    meta(ui, serialize) b32 pressing_jump;
    meta(ui, serialize) b32 aiming;

 meta(ui, serialize) bool ellipsoid_collision_shape;
 meta(ui, serialize) v3 ellipsoid_radius;

    meta(ui, serialize) SceneID scene_id;

    meta(ui, serialize) mat4 scene_transform;

 meta(ui, serialize) bool disable_collision;


    Animation *curr_anim;
    Animation *next_anim;
    float anim_time;
    float blend_time;

    Animation *animation;

    float speed;

    meta(ui: const, serialize) float height_above_ground;

 meta(ui, serialize) float point_light_scale;

    float z_rot;
    int last_move;

    float last_gun_time;
 float last_jump_z;
 bool should_jump;
};

enum AnimationType {
    ANIMATION_JUMP,
    ANIMATION_SHOOT,
    ANIMATION_RUN,
    ANIMATION_FORWARD_GUN_WALK,
    ANIMATION_BACKWARD_GUN_WALK,
    ANIMATION_GUN_IDLE,
    ANIMATION_COUNT
};

struct ShadowMap {
    FrameBuffer framebuffer;
    v3 light_dir;
    v3 light_p;
    mat4 view;
    mat4 projection;
    int tex_width, tex_height;

    float znear, zfar;
    float width, height;

    Texture depth_texture;
};

enum CameraType {
    CAMERA_TYPE_PERSPECTIVE,
    CAMERA_TYPE_ORTHOGRAPHIC,
};

struct Camera {
    CameraType type;

    v3 position;
    mat4 view;
    mat4 projection;

    float znear, zfar, width, height;
    v3 forward, right, up;
};

enum EditorMode {
 EDITOR_MODE_GIZMO,
 EDITOR_MODE_COLLISION_MESH,
};

enum EditorOpType {
    EDITOR_OP_TRANSLATE_ENTITY,
    EDITOR_OP_ROTATE_ENTITY,
    EDITOR_OP_SCALE_ENTITY,
    EDITOR_OP_PASTE_ENTITY,
    EDITOR_OP_DELETE_ENTITY,
    EDITOR_OP_SPAWN_ENTITY,

 EDITOR_OP_CREATE_MESH_COLLISION_VERTEX,
 EDITOR_OP_DELETE_MESH_COLLISION_TRIANGLE,
};

struct EditorOp {
    int type;
    entity_id entity;

    union {
        struct {
            v3 prev_p;
            v3 new_p;
        } translate;
        struct {
            quat prev_rot;
            quat new_rot;
        } rotate;
        struct {
            v3 prev_scale;
            v3 new_scale;
        } scale;
        struct {
            entity_id copy_from;
            entity_id id;
   int mesh_index;
            v3 p;
        } paste;
        struct {
            Entity entity_data;
        } del;
  struct {
   v3 pos;
  } place_collision_vertex;
  struct {
   int index;
   v3 v0, v1, v2;
  } delete_collision_triangle;
    };
};

enum GizmoMode {
 GIZMO_TRANSLATION,
 GIZMO_ROTATION,
 GIZMO_SCALE
};

struct Gizmo {
 GizmoMode mode;
 v3 init_position;
 quat init_rotation;
 v3 init_scale;

 bool active;

    int dragging_axis;
 bool did_drag;
 bool uniform_scale;


 v3 hit_p;

 v3 delta_p;

 v3 scale;
 float delta_s;

 float rotation_angle;
 quat rotation;
 v3 rotation_right_axis;
 v3 rotation_up_axis;
 v3 rotation_axis;
};

struct Editor {
 meta(ui) EditorMode mode;

    Array<EditorOp> ops;
    Array<EditorOp> undos;

 Gizmo gizmo;

 meta(ui) bool copy_entity_mesh;

    meta(ui) entity_id selected_entity;
 meta(ui) int selected_entity_mesh;

    meta(ui) entity_id copied_entity;

    meta(ui) v3 last_camera_p;
};

struct World {
 std::unordered_map<uint64_t, bool> occupied;

    Arena arena;

    meta(ui) Editor editor;

    meta(ui, serialize) Array<Entity> entities;

    std::unordered_map<entity_id, usize> entities_id_map;
 std::unordered_map<SceneID, int> scene_collision_mesh;

    meta(serialize) entity_id next_entity_id;

    meta(serialize) v3 player_camera_p;
    meta(serialize) v3 player_camera_rotation;
    meta(serialize) v3 player_camera_drotation;

    meta(serialize) v3 editor_camera_p;
    meta(serialize) v3 editor_camera_rotation;

    meta(serialize) entity_id player_id;

    entity_id moving_box;

    Camera last_game_camera;

    float aim_camera_transition_t;

 meta(serialize) Array<CollisionMesh> collision_meshes;
};

struct LoadedSound
{
    int sample_count;
    float *samples;
};

struct SoundPlaying
{
    entity_id entity;
    int samples_played;
    LoadedSound *sound;
    SoundPlaying *next;
    SoundPlaying *prev;
};

struct SoundState
{
 float *buffer;
    int sample_count;

 std::atomic_int32_t read_index;
 std::atomic_int32_t write_index;
};

struct ProfilerBlockStat
{
 const char *name;
 uint64_t cycle_count;
 uint32_t call_count;
};

static ProfilerBlockStat profiler_block_stats[512];


struct ProfilerBlock
{
 uint64_t start_cycle;
 uint32_t id;
 const char *name;

 ProfilerBlock(const char *name, uint32_t id) : name(name), id(id)
 {
  profiler_block_stats[id].name = name;
  start_cycle = rdtsc();
 }

 ~ProfilerBlock()
 {
  uint64_t cycle_count = rdtsc() - start_cycle;

  assert(id < (sizeof(profiler_block_stats) / sizeof(*profiler_block_stats)));
  profiler_block_stats[id].cycle_count += cycle_count;
  profiler_block_stats[id].call_count++;
 }
};





struct Game {

    Arena *memory;


    std::atomic_bool is_initialized;

    World *world;

    ShadowMap shadow_map;

    RenderPass mesh_render_pass;
    RenderPass shadow_map_render_pass;
    RenderPass outline_render_pass;
    ConstantBuffer constant_buffer;

    RenderPass debug_lines_render_pass;
    VertexBuffer debug_lines_vertex_buffer;
    ConstantBuffer debug_lines_constant_buffer;

    Arena asset_arena;

 meta(ui) Array<Scene> scenes;
 usize next_scene_id;

    Animation animations[ANIMATION_COUNT];

    b32 in_editor;
    meta(ui) bool debug_collision;

    meta(ui) int frame;
    meta(ui) float time;

    RasterizerState default_rasterizer_state, wireframe_rasterizer_state;
    DepthStencilState default_depth_stencil_state;
    DepthStencilState disable_depth_state;

    FrameBuffer debug_asset_fb;
    Texture debug_asset_tex;

    SoundState sound_state;

    SoundPlaying *first_playing_sound;
    LoadedSound loaded_sounds[32];

 Scene default_scene;

    meta(ui) bool show_normals;
    meta(ui) bool render_bones;
    meta(ui) bool frustum_culling;
    meta(ui) float master_volume;

 bool play_in_editor;
};

struct Constants {
    mat4 view;
    mat4 projection;
    mat4 model;
    mat4 light_transform;
    mat4 bones[96];

 v4 point_light_color[8];
 v4 point_light_position[8];
 int point_light_count;

    v3 camera_p;
    v3 player_p;
    v3 color;
    float diffuse_factor;
    float specular_factor;
    float specular_exponent_factor;
    int skinned;
    int has_normal_map;
    int show_normals;
};



v3i get_cell(v3 p)
{
 v3i res;

 res.x = roundf(p.x / ((0.8f)));
 res.y = roundf(p.y / ((0.8f)));
 res.z = roundf(p.z / ((0.8f)));
 return res;
}

const int MAX_CELL_POW = 15;
const int MAX_CELL = (1 << MAX_CELL_POW);

uint64_t pack_cell(v3i c)
{
 assert(abs(c.x) < MAX_CELL/2 && abs(c.y) < MAX_CELL/2 && abs(c.z) < MAX_CELL/2);
 return ((uint64_t)(c.x + MAX_CELL/2)) | ((c.y + MAX_CELL/2) << MAX_CELL_POW) | ((c.z+ MAX_CELL/2) << (MAX_CELL_POW*2));
}
v3i unpack_cell(uint64_t x)
{
 v3i res;

 res.x = (x >> 0) & (MAX_CELL - 1);
 res.y = (x >> MAX_CELL_POW) & (MAX_CELL - 1);
 res.z = (x >> (MAX_CELL_POW * 2)) & (MAX_CELL - 1);
 return res - V3i(MAX_CELL/2, MAX_CELL/2, MAX_CELL/2);
};

v3 get_closest_point_in_cell(v3i cell, v3 p)
{
 v3 box_min = V3(cell) * (0.8f) - 0.5f * V3((0.8f));
 v3 box_max = V3(cell) * (0.8f) + 0.5f * V3((0.8f));

 v3 result;
 result.x = (p.x >= box_min.x && p.x <= box_max.x ? p.x :
      (p.x <= box_min.x ? box_min.x : box_max.x));
 result.y = (p.y >= box_min.y && p.y <= box_max.y ? p.y :
      (p.y <= box_min.y ? box_min.y : box_max.y));
 result.z = (p.z >= box_min.z && p.z <= box_max.z ? p.z :
      (p.z <= box_min.z ? box_min.z : box_max.z));
 return result;
}

void render_cell(v3i x, float s = 1, v3 color = V3(1, 1, 0))
{
 push_cube_outline(V3(x) * (0.8f), V3((0.8f)*0.5f * s), color);
}

struct State
{
 v3i p;


 int jump;
 int fscore;




 bool operator==(const State &rhs) const
 {
  return p.x == rhs.p.x && p.y == rhs.p.y && p.z == rhs.p.z && jump == rhs.jump;
 }
 bool operator<(const State &rhs) const
 {
  if (fscore == rhs.fscore)
   return pack_cell(p) < pack_cell(rhs.p);
  return fscore < rhs.fscore;
 }
};

struct StateHasher
{
  std::size_t operator()(const State& k) const
  {
   return pack_cell(k.p) | ((uint64_t)(k.jump + 1) << (3 * MAX_CELL_POW));
  }
};
# 34 "code/game.cpp" 2
# 1 "code/scene.cpp" 1



v3 ufbx_to_v3(ufbx_vec3 v)
{
 return v3{(float)v.x, (float)v.y, (float)v.z};
}

v2 ufbx_to_v2(ufbx_vec2 v)
{
 return v2{(float)v.x, (float)v.y};
}

quat ufbx_to_quat(ufbx_quat q)
{
 return quat{(float)q.x, (float)q.y, (float)q.z, (float)q.w};
}

mat4 ufbx_to_mat4(ufbx_matrix m)
{
 mat4 result = {};

 for (int i = 0; i < 4; i++) {
  result.e[0][i] = m.cols[i].x;
  result.e[1][i] = m.cols[i].y;
  result.e[2][i] = m.cols[i].z;
 }

 result.e[3][3] = 1;

 return (result);
}
# 50 "code/scene.cpp"
Texture load_texture(Arena *arena, Scene &scene, ufbx_texture *utex, bool srgb = true)
{
 assert(utex->type == UFBX_TEXTURE_FILE);


 stbi_set_flip_vertically_on_load(true);

 String name = make_string(arena, utex->filename.length, utex->filename.data);
 assert(name.count);

 if (name.count) {
  for (int i = 0; i < g_rc->loaded_textures.count; i++) {
   if (strings_equal(g_rc->loaded_textures[i].name, name))
    return g_rc->loaded_textures[i];
  }
 }
 void *data;
 int width, height, n_channels;
 Arena *temp = begin_temp_memory();
 if (utex->content.size) {
  data = stbi_load_from_memory((stbi_uc *)utex->content.data,
    (int)utex->content.size, &width, &height,
    &n_channels, 4);
 } else {
  int last_slash = (int)utex->filename.length - 1;

  while (last_slash >= 0 &&
    (utex->filename.data[last_slash] != '/' &&
     utex->filename.data[last_slash] != '\\'))
   last_slash--;
  last_slash++;

  String path = concact_string(temp, scene.path, make_string(temp, utex->filename.length - last_slash, utex->filename.data + last_slash));
  path = concact_string(temp, path, make_string(temp, 1, ""));


  data = stbi_load(path.data, &width, &height, &n_channels, 4);
  if (!data)
   printf("failed to load texture file: %s\n", path.data);
 }

 end_temp_memory();

 Texture texture = create_texture(name, data, width, height, srgb);
 if (name.count)
  g_rc->loaded_textures.push(texture);

 return texture;
}

Material load_material(Arena *arena, Scene &scene, ufbx_material *umat)
{
 Material mat = {};

 if (umat->fbx.diffuse_color.texture_enabled)
  mat.diffuse = load_texture(arena, scene, umat->fbx.diffuse_color.texture);
 if (umat->fbx.specular_color.texture_enabled)
  mat.specular = load_texture(arena, scene, umat->fbx.specular_color.texture);
 if (umat->fbx.normal_map.texture_enabled)
  mat.normal_map = load_texture(arena, scene, umat->fbx.normal_map.texture, false);
 else if (umat->fbx.bump.texture_enabled)
  mat.normal_map = load_texture(arena, scene, umat->fbx.bump.texture, false);

 if (umat->fbx.specular_exponent.texture_enabled)
  mat.specular_exponent = load_texture(arena, scene, umat->fbx.specular_exponent.texture, false);

 mat.diffuse_factor = umat->fbx.diffuse_factor.has_value ? umat->fbx.diffuse_factor.value_real : 1;
 mat.specular_factor = umat->fbx.specular_factor.has_value ? umat->fbx.specular_factor.value_real : 1;
 mat.specular_exponent_factor = umat->fbx.specular_exponent.has_value ? umat->fbx.specular_exponent.value_real : 20;
 return mat;
}

Vertex get_ufbx_vertex(ufbx_mesh *umesh, ufbx_skin_deformer *skin, int index)
{
 Vertex v = {};

 v.position = ufbx_to_v3(ufbx_get_vertex_vec3(&umesh->vertex_position, index));
 v.normal = ufbx_to_v3(ufbx_get_vertex_vec3(&umesh->vertex_normal, index));
 if (umesh->vertex_uv.exists)
  v.uv = ufbx_to_v2(ufbx_get_vertex_vec2(&umesh->vertex_uv, index));

 if (skin) {
  uint32_t vertex = umesh->vertex_indices.data[index];

  ufbx_skin_vertex skin_vertex = skin->vertices.data[vertex];
  size_t num_weights = skin_vertex.num_weights;
  if (num_weights > 4)
   num_weights = 4;

  float total_weight = 0.0f;
  for (size_t i = 0; i < num_weights; i++) {
   ufbx_skin_weight skin_weight = skin->weights.data[skin_vertex.weight_begin + i];
   v.indices[i] = skin_weight.cluster_index;
   v.weights[i] = (float)skin_weight.weight;
   total_weight += (float)skin_weight.weight;
  }

  for (size_t i = 0; i < num_weights; i++)
   v.weights[i] /= total_weight;
 }
 return v;
}

void make_bones_transform_relative_to_parent(Array<Bone> bones, usize index, Array<bool> computed)
{
 if (computed[index] || bones[index].parent == -1)
  return ;
 make_bones_transform_relative_to_parent(bones, bones[index].parent, computed);
 mat4 M = identity();
 int i = bones[index].parent;
 while (i != -1)
 {
  M = bones[i].transform * M;
  i = bones[i].parent;
 }
 bones[index].transform = inverse(M) * bones[index].transform;
 computed[index] = true;
}

Mesh load_mesh(Arena *arena, Scene &scene, ufbx_node *unode)
{
 ufbx_mesh *umesh = unode->mesh;

 Mesh mesh = {};

 mesh.name = make_string(arena, unode->name.length, unode->name.data);
 mesh.parts = make_array<MeshPart>(arena, umesh->material_parts.count);

 Arena *tmp_arena = begin_temp_memory();

 Array<uint32_t> tri_indices = make_array<uint32_t>(tmp_arena, umesh->max_face_triangles * 3);

 Array<Vertex> vertices = make_array_max<Vertex>(tmp_arena, umesh->num_triangles * 3);
 Array<uint32_t> indices = make_array_max<uint32_t>(tmp_arena, umesh->num_triangles * 3);

 ufbx_skin_deformer *skin = 0;

 if (umesh->skin_deformers.count) {
  assert(umesh->skin_deformers.count == 1);
  skin = umesh->skin_deformers.data[0];
  assert(skin->skinning_method == UFBX_SKINNING_METHOD_LINEAR);


  mesh.bones = make_array<Bone>(arena, skin->clusters.count);
  for (usize i = 0; i < mesh.bones.count; i++) {
   ufbx_skin_cluster *cluster = skin->clusters.data[i];

   mesh.bones[i].inv_bind = ufbx_to_mat4(cluster->geometry_to_bone);
   mesh.bones[i].transform = inverse(mesh.bones[i].inv_bind);
   mesh.bones[i].parent = -1;
   mesh.bones[i].name = make_string(arena, cluster->bone_node->name.length,
     cluster->bone_node->name.data);


   for (int j = 0; j < mesh.bones.count; j++) {
    if (cluster->bone_node->parent == skin->clusters.data[j]->bone_node) {
     mesh.bones[i].parent = j;
     break ;
    }
   }






   if (mesh.bones[i].parent == -1)
    assert(cluster->bone_node->parent->bone == 0);
  }
  Array<bool> computed = make_zero_array<bool>(tmp_arena, mesh.bones.count);
  for (usize i = 0; i < mesh.bones.count; i++)
   make_bones_transform_relative_to_parent(mesh.bones, i, computed);
 }
 uint32_t max_used_index = 0;
 for (usize part_idx = 0; part_idx < umesh->material_parts.count; part_idx++) {
  MeshPart &part = mesh.parts[part_idx];
  part.offset = indices.count;

  ufbx_mesh_part upart = umesh->material_parts.data[part_idx];
  uint32_t num_vertices = 0;

  for (usize face_idx = 0; face_idx < upart.num_faces; face_idx++) {
   ufbx_face face = umesh->faces.data[upart.face_indices.data[face_idx]];
   uint32_t num_tris = ufbx_triangulate_face(tri_indices.data, tri_indices.count, umesh, face);

   for (size_t i = 0; i < num_tris * 3; i++) {
    max_used_index = max(max_used_index, tri_indices[i]);
    indices.push(tri_indices[i]);
   }
  }
  if (upart.face_indices.count && umesh->face_material.count) {

   part.material = load_material(arena, scene,
     umesh->materials.data[umesh->face_material.data[upart.face_indices.data[0]]]);
  }

  part.indices_count = indices.count - part.offset;
 }

 for (uint32_t index = 0; index < max_used_index + 1; index++)
  vertices.push(get_ufbx_vertex(umesh, skin, index));

 mesh.vertex_buffer = create_vertex_buffer(VERTEX_BUFFER_IMMUTABLE,
   vertices.count * sizeof(Vertex), vertices.data);
 mesh.index_buffer = create_index_buffer(indices.count, indices.data);


 mesh.box_min = V3(1e18);
 mesh.box_max = V3(-1e18);
 for (usize i = 0; i < indices.count; i++) {
  mesh.box_min = min(mesh.box_min, vertices[(int)indices[i]].position);
  mesh.box_max = max(mesh.box_max, vertices[(int)indices[i]].position);
 }

 mesh.vertices_count = vertices.count;
 mesh.indices_count = indices.count;

 mesh.vertices = make_array<v3>(arena, vertices.count);


 for (usize i = 0; i < vertices.count; i++) {
  mesh.vertices[i] = vertices[i].position;
 }


 mesh.indices = clone_array(arena, indices);
# 285 "code/scene.cpp"
 end_temp_memory();
 return mesh;
}

void *ufbx_arena_realloc(void *user, void *old_ptr, size_t old_size, size_t new_size)
{
 Arena *arena = (Arena *)user;

 void *data = _arena_alloc("code/scene.cpp", __func__, 293, arena, new_size);

 if (old_size > new_size)
  old_size = new_size;
 if (old_size)
  memcpy(data, old_ptr, old_size);
 return data;
}

b32 quat_equal(quat a, quat b)
{
 const float eps = 1e-9;
 return fabsf(a.x - b.x) < eps &&
  fabsf(a.y - b.y) < eps &&
  fabsf(a.z - b.z) < eps &&
  fabsf(a.w - b.w) < eps;
}

mat4 get_animated_node_transform(Animation &anim, NodeAnimation &node, float anim_time)
{
 int frame = anim_time / anim.frametime;
 if (frame >= anim.frame_count)
  frame = anim.frame_count - 1;

 int next_frame = frame + 1 == anim.frame_count ? frame : frame + 1;
 float t = (anim_time - frame * anim.frametime) / anim.frametime;

 v3 position = node.const_position;
 quat rotation = node.const_rotation;
 v3 s = node.const_scale;

 if (node.position.count)
  position = lerp(node.position[frame], node.position[next_frame], t);
 if (node.rotation.count)
  rotation = quat_lerp(node.rotation[frame], node.rotation[next_frame], t);
 if (node.scale.count)
  s = lerp(node.scale[frame], node.scale[next_frame], t);
 return translate(position) * quat_to_mat(rotation) * scale(s);
}

void get_animated_node_transform(Animation &anim, NodeAnimation &node, float anim_time,
  v3 &position, v3 &s, quat &rotation)
{
 int frame = anim_time / anim.frametime;
 if (frame >= anim.frame_count)
  frame = anim.frame_count - 1;

 int next_frame = frame + 1 == anim.frame_count ? frame : frame + 1;
 float t = (anim_time - frame * anim.frametime) / anim.frametime;

 position = node.const_position;
 rotation = node.const_rotation;
 s = node.const_scale;

 if (node.position.count)
  position = lerp(node.position[frame], node.position[next_frame], t);
 if (node.rotation.count)
  rotation = quat_lerp(node.rotation[frame], node.rotation[next_frame], t);
 if (node.scale.count)
  s = lerp(node.scale[frame], node.scale[next_frame], t);
}

Animation load_animation(Arena *arena, ufbx_scene *uscene, ufbx_anim_stack *stack)
{
 Arena *temp = begin_temp_memory();

 Animation anim = {};

 anim.timebegin = stack->time_begin;
 anim.duration = stack->time_end - stack->time_begin;
 int target_framerate = 60;

 anim.frame_count = (u32)(anim.duration * target_framerate);
 if (anim.frame_count < 2)
  anim.frame_count = 2;
 anim.frametime = anim.duration / (anim.frame_count - 1);

 Array<v3> position = make_array<v3>(temp, anim.frame_count);
 Array<quat> rotation = make_array<quat>(temp, anim.frame_count);
 Array<v3> scale = make_array<v3>(temp, anim.frame_count);

 anim.nodes = make_array_max<NodeAnimation>(arena, uscene->nodes.count);
 for (usize j = 0; j < uscene->nodes.count; j++) {
  ufbx_node *node = uscene->nodes.data[j];
  if (node->is_root)
   continue ;
  NodeAnimation node_anim = {};
  node_anim.name = make_string(arena, (usize)node->name.length, (char *)node->name.data);

  b32 const_position = true;
  b32 const_scale = true;
  b32 const_rotation = true;

  for (usize frame = 0; frame < anim.frame_count; frame++) {
   float t = anim.timebegin + frame * anim.frametime;
   ufbx_transform transform = ufbx_evaluate_transform(stack->anim, node, t);

   position[frame] = ufbx_to_v3(transform.translation);
   rotation[frame] = ufbx_to_quat(transform.rotation);
   scale[frame] = ufbx_to_v3(transform.scale);

   if (frame) {



    if (dot(rotation[frame], rotation[frame - 1]) < 0.0f)
     rotation[frame] = -rotation[frame];

    if (!v3_equal(position[frame - 1], position[frame]))
     const_position = false;
    if (!quat_equal(rotation[frame - 1], rotation[frame]))
     const_rotation = false;
    if (!v3_equal(scale[frame - 1], scale[frame]))
     const_scale = false;
   }

  }



  if (const_position)
   node_anim.const_position = position[0];
  else
   node_anim.position = clone_array(arena, position);
  if (const_rotation)
   node_anim.const_rotation = rotation[0];
  else
   node_anim.rotation = clone_array(arena, rotation);
  if (const_scale)
   node_anim.const_scale = scale[0];
  else
   node_anim.scale = clone_array(arena, scale);

  anim.nodes.push(node_anim);
 }

 end_temp_memory();

 return anim;
}

Animation load_animation(Arena *arena, Game &game, const char *filename)
{
 Scene scene = {};

 Arena *temp = begin_temp_memory();

 ufbx_load_opts opts = {};

 opts.target_axes.right = UFBX_COORDINATE_AXIS_POSITIVE_X;
 opts.target_axes.up = UFBX_COORDINATE_AXIS_POSITIVE_Z;
 opts.target_axes.front = UFBX_COORDINATE_AXIS_POSITIVE_Y;
 opts.target_unit_meters = 1;
 opts.temp_allocator.allocator.realloc_fn = ufbx_arena_realloc;
 opts.temp_allocator.allocator.user = temp;
 opts.result_allocator.allocator.realloc_fn = ufbx_arena_realloc;
 opts.result_allocator.allocator.user = temp;

 ufbx_error error;
 ufbx_scene *uscene = ufbx_load_file(filename, &opts, &error);
 if (!uscene) {
  LOG_ERROR("failed to load animation %s: %s", filename, error.description.data);
  assert(0);
 }
 assert(uscene->anim_stacks.count);
 return load_animation(arena, uscene, uscene->anim_stacks.data[0]);
}

Scene *load_scene(Arena *arena, Game &game, const char *filename)
{
 Scene scene = {};

 Arena *temp = begin_temp_memory();
 {
  int slash = -1;

  for (int i = 0; filename[i]; i++) {
   if (filename[i] == '/' || filename[i] == '\\')
    slash = i;
  }

  int len = (int)strlen(filename);
  scene.path = make_string(arena, slash + 1, filename);
  scene.name = make_string(arena, len - (slash + 1), filename + slash + 1);
 }

 ufbx_load_opts opts = {};

 opts.target_axes.right = UFBX_COORDINATE_AXIS_POSITIVE_X;
 opts.target_axes.up = UFBX_COORDINATE_AXIS_POSITIVE_Z;
 opts.target_axes.front = UFBX_COORDINATE_AXIS_POSITIVE_Y;
 opts.target_unit_meters = 1;





 opts.generate_missing_normals = true;
 opts.load_external_files = true;




 ufbx_error error;
 ufbx_scene *uscene = ufbx_load_file(filename, &opts, &error);
 if (!uscene) {
  LOG_ERROR("failed to load %s: %s", filename, error.description.data);
  assert(0);
 }


 usize total_num_triangles = 0;
 for (size_t i = 0; i < uscene->nodes.count; i++)
  if (uscene->nodes.data[i]->mesh)
   total_num_triangles += uscene->nodes.data[i]->mesh->num_triangles;

 LOG_INFO("loading scene %s (%zd meshes, %zd triangles)", filename, uscene->meshes.count, total_num_triangles);

 mat4 root_transform = ufbx_to_mat4(uscene->root_node->node_to_parent);

 scene.meshes = make_array_max<Mesh>(arena, uscene->meshes.count);

 for (usize i = 0; i < uscene->nodes.count; i++) {
  if (uscene->nodes.data[i]->mesh) {
   Mesh mesh = load_mesh(arena, scene, uscene->nodes.data[i]);
   mesh.transform = ufbx_to_mat4(uscene->nodes.data[i]->geometry_to_world);

   double center[3] = {};
   for (usize j = 0; j < mesh.vertices.count; j++)
    for (int k = 0; k < 3; k++)
     center[k] += mesh.vertices[j].e[k];
   for (int k = 0; k < 3; k++)
    center[k] /= mesh.vertices.count;

   mesh.default_transform = root_transform * translate(-V3(center[0], center[1], center[2]));

   scene.meshes.push(mesh);
  }
 }

 end_temp_memory();
 scene.id = ++game.next_scene_id;
 game.scenes.push(scene);
 return &game.scenes[game.scenes.count - 1];
}
# 35 "code/game.cpp" 2

SceneID get_scene_id_by_name(Game &game, String name)
{
for (int i = 0; i < game.scenes.count; i++) {
  if (strings_equal(game.scenes[i].name, name))
   return game.scenes[i].id;
 }
 LOG_WARN("couldn't find scene %.*s", (int)name.count, name.data);
 return 0;
}

Scene &get_scene_by_id(Game &game, SceneID id)
{
 for (int i = 0; i < game.scenes.count; i++) {
  if (game.scenes[i].id == id)
   return game.scenes[i];
 }
 return game.default_scene;
}

Camera make_perspective_camera(mat4 view, float znear, float zfar, float width_fov_degree, float height_over_width)
{
 Camera camera = {};

 camera.type = CAMERA_TYPE_PERSPECTIVE;
 camera.znear = znear;
 camera.zfar = zfar;
 camera.width = 2 * znear * tanf((3.14159265359f / 180.f) * (width_fov_degree / 2));
 camera.height = camera.width * height_over_width;
 camera.view = view;
 camera.projection = perspective_projection(znear, zfar, camera.width, camera.height);

 mat4 inv_view = inverse(view);
 camera.position = V3(inv_view.e[0][3], inv_view.e[1][3], inv_view.e[2][3]);
 camera.right = V3(inv_view.e[0][0], inv_view.e[1][0], inv_view.e[2][0]);
 camera.up = V3(inv_view.e[0][1], inv_view.e[1][1], inv_view.e[2][1]);
 camera.forward = -V3(inv_view.e[0][2], inv_view.e[1][2], inv_view.e[2][2]);

 return camera;
}

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
 camera.right = V3(inv_view.e[0][0], inv_view.e[0][1], inv_view.e[0][2]);
 camera.up = V3(inv_view.e[1][0], inv_view.e[1][1], inv_view.e[1][2]);
 camera.forward = V3(inv_view.e[2][0], inv_view.e[2][1], inv_view.e[2][2]);

 return camera;
}

Entity *get_entity(World &world, entity_id id);
mat4 get_entity_transform(World &world, Entity &e);
v3 get_world_p(World &world, entity_id id);





# 1 "code/sound.cpp" 1



LoadedSound load_wav_file(Arena *arena, const char *filename)
{
    ma_decoder_config config = ma_decoder_config_init(ma_format_f32, 2, 48000);

    ma_decoder decoder;
    ma_result result = ma_decoder_init_file(filename, &config, &decoder);
    if (result != MA_SUCCESS)
        assert(0);
    LoadedSound sound = {};

    ma_uint64 samplesToRead = 1024;
 sound.samples = (float *)_arena_alloc("code/sound.cpp", __func__, 15, arena, samplesToRead * sizeof(float) * 2);

    while (1) {
        ma_uint64 samplesRead = 0;
        result = ma_decoder_read_pcm_frames(&decoder, sound.samples + sound.sample_count*2,
   samplesToRead, &samplesRead);
        sound.sample_count += samplesRead;
        if (samplesRead < samplesToRead)
            break ;

  _arena_alloc("code/sound.cpp", __func__, 25, arena, samplesToRead * sizeof(float) * 2);
    }
 LOG_INFO("loaded sound %s, %d samples", filename, sound.sample_count);
    return sound;
}

void audio_write_callback(ma_device* device, void* output, const void* input, ma_uint32 frame_count)
{
 Game &game = *((Game *)device->pUserData);
 if (!game.is_initialized)
  return ;

 int read_index = game.sound_state.read_index;
 int write_index = game.sound_state.write_index;


 if (read_index == write_index)
  return ;

 if (read_index < write_index) {
  int samples_to_read = min(write_index - read_index, (int)frame_count);
  memcpy(output, game.sound_state.buffer + read_index * 2,
   samples_to_read * 2 * sizeof(float));
  game.sound_state.read_index = read_index + samples_to_read;
 }
 else {
  int samples_to_copy = min((int)frame_count, game.sound_state.sample_count - read_index);
  memcpy(output, game.sound_state.buffer + read_index * 2,
   samples_to_copy * 2 * sizeof(float));
  frame_count -= samples_to_copy;
  memcpy(output, game.sound_state.buffer,
   min(write_index, (int)frame_count) * 2 * sizeof(float));

  game.sound_state.read_index = (read_index + samples_to_copy
   + min(write_index, (int)frame_count)) % game.sound_state.sample_count;
 }
# 82 "code/sound.cpp"
}

void play_sound(Game &game, LoadedSound &loaded_sound, entity_id entity = 0)
{
 SoundPlaying *sound = (SoundPlaying *)arena_alloc_zero(game.memory, sizeof(SoundPlaying));
 sound->sound = &loaded_sound;
 sound->entity = entity;

 sound->next = game.first_playing_sound;
 if (game.first_playing_sound)
  game.first_playing_sound->prev = sound;
 game.first_playing_sound = sound;
}

void update_sound(Game &game, World &world)
{
 if (!game.first_playing_sound)
  return ;

 SoundState &state = game.sound_state;



 int frames_to_write = 1;
 int fps = 60;
 int max_samples_to_write = ((48000 * frames_to_write) / fps);


 int write_index = state.write_index;
 int read_index = state.read_index;


 int can_write = 0;
 if (read_index <= write_index)
  can_write = read_index + state.sample_count - write_index;
 else
  can_write = read_index - write_index;

 can_write -= 1;

 max_samples_to_write = min(max_samples_to_write, can_write);

 if (max_samples_to_write <= 0)
  return ;

 Entity *player = get_entity(world, world.player_id);
 v3 player_forward = normalize(V3(cosf(world.player_camera_rotation.z), sinf(world.player_camera_rotation.z), 0));
 v3 player_up = V3(0, 0, 1);
 v3 player_right = normalize(cross(player_forward, player_up));

 int index = write_index;
 for (int sample = 0; sample < max_samples_to_write; sample++) {
  for (int i = 0; i < 2; i++)
   state.buffer[index * 2 + i] = 0;
  index++;
  if (index == state.sample_count)
   index = 0;
 }


 for (SoundPlaying *playing_sound = game.first_playing_sound;
  playing_sound;) {

  Entity *e = get_entity(world, playing_sound->entity);
  float volume[2] = {1, 1};
  if (e) {
   v3 to_e = normalize(e->position - world.player_camera_p);
   float x = dot(to_e, player_right);
   float y = dot(to_e, player_forward);
   float a = fabsf(atan2(y, x));
   a = a / 3.14159265359f;
   volume[0] = a;
   volume[1] = 1 - a;

   float dist = 1 - logf(length(e->position - world.player_camera_p)) / 5;
   if (dist < 0)
    dist = 0;
   volume[0] *= dist;
   volume[1] *= dist;
  }

  int samples_to_write = min(max_samples_to_write,
  playing_sound->sound->sample_count - playing_sound->samples_played);

  index = write_index;
  for (int sample = 0; sample < samples_to_write; sample++) {
   for (int i = 0; i < 2; i++) {
    state.buffer[index * 2 + i] +=
     playing_sound->sound->samples[playing_sound->samples_played
      * 2 + i] * volume[i] * game.master_volume;
   }
   playing_sound->samples_played++;
   index++;
   if (index == state.sample_count)
    index = 0;
  }
  SoundPlaying *next = playing_sound->next;
  if (playing_sound->samples_played == playing_sound->sound->sample_count) {
   if (next)
    next->prev = playing_sound->prev;
   if (playing_sound->prev)
    playing_sound->prev->next = next;
   if (game.first_playing_sound == playing_sound)
    game.first_playing_sound = next;
  }
  playing_sound = next;
 }


 int new_write_index = (write_index + max_samples_to_write) % state.sample_count;
 state.write_index = new_write_index;
}
# 105 "code/game.cpp" 2
# 1 "code/renderer.cpp" 1
void init_render_context(Arena *arena, RenderContext &rc, Platform &platform)
{

 init_render_context_opengl(rc, platform);



 rc.loaded_textures = make_array_max<Texture>(arena, 256);
 rc.debug_lines = make_array_max<v3>(arena, 4 * 5000000);

 uint32_t white_color = 0xffffffff;
 rc.white_texture = create_texture(make_cstring("__white_texture"), &white_color, 1, 1, true);
}

void push_line(v3 a, v3 b, v3 color = V3(1))
{
 g_rc->debug_lines.push(a);
 g_rc->debug_lines.push(color);
 g_rc->debug_lines.push(b);
 g_rc->debug_lines.push(color);
}

void push_cube_outline(v3 center, v3 radius, v3 color = V3(1))
{
 v3 p00 = center + V3(-radius.x, -radius.y, -radius.z);
 v3 p01 = center + V3(+radius.x, -radius.y, -radius.z);
 v3 p02 = center + V3(+radius.x, +radius.y, -radius.z);
 v3 p03 = center + V3(-radius.x, +radius.y, -radius.z);
 v3 p10 = center + V3(-radius.x, -radius.y, +radius.z);
 v3 p11 = center + V3(+radius.x, -radius.y, +radius.z);
 v3 p12 = center + V3(+radius.x, +radius.y, +radius.z);
 v3 p13 = center + V3(-radius.x, +radius.y, +radius.z);

 push_line(p00, p01, color);
 push_line(p01, p02, color);
 push_line(p02, p03, color);
 push_line(p03, p00, color);

 push_line(p10, p11, color);
 push_line(p11, p12, color);
 push_line(p12, p13, color);
 push_line(p13, p10, color);

 push_line(p00, p10, color);
 push_line(p01, p11, color);
 push_line(p02, p12, color);
 push_line(p03, p13, color);
}

void push_box_outline(v3 center,
  v3 x_axis, v3 y_axis, v3 z_axis, v3 color = V3(1))
{
# 65 "code/renderer.cpp"
 v3 p[8];
 int i = 0;






 for (int dz = -1; dz <= 1; dz++) {
  for (int dy = -1; dy <= 1; dy++) {
   for (int dx = -1; dx <= 1; dx++) {
    if (dx && dy && dz) {
     p[i++] = center + x_axis * dx
      + y_axis * dy
      + z_axis * dz;
    }
   }
  }
 }

 v3 p00 = p[0];
 v3 p01 = p[1];
 v3 p02 = p[3];
 v3 p03 = p[2];

 v3 p10 = p[4];
 v3 p11 = p[5];
 v3 p12 = p[7];
 v3 p13 = p[6];


 push_line(p00, p01, color);
 push_line(p01, p02, color);
 push_line(p02, p03, color);
 push_line(p03, p00, color);

 push_line(p10, p11, color);
 push_line(p11, p12, color);
 push_line(p12, p13, color);
 push_line(p13, p10, color);

 push_line(p00, p10, color);
 push_line(p01, p11, color);
 push_line(p02, p12, color);
 push_line(p03, p13, color);
}

void push_triangle_outline(v3 p0, v3 p1, v3 p2, v3 color = V3(1))
{
 push_line(p0, p1, color);
 push_line(p1, p2, color);
 push_line(p2, p0, color);
}

void push_ellipsoid_outline(v3 p, v3 r, v3 color = V3(1))
{
 v3 x = V3(r.x, 0, 0);
 v3 y = V3(0, r.y, 0);
 v3 z = V3(0, 0, r.z);

 int itr_count = 60;
 float a = (2 * 3.14159265359f) / itr_count;

 for (int itr = 0; itr < itr_count; itr++)
 {
  float a0 = a * itr;
  float a1 = a * (itr + 1);

  v3 px0 = p + x * cosf(a0) + y * sinf(a0);
  v3 px1 = p + x * cosf(a1) + y * sinf(a1);

  v3 py0 = p + y * cosf(a0) + z * sinf(a0);
  v3 py1 = p + y * cosf(a1) + z * sinf(a1);

  v3 pz0 = p + x * cosf(a0) + z * sinf(a0);
  v3 pz1 = p + x * cosf(a1) + z * sinf(a1);

  push_line(px0, px1, color);
  push_line(py0, py1, color);
  push_line(pz0, pz1, color);
 }
}

void push_circle(v3 p, float r, v3 right_axis, v3 up_axis, v3 color = V3(1))
{
 right_axis = normalize(right_axis);
 up_axis = normalize(up_axis);

 int itr_count = 60;
 float a = (2 * 3.14159265359f) / itr_count;

 for (int itr = 0; itr < itr_count; itr++)
 {
  float a0 = a * itr;
  float a1 = a * (itr + 1);

  v3 p0 = p + r * (right_axis * cosf(a0) + up_axis * sinf(a0));
  v3 p1 = p + r * (right_axis * cosf(a1) + up_axis * sinf(a1));

  push_line(p0, p1, color);
 }
}

void compute_bone_transform(Array<Bone> bones, int i, Array<bool> computed)
{
 if (computed[i] || bones[i].parent == -1)
  return ;
 compute_bone_transform(bones, bones[i].parent, computed);
 bones[i].transform = bones[bones[i].parent].transform * bones[i].transform;
 computed[i] = true;
}

void render_bones(Array<Bone> bones, mat4 transform, Animation *anim, float anim_time)
{
 if (bones.count == 0)
  return ;

 Arena *temp = begin_temp_memory();

 Array<Bone> anim_bones = clone_array(temp, bones);

 if (anim) {
  for (usize i = 0; i < bones.count; i++) {
   int index = -1;
   for (usize j = 0; j < anim->nodes.count; j++) {
    if (strings_equal(anim->nodes[j].name, anim_bones[i].name)) {
     index = (int)j;
     break ;
    }
   }
   if (index != -1)
    anim_bones[i].transform = anim->nodes[index].transform;
  }
 }

 Array<bool> computed = make_array<bool>(temp, anim_bones.count);
 for (int i = 0; i < anim_bones.count; i++)
  computed[i] = false;
 for (int i = 0; i < anim_bones.count; i++)
  compute_bone_transform(anim_bones, i, computed);
 for (int i = 0; i < anim_bones.count; i++)
  anim_bones[i].transform = transform * anim_bones[i].transform;

 for (usize i = 0; i < anim_bones.count; i++) {
  v3 P = (anim_bones[i].transform * v4{0, 0, 0, 1}).xyz;

  if (anim_bones[i].parent != -1) {
   v3 parentP = (anim_bones[anim_bones[i].parent].transform * v4{0, 0, 0, 1}).xyz;
   push_line(P, parentP);
  }
 }

 end_temp_memory();
}

Array<Bone> get_animated_bones(Arena *arena, Array<Bone> bones, mat4 transform, Animation *anim, float anim_time)
{
 Array<Bone> anim_bones = clone_array(arena, bones);

 for (usize i = 0; i < bones.count; i++) {
  int index = -1;
  for (usize j = 0; j < anim->nodes.count; j++) {
   if (strings_equal(anim->nodes[j].name, anim_bones[i].name)) {
    index = (int)j;
    break ;
   }
  }
  if (index != -1) {
   anim_bones[i].transform = anim->nodes[index].transform;
  }
 }

 Array<bool> computed = make_zero_array<bool>(arena, anim_bones.count);
 for (int i = 0; i < anim_bones.count; i++)
  compute_bone_transform(anim_bones, i, computed);
 for (int i = 0; i < anim_bones.count; i++)
  anim_bones[i].transform = transform * anim_bones[i].transform;

 return anim_bones;
}

void render_scene(Game &game, World &world, SceneID scene_id, Camera camera, mat4 transform, Animation *anim = 0, float anim_time = 0, v3 color = V3(1))
{
 if (!scene_id)
  return ;
 if (anim)
  anim_time = fmod(anim_time, anim->duration);
 Scene &scene = get_scene_by_id(game, scene_id);

 for (usize mesh_idx = 0; mesh_idx < scene.meshes.count; mesh_idx++) {
  Mesh &mesh = scene.meshes[mesh_idx];

  Constants constants = {};
  constants.view = camera.view;
  constants.projection = camera.projection;
  constants.camera_p = camera.position;
  constants.light_transform = game.shadow_map.projection * game.shadow_map.view;
  constants.color = color;
  constants.player_p = get_entity(*game.world, game.world->player_id)->position;
  constants.show_normals = game.show_normals;


  {
   for (int i = 0; i < game.world->entities.count; i++) {
    Entity &e = game.world->entities[i];

    if (constants.point_light_count == (sizeof(constants.point_light_color) / sizeof(*constants.point_light_color)))
     break ;
    if (e.type == EntityType_PointLight) {
     int j = constants.point_light_count++;
     constants.point_light_position[j].xyz = e.position;
     constants.point_light_color[j].xyz = e.color * e.point_light_scale;
    }
   }
  }

  mat4 mesh_transform = transform * mesh.transform;

  constants.model = mesh_transform;

  if (mesh.bones.count && anim) {
   constants.skinned = 1;
   Arena *temp = begin_temp_memory();
   Array<Bone> bones = get_animated_bones(temp, mesh.bones, mesh_transform, anim, anim_time);

   for (usize j = 0; j < bones.count; j++)
    constants.bones[j] = bones[j].transform * bones[j].inv_bind;

   if (game.render_bones)
    render_bones(bones, mesh_transform, anim, anim_time);
   end_temp_memory();
  }




  bool cull_mesh = false;

  if (!constants.skinned && game.frustum_culling)
  {
   Camera cam = camera;


   v3 c[2] = {mesh.box_min, mesh.box_max};
   v3 p[8];
   mat4 to_camera = cam.view * mesh_transform;
   for (int x = 0; x < 2; x++)
   for (int y = 0; y < 2; y++)
   for (int z = 0; z < 2; z++)
    p[(x<<2)|(y<<1)|z] = (to_camera * V4(c[x].x, c[y].y, c[z].z, 1)).xyz;

   v3 frustum_points[8];

   float zfar_width, zfar_height;

   if (cam.type == CAMERA_TYPE_ORTHOGRAPHIC)
    zfar_width = cam.width, zfar_height = cam.height;
   else
    zfar_width = cam.width * (cam.zfar / cam.znear), zfar_height = cam.height * (cam.zfar /cam.znear);


   frustum_points[0] = V3(-0.5f * cam.width, -0.5f * cam.height, -cam.znear);
   frustum_points[1] = V3(+0.5f * cam.width, -0.5f * cam.height, -cam.znear);
   frustum_points[2] = V3(+0.5f * cam.width, +0.5f * cam.height, -cam.znear);
   frustum_points[3] = V3(-0.5f * cam.width, +0.5f * cam.height, -cam.znear);

   frustum_points[4] = V3(-0.5f * zfar_width, -0.5f * zfar_height, -cam.zfar);
   frustum_points[5] = V3(+0.5f * zfar_width, -0.5f * zfar_height, -cam.zfar);
   frustum_points[6] = V3(+0.5f * zfar_width, +0.5f * zfar_height, -cam.zfar);
   frustum_points[7] = V3(-0.5f * zfar_width, +0.5f * zfar_height, -cam.zfar);

   v3 frustum_planes[6][3] = {
    {frustum_points[0], frustum_points[3], frustum_points[4]},
    {frustum_points[2], frustum_points[1], frustum_points[5]},
    {frustum_points[3], frustum_points[2], frustum_points[6]},
    {frustum_points[1], frustum_points[0], frustum_points[4]},
    {frustum_points[0], frustum_points[1], frustum_points[2]},
    {frustum_points[5], frustum_points[4], frustum_points[6]},
   };

   for (int i = 0; i < (sizeof(frustum_planes) / sizeof(*frustum_planes)); i++) {
    v3 normal = cross(frustum_planes[i][1] - frustum_planes[i][0], frustum_planes[i][2] - frustum_planes[i][0]);
    bool cull = true;
    for (int j = 0; j < (sizeof(p) / sizeof(*p)); j++) {
     if (dot(p[j] - frustum_planes[i][0], normal) < 0) {
      cull = false;
      break ;
     }
    }
    if (cull) {
     cull_mesh = true;
     break ;
    }
   }
  }

  if (!cull_mesh) {
   for (usize j = 0; j < mesh.parts.count; j++) {
    MeshPart &part = mesh.parts[j];

    bind_texture(0, part.material.diffuse.valid ? part.material.diffuse : g_rc->white_texture);
    bind_texture(1, part.material.specular.valid ? part.material.specular : g_rc->white_texture);
    bind_texture(2, part.material.normal_map);
    bind_texture(3, part.material.specular_exponent.valid ? part.material.specular_exponent : g_rc->white_texture);

    constants.diffuse_factor = part.material.diffuse_factor;
    constants.specular_factor = part.material.specular_factor;
    constants.specular_exponent_factor = part.material.specular_exponent_factor;
    constants.has_normal_map = part.material.normal_map.valid;

    update_constant_buffer(game.constant_buffer, &constants);
    bind_vertex_buffer(mesh.vertex_buffer);
    bind_index_buffer(mesh.index_buffer);

    draw_indexed((int)part.offset, (int)part.indices_count);
   }
  }
 }
}
# 106 "code/game.cpp" 2
# 1 "code/collision.cpp" 1
# 20 "code/collision.cpp"
void intersect_line(v3 A, v3 B, v3 dir, CollisionInfo &info)
{
 v3 D = dir;
 v3 d = normalize(B - A);
 v3 V = (A - dot(A, d) * d);
 v3 X = D - d * dot(D, d);

 float a = dot(X, X);
 float b = -2*dot(X, V);
 float c = dot(V, V) - 1;
 float delta = b * b - 4 * a * c;
 if (delta < 0 || fabsf(a) < 1e-6)
  return ;
 delta = sqrtf(delta);

 auto check_t = [&](float t) {
  if (t < 0 || t >= info.t)
   return ;

  v3 h = t * dir;
  v3 p = h - ((h - A) - d * dot(h - A, d));
  if (dot(p - A, B - A) >= 0 && length_sq(p - A) <= length_sq(B - A)) {
   info.t = t;
   info.hit_p = p;
  }
 };
 check_t((-b + delta)/(2*a));
 check_t((-b - delta)/(2*a));
}

void intersect_vertex(v3 p, v3 dir, CollisionInfo &info)
{
 float a = dot(dir, dir);
 float b = -2*dot(dir, p);
 float c = dot(p, p) - 1;
 float delta = b * b - 4 * a * c;
 if (delta < 0)
  return ;
 delta = sqrtf(delta);
 float t0 = (-b - delta)/(2*a);
 float t1 = (-b + delta)/(2*a);

 if (t1 >= 0 && t1 < t0)
  t0 = t1;

 if (t0 >= 0 && t0 <= info.t) {
  info.t = t0;
  info.hit_p = p;
 }
}

void intersect_triangle_plane(v3 v0, v3 v1, v3 v2, v3 dir, CollisionInfo &info)
{
 v3 u = v1 - v0;
 v3 v = v2 - v0;
 v3 n = cross(u, v);

 v3 normal = normalize(n);

 if (fabsf(dot(normal, dir)) < 1e-6)
  return ;

 if (dot(normal, dir) > 0)
  normal *= -1;

 float t0 = (1 + dot(v0, normal)) / (dot(normal, dir));
 float t1 = (-1 + dot(v0, normal)) / (dot(normal, dir));

 float one_over_length_n_sq = 1.f / dot(n, n);

 auto check_t = [&](float t) {
  if (t >= info.t || t < 0)
   return ;

  v3 p = t * dir;
  p = v0 + (p-v0)-dot(p - v0, normal)*normal;
# 104 "code/collision.cpp"
  float A = dot(cross(p - v0, v), n) * one_over_length_n_sq;
  float B = -dot(cross(p - v0, u), n) * one_over_length_n_sq;

  if (A >= 0 && B >= 0 && A + B <= 1) {
   info.t = t;
   info.hit_p = p;
  }
 };
 check_t(t0);
 check_t(t1);
}

CollisionInfo ellipsoid_intersect_triangle(v3 targetP, v3 ep, v3 er, v3 v0, v3 v1, v3 v2)
{
 CollisionInfo info = {};

 info.t = FLT_MAX;

 v3 inv_r = V3(1/er.x, 1/er.y, 1/er.z);
# 133 "code/collision.cpp"
 v3 t0 = inv_r * (v0 - ep);
 v3 t1 = inv_r * (v1 - ep);
 v3 t2 = inv_r * (v2 - ep);
 v3 dir = inv_r * (targetP - ep);

 intersect_triangle_plane(t0, t1, t2, dir, info);

 intersect_vertex(t0, dir, info);
 intersect_vertex(t1, dir, info);
 intersect_vertex(t2, dir, info);

 intersect_line(t0, t1, dir, info);
 intersect_line(t1, t2, dir, info);
 intersect_line(t0, t2, dir, info);




 info.hit_normal = inv_r * (info.t * dir - info.hit_p);
 info.hit_p = ep + (info.hit_p * er);

 return info;
}

CollisionInfo ellipsoid_intersect_ellipsoid(v3 targetP, v3 ep, v3 er, v3 tp, v3 tr)
{
 CollisionInfo info = {};

 info.t = FLT_MAX;







 v3 radius = er + tr;

 mat4 m = translate(tp) * scale(radius);
 mat4 M = scale(1 / radius) * translate(-tp);

 v3 ray_origin = (M * V4(ep, 1)).xyz;
 v3 ray_dir = (M * V4(targetP, 1)).xyz - ray_origin;

 float a = dot(ray_dir, ray_dir);
 float b = 2 * dot(ray_origin, ray_dir);
 float c = dot(ray_origin, ray_origin) - 1;

 float delta = b * b - 4 * a * c;

 if (delta < 0)
  return info;

 delta = sqrtf(delta);
 float t0 = (-b - delta) / (2 * a);
 float t1 = (-b + delta) / (2 * a);

 if (t1 >= 0 && t1 < t0)
  t0 = t1;

 if (t0 >= 0) {
  info.t = t0;
  v3 e_p = (m * V4(ray_origin + info.t * ray_dir, 1)).xyz;

  v3 hit_p = ep + info.t * (targetP - ep);


  info.hit_normal = (transpose(M) * V4(ray_origin + t0*ray_dir, 0)).xyz;

  v3 N = info.hit_normal;
  mat4 to_ep = scale(1 / tr) * translate(-tp);
  mat4 from_ep = translate(tp) * scale(tr);

  N = (transpose(from_ep) * V4(N, 0)).xyz;
  N = normalize(N);
  N = (transpose(to_ep) * V4(N, 0)).xyz;
  info.hit_p = tp + N;



 }
 return info;
}

CollisionInfo move_entity(World &world, Entity &e, v3 delta_p, Array<CollisionShape> shapes)
{

 CollisionInfo first_hit = {};

 v3 e_radius = e.scale * e.ellipsoid_radius;

 for (int itr = 0; itr < 4; itr++) {
  CollisionInfo hit_info = {};
  hit_info.t = FLT_MAX;
  if (length(delta_p) < 1e-7) {

   break ;
  }

  for (int i = 0; i < shapes.count; i++) {
   CollisionInfo info;

   if (!shapes[i].ellipsoid) {
    for (int j = 0; j + 2 < shapes[i].collision_mesh.vertices.count; j += 3) {
     v3 p0 = (shapes[i].transform * V4(shapes[i].collision_mesh.vertices[j + 0], 1)).xyz;
     v3 p1 = (shapes[i].transform * V4(shapes[i].collision_mesh.vertices[j + 1], 1)).xyz;
     v3 p2 = (shapes[i].transform * V4(shapes[i].collision_mesh.vertices[j + 2], 1)).xyz;

     v3 n = cross(p1 - p0, p2 - p0);
     if (fabsf(dot(normalize(n), e.position - p0)) > 2 * max(e_radius.x, max(e_radius.y, e_radius.z)))
      continue ;

     info = ellipsoid_intersect_triangle(e.position + delta_p, e.position, e_radius,
       p0, p1, p2);
     if (info.t < hit_info.t)
      hit_info = info;
    }
   }
   else {
    info = ellipsoid_intersect_ellipsoid(e.position + delta_p, e.position, e_radius,
      V3(shapes[i].transform.e[0][3],
       shapes[i].transform.e[1][3],
       shapes[i].transform.e[2][3]),
      shapes[i].ellipsoid_radius
      * shapes[i].scale);
    if (info.t < hit_info.t)
     hit_info = info;
   }
  }

  if (!itr)
   first_hit = hit_info;

  if (hit_info.t == FLT_MAX) {
   e.position += delta_p;
   break ;
  }

  v3 far = e.position + delta_p * hit_info.t;
  v3 fix = far - normalize(delta_p) * (2*0.0125f);


  float t = length(fix - e.position) / length(delta_p);

  if (dot(fix - e.position, delta_p) < 0) {


   t = 0;
  }



  if (t < 0)
   t = 0;
  if (t > 1)
   t = 1;
  if (t > hit_info.t)
   t = hit_info.t;

  if (t >= 1) {
   e.position += delta_p * t;
   break ;
  }




  e.position += t * delta_p;


  v3 normal = normalize(hit_info.hit_normal);

  delta_p *= (1 - t);
  delta_p = delta_p - normal*dot(normal, delta_p) * 1.2f;
 }

 return first_hit;
}

void move_entity(World &world, Entity &e, v3 delta_p)
{
 ProfilerBlock _block___LINE__(__FUNCTION__, 0);;
 assert(e.ellipsoid_collision_shape);

 Arena *temp = begin_temp_memory();

 Array<CollisionShape> shapes = make_array_max<CollisionShape>(temp, world.entities.count + 64);

 for (int i = 0; i < world.entities.count; i++) {
  Entity &test = world.entities[i];

  if (test.id == e.id || test.disable_collision)
   continue ;

  CollisionShape shape = {};
  if (test.ellipsoid_collision_shape) {
   shape.ellipsoid = true;
   shape.ellipsoid_radius = test.ellipsoid_radius;
   shape.transform = get_entity_transform(world, test);
   shape.scale = test.scale;
  }
  else {
   if (!world.scene_collision_mesh.count(test.scene_id))
    continue ;
   shape.collision_mesh = world.collision_meshes[world.scene_collision_mesh[test.scene_id]];
   shape.transform = get_entity_transform(world, test) * test.scene_transform;
  }
  shapes.push(shape);
 }
# 350 "code/collision.cpp"
 v3 old_p = e.position;


 {
  int itr = floorf(length(delta_p.xy) / ((2*0.0125f)));
  v3 delta = normalize(V3(delta_p.x, delta_p.y, 0)) * (2*0.0125f);
  for (int i = 0; i < itr; i++) {
   v3 p = e.position;
   move_entity(world, e, delta, shapes);
   if (length(e.position - p) < 1e-6)
    break ;
  }
 }






 {
  int itr = floorf(fabsf(delta_p.z) / ((2*0.0125f)));
  v3 delta = normalize(V3(0, 0, delta_p.z)) * (2*0.0125f);
  for (int i = 0; i < itr; i++) {
   v3 p = e.position;
   move_entity(world, e, delta, shapes);
   if (length(e.position - p) < 1e-6)
    break ;
  }
 }
# 398 "code/collision.cpp"
 v3 save_p = e.position;


 CollisionInfo collision = move_entity(world, e, V3(0, 0, -1), shapes);
 e.position = save_p;



 float height_above_ground = e.position.z - collision.hit_p.z - e.scale.z*e.ellipsoid_radius.z
  - (2*0.0125f);
 if (collision.t < 0 || collision.t == FLT_MAX)
  height_above_ground = 10000;
 if (height_above_ground < 0)
  height_above_ground = 0;
 e.height_above_ground = height_above_ground;

 if (e.height_above_ground > 0.6)
  e.can_jump = false;
 e.on_ground = e.height_above_ground < 0.01f;
 if (e.on_ground)
  e.can_jump = true;

 end_temp_memory();
}
# 107 "code/game.cpp" 2
# 1 "code/world.cpp" 1
Entity *make_entity(World &world)
{
 Entity e = {};

 e.id = ++world.next_entity_id;
 world.entities.push(e);
 world.entities_id_map[e.id] = world.entities.count - 1;
 return &world.entities[world.entities.count - 1];
}

Entity *make_entity(World &world, EntityType type, SceneID scene_id, v3 position, mat4 scene_transform = identity())
{
 Entity *e = make_entity(world);

 e->type = type;
 e->position = position;
 e->scene_id = scene_id;
 e->scene_transform = scene_transform;
 e->color = V3(1);
 e->scale = V3(1);
 e->rotation = identity_quat();
 return e;
}

Entity *get_entity(World &world, entity_id id)
{






 auto it = world.entities_id_map.find(id);
 if (it != world.entities_id_map.end())
  return &world.entities[it->second];

 return 0;
}

void remove_entity(World &world, entity_id id)
{
 assert(world.entities_id_map.count(id));

 usize index = world.entities_id_map[id];
 world.entities_id_map.erase(id);
 if (index != world.entities.count - 1) {
  world.entities[index] = world.entities[world.entities.count - 1];
  world.entities_id_map[world.entities[index].id] = index;
 }
 world.entities.count--;
}

v3 get_world_p(World &world, entity_id id)
{
 Entity *e = get_entity(world, id);
 assert(e);

 v3 position = e->position;
 while (e->parent)
 {
  Entity *p = get_entity(world, e->parent);
  assert(p);
  position += p->position;
  e = p;
 }
 return position;
}

mat4 get_entity_transform(World &world, Entity &e)
{




 return translate(get_world_p(world, e.id))
  * quat_to_mat(e.rotation) * scale(e.scale);

}

Animation *get_blended_animation(Arena *arena, Entity &e)
{
 if (!e.curr_anim)
  return 0;

 Animation *anim = (Animation *)_arena_alloc("code/world.cpp", __func__, 85, arena, sizeof(Animation));

 usize max_nodes_count = e.curr_anim->nodes.count;
 if (e.next_anim && e.next_anim->nodes.count > max_nodes_count)
  max_nodes_count = e.next_anim->nodes.count;

 anim->nodes = make_array<NodeAnimation>(arena, max_nodes_count);
 if (e.next_anim)
  assert(e.curr_anim->nodes.count == e.next_anim->nodes.count);

 float blend_duration = e.next_anim ? e.next_anim->duration*0.2f : 0;
 if (e.blend_time > blend_duration && e.next_anim) {
  e.curr_anim = e.next_anim;
  e.anim_time = e.blend_time;
  e.next_anim = 0;
 }

 if (!e.next_anim) {
  assert(e.curr_anim);
  for (int j = 0; j < e.curr_anim->nodes.count; j++) {
   anim->nodes[j].name = e.curr_anim->nodes[j].name;
   anim->nodes[j].transform = get_animated_node_transform(*e.curr_anim,
     e.curr_anim->nodes[j], fmod(e.anim_time, e.curr_anim->duration));
  }
 } else {
  float t1 = fmod(e.anim_time, e.curr_anim->duration);
  float t2 = fmod(e.blend_time, e.next_anim->duration);
  float t3 = (e.blend_time) / blend_duration;


  for (int j = 0; j < e.curr_anim->nodes.count; j++) {
   quat q1, q2;
   v3 p1, s1, p2, s2;
   assert(strings_equal(e.curr_anim->nodes[j].name, e.next_anim->nodes[j].name));
   get_animated_node_transform(*e.curr_anim, e.curr_anim->nodes[j], t1, p1, s1, q1);
   get_animated_node_transform(*e.next_anim, e.next_anim->nodes[j], t2, p2, s2, q2);

   anim->nodes[j].name = e.curr_anim->nodes[j].name;

   v3 p = lerp(p1, p2, t3);
   quat q = quat_lerp(q1, q2, t3);
   v3 s = lerp(s1, s2, t3);

   anim->nodes[j].transform = translate(p) * quat_to_mat(q) * scale(s);

  }
 }
 return anim;
}

void render_entities(Game &game, World &world, Camera camera, bool shadow_map_pass = false)
{

 bind_constant_buffer(game.constant_buffer, 0);

 for (usize i = 0; i < world.entities.count; i++) {
  Entity &e = world.entities[i];
  if (!e.scene_id)
   continue ;

  mat4 entity_transform = get_entity_transform(world, e);
  mat4 scene_transform = entity_transform * e.scene_transform;

  Arena *temp = begin_temp_memory();

  Animation *anim = get_blended_animation(temp, e);
  render_scene(game, world, e.scene_id, camera, scene_transform, anim, 0, e.color);

  end_temp_memory();
 }
}

void update_player(Game &game, World &world, GameInput &input, float dt)
{

 Entity *_player = get_entity(world, world.player_id);
 if (!_player)
  return ;
 Entity &player = *_player;

 b32 camera_shoot_mode = false;
 b32 walk_backward = false;
 {
  v3 player_forward = normalize(V3(cosf(world.player_camera_rotation.z), sinf(world.player_camera_rotation.z), 0));
  v3 player_up = V3(0, 0, 1);
  v3 player_right = normalize(cross(player_forward, player_up));
  v3 a = {};

  bool forward = false;

  if ((input.buttons[BUTTON_F].is_down))
  {
   a += player_forward;
   forward = true;
  }
  if ((input.buttons[BUTTON_G].is_down))
  {
   a -= player_forward;
   walk_backward = true;
  }
  a = normalize(a);

  player.moved = forward;
  player.run = input.buttons[BUTTON_LEFT_SHIFT].is_down && forward;
  if (!(input.buttons[BUTTON_MOUSE_LEFT].is_down) || player.moved)
   player.shooting = false;
  else
  {
   player.shooting = true;
   player.run = false;
   player.moved = false;
   a = {};
  }

  if (!(input.buttons[BUTTON_MOUSE_RIGHT].is_down) || player.moved) {
   if (player.aiming)
    world.aim_camera_transition_t = 0;
   player.aiming = 0;
  } else {
   if (!player.aiming)
    world.aim_camera_transition_t = 0;
   player.aiming = 1;
  }

  if (player.shooting) {

   if (game.time - player.last_gun_time > 0.3) {


    v3 dir = normalize(world.player_camera_p + 4 * player_forward
     - player.position);
    push_line(player.position, player.position + dir * 5, V3(0));





   }
  }



  a += -40 * player_up;
  a.xy = a.xy * (player.run ? 50 : 30);
  bool jumped = false;
  if ((input.buttons[BUTTON_SPACE].is_down) && player.can_jump)
  {
   if (!player.pressing_jump)
    play_sound(game, game.loaded_sounds[1]);

   a += 200 * player_up;
   a.xy = {};
   jumped = true;
   player.pressing_jump = true;
  }
  else
   player.pressing_jump = false;



  a -= player.dp * 3;

  {
   v3 delta_p = 0.5f * dt * dt * a + dt * player.dp;

   move_entity(world, player, delta_p);
   player.dp += a * dt;

  }
# 265 "code/world.cpp"
  {


   Animation *next_anim = 0;
   player.anim_time += dt;
   player.blend_time += dt;

   if (!player.curr_anim)
    player.curr_anim = &game.animations[ANIMATION_GUN_IDLE];


   if (!player.on_ground) {
    if (player.curr_anim != &game.animations[ANIMATION_JUMP]
      && player.next_anim != &game.animations[ANIMATION_JUMP]
      && !jumped)
     next_anim = player.next_anim;
    else
     next_anim = &game.animations[ANIMATION_JUMP];
   }
   else if (player.shooting)
    next_anim = &game.animations[ANIMATION_SHOOT];
   else if (player.run)
    next_anim = &game.animations[ANIMATION_RUN];
   else if (walk_backward)
    next_anim = &game.animations[ANIMATION_BACKWARD_GUN_WALK];
   else if (player.moved)
    next_anim = &game.animations[ANIMATION_FORWARD_GUN_WALK];
   else
    next_anim = &game.animations[ANIMATION_GUN_IDLE];

   if (!player.curr_anim)
    player.curr_anim = next_anim;
   else if (!next_anim)
    ;
   else if (!player.next_anim) {
    if (player.curr_anim != next_anim) {
     player.next_anim = next_anim;
     player.blend_time = 0;
    }
   }
   else if (next_anim == player.next_anim)
    ;
   else if (next_anim != player.curr_anim) {






   }
   else {
    player.next_anim = 0;
    player.blend_time = 0;
   }







  }
 }
}



void update_enemies(Game &game, World &world, GameInput &input, float dt)
{
 Entity *player = get_entity(world, world.player_id);
 if (!player)
  return ;

 for (int i = 0; i < world.entities.count; i++) {
  Entity &e = world.entities[i];
  if (e.type != EntityType_Enemy)
   continue ;
  assert(e.ellipsoid_collision_shape);

  v3 targetP = player->position;
  v3 dP = {};
  v3 s = e.scale * e.ellipsoid_radius;

  bool jump = false;
  {
   auto target_cell = get_cell(targetP);
   auto start_cell = get_cell(e.position);
# 465 "code/world.cpp"
   auto cell_dist = [&](v3i a, v3i b) -> int {
    return roundf(10.f * sqrtf((b.x-a.x)*(b.x-a.x) + (b.y-a.y)*(b.y-a.y) + (b.z-a.z)*(b.z-a.z)));
   };

   int MAX_JUMP_CELL_COUNT = 5;

   State istate;
   istate.p = start_cell;
   if (!e.can_jump && e.dp.z < 0)
    istate.jump = -1;
   else {
    if (e.can_jump)
     istate.jump = 0;
    else {
     istate.jump = roundf((e.position.z - e.last_jump_z) / (0.8f));
    }
    if (istate.jump < 0)
     istate.jump = 0;
   }
   LOG_DEBUG("istate jump: %d", istate.jump);

   auto get_state_childs = [&](State state, State *childs, int &count)
   {

    bool on_ground = world.occupied.count(pack_cell(state.p + V3i(0, 0, -1))) != 0;

    if (on_ground)
     state.jump = 0;
    count = 0;

    if (state.jump == -1) {
     for (int dx = -1; dx <= 1; dx++)
     for (int dy = -1; dy <= 1; dy++) {
      State s = state;

      s.p = s.p + V3i(dx, dy, -1);
      childs[count++] = s;
     }
     return ;
    }



    if (!on_ground) {
    for (int dx = -1; dx <= 1; dx++)
    for (int dy = -1; dy <= 1; dy++) {
     State s = state;
     s.p = s.p + V3i(dx, dy, -1);
     s.jump = -1;
     childs[count++] = s;
    }
    }
    if (state.jump < MAX_JUMP_CELL_COUNT) {
     for (int dx = -1; dx <= 1; dx++)
     for (int dy = -1; dy <= 1; dy++) {
      State s = state;
      s.p = s.p + V3i(dx, dy, +1);
      s.jump++;
      childs[count++] = s;
     }
    }
    for (int dx = -1; dx <= 1; dx++)
    for (int dy = -1; dy <= 1; dy++) {
     State s = state;
     s.p = s.p + V3i(dx, dy, 0);
     if (!on_ground)
      s.jump = -1;
     else
      s.jump = 0;
     childs[count++] = s;
    }
# 546 "code/world.cpp"
   };

   std::unordered_map<State, int, StateHasher> visited;

   Arena *temp = begin_temp_memory();


   int itr = 0;
   float best_length_sq = length_sq(e.position - targetP);


   std::unordered_map<State, int, StateHasher> gScore, fScore;

   std::unordered_map<State, State, StateHasher> parent;
   State best_cell = istate;

   gScore[istate] = 0;
   fScore[istate] = cell_dist(istate.p, target_cell);
   istate.fscore = fScore[istate];
   best_length_sq = istate.fscore;

   std::set<State> set;
   set.insert(istate);
   visited[istate] = true;


   bool on_ground = world.occupied.count(pack_cell(istate.p + V3i(0, 0, -1))) != 0;
   LOG_DEBUG("PLAYER %d %d %d", target_cell.x, target_cell.y, target_cell.z);

   while (!set.empty() && itr < 2*8192)
   {
    auto state = *set.begin();

    set.erase(set.begin());







    render_cell(state.p, 0.2);


    State childs[36];
    int child_count;

    get_state_childs(state, childs, child_count);
    for (int i = 0; i < child_count; i++) {
     if (!world.occupied.count(pack_cell(childs[i].p))) {
      if (childs[i].p.x == target_cell.x && childs[i].p.y == target_cell.y
        && childs[i].p.z == target_cell.z)
       LOG_DEBUG("FOUND BEST 2 !!!");

      int tentative_gScore = gScore[state] + cell_dist(childs[i].p, state.p);
      if (!gScore.count(childs[i]) || tentative_gScore < gScore[childs[i]]) {
       parent[childs[i]] = state;
       gScore[childs[i]] = tentative_gScore;
       childs[i].fscore = fScore[childs[i]];

       fScore[childs[i]] = tentative_gScore + cell_dist(childs[i].p, target_cell);
       childs[i].fscore = fScore[childs[i]];
       if (!set.count(childs[i]))
        set.insert(childs[i]);

      }
      if (cell_dist(childs[i].p, target_cell) < best_length_sq) {
       best_length_sq = cell_dist(childs[i].p, target_cell);
       best_cell = childs[i];
      }
     }
    }

    itr++;
   }
   LOG_DEBUG("ITR = %d", itr);

   if (parent.count(best_cell)) {

    Array<v3i> path = make_array_max<v3i>(temp, 1024);

    State s = best_cell;

    while (1) {
     path.push(s.p);
     if (!parent.count(s))
      break ;
     s = parent[s];
    }
    for (int i = 0; i < path.count; i++) {
     v3 color = V3(0, 1, 0);
     if (world.occupied.count(pack_cell(path[i])))
      color = V3(1, 0, 0);
     render_cell(path[i], 0.9f, V3(0, 1, 0));
    }




    LOG_DEBUG("PATH LENGTH: %zu", path.count);
    if (path[path.count - 2].z > start_cell.z)
     jump = true;
    dP = V3(path[path.count - 2]) * (0.8f) - V3(path[path.count - 1]) * (0.8f);
   }
   else {
    dP = get_closest_point_in_cell(start_cell, targetP) - e.position;
    LOG_DEBUG("best cell is current!");
   }
# 683 "code/world.cpp"
   end_temp_memory();
  }


  dP = normalize(dP);

  e.run = true;

  v2 D = normalize(dP.xy);
  quat target_rot = rotate_around_axis_quat(V3(0, 0, 1), atan2(D.y, D.x));
  e.rotation = quat_lerp(e.rotation, target_rot, dt * 10);







   e.moved = true;

  v3 a = dP;

  a += -(30) * V3(0, 0, 1);
  a.xy = a.xy * 45;


  bool jumped = false;
  if (jump && e.can_jump)
  {
   a += 200 * V3(0, 0, 1);
   a.xy = {};
   jumped = true;
   LOG_DEBUG("JUMPED!!");
   if (!e.pressing_jump)
    e.last_jump_z = e.position.z;

   e.pressing_jump = true;
   e.should_jump = false;
  }
  else
   e.pressing_jump = false;



  a -= e.dp * 3;
  {
   v3 delta_p = 0.5f * dt * dt * a + dt * e.dp;
   v3 old_p = e.position;
   move_entity(world, e, delta_p);
   if (length(e.position - old_p) < 0.1f * length(delta_p))
    e.should_jump = true;
   e.dp += a * dt;

  }
  {


   Animation *next_anim = 0;
   e.anim_time += dt;
   e.blend_time += dt;

   if (!e.curr_anim)
    e.curr_anim = &game.animations[ANIMATION_GUN_IDLE];


   if (!e.on_ground) {
    if (e.curr_anim != &game.animations[ANIMATION_JUMP]
      && e.next_anim != &game.animations[ANIMATION_JUMP]
      && !jumped)
     next_anim = e.next_anim;
    else
     next_anim = &game.animations[ANIMATION_JUMP];
   }
   else if (e.shooting)
    next_anim = &game.animations[ANIMATION_SHOOT];
   else if (e.run)
    next_anim = &game.animations[ANIMATION_RUN];
   else if (e.moved)
    next_anim = &game.animations[ANIMATION_FORWARD_GUN_WALK];
   else
    next_anim = &game.animations[ANIMATION_GUN_IDLE];

   if (!e.curr_anim)
    e.curr_anim = next_anim;
   else if (!next_anim)
    ;
   else if (!e.next_anim) {
    if (e.curr_anim != next_anim) {
     e.next_anim = next_anim;
     e.blend_time = 0;
    }
   }
   else if (next_anim == e.next_anim)
    ;
   else if (next_anim != e.curr_anim) {




   }
   else {
    e.next_anim = 0;
    e.blend_time = 0;
   }







  }
 }
}

Camera update_camera(Game &game, World &world, GameInput &input, float dt)
{
 Camera camera = {};

 Entity *player = get_entity(world, world.player_id);
 if (!player)
  assert(0);





 if (game.in_editor && (ImGui::GetIO().WantCaptureMouse || !(input.buttons[BUTTON_MOUSE_LEFT].is_down)))
  input.mouse_dp = {};


 v3 camera_rot;
 if (game.in_editor) {
  world.editor_camera_rotation.x += -input.mouse_dp.y * dt * 0.2f;
  world.editor_camera_rotation.z += -input.mouse_dp.x * dt * 0.2f;

  if (world.editor_camera_rotation.x > 3.14159265359f / 2 - 0.001f)
   world.editor_camera_rotation.x = 3.14159265359f / 2 - 0.001f;
  if (world.editor_camera_rotation.x < -3.14159265359f / 2 + 0.001f)
   world.editor_camera_rotation.x = -3.14159265359f / 2 + 0.001f;
  camera_rot = world.editor_camera_rotation;
 }
 else {
  v2 mouse_dp = (input.mouse_dp) * 0.1f;
  v3 a = {-24*mouse_dp.y, 0, -20*mouse_dp.x};


  a -= world.player_camera_drotation * 10;
  world.player_camera_rotation += 0.5 * a * dt * dt + world.player_camera_drotation * dt;
  world.player_camera_drotation += a * dt;
  {
   if (world.player_camera_rotation.x >= 3.14159265359f / 2)
    world.player_camera_rotation.x = 3.14159265359f / 2;
   if (world.player_camera_rotation.x <= -3.14159265359f / 2)
    world.player_camera_rotation.x = -3.14159265359f / 2;
  }
  world.player_camera_rotation.z = fmod(world.player_camera_rotation.z, 2*3.14159265359f);

  camera_rot = world.player_camera_rotation;
  camera_rot.z -= 3.14159265359f / 2;
  player->rotation = zrotation_quat(world.player_camera_rotation.z);


  {
   v3 player_forward = normalize(V3(cosf(world.player_camera_rotation.z),
      sinf(world.player_camera_rotation.z), 0));
   v3 player_right = normalize(cross(player_forward, V3(0, 0, 1)));
   float o = camera_rot.x;

   float t[4] = {-3.14159265359f/2, 0, 3.14159265359f/2.5, 3.14159265359f/2};
   assert(player->ellipsoid_collision_shape);

   v3 player_p = get_world_p(world, player->id);

   v3 v[4] = {
    player_p + V3(0, 0, player->ellipsoid_radius.z*3),
    player_p - player_forward * 3 + V3(0, 0, player->ellipsoid_radius.z * 0.5),
    player_p - player_forward * 1.5
     + V3(0, 0, -player->ellipsoid_radius.z +0.2),
    player_p - V3(0, 0, player->ellipsoid_radius.z-0.1),

   };




   mat4 M;
   mat4 V;

   for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
     M.e[i][j] = 1;
     for (int k = 0; k < 3 - i; k++)
      M.e[i][j] *= t[j];

     if (i == 3)
      V.e[i][j] = 0;
     else
      V.e[i][j] = v[j].e[i];
    }
   }

   M = inverse(M);

   mat4 A = V * M;

   v3 target_camera_p = (A * V4(o*o*o, o*o, o, 1)).xyz;

   if (player->aiming)
    target_camera_p += player_right * 1.f + player_forward * 0.9 + V3(0, 0, 0.7);
   else
    target_camera_p += player_right * 0.3f;
   float transition_time = 1.5f;
   if (world.aim_camera_transition_t < transition_time)
    world.player_camera_p += (15 + 40 * world.aim_camera_transition_t/transition_time) * dt * (target_camera_p - world.player_camera_p);
   else
    world.player_camera_p += (15 + 40) * dt * (target_camera_p - world.player_camera_p);

   world.aim_camera_transition_t = min(transition_time, dt + world.aim_camera_transition_t);
# 915 "code/world.cpp"
  }
 }



 mat4 camera_transform;
 if (game.in_editor)
  camera_transform = translate(world.editor_camera_p) * zrotation(camera_rot.z) * xrotation(camera_rot.x);
 else
  camera_transform = translate(world.player_camera_p) * zrotation(camera_rot.z) * xrotation(camera_rot.x) ;

 v3 camera_x = (camera_transform * v4{1, 0, 0, 0}).xyz;
 v3 camera_y = (camera_transform * v4{0, 0, 1, 0}).xyz;
 v3 camera_z = (camera_transform * v4{0, -1, 0, 0}).xyz;




  if (game.in_editor && !(input.buttons[BUTTON_LEFT_CONTROL].is_down)
    && !ImGui::GetIO().WantCaptureKeyboard)


  if (!(input.buttons[BUTTON_LEFT_CONTROL].is_down) && !(input.buttons[BUTTON_LEFT_CONTROL].was_down))
  {
   v3 camera_dp = {};
   if ((input.buttons[BUTTON_W].is_down))
    camera_dp -= camera_z;
   if ((input.buttons[BUTTON_S].is_down))
    camera_dp += camera_z;
   if ((input.buttons[BUTTON_A].is_down))
    camera_dp -= camera_x;
   if ((input.buttons[BUTTON_D].is_down))
    camera_dp += camera_x;
   if ((input.buttons[BUTTON_Q].is_down))
    camera_dp += camera_y;
   if ((input.buttons[BUTTON_E].is_down))
    camera_dp -= camera_y;
   world.editor_camera_p += normalize(camera_dp) * dt * 8;
  }

 mat4 rotation = {
  camera_x.x, camera_x.y, camera_x.z, 0,
  camera_y.x, camera_y.y, camera_y.z, 0,
  camera_z.x, camera_z.y, camera_z.z, 0,
  0, 0, 0, 1
 };

 v3 p = game.in_editor ? world.editor_camera_p : world.player_camera_p;
 mat4 view = rotation * translate(-p);


 camera = make_perspective_camera(view, 0.1f, 100, 100, (float)g_rc->window_height / g_rc->window_width);
# 982 "code/world.cpp"
 return camera;
}
# 108 "code/game.cpp" 2
# 1 "code/editor.cpp" 1
# 17 "code/editor.cpp"
entity_id raycast_to_entities(Game &game, World &world, v3 camera_ray_origin, v3 camera_ray_dir,
  float &hit_t, int &mesh_index)
{
 entity_id hit_id = 0;

 v3 hit_triangle[3];

 mesh_index = -1;
 float min_t = FLT_MAX;

 for (usize i = 0; i < world.entities.count; i++) {
  Entity &e = world.entities[i];
  if (!e.scene_id)
   continue ;

  Scene &scene = get_scene_by_id(game, e.scene_id);
  mat4 transform = get_entity_transform(world, e) * e.scene_transform;
  for (usize j = 0; j < scene.meshes.count; j++) {
   Mesh &mesh = scene.meshes[j];
   mat4 mesh_transform = transform * mesh.transform;

   {
    v3 x_axis = normalize((mesh_transform * V4(1, 0, 0, 0)).xyz);
    v3 y_axis = normalize((mesh_transform * V4(0, 1, 0, 0)).xyz);
    v3 z_axis = normalize((mesh_transform * V4(0, 0, 1, 0)).xyz);

    v3 a = (mesh_transform * V4(mesh.box_min, 1)).xyz;
    v3 b = (mesh_transform * V4(mesh.box_max, 1)).xyz;

    v3 c = (a + b) * 0.5f;

    x_axis *= fabsf(dot((b - a)*0.5f, x_axis));
    y_axis *= fabsf(dot((b - a)*0.5f, y_axis));
    z_axis *= fabsf(dot((b - a)*0.5f, z_axis));

    float t = ray_hit_box(camera_ray_origin, camera_ray_dir, c, x_axis, y_axis, z_axis);
    if (t < 0)
     continue ;
   }

   for (usize t = 0; t < mesh.indices.count; t += 3) {
    v3 v0 = (mesh_transform * V4(mesh.vertices[(int)mesh.indices[t+0]], 1)).xyz;
    v3 v1 = (mesh_transform * V4(mesh.vertices[(int)mesh.indices[t+1]], 1)).xyz;
    v3 v2 = (mesh_transform * V4(mesh.vertices[(int)mesh.indices[t+2]], 1)).xyz;

    float hit_t;
    if (ray_hit_triangle(camera_ray_origin, camera_ray_dir, v0, v1, v2, &hit_t) &&
      hit_t < min_t) {
     min_t = hit_t;
     hit_id = e.id;
     hit_triangle[0] = v0;
     hit_triangle[1] = v1;
     hit_triangle[2] = v2;
     mesh_index = (int)j;
    }
   }
  }
 }

 if (hit_id) {

 }

 hit_t = min_t;
 return hit_id;
}

void do_editor_op(Game &game, World &world, Editor &editor, EditorOp &op)
{
 Entity *e = get_entity(world, op.entity);
 if (e) {
  if (op.type == EDITOR_OP_TRANSLATE_ENTITY)
   e->position = op.translate.new_p;
  else if (op.type == EDITOR_OP_ROTATE_ENTITY)
   e->rotation = op.rotate.new_rot;
  else if (op.type == EDITOR_OP_SCALE_ENTITY)
   e->scale = op.scale.new_scale;
  else if (op.type == EDITOR_OP_CREATE_MESH_COLLISION_VERTEX) {
   CollisionMesh &cmesh = world.collision_meshes[world.scene_collision_mesh[e->scene_id]];
   cmesh.vertices.push(op.place_collision_vertex.pos);
  }
  else if (op.type == EDITOR_OP_DELETE_MESH_COLLISION_TRIANGLE) {
   CollisionMesh &cmesh = world.collision_meshes[world.scene_collision_mesh[e->scene_id]];
   for (int i = op.delete_collision_triangle.index + 3; i < cmesh.vertices.count; i++)
    cmesh.vertices[i - 3] = cmesh.vertices[i];
   cmesh.vertices.count -= 3;
  }
 }
 if (op.type == EDITOR_OP_PASTE_ENTITY) {
  Entity *copy_from = get_entity(world, op.paste.copy_from);
  if (copy_from) {



   Entity *copied;

   if (op.paste.id) {
    Entity p = *copy_from;
    p.id = op.paste.id;
    world.entities.push(p);
    copied = &world.entities[world.entities.count - 1];
    world.entities_id_map[p.id] = world.entities.count - 1;
   }
   else {
    Entity *p = make_entity(world);
    entity_id id = p->id;
    *p = *copy_from;
    p->id = id;
    op.paste.id = p->id;
    copied = p;
   }
   copied->position = op.paste.p;
   if (op.paste.mesh_index >= 0) {
    Scene scene = {};
    scene.id = ++game.next_scene_id;
    Scene &s = get_scene_by_id(game, copied->scene_id);
    assert(op.paste.mesh_index < s.meshes.count);

    scene.meshes = make_array<Mesh>(&world.arena, 1);
    scene.meshes[0] = s.meshes[op.paste.mesh_index];

    copied->scene_transform = identity();

    scene.meshes[0].transform = scene.meshes[0].default_transform;

    game.scenes.push(scene);
    copied->scene_id = scene.id;
   }
  }
 }
 else if (op.type == EDITOR_OP_DELETE_ENTITY)
  remove_entity(world, op.entity);

 editor.ops.push(op);
}

void redo_editor_op(Game &game, World &world, Editor &editor)
{
 if (!editor.undos.count)
  return ;
 EditorOp op = editor.undos[editor.undos.count - 1];
 editor.undos.count--;

 do_editor_op(game, world, editor, op);
}

void undo_editor_op(Game &game, World &world, Editor &editor)
{
 if (!editor.ops.count)
  return ;
 EditorOp op = editor.ops[editor.ops.count - 1];

 editor.undos.push(op);
 editor.ops.count--;
 Entity *e = get_entity(world, op.entity);
 if (e) {
  if (op.type == EDITOR_OP_TRANSLATE_ENTITY)
   e->position = op.translate.prev_p;
  else if (op.type == EDITOR_OP_ROTATE_ENTITY)
   e->rotation = op.rotate.prev_rot;
  else if (op.type == EDITOR_OP_SCALE_ENTITY)
   e->scale = op.scale.prev_scale;
  else if (op.type == EDITOR_OP_CREATE_MESH_COLLISION_VERTEX) {
   CollisionMesh &cmesh = world.collision_meshes[world.scene_collision_mesh[e->scene_id]];
   assert(cmesh.vertices.count > 0);
   cmesh.vertices.count--;
  }
  else if (op.type == EDITOR_OP_DELETE_MESH_COLLISION_TRIANGLE) {
   CollisionMesh &cmesh = world.collision_meshes[world.scene_collision_mesh[e->scene_id]];

   cmesh.vertices.count += 3;
   for (int i = cmesh.vertices.count - 1; i >= op.delete_collision_triangle.index + 3; i--)
    cmesh.vertices[i] = cmesh.vertices[i - 3];

   cmesh.vertices[op.delete_collision_triangle.index + 0] = op.delete_collision_triangle.v0;
   cmesh.vertices[op.delete_collision_triangle.index + 1] = op.delete_collision_triangle.v1;
   cmesh.vertices[op.delete_collision_triangle.index + 2] = op.delete_collision_triangle.v2;
  }
 }
 if (op.type == EDITOR_OP_PASTE_ENTITY)
  remove_entity(world, op.paste.id);
 else if (op.type == EDITOR_OP_DELETE_ENTITY) {
  world.entities.push(op.del.entity_data);
  world.entities_id_map[op.del.entity_data.id] = world.entities.count - 1;
 }
}

void editor_render_gizmo(World &world, Editor &editor, Gizmo &gizmo, Camera &camera)
{
 Entity *e = get_entity(world, editor.selected_entity);
 if (!e)
  return ;

 v3 axis[3] = {V3(1, 0, 0), V3(0, 1, 0), V3(0, 0, 1)};

 if (gizmo.mode == GIZMO_SCALE || gizmo.mode == GIZMO_ROTATION) {
  mat4 transform = get_entity_transform(world, *e);
  for (int i = 0; i < 3; i++)
   axis[i] = normalize(V3(transform.e[0][i], transform.e[1][i], transform.e[2][i]));
 }

 float dist_to_camera = length(e->position - camera.position);
 float axis_length = (0.07f * 2) * dist_to_camera;
 float other_axis_length = ((0.07f * 2) * 0.3f) * dist_to_camera;
 float scale_box_radius = ((0.07f * 2) * 0.1f) * dist_to_camera;

 for (int i = 0; i < 3; i++) {
  v3 color = {};
  color.e[i] = 1;

  if (gizmo.active && (gizmo.dragging_axis == i && !gizmo.uniform_scale))
   color = V3(1, 1, 0);

  if (gizmo.mode == GIZMO_SCALE) {
   push_line(e->position, e->position + axis_length * axis[i], color);
   push_box_outline(e->position + (axis_length - scale_box_radius) * axis[i],
    axis[0] * scale_box_radius,
    axis[1] * scale_box_radius,
    axis[2] * scale_box_radius, color);
  }
  else if (gizmo.mode == GIZMO_TRANSLATION) {
   push_line(e->position, e->position + axis_length * axis[i], color);
   push_ellipsoid_outline(e->position + axis_length * axis[i], V3(axis_length * (0.2f)), color);





  }
  else if (gizmo.mode == GIZMO_ROTATION) {
     push_circle(e->position,
       (0.5f*(1.5f + 2)),
       axis[(i + 1) % 3], axis[(i + 2) % 3], color);
  }
 }
 if (gizmo.mode == GIZMO_SCALE) {
  v3 color = gizmo.active && gizmo.uniform_scale ? V3(1, 1, 0) : V3(1);
  push_box_outline(e->position,
     axis[0] * scale_box_radius,
     axis[1] * scale_box_radius,
     axis[2] * scale_box_radius, color);
 }
}

void editor_update_gizmo(World &world, Editor &editor, Gizmo &gizmo, Camera &camera,
  v3 camera_ray_origin, v3 camera_ray_dir)
{
 Entity *e = get_entity(world, editor.selected_entity);
 if (!e) {
  gizmo.active = false;
  return ;
 }
 v3 axis[3] = {V3(1, 0, 0), V3(0, 1, 0), V3(0, 0, 1)};

 if (gizmo.mode == GIZMO_SCALE || gizmo.mode == GIZMO_ROTATION) {
  mat4 transform = get_entity_transform(world, *e);
  for (int i = 0; i < 3; i++)
   axis[i] = normalize(V3(transform.e[0][i], transform.e[1][i], transform.e[2][i]));
 }

 float dist_to_camera = length(e->position - camera.position);
 float axis_length = (0.07f * 2) * dist_to_camera;
 float other_axis_length = ((0.07f * 2) * 0.3f) * dist_to_camera;
 float scale_box_radius = ((0.07f * 2) * 0.1f) * dist_to_camera;

 if (!gizmo.active) {
  float min_axis_t = FLT_MAX;
  int best_axis = -1;

  for (int i = 0; i < 3; i++) {
   float t = -1;
   if (gizmo.mode == GIZMO_ROTATION) {
    if (ray_hit_plane(camera_ray_origin, camera_ray_dir, axis[i], e->position, &t)) {
     v3 p = camera_ray_origin + t * camera_ray_dir;
     if (t < 0
      || length(p - e->position) < 1.5f
      || length(p - e->position) > 2)
      continue ;
    }
   }
   else if (gizmo.mode == GIZMO_TRANSLATION) {
    t = ray_hit_box(camera_ray_origin, camera_ray_dir, e->position + 0.5f * axis_length * axis[i],
      axis[0] * 0.5f * (i == 0 ? axis_length * (1+2*(0.2f)) : other_axis_length),
      axis[1] * 0.5f * (i == 1 ? axis_length * (1+2*(0.2f)) : other_axis_length),
      axis[2] * 0.5f * (i == 2 ? axis_length * (1+2*(0.2f)) : other_axis_length));
   }
   else if (gizmo.mode == GIZMO_SCALE) {
    t = ray_hit_box(camera_ray_origin, camera_ray_dir, e->position +
      (axis_length - scale_box_radius)
      * axis[i],
      axis[0] * scale_box_radius,
      axis[1] * scale_box_radius,
      axis[2] * scale_box_radius);
   }
   if (t >= 0 && t < min_axis_t) {
    min_axis_t = t;
    best_axis = i;
   }
  }
  gizmo.uniform_scale = false;
  if (gizmo.mode == GIZMO_SCALE) {
   float t = ray_hit_box(camera_ray_origin, camera_ray_dir, e->position,
      axis[0] * scale_box_radius,
      axis[1] * scale_box_radius,
      axis[2] * scale_box_radius);
   if (t >= 0 && t < min_axis_t) {
    min_axis_t = t;
    gizmo.uniform_scale = true;
   }
  }
  if (min_axis_t != FLT_MAX) {
   gizmo.init_position = e->position;
   gizmo.init_rotation = e->rotation;
   gizmo.init_scale = e->scale;
   gizmo.active = true;
   gizmo.did_drag = false;
   gizmo.dragging_axis = best_axis;
  }
 }

 if (gizmo.active) {
  if (gizmo.did_drag && !v3_equal(camera.position, editor.last_camera_p))
   gizmo.did_drag = false;

  if (gizmo.mode == GIZMO_SCALE && gizmo.uniform_scale)
   gizmo.dragging_axis = 2;

  v3 plane_normal;



  {





   plane_normal = normalize(camera.forward - dot(axis[gizmo.dragging_axis], camera.forward) * axis[gizmo.dragging_axis] );
  }
  if (gizmo.mode == GIZMO_ROTATION)
   plane_normal = axis[gizmo.dragging_axis];



  float hit_t;
  if (ray_hit_plane(camera_ray_origin, camera_ray_dir, plane_normal, e->position, &hit_t)) {
   v3 hit_p = camera_ray_origin + hit_t * camera_ray_dir;

   if (!gizmo.did_drag)
    gizmo.hit_p = hit_p;

   if (gizmo.mode == GIZMO_TRANSLATION) {
    v3 delta_p = axis[gizmo.dragging_axis] * dot(hit_p - e->position, axis[gizmo.dragging_axis]);

    if (!gizmo.did_drag)
     gizmo.delta_p = delta_p;
    else
     e->position += delta_p - gizmo.delta_p;
   }
   else if (gizmo.mode == GIZMO_SCALE) {
    float delta_s = dot(hit_p - e->position, axis[gizmo.dragging_axis]);

    if (!gizmo.did_drag) {
     gizmo.scale = e->scale;
     gizmo.delta_s = delta_s;
    }
    else {
     v3 new_scale = e->scale;
     if (gizmo.uniform_scale) {
      v3 v = gizmo.hit_p - e->position;
      float l = dot(hit_p - e->position, normalize(v));
      float s = (l >= length(v) ? 1 + (l - length(v)) : 1 / (length(v) - l + 1));
      new_scale = s * gizmo.scale;
     }
     else
      new_scale.e[gizmo.dragging_axis] = gizmo.scale.e[gizmo.dragging_axis] + delta_s - gizmo.delta_s;
     for (int i = 0; i < 3; i++)
      if (new_scale.e[gizmo.dragging_axis] < 0.01f)
       new_scale.e[gizmo.dragging_axis] = 0.01f;
     e->scale = new_scale;
    }
   }
   else if (gizmo.mode == GIZMO_ROTATION) {
    v3 p = normalize(hit_p - e->position) * (0.5f*(1.5f + 2));

    push_cube_outline(e->position + p, V3(0.1), V3(0));

    v3 right_axis = axis[(gizmo.dragging_axis+1)%3];
    v3 up_axis = axis[(gizmo.dragging_axis+2)%3];

    if (gizmo.did_drag)
     right_axis = gizmo.rotation_right_axis, up_axis = gizmo.rotation_up_axis;
    else
     gizmo.rotation_axis = axis[gizmo.dragging_axis];

    p.xy = V2(dot(p, right_axis), dot(p, up_axis));

    float a = atan2(p.y, p.x);

    if (!gizmo.did_drag) {
     gizmo.did_drag = true;
     gizmo.rotation_angle = a;
     gizmo.rotation = e->rotation;
     gizmo.rotation_right_axis = right_axis;
     gizmo.rotation_up_axis = up_axis;
     gizmo.rotation_axis = axis[gizmo.dragging_axis];
    }
    else {
     e->rotation = rotate_around_axis_quat(gizmo.rotation_axis, a - gizmo.rotation_angle)
      * gizmo.rotation;
    }
   }

   if (!gizmo.did_drag)
    gizmo.did_drag = true;
  }
 }
}

void editor_push_entity_op(Game &game, World &world, Editor &editor)
{
 Entity *e = get_entity(world, editor.selected_entity);
 if (!editor.gizmo.active || !e)
  return ;

 EditorOp op = {};
 op.entity = editor.selected_entity;
 bool push = true;
 if (editor.gizmo.mode == GIZMO_TRANSLATION) {
  op.type = EDITOR_OP_TRANSLATE_ENTITY;
  op.translate.prev_p = editor.gizmo.init_position;
  op.translate.new_p = e->position;
  push &= (!v3_equal(op.translate.prev_p, op.translate.new_p));
 }
 else if (editor.gizmo.mode == GIZMO_SCALE) {
  op.type = EDITOR_OP_SCALE_ENTITY;
  op.scale.prev_scale = editor.gizmo.init_scale;
  op.scale.new_scale = e->scale;
  push &= (!v3_equal(op.scale.prev_scale, op.scale.new_scale));
 }
 else if (editor.gizmo.mode == GIZMO_ROTATION) {
  op.type = EDITOR_OP_ROTATE_ENTITY;
  op.rotate.prev_rot = editor.gizmo.init_rotation;
  op.rotate.new_rot = e->rotation;
  push &= (!quat_equal(op.rotate.prev_rot, op.rotate.new_rot));
 }
 if (push)
  do_editor_op(game, world, editor, op);
 editor.gizmo.active = false;
}

void editor_edit_collision_mesh(Game &game, GameInput &input, World &world, Editor &editor, Camera &camera, v3 camera_ray_origin,
  v3 camera_ray_dir)
{
 float min_hit_t;
 int hit_entity_mesh_index;
 entity_id hit_entity = raycast_to_entities(game, world, camera_ray_origin, camera_ray_dir,
   min_hit_t, hit_entity_mesh_index);

 Entity *e = get_entity(world, hit_entity);

 if (!e || !e->scene_id)
  return ;

 mat4 to_world = get_entity_transform(world, *e) * e->scene_transform;
 mat4 to_object = inverse(to_world);

 if (!world.scene_collision_mesh.count(e->scene_id)) {
  CollisionMesh cmesh = {};

  cmesh.vertices = make_array_max<v3>(&world.arena, 3 * 1000);
  cmesh.scene = e->scene_id;
  world.collision_meshes.push(cmesh);
  world.scene_collision_mesh[e->scene_id] = world.collision_meshes.count - 1;
 }

 CollisionMesh &cmesh = world.collision_meshes[world.scene_collision_mesh[e->scene_id]];

 v3 hit_p = camera_ray_origin + min_hit_t * camera_ray_dir;

 float dist_to_snap = 0.01f * length(hit_p - camera.position);

 if (((input.buttons[BUTTON_MOUSE_RIGHT].is_down) && !(input.buttons[BUTTON_MOUSE_RIGHT].was_down))) {
  hit_p = (to_object * V4(hit_p, 1)).xyz;

  float closest_dist = FLT_MAX;
  int best_p = 0;
  for (int i = 0; i < cmesh.vertices.count; i++) {
   float dist = length_sq(cmesh.vertices[i] - hit_p);
   if (dist < closest_dist) {
    closest_dist = dist;
    best_p = i;
   }
  }
  if (closest_dist < dist_to_snap * dist_to_snap)
   hit_p = cmesh.vertices[best_p];

  EditorOp op = {};
  op.type = EDITOR_OP_CREATE_MESH_COLLISION_VERTEX;
  op.entity = hit_entity;
  op.place_collision_vertex.pos = hit_p;

  do_editor_op(game, world, editor, op);

  hit_p = (to_world * V4(hit_p, 1)).xyz;
 }

 int hit_triangle = -1;
 {
  float min_t = FLT_MAX;
  for (int i = 0; i + 2 < cmesh.vertices.count; i += 3) {
   v3 v0 = (to_world * V4(cmesh.vertices[i + 0], 1)).xyz;
   v3 v1 = (to_world * V4(cmesh.vertices[i + 1], 1)).xyz;
   v3 v2 = (to_world * V4(cmesh.vertices[i + 2], 1)).xyz;

   float hit_t;
   if (ray_hit_triangle(camera_ray_origin, camera_ray_dir, v0, v1, v2, &hit_t)
     && hit_t < min_t) {
    min_t = hit_t;
    hit_triangle = i;
   }
  }
 }

 if (((input.buttons[BUTTON_DELETE].is_down) && !(input.buttons[BUTTON_DELETE].was_down))) {
  if (hit_triangle != -1) {
   EditorOp op = {};
   op.type = EDITOR_OP_DELETE_MESH_COLLISION_TRIANGLE;
   op.entity = hit_entity;
   op.delete_collision_triangle.index = hit_triangle;
   op.delete_collision_triangle.v0 = cmesh.vertices[hit_triangle + 0];
   op.delete_collision_triangle.v1 = cmesh.vertices[hit_triangle + 1];
   op.delete_collision_triangle.v2 = cmesh.vertices[hit_triangle + 2];
   do_editor_op(game, world, editor, op);
  }
 }

 push_cube_outline(hit_p, V3(dist_to_snap), V3(1, 1, 0));
 for (int i = (cmesh.vertices.count/3)*3; i < cmesh.vertices.count; i++)
  push_cube_outline((to_world * V4(cmesh.vertices[i], 1)).xyz, V3(dist_to_snap), V3(1, 0, 0));

 for (int i = 0; i + 2 < cmesh.vertices.count; i += 3) {
  v3 color = i == hit_triangle ? V3(1, 1, 0) : V3(1, 0, 0);
  push_triangle_outline((to_world * V4(cmesh.vertices[i + 0], 1)).xyz,
     (to_world * V4(cmesh.vertices[i + 1], 1)).xyz,
     (to_world * V4(cmesh.vertices[i + 2], 1)).xyz, color);
 }
}

void update_editor(Game &game, World &world, Editor &editor, GameInput &input, Camera &camera)
{
 {
  Entity *e = get_entity(world, editor.selected_entity);

  if (e) {
   ImGui::Begin("Entity");

   if (ImGui::Button("Reset scale"))
    e->scale = V3(1);
   if (ImGui::Button("Reset rotation"))
    e->rotation = identity_quat();
   if (e->id != world.player_id && ImGui::Button("delete")) {
    EditorOp op = {};
    op.entity = e->id;
    op.type = EDITOR_OP_DELETE_ENTITY;
    op.del.entity_data = *e;
    do_editor_op(game, world, editor, op);
    e = 0;
   }
   if (e)
    imgui_edit_struct_Entity(*e, "selected entity", false);

   ImGui::End();
  }
 }

 if (!game.in_editor)
  return ;

 v3 camera_ray_origin, camera_ray_dir;

 {
  v2 mouse_p = (input.mouse_p * V2(1.f / g_rc->window_width, 1.f / g_rc->window_height)) * 2 - V2(1);
  mouse_p.y *= -1;
  camera_ray_origin = camera.position;
  camera_ray_dir = camera.forward * camera.znear
   + mouse_p.x * camera.right * camera.width * 0.5f
   + mouse_p.y * camera.up * camera.height * 0.5f;
 }

 if ((input.buttons[BUTTON_LEFT_CONTROL].is_down) && ((input.buttons[BUTTON_Q].is_down) && !(input.buttons[BUTTON_Q].was_down)))
  editor.mode = (editor.mode == EDITOR_MODE_COLLISION_MESH ? EDITOR_MODE_GIZMO : EDITOR_MODE_COLLISION_MESH);

 switch (editor.mode) {
  case EDITOR_MODE_GIZMO: {
   if ((input.buttons[BUTTON_LEFT_CONTROL].is_down)) {
    if ((input.buttons[BUTTON_T].is_down)) editor.gizmo.mode = GIZMO_TRANSLATION;
    else if ((input.buttons[BUTTON_R].is_down)) editor.gizmo.mode = GIZMO_ROTATION;
    else if ((input.buttons[BUTTON_S].is_down)) editor.gizmo.mode = GIZMO_SCALE;
   }

   if ((input.buttons[BUTTON_MOUSE_RIGHT].is_down)) {
    editor_update_gizmo(world, editor, editor.gizmo, camera, camera_ray_origin, camera_ray_dir);

    if (!editor.gizmo.active) {

     float min_hit_t;
     int hit_entity_mesh_index;
     entity_id hit_entity = raycast_to_entities(game, world, camera_ray_origin, camera_ray_dir, min_hit_t, hit_entity_mesh_index);
     editor.selected_entity = hit_entity;
     editor.selected_entity_mesh = hit_entity_mesh_index;
    }
   }
   else
    editor_push_entity_op(game, world, editor);
   editor_render_gizmo(world, editor, editor.gizmo, camera);
   break ;
  }
  case EDITOR_MODE_COLLISION_MESH: {
   editor_edit_collision_mesh(game, input, world, editor, camera, camera_ray_origin, camera_ray_dir);
   break ;
  }
 }

 if ((input.buttons[BUTTON_LEFT_CONTROL].is_down) && ((input.buttons[BUTTON_C].is_down) && !(input.buttons[BUTTON_C].was_down)) && editor.selected_entity) {
  editor.copied_entity = editor.selected_entity;
  editor.copy_entity_mesh = (input.buttons[BUTTON_LEFT_SHIFT].is_down);
 }

 if ((input.buttons[BUTTON_LEFT_CONTROL].is_down) && ((input.buttons[BUTTON_V].is_down) && !(input.buttons[BUTTON_V].was_down))) {
  Entity *copy_from = get_entity(world, editor.copied_entity);
  if (copy_from) {
   EditorOp op = {};
   op.type = EDITOR_OP_PASTE_ENTITY;
   op.paste.copy_from = copy_from->id;
   op.paste.mesh_index = (editor.copy_entity_mesh ? editor.selected_entity_mesh : -1);

   float min_hit_t;
   int hit_mesh_index;
   entity_id hit_entity = raycast_to_entities(game, world, camera_ray_origin,
     camera_ray_dir, min_hit_t, hit_mesh_index);
   if (hit_entity)
    op.paste.p = camera_ray_origin + min_hit_t * camera_ray_dir + V3(0, 0, copy_from->scale.z);
   else
    op.paste.p = camera.position + camera.forward * 2 * copy_from->scale;

   do_editor_op(game, world, editor, op);
  }
 }

 if ((input.buttons[BUTTON_LEFT_CONTROL].is_down) && ((input.buttons[BUTTON_Z].is_down) && !(input.buttons[BUTTON_Z].was_down)))
  undo_editor_op(game, world, editor);
 if ((input.buttons[BUTTON_LEFT_CONTROL].is_down) && ((input.buttons[BUTTON_X].is_down) && !(input.buttons[BUTTON_X].was_down)))
  redo_editor_op(game, world, editor);

 editor.last_camera_p = camera.position;
}
# 109 "code/game.cpp" 2


ShadowMap create_shadow_map(int texture_width, int texture_height,
  v3 light_p, v3 light_dir, float znear, float zfar, float width, float height, v3 up = V3(0, 0, 1))
{
 ShadowMap shadow_map = {};

 shadow_map.tex_width = texture_width;
 shadow_map.tex_height = texture_height;
 shadow_map.light_p = light_p;
 shadow_map.light_dir = light_dir;

 shadow_map.view = lookat(shadow_map.light_p, shadow_map.light_dir, up);
 shadow_map.znear = znear;
 shadow_map.zfar = zfar;
 shadow_map.width = width;
 shadow_map.height = height;
 shadow_map.projection = orthographic_projection(znear, zfar, width, height);

 shadow_map.depth_texture = create_depth_texture(texture_width, texture_height);

 shadow_map.framebuffer = create_frame_buffer(true);

 bind_framebuffer_depthbuffer(shadow_map.framebuffer, shadow_map.depth_texture);

 if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  assert(0);
 return shadow_map;
}

extern "C" void game_update_and_render(Platform &platform, Arena *memory, GameInput &input, float dt)
{
 g_rc = (RenderContext *)platform.render_context;
 g_temp_arena = &platform.temp_arena;
 ImGui::SetCurrentContext((ImGuiContext *)platform.imgui_context);

 Game &game = *((Game *)memory->data);
 if (!game.is_initialized) {
  assert(memory->used == 0);
  arena_alloc_zero(memory, sizeof(game));

  init_render_context(memory, *g_rc, platform);

  usize temp_arena_size = (1024ULL * (1024ULL * 1024));
  g_temp_arena->arena = make_arena(_arena_alloc("code/game.cpp", __func__, 153, memory, temp_arena_size), temp_arena_size);


  game.scenes = make_array_max<Scene>(memory, 1024);





  game.world = new World();
  World &world = *game.world;

  world.arena = make_arena(_arena_alloc("code/game.cpp", __func__, 165, memory, (1024ULL * (1024ULL * 64))), (1024ULL * (1024ULL * 64)));
  world.editor.ops = make_array_max<EditorOp>(&world.arena, 8192);
  world.editor.undos = make_array_max<EditorOp>(&world.arena, 8192);

  game.asset_arena = make_arena(_arena_alloc("code/game.cpp", __func__, 169, memory, (1024ULL * (1024ULL * 2048))), (1024ULL * (1024ULL * 2048)));

  game.default_rasterizer_state = create_rasterizer_state(RASTERIZER_FILL_SOLID, RASTERIZER_CULL_NONE);
  game.default_depth_stencil_state = create_depth_stencil_state(true);
  game.disable_depth_state = create_depth_stencil_state(false);
  game.wireframe_rasterizer_state = create_rasterizer_state(RASTERIZER_FILL_WIREFRAME, RASTERIZER_CULL_NONE);




  VertexInputLayout input_layout = create_vertex_input_layout(g_vertex_input_elements,
    (sizeof(g_vertex_input_elements) / sizeof(*g_vertex_input_elements)), sizeof(Vertex));

  game.mesh_render_pass = create_render_pass(




    load_shader(make_cstring("shaders/vertex.glsl"), SHADER_TYPE_VERTEX),
    load_shader(make_cstring("shaders/fragment.glsl"), SHADER_TYPE_FRAGMENT),

    PRIMITIVE_TRIANGLES, game.default_depth_stencil_state, game.default_rasterizer_state,
    input_layout);

  game.outline_render_pass = create_render_pass(
    load_shader(make_cstring("shaders/outline_vertex.glsl"), SHADER_TYPE_VERTEX),
    load_shader(make_cstring("shaders/outline_fragment.glsl"), SHADER_TYPE_FRAGMENT),
    PRIMITIVE_TRIANGLES, game.default_depth_stencil_state, game.default_rasterizer_state,
    input_layout);

  game.shadow_map_render_pass = create_render_pass(
    load_shader(make_cstring("shaders/vertex.glsl"), SHADER_TYPE_VERTEX),
    load_shader(make_cstring("shaders/shadow_map_fs.glsl"), SHADER_TYPE_FRAGMENT),
    PRIMITIVE_TRIANGLES, game.default_depth_stencil_state, game.default_rasterizer_state,
    input_layout);

  game.shadow_map = create_shadow_map(4096, 4096,
    V3(24, 0, 24), V3(-1, 0, -1),
    1, 75, 50, 40);
  {
   VertexInputElement input_elements[] = {
    {0, 3, INPUT_ELEMENT_FLOAT, "POSITION"},
    {sizeof(v3), 3, INPUT_ELEMENT_FLOAT, "COLOR"},
   };
   VertexInputLayout line_input_layout = create_vertex_input_layout(input_elements, (sizeof(input_elements) / sizeof(*input_elements)),
     sizeof(v3) * 2);

   game.debug_lines_render_pass = create_render_pass(




     load_shader(make_cstring("shaders/debug_lines_vs.glsl"), SHADER_TYPE_VERTEX),
     load_shader(make_cstring("shaders/debug_lines_fs.glsl"), SHADER_TYPE_FRAGMENT),

     PRIMITIVE_LINES, game.default_depth_stencil_state, game.default_rasterizer_state,
     line_input_layout);

   game.debug_lines_vertex_buffer = create_vertex_buffer(VERTEX_BUFFER_DYNAMIC,
     g_rc->debug_lines.capacity * sizeof(v3));

   ConstantBufferElement elems[] = {
    {CONSTANT_BUFFER_ELEMENT_MAT4},
   };
   game.debug_lines_constant_buffer = create_constant_buffer(make_array<ConstantBufferElement>(elems, (sizeof(elems) / sizeof(*elems))));
  }



  {
   StructMetaData constant_metadata = get_struct_Constants_info();
   Arena *temp = begin_temp_memory();

   auto elems = make_array<ConstantBufferElement>(temp, constant_metadata.member_count);


   for (int i = 0; i < constant_metadata.member_count; i++) {

    elems[i] = {};
    bool found = false;
    for (int j = 0; j < CONSTANT_BUFFER_ELEMENT_COUNT; j++) {
     const char *type_name = get_constant_buffer_element_typename((ConstantBufferElementType)j);

     int k = 0;
     while (constant_metadata.members[i].type_name[k]
       == type_name[k] && type_name[k])
      k++;
     if (!constant_metadata.members[i].type_name[k] && !type_name[k]) {
      elems[i].type = (ConstantBufferElementType)j;
      found = true;
      break ;
     }
    }
    if (!found) {
     LOG_ERROR("in constant buffer elements, couldn't find type for %s", constant_metadata.members[i].type_name);
     assert(0);
    }
    if (constant_metadata.members[i].is_array)
     elems[i].array_size = constant_metadata.members[i].array_size;
   }

   game.constant_buffer = create_constant_buffer(elems);
   end_temp_memory();
  }

  load_scene(&game.asset_arena, game, "data/parking/zma_carpark_b2.obj");
  load_scene(&game.asset_arena, game, "data/Sponza/Sponza.fbx");
  load_scene(&game.asset_arena, game, "data/cube.fbx");
  load_scene(&game.asset_arena, game, "data/sphere.fbx");




  load_scene(&game.asset_arena, game, "data/Ybot.fbx");


  game.animations[ANIMATION_JUMP] = load_animation(&game.asset_arena, game, "data/jump.fbx");
  game.animations[ANIMATION_SHOOT] = load_animation(&game.asset_arena, game, "data/shoot.fbx");

  game.animations[ANIMATION_SHOOT].duration *= 0.6;
  game.animations[ANIMATION_RUN] = load_animation(&game.asset_arena, game, "data/run.fbx");
  game.animations[ANIMATION_FORWARD_GUN_WALK] = load_animation(&game.asset_arena, game, "data/forward_gun_walk.fbx");
  game.animations[ANIMATION_BACKWARD_GUN_WALK] = load_animation(&game.asset_arena, game, "data/backward_gun_walk.fbx");
  game.animations[ANIMATION_GUN_IDLE] = load_animation(&game.asset_arena, game, "data/gun_idle.fbx");
# 303 "code/game.cpp"
  FILE *fd = fopen("world.bin", "rb");
  if (!fd) {
   world.entities = make_array_max<Entity>(&world.arena, 4096);

   Entity *ground = make_entity(world, EntityType_Static, get_scene_id_by_name(game, make_cstring("cube.fbx")), V3(0, 0, -0.5));
   ground->scale = V3(100, 100, 0.5);
   ground->color = V3(0.3f);

   Entity *player = make_entity(world, EntityType_Player, get_scene_id_by_name(game, make_cstring("Ybot.fbx")),
     V3(0, 0, 4));
   player->ellipsoid_radius = V3(0.55f, 0.55f, 0.95f);
   player->ellipsoid_collision_shape = true;
   player->scene_transform = translate(0, 0, -player->ellipsoid_radius.z) * zrotation(3*3.14159265359f/2) * scale(V3(1.1));
   player->color = V3(0, 1, 1);

   world.player_id = player->id;
   world.editor_camera_p = V3(0, 0, 3);

   world.collision_meshes = make_array_max<CollisionMesh>(&world.arena, game.scenes.capacity);
  }
  else {
   serialize_World(fd, false, world, &world.arena);
   for (int i = 0; i < world.entities.count; i++)
    world.entities_id_map[world.entities[i].id] = i;
   for (int i = 0; i < world.collision_meshes.count; i++)
    world.scene_collision_mesh[world.collision_meshes[i].scene] = i;

   fclose(fd);
  }

  {
   game.debug_asset_fb = create_frame_buffer(false, true);

   Texture texture = create_texture(make_cstring("Debug Asset"), 0, 128, 128,
     true, false);
   Texture depth = create_depth_texture(texture.width, texture.height);
   bind_framebuffer_color(game.debug_asset_fb, texture);
   bind_framebuffer_depthbuffer(game.debug_asset_fb, depth);

   game.debug_asset_tex = create_texture(make_cstring("debug asset temp"),
      0, texture.width, texture.height, 1, 0);

   assert(glCheckFramebufferStatus(GL_FRAMEBUFFER)
     == GL_FRAMEBUFFER_COMPLETE);
  }

  game.sound_state.sample_count = 48000*10;
  game.sound_state.buffer = (float *)arena_alloc_zero(memory, game.sound_state.sample_count * 2 * sizeof(float));


  game.loaded_sounds[0] = load_wav_file(memory, "data/music.wav");
  game.loaded_sounds[1] = load_wav_file(memory, "data/jump.wav");

  game.master_volume = 1;
  game.frustum_culling = true;

  game.is_initialized = 1;
 }
 game.memory = memory;


 if (game.frame == 0)
  play_sound(game, game.loaded_sounds[0], 0);

 World &world = *game.world;

 if (((input.buttons[BUTTON_F1].is_down) && !(input.buttons[BUTTON_F1].was_down)))
  game.in_editor = !game.in_editor;

 if (ImGui::GetIO().WantCaptureKeyboard) {



 }

 begin_render_frame();

 for (int i = 0; i < (sizeof(profiler_block_stats) / sizeof(*profiler_block_stats)); i++)
  profiler_block_stats[i] = {};
 std::unordered_map<uint64_t, bool> occupied;

 int itr_count = 0;
 for (int i = 0; i < world.entities.count; i++) {
  Entity &e = world.entities[i];
  if (e.disable_collision || e.ellipsoid_collision_shape || !world.scene_collision_mesh.count(e.scene_id))
   continue ;

  CollisionMesh cmesh = world.collision_meshes[world.scene_collision_mesh[e.scene_id]];

  mat4 transform = get_entity_transform(world, e) * e.scene_transform;
  for (int j = 0; j + 2 < cmesh.vertices.count; j += 3) {
   v3 p0 = (transform * V4(cmesh.vertices[j + 0], 1)).xyz;
   v3 p1 = (transform * V4(cmesh.vertices[j + 1], 1)).xyz;
   v3 p2 = (transform * V4(cmesh.vertices[j + 2], 1)).xyz;

   v3i c1 = get_cell(p0);
   v3i c2 = get_cell(p1);
   v3i c3 = get_cell(p2);

   int d_u = roundf(length(p1 - p0) / (0.8f));
   int d_v = roundf(length(p2 - p0) / (0.8f));

   for (int u = 0; u <= d_u; u++) {
    float U = (u * (0.8f)) / length(p1 - p0);
    if (U > 1)
     break ;
    for (int v = 0; v <= d_v; v++) {
      float V = (v * (0.8f)) / length(p2 - p0);
      if (U + V > 1)
       break ;
      v3 p = p0 + (p1 - p0) * U + (p2 - p0) * V;
      v3i cell = get_cell(p);

      occupied[pack_cell(cell)] = true;
      itr_count++;

    }
   }
  }
 }
 world.occupied = occupied;

 LOG_DEBUG("occupied %zu (itr: %d)\n", occupied.size(), itr_count);
 for (auto [x, y] : occupied) {

 }

 if (!game.in_editor || game.play_in_editor) {
  update_player(game, world, input, dt);
  update_enemies(game, world, input, dt);
 }
 Camera game_camera = update_camera(game, world, input, dt);

 if (!game.in_editor)
  world.last_game_camera = game_camera;


 update_editor(game, world, world.editor, input, game_camera);
 if (game.in_editor) {

  Camera c = world.last_game_camera;

  v3 p[4] = {
   c.position + c.forward * c.znear - c.right * c.width * 0.5f - c.up * c.height * 0.5f,
   c.position + c.forward * c.znear + c.right * c.width * 0.5f - c.up * c.height * 0.5f,
   c.position + c.forward * c.znear + c.right * c.width * 0.5f + c.up * c.height * 0.5f,
   c.position + c.forward * c.znear - c.right * c.width * 0.5f + c.up * c.height * 0.5f,
  };
  push_line(c.position, p[0]);
  push_line(c.position, p[1]);
  push_line(c.position, p[2]);
  push_line(c.position, p[3]);
  push_line(p[0], p[1]);
  push_line(p[1], p[2]);
  push_line(p[2], p[3]);
  push_line(p[3], p[0]);
 }
 push_cube_outline(game.shadow_map.light_p, V3(0.3));
 push_line(game.shadow_map.light_p, game.shadow_map.light_p + 0.5 * game.shadow_map.light_dir);

 begin_render_pass(game.shadow_map_render_pass);
 {
  set_viewport(0, 0, game.shadow_map.tex_width, game.shadow_map.tex_height);
  bind_framebuffer(game.shadow_map.framebuffer);
  clear_framebuffer_depth(game.shadow_map.framebuffer, 1);
  render_entities(game, world, make_orthographic_camera(game.shadow_map.view, game.shadow_map.znear,
   game.shadow_map.zfar, game.shadow_map.width, game.shadow_map.height), true);
 }
 end_render_pass();

 begin_render_pass(game.mesh_render_pass);
 {
  set_viewport(0, 0, g_rc->window_width, g_rc->window_height);

  bind_framebuffer(g_rc->window_framebuffer);
  clear_framebuffer_color(g_rc->window_framebuffer, V4(0.392f, 0.584f, 0.929f, 1.f));
  clear_framebuffer_depth(g_rc->window_framebuffer, 1);

  bind_texture(4, game.shadow_map.depth_texture);
  render_entities(game, world, game_camera, false);






 }
 end_render_pass();





 begin_render_pass(game.debug_lines_render_pass);
 {


  update_vertex_buffer(game.debug_lines_vertex_buffer, (int)g_rc->debug_lines.count * sizeof(v3),
    g_rc->debug_lines.data);
  mat4 mvp = game_camera.projection * game_camera.view;
  bind_constant_buffer(game.debug_lines_constant_buffer, 1);
  update_constant_buffer(game.debug_lines_constant_buffer, &mvp);
  bind_vertex_buffer(game.debug_lines_vertex_buffer);
  draw(0, (int)(g_rc->debug_lines.count / 2));
 }
 end_render_pass();


 {
  ImGuiIO &io = ImGui::GetIO();
  ImGui::Begin("debug");
  ImGui::Text("average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
  ImGui::Text("resolution: %dx%d", g_rc->window_width, g_rc->window_height);

  ImGui::Text("entity count: %ld", world.entities.count);
  if (ImGui::Button("new cube")) {
   Entity *entity = make_entity(world, EntityType_Static,
     get_scene_id_by_name(game, make_cstring("cube.fbx")), game_camera.position
     + game_camera.forward * 4);
   world.editor.selected_entity = entity->id;
  }
  if (ImGui::Button("new sphere")) {
   Entity *entity = make_entity(world, EntityType_Static,
     get_scene_id_by_name(game, make_cstring("sphere.fbx")), game_camera.position
     + game_camera.forward * 4);
   entity->ellipsoid_radius = V3(1);
   entity->ellipsoid_collision_shape = true;
   world.editor.selected_entity = entity->id;
  }

  if (ImGui::Button("save world")) {
   FILE *fd = fopen("world.bin", "wb");
   if (!fd)
    assert(0);
   serialize_World(fd, true, world);
   fclose(fd);
  }

  ImGui::Checkbox("play in editor", &game.play_in_editor);

  if (get_entity(world, world.editor.copied_entity))
   ImGui::Text("copying entity %ld", world.editor.copied_entity);
  imgui_edit_struct_Game(game, "game");
  imgui_edit_struct_World(world, "world");


  ImGui::End();
 }


 end_render_frame();

 update_sound(game, world);
# 565 "code/game.cpp"
 game.time += dt;

 if (game.frame == 0 || !game.in_editor)
  ImGui::SetWindowFocus(NULL);
 game.frame++;
}
