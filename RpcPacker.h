//
// Created by wangyubo on 2022/9/14.
//

#ifndef TEMPLATEINSTANCE_RPCPACKER_H
#define TEMPLATEINSTANCE_RPCPACKER_H

#include <tuple>

struct RpcPacker
{
    // pack
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
        auto size = 0;
        ((size += doSave(std::get<I>(tupArgs))), ...);
        return size;
    }

    template <typename T>
    static unsigned int doSave(T arg)
    {
        // do serialize
        return sizeof(arg);
    }

    // unpack
    template <typename... Args> // t: tuple<int, string, int>
    static void unpackArgs(unsigned char* data, std::tuple<Args...>& tupArgs)
    {
        

    }

    
    

};





#endif //TEMPLATEINSTANCE_RPCPACKER_H
