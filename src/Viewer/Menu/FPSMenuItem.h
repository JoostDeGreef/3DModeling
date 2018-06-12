#pragma once

class FPSMenuItem : public MenuItem
{
public:
    FPSMenuItem(Menu& menu)
        : MenuItem(menu)
    {
        m_showFPS = Settings::GetBool("ShowFPS");
    }

    virtual const std::string GetText() const override
    {
        return m_showFPS ? "FPS On" : "FPS Off";
    }

    virtual void Execute() override
    {
        m_showFPS = !m_showFPS;
        Settings::SetBool("ShowFPS", m_showFPS);
    }
protected:
    bool m_showFPS;
};

