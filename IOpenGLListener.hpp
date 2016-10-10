#pragma once

class CPolygon;

class IOpenGLListener
{
public:
    virtual ~IOpenGLListener(){};
    virtual void notifyPolygonChanged(CPolygon & polygon) = 0;
};

