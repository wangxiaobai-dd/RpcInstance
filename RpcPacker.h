//
// Created by wangyubo on 2022/9/14.
//

#ifndef TEMPLATEINSTANCE_RPCPACKER_H
#define TEMPLATEINSTANCE_RPCPACKER_H

#include <tuple>

/*
 *  This is a test frame for internal project.  Ignore it.
 */

struct RpcPacker
{
    /****************************** pack ******************************/

    template <typename... Args>
    static unsigned int packArgs(unsigned char* buffer, Args&& ...args)
    {
        return packTupleArgs(buffer, std::move(std::make_tuple(std::forward<Args>(args)...)));
    }

    template <typename... Args>
    static unsigned int packTupleArgs(unsigned char* buffer,  std::tuple<Args...> tupArgs)
    {
        return serializeArg(buffer, typename std::index_sequence_for<Args...>{}, std::move(tupArgs));
    }

    template <std::size_t... I, typename... Args>
    static unsigned int serializeArg(unsigned char* buffer, const std::index_sequence<I...>&, std::tuple<Args...> tupArgs)
    {
        unsigned int size = 0;
        ((size += doSave(std::get<I>(tupArgs))), ...);
        return size;
    }

    template <typename T>
    static unsigned int doSave(T arg)
    {
        // do serialize
        return sizeof(arg);
    }


    /****************************** unpack ******************************/
    template <typename... Args> // t: tuple<int, string, int>
    static void unpackArgs(unsigned char* data, std::tuple<Args...>& tupRet)
    {
        deserializeArg(data, typename std::index_sequence_for<Args...>{}, tupRet);
    }

    template <std::size_t... I, typename... Args>
    static void deserializeArg(unsigned char* data, const std::index_sequence<I...>&, std::tuple<Args...>& tupRet)
    {
        ((doLoad(data,std::get<I>(tupRet))), ...);
    }

    template <typename T>
    static void doLoad(unsigned char* data, T& ret)
    {
        // do deserialize
        if constexpr(std::is_same_v<T, int>)
        {
            std::cout << "typeid:int" << std::endl;
            ret = 100;
        }
        else if constexpr(std::is_same_v<T, std::string>)
        {
            ret = "meepo";
        }
        std::cout << "doLoad:" << typeid(T).name() << std::endl;
    }

};





#endif //TEMPLATEINSTANCE_RPCPACKER_H
