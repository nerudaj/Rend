#pragma once

#include "Interfaces/ToolInterface.hpp"
#include "ToolProperties/ItemToolProperty.hpp"
#include "Tools/SidebarUserItem.hpp"
#include "Tools/ToolWithDragAndSelect.hpp"
#include "Utilities/CoordConverter.hpp"

class ToolItem final : public ToolWithDragAndSelect
{
public:
    [[nodiscard]] ToolItem(
        std::function<void(void)> onStateChanged,
        mem::Rc<ShortcutEngineInterface> shortcutEngine,
        mem::Rc<LayerObserverInterface> layerObserver,
        mem::Rc<Gui> gui,
        const mem::Rc<CommandQueue> commandQueue,
        const std::filesystem::path& spritesheetPath,
        const dgm::Clip& spritesheetClip,
        const LevelD& level) noexcept
        : ToolWithDragAndSelect(onStateChanged, shortcutEngine, layerObserver)
        , sidebarUser(gui)
        , commandQueue(commandQueue)
    {
        configure(
            sf::Vector2u(level.mesh.tileWidth, level.mesh.tileHeight),
            spritesheetPath,
            spritesheetClip);
        loadFrom(level);
    }

public: // PenUserInterface
    void penClicked(const sf::Vector2i& position) override;

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

    void validateBeforeSave() const;

    void drawTo(
        tgui::CanvasSFML::Ptr& canvas,
        std::size_t layerIdx,
        uint8_t opacity) override;

    ExpectedPropertyPtr getProperty(const sf::Vector2i& penPos) const override;

    void setProperty(const ToolPropertyInterface& prop) override;

    std::optional<GenericObject>
    getHighlightedObject(const sf::Vector2i& penPos) const override;

    std::vector<sf::Vector2u>
    getPositionsOfObjectsWithTag(unsigned tag) const override;

    [[nodiscard]] std::optional<TileRect>
    getBoundingBox() const noexcept override;

protected: // ToolInterface
    void buildCtxMenuInternal(tgui::MenuBar::Ptr& menu) override;

protected: // ToolWithDragAndSelect
    virtual std::optional<std::size_t>
    getObjectIndexFromMousePos(const sf::Vector2i& pos) const final override;
    virtual sf::Vector2i
    getPositionOfObjectWithIndex(std::size_t index) const final override;
    virtual void
    selectObjectsInArea(const sf::IntRect& selectedArea) final override;
    virtual void
    moveSelectedObjectsTo(const sf::Vector2i& point) final override;
    virtual void createMoveCommand(
        const sf::Vector2i& src, const sf::Vector2i& dest) final override;
    virtual void createDeleteCommand() final override;

protected:
    bool isValidPenPosForDrawing(const sf::Vector2i& pos) const
    {
        return !(
            pos.x < 0 || pos.y < 0 || pos.x >= levelSize.x
            || pos.y >= levelSize.y);
    }

    void loadFrom(const LevelD& lvd);

    void configure(
        const sf::Vector2u& tileDimensions,
        const std::filesystem::path& texturePath,
        const dgm::Clip& clip);

private:
    std::vector<LevelD::Thing> items;
    CoordConverter coordConverter;
    sf::Vector2i levelSize;
    SidebarUserItem sidebarUser;
    mem::Rc<CommandQueue> commandQueue;
};
