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
#define PI (2*acos(0.0))

#define A 40
#define MY_INF 200
#define eps 1e-1
#define DRAW_SEG_COUNT 50
#define RIGHT_ANGLE_DEGREE 90
#define SIZE 109


using namespace std;

double paramT;



double cameraAngle; // this is the angle created by the camera
// in xy plane with the x axis

double cameraDistance; // distance of camera along the xy plane
double cameraHeight; // distance of camera along the z axis



int drawgrid;
int drawaxes;
double angle;


double degreeToRad( double degree)
{
    return degree * PI / 180;
}

struct Point
{
	double x,y,z;
    Point(){}
	Point(double x, double y, double z)
	{
        this->x = x;
        this->y = y;
        this->z = z;
	}

    Point operator+(const Point &p) const
    {
        return Point(x+p.x, y+p.y, z+p.z);
    }

    Point operator*(double i) const
    {
        return Point( x*i, y*i, z*i );
    }

    Point operator+=(const Point &p)
    {
        this->x += p.x;
        this->y += p.y;
        this->z += p.z;

        return *this;
    }

    Point operator-=(const Point &p)
    {
        (*this) += p * (-1);

        return *this;
    }

    Point getCrossProduct(const Point &p) const
    {
        Point ret;

        ret.x = this->y * p.z - this->z * p.y;
        ret.y = this->z * p.x - this->x * p.z;
        ret.z = this->x * p.y - this->y * p.x;

        return ret;
    }

    double getDisFromMain() const
    {
        return sqrt(x*x + y*y + z*z);
    }

    double getDotProduct(const Point &p) const
    {
        return (this->x) * p.x + (this->y) * p.y + (this->z) * p.z;
    }

    bool isParallel( const Point &p ) const
    {
        Point crossVec = getCrossProduct(p);
        if ( crossVec.getDisFromMain() <= eps )
        {
            return true;
        }

        return false;
    }

    bool isUnit()
    {
        return ( abs(getDisFromMain() - 1) <= eps );
    }

    bool isPerpendicular( const Point &p ) const
    {
        return ( abs( getDotProduct(p) ) <= eps );
    }

    void rotateAroundVector( const Point &vec, double radAngle )
    {

        if ( isParallel(vec) )
        {
            return;
        }

//        cout << vec.x << " " << vec.y << " " << vec.z << endl;

        Point unitVec = vec * ( 1 / vec.getDisFromMain() );


//        cout << unitVec.x << " " << unitVec.y << " " << unitVec.z << endl;

//        cout << unitVec.getDisFromMain() << endl;
        assert( ( "unit vector is not unit", unitVec.isUnit() ) ) ;

        Point perpendicularUnitVector = unitVec;
        assert( ( " Given vector is not perpendicular ", this->isPerpendicular(perpendicularUnitVector) )   );


        Point crossVector = perpendicularUnitVector.getCrossProduct(*this);

        this->x = this->x * cos(radAngle) + crossVector.x * sin(radAngle);
        this->y = this->y * cos(radAngle) + crossVector.y * sin(radAngle);
        this->z = this->z * cos(radAngle) + crossVector.z * sin(radAngle);

    }



};

//Point getRotatedVecAroundPerpendicularAxis( const Point &vec, const Point &perpendicularUnitVector, double radAngle )
//{
//    assert( ( " Given vector is not perpendicular ", vec.getDotProduct(perpendicularUnitVector) == 0)   );
//    assert( ( "Given vector is not unit", perpendicularUnitVector.getDisFromMain() == 1 ) ) ;
//
//    Point crossVector = perpendicularUnitVector.getCrossProduct(vec);
//
//    Point ret = vec * cos(radAngle) + crossVector * sin(radAngle);
//    return ret;
//}

const Point unitXVec(1,0,0);
const Point unitYVec(0,1,0);
const Point unitZVec(0,0,1);

const Point noTranslation(0,0,0);

struct Rotation
{
    double angle;
    Point vec;

    Rotation(){}

    Rotation(double angle, Point vec)
    {
        this->angle = angle;
        this->vec = vec;
    }
};

const Rotation noRotation(0, Point(0, 0, 0));
const Rotation rightAngleRotAroundXAxis( RIGHT_ANGLE_DEGREE, unitXVec );
const Rotation rightAngleRotAroundYAxis( RIGHT_ANGLE_DEGREE, unitYVec );
const Rotation rightAngleRotAroundZAxis( RIGHT_ANGLE_DEGREE, unitZVec );

