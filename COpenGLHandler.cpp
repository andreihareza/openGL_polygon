#include "COpenGLHandler.hpp"

#include <iostream>
#include <cmath>
#include "utility.hpp"
#include "CPolygon.hpp"

using std::endl;

std::vector<float> COpenGLHandler::stPointColors{};
std::vector<float> COpenGLHandler::stPointCoords{};
int COpenGLHandler::stPointsNum{};
int COpenGLHandler::stIntersectionPointsNum{};
int COpenGLHandler::stInsidePointsNum{};

unsigned int COpenGLHandler::stVboId{};
unsigned int COpenGLHandler::stColorBufferId{};
unsigned int COpenGLHandler::stShaderId{};
unsigned int COpenGLHandler::stRotateShaderId{};
unsigned int COpenGLHandler::stResizeMatrixId{};
unsigned int COpenGLHandler::stRotationMatrixId{};
unsigned int COpenGLHandler::stPreRotationMatrixId{};
unsigned int COpenGLHandler::stPostRotationMatrixId{};

int COpenGLHandler::stCoord_x{};
int COpenGLHandler::stCoord_y{};

CPolygon * COpenGLHandler::stPolygon{nullptr};

bool COpenGLHandler::stIsDragging{};
int COpenGLHandler::stDraggingPointIndex{};

std::vector<float> COpenGLHandler::stResizeMatrix{};
std::vector<float> COpenGLHandler::stRotationMatrix{};
std::vector<float> COpenGLHandler::stPreRotationMatrix{};
std::vector<float> COpenGLHandler::stPostRotationMatrix{};

bool COpenGLHandler::stIsPolygonRotated{false};

COpenGLHandler::COpenGLHandler(int argc, char ** argv)
{
    init(argc, argv);
}

COpenGLHandler::~COpenGLHandler()
{
    cleanup();
}

