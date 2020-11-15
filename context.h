#pragma once
#include <GLFW/glfw3.h>

#include "utils/shader.h"
#include "utils/logger.h"
#include "camera/camera.h"

namespace ctx {
    bool is_lmb_pressed = false;

    int width = 1920;
    int height = 1080;

    double mouse_x = 0.;
    double mouse_y = 0.;

    double scroll_x = 0.;
    double scroll_y = 0.;

    double start_time;
    double time_on_pause = 0.;
    double paused_time;

    Camera cam = Camera(glm::vec3(0., 0., 3.), glm::vec3(0., 0., -1.), glm::vec3(0., 1., 0.));

    lg::Logger logger;

    sh::Shader shader;

    GLFWwindow* window;

    double delta_time;
    int fps;

    double timer;
}
