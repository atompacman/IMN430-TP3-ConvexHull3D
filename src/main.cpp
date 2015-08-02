/************************************************************************/
/* PRESS 'm' TO TOGGLE BETWEEN VISUALIZATION MODES                      */
/************************************************************************/

#include <fstream>
#include <vector>
#include <windows.h>

#include "ConvexHull3D.h"
#include "Point.h"
#include "Vector.h"

#include "GL/gl.h"
#include "GL/glu.h"
#include "glut/glut.h"

#define GLUT_DISABLE_ATEXIT_HACK
#define PI 3.14159265359
#define DEG_2_RAD 0.01745329251994329547437168059786927187815

#define WIN_WIDTH  1000
#define WIN_HEIGHT  800

// Mouse
int g_MouseButton = GLUT_LEFT_BUTTON;
int g_MouseState  = GLUT_DOWN;
int g_MouseX      = 0;
int g_MouseY      = 0;

// Camera info
int g_CamTheta = 45;
int g_CamPhi = 90;
int g_CamZoom = 450;

// Visualisation mode (press 'm')
enum Mode { WIREFRAME, FACETS, POINTS_ONLY };
Mode g_Mode = FACETS;

// Points
Point g_Centroid(0,0,0);

void mouseButton(int i_Button, int i_State, int i_X, int i_Y)
{
    g_MouseButton = i_Button;
    g_MouseState = i_State;

    if (i_State == GLUT_DOWN) {
        g_MouseX = i_X;
        g_MouseY = i_Y;
    }
}

void mouseMove(int i_X, int i_Y)
{
    int	dx =  i_X - g_MouseX;
    int	dy = -i_Y + g_MouseY;
    g_MouseX = i_X;
    g_MouseY = i_Y;

    switch (g_MouseButton) {
    // rotation
    case GLUT_LEFT_BUTTON:
        g_CamTheta = (g_CamTheta - dx) % 360;
        g_CamPhi = max(-90, min(90, g_CamPhi - dy));
        break;

    // Zoom
    case GLUT_RIGHT_BUTTON:
        g_CamZoom = max(1, g_CamZoom + dx - dy);
        break;

    default:
        return;
    }

    glutPostRedisplay();
}

void handleKeyboard(unsigned char key, int, int)
{
    switch (key) {
    case 'm':
        g_Mode = Mode((g_Mode + 1) % 3);
        break;
    }
    glutPostRedisplay();
}

inline void addCenteredVertex(sPoint i_Pt)
{
    glVertex3d(i_Pt->m_x - g_Centroid.m_x, 
               i_Pt->m_y - g_Centroid.m_y,
               i_Pt->m_z - g_Centroid.m_z);
}

void drawConvexHull()
{
    // For each facet
    for (sptr<Facet>& facet : g_ConvexHull->m_Facets) {
        if (!facet) {
            continue;
        }

        // Draw each vertex
        glBegin(g_Mode == FACETS ? GL_POLYGON : GL_LINES);

        // Get normal
        Vector& normal(facet->m_Normal);

        sptr<HalfEdge> edge(facet->m_AnEdge);
        do {
            addCenteredVertex(edge->m_Origin);
            glNormal3d(normal.m_x, normal.m_y, normal.m_z);
            edge = edge->m_Next;
        } while (edge != facet->m_AnEdge);

        glEnd();
    }
}

void draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 1, 1, 3000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(
        g_CamZoom*sin(DEG_2_RAD * g_CamTheta)*cos(DEG_2_RAD * g_CamPhi),
        g_CamZoom*sin(DEG_2_RAD * g_CamPhi),
        g_CamZoom*cos(DEG_2_RAD * g_CamTheta)*cos(DEG_2_RAD * g_CamPhi),
        0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // Draw points
    glBegin(GL_POINTS);
    for (sPoint pnt : g_Pts) {
        addCenteredVertex(pnt);
    }
    glEnd();

    // Toggle lightning
    if (g_Mode == FACETS) {
        glEnable(GL_LIGHTING);
    } else {
        glDisable(GL_LIGHTING);
    }

    // Draw convex hull
    if (g_Mode != POINTS_ONLY) {
        drawConvexHull();
    }

    glutSwapBuffers();
}

void initOpenGL(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    if (!glutCreateWindow("ConvexHull3D")) {
        std::cerr << "Could not create OpenGL window" << std::endl;
        exit(0);
    }

    // Activate z-buffer
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    // Callbacks
    glutDisplayFunc(draw);
    glutKeyboardFunc(handleKeyboard);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMove);

    // Lightning
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glFrontFace(GL_CCW);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHT0);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
}

void readVertexFile(const char* i_Filepath)
{
    // Open file
    std::ifstream file(i_Filepath);

    // Check that it opened correctly
    if (file.bad()) {
        std::cerr << "Invalid file \"" << i_Filepath << "\"" << std::endl;
    }

    // Read points
    double x, y, z;
    while (!file.eof()) {
        file >> x;
        file >> y;
        file >> z;

        g_Pts.emplace_back(new Point(x, y, z));

        g_Centroid.m_x += x;
        g_Centroid.m_y += y;
        g_Centroid.m_z += z;
    }

    g_Centroid /= g_Pts.size();
}

int main(int argc, char** argv)
{
    // Check that there's a vertex list file path in arguments
    if (argc != 2) {
        std::cerr << "Expected a vertex list file path in arguments" << std::endl;
    }

    // Initialize OpenGL
    initOpenGL(argc, argv);

    // Read vertex in file
    readVertexFile(argv[1]);

    // Compute convex hull
    compute3DConvexHull();

    // Start main rendering loop
    glutMainLoop();

    return 0;
}