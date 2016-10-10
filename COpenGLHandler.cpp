#include "COpenGLHandler.hpp"

#include <iostream>
#include "utility.hpp"
#include "CPolygon.hpp"

using std::endl;

std::vector<float> COpenGLHandler::stPointColors{};
std::vector<float> COpenGLHandler::stPointCoords{};

unsigned int COpenGLHandler::stVboId{};
unsigned int COpenGLHandler::stColorBufferId{};
unsigned int COpenGLHandler::stShadersId{};

int COpenGLHandler::stCoord_x{};
int COpenGLHandler::stCoord_y{};

CPolygon * COpenGLHandler::stPolygon{nullptr};

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

    /* Background color white */
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
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

    /* Save polygon for point adding */
    stPolygon = &polygon;
    glutMouseFunc(COpenGLHandler::mouseFunction);

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

    for (auto & line : polygon)
    {
        /* Point coords */
        const auto & coords = line.getOpenGLPointCoords();
        stPointCoords.insert(stPointCoords.end(), coords.begin(), coords.end());

        /* Point colors */
        const auto & colors = line.getOpenGLPointColors();
        stPointColors.insert(stPointColors.end(), colors.begin(), colors.end());
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
    glDrawArrays(GL_LINES, 0, stPointCoords.size()/NUtility::dimensions);

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
    /* Load shaders from file */
    stShadersId = loadShaders("shader.vert", "shader.frag");

    glUseProgram(stShadersId);
}

void COpenGLHandler::destroyShaders()
{
    std::cout << "COpenGLHandler::" << __func__ << "()" << endl;

    /* Cleanup */
    glDeleteProgram(stShadersId);
}

void COpenGLHandler::mouseFunction(int button, int state, int x, int y)
{
    /* Mouse button coordonates are related to top right corner, not bottom right */
    y = NUtility::height - y;

    std::cout << "COpenGLHandler::" << __func__ << "(): " <<
        button << ' ' << state << ' ' << x << ' ' << y << endl;

    /* Mouse left click is used to add points to polygon */
    if (button == GLUT_LEFT_BUTTON)
    {
        /* On press, save click position */
        if (state == GLUT_DOWN)
        {
            stCoord_x = x;
            stCoord_y = y;
        }

        /* On release, in case mouse was not moved, add a point at that position */
        if (state == GLUT_UP)
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

