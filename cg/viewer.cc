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

int zBuffer[nCols][nRows];

int translation_x = 0;
int translation_y = 0;

float scale = 1.0f;

float rotation_x = 0;
float rotation_y = 0;
float rotation_z = 0;

bool dda = false;

Vector3f camera(0.0f, 0.0f, 50.0f);
Vector3f light(500.0f, 500.0f, 10000.0f);
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
	int v1, v2, v3;

	xmax =-10000; ymax =-10000; zmax =-10000;
	xmin =10000; ymin =10000; zmin =10000;
	Vector3f av;
	av[0] = av[1] = av[2] = 0.f;

	while (!f.eof())
	{
		f.getline(buf, sizeof(buf));
		sscanf(buf, "%s", header);

		if (strcmp(header, "v") == 0)
		{
			sscanf(buf, "%s %f %f %f", header, &x, &y, &z);

			_v.push_back(Vector3f(x,y,z));

			av[0] += x; av[1] += y; av[2] += z;

			if (x > xmax) xmax = x;
			if (y > ymax) ymax = y;
			if (z > zmax) zmax = z;

			if (x < xmin) xmin = x;
			if (y < ymin) ymin = y;
			if (z < zmin) zmin = z;
		}
		else if (strcmp(header, "f") == 0)
		{
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

	for (unsigned int i = 0; i < _v.size(); i++)
	{
		for (int j = 0; j < 3; j++) _v[i][j] = (_v[i][j]-av[j])/range*400;
	}
	cout << "trig " << _trig.size() << " vertices " << _v.size() << endl;
	f.close();
};

void vectorSubtract(Vector3f &result, Vector3f a, Vector3f b)
{
	float u0 = a[0] - b[0];
	float u1 = a[1] - b[1];
	float u2 = a[2] - b[2];

	result[0] = u0;
	result[1] = u1;
	result[2] = u2;
}

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

float f(int a, int b, int x, int y, Vector3f vec1, Vector3f vec2, Vector3f vec3)
{
	Vector3f vecs[3] = {vec1, vec2, vec3};

	float y_a = vecs[a][1];
	float y_b = vecs[b][1];

	float x_a = vecs[a][0];
	float x_b = vecs[b][0];

	return ((y_a - y_b) * x) + ((x_b - x_a) * y) + (x_a * y_b) - (x_b * y_a);
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

	for (int x = min_x; x < max_x; x++)
	{
		for (int y = min_y; y < max_y; y++)
		{
			float alpha = f(1,2,x,y,vec1,vec2,vec3) / f(1,2,vec1[0],vec1[1],vec1,vec2,vec3);
			float beta  = f(2,0,x,y,vec1,vec2,vec3) / f(2,0,vec2[0],vec2[1],vec1,vec2,vec3);
			float gamma = f(0,1,x,y,vec1,vec2,vec3) / f(0,1,vec3[0],vec3[1],vec1,vec2,vec3);

			float z = (vec1[2] + vec2[2] + vec3[2]) / 3;

			if (alpha > 0 && beta > 0 && gamma > 0 && zBuffer[x + nCols/2][y + nRows/2] <= z)
			{
				draw_pixel(x,y);
				zBuffer[x + nCols/2][y + nRows/2] = z;
			}
		}
	}
}

void normalise(Vector3f &normal)
{
	float a = normal[0];
	float b = normal[1];
	float c = normal[2];

	float magnitude = sqrt(a*a + b*b + c*c);

	normal[0] = a/magnitude;
	normal[1] = b/magnitude;
	normal[2] = c/magnitude;
}

void triangleCenter(Vector3f &center, Vector3f vec1, Vector3f vec2, Vector3f vec3)
{
	float x = vec1[0] + vec2[0] + vec3[0];
	float y = vec1[1] + vec2[1] + vec3[1];
	float z = vec1[2] + vec2[2] + vec3[2];

	center[0] = x/3;
	center[1] = y/3;
	center[2] = z/3;
}

float dotProduct(Vector3f a, Vector3f b)
{
	float sum = 0;

	for (int i = 0; i < 3; i++)
	{
		sum = a[i] + b[i];
	}

	return sum;
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

		// Normalise the Normal
		normalise(triangleNormal);

		if (triangleNormal[2] < 0) continue;

		// Find the triangle center
		Vector3f center;
		triangleCenter(center, v1, v2, v3);

		float i_a = 0.8;
		float k_a = 0.8;
		float k_d = 0.8;
		float k_s = 0.1;
		int n = 1;

		Vector3f _L;
		vectorSubtract(_L, center, light);
		normalise(_L);

		Vector3f _V;
		vectorSubtract(_V, camera, triangleNormal);
		normalise(_V);

		Vector3f _R;
		float q = 2 * dotProduct(_L, triangleNormal);
		vectorSubtract(_R, triangleNormal, _L);
		_R[0] = q * _R[0];
		_R[1] = q * _R[1];
		_R[2] = q * _R[2];
		normalise(_R);

		//cout << dotProduct(_V, _R) << endl;

		float ambient = k_a * i_a; 
		float diffuse = k_d * dotProduct(_L, triangleNormal);
		float specular = pow(k_s * dotProduct(_V, _R), n);

		//if (specular > 0.01)
			//cout << "Ambient: " << ambient << endl << "Diffuse: " << diffuse << endl << "Specular: " << specular << endl << endl;

		float I = ambient + diffuse + specular;
		//float I = specular;

		glColor3f(I,I,I);
		
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
		//

		draw(v1, v2, v3);
	}

	glFlush();// Output everything
	for (int i = 0; i < nCols; i++)
	{
		for (int j = 0; j < nRows; j++)
		{
			zBuffer[i][j] = -99999.0f;
		}
	}
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


	glutInit(&argc, argv);
	glutInitWindowSize(nRows, nCols);
	glutCreateWindow("Model Viewer");
	gluOrtho2D(-nRows/2, nRows/2, -(float)nCols/2,  (float)nCols/2);
	glutDisplayFunc(display);// Callback function
	glutKeyboardUpFunc(keyboard);
	glutSpecialUpFunc(keyboardSpecial);
	glutMainLoop();// Display everything and wait
}
