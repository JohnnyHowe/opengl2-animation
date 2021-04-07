//  ========================================================================
//  COSC363: Computer Graphics (2021);  University of Canterbury.
//
//  FILE NAME: Teapot.cpp
//  See Lab01.pdf for details
//  ========================================================================

#include <GL/freeglut.h>
#include <iostream>
#include <fstream>
using namespace std; 

# define PI           3.14159265358979323846
# define GRAVITY	9.81
string THISDIR = "..\\OpenGLProject\\";

float rotationRadius = 8;

// Timing thing
float timer = 0;	// between 0 and maxTimer
float period = PI * 2 * sqrt(rotationRadius / GRAVITY);
float timeScale = 1.5;
float dt = 1.0 / 60.0;	// seconds

// Camera things
float cameraPositionDefault[3] = {0, 7, 40 };
float cameraAngleDefault[2] = { -PI / 2, 0 };
float cameraPosition[3] = {0, 0, 0 };
float cameraAngle[2] = { 0, 0 };	// Horizontal and vertical in radians
float rotationSpeed = 0.1;

// Textures
GLuint textures[4];

// Vase things
const int vasePoints = 12;  // Total number of vertices on the base curve
 
float vasex_init[vasePoints] = { 0.8, 1.0, 0.95, 0.9, 0.8, 0.6, 0.6, 0.8, 0.9, 0.95, 1.0, 0.8 };
float vasey_init[vasePoints] = { 0.5, 0.9, 1.0, 0.9, 0.8, 0.6, 0.4, 0.2, 0.1, 0.0, 0.1, 0.5 };
float vasez_init[vasePoints] = { 0 };

float rotationOrigin[3] = { 0, 10, 0 };
float vasePosition[3];
float vaseRotation = 0;

// Ball
float ballPosition[3];
float ballTurningPoint[3] = { 0, 2, 0 };
float ballBouncingPoint[3] = { 0, -9, 10 };

// ================================================================================================
// Other
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
// Texture Jazz
// ================================================================================================

