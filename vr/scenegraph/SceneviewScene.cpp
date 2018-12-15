#include "SceneviewScene.h"
#include "GL/glew.h"
#include <QGLWidget>

#include "lib/ResourceLoader.h"
#include "gl/textures/Texture2D.h"
#include "gl/textures/TextureParameters.h"
#include "gl/textures/TextureParametersBuilder.h"
#include "gl/shaders/CS123Shader.h"
#include "ui/Settings.h"

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "gl/shaders/ShaderAttribLocations.h"
#include <glm/gtc/type_ptr.hpp>
#include "gl/datatype/FBO.h"
#include "gl/util/FullScreenQuad.h"
#include <iostream>

using namespace CS123::GL;


SceneviewScene::SceneviewScene(int width, int height)
{
    m_width = width;
    m_height = height;

    // Load shaders.
    loadGeometryShader();
    loadPhongShader();

    // Takes in number of color attachments - we'll have at least 3.
    // Can also just make our own gBuffer class instead of an FBO that generates our gPosition, gNormal, and gColor buffers.
    m_FBO = std::make_unique<FBO>(6, FBO::DEPTH_STENCIL_ATTACHMENT::DEPTH_ONLY, width, height, TextureParameters::WRAP_METHOD::CLAMP_TO_EDGE, TextureParameters::FILTER_METHOD::LINEAR, GL_FLOAT);

    // How many color attachments do these need?
    m_blurFBO1 = std::make_unique<FBO>(2, FBO::DEPTH_STENCIL_ATTACHMENT::DEPTH_ONLY, width, height, TextureParameters::WRAP_METHOD::CLAMP_TO_EDGE);
    m_blurFBO2 = std::make_unique<FBO>(2, FBO::DEPTH_STENCIL_ATTACHMENT::DEPTH_ONLY, width, height, TextureParameters::WRAP_METHOD::CLAMP_TO_EDGE);

    // Initialize shape member variables.
    settingsChanged();

    QImage image(":/scenes/wood.jpg");
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
}

SceneviewScene::~SceneviewScene()
{
//    glDeleteTextures(1, &m_id);
}

void SceneviewScene::loadGeometryShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/shaders/gshader.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/shaders/gshader.frag");
    m_geoShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);
}

// Also loads blur shaders
void SceneviewScene::loadPhongShader() {
    //std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/shader.vert");
    //std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/shader.frag");
        std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/shaders/quad.vert");
        std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/shaders/lightshader.frag");
    m_phongShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);

    vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/shaders/quad.vert");
    fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/shaders/horizontalBlur.frag");
    m_horizontalBlur = std::make_unique<CS123Shader>(vertexSource, fragmentSource);

    fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/shaders/verticalBlur.frag");
    m_verticalBlur = std::make_unique<CS123Shader>(vertexSource, fragmentSource);

    m_fullquad = std::make_unique<FullScreenQuad>();
}

// THIS IS THE WORKING STENCIL METHOD
void SceneviewScene::render(glm::mat4x4 projectionMatrix, glm::mat4x4 viewMatrix) {
    m_phongShader->bind();
    setSceneUniforms(projectionMatrix, viewMatrix);
    setLights();

    // Set "m" uniform and ambient, diffuse, specular, etc uniforms.
    renderGeometry();

    //glBindTexture(GL_TEXTURE_2D, 0);
    m_phongShader->unbind();

}

//void SceneviewScene::render(glm::mat4x4 projectionMatrix, glm::mat4x4 viewMatrix, std::shared_ptr<CS123::GL::FBO> eye_fbo) {

//    // Don't render to eye right away.
//    eye_fbo->unbind();

//    // FIRST PASS
//    // Render into blurFBO1 from phong model.
//    m_blurFBO1->bind();
//    m_phongShader->bind();

//    // Clear both bits because that's what we do.
//    glClear(GL_COLOR_BUFFER_BIT);
//    glClear(GL_DEPTH_BUFFER_BIT);

//    // Set scene info.
//    setSceneUniforms(projectionMatrix, viewMatrix);
//    setLights();
//    glBindTexture(GL_TEXTURE_2D, m_id);

//    // Time for some shapes!
//    renderGeometry();

//    // What does this do.
//    glBindTexture(GL_TEXTURE_2D, 0);

//    // End first pass.
//    m_phongShader->unbind();
//    m_blurFBO1->unbind();

//    // SECOND PASS
//    // Render to FBO2 while blurring horizontally.

//    m_blurFBO2->bind();
//    m_horizontalBlur->bind();

//    // Clear both bits because that's what we do.
//    glClear(GL_COLOR_BUFFER_BIT);
//    glClear(GL_DEPTH_BUFFER_BIT);

//    // Render from FBO1, blurring, to FBO2.
//    m_horizontalBlur->setTexture("tex", m_blurFBO1->getColorAttachment(0));
//    m_horizontalBlur->setTexture("camera_pos_tex", m_blurFBO1->getColorAttachment(1));


//    m_fullquad->draw();

//    // End second pass.
//    m_horizontalBlur->unbind();
//    m_blurFBO1->getColorAttachment(0).unbind();
//    m_blurFBO1->getColorAttachment(1).unbind();
//    m_blurFBO2->unbind();

//    // THIRD PASS
//    // Render to each eye using the quad shader.
//    eye_fbo->bind();

//    // Bind the vertical blur.
//    m_verticalBlur->bind();

//    // Clear both bits because that's what we do.
//    glClear(GL_COLOR_BUFFER_BIT);
//    glClear(GL_DEPTH_BUFFER_BIT);

