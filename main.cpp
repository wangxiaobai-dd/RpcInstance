
#include "RpcBase.h"

void test(int a, std::string b)
{
    std::cout << "test" << " " << a << b << std::endl;
}

struct TestMem
{
    void test2()
    {
        std::cout << "test2" << std::endl;
    }
};

int main()
{
    RpcBase rpc;
  //  rpc.bind(RPC::CALL_SESSION, test);
    TestMem mem;
    rpc.bind(RPC::CALL_SESSION, &TestMem::test2, &mem);

    //rpc.call(RPC::CALL_SESSION);
    rpc.testLocalCall(RPC::CALL_SESSION);
    //rpc.call();
}