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
    m_FBO = std::make_unique<FBO>(5, FBO::DEPTH_STENCIL_ATTACHMENT::DEPTH_ONLY, width, height, TextureParameters::WRAP_METHOD::CLAMP_TO_EDGE, TextureParameters::FILTER_METHOD::LINEAR, GL_FLOAT);

    // How many color attachments do these need?
    m_blurFBO1 = std::make_unique<FBO>(1, FBO::DEPTH_STENCIL_ATTACHMENT::DEPTH_ONLY, width, height, TextureParameters::WRAP_METHOD::CLAMP_TO_EDGE);
    m_blurFBO2 = std::make_unique<FBO>(1, FBO::DEPTH_STENCIL_ATTACHMENT::DEPTH_ONLY, width, height, TextureParameters::WRAP_METHOD::CLAMP_TO_EDGE);

    // Initialize shape member variables.
    settingsChanged();
}

SceneviewScene::~SceneviewScene()
{
}

void SceneviewScene::loadGeometryShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/shaders/gshader.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/shaders/gshader.frag");
    m_geoShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);
}

// Also loads blur shaders
void SceneviewScene::loadPhongShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/shader.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/shader.frag");
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

void SceneviewScene::render(glm::mat4x4 projectionMatrix, glm::mat4x4 viewMatrix, std::shared_ptr<CS123::GL::FBO> eye_fbo) {

    // Don't render to eye right away.
//    eye_fbo->unbind();

    // FIRST PASS
    // Render into blurFBO1 from phong model.
//    m_blurFBO1->bind();
    m_phongShader->bind();

    // Clear both bits because that's what we do.
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    // Set scene info.
    setSceneUniforms(projectionMatrix, viewMatrix);
    setLights();

    // nicole's important texture nonsense
//     QImage image(":/scenes/wood.jpg");
//     Texture2D texture(image.bits(), image.width(), image.height());
//     TextureParametersBuilder builder;
//     builder.setFilter(TextureParameters::FILTER_METHOD::LINEAR);
//     builder.setWrap(TextureParameters::WRAP_METHOD::REPEAT);
//     TextureParameters parameters = builder.build();
//     parameters.applyTo(texture);
//     m_phongShader->setTexture("tex", texture);

    // Time for some shapes!
    renderGeometry();

    // What does this do.
    glBindTexture(GL_TEXTURE_2D, 0);

    // End first pass.
    m_phongShader->unbind();
//    m_blurFBO1->unbind();

//    // SECOND PASS
//    // Render to FBO2 while blurring horizontally.
//    m_blurFBO2->bind();
//    m_horizontalBlur->bind();

//    // Clear both bits because that's what we do.
//    glClear(GL_COLOR_BUFFER_BIT);
//    glClear(GL_DEPTH_BUFFER_BIT);

//    // Render from FBO1, blurring, to FBO2.
//    m_blurFBO1->getColorAttachment(0).bind();
//    m_fullquad->draw();

//    // End second pass.
//    m_horizontalBlur->unbind();
//    m_blurFBO2->unbind();

//    // THIRD PASS
//    // Render to each eye using the quad shader.
//    eye_fbo->bind();

//    // Bind the vertical blur.
//    m_verticalBlur->bind();

//    // Clear both bits because that's what we do.
//    glClear(GL_COLOR_BUFFER_BIT);
//    glClear(GL_DEPTH_BUFFER_BIT);

//    // Render from FBO2, blurring, to the fullscreen quad.
//    m_blurFBO2->getColorAttachment(0).bind();
//    m_fullquad->draw();

//    // End third pass.
//    m_verticalBlur->unbind();
}

// nicole's deferred shading stuff
// Also should take in an eye fbo to unbind, bind the real fbo,
//void SceneviewScene::render(glm::mat4x4 projectionMatrix, glm::mat4x4 viewMatrix, std::unique_ptr<FBO> eye_fbo) {

//    // Unbind eye for now.
//    eye_fbo->unbind();

//    // Bind FBO.
//    m_FBO->bind();

//    // Use geometry shader.
//    m_geoShader->bind();
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    // Set uniforms passed into vert.
//    setSceneUniforms(projectionMatrix, viewMatrix);

//    // Is this necessary?
//    glViewport(0, 0, m_width, m_height);

//    // Set "m" uniform and ambient, diffuse, specular, etc uniforms.
//    renderGeometry();

