#pragma once

#include "AppOptions.hpp"
#include "AudioPlayer.hpp"
#include "Gui.hpp"
#include "Jukebox.hpp"
#include "LoggerInterface.hpp"
#include "PhysicalController.hpp"
#include <DGM/classes/ResourceManager.hpp>
#include <Dialogs/ErrorInfoDialog.hpp>
#include <Memory.hpp>

struct [[nodiscard]] DependencyContainer final
{
    mem::Rc<AudioPlayer> audioPlayer;
    mem::Rc<PhysicalController> controller;
    mem::Rc<Gui> gui;
    mem::Rc<Jukebox> jukebox;
    mem::Rc<dgm::ResourceManager> resmgr;
    mem::Rc<AppOptions> settings;
    mem::Rc<LoggerInterface> logger;
    ErrorInfoDialog warningDialog;
    std::function<void(const std::filesystem::path&)> lateLoadMapIntoManager;
};
