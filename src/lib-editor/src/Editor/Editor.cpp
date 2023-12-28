#include "Editor/Editor.hpp"
#include "Commands/ResizeCommand.hpp"
#include "Commands/ShrinkToFitCommand.hpp"
#include "Configs/Strings.hpp"
#include "JsonHelper.hpp"
#include "Utilities/Literals.hpp"
#include "Utilities/Utilities.hpp"
#include <filesystem>
#include <fstream>

const sf::Vector2f LEFT_VEC(-24.f, 0.f);
const sf::Vector2f UP_VEC(0.f, -24.f);
const sf::Vector2f DOWN_VEC(0.f, 24.f);
const sf::Vector2f RIGHT_VEC(24.f, 0.f);

const std::vector<bool> DEFAULT_BLOCKS = { false, false, false, false,
                                           false, true,  true,  true,
                                           true,  true,  true,  true,
                                           true,  true,  true,  true };

import TexturePath;
import Resources;

Editor::Editor(
    mem::Rc<Gui> _gui,
    tgui::CanvasSFML::Ptr& _canvas,
    std::function<void(void)> _onStateChanged,
    mem::Rc<CommandQueue> _commandQueue,
    mem::Rc<ShortcutEngineInterface> _shortcutEngine,
    mem::Rc<LevelMetadata> metadata,
    const LevelD& level,
    const std::filesystem::path& _graphicsDir)
    : gui(_gui)
    , canvas(_canvas)
    , onStateChanged(_onStateChanged)
    , commandQueue(_commandQueue)
    , shortcutEngine(_shortcutEngine)
    , levelMetadata(metadata)
    , graphicsDir(_graphicsDir)
    , physicalPen(
          [this]() -> PenUserInterface& { return stateMgr.getActiveTool(); })
    , layerController(gui->gui)
{
    configureCamera(
        level.mesh.layerWidth, level.mesh.layerHeight, level.mesh.tileWidth);
    configureMouseIndicator();
    configureMeshTool(level);
    switchTool(EditorState::Mesh);
    configureItemTool(level);
    populateMenuBar();
    configureCanvasCallbacks();
}

void Editor::configureItemTool(const LevelD& level)
{
    auto spritesheetPath = graphicsDir / "editor-items.png";
    auto clip =
        dgm::JsonLoader().loadClipFromFile(spritesheetPath.string() + ".clip");

    stateMgr.addState<ToolItem>(
        EditorState::Item,
        onStateChanged,
        shortcutEngine,
        layerController,
        gui,
        commandQueue,
        spritesheetPath,
        clip,
        level);
}

void Editor::configureCanvasCallbacks()
{
    canvas->onRightMouseRelease([this] { handleRmbClicked(); });
    canvas->onMousePress([this] { physicalPen.penDown(); });
    canvas->onMouseRelease([this] { physicalPen.penUp(); });
}

void Editor::configureMeshTool(const LevelD& level)
{
    auto tilesetPath =
        graphicsDir / TexturePath::getTilesetName(levelMetadata->texturePack);
    auto clip =
        dgm::JsonLoader().loadClipFromFile(tilesetPath.string() + ".clip");

    stateMgr.addState<ToolMesh>(
        EditorState::Mesh,
        onStateChanged,
        shortcutEngine,
        layerController,
        gui,
        commandQueue,
        tilesetPath,
        clip,
        DEFAULT_BLOCKS,
        level);
}

constexpr bool
Editor::isMouseWithinBoundaries(const sf::Vector2f& mousePos) const noexcept
{
    return mousePos.x < 0.f && mousePos.y < 0.f;
}

void Editor::populateMenuBar()
{
    constexpr const char* MENU_NAME = "Editor";

    auto menu = gui->gui.get<tgui::MenuBar>("TopMenuBar");

    // Cleanup previously built data
    menu->removeMenu(MENU_NAME);
    shortcutEngine->unregisterShortcutGroup(MENU_NAME);

    auto addEditorMenuItem =
        [&](const std::string& label,
            std::function<void(void)> callback,
            sf::Keyboard::Key shortcut = sf::Keyboard::KeyCount,
            bool ctrlRequired = false)
    {
        menu->addMenuItem(label);
        menu->connectMenuItem(MENU_NAME, label, callback);

        if (shortcut != sf::Keyboard::KeyCount)
            shortcutEngine->registerShortcut(
                MENU_NAME, { ctrlRequired, false, shortcut }, callback);
    };

    using namespace Strings::Editor::ContextMenu;

    // Build menu
    menu->addMenu(MENU_NAME);
    addEditorMenuItem(
        MESH_MODE, [this] { switchTool(EditorState::Mesh); }, sf::Keyboard::M);
    addEditorMenuItem(
        ITEM_MODE,
        [this]
        {
            switchTool(EditorState::Item);
            layerController->moveDown();
        },
        sf::Keyboard::I);
    addEditorMenuItem(
        EDIT_METADATA,
        [this]
        {
            editMetadataDialog.open(
                *levelMetadata,
                std::bind(&Editor::handleChangedMetadata, this));
        });
    addEditorMenuItem(
        LAYER_UP, [this] { handleSwitchLayerUp(); }, sf::Keyboard::Up, true);
    addEditorMenuItem(
        LAYER_DOWN,
        [this] { layerController->moveDown(); },
        sf::Keyboard::Down,
        true);

    // Scrolling
    shortcutEngine->registerShortcut(
        MENU_NAME,
        { false, false, sf::Keyboard::Up },
        [&]
        {
            if (canScroll()) camera.move(UP_VEC);
        });
    shortcutEngine->registerShortcut(
        MENU_NAME,
        { false, false, sf::Keyboard::Left },
        [&]
        {
            if (canScroll()) camera.move(LEFT_VEC);
        });
    shortcutEngine->registerShortcut(
        MENU_NAME,
        { false, false, sf::Keyboard::Down },
        [&]
        {
            if (canScroll()) camera.move(DOWN_VEC);
        });
    shortcutEngine->registerShortcut(
        MENU_NAME,
        { false, false, sf::Keyboard::Right },
        [&]
        {
            if (canScroll()) camera.move(RIGHT_VEC);
        });
}