void COpenGLHandler::init (int argc, char ** argv)
{
    std::cout << "COpenGLHandler::" << __func__ << "()" << endl;

    /* Initialize open GL */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    /* Initialize window */
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(NUtility::width, NUtility::height);
    glutCreateWindow("Intersectii poligon");

    /* Initialize glew */
    glewInit();

    /* Background color black */
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void COpenGLHandler::draw(CPolygon & polygon)
{
    std::cout << "COpenGLHandler::" << __func__ << "()" << endl;

    /* Get points coord and color in openGL buffer format */
    preparePointsForDraw(polygon);

    /* Create shaders for drawing */
    createShaders();

    /* Line width */
    glLineWidth(3.0f);

    /* Point size */
    glPointSize(5.0f);

    /* Save polygon for point adding */
    stPolygon = &polygon;
    glutMouseFunc(COpenGLHandler::mouseFunction);
    glutKeyboardFunc(COpenGLHandler::keyboardFunction);

    /* Draw */
    glutDisplayFunc(COpenGLHandler::renderFunction);
    glutMainLoop();
}

void COpenGLHandler::preparePointsForDraw(CPolygon & polygon)
{
    std::cout << "COpenGLHandler::" << __func__ << "()" << endl;

    /* Clear previous points */
    stPointCoords.clear();
    stPointColors.clear();

    /* Add lines to buffer */
    for (auto & line : polygon)
    {
        /* Point coords */
        const auto & coords = line.getOpenGLPointCoords();
        stPointCoords.insert(stPointCoords.end(), coords.begin(), coords.end());

        /* Point colors */
        const auto & colors = line.getOpenGLPointColors();
        stPointColors.insert(stPointColors.end(), colors.begin(), colors.end());
    }

    /* Add points to buffer */
    for (auto & line : polygon)
    {
        /* Point coords */
        const auto & coords = line.getOpenGLFirstPointCoords();
        stPointCoords.insert(stPointCoords.end(), coords.begin(), coords.end());

        /* Point color */
        const auto & colors = line.getOpenGLFirstPointColors();
        stPointColors.insert(stPointColors.end(), colors.begin(), colors.end());
    }

    /* Add intersection points to buffer */
    const auto & intersectionPoints = polygon.getOpenGLIntersectionPoints();
    stPointCoords.insert(stPointCoords.end(), intersectionPoints.begin(), intersectionPoints.end());

    const auto & intersectionPointsColors = polygon.getOpenGLIntersectionPointsColors();
    stPointColors.insert(stPointColors.end(), intersectionPointsColors.begin(), intersectionPointsColors.end());

    /* Add inside points to buffer */
    const auto & insidePoints = polygon.getOpenGLInsidePoints();
    stPointCoords.insert(stPointCoords.end(), insidePoints.begin(), insidePoints.end());

    const auto & insidePointsColors = polygon.getOpenGLInsidePointsColors();
    stPointColors.insert(stPointColors.end(), insidePointsColors.begin(), insidePointsColors.end());

    /* Number of points/lines in polygon */
    stPointsNum = polygon.size();

    /* Number of intersection points */
    stIntersectionPointsNum = polygon.getIntersectionPointsNum();

    /* Number of points inside */
    stInsidePointsNum = polygon.getInsidePointsNum();

    /* Update transformation matrices to rotate polygon in (0, 0) */
    if (stIsPolygonRotated == true)
    {
        updatePreRotateTransformationMatrices();
    }

    /* Create VBO */
    createVBO();
}

void COpenGLHandler::renderFunction()
{
    std::cout << "COpenGLHandler::" << __func__ << "()" << endl;

    /* Clear previous data */
    glClear(GL_COLOR_BUFFER_BIT);

    /* Draw polygon line by line */
    glDrawArrays(GL_LINES, 0, 2*stPointsNum);

    /*  Draw each point */
    glDrawArrays(GL_POINTS, 2*stPointsNum, stPointsNum);

    /* Draw intersection points */
    glDrawArrays(GL_POINTS, 3*stPointsNum, stIntersectionPointsNum);

    /* Draw inside points */
    glPointSize(1.0f);
    glDrawArrays(GL_POINTS, 3*stPointsNum+stIntersectionPointsNum, stInsidePointsNum);
    glPointSize(5.0f);


    /* Refresh image */
    glFlush();
}

void COpenGLHandler::cleanup()
{
    /* Cleanup */
    destroyShaders();
    destroyVBO();
}

void COpenGLHandler::createVBO()
{
    std::cout << "COpenGLHandler::" << __func__ << "()" << endl;

    /* Create buffer and set as active */
    glGenBuffers(1, &stVboId);
    glBindBuffer(GL_ARRAY_BUFFER, stVboId);

    /* Fill buffer with point coords */
    glBufferData(GL_ARRAY_BUFFER, stPointCoords.size()*sizeof(float),
            stPointCoords.data(), GL_STATIC_DRAW);

    /* Set buffer as coord buffer */
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, NUtility::dimensions, GL_FLOAT, false, 0, 0);


    /* Create buffer and set as active */
    glGenBuffers(1, &stColorBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, stColorBufferId);

    /* Fill buffer with point colors */
    glBufferData(GL_ARRAY_BUFFER, stPointColors.size()*sizeof(float),
            stPointColors.data(), GL_STATIC_DRAW);

    /* Set buffer as color buffer */
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, false, 0, 0);
}

void COpenGLHandler::destroyVBO()
{
    std::cout << "COpenGLHandler::" << __func__ << "()" << endl;

    /* Cleanup */
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &stVboId);
    glDeleteBuffers(1, &stColorBufferId);
}


void COpenGLHandler::createShaders()
{
    std::cout << "COpenGLHandler::" << __func__ << "()" << endl;

    /* Create matrices */
    createResizeMatrix();
    createRotationMatrix();

    /* Load shaders from file */
    stShaderId = loadShaders("shader.vert", "shader.frag");
    stRotateShaderId = loadShaders("shaderRotate.vert", "shader.frag");

    useDefaultShader();
}

void COpenGLHandler::useDefaultShader()
{
    std::cout << "COpenGLHandler::" << __func__ << "()" << endl;
    glUseProgram(stShaderId);

    stResizeMatrixId = glGetUniformLocation(stShaderId, "resizeMatrix");
    glUniformMatrix4fv(stResizeMatrixId, 1, GL_TRUE, stResizeMatrix.data());
}

void COpenGLHandler::useRotationShader()
{
    std::cout << "COpenGLHandler::" << __func__ << "()" << endl;
    glUseProgram(stRotateShaderId);

    stResizeMatrixId = glGetUniformLocation(stRotateShaderId, "resizeMatrix");
    glUniformMatrix4fv(stResizeMatrixId, 1, GL_TRUE, stResizeMatrix.data());

    stRotationMatrixId = glGetUniformLocation(stRotateShaderId, "rotationMatrix");
    glUniformMatrix4fv(stRotationMatrixId, 1, GL_TRUE, stRotationMatrix.data());

    updatePreRotateTransformationMatrices();
}

