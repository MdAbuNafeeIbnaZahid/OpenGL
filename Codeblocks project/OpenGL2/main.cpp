/*
 * GLUT Shapes Demo
 *
 * Written by Nigel Stewart November 2003
 *
 * This program is test harness for the sphere, cone
 * and torus shapes in GLUT.
 *
 * Spinning wireframe and smooth shaded shapes are
 * displayed until the ESC or q key is pressed.  The
 * number of geometry stacks and slices can be adjusted
 * using the + and - keys.
 */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <bits/stdc++.h>
#define pi (2*acos(0.0))

#define A 20
#define MY_INF 200
#define eps 1e-1

using namespace std;

double paramT;



double cameraAngle; // this is the angle created by the camera
// in xy plane with the x axis

double cameraDistance; // distance of camera along the xy plane
double cameraHeight; // distance of camera along the z axis



int drawgrid;
int drawaxes;
double angle;

struct point
{
	double x,y,z;
};


void drawAxes()
{
	if(drawaxes==1)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);{
			glVertex3f( MY_INF,0,0);
			glVertex3f(-MY_INF,0,0);

			glVertex3f(0,-MY_INF,0);
			glVertex3f(0, MY_INF,0);

			glVertex3f(0,0, MY_INF);
			glVertex3f(0,0,-MY_INF);
		}glEnd();
	}
}


void drawGrid()
{
	int i;
	if(drawgrid==1)
	{
		glColor3f(0.6, 0.6, 0.6);	//grey
		glBegin(GL_LINES);{
			for(i=-8;i<=8;i++){

				if(i==0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i*10, -90, 0);
				glVertex3f(i*10,  90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i*10, 0);
				glVertex3f( 90, i*10, 0);
			}
		}glEnd();
	}
}

void drawSquare(double a) // This method draws a square in the z = 0 plane
// center of the square is in (0,0,0)
{
    //glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( a, a,0);
		glVertex3f( a,-a,0);
		glVertex3f(-a,-a,0);
		glVertex3f(-a, a,0);
	}glEnd();
}


void drawCircle(double radius,int segments)
{
    int i;
    struct point points[100];
    glColor3f(0.7,0.7,0.7);
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(i=0;i<segments;i++)
    {
        glBegin(GL_LINES);
        {
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}

void drawCylinder(double radius, double height, int segments)
{
    assert( segments <= 100 );
    struct point pointAr[3][109];

    // generate upper and lower cirlce
    for (int i = 0; i <= segments; i++)
    {
        pointAr[0][i].x = pointAr[1][i].x = radius*cos(((double)i/(double)segments)*2*pi);
        pointAr[0][i].y = pointAr[1][i].y = radius*sin(((double)i/(double)segments)*2*pi);

        // fixing the z coordinate of upper circle
        pointAr[0][i].z = height;

        // fixing the z coordinate of lower circle
        pointAr[1][i].z = -height;
    }


    // This for loop is not complete
    for (int i = 0; i < segments; i++)
    {
        glBegin(GL_QUADS);{
            glVertex3f( a, a,0);
            glVertex3f( a,-a,0);
            glVertex3f(-a,-a,0);
            glVertex3f(-a, a,0);
        }glEnd();
    }
}

void drawCone(double radius,double height,int segments)
{
    assert(segments <= 100);

    int i;
    double shade;
    struct point points[109];
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw triangles using generated points
    for(i=0;i<segments;i++)
    {
        //create shading effect
        if(i<segments/2)shade=2*(double)i/(double)segments;
        else shade=2*(1.0-(double)i/(double)segments);
        glColor3f(shade,shade,shade);

        glBegin(GL_TRIANGLES);
        {
            glVertex3f(0,0,height);
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}


void drawSphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}


void drawSS()
{
    glColor3f(1,0,0);
    drawSquare(20);

    glRotatef(angle,0,0,1);
    glTranslatef(110,0,0);
    glRotatef(2*angle,0,0,1);
    glColor3f(0,1,0);
    drawSquare(15);

    glPushMatrix();
    {
        glRotatef(angle,0,0,1);
        glTranslatef(60,0,0);
        glRotatef(2*angle,0,0,1);
        glColor3f(0,0,1);
        drawSquare(10);
    }
    glPopMatrix();

    glRotatef(3*angle,0,0,1);
    glTranslatef(40,0,0);
    glRotatef(4*angle,0,0,1);
    glColor3f(1,1,0);
    drawSquare(5);
}

void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
            if (cameraDistance > 0)
            {
                cameraDistance -= 1;
            }
			break;

        case '2':
            cameraDistance += 1;   // no limit of taking the camera away

		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			cameraHeight -= 1.0;
			break;
		case GLUT_KEY_UP:		// up arrow key
			cameraHeight += 1.0;
			break;

		case GLUT_KEY_RIGHT:
			cameraAngle += 0.03;
			break;
		case GLUT_KEY_LEFT:
			cameraAngle -= 0.03;
			break;

		case GLUT_KEY_PAGE_UP:
			break;
		case GLUT_KEY_PAGE_DOWN:
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
            if ( paramT < A )
            {
                paramT += eps;
            }
			break;
		case GLUT_KEY_END:
            if (paramT > 0)
            {
                paramT -= eps;
            }
			break;

		default:
			break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes=1-drawaxes;
			}
			break;

		case GLUT_RIGHT_BUTTON:
			//........
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}


void draw6Squares()
{
    // draw square in z = A plane
    glPushMatrix();
    {
        glTranslatef(0, 0, A);
        drawSquare(A-paramT);
    }
    glPopMatrix();

    // drawSquare in z = -A plane
    glPushMatrix();
    {
        glTranslatef(0, 0, -A);
        drawSquare(A-paramT);
    }
    glPopMatrix();

    // draw square in y = A plane
    glPushMatrix();
    {
        glTranslatef(0, A, 0);
        glRotatef(90, 1, 0, 0);
        drawSquare(A-paramT);
    }
    glPopMatrix();

//     draw square in y = -A plane
    glPushMatrix();
    {
        glTranslatef(0, -A, 0);
        glRotatef(90, 1, 0, 0);
        drawSquare(A-paramT);
    }
    glPopMatrix();
//

//     draw square in x = A plane
    glPushMatrix();
    {
        glTranslatef(A, 0, 0);
        glRotatef(90, 0, 1, 0);
        drawSquare(A-paramT);
    }
    glPopMatrix();

    // draw square in x = -A plane
    glPushMatrix();
    {
        glTranslatef(-A, 0, 0);
        glRotatef(90, 0, 1, 0);
        drawSquare(A-paramT);
    }
    glPopMatrix();
}


void offlienDisplay()
{
//    cout << "starting of offlineDisplay" << endl;
    //clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

	//gluLookAt(100,100,100,	0,0,0,	0,0,1);
	//gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	gluLookAt(cameraDistance*cos(cameraAngle),cameraDistance*sin(cameraAngle),
	cameraHeight,

    0,0,0,	0,0,1);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	draw6Squares();
//	drawGrid();

    //glColor3f(1,0,0);
    //drawSquare(10);

//    drawSS();

    //drawCircle(30,24);

    //drawCone(20,50,24);

	//drawSphere(30,24,20);




	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}

