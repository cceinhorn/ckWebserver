#pragma once

#include <unistd.h>
#include <sys/syscall.h>

namespace CurrentThread 
{
    extern __thread int t_cachedTid;        // 保存tid缓存  系统调用耗时  拿到后保存

    void cacheTid();

    inline int tid()        // 内联函数只在当前文件中起作用
    {
        if (__builtin_expect(t_cachedTid == 0, 0)) {    // __builtin_expect 是一种底层优化
            cacheTid();
        }
        return t_cachedTid;
    }
}