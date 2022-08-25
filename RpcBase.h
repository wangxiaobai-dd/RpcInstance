//
// Created by Meepo on 2022/8/10.
//

#ifndef TEMPLATEINSTANCE_RPCBASE_H
#define TEMPLATEINSTANCE_RPCBASE_H

#include <unordered_map>
#include <iostream>
#include "RpcData.h"
#include "RpcInvoker.h"
#include "RpcCallback.h"
#include "IdWorer.h"

const constexpr size_t NONE_TIMEOUT = 0;
const constexpr size_t MAX_MSG_LEN = 1024 * 64;

template <class T>
inline void constructBuffer(T* ptr)
{
    new(static_cast<void*>(ptr)) T();
}

#define CREATE_MSG(msg, name, len)\
        char buffer##name[len] = {0}; \
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

    virtual void sendMsg(RpcMsg* msg, unsigned int size){ std::cout << "send" << std::endl; }


    /**************************************** server 注册 ******************************************/
public:

    void route(RpcMsg* msg)
    {
        if(!msg)
            return;
        auto iter = functionMap.find(CALL_TYPE(msg->type));
        if(iter == functionMap.end())
            return;
        std::string result;
        std::string cmd(reinterpret_cast<char*>(&msg->data[0]), msg->size);
        iter->second(cmd.c_str(), cmd.size(), result);
    }

    // 普通函数 参数未使用 stDataBaseCmd
    template <typename Function>
    std::enable_if_t<!std::is_same_v<typename function_traits<Function>::argType0, const stDataBaseCmd*>>
    bind(CALL_TYPE type, Function func)
    {
        functionMap[type] = std::bind(RpcInvoker::Invoker<Function>::apply, std::move(func), std::placeholders::_1,
                                      std::placeholders::_2, std::placeholders::_3);
    }

    // 成员函数 参数未使用 stDataBaseCmd
    template <typename Function, typename Object>
    std::enable_if_t<!std::is_same_v<typename function_traits<Function>::argType0, const stDataBaseCmd*>>
    bind(CALL_TYPE type, Function func, Object* object)
    {
        functionMap[type] = std::bind(RpcInvoker::Invoker<Function>::template applyMember<Object>, std::move(func),
                                      object,
                                      std::placeholders::_1,
                                      std::placeholders::_2, std::placeholders::_3);
    }

    // 普通函数 参数使用 stDataBaseCmd
    template <typename Function>
    std::enable_if_t<std::is_same_v<typename function_traits<Function>::argType0, const stDataBaseCmd*>>
    bind(CALL_TYPE type, Function func)
    {
        functionMap[type] = std::bind(RpcInvoker::Invoker<Function>::applyUsingCmd, std::move(func),
                                      std::placeholders::_1,
                                      std::placeholders::_2, std::placeholders::_3);
    }

    // 成员函数 参数使用 stDataBaseCmd
    template <typename Function, typename Object>
    std::enable_if_t<std::is_same_v<typename function_traits<Function>::argType0, const stDataBaseCmd*>>
    bind(CALL_TYPE type, Function func, Object* object)
    {
        functionMap[type] = std::bind(RpcInvoker::Invoker<Function>::template applyMemUsingCmd<Object>, std::move(func),
                                      object,
                                      std::placeholders::_1,
                                      std::placeholders::_2, std::placeholders::_3);
    }

protected:
    std::unordered_map<CALL_TYPE, std::function<void(const char*, size_t, std::string&)>> functionMap; // function<data, dataSize, result>

/**************************************** client 调用 ******************************************/

public:

    template <typename... Args>
    void call(CALL_TYPE type, Args&& ... args)
    {
        rpcid = idWorker.genId();

        msgpack_codec codec;
        auto msg = codec.pack_args(std::forward<Args>(args)...);
        size_t size = msg.size();
        char* data = msg.release();
        CREATE_MSG(RpcMsg, send, MAX_MSG_LEN)
        SEND_MSG(data, size)
    }

    void call(CALL_TYPE type, stDataBaseCmd* cmd, size_t cmdSize)
    {
        if(!cmd)
            return;

        rpcid = idWorker.genId();

        CREATE_MSG(RpcMsg, send, MAX_MSG_LEN)
        send->rpcid = rpcid;
        send->type = type;
        std::memcpy(send->data, cmd, sizeof(*cmd));
        send->size = sizeof(*cmd);
        std::cout << "size call" << send->size << std::endl;
    }

    template <std::uint32_t TIMEOUT = NONE_TIMEOUT, typename... Args>
    void call(CALL_TYPE type, std::function<void(std::string_view)>& func, Args&&... args)
    {
        rpcid = idWorker.genId();

        auto cb = std::make_shared<RpcCB>(std::move(func), NONE_TIMEOUT);
        callbackMap.emplace(rpcid, cb);

        msgpack_codec codec;
        auto msg = codec.pack_args(std::forward<Args>(args)...);
        size_t size = msg.size();
        char* data = msg.release();
        CREATE_MSG(RpcMsg, send, MAX_MSG_LEN)
        SEND_MSG(data, size)
    }

    template <std::uint32_t TIMEOUT = NONE_TIMEOUT>
    void call(CALL_TYPE type, std::function<void(std::string_view)>& func, stDataBaseCmd* cmd, size_t cmdSize)
    {
        rpcid = idWorker.genId();

        auto cb = std::make_shared<RpcCB>(std::move(func), NONE_TIMEOUT);
        callbackMap.emplace(rpcid, cb);

        CREATE_MSG(RpcMsg, send, MAX_MSG_LEN)
        send->rpcid = rpcid;
        send->type = type;
        std::memcpy(send->data, cmd, sizeof(*cmd));
        send->size = sizeof(*cmd);
        std::cout << "size call" << send->size << std::endl;
    }

/**************************************** 测试开始 ******************************************/

    // 使用 stDataBaseCmd
    void testLocalDataCall(RpcMsg* msg)
    {
        if(!msg)
            return;
        auto iter = functionMap.find(CALL_TYPE(msg->type));
        if(iter == functionMap.end())
            return;
        std::string result;
        std::string cmd(reinterpret_cast<char*>(&msg->data[0]), msg->size);
        iter->second(cmd.c_str(), cmd.size(), result);
        std::cout << "result:" << result << "\n" << std::endl;
    }

    // 使用参数包
    template <typename... Args>
    void testLocalCall(CALL_TYPE type, Args&& ... args)
    {
        auto iter = functionMap.find(type);
        if(iter == functionMap.end())
            return;
        msgpack_codec codec;
        auto msg = codec.pack_args(std::forward<Args>(args)...);
        size_t size = msg.size();
        char* data = msg.release();
        std::string result;
        iter->second(data, size, result);
        std::cout << "result:" << result << "\n" << std::endl;
    }

    // 回调
    void testCallback(std::uint64_t rpcid, const char* data)
    {
        auto iter = callbackMap.find(rpcid);
        if(iter == callbackMap.end())
            return;
        iter->second->callback(data);
    }

/**************************************** 测试结束 ******************************************/

protected:
    std::uint64_t rpcid = 0;
    std::unordered_map<std::uint64_t, std::shared_ptr<RpcCB>> callbackMap;
    IdWorker idWorker;
};


#endif //TEMPLATEINSTANCE_RPCBASE_H