void COpenGLHandler::updatePreRotateTransformationMatrices()
{
    std::cout << "COpenGLHandler::" << __func__ << "()" << endl;
    stPreRotationMatrix.resize(16, 0.0f);
    stPostRotationMatrix.resize(16, 0.0f);

    stPreRotationMatrix[0] = 1.0f;
    stPreRotationMatrix[5] = 1.0f;
    stPreRotationMatrix[10] = 1.0f;
    stPreRotationMatrix[15] = 1.0f;

    stPostRotationMatrix[0] = 1.0f;
    stPostRotationMatrix[5] = 1.0f;
    stPostRotationMatrix[10] = 1.0f;
    stPostRotationMatrix[15] = 1.0f;

    auto centerOfMass = stPolygon->getCenterOfMass();
    std::cout << "COpenGLHandler::" << __func__ << "(): centerOfMass:"
        << centerOfMass.first << ' ' << centerOfMass.second << endl;

    stPreRotationMatrix[3] = -centerOfMass.first;
    stPreRotationMatrix[7] = -centerOfMass.second;

    stPostRotationMatrix[3] = centerOfMass.first;
    stPostRotationMatrix[7] = centerOfMass.second;

    /* Pre-post rotation */
    stPreRotationMatrixId = glGetUniformLocation(stRotateShaderId, "preRotationMatrix");
    glUniformMatrix4fv(stPreRotationMatrixId, 1, GL_TRUE, stPreRotationMatrix.data());

    stPostRotationMatrixId = glGetUniformLocation(stRotateShaderId, "postRotationMatrix");
    glUniformMatrix4fv(stPostRotationMatrixId, 1, GL_TRUE, stPostRotationMatrix.data());
}

void COpenGLHandler::createResizeMatrix()
{
    stResizeMatrix.resize(16, 0.0f);

    stResizeMatrix[0] = 2.0f/NUtility::width;
    stResizeMatrix[3] = -1;
    stResizeMatrix[5] = 2.0f/NUtility::height;
    stResizeMatrix[7] = -1;
    stResizeMatrix[10] = 1.0f;
    stResizeMatrix[15] = 1.0f;
}

void COpenGLHandler::createRotationMatrix()
{
    stRotationMatrix.resize(16, 0.0f);

    stRotationMatrix[0] = cos(M_PI/4);
    stRotationMatrix[1] = -sin(M_PI/4);
    stRotationMatrix[4] = sin(M_PI/4);
    stRotationMatrix[5] = cos(M_PI/4);

    stRotationMatrix[15] = 1.0f;
}

void COpenGLHandler::destroyShaders()
{
    std::cout << "COpenGLHandler::" << __func__ << "()" << endl;

    /* Cleanup */
    glDeleteProgram(stShaderId);
    glDeleteProgram(stRotateShaderId);
}

void COpenGLHandler::mouseFunction(int button, int state, int x, int y)
{
    /* Mouse button coordonates are related to top right corner, not bottom right */
    y = NUtility::height - y;

    std::cout << "COpenGLHandler::" << __func__ << "(): " <<
        button << ' ' << state << ' ' << x << ' ' << y << endl;

    if (stIsPolygonRotated == true)
    {
        return;
    }

    /* Mouse left click is used to add points to polygon */
    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            handleMouseLeftClickPress(x, y);
        }

        /* On release, in case mouse was not moved, add a point at that position */
        if (state == GLUT_UP)
        {
            handleMouseLeftClickRelease(x, y);
        }
    }
    if (button == GLUT_RIGHT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            handleMouseRightClickPress(x, y);
        }

        if (state == GLUT_DOWN)
        {
            handleMouseRightClickRelease(x, y);
        }
    }
}

void COpenGLHandler::keyboardFunction(unsigned char key, int x, int y)
{
    std::cout << "COpenGLHandler::" << __func__ << "(): " << key << endl;

    (void)x;
    (void)y;

    if (key == 'r' || key == 'R')
    {
        handleRotateKeyPress();
    }
}

void COpenGLHandler::handleMouseLeftClickPress(int x, int y)
{
    std::cout << "COpenGLHandler::" << __func__ << "(): " << x << ' ' << y << endl;
    CLine::Point closestPoint;
    int closestPos = stPolygon->findClosest({x, y}, closestPoint);

    auto dist = [](int from, int to)
    {
        auto dif = from - to;
        return dif < 0? -dif : dif;
    };

    /* Distance between polygon's closest point and current point */
    int totalDist = dist(x, closestPoint.first) + dist(y, closestPoint.second);

    std::cout << "COpenGLHandler::" << __func__ << "(): totalDist = " << totalDist << endl;

    /* When a point is close to current click position, drag it */
    if (totalDist < 5)
    {
        stIsDragging = true;
        stDraggingPointIndex = closestPos;
        glutMotionFunc(COpenGLHandler::mouseMovement);
    }
    else
    {
        /* When no point is close to current click position, prepare to add a new point */
        stCoord_x = x;
        stCoord_y = y;
    }
}

