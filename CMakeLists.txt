cmake_minimum_required(VERSION 3.20)
project(templateInstance)

set(CMAKE_CXX_STANDARD 14)

add_executable(templateInstance main.cpp RpcData.h RpcBase.h RpcRouter.h RpcSession.h RpcScene.h)
