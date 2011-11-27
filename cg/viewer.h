#ifndef _rt_H
#define _rt_H

#include <cmath>
#include <vector>
#include <GLUT/glut.h>

#include <map>

using namespace std;

//
// Sample code for physics simulation
//

class Vector3f;
class Triangle;
class TriangleMesh;
class TriangleComparer;

class Vector3f {

	float _item[3];

	public:

	float & operator [] (int i) {
		return _item[i];
    	}

	Vector3f(float x, float y, float z) 
	{  _item[0] = x ; _item[1] = y ; _item[2] = z; };

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
    
    bool operator==(const Vector3f & other) const
    {
        return _item[0] == other._item[0] &&
            _item[1] == other._item[1] &&
        _item[2] == other._item[2];
    }
    
    bool operator<(const Vector3f &other) const
    {
        if (_item[0] < other._item[0])
        {
            return true;
        }
        else if (_item[0] > other._item[0])
        {
            return false;
        }
        else
        {
            if (_item[1] < other._item[1])
            {
                return true;
            }
            else if (_item[1] > other._item[1])
            {
                return false;
            }
            else
            {
                if (_item[2] < other._item[2])
                {
                    return true;
                }
                else if (_item[2] > other._item[2])
                {
                    return false;
                }
                else
                {
                    return false;
                }
            }
        }
    }
};

ostream & operator << (ostream & stream, Vector3f & obj) 
{
	return stream << '(' << obj[0] << ',' << obj[1] << ',' << obj[2] << ')';
};

class Triangle {
friend class TriangleMesh;

public:
	int _vertex[3];
    
	Triangle(int v1, int v2, int v3) 
	{
		_vertex[0] = v1;  _vertex[1] = v2;  _vertex[2] = v3;  
		
	};  
};

//class VectorComparer : public std::binary_function<Vector3f, Vector3f, bool>
//{
//public:
//    bool operator () (Vector3f &vec1, 
//                      Vector3f &vec2) const
//    {
//        return vec1.operator<(vec2);
//    }
//};

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


class TriangleMesh 
{
public: 
	vector <Vector3f> _v;
	vector <Triangle> _trig;

	float _xmax, _xmin, _ymax, _ymin, _zmin, _zmax;

	TriangleMesh(char * filename) { loadFile(filename) ;};
	TriangleMesh() {};
	void loadFile(char * filename);

	int trigNum() { return _trig.size() ;};
	int vNum() { return _v.size();};
	Vector3f v(int i) { return _v[i];};

	void getTriangleVertices(int i, Vector3f & v1, Vector3f & v2, Vector3f & v3)
	{
		v1 = _v[_trig[i]._vertex[0]]; 
		v2 = _v[_trig[i]._vertex[1]]; 
		v3 = _v[_trig[i]._vertex[2]]; 
	}
			
};

#endif //_rt_H
