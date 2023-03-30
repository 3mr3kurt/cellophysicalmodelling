#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct{
    float sample_rate;
    float frequency;
    float* buffer;
    unsigned int buffer_size;
    unsigned int write_index;
    float bow_force;
} CelloModel;

CelloModel* init_cello_model(float sample_rate){
    CelloModel* model = (CelloModel*)malloc(sizeof(CelloModel));
    model->sample_rate = sample_rate;
    model->buffer = NULL;
    model->buffer_size = 0;
    model->write_index = 0;
    model->bow_force = 0.0f;
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
    for (unsigned int i = 0;i<model->buffer_size; i++){
        model->buffer[i] = (float)rand()/(float)RAND_MAX*2.0f-1.0f;
    }
}

float process(CelloModel* model){
    unsigned int read_index = (model->write_index+1)%model->buffer_size;

    float output = model->buffer[read_index];
    float new_sample = (model->buffer[read_index]+model->buffer[model->write_index])*0.5f;

    new_sample += model->bow_force*((float)rand()/(float)RAND_MAX*2.0f-1.0f);

    model->buffer[model->write_index]=new_sample;
    model->write_index = read_index;

    return output;
}

void destroy_cello_model(CelloModel* model){
    if(model->buffer){
        free(model->buffer);
    }
    free(model);
}