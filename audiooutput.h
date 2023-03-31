#include <portaudio.h>
#include "cellomodel.h"

typedef struct{
    PaStream* stream;
    CelloModel* cello_model;
} AudioOutput;

static int audio_callback(const void* input_buffer, void* output_buffer, unsigned long frames_per_buffer, const PaStreamCallbackTimeInfo* time_info, PaStreamCallbackFlags status_flags, void* user_data){
    AudioOutput* audio_output = (AudioOutput*)user_data;
    CelloModel* cello_model = audio_output->cello_model;
    float* out = (float*) output_buffer;
    //printf("Audio callback called.\n"); 
    for (unsigned long i = 0; i<frames_per_buffer;i++){
        *out++ = process(cello_model);
    }

    return paContinue;
}

AudioOutput* init_audio_output(){
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        printf("PortAudio initialization error: %s\n", Pa_GetErrorText(err));
        return NULL;
    }

    int numDevices = Pa_GetDeviceCount();
    if (numDevices < 0) {
        printf("PortAudio error: %s\n", Pa_GetErrorText(numDevices));
    } else {
        printf("Number of devices: %d\n", numDevices);
    }

    AudioOutput* audio_output = (AudioOutput*)malloc(sizeof(AudioOutput));
    audio_output->stream = NULL;
    audio_output->cello_model = NULL;
    const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice());
    if (deviceInfo) {
        printf("Default output device: %s\n", deviceInfo->name);
        printf("Default sample rate: %lf\n", deviceInfo->defaultSampleRate);
    } else {
        printf("Cannot get default output device info.\n");
    }
    //PaError err = Pa_Initialize();
    if(err!=paNoError){
        fprintf(stderr, "Portaudio error: %s\n", Pa_GetErrorText(err));
        return NULL;
    }
    return audio_output;
}

void connect_model_to_audio_output(CelloModel* cello_model, AudioOutput* audio_output){
    audio_output->cello_model = cello_model;
}

int start_audio_output(AudioOutput* audio_output){
    PaError err;

    if(audio_output->cello_model==NULL){
        fprintf(stderr, "Error: Cello model not connect to the audio output\n");
        return -1;
    }
    err = Pa_OpenDefaultStream(&audio_output->stream, 0, 1, paFloat32, audio_output->cello_model->sample_rate, 512, audio_callback, audio_output);
    if(err!=paNoError){
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
        return -1;
    }
    err = Pa_StartStream(audio_output->stream);
    if (err!= paNoError){
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
        return -1;
    }
    return 0;
}

int stop_audio_output(AudioOutput* audio_output){
    PaError err = Pa_StopStream(audio_output->stream);
    if (err!= paNoError){
        fprintf(stderr,"PortAudio error: %s\n", Pa_GetErrorText(err));
        return -1;
    }
    return 0;
}

void destroy_audio_output(AudioOutput* audio_output){
    if (audio_output->stream){
        PaError err = Pa_CloseStream(audio_output->stream);
        if (err != paNoError) {
            fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
        }
    }
    Pa_Terminate();
    free(audio_output);
}