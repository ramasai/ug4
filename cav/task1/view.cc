#include <iostream>
#include <fstream>
#include <map>
#include <set>

#ifdef __APPLE__
    #include <GLUT/GLUT.h>
#else
    #include <GL/glut.h>
#endif

#include "view.h"

#define NROWS 480
#define NCOLS 480

GLdouble bodyWidth = 1.0;
GLfloat angle = -150;   /* in degrees */
GLfloat xloc = 0, yloc = 0, zloc = 0;

int moving, begin;

int newModel = 1;

GLfloat light_ambient[] = {0.5, 0.5, 0.5, 1.0};  /* Red diffuse light. */
GLfloat light_diffuse[] = {0.8, 0.8, 0.8, 1.0};  /* Red diffuse light. */
GLfloat light_specular[] = {0.8, 0.8, 0.8, 1.0};  /* Red diffuse light. */
GLfloat light_position[] = {0.0, 0.0, 1.0, 0.0};  /* Infinite light location. */

static float modelAmb[4] = {0.2, 0.2, 0.2, 1.0};
static float matAmb[4] = {0.2, 0.2, 0.2, 1.0};
static float matDiff[4] = {0.8, 0.8, 0.8, 1.0};
static float matSpec[4] = {0.4, 0.4, 0.4, 1.0};
static float matEmission[4] = {0.0, 0.0, 0.0, 1.0};

static float modelAmb2[4] = {0.5, 0.5, 0.5, 1.0};
static float matAmb2[4] = {0.5, 0.5, 0.5, 1.0};
static float matDiff2[4] = {0.8, 0., 0., 1.0};
static float matSpec2[4] = {0.4, 0., 0., 1.0};
static float matEmission2[4] = {0.0, 0.0, 0.0, 1.0};

Model trig;

void update()
{
    for (int ji = 0; ji < trig.jointNum(); ji++)
    {
        // For each joint calculate new position.
        Vector3f current;
        TransformMatrix mat = trig.getTransformMatrix(ji);
        Vector3f newCoord(-mat.getCell(1,4),
                          -mat.getCell(2,4),
                          -mat.getCell(3,4));
        trig.setCurrentJoint(ji, newCoord);
    }

    // Linear Blending
	for (int vi = 0; vi < trig.vertexNum(); vi++)
	{
        Vector3f vertex;
        trig.getVertex(vi, vertex);

        vector <float> weights = trig.getWeights(vi);

        Vector3f sum(0,0,0);

		for (int bi = 0; bi < trig.jointNum(); bi++)
		{
            Vector3f original;
            trig.getOriginalJoint(bi, original);

            Vector3f current;
            trig.getCurrentJoint(bi, current);

            Vector3f newPos = vertex - original + current;

            newPos *= weights[bi];

            sum = sum + newPos;
		}

        trig.setVertex(vi, sum);
	}

    glutPostRedisplay();
}

float rot = 0.0f;

void keyboard(unsigned char key, int x, int y)
{
    rot -= 0.1f;

	switch (key) {
		case 'w':
			trig.rotateJointX(19, rot);
			break;
		default:
			break;
	}

	update();
}

void mouse(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    moving = 1;
    begin = x;
  }
  if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
    moving = 0;
  }
}

void motion(int x, int y)
{
  if (moving) {
    angle = angle + (x - begin);
    begin = x;
    newModel = 1;
    glutPostRedisplay();
  }
}

int xt = 1, yt = 1, zt = 1, xr = 1;

void translate(int x, int y, int z)
{
  GLfloat newz;

  if (xt)
    xloc += ((GLfloat) x) / 100;
  if (yt)
    yloc += ((GLfloat) y) / 100;
  if (zt) {
    newz = zloc - ((GLfloat) z) / 100;
    if (newz > -60.0 && newz < 13.0)
      zloc = newz;
  }
  newModel = 1;
  glutPostRedisplay();
}

void rotate(int x, int y, int z)
{
  if (xr) {
    angle += x / 2.0;
    newModel = 1;
    glutPostRedisplay();
  }
}

void button(int button, int state)
{
  if (state == GLUT_DOWN) {
    switch (button) {
    case 1:
      xt = yt = zt = xr = 1;
      break;
    case 5:
      xt = 1;
      yt = zt = xr = 0;
      break;
    case 6:
      yt = 1;
      xt = zt = xr = 0;
      break;
    case 7:
      zt = 1;
      xt = yt = xr = 0;
      break;
    case 8:
      xr = 1;
      xt = yt = zt = 0;
      break;
    case 9:
      xloc = yloc = zloc = 0;
      newModel = 1;
      glutPostRedisplay();
      break;
    }
  }
}

/*
bool contain(Edge & e, map < pair <int, int> , Edge > & list)
{

    pair <int, int> key;

    key.first = e.v1;
    key.second = e.v2;

    if (list.find(key) == list.end()) return false;
    else return true;
}
*/

