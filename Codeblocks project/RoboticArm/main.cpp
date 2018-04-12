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

#define DEBUG

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
#define DRAW_SEG_COUNT 5
#define RIGHT_ANGLE_DEGREE 90
#define SIZE 109




using namespace std;


#ifdef DEBUG
    #define dbg(x) cout<<#x<<" : "<<x<<endl
    #define dbg2(x, y) cout<<#x<<" : "<<(x)<<", "<<#y<<" : "<<(y)<<endl
    #define dbg3(x, y, z) cout<<#x<<" : "<<(x)<<", "<<#y<<" : "<<(y) << ", " << #z << " : " << (z) <<endl
    #define dbg4(a,b,c,d) cout<<#a<<" : "<<(a)<<", "<<#b<<" : "<<(b)<<", "<<#c<<" : "<<(c)<<", "<<#d<<": "<<(d)<<endl
    #define dbgAr(ar, si, ei) cout<<#ar<<" : "<<endl; prAr(ar, si, ei)
#else
    #define dbg(x)
    #define dbg2(x, y)
    #define dbg3(x, y, z)
    #define dbg4(a,b,c,d)
    #define dbgAr(ar, si, ei)
#endif // DEBUG



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



class Angle
{
    const double minVal = 0;
    const double maxVal = RIGHT_ANGLE_DEGREE;
    const double defaultVal = RIGHT_ANGLE_DEGREE/2;

    double val = defaultVal;

public:

    double getVal()
    {
        return val;
    }

    void increase()
    {
        if (val < maxVal)
        {
            val += 10*eps;
        }
    }

    void decrease()
    {
        if ( val > minVal )
        {
            val -= 10*eps;
        }
    }
};


class CameraOrientation
{

private:




    Point upDirection = unitZVec;
    const Point lookAtPoint = Point(0,0,0);
    const double heightDisChange = eps * 10;



    double disOnXY = MY_INF;
    double angleWithX = 0;
    double height = MY_INF;


public:

    Point getCameraPosition()
    {
        double camX = disOnXY * cos(angleWithX);
        double camY = disOnXY * sin(angleWithX);
        double camZ = height;

        Point camPos(camX, camY, camZ);
        return camPos;
    }

    Point getLookAtPoint()
    {
        return lookAtPoint;
    }


    void decreaseDisOnXY()
    {
        if ( disOnXY > 0 )
        {
            disOnXY -= heightDisChange;
        }
    }

    void increaseDisOnXY()
    {
        disOnXY += heightDisChange;
    }

    void increaseAngleWithX()
    {
        angleWithX += eps;
    }

    void decreaseAngleWithX()
    {
        angleWithX -= eps;
    }

    void increaseHeight()
    {
        height += heightDisChange;
    }

    void decreaseHeight()
    {
        height -= heightDisChange;
    }



    Point getUpDirection()
    {
        return upDirection;
    }


};




CameraOrientation cameraOrientation;


