#ifndef _rt_H
#define _rt_H

#include <cmath>
#include <vector>

#include <GL/glut.h>

using namespace std;

class Vector3f;
class Triangle;
class TriangleMesh;

class Vector3f {

	float _item[3];

	public:

	float & operator [] (int i) {
		return _item[i];
	}

	Vector3f(float x, float y, float z) {
		_item[0] = x ; _item[1] = y ; _item[2] = z;
	};

	Vector3f() {};

	Vector3f & operator = (Vector3f & obj) 
	{
		_item[0] = obj[0];
		_item[1] = obj[1];
		_item[2] = obj[2];

		return *this;
	};

	Vector3f & operator += (Vector3f & obj) 
	{
		_item[0] += obj[0];
		_item[1] += obj[1];
		_item[2] += obj[2];

		return *this;
	};

	float normalize() {
		float ret = sqrt(_item[0]*_item[0] + _item[1]*_item[1] + _item[2]*_item[2]); 

		for (int i = 0; i < 3; i++) _item[i]/=ret;

		return ret;
	}
};

ostream & operator << (ostream & stream, Vector3f & obj) 
{
	stream << obj[0] << ' ' << obj[1] << ' ' << obj[2] << ' ';
};

float operator * (Vector3f & v1, Vector3f & v2)  
{
	return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2]; 
};

class Volume {

	private:
		int _size[3];
		unsigned short **_volume ; 

	public:
		void rasterize(int x, int y, int z);

		void load(char * filename);

		unsigned short volume(int x, int y, int z) {
			return _volume[y][z * _size[2] + x];
		}; 

		int sizex() {
			return _size[0];
		};

		int sizey() {
			return _size[1];
		};

		int sizez() {
			return _size[2];
		};
};

/**
 * Max/min functions
 */

float fmax(float f1,float f2, float f3) {
	float f = f1;

	if (f < f2) f = f2;
	if (f < f3) f = f3;

	return f;
};

float fmin(float f1,float f2, float f3) {
	float f = f1;

	if (f > f2) f = f2;
	if (f > f3) f = f3;

	return f;
};

#endif
