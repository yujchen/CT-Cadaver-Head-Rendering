//Yu-Jui Chen 998896915
//ECS177 Scientific Visualization
//Professor Nelson Max
//Jan 25, 2017
//Compatible with Windows, not linux
#define TRACE_BACK
#define PI 3.14159265358979323
#define dfile 0

#include <GL/glew.h>
#include <GL/glut.h>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <math.h> 
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <iomanip>
#include <stdint.h>
using namespace std;


int button, state, mousex, mousey, dragX, dragY, cube = 1;
int width = 512, height = 512, isSpinning, lmax, do_neighbors = 2,
tX = 0, tY = 0, tZ = 0, ttX = 0, ttY = 0, ttZ = 0, just_up = 0,
tW = 0, ttW = 0;
double winRadius = 500, dX, dY;
double tt, transfac = .003, rad = 1.2,
sumx = 0., sumy = 0., sumz = 0.;
double xxmin, xxmax, ymin, ymax, zmin, zmax, large = 1000000.;
clock_t ticks1, ticks2, ticks3, ticks4;
int iterations = 0, delay = 4000;
int use_checkImage = 0, single_slice = 0;
float mm[16];
float m[16];


int dimensions = 113 * 256 * 256;
const int dimension = 113 * 256 * 256;
unsigned short CT[dimension];
unsigned short newCT[113][256][256];
float fCT[113][256][256];
float modCT[113][256][256][4];

//static GLubyte timage[113][256][256][4];
float rmin = 99999, rmax = 0;
unsigned short hist[65536];
int sum1 = 0;
static GLuint texName;
int res = 1;
void histogram();
void display();
void texting(void);
int s_initz = 60, s_inity = 123, s_initx = 123;
int initz = 0, inity = 0, initx = 0;
int ab = 0, ac = 0;
int single = -1;
int dispxyz = 0;
void ReadCTHead();
//Given code
void reshape(int w, int h) {
	width = w;
	height = h;
	if (width > height)
		winRadius = (double)width*.5;
	else
		winRadius = (double)height*.5;

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, (GLfloat)w / (GLfloat)h, 0.1, 60.0);
	glTranslatef(0., 0., -4.);
	display();
}


//Given code
void myRotated(double a, double X, double Y, double Z) {

	// Multiply on the right instead of the left
	//float m[16];
	glMatrixMode(GL_MODELVIEW);
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	glLoadIdentity();
	//cout << "a: " << a << "; X: " << X << "; Y: " << Y << "; Z: " << Z << endl;
	glTranslatef(transfac*ttX, transfac*ttY, transfac*ttZ);
	glRotated(a, X, Y, Z);
	glTranslatef(-transfac*ttX, -transfac*ttY, -transfac*ttZ);
	glMultMatrixf(m);
}

//Given code
void doRotation(double dX, double dY) {
	double offX = 2.0*(double)dragX / (double)width - 1.0;
	double offY = 1.0 - 2.0*(double)dragY / (double)height;
	double offZ = 0.25;
	double X, Y, Z, a; //s, c, t, d,
	X = -offZ*dY;
	Y = offZ*dX;
	Z = offX*dY - offY*dX;
	a = 180.*1.5*sqrt(dX*dX + dY*dY) / (PI*winRadius);
	myRotated(a, X, Y, Z);
	return;
}


//Given Code
void doTranslation(int tX, int tY, int tZ) {

	// Multiply on the right instead of the left

	float m[16];
	glMatrixMode(GL_MODELVIEW);
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	glLoadIdentity();
	glTranslatef(transfac*tX, transfac*tY, transfac*tZ);
	glMultMatrixf(m);
}


