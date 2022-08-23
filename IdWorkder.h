//
// Created by Meepo on 2022/8/22.
//

#ifndef TEMPLATEINSTANCE_IDWORKDER_H
#define TEMPLATEINSTANCE_IDWORKDER_H

#include <chrono>

class SnowflakeIdWorker
{
    /*https://zhuanlan.zhihu.com/p/85837641
     * https://cloud.tencent.com/developer/article/1772047
   * snowflake
   * 0 41bit时间戳(69年) 10bit工作机器id(1024) 12bit序列号(4096)
   */
    std::uint64_t genID()
    {
        auto timestamp = genTimeStamp();
        if(lastTimeStamp == timestamp)

    }



private:
    std::uint64_t genTimeStamp()
    {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    }

    std::uint64_t epoch = 0;
    std::uint64_t lastTimeStamp = 0;
    std::uint64_t sequence = 0;
};


#endif //TEMPLATEINSTANCE_IDWORKDER_H
