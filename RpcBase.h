//
// Created by Meepo on 2022/8/10.
//

#ifndef TEMPLATEINSTANCE_RPCBASE_H
#define TEMPLATEINSTANCE_RPCBASE_H

#include <unordered_map>
#include <iostream>
#include "RpcData.h"
#include "RpcRouter.h"

const constexpr size_t DEFAULT_TIMEOUT = 5000;
const constexpr size_t MAX_MSG_LEN = 1024 * 64;

template <class T>
inline void constructBuffer(T* ptr)
{
    new(static_cast<void*>(ptr)) T();
}

#define CREATE_MSG(msg, name, len)\
        char buffer##name[len] = {0};\
        msg* name = (msg*)buffer##name;\
        constructBuffer(name);

#define SEND_MSG(data, size)\
            send->rpcid = rpcid;\
            send->type = type;\
            std::memcpy(send->data, data, size);\
            send->size = size;\
            sendMsg(send, send->getSize());

using namespace RPC;

class RpcBase
{
public:

    RpcBase(){}

    virtual ~RpcBase(){}

    virtual void sendMsg(RpcMsg* msg, unsigned int size){ std::cout << "send" << std::endl; };

    /**************************************** 注册 ******************************************/
public:
    template <typename Function>
    void bind(CALL_TYPE type, Function func)
    {
        functionMap[type] = std::bind(&RpcRouter::invoker<Function>::apply, std::move(func), std::placeholders::_1,
                                      std::placeholders::_2, std::placeholders::_3);
    }

private:
    std::unordered_map<CALL_TYPE, std::function<void(const char*, size_t, std::string&)>> functionMap;


    /**************************************** 调用 ******************************************/
public:

    //  template <typename... Args>
    // void call(CALL_TYPE type, Args&& ... args);


    template <typename... Args>
    void call(CALL_TYPE type, Args&& ... args)
    {
        rpcid++;
        msgpack_codec codec;
        auto msg = codec.pack_args((int)type, std::forward<Args>(args)...);

        size_t size = msg.size();
        char* data = msg.release();

        CREATE_MSG(RpcMsg, send, MAX_MSG_LEN)

        SEND_MSG(data, size)
    }

    template <typename... Args>
    void testLocalCall(CALL_TYPE type, Args&& ... args)
    {
        auto iter = functionMap.find(type);
        if(iter == functionMap.end())
            return;
        msgpack_codec codec;
        auto msg = codec.pack_args((int)type, std::forward<Args>(args)...); // todo type 不用打包也可以
        size_t size = msg.size();
        char* data = msg.release();
        std::string result;
        iter->second(data, size, result);
    }

private:
    static std::uint64_t rpcid;

};

std::uint64_t RpcBase::rpcid = 0;


#endif //TEMPLATEINSTANCE_RPCBASE_H
/*
 * https://www.cnblogs.com/qicosmos/p/4325949.html
 * https://blog.csdn.net/lichao201005/article/details/124264766
 * https://zhuanlan.zhihu.com/p/335994370
 * https://www.cnblogs.com/abelian/p/6291854.html
 * */