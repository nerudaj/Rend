#pragma once

#include "Gui.hpp"
#include "Interfaces/PenUserInterface.hpp"
#include "Interfaces/ToolInterface.hpp"
#include "LevelMesh/DrawableLeveldMesh.hpp"
#include "ToolProperties/MeshToolProperty.hpp"
#include "Tools/SidebarUserMesh.hpp"

class [[nodiscard]] ToolMesh final : public ToolInterface
{
public:
    enum class DrawMode
    {
        Pencil,
        RectFill,
        RectEdge,
        Line
    };

public:
    ToolMesh(
        std::function<void(void)> onStateChanged,
        mem::Rc<ShortcutEngineInterface> shortcutEngine,
        mem::Rc<LayerObserverInterface> layerObserver,
        mem::Rc<Gui> gui,
        mem::Rc<CommandQueue> commandQueue,
        const std::filesystem::path& texturePath,
        const dgm::Clip& textureClip,
        std::vector<bool> defaultBlocks,
        const LevelD& level)
        : ToolInterface(onStateChanged, shortcutEngine, layerObserver)
        , sidebarUser(gui)
        , commandQueue(commandQueue)
    {
        configure(texturePath, textureClip, defaultBlocks);
        loadFrom(level);
    }

public: // PenUserInterface
    void penClicked(const sf::Vector2i& position) override;
    void penDragStarted(const sf::Vector2i& start) override;
    void
    penDragUpdate(const sf::Vector2i& start, const sf::Vector2i& end) override;
    void
    penDragEnded(const sf::Vector2i& start, const sf::Vector2i& end) override;

    void penDragCancel(const sf::Vector2i&) override
    {
        dragging = false;
    }

    void penDelete() override {}

public: // ToolInterface
    void buildSidebar() override
    {
        sidebarUser.buildSidebar();
    }

    void resize(
        unsigned width, unsigned height, bool isTranslationDisabled) override;

    void shrinkTo(const TileRect& boundingBox) override;

    void saveTo(LevelD& lvd) const override;

    void restoreFrom(const LevelD& snapshot) override
    {
        loadFrom(snapshot);
    }

    void drawTo(
        tgui::CanvasSFML::Ptr& canvas,
        std::size_t layerIdx,
        uint8_t opacity) override;

    ExpectedPropertyPtr getProperty(const sf::Vector2i& penPos) const override;

    void setProperty(const ToolPropertyInterface& prop) override;

    void buildCtxMenuInternal(tgui::MenuBar::Ptr& menu) override;

    // No highlight here
    virtual std::optional<GenericObject>
    getHighlightedObject(const sf::Vector2i&) const override
    {
        return {};
    }

    virtual std::vector<sf::Vector2u>
    getPositionsOfObjectsWithTag(unsigned) const override
    {
        return {};
    }

    [[nodiscard]] std::optional<TileRect>
    getBoundingBox() const noexcept override;

public:
    void configure(
        const std::filesystem::path& texturePath,
        const dgm::Clip& textureClip,
        const std::vector<bool>& defaultBlockSetting);

    /**
     *  Copy data from existing source map to target
     *  arrays.
     *
     *  translation vector defines how source data are
     *  mapped to target. Source point S will be mapped
     *  to target point T according to:
     *  S + translation = T
     *
     *  start bound is inclusive
     *  end bound is exclusive
     */
    void copySourceRectToTarget(
        DrawableLeveldMesh& sourceMap,
        const sf::Vector2u& start,
        const sf::Vector2u& end,
        const sf::Vector2i& translation,
        std::vector<int>& targetTileValues,
        std::vector<int>& targetSolidValues,
        unsigned targetWidth);

    void changeDrawingMode(DrawMode newMode);

    void sanitizeBeforeSave();

private:
    void loadFrom(const LevelD& lvd);

    void toggleOverlay();

    [[nodiscard]] constexpr auto&& getMap(this auto&& self) noexcept
    {
        return self.maps[self.getCurrentLayerIdx()];
    }

    [[nodiscard]] sf::Vector2u
    worldToTilePos(const sf::Vector2i& position) const noexcept
    {
        unsigned tileX = position.x / getMap().getTileSize().x;
        unsigned tileY = position.y / getMap().getTileSize().y;
        return { tileX, tileY };
    }

    [[nodiscard]] bool isPositionValid(const sf::Vector2u& tilePos) const
    {
        return !(
            tilePos.x < 0 || tilePos.y < 0
            || tilePos.x >= getMap().getMapDimensions().x
            || tilePos.y >= getMap().getMapDimensions().y);
    }

private:
    DrawMode mode = DrawMode::Pencil;
    sf::RectangleShape rectShape;
    std::vector<DrawableLeveldMesh> maps;
    SidebarUserMesh sidebarUser;
    mem::Rc<CommandQueue> commandQueue;

    std::vector<bool> defaultBlocks;

    bool drawing = false;
    bool enableOverlay = false;
    bool dragging = false;
};

namespace std
{
    std::string to_string(ToolMesh::DrawMode mode);
}