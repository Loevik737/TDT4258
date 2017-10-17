#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

void setupTimer(uint16_t period)
{
	*CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_TIMER1;
	*TIMER1_TOP = period;
	*TIMER1_CMD = 1;
}