class CameraOrientation
{

private:


    Point cameraPosition = Point(-MY_INF, 0, 0);
    Point upDirection = unitZVec;
    Point rightDirection =  unitYVec * (-1);
    Point forwardDirection = unitXVec;

public:

    Point getCameraPosition()
    {
        return cameraPosition;
    }

    Point getForwardDirection()
    {
        return forwardDirection;
    }

    Point getUpDirection()
    {
        return upDirection;
    }

    void moveForward()
    {
        cameraPosition += forwardDirection;
    }

    void moveBackward()
    {
        cameraPosition -= forwardDirection;
    }

    void moveRight()
    {
        cameraPosition += rightDirection;
    }

    void moveLeft()
    {
        cameraPosition -= rightDirection;
    }

    void moveUp()
    {
        cameraPosition += upDirection;
    }

    void moveDown()
    {
        cameraPosition -= upDirection;
    }


    void rotateAround( Point axis, double radAng )
    {

        assert( upDirection.isUnit() );
        assert( rightDirection.isUnit() );
        assert( forwardDirection.isUnit() );


        upDirection.rotateAroundVector(axis, radAng);
        rightDirection.rotateAroundVector(axis, radAng);
        forwardDirection.rotateAroundVector(axis, radAng);

        assert( upDirection.isUnit() );
        assert( rightDirection.isUnit() );
        assert( forwardDirection.isUnit() );
    }


    void rotateLeft()
    {
        rotateAround(upDirection, eps);
    }

    void rotateRight()
    {
        rotateAround(upDirection, -eps);
    }


    // These 4 methods are not tested yet
    void rotateUp()
    {
        rotateAround(rightDirection, eps);
    }

    void rotateDown()
    {
        rotateAround(rightDirection, -eps);
    }

    void tiltClockWise()
    {
        rotateAround(forwardDirection, eps);
    }

    void tiltCounterClockwise()
    {
        rotateAround(forwardDirection, -eps);
    }
};


CameraOrientation cameraOrientation;


void myLookAt( CameraOrientation cameraOrientation )
{
    Point cameraPosition = cameraOrientation.getCameraPosition();
    Point forwardDirection = cameraOrientation.getForwardDirection();
    Point upDirection = cameraOrientation.getUpDirection();

    Point lookAtPoint = cameraPosition + forwardDirection;

    gluLookAt(cameraPosition.x, cameraPosition.y, cameraPosition.z,
    lookAtPoint.x, lookAtPoint.y, lookAtPoint.z,
    upDirection.x, upDirection.y, upDirection.z);
}



void myRotate(Rotation rot)
{
    double angle = rot.angle;
    Point vec = rot.vec;
    glRotated( angle, vec.x, vec.y, vec.z );
}

void myTranslate( Point vec )
{
    glTranslated(vec.x, vec.y, vec.z);
}

void drawRotatedTranslated( Rotation rot, Point trans, void (*drawFunc)() )
{
    glPushMatrix();{
        myTranslate(trans);
        myRotate(rot);
        drawFunc();
    }glPopMatrix();
}

void drawRotatedTranslated(double angle, double rx, double ry, double rz, double tx, double ty, double tz,
void (*drawFunc)() )
{
    Point rotateVec(rx, ry, rz);
    Rotation rotation(angle, rotateVec);
    Point translateVec(tx, ty, tz);
    drawRotatedTranslated(rotation, translateVec, drawFunc);
}


void drawRotatedTranslated(Rotation rotation, double tx, double ty, double tz, void (*drawFunc)() )
{
    Point translateVec(tx,ty,tz);
    drawRotatedTranslated(rotation, translateVec, drawFunc);
}




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
void drawQuad( Point a, Point b, Point c, Point d )
{
    glBegin(GL_QUADS);{
		glVertex3f( a.x, a.y, a.z);
		glVertex3f( b.x, b.y, b.z);
		glVertex3f( c.x, c.y, c.z);
		glVertex3f( d.x, d.y, d.z);
	}glEnd();
}



