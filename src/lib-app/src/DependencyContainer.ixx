module;

#include <DGM/classes/ResourceManager.hpp>

export module DependencyContainer;

import Audio;
import Gui;
import Memory;
import Options;
import Input;

export struct [[nodiscard]] DependencyContainer final
{
    mem::Rc<AudioPlayer> audioPlayer;
    mem::Rc<PhysicalController> controller;
    mem::Rc<Gui> gui;
    mem::Rc<Jukebox> jukebox;
    mem::Rc<const dgm::ResourceManager> resmgr;
    mem::Rc<AppOptions> settings;
};
