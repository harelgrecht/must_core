#include "real_time_thread.hpp"

RealTimeThread::RealTimeThread(void (*routine)(void *), void *arg) : thread_(routine) {
    pthread_t nativeHandle = thread_.native_handle();

    sched_param schedParam;
    schedParam.sched_priority = MAX_PRIORETY;
    int ret = pthread_setschedparam(nativeHandle, THREAD_POLICY, &schedParam);
    if (ret != 0) {
        Logger.getInstance().Log(LogLevel.ERROR, "Failed to set scheduling parameters: " << strerror(ret));
    } else {
        Logger.getInstance().Log(LogLevel.INFO, "Thread scheduling parameters set successfully.");
    }
}

RealTimeThread::~RealTimeThread() {
    if(thread_.joinable()) {
        thread_.join();
    }
}