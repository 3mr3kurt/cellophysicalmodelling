#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

typedef struct{
    float sample_rate;
    float frequency;
    float* buffer;
    unsigned int buffer_size;
    unsigned int write_index;
    float bow_force;
    unsigned int attack_samples;
    float vibrato_rate;
    float vibrato_depth;
    unsigned int step;
    unsigned int release_samples;
    int release_enabled;
} CelloModel;

CelloModel* init_cello_model(float sample_rate){
    CelloModel* model = (CelloModel*)malloc(sizeof(CelloModel));
    model->sample_rate = sample_rate;
    model->buffer = NULL;
    model->buffer_size = 0;
    model->write_index = 0;
    model->bow_force = 0.0f;
    model->attack_samples = 0;
    model->vibrato_rate = 20.0f;
    model->vibrato_depth = 1.0f;
    model->step = 0;
    return model;
}

void set_frequency(CelloModel* model, float frequency){
    model->frequency = frequency;
    unsigned int new_size = (unsigned int)(model->sample_rate/frequency);
    model->buffer = (float*)realloc(model->buffer, new_size*sizeof(float));

    memset(model->buffer, 0, new_size*sizeof(float));

    model->buffer_size = new_size;
}

void bow(CelloModel* model, float force){
    model->bow_force = force;
    model->attack_samples = 0;
    model->release_samples = 0;
    model->release_enabled = 0;
}

float process(CelloModel* model){
    // Apply vibrato
    float vibrato = sinf(2 * M_PI * model->vibrato_rate * model->step / model->sample_rate);
    float modulated_freq = model->frequency * powf(2.0f, model->vibrato_depth * vibrato / 12.0f);
    unsigned int read_index = (model->write_index + 1) % model->buffer_size;
    unsigned int delay_samples = (unsigned int)(model->sample_rate / modulated_freq);
    float output = model->buffer[read_index];
    float new_sample = (model->buffer[read_index]+model->buffer[model->write_index])*0.5f;

    new_sample += model->bow_force*((float)rand()/(float)RAND_MAX*2.0f-1.0f);

    float attack_time = 0.1f; // Adjust this value to change the attack time
    float attack_samples = attack_time * model->sample_rate;
    // Attack stage
    if (model->attack_samples < attack_samples) {
        float gain = (float)model->attack_samples / attack_samples;
        new_sample *= gain;
        model->attack_samples++;
    }
    // Release stage
    else if (model->release_enabled) {
        float release_time = 0.1f; // Adjust this value to change the release time
        float release_samples = release_time * model->sample_rate;
        if (model->release_samples < release_samples) {
            float gain = 1.0f - ((float)model->release_samples / release_samples);
            new_sample *= gain;
            model->release_samples++;
        }
    }

    model->buffer[model->write_index]=new_sample;
    model->write_index = read_index;
    model->step++;
    return output;
}

void release(CelloModel* model) {
    model->release_enabled = 1;
    model->release_samples = 0;
}

void destroy_cello_model(CelloModel* model){
    if(model->buffer){
        free(model->buffer);
    }
    free(model);
}
