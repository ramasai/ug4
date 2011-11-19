/**
 * Draws a pixel at the coordinate (x,y).
 */
inline void draw_pixel(int x, int y)
{
	glBegin(GL_POINTS);
		glVertex2i((int)x, (int)y);
	glEnd();
}

/*
 * Draws a line using the Midpoint line algorithm from (x0,y0) to (x1,y1).
 */
void bresenhams_line(int x0, int y0, int x1, int y1)
{
	int dx = abs(x1-x0);
	int dy = abs(y1-y0);

	int sx, sy;
	int err = dx - dy;

	sx = (x0 < x1) ? 1 : -1;
	sy = (y0 < y1) ? 1 : -1;

	while(true)
	{
		draw_pixel(x0, y0);

		if (x0 == x1 && y0 == y1) break;

		int e2 = 2 * err;

		if (e2 > -dy)
		{
			err = err - dy;
			x0 = x0 + sx;
		}

		if (e2 < dx)
		{
			err = err + dx;
			y0 = y0 + sy;
		}
	}
}

/*
 * Draws a line using the DDA line algorithm from (x0,y0) to (x1,y1).
 */
void dda_line(int x1, int y1, int x2, int y2)
{
	float x,y;

	int dx = x2-x1;
	int dy = y2-y1;

	int n = max(abs(dx), abs(dy));
	float dt = n, dxdt = dx/dt, dydt = dy/dt;

	x = x1;
	y = y1;

	while (n--) {
		draw_pixel((int)x, (int)y);

		x += dxdt;
		y += dydt;
	}
}
