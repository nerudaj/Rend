#pragma once

#include "Commands/CommandQueue.hpp"
#include "Interfaces/CurrentLayerObserverInterface.hpp"
#include "Interfaces/PenUserInterface.hpp"
#include "Interfaces/ShortcutEngineInterface.hpp"
#include "Interfaces/ToolPropertyInterface.hpp"
#include "Utilities/Rect.hpp"
#include <DGM/dgm.hpp>
#include <LevelD.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <expected>
#include <nlohmann/json.hpp>
#include <optional>

import Error;

using ExpectedPropertyPtr =
    std::expected<mem::Box<ToolPropertyInterface>, BaseError>;

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
        mem::Rc<ShortcutEngineInterface> shortcutEngine,
        mem::Rc<LayerObserverInterface> layerObserver) noexcept
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

    virtual void
    resize(unsigned width, unsigned height, bool isTranslationDisabled) = 0;

    virtual void shrinkTo(const TileRect& boundingBox) = 0;

    virtual void saveTo(LevelD& lvd) const = 0;

    virtual void restoreFrom(const LevelD& snapshot) = 0;

    virtual void drawTo(
        tgui::CanvasSFML::Ptr& canvas,
        std::size_t layerIdx,
        uint8_t opacity) = 0;

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

    [[nodiscard]] inline std::size_t getCurrentLayerIdx() const noexcept
    {
        return layerObserver->getCurrentLayerIdx();
    }

    [[nodiscard]] inline std::size_t getLayerCount() const noexcept
    {
        return layerObserver->getLayerCount();
    }

protected: // Protected virtual interface
    virtual void buildCtxMenuInternal(tgui::MenuBar::Ptr& menu) = 0;

private:
    std::function<void(void)> onStateChangedCallback;
    mem::Rc<ShortcutEngineInterface> shortcutEngine;
    mem::Rc<LayerObserverInterface> layerObserver;
    std::vector<unsigned> ctxMenuSignalHandlers;
};

template<class T>
concept DerivedFromTool = std::derived_from<T, ToolInterface>;

namespace Helper
{
    sf::Vector2i minVector(const sf::Vector2i& a, const sf::Vector2i& b);

    sf::Vector2i maxVector(const sf::Vector2i& a, const sf::Vector2i& b);
} // namespace Helper
