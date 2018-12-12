#include "view.h"

#include "viewformat.h"
#include "ResourceLoader.h"
#include "CS123XmlSceneParser.h"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "gl/shaders/CS123Shader.h"
#include "CS123SceneData.h"
#include "gl/datatype/FBO.h"
#include "gl/textures/Texture2D.h"
#include "gl/textures/TextureParametersBuilder.h"

#include <QApplication>
#include <QKeyEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <iostream>
#include "scenegraph/SceneviewScene.h"

View::View(QWidget *parent)
    : QGLWidget(ViewFormat(), parent),
    m_time(),
    m_timer(),
    m_captureMouse(false),
    m_hmd(NULL)
{
    /* View needs all mouse move events, not just mouse drag events */
    setMouseTracking(true);
    std::fill(std::begin(m_keys), std::end(m_keys), false);
    m_camera = QuaternionCamera();

    /* Hide the cursor */
    if (m_captureMouse) {
        QApplication::setOverrideCursor(Qt::BlankCursor);
    }

    /* View needs keyboard focus */
    setFocusPolicy(Qt::StrongFocus);

    /* The update loop is implemented using a timer */
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));

}

View::~View()
{
    if(m_hmd) {

        /* Shutdown the VR system */
        vr::VR_Shutdown();
        m_hmd = NULL;
    }
}

void View::initializeGL() {
   /* All OpenGL initialization *MUST* be done during or after this
    * method. Before this method is called, there is no active OpenGL
    * context and all OpenGL calls have no effect.
    */

    /* Initialize glew */
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        /* Problem: glewInit failed, something is seriously wrong. */
        std::cerr << "Something is very wrong, glew initialization failed." << std::endl;
    }
    std::cout << "Using GLEW " <<  glewGetString( GLEW_VERSION ) << std::endl;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glClearColor(0.f, 0.f, 0.f, 1.f);

    m_near = 0.1f;
    m_far = 100.0f;

    initVR();

   /* Start a timer that will try to get 90 frames per second (the actual
    * frame rate depends on the operating system and other running programs)
    */
    m_time.start();
    m_timer.start(1000 / 90);
}

void View::initVR() {

    vr::EVRInitError error = vr::VRInitError_None;

   /* VR_Init will attempt to connect to a hmd attached
    * to the machine. The VRApplication_Scene parameter
    * specifies a 3D application that will be drawing an
    * environment. Be sure to call VR_Shutdown in the
    * destructor!
    */
    m_hmd = vr::VR_Init(&error, vr::VRApplication_Scene);

    /* ensure no errors occured during initialization */
    if (error != vr::VRInitError_None) {
        m_hmd = nullptr;
        string message = vr::VR_GetVRInitErrorAsEnglishDescription(error);
        std::cout << "Unable to init VR" + message;
        return;
    }


    /*
     * Provides the game with the minimum size that it should use for
     * its offscreen render target to minimize pixel stretching.
     * This size is matched with the projection matrix and distortion
     * function and will change from display to display depending on
     * resolution, distortion, and field of view.
     */
    m_hmd->GetRecommendedRenderTargetSize(&m_eyeWidth, &m_eyeHeight);


    /* VRCompositor simplifies the process of displaying images
     * by taking care of distortion, prediction, synchronization
     * and other subtle rendering issues.
     */
    if (!vr::VRCompositor()) {
        string message = "Compositor initialization failed. See log file for details";
        std::cout << "Unable to init VR" + message;
    }

   /* if running on a windows enviorment, we need
    * to initialize VRSystem to use the MinGW compiler
    * instead of the default.
    */
    #if defined(__GNUC__) && defined(_WIN32)
        vr::IVRSystem *vr_sys = vr::VRSystem();
        typedef void (vr::IVRSystem::*FuncPtr1)(vr::HmdMatrix34_t *, vr::EVREye);
        FuncPtr1 get_eye_to_head_transform = reinterpret_cast<FuncPtr1>(&vr::IVRSystem::GetEyeToHeadTransform);
        typedef void (vr::IVRSystem::*FuncPtr2)(vr::HmdMatrix44_t *, vr::EVREye, float, float);
        FuncPtr2 get_projection = reinterpret_cast<FuncPtr2>(&vr::IVRSystem::GetProjectionMatrix);

        vr::HmdMatrix34_t leftView, rightView;
        (vr_sys->*get_eye_to_head_transform)(&leftView, vr::Eye_Left);
        (vr_sys->*get_eye_to_head_transform)(&rightView, vr::Eye_Right);

        m_leftPose = vrMatrixToGlm(leftView);
        m_rightPose = vrMatrixToGlm(rightView);

        vr::HmdMatrix44_t leftProj, rightProj;
        (vr_sys->*get_projection)(&leftProj, vr::Eye_Left, m_near, m_far);
        (vr_sys->*get_projection)(&rightProj, vr::Eye_Right, m_near, m_far);

        m_leftProjection = vrMatrixToGlm(leftProj);
        m_rightProjection = vrMatrixToGlm(rightProj);

    # else
        /* Instead of [Model * View * Projection]
         * The VR equivalent is [Model * View * Eye * Projection]
         *
         * Normally View and Eye will be multiplied together and
         * treated as View in your application.
         */
        m_leftPose = vrMatrixToGlm(m_hmd->GetEyeToHeadTransform(vr::Eye_Left));
        m_rightPose = vrMatrixToGlm(m_hmd->GetEyeToHeadTransform(vr::Eye_Right));

        m_leftProjection = vrMatrixToGlm(m_hmd->GetProjectionMatrix(vr::Eye_Left, m_near, m_far));
        m_rightProjection = vrMatrixToGlm(m_hmd->GetProjectionMatrix(vr::Eye_Right, m_near, m_far));
    #endif

        /* VR frame buffers for each eye*/
        m_leftEyeBuffer = std::make_shared<CS123::GL::FBO>(1, CS123::GL::FBO::DEPTH_STENCIL_ATTACHMENT::DEPTH_ONLY, m_eyeWidth, m_eyeHeight,
                                                CS123::GL::TextureParameters::WRAP_METHOD::CLAMP_TO_EDGE,
                                                CS123::GL::TextureParameters::FILTER_METHOD::LINEAR,
                                                GL_UNSIGNED_BYTE);
        m_rightEyeBuffer = std::make_shared<CS123::GL::FBO>(1, CS123::GL::FBO::DEPTH_STENCIL_ATTACHMENT::DEPTH_ONLY, m_eyeWidth, m_eyeHeight,
                                                 CS123::GL::TextureParameters::WRAP_METHOD::CLAMP_TO_EDGE,
                                                 CS123::GL::TextureParameters::FILTER_METHOD::LINEAR,
                                                 GL_UNSIGNED_BYTE);

        // Comment this to detach program from house.xml.
        fileOpen();

}

