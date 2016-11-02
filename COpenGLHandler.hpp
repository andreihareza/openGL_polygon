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

    static void keyboardFunction(
            unsigned char key,
            int x, int y);

    static void createVBO();
    static void createShaders();

    static void useDefaultShader();
    static void useRotationShader();

    static void destroyVBO();
    static void destroyShaders();

    static unsigned int loadShaders(const char * vertex_file_path,
        const char * fragment_file_path);

    static void handleMouseLeftClickPress(int x, int y);
    static void handleMouseLeftClickRelease(int x, int y);
    static void handleMouseRightClickPress(int x, int y);
    static void handleMouseRightClickRelease(int x, int y);
    static void handleRotateKeyPress();

    static void mouseMovement(int x, int y);

    static void createResizeMatrix();
    static void createRotationMatrix();
    static void updatePreRotateTransformationMatrices();




    static std::vector<float> stPointCoords;
    static std::vector<float> stPointColors;
    static int stPointsNum;
    static int stIntersectionPointsNum;
    static int stInsidePointsNum;

    static unsigned int stVboId;
    static unsigned int stColorBufferId;
    static unsigned int stShaderId;
    static unsigned int stRotateShaderId;
    static unsigned int stResizeMatrixId;
    static unsigned int stRotationMatrixId;
    static unsigned int stPreRotationMatrixId;
    static unsigned int stPostRotationMatrixId;

    static int stCoord_x;
    static int stCoord_y;

    static CPolygon * stPolygon;

    static bool stIsDragging;
    static int stDraggingPointIndex;

    static std::vector<float> stResizeMatrix;
    static std::vector<float> stRotationMatrix;
    static std::vector<float> stPreRotationMatrix;
    static std::vector<float> stPostRotationMatrix;

    static bool stIsPolygonRotated;
};

