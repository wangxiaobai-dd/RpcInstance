
#include "RpcBase.h"

void test(int a, std::string b)
{
    std::cout << "test" << " " << a << b << std::endl;
}

void testData(const stDataBaseCmd* data, unsigned int size)
{
    stTestDataCmd* cmd = (stTestDataCmd*) data;
    std::cout << cmd->userID << std::endl;
}

struct TestMem
{
    void test2()
    {
        std::cout << "test2" << std::endl;
    }
};

//todo exception

int main()
{
    RpcBase rpc;
    rpc.bind(RPC::CALL_SESSION, test);
    TestMem mem;
    rpc.bind(RPC::CALL_SESSION_NORMAL, &TestMem::test2, &mem);

    //rpc.call(RPC::CALL_SESSION);
    rpc.testLocalCall(RPC::CALL_SESSION_NORMAL);
    //rpc.call();

    rpc.bind(RPC::CALL_SESSION_DATA, testData);
    stTestDataCmd dataCmd;
    dataCmd.userID = 100;
    CREATE_MSG(RpcMsg, send, MAX_MSG_LEN)
    send->rpcid = 1;
    send->type = CALL_SESSION_DATA;
    std::memcpy(send->data, &dataCmd, sizeof(dataCmd));
    send->size = sizeof(dataCmd);
    rpc.testLocalDataCall(send);
}