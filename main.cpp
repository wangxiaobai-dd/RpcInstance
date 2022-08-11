#include "RpcBase.h"

void test()
{
    std::cout << "test" << std::endl;
}

int main()
{
    RpcBase rpc;
    rpc.bind(RPC::CALL_SESSION, test);
}