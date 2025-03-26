#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>

int static window_width = 800;
int static window_height = 600;

double static cursor_xpos;
double static cursor_ypos;
GLFWcursor* hresizeCursor = nullptr;
GLFWcursor* vresizeCursor = nullptr;

bool flag_holding = false;
std::vector<bool> flag_moving;

typedef struct {
    float R, G, B, A;
} Color4;

struct Viewport {
    int xpos, ypos, xsize, ysize;
    Color4 color;
    void draw() {
        glScissor(xpos, ypos, xsize, ysize);
        glViewport(xpos, ypos, xsize, ysize);
        glClearColor(color.R, color.G, color.B, color.A);
        glClear(GL_COLOR_BUFFER_BIT);
    }
};

std::vector<float> topViewportRatios = { 0.25f, 0.25f, 0.25f, 0.25f }; // Three viewports
float bottomHeightRatio = 0.5f;

double last_drag_pos[2] = { 0.0, 0.0 };

void draw(GLFWwindow* window) {
    int topY = window_height * bottomHeightRatio;
    int topHeight = window_height * (1.0f - bottomHeightRatio);
    int xStart = 0;

    for (size_t i = 0; i < topViewportRatios.size(); i++) {
        int width = window_width * topViewportRatios[i];
        glScissor(xStart, topY, width, topHeight);
        glViewport(xStart, topY, width, topHeight);
        glClearColor(i % 2 == 0 ? 1.0f : 0.0f, i % 3 == 0 ? 1.0f : 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        xStart += width;
    }

    glScissor(0, 0, window_width, window_height * bottomHeightRatio);
    glViewport(0, 0, window_width, window_height * bottomHeightRatio);
    glClearColor(0.f, 1.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    cursor_xpos = xpos;
    cursor_ypos = ypos;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            last_drag_pos[0] = cursor_xpos;
            last_drag_pos[1] = cursor_ypos;

            int xStart = 0;
            for (size_t i = 0; i < topViewportRatios.size() - 1; i++) {
                xStart += window_width * topViewportRatios[i];
                if (std::abs(cursor_xpos - xStart) <= 7.5) {
                    flag_moving[i] = true;
                    flag_holding = true;
                }
            }
        }
        else if (action == GLFW_RELEASE) {
            std::fill(flag_moving.begin(), flag_moving.end(), false);
            flag_holding = false;
        }
    }
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void updateViewportRatios() {
    for (size_t i = 0; i < flag_moving.size(); i++) {
        if (flag_moving[i]) {
            double delta_x = cursor_xpos - last_drag_pos[0];
            float ratio_change = delta_x / window_width;
            float new_ratio = topViewportRatios[i] + ratio_change;
            float new_next_ratio = topViewportRatios[i + 1] - ratio_change;

            if (new_ratio >= 0.1f && new_next_ratio >= 0.1f) {
                topViewportRatios[i] = new_ratio;
                topViewportRatios[i + 1] = new_next_ratio;
            }
            last_drag_pos[0] = cursor_xpos;
        }
    }
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "OpenUI", NULL, NULL);
    glfwMakeContextCurrent(window);
    glEnable(GL_SCISSOR_TEST);

    flag_moving.resize(topViewportRatios.size() - 1, false);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        updateViewportRatios();
        draw(window);

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}