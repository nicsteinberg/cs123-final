#ifndef GBUFFERSHADER_H
#define GBUFFERSHADER_H

#include "gl/shaders/Shader.h"

namespace CS123 { namespace GL {

class GBufferShader : public Shader {
public:
    GBufferShader(const std::string &vertexSource, const std::string &fragmentSource);

    void setModelMatrix(const glm::mat4 &modelMatrix);
    void setViewMatrix(const glm::mat4 &viewMatrix);
    void setProjectionMatrix(const glm::mat4 &projectionMatrix);

private:

    void setModelMatrix();
    void setViewMatrix();
    void setProjectionMatrix();
    void setNormalMatrix();
    void setMVPMatrix();

    glm::mat4 m_modelMatrix;
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;
    glm::mat4 m_mvpMatrix;
};

}}

#endif // GBUFFERSHADER_H
