#include "GBufferShader.h"

using namespace CS123::GL;

GBufferShader::GBufferShader(const std::string &vertexSource, const std::string &fragmentSource) :
    Shader(vertexSource, fragmentSource)
{
}

void GBufferShader::setModelMatrix(const glm::mat4 &modelMatrix) {
    m_modelMatrix = modelMatrix;
    setModelMatrix();
    setNormalMatrix();
    setMVPMatrix();
}

void GBufferShader::setViewMatrix(const glm::mat4 &viewMatrix) {
    m_viewMatrix = viewMatrix;
    setViewMatrix();
    setNormalMatrix();
    setMVPMatrix();
}

void GBufferShader::setProjectionMatrix(const glm::mat4 &projectionMatrix) {
    m_projectionMatrix = projectionMatrix;
    setProjectionMatrix();
    setMVPMatrix();
}

void GBufferShader::setModelMatrix() {
    setUniform("m", m_modelMatrix);
}

void GBufferShader::setViewMatrix() {
    setUniform("v", m_viewMatrix);
}

void GBufferShader::setProjectionMatrix() {
    setUniform("p", m_projectionMatrix);
}

void GBufferShader::setNormalMatrix() {
    setUniform("normalMatrix", glm::mat3(glm::inverse(glm::transpose(m_viewMatrix * m_modelMatrix))));
}

void GBufferShader::setMVPMatrix() {
//    setUniform("mvp", m_projectionMatrix * m_viewMatrix * m_modelMatrix);
}