void View::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void View::paintGL() {

   if (m_hmd) {

       /* render the current location of the controllers */
       obtainControllerPositions();

       /* Left Eye */
       m_leftEyeBuffer->bind();
       glViewport(0, 0, m_eyeWidth, m_eyeHeight);
       renderEye(vr::Eye_Left);
       m_leftEyeBuffer->unbind();

       /* Right Eye */
       m_rightEyeBuffer->bind();
       glViewport(0, 0, m_eyeWidth, m_eyeHeight);
       renderEye(vr::Eye_Right);
       m_rightEyeBuffer->unbind();

       /* Submit frames to the Compositor */
       vr::Texture_t leftEyeTexture = {(void*) m_leftEyeBuffer->getColorAttachment(0).id(), vr::TextureType_OpenGL, vr::ColorSpace_Gamma};
       /* Updated scene texture to display for left eye*/
       vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);

       vr::Texture_t rightEyeTexture = {(void*) m_rightEyeBuffer->getColorAttachment(0).id(), vr::TextureType_OpenGL, vr::ColorSpace_Gamma};
       /* Updated scene texture to display for right eye*/
       vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);

   } else {
       glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

       /* render the selected scene */
       if(m_scene != nullptr) {
           m_scene->render(m_camera.getProjectionMatrix(), m_camera.getViewMatrix());
       }
   }
}

void View::renderEye(vr::EVREye eye) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4x4 p = eye == vr::Eye_Left ? m_leftProjection : m_rightProjection;
    glm::mat4x4 v = (eye == vr::Eye_Right ? m_leftPose : m_rightPose) * m_hmdPose;

    /* render the selected scene */
    if(m_scene != nullptr) {
        m_scene->render(p, v);
    }
}

void View::obtainControllerPositions() {

    /* don't draw controllers if somebody else has input focus */
    if(m_hmd->IsInputFocusCapturedByAnotherProcess() )
        return;

    /* enumerate all controllers */
    for (vr::TrackedDeviceIndex_t trackedDeviceIndex = vr::k_unTrackedDeviceIndex_Hmd + 1;
         trackedDeviceIndex < vr::k_unMaxTrackedDeviceCount;
         trackedDeviceIndex++) {

        /* controller not connected, ignore it */
        if (!m_hmd->IsTrackedDeviceConnected(trackedDeviceIndex))
            continue;

        /* not a hand controller, ignore it */
        if(m_hmd->GetTrackedDeviceClass(trackedDeviceIndex) != vr::TrackedDeviceClass_Controller)
            continue;


        /* controller position invalid, ignore it */
        if(!m_trackedDevicePose[trackedDeviceIndex].bPoseIsValid)
            continue;

        /* controller transformation matrix, use it as needed */
        const glm::mat4 &mat = m_mat4DevicePose[trackedDeviceIndex];
    }
}



void View::updatePoses() {

    /* You can update your head and hand positions here */
    if(m_hmd) {
        vr::VRCompositor()->WaitGetPoses(m_trackedDevicePose, vr::k_unMaxTrackedDeviceCount, nullptr, 0);

        if (m_trackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid) {
            glm::mat4 h = vrMatrixToGlm(m_trackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking);
            m_hmdPose = glm::inverse(h);
        }
    }
}