bool contain(Edge & e, vector < Edge > & list)
{
    for (int i = 0; i < list.size(); i++)
    {
        if ((list[i]._v1 == e._v1 && list[i]._v2 == e._v2) ||
            (list[i]._v2 == e._v1 && list[i]._v1 == e._v2))
        {
            return true;
        }
    }

    return false;
}

int edgeID(Edge &e, vector<Edge> &list)
{
    for (int i = 0; i < list.size(); i++)
    {
        if ((list[i]._v1 == e._v1 && list[i]._v2 == e._v2) ||
            (list[i]._v2 == e._v1 && list[i]._v1 == e._v2))
        {
            return i;
        }
    }

    return -1;
}

int find(Edge & e, vector <Edge> list)
{
    for (int i = 0; i < list.size(); i++) {
        if (list[i] == e) return i;
    }

    return -1;
}

void Model::loadSkeleton(char * filename)
{
	cout << "Loading skeleton... (" << filename << ")" << endl;
    ifstream f(filename);

    if (f == NULL) {
        cerr << "failed reading skeleton data file " << filename << endl;
        exit(1);
    }

    char buf[1024];
    int index, parent;
	float x, y, z;

    while (!f.eof()) {
        f.getline(buf, sizeof(buf));
        sscanf(buf, "%d %f %f %f %d", &index, &x, &y, &z, &parent);

		Vector3f joint(x, y, z);
		Vector3f currentJoint(joint);
		_original.push_back(joint);
		_current.push_back(currentJoint);
		_parent.push_back(parent);

        // Create matrices.

        // Rotation matrix is just the identity matrix when starting.
        TransformMatrix rotationMatrix;
        _rotationMatrix.push_back(rotationMatrix);

        // Translation matrix
        TransformMatrix translationMatrix;

        if (_original.size() >= 1) {
            Vector3f parentJoint;
            getOriginalJoint(parent, parentJoint);

            Vector3f translation = parentJoint - joint;
            translationMatrix.translate(translation);
        }

        _translationMatrix.push_back(translationMatrix);
	}
}

void Model::loadWeights(char * filename)
{
	cout << "Loading weights... (" << filename << ")" << endl;
    ifstream f(filename);

    if (f == NULL) {
        cerr << "failed reading weights data file " << filename << endl;
        exit(1);
    }

    char buf[1024];

	// This is dreadful but what can you do.
	float x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20;
	float weights[21];

    while (!f.eof()) {
        f.getline(buf, sizeof(buf));
        sscanf(buf, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
				&x0, &x1, &x2, &x3, &x4, &x5, &x6, &x7, &x8, &x9, &x10,
				&x11, &x12, &x13, &x14, &x15, &x16, &x17, &x18, &x19, &x20);

		weights[0] = x0;
		weights[1] = x1;
		weights[2] = x2;
		weights[3] = x3;
		weights[4] = x4;
		weights[5] = x5;
		weights[6] = x6;
		weights[7] = x7;
		weights[8] = x8;
		weights[9] = x9;
		weights[10] = x10;
		weights[11] = x11;
		weights[12] = x12;
		weights[13] = x13;
		weights[14] = x14;
		weights[15] = x15;
		weights[16] = x16;
		weights[17] = x17;
		weights[18] = x18;
		weights[19] = x19;
		weights[20] = x20;

		vector <float> weight;

        float sum = 0.0f;
		for (int i = 0; i < 21; i++) {
            sum += weights[i];
		}

        for (int i = 0; i < 21; i++) {
            weight.push_back(weights[i]/sum);
        }

		_weights.push_back(weight);
	}
}

