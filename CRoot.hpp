#pragma once
#include "CPolygon.hpp"
#include "COpenGLHandler.hpp"

class CRoot
{
public:
    CRoot(int argc, char ** argv);
    void init();

private:
    std::string getInputData();

    CPolygon mPolygon;
    COpenGLHandler mOpenGLHandler;
};

