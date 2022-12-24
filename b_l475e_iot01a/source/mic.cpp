#include "mic.h"
#include "stm32l475e_iot01_audio.h"
#include <string.h>
#include <cstddef>
#include <cstdio>

Mic::Mic (EventQueue &event_queue, Mutex &mutex, ConditionVariable &cond, char* buffer) :
    event_queue_(&event_queue), mutex_(&mutex), cond_(&cond), buffer_(buffer) {
}

Mic::~Mic() {
}

// callback that gets invoked when TARGET_AUDIO_BUFFER is full
void Mic::target_audio_buffer_full() {
    // pause audio stream
    int32_t ret = BSP_AUDIO_IN_Pause(AUDIO_INSTANCE);
    if (ret != BSP_ERROR_NONE) {
        printf("Error Audio Pause (%d)\n", ret);
    }
    else {
        printf("Done recording.\n");
    }

    // create WAV file
    size_t wavFreq = AUDIO_SAMPLING_FREQUENCY;
    size_t dataSize = (TARGET_AUDIO_BUFFER_NB_SAMPLES * 2);
    size_t fileSize = 44 + (TARGET_AUDIO_BUFFER_NB_SAMPLES * 2);

    unsigned int wav_header[44] = {
        0x52, 0x49, 0x46, 0x46, // RIFF
        fileSize & 0xff, (fileSize >> 8) & 0xff, (fileSize >> 16) & 0xff, (fileSize >> 24) & 0xff,
        0x57, 0x41, 0x56, 0x45, // WAVE
        0x66, 0x6d, 0x74, 0x20, // fmt
        0x10, 0x00, 0x00, 0x00, // length of format data
        0x01, 0x00, // type of format (1=PCM)
        0x01, 0x00, // number of channels
        wavFreq & 0xff, (wavFreq >> 8) & 0xff, (wavFreq >> 16) & 0xff, (wavFreq >> 24) & 0xff,
        0x00, 0x7d, 0x00, 0x00, // 	(Sample Rate * BitsPerSample * Channels) / 8
        0x02, 0x00, 0x10, 0x00,
        0x64, 0x61, 0x74, 0x61, // data
        dataSize & 0xff, (dataSize >> 8) & 0xff, (dataSize >> 16) & 0xff, (dataSize >> 24) & 0xff,
    };

    printf("Total complete events: %u, index is %u\n", transfer_complete_events, TARGET_AUDIO_BUFFER_IX);

    // print both the WAV header and the audio buffer in HEX format to serial
    // you can use the script in `hex-to-buffer.js` to make a proper WAV file again
    printf("WAV file:\n");
    for (size_t ix = 0; ix < 44; ix++) {
        printf("%02x", wav_header[ix]);
    }
   
    uint8_t *buf = (uint8_t*)TARGET_AUDIO_BUFFER;
    for (size_t ix = 0; ix < TARGET_AUDIO_BUFFER_NB_SAMPLES * 2; ix = ix + 100) {
        mutex_->lock();
        int len = 0;
        for (size_t j = 0; j < 100; j++) {
            printf("%02x", buf[ix + j]);
            len += sprintf(buffer_ + len, "%02x", buf[ix + j]);
        }
        cond_->notify_all();
        mutex_->unlock();
    }
    printf("\n");
}

void Mic::BSP_AUDIO_IN_HalfTransfer_CallBack(uint32_t Instance) {
    half_transfer_events++;
    if (half_transfer_events < SKIP_FIRST_EVENTS) return;

    uint32_t buffer_size = PCM_BUFFER_LEN / 2; /* Half Transfer */
    uint32_t nb_samples = buffer_size / sizeof(int16_t); /* Bytes to Length */

    if ((TARGET_AUDIO_BUFFER_IX + nb_samples) > TARGET_AUDIO_BUFFER_NB_SAMPLES) {
        return;
    }

    /* Copy first half of PCM_Buffer from Microphones onto Fill_Buffer */
    memcpy(((uint8_t*)TARGET_AUDIO_BUFFER) + (TARGET_AUDIO_BUFFER_IX * 2), PCM_Buffer, buffer_size);
    TARGET_AUDIO_BUFFER_IX += nb_samples;

    if (TARGET_AUDIO_BUFFER_IX >= TARGET_AUDIO_BUFFER_NB_SAMPLES) {
        // _event_queue->call(callback(this, &MyAudio::print_stats));
        event_queue_->call(callback(this, &Mic::target_audio_buffer_full));
        return;
    }
}

