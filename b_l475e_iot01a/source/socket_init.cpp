#include <cstdio>
#include "socket_init.h"

SocketInit::SocketInit(Semaphore &sem, Mutex &mutex, ConditionVariable &cond, char* buffer) :
    sem_(&sem), mutex_(&mutex), cond_(&cond), buffer_(buffer) {
    socket_ = new SocketModule(*sem_);
}

SocketInit::~SocketInit(){
    delete socket_;
}

void SocketInit::start(){
    socket_->run();
    printf("\r\nSocket Connected!\r\n");
    thread_.start(callback(this, &SocketInit::setupSend));
}

void SocketInit::setupSend(){
    mutex_->lock();
    while (true) {
        cond_->wait();
        socket_->send_http_request(buffer_);
        ThisThread::sleep_for(1ms);
    }
    mutex_->unlock();
}