//    // Gives texture to .frag? Which texture is this? Does this really need to be here?
//    glBindTexture(GL_TEXTURE_2D, 0);

//    // Unbind geometry shader.
//    m_geoShader->unbind();

//    // Should we call this before or after unbinding geoshader?
//    m_FBO->unbind();

//    eye_fbo->bind();

//    // bind all our textures (gPosition, gNormal, gDiffuse, gAmbient, gSpecular)
//    // are activeTexture calls necessary? consider using setTexture here!
//    glActiveTexture(GL_TEXTURE0);
//    m_FBO->getColorAttachment(0).bind();
//    glActiveTexture(GL_TEXTURE1);
//    m_FBO->getColorAttachment(1).bind();
//    glActiveTexture(GL_TEXTURE2);
//    m_FBO->getColorAttachment(2).bind();
//    glActiveTexture(GL_TEXTURE3);
//    m_FBO->getColorAttachment(3).bind();
//    glActiveTexture(GL_TEXTURE4);
//    m_FBO->getColorAttachment(4).bind();

//    // Bind light shader.
//    m_phongShader->bind();

//    // Send lighting uniforms to fragment shader.
//    setLights();

//    m_phongShader->unbind();

//    // Render full screen quad.
//    std::vector<GLfloat> quadData = {-1.f, -1.f, 0.f,
//                                     0, 1,
//                                     -1.f, +1.f, 0.f,
//                                     0, 0,
//                                     +1.f, -1.f, 0.f,
//                                     1, 1,
//                                     +1.f, +1.f, 0.f,
//                                     1, 0};
//    m_quad = std::make_unique<OpenGLShape>();
//    m_quad->setVertexData(&quadData[0], quadData.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, 4);
//    m_quad->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
//    m_quad->setAttribute(ShaderAttrib::TEXCOORD0, 2, 3*sizeof(GLfloat), VBOAttribMarker::DATA_TYPE::FLOAT, false);
//    m_quad->buildVAO();
//    m_quad->draw();
//}

//void SceneviewScene::render(
//        glm::mat4x4 projectionMatrix,
//        glm::mat4x4 viewMatrix,
//        glm::mat4 m_mat4DevicePose[vr::k_unMaxTrackedDeviceCount],
//bool m_activeTrackedDevice[vr::k_unMaxTrackedDeviceCount]) {

//    // TODO: use controller positions if necessary
//    render(projectionMatrix, viewMatrix);
//}


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
        //m_geoShader->applyMaterial(prim.material);

        if (prim.material.textureMap.isUsed) {
            //glBindTexture(GL_TEXTURE_2D, getTexture(prim.material));
            //m_geoShader->setUniform("useTexture", 1);
//            m_phongShader->setUniform("useTexture", 1);
            //m_phongShader->setUniform("repeatUV", glm::vec2(prim.material.textureMap.repeatU, prim.material.textureMap.repeatV));
            //getTexture(prim.material);
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
    m_sphere = std::make_unique<Sphere>(10, 10);
}

// Is it ok that these are in a helper method?
GLuint SceneviewScene::getTexture(CS123SceneMaterial material) {
    QImage image;
    if (material.textureMap.imageSet) {
        image = material.textureMap.image;
    } else {
        QImage image = QImage(material.textureMap.filename.data());
        material.textureMap.image = image;
        material.textureMap.imageSet = true;
    }

    //image = QGLWidget::convertToGLFormat(image);

    Texture2D texture(image.bits(), image.width(), image.height());
    TextureParametersBuilder builder;
    builder.setFilter(TextureParameters::FILTER_METHOD::LINEAR);
    builder.setWrap(TextureParameters::WRAP_METHOD::REPEAT);
    TextureParameters parameters = builder.build();
    parameters.applyTo(texture);

    m_phongShader->setTexture("tex", texture);

    return m_id;
}

//QImage image(":/images/ostrich.jpg");
//glGenTextures(1, &m_textureID);
//glBindTexture(GL_TEXTURE_2D, m_textureID);
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());

// use resize gl? when dealing with vr's renderbuffers. bind passed in lefteyebuffer or righteyebuffer before rendering.
// i thought default was screen but idk. maybe you can't bind an fbo within an fbo. so you have to unbind whatever fbo is bound (lefteyebuffer) and then
// rebind it when you're ready to draw.

