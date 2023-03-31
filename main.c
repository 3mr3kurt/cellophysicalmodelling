#include <stdio.h>
#include <unistd.h>
#include "cellomodel.h"
#include "audiooutput.h"
#include <glib.h>
#define NOTE_DURATION 1.0 

typedef struct{
    float frequency;
    float duration;
}Note;
Note sequence[] = {
    {146.83, 0.25}, // D3
    {110.00, 0.25}, // A2
    {146.83, 0.25}, // D3
    {174.61, 0.25}, // F3
    {220.00, 0.25}, // A3
    {174.61, 0.25}, // F3
    {146.83, 0.25}, // D3
    {110.00, 0.25}, // A2
    {146.83, 0.25}, // D3
    {110.00, 0.25}, // A2
    {146.83, 0.25}, // D3
    {174.61, 0.25}, // F3
    {220.00, 0.25}, // A3
    {174.61, 0.25}, // F3
    {146.83, 0.25}, // D3
    {110.00, 0.25},
    {146.83, 1.0}, // D3
};

const int SEQUENCE_LENGTH = sizeof(sequence)/sizeof(Note);

void play_note(CelloModel *cello_model,AudioOutput *audio_output,float frequency, float duration_ms){
    set_frequency(cello_model, frequency);
    bow(cello_model, 0.1f);
    usleep(duration_ms*1000);
}

int main() {
    // Initialize the cello model and audio output
    CelloModel* cello_model = init_cello_model(44100);
    set_frequency(cello_model, 440.0); // note
    bow(cello_model, 0.1f);
    AudioOutput* audio_output = init_audio_output();


    // Connect the cello model to the audio output
    connect_model_to_audio_output(cello_model, audio_output);

    // Start the audio output
    if (start_audio_output(audio_output) == 0) {
        printf("Playing cello sound... Press Enter to stop.\n");
        //getchar(); // Wait for user input
        for (int i = 0; i < SEQUENCE_LENGTH; ++i) {
            play_note(cello_model, audio_output, sequence[i].frequency, sequence[i].duration*1000);
        }
        // Stop the audio output
        stop_audio_output(audio_output);
    }

    // Cleanup
    destroy_audio_output(audio_output);
    destroy_cello_model(cello_model);

    return 0;
}