void loadBMP(string filename)
{
	char *imageData;
	char header1[18], header2[24];
	short int planes, bpp;
	int wid, hgt;
	int nbytes, size, indx, temp;
	ifstream file(filename.c_str(), ios::in | ios::binary);
	if (!file)
	{
		cout << "*** Error opening image file: " << filename.c_str() << endl;
		exit(1);
	}
	file.read(header1, 18);		   //Initial part of header
	file.read((char *)&wid, 4);	   //Width
	file.read((char *)&hgt, 4);	   //Height
	file.read((char *)&planes, 2); //Planes
	file.read((char *)&bpp, 2);	   //Bits per pixel
	file.read(header2, 24);		   //Remaining part of header

	//	cout << "Width =" << wid << "   Height = " << hgt << " Bpp = " << bpp << endl;

	nbytes = bpp / 8;		   //No. of bytes per pixels
	size = wid * hgt * nbytes; //Total number of bytes to be read
	imageData = new char[size];
	file.read(imageData, size);
	//Swap r and b values
	for (int i = 0; i < wid * hgt; i++)
	{
		indx = i * nbytes;
		temp = imageData[indx];
		imageData[indx] = imageData[indx + 2];
		imageData[indx + 2] = temp;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, 3, wid, hgt, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
	delete imageData;
}

void loadTextures()				
{
	glGenTextures(2, textures); 				// Create a Texture object

	glBindTexture(GL_TEXTURE_2D, textures[0]);		//Use this texture
    loadBMP(THISDIR + "skybox.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);

	glBindTexture(GL_TEXTURE_2D, textures[1]);		//Use this texture
    loadBMP(THISDIR + "grass.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	

	glBindTexture(GL_TEXTURE_2D, textures[2]);		//Use this texture
    loadBMP(THISDIR + "wood.bmp");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

}

// ================================================================================================
// Drawing
// ================================================================================================

void drawAxes(float axisLength=10) {
    glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(axisLength, 0, 0);
	glEnd();
    glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, axisLength, 0);
	glEnd();
    glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, axisLength);
	glEnd();
}

void drawVase() {

	glTranslatef(0, 0.5, 0);

	// Init arrays
	float vx[vasePoints], vy[vasePoints], vz[vasePoints];
	float wx[vasePoints], wy[vasePoints], wz[vasePoints];
	for (int i = 0; i < vasePoints; i++)
	{
		vx[i] = vasex_init[i];
		vy[i] = -vasey_init[i];
		vz[i] = vasez_init[i];
	}

	// Sweep
	for (int j = 0; j <= 36; j++) {
		float a = (10 * j) * (PI / 180);

		// Calculate points
		for (int i = 0; i < vasePoints; i++) {
			wx[i] = vasex_init[i] * cos(a) + vasez_init[i] * sin(a);
			wy[i] = -vasey_init[i];
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

void drawFloor()
{
	glColor3f(0., 0.5,  0.);			//Floor colour
	float y = -10;
	float s = 50;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glBegin(GL_QUADS);
	  glNormal3f(0.0, 1.0, 0.0);   //Facing +z (Front side)
	  glTexCoord2f(0., 0.);
	  glVertex3f(-s, y, -s);
	  glTexCoord2f(1., 0.);
	  glVertex3f(s, y, -s);
	  glTexCoord2f(1.0, 1.0);
	  glVertex3f(s, y, s);
	  glTexCoord2f(0.0, 1.0);
	  glVertex3f(-s, y, s);
	  glEnd();
	  glDisable(GL_TEXTURE_2D);

	//for(int i = -50; i <= 50; i ++)
	//{
	//	glBegin(GL_LINES);			//A set of grid lines on the xz-plane
	//		glVertex3f(-50, -0.75, i);
	//		glVertex3f(50, -0.75, i);
	//		glVertex3f(i, -0.75, -50);
	//		glVertex3f(i, -0.75, 50);
	//	glEnd();
	//}
}

void drawTextureBox(float size=0.5, float squash = 0) {
	glPushMatrix();
	
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0/3);
	glVertex3f(-size, -size, -size);
	glTexCoord2f(0.25, 1.0/3);
	glVertex3f(size, -size, -size);
	glTexCoord2f(0.25, 2.0/3);
	glVertex3f(size, size, -size);
	glTexCoord2f(0., 2.0/3);
	glVertex3f(-size, size, -size);

	glTexCoord2f(0.25, 1.0/3);
	glVertex3f(size, -size, -size);
	glTexCoord2f(0.5, 1.0/3);
	glVertex3f(size, -size, size);
	glTexCoord2f(0.5, 2.0/3);
	glVertex3f(size, size, size);
	glTexCoord2f(0.25, 2.0/3);
	glVertex3f(size, size, -size);

	glTexCoord2f(0.5, 1.0/3);
	glVertex3f(size, -size, size);
	glTexCoord2f(0.75, 1.0/3);
	glVertex3f(-size, -size, size);
	glTexCoord2f(0.75, 2.0/3);
	glVertex3f(-size, size, size);
	glTexCoord2f(0.5, 2.0/3);
	glVertex3f(size, size, size);

	glTexCoord2f(0.75, 1.0/3);
	glVertex3f(-size, -size, size);
	glTexCoord2f(1.0, 1.0/3);
	glVertex3f(-size, -size, -size);
	glTexCoord2f(1.0, 2.0/3);
	glVertex3f(-size, size, -size);
	glTexCoord2f(0.75, 2.0/3);
	glVertex3f(-size, size, size);

	float e = squash;
	glTexCoord2f(0.5, 1);
	glVertex3f(-size, size-e, size);
	glTexCoord2f(0.5, 2.0/3);
	glVertex3f(size, size-e, size);
	glTexCoord2f(0.25, 2.0/3);
	glVertex3f(size, size-e, -size);
	glTexCoord2f(0.25, 1);
	glVertex3f(-size, size-e, -size);

	glTexCoord2f(0.25, 0);
	glVertex3f(-size, -size, size);
	glTexCoord2f(0.5, 0);
	glVertex3f(size, -size, size);
	glTexCoord2f(0.5, 1.0/3);
	glVertex3f(size, -size, -size);
	glTexCoord2f(0.25, 1.0/3);
	glVertex3f(-size, -size, -size);

	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void drawSkybox(float size = 30) {
	glPushMatrix();
	glTranslatef(cameraPosition[0], cameraPosition[1], cameraPosition[2]);
	drawTextureBox(size);
	glPopMatrix();
}

void drawTrampoline() {
	glPushMatrix();
	glColor3f(0.2, 0.2, 0.8);
	glRotatef(15, 1, 0, 0);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glVertex3f(-1, 0, -1);
	glVertex3f(1, 0, -1);
	glVertex3f(1, 0, 1);
	glVertex3f(-1, 0, 1);
	glEnd();
	glPopMatrix();
}

void display(void) 
{ 
	float lpos[4] = {0., 10., 0., 1.0};  //light's position

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(
		cameraPosition[0], 
		cameraPosition[1],
		cameraPosition[2],
		cameraPosition[0] + cos(cameraAngle[0]),
		cameraPosition[1] + cameraAngle[1],
		cameraPosition[2] + sin(cameraAngle[0]), 
		0,
		1,
		0);  //Camera position and orientation

	glLightfv(GL_LIGHT0,GL_POSITION, lpos);   //Set light position

	glDisable(GL_LIGHTING);			//Disable lighting when drawing floor.
    drawFloor();

	glBindTexture(GL_TEXTURE_2D, textures[0]);
	drawSkybox(100);

	// Wood
	glPushMatrix();
	glColor3f(0.3, 0.3, 0.3);
	glTranslatef(0, rotationRadius * 1.75, 0);
	glTranslatef(0, -rotationRadius / 2.0, 0);
	glRotatef(vaseRotation + 90, 0, 0, 1);
	glTranslatef(0, rotationRadius / 2.0, 0);
	glScalef(0.2, rotationRadius * 0.7, 0.2);
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	drawTextureBox();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0, rotationOrigin[1] - 1, 0);
	glScalef(19, 1, 1);
	drawTextureBox();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(10, 0, 0);
	glScalef(1, 20, 1);
	drawTextureBox();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-10, 0, 0);
	glScalef(1, 20, 1);
	drawTextureBox();
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);			//Enable lighting when drawing the teapot

	// Vase
	glColor3f(1, 0.8, 0);
	glPushMatrix();
	glTranslatef(vasePosition[0], vasePosition[1], vasePosition[2]);
	glRotatef(vaseRotation, 0, 0, 1);
	glRotatef(-90, 1, 0, 0);
	glScalef(2, 3, 2);
	drawVase();
	glPopMatrix();

	// Ball
    glColor3f(1, 1, 1);
	glPushMatrix();
	glTranslatef(ballPosition[0], ballPosition[1], ballPosition[2]);
	glutSolidSphere(1, 20, 20);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(ballPosition[0], ballPosition[1], -ballPosition[2]);
	//glutSolidSphere(0.2, 10, 10);
	glPopMatrix();

	// Trampolines
	glPushMatrix();
	glTranslatef(ballBouncingPoint[0], ballBouncingPoint[1] - 0.5, 0.5-ballBouncingPoint[2]);
	glRotatef(100, 1, 0, 0);
	glutSolidCylinder(1.5, 2.5, 20, 1);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(ballBouncingPoint[0], ballBouncingPoint[1] - 0.5, -0.5+ballBouncingPoint[2]);
	glRotatef(80, 1, 0, 0);
	glutSolidCylinder(1.5, 2.5, 20, 1);
	glPopMatrix();

	glutSwapBuffers(); 

	glFlush(); 
} 

// ================================================================================================
// Handling input
// ================================================================================================

void specialHandler(int key, int x, int y) {
	if (key == GLUT_KEY_UP) {
		// move forward
		cameraPosition[0] += cos(cameraAngle[0]);
		cameraPosition[2] += sin(cameraAngle[0]);
	}
	if (key == GLUT_KEY_DOWN) {
		// move back
		cameraPosition[0] -= cos(cameraAngle[0]);
		cameraPosition[2] -= sin(cameraAngle[0]);
	}
	if (key == GLUT_KEY_LEFT) {
		// look left
		cameraAngle[0] = fmod(cameraAngle[0] - rotationSpeed, PI * 2);
	}
	if (key == GLUT_KEY_RIGHT) {
		// look right
		cameraAngle[0] = fmod(cameraAngle[0] + rotationSpeed, PI * 2);
	}
}

void resetCamera() {
		cameraPosition[0] = cameraPositionDefault[0];
		cameraPosition[1] = cameraPositionDefault[1];
		cameraPosition[2] = cameraPositionDefault[2];
		cameraAngle[0] = cameraAngleDefault[0];
		cameraAngle[1] = cameraAngleDefault[1];
		timer = 0;
}

void keyHandler(unsigned char key, int x, int y) {
	if (key == 'w') {
		cameraPosition[0] += cos(cameraAngle[0]);
		cameraPosition[2] += sin(cameraAngle[0]);
	}
	if (key == 's') {
		cameraPosition[0] -= cos(cameraAngle[0]);
		cameraPosition[2] -= sin(cameraAngle[0]);
	}
	if (key == 'a') {
		cameraPosition[0] += cos(cameraAngle[0] - PI / 2.0);
		cameraPosition[2] += sin(cameraAngle[0] - PI / 2.0);
	}
	if (key == 'd') {
		cameraPosition[0] += cos(cameraAngle[0] + PI / 2.0);
		cameraPosition[2] += sin(cameraAngle[0] + PI / 2.0);
	}
	if (key == 'r') {
		resetCamera();
	}
	if (key == '8') {
		cameraAngle[1] += rotationSpeed;
	}
	if (key == '5') {
		cameraAngle[1] -= rotationSpeed;
	}
	if (key == '4') {
		cameraAngle[0] -= rotationSpeed;
	}
	if (key == '6') {
		cameraAngle[0] += rotationSpeed;
	}
}

// ================================================================================================
// Animating
// ================================================================================================

void moveObjects() {
	// vase
	//vasePosition[0] = vaseRotationOrigin[0] + sin(timer) * rotationRadius;
	//vasePosition[1] = vaseRotationOrigin[1] + cos(timer) * rotationRadius;

	float vaseInitialAngle = (PI / 8);
	float vaseAngle = vaseInitialAngle * cos(sqrt(GRAVITY / rotationRadius) * timer);
	vaseRotation = vaseAngle * 180 / PI + 90;

	vasePosition[0] = -cos(vaseAngle + PI / 2) * rotationRadius + rotationOrigin[0];
	vasePosition[1] = -sin(vaseAngle + PI / 2) * rotationRadius + rotationOrigin[1];
	vasePosition[2] = rotationOrigin[2];

	//float ballInitialAngle = (PI / 2);
	//float ballAngle = ballInitialAngle * cos(sqrt(GRAVITY / rotationRadius) * timer);

	//ballPosition[2] = -cos(ballAngle + PI / 2) * rotationRadius + rotationOrigin[0];
	//ballPosition[1] = -sin(ballAngle + PI / 2) * rotationRadius + rotationOrigin[1];
	//ballPosition[0] = rotationOrigin[2];

	// Ball
	float z;
	if (timer < period / 2) {
		z = (ballTurningPoint[2] - ballBouncingPoint[2]) + ballBouncingPoint[2] * (4 * timer) / period;
	}
	else {
		z = (ballTurningPoint[2] + ballBouncingPoint[2]) - ballBouncingPoint[2] * (4 * (timer - period / 2)) / period;
	}
	float xc = (ballBouncingPoint[1] - ballTurningPoint[1]) / ((ballBouncingPoint[2] - ballTurningPoint[2]) * (ballBouncingPoint[2] - ballTurningPoint[2]));
	float x = z - ballTurningPoint[2];
	ballPosition[1] = xc * x * x + ballTurningPoint[1];
	ballPosition[2] = z;
}

void timerFunc(int x) {
	moveObjects();
	timer = fmod(timer + dt * timeScale, period);
	glutTimerFunc(1000 * dt, timerFunc, 0);
	glutPostRedisplay();
}

// ================================================================================================
// Running and Initalizing
// ================================================================================================

void initialize(void)
{
    glClearColor(141.0/255, 177.0/255, 247.0/255, 1.0f);

	loadTextures();

	glEnable(GL_LIGHTING);		//Enable OpenGL states
	glEnable(GL_LIGHT0);
 	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-5.0, 5.0, -5.0, 5.0, 10.0, 1000.0);   //Camera Frustum
}

int main(int argc, char **argv) 
{ 

	cout << period;
	glutInit(&argc, argv);            
	glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH);  
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Teapot");
	initialize();
	glutSpecialFunc(specialHandler);
	glutKeyboardFunc(keyHandler);
	glutDisplayFunc(display);
	timerFunc(0);
	resetCamera();
	glutMainLoop();
	return 0; 
}


