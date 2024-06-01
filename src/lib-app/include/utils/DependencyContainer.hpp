#pragma once

#include "AppOptions.hpp"
#include "AudioPlayer.hpp"
#include "Gui.hpp"
#include "Jukebox.hpp"
#include "PhysicalController.hpp"
#include "logging/LoggerInterface.hpp"
#include <DGM/classes/ResourceManager.hpp>
#include <Memory.hpp>

struct [[nodiscard]] DependencyContainer final
{
    mem::Rc<AudioPlayer> audioPlayer;
    mem::Rc<PhysicalController> controller;
    mem::Rc<Gui> gui;
    mem::Rc<Jukebox> jukebox;
    mem::Rc<const dgm::ResourceManager> resmgr;
    mem::Rc<AppOptions> settings;
    mem::Rc<LoggerInterface> logger;
};