void View::updateInputs() {

    /* don't render when not using headset */
    if (!m_hmd) {

        float moveSpeed = 0.5f;
        float rotAmt = 1;

        if(m_keys[Qt::Key_W]) {
            m_camera.translate(m_camera.getLook() * moveSpeed);
        }
        if(m_keys[Qt::Key_S]) {
            m_camera.translate(-m_camera.getLook() * moveSpeed);
        }

        if(m_keys[Qt::Key_A]) {
            m_camera.translate(-m_camera.getRight() * moveSpeed);
        }
        if(m_keys[Qt::Key_D]) {
            m_camera.translate(m_camera.getRight() * moveSpeed);
        }

        if(m_keys[Qt::Key_Z]) {
            m_camera.translate(-m_camera.getUp() * moveSpeed);
        }
        if(m_keys[Qt::Key_X]) {
            m_camera.translate(m_camera.getUp() * moveSpeed);
        }

        if(m_keys[Qt::Key_U]) {
            m_camera.rotateU(rotAmt);
        }
        if(m_keys[Qt::Key_J]) {
            m_camera.rotateU(-rotAmt);
        }

        if(m_keys[Qt::Key_H]) {
            m_camera.rotateV(rotAmt);
        }
        if(m_keys[Qt::Key_K]) {
            m_camera.rotateV(-rotAmt);
        }

        if(m_keys[Qt::Key_N]) {
            m_camera.rotateW(-rotAmt);
        }
        if(m_keys[Qt::Key_M]) {
            m_camera.rotateW(rotAmt);
        }

        return;
    }

    /* process controller inputs */
    for(vr::TrackedDeviceIndex_t deviceIndex = 0;
        deviceIndex < vr::k_unMaxTrackedDeviceCount;
        deviceIndex++) {

        vr::VRControllerState_t state;
        if(m_hmd->GetControllerState(deviceIndex, &state, sizeof(state))) {
            m_activeTrackedDevice[deviceIndex] = (state.ulButtonPressed == 0);

            if (state.ulButtonPressed) {
                /* controller button is presses */
            }

            if (state.ulButtonTouched) {
                /* controller button is being touched */
            }
        }
    }

    for (unsigned int deviceIndex = 0; deviceIndex < vr::k_unMaxTrackedDeviceCount; deviceIndex++) {
        if (m_trackedDevicePose[deviceIndex].bPoseIsValid) {
            /* update the controller's position matrix */
            m_mat4DevicePose[deviceIndex] = vrMatrixToGlm(m_trackedDevicePose[deviceIndex].mDeviceToAbsoluteTracking);
        } else {
            /* invalid position, don't track */
            m_activeTrackedDevice[deviceIndex] = false;
        }
    }
}

void View::tick() {
    /* Get the number of seconds since the last tick (variable update rate) */
    float dt = m_time.restart() * 0.001f;

    /* VR updates */
    updatePoses();
    updateInputs();

    // TODO: do any other updating here

    /* Flag this view for repainting (Qt will call paintGL() soon after) */
    update();
}

/* Keyboard events */

void View::keyPressEvent(QKeyEvent *event) {

    if(event->key() >= 0 && event->key() < 68836) {
        m_keys[event->key()] = true;
    }

}

void View::keyReleaseEvent(QKeyEvent *event) {

    if(event->key() >= 0 && event->key() < 68836) {
        m_keys[event->key()] = false;
    }
}

/* Mouse events */

void View::mousePressEvent(QMouseEvent *event) {}

void View::mouseReleaseEvent(QMouseEvent *event) {}

void View::mouseMoveEvent(QMouseEvent *event) {
   /* This starter code implements mouse capture, which gives the change in
    * mouse position since the last mouse movement. The mouse needs to be
    * recentered after every movement because it might otherwise run into
    * the edge of the screen, which would stop the user from moving further
    * in that direction. Note that it is important to check that deltaX and
    * deltaY are not zero before recentering the mouse, otherwise there will
    * be an infinite loop of mouse move events.
    */
    if(m_captureMouse) {
        int deltaX = event->x() - width() / 2;
        int deltaY = event->y() - height() / 2;
        if (!deltaX && !deltaY) return;
        QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));

        // TODO: Handle mouse movements here
    }
}


/* File dialog */
void View::fileOpen() {

    // Uncomment this for file dialog.
    //QString file = QFileDialog::getOpenFileName(this, "Open File", "../tavr/scenes");

    // Comment this to detach the program from house.xml.
    QString file = "C:/Users/Roxas/Desktop/msteinb1/cs123-final/vr/scenes/house.xml";

    if (!file.isNull() && file.endsWith(".xml")) {
        CS123XmlSceneParser parser(file.toLatin1().data());
        if (parser.parse()) {
            m_scene = std::make_unique<SceneviewScene>(m_eyeWidth, m_eyeHeight);
            Scene::parse(m_scene.get(), &parser);
        } else {
            QMessageBox::critical(this, "Error", "Could not load scene \"" + file + "\"");
        }
    }
}
