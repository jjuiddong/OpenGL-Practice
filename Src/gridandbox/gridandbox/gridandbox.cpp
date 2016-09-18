#include <GL/glut.h>
#include <thread>
#include "Matrices.h"

using namespace std;

GLfloat light_diffuse[] = { 1.0, 0.0, 0.0, 1.0 };  /* Red diffuse light. */
GLfloat light_position[] = { 10.0, 10.0, 10.0, 0.0 };  /* Infinite light location. */
GLfloat normals[6][3] = {  /* Normals for the 6 faces of a cube. */
	{ -1.0, 0.0, 0.0 },{ 0.0, 1.0, 0.0 },{ 1.0, 0.0, 0.0 },
	{ 0.0, -1.0, 0.0 },{ 0.0, 0.0, 1.0 },{ 0.0, 0.0, -1.0 } };
GLint faces[6][4] = {  /* Vertex indices for the 6 faces of a cube. */
	{ 0, 1, 2, 3 },{ 3, 2, 6, 7 },{ 7, 6, 5, 4 },
	{ 4, 5, 1, 0 },{ 5, 6, 2, 1 },{ 7, 4, 0, 3 } };
GLfloat vertices[8][3];  /* Will be filled in with X,Y,Z vertexes. */

bool isLoop = true;
bool isRead = false;
int screenWidth = 800;
int screenHeight = 600;

bool mouseLeftDown;
bool mouseRightDown;
float mouseX, mouseY;
float cameraAngleX;
float cameraAngleY;
float cameraDistance;
Matrix4 matrixView;
Matrix4 matrixModel;
Matrix4 matrixModelView;    // = matrixView * matrixModel
Matrix4 matrixProjection;


void drawGrid(float size = 10.0f, float step = 1.0f);
void mouseCB(int button, int stat, int x, int y);



///////////////////////////////////////////////////////////////////////////////
// draw a grid on XZ-plane
///////////////////////////////////////////////////////////////////////////////
void drawGrid(float size, float step)
{
	// disable lighting
	glDisable(GL_LIGHTING);

	// 20x20 grid
	glBegin(GL_LINES);

	glColor3f(0.5f, 0.5f, 0.5f);
	for (float i = step; i <= size; i += step)
	{
		glVertex3f(-size, 0, i);   // lines parallel to X-axis
		glVertex3f(size, 0, i);
		glVertex3f(-size, 0, -i);   // lines parallel to X-axis
		glVertex3f(size, 0, -i);

		glVertex3f(i, 0, -size);   // lines parallel to Z-axis
		glVertex3f(i, 0, size);
		glVertex3f(-i, 0, -size);   // lines parallel to Z-axis
		glVertex3f(-i, 0, size);
	}

	// x-axis
	glColor3f(1, 0, 0);
	glVertex3f(-size, 0, 0);
	glVertex3f(size, 0, 0);

	// z-axis
	glColor3f(0, 0, 1);
	glVertex3f(0, 0, -size);
	glVertex3f(0, 0, size);

	glEnd();

	// enable lighting back
	glEnable(GL_LIGHTING);
}


void drawBox(void)
{
	int i;

	for (i = 0; i < 6; i++) {
		glBegin(GL_QUADS);
		glNormal3fv(&normals[i][0]);
		glVertex3fv(&vertices[faces[i][0]][0]);
		glVertex3fv(&vertices[faces[i][1]][0]);
		glVertex3fv(&vertices[faces[i][2]][0]);
		glVertex3fv(&vertices[faces[i][3]][0]);
		glEnd();
	}
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	// save the initial ModelView matrix before modifying ModelView matrix
	glPushMatrix();

	// tramsform camera
	matrixView.identity();
	matrixView.rotate(cameraAngleY, 0, 1, 0);
	matrixView.rotate(cameraAngleX, 1, 0, 0);
	matrixView.translate(0, 0, -cameraDistance);
	//@@ the equivalent code for using OpenGL routine is:
	//@@ glTranslatef(0, 0, -cameraDistance);
	//@@ glRotatef(cameraAngleX, 1, 0, 0);   // pitch
	//@@ glRotatef(cameraAngleY, 0, 1, 0);   // heading

	// copy view matrix to OpenGL
	glLoadMatrixf(matrixView.get());

	drawGrid();                         // draw XZ-grid with default size


	matrixModel.identity();
	static int angleY = 0;
	angleY++;
	if (angleY >= 360)
		angleY = 0;
	matrixModel.rotateY(angleY);

	// compute modelview matrix
	matrixModelView = matrixView * matrixModel;

	// copy modelview matrix to OpenGL
	glLoadMatrixf(matrixModelView.get());

 	glEnable(GL_LIGHTING);
 	drawBox();

	glPopMatrix();

	glutSwapBuffers();
}

void init(void)
{
	/* Setup cube vertex data. */
	vertices[0][0] = vertices[1][0] = vertices[2][0] = vertices[3][0] = -1;
	vertices[4][0] = vertices[5][0] = vertices[6][0] = vertices[7][0] = 1;
	vertices[0][1] = vertices[1][1] = vertices[4][1] = vertices[5][1] = -1;
	vertices[2][1] = vertices[3][1] = vertices[6][1] = vertices[7][1] = 1;
	vertices[0][2] = vertices[3][2] = vertices[4][2] = vertices[7][2] = 1;
	vertices[1][2] = vertices[2][2] = vertices[5][2] = vertices[6][2] = -1;

	/* Enable a single OpenGL light. */
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	/* Use depth buffering for hidden surface elimination. */
	glEnable(GL_DEPTH_TEST);

	/* Setup the view of the cube. */
	glMatrixMode(GL_PROJECTION);
	gluPerspective( /* field of view in degree */ 40.0,
		/* aspect ratio */ (float)screenWidth / (float)screenHeight,
		/* Z near */ 1.0, /* Z far */ 100.0);

// 	glMatrixMode(GL_MODELVIEW);
// 	gluLookAt(10.0, 10.0, 10.0,  /* eye is at (0,0,5) */
// 		0.0, 0.0, 0.0,      /* center is at (0,0,0) */
// 		0.0, 1.0, 0.);      /* up is in positive Y direction */

	cameraDistance = 6.f;
}


void keyboardCB(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: // ESCAPE
		exit(0);
		break;
	default:
		;
	}
}


void mouseCB(int button, int state, int x, int y)
{
	mouseX = x;
	mouseY = y;

	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mouseLeftDown = true;
		}
		else if (state == GLUT_UP)
			mouseLeftDown = false;
	}

	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mouseRightDown = true;
		}
		else if (state == GLUT_UP)
			mouseRightDown = false;
	}
}


void mouseMotionCB(int x, int y)
{
	if (mouseLeftDown)
	{
		cameraAngleY += (x - mouseX);
		cameraAngleX += (y - mouseY);
		mouseX = x;
		mouseY = y;
	}
	if (mouseRightDown)
	{
		cameraDistance -= (y - mouseY) * 0.2f;
		mouseY = y;
	}
}

void glThred(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(screenWidth, screenHeight);  // window size
	glutCreateWindow("Grid and Box");
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboardCB);
	glutMouseFunc(mouseCB);
	glutMotionFunc(mouseMotionCB);
	init();
	isRead = true;
	glutMainLoop();
	isLoop = false;
}

int main(int argc, char **argv)
{
	thread gl = std::thread(glThred, argc, argv);

	while (isLoop)
	{
		if (isRead)
			glutPostRedisplay();
	}

	return 0;             /* ANSI C requires main to return int. */
}