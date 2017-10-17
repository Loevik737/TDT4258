#include "songs.h"

#define DEFAULT_VOL 10

#define COIN_BPM 400

struct Tone coin_ch1[] = {
	{B5, BEAT(COIN_BPM) / 2},
	{E6, BEAT(COIN_BPM) * 5 / 2},
};

struct Voice coin_voices[] = {
	{TONES(coin_ch1), DEFAULT_VOL},
};

struct Song coin = SONG(coin_voices);

#define POWER_UP_BPM 400

struct Tone power_up_ch1[] = {
	{G4, BEAT(POWER_UP_BPM) / 5},
	{B4, BEAT(POWER_UP_BPM) / 5},
	{D5, BEAT(POWER_UP_BPM) / 5},
	{G5, BEAT(POWER_UP_BPM) / 5},
	{B5, BEAT(POWER_UP_BPM) / 5},

	{Ab4, BEAT(POWER_UP_BPM) / 5},
	{C5, BEAT(POWER_UP_BPM) / 5},
	{Eb5, BEAT(POWER_UP_BPM) / 5},
	{Ab5, BEAT(POWER_UP_BPM) / 5},
	{C6, BEAT(POWER_UP_BPM) / 5},

	{Bb4, BEAT(POWER_UP_BPM) / 5},
	{D5, BEAT(POWER_UP_BPM) / 5},
	{F5, BEAT(POWER_UP_BPM) / 5},
	{Bb5, BEAT(POWER_UP_BPM) / 5},
	{D6, BEAT(POWER_UP_BPM) / 5},
};

struct Voice power_up_voices[] = {
	{TONES(power_up_ch1), DEFAULT_VOL},
};

struct Song power_up = SONG(power_up_voices);

struct Tone one_up_ch1[] = {
	{E5, SEC_TO_TICK(0.15)},
	{G5, SEC_TO_TICK(0.15)},
	{E6, SEC_TO_TICK(0.15)},
	{C6, SEC_TO_TICK(0.15)},
	{D6, SEC_TO_TICK(0.15)},
	{G6, SEC_TO_TICK(0.15)},

	{SILENT, SEC_TO_TICK(1.0)},
};

struct Voice one_up_voices[] = {
	{TONES(one_up_ch1), DEFAULT_VOL},
};

struct Song one_up = SONG(one_up_voices);

#define DEATH_BPM 400

struct Tone death_ch1[] = {
	{C5, BEAT(DEATH_BPM) / 4},
	{Cs5, BEAT(DEATH_BPM) / 4},
	{D5, BEAT(DEATH_BPM) / 2},
	{SILENT, BEAT(DEATH_BPM) * 3},
	{B4, BEAT(DEATH_BPM)},
	{F5, BEAT(DEATH_BPM)},
	{SILENT, BEAT(DEATH_BPM)},
	{F5, BEAT(DEATH_BPM)},
	{F5, BEAT(DEATH_BPM) * 4 / 3},
	{E5, BEAT(DEATH_BPM) * 4 / 3},
	{D5, BEAT(DEATH_BPM) * 4 / 3},
	{C5, BEAT(DEATH_BPM)},
	{E4, BEAT(DEATH_BPM)},
	{G3, BEAT(DEATH_BPM)},
	{E4, BEAT(DEATH_BPM)},
	{C4, BEAT(DEATH_BPM)},

	{SILENT, BEAT(DEATH_BPM) * 3},
};

struct Tone death_ch2[] = {
	{SILENT, BEAT(DEATH_BPM) * 4},
	{G3, BEAT(DEATH_BPM)},
	{SILENT, BEAT(DEATH_BPM) * 2},
	{G3, BEAT(DEATH_BPM)},
	{G3, BEAT(DEATH_BPM) * 4 / 3},
	{A3, BEAT(DEATH_BPM) * 4 / 3},
	{B3, BEAT(DEATH_BPM) * 4 / 3},
	{C4, BEAT(DEATH_BPM)},
	{SILENT, BEAT(DEATH_BPM) * 3},
	{C3, BEAT(DEATH_BPM)},
	{SILENT, BEAT(DEATH_BPM) * 3},
};

struct Tone death_ch3[] = {
	{SILENT, BEAT(DEATH_BPM) * 4},
	{G4, BEAT(DEATH_BPM)},
	{D5, BEAT(DEATH_BPM)},
	{SILENT, BEAT(DEATH_BPM)},
	{D5, BEAT(DEATH_BPM)},
	{D5, BEAT(DEATH_BPM) * 4 / 3},
	{C5, BEAT(DEATH_BPM) * 4 / 3},
	{B4, BEAT(DEATH_BPM) * 4 / 3},
	{G4, BEAT(DEATH_BPM)},

	{SILENT, BEAT(DEATH_BPM) * 7},
};

struct Voice death_voices[] = {
	{TONES(death_ch1), DEFAULT_VOL},
	{TONES(death_ch2), DEFAULT_VOL},
	//{TONES(death_ch3), DEFAULT_VOL},
};

struct Song death = SONG(death_voices);

#define ZELDA_BPM 120

struct Tone zelda_ch1[] = {
	{E5, BEAT(ZELDA_BPM) * 3 / 2},
	{F5, BEAT(ZELDA_BPM) / 2},
	{G5, BEAT(ZELDA_BPM)},

