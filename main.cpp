
#include "RpcBase.h"
#include "RpcPacker.h"

int test(int a, std::string b)
{
    std::cout << "test:" << a << " " << b << std::endl;
    return 0;
}

void testNoParam()
{
    std::cout << "testNoParam:" << std::endl;
}

int testNoParam2()
{
    std::cout << "testNoParam2:" << std::endl;
    return 1;
}

void testData(const stDataBaseCmd* data, size_t size)
{
    stTestDataCmd* cmd = (stTestDataCmd*) data;
    std::cout << "testData, userID:" << cmd->userID << std::endl;
}

struct TestMem
{
    void test2()
    {
        std::cout << "test2:" << std::endl;
    }

    void testMemData(const stDataBaseCmd* data, size_t size)
    {
        stTestDataCmd* cmd = (stTestDataCmd*) data;
        std::cout << "testData, userID:" << cmd->userID << std::endl;
    }
};

void testCB(std::string_view data)
{
    std::cout << "testCB" << ":" << data << std::endl;
}

//todo exception

int main()
{
    RpcBase rpc;

    // 普通函数
    rpc.bind(RPC::CALL_SESSION, test);
    rpc.testLocalCall(RPC::CALL_SESSION, 1, "meepo");

    // 成员函数
    TestMem mem;
    rpc.bind(RPC::CALL_SESSION_MEM, &TestMem::test2, &mem);
    rpc.testLocalCall(RPC::CALL_SESSION_MEM);

    // 使用 struct data
    rpc.bind(RPC::CALL_SESSION_DATA, testData);
    stTestDataCmd dataCmd;
    dataCmd.userID = 100;

    CREATE_MSG(RpcMsg, send, MAX_MSG_LEN)
    send->rpcid = 1;
    send->type = RPC::CALL_SESSION_DATA;
    std::memcpy(send->data, &dataCmd, sizeof(dataCmd));
    send->size = sizeof(dataCmd);
    rpc.testLocalDataCall(send);

    // 无参数
    rpc.bind(RPC::CALL_SESSION_NO_PARAM, testNoParam2);
    rpc.testLocalCall(RPC::CALL_SESSION_NO_PARAM);

    // 成员函数 structData
    rpc.bind(RPC::CALL_SESSION_MEM_DATA, &TestMem::testMemData, &mem);
    send->type = RPC::CALL_SESSION_MEM_DATA;
    rpc.testLocalDataCall(send);

    // 回调
    rpc.testRegisterCB(rpc.getRpcId(), testCB);

    CREATE_MSG(RpcResultMsg, retMsg, MAX_MSG_LEN)
    retMsg->state = 1;
    retMsg->rpcid = rpc.getRpcId();
    std::string resultData = "meepo";
    std::memcpy(retMsg->data, resultData.data(), resultData.size());
    retMsg->size = resultData.size();
    rpc.dealResult(retMsg);

    // call test
    rpc.callCb(RPC::CALL_SESSION_CB, testCB, 1);
    rpc.callCb(RPC::CALL_SESSION_CB, [](std::string_view data){});

    rpc.call(RPC::CALL_SESSION, 1, "meepocall");

    // snowflake
    std::shared_ptr<SnowflakeIdWorker> sfIdWorker = std::make_shared<SnowflakeIdWorker>(1, 1);
    std::cout << "genId:" << sfIdWorker->genId() << std::endl;

    // rpcpacker
    unsigned char buf[1000] = {0};
    std::cout << "packArgs,size:" << RpcPacker::packArgs(buf, 1, 1, 1)  << std::endl;

    std::tuple<int, std::string, bool> tup;
    std::tuple<> emptyTup;

    using argsType = typename function_traits<decltype(test)>::args_tuple;
    argsType funcTup;
    RpcPacker::unpackArgs(buf, funcTup);
    printParam(funcTup);
}