#pragma once

#include <stdio.h>
#include <fstream>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <vector>

#include "IOpenGLListener.hpp"

class COpenGLHandler : public IOpenGLListener
{
public:
    COpenGLHandler(int argc, char ** argv);
    virtual ~COpenGLHandler();

    void init(int argc, char ** argv);

    void draw(CPolygon & polygon);

    void preparePointsForDraw(CPolygon & polygon);

    virtual void notifyPolygonChanged(CPolygon & polygon) override;

private:
    void cleanup();

    static void renderFunction();
    static void mouseFunction(
        int button, int state,
        int x, int y);

    static void createVBO();
    static void createShaders();

    static void destroyVBO();
    static void destroyShaders();

    static unsigned int loadShaders(const char * vertex_file_path,
        const char * fragment_file_path);

    static void handleMouseLeftClickPress(int x, int y);
    static void handleMouseLeftClickRelease(int x, int y);

    static void mouseMovement(int x, int y);

    static std::vector<float> stPointCoords;
    static std::vector<float> stPointColors;

    static unsigned int stVboId;
    static unsigned int stColorBufferId;
    static unsigned int stShadersId;

    static int stCoord_x;
    static int stCoord_y;

    static CPolygon * stPolygon;

    static bool stIsDragging;
    static int stDraggingPointIndex;
};

