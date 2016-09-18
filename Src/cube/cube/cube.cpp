//https://www.opengl.org/archives/resources/code/samples/glut_examples/examples/examples.html
#include <GL/glut.h>
#include <thread>

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

	glDisable(GL_LIGHTING);
	glLineWidth(2.5);
	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(15, 0, 0);
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0, 15, 0);
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0, 0, 15);
	glEnd();

	//glRotatef(1.f, 1.0, 0.0, 0.0);
	//glRotatef(1.f, 0.0, 0.0, 1.0);
	//	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_LIGHTING);
	drawBox();

	//	glRotatef(1.f, 0.0,1.0, 0.0);
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
		/* aspect ratio */ 1.0,
		/* Z near */ 1.0, /* Z far */ 100.0);
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(10.0, 10.0, 10.0,  /* eye is at (0,0,5) */
		0.0, 0.0, 0.0,      /* center is at (0,0,0) */
		0.0, 1.0, 0.);      /* up is in positive Y direction */

							/* Adjust cube position to be asthetic angle. */
							// 	glTranslatef(0.0, 0.0, -1.0);
							// 	glRotatef(60, 1.0, 0.0, 0.0);
							// 	glRotatef(-20, 0.0, 0.0, 1.0);
}

bool isLoop = true;
bool isRead = false;
void glThred(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("red 3D lighted cube");
	glutDisplayFunc(display);
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