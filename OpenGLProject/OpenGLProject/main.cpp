//  ========================================================================
//  COSC363: Computer Graphics (2021);  University of Canterbury.
//
// Can move with arrow keys:
//	up: forward
//	down: back
//	left: turn left
//	right: turn right
// 
// wasd keys work for movement too
//
//  FILE NAME: Teapot.cpp
//  See Lab01.pdf for details
//  ========================================================================

#include <cmath>
#include <GL/freeglut.h>
#include <iostream>

# define PI           3.14159265358979323846
using namespace std; 


// Camera things
float cameraPosition[3] = { 0, 10, 0 };
float cameraDirectionOffset[3] = { 0, -0.5, 0 };
float cameraAngle = 0;
float rotationSpeed = 0.1;

// Vase things
const int vasePoints = 50;  // Total number of vertices on the base curve

float vasex_init[vasePoints] = { 0, 8, 8, 7.5, 6.7, 5, 5.5, 4, 4, 5, 5.6, 6.1, 6.8, 7.1, 7.5, 8, 8.4, 8.7, 9, 9.3,
                      9.8, 10, 10.2, 10.4, 10.6, 10.9, 11, 11.1, 11.2, 11.3, 11.4, 11.3, 11.2, 11.1, 11, 10.5, 9.5, 8.2, 7, 6.2,
                      6, 6.2, 6.8, 7.6, 8.5, 7, 6.1, 5.3, 4.7, 4.5 };
float vasey_init[vasePoints] = { 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
                      19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
                      39, 40, 41, 42, 43, 43, 42, 41, 40, 39 };
float vasez_init[vasePoints] = { 0 };

// ================================================================================================
// Math
// ================================================================================================

void normal(float x1, float y1, float z1,
	float x2, float y2, float z2,
	float x3, float y3, float z3)
{
	float nx, ny, nz;
	nx = y1 * (z2 - z3) + y2 * (z3 - z1) + y3 * (z1 - z2);
	ny = z1 * (x2 - x3) + z2 * (x3 - x1) + z3 * (x1 - x2);
	nz = x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2);

	glNormal3f(nx, ny, nz);
}

// ================================================================================================
// Handling input
// ================================================================================================

void specialHandler(int key, int x, int y) {
	if (key == GLUT_KEY_UP) {
		// move forward
		cameraPosition[0] += cos(cameraAngle);
		cameraPosition[2] += sin(cameraAngle);
	}
	if (key == GLUT_KEY_DOWN) {
		// move back
		cameraPosition[0] -= cos(cameraAngle);
		cameraPosition[2] -= sin(cameraAngle);
	}
	if (key == GLUT_KEY_LEFT) {
		// look left
		cameraAngle = fmod(cameraAngle - rotationSpeed, PI * 2);
	}
	if (key == GLUT_KEY_RIGHT) {
		// look right
		cameraAngle = fmod(cameraAngle + rotationSpeed, PI * 2);
	}
	glutPostRedisplay();
}

void keyHandler(unsigned char key, int x, int y) {
	if (key == 'w') {
		cameraPosition[0] += cos(cameraAngle);
		cameraPosition[2] += sin(cameraAngle);
	}
	if (key == 's') {
		cameraPosition[0] -= cos(cameraAngle);
		cameraPosition[2] -= sin(cameraAngle);
	}
	if (key == 'a') {
		cameraPosition[0] += cos(cameraAngle - PI / 2.0);
		cameraPosition[2] += sin(cameraAngle - PI / 2.0);
	}
	if (key == 'd') {
		cameraPosition[0] += cos(cameraAngle + PI / 2.0);
		cameraPosition[2] += sin(cameraAngle + PI / 2.0);
	}
	glutPostRedisplay();
}


// ================================================================================================
// Drawing
// ================================================================================================