//slicing along the y direction
void x_slice()
{
	int initx, start, end, step, slices;
	float x;
	slices = 256;
	glBindTexture(GL_TEXTURE_3D, texName);
	if (m[2] > 0)
	{
		start = 0;
		end = slices;
		step = 1;
	}
	else
	{
		start = slices - 1;
		end = -1;
		step = -1;
	}


	for (initx = start; initx != end; initx += step)
	{
		x = initx / (slices - 1.);
		if (single == 1)
		{
			x = s_initx / (slices - 1.);
		}
		glBegin(GL_POLYGON);

		glTexCoord3f(x, 0., 0.);
		glVertex3f(x - 0.5, -0.5, -0.5);
		glTexCoord3f(x, 1., 0.);
		glVertex3f(x - 0.5, 0.5, -0.5);
		glTexCoord3f(x, 1., 1.);
		glVertex3f(x - 0.5, 0.5, 0.5);
		glTexCoord3f(x, 0., 1.);
		glVertex3f(x - 0.5, -0.5, 0.5);

		glEnd();

	}
}


//slicing along the y-direction
void y_slice()
{
	int inity, start, end, step, slices;
	float y;
	slices = 256;


	glBindTexture(GL_TEXTURE_3D, texName);

	if (m[6] > 0)// deciding slicing front to back or back to front
	{
		start = 0;
		end = slices;
		step = 1;
	}
	else
	{
		start = slices - 1;
		end = -1;
		step = -1;
	}
	for (inity = start; inity != end; inity += step)
	{
		y = inity / (slices - 1.);
		if (single == 1)
		{
			y = s_inity / (slices - 1.);
		}
		glBegin(GL_POLYGON);

		glTexCoord3f(0., y, 0.);
		glVertex3f(-0.5, y - 0.5, -0.5);

		glTexCoord3f(1., y, 0.);
		glVertex3f(0.5, y - 0.5, -0.5);

		glTexCoord3f(1., y, 1.);
		glVertex3f(0.5, y - 0.5, 0.5);

		glTexCoord3f(0., y, 1.);
		glVertex3f(-0.5, y - 0.5, 0.5);

		glEnd();

	}
}


//slicing along the z-direction
void z_slice()
{
	int initz, start, end, step, slices;
	float z;
	slices = 256;


	glBindTexture(GL_TEXTURE_3D, texName);

	if (m[10] > 0) // deciding slicing front to back or back to front
	{
		start = 0;
		end = slices;
		step = 1;
	}
	else
	{
		start = slices - 1;
		end = -1;
		step = -1;
	}

	for (initz = start; initz != end; initz += step)
	{
		z = initz / (slices - 1.);

		if (single == 1)
		{
			z = s_initz / (slices - 1.);
		}
		glBegin(GL_POLYGON);
		glTexCoord3f(0., 0., z);
		glVertex3f(-0.5, -0.5, z - 0.5);

		glTexCoord3f(0., 1., z);
		glVertex3f(-0.5, 0.5, z - 0.5);

		glTexCoord3f(1., 1., z);
		glVertex3f(0.5, 0.5, z - 0.5);

		glTexCoord3f(1., 0., z);
		glVertex3f(0.5, -0.5, z - 0.5);
		glEnd();

	}
}




void display(void) {
	//int k, j, i, l, m, iw, mstop, sum = 0;
	//double t, s, c, xx, yy, zz;
	//int j;
	ticks4 = clock();
	tt = (double)(ticks4 - ticks3) / (double)CLOCKS_PER_SEC;
	if (state == GLUT_DOWN && tt > .05) isSpinning = 0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//j = iterations - delay;
	glColor4f(1., .2, 0.2, 1);
	if (isSpinning) {
		glMatrixMode(GL_MODELVIEW);
		//cout << "dx: " << dX << "; dy:" << dY << endl;
		doRotation(dX, dY);
	}
	glDisable(GL_LIGHTING);
	glutWireCube(1.);       //glutWireCube cube at
	glEnable(GL_LIGHTING);
	//glRotatef(-90., 0, 0, 1);
	//glScalef(-1, 2, 1);
	//glTranslatef(-0.5, -0.5, -0.5);




	glEnable(GL_TEXTURE_3D);

	if (single == 1)
	{
		if (dispxyz == 0)
		{
			z_slice();
		}
		else if (dispxyz == 1)
		{
			y_slice();
		}
		else if (dispxyz == 2)
		{
			x_slice();
		}
	}
	else
	{

		if ((fabs(m[10]) > fabs(m[6])) && (fabs(m[10]) > fabs(m[2])))
		{
			z_slice();
		}
		else if (fabs(m[6]) > fabs(m[2]) && single == -1)
		{
			y_slice();
		}
		else if (single == -1)
		{
			x_slice();
		}

	}


	//glTranslatef(0.5, 0.5, 0.5);
	//glScalef(-1, 0.5, 1);
	//glRotatef(90, 0, 0, 1);

	glDisable(GL_TEXTURE_3D);
	glutSwapBuffers();
	glFlush();
	++iterations;
}

