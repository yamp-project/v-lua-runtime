#pragma once

#include <optional>

namespace lua
{
    template<typename T>
    struct CApiClassTraits;

    template<typename ReturnType, typename Class, typename... Args>
    struct CApiClassTraits<ReturnType(*)(Class*, Args...)>
    {
        using class_type = typename Class;
        using arguments = std::tuple<Args...>;
        using return_type = typename ReturnType;
    };

    template<typename ReturnType, typename... Args>
    struct CApiClassTraits<ReturnType(*)(Args...)>
    {
        //using class_type = typename Class;
        using arguments = std::tuple<Args...>;
        using return_type = typename ReturnType;
    };

    template<typename T>
    struct TypeTraits;

    template<typename C, typename... Args>
    struct TypeTraits<C(Args...)>
    {
    public:
        using class_type = typename C;
        using arguments = std::tuple<Args...>;
    };

    // primary template
    template<class T, class...>
    struct type {
        using value_type = T;
    };

    // specialization for template instantiated types
    template<template<class, class...> class T, class F, class... Rest>
    struct type<T<F, Rest...>> {
        using value_type = typename type<F>::value_type;
    };

    // helper alias
    template<class... Ts>
    using type_t = typename type<Ts...>::value_type;

    template<typename T>
    struct RemoveOptional;

    template<typename T>
    struct RemoveOptional<std::optional<T>>
    {
        using real_type = T;
    };
}