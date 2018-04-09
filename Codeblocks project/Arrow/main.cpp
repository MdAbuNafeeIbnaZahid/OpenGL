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

#define A 100
#define MY_INF 200
#define eps 1e-1
#define DRAW_SEG_COUNT 50
#define RIGHT_ANGLE_DEGREE 90
#define SIZE 109


using namespace std;




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


    Point cameraPosition = Point(0, 0, MY_INF);
    Point upDirection = unitYVec;
    Point rightDirection =  unitXVec;
    Point forwardDirection = unitZVec * (-1);

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




class Arrow
{
    double len = 4;
    Point tail = Point(0,0,0);
    Point dir = Point(1, 0, 0 );

public:
    Point getTail()
    {
        return tail;
    }

    Point getDir()
    {
        return dir;
    }


    void draw()
    {
        double degreeAngle = acos(dir.x) * 180 / PI;
        if ( dir.y < 0 )
        {
            degreeAngle *= -1;
        }
//        cout << "degreeAngle = " << degreeAngle << endl;
//        cout << "tail.x = " << tail.x << endl;
//        cout << "tail.y = " << tail.y << endl;

//        cout << "dir.x = " << dir.x << endl;
//        cout << "dir.y = " << dir.y << endl;

        glPushMatrix();{
            myTranslate(tail);
            Rotation rotation(degreeAngle, unitZVec);
            myRotate(rotation);
            drawWithoutRotationTranslation();
        }glPopMatrix();
    }

    void drawWithoutRotationTranslation()
    {
        glColor3d(100, 100, 100); // arrow tail need to white
        glBegin(GL_LINES);{
            glVertex2d(0, 0);
            glVertex2d(2*len, 0);
        }glEnd();

        glColor3d(100, 0, 0); // arrow head need to red
        glBegin(GL_TRIANGLES);{
            glVertex2d(2*len, 0);
            glVertex2d(len, -len);
            glVertex2d(len, len);
        }glEnd();
    }

    void moveForward()
    {
        tail +=  dir * (0.1) ;



        if ( tail.x < -A || tail.x > A )
        {
            dir.x *= -1;
        }

        if ( tail.y < -A || tail.y > A)
        {
            dir.y *= -1;
        }
    }


    void rotateCounterClockwise()
    {
        dir.rotateAroundVector(unitZVec, eps);
        draw();
    }

    void rotateClockWise()
    {
        dir.rotateAroundVector(unitZVec, -eps);
        draw();
    }
};


Arrow arrow;





void drawBoundary()
{
    glColor3d(1, 1, 1);
    glBegin(GL_LINES);
    {
        glVertex2d(-A, -A);
        glVertex2d(A, -A);

        glVertex2d(A, -A);
        glVertex2d(A, A);

        glVertex2d(A, A);
        glVertex2d(-A, A);

        glVertex2d(-A, A);
        glVertex2d(-A, -A);

    }glEnd();
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
			arrow.rotateClockWise();
			break;
		case GLUT_KEY_LEFT:
			arrow.rotateCounterClockwise();
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
			break;
		case GLUT_KEY_END:
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

    drawBoundary();
    arrow.draw();


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

    arrow.moveForward();
    arrow.draw();

    glutPostRedisplay();
}

void init(){

	//codes for initialization


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
	glutInitWindowSize(5*A, 5*A);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("Arrow");

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


