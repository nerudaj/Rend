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

    auto addEditorMenuItem = [&](const std::string& label,
                                 std::function<void(void)> callback,
                                 sf::Keyboard::Key shortcut,
                                 bool ctrlRequired = false)
    {
        menu->addMenuItem(label);
        menu->connectMenuItem(MENU_NAME, label, callback);

        shortcutEngine->registerShortcut(
            MENU_NAME, { ctrlRequired, false, shortcut }, callback);
    };

    using namespace Strings::Editor::ContextMenu;

    // Build menu
    menu->addMenu(MENU_NAME);
    addEditorMenuItem(
        MESH_MODE, [this] { switchTool(EditorState::Mesh); }, sf::Keyboard::M);
    addEditorMenuItem(
        ITEM_MODE, [this] { switchTool(EditorState::Item); }, sf::Keyboard::I);
    addEditorMenuItem(
        RESIZE, [this] { resizeDialog(); }, sf::Keyboard::R);
    addEditorMenuItem(
        SHRINK,
        [this] { commandQueue->push<ShrinkToFitCommand>(*this); },
        sf::Keyboard::S);
    addEditorMenuItem(
        LAYER_UP,
        [this] { layerController->moveUp(); },
        sf::Keyboard::Up,
        true);
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
    if (!isInitialized()) return;

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
        std::cout << "Zoom: " << camera.getCurrentZoomLevel() << std::endl;
    }
}

void Editor::draw()
{
    if (!isInitialized()) return;

    // Primary render
    stateMgr.forallStates([this](ToolInterface& tool, bool active)
                          { tool.drawTo(canvas, active ? 255 : 128); });

    gui->gui.get<tgui::Label>("LayerLabel")
        ->setText(layerController->toString());

    drawTagHighlight();

    canvas->draw(mouseIndicator);
}

void Editor::init(
    unsigned levelWidth,
    unsigned levelHeight,
    const std::filesystem::path& configPath)
{
    auto config = JsonHelper::loadFromFile(configPath.string());
    config["configFolder"] = configPath.parent_path().string();

    stateMgr.forallStates([&config](ToolInterface& tool)
                          { tool.configure(config); });

    stateMgr.forallStates(
        [levelWidth, levelHeight](ToolInterface& tool) {
            tool.resize(levelWidth, levelHeight, "isTranslationDisabled"_false);
        });

    const auto tileSize =
        config["toolMesh"]["texture"]["tileDimensions"][0].get<float>();

    // Configure camera
    camera.init();
    camera.resetPosition();
    camera.resetZoom();
    camera.move(
        sf::Vector2f(levelWidth, levelHeight) * static_cast<float>(tileSize)
        - sf::Vector2f(canvas->getSize()) / 2.f);

    initialized = true;

    populateMenuBar();

    // By default selecting mesh tool
    switchTool(EditorState::Mesh);

    // Configure canvas callbacks
    canvas->onRightMouseRelease([this] { handleRmbClicked(); });
    canvas->onMousePress([this] { physicalPen.penDown(); });
    canvas->onMouseRelease([this] { physicalPen.penUp(); });
}

void Editor::switchTool(EditorState state)
{
    stateMgr.changeState(state);
    stateMgr.getActiveTool().buildSidebar();

    auto menu = gui->gui.get<tgui::MenuBar>("TopMenuBar");
    stateMgr.getActiveTool().buildCtxMenu(menu);
}

LevelD Editor::save() const
{
    LevelD result;

    stateMgr.forallStates([&result](const ToolInterface& tool)
                          { tool.saveTo(result); });

    return result;
}

void Editor::loadFrom(
    const LevelD& lvd,
    const std::filesystem::path& pathToJsonConfig,
    bool skipInit)
{
    if (not skipInit)
        // Currently using this to be able to load the config
        init(1, 1, pathToJsonConfig);

    stateMgr.forallStates([&lvd](ToolInterface& tool) { tool.loadFrom(lvd); });

    camera.resetPosition();
    camera.move(
        sf::Vector2f(lvd.mesh.layerWidth, lvd.mesh.layerHeight)
            * static_cast<float>(lvd.mesh.tileWidth)
        - sf::Vector2f(canvas->getSize()) / 2.f);
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

Editor::Editor(
    mem::Rc<Gui> gui,
    tgui::CanvasSFML::Ptr& canvas,
    std::function<void(void)> onStateChanged,
    mem::Rc<CommandQueue> commandQueueRef,
    mem::Rc<ShortcutEngineInterface> shortcutEngineRef)
    : gui(gui)
    , canvas(canvas)
    , commandQueue(commandQueueRef)
    , shortcutEngine(shortcutEngineRef)
    , physicalPen(
          [this]() -> PenUserInterface& { return stateMgr.getActiveTool(); })
{
    stateMgr.addState<ToolMesh>(
        EditorState::Mesh,
        onStateChanged,
        shortcutEngine,
        layerController,
        gui,
        commandQueue);

    stateMgr.addState<ToolItem>(
        EditorState::Item,
        onStateChanged,
        shortcutEngine,
        layerController,
        gui,
        commandQueue);

    mouseIndicator.setRadius(8.f);
    mouseIndicator.setFillColor(sf::Color::Green);
}
