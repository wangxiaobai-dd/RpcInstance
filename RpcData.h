//
// Created by Meepo on 2022/8/10.
//

#ifndef TEMPLATEINSTANCE_RPCDATA_H
#define TEMPLATEINSTANCE_RPCDATA_H

namespace RPC
{
    enum CALL_TYPE
    {
        CALL_SESSION = 1,
        CALL_SESSION_MEM = 2,
        CALL_SESSION_DATA = 3,
        CALL_SESSION_NO_PARAM = 4,
        CALL_SESSION_MEM_DATA = 5,
        CALL_MAX
    };

    struct RpcMsg
    {
        unsigned int rpcid = 0;
        unsigned int type = 0;
        unsigned int size = 0;
        unsigned char data[0];

        unsigned int getSize()
        {
            return sizeof(*this) + size * sizeof(data[0]);
        }
    };

    struct RpcResultMsg
    {
        unsigned int rpcid = 0;
        unsigned char state = 0;    // 0:fail 1:success
        unsigned int size = 0;
        unsigned char data[0];

        unsigned int getSize()
        {
            return sizeof(*this) + size * sizeof(data[0]);
        }
    };


    struct stDataBaseCmd
    {

    };

    struct stTestDataCmd : stDataBaseCmd
    {
        int userID = 0;
        int unionID = 0;
        int value = 0;
    };

};

// Test
struct Sept
{
    void addSeptMoney(int num) {}
};

struct SessUser
{
    SessUser(int _userID)
    {
        userID = _userID;
    }
    int userID = 0;
    int getLevel() { return 0; }
};

SessUser* getUserByID(int userID)
{
    return new SessUser(userID);
}

#endif //TEMPLATEINSTANCE_RPCDATA_H
