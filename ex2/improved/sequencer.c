#include "sequencer.h"
#include "efm32gg.h"

void enableDAC();
void disableDAC();
void enableTimer();
void disableTimer();

static struct Song *song = 0;
static uint32_t tick = 0;

static uint32_t square_tick(uint32_t t, uint32_t freq, uint16_t amp)
{
	uint32_t p = SAMPLE_RATE / freq;
	if (t % p < p / 2) {
		return 0;
	} else {
		return amp;
	}
}

uint32_t seq_get_frame_from_song()
{
	uint32_t out = 0;
	bool has_playing = false;

	if (song) {
		for (int i = 0; i < song->num_voices; ++i) {
			struct Voice *c = &song->voices[i];

			if (c->done) {
				continue;
			}

			has_playing = true;

			if (tick >=
			    c->note_begin + c->tones[c->note_index].duration) {
				c->note_begin +=
				    c->tones[c->note_index].duration;
				c->note_index += 1;
				if (c->note_index >= c->tones_length) {
					c->note_index = 0;
					c->done = true;
					continue;
				}
			}
			if (c->tones[c->note_index].tone != SILENT) {
				uint32_t displaced_tick = tick + i * 4;
				uint32_t r = 0;
				r = square_tick(displaced_tick,
						c->tones[c->note_index].tone,
						c->amp);
				out += OUT_RANGE + r;
			}
		}
	}

	if (!has_playing) {
		// Enable energy saving
		*SCR = 6;

		disableDAC();
		disableTimer();
	}

	return out;
}

void seq_play_song(struct Song *s)
{
	song = s;

	if (song) {
		for (int i = 0; i < song->num_voices; ++i) {
			song->voices[i].note_index = 0;
			song->voices[i].note_begin = tick;
			song->voices[i].done = false;
		}
	}
	// Disable energy saving while playing a song
	*SCR = 2;

	enableDAC();
	enableTimer();
}

void seq_tick()
{
	tick += 1;
}