void Editor::handleRmbClicked()
{
    auto prop =
        stateMgr.getActiveTool().getProperty(physicalPen.getCurrentPenPos());

    if (!canOpenPropertyDialog() || !prop.has_value()) return;

    editPropertyDialog.open(std::move(prop.value()), stateMgr.getActiveTool());
}

void Editor::handleChangedMetadata()
{
    bool somethingChanged =
        levelMetadata->author != editMetadataDialog.getAuthorName()
        || levelMetadata->skyboxTheme != editMetadataDialog.getSkyboxTheme()
        || levelMetadata->texturePack != editMetadataDialog.getTexturePack();

    if (!somethingChanged) return;

    levelMetadata->author = editMetadataDialog.getAuthorName();
    levelMetadata->skyboxTheme = editMetadataDialog.getSkyboxTheme();
    levelMetadata->texturePack = editMetadataDialog.getTexturePack();

    onStateChanged();

    LevelD level;
    stateMgr.getTool(EditorState::Mesh).saveTo(level);
    configureMeshTool(level);
    switchTool(EditorState::Mesh);
}

void Editor::handleSwitchLayerUp()
{
    if (stateMgr.getCurrentState() == EditorState::Item)
    {
        throw std::runtime_error("Cannot switch layers in item mode");
    }

    layerController->moveUp();
}

void Editor::drawTagHighlight()
{
    auto&& object = stateMgr.getActiveTool().getHighlightedObject(
        physicalPen.getCurrentPenPos());
    if (!object.has_value() || object->tag == 0) return;

    std::vector<sf::Vertex> positions;
    auto computeLinesToDraw = [&object, &positions](ToolInterface& t)
    {
        const auto vec = t.getPositionsOfObjectsWithTag(object->tag);
        for (auto&& pos : vec)
        {
            positions.push_back(sf::Vertex(sf::Vector2f(object->position)));
            positions.push_back(sf::Vertex(sf::Vector2f(pos)));
        }
    };

    stateMgr.forallStates(computeLinesToDraw);
    canvas->draw(positions.data(), positions.size(), sf::Lines);
}

void Editor::handleEvent(const sf::Event& event, const sf::Vector2i& mousePos)
{
    // Update mouse position for both indicator and current tool
    auto realMousePos = camera.getWorldCoordinates(sf::Vector2f(mousePos));
    mouseIndicator.setPosition(realMousePos);
    physicalPen.updatePenPosition(sf::Vector2i(realMousePos));

    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Escape)
            physicalPen.penCancel();
        else if (event.key.code == sf::Keyboard::Delete)
            physicalPen.penDelete();
    }
    else if (
        event.type == sf::Event::MouseWheelScrolled
        && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
    {
        camera.zoom(event.mouseWheelScroll.delta * -0.25f);
    }
}

void Editor::draw()
{
    // Primary render
    for (std::size_t idx = 0; idx <= layerController->getCurrentLayerIdx();
         ++idx)
    {
        stateMgr.forallStates(
            [&](ToolInterface& tool, bool active)
            { tool.drawTo(canvas, idx, active ? 255 : 128); });
    }

    canvas->draw(mouseIndicator);
}

void Editor::switchTool(EditorState state)
{
    stateMgr.changeState(state);
    stateMgr.getActiveTool().buildSidebar();

    auto menu = gui->gui.get<tgui::MenuBar>("TopMenuBar");
    stateMgr.getActiveTool().buildCtxMenu(menu);
}

LevelD Editor::save()
{
    LevelD result;

    dynamic_cast<ToolMesh&>(stateMgr.getTool(EditorState::Mesh))
        .sanitizeBeforeSave();
    stateMgr.forallStates([&result](const ToolInterface& tool)
                          { tool.saveTo(result); });

    return result;
}

void Editor::resizeDialog()
{
    dialog.open(
        [this]
        {
            commandQueue->push<ResizeCommand>(
                *this,
                dialog.getLevelWidth(),
                dialog.getLevelHeight(),
                dialog.isTranslationDisabled());
        });
}

void Editor::resize(unsigned width, unsigned height, bool isTranslationDisabled)
{
    stateMgr.forallStates(
        [width, height, isTranslationDisabled](ToolInterface& t)
        { t.resize(width, height, isTranslationDisabled); });
}

void Editor::shrinkToFit()
{
    std::optional<TileRect> boundingBox;
    stateMgr.forallStates(
        [&boundingBox](const ToolInterface& t) {
            boundingBox =
                Utilities::unifyRects(boundingBox, t.getBoundingBox());
        });

    if (boundingBox.has_value())
    {
        stateMgr.forallStates([&boundingBox](ToolInterface& t)
                              { t.shrinkTo(boundingBox.value()); });
    }
}
