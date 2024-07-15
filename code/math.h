#ifndef MATH_H
#define MATH_H

#include <math.h>

#define PI 3.14159265359f
#define DEG2RAD (PI / 180.f)

int sign(float x)
{
	if (x < 0) return -1;
	else if (x > 0) return 1;
	return 0;
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
	float e[3];
};

v3 V3(float x)
{
	return v3{x, x, x};
}

v3 V3(float x, float y, float z)
{
	v3 result;

	result.x = x;
	result.y = y;
	result.z = z;
	return result;
}

v3 operator+(v3 a, v3 b)
{
	return v3{a.x + b.x, a.y + b.y, a.z + b.z};
}

v3 operator-(v3 a, v3 b)
{
	return v3{a.x - b.x, a.y - b.y, a.z - b.z};
}

v3 operator*(v3 a, float b)
{
	return v3{a.x * b, a.y * b, a.z * b};
}

v3 operator*(float a, v3 b)
{
	return v3{a * b.x, a * b.y, a * b.z};
}

v3 operator/(v3 a, float b)
{
	float inv = 1.f / b;
	return v3{a.x * inv, a.y * inv, a.z * inv};
}

v3 operator*(v3 a, v3 b)
{
	return v3{a.x * b.x, a.y * b.y, a.z * b.z};
}

v3 operator-(v3 a)
{
	return v3{-a.x, -a.y, -a.z};
}

v3 &operator+=(v3 &a, v3 b)
{
	return a = a + b;
}

v3 &operator-=(v3 &a, v3 b)
{
	return a = a - b;
}

v3 &operator*=(v3 &a, v3 b)
{
	return a = a * b;
}

v3 &operator*=(v3 &a, float b)
{
	return a = a * b;
}

v3 &operator/=(v3 a, float b)
{
	return a = a / b;
}