void Mic::BSP_AUDIO_IN_TransferComplete_CallBack(uint32_t Instance) {
    transfer_complete_events++;
    if (transfer_complete_events < SKIP_FIRST_EVENTS) return;

    uint32_t buffer_size = PCM_BUFFER_LEN / 2; /* Half Transfer */
    uint32_t nb_samples = buffer_size / sizeof(int16_t); /* Bytes to Length */

    if ((TARGET_AUDIO_BUFFER_IX + nb_samples) > TARGET_AUDIO_BUFFER_NB_SAMPLES) {
        return;
    }

    /* Copy second half of PCM_Buffer from Microphones onto Fill_Buffer */
    memcpy(((uint8_t*)TARGET_AUDIO_BUFFER) + (TARGET_AUDIO_BUFFER_IX * 2),
        ((uint8_t*)PCM_Buffer) + (nb_samples * 2), buffer_size);
    TARGET_AUDIO_BUFFER_IX += nb_samples;

    if (TARGET_AUDIO_BUFFER_IX >= TARGET_AUDIO_BUFFER_NB_SAMPLES) {
        // _event_queue->call(callback(this, &MyAudio::print_stats));
        event_queue_->call(callback(this, &Mic::target_audio_buffer_full));
        return;
    }
}

void Mic::BSP_AUDIO_IN_Error_CallBack(uint32_t Instance) {
    printf("BSP_AUDIO_IN_Error_CallBack\n");
}

void Mic::print_stats() {
    printf("Half %u, Complete %u, IX %u\n", half_transfer_events, transfer_complete_events, TARGET_AUDIO_BUFFER_IX);
}

void Mic::start_recording() {
    int32_t ret;
    uint32_t state;

    ret = BSP_AUDIO_IN_GetState(AUDIO_INSTANCE, &state);
    if (ret != BSP_ERROR_NONE) {
        printf("Cannot start recording: Error getting audio state (%d)\n", ret);
        return;
    }
    if (state == AUDIO_IN_STATE_RECORDING) {
        printf("Cannot start recording: Already recording\n");
        return;
    }

    // reset audio buffer location
    TARGET_AUDIO_BUFFER_IX = 0;
    transfer_complete_events = 0;
    half_transfer_events = 0;

    ret = BSP_AUDIO_IN_Record(AUDIO_INSTANCE, (uint8_t *) PCM_Buffer, PCM_BUFFER_LEN);
    if (ret != BSP_ERROR_NONE) {
        printf("Error Audio Record (%d)\n", ret);
        return;
    }
    else {
        printf("Start Recording...\n");
    }
}


void Mic::setup() {
    printf("Hello from the B-L475E-IOT01A microphone (MP34DT01).\n");

    if (!TARGET_AUDIO_BUFFER) {
        printf("Failed to allocate TARGET_AUDIO_BUFFER buffer\n");
        return;
    }

    // set up the microphone
    MicParams.BitsPerSample = 16;
    MicParams.ChannelsNbr = AUDIO_CHANNELS;
    MicParams.Device = AUDIO_IN_DIGITAL_MIC1;
    MicParams.SampleRate = AUDIO_SAMPLING_FREQUENCY;
    MicParams.Volume = 32;

    int32_t ret = BSP_AUDIO_IN_Init(AUDIO_INSTANCE, &MicParams);

    if (ret != BSP_ERROR_NONE) {
        printf("Error Audio Init (%d)\r\n", ret);
        return;
    } else {
        printf("Audio Initialized.\t(Audio Freq = %d)\r\n", AUDIO_SAMPLING_FREQUENCY);
    }

    printf("(Press the USER button to record a message.)\n");

    // hit the blue button to record a message
    button_ = new InterruptIn(BUTTON1);
    button_->fall(event_queue_->event(callback(this, &Mic::start_recording)));

    event_queue_->dispatch_forever();
}

void Mic::start(){
    thread_.start(callback(this, &Mic::setup));
}