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
	// v3 y_axis = normalize(cross(max - min, V3(0, 0, 1)));
	// v3 x_axis = normalize(cross(y_axis, V3(0, 0, 1)));
	// v3 z_axis = normalize(cross(x_axis, y_axis));

	// v3 center = (min + max) * 0.5f;
	// v3 radius = (max - min) * 0.5f;


	// push_line(center, center + x_axis*fabsf(radius.x), V3(1, 0, 0));
	// push_line(center, center + y_axis*fabsf(radius.y), V3(0, 1, 0));
	// push_line(center, center + z_axis*fabsf(radius.z), V3(0, 0, 1));

	v3 p[8];
	int i = 0;
	/*
		-1 -1 -1
		1, -1 -1
		-1 1 -1
		1 1 -1
	*/
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

void init_render_context(Arena *arena, RenderContext &rc)
{
	rc.loaded_textures = make_array_max<Texture>(arena, 256);
    rc.debug_lines = make_array_max<v3>(arena, 4 * 500000);

   uint32_t white_color = 0xffffffff;
    rc.white_texture = create_texture(make_cstring("__white_texture"), &white_color, 1, 1, true);
}