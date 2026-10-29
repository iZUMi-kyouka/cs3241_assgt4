// CS3241Lab4.cpp : Defines the entry point for the console application.
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#include "GL/glut.h"
#ifndef M_PI
#define M_PI 3.141592654
#endif
#elif defined(__APPLE__)
// This assignment deliberately uses Apple's legacy OpenGL/GLUT frameworks.
#define GL_SILENCE_DEPRECATION
#define GLUT_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/GLUT.h>
#else
#include <GL/glut.h>
#endif

#define MAXPTNO 1000
#define NLINESEGMENT 32
#define NOBJECTONCURVE 8

// The PDF specifies color names, not RGB/hex values.
enum : unsigned int {
    COLOR_CONTROL_LINE = 0x00FF00,              // green
    COLOR_C1_ORIGINAL_CONTROL_POINT = 0xC0C0C0, // dark grey
    COLOR_C1_ADJUSTED_CONTROL_POINT = 0xFF0000  // red
};

using namespace std;

// Global variables that you can use
struct Point {
    int x, y;
};

// Storage of control points
int nPt = 0;
std::vector<Point> ptList(MAXPTNO);

// Display options
bool isDisplayingControlPoints = true;
bool isDisplayingControlLines = true;
bool isDisplayingTangentVectors = false;
bool isDisplayingObjects = false;
bool isC1ContinuityMode = false;

void setColor(unsigned int rgb) {
    glColor3ub(static_cast<GLubyte>((rgb >> 16) & 0xFF), static_cast<GLubyte>((rgb >> 8) & 0xFF),
               static_cast<GLubyte>(rgb & 0xFF));
}

void drawRightArrow() {
    setColor(COLOR_CONTROL_LINE);
    glBegin(GL_LINE_STRIP);
    glVertex2f(0, 0);
    glVertex2f(100, 0);
    glVertex2f(95, 5);
    glVertex2f(100, 0);
    glVertex2f(95, -5);
    glEnd();
}

double fac(double n) {
    return n <= 1 ? 1 : n * fac(n - 1);
}

double choose(double n, double k) {
    return fac(n) / (fac(k) * fac(n - k));
}

double basis(int n, int i, double t) {
    return choose(n, i) * pow(1 - t, n - i) * pow(t, i);
}

void drawBezierCurve(int firstPoint, double t) {
    double rX = 0;
    double rY = 0;
    for (int i = 0; i < 4; i++) {
        double b = basis(3, i, t);
        rX += b * ptList[firstPoint + i].x;
        rY += b * ptList[firstPoint + i].y;
    }
    glVertex2d(rX, rY);
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    if (isDisplayingControlPoints) {

        glPointSize(5);
        glBegin(GL_POINTS);
        for (int i = 0; i < nPt; i++) {
            glColor3f(0, 0, 0);
            glVertex2d(ptList[i].x, ptList[i].y);
        }
        glEnd();
        glPointSize(1);
    }

    if (isDisplayingControlLines) {
        setColor(COLOR_CONTROL_LINE);
        glLineWidth(3);
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < nPt; i++) {
            glVertex2d(ptList[i].x, ptList[i].y);
        }
        glEnd();
    }

    glColor3f(0, 0, 0);
    glLineWidth(1);
    for (int firstPoint = 0; firstPoint + 3 < nPt; firstPoint += 3) {
        glBegin(GL_LINE_STRIP);
        for (int segment = 0; segment <= NLINESEGMENT; segment++) {
            double t = static_cast<double>(segment) / NLINESEGMENT;
            drawBezierCurve(firstPoint, t);
        }
        glEnd();
    }

    glPopMatrix();
    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, h, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void init(void) {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void readFile() {

    std::ifstream file;
    file.open("savefile.txt");
    file >> nPt;

    if (nPt > MAXPTNO) {
        cout << "Error: File contains more than the maximum number of points." << endl;
        nPt = MAXPTNO;
    }

    for (int i = 0; i < nPt; i++) {
        file >> ptList[i].x;
        file >> ptList[i].y;
    }
    file.close(); // is not necessary because the destructor closes the open file by default
}

void writeFile() {
    std::ofstream file;
    file.open("savefile.txt");
    file << nPt << endl;

    for (int i = 0; i < nPt; i++) {
        file << ptList[i].x << " ";
        file << ptList[i].y << endl;
    }
    file.close(); // is not necessary because the destructor closes the open file by default
}

void keyboard(unsigned char key, int /*x*/, int /*y*/) {
    switch (key) {
        case 'r':
        case 'R':
            readFile();
            break;

        case 'w':
        case 'W':
            writeFile();
            break;

        case 'T':
        case 't':
            isDisplayingTangentVectors = !isDisplayingTangentVectors;
            break;

        case 'o':
        case 'O':
            isDisplayingObjects = !isDisplayingObjects;
            break;

        case 'p':
        case 'P':
            isDisplayingControlPoints = !isDisplayingControlPoints;
            break;

        case 'L':
        case 'l':
            isDisplayingControlLines = !isDisplayingControlLines;
            break;

        case 'C':
        case 'c':
            isC1ContinuityMode = !isC1ContinuityMode;
            break;

        case 'e':
        case 'E':
            nPt = 0;
            break;

        case 'Q':
        case 'q':
            exit(0);
            break;

        default:
            break;
    }

    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    /*button: GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, or GLUT_RIGHT_BUTTON */
    /*state: GLUT_UP or GLUT_DOWN */
    enum {
        MOUSE_LEFT_BUTTON = 0,
        MOUSE_MIDDLE_BUTTON = 1,
        MOUSE_RIGHT_BUTTON = 2,
        MOUSE_SCROLL_UP = 3,
        MOUSE_SCROLL_DOWN = 4
    };
    if ((button == MOUSE_LEFT_BUTTON) && (state == GLUT_UP)) {
        if (nPt == MAXPTNO) {
            cout << "Error: Exceeded the maximum number of points." << endl;
            return;
        }
        ptList[nPt].x = x;
        ptList[nPt].y = y;
        nPt++;
    }
    glutPostRedisplay();
}

int main(int argc, char **argv) {
    cout << "CS3241 Lab 4" << endl << endl;
    cout << "Left mouse click: Add a control point" << endl;
    cout << "Q: Quit" << endl;
    cout << "P: Toggle displaying control points" << endl;
    cout << "L: Toggle displaying control lines" << endl;
    cout << "E: Erase all points (Clear)" << endl;
    cout << "C: Toggle C1 continuity" << endl;
    cout << "T: Toggle displaying tangent vectors" << endl;
    cout << "O: Toggle displaying objects" << endl;
    cout << "R: Read in control points from \"savefile.txt\"" << endl;
    cout << "W: Write control points to \"savefile.txt\"" << endl;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("CS3241 Assignment 4");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutMainLoop();

    return 0;
}
