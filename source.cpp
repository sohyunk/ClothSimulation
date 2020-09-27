#include <iostream>
#include <ctime>
#include "PlaneCloth.h"
#include "Ball.h"
#include "CollisionProcessing.h"

int width = 600;
int height = 600;
float zoom = 15.0f;
float rotx = 0;
float roty = 0.001f;
float tx = 0;
float ty = 0;
int lastx = 0;
int lasty = 0;
bool simulation = false;
unsigned char Buttons[3] = { 0 };

PlaneCloth	*m_PClo;
Ball		*m_Ball;
CollisionProcessing *CollisionCheck;

void Init(void) 
{
	glShadeModel(GL_SMOOTH);
	glClearColor(0.2f, 0.2f, 0.4f, 0.5f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_COLOR_MATERIAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat lightPos[4] = { -1.0,1.0,0.5,0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, (GLfloat *)&lightPos);

	glEnable(GL_LIGHT1);

	GLfloat lightAmbient1[4] = { 0.0,0.0,0.0,0.0 };
	GLfloat lightPos1[4] = { 1.0,0.0,-0.2,0.0 };
	GLfloat lightDiffuse1[4] = { 0.5,0.5,0.3,0.0 };

	glLightfv(GL_LIGHT1, GL_POSITION, (GLfloat *)&lightPos1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, (GLfloat *)&lightAmbient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, (GLfloat *)&lightDiffuse1);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	m_PClo = new PlaneCloth(40, 40);
	m_Ball = new Ball();
	CollisionCheck = new CollisionProcessing(m_PClo, m_Ball);
}

void Darw(void)
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	m_PClo->Draw();
	glDisable(GL_LIGHTING);
}

void Update(void)
{
	if (simulation)
	{
		m_PClo->Simulation();
		CollisionCheck->Resolve();
	}
	if (m_Ball->is_move)
	{
		m_Ball->SetPosition();
		CollisionCheck->Resolve();
	}
	::glutPostRedisplay();
}

void Display(void)
{
	// 배경색
	glClearColor(0.6f, 0.6f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glDisable(GL_LIGHTING);
	glEnable(GL_LIGHTING);

	// 카메라
	zoom = 15;
	tx = 0.950000;
	ty = 3.000000;
	rotx = 15.500000;
	roty = -50;

	glTranslatef(0, 0, -zoom);
	glTranslatef(tx, ty, 0);
	glRotatef(rotx, 1, 0, 0);
	glRotatef(roty, 0, 1, 0);

	glPushMatrix();
	m_Ball->DrawBall();
	glPopMatrix();

	Darw();
	glutSwapBuffers();
}

void Reshape(int w, int h)
{
	if(w==0) {
		h = 1;
	}
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,(float)w/h,0.1,1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void Motion(int x,int y)
{
	int diffx = x - lastx;
	int diffy = y - lasty;
	lastx = x;
	lasty = y;

	if (Buttons[2])
	{
		zoom -= (float) 0.05f * diffx;
	}
	else if (Buttons[0])
	{
		rotx += (float) 0.5f * diffy;
		roty += (float) 0.5f * diffx;
	}
	else if (Buttons[1])
	{
		tx += (float) 0.05f * diffx;
		ty -= (float) 0.05f * diffy;
	}
	glutPostRedisplay();
}

void Mouse(int button,int state,int x,int y)
{
	lastx = x;
	lasty = y;
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		Buttons[0] = ((GLUT_DOWN == state) ? 1 : 0);
		break;
	case GLUT_MIDDLE_BUTTON:
		Buttons[1] = ((GLUT_DOWN == state) ? 1 : 0);
		break;
	case GLUT_RIGHT_BUTTON:
		Buttons[2] = ((GLUT_DOWN == state) ? 1 : 0);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void SpecialInput(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		break;
	case GLUT_KEY_RIGHT:
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'q':
	case 'Q':
		exit(0);
	case 'r':
	case 'R':
		m_PClo->Init();
		break;
	case 'b':
	case 'B':
		m_Ball->is_move = !(m_Ball->is_move);
		break;
	case ' ':
		simulation = !simulation;
		break;
	}
	glutPostRedisplay();
}

int main(int argc,char** argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(width,height);
	glutInitWindowPosition(100,100);
	glutCreateWindow("Cloth Simulation");
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutIdleFunc(Update);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialInput);
	Init();
	glutMainLoop();
}