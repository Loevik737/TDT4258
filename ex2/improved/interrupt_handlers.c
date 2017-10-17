#include <stdint.h>
#include <stdbool.h>
#include "sequencer.h"
#include "songs.h"

#include "efm32gg.h"

void __attribute__ ((interrupt)) TIMER1_IRQHandler()
{
	*TIMER1_IFC = 1;

	uint32_t out = seq_get_frame_from_song();

	*DAC0_CH0DATA = out;
	*DAC0_CH1DATA = out;

	seq_tick();
}

bool gpio_interrupt_skip = true;

void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler()
{
	uint32_t event = *GPIO_IF;
	*GPIO_IFC |= event;

	// Skip the first interrupt, as this is a phantom interrupt
	if (gpio_interrupt_skip) {
		gpio_interrupt_skip = false;
		return;
	}

	switch (event) {
	case (1 << 0):
		seq_play_song(0);
		break;
	case (1 << 2):
		seq_play_song(&one_up);
		break;
	case (1 << 4):
		seq_play_song(&coin);
		break;
	}
}

void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler()
{
	uint32_t event = *GPIO_IF;
	*GPIO_IFC |= event;

	// Skip the first interrupt, as this is a phantom interrupt
	if (gpio_interrupt_skip) {
		gpio_interrupt_skip = false;
		return;
	}

	switch (event) {
	case (1 << 1):
		seq_play_song(&zelda);
		break;
	case (1 << 3):
		seq_play_song(&death);
		break;
	case (1 << 5):
		seq_play_song(&power_up);
		break;
	}
}

void __attribute__ ((interrupt)) DMA_IRQHandler()
{
}
