//
// Created by Meepo on 2022/8/10.
//

#ifndef TEMPLATEINSTANCE_RPCINVOKER_H
#define TEMPLATEINSTANCE_RPCINVOKER_H

#include <type_traits>
#include <utility>
#include "RpcTraits.h"
#include "codec.h"


template <class TupType, size_t... I>
void printParam(const TupType& tup, std::index_sequence<I...>)
{
    std::cout << "printParam: (";
    (..., (std::cout << (I == 0 ? "" : ", ") << std::get<I>(tup)));
    std::cout << ")" << std::endl;
}

template <class... T>
void printParam(const std::tuple<T...>& tup)
{
    printParam(tup, std::make_index_sequence<sizeof...(T)>());
}

struct RpcInvoker
{

    template <typename Function>
    struct Invoker
    {
        static inline void apply(const Function& func, const char* data, size_t size, std::string& result)
        {
            std::cout << "apply" << std::endl;
            using args_tuple = typename function_traits<Function>::args_tuple;
            try
            {
                msgpack_codec codec;
                auto tup = codec.unpack<args_tuple>(data, size); // todo 自己的序列化
                printParam(tup);
                call(func, result, std::move(tup));
            }
            catch(const std::invalid_argument& e)
            {
                result = "ARGS FAIL";
            }
            catch(const std::exception& e)
            {
                result = "FAIL";
            }
        }

        template <typename Object>
        static inline void
        applyMember(const Function& func, Object* object, const char* data, size_t size, std::string& result)
        {
            std::cout << "applyMember" << std::endl;
            using args_tuple = typename function_traits<Function>::args_tuple;
            msgpack_codec codec;
            auto tup = codec.unpack<args_tuple>(data, size);
            callMember(func, object, result, std::move(tup));
            printParam(tup);
        }

        static inline void applyUsingCmd(const Function& func, const char* data, size_t size, std::string& result)
        {
            std::cout << "applyUsingCmd" << std::endl;
            std::invoke(func, (const RPC::stDataBaseCmd*) data, size);
            result = "OK";
        }

        template <typename Object>
        static inline void
        applyMemUsingCmd(const Function& func, Object* object, const char* data, size_t size, std::string& result)
        {
            std::cout << "applyMemUsingCmd" << std::endl;
            std::invoke(func, object, (const RPC::stDataBaseCmd*) data, size);
            result = "OK";
        }
    };

    template <typename Function, typename... Args>
    static std::enable_if_t<std::is_void_v<std::invoke_result_t<Function, Args...>>>
    call(Function& func, std::string& result, std::tuple<Args...> tup)
    {
        callHelper(func, std::make_index_sequence<sizeof...(Args)>{}, std::move(tup));
        result = "invoke return void. OK";
    }

    template <typename Function, typename... Args>
    static std::enable_if_t<!std::is_void_v<std::invoke_result_t<Function, Args...>>>
    call(Function& func, std::string& result, std::tuple<Args...> tup)
    {
       callHelper(func, std::make_index_sequence<sizeof...(Args)>{}, std::move(tup));
        // do something with ret
        result = "invoke return non-void. OK";
    }

    template <typename Function, size_t... I, typename...Args>
    static std::invoke_result_t<Function, Args...>
    callHelper(Function& func, const std::index_sequence<I...>&, std::tuple<Args...> tup)
    {
        return std::invoke(func, std::forward<Args>(std::get<I>(tup))...);
        //return std::forward<Function>(func)(std::forward<Args>(std::get<I + 1>(tup))...);
        //return func(std::move(std::get<I>(tup))...);
    }

    template <typename Function, typename Object, typename... Args>
    static std::enable_if_t<std::is_void_v<std::invoke_result_t<Function, Object, Args...>>>
    callMember(const Function& func, Object* object, std::string& result, std::tuple<Args...> tup)
    {
        callMemHelper(func, object, typename std::make_index_sequence<sizeof...(Args)>{}, std::move(tup));
        result = "call member return void. OK";
    }

    template <typename Function, typename Object, typename... Args>
    static std::enable_if_t<!std::is_void_v<std::invoke_result_t<Function, Object, Args...>>>
    callMember(const Function& func, Object* object, std::string& result, std::tuple<Args...> tup)
    {
       callMemHelper(func, object, typename std::make_index_sequence<sizeof...(Args)>{}, std::move(tup));
        // result = msgpack_codec::pack_args_str("OK", ret);  // OK need enum
        result = "call member return non void. OK";
    }

    template <typename Function, typename Object, size_t... I, typename...Args>
    static std::invoke_result_t<Function, Object, Args...>
    callMemHelper(const Function& func, Object object, const std::index_sequence<I...>&, std::tuple<Args...> tup)
    {
        return std::invoke(func, object, std::forward<Args>(std::get<I>(tup))...);
    }
};


#endif //TEMPLATEINSTANCE_RPCINVOKER_H
