export module CoreTypes;

export
{
    template<class... Ts>
    struct overloaded : Ts...
    {
        using Ts::operator()...;
    };
}