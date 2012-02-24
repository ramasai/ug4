class Vector3f;

class Vector3f {

	float _item[3];

public:

	// Constructors
	Vector3f(float x, float y, float z) {
		_item[0] = x;
		_item[1] = y;
		_item[2] = z;
	};

	Vector3f() {};

	// Square Bracket Accessors
	float & operator [] (int i) {
		return _item[i];
	}

	float operator [] (int i) const {
		return _item[i];
	}

	// Operators
	Vector3f & operator = (Vector3f & obj) 
	{
		_item[0] = obj[0];
		_item[1] = obj[1];
		_item[2] = obj[2];

		return *this;
	};

	Vector3f & operator += (const Vector3f & obj) 
			{
		_item[0] += obj[0];
		_item[1] += obj[1];
		_item[2] += obj[2];

		return *this;
			};

	Vector3f & operator -= (const Vector3f & obj) 
			{
		_item[0] -= obj[0];
		_item[1] -= obj[1];
		_item[2] -= obj[2];

		return *this;
			};

	Vector3f & operator /= (const float & f) 
			{
		_item[0] /= f;
		_item[1] /= f;
		_item[2] /= f;

		return *this;
			};

	// Distance
	float distance(Vector3f & v) 
	{
		float ret=0.f;

		ret = (v[0]-_item[0])*(v[0]-_item[0]) 
            		+ (v[1]-_item[1])*(v[1]-_item[1])
            		+ (v[2]-_item[2])*(v[2]-_item[2]);

		ret = sqrt(ret);

		return ret;
	}

	// Dot Product
	float dot(const Vector3f & input) const
	{
		return input._item[0]*_item[0] + input._item[1]*_item[1] + input._item[2]*_item[2] ; 
	}

	// Normalise
	float normalize()
	{
		float ret = sqrt(_item[0]*_item[0] + _item[1]*_item[1] + _item[2]*_item[2]); 
		for (int i = 0; i < 3; i++) {
			_item[i] /= ret;
		}

		return ret;
	}

	friend Vector3f operator % ( const Vector3f & ob1, const Vector3f & ob2);
	friend Vector3f operator - ( const Vector3f & ob1, const Vector3f & ob2);
	friend Vector3f operator + ( const Vector3f & ob1, const Vector3f & ob2);
};

// Multiple Operators
Vector3f operator % ( const Vector3f & ob1, const Vector3f & ob2) {
	int i;              // loop counter
	Vector3f ret;            // return value
	int N_DV3 = 3;

	for (i = 0; i < N_DV3; i++)
	{
		ret._item[i] =
				ob1[(i + 1) % N_DV3] *
				ob2[(i + 2) % N_DV3] -
				ob2[(i + 1) % N_DV3] *
				ob1[(i + 2) % N_DV3];
	}

	return ret;
}

Vector3f operator + (const Vector3f& v1, const Vector3f& v2)
{
	Vector3f v(v1);
	for (int i = 0; i < 3; i++)
		v[i] += v2[i];

	return v;
}

Vector3f operator - (const Vector3f& v1, const Vector3f& v2)
{

	Vector3f v(v1);
	for (int i = 0; i < 3 ; i++)
		v[i] -= v2[i];

	return v;
}

float distance(Vector3f & v1, Vector3f & v2)
{

	float ret;

	ret = sqrt((v2[0]-v1[0])*(v2[0]-v1[0]) + (v2[1]-v1[1])*(v2[1]-v1[1]) + (v2[2]-v1[2])*(v2[2]-v1[2]));

	return ret;
}

std::ostream & operator << (std::ostream & stream, Vector3f & obj)
{
	stream << obj[0] << ' ' << obj[1] << ' ' << obj[2] << ' ';
	return stream;
};
