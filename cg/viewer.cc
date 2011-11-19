/**
 * Not implemented:
 *
 *  - Antialiasing
 *  - Perspective transformation
 */
#include <iostream>
#include <algorithm>
#include <fstream>
#include <GL/glut.h>
#include <cstring>

#include "viewer.h"
#include "transformation.h"
#include "line_drawing.h"

using namespace std;

const int nRows = 800;
const int nCols = 600;

int zBuffer[nRows][nCols];

int translation_x = 0;
int translation_y = 0;

float scale = 1.0f;

float rotation_x = 0;
float rotation_y = 0;
float rotation_z = 0;

bool dda = false;

Vector3f light(0.0f, 0.0f, 100.0f);
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

void faceNormal(Vector3f &result, Vector3f vec1, Vector3f vec2, Vector3f vec3)
{
	float u0 = vec2[0] - vec1[0];
	float u1 = vec2[1] - vec1[1];
	float u2 = vec2[2] - vec1[2];
	Vector3f U(u0, u1, u2);

	float v0 = vec3[0] - vec1[0];
	float v1 = vec3[1] - vec1[1];
	float v2 = vec3[2] - vec1[2];
	Vector3f V(v0, v1, v2);

	float nx = (U[1]*V[2]) - (U[2]*V[1]);
	float ny = (U[2]*V[0]) - (U[0]*V[2]);
	float nz = (U[0]*V[1]) - (U[1]*V[0]);

	result[0] = nx;
	result[1] = ny;
	result[2] = nz;
}

bool isInside(int x, int y, Vector3f vec1, Vector3f vec2, Vector3f vec3)
{
	Vector3f vs[3] = {vec1, vec2, vec3};
	float xs[3] = {0,0,0};
	float ys[3] = {0,0,0};

	for (int i = 0; i < 3; i++)
	{
		xs[i] = vs[i][0];
		ys[i] = vs[i][2];
	}

	bool inside = false;

	for (int i = 0, j = 2; i < 3; j = i++)
	{
		if(((ys[i] > y) != (ys[j] > y)) && (x < (xs[j]-xs[i]) * (y-ys[i]) / (ys[j]-ys[i]) + xs[i]))
			inside = !inside;
	}

	if(inside)
		cout << "(" << x << ", " << y << ") - " << inside << endl;

	return true;
}

void draw(Vector3f vec1, Vector3f vec2, Vector3f vec3)
{
	//cout << "Vector 1: " << vec1 << endl;
	//cout << "Vector 2: " << vec2 << endl;
	//cout << "Vector 3: " << vec3 << endl;

	float min_x = min(vec1[0], vec2[0]);
	min_x = min(min_x, vec3[0]);

	float max_x = max(vec1[0], vec2[0]);
	max_x = max(max_x, vec3[0]);

	float min_y = min(vec1[1], vec2[1]);
	min_y = min(min_y, vec3[1]);

	float max_y = max(vec1[1], vec2[1]);
	max_y = max(max_y, vec3[1]);

	//cout << "X: " << min_x << "," << max_x << endl;
	//cout << "Y: " << min_y << "," << max_y << endl;

	for (int i = min_x; i < max_x; i++)
	{
		for (int j = min_y; j < max_y; j++)
		{
			if (isInside(i, j, vec1, vec2, vec3))
			{
				draw_pixel(i, j);
			}
		}
	}
}

void display()
{
	// Clear OpenGL Window
	glClear(GL_COLOR_BUFFER_BIT);

	int trignum = trig.trigNum();
	Vector3f v1, v2, v3;

	// change the colour of the pixel
	glColor3f(1,1,1);

	// for all the triangles, get the location of the vertices,
	// project them on the xy plane, and color the corresponding pixel by white
	//
	for (int i = 0 ; i < trignum; i++)
	{
		/*** do the rasterization of the triangles here using glRecti ***/
		trig.getTriangleVertices(i, v1,v2,v3);

		// calculate normal vector
		translate_vector(v1, translation_x, translation_y, 0);
		translate_vector(v2, translation_x, translation_y, 0);
		translate_vector(v3, translation_x, translation_y, 0);

		rotate_vector_x(v1, rotation_x);
		rotate_vector_x(v2, rotation_x);
		rotate_vector_x(v3, rotation_x);

		rotate_vector_y(v1, rotation_y);
		rotate_vector_y(v2, rotation_y);
		rotate_vector_y(v3, rotation_y);

		scale_vector(v1, scale, scale, scale);
		scale_vector(v2, scale, scale, scale);
		scale_vector(v3, scale, scale, scale);

		Vector3f triangleNormal;
		faceNormal(triangleNormal, v1, v2, v3);

		if (triangleNormal[2] > 0) continue;
		
		//
		// colouring the pixels at the vertex location
		// (just doing parallel projectiion to the xy plane.
		// only use glBegin(GL_POINTS) for rendering the scene
		//
	   //glBegin(GL_POINTS);
	   // glVertex2i((int)v1[0],(int)v1[1]);
	   // glVertex2i((int)v2[0],(int)v2[1]);
	   // glVertex2i((int)v3[0],(int)v3[1]);
	   //glEnd();

		//bresenhams_line((int)v1[0],(int)v1[1], (int)v2[0], (int)v2[1]);
		//bresenhams_line((int)v2[0],(int)v2[1], (int)v3[0], (int)v3[1]);
		//bresenhams_line((int)v3[0],(int)v3[1], (int)v1[0], (int)v1[1]);
		draw(v1, v2, v3);
	}

	glFlush();// Output everything
	cout << "done!";
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

	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	//cout << "Key pressed: " << key << " - " << (int)key << " (" << x << "," << y << ")" << endl;

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
			scale -= 0.1;
			break;
		case 'x':
			scale += 0.1;
			break;
		case ' ':
			dda = !dda;
	}

	glutPostRedisplay();
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

	for (int i = 0; i < nCols; i++)
	{
		for (int j = 0; j < nRows; j++)
		{
			zBuffer[i][j] = -99999.0f;
		}
	}

	glutInit(&argc, argv);
	glutInitWindowSize(nRows, nCols);
	glutCreateWindow("Model Viewer");
	gluOrtho2D(-nRows/2, nRows/2, -(float)nCols/2,  (float)nCols/2);
	glutDisplayFunc(display);// Callback function
	glutKeyboardUpFunc(keyboard);
	glutSpecialUpFunc(keyboardSpecial);
	glutMainLoop();// Display everything and wait
}
