#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
// Exception handling
#include <stdexcept>
// Random Viewport debug colors
#include <random>
std::random_device rng;
std::uniform_real_distribution<float> colorDist(0.25f, 1.f);


int static window_width = 800;
int static window_height = 600;

double static cursor_xpos;
double static cursor_ypos;
GLFWcursor* hresizeCursor = nullptr;
GLFWcursor* vresizeCursor = nullptr;

bool flag_holding = false;
std::vector<bool> flag_moving_horizontal;
std::vector<bool> flag_moving_vertical;


std::vector<float> topViewportRatios = { 0.33f, 0.33f, 0.34f }; // Three viewports
std::vector<float> verticalViewportRatios = { 0.25f, 0.5f, 0.25f };
float bottomHeightRatio = 0.5f;

double last_drag_pos[2] = { 0.0, 0.0 };

struct Viewport {
    Viewport() {
        debug_RGBA[0] = colorDist(rng); // R
        debug_RGBA[1] = colorDist(rng); // G
        debug_RGBA[2] = colorDist(rng); // B
    }
public:
    void draw(int xpos, int ypos, int width, int height) {
        glScissor(xpos, ypos, width, height);
        glViewport(xpos, ypos, width, height);
        glClearColor(debug_RGBA[0], debug_RGBA[1], debug_RGBA[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
private:
    float debug_RGBA[3];
};

static Viewport vpV[2]; // vertical viewports
static Viewport vpH[3]; // Top row, horizontal viewports

void draw(GLFWwindow* window) {
    int topY = window_height * bottomHeightRatio;
    int topHeight = window_height * (1.0f - bottomHeightRatio);
    int xStart = 0;

    int yStart = 0;
    for (size_t i = 0; i < verticalViewportRatios.size(); i++) {
        int height = window_height * verticalViewportRatios[i];

        vpV[i].draw(0, yStart, window_width, height);
        /*
        glScissor(0, yStart, window_width, window_height * bottomHeightRatio);
        glViewport(0, yStart, window_width, window_height * bottomHeightRatio);
        glClearColor(colorDist(rng), colorDist(rng), colorDist(rng), 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        */
        if (i == 2) {
            for (size_t j = 0; j < topViewportRatios.size(); j++) {
                int width = window_width * topViewportRatios[j];

                vpH[j].draw(xStart, yStart, width, height);
                /*
                glScissor(xStart, yStart, width, height);
                glViewport(xStart, yStart, width, height);
                glClearColor(colorDist(rng), colorDist(rng), colorDist(rng), 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                */
                xStart += width;
            }
        }

        yStart += height;
    }
    
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
            // Handle mouse control over horizontal resizing
            int xStart = 0;
            for (size_t i = 0; i < topViewportRatios.size() - 1; i++) {
                xStart += window_width * topViewportRatios[i];
                if (std::abs(cursor_xpos - xStart) <= 7.5) {
                    flag_moving_horizontal[i] = true;
                    flag_holding = true;
                }
            }
            // Handle mouse control over vertical resizing
            int yStart = 0;
            for (size_t i = 0; i < verticalViewportRatios.size() - 1; i++) {
                yStart += window_height * verticalViewportRatios[i];
                if (std::abs((window_height - cursor_ypos) - yStart) <= 7.5) {
                    flag_moving_vertical[i] = true;
                    flag_holding = true;
                }
            }
            /*
            int yBoundary = window_height * bottomHeightRatio;
            if (std::abs((window_height - cursor_ypos) - yBoundary) <= 7.5) {
                flag_moving_vertical = true;
                flag_holding = true;
            }
            */
        }
        else if (action == GLFW_RELEASE) {
            std::fill(flag_moving_horizontal.begin(), flag_moving_horizontal.end(), false);
            std::fill(flag_moving_vertical.begin(), flag_moving_vertical.end(), false);
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
    for (size_t i = 0; i < flag_moving_horizontal.size(); i++) {
        if (flag_moving_horizontal[i]) {
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

    for (size_t i = 0; i < flag_moving_vertical.size(); i++) {
        if (flag_moving_vertical[i]) {
            double delta_y = cursor_ypos - last_drag_pos[1];
            float ratio_change = -delta_y / window_height;
            float new_ratio = verticalViewportRatios[i] + ratio_change;
            float new_next_ratio = verticalViewportRatios[i + 1] - ratio_change;

            if (new_ratio >= 0.1f && new_next_ratio >= 0.1f) {
                verticalViewportRatios[i] = new_ratio;
                verticalViewportRatios[i + 1] = new_next_ratio;
            }
            last_drag_pos[1] = cursor_ypos;
        }

    }


    
}

int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "OpenUI", NULL, NULL);
    glfwMakeContextCurrent(window);
    glEnable(GL_SCISSOR_TEST);

    flag_moving_horizontal.resize(topViewportRatios.size() - 1, false);
    flag_moving_vertical.resize(verticalViewportRatios.size() - 1, false);

    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        updateViewportRatios();
        draw(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
