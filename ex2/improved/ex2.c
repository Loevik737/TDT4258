#include <stdint.h>
#include <stdbool.h>
#include "songs.h"

#include "efm32gg.h"

#define PI (3.14159265359f)

// The period between sound samples, in clock cycles
#define SAMPLE_PERIOD 317

void setupGPIO();
void setupTimer(uint32_t period);
void setupDAC();
void setupNVIC();

#if 0
void setupDMA(uint16_t period);
#endif

int main(void)
{
	// Call the peripheral setup functions
	setupGPIO();
	setupDAC();
	setupTimer(SAMPLE_PERIOD);

	// Enable interrupt handling
	setupNVIC();

	// Set energy saving mode
	*SCR = 6;

	// Play startup song
	seq_play_song(&zelda);

	__asm__("wfi");

	return 0;
}

void setupNVIC()
{
	// Enable timer interrupt
	*ISER0 |= (1 << 12);

	// Enable GPIO interrupts
	*GPIO_IEN = 0xff;
	*ISER0 |= 0x802;

	*GPIO_IF = 0;
	*GPIO_IFC = *GPIO_IF;
}

#if 0
// This is our attempt at setting up feeding the DAC using DMA. Due to time
// constraints this was not finished.
void setupDMA(uint16_t period)
{
	*CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_PRS;
	*CMU_HFCORECLKEN0 |= CMU_HFCORECLKEN0_DMA;

	// Setup the PRS system such that one of the timers trigger a PRS channel
	*PRS_CH0_CTRL |= (PRS_SOURCESEL_TIMER0 << 16);
	*PRS_CH0_CTRL |= (TIMER_UNDERFLOW);

	*CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_TIMER1;
	*TIMER1_TOP = period;

	// Setup DAC to trigger on the PRS channel

	// Setup  the  DMA  control block  for Ping  Pong  mode  (section  8.4.3
	// in  [13]).   Note that the control block must be 512 byte aligned in
	// memory.

	// Setup the DMA to send a new sample to the DAC every time the DAC is
	// ready (SOURCESEL and SIGSEL in DMA CH0 CTRL).
	*DMA_CH0_CTRL |= (DMA_SOURCESEL_DAC0 << 16);
	*DMA_CH0_CTRL |= (DMA_SIGSEL_DAC0CH0);
	*DMA_CH0_CTRL |= (DMA_SIGSEL_DAC0CH1);

	//Enable DMA interrupts and use the DMA interrupt handler to generate new
	//blocks of data for the DMA to transfer
	*DMA_IEN |= (1 << 0);
}
#endif
