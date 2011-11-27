/**
 * Not implemented:
 *
 *  - Antialiasing
 *  - Perspective transformation
 */
#include <iostream>
#include <algorithm>
#include <fstream>
#include <GLUT/glut.h>
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

Vector3f camera(0.0f, 0.0f, 50.0f);
Vector3f light(500.0f, 500.0f, 10000.0f);

TriangleMesh trig;
map <int, vector<Triangle> > reverse_index;

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
            
            reverse_index[v1-1].push_back(trig);
            reverse_index[v2-1].push_back(trig);
            reverse_index[v3-1].push_back(trig);
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

void vertexNormal(Vector3f &normal, Vector3f vertex)
{
    int j = 0;
    
    for(std::vector<Vector3f>::iterator it = trig._v.begin(); it != trig._v.end(); ++it)
    {
        Vector3f vec = trig._v.at(j);
        
        if (vec == vertex)
        {
            break;
        }
        j++;
    }
    
    vector<Triangle> v(reverse_index[j]);
            
    float vecNormalX = 0;
    float vecNormalY = 0;
    float vecNormalZ = 0;
    int vecTrig = 0;
    
    for(std::vector<Triangle>::iterator it = v.begin(); it != v.end(); ++it) {
        Triangle tri = *it;
        
        Vector3f v1(trig._v[tri._vertex[0]][0], trig._v[tri._vertex[0]][1], trig._v[tri._vertex[0]][2]);
        Vector3f v2(trig._v[tri._vertex[1]][0], trig._v[tri._vertex[1]][1], trig._v[tri._vertex[1]][2]);
        Vector3f v3(trig._v[tri._vertex[2]][0], trig._v[tri._vertex[2]][1], trig._v[tri._vertex[2]][2]);
        
        Vector3f fnormal;
        faceNormal(fnormal, v1, v2, v3);
        
        vecTrig++;
        vecNormalX += fnormal[0];
        vecNormalY += fnormal[1];
        vecNormalZ += fnormal[2];
    }
    
    normal[0] = vecNormalX/vecTrig;
    normal[1] = vecNormalY/vecTrig;
    normal[2] = vecNormalZ/vecTrig;
        
    normalise(normal);
}

void draw(int index, Vector3f vec1, Vector3f vec2, Vector3f vec3)
{
    Vector3f vecs[3] = {vec1, vec2, vec3};
    Vector3f vecNormals[3];

	float min_x = fmin(vec1[0], vec2[0], vec3[0]);
	float max_x = fmax(vec1[0], vec2[0], vec3[0]);
	float min_y = fmin(vec1[1], vec2[1], vec3[1]);
	float max_y = fmax(vec1[1], vec2[1], vec3[1]);

	//cout << "X: " << min_x << "," << max_x << endl;
    
    float I_a = 0.2;
    float k_d = 0.1;
    float k_s = 0.9;
    int n = 2;
    
    Vector3f _L[3];
    Vector3f _V[3];
    Vector3f _R[3];
    
    float I[3];

    for (int i = 0; i < 3; i++) {
        vertexNormal(vecNormals[i], vecs[i]);
        
        vectorSubtract(_L[i], light, vecs[i]);
        normalise(_L[i]);
        
        vectorSubtract(_V[i], camera, vecNormals[i]);
        normalise(_V[i]);
        
        float q = 2 * dotProduct(_L[i], vecNormals[i]);
        vectorSubtract(_R[i], vecNormals[i], _L[i]);
        _R[i][0] = q * _R[i][0];
        _R[i][1] = q * _R[i][1];
        _R[i][2] = q * _R[i][2];
        normalise(_R[i]);
        
        float ambient = I_a;
        float diffuse = k_d * dotProduct(_L[i], vecNormals[i]);
        float specular = pow(k_s * dotProduct(_V[i], _R[i]), n);
        
        I[i] = ambient + diffuse + specular;
        
//        cout << "Ambient: " << ambient << endl << "Diffuse: " << diffuse << endl << "Specular: " << specular << endl << endl;
    }

	for (int x = min_x; x < max_x; x++)
	{
		for (int y = min_y; y < max_y; y++)
		{
//			float alpha = f(1,2,x,y,vec1,vec2,vec3) / f(1,2,vec1[0],vec1[1],vec1,vec2,vec3);
//			float beta  = f(2,0,x,y,vec1,vec2,vec3) / f(2,0,vec2[0],vec2[1],vec1,vec2,vec3);
//			float gamma = f(0,1,x,y,vec1,vec2,vec3) / f(0,1,vec3[0],vec3[1],vec1,vec2,vec3);
            
            float alpha_num = ((vec2[1] - vec3[1]) * (x - vec3[0])) + ((vec3[0] - vec2[0]) * (y - vec3[1]));
            float alpha_dem = ((vec2[1] - vec3[1]) * (vec1[0] - vec3[0])) + ((vec3[0] - vec2[0]) * (vec1[1] - vec3[1]));
            float alpha = alpha_num/alpha_dem;
            
            float beta_num = ((vec3[1] - vec1[1]) * (x - vec3[0])) + ((vec1[0] - vec3[0]) * (y - vec3[1]));
            float beta_dem = ((vec2[1] - vec3[1]) * (vec1[0] - vec3[0])) + ((vec3[0] - vec2[0]) * (vec1[1]-vec3[1]));
            float beta = beta_num/beta_dem;

            float gamma = 1 - alpha - beta;
			float z = (alpha * vec1[2] + beta * vec2[2] + gamma * vec3[2]);

			if (alpha > 0 && beta > 0 && gamma > 0 && zBuffer[x + nCols/2][y + nRows/2] < z)
			{
//                cout << "Sum is " << sum << endl;
//                cout << "Alpha: " << alpha << endl << "Beta: " << beta << endl << "Gamma: " << gamma << endl << endl;

                float ill = (alpha * I[0] + beta * I[1] + gamma * I[2]);
                glColor3f(ill * 0.905882, ill * 0.419246, ill * 0.219607);
                
				draw_pixel(x,y);
				zBuffer[x + nCols/2][y + nRows/2] = z;
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
	glColor3f(0,0,0);

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

		draw(i, v1, v2, v3);
	}

	glFlush();// Output everything
    
//	for (int i = 0; i < nCols; i++)
//	{
//		for (int j = 0; j < nRows; j++)
//		{
//			zBuffer[i][j] = -99999.0f;
//		}
//	}
}

void keyboardSpecial(int key, int x, int y)
{
	switch (key)
	{
		case GLUT_KEY_UP:
			rotation_x += 0.30;
			break;
		case GLUT_KEY_LEFT:
			rotation_y += 0.30;
			break;
		case GLUT_KEY_DOWN:
			rotation_x -= 0.30;
			break;
		case GLUT_KEY_RIGHT:
			rotation_y -= 0.30;
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
