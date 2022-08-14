//
// Created by Meepo on 2022/8/10.
//

#ifndef TEMPLATEINSTANCE_RPCROUTER_H
#define TEMPLATEINSTANCE_RPCROUTER_H

#include <type_traits>
#include <utility>
#include "RpcTraits.h"
#include "codec.h"

template <class TupType, size_t... I>
void printParam(const TupType& tup, std::index_sequence<I...>)
{
    std::cout << "(";
    (..., (std::cout << (I == 0 ? "" : ", ") << std::get<I>(tup)));
    std::cout << ")\n";
}

template <class... T>
void printParam(const std::tuple<T...>& tup)
{
    printParam(tup, std::make_index_sequence<sizeof...(T)>());
}

struct RpcRouter
{
    template <typename Function>
    struct invoker
    {
        static inline void apply(const Function& func, const char* data, size_t size, std::string& result)
        {
            using args_tuple = typename function_traits<Function>::args_tuple;
            msgpack_codec codec;
            auto tup = codec.unpack<args_tuple>(data, size);
            call(func, result, std::move(tup));
            printParam(tup);
        }

        template <typename Object>
        static inline void
        applyMember(const Function& func, Object* object, const char* data, size_t size, std::string& result)
        {
            using args_tuple = typename function_traits<Function>::args_tuple;
            msgpack_codec codec;
            auto tup = codec.unpack<args_tuple>(data, size);
            callMember(func, object, result, std::move(tup));
            printParam(tup);
        }
    };

    template <typename Function, typename... Args>
    static std::enable_if_t<std::is_void_v<std::invoke_result_t<Function, Args...>>>
    call(Function&& func, std::string& result, std::tuple<Args...>&& tup)
    {
        std::cout << "invoke return void." << std::endl;

        callHelper(func, std::make_index_sequence<sizeof...(Args)>{}, std::move(tup));
        result = "OK";
    }

    template <typename Function, typename... Args>
    static std::enable_if_t<!std::is_void_v<std::invoke_result_t<Function, Args...>>>
    call(Function&& func, std::string& result, std::tuple<Args...>&& tup)
    {
        std::cout << "invoke return non-void." << std::endl;

        auto ret = callHelper(func, std::make_index_sequence<sizeof...(Args)>{}, std::move(tup));
        // do something with ret
        result = msgpack_codec::pack_args_str("OK", ret);
    }

    template <typename Function, size_t... I, typename...Args>
    static std::invoke_result_t<Function, Args...>
    callHelper(Function&& func, const std::index_sequence<I...>&, std::tuple<Args...>&& tup)
    {
        return std::forward<Function>(func)(std::forward<Args>(std::get<I + 1>(tup))...);
        //return func(std::move(std::get<I + 1>(tup))...);
    }

    template <typename Function, typename Object, typename... Args>
    static std::enable_if_t<std::is_void_v<std::invoke_result_t<Function, Args...>>>
    callMember(const Function& func, Object* object, std::string result, std::tuple<Args...>&& tup)
    {
        callMemHelper(func, object, typename std::make_index_sequence<sizeof...(Args)>{}, std::move(tup));
        result = "OK";
    }

    template <typename Function, typename Object, typename... Args>
    static std::enable_if_t<!std::is_void_v<std::invoke_result_t<Function, Args...>>>
    callMember(const Function& func, Object* object, std::string result, std::tuple<Args...>&& tup)
    {
        auto ret = callMemHelper(func, object, typename std::make_index_sequence<sizeof...(Args)>{}, std::move(tup));
        result = msgpack_codec::pack_args_str("OK", ret);
    }

    template <typename Function, size_t... I, typename...Args>
    static std::invoke_result_t<Function, Args...>
    callMemHelper(const Function& func, const std::index_sequence<I...>&, std::tuple<Args...>&& tup)
    {
        return func(std::move(std::get<I + 1>(tup))...);
        //todo invoke
    }
};


#endif //TEMPLATEINSTANCE_RPCROUTER_H
