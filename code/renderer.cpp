void push_line(v3 a, v3 b, v3 color = V3(1))
{
    g_rc.debug_lines.push(a);
    g_rc.debug_lines.push(color);
    g_rc.debug_lines.push(b);
    g_rc.debug_lines.push(color);
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
	float a = (2 * PI) / itr_count;

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

// void begin_frame(int window_width, int window_height)
// {

// }

// void end_frame()
// {

// }

