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
         //   std::cout << "apply" << std::endl;

            using args_tuple = typename function_traits<Function>::args_tuple_2nd;
            msgpack_codec codec;
            auto tp = codec.unpack<args_tuple>(data, size);
            invoke(func, result, std::move(tp)); // void test();  args_tuple: std::tuple<int>;
        }
    };

    template<typename Function, typename ArgCallType, typename... Args>
    static void
    invoke(Function& func, std::string& result,  std::tuple<ArgCallType, Args...> tp)
    {
       // std::cout << "invoke" << std::endl;
        invokeHelper(func, std::make_index_sequence<sizeof...(Args)>{}, tp);
    }


    template<typename Function, size_t... I, typename ArgCallType, typename...Args>
    static std::invoke_result_t<Function, Args...>
    invokeHelper(Function& func, const std::index_sequence<I...>&, std::tuple<ArgCallType, Args...> tp)
    {
        return func(std::move(std::get<I + 1>(tp))...);
    }
};


#endif //TEMPLATEINSTANCE_RPCROUTER_H
