#ifndef OPENGLSCENE_H
#define OPENGLSCENE_H

#include "Scene.h"
#include "glm.hpp"
#include <openvr.h>
#include "gl/datatype/FBO.h"

// Maximum number of lights, as defined in shader.
const int MAX_NUM_LIGHTS = 10;

using std::string;

/**
 * @class  OpenGLScene
 *
 * Basic Scene abstract class that supports OpenGL. Students will extend this class in ShapesScene
 * and SceneviewScene.
 */
class OpenGLScene : public Scene {
public:
    virtual ~OpenGLScene();

    virtual void render(glm::mat4x4 projectionMatrix, glm::mat4x4 viewMatrix, std::shared_ptr<CS123::GL::FBO> eye_fbo) = 0;
    virtual void render(glm::mat4x4 projectionMatrix, glm::mat4x4 viewMatrix) = 0;
//    virtual void render(
//            glm::mat4x4 projectionMatrix,
//            glm::mat4x4 viewMatrix,
//            glm::mat4 m_mat4DevicePose [vr::k_unMaxTrackedDeviceCount],
//            bool m_activeTrackedDevice[vr::k_unMaxTrackedDeviceCount]) = 0;

protected:

    void setClearColor();
};

#endif // OPENGLSCENE_H
