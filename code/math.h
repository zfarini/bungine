#pragma once

#define PI 3.14159265359f
#define DEG2RAD (PI / 180.f)
#define RAD2DEG (180.f / PI)
function int sign(float x) {
    if (x < 0)
        return -1;
    else if (x > 0)
        return 1;
    return 0;
}

// TODO: cleanup maybe just template for vectors
// ~ V2
union v2 {
    struct {
        float x, y;
    };
    struct {
        float u, v;
    };
    float e[2];
};

function v2 V2(float x) { return v2{x, x}; }
function v2 V2(float x, float y) { return v2{x, y}; }

function v2 operator+(v2 a, v2 b) { return v2{a.x + b.x, a.y + b.y}; }

function v2 operator-(v2 a, v2 b) { return v2{a.x - b.x, a.y - b.y}; }

function v2 operator*(v2 a, float b) { return v2{a.x * b, a.y * b}; }

function v2 operator*(float a, v2 b) { return v2{a * b.x, a * b.y}; }

function v2 operator*(v2 a, v2 b) { return v2{a.x * b.x, a.y * b.y}; }

function v2 operator/(v2 a, float b) {
    float inv = 1.f / b;
    return v2{a.x * inv, a.y * inv};
}

function v2 &operator+=(v2 &a, v2 b) { return a = a + b; }

function v2 &operator-=(v2 &a, v2 b) { return a = a - b; }

function v2 &operator*=(v2 &a, v2 b) { return a = a * b; }

function v2 &operator*=(v2 &a, float b) { return a = a * b; }

function v2 &operator/=(v2 &a, float b) { return a = a / b; }

function float dot(v2 a, v2 b) { return a.x * b.x + a.y * b.y; }

function float length_sq(v2 a) { return dot(a, a); }

function float length(v2 a) { return sqrtf(dot(a, a)); }

function v2 normalize(v2 a) {
    float len = length_sq(a);

    // TODO:
    if (len < 1e-9)
        return {};
    else
        return a / sqrtf(len);
}

// ~ v3
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

function v3 V3(float x) { return v3{x, x, x}; }

function v3 V3(float x, float y, float z) {
    v3 result;

    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}

function v3 operator+(v3 a, v3 b) { return v3{a.x + b.x, a.y + b.y, a.z + b.z}; }

function v3 operator-(v3 a, v3 b) { return v3{a.x - b.x, a.y - b.y, a.z - b.z}; }

function v3 operator*(v3 a, float b) { return v3{a.x * b, a.y * b, a.z * b}; }

function v3 operator*(float a, v3 b) { return v3{a * b.x, a * b.y, a * b.z}; }

function v3 operator/(v3 a, float b) {
    float inv = 1.f / b;
    return v3{a.x * inv, a.y * inv, a.z * inv};
}

function v3 operator*(v3 a, v3 b) { return v3{a.x * b.x, a.y * b.y, a.z * b.z}; }

function v3 operator-(v3 a) { return v3{-a.x, -a.y, -a.z}; }

function v3 &operator+=(v3 &a, v3 b) { return a = a + b; }

function v3 &operator-=(v3 &a, v3 b) { return a = a - b; }

function v3 &operator*=(v3 &a, v3 b) { return a = a * b; }

function v3 &operator*=(v3 &a, float b) { return a = a * b; }

function v3 &operator/=(v3 &a, float b) { return a = a / b; }

function v3 operator/(float a, v3 b) { return v3{a / b.x, a / b.y, a / b.z}; }

function float dot(v3 a, v3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

function float length_sq(v3 a) { return dot(a, a); }

function float length(v3 a) { return sqrtf(dot(a, a)); }

function v3 normalize(v3 a) {
    float len = length_sq(a);
    // TODO: cleanup
    if (len < 1e-9)
        return {};
    else
        return a / sqrtf(len);
}

function v3 cross(v3 a, v3 b) {
    return v3{a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
              a.x * b.y - a.y * b.x};
}

function v3 min(v3 a, v3 b) { return V3(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z)); }

function v3 max(v3 a, v3 b) { return V3(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z)); }

function bool v3_equal(v3 a, v3 b, float eps = 1e-6) {
    return fabsf(a.x - b.x) < eps && fabsf(a.y - b.y) < eps &&
           fabsf(a.z - b.z) < eps;
}

union v3i {
    struct {
        int x, y, z;
    };
    int e[3];
};

function v3i V3i(int x, int y, int z) { return v3i{x, y, z}; }

function v3i V3i(int x) { return v3i{x, x, x}; }

function v3 V3(v3i v) { return v3{(float)v.x, (float)v.y, (float)v.z}; }

function v3i operator+(v3i a, v3i b) { return v3i{a.x + b.x, a.y + b.y, a.z + b.z}; }

function v3i operator-(v3i a, v3i b) { return v3i{a.x - b.x, a.y - b.y, a.z - b.z}; }

function v3i operator-(v3i a) { return v3i{-a.x, -a.y, -a.z}; }

