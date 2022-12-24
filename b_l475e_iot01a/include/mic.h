#ifndef __MIC_H__
#define __MIC_H__

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include "stm32l475e_iot01_audio.h"
#include "mbed.h"

class Mic {
public:
    Mic(EventQueue &event_queue, Mutex &mutex, ConditionVariable &cond, char* buffer);
    ~Mic();

    void target_audio_buffer_full();
    void BSP_AUDIO_IN_HalfTransfer_CallBack(uint32_t Instance);
    void BSP_AUDIO_IN_TransferComplete_CallBack(uint32_t Instance);
    void BSP_AUDIO_IN_Error_CallBack(uint32_t Instance);
    void print_stats();
    void start_recording();
    void setup();
    void start();

private:
    EventQueue *event_queue_;
    InterruptIn *button_;
    Thread thread_;
    Mutex *mutex_;
    ConditionVariable *cond_;

    char *buffer_;

    uint16_t PCM_Buffer[PCM_BUFFER_LEN / 2];
    BSP_AUDIO_Init_t MicParams;

    // Place to store final audio (alloc on the heap), here two seconds...
    size_t TARGET_AUDIO_BUFFER_NB_SAMPLES = AUDIO_SAMPLING_FREQUENCY * 1.8;
    int16_t *TARGET_AUDIO_BUFFER = (int16_t*)calloc(TARGET_AUDIO_BUFFER_NB_SAMPLES, sizeof(int16_t));
    size_t TARGET_AUDIO_BUFFER_IX = 0;

    // we skip the first 50 events (100 ms.) to not record the button click
    size_t SKIP_FIRST_EVENTS = 50;
    size_t half_transfer_events = 0;
    size_t transfer_complete_events = 0;

};

#endif // __MIC_H__