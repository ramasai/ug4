#ifndef _rt_H
#define _rt_H

#include <cmath>
#include <map>
#include <vector>
#include <cstring>

#ifdef __APPLE__
    #include <GLUT/GLUT.h>
#else
    #include <GL/glut.h>
#endif

using namespace std;

#include "triangle.h"
#include "matrix4f.h"

class Model;
class Edge;

// class Vector3f {

// 	float _item[3];

// 	public:

// 	float & operator [] (int i) {
// 		return _item[i];
// 	}

// 	float operator [] (int i) const {
// 		return _item[i];
// 	}

// 	Vector3f(float x, float y, float z) {
// 		_item[0] = x;
// 		_item[1] = y;
// 		_item[2] = z;
// 	};

// 	Vector3f() {};

// 	Vector3f & operator = (const Vector3f & obj)
// 	{
// 		_item[0] = obj[0];
// 		_item[1] = obj[1];
// 		_item[2] = obj[2];

// 		return *this;
// 	};

// 	Vector3f & operator += (const Vector3f & obj)
// 	{
// 		_item[0] += obj[0];
// 		_item[1] += obj[1];
// 		_item[2] += obj[2];

// 		return *this;
// 	};

// 	Vector3f & operator -= (const Vector3f & obj)
// 	{
// 		_item[0] -= obj[0];
// 		_item[1] -= obj[1];
// 		_item[2] -= obj[2];

// 		return *this;
// 	};

// 	Vector3f & move(float x, float y, float z)
// 	{
// 		_item[0] += x;
// 		_item[1] += y;
// 		_item[2] += z;
// 	}

// 	Vector3f & operator /= (const float & f)
// 	{
// 		_item[0] /= f;
// 		_item[1] /= f;
// 		_item[2] /= f;

// 		return *this;
// 	};

	// Vector3f & operator *= (const float & f)
	// {
	// 	_item[0] *= f;
	// 	_item[1] *= f;
	// 	_item[2] *= f;

	// 	return *this;
	// };

// 	float distance(Vector3f & v)
// 	{
// 		float ret=0.f;

// 		ret = (v[0]-_item[0])*(v[0]-_item[0])
// 		    + (v[1]-_item[1])*(v[1]-_item[1])
// 		    + (v[2]-_item[2])*(v[2]-_item[2]);

// 		ret = sqrt(ret);

// 		return ret;
// 	}

// 	float dot(const Vector3f & input) const
// 	{
// 		return input._item[0]*_item[0] + input._item[1]*_item[1] + input._item[2]*_item[2] ;
// 	}

// 	float normalize()
// 	{
// 		float ret = sqrt(_item[0]*_item[0] + _item[1]*_item[1] + _item[2]*_item[2]);
// 		for (int i = 0; i < 3; i++) _item[i] /= ret;

// 		return ret;
// 	}

// 	friend Vector3f operator % ( const Vector3f & ob1, const Vector3f & ob2);
// 	friend Vector3f operator - ( const Vector3f & ob1, const Vector3f & ob2);
// 	friend Vector3f operator + ( const Vector3f & ob1, const Vector3f & ob2);
// 	friend Vector3f operator * ( const Vector3f & ob1, const Vector3f & ob2);
// };

// Vector3f operator % ( const Vector3f & ob1, const Vector3f & ob2)
// {
//     int i;
//     Vector3f ret;
//     int N_DV3 = 3;

//     for (i = 0; i < N_DV3; i++)
// 	ret._item[ i ] = ob1[ ( i + 1 ) % N_DV3 ]
// 		* ob2[ ( i + 2 ) % N_DV3 ]
// 	    - ob2[ ( i + 1 ) % N_DV3 ]
// 		* ob1[ ( i + 2 ) % N_DV3 ];

//     return ret;
// }

// Vector3f operator + (const Vector3f& v1, const Vector3f& v2)
// {
//     Vector3f v(v1);
//     for (int i = 0; i < 3; i++)
// 	v[i] += v2[i];

//     return v;
// }

// Vector3f operator - (const Vector3f& v1, const Vector3f& v2)
// {
//     Vector3f v(v1);
//     for (int i = 0; i < 3 ; i++)
// 	v[i] -= v2[i];