//Given code
void mouse(int but, int sta, int x, int y) {


	button = but;
	state = sta;
	if (state == GLUT_DOWN) {
		dragX = x;
		dragY = y;
		dX = 0;
		dY = 0;
	}
	if (state == GLUT_UP && button == GLUT_LEFT_BUTTON) {
		/*
		dX = x - dragX;
		dY = dragY - y;
		if(dX!=0||dY!=0)
		isSpinning = 1;
		else
		isSpinning = 0;
		*/
		ticks1 = clock();
		isSpinning = 0;
		just_up = 1;
	}
}

//Given code
void passive_motion(int x, int y) {
	double t;
	if (just_up) {
		ticks2 = clock();
		t = (double)(ticks2 - ticks1) / (double)CLOCKS_PER_SEC;
		just_up = 0;
		if (t < .01) {
			dX = .2*(x - dragX);
			dY = .2*(dragY - y);
			isSpinning = 1;
		}
	}
}

//Given code
void motion(int x, int y) {
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
		dX = x - dragX;
		dY = dragY - y;
		glMatrixMode(GL_MODELVIEW);
		doRotation(dX, dY);
		dragX = x;
		dragY = y;
	}
	if (state == GLUT_DOWN && button == GLUT_MIDDLE_BUTTON) {
		tX = x - dragX;
		tY = dragY - y;
		ttX += tX;
		ttY += tY;
		dragX = x;
		dragY = y;
		doTranslation(tX, tY, 0);
	}
	if (state == GLUT_DOWN && button == GLUT_RIGHT_BUTTON) {
		tZ = x - dragX;
		ttZ += tZ;
		tW = y - dragY;
		ttW += tW;
		dragX = x;
		dragY = y;
		doTranslation(0, 0, tZ);
	}
	ticks3 = clock();
	tt = (double)(ticks3 - ticks4) / (double)CLOCKS_PER_SEC;
	display();
}

//Code given in class
void initScreen() {
	//GLfloat light_position[4] = { 1., 14., 10., 0. };
	//GLfloat white_light[4] = { 1., 1., .6, 0. };
	//  GLfloat lmodel_ambient[4] = { .4, .4, .4, 1. };
	glClearColor(0., 0., 0., 0.);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// glOrtho(-.9, .9, -.9, .9, -1., 1.);
	if (0)
		glTranslatef(0., 0., -4.);
	else
		glTranslatef(0., 0., -4.);
	gluPerspective(40., width / height, 0.1, 60.);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glShadeModel(GL_SMOOTH);

	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	//glLightfv(GL_LIGHT0, GL_AMBIENT, lmodel_ambient);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	//glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_NORMALIZE);


	texting();

}




unsigned short ShortSwap(unsigned short s)
{
	unsigned char b1, b2;

	b1 = s & 255;
	b2 = (s >> 8) & 255;

	return (b1 << 8) + b2;

}

void histogram()
{
	for (int z = 0; z < 113; z++)
	{
		for (int y = 0; y < 256; y++)
		{
			for (int x = 0; x < 256; x++)
			{
				int index = newCT[z][y][x];
				hist[index]++;
				sum1++;
			}
		}
	}
	for (int a = 0; a < 65536; a++)
	{
		if (hist[a] != 0)
		{
			//cout << "[" << a << "]" << hist[a] << endl;
			//cout << a << ',' << hist[a] << endl;
		}
	}
	//cout << sum1 << endl;
}



void texting(void)
{
	glEnable(GL_TEXTURE_3D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	//glEnable(GL_DEPTH_TEST);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_3D, texName);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage3D = (PFNGLTEXIMAGE3DPROC)wglGetProcAddress("glTexImage3D");
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, 256, 256, 113, 0, GL_RGBA, GL_FLOAT, modCT);
	glDisable(GL_TEXTURE_3D);

}



