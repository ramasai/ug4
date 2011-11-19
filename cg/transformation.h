/**
 * Transforms a vector using the transformation matrix (a-p)
 */
void transform(Vector3f &v,
		float a, float b, float c, float d,
		float e, float f, float g, float h,
		float i, float j, float k, float l,
		float m, float n, float o, float p)
{
	float v1 = v[0];
	float v2 = v[1];
	float v3 = v[2];
	float v4 = 1.0f;

	float ov1 = (v1*a) + (v2*e) + (v3*i) + (v4*m);
	float ov2 = (v1*b) + (v2*f) + (v3*j) + (v4*n);
	float ov3 = (v1*c) + (v2*g) + (v3*k) + (v4*o);
	float ov4 = (v1*d) + (v2*h) + (v3*l) + (v4*p);

	v[0] = ov1/ov4;
	v[1] = ov2/ov4;
	v[2] = ov3/ov4;
}

void translate_vector(Vector3f &v, float x, float y, float z)
{
	transform(v,
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			x,y,z,1);
}

void scale_vector(Vector3f &v, float x, float y, float z)
{
	transform(v,
			x,0,0,0,
			0,y,0,0,
			0,0,z,0,
			0,0,0,1);
}

void rotate_vector_x(Vector3f &v, float d)
{
	transform(v,
			1,     0,      0,0,
			0,cos(d),-sin(d),0,
			0,sin(d), cos(d),0,
			0,     0,      0,1);
}

void rotate_vector_y(Vector3f &v, float d)
{
	transform(v,
			cos(d) ,0,sin(d),0,
			0      ,1,     0,0,
			-sin(d),0,cos(d),0,
			0      ,0,     0,1);
}

void rotate_vector_z(Vector3f &v, float d)
{
	transform(v,
			cos(d),-sin(d),0,0,
			sin(d),cos(d) ,0,0,
			0     ,      0,1,0,
			0     ,      0,0,1);
}
