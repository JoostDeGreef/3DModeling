#include <string>
#include <unordered_map>
#include <algorithm>
using namespace std;

#include "Geometry.h"
#include "GLWrappers.h"
#include "Shaders.h"
using namespace Viewer;

namespace
{
    static const char * const textVertexShader = R"VertexShader(
#version 440

in vec4 pos_uv;
out vec2 uv;

void main()
{
    gl_Position = vec4(pos_uv.xy, 0, 1);
    uv = pos_uv.zw;
}
)VertexShader";

    static const char * const textFragmentShader = R"FragmentShader(
#version 440

in vec2 uv;
uniform sampler2D texture;
uniform vec4 inputColor;

out vec4 color;

void main()
{
    color = vec4(inputColor.rgb, uv.y); /*texture2D(texture, uv).a);*/
}
)FragmentShader";

    GLuint LoadShaders(const char * const vertex, const char * const fragment) {

        // Create the shaders
        GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

        auto CheckCompilation = [](GLuint shaderID)
        {
            GLint result = GL_FALSE;
            int infoLogLength;
            glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
            glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
            if (infoLogLength > 0) {
                std::vector<char> errorMessage(infoLogLength + 1);
                glGetShaderInfoLog(shaderID, infoLogLength, NULL, &errorMessage[0]);
                return std::string(errorMessage.begin(),errorMessage.end());
            }
            return std::string();
        };

        // Compile Vertex Shader
        glShaderSource(vertexShaderID, 1, &vertex, NULL);
        glCompileShader(vertexShaderID);

        // Check Vertex Shader
        auto vertexError = CheckCompilation(vertexShaderID);
        if (!vertexError.empty())
        {
            // todo: log if error not empty
            return 0;
        }

        // Compile Fragment Shader
        glShaderSource(fragmentShaderID, 1, &fragment, NULL);
        glCompileShader(fragmentShaderID);

        // Check Fragment Shader
        auto fragmentError = CheckCompilation(fragmentShaderID);
        if (!fragmentError.empty())
        {
            // todo: log if error not empty
            glDeleteShader(vertexShaderID);
            return 0;
        }

        // Link the program
        GLuint programID = glCreateProgram();
        glAttachShader(programID, vertexShaderID);
        glAttachShader(programID, fragmentShaderID);
        glLinkProgram(programID);

        glDetachShader(programID, vertexShaderID);
        glDetachShader(programID, fragmentShaderID);

        glDeleteShader(vertexShaderID);
        glDeleteShader(fragmentShaderID);

        // Check the program
        GLint result = GL_FALSE;
        int infoLogLength;
        glGetProgramiv(programID, GL_LINK_STATUS, &result);
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0) 
        {
            std::vector<char> errorMessage(infoLogLength + 1);
            glGetProgramInfoLog(programID, infoLogLength, NULL, &errorMessage[0]);
            // todo: log error
            glDeleteProgram(programID);
            programID = 0;
        }

        return programID;
    }

    static Shaders shaders;
}

Shaders::Shaders()
    : m_selected(Type::None)
{
}

Shaders::Type Shaders::Select(Type shader)
{
    Type prev = shaders.m_selected;

    if (shaders.m_selected != shader)
    {
        auto AddAndSelectShader = [&](Type shader, const char * const vertex, const char * const fragment)
        {
            GLuint programId = LoadShaders(vertex, fragment);
            if (programId == 0)
            {
                shader = Type::None;
            }
            shaders.m_shaders.emplace(shader, programId);
            shaders.m_selected = shader;
        };

        auto iter = shaders.m_shaders.find(shader);
        if (iter == shaders.m_shaders.end())
        {
            switch (shader)
            {
            default:
                // log this?
            case Type::None:
                shaders.m_shaders.emplace(Type::None, 0);
                shaders.m_selected = Type::None;
                break;
            case Type::Text:
                AddAndSelectShader(Type::Text, textVertexShader, textFragmentShader);
                break;
            }
            iter = shaders.m_shaders.find(shaders.m_selected);
        }
        else
        {
            shaders.m_selected = shader;
        }
        glUseProgram(iter->second);
    }

    return prev;
}

int Shaders::GetUniformLocation(const char * const name)
{
    // store this value?
    auto iter = shaders.m_shaders.find(shaders.m_selected);
    assert(iter != shaders.m_shaders.end());
    return glGetUniformLocation(iter->second,name);
}

