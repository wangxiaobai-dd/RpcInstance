//
// Created by Meepo on 2022/8/11.
//

#ifndef TEMPLATEINSTANCE_RPCTRAITS_H
#define TEMPLATEINSTANCE_RPCTRAITS_H

#include <tuple>
#include <functional>

template <typename T> struct function_traits;

template <typename Ret, typename Arg, typename... Args>
struct function_traits<Ret(Arg, Args...)> {
public:
    enum { arity = sizeof...(Args) + 1 };
    typedef Ret function_type(Arg, Args...);
    typedef Ret return_type;
    using stl_function_type = std::function<function_type>;
    typedef Ret (*pointer)(Arg, Args...);

    typedef std::tuple<Arg, Args...> tuple_type;
    typedef std::tuple<std::remove_const_t<std::remove_reference_t<Arg>>,
            std::remove_const_t<std::remove_reference_t<Args>>...>
            bare_tuple_type;
    using args_tuple_2nd =
            std::tuple<int,
                    std::remove_const_t<std::remove_reference_t<Args>>...>;
};

// 空参数
template <typename Ret> struct function_traits<Ret()> {
public:
    enum { arity = 0 };
    typedef Ret function_type();
    typedef Ret return_type;
    using stl_function_type = std::function<function_type>;
    typedef Ret (*pointer)();

    typedef std::tuple<> tuple_type;
    typedef std::tuple<> bare_tuple_type;

    using args_tuple_2nd = std::tuple<int>;
};

// 函数指针
template <typename Ret, typename... Args>
struct function_traits<Ret (*)(Args...)> : function_traits<Ret(Args...)> {};

// std::function
template <typename Ret, typename... Args>
struct function_traits<std::function<Ret(Args...)>>
        : function_traits<Ret(Args...)> {};

// 成员函数
template <typename ReturnType, typename ClassType, typename... Args>
struct function_traits<ReturnType (ClassType::*)(Args...)>
        : function_traits<ReturnType(Args...)> {};

// 成员函数 const
template <typename ReturnType, typename ClassType, typename... Args>
struct function_traits<ReturnType (ClassType::*)(Args...) const>
        : function_traits<ReturnType(Args...)> {};

// 函数对象
template <typename Callable>
struct function_traits : function_traits<decltype(&Callable::operator())> {};

#endif //TEMPLATEINSTANCE_RPCTRAITS_H
