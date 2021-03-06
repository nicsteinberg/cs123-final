#ifndef SCENEVIEWSCENE_H
#define SCENEVIEWSCENE_H

#include "OpenGLScene.h"
#include "gl/datatype/FBO.h"
#include "gl/util/FullScreenQuad.h"

#include <memory>



namespace CS123 { namespace GL {

    class CS123Shader;
    class Texture2D;
}}

/**
 *
 * @class SceneviewScene
 *
 * A complex scene consisting of multiple objects. Students will implement this class in the
 * Sceneview assignment.
 *
 * Here you will implement your scene graph. The structure is up to you - feel free to create new
 * classes and data structures as you see fit. We are providing this SceneviewScene class for you
 * to use as a stencil if you like.
 *
 * Keep in mind that you'll also be rendering entire scenes in the next two assignments, Intersect
 * and Ray. The difference between this assignment and those that follow is here, we are using
 * OpenGL to do the rendering. In Intersect and Ray, you will be responsible for that.
 */
class SceneviewScene : public OpenGLScene {
public:
    SceneviewScene(int width, int height);
    virtual ~SceneviewScene();

    virtual void render(glm::mat4x4 projectionMatrix, glm::mat4x4 viewMatrix) override;
    virtual void render(glm::mat4x4 projectionMatrix, glm::mat4x4 viewMatrix, std::shared_ptr<CS123::GL::FBO> eye_fbo) override;
//    virtual void render(
//            glm::mat4x4 projectionMatrix,
//            glm::mat4x4 viewMatrix,
//            glm::mat4 m_mat4DevicePose [vr::k_unMaxTrackedDeviceCount],
//            bool m_activeTrackedDevice[vr::k_unMaxTrackedDeviceCount]) override;

    virtual void settingsChanged() override;

    // Use this method to set an internal selection, based on the (x, y) position of the mouse
    // pointer.  This will be used during the "modeler" lab, so don't worry about it for now.
    void setSelection(int x, int y);

private:


private:

    void loadPhongShader();
    void loadGeometryShader();
    void setSceneUniforms(glm::mat4x4 &projectionMatrix, glm::mat4x4 &viewMatrix);
    void setLights();
    void renderGeometry();

    std::unique_ptr<CS123::GL::CS123Shader> m_phongShader, m_geoShader, m_horizontalBlur, m_verticalBlur;

    std::unique_ptr<FBO> m_FBO, m_blurFBO1, m_blurFBO2;

    int m_width, m_height;
    GLuint m_id;

    //GLuint m_blurHProgram, m_blurVProgram, m_phongProgram;
    std::unique_ptr<OpenGLShape> m_quad;
    std::unique_ptr<FullScreenQuad> m_fullquad;

};

#endif // SCENEVIEWSCENE_H
