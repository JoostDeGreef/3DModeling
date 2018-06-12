#pragma once

class RenderModeMenuItem : public StaticMenuItem
{
    class RenderModeMenuSubItem : public StaticMenuItem
    {
    public:
        RenderModeMenuSubItem(RenderModeMenuItem& parent, std::string&& text, RenderMode renderMode)
            : StaticMenuItem(parent.GetMenu(), std::move(text))
            , m_parent(parent)
            , m_renderMode(renderMode)
        {}
        void Execute() override
        {
            m_parent.SetRenderMode(m_renderMode);
        }
        void Draw(const double& x, const double& y, const double& pixel, const bool mouseOver) override
        {
            auto color = mouseOver ? Geometry::Color::Red()
                : (m_parent.GetRenderMode() == m_renderMode ? Geometry::Color::Green()
                    : Geometry::Color::White());
            m_parent.GetMenu().GetFont().PixelSize(pixel).Color(color).Draw(x, y, GetText());
        }

    private:
        RenderModeMenuItem & m_parent;
        RenderMode m_renderMode;
    };

    class ViewingModeMenuSubItem : public StaticMenuItem
    {
    public:
        ViewingModeMenuSubItem(RenderModeMenuItem& parent, std::string&& text, ViewingMode viewingMode)
            : StaticMenuItem(parent.GetMenu(), std::move(text))
            , m_parent(parent)
            , m_viewingMode(viewingMode)
        {}
        void Execute() override
        {
            m_parent.SetViewingMode(m_viewingMode);
        }
        void Draw(const double& x, const double& y, const double& pixel, const bool mouseOver) override
        {
            auto color = mouseOver ? Geometry::Color::Red()
                : (m_parent.GetViewingMode() == m_viewingMode ? Geometry::Color::Green()
                    : Geometry::Color::White());
            m_parent.GetMenu().GetFont().PixelSize(pixel).Color(color).Draw(x, y, GetText());
        }

    private:
        RenderModeMenuItem & m_parent;
        ViewingMode m_viewingMode;
    };


public:
    RenderModeMenuItem(Menu& menu, UserInterface& ui)
        : StaticMenuItem(menu, "RenderMode")
        , m_ui(ui)
    {
        using namespace std::placeholders;

        m_renderMode = Settings::GetRenderMode();
        m_viewingMode = Settings::GetViewingMode();

        Add(make_shared<RenderModeMenuSubItem>(*this, "Solid", RenderMode::Solid));
        Add(make_shared<RenderModeMenuSubItem>(*this, "WireFrame", RenderMode::WireFrame));
        //Add(make_shared<RenderModeMenuSubItem>(*this, "Transparent", RenderMode::Transparent));

        Add(make_shared<DelimiterMenuItem>(GetMenu()));

        Add(make_shared<ViewingModeMenuSubItem>(*this, "Orthogonal", ViewingMode::Orthogonal));
        Add(make_shared<ViewingModeMenuSubItem>(*this, "Perspective", ViewingMode::Perspective));
    }

    RenderMode GetRenderMode() const
    {
        return m_renderMode;
    }
    void SetRenderMode(RenderMode renderMode)
    {
        m_renderMode = renderMode;
        m_ui.SetRenderMode(GetRenderMode());
        Settings::SetRenderMode(m_renderMode);
    }

    ViewingMode GetViewingMode() const
    {
        return m_viewingMode;
    }
    void SetViewingMode(ViewingMode viewingMode)
    {
        m_viewingMode = viewingMode;
        m_ui.SetViewingMode(GetViewingMode());
        Settings::SetViewingMode(m_viewingMode);
    }

    Menu& GetMenu() { return m_menu; }
protected:
    RenderMode m_renderMode;
    ViewingMode m_viewingMode;
    UserInterface& m_ui;
};

