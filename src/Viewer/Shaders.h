#pragma once

namespace Viewer
{
    class Shaders
    {
    public:
        enum class Type
        {
            None,
            Text,
        };

        Shaders();

        static Type Select(Type shader);
        static int GetUniformLocation(const char * const name);
    private:
        Type m_selected;
        std::map<Type, unsigned int> m_shaders;
    };
}; // namespace Viewer