void offlineAnimate()
{
    // We are not animating anything

//    cout << "starting of offlineAnimate " << endl;
    glutPostRedisplay();
}

void init(){

	//codes for initialization
	drawgrid=1;
	drawaxes=1;

	cameraDistance = MY_INF;
	cameraHeight = 0;

//	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;

	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){

    cout << "start of main" << endl;

	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");

	cout << "just before init" << endl;
	init();
	cout << "just after init" << endl;

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(offlienDisplay);	//display callback function
	glutIdleFunc(offlineAnimate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	cout << "just before glutMainLoop" << endl;
	glutMainLoop();		//The main loop of OpenGL
	cout << "just after glutMainLoop" << endl;

	return 0;
}






//void display(){
//
//	//clear the display
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glClearColor(0,0,0,0);	//color black
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	/********************
//	/ set-up camera here
//	********************/
//	//load the correct matrix -- MODEL-VIEW matrix
//	glMatrixMode(GL_MODELVIEW);
//
//	//initialize the matrix
//	glLoadIdentity();
//
//	//now give three info
//	//1. where is the camera (viewer)?
//	//2. where is the camera looking?
//	//3. Which direction is the camera's UP direction?
//
//	//gluLookAt(100,100,100,	0,0,0,	0,0,1);
//	//gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
//	gluLookAt(0,0,200,	0,0,0,	0,1,0);
//
//
//	//again select MODEL-VIEW
//	glMatrixMode(GL_MODELVIEW);
//
//
//	/****************************
//	/ Add your objects from here
//	****************************/
//	//add objects
//
//	drawAxes();
//	drawGrid();
//
//    //glColor3f(1,0,0);
//    //drawSquare(10);
//
//    drawSS();
//
//    //drawCircle(30,24);
//
//    //drawCone(20,50,24);
//
//	//drawSphere(30,24,20);
//
//
//
//
//	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
//	glutSwapBuffers();
//}
//
//
//void animate(){
//	angle+=0.05;
//	//codes for any changes in Models, Camera
//	glutPostRedisplay();
//}
