#include "Interfaces/ToolInterface.hpp"
#include "TestHelpers/NullCallback.hpp"
#include "Tools/LayerController.hpp"
#include <Editor/EditorStateManager.hpp>
#include <Shortcuts/ShortcutEngine.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <catch.hpp>

import Memory;

class ToolMock : public ToolInterface
{
public:
    ToolMock(
        std::function<void(void)> onStateChanged,
        mem::Rc<ShortcutEngineInterface> shortcutEngine,
        mem::Rc<LayerObserverInterface> layerObserver,
        const std::string& name,
        std::vector<std::string>& invocations)
        : ToolInterface(onStateChanged, shortcutEngine, layerObserver)
        , name(name)
        , invocations(invocations)
    {
    }

public:
    virtual void penClicked(const sf::Vector2i& position) override {}

    virtual void penDragStarted(const sf::Vector2i& start) override {}

    virtual void
    penDragUpdate(const sf::Vector2i& start, const sf::Vector2i& end) override
    {
    }

    virtual void
    penDragEnded(const sf::Vector2i& start, const sf::Vector2i& end) override
    {
    }

    virtual void penDragCancel(const sf::Vector2i& origin) override {}

    virtual void penDelete() override {}

    virtual void buildSidebar() override
    {
        invocations.push_back(name + ":buildSidebar");
    }

    virtual void configure(nlohmann::json&) override {}

    virtual void resize(unsigned, unsigned, bool) override {}

    void build(unsigned, unsigned) override {}

    virtual void shrinkTo(const TileRect&) override {}

    virtual void saveTo(LevelD&) const override {}

    virtual void loadFrom(const LevelD&) override {}

    virtual void drawTo(tgui::CanvasSFML::Ptr&, uint8_t) override {}

    virtual ExpectedPropertyPtr getProperty(const sf::Vector2i&) const override
    {
        return std::unexpected(BaseError());
    }

    virtual void setProperty(const ToolPropertyInterface&) override {}

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

    virtual void buildCtxMenuInternal(tgui::MenuBar::Ptr&) override {}

    std::optional<TileRect> getBoundingBox() const noexcept override
    {
        return TileRect();
    }

private:
    std::string name;
    std::vector<std::string>& invocations;
};

TEST_CASE("[EditorStateManager]")
{
    auto&& engine = mem::Rc<ShortcutEngine> { [] { return false; } };
    auto&& gui = mem::Rc<tgui::Gui>();
    auto&& layerController = mem::Rc<LayerController>(*gui);
    auto&& invocations = std::vector<std::string> {};
    EditorStateManager manager;

    manager.addState<ToolMock>(
        EditorState::Mesh,
        Null::Callback,
        engine,
        layerController,
        "tool1",
        invocations);
    manager.addState<ToolMock>(
        EditorState::Item,
        Null::Callback,
        engine,
        layerController,
        "tool2",
        invocations);

    SECTION("Loops over all states in insertion order")
    {
        manager.forallStates([](ToolInterface& t) { t.buildSidebar(); });

        REQUIRE(invocations.size() == 2u);
        REQUIRE(invocations[0] == "tool1:buildSidebar");
        REQUIRE(invocations[1] == "tool2:buildSidebar");
    }
}
