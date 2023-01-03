#pragma once

#include "../../../../../../Android/Sdk/ndk/21.4.7075529/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include/c++/v1/chrono"
#include "../../../../../../Android/Sdk/ndk/21.4.7075529/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include/android/log.h"

class Timer
{
public:
    Timer() : m_Tag("Timer")
    {
        m_StartTimepoint = std::chrono::high_resolution_clock::now();
    }
    Timer(std::string tag) : m_Tag(tag)
    {
        m_StartTimepoint = std::chrono::high_resolution_clock::now();
    }

    ~Timer()
    {
        stop();
    }

    void stop()
    {
        auto endTimepoint = std::chrono::high_resolution_clock::now();
        long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
        long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

        long durationUs = end - start;
        long durationMs = durationUs * 0.001;
        __android_log_print(ANDROID_LOG_DEBUG, m_Tag.c_str(), "%d us (%f ms)", durationUs, durationMs);
    }
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
    std::string m_Tag;
};
