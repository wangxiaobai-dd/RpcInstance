//
// Created by Meepo on 2022/8/24.
//

#ifndef TEMPLATEINSTANCE_RPCCALLBACK_H
#define TEMPLATEINSTANCE_RPCCALLBACK_H

#include <memory>
#include <functional>

class RpcCB : public std::enable_shared_from_this<RpcCB> {
public:
    RpcCB(std::function<void(std::string_view)>& _cb, size_t _timeout) : cb(std::move(_cb)), timeout(_timeout) {}

    void callback(std::string_view data)
    {
        cb(data);
    }

    bool isTimeout(uint32_t now) const
    {
        if(!timeout)
            return false;
        return now > timeout;
    }

private:
    std::function<void(std::string_view)> cb;
    size_t timeout;
};

#endif //TEMPLATEINSTANCE_RPCCALLBACK_H
