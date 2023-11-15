#pragma once

#include "include/Commands/CommandQueue.hpp"
#include "include/Interfaces/CurrentLayerObserverInterface.hpp"
#include "include/Interfaces/PenUserInterface.hpp"
#include "include/Interfaces/ShortcutEngineInterface.hpp"
#include "include/Interfaces/ToolPropertyInterface.hpp"
#include "include/Utilities/Box.hpp"
#include "include/Utilities/Error.hpp"
#include "include/Utilities/GC.hpp"
#include "include/Utilities/Rect.hpp"
#include <DGM/dgm.hpp>
#include <LevelD.hpp>
#include <TGUI/TGUI.hpp>
#include <expected>
#include <json.hpp>
#include <optional>

using ExpectedPropertyPtr =
    std::expected<Box<ToolPropertyInterface>, BaseError>;

struct GenericObject
{
    sf::Vector2u position;
    unsigned tag;
};

class ToolInterface : public PenUserInterface
{
public:
    [[nodiscard]] ToolInterface(
        std::function<void(void)> onStateChanged,
        GC<ShortcutEngineInterface> shortcutEngine,
        GC<LayerObserverInterface> layerObserver) noexcept
        : onStateChangedCallback(onStateChanged)
        , shortcutEngine(shortcutEngine)
        , layerObserver(layerObserver)
    {
    }

    virtual ~ToolInterface() = default;

public: // Non virtual interface
    void buildCtxMenu(tgui::MenuBar::Ptr& menu);
    void destroyCtxMenu(tgui::MenuBar::Ptr& menu);

public: // Public virtual interface
    virtual void buildSidebar() = 0;

    virtual void configure(nlohmann::json& config) = 0;

    virtual void
    resize(unsigned width, unsigned height, bool isTranslationDisabled) = 0;

    virtual void shrinkTo(const TileRect& boundingBox) = 0;

    virtual void saveTo(LevelD& lvd) const = 0;
    virtual void loadFrom(const LevelD& lvd) = 0;

    virtual void drawTo(tgui::Canvas::Ptr& canvas, uint8_t opacity) = 0;

    [[nodiscard]] virtual ExpectedPropertyPtr
    getProperty(const sf::Vector2i& penPos) const = 0;
    virtual void setProperty(const ToolPropertyInterface& prop) = 0;

    [[nodiscard]] virtual std::optional<GenericObject>
    getHighlightedObject(const sf::Vector2i& penPos) const = 0;

    [[nodiscard]] virtual std::vector<sf::Vector2u>
    getPositionsOfObjectsWithTag(unsigned tag) const = 0;

    /**
     * \Return a bounding box around the contents of the tool
     */
    [[nodiscard]] virtual std::optional<TileRect>
    getBoundingBox() const noexcept = 0;

protected: // Protected non-virtual interface
    void addCtxMenuItem(
        tgui::MenuBar::Ptr& menu,
        const std::string& label,
        std::function<void(void)> callback,
        sf::Keyboard::Key shortcutKey);

    void signalStateChanged()
    {
        onStateChangedCallback();
    }

    [[nodiscard]] inline std::size_t getCurrentLayerId() const noexcept
    {
        return layerObserver->getCurrentLayerId();
    }

    [[nodiscard]] inline std::size_t getLayerCount() const noexcept
    {
        return layerObserver->getLayerCount();
    }

protected: // Protected virtual interface
    virtual void buildCtxMenuInternal(tgui::MenuBar::Ptr& menu) = 0;

private:
    std::function<void(void)> onStateChangedCallback;
    GC<ShortcutEngineInterface> shortcutEngine;
    GC<LayerObserverInterface> layerObserver;
    std::vector<unsigned> ctxMenuSignalHandlers;
};

template<class T>
concept DerivedFromTool = std::derived_from<T, ToolInterface>;

namespace Helper
{
    sf::Vector2i minVector(const sf::Vector2i& a, const sf::Vector2i& b);

    sf::Vector2i maxVector(const sf::Vector2i& a, const sf::Vector2i& b);
} // namespace Helper
