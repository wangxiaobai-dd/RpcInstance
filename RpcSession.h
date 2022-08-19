//
// Created by Meepo on 2022/8/10.
//

#ifndef TEMPLATEINSTANCE_RPCSESSION_H
#define TEMPLATEINSTANCE_RPCSESSION_H

#include "RpcBase.h"

// todo

template <typename Function> struct SessInvoker;

class RpcSession : public RpcBase
{
public:
    template<typename Function>
    void bindSept(CALL_TYPE type, Function func)
    {

    }

    template<typename Function>
    void bindUser(CALL_TYPE type, Function func)
    {
        functionMap[type] = std::bind(SessInvoker<Function>::applyUser, std::move(func), std::placeholders::_1,
                                      std::placeholders::_2, std::placeholders::_3);
    }
};

template <typename Function>
struct SessInvoker : public RpcInvoker::Invoker<Function>
{
    static inline void applyUser(const Function& func, const char* data, size_t size, std::string& result)
    {
        // xxx cmd = (xxx)data;
        // cmd->userID;
        //SessUser* user = getUserByID(userID);
        //if(user)
        //   RpcRouter::call(func, user, data, size);
    }
    //callSessUser(type, userID, 100, 1);
    //callSessSept(type, septID);

    static inline void applySept(const Function& func, const char* data, size_t size, std::string& result)
    {}
};



#endif //TEMPLATEINSTANCE_RPCSESSION_H
