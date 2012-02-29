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

// class Vector3f;
class Triangle;
class Model;
class Edge;
class TransformMatrix;

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

// ostream & operator << (ostream & stream, Vector3f & obj)
// {
// 	stream << obj[0] << ' ' << obj[1] << ' ' << obj[2] << ' ';
// 	return stream;
// };

class Triangle {
	
	friend class Model;

	int _id;
	int _vertex[3];
	int _normal[3];
	int _edge[3];
	float _min, _max;
	float _color;
	float _area;
	float _ratio;

public:

	Triangle(int v1, int v2, int v3, int n1, int n2, int n3)
	{
		_vertex[0] = v1;  _vertex[1] = v2;  _vertex[2] = v3;
		_normal[0] = n1;  _normal[1] = n2;  _normal[2] = n3;
	};

	void setMorseMinMax(float min, float max)
	{
		_min = min; _max = max;
	}

	void getMorseMinMax(float & min, float & max)
	{
		min = _min; max = _max;
	}

	void setEdge(int e1, int e2, int e3)
	{
		_edge[0] = e1;
		_edge[1] = e2;
		_edge[2] = e3;
	};

	int edge(int i) { return _edge[i];};
	int id() { return _id;};
	void setColor(float f) { _color = f ;};
	float color() { return _color;};
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

class TransformMatrix
{
	float _entries[4][4];

public:
	TransformMatrix() {
		blankOut();
		identity();
	}

	TransformMatrix(Vector3f translation, bool inverse) {
		if (inverse) translation *= -1;

		blankOut();
		identity();

		translate(translation);
	}

	void translate(Vector3f translation)
	{
		setCell(1,4,translation[0]);
		setCell(2,4,translation[1]);
		setCell(3,4,translation[2]);
	}

	float getCell(int n, int m) {
		return _entries[n-1][m-1];
	}

	void setCell(int n, int m, float value) {
		_entries[n-1][m-1] = value;
	}

	void blankOut() {
		for (int i = 1; i <= 4; ++i)
		{
			for (int j = 1; j <= 4; ++j)
			{
				setCell(i, j, 0.0f);
			}
		}
	}

	void identity() {
		for (int i = 1; i <= 4; ++i)
		{
			setCell(i, i, 1.0f);
		}
	}

	void rotateX(float angle) {
		blankOut();
		identity();

		setCell(2, 2, cos(angle));
		setCell(2, 3, -sin(angle));
		setCell(3, 2, sin(angle));
		setCell(3, 3, cos(angle));
	}

	void rotateY(float angle) {
		blankOut();
		identity();

		setCell(1, 1, cos(angle));
		setCell(1, 3, sin(angle));
		setCell(3, 1, -sin(angle));
		setCell(3, 3, cos(angle));
	}

	void rotateZ(float angle) {
		blankOut();
		identity();

		setCell(1, 1, cos(angle));
		setCell(1, 2, -sin(angle));
		setCell(2, 1, sin(angle));
		setCell(2, 2, cos(angle));
	}

	void printMatrix() {
		cout << "[";

		for (int i = 1; i <= 4; ++i)
		{
			for (int j = 1; j <= 4; ++j)
			{
				cout << getCell(i, j) << ", ";
			}

			cout << "]" << endl;
		}
	}

	TransformMatrix multiply(TransformMatrix other) {
		TransformMatrix newMatrix;

		for (int i = 1; i <= 4; ++i)
		{
			for (int j = 1; j <= 4; ++j)
			{
				newMatrix.setCell(i, j, getCell(i, j));
			}
		}

		for (int i = 1; i <= 4; ++i)
		{
			for (int j = 1; j <= 4; ++j)
			{
				float sum = 0.0f;

				for (int k = 1; k <= 4; ++k)
				{
					sum += getCell(i, k) * other.getCell(k, j);
				}

				newMatrix.setCell(i, j, sum);
			}
		}

		return newMatrix;
	}
};

// TRIANGLE MESH
class Model
{
	vector <Vector3f> _v;
	vector <Vector3f> _vn;
	vector <Triangle> _trig;
	vector <Node> _node;
	vector <Edge> _edge;

	float _xmax, _xmin, _ymax, _ymin, _zmin, _zmax;

	vector <Vector3f> _original;
	vector <Vector3f> _current;
	vector <int> _parent;

	vector < vector <float> > _weights;

	vector < TransformMatrix > _translationMatrix;
	vector < TransformMatrix > _rotationMatrix;

public:
	Model(char * filename) { loadModel(filename) ;};
	Model() {};
	void loadModel(char * filename);
	void loadWeights(char * filename);
	void loadSkeleton(char * filename);

	int trigNum() { return _trig.size() ;};
	int jointNum() { return _original.size() ;};
	int vertexNum() { return _v.size() ;};

	void getTriangleVertices(int i, Vector3f & v1, Vector3f & v2, Vector3f & v3)
	{
		v1 = _v[_trig[i]._vertex[0]];
		v2 = _v[_trig[i]._vertex[1]];
		v3 = _v[_trig[i]._vertex[2]];
	}

	void getOriginalJoint(int i, Vector3f &joint)
	{
		joint = _original[i];
	}

	void getCurrentJoint(int i, Vector3f &joint)
	{
		joint = _current[i];
	}

	void setCurrentJoint(int i, Vector3f &joint)
	{
		_current[i] = joint;
	}

	void getVertex(int i, Vector3f &vertex)
	{
		vertex = _v[i];
	}

	void setVertex(int i, Vector3f vertex)
	{
		_v[i][0] = vertex[0];
		_v[i][1] = vertex[1];
		_v[i][2] = vertex[2];
	}

	void rotateJointX(int jointIndex, float degrees) {
		_rotationMatrix[jointIndex].rotateX(degrees);
	}

	TransformMatrix getTransformMatrix(int i)
	{
		int parent;
		getJointParent(i, parent);

		TransformMatrix trans;

		while (parent != -1)
		{
			// cout << "Parent: " << parent << endl << endl;
			trans = trans.multiply(_rotationMatrix[parent]);
			if (parent != -1)
			{
				trans = trans.multiply(_translationMatrix[parent]);
			}
			getJointParent(parent, parent);
		}

		trans = trans.multiply(_rotationMatrix[i]);
		trans = trans.multiply(_translationMatrix[i]);

		return trans;
	}

	void getJointParent(int i, int &parent)
	{
		parent = _parent[i];
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

	vector<float> getWeights(int i)
	{
		return _weights[i];
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

	void setCurrentJointPos(int i, float x, float y, float z)
	{
		_current[i].move(x, y, z);
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

#endif //_rt_H
