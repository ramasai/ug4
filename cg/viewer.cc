#include <iostream>
#include <fstream>
#include <GL/glut.h>
#include <cstring>

#include "viewer.h"

using namespace std;

int nRows = 800;
int nCols = 600;

int translation_x = 0;
int translation_y = 0;

float scale = 1.0f;

float rotation_x = 0;
float rotation_y = 0;
float rotation_z = 0;

TriangleMesh trig;

void TriangleMesh::loadFile(char * filename)
{
	ifstream f(filename);

	if (f == NULL) {
		cerr << "failed reading polygon data file " << filename << endl;
		exit(1);
	}

	char buf[1024];
	char header[100];
	float x,y,z;
	float xmax,ymax,zmax,xmin,ymin,zmin;
	int v1, v2, v3, n1, n2, n3;

	xmax =-10000; ymax =-10000; zmax =-10000;
	xmin =10000; ymin =10000; zmin =10000;
	Vector3f av;
	av[0] = av[1] = av[2] = 0.f;

	while (!f.eof()) {
		    f.getline(buf, sizeof(buf));
		    sscanf(buf, "%s", header);

		    if (strcmp(header, "v") == 0) {
			sscanf(buf, "%s %f %f %f", header, &x, &y, &z);

		//	x *= 1000; y *= 1000; z *= 1000;

			_v.push_back(Vector3f(x,y,z));


			av[0] += x; av[1] += y; av[2] += z;

			if (x > xmax) xmax = x;
			if (y > ymax) ymax = y;
			if (z > zmax) zmax = z;

			if (x < xmin) xmin = x;
			if (y < ymin) ymin = y;
			if (z < zmin) zmin = z;
		    }
		    else if (strcmp(header, "f") == 0) {
			sscanf(buf, "%s %d %d %d", header, &v1, &v2, &v3);

			Triangle trig(v1-1, v2-1, v3-1);
			_trig.push_back(trig);

		    }
 	}

	_xmin = xmin; _ymin = ymin; _zmin = zmin;
	_xmax = xmax; _ymax = ymax; _zmax = zmax;

	float range;
	if (xmax-xmin > ymax-ymin) range = xmax-xmin;
	else range = ymax-ymin;

	for (int j = 0; j < 3; j++) av[j] /= _v.size();

	for (int i = 0; i < _v.size(); i++)
	{
		for (int j = 0; j < 3; j++) _v[i][j] = (_v[i][j]-av[j])/range*400;
	}
	cout << "trig " << _trig.size() << " vertices " << _v.size() << endl;
	f.close();
};

void draw_pixel(int x, int y)
{
	glBegin(GL_POINTS);
		glVertex2i((int)x, (int)y);
	glEnd();
}

