        .syntax unified
	
	      .include "efm32gg.s"

	/////////////////////////////////////////////////////////////////////////////
	//
  // Exception vector table
  // This table contains addresses for all exception handlers
	//
	/////////////////////////////////////////////////////////////////////////////
	
        .section .vectors
	
	      .long   stack_top               /* Top of Stack                 */
	      .long   _reset                  /* Reset Handler                */
	      .long   dummy_handler           /* NMI Handler                  */
	      .long   dummy_handler           /* Hard Fault Handler           */
	      .long   dummy_handler           /* MPU Fault Handler            */
	      .long   dummy_handler           /* Bus Fault Handler            */
	      .long   dummy_handler           /* Usage Fault Handler          */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* SVCall Handler               */
	      .long   dummy_handler           /* Debug Monitor Handler        */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* PendSV Handler               */
	      .long   dummy_handler           /* SysTick Handler              */

	      /* External Interrupts */
	      .long   dummy_handler
	      .long   gpio_handler            /* GPIO even handler */
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   gpio_handler            /* GPIO odd handler */
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler

	      .section .text

	/////////////////////////////////////////////////////////////////////////////
	//
	// Reset handler
  // The CPU will start executing here after a reset
	//
	/////////////////////////////////////////////////////////////////////////////

	      .globl  _reset
	      .type   _reset, %function
        .thumb_func
_reset: 

	ldr r1, =CMU_BASE + CMU_HFPERCLKEN0 
	ldr r0, [r1]
	ldr r2,= 1<<CMU_HFPERCLKEN0_GPIO
	
	
	orr r0, r0,r2
	str r0, [r1]
	ldr r0,=0x2
	ldr r1,=GPIO_PA_BASE + GPIO_CTRL
	str r0, [r1]
	ldr r0, =0x55555555
	ldr r1, =GPIO_PA_BASE + GPIO_MODEH
	str r0, [r1]
	ldr r0,=0xf300
	ldr r1,=GPIO_PA_BASE+ GPIO_DOUT
	str r0, [r1]
	
	ldr r0,=0x2
	ldr r1,=GPIO_PC_BASE + GPIO_CTRL
	str r0, [r1]
	ldr r0, =0x33333333
	ldr r1, =GPIO_PC_BASE + GPIO_MODEL
	str r0, [r1]
	ldr r0,=0xff
	ldr r1,=GPIO_PC_BASE+ GPIO_DOUT
	str r0, [r1]
	ldr r9,= 0
	
	
loop:
	ldr r1,= GPIO_PC_BASE + GPIO_DIN
	ldr r0,[r1]
	bic r2, r0, r10
	and r2, r2, #4
	cbz r2, postIncrement
	cmp r9, #255
	bge postIncrement
	add r9, r9, #1
postIncrement:
	
	bic r2, r0, r10
	and r2, r2, #1
	cbz r2, postDecrement
	cmp r9, #0
	ble postDecrement
	sub r9, r9, #1
postDecrement:
	bic r2, r0, r10
	and r2, r2, #2
	cbz r2, postClear
	ldr r9, =0
postClear:
	mov r10,r0
	lsl r0,r9, #8
	mvn r0, r0
	ldr r1,=GPIO_PA_BASE+ GPIO_DOUT
	str r0, [r1]
	B loop
	b .  // do nothing
	
	/////////////////////////////////////////////////////////////////////////////
	//
  // GPIO handler
  // The CPU will jump here when there is a GPIO interrupt
	//
	/////////////////////////////////////////////////////////////////////////////
	
        .thumb_func
gpio_handler:  

	      b .  // do nothing
	
	/////////////////////////////////////////////////////////////////////////////
	
        .thumb_func
dummy_handler:  
        b .  // do nothing