/// <summary>
/// Draw a vase at (0, 0, 0) with max offsets +- (0.5, 0.5, 0.5) 
/// </summary>
void drawVase() {
	glScalef(1 / 11.4, 1 / 43.0, 1 / 11.4);

	// Init arrays
	float vx[vasePoints], vy[vasePoints], vz[vasePoints];
	float wx[vasePoints], wy[vasePoints], wz[vasePoints];
	for (int i = 0; i < vasePoints; i++)
	{
		vx[i] = vasex_init[i];
		vy[i] = vasey_init[i];
		vz[i] = vasez_init[i];
	}

	// Sweep
	for (int j = 0; j <= 36; j++) {
		float a = (10 * j) * (PI / 180);

		// Calculate points
		for (int i = 0; i < vasePoints; i++) {
			wx[i] = vasex_init[i] * cos(a) + vasez_init[i] * sin(a);
			wy[i] = vasey_init[i];
			wz[i] = vasex_init[i] * sin(a) + vasez_init[i] * cos(a);
		}

		// Draw
		glBegin(GL_TRIANGLE_STRIP);
		for (int i = 0; i < vasePoints; i++) {
			if (i > 0) normal(vx[i - 1], vy[i - 1], vz[i - 1], wx[i - 1], wy[i - 1], wz[i - 1], vx[i], vy[i], vz[i]);
			glVertex3f(vx[i], vy[i], vz[i]);
			if (i > 0) normal(wx[i - 1], wy[i - 1], wz[i - 1], wx[i], wy[i], wz[i], vx[i], vy[i], vz[i]);
			glVertex3f(wx[i], wy[i], wz[i]);
		}
		glEnd();

		// Update arrays
		for (int i = 0; i < vasePoints; i++)
		{
			vx[i] = wx[i];
			vy[i] = wy[i];
			vz[i] = wz[i];
		}
	}
}

//--Draws a grid of lines on the floor plane -------------------------------
void drawFloor()
{
	glColor3f(0., 0.5,  0.);			//Floor colour
	for(int i = -50; i <= 50; i ++)
	{
		glBegin(GL_LINES);			//A set of grid lines on the xz-plane
			glVertex3f(-50, -0.75, i);
			glVertex3f(50, -0.75, i);
			glVertex3f(i, -0.75, -50);
			glVertex3f(i, -0.75, 50);
		glEnd();
	}
}

//--Display: ---------------------------------------------------------------
//--This is the main display module containing function calls for generating
//--the scene.
void display(void) 
{ 
	float lpos[4] = {0., 10., 10., 1.0};  //light's position

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    gluLookAt(
		cameraPosition[0], 
		cameraPosition[1],
		cameraPosition[2],
		cameraPosition[0] + cos(cameraAngle) + cameraDirectionOffset[0],
		cameraPosition[1] + cameraDirectionOffset[1],
		cameraPosition[2] + sin(cameraAngle) + cameraDirectionOffset[2], 
		0,
		1,
		0);  //Camera position and orientation

	glLightfv(GL_LIGHT0,GL_POSITION, lpos);   //Set light position

	glDisable(GL_LIGHTING);			//Disable lighting when drawing floor.
    drawFloor();

	glEnable(GL_LIGHTING);			//Enable lighting when drawing the teapot
    glColor3f(0.0, 1.0, 1.0);
    // glutSolidTeapot(1.0);

	glScalef(1, 3, 1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	drawVase();

	glFlush(); 
} 

// ================================================================================================
// Start her up
// ================================================================================================

void initialize(void)
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glEnable(GL_LIGHTING);		//Enable OpenGL states
	glEnable(GL_LIGHT0);
 	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-5.0, 5.0, -5.0, 5.0, 10.0, 1000.0);   //Camera Frustum
}


//  ------- Main: Initialize glut window and register call backs -------
int main(int argc, char **argv) 
{ 

	glutInit(&argc, argv);            
	glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH);  
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Ooooh Aaaah");
	initialize();
	glutDisplayFunc(display);
	glutSpecialFunc(specialHandler);
	glutKeyboardFunc(keyHandler);
	glutMainLoop();
	return 0; 
}