//     return v;
// }

// Vector3f operator * (const Vector3f& v1, const Vector3f& v2)
// {
//     Vector3f v(v1);
//     for (int i = 0; i < 3 ; i++)
// 	v[i] *= v2[i];

//     return v;
// }

// float distance(Vector3f & v1, Vector3f & v2)
// {
// 	float ret;

// 	ret = sqrt((v2[0]-v1[0])*(v2[0]-v1[0]) + (v2[1]-v1[1])*(v2[1]-v1[1]) + (v2[2]-v1[2])*(v2[2]-v1[2]));

// 	return ret;
// }

ostream & operator << (ostream & stream, Vector3f & obj)
{
	stream << obj[0] << ' ' << obj[1] << ' ' << obj[2] << ' ';
	return stream;
};

ostream & operator << (ostream & stream, Matrix4f & obj) 
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			stream << obj(i,j) << ' ';
		}
		stream << '\n';
	}
};

class Edge {

	friend bool contain(Edge & e, map < pair <int, int> , Edge > & list) ;
	friend int edgeID(Edge & e, map < pair <int, int> , Edge > & list) ;
	friend bool contain(Edge & e, vector < Edge > & list) ;
	friend int edgeID(Edge & e, vector < Edge > & list) ;

	int _v1,_v2;
	vector <int> _trig_list;
	float length;
	int _id;

public:

	Edge () {};

	Edge (int i, int j)
	{
		_v1 = i; _v2 = j;
	};

	bool operator == (Edge & e)
	{
		if (this->_v1 == e._v1 && this->_v2 == e._v2) return true;
		if (this->_v1 == e._v2 && this->_v2 == e._v1) return true;

		return false;
	}

	void add_triangle(int trig)
	{
		for (int i = 0; i < _trig_list.size(); i++)
			if (trig == _trig_list[i]) return;

		_trig_list.push_back(trig);
	}

	void other_trig(const int trig, vector <int> & others)
	{
		for (int i = 0; i < _trig_list.size(); i++) {
			if (_trig_list[i] == trig) continue;
			else others.push_back(_trig_list[i]);
		}
	}

	void setId(int id) { _id = id;};
	int id() { return _id;};
	int v1() { return _v1;};
	int v2() { return _v2;};
	vector<int> getTrigList() { return _trig_list ;};
};

struct Node {
	int _id;

	vector<int> edges_to;
	vector<float> edges_cost;

	bool done;
	float cost;
};

// FLOAT ORDERING
float fmax(float f1, float f2, float f3) {
	float f = f1;

	if (f < f2) f = f2;
	if (f < f3) f = f3;

	return f;
};

float fmin(float f1, float f2, float f3) {
	float f = f1;

	if (f > f2) f = f2;
	if (f > f3) f = f3;

	return f;
};

// TRIANGLE MESH
class Model
{
	vector <Vector3f> _v;
	vector <Vector3f> _vo;
	vector <Vector3f> _vn;
	vector <Triangle> _trig;
	vector <Node> _node;
	vector <Edge> _edge;

	float _xmax, _xmin, _ymax, _ymin, _zmin, _zmax;

public:
	Model(char * filename) { loadModel(filename) ;};
	Model() {};
	void loadModel(char * filename);

	int trigNum() { return _trig.size() ;};
	int vertexNum() { return _v.size() ;};

	void getVertex(int index, Vector3f &vec) {
		vec = _v[index];
	}

	void getOriginalVertex(int index, Vector3f &vec) {
		vec = _v[index];
	}

	void setVertex(int index, Vector3f vec) {
		_v[index] = vec;
	}

	void getTriangleVertices(int i, Vector3f & v1, Vector3f & v2, Vector3f & v3)
	{
		v1 = _v[_trig[i]._vertex[0]];
		v2 = _v[_trig[i]._vertex[1]];
		v3 = _v[_trig[i]._vertex[2]];
	}

	void getTriangleNormals(int i, Vector3f & v1, Vector3f & v2, Vector3f & v3)
	{
		v1 = _vn[_trig[i]._normal[0]];
		v2 = _vn[_trig[i]._normal[1]];
		v3 = _vn[_trig[i]._normal[2]];
	}

	void getMorseValue(int i, float & v1, float & v2, float & v3)
	{
		v1 = _node[_trig[i]._vertex[0]].cost;
		v2 = _node[_trig[i]._vertex[1]].cost;
		v3 = _node[_trig[i]._vertex[2]].cost;
	}

	float color(int i) { return _trig[i].color();};

	void setMorseMinMax(int i, float min, float max)
	{
		_trig[i].setMorseMinMax(min,max);
	}

	void getMorseMinMax(int i, float & min, float & max)
	{
		_trig[i].getMorseMinMax(min,max);
	}

	void calcTriangleArea()
	{
		Vector3f v1,v2,v3;

		for (int i = 0 ;i < _trig.size(); i++)
		{
			getTriangleVertices(i, v1,v2,v3);
			v3 -= v1;
			v2 -= v1;

			_trig[i]._area = 0.5f*sqrt(v3.dot(v3)*v2.dot(v2) - (v3.dot(v2)*(v3.dot(v2))));
		}
	}
};

class Skeleton {
	// Joints
	vector <Vector3f> _originalJoints;
	vector <Vector3f> _currentJoints;
	vector <int> _parents;

	// Weights
	vector < vector <float> > _weights;

	// Transformation Matrices
	vector < Matrix4f > _originalRotationMatrix;
	vector < Matrix4f > _currentRotationMatrix;

	vector < Matrix4f > _originalTranslationMatrix;
	vector < Matrix4f > _currentTranslationMatrix;

public:
	void loadSkeleton(char * filename);
	void loadWeights(char * filename);
	int jointNum() { return _originalJoints.size() ;}

	void getCurrentJoint(int index, Vector3f &vec) {
		vec = _currentJoints[index];
	}

	void setCurrentJoint(int index, Vector3f vec) {
		_currentJoints[index] = vec;
	}

	void getOriginalJoint(int index, Vector3f &vec) {
		vec = _originalJoints[index];
	}

	int getJointParent(int index) {
		return _parents[index];
	}

	vector < float > getWeights(int index) {
		return _weights[index];
	}

	Vector3f getRootOffset() {
		return _originalJoints[0];
	}

	Vector3f getLocalCoords(int index) {
		Matrix4f translationMatrix = _originalTranslationMatrix[index];
		Vector3f localCoords(translationMatrix(0,3),
			translationMatrix(1,3),
			translationMatrix(2,3));

		return localCoords;
	}

	Vector3f getRecursiveCoords(int index) {
		if (index == 0)
		{
			return Vector3f(0,0,0);
		}

		Vector3f ret;

		int parentIndex = getJointParent(index);
		ret += getLocalCoords(parentIndex);
		ret += getRecursiveCoords(parentIndex);

		return ret;
	}

	Matrix4f getCurrentTransformMatrix(int index, int orig) {
		Matrix4f matrix;
		matrix.setIdentity();

		int parentIndex = getJointParent(index);

		Matrix4f parentMatrix;

		if (parentIndex != -1) {
			parentMatrix = getCurrentTransformMatrix(parentIndex, orig);
		} else {
			parentMatrix.setIdentity();
		}

		matrix *= parentMatrix;
		matrix *= _currentRotationMatrix[index];

		if (index != orig) {
			matrix *= _currentTranslationMatrix[index];
		}

		return matrix;
	}

	Matrix4f getOriginalTransformMatrix(int index, int orig) {
		Matrix4f matrix;
		matrix.setIdentity();

		int parentIndex = getJointParent(index);

		Matrix4f parentMatrix;

		if (parentIndex != -1) {
			parentMatrix = getOriginalTransformMatrix(parentIndex, orig);
		} else {
			parentMatrix.setIdentity();
		}

		matrix *= parentMatrix;
		matrix *= _originalRotationMatrix[index];

		if (index != orig) {
			matrix *= _originalTranslationMatrix[index];
		}

		return matrix;
	}

	void rotateJointX(int index, float rotation)
	{
		_currentRotationMatrix[index] = rotX(rotation);
	}
};

#endif //_rt_H
