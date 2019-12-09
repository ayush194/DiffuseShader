#ifndef APP_H
#define APP_H

#include "opengl_includes.h"
//glm for matrix operations
#include "glm_includes.h"
#include "camera.h"

class App {
public:
    static unsigned int SCR_WIDTH;
    static unsigned int SCR_HEIGHT;
    static float LAST_TIME;
    static float DELTA_TIME;
    static float CURSOR_POS[2];
    static bool FIRST_MOUSE;
    static Camera CAMERA;
    static GLFWwindow* window;

    App();
    static void updateFrame();
    static void endFrame();
    static void clearColor();
    static glm::mat4 getViewMatrix();
    static glm::mat4 getPerspectiveProjectionMatrix();
    static glm::mat4 getOrthographicProjectionMatrix();
    static void cursorPosCallback(GLFWwindow*, double, double);
    static void scrollCallback(GLFWwindow*, double, double);
    static void framebufferSizeCallback(GLFWwindow*, int, int);
    static void errorCallback(int, const char*);
    static void processInput();
    static void drawAxes();
};

#endif