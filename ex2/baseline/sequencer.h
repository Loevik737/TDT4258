#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <stdint.h>
#include <stdbool.h>

#define OUT_RANGE 4096
#define SAMPLE_RATE 44100

#define SEC_TO_TICK(d) (uint32_t)(d*44100.0)
#define BEAT(bpm) ((44100*60)/bpm)

#define NUM_VOICES(ch) (sizeof(ch) / sizeof(struct Voice))

struct Tone {
	uint32_t tone;
	uint32_t duration;
};

struct Voice {
	struct Tone *tones;
	uint32_t tones_length;
	uint32_t amp;
	uint32_t note_index;
	uint32_t note_begin;
	bool done;
};

struct Song {
	struct Voice *voices;
	uint32_t num_voices;
};

#define SONG_LENGTH(voices) (sizeof(voices) / sizeof(struct Voice))
#define SONG(voices) {voices, SONG_LENGTH(voices)}

#define TONES_LENGTH(tones) (sizeof(tones) / sizeof(struct Tone))
#define TONES(tones) tones, TONES_LENGTH(tones)

#define C3 131
#define Cs3 139
#define D3 147
#define E3 165
#define F3 175
#define G3 196
#define A3 220
#define B3 247

#define C4 262
#define Cs4 277
#define D4 294
#define E4 330
#define F4 349
#define G4 392
#define Ab4 415
#define A4 440
#define Bb4 466
#define B4 494

#define C5 523
#define Cs5 554
#define D5 587
#define Eb5 622
#define E5 659
#define F5 698
#define Fs5 740
#define G5 784
#define Ab5 831
#define A5 880
#define Bb5 932
#define B5 988

#define C6 1046
#define Cs6 1109
#define D6 1175
#define E6 1319
#define F6 1397
#define G6 1568
#define A6 1760
#define B6 1976

#define SILENT 44100

void seq_play_song(struct Song *);
uint32_t seq_get_frame_from_song();
void seq_tick();

#endif