	{E5, BEAT(ZELDA_BPM)},
	{D5, BEAT(ZELDA_BPM)},
	{C5, BEAT(ZELDA_BPM) * 3},

	{B4, BEAT(ZELDA_BPM)},
	{A4, BEAT(ZELDA_BPM) * 2},
	{SILENT, BEAT(ZELDA_BPM)},

	{E5, BEAT(ZELDA_BPM) * 3 / 2},
	{F5, BEAT(ZELDA_BPM) / 2},
	{G5, BEAT(ZELDA_BPM)},

	{E5, BEAT(ZELDA_BPM)},
	{D5, BEAT(ZELDA_BPM)},
	{C5, BEAT(ZELDA_BPM)},

	{D5, BEAT(ZELDA_BPM) * 2},
	{E5, BEAT(ZELDA_BPM) * 2},
	{SILENT, BEAT(ZELDA_BPM)},

	{A4, BEAT(ZELDA_BPM) / 2},
	{E5, BEAT(ZELDA_BPM) * 3 / 8},
	{SILENT, BEAT(ZELDA_BPM) / 8},

	{E5, BEAT(ZELDA_BPM) / 2},
	{G4, BEAT(ZELDA_BPM) / 2},
	{A4, BEAT(ZELDA_BPM)},
	{SILENT, BEAT(ZELDA_BPM)},

	{E5, BEAT(ZELDA_BPM) / 2},
	{B4, BEAT(ZELDA_BPM) / 2},
	{C5, BEAT(ZELDA_BPM)},
	{D5, BEAT(ZELDA_BPM)},

	{E5, BEAT(ZELDA_BPM) / 2},
	{A4, BEAT(ZELDA_BPM) / 2},
	{A5, BEAT(ZELDA_BPM) * 3 / 2},
	{G5, BEAT(ZELDA_BPM) / 2},

	{E5, BEAT(ZELDA_BPM)},
	{SILENT, BEAT(ZELDA_BPM)},
	{A4, BEAT(ZELDA_BPM) / 2},
	{Fs5, BEAT(ZELDA_BPM) / 2},

	{E5, BEAT(ZELDA_BPM) * 2},
	{A4, BEAT(ZELDA_BPM) / 2},
	{Fs5, BEAT(ZELDA_BPM) / 2},

	{E5, BEAT(ZELDA_BPM) * 2},
	{A4, BEAT(ZELDA_BPM) / 2},
	{Fs5, BEAT(ZELDA_BPM) * 3 / 8},
	{SILENT, BEAT(ZELDA_BPM) / 8},
	{Fs5, BEAT(ZELDA_BPM) / 2},

	{E5, BEAT(ZELDA_BPM) * 5 / 2},

	{SILENT, BEAT(ZELDA_BPM) * 3},
};

struct Tone zelda_ch2[] = {
	{SILENT, BEAT(ZELDA_BPM) * 12},

	{F3, BEAT(ZELDA_BPM)},
	{E4, BEAT(ZELDA_BPM) * 2},

	{G3, BEAT(ZELDA_BPM)},
	{D4, BEAT(ZELDA_BPM) * 2},

	{C4, BEAT(ZELDA_BPM)},
	{E4, BEAT(ZELDA_BPM) * 2},

	{C4, BEAT(ZELDA_BPM)},
	{G4, BEAT(ZELDA_BPM) * 2},

	{F3, BEAT(ZELDA_BPM) / 2},
	{A3, BEAT(ZELDA_BPM) / 2},
	{C4, BEAT(ZELDA_BPM) / 2},
	{E4, BEAT(ZELDA_BPM) / 2},
	{F4, BEAT(ZELDA_BPM) / 2},
	{A4, BEAT(ZELDA_BPM) / 2},

	{G3, BEAT(ZELDA_BPM) / 2},
	{B3, BEAT(ZELDA_BPM) / 2},
	{D4, BEAT(ZELDA_BPM) / 2},
	{G4, BEAT(ZELDA_BPM) / 2},
	{B4, BEAT(ZELDA_BPM) / 2},
	{D5, BEAT(ZELDA_BPM) / 2},

	{E3, BEAT(ZELDA_BPM) / 2},
	{G3, BEAT(ZELDA_BPM) / 2},
	{B3, BEAT(ZELDA_BPM) / 2},
	{E4, BEAT(ZELDA_BPM) / 2},
	{G4, BEAT(ZELDA_BPM) / 2},
	{B4, BEAT(ZELDA_BPM) / 2},

	{A3, BEAT(ZELDA_BPM) / 2},
	{Cs4, BEAT(ZELDA_BPM) / 2},
	{E4, BEAT(ZELDA_BPM) / 2},
	{A4, BEAT(ZELDA_BPM) / 2},
	{Cs5, BEAT(ZELDA_BPM) / 2},
	{E5, BEAT(ZELDA_BPM) / 2},

	{SILENT, BEAT(ZELDA_BPM) * 12},
};

struct Voice zelda_voices[] = {
	{TONES(zelda_ch1), DEFAULT_VOL},
	{TONES(zelda_ch2), DEFAULT_VOL},
};

struct Song zelda = SONG(zelda_voices);