void myLookAt( CameraOrientation cameraOrientation )
{
    Point cameraPosition = cameraOrientation.getCameraPosition();
    Point upDirection = cameraOrientation.getUpDirection();
    Point lookAtPoint = cameraOrientation.getLookAtPoint();

//    dbg(cameraPosition.z);
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

void myRotate(double angle, Point vec)
{
    glRotated(angle, vec.x, vec.y, vec.z);
}

void myRotate(Angle angle, Point vec)
{
    glRotated(angle.getVal(), vec.x, vec.y, vec.z);
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





class Robot
{
private:


    const double scaleSize = 5;

    const double bigRadius = 10;
    const double smallRadius = 10;

    Angle bigArmAngleAroundX;
    Angle bigArmAngleAroundY;

    Angle smallArmAngleAroundX;

    Angle triangleAngleAroundZ;


public:

    void increaseBigArmAngleAroundX()
    {
        bigArmAngleAroundX.increase();
    }

    void decreaseBigArmAngleAroundX()
    {
        bigArmAngleAroundX.decrease();
    }

    void increaseBigArmAngleAroundY()
    {
        bigArmAngleAroundY.increase();
    }

    void decreaseBigArmAngleAroundY()
    {
        bigArmAngleAroundY.decrease();
    }

    void increaseSmallArmAngleAroundX()
    {
        smallArmAngleAroundX.increase();
    }

    void decreaseSmallArmAngleAroundX()
    {
        smallArmAngleAroundX.decrease();
    }

    void decreaseTriangleAngleAroundZ()
    {
        triangleAngleAroundZ.decrease();
    }

    void increaseTriangleAngleAroundZ()
    {
        triangleAngleAroundZ.increase();
    }

    void drawEllipsoid(double radius) // Two end points of the arms are at (0,0,0) and (0,0,-2*radius*scaleSize)
    {
        glPushMatrix();{
            glTranslated(0, 0, -radius*scaleSize);
            glScaled(1, 1, scaleSize);
            glutWireSphere(radius, DRAW_SEG_COUNT, DRAW_SEG_COUNT);
        }glPopMatrix();
    }

    void drawBigArmAndBelow()
    {
        drawEllipsoid(bigRadius); // Two end points of the arms are at (0,0,0) and (0,0,-2*bigRadius*scaleSize)
        glPushMatrix();{
            glTranslated(0,0,-2*bigRadius*scaleSize);
            myRotate(smallArmAngleAroundX.getVal(), unitXVec);
            drawSmallArmAndBelow();
        }glPopMatrix();
    }

    void drawSmallArmAndBelow()
    {
        drawEllipsoid(bigRadius); // small arm is drawn
        glPushMatrix();{
            glTranslated( 0,0,-2*bigRadius*scaleSize );
            myRotate(triangleAngleAroundZ, unitZVec);
            drawTriangleAndBelow();
        }glPopMatrix();
    }

    void drawTriangleAndBelow()
    {
        drawTriangle();
    }

    void drawTriangle()
    {
        double halfBase = 30;
        glBegin(GL_TRIANGLES);{
            glVertex3d(0,0,0);
            glVertex3d(0,-halfBase,-halfBase);
            glVertex3d(0,halfBase,-halfBase);
        }glEnd();
    }



    void draw()
    {
        glPushMatrix();{
            myRotate(bigArmAngleAroundX.getVal(), unitXVec);
            myRotate(bigArmAngleAroundY.getVal(), unitYVec);
            drawBigArmAndBelow();
        }glPopMatrix();
    }
};


Robot robot;



void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
            robot.decreaseBigArmAngleAroundX();
			break;
        case '2':
            robot.increaseBigArmAngleAroundX();
            break;

        case '3':
            robot.decreaseSmallArmAngleAroundX();
			break;
        case '4':
            robot.increaseSmallArmAngleAroundX();
            break;

        case '5':
            robot.increaseTriangleAngleAroundZ();
			break;
        case '6':
            robot.decreaseTriangleAngleAroundZ();
            break;

        case 'q':
            robot.increaseBigArmAngleAroundY();
            break;
        case 'w':
            robot.decreaseBigArmAngleAroundY();
            break;

		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
            cameraOrientation.decreaseHeight();
			break;
		case GLUT_KEY_UP:		// up arrow key
            cameraOrientation.increaseHeight();
			break;

		case GLUT_KEY_RIGHT:
			cameraOrientation.increaseAngleWithX();
			break;
		case GLUT_KEY_LEFT:
			cameraOrientation.decreaseAngleWithX();
			break;

		case GLUT_KEY_PAGE_UP:
            cameraOrientation.increaseDisOnXY();
			break;
		case GLUT_KEY_PAGE_DOWN:
            cameraOrientation.decreaseDisOnXY();
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

	drawAxes();
    robot.draw();


	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}

void offlineAnimate()
{

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


