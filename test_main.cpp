/*

int static window_width = 800;
int static window_height = 600;

double static cursor_xpos;
double static cursor_ypos;
GLFWcursor* hresizeCursor = nullptr;
GLFWcursor* vresizeCursor = nullptr;
GLFWcursor* handCursor = nullptr;
void glfwInitCursors();

void draw(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow* window);
*/
// Temp Storage
/*
/// Temp Storage
///---------------------------------------------------------------------------------
struct Color4 {
    float R;
    float G;
    float B;
    float A;
};

struct Viewport {
    Viewport(int xpos, int ypos, int xsize, int ysize, Color4 color) {
        this->xpos = xpos;
        this->ypos = ypos;
        this->xsize = xsize;
        this->ysize = ysize;
        this->color = color;
    }
    void draw() {
        glScissor(xpos, ypos, xsize, ysize);
        glViewport(xpos, ypos, xsize, ysize);
        glClearColor(color.R, color.G, color.B, color.A);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    void resize(int xsize, int ysize) {
        this->xsize = xsize;
        this->ysize = ysize;
    }
private:
    int xpos = 0;
    int ypos = 0;
    int xsize = 0;
    int ysize = 0;
    Color4 color = { 0.f, 0.f, 0.f, 1.f };
};

Viewport testViewport(0, 0, window_width, window_height, { 1.f, 0.f, 1.f, 1.f });
///---------------------------------------------------------------------------------
*/
/*
// Bottom viewport ratio (of total window height)
float ratio0[2] = { 1.0f, 0.5f };    // Bottom viewport takes up half of window height
// Top viewports ratio (width ratio, height ratio)
float ratio1[2] = { 0.25f, 0.5f };  // Top-left viewport
float ratio2[2] = { 0.25f, 0.5f };  // Top-right viewport
float ratio3[2] = { 0.5f, 0.5f };


double on_click_pos[2] = { 0.0, 0.0 };
double last_drag_pos[2] = { 0.0, 0.0 };
bool flag_holding = false;
bool flag_moving[2] = { false, false };
*/
/*
struct Div {
    int x_bound;
    int y_bound;
public:
    bool inbounds(float offset_x = 0.f, float offset_y = 0.f) const {
        return (std::abs(cursor_xpos - x_bound) <= offset_x) && (std::abs(cursor_ypos - y_bound) <= offset_y);
    }
    const bool inbounds_x(float offset = 0.f, float corner_padding = 0.f) {
        bool inbounds_y = corner_padding <= cursor_ypos && cursor_ypos <= y_bound - corner_padding;
        return (std::abs(cursor_xpos - x_bound) <= offset) && inbounds_y;
    }
    const bool inbounds_y(float offset = 0.f, float corner_padding = 0.f) {
        bool inbounds_x = corner_padding <= cursor_ypos && cursor_ypos <= y_bound - corner_padding;
        return (std::abs(cursor_ypos - y_bound) <= offset) && inbounds_x;
    }
};

struct VDiv {
    int x_bound;
    int y_bound[2];
public:
    bool inbounds(float offset_x = 0.f, float offset_y = 0.f) const {
        int flipped_cursor_ypos = window_height - cursor_ypos;
        bool inbounds_y = (y_bound[0] + offset_y) <= flipped_cursor_ypos && flipped_cursor_ypos <= (y_bound[1] - offset_y);
        return (std::abs(cursor_xpos - x_bound) <= offset_x) && inbounds_y;
    }
};
*/

// Main
/*
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "OpenUI", NULL, NULL);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwInitCursors();

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glEnable(GL_SCISSOR_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        int x_boundary = window_width * ratio1[0];
        int y_boundary = window_height * (1.0f - ratio0[1]); // Corrected position calculation

        Div vertical0{ window_width * (ratio1[0]), window_height * (1.0f - ratio1[1]) };
        Div vertical1{ window_width * (ratio1[0] + ratio2[0]), window_height * (1.0f - ratio1[1]) };



        bool inbounds_x = 10.0 <= cursor_xpos && cursor_xpos <= window_width - 10.0;
        bool on_horizontal_separation = std::abs(cursor_ypos - y_boundary) <= 7.5 && inbounds_x;


        VDiv vert0{ window_width * (ratio1[0]),             { window_height * ratio0[1], window_height * (ratio0[1] + ratio1[1]) } };
        VDiv vert1{ window_width * (ratio1[0] + ratio2[0]), { window_height * ratio0[1], window_height * (ratio0[1] + ratio1[1]) } };

        bool on_vert0 = vert0.inbounds(7.5f, 10.0f);
        bool on_vert1 = vert1.inbounds(7.5f, 10.0f);

        VDiv vertDivs[2] = {
            { window_width * (ratio1[0]),             { window_height * ratio0[1], window_height * (ratio0[1] + ratio1[1]) } },
            { window_width * (ratio1[0] + ratio2[0]), { window_height * ratio0[1], window_height * (ratio0[1] + ratio1[1]) } }
        };

        for (VDiv vertDiv : vertDivs) {

        }

        // Set cursor based on position and state
        if (on_vert0 || on_vert1 && !flag_holding) {
            glfwSetCursor(window, hresizeCursor);
        }
        else if (on_horizontal_separation && !flag_holding) {
            glfwSetCursor(window, vresizeCursor);
        }
        else if (!flag_holding) {
            glfwSetCursor(window, NULL);
        }

        // Handle dragging for horizontal divider
        if (flag_moving[1]) {
            double delta_y = cursor_ypos - last_drag_pos[1];
            float ratio_change = delta_y / window_height;

            // Update the bottom height ratio (inversely)
            float new_ratio = ratio0[1] - ratio_change;
            if (new_ratio >= 0.1f && new_ratio <= 0.9f) {
                ratio0[1] = new_ratio;

                // Top viewports height should be complement of bottom ratio
                ratio1[1] = 1.0f - ratio0[1];
                ratio2[1] = 1.0f - ratio0[1];
            }

            last_drag_pos[0] = cursor_xpos;
            last_drag_pos[1] = cursor_ypos;
        }

        // Handle dragging for vertical divider
        if (flag_moving[0]) {
            double delta_x = cursor_xpos - last_drag_pos[0];
            float ratio_change = delta_x / window_width;

            // Update ratio with constraints
            float new_ratio = ratio1[0] + ratio_change;
            if (new_ratio >= 0.1f && new_ratio <= 0.9f) {
                ratio1[0] = new_ratio;
                ratio2[0] -= ratio_change;
            }

            last_drag_pos[0] = cursor_xpos;
            last_drag_pos[1] = cursor_ypos;
        }

        draw(window);

        glfwPollEvents();
    }

    glfwDestroyCursor(hresizeCursor);
    glfwDestroyCursor(vresizeCursor);
    glfwDestroyCursor(handCursor);
    glfwTerminate();
    return 0;
}

void glfwInitCursors() {
    hresizeCursor = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    vresizeCursor = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
    handCursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    cursor_xpos = xpos;
    cursor_ypos = ypos;
}

void draw(GLFWwindow* window) {
    // Bottom viewport
    glScissor(0, 0, window_width, window_height * ratio0[1]);
    glViewport(0, 0, window_width, window_height * ratio0[1]);
    glClearColor(0.f, 1.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Calculate the Y position for top viewports
    int topY = window_height * ratio0[1];
    int topHeight = window_height * (1.0f - ratio0[1]);

    // Top-left viewport 
    int topL = window_width * (0);

    glScissor(topL, topY, window_width * ratio1[0], topHeight);
    glViewport(topL, topY, window_width * ratio1[0], topHeight);
    glClearColor(1.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Top-middle viewport
    int topM = window_width * (ratio1[0]);

    glScissor(topM, topY, window_width * ratio2[0], topHeight);
    glViewport(topM, topY, window_width * ratio2[0], topHeight);
    glClearColor(0.f, 0.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Top-right viewport
    int topR = window_width * (ratio1[0] + ratio2[0]);

    glScissor(topR, topY, window_width * ratio3[0], topHeight);
    glViewport(topR, topY, window_width * ratio3[0], topHeight);
    glClearColor(1.f, 0.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);



    glfwSwapBuffers(window);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    window_width = width;
    window_height = height;
    testViewport.resize(width, height);
    glViewport(0, 0, width, height);
    draw(window);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            glfwGetCursorPos(window, &on_click_pos[0], &on_click_pos[1]);
            last_drag_pos[0] = on_click_pos[0];
            last_drag_pos[1] = on_click_pos[1];

            int x_boundary = window_width * ratio1[0];
            int y_boundary = window_height * (1.0f - ratio0[1]); // Corrected position

            bool inbounds_y = 10.0 <= cursor_ypos && cursor_ypos <= y_boundary - 10.0;
            bool inbounds_x = 10.0 <= cursor_xpos && cursor_xpos <= window_width - 10.0;

            if (std::abs(cursor_xpos - x_boundary) <= 7.5 && inbounds_y) {
                flag_moving[0] = true;
                flag_holding = true;
                glfwSetCursor(window, hresizeCursor);
            }
            else if (std::abs(cursor_ypos - y_boundary) <= 7.5 && inbounds_x) {
                flag_moving[1] = true;
                flag_holding = true;
                glfwSetCursor(window, vresizeCursor);
            }
        }
        else if (action == GLFW_RELEASE) {
            flag_moving[0] = false;
            flag_moving[1] = false;
            flag_holding = false;
        }
    }
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}
*/