void drawSquare(double a) // This method draws a square in the z = 0 plane
// center of the square is in (0,0,0)
{
	glBegin(GL_QUADS);{
		glVertex3f( a, a,0);
		glVertex3f( a,-a,0);
		glVertex3f(-a,-a,0);
		glVertex3f(-a, a,0);
	}glEnd();
}

void drawCustomSquare()
{
    drawSquare(A-paramT);
}


void draw6Squares()
{
    // Custom square is on the xy plane


    // Will draw 2 squares parallel to the xy plane
    drawRotatedTranslated(noRotation, Point(0, 0, -A), drawCustomSquare); // at z = -A plane
    drawRotatedTranslated(noRotation, Point(0, 0, A), drawCustomSquare); // at z = A plane

    // Will draw 2 squares parallel to the yz plane
    // Need to rotate the custom square around the Y axis
    drawRotatedTranslated(rightAngleRotAroundYAxis, Point(-A, 0, 0), drawCustomSquare); // at x = -A plane
    drawRotatedTranslated(rightAngleRotAroundYAxis, Point(A, 0, 0), drawCustomSquare); // at x = A plane


    // Will draw 2 squares parallel to the zx plane
    // Need to rotate the custom square around the X axis
    drawRotatedTranslated(rightAngleRotAroundXAxis, Point(0, -A, 0), drawCustomSquare); // at y = -A plane
    drawRotatedTranslated(rightAngleRotAroundXAxis, Point(0, A, 0), drawCustomSquare); // at y = A plane
}




void drawCircle(double radius,int segments)
{
    assert( segments < SIZE );


    Point points[SIZE];
    glColor3f(0.7,0.7,0.7);
    //generate points
    for(int i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*PI);
        points[i].y=radius*sin(((double)i/(double)segments)*2*PI);
    }
    //draw segments using generated points
    for(int i=0;i<segments;i++)
    {
        glBegin(GL_LINES);
        {
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}

void drawOneFourthCylinder(double radius, double height, int segments) // this method draws a cylinder along the z axis
// the middle point of the cylinder is (0,0,0)
{

    assert( segments < SIZE );
    Point pointAr[3][SIZE];

    // generate upper and lower cirlce
    for (int i = 0; i <= segments; i++)
    {
        pointAr[0][i].x = pointAr[1][i].x = radius*cos(((double)i/(double)segments)*2*PI);
        pointAr[0][i].y = pointAr[1][i].y = radius*sin(((double)i/(double)segments)*2*PI);

        // fixing the z coordinate of upper circle
        pointAr[0][i].z = height/2;

        // fixing the z coordinate of lower circle
        pointAr[1][i].z = -height/2;
    }


    // I am drawing the one foruth of the cylinder
    for (int i = 0; i <= segments/4; i++)
    {
        drawQuad( pointAr[0][i], pointAr[0][i+1], pointAr[1][i+1], pointAr[1][i] );
    }
}

// This custom one fourth cylinder is along the Z axis
// It covers the (+x, +y) plane
void drawCustomOneFourthCylinder()
{
    glPushMatrix();{
        glTranslated(A-paramT, A-paramT, 0);
        drawOneFourthCylinder(paramT, 2*(A-paramT), DRAW_SEG_COUNT);
    }glPopMatrix();
}

// These 4 one fourth cylinders are alongh the Z axis
void draw4OneFourthCylinderAlongZAxis()
{
    for ( int rotAngle = 0; rotAngle < 360; rotAngle += RIGHT_ANGLE_DEGREE )
    {
        glPushMatrix();{
            Rotation rotation( rotAngle, unitZVec  );
            myRotate(rotation);
            drawCustomOneFourthCylinder();
        }glPopMatrix();
    }
}




void draw12OneFourthCylinders()
{
    glColor3f(0, 1, 0); // cylinders have color green


    draw4OneFourthCylinderAlongZAxis();
    drawRotatedTranslated(rightAngleRotAroundXAxis, noTranslation, draw4OneFourthCylinderAlongZAxis);
    drawRotatedTranslated(rightAngleRotAroundYAxis, noTranslation, draw4OneFourthCylinderAlongZAxis);

}


// this one eighth sphere's center is at (0,0,0)
// it covers the positive sides of x, y, z axis
void drawOneEighthSphere(double radius,int slices,int stacks)
{
    assert( slices < SIZE );
    assert( stacks < SIZE );

	Point points[SIZE][SIZE];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(PI/2));
		r=radius*cos(((double)i/(double)stacks)*(PI/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*PI);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*PI);
			points[i][j].z=h;
		}
	}


	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
