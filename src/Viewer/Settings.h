#pragma once

namespace Viewer
{

    class Settings
    {
    public:
        static void AttachDB(const std::string& filename);

        static std::string GetString(const std::string& key, const std::string& def = "");
        static int GetInt(const std::string& key, const int def = 0);
        static double GetDouble(const std::string& key, const double def = 0.0);
        static bool GetBool(const std::string& key, const bool def = false);

        static bool TryGetString(const std::string& key, std::string& value);
        static bool TryGetInt(const std::string& key, int& value);
        static bool TryGetDouble(const std::string& key, double& value);
        static bool TryGetBool(const std::string& key, bool& value);

        static void SetString(const std::string& key, const std::string& value);
        static void SetInt(const std::string& key, const int value);
        static void SetDouble(const std::string& key, const double value);
        static void SetBool(const std::string& key, const bool value);

        static Geometry::RenderMode GetRenderMode() { return (Geometry::RenderMode)Geometry::Numerics::Clamp(GetInt("RenderMode", 0), 0, 2); }
        static void SetRenderMode(const Geometry::RenderMode renderMode) { Settings::SetInt("RenderMode", (int)renderMode); }

        static Geometry::ViewingMode GetViewingMode() { return (Geometry::ViewingMode)Geometry::Numerics::Clamp(GetInt("ViewingMode", 0), 0, 1); }
        static void SetViewingMode(const Geometry::ViewingMode viewingMode) { Settings::SetInt("ViewingMode", (int)viewingMode); }
    };
}; // Viewer



