#include "OpenGLScene.h"

#include <GL/glew.h>

OpenGLScene::~OpenGLScene()
{
}

void OpenGLScene::setClearColor() {
    glClearColor(0, 0, 0, 0);
}
