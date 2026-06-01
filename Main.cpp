#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
// Random Viewport debug colors
#include <random>
std::random_device rng;
std::uniform_real_distribution<float> colorDist(0.25f, 1.f);

int static window_width = 800;
int static window_height = 600;

double static cursor_xpos;
double static cursor_ypos;

double last_drag_pos[2] = { 0.0, 0.0 };
const float minViewportRatio = 0.1f;
const double dividerHitRadius = 7.5;

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

int ratioToPixels(int totalPixels, float ratio) {
    return static_cast<int>(totalPixels * ratio);
}

float clampRatioChange(float ratio, float nextRatio, float ratioChange) {
    float minChange = minViewportRatio - ratio;
    float maxChange = nextRatio - minViewportRatio;
    return std::clamp(ratioChange, minChange, maxChange);
}

enum class SplitAxis {
    Horizontal,
    Vertical
};

struct Rect {
    int xpos = 0;
    int ypos = 0;
    int width = 0;
    int height = 0;

    bool contains(double x, double y) const {
        return xpos <= x && x <= xpos + width && ypos <= y && y <= ypos + height;
    }
};

struct ViewportNode {
    ViewportNode() = default;
    //TODO: rename first and second -> left and right to match tree terminology
    ViewportNode(SplitAxis axis, float firstRatio, ViewportNode* firstChild, ViewportNode* secondChild) {
        isLeaf = false;
        splitAxis = axis;
        splitRatio = firstRatio;
        first = firstChild;
        second = secondChild;
    }

    bool isLeaf = true;
    SplitAxis splitAxis = SplitAxis::Horizontal;
    float splitRatio = 0.5f;
    Viewport viewport;
    ViewportNode* first = nullptr;
    ViewportNode* second = nullptr;
    Rect rect;
    Rect firstRect;
    Rect secondRect;
};

//-------------------------------------------------------------------------------------------------
/// Temporary static creation of viewports for debugging viewport functionality.
// TODO: dynamically split and join these nodes based on user interactions and application needs.

static ViewportNode bottomViewport;
static ViewportNode middleViewport;
static ViewportNode topLeftViewport;
static ViewportNode topMiddleViewport;
static ViewportNode topRightViewport;

static ViewportNode topMiddleRightSplit(
    SplitAxis::Vertical,
    0.33f / (0.33f + 0.34f),
    &topMiddleViewport,
    &topRightViewport
);

static ViewportNode topRowSplit(
    SplitAxis::Vertical,
    0.33f,
    &topLeftViewport,
    &topMiddleRightSplit
);

static ViewportNode upperRowsSplit(
    SplitAxis::Horizontal,
    0.5f / (0.5f + 0.25f),
    &middleViewport,
    &topRowSplit
);

static ViewportNode viewportRoot(
    SplitAxis::Horizontal,
    0.25f,
    &bottomViewport,
    &upperRowsSplit
);

static ViewportNode* activeSplit = nullptr;
//-------------------------------------------------------------------------------------------------

void updateChildRects(ViewportNode& node) {
    if (node.splitAxis == SplitAxis::Horizontal) {
        int firstHeight = ratioToPixels(node.rect.height, node.splitRatio);
        node.firstRect = { node.rect.xpos, node.rect.ypos, node.rect.width, firstHeight };
        node.secondRect = {
            node.rect.xpos,
            node.rect.ypos + firstHeight,
            node.rect.width,
            node.rect.height - firstHeight
        };
    }
    else {
        int firstWidth = ratioToPixels(node.rect.width, node.splitRatio);
        node.firstRect = { node.rect.xpos, node.rect.ypos, firstWidth, node.rect.height };
        node.secondRect = {
            node.rect.xpos + firstWidth,
            node.rect.ypos,
            node.rect.width - firstWidth,
            node.rect.height
        };
    }
}

void drawViewportTree(ViewportNode& node, Rect rect) {
    node.rect = rect;

    if (node.isLeaf) {
        node.viewport.draw(rect.xpos, rect.ypos, rect.width, rect.height);
        return;
    }

    updateChildRects(node);
    drawViewportTree(*node.first, node.firstRect);
    drawViewportTree(*node.second, node.secondRect);
}

void draw() {
    if (window_width <= 0 || window_height <= 0) {
        return;
    }

    drawViewportTree(viewportRoot, { 0, 0, window_width, window_height });
}

ViewportNode* hitTestViewportSplit(ViewportNode& node, double xpos, double ypos) {
    if (node.isLeaf || !node.rect.contains(xpos, ypos)) {
        return nullptr;
    }

    updateChildRects(node);

    if (node.firstRect.contains(xpos, ypos)) {
        if (ViewportNode* split = hitTestViewportSplit(*node.first, xpos, ypos)) {
            return split;
        }
    }

    if (node.secondRect.contains(xpos, ypos)) {
        if (ViewportNode* split = hitTestViewportSplit(*node.second, xpos, ypos)) {
            return split;
        }
    }

    if (node.splitAxis == SplitAxis::Horizontal) {
        int dividerY = node.firstRect.ypos + node.firstRect.height;
        bool cursorWithinSplit = node.rect.xpos <= xpos && xpos <= node.rect.xpos + node.rect.width;
        if (cursorWithinSplit && std::abs(ypos - dividerY) <= dividerHitRadius) {
            return &node;
        }
    }
    else {
        int dividerX = node.firstRect.xpos + node.firstRect.width;
        bool cursorWithinSplit = node.rect.ypos <= ypos && ypos <= node.rect.ypos + node.rect.height;
        if (cursorWithinSplit && std::abs(xpos - dividerX) <= dividerHitRadius) {
            return &node;
        }
    }

    return nullptr;
}

void cursor_position_callback(GLFWwindow*, double xpos, double ypos) {
    cursor_xpos = xpos;
    cursor_ypos = ypos;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    window_width = width;
    window_height = height;

    if (window_width > 0 && window_height > 0) {
        draw();
        glfwSwapBuffers(window);
    }
}

void mouse_button_callback(GLFWwindow*, int button, int action, int) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            last_drag_pos[0] = cursor_xpos;
            last_drag_pos[1] = cursor_ypos;

            double flippedCursorY = window_height - cursor_ypos;
            activeSplit = hitTestViewportSplit(viewportRoot, cursor_xpos, flippedCursorY);
        }
        else if (action == GLFW_RELEASE) {
            activeSplit = nullptr;
        }
    }
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void updateViewportTree(GLFWwindow* window) {
    if (window_width <= 0 || window_height <= 0) {
        return;
    }

    if (activeSplit != nullptr) {
        glfwGetCursorPos(window, &cursor_xpos, &cursor_ypos);

        if (activeSplit->splitAxis == SplitAxis::Vertical) {
            if (activeSplit->rect.width <= 0) {
                return;
            }

            double delta_x = cursor_xpos - last_drag_pos[0];
            float ratio_change = static_cast<float>(delta_x / activeSplit->rect.width);
            float applied_change = clampRatioChange(activeSplit->splitRatio, 1.0f - activeSplit->splitRatio, ratio_change);

            activeSplit->splitRatio += applied_change;
            last_drag_pos[0] += applied_change * activeSplit->rect.width;
        }
        else {
            if (activeSplit->rect.height <= 0) {
                return;
            }

            double delta_y = cursor_ypos - last_drag_pos[1];
            float ratio_change = static_cast<float>(-delta_y / activeSplit->rect.height);
            float applied_change = clampRatioChange(activeSplit->splitRatio, 1.0f - activeSplit->splitRatio, ratio_change);

            activeSplit->splitRatio += applied_change;
            last_drag_pos[1] -= applied_change * activeSplit->rect.height;
        }
    }
}

int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "OpenUI", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwGetFramebufferSize(window, &window_width, &window_height);
    glEnable(GL_SCISSOR_TEST);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        updateViewportTree(window);
        draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
