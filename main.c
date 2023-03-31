#include <stdio.h>
#include <unistd.h>
#include "cellomodel.h"
#include "audiooutput.h"
#include <glib.h>
#include <smf.h>

void play_note(CelloModel *cello_model, AudioOutput *audio_output, float frequency, float duration_ms);

void play_midi_file(const char *filename, CelloModel *cello_model, AudioOutput *audio_output) {
    smf_t *smf = smf_load(filename);
    if (smf == NULL) {
        fprintf(stderr, "Error loading MIDI file: %s\n", filename);
        return;
    }

    smf_event_t *event;
    while ((event = smf_get_next_event(smf)) != NULL) {
        if (smf_event_is_metadata(event)) {
            continue;
        }

        smf_event_t *next_event = smf_peek_next_event(smf);
        double delta_time_ms = 0.0;

        if (next_event) {
            smf_tempo_t *tempo = smf_get_tempo_by_pulses(smf, event->time_pulses);
            if (tempo) {
                double tempo_microseconds_per_quarter_note = tempo->microseconds_per_quarter_note/2;
                double delta_time_ticks = next_event->time_pulses - event->time_pulses;
                delta_time_ms = (delta_time_ticks * tempo_microseconds_per_quarter_note) / (smf->ppqn * 1000.0);
            }
        }

        if (event->midi_buffer_length != 3) {
            continue;
        }

        uint8_t status = event->midi_buffer[0] & 0xF0;
        uint8_t channel = event->midi_buffer[0] & 0x0F;
        uint8_t note = event->midi_buffer[1];
        uint8_t velocity = event->midi_buffer[2];

        if (status == 0x90 && velocity > 0) { // Note on
            float frequency = 440.0f * powf(2.0f, (note - 69.0f) / 12.0f);
            play_note(cello_model, audio_output, frequency, delta_time_ms);
        }

        usleep(delta_time_ms * 1000);
    }

    smf_delete(smf);
}

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
        printf("Playing cello sound...\n");
        //getchar(); // Wait for user input
        // Play the MIDI file
        play_midi_file("winter3.mid", cello_model, audio_output);
        // Stop the audio output
        stop_audio_output(audio_output);
    }

    // Cleanup
    destroy_audio_output(audio_output);
    destroy_cello_model(cello_model);

    return 0;
}
