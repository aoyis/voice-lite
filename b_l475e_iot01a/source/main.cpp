#include <cstdio>
#include <string>
#include "mbed.h"
#include "mbed-trace/mbed_trace.h"
#include "stm32l475e_iot01_audio.h"
#include "mic.h"
#include "socket_init.h"

EventQueue event_queue;
Semaphore socket_sem(1);
Mutex mutex_socket;
ConditionVariable cond_socket(mutex_socket);
char socket_buffer[1024];

SocketInit* socket_init;
Mic* mic;

void BSP_AUDIO_IN_HalfTransfer_CallBack(uint32_t Instance) {
    mic->BSP_AUDIO_IN_HalfTransfer_CallBack(Instance);
}

void BSP_AUDIO_IN_TransferComplete_CallBack(uint32_t Instance) {
    mic->BSP_AUDIO_IN_TransferComplete_CallBack(Instance);
}

void BSP_AUDIO_IN_Error_CallBack(uint32_t Instance) {
    mic->BSP_AUDIO_IN_Error_CallBack(Instance);
}

int main() {

#ifdef MBED_CONF_MBED_TRACE_ENABLE
    mbed_trace_init();
#endif 

    mic = new Mic(event_queue, mutex_socket, cond_socket, socket_buffer);
    mic->start();
    socket_init = new SocketInit(socket_sem, mutex_socket, cond_socket, socket_buffer);
    socket_init->start();

    while(true) 

    return 0;
}