void ReadCTHead()
{

	FILE* inf;
	unsigned short h;
//	inf = fopen("CThead", "rb");
	fopen_s(&inf, "CThead", "rb");
	int min = 99999, max = 0;
	if (inf != NULL)
	{
		for (int z = 0; z < 113; z++)
		{
			for (int y = 0; y < 256; y++)
			{
				for (int x = 0; x < 256; x++)
				{
					fread(&h, sizeof(h), 1, inf);
					if (min > h && h>10000)
					{
						min = h;
					}
					if (max < h)
					{
						max = h;
					}
					if (h < 64419)
					{
						modCT[z][y][x][0] = 0;
						modCT[z][y][x][1] = 0;
						modCT[z][y][x][2] = 0;
						modCT[z][y][x][3] = 0;
					}
					else
					{
						modCT[z][y][x][0] = (float)(h - 64419.f) / (65535.f - 64419.f);
						modCT[z][y][x][1] = (float)(h - 64419.f) / (65535.f - 64419.f);
						modCT[z][y][x][2] = (float)(h - 64419.f) / (65535.f - 64419.f);
						modCT[z][y][x][3] = (float)(h - 64419.f) / (65535.f - 64419.f);
					}
				}
			}
		}
		//cout << min << ',' << max << endl;


		for (int z = 0; z < 113; z++)
		{
			for (int y = 0; y < 256; y++)
			{
				for (int x = 0; x < 256; x++)
				{
					if (modCT[z][y][x][3] < 0.10)
					{
						modCT[z][y][x][0] = 255;
						modCT[z][y][x][3] = 0.2;
					}
					if (modCT[z][y][x][3] > 0.20 && modCT[z][y][x][3] < 0.201)
					{
						modCT[z][y][x][1] = 0.1;
						modCT[z][y][x][3] = 0.1;
					}
					//cout << modCT[z][y][x][0];
				}
			}
		}
	}
}


//Given code
void key_press(unsigned char key, int x, int y)
{
	if (key == 'a')
	{
		if (single == 1)
		{
			if (s_initz < 255 && dispxyz == 0)
			{
				s_initz++;
				//cout << "s_initz:" << s_initz << endl;
				isSpinning = 0;
				//texting();
				display();
			}
			if (s_inity < 255 && dispxyz == 1)
			{
				s_inity++;
				//cout << "s_inity:" << s_inity << endl;
				isSpinning = 0;
				//texting();
				display();
			}
			if (s_initx < 255 && dispxyz == 2)
			{
				s_initx++;
				//cout << "s_initx:" << s_initx << endl;
				isSpinning = 0;
				//texting();
				display();
			}
		}

	}
	if (key == 's')
	{
		if (single == 1)
		{
			if (s_initz > 1 && dispxyz == 0)
			{
				s_initz--;
				//cout << "s_initz:" << s_initz << endl;    
				isSpinning = 0;
				//texting();
				display();
			}
			if (s_inity > 1 && dispxyz == 1)
			{
				s_inity--;
				//cout << "s_inity:" << s_inity << endl;    
				isSpinning = 0;
				//texting();
				display();
			}
			if (s_initx > 1 && dispxyz == 2)
			{
				s_initx--;
				//cout << "s_initx:" << s_initx << endl;    
				isSpinning = 0;
				//texting();
				display();
			}
		}
	}
	else if (key == 'd')
	{
		single = (-1) * single;
		isSpinning = 0;
		//texting();
		display();
	}
	else if (key == 'f')
	{
		if (single == 1)
		{
			dispxyz++;
			if (dispxyz == 3)
			{
				dispxyz = 0;
			}
			if (dispxyz == 0)
			{
				cout << "Slicing in z-direction. " << endl;
			}
			if (dispxyz == 1)
			{
				cout << "Slicing in y-direction. " << endl;
			}
			if (dispxyz == 2)
			{
				cout << "Slicing in x-direction. " << endl;
			}
			//cout << "slicing: " << dispxyz << endl;
			isSpinning = 0;
			//texting();
			display();
		}
	}
	else if (key == 'v')
	{
		//cout << "a: " << a << "; X: " << X << "; Y: " << Y << "; Z: " << Z << endl;
		glTranslatef(0.5, 0.5, 0.5);
		//glRotated(90, 1, 0, 0);
		glTranslatef(-0.5, -0.5, -0.5);
		glMultMatrixf(m);
		display();
	}
}