void wiki_line(int x0, int y0, int x1, int y1)
{
	int dx = abs(x1-x0);
	int dy = abs(y1-y0);

	int sx, sy;
	int err = dx - dy;

	if (x0 < x1)
	{
		sx = 1;
	}
	else
	{
		sx = -1;
	}

	if (y0 < y1)
	{
		sy = 1;
	}
	else
	{
		sy = -1;
	}

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

void midpoint_line(int x1, int y1, int x2, int y2)
{
	int dx = x2 - x1;
	int dy = y2 - y1;

	int d = 2 * (dy-dx);

	int increE = 2*dy;
	int incrNE = 2*(dy-dx);

	int x = x1;
	int y = y1;

	draw_pixel(x, y);

	while (x < x2) {
		if (d <= 0) {
			d += increE;
			x++;
		} else {
			d+= incrNE;
			x++;
			y++;
		}

		draw_pixel(x, y);
	}
}

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

void transform(Vector3f &v,
		float a, float b, float c, float d,
		float e, float f, float g, float h,
		float i, float j, float k, float l,
		float m, float n, float o, float p)
{
	float v1 = v[0];
	float v2 = v[1];
	float v3 = v[2];
	float v4 = 1.0f;

	float ov1 = (v1*a) + (v2*e) + (v3*i) + (v4*m);
	float ov2 = (v1*b) + (v2*f) + (v3*j) + (v4*n);
	float ov3 = (v1*c) + (v2*g) + (v3*k) + (v4*o);
	float ov4 = (v1*d) + (v2*h) + (v3*l) + (v4*p);

	v[0] = ov1/ov4;
	v[1] = ov2/ov4;
	v[2] = ov3/ov4;
}

void translate_vector(Vector3f &v, float x, float y, float z)
{
	transform(v,
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			x,y,z,1);
}

void scale_vector(Vector3f &v, float x, float y, float z)
{
	transform(v,
			x,0,0,0,
			0,y,0,0,
			0,0,z,0,
			0,0,0,1);
}

void rotate_vector_x(Vector3f &v, float d)
{
	transform(v,
			1,     0,      0,0,
			0,cos(d),-sin(d),0,
			0,sin(d), cos(d),0,
			0,     0,      0,1);
}

void rotate_vector_y(Vector3f &v, float d)
{
	transform(v,
			cos(d) ,0,sin(d),0,
			0      ,1,     0,0,
			-sin(d),0,cos(d),0,
			0      ,0,     0,1);
}

void rotate_vector_z(Vector3f &v, float d)
{
	transform(v,
			cos(d),-sin(d),0,0,
			sin(d),cos(d) ,0,0,
			0     ,      0,1,0,
			0     ,      0,0,1);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // Clear OpenGL Window

	/** drawing a line for test **/

	/*** clear the Zbuffer here ****/

	int trignum = trig.trigNum();
	Vector3f v1,v2,v3;

	glColor3f(1,1,1);  // change the colour of the pixel

	//
	// for all the triangles, get the location of the vertices,
	// project them on the xy plane, and color the corresponding pixel by white
	//

	for (int i = 0 ; i < trignum; i++)
	{
		/*** do the rasterization of the triangles here using glRecti ***/
		trig.getTriangleVertices(i, v1,v2,v3);

		translate_vector(v1, translation_x, translation_y, 0);
		translate_vector(v2, translation_x, translation_y, 0);
		translate_vector(v3, translation_x, translation_y, 0);

		rotate_vector_x(v1, rotation_x);
		rotate_vector_x(v2, rotation_x);
		rotate_vector_x(v3, rotation_x);

		rotate_vector_y(v1, rotation_y);
		rotate_vector_y(v2, rotation_y);
		rotate_vector_y(v3, rotation_y);

		/*
		rotate_vector_z(v1, 0.523598);
		rotate_vector_z(v2, 0.523598);
		rotate_vector_z(v3, 0.523598);
		*/

		scale_vector(v1, scale, scale, scale);
		scale_vector(v2, scale, scale, scale);
		scale_vector(v3, scale, scale, scale);

		//
		// colouring the pixels at the vertex location
		// (just doing parallel projectiion to the xy plane.
		// only use glBegin(GL_POINTS) for rendering the scene
		//
		glBegin(GL_POINTS);
			glVertex2i((int)v1[0],(int)v1[1]);
			glVertex2i((int)v2[0],(int)v2[1]);
			glVertex2i((int)v3[0],(int)v3[1]);
		glEnd();

		/*
		dda_line((int)v1[0],(int)v1[1], (int)v2[0], (int)v2[1]);
		dda_line((int)v2[0],(int)v2[1], (int)v3[0], (int)v3[1]);
		dda_line((int)v3[0],(int)v3[1], (int)v1[0], (int)v1[1]);
		*/

		/*
		midpoint_line((int)v1[0],(int)v1[1], (int)v2[0], (int)v2[1]);
		midpoint_line((int)v2[0],(int)v2[1], (int)v3[0], (int)v3[1]);
		midpoint_line((int)v3[0],(int)v3[1], (int)v1[0], (int)v1[1]);
		*/

		wiki_line((int)v1[0],(int)v1[1], (int)v2[0], (int)v2[1]);
		wiki_line((int)v2[0],(int)v2[1], (int)v3[0], (int)v3[1]);
		wiki_line((int)v3[0],(int)v3[1], (int)v1[0], (int)v1[1]);
	}

	glFlush();// Output everything
}

void keyboardSpecial(int key, int x, int y)
{
	switch (key)
	{
		case GLUT_KEY_UP:
			rotation_x += 0.05;
			break;
		case GLUT_KEY_LEFT:
			rotation_y += 0.05;
			break;
		case GLUT_KEY_DOWN:
			rotation_x -= 0.05;
			break;
		case GLUT_KEY_RIGHT:
			rotation_y -= 0.05;
			break;
	}

	display();
}

void keyboard(unsigned char key, int x, int y)
{
	cout << "Key pressed: " << key << " - " << (int)key << " (" << x << "," << y << ")" << endl;

	switch (key)
	{
		case 'w':
			translation_y += 10;
			break;
		case 'a':
			translation_x -= 10;
			break;
		case 's':
			translation_y -= 10;
			break;
		case 'd':
			translation_x += 10;
			break;
		case 'z':
			scale += 0.1;
			break;
		case 'x':
			scale -= 0.1;
			break;
	}

	display();
}

int main(int argc, char **argv)
{
	if (argc >  1)  {
		trig.loadFile(argv[1]);
	}
	else {
		cerr << argv[0] << " <filename> " << endl;
		exit(1);
	}

	int width, height;
	glutInit(&argc, argv);
	glutInitWindowSize(nRows, nCols);
	glutCreateWindow("Model Viewer");
	gluOrtho2D(-nRows/2, nRows/2, -(float)nCols/2,  (float)nCols/2);
	glutDisplayFunc(display);// Callback function
	glutKeyboardUpFunc(keyboard);
	glutSpecialUpFunc(keyboardSpecial);
	glutMainLoop();// Display everything and wait
}