function v3i max(v3i a, v3i b) {
    return V3i(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
}

function v3i min(v3i a, v3i b) {
    return V3i(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
}

// ~ v4
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

function v4 V4(float x, float y, float z, float w) {
    v4 v;

    v.x = x, v.y = y, v.z = z, v.w = w;
    return v;
}

function v4 V4(v3 xyz, float w) {
    v4 v;

    v.xyz = xyz;
    v.w = w;
    return v;
}

function v4 operator+(v4 a, v4 b) {
    return v4{a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

function v4 operator-(v4 a, v4 b) {
    return v4{a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

function v4 operator*(v4 a, float b) { return v4{a.x * b, a.y * b, a.z * b, a.w * b}; }

function float dot(v4 a, v4 b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }

function v4 operator-(v4 a) { return v4{-a.x, -a.y, -a.z, -a.w}; }

function float length(v4 a) { return sqrtf(dot(a, a)); }

// ~ mat4
union mat4 {
    float e[4][4]; // row-major
    float v[16];
};

function mat4 mat4_rows(v4 r0, v4 r1, v4 r2, v4 r3) {
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

function mat4 mat4_cols(v4 c0, v4 c1, v4 c2, v4 c3) {
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

function mat4 transpose(mat4 a) {
    mat4 b;

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            b.e[i][j] = a.e[j][i];
    return b;
}

function mat4 operator*(mat4 a, mat4 b) {
    mat4 c = {};

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 4; k++)
                c.e[i][j] += a.e[i][k] * b.e[k][j];
    return c;
}

function v4 operator*(mat4 a, v4 b) {
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

function mat4 scale(float a) {
    mat4 S = {};

    S.e[0][0] = S.e[1][1] = S.e[2][2] = a;
    S.e[3][3] = 1;
    return S;
}

function mat4 scale(float x, float y, float z) {
    mat4 S = {};

    S.e[0][0] = x;
    S.e[1][1] = y;
    S.e[2][2] = z;
    S.e[3][3] = 1;
    return S;
}

function mat4 scale(v3 v) { return scale(v.x, v.y, v.z); }

function mat4 identity() { return scale(1); }

function mat4 translate(float x, float y, float z) {
    mat4 T = identity();

    T.e[0][3] = x;
    T.e[1][3] = y;
    T.e[2][3] = z;
    return T;
}

function mat4 translate(v3 t) { return translate(t.x, t.y, t.z); }

function mat4 operator*(float a, mat4 b) {
    mat4 c;

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            c.e[i][j] = a * b.e[i][j];
    return c;
}

function mat4 zrotation(float a) {
    float c = cosf(a);
    float s = sinf(a);

    return {c, -s, 0, 0, s, c, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
}

function mat4 xrotation(float a) {
    float c = cosf(a);
    float s = sinf(a);

    return {1, 0, 0, 0, 0, c, -s, 0, 0, s, c, 0, 0, 0, 0, 1};
}

function mat4 yrotation(float a) {
    float c = cosf(a);
    float s = sinf(a);

    return {c, 0, s, 0, 0, 1, 0, 0, -s, 0, c, 0, 0, 0, 0, 1};
}

function mat4 rotate_around_axis(v3 u, float a) {
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

function mat4 perspective_projection(float znear, float zfar, float width, float height) {
    return {znear * 2 / width,
            0,
            0,
            0,
            0,
            znear * 2 / height,
            0,
            0,
#ifdef RENDERER_DX11
            0,
            0,
            zfar / (znear - zfar),
            zfar * znear / (znear - zfar),
#else
            0,
            0,
            (zfar + znear) / (znear - zfar),
            2 * znear * zfar / (znear - zfar),
#endif
            0,
            0,
            -1,
            0};
}

function mat4 orthographic_projection(float znear, float zfar, float width, float height) {
    return {2.f / width,
            0,
            0,
            0,
            0,
            2.f / height,
            0,
            0,
#ifdef RENDERER_DX11
            0,
            0,
            1.f / (znear - zfar),
            znear / (znear - zfar),
#else
            0,
            0,
            -2.f / (zfar - znear),
            (-znear - zfar) / (zfar - znear),
#endif
            0,
            0,
            0,
            1};
}

function mat4 lookat(v3 position, v3 dir, v3 up) {
    v3 z_axis = normalize(-dir);
    // if (fabsf(dot(normalize(dir), normalize(up))) > 0.95)
    //	assert(0);
    v3 x_axis = normalize(cross(up, z_axis));
    v3 y_axis = normalize(cross(z_axis, x_axis));

    mat4 transform = {
        x_axis.x, x_axis.y, x_axis.z, 0, y_axis.x, y_axis.y, y_axis.z, 0,
        z_axis.x, z_axis.y, z_axis.z, 0, 0,        0,        0,        1};

    return transform * translate(-position);
}

using quat = v4;

function quat Quat(float x, float y, float z, float w) { return {x, y, z, w}; }

function quat operator*(quat a, quat b) {
    return {
        a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y, // i
        a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x, // j
        a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w, // k
        a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z, // 1
    };
}

function mat4 quat_to_mat(quat a) {
    float x = a.x;
    float y = a.y;
    float z = a.z;
    float w = a.w;

    mat4 matrix;

    float wx, wy, wz, xx, yy, yz, xy, xz, zz;
    // adapted from Shoemake
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

function quat quat_lerp(quat a, quat b, float t) {
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

function mat4 inverse(mat4 matrix) {
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

function bool ray_hit_plane(v3 ray_origin, v3 ray_dir, v3 plane_normal, v3 plane_point,
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

function bool ray_hit_triangle(v3 ray_origin, v3 ray_dir, v3 v0, v3 v1, v3 v2, float *hit_t)
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

function void push_cube_outline(v3 p, v3 r, v3 color);

// TODO: cleanup
function float ray_hit_box(v3 ray_origin, v3 ray_dir, v3 box_center, v3 box_xaxis,
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
        /*
                dot(O + t * D, normal) = dot(normal, p)

                t * dot(D, normal) = dot(normal, p) - dot(O, normal) / dot(D,
           normal)

        */
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

function quat rotate_around_axis_quat(v3 axis, float a) {
    axis = normalize(axis);
    float s = sinf(a / 2);
    float c = cosf(a / 2);
    return V4(axis * s, c);
}

function quat zrotation_quat(float a) { return rotate_around_axis_quat(V3(0, 0, 1), a); }

function quat identity_quat() { return Quat(0, 0, 0, 1); }
