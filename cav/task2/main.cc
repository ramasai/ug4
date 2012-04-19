#include <iostream>
#include <fstream>

#include <GL/glut.h>

#include "main.h"

int nRows = 480;
int nCols = 480; 

Volume vol;

#define SKIN_VAL 0.2f
#define BONE_VAL 0.5f

float tmpf = SKIN_VAL;

void Volume::load(char * filename)
{  
	int   temp;
	int   data_compression;
	int   plane_size;
	int   volume_size;
	double f[3];
	int   size[3];
	int   magic_num;
	int   y_counter;
	int   icon_width, icon_height;
	int   compressed_size;

	unsigned char *compressed_ptr;
	unsigned char *scan_ptr = NULL;

	ifstream fin;

	fin.open(filename);

	// Initialize
	if (fin == NULL)
	{
		cerr << filename << " cannot be found " << endl;
		return;
	}

	fin >> size[2] >> size[0] >> size[1];

	plane_size = size[2] * size[0];
	volume_size = plane_size * size[1];

	_volume = new unsigned short * [size[1]]; 
	for (int i = 0; i < size[1];i++) 
	{
		_volume[i] = new unsigned short[plane_size]; 
	}

	// Read In Data Plane By Plane
	for( y_counter=0; y_counter<size[1]; y_counter++ ) 
	{
		for (int i = 0; i < size[2]; i++) 
		{
			for (int j = 0; j < size[0]; j++) {
				_volume[size[1]-1 - y_counter][i*size[2] + j]=fin.get();
				fin >>_volume[size[1]-1 - y_counter][i*size[2] + j];
			}
		}
	}

	_size[0] = size[0];
	_size[1] = size[1];
	_size[2] = size[2];

	cerr <<"Read " << volume_size << " points." << endl;
}

void drawBox(int x, int y, float r, float b, float g) {
	glColor3f(r, g, b);

	glRecti(x - vol.sizex() / 2,
			2 * y -vol.sizey(),
			x - vol.sizex() / 2 + 1,
			2 * y - vol.sizey() + 2);
}

void drawBox(int x, int y, float color) {
	drawBox(x, y, color, color, color);
}

void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);

	Vector3f v1,v2,v3;
	Vector3f N;

	// if you change this value and you will see different contours  
	int skin_threshold = SKIN_VAL * 255.0f;
	int bone_threshold = BONE_VAL * 255.0f;

	for (int xi = 1 ; xi < vol.sizex()-1 ; xi++)  
	{
		for (int yi = 1 ; yi < vol.sizey()-1 ; yi++)  
		{
			float r_i = 0.0f;
			float g_i = 0.0f;
			float b_i = 0.0f;

			for (int zi = vol.sizez()-2; zi >= 0; zi--) 
			{
				int volume = vol.volume(xi, yi, zi);
				bool is_skin = volume > skin_threshold;
				bool is_bone = volume > bone_threshold;

				float a = 0.0f;
				float e = 0.0f;

				if (is_bone) {
					e = (float)volume / 255.f;
					a = 1.0f;
				} else if (is_skin) {
					e = (float)volume / 255.f;
					a = 0.03f;
				}

				// i = contents of the frame buffer
				// e = light from the cell
				// a = object opacity
				r_i = (a * e) + (1-a) * r_i;
				g_i = (a * e) + (1-a) * g_i;
				b_i = (a * e) + (1-a) * b_i;
			}	

			drawBox(xi, yi, r_i, g_i, b_i);
		}
	}

	glFlush();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 32:
			if (tmpf == BONE_VAL) {
				tmpf = SKIN_VAL;
			} else {
				tmpf = BONE_VAL;
			}
			break;
	}

	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	if (argc >  1)  {
		vol.load(argv[1]);
	}
	else {
		cerr << argv[0] << " <filename> " << endl;
		exit(1);
	}

	int width, height;

	glutInit(&argc, argv);
	glutInitWindowSize(nRows, nCols);
	glutCreateWindow("Volume Rendering - 0824586");

	gluOrtho2D(-nRows/2, nRows/2, -(float)nCols/2,  (float)nCols/2);
	glutDisplayFunc(myDisplay);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
};
