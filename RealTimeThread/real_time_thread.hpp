#pragma once

#include <pthread.h>
#include <thread>
#include <sched.h>
#include <iostream>
#include <cstring>
#include <functional> 
#include "../LogHandler/logger.hpp"

constexpr int THREAD_POLICY = SCHED_FIFO;

class RealTimeThread {
    public:
        RealTimeThread(std::function<void(void*)>, void *arg);
        ~RealTimeThread();

    private:
        std::thread thread_;
        
        const int MAX_PRIORETY = sched_get_priority_max(THREAD_POLICY);
};
