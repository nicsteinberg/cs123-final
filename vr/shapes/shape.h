#ifndef SHAPE_H
#define SHAPE_H

#include "openglshape.h"

/**
 * @class Shape
 *
 * The superclass for all shapes.
 */
class Shape
        : public OpenGLShape
{
public:
    // Constructor for shape. Pass in parameters.
    Shape(); //int param1, int param2
    virtual ~Shape();

protected:
    int m_param1;
    int m_param2;
    std::vector<float> m_verts;


};

#endif // SHAPE_H
