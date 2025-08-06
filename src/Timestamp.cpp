#include <cstddef>
#include <cstdint>
#include <time.h>
#include <sys/time.h>     // gettimeofday
#include <stdint.h>       // int64_t

#include "Timestamp.h"

Timestamp::Timestamp() : microSecondsSinceEpoch_(0) {}


Timestamp::Timestamp(int64_t SecondsSinceEpoch) : microSecondsSinceEpoch_(SecondsSinceEpoch) {}

// 静态 秒
// Timestamp Timestamp::now() 
// {
//     return Timestamp(time(NULL));   // 返回自1970年1月1日 00:00:00 起所经过的秒数
// }


Timestamp Timestamp::now() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);  // 使用 nullptr 更现代
    return Timestamp(static_cast<int64_t>(tv.tv_sec) * 1000000 + tv.tv_usec);
}

// 严格单调
// Timestamp Timestamp::now() {
//     struct timespec ts;
//     clock_gettime(CLOCK_REALTIME, &ts);  // 可改为 CLOCK_MONOTONIC 如需要单调递增
//     return Timestamp(static_cast<int64_t>(ts.tv_sec) * 1000000 + ts.tv_nsec / 1000);
// }


std::string Timestamp::toString() const 
{
    char buf[128] = {0};
    tm *tm_time = localtime(&microSecondsSinceEpoch_);
    snprintf(buf, 128, "%4d-%02d-%02d %02d:%02d:%02d",
            tm_time->tm_year + 1900,
            tm_time->tm_mon + 1,
            tm_time->tm_mday,
            tm_time->tm_hour,
            tm_time->tm_min,
            tm_time->tm_sec);
    
    return buf;     // const char* 到 std::string隐式转换
}

// #include <iostream>
// #include <cstdio>
// int main() {
//     int a = 1;
//     printf("% 2d\n", a);
//     std::cout << Timestamp::now().toString() << std::endl;
//     return 0;
// }

std::string Timestamp::toFormattedString(bool showMicroseconds) const
{
    char buf[64] = {0};
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
    // 使用localtime函数将秒数格式化成日历时间
    tm *tm_time = localtime(&seconds);
    if (showMicroseconds)
    {
        int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondsPerSecond);
        snprintf(buf, sizeof(buf), "%4d/%02d/%02d %02d:%02d:%02d.%06d",
                tm_time->tm_year + 1900,
                tm_time->tm_mon + 1,
                tm_time->tm_mday,
                tm_time->tm_hour,
                tm_time->tm_min,
                tm_time->tm_sec,
                microseconds);
    }
    else
    {
        snprintf(buf, sizeof(buf), "%4d/%02d/%02d %02d:%02d:%02d",
                tm_time->tm_year + 1900,
                tm_time->tm_mon + 1,
                tm_time->tm_mday,
                tm_time->tm_hour,
                tm_time->tm_min,
                tm_time->tm_sec);
    }
    return buf;
}