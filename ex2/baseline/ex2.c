#include <stdint.h>
#include <stdbool.h>
#include "songs.h"

#include "efm32gg.h"

// The period between sound samples, in clock cycles
#define SAMPLE_PERIOD 317

void setupGPIO();
void setupTimer(uint32_t period);
void setupDAC();

extern uint32_t tick;

int main(void)
{
	// Call the peripheral setup functions
	setupGPIO();
	setupDAC();
	setupTimer(SAMPLE_PERIOD);

	// Play startup song
	seq_play_song(&zelda);

	uint32_t last_buttons = 0xffffffff;

	while (1) {
		uint32_t c_buttons = *GPIO_PC_DIN;
		uint32_t buttons = c_buttons | ~last_buttons;

		if ((buttons & 0x01) == 0) {
			seq_play_song(0);
		} else if ((buttons & 0x02) == 0) {
			seq_play_song(&zelda);
		} else if ((buttons & 0x04) == 0) {
			seq_play_song(&one_up);
		} else if ((buttons & 0x08) == 0) {
			seq_play_song(&death);
		} else if ((buttons & 0x10) == 0) {
			seq_play_song(&coin);
		} else if ((buttons & 0x20) == 0) {
			seq_play_song(&power_up);
		}
		last_buttons = c_buttons;

		if (*TIMER1_CNT > SAMPLE_PERIOD - 50) {
			uint32_t out = seq_get_frame_from_song();

			*DAC0_CH0DATA = out;
			*DAC0_CH1DATA = out;

			seq_tick();
		}
	}

	return 0;
}

