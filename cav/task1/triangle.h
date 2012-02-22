#ifndef TRIANGLE1_H_
#define TRIANGLE1_H_

class Triangle {

	friend class TriangleMesh;

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
		_vertex[0] = v1;
		_vertex[1] = v2;
		_vertex[2] = v3;

		_normal[0] = n1;
		_normal[1] = n2;
		_normal[2] = n3;
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

#endif /* TRIANGLE1_H_ */
