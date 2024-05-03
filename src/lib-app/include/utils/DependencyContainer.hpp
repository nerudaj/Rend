#pragma once

#include "Gui.hpp"
#include <DGM/classes/ResourceManager.hpp>

import Audio;
import Memory;
import Options;
import Input;

struct [[nodiscard]] DependencyContainer final
{
    mem::Rc<AudioPlayer> audioPlayer;
    mem::Rc<PhysicalController> controller;
    mem::Rc<Gui> gui;
    mem::Rc<Jukebox> jukebox;
    mem::Rc<const dgm::ResourceManager> resmgr;
    mem::Rc<AppOptions> settings;
};
