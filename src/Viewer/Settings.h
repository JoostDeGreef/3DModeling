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

    };
}; // Viewer


