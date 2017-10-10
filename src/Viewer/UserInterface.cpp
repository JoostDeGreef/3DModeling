#include <algorithm>
#include <chrono>
#include <list>
#include <map>
#include <string>
#include <thread>
#include <vector>
#include <mutex>

using namespace std;
using namespace std::chrono_literals;

#include "GLAD/glad.h"
#include "GLFW/glfw3.h"

#include "Aliases.h"
using namespace Geometry;

#include "UserInterface.h"
using namespace Viewer;

class State
{
public:
    void KeyCallback(const int key, const int scancode, const int action, const int mods);
    void CharCallback(const unsigned int c);
    void CursorPosCallback(const double& xpos, const double& ypos);
    void MouseButtonCallback(const int button, const int action, const int mods);
    void WindowSizeCallback(const int width, const int height);
    void ScrollCallback(const double x, const double y);

    bool Init();
    bool Cleanup();
    void Run();
    void Draw();

private:
    GLFWwindow* m_window;
    std::mutex m_mutex;
    GLsizei m_width;
    GLsizei m_height;
};

namespace
{
    State& GetState()
    {
        static State state;
        return state;
    }
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        GetState().KeyCallback(key, scancode, action, mods);
    }

    static void char_callback(GLFWwindow *window, unsigned int c)
    {
        GetState().CharCallback(c);
    }

    static void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos)
    {
        GetState().CursorPosCallback(xpos, ypos);
    }

    static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
    {
        GetState().MouseButtonCallback(button, action, mods);
    }

    static void error_callback(int error, const char* description)
    {
        //    Log::Error("{0}", description);
    }

    static void window_size_callback(GLFWwindow* window, int width, int height)
    {
        GetState().WindowSizeCallback(width, height);
    }

    static void scroll_callback(GLFWwindow* window, double x, double y)
    {
        GetState().ScrollCallback(x, y);
    }

    static void  window_refresh_callback(GLFWwindow* window)
    {
        //Log::Error("window_refresh_callback");
    }
}

bool State::Init()
{
    //    Log::Info("Starting UI...");
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
    {
        //        Log::Error("Could not initialize OpenGL");
        return false;
    }
    return true;
}

bool State::Cleanup()
{
    glfwDestroyWindow(m_window);
    m_window = nullptr;
    glfwTerminate();
    return true;
}

void State::Run()
{
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    // load the background image from disk before showing the window
    m_window = glfwCreateWindow(800, 600, "Viewer", NULL, NULL);
    if (!m_window)
    {
        glfwTerminate();
        //Log::Error("Could not create OpenGL window");
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        //Log::Error("Could not load all required OpenGL functionality");
        exit(EXIT_FAILURE);
    }

    glfwGetFramebufferSize(m_window, &m_width, &m_height);
    WindowSizeCallback(m_width, m_height);

    glfwSwapInterval(1);
    glfwSetKeyCallback(m_window, key_callback);
    glfwSetWindowSizeCallback(m_window, window_size_callback);
    glfwSetWindowRefreshCallback(m_window, window_refresh_callback);
    glfwSetMouseButtonCallback(m_window, mouse_button_callback);
    glfwSetCursorPosCallback(m_window, cursor_pos_callback);
    glfwSetCharCallback(m_window, char_callback);
    glfwSetScrollCallback(m_window, scroll_callback);
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    auto render = [&]()
    {
        glfwMakeContextCurrent(m_window);
        GLfloat light_position[] = { 4.0, 4.0, 4.0, 0.0 };
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glShadeModel(GL_SMOOTH);
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        glEnable(GL_LIGHT0);
        glFrontFace(GL_CCW);
        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_LIGHTING);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquation(GL_FUNC_ADD);
        glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
        glDisable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        glStencilMask(0x00);
        glDepthFunc(GL_LESS);
        //glDepthFunc(GL_GREATER);
        while (!glfwWindowShouldClose(m_window))
        {
            Draw();
        }
    };

    glfwMakeContextCurrent(nullptr);
    thread renderThread(render);
    while (!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();
    }
    renderThread.join();

    glfwSetKeyCallback(m_window, nullptr);
    glfwSetWindowSizeCallback(m_window, nullptr);
    glfwSetWindowRefreshCallback(m_window, nullptr);
}

void State::Draw()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    bool text = false;
    glViewport(0, 0, m_width, m_height);
    glStencilMask(0xFF);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glStencilMask(0x00);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
//    glOrtho(-m_state.m_ratio, m_state.m_ratio, -1., 1., -1.1, 1.1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

//    m_background.Draw();
//    m_playingfield->Draw();

    // draw the mouse last
//    m_mouse.Draw();
    lock.unlock();

    glfwSwapBuffers(m_window);
}

void State::KeyCallback(const int key, const int scancode, const int action, const int mods)
{
    std::lock_guard<std::mutex> lock(m_mutex);
}

void State::CharCallback(const unsigned int c)
{
    std::lock_guard<std::mutex> lock(m_mutex);
}

void State::CursorPosCallback(const double& xpos, const double& ypos)
{
    std::lock_guard<std::mutex> lock(m_mutex);
}

void State::MouseButtonCallback(const int button, const int action, const int mods)
{
    std::lock_guard<std::mutex> lock(m_mutex);
}

void State::WindowSizeCallback(const int width, const int height)
{
    std::lock_guard<std::mutex> lock(m_mutex);
}

void State::ScrollCallback(const double x, const double y)
{
    std::lock_guard<std::mutex> lock(m_mutex);
}

UserInterface::UserInterface()
{
}
UserInterface::~UserInterface()
{
}

bool UserInterface::Init()
{
    return GetState().Init();
}

void UserInterface::Run()
{   
    return GetState().Run();
}

bool UserInterface::Cleanup()
{
    return GetState().Cleanup();
}

void UserInterface::Draw()
{
    return GetState().Draw();
}