//Given code
int main(int argc, char** argv) {
	ReadCTHead();

	//cout << "hello" << endl;

	glutInit(&argc, argv);  //Intialize GLUT library
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);//Specify initial window size
	glutInitWindowPosition(0, 0);     //Specify initial window position
	glutCreateWindow("Program 4 - CT Cadaver Head");       //Create Window with a given title
	initScreen();                     //
	glutDisplayFunc(display);         //Specify function to draw scene
	glutReshapeFunc(reshape);         //Specify function to set up viewing
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(passive_motion);
	glutKeyboardFunc(key_press);
	glutMainLoop();                   //Start the event loop
	return 0;
}







//temp =  ((CT[z][y][x] & 0xff) << 8) | ((CT[z][y][x] & 0xff00) >> 8);
//CT[z][y][x] = temp;
//reverseBytes(&CT[z][y][x], sizeof(unsigned short));
//cout << CT[z][y][x] << ',';
//            if (CT[z][y][x] < 10000)
//          {
//          CT[z][y][x] = 0;
//      }





/*
int x = 0, y = 0 , z = 0;       //Cadavar Data set is 16 bit integer encoded in binary. UNSIGNED
string file = "CThead"; //apparently ifstream is not happy with just passing a string in
//ifstream inf;
//inf.open(file.c_str(), ios::in | ios::binary);
//FILE *inf = fopen("CThead", "rb");
//fread(CT, sizeof(unsigned short), dimensions, inf);
ifstream inf(file.c_str(), ios::in | ios::binary);
int end;
inf.seekg(0, inf.end);
end = inf.tellg();
int i = 0;
inf.seekg(0);

while (inf.tellg()!= end)
{
inf.read((char*)&CT[i], sizeof(unsigned short));
//cout << CT[i] << endl;
i++;
}
i = 0;
for (int z = 0; z < 113; z++ )
{
for ( int y = 0; y < 256; y++ )
{
for( int x = 0; x < 256; x++ )
{
newCT[z][y][x] = CT[i];
i++;
}
}
}

for (int z = 0; z < 113; z++ )
{
for ( int y = 0; y < 256; y++ )
{
for( int x = 0; x < 256; x++ )
{
fCT[z][y][x] = newCT[z][y][x];
}
}
}

for (int z = 0; z < 113; z++ )
{
for ( int y = 0; y < 256; y++ )
{
for( int x = 0; x < 256; x++ )
{

if ( rmin > fCT[z][y][x] && fCT[z][y][x] > 50000 )
{
rmin = newCT[z][y][x];
}
if ( rmax < fCT[z][y][x] )
{
rmax = newCT[z][y][x];
}
}
}
}
for (int z = 0; z < 113; z++ )
{
for ( int y = 0; y < 256; y++ )
{
for( int x = 0; x < 256; x++ )
{
if (fCT[z][y][x] < rmin)
{
modCT[z][y][x][0] = 0;
modCT[z][y][x][1] = 0;
modCT[z][y][x][2] = 0;
modCT[z][y][x][3] = 0;
}
else
{
modCT[z][y][x][0] = (fCT[z][y][x] - rmin) / (rmax - rmin) ;
modCT[z][y][x][1] = (fCT[z][y][x] - rmin) / (rmax - rmin) ;
modCT[z][y][x][2] = (fCT[z][y][x] - rmin) / (rmax - rmin) ;
modCT[z][y][x][3] = (fCT[z][y][x] - rmin) / (rmax - rmin) ;
cout << modCT[z][y][x][0] << endl;
}
//cout << modCT[z][y][x][0] << ',';

}
//cout << endl;

}
//cout << endl;
}
*/