void Model::loadModel(char * filename)
{
	cout << "Loading model... (" << filename << ")" << endl;
    ifstream f(filename);

    if (f == NULL) {
        cerr << "failed reading polygon data file " << filename << endl;
        exit(1);
    }

    char buf[1024];
    char header[100];
    float x,y,z;
    float xmax,ymax,zmax,xmin,ymin,zmin;
    int v1, v2, v3, n1, n2, n3;

    xmax =-10000; ymax =-10000; zmax =-10000;
    xmin =10000; ymin =10000; zmin =10000;

    while (!f.eof()) {
        f.getline(buf, sizeof(buf));
        sscanf(buf, "%s", header);

        if (strcmp(header, "v") == 0) {
            // Looks like we've got a vertex.
            // Extract the data from the file.
            sscanf(buf, "%s %f %f %f", header, &x, &y, &z);

            // Store the vertex information.
            _v.push_back(Vector3f(x,y,z));

            // Store the normal information.
            _vn.push_back(Vector3f(0.f,0.f,1.f));

            // Create a node and give it an ID of the current iteration we are on.
            Node node;
            node._id = _v.size()-1;

            // Store it.
            _node.push_back(node);

            // If any of the coordinates lie outside our current maximums or
            // minimums then we should update them.
            if (x > xmax) xmax = x;
            if (y > ymax) ymax = y;
            if (z > zmax) zmax = z;

            if (x < xmin) xmin = x;
            if (y < ymin) ymin = y;
            if (z < zmin) zmin = z;
        }
        else if (strcmp(header, "f") == 0)
        {
            // Looks like we have a triangle. Let's store it.
            sscanf(buf, "%s %d %d %d", header, &v1, &v2, &v3);

            // Store the indexes of the contained vertices.
            Triangle trig(v1-1, v2-1, v3-1, v1-1, v2-1, v3-1);
            trig._id = _trig.size();
            _trig.push_back(trig);

            // Create new edges representing the edges of the triangle.
            Edge e1(v1-1, v2-1);
            Edge e2(v2-1, v3-1);
            Edge e3(v3-1, v1-1);

            int id1,id2,id3;

            // If e1 isn't in the list of edges...
            if ((id1 = edgeID(e1, _edge)) < 0)
            {
                // ID of edge is its index into the list.
                id1 = _edge.size();
                e1.setId(id1);
                e1.add_triangle(trig._id);

                // Add it to the list.
                _edge.push_back(e1);
                _edge[_edge.size()-1] = e1;


                // Store the edges that lead to the the node in the list of nodes.
                _node[v1-1].edges_to.push_back(v2-1);
                _node[v2-1].edges_to.push_back(v1-1);

                // Store the distance between the two nodes.
                _node[v1-1].edges_cost.push_back(_v[v1-1].distance(_v[v2-1]));
                _node[v2-1].edges_cost.push_back(_v[v1-1].distance(_v[v2-1]));
            }

            // If e2 isn't in the list of edges...
            if ((id2 = edgeID(e2, _edge)) < 0)
            {
                id2 = _edge.size();
                e2.setId(id2);
                e2.add_triangle(trig._id);
                _edge.push_back(e2);
                _edge[_edge.size()-1] = e2;

                _node[v2-1].edges_to.push_back(v3-1);
                _node[v3-1].edges_to.push_back(v2-1);

                _node[v2-1].edges_cost.push_back(_v[v2-1].distance(_v[v3-1]));
                _node[v3-1].edges_cost.push_back(_v[v3-1].distance(_v[v2-1]));
            }

            // If e3 isn't in the list of edges...
            if ((id3 = edgeID(e3, _edge)) < 0)
            {
                id3 = _edge.size();
                e3.setId(id3);
                e3.add_triangle(trig._id);
                _edge.push_back(e3);
                _edge[_edge.size()-1] = e3;

                _node[v3-1].edges_to.push_back(v1-1);
                _node[v1-1].edges_to.push_back(v3-1);


                _node[v3-1].edges_cost.push_back(_v[v3-1].distance(_v[v1-1]));
                _node[v1-1].edges_cost.push_back(_v[v1-1].distance(_v[v3-1]));
            }

            // I'm not sure if these are needed? I'll comment them out for now.
            //_edge[id1].add_triangle(trig._id);
            //_edge[id2].add_triangle(trig._id);
            //_edge[id3].add_triangle(trig._id);

            // Add the edge indexes to the triangle.
            _trig[_trig.size()-1].setEdge(id1,id2,id3);
        }
    }

    // A list that joins the vertices to their faces.
    vector < vector < int > > facelist (_v.size());

    // The normals of the faces.
    vector < Vector3f > facenorm (_trig.size());

    // Needless output for now. I'll enable this later if needed.
    //for (int i = 0; i < _edge.size(); i++) {
        //cout << " edge " << i << " trig list " << _edge[i].getTrigList().size()<< endl;
    //}

    // Compute all of the faces.
    for (int i = 0; i < _trig.size(); i++)
    {
        // Compute the normal of the face.
        Vector3f tmpv = (_v[_trig[i]._vertex[2]] - _v[_trig[i]._vertex[0]]) %
                (_v[_trig[i]._vertex[1]] - _v[_trig[i]._vertex[0]]) ;

        tmpv.normalize();
        facenorm[i] = tmpv;

        facelist[_trig[i]._vertex[0]].push_back(i);
        facelist[_trig[i]._vertex[1]].push_back(i);
        facelist[_trig[i]._vertex[2]].push_back(i);
    }

    for (int i = 0; i < _v.size(); i++)
    {
        // Create a blank new vector object.
        Vector3f N(0.f,0.f,0.f);

        for (int j = 0; j < facelist[i].size(); j++)
        {
            N += facenorm[facelist[i][j]];
        }

        // Create the average of the surrounding faces to be the normal at the point.
        N /= (float)facelist[i].size();
        _vn[i] = N;
    }

    // Set our minimums and maximums.
    _xmin = xmin; _ymin = ymin; _zmin = zmin;
    _xmax = xmax; _ymax = ymax; _zmax = zmax;

    // Close the file.
    f.close();
};

