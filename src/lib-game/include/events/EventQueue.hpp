#pragma once

#include "AnimationEvents.hpp"
#include "AudioEvents.hpp"
#include "GameRuleEvents.hpp"
#include "PhysicsEvents.hpp"
#include "RenderingEvents.hpp"
#include <vector>

class EventQueue final
{
public:
    /// <summary>
    /// Push new event into event queue. Event will be automatically
    /// placed into appropriate variant vector;
    /// </summary>
    /// <typeparam name="T">Type of event</typeparam>
    /// <typeparam name="...Args">Arguments for the construction of
    /// event</typeparam> <param name="...args"></param>
    template<class T, class... Args>
        requires std::constructible_from<T, Args...>
    constexpr inline static void add(Args&&... args)
    {
        if constexpr (isTypeInVariant<T>(animationEvents))
        {
            animationEvents.emplace_back<T>(std::forward<Args>(args)...);
        }
        else if constexpr (isTypeInVariant<T>(audioEvents))
        {
            audioEvents.emplace_back<T>(std::forward<Args>(args)...);
        }
        else if constexpr (isTypeInVariant<T>(gameEvents))
        {
            gameEvents.emplace_back<T>(std::forward<Args>(args)...);
        }
        else if constexpr (isTypeInVariant<T>(physicsEvents))
        {
            physicsEvents.emplace_back<T>(std::forward<Args>(args)...);
        }
        else if constexpr (isTypeInVariant<T>(renderingEvents))
        {
            renderingEvents.emplace_back<T>(std::forward<Args>(args)...);
        }
        else
        {
            assert(false);
        }
    }

    template<class EventType, class Visitor>
    constexpr inline static void processEvents(Visitor& visitor)
    {
        if constexpr (std::is_same_v<EventType, AnimationEvent>)
        {
            visit(visitor, animationEvents);
        }
        if constexpr (std::is_same_v<EventType, AudioEvent>)
        {
            visit(visitor, audioEvents);
        }
        else if constexpr (std::is_same_v<EventType, GameEvent>)
        {
            visit(visitor, gameEvents);
        }
        else if constexpr (std::is_same_v<EventType, PhysicsEvent>)
        {
            visit(visitor, physicsEvents);
        }
        else if constexpr (std::is_same_v<EventType, RenderingEvent>)
        {
            visit(visitor, renderingEvents);
        }
    }

    static void clearAudioEvents()
    {
        audioEvents.clear();
    }

private:
    template<class T, class... Ts>
    [[nodiscard]] constexpr static bool
    isTypeInVariant(const std::vector<std::variant<Ts...>>&) noexcept
    {
        return std::disjunction_v<std::is_same<T, Ts>...>;
    }

    constexpr inline static void visit(auto&& visitor, auto& events)
    {
        for (unsigned i = 0; i < events.size(); i++)
        {
            std::visit(visitor, events[i]);
        }
        events.clear();
    }

public:
    static inline std::vector<AnimationEvent> animationEvents;
    static inline std::vector<AudioEvent> audioEvents;
    static inline std::vector<GameEvent> gameEvents;
    static inline std::vector<PhysicsEvent> physicsEvents;
    static inline std::vector<RenderingEvent> renderingEvents;
};
