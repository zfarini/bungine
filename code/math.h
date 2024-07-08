#ifndef MATH_H
#define MATH_H

#include <math.h>

#define PI 3.14159265359f

float degree_to_radians(float a)
{
	return a * (PI / 180);
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
// ~ mat4
struct mat4
{
	float e[4][4]; // row-major
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

	S.e[0][0] = S.e[1][1] = S.e[2][2] = S.e[3][3] = a;
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

mat4 perspective_projection(float znear, float zfar, float width_fov_degree, float height_over_width)
{
	float width = 2 * znear * tanf(degree_to_radians(width_fov_degree / 2));
	float height = width * height_over_width;
	
	return {
		znear * 2 / width, 0, 0, 0,
		0, znear * 2 / height, 0, 0,
		0, 0, zfar / (znear - zfar), zfar * znear / (zfar - znear),
		0, 0, -1, 0
	};
}

using quat = v4;


#endif