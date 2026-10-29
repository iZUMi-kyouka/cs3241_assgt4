// AI assistance was used for design consultation, debugging, and researching implementation details.
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <random>

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

#define MAXPTNO 10000
#define NLINESEGMENT 32
#define NOBJECTONCURVE 8

// The PDF specifies color names, not RGB/hex values.
enum : unsigned int {
    COLOR_CONTROL_LINE = 0x00FF00,              // green
    COLOR_C1_ORIGINAL_CONTROL_POINT = 0xC0C0C0, // dark grey
    COLOR_C1_ADJUSTED_CONTROL_POINT = 0xFF0000  // red
};

enum : unsigned int { COLOR_CRANE_CORAL = 0xF38B7C, COLOR_CRANE_IVORY = 0xF5EBD6 };

using namespace std;

// Global variables that you can use
struct Point {
    int x, y;
};

struct Vector2 {
    double x, y;
};

// Storage of control points
int nPt = 0;
std::vector<Point> ptList(MAXPTNO);
std::vector<Point> c1PtList;
std::array<unsigned int, ((MAXPTNO - 1) / 3) * NOBJECTONCURVE> craneColors;

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

const std::vector<Point> &getDrawingPoints() {
    if (!isC1ContinuityMode) {
        return ptList;
    }

    c1PtList.assign(ptList.begin(), ptList.begin() + nPt);
    for (int i = 4; i < nPt; i += 3) {
        // Reflect the incoming handle across the shared endpoint to match derivatives.
        c1PtList[i].x = 2 * ptList[i - 1].x - ptList[i - 2].x;
        c1PtList[i].y = 2 * ptList[i - 1].y - ptList[i - 2].y;
    }
    return c1PtList;
}

Vector2 evaluateBezierPosition(const std::vector<Point> &points, int firstPoint, double t) {
    Vector2 position{0, 0};
    for (int i = 0; i < 4; i++) {
        double b = basis(3, i, t);
        position.x += b * points[firstPoint + i].x;
        position.y += b * points[firstPoint + i].y;
    }
    return position;
}

Vector2 evaluateBezierDerivative(const std::vector<Point> &points, int firstPoint, double t) {
    Vector2 derivative{0, 0};
    for (int i = 0; i < 3; i++) {
        double b = 3 * basis(2, i, t);
        derivative.x += b * (points[firstPoint + i + 1].x - points[firstPoint + i].x);
        derivative.y += b * (points[firstPoint + i + 1].y - points[firstPoint + i].y);
    }
    return derivative;
}

void drawBezierCurve(const std::vector<Point> &points, int firstPoint, double t) {
    Vector2 position = evaluateBezierPosition(points, firstPoint, t);
    glVertex2d(position.x, position.y);
}

void drawBezierTangents(const std::vector<Point> &points, int firstPoint) {
    for (int sample = 0; sample < NOBJECTONCURVE; sample++) {
        double t = static_cast<double>(sample) / NOBJECTONCURVE;
        Vector2 position = evaluateBezierPosition(points, firstPoint, t);
        Vector2 derivative = evaluateBezierDerivative(points, firstPoint, t);
        // A stationary point has no defined tangent direction.
        if (derivative.x * derivative.x + derivative.y * derivative.y <= 1e-12) {
            continue;
        }

        double angle = std::atan2(derivative.y, derivative.x) * 180 / std::acos(-1.0);
        glPushMatrix();
        glTranslated(position.x, position.y, 0);
        glRotated(angle, 0, 0, 1);
        drawRightArrow();
        glPopMatrix();
    }
}

void initializeCraneColors() {
    std::random_device seed;
    std::mt19937 generator(seed());
    std::uniform_int_distribution<int> chooseColor(0, 1);
    for (unsigned int &color : craneColors) {
        color = chooseColor(generator) == 0 ? COLOR_CRANE_CORAL : COLOR_CRANE_IVORY;
    }
}

void setCraneShade(unsigned int baseColor, double shade) {
    // Light faces blend toward white; dark faces retain the paper's hue.
    double red = (baseColor >> 16) & 0xFF;
    double green = (baseColor >> 8) & 0xFF;
    double blue = baseColor & 0xFF;
    if (shade > 1) {
        red += (255 - red) * (shade - 1);
        green += (255 - green) * (shade - 1);
        blue += (255 - blue) * (shade - 1);
    } else {
        red *= shade;
        green *= shade;
        blue *= shade;
    }
    glColor3d(red / 255, green / 255, blue / 255);
}

void drawOrigamiCrane(unsigned int baseColor) {
    // Local +x points toward the beak; negative y raises the wings on screen.
    // Tail to beak spans 50 units, with the body centered on the curve.
    static const Vector2 vertices[] = {
        {-25, 7},   // 0: tail tip
        {-11, 7},   // 1: back of body
        {-6, 10},   // 2: bottom of body
        {7, 6},     // 3: front of body
        {-1, 0},    // 4: central fold
        {3, -3},    // 5: shoulder
        {-22, -29}, // 6: broad wing tip
        {-9, -7},   // 7: broad wing fold
        {11, -20},  // 8: far wing tip
        {14, -13},  // 9: outside neck bend
        {16, -11},  // 10: inside neck bend
        {18, -13},  // 11: head
        {25, -10},  // 12: beak
        {18, -10}   // 13: underside of head
    };
    struct Face {
        int a, b, c;
        double shade;
    };
    static const Face faces[] = {{4, 8, 3, 0.87}, // far wing
                                 {0, 1, 4, 0.94}, // tail
                                 {6, 1, 7, 1.25}, // broad wing's light face
                                 {6, 7, 5, 1.08},    {1, 4, 7, 0.92},   {7, 4, 5, 0.82},
                                 {1, 2, 4, 0.90}, // folded body
                                 {2, 3, 4, 1.12},    {4, 3, 5, 0.96},   {5, 3, 9, 0.88}, // neck
                                 {3, 10, 9, 1.18},   {9, 10, 11, 1.12}, // head and beak
                                 {10, 13, 11, 0.96}, {11, 13, 12, 1.20}};
    glBegin(GL_TRIANGLES);
    glNormal3d(0, 0, 1);
    for (const Face &face : faces) {
        setCraneShade(baseColor, face.shade);
        for (int index : {face.a, face.b, face.c}) {
            glVertex2d(vertices[index].x, vertices[index].y);
        }
    }
    glEnd();

    static const int creases[][2] = {{6, 7}, {7, 4}, {1, 4}, {4, 2}, {4, 3}, {5, 9}};
    GLfloat previousLineWidth = 1;
    glGetFloatv(GL_LINE_WIDTH, &previousLineWidth);
    glLineWidth(1);
    setCraneShade(baseColor, 0.58);
    glBegin(GL_LINES);
    for (const auto &crease : creases) {
        glVertex2d(vertices[crease[0]].x, vertices[crease[0]].y);
        glVertex2d(vertices[crease[1]].x, vertices[crease[1]].y);
    }
    glEnd();
    glLineWidth(previousLineWidth);
}

void drawBezierObjects(const std::vector<Point> &points, int firstPoint) {
    for (int sample = 0; sample < NOBJECTONCURVE; sample++) {
        double t = static_cast<double>(sample) / NOBJECTONCURVE;
        Vector2 position = evaluateBezierPosition(points, firstPoint, t);
        Vector2 derivative = evaluateBezierDerivative(points, firstPoint, t);
        if (derivative.x * derivative.x + derivative.y * derivative.y <= 1e-12) {
            continue;
        }

        // Rotation maps local x onto T and local y onto N = (-Ty, Tx).
        double angle = std::atan2(derivative.y, derivative.x) * 180 / std::acos(-1.0);
        glPushMatrix();
        glTranslated(position.x, position.y, 0);
        glRotated(angle, 0, 0, 1);
        drawOrigamiCrane(craneColors[(firstPoint / 3) * NOBJECTONCURVE + sample]);
        glPopMatrix();
    }
}

void display(void) {
    const std::vector<Point> &points = getDrawingPoints();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();

    if (isDisplayingControlLines) {
        setColor(COLOR_CONTROL_LINE);
        glLineWidth(3);
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < nPt; i++) {
            glVertex2d(points[i].x, points[i].y);
        }
        glEnd();
    }

    glColor3f(0, 0, 0);
    glLineWidth(1);
    for (int firstPoint = 0; firstPoint + 3 < nPt; firstPoint += 3) {
        glBegin(GL_LINE_STRIP);
        for (int segment = 0; segment <= NLINESEGMENT; segment++) {
            double t = static_cast<double>(segment) / NLINESEGMENT;
            drawBezierCurve(points, firstPoint, t);
        }
        glEnd();
    }

    if (isDisplayingObjects) {
        for (int firstPoint = 0; firstPoint + 3 < nPt; firstPoint += 3) {
            drawBezierObjects(points, firstPoint);
        }
    }

    if (isDisplayingTangentVectors) {
        glLineWidth(1);
        for (int firstPoint = 0; firstPoint + 3 < nPt; firstPoint += 3) {
            drawBezierTangents(points, firstPoint);
        }
    }

    if (isDisplayingControlPoints) {
        glPointSize(5);
        glBegin(GL_POINTS);
        for (int i = 0; i < nPt; i++) {
            if (isC1ContinuityMode && i >= 4 && (i - 4) % 3 == 0) {
                setColor(COLOR_C1_ORIGINAL_CONTROL_POINT);
            } else {
                glColor3f(0, 0, 0);
            }
            glVertex2d(ptList[i].x, ptList[i].y);
        }
        if (isC1ContinuityMode) {
            setColor(COLOR_C1_ADJUSTED_CONTROL_POINT);
            for (int i = 4; i < nPt; i += 3) {
                glVertex2d(points[i].x, points[i].y);
            }
        }
        glEnd();
        glPointSize(1);
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
    initializeCraneColors();
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
            c1PtList.clear();
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
