#pragma once

#include <concepts>
#include <tuple>
#include <variant>
#include <vector>

template<class BaseType, class Variant>
concept IsTypeInVariant = std::constructible_from<Variant, BaseType>;

template<class Visitor, class Variant>
concept CanVisitVariant = requires(Visitor&& visitor, Variant&& variant) {
    {
        std::visit(visitor, variant)
    };
};

template<class T>
struct is_variant : std::false_type
{
};

template<class... Ts>
struct is_variant<std::variant<Ts...>> : std::true_type
{
};

template<class T>
concept IsVariant = is_variant<T>::value;

template<IsVariant... Variants>
class Multiqueue
{
public:

public:
    using Queues = std::tuple<std::vector<Variants>...>;

    template<class T, class... Args>
        requires std::constructible_from<T, Args...>
    void emplace(Args&&... args)
    {
        const auto index = getIndex<true, T, Variants...>(0);
        std::get<index>(queues).push_back(T(std::forward<Args>(args)...));
    }

    template<class Visitor>
    void processAndClear(Visitor& visitor)
    {
        const auto index = getIndex<false, Visitor, Variants...>(0);
        auto& queue = std::get<index>(queues);
        for (unsigned i = 0; i < queue.size(); ++i)
            std::visit(visitor, queue[i]);
        queue.clear();
    }

    template<class Variant>
    void clear()
    {
        std::get<std::vector<Variant>>(queues).clear();
    }

    template<class Variant>
    [[nodiscard]] constexpr bool isEmpty() const
    {
        return std::get<std::vector<Variant>>(queues).empty();
    }

private:
    template<bool DeducingFromBaseType, class T>
    static consteval std::size_t getIndex(std::size_t index) noexcept
    {
        return index;
    }

    template<
        bool DeducingFromBaseType,
        class T,
        class Variant,
        class... RestOfTheVariants>
    static consteval std::size_t getIndex(std::size_t index) noexcept
    {
        if constexpr (
            DeducingFromBaseType && IsTypeInVariant<T, Variant>
            || !DeducingFromBaseType && CanVisitVariant<T, Variant>)
        {
            return index;
        }

        return getIndex<DeducingFromBaseType, T, RestOfTheVariants...>(
            index + 1);
    }

private:
    Queues queues;
};
