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

#include <iostream>

using namespace CS123::GL;


SceneviewScene::SceneviewScene()
{
    // Load shader. Will later want to load geo shader here.
    loadPhongShader();

    // Is this the correct width and height? Takes in number of color attachments - we'll have at least 3.
    // Can also just make our own gBuffer class instead of an FBO that generates our gPosition, gNormal, and gColor buffers.
    //m_FBO = std::make_unique<FBO>(3, FBO::DEPTH_STENCIL_ATTACHMENT::DEPTH_ONLY, width(), height(), TextureParameters::WRAP_METHOD::CLAMP_TO_EDGE, GL_FLOAT);

    // Initialize shape member variables.
    settingsChanged();
}

SceneviewScene::~SceneviewScene()
{
}

void SceneviewScene::loadGeometryShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/gshader.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/gshader.frag");
    m_geoShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);
}

void SceneviewScene::loadPhongShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/shader.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/shader.frag");
    // std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/quad.vert");
    // std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/lightshader.frag");
    m_phongShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);
}

//void SceneviewScene::render(glm::mat4x4 projectionMatrix, glm::mat4x4 viewMatrix) {

//    // Bind FBO.
//    m_FBO->bind();

//    // Use this program ID.
//    m_geoShader->bind();

//    // Is this necessary? Why these bits?
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    // Set uniforms passed into vert.
//    setSceneUniforms(projectionMatrix, viewMatrix);

//    // Is this necessary?
//    glViewPort(0, 0, width, height);

//    // Set "m" uniform and ambient, diffuse, specular, etc uniforms.
//    renderGeometry();

//    // Gives texture to .frag? Which texture is this?
//    glBindTexture(GL_TEXTURE_2D, 0);

//    // Unbind geometry shader.
//    m_geoShader->unbind();

//    // Should we call this before or after unbinding geoshader?
//    m_FBO->unbind();

//    // bind all our textures (gPosition, gNormal, gColor)
//    // are activeTexture calls necessary?
//    glActiveTexture(GL_TEXTURE0);
//    m_FBO->getColorAttachment(0).bind();
//    glActiveTexture(GL_TEXTURE1);
//    m_FBO->getColorAttachment(1).bind();
//    glActiveTexture(GL_TEXTURE2);
//    m_FBO->getColorAttachment(2).bind();

//    // Bind light shader.
//    m_phongShader->bind();

//    // Send lighting uniforms to fragment shader.
//    setLights();

//    m_phongShader->unbind();

//    // render quad
//    // m_quad->draw();
//}

void SceneviewScene::render(glm::mat4x4 projectionMatrix, glm::mat4x4 viewMatrix) {
    m_phongShader->bind();
    setSceneUniforms(projectionMatrix, viewMatrix);
    setLights();

    // Set "m" uniform and ambient, diffuse, specular, etc uniforms.
    renderGeometry();

    glBindTexture(GL_TEXTURE_2D, 0);
    m_phongShader->unbind();

}

//void SceneviewScene::render(glm::mat4x4 projectionMatrix, glm::mat4x4 viewMatrix) {
//    int num_rays = 5;
//    float aperture_size = 0.05f;
//    glm::mat4x4 p = projectionMatrix;

//    glm::vec3 point_of_focus = glm::vec3(0.f);
//    glm::vec3 eye = glm::vec3(viewMatrix[2], viewMatrix[6], viewMatrix[10]);
//    glm::vec3 up = glm::vec3(viewMatrix[0], viewMatrix[4], viewMatrix[8]);
//    glm::vec3 right = glm::normalize(glm::cross(point_of_focus - eye, up));
//    up = glm::normalize(glm::cross(point_of_focus - eye, right));

//    for (int i = 0; i < num_rays; i++) {

//        glm::vec3 bokeh = right * cosf(2 * M_PI * i / num_rays) + up * sinf(2 * M_PI * i / num_rays);
//        glm::vec3 displaced_eye = eye + aperture_size * bokeh;
//        glm::mat4x4 v = glulookAt(displaced_eye, point_of_focus, up);

//        m_phongShader->bind();
//        setSceneUniforms(p, v);
//        setLights();
//        renderGeometry();
//        glBindTexture(GL_TEXTURE_2D, 0);
//        glAccum(i ? GL_ACCUM : GL_LOAD, 1.f / num_rays);
//        m_phongShader->unbind();

//    }

//    glAccum(GL_RETURN, 1);
////    wglSwapBuffers();

//}

void SceneviewScene::render(
    glm::mat4x4 projectionMatrix,
    glm::mat4x4 viewMatrix,
    glm::mat4 m_mat4DevicePose[vr::k_unMaxTrackedDeviceCount],
    bool m_activeTrackedDevice[vr::k_unMaxTrackedDeviceCount]) {

    // TODO: use controller positions if necessary
    render(projectionMatrix, viewMatrix);
}


void SceneviewScene::setSceneUniforms(glm::mat4x4 &projectionMatrix, glm::mat4x4 &viewMatrix) {
    m_phongShader->setUniform("useLighting", true);
    m_phongShader->setUniform("useArrowOffsets", false);
    m_phongShader->setUniform("p", projectionMatrix);
    m_phongShader->setUniform("v", viewMatrix);

    //m_geoShader->setUniform("p", projectionMatrix);
    //m_geoShader->setUniform("v", viewMatrix);
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
        m_phongShader->setUniform("m", prim.compositeTransformation);
        m_phongShader->applyMaterial(prim.material);

        //m_geoShader->setUniform("m", prim.compositeTransformation);
        //m_geoShader->applyMaterial(prim.material); // Will have to add more uniforms to this helper!

//        if (prim.material.textureMap.isUsed) {
//            getTexture(prim.material);
//        }

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
    m_cube = std::make_unique<Cube>(3);
    m_cone = std::make_unique<Cone>(3, 3);
    m_cylinder = std::make_unique<Cylinder>(3, 30);
    m_sphere = std::make_unique<Sphere>(10, 10);
}

// Is it ok that these are in a helper method?
QImage SceneviewScene::getTexture(CS123SceneMaterial material) {
    QImage fImage;
    if (material.textureMap.imageSet) {
        fImage = material.textureMap.image;
    } else {
        QImage image = QImage(material.textureMap.filename.data());
        fImage = QGLWidget::convertToGLFormat(image);
    }

    Texture2D texture(fImage.bits(), fImage.width(), fImage.height());
    TextureParametersBuilder builder;
    builder.setFilter(TextureParameters::FILTER_METHOD::LINEAR);
    builder.setWrap(TextureParameters::WRAP_METHOD::REPEAT);
    TextureParameters parameters = builder.build();
    parameters.applyTo(texture);
}