//        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0; j <= slices/4; j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);


                // We will not draw lower hemisphere

			}glEnd();
		}
	}
}

// This one eighth sphere is drawn with the center (A-paramT, A-paramT, A-paramT)
// It is alongh the (+x,+y,+Z)
void drawCustomOneEighthSphere()
{
    glPushMatrix();{
        glTranslated(A-paramT, A-paramT, A-paramT);
        drawOneEighthSphere(paramT, DRAW_SEG_COUNT, DRAW_SEG_COUNT);
    }glPopMatrix();
}


void drawUpper4OneEighthSphere()
{
    Point rotAxis(0,0,1);
    for ( int rotAngle = 0; rotAngle < 360; rotAngle += RIGHT_ANGLE_DEGREE )
    {
        glPushMatrix();{
            Rotation rotation(rotAngle, rotAxis);
            myRotate(rotation);
            drawCustomOneEighthSphere();
        }glPopMatrix();
    }
}

void drawLower4OneEighthSphere()
{
    glPushMatrix();{
        // Will rotate 180 degree around the x axis
        myRotate( rightAngleRotAroundXAxis );
        myRotate( rightAngleRotAroundXAxis );
        drawUpper4OneEighthSphere();
    }glPopMatrix();
}


void draw8OneEighthSpheres()
{
    glColor3f(1, 0, 0); // cylinders have color green

    drawUpper4OneEighthSphere();
    drawLower4OneEighthSphere();


}



void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
            cameraOrientation.rotateLeft();
			break;
        case '2':
            cameraOrientation.rotateRight();
            break;

        case '3':
            cameraOrientation.rotateUp();
			break;
        case '4':
            cameraOrientation.rotateDown();
            break;

        case '5':
            cameraOrientation.tiltClockWise();
			break;
        case '6':
            cameraOrientation.tiltCounterClockwise();
            break;

		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			cameraOrientation.moveBackward();
			break;
		case GLUT_KEY_UP:		// up arrow key
			cameraOrientation.moveForward();
			break;

		case GLUT_KEY_RIGHT:
			cameraOrientation.moveRight();
			break;
		case GLUT_KEY_LEFT:
			cameraOrientation.moveLeft();
			break;

		case GLUT_KEY_PAGE_UP:
            cameraOrientation.moveUp();
			break;
		case GLUT_KEY_PAGE_DOWN:
            cameraOrientation.moveDown();
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
            if ( paramT < A )
            {
                paramT += eps;
                cout << "paramT = " << paramT << endl;
            }
			break;
		case GLUT_KEY_END:
            if (paramT > 0)
            {
                paramT -= eps;
                cout << "paramT = " << paramT << endl;
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
//				drawaxes=1-drawaxes;
			}
			break;

		case GLUT_RIGHT_BUTTON:
//			........
			break;

		case GLUT_MIDDLE_BUTTON:
//			........
			break;

		default:
			break;
	}
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
//	gluLookAt(cameraDistance*cos(cameraAngle),cameraDistance*sin(cameraAngle),
//	cameraHeight,
//
//    0,0,0,	0,0,1);


    myLookAt( cameraOrientation );


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects



	drawAxes();
	draw6Squares();
    draw12OneFourthCylinders();
    draw8OneEighthSpheres();


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

//				lines parallel to Y-axis
				glVertex3f(i*10, -90, 0);
				glVertex3f(i*10,  90, 0);

//				lines parallel to X-axis
				glVertex3f(-90, i*10, 0);
				glVertex3f( 90, i*10, 0);
			}
		}glEnd();
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



void display(){

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
	gluLookAt(0,0,200,	0,0,0,	0,1,0);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************ŝ
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	drawGrid();

    //glColor3f(1,0,0);
    //drawSquare(10);

    drawSS();

    //drawCircle(30,24);

    //drawCone(20,50,24);

	//drawSphere(30,24,20);




	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	angle+=0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}




void drawCone(double radius,double height,int segments)
{
    assert(segments <= 100);

    int i;
    double shade;
    Point points[109];
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*PI);
        points[i].y=radius*sin(((double)i/(double)segments)*2*PI);
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