void COpenGLHandler::handleMouseLeftClickRelease(int x, int y)
{
    std::cout << "COpenGLHandler::" << __func__ << "(): " << x << ' ' << y << endl;

    /* If we were dragging before, stop dragging */
    if (stIsDragging == true)
    {
        stIsDragging = false;
        glutMotionFunc(nullptr);
    }
    /* If we were to add a new point, add it if mouse was not moved away */
    else
    {
        auto dist = [](int from, int to)
        {
            auto dif = from - to;
            return dif < 0? -dif : dif;
        };

        int totalDist = dist(x, stCoord_x) + dist(y, stCoord_y);
        if (totalDist < 5)
        {
            stPolygon->addPoint({x, y});
        }
    }
}

void COpenGLHandler::mouseMovement(int x, int y)
{
    /* Mouse button coordonates are related to top right corner, not bottom right */
    y = NUtility::height - y;

    /* This is spammy. Commented */
    // std::cout << "COpenGLHandler::" << __func__ << "(): " << x << ' ' << y << endl;

    /* Whenever we are dragging and mouse moves, update the polygon */
    stPolygon->editPoint(stDraggingPointIndex, {x, y});
}

void COpenGLHandler::handleMouseRightClickPress(int x, int y)
{
    std::cout << "COpenGLHandler::" << __func__ << "(): " << x << ' ' << y << endl;
    stCoord_x = x;
    stCoord_y = y;
}

void COpenGLHandler::handleMouseRightClickRelease(int x, int y)
{
    std::cout << "COpenGLHandler::" << __func__ << "(): " << x << ' ' << y << endl;

    auto dist = [](int from, int to)
    {
        auto dif = from - to;
        return dif < 0? -dif : dif;
    };

    /* Delete the point it if mouse was not moved away */
    int totalDist = dist(x, stCoord_x) + dist(y, stCoord_y);
    if (totalDist < 5)
    {
        CLine::Point closestPoint;
        int closestPos = stPolygon->findClosest({x, y}, closestPoint);

        /* Distance between polygon's closest point and current point */
        totalDist = dist(x, closestPoint.first) + dist(y, closestPoint.second);

        std::cout << "COpenGLHandler::" << __func__ << "(): totalDist = " << totalDist << endl;

        /* If there's a point close to where we clicked, delete it */
        if (totalDist < 5)
        {
            stPolygon->deletePoint(closestPos);
        }
    }
}

void COpenGLHandler::handleRotateKeyPress()
{
    if (stIsPolygonRotated == false)
    {
        useRotationShader();
        stIsPolygonRotated = true;
    }
    else
    {
        useDefaultShader();
        stIsPolygonRotated = false;
    }

    glutPostRedisplay();
}

void COpenGLHandler::notifyPolygonChanged(CPolygon & polygon)
{
    std::cout << "COpenGLHandler::" << __func__ << "()" << endl;

    /* Destroy previous buffer */
    destroyVBO();

    /* Create all data for current polygon */
    preparePointsForDraw(polygon);

    /* Redraw image */
    glutPostRedisplay();
}

unsigned int COpenGLHandler::loadShaders(const char * vertex_file_path,
        const char * fragment_file_path)
{
    // Creeaza shadere
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Citeste din fisier shader-ul de varf
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if(VertexShaderStream.is_open())
    {
        std::string Line = "";
        while(getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }

    // Citeste din fisier shader-ul de fragment
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if(FragmentShaderStream.is_open()){
        std::string Line = "";
        while(getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compileaza shader-ul de varf
    printf("Compilare shader : %s\n", vertex_file_path);
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);

    // Verifica
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> VertexShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

    // Compileaza shader-ul de fragemnt
    printf("Compilare shader : %s\n", fragment_file_path);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);

    // Verifica
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

    // Leaga programul
    fprintf(stdout, "Legare program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Verifica
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> ProgramErrorMessage( std::max(InfoLogLength, int(1)) );
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

	glUseProgram(0);

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(FragmentShaderID);
    glDeleteShader(VertexShaderID);

    return ProgramID;
}

