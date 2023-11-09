#pragma once

#include <events/AnimationEvents.hpp>
#include <events/AudioEvents.hpp>
#include <events/GameRuleEvents.hpp>
#include <events/RenderingEvents.hpp>
#include <utils/Multiqueue.hpp>

using EventQueue =
    Multiqueue<AudioEvent, AnimationEvent, GameEvent, RenderingEvent>;
