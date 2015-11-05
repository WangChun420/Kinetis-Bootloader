/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* blink red light*/
#include "MK22F51212.h"
#include "bl_communication.h"

#define RELOCATED_VECTORS          0x2000                            // Start address of relocated interrutp vector table
// enable GPIO clock
#define INIT_ALL_CPIO_CLK	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK|SIM_SCGC5_PORTE_MASK;


// init PTEA1 to gpio
#define APP_LED_INIT   PORTA->PCR[1]=(0|PORT_PCR_MUX(1));\
	              	   PORTA->PCR[2]=(0|PORT_PCR_MUX(1));\
	              	   PTA->PDDR  |= GPIO_PDDR_PDD( GPIO_PIN(1)  );\
	              	   PTA->PDDR  |= GPIO_PDDR_PDD( GPIO_PIN(2)  );\
	              	   PTA->PDOR |= GPIO_PDOR_PDO(GPIO_PIN(1));\
	              	   PTA->PCOR |= GPIO_PCOR_PTCO(GPIO_PIN(2)); // clear the PTA1 pin

#define GPIO_PIN_MASK            0x1Fu
#define GPIO_PIN(x)              (((1)<<(x & GPIO_PIN_MASK)))

void flash_protect();
void init_PIT(void);

int main(void)
{
	int j;
    int i;
 //   SCB_VTOR = RELOCATED_VECTORS;

    EnableInterrupts;			// enable interrupt
    INIT_ALL_CPIO_CLK;
    flash_protect();           // protect the entire flash

    INIT_CLOCKS_TO_MODULES;    // init clock module	-- need add to APP of uers - 2
    UART_Initialization();     // init UART module -- need add to APP of uers - 3

    APP_LED_INIT;
    init_PIT();				   // init timer
    NVIC_EnableIRQ(48);       // enable PIT timer interrupt

   while(1)
    {

    	UpdateAPP();         // update user's application -- need add to the app of user's - 4
    }
    /* Never leave main */
    return 0;
}

//-----------------------------------------------------------------------------
// FUNCTION:    init_PIT
// SCOPE:       Applicaiton function
// DESCRIPTION: Initialize the PIT  module
//
// PARAMETERS:  none
//
// RETURNS:     none
//-----------------------------------------------------------------------------
void init_PIT(void)
{
	SIM_SCGC6=SIM_SCGC6|0x00800000; //enable PIT clock
	PIT_MCR = 0x00;                 // turn on PIT
	PIT_LDVAL0 = 0x003FFFFF;        // setup timer so that the LED have enough time to flash
	PIT_TCTRL0 = PIT_TCTRL1|0x02;   // enable Timer 1 interrupts
	PIT_TCTRL0 |= 0x01;             // start Timer 1
}
//-----------------------------------------------------------------------------
// FUNCTION:    PIT_IRQHandler
// SCOPE:       Applicaiton function
// DESCRIPTION: PIT interrupt
//
// PARAMETERS:  none
//
// RETURNS:     none
//-----------------------------------------------------------------------------
void PIT0_IRQHandler()
{
	GPIOA_PTOR = 0x000000004;

	PIT_TFLG0=0x01;
	PIT_TCTRL0; //dummy read the PIT1 control reg to enable another interput
}

//-----------------------------------------------------------------------------
// FUNCTION:    flash_protect
// SCOPE:       Applicaiton function
// DESCRIPTION: protected flash regions from program and erase operations
//
// PARAMETERS:  none
//
// RETURNS:     none
//-----------------------------------------------------------------------------
void flash_protect()
{
	FTFA_FPROT0 = 0x00;
	FTFA_FPROT1 = 0x00;
	FTFA_FPROT2 = 0x00;
	FTFA_FPROT3 = 0x00;

}
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
