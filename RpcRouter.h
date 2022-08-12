//
// Created by Meepo on 2022/8/10.
//

#ifndef TEMPLATEINSTANCE_RPCROUTER_H
#define TEMPLATEINSTANCE_RPCROUTER_H

#include <type_traits>

#include "RpcTraits.h"
#include "codec.h"

struct RpcRouter
{
    template<typename Function>
    struct invoker
    {
        static inline void apply(Function& func, const char* data, size_t size, std::string& result)
        {
            /*
            using args_tuple = typename function_traits<Function>::args_tuple;
            msgpack_codec codec;
            auto tp = codec.unpack<args_tuple>(data, size);
            invoke(func, result, std::move(tp));
             */
        }
    };

    template<typename Function, typename... Args>
    static typename std::enable_if<std::is_void_v<std::invoke_result_t<Function(Args...)>>>::type
    invoke(Function&& func, std::string& result, const std::tuple<Args...>& tp)
    {
        callHelper(func, std::make_index_sequence<sizeof...(Args)>{}, tp);
    }

    template<typename Function, size_t... I, typename...Args>
    static std::invoke_result_t<Function, Args...>
    invokeHelper(Function&& func, const std::index_sequence<I...>&, std::tuple<Args...>& tp)
    {
        return func(std::move(std::get<I>(tp))...);
    }
};


#endif //TEMPLATEINSTANCE_RPCROUTER_H
