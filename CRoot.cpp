#include "CRoot.hpp"
#include <iostream>
#include <sstream>
#include <vector>

using namespace std::string_literals;
using std::endl;

CRoot::CRoot(int argc, char ** argv)
    : mOpenGLHandler(argc, argv)
{
    init();
}

void CRoot::init()
{
    std::cout << "CRoot::" << __func__ << "()" << endl;

    /* create stringstream with polygon points */
    // std::istringstream iss(getInputData());

    /* Initialize polygon from string */
    // mPolygon.init(iss, &mOpenGLHandler);

    /* Initialize empty polygon */
    mPolygon.init(&mOpenGLHandler);

    /* Draw polygon */
    mOpenGLHandler.draw(mPolygon);
}

std::string CRoot::getInputData()
{
    std::cout << "CRoot::" << __func__ << "()" << endl;

    std::string data;
    data += "5"s + "\n"s;

    data += "100.0 300.0"s + "\n"s;
    data += "100.0 100.0"s + "\n"s;
    data += "300.0 300.0"s + "\n"s;
    data += "230.0 270.0"s + "\n"s;
    data += "300.0 100.0"s + "\n"s;

   return data;
}