//    m_verticalBlur->setTexture("tex", m_blurFBO2->getColorAttachment(0));
//    m_verticalBlur->setTexture("camera_pos_tex", m_blurFBO2->getColorAttachment(1));

//    // Render from FBO2, blurring, to the fullscreen quad.
//    m_fullquad->draw();

//    // End third pass.
//    m_verticalBlur->unbind();
//    m_blurFBO2->getColorAttachment(0).unbind();
//    m_blurFBO2->getColorAttachment(1).unbind();

//}

// nicole's deferred shading stuff
void SceneviewScene::render(glm::mat4x4 projectionMatrix, glm::mat4x4 viewMatrix, std::shared_ptr<CS123::GL::FBO> eye_fbo) {

    // Unbind eye for now.
    eye_fbo->unbind();

    // Bind FBO.
    m_FBO->bind();

    // Use geometry shader.
    m_geoShader->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set uniforms passed into vert.
    setSceneUniforms(projectionMatrix, viewMatrix);

    // Set "m" uniform and ambient, diffuse, specular, etc uniforms.
    renderGeometry();

    // Unbinds texture.
    glBindTexture(GL_TEXTURE_2D, 0);

    // Unbind geometry shader and first FBO.
    m_geoShader->unbind();
    m_FBO->unbind();

    // Now render to screen.
    eye_fbo->bind();

    // Bind light shader.
    m_phongShader->bind();

    // Send textures to shader.
    m_phongShader->setTexture("gPosition", m_FBO->getColorAttachment(0));
    m_phongShader->setTexture("gNormal", m_FBO->getColorAttachment(1));
    m_phongShader->setTexture("gDiffuse", m_FBO->getColorAttachment(2));
    m_phongShader->setTexture("gAmbient", m_FBO->getColorAttachment(3));
    m_phongShader->setTexture("gSpecular", m_FBO->getColorAttachment(4));
    m_phongShader->setTexture("gCameraPos", m_FBO->getColorAttachment(5));

    // Send lighting uniforms to fragment shader.
    setLights();

    // Draw full screen quad.
    m_fullquad->draw();

    m_phongShader->unbind();
    m_FBO->getColorAttachment(0).unbind();
    m_FBO->getColorAttachment(1).unbind();
    m_FBO->getColorAttachment(2).unbind();
    m_FBO->getColorAttachment(3).unbind();
    m_FBO->getColorAttachment(4).unbind();
    m_FBO->getColorAttachment(5).unbind();
}

//void SceneviewScene::render(
//        glm::mat4x4 projectionMatrix,
//        glm::mat4x4 viewMatrix,
//        glm::mat4 m_mat4DevicePose[vr::k_unMaxTrackedDeviceCount],
//bool m_activeTrackedDevice[vr::k_unMaxTrackedDeviceCount]) {

//    // TODO: use controller positions if necessary
//    render(projectionMatrix, viewMatrix);
//}


void SceneviewScene::setSceneUniforms(glm::mat4x4 &projectionMatrix, glm::mat4x4 &viewMatrix) {
//    m_phongShader->setUniform("useLighting", true);
//    m_phongShader->setUniform("useArrowOffsets", false);
//    m_phongShader->setUniform("p", projectionMatrix);
//    m_phongShader->setUniform("v", viewMatrix);

    m_geoShader->setUniform("p", projectionMatrix);
    m_geoShader->setUniform("v", viewMatrix);
}

void SceneviewScene::setLights()
{
    // Set up the lighting for scene using m_phongShader.
    for (unsigned int i = 0; i < m_lights.size(); i++) {
        m_phongShader->setLight(m_lights[i]);
    }
}

void SceneviewScene::renderGeometry() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Render every primitive.
    for (unsigned int i = 0; i < m_primitives.size(); i++) {
        PrimTransf prim = m_primitives[i];

        // Apply matrix transformations and material.
//        m_phongShader->setUniform("m", prim.compositeTransformation);
//        m_phongShader->applyMaterial(prim.material);

        m_geoShader->setUniform("m", prim.compositeTransformation);
        m_geoShader->applyMaterial(prim.material);

        if (prim.material.textureMap.isUsed) {
            m_geoShader->setUniform("useTexture", 1);
            m_geoShader->setUniform("repeatUV", glm::vec2(prim.material.textureMap.repeatU, prim.material.textureMap.repeatV));
//            m_phongShader->setUniform("useTexture", 1);
//            m_phongShader->setUniform("repeatUV", glm::vec2(prim.material.textureMap.repeatU, prim.material.textureMap.repeatV));
            glBindTexture(GL_TEXTURE_2D, prim.material.textureMap.id);
        } else {
            m_geoShader->setUniform("useTexture", 0);
//            m_phongShader->setUniform("useTexture", 0);
        }

        switch (prim.type) {
        case PrimitiveType::PRIMITIVE_CUBE:
            m_cube->draw();
            break;
        case PrimitiveType::PRIMITIVE_CONE:
            m_cone->draw();
            break;
        case PrimitiveType::PRIMITIVE_CYLINDER:
            m_cylinder->draw();
            break;
        default:
            m_sphere->draw();
        }
    }
}

void SceneviewScene::settingsChanged() {
    m_cube = std::make_unique<Cube>(1);
    m_cone = std::make_unique<Cone>(3, 3);
    m_cylinder = std::make_unique<Cylinder>(3, 30);
    m_sphere = std::make_unique<Sphere>(25, 25);
}

