#include "Loader.hpp"
#include <DGM/dgm.hpp>
#include <Settings.hpp>
#include <app/AppStateMainMenu.hpp>
#include <audio/AudioPlayer.hpp>
#include <core/Scene.hpp>
#include <cxxopts.hpp>
#include <settings/GameTitle.hpp>

import Memory;

CmdSettings processCmdParameters(int argc, char* argv[])
{
    cxxopts::Options options("MyProgram", "One line description of MyProgram");
    // clang-format off
    options.add_options()
        ("s,skip-menu", "Start game directly")
        ("r,resource-dir", "Path to resources", cxxopts::value<std::string>())
        ("m,map", "Map name", cxxopts::value<std::string>())
    ("d,demofile", "Path to demo file", cxxopts::value<std::string>())
        ("p,play-demo", "Whether to replay demo file")
        ("c,count", "Number of players (1-4)", cxxopts::value<unsigned>());
    // clang-format on
    auto args = options.parse(argc, argv);

    CmdSettings result;

    if (args.count("skip-menu") > 0)
        result.skipMainMenu = args["skip-menu"].as<bool>();
    if (args.count("resource-dir") > 0)
        result.resourcesDir = args["resource-dir"].as<std::string>();
    if (args.count("map") > 0) result.mapname = args["map"].as<std::string>();
    if (args.count("demofile") > 0)
        result.demoFile = args["demofile"].as<std::string>();
    if (args.count("play-demo") > 0)
        result.playDemo = args["play-demo"].as<bool>();
    if (args.count("count") > 0)
        result.playerCount = args["count"].as<unsigned>();

    return result;
}

AppSettings loadAppSettings(const std::filesystem::path& path)
{
    try
    {
        return loadFromFile(path);
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return AppSettings {};
}

int main(int argc, char* argv[])
{
    const auto CONFIG_FILE_PATH = "app.json";

    auto&& settings = mem::Rc<Settings>();
    settings->cmdSettings = processCmdParameters(argc, argv);
    settings->appSettings = loadAppSettings(CONFIG_FILE_PATH);

    if (settings->cmdSettings.playerCount > 4)
    {
        throw std::runtime_error("Cannot have more than 4 players!");
    }

    dgm::WindowSettings windowSettings = {
        .resolution = sf::Vector2u(
            settings->appSettings.windowWidth,
            settings->appSettings.windowHeight),
        .title = GAME_TITLE,
        .useFullscreen = settings->appSettings.fullscreen
    };

    dgm::Window window(windowSettings);
    window.getWindowContext().setFramerateLimit(60);

    dgm::App app(window);
    auto&& gui = mem::Rc<tgui::Gui>();
    gui->setTarget(window.getWindowContext());
    auto&& resmgr = mem::Rc<dgm::ResourceManager>();
    auto&& audioPlayer = mem::Rc<AudioPlayer>(CHANNEL_COUNT, resmgr);
    audioPlayer->setSoundVolume(settings->appSettings.soundVolume);

    try
    {
        Loader::loadResources(*resmgr, settings->cmdSettings.resourcesDir);
    }
    catch (std::exception& e)
    {
        std::cerr << std::format("error:Loading resources: {}\n", e.what());
        throw;
    }
    gui->setFont(resmgr->get<tgui::Font>("pico-8.ttf").value());

    app.pushState<AppStateMainMenu>(resmgr, gui, audioPlayer, settings);
    app.run();

    auto outWindowSettings = window.close();

    // Update configuration file
    settings->appSettings.windowWidth = outWindowSettings.resolution.x;
    settings->appSettings.windowHeight = outWindowSettings.resolution.y;
    settings->appSettings.fullscreen = outWindowSettings.useFullscreen;
    saveToFile(CONFIG_FILE_PATH, settings->appSettings);

    return 0;
}