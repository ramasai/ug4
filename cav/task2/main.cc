#include <iostream>
#include <fstream>

#include <GL/glut.h>

#include "main.h"

int nRows = 480;
int nCols = 480; 
Volume vol;

static int spin = 0;
static int spin2 = 0;
static int begin;
static int begin2;

int button_id; 
int state_id;

Vector3f L(0,0,1);
Vector3f V(0,0,1);
float tmpf = 0.2;


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

void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);

	Vector3f v1,v2,v3;

	Vector3f N;
	// if you change this value and you will see different contours  
	int threshold = tmpf * 270.0f;

	for (int xi = 1 ; xi < vol.sizex()-1 ; xi++)  
	{
		for (int yi = 1 ; yi < vol.sizey()-1 ; yi++)  
		{
			//cerr << "(x, y): ("<< xi << ", " << yi << ")" << endl; 

			for (int zi = 1 ; zi < vol.sizez()-1 ; zi++) 
			{
				if (vol.volume(xi,yi,zi) > threshold) 
				{
					float colour = (float)vol.volume(xi, yi, zi) / 255.f;
					glColor3f(colour, colour, colour);

					glRecti(xi - vol.sizex() / 2,
							2 * yi -vol.sizey(),
							xi - vol.sizex() / 2 + 1,
							2 * yi - vol.sizey() + 2);

					break;
				}
			}	
		}
	}

	glFlush();
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
	glutMainLoop();
};