float dot(v3 a, v3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

float length_sq(v3 a)
{
	return dot(a, a);
}

float length(v3 a)
{
	return sqrtf(dot(a, a));
}

v3 normalize(v3 a)
{
	float len = length_sq(a);
	// TODO:
	if (len < 1e-9)
		return {};
	else
		return a / sqrtf(len);
}

v3 cross(v3 a, v3 b)
{
	return v3 {
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	};
}

union v3i
{
	struct {
		int x, y, z;
	};
	int e[3];
};

v3i V3i(int x, int y, int z)
{
	return v3i{x, y, z};
}

v3i V3i(int x)
{
	return v3i{x, x, x};
}

v3 V3(v3i v)
{
	return v3{(float)v.x, (float)v.y, (float)v.z};
}

v3i operator+(v3i a, v3i b)
{
	return  v3i{a.x + b.x, a.y + b.y, a.z + b.z};
}

v3i operator-(v3i a, v3i b)
{
	return v3i{a.x - b.x, a.y - b.y, a.z - b.z};
}

v3i operator-(v3i a)
{
	return v3i{-a.x, -a.y, -a.z};
}

v3i max(v3i a, v3i b)
{
	return V3i(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
}

v3i min(v3i a, v3i b)
{
	return V3i(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
}

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

v2 V2(float x)
{
	return v2{x, x};
}
v2 V2(float x, float y)
{
	return v2{x, y};
}

v2 operator+(v2 a, v2 b)
{
	return v2{a.x + b.x, a.y + b.y};
}

v2 operator-(v2 a, v2 b)
{
	return v2{a.x - b.x, a.y - b.y};
}

v2 operator*(v2 a, float b)
{
	return v2{a.x * b, a.y * b};
}

v2 operator*(float a, v2 b)
{
	return v2{a * b.x, a * b.y};
}

v2 operator/(v2 a, float b)
{
	float inv = 1.f / b;
	return v2{a.x * inv, a.y * inv};
}

float dot(v2 a, v2 b)
{
	return a.x * b.x + a.y * b.y;
}

float length_sq(v2 a)
{
	return dot(a, a);
}

float length(v2 a)
{
	return sqrtf(dot(a, a));
}

v2 normalize(v2 a)
{
	float len = length_sq(a);

	// TODO:
	if (len < 1e-9)
		return {};
	else
		return a / sqrtf(len);
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

v4 V4(float x, float y, float z, float w)
{
	v4 v;

	v.x = x, v.y = y, v.z = z, v.w = w;
	return v;
}

v4 V4(v3 xyz, float w)
{
	v4 v;

	v.xyz = xyz;
	v.w = w;
	return v;
}

v4 operator+(v4 a, v4 b)
{
	return v4{a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

v4 operator-(v4 a, v4 b)
{
	return v4{a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

v4 operator*(v4 a, float b)
{
	return v4{a.x * b, a.y * b, a.z * b, a.w * b};
}

float dot(v4 a, v4 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

v4 operator-(v4 a)
{
	return v4{-a.x, -a.y, -a.z, -a.w};
}

float length(v4 a)
{
	return sqrtf(dot(a, a));
}

// ~ mat4
union mat4
{
	float e[4][4]; // row-major
	float v[16];
};

mat4 mat4_rows(v4 r0, v4 r1, v4 r2, v4 r3)
{
	mat4 m;

	m.e[0][0] = r0.e[0], m.e[0][1] = r0.e[1], m.e[0][2] = r0.e[2], m.e[0][3] = r0.e[3];
	m.e[1][0] = r1.e[0], m.e[3][1] = r1.e[1], m.e[1][2] = r1.e[2], m.e[1][3] = r1.e[3];
	m.e[2][0] = r2.e[0], m.e[2][1] = r2.e[1], m.e[2][2] = r2.e[2], m.e[2][3] = r2.e[3];
	m.e[3][0] = r3.e[0], m.e[1][1] = r3.e[1], m.e[3][2] = r3.e[2], m.e[3][3] = r3.e[3];
	return m;
}

mat4 mat4_cols(v4 c0, v4 c1, v4 c2, v4 c3)
{
	mat4 m;

	m.e[0][0] = c0.e[0], m.e[1][0] = c0.e[1], m.e[2][0] = c0.e[2], m.e[3][0] = c0.e[3];
	m.e[0][1] = c1.e[0], m.e[1][1] = c1.e[1], m.e[2][1] = c1.e[2], m.e[3][1] = c1.e[3];
	m.e[0][2] = c2.e[0], m.e[1][2] = c2.e[1], m.e[2][2] = c2.e[2], m.e[3][2] = c2.e[3];
	m.e[0][3] = c3.e[0], m.e[1][3] = c3.e[1], m.e[2][3] = c3.e[2], m.e[3][3] = c3.e[3];
	return m;
}

mat4 transpose(mat4 a)
{
	mat4 b;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			b.e[i][j] = a.e[j][i];
	return b;
}

mat4 operator*(mat4 a, mat4 b)
{
	mat4 c = {};

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
				c.e[i][j] += a.e[i][k] * b.e[k][j];
	return c;
}

v4 operator*(mat4 a, v4 b)
{
	v4 c = {};

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			c.e[i] += a.e[i][j] * b.e[j];
	return c;
}

mat4 scale(float a)
{
	mat4 S = {};

	S.e[0][0] = S.e[1][1] = S.e[2][2] = a;
	S.e[3][3] = 1;
 	return S;
}

mat4 scale(float x, float y, float z)
{
	mat4 S = {};

	S.e[0][0] = x;
	S.e[1][1] = y;
	S.e[2][2] = z;
	S.e[3][3] = 1;
	return S;
}

mat4 scale(v3 v)
{
	return scale(v.x, v.y, v.z);
}

mat4 identity()
{
	return scale(1);
}

mat4 translate(float x, float y, float z)
{
	mat4 T = identity();

	T.e[0][3] = x;
	T.e[1][3] = y;
	T.e[2][3] = z;
	return T;
}

mat4 translate(v3 t)
{
	return translate(t.x, t.y, t.z);
}

mat4 operator*(float a, mat4 b)
{
	mat4 c;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			c.e[i][j] = a * b.e[i][j];
	return c;
}

mat4 zrotation(float a)
{
	float c = cosf(a);
	float s = sinf(a);
	
	return {
		c, -s, 0, 0,
		s, c, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
}

mat4 xrotation(float a)
{
	float c = cosf(a);
	float s = sinf(a);

	return {
		1, 0, 0, 0,
		0, c, -s, 0,
		0, s, c, 0,
		0, 0, 0, 1
	};
}

mat4 yrotation(float a)
{
	float c = cosf(a);
	float s = sinf(a);

	return {
		c, 0, s, 0,
		0, 1, 0, 0,
		-s, 0, c, 0,
		0, 0, 0, 1
	};
}

mat4 rotate_around_axis(v3 u, float a)
{
	u = normalize(u);
	float c = cosf(a);
	float s = sinf(a);

	return {
		c+u.x*u.x*(1-c), u.x*u.y*(1-c)-u.z*s, u.x*u.z*(1-c)+u.y*s, 0,
		u.y*u.x*(1-c)+u.z*s, c+u.y*u.y*(1-c), u.y*u.z*(1-c)-u.x*s, 0,
		u.z*u.x*(1-c)-u.y*s, u.z*u.y*(1-c)+u.x*s, c+u.z*u.z*(1-c), 0,
		0, 0, 0, 1
	};
}

mat4 perspective_projection(float znear, float zfar, float width_fov_degree, float height_over_width)
{
	float width = 2 * znear * tanf(DEG2RAD * (width_fov_degree / 2));
	float height = width * height_over_width;
	
	return {
		znear * 2 / width, 0, 0, 0,
		0, znear * 2 / height, 0, 0,
		0, 0, zfar / (znear - zfar), zfar * znear / (znear - zfar),
		0, 0, -1, 0
	};
}

mat4 orthographic_projection(float znear, float zfar, float width, float height)
{
	//
	return {
		2.f / width, 0, 0, 0,
		0, 2.f / height, 0, 0,
		0, 0, 1.f / (znear-zfar), znear / (znear-zfar),
		0, 0, 0, 1	
	};
}

mat4 lookat(v3 position, v3 dir, v3 up)
{
	v3 z_axis = normalize(-dir);
	//if (fabsf(dot(normalize(dir), normalize(up))) > 0.95)
	//	assert(0);
	v3 x_axis = normalize(cross(up, z_axis));
	v3 y_axis = normalize(cross(z_axis, x_axis));

	mat4 transform = {
		x_axis.x, x_axis.y, x_axis.z, 0,
		y_axis.x, y_axis.y, y_axis.z, 0,
		z_axis.x, z_axis.y, z_axis.z, 0,
		0, 0, 0, 1
	};

	return transform * translate(-position);
}

using quat = v4;

mat4 quat_to_mat(quat a)
{
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

	matrix.v[0] = 1.0f - 2.0f*(yy + zz);
	matrix.v[4] = 2.0f*(xy - wz);
	matrix.v[8] = 2.0f*(xz + wy);
	matrix.v[12] = 0.0;

	matrix.v[1] = 2.0f*(xy + wz);
	matrix.v[5] = 1.0f - 2.0f*(xx + zz);
	matrix.v[9] = 2.0f*(yz - wx);
	matrix.v[13] = 0.0;

	matrix.v[2] = 2.0f*(xz - wy);
	matrix.v[6] = 2.0f*(yz + wx);
	matrix.v[10] = 1.0f - 2.0f*(xx + yy);
	matrix.v[14] = 0.0;

	matrix.v[3] = 0;
	matrix.v[7] = 0;
	matrix.v[11] = 0;
	matrix.v[15] = 1;

	return transpose(matrix);
}

quat quat_lerp(quat a, quat b, float t)
{
	float l2 = dot(a, b);
	if(l2 < 0.0f) 
		b = -b;
	v4 c;
	// c = a + t(b - a)  -->   c = a - t(a - b)
	// the latter is slightly better on x64
	c.x = a.x - t*(a.x - b.x);
	c.y = a.y - t*(a.y - b.y);
	c.z = a.z - t*(a.z - b.z);
	c.w = a.w - t*(a.w - b.w);
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

template<typename T>
T lerp(const T a, const T b, float t)
{
	return (1 - t) * a + t * b;
}

struct Transform {
	quat rotation;
	v3 translation;
	v3 scale;
};

#endif