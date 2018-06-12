#pragma once

class FontMenuItem : public StaticMenuItem
{
    class FontMenuSubItem : public StaticMenuItem
    {
    public:
        FontMenuSubItem(FontMenuItem& parent, const std::string& fontName)
            : StaticMenuItem(parent.GetMenu(), std::string(fontName))
            , m_parent(parent)
            , m_fontName(fontName)
        {}
        void Execute() override
        {
            m_parent.SetFontName(m_fontName);
        }
        void Draw(const double& x, const double& y, const double& pixel, const bool mouseOver) override
        {
            auto color = mouseOver ? Geometry::Color::Red()
                : (m_parent.GetFontName() == m_fontName ? Geometry::Color::Green()
                    : Geometry::Color::White());
            m_parent.GetMenu().GetFont().PixelSize(pixel).Color(color).Draw(x, y, GetText());
        }

    private:
        FontMenuItem & m_parent;
        std::string m_fontName;
    };
    class FontSizeMenuSubItem : public StaticMenuItem
    {
    public:
        FontSizeMenuSubItem(FontMenuItem& parent, std::string&& text, const int fontSize)
            : StaticMenuItem(parent.GetMenu(), std::move(text))
            , m_parent(parent)
            , m_fontSize(fontSize)
        {}
        void Execute() override
        {
            m_parent.SetFontSize(m_fontSize);
        }
        void Draw(const double& x, const double& y, const double& pixel, const bool mouseOver) override
        {
            auto color = mouseOver ? Geometry::Color::Red()
                : (m_parent.GetFontSize() == m_fontSize ? Geometry::Color::Green()
                    : Geometry::Color::White());
            m_parent.GetMenu().GetFont().PixelSize(pixel).Color(color).Draw(x, y, GetText());
        }

    private:
        FontMenuItem & m_parent;
        int m_fontSize;
    };
public:
    FontMenuItem(Menu& menu, UserInterface& ui)
        : StaticMenuItem(menu, "UI Font")
        , m_ui(ui)
    {
        using namespace std::placeholders;

        Add(make_shared<FontSizeMenuSubItem>(*this, "Small", 30));
        Add(make_shared<FontSizeMenuSubItem>(*this, "Normal", 40));
        Add(make_shared<FontSizeMenuSubItem>(*this, "Large", 50));

        Add(make_shared<DelimiterMenuItem>(menu));

        for (auto& fontName : Data::GetFontNames())
        {
            Add(make_shared<FontMenuSubItem>(*this, fontName));
        }

        m_fontSize = Settings::GetInt("FontSize", 40);
        m_fontName = Settings::GetString("FontName", m_items.back()->GetText());
    }

    void SetFontSize(int fontSize)
    {
        m_fontSize = fontSize;
        Settings::SetInt("FontSize", m_fontSize);
        m_ui.SetFontName(GetFontName());
    }
    const int GetFontSize() const
    {
        return m_fontSize;
    }
    const std::string& GetFontName() const
    {
        return m_fontName;
    }
    void SetFontName(const std::string& fontName)
    {
        m_fontName = fontName;
        Settings::SetString("FontName", m_fontName);
        m_ui.SetFontName(GetFontName());
    }
    Menu& GetMenu() { return m_menu; }
protected:
    std::string m_fontName;
    int m_fontSize;
    UserInterface& m_ui;
};

