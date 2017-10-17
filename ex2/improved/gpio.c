#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

void setupGPIO()
{
	*CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_GPIO;

	*GPIO_PC_CTRL = 0;	// set high drive strength
	*GPIO_PC_MODEL = 0x33333333;	// set pins C1-7 as input
	*GPIO_PC_DOUT = 0xff;	// enable pull up
	*GPIO_EXTIPSELL = 0x22222222;
	*GPIO_EXTIFALL = 0xff;	// set interrupt on button pressed
}
