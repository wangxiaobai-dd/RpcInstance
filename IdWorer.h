//
// Created by Meepo on 2022/8/22.
//

#ifndef TEMPLATEINSTANCE_IDWORER_H
#define TEMPLATEINSTANCE_IDWORER_H

#include <chrono>
#include <iostream>

class IdWorker
{
public:
    IdWorker(){}
    ~IdWorker(){}
    virtual std::uint64_t genId()
    {
        curId = (curId & std::numeric_limits<std::uint64_t>::max()) + 1;
        return curId;
    }
    std::uint64_t curId = 0;
};

class SnowflakeIdWorker : public IdWorker
{
public:
    SnowflakeIdWorker() = delete;

    virtual ~SnowflakeIdWorker(){}

    SnowflakeIdWorker(std::uint64_t _workerId, std::uint64_t _dataCenterId) : workerId(_workerId),
                                                                              dataCenterId(_dataCenterId)
    {
        if(workerId > maxWorkerId)
            std::cout << "workderId overflow" << std::endl;
        if(dataCenterId > maxDataCenterId)
            std::cout << "dataCenterId overflow" << std::endl;
    }

    /*
     * 0 41bit时间戳(69年) 5bit工作机器id 5bit数据中心 12bit序列号(4096)
     */
    std::uint64_t genId() override
    {
        auto timestamp = genTimeStamp();
        if(lastTimeStamp == timestamp)
        {
            sequence = (sequence + 1) & sequenceMask;
            if(sequence == 0)
                timestamp += 1;
        } else
        {
            sequence = 0;
        }
        lastTimeStamp = timestamp;
        return ((timestamp - baseEpoch) << timestampLeftShift)
               | (dataCenterId << dataCenterIdShift)
               | (workerId << workerIdShift)
               | sequence;
    }

protected:
    virtual std::uint64_t genTimeStamp()
    {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    }

    std::uint64_t baseEpoch = 1660924800000;    // 基准 2022-08-20 00:00:00
    std::uint64_t workerIdBits = 5;             // 机器id 所占位数
    std::uint64_t dataCenterIdBits = 5;               // 数据id 所占位数
    std::uint64_t maxWorkerId = -1u ^ (-1u << workerIdBits); // 支持的最大机器id
    std::uint64_t maxDataCenterId = -1u ^ (-1u << dataCenterIdBits); // 支持的最大数据标识id
    std::uint64_t sequenceBits = 12;            // 序列在id中占的位数
    std::uint64_t workerIdShift = sequenceBits; // 机器ID向左移12位
    std::uint64_t dataCenterIdShift = sequenceBits + workerIdBits; // 数据标识id向左移17位(12+5)
    std::uint64_t timestampLeftShift = sequenceBits + workerIdBits + dataCenterIdBits; // 时间截向左移22位(5+5+12)
    std::uint64_t sequenceMask = -1L ^ (-1L << sequenceBits);
    std::uint64_t workerId = 0;                 // 工作机器ID(0~31)
    std::uint64_t dataCenterId = 0;             // 数据中心ID(0~31)
    std::uint64_t sequence = 0;                 // 毫秒内序列(0~4095)
    std::uint64_t lastTimeStamp = 0;            // 上次生成ID的时间截
};


#endif //TEMPLATEINSTANCE_IDWORER_H
