#ifndef __SOCKET_INIT_H__
#define __SOCKET_INIT_H__

#include "mbed.h"
#include "socket_module.h"

class SocketInit {
public:
    SocketInit(Semaphore &sem, Mutex &mutex, ConditionVariable &cond, char* buffer);
    ~SocketInit();
    void start();

private:
    void setupSend();

private:
    SocketModule *socket_;
    Thread thread_;
    Semaphore *sem_;
    Mutex *mutex_;
    ConditionVariable *cond_;
    char* buffer_;
};

#endif