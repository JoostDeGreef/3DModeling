#include <assert.h>
#include <algorithm>
#include <chrono>
#include <list>
#include <map>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <functional>

using namespace std;
using namespace std::chrono_literals;

#include "Geometry.h"
#include "GLWrappers.h"
#include "Font.h"
#include "Menu.h"
#include "UserInterface.h"
#include "RenderObjects.h"
#include "RenderInfo.h"
#include "Shaders.h"
#include "Settings.h"
using namespace Geometry;

namespace Viewer
{
    namespace Internal
    {
        class State
        {
        public:
            State()
                : m_pixelSize(-1)
            {
                SetFontName("Prida");
            }
            ~State()
            {
                assert(m_window == nullptr); // forgot to call cleanup?
                ClearShapes();
            }
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
            void DrawMouse();
            void DrawShapes();
            void DrawMenu();
            void DrawFPS();
            void Exit();

            void ClearShapes()
            {
                m_shapes.clear();
            }
            void AddShape(const Geometry::ShapePtr& shape)
            {
                m_shapes.emplace_back(std::move(shape));
            }
            void SetRenderMode(const Geometry::RenderMode renderMode)
            {
                for (auto& shape : m_shapes)
                {
                    shape->SetRenderMode(renderMode);
                }
            }
            void SetFontName(const std::string& fontName)
            {
                m_font = std::make_shared<Font>(std::move(std::string(fontName)), Settings::GetInt("FontSize",40));
                m_font->PixelSize(m_pixelSize);
                m_menu.SetFont(m_font);
            }
            Quat State::CalculateRotation();

            Menu& GetMenu() { return m_menu; }

        private:
            // state protection
            std::mutex m_mutex;

            // window
            GLFWwindow* m_window;
            GLsizei m_width;
            GLsizei m_height;
            GLfloat m_ratio;
            double m_x;
            double m_y;
            double m_pixelSize;

            // mouse related
            Quat m_rotation;
            double m_zoom;
            double m_zoomMin;
            double m_zoomMax;
            bool m_mouseDragging;
            Vector2d m_mousePos;
            Vector2d m_mouseDownPos;

            // drawable objects
            std::vector<Geometry::ShapePtr> m_shapes;
            std::shared_ptr<Font> m_font;

            // menu
            Menu m_menu;
        };
    } // namespace Internal

    namespace
    {
        std::shared_ptr<Internal::State> GetState()
        {
            static std::weak_ptr<Internal::State> weak_state;
            if (auto shared_state = weak_state.lock())
            {
                return shared_state;
            }
            auto shared_state = make_shared<Internal::State>();
            weak_state = shared_state;
            return shared_state;
        }
        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            GetState()->KeyCallback(key, scancode, action, mods);
        }

        static void char_callback(GLFWwindow *window, unsigned int c)
        {
            GetState()->CharCallback(c);
        }

        static void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos)
        {
            GetState()->CursorPosCallback(xpos, ypos);
        }

        static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
        {
            GetState()->MouseButtonCallback(button, action, mods);
        }

        static void error_callback(int error, const char* description)
        {
            //    Log::Error("{0}", description);
        }

        static void window_size_callback(GLFWwindow* window, int width, int height)
        {
            GetState()->WindowSizeCallback(width, height);
        }

        static void scroll_callback(GLFWwindow* window, double x, double y)
        {
            GetState()->ScrollCallback(x, y);
        }

        static void  window_refresh_callback(GLFWwindow* window)
        {
            //Log::Error("window_refresh_callback");
        }
    }

    namespace Internal
    {
        bool State::Init()
        {
            m_zoom = 1.0;
            m_zoomMin = 0.5;
            m_zoomMax = 16;
            m_rotation.Set(Vector3d(1,0,0),0);
            m_mouseDragging = false;

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
            ClearShapes();
            DisposeAllRenderObjects();
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
            //    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

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
                glActiveTexture(GL_TEXTURE0);
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
            glOrtho(-m_ratio, m_ratio, -1.0, 1.0, -1.1, 1.1);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            Shaders::Select(Shaders::Type::None);
            DrawShapes();

            glDisable(GL_DEPTH_TEST);
            glDisable(GL_LIGHTING);
            glDepthMask(GL_FALSE);

            Shaders::Select(Shaders::Type::None);
            DrawMenu();

            Shaders::Select(Shaders::Type::None);
            DrawFPS();

            Shaders::Select(Shaders::Type::None);
            DrawMouse();

            glEnable(GL_DEPTH_TEST);
            glEnable(GL_LIGHTING);
            glDepthMask(GL_TRUE);

            lock.unlock();

            glfwSwapBuffers(m_window);

            HandleDisposedRenderObjects();
        }

        void State::DrawShapes()
        {
            RenderInfo renderInfo;

            auto EdgeDummy = [](const EdgeRaw& edge) {};
            auto EdgeNormal = [](const EdgeRaw& edge)
            {
                glNormal(edge->GetStartNormal());
            };
            auto EdgeTextureCoord = [](const EdgeRaw& edge)
            {
                glTextureCoord(edge->GetStartTextureCoord());
            };

            auto DrawFace2 = [&](const FaceRaw& face, 
                                 std::function<void(const EdgeRaw& edge)> edgeNormal, 
                                 std::function<void(const EdgeRaw& edge)> edgeTextureCoord)
            {
                face->ForEachEdge([&](const EdgeRaw& edge)
                {
                    renderInfo.Push(edge);
                    edgeNormal(edge);
                    edgeTextureCoord(edge);
                    glVertex(*edge->GetStartVertex());
                    renderInfo.Pop();
                });
            };
            auto DrawFace1 = [&](const FaceRaw& face, std::function<void(const EdgeRaw& edge)> edgeNormal)
            {
                if (face->GetStartEdge()->GetStartTextureCoord())
                {
                    DrawFace2(face, edgeNormal, EdgeTextureCoord);
                }
                else
                {
                    DrawFace2(face, edgeNormal, EdgeDummy);
                }
            };
            auto DrawFace0 = [&](const FaceRaw& face)
            {
                if (face->GetStartEdge()->GetStartNormal())
                {
                    DrawFace1(face, EdgeNormal);
                }
                else if (face->GetNormal())
                {
                    glNormal(*face->GetNormal());
                    DrawFace1(face, EdgeDummy);
                }
                else
                {
                    DrawFace1(face, EdgeDummy);
                }
            };
            auto DrawFaces = [&](const HullRaw& hull)
            {
                const auto& faces = hull->GetFaces();
                for (auto face : faces)
                {
                    switch (face->GetEdgeCount())
                    {
                    case 0:
                    case 1:
                    case 2:
                        assert(false);
                    case -1:
                    default:
                        glBegin(GL_POLYGON);
                        break;
                    case 3:
                        glBegin(GL_TRIANGLES);
                        break;
                    }
                    renderInfo.Push(face);
                    DrawFace0(face);
                    renderInfo.Pop();
                    glEnd();
                }
            };
            // todo: do not draw patches which are outside the view.
            //Projection<double> projection;
            //Vertex front = projection.Project(Vector2d(projection.GetWidth()/2, projection.GetHeight()/2), -100);
            //front.Normalize();
            auto DrawHull = [&](const HullRaw& hull)
            {
                HullRenderObject& renderObject = GetRenderObject(hull);
                unsigned int updateNeeded = renderObject.NeedsUpdate();
                unsigned int displayList = renderObject.GetDisplayList();
                auto UpdateDisplayList = [&](std::unique_lock<std::mutex>&& lock)
                {
                    if (lock)
                    {
                        displayList = glGenLists(1);
                        glNewList(displayList, GL_COMPILE);
                        renderInfo.Push(hull);
                        //glBindTexture(GL_TEXTURE_2D, hull->GetTextureId());
                        DrawFaces(hull);
                        renderInfo.Pop();
                        glEndList();
                        renderObject.SetDisplayList(displayList);
                    }
                    else
                    {
                        renderObject.Invalidate();
                    }
                };
                if (updateNeeded > 5 || displayList == 0)
                {
                    UpdateDisplayList(hull->GetLock());
                }
                else if (updateNeeded > 0)
                {
                    UpdateDisplayList(hull->TryGetLock());
                }
                glCallList(displayList);
            };

            // todo: adjust far clipping plane?
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(-m_ratio / m_zoom, m_ratio / m_zoom, -1. / m_zoom, 1. / m_zoom, -1.2, 1.2);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            glMultMatrix(CalculateRotation());

            for (const Geometry::ShapeRaw& shape : m_shapes)
            {
                renderInfo.Push(shape);
                shape->ForEachHull(DrawHull);
                renderInfo.Pop();
            }

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(-m_ratio, m_ratio, -1., 1., 1.1, -1.1);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

        }

        void State::DrawMouse()
        {
            // TODO
        }

        void State::DrawMenu()
        {
            assert(m_font);
            m_menu.Draw(m_width,m_height,m_x,m_y);
        }

        void State::DrawFPS()
        {
            if (Settings::GetBool("ShowFPS"))
            {
                std::string fps = "FPS 123";
                auto size = m_font->GetSize(fps);
                m_font->Color(Geometry::Color::Red()).Draw(m_x - size[0] - 2 * m_pixelSize, 2 * m_pixelSize - m_y, fps);
            }
        }

        Quat State::CalculateRotation()
        {
            if (m_mouseDragging)
            {
                Vector2d dir = (m_mousePos - m_mouseDownPos) / m_zoom;
                Vector3d axis(dir[1], -dir[0], 0);
                double angle = axis.Length();
                if (angle > 0)
                {
                    axis = (axis / angle);
                    axis = m_rotation.Transform(axis);
                    return Quat(axis, angle)*m_rotation;
                }
                return m_rotation;
            }
            return m_rotation;
        }

        void State::KeyCallback(const int key, const int scancode, const int action, const int mods)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if(!m_menu.HandleKey(key,scancode,action,mods))
            { 
            }
        }

        void State::CharCallback(const unsigned int c)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (!m_menu.HandleChar(c))
            {
            }
        }

        void State::CursorPosCallback(const double& xpos, const double& ypos)
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            double x = (2 * xpos / m_width - 1) * m_x;
            double y = (2 * ypos / m_height - 1) * m_y;
            m_mousePos.Set(x, -y);

            if (!m_menu.HandleCursorPos(xpos,ypos))
            {
            }

        }

        void State::MouseButtonCallback(const int button, const int action, const int mods)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (!m_menu.HandleMouseButton(button, action, mods))
            {
                if (GLFW_MOUSE_BUTTON_LEFT == button && 0 == mods)
                {
                    if (GLFW_PRESS == action)
                    {
                        m_rotation = CalculateRotation();
                        m_rotation.Normalize();
                        m_mouseDragging = true;
                        m_mouseDownPos = m_mousePos;
                    }
                    else if (GLFW_RELEASE == action)
                    {
                        m_rotation = CalculateRotation();
                        m_rotation.Normalize();
                        m_mouseDragging = false;
                    }
                }
            }
        }

        void State::WindowSizeCallback(const int width, const int height)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_width = width;
            m_height = height;
            m_ratio = m_width / (float)m_height;
            m_x = 1;
            m_y = 1;
            if (m_ratio >= 1)
            {
                m_x = m_ratio;
            }
            else
            {
                m_y = 1 / m_ratio;
            }
            m_pixelSize = 2.0/m_height;
            assert(m_font);
            m_font->PixelSize(m_pixelSize);
            if (!m_menu.HandleWindowSize(width, height))
            {
            }
        }

        void State::ScrollCallback(const double x, const double y)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            double factor = 1 + y / 10;
            m_zoom = Numerics::Clamp(m_zoom*factor, m_zoomMin, m_zoomMax);
            if (!m_menu.HandleScroll(x,y))
            {
            }
        }

        void State::Exit()
        {
            glfwSetWindowShouldClose(m_window, GLFW_TRUE);
        }
    } // namespace Internal


    UserInterface::UserInterface()
        : m_state(GetState())
    {
    }
    UserInterface::~UserInterface()
    {
    }

    bool UserInterface::Init()
    {
        return m_state->Init();
    }

    void UserInterface::Run()
    {
        return m_state->Run();
    }

    bool UserInterface::Cleanup()
    {
        return m_state->Cleanup();
    }

    void UserInterface::Draw()
    {
        return m_state->Draw();
    }

    void UserInterface::AddShape(const Geometry::ShapePtr& shape)
    {
        return m_state->AddShape(shape);
    }

    void UserInterface::ClearShapes()
    {
        return m_state->ClearShapes();
    }

    void UserInterface::SetRenderMode(const Geometry::RenderMode renderMode)
    {
        return m_state->SetRenderMode(renderMode);
    }

    void UserInterface::SetFontName(const std::string& fontName)
    {
        return m_state->SetFontName(fontName);
    }

    void UserInterface::Exit()
    {
        return m_state->Exit();
    }

    Menu& UserInterface::GetMenu()
    {
        return m_state->GetMenu();
    }


} // namespace Viewer