// If this is the first time we've shown the model then set it all up.
void recalcModelView(void)
{
    glPopMatrix();
    glPushMatrix();
    glTranslatef(xloc, yloc, zloc);
    glRotatef(angle, 0.0, 1.0, 0.0);
    glTranslatef(0, 0, 0);
    newModel = 0;
}

void DrawLine(Vector3f a, Vector3f b)
{
	glBegin(GL_LINES);
		glVertex3f(a[0], a[1], a[2]);
		glVertex3f(b[0], b[1], b[2]);
	glEnd();
}

void myDisplay()
{
    if (newModel)
        recalcModelView();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear OpenGL Window
    int trignum = trig.trigNum();
    Vector3f v1,v2,v3,n1,n2,n3;

    // Loop through each triangle and draw it.
    for (int i = 0 ; i < trignum; i++)
    {
        float m1,m2,m3;

        // Load in all of the data from our model.
        // ALL OF THE DATA.
        trig.getTriangleVertices(i,v1,v2,v3);
        trig.getTriangleNormals(i,n1,n2,n3);
        trig.getMorseValue(i, m1, m2, m3);

        m1 = m2 = m3 = trig.color(i);

        // Set the skin colour
        GLfloat skinColor[] = {0.9, 0.2, 0.5, 1.0};

		glBegin(GL_TRIANGLES);
			// Draw model
			skinColor[1] = m1; skinColor[0] = 1-m1;
			glMaterialfv(GL_FRONT, GL_DIFFUSE, skinColor);
			glNormal3f(-n1[0],-n1[1],-n1[2]);
			glVertex3f(v1[0],v1[1],v1[2]);

			skinColor[1] = m2; skinColor[0] = 1-m2;
			glMaterialfv(GL_FRONT, GL_DIFFUSE, skinColor);
			glNormal3f(-n2[0],-n2[1],-n2[2]);
			glVertex3f(v2[0],v2[1],v2[2]);

			skinColor[1] = m3; skinColor[0] = 1-m3;
			glMaterialfv(GL_FRONT, GL_DIFFUSE, skinColor);
			glNormal3f(-n3[0],-n3[1],-n3[2]);
			glVertex3f(v3[0],v3[1],v3[2]);

			skinColor[1] = m1; skinColor[0] = 1-m1;
			glMaterialfv(GL_FRONT, GL_DIFFUSE, skinColor);
			glNormal3f(-n1[0],-n1[1],-n1[2]);
			glVertex3f(v1[0],v1[1],v1[2]);
		glEnd();
    }

	// Draw skeleton
    int jointNum = trig.jointNum();

	Vector3f joint;
	int jointParent;

    for (int i = 0 ; i < jointNum; i++)
	{
		trig.getCurrentJoint(i, joint);
		trig.getJointParent(i, jointParent);

		// The first joint has no parent.
		if (jointParent != -1)
		{
			// Get the parent joint.
			Vector3f parent;
			trig.getCurrentJoint(jointParent, parent);

			// Draw a line from the joint to the parent.
			DrawLine(joint, parent);
		}
	}

    glutSwapBuffers();
}

int main(int argc, char **argv)
{
    // Load the model file into the
    if (argc > 3)  {
        trig.loadModel(argv[1]);
        trig.loadSkeleton(argv[2]);
        trig.loadWeights(argv[3]);
    }
    else {
        cerr << argv[0] << " <model filename> <initial skeleton filename> <weights filename>" << endl;
        exit(1);
    }

    // Lots of setup.
    int width, height;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

    glutInitWindowSize(NROWS, NCOLS);
    glutCreateWindow("Computer Animation and Visualisation - 0824586");

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

    /* Use depth buffering for hidden surface elimination. */
    glEnable(GL_DEPTH_TEST);

    /* Setup the view of the cube. */
    glMatrixMode(GL_PROJECTION);
    gluPerspective( /* field of view in degree */ 40.0,
    /* aspect ratio */ 1., /* Z near */ 1.0, /* Z far */ 1000.0);

    glMatrixMode(GL_MODELVIEW);

    gluLookAt(0.0, 0.0, 7.0,  /* eye is at (0,0,5) */
          0.0, 0.0, 0.0,      /* center is at (0,0,0) */
          0.0, 1.0, 0.0);      /* up is in positive Y direction */
    glPushMatrix();       /* dummy push so we can pop on model recalc */

    // Callbacks
    glutDisplayFunc(myDisplay);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
	glutKeyboardFunc(keyboard);

    glutMainLoop();// Display everything and wait
}
