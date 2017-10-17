#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

void setupGPIO()
{
	*GPIO_PC_CTRL = 2;	// set high drive strength
	*GPIO_PC_MODEL = 0x33333333;	// set pins C1-7 as input
	*GPIO_PC_DOUT = 0xff;
}
