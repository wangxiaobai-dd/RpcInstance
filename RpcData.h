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
        CALL_SESSION_DATA = 2,
        CALL_MAX
    };

    struct RpcMsg
    {
        unsigned int rpcid = 0;
        unsigned int type = 0;
        unsigned char data[0];
        unsigned int size = 0;

        unsigned int getSize()
        {
            return sizeof(*this) + size * sizeof(data[0]);
        }
    };

    struct RpcResultMsg
    {

    };


    struct stTestDataCmd
    {
        int userID = 0;
        int unionID = 0;
        int value = 0;
    };

};

#endif //TEMPLATEINSTANCE_RPCDATA_H
