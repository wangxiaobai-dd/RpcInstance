//
// Created by Meepo on 2022/8/10.
//

#ifndef TEMPLATEINSTANCE_RPCSESSION_H
#define TEMPLATEINSTANCE_RPCSESSION_H

#include "RpcBase.h"

class RpcSession : public RpcBase
{
    template<typename Function>
    void bindSept(CALL_TYPE type, Function &&func)
    {

    }

    template<typename Function>
    void bindUnion(CALL_TYPE type, Function &&func)
    {

    }

    template<typename Function>
    void bindUser(CALL_TYPE type, Function &&func)
    {

    }

};

// Test
struct Sept
{
    void addSeptMoney() {}
};

struct User
{
    void getLevel() {}
};

#endif //TEMPLATEINSTANCE_RPCSESSION_H
