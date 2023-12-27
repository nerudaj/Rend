#pragma once

#include "Camera.hpp"
#include "Commands/CommandQueue.hpp"
#include "Dialogs/EditMetadataDialog.hpp"
#include "Dialogs/EditPropertyDialog.hpp"
#include "Dialogs/ResizeLevelDialog.hpp"
#include "Editor/EditorState.hpp"
#include "Editor/EditorStateManager.hpp"
#include "Globals.hpp"
#include "Interfaces/EditorInterface.hpp"
#include "Shortcuts/ShortcutEngine.hpp"
#include "Tools/LayerController.hpp"
#include "Tools/PhysicalPen.hpp"
#include "Tools/ToolItem.hpp"
#include "Tools/ToolMesh.hpp"
#include <DGM/dgm.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

import Memory;

class [[nodiscard]] Editor final : public EditorInterface
{
public:
    Editor(
        mem::Rc<Gui> gui,
        tgui::CanvasSFML::Ptr& canvas,
        std::function<void(void)> onStateChanged,
        mem::Rc<CommandQueue> commandQueue,
        mem::Rc<ShortcutEngineInterface> shortcutEngine,
        mem::Rc<LevelMetadata> metadata,
        const LevelD& level,
        const std::filesystem::path& graphicsDir);

    Editor(Editor&&) = delete;
    Editor(const Editor&) = default;

public:
    virtual void
    handleEvent(const sf::Event& event, const sf::Vector2i& mousePos) override;

    virtual void draw() override;

    virtual void switchTool(EditorState state) override;

    [[nodiscard]] virtual LevelD save() override;

    virtual void resizeDialog() override;

    virtual void resize(
        unsigned width, unsigned height, bool isTranslationDisabled) override;

    virtual void shrinkToFit() override;

    void restoreFromSnapshot(const LevelD& snapshot) override
    {
        stateMgr.forallStates([&snapshot](ToolInterface& tool, bool)
                              { tool.restoreFrom(snapshot); });
    }

private:
    [[nodiscard]] constexpr bool
    isMouseWithinBoundaries(const sf::Vector2f& mousePos) const noexcept;

    [[nodiscard]] bool canScroll() const
    {
        return !gui->isAnyModalOpened();
    }

    [[nodiscard]] bool canOpenPropertyDialog() const
    {
        // If property window is opened, do not open new one
        return canScroll();
    }

protected:
    void populateMenuBar();

    void handleRmbClicked();

    void handleChangedMetadata();

    void handleSwitchLayerUp();

    void drawTagHighlight();

private: // Initialization
    void
    configureCamera(unsigned levelWidth, unsigned levelHeight, unsigned tileDim)
    {
        camera.init();
        camera.resetPosition();
        camera.resetZoom();
        camera.move(
            sf::Vector2f(
                static_cast<float>(levelWidth), static_cast<float>(levelHeight))
                * static_cast<float>(tileDim) / 2.f
            - sf::Vector2f(canvas->getSize()) / 2.f);
    }

    void configureMouseIndicator()
    {
        mouseIndicator.setRadius(8.f);
        mouseIndicator.setFillColor(sf::Color::Green);
    }

    void configureCanvasCallbacks();

    void configureMeshTool(
        const std::filesystem::path& graphicsDir,
        std::function<void()>& onStateChanged,
        mem::Rc<Gui>& gui,
        const LevelD& level);

    void configureItemTool(
        const std::filesystem::path& graphicsDir,
        std::function<void()>& onStateChanged,
        mem::Rc<Gui>& gui,
        const LevelD& level);

private: // Dependencies
    mem::Rc<Gui> gui;
    tgui::CanvasSFML::Ptr& canvas;
    mem::Rc<CommandQueue> commandQueue;
    mem::Rc<ShortcutEngineInterface> shortcutEngine;
    mem::Rc<LevelMetadata> levelMetadata;

private:
    ResizeDialog dialog = ResizeDialog(gui);
    EditPropertyDialog editPropertyDialog = EditPropertyDialog(gui);
    EditMetadataDialog editMetadataDialog = EditMetadataDialog(gui);
    Camera camera = Camera(canvas);
    EditorStateManager stateMgr;
    sf::CircleShape mouseIndicator;
    PhysicalPen physicalPen;
    mem::Rc<LayerController> layerController;
};