#include "Gui.hpp"
#include "Loader.hpp"
#include "utils/DependencyContainer.hpp"
#include <DGM/dgm.hpp>
#include <app/AppStateMainMenu.hpp>
#include <core/Scene.hpp>
#include <cxxopts.hpp>
#include <settings/GameTitle.hpp>

import Memory;
import Options;
import Audio;
import Input;
import Resources;
import Error;

CmdParameters processCmdParameters(int argc, char* argv[])
{
    cxxopts::Options options("MyProgram", "One line description of MyProgram");
    // clang-format off
    options.add_options()
        ("s,skip-menu", "Start game directly")
        ("r,resource-dir", "Path to resources", cxxopts::value<std::string>())
        ("m,map", "Map name", cxxopts::value<std::string>())
        ("d,demofile", "Path to demo file", cxxopts::value<std::string>())
        ("p,play-demo", "Whether to replay demo file")
        ("c,count", "Number of players (1-4)", cxxopts::value<unsigned>())
        ("l,limit", "Fraglimit", cxxopts::value<unsigned>());
    // clang-format on
    auto args = options.parse(argc, argv);

    CmdParameters result;

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
    if (args.count("limit") > 0)
        result.fraglimit = args["limit"].as<unsigned>();

    return result;
}

AppOptions loadAppSettings(const std::filesystem::path& path)
{
    try
    {
        std::ifstream load(path);
        nlohmann::json j;
        load >> j;
        AppOptions result = j;
        return result;
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return AppOptions {};
}

ExpectSuccess createAppdataFolder(const std::filesystem::path& path)
{
    std::error_code error;
    if (!std::filesystem::create_directory(path, error))
    {
        return std::unexpected(error.message());
    }

    return ReturnFlag::Success;
}

int main(int argc, char* argv[])
{
    const auto APPDATA_FOLDER = Filesystem::getAppdataPath() / "Rend";
    const auto status = createAppdataFolder(APPDATA_FOLDER);
    status.or_else(
        [&APPDATA_FOLDER](const ErrorMessage& msg) -> ExpectSuccess
        {
            std::cerr << std::format(
                "Error: Could not create folder {}. Reason: {}",
                APPDATA_FOLDER.string(),
                msg) << std::endl;
            return ReturnFlag::Failure;
        });

    const auto CONFIG_FILE_PATH = APPDATA_FOLDER / "app.json";

    auto&& settings = mem::Rc<AppOptions>(loadAppSettings(CONFIG_FILE_PATH));
    settings->cmdSettings = processCmdParameters(argc, argv);

    if (settings->cmdSettings.playerCount > 4)
    {
        throw std::runtime_error("Cannot have more than 4 players!");
    }

    dgm::WindowSettings windowSettings = {
        .resolution = sf::Vector2u(
            settings->display.resolution.width,
            settings->display.resolution.height),
        .title = GAME_TITLE,
        .useFullscreen = settings->display.isFullscreen
    };

    tgui::Texture::setDefaultSmooth(false);

    dgm::Window window(windowSettings);
    window.getWindowContext().setFramerateLimit(60);

    dgm::App app(window);

    auto&& gui = mem::Rc<Gui>();
    gui->gui.setTarget(window.getWindowContext());

    try
    {
        gui->theme->load((settings->cmdSettings.resourcesDir / "editor"
                          / "TransparentGrey.txt")
                             .string());
        // tgui::Theme::setDefault(gui->theme);
    }
    catch (const std::exception& e)
    {
        std::cerr << std::format("error: Loading theme: {}\n", e.what());
        throw;
    }

    auto&& resmgr = mem::Rc<dgm::ResourceManager>();

    try
    {
        Loader::loadResources(*resmgr, settings->cmdSettings.resourcesDir);
    }
    catch (std::exception& e)
    {
        std::cerr << std::format("error:Loading resources: {}\n", e.what());
        throw;
    }

    auto&& audioPlayer = mem::Rc<AudioPlayer>(CHANNEL_COUNT, resmgr);
    audioPlayer->setSoundVolume(settings->audio.soundVolume);
    auto&& jukebox =
        mem::Rc<Jukebox>(*resmgr, settings->cmdSettings.resourcesDir);
    jukebox->setVolume(settings->audio.musicVolume);
    gui->gui.setFont(resmgr->get<tgui::Font>("pico-8.ttf").value());

    auto&& controller = mem::Rc<PhysicalController>(window.getWindowContext());
    controller->updateSettings(settings->input);

    auto&& dic = mem::Rc<DependencyContainer>(
        audioPlayer, controller, gui, jukebox, resmgr, settings);

    app.pushState<AppStateMainMenu>(dic);
    app.run();

    auto outWindowSettings = window.close();

    // Update configuration file
    settings->display.resolution.width = outWindowSettings.resolution.x;
    settings->display.resolution.height = outWindowSettings.resolution.y;
    settings->display.isFullscreen = outWindowSettings.useFullscreen;

    nlohmann::json json = *settings;
    std::ofstream save(CONFIG_FILE_PATH);
    save << json.dump(4);

    return 0;
}