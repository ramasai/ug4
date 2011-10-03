#include <iostream>
#include <fstream>
#include <GL/glut.h>
#include "demo1.h"
#include <cstring>

using namespace std;

int nRows = 800;
int nCols = 600; 

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

void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT); // Clear OpenGL Window

	/** drawing a line for test **/

	/*** clear the Zbuffer here ****/

	int trignum = trig.trigNum();
	Vector3f v1,v2, v3;

	glColor3f(1,1,1);  // change the colour of the pixel

	//
	// for all the triangles, get the location of the vertices,
	// project them on the xy plane, and color the corresponding pixel by white
	//

	for (int i = 0 ; i < trignum; i++)  
	{
		/*** do the rasterization of the triangles here using glRecti ***/
		trig.getTriangleVertices(i, v1,v2,v3);

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
		dda_line((int)v3[0],(int)v3[1], (int)v2[0], (int)v1[1]);
		*/

		midpoint_line((int)v1[0],(int)v1[1], (int)v2[0], (int)v2[1]);
		midpoint_line((int)v2[0],(int)v2[1], (int)v3[0], (int)v3[1]);
		midpoint_line((int)v3[0],(int)v3[1], (int)v2[0], (int)v1[1]);
	}

	glFlush();// Output everything
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
	glutCreateWindow("SimpleExample");
	gluOrtho2D(-nRows/2, nRows/2, -(float)nCols/2,  (float)nCols/2);
	glutDisplayFunc(myDisplay);// Callback function
	glutMainLoop();// Display everything and wait
}
