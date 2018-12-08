#ifndef VIEW_H
#define VIEW_H

#include "GL/glew.h"
#include <qgl.h>
#include <QTime>
#include <QTimer>
#include <memory>
#include <openvr.h>
#include "glm/glm.hpp"

#include "gl/datatype/FBO.h"
#include "camera/QuaternionCamera.h"

class SceneviewScene;

class View : public QGLWidget {
    Q_OBJECT

public:
    View(QWidget *parent);
    ~View();

private:
    QTime m_time;
    QTimer m_timer;
    bool m_captureMouse;
    bool m_keys[68836];

    std::unique_ptr<SceneviewScene> m_scene;
    QuaternionCamera m_camera;

    float m_near, m_far;

    // head mounted display (goggles)
    vr::IVRSystem *m_hmd;

    // position of the tracking devices (hand controllers and goggles)
    vr::TrackedDevicePose_t m_trackedDevicePose[vr::k_unMaxTrackedDeviceCount];
    glm::mat4 m_mat4DevicePose [vr::k_unMaxTrackedDeviceCount];
    bool m_activeTrackedDevice[vr::k_unMaxTrackedDeviceCount];

    // size of each eye "screen"
    uint32_t m_eyeWidth, m_eyeHeight;

    // view matrices for rendering the vr scene
    glm::mat4 m_leftProjection, m_leftPose;
    glm::mat4 m_rightProjection, m_rightPose;
    glm::mat4 m_hmdPose;

    // vr eye frame buffers
    std::shared_ptr<CS123::GL::FBO> m_leftEyeBuffer;
    std::shared_ptr<CS123::GL::FBO> m_rightEyeBuffer;
    bool createFrameBuffer(int nWidth, int nHeight, CS123::GL::FBO &eyeFBO);

    // initialized openGL
    void initializeGL();

    // initialized the vr system
    void initVR();
    void paintGL();

    // render the scene for a given eye
    void renderEye(vr::EVREye eye);
    void resizeGL(int w, int h);

    // hand controllers pos
    void obtainControllerPositions();

    // updates the position of the hmd
    void updatePoses();

    // updates current controller inputs
    void updateInputs();

    // mouse and keyboard events
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    // converts a vr hmd matrix to a glm::mat4
    glm::mat4 vrMatrixToGlm(const vr::HmdMatrix34_t &mat) {
        return glm::mat4x4(
            mat.m[0][0], mat.m[1][0], mat.m[2][0], 0.0f,
            mat.m[0][1], mat.m[1][1], mat.m[2][1], 0.0f,
            mat.m[0][2], mat.m[1][2], mat.m[2][2], 0.0f,
            mat.m[0][3], mat.m[1][3], mat.m[2][3], 1.0f
        );
    }

    // converts a vr hmd matrix to a glm::mat4
    glm::mat4 vrMatrixToGlm(const vr::HmdMatrix44_t &mat) {
        return glm::mat4x4(
            mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
            mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
            mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
            mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]
        );
    }

public slots:
    void fileOpen();

private slots:
    void tick();
};

#endif // VIEW_H
