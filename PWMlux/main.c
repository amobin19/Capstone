#include <msp430.h>
#include "LED.h"
#include "pir.h"

// Function prototypes
void ConfigureClockModule(void);

void main(void){
    // Stop the watchdog timer, and configure the clock module.
    WDTCTL = WDTPW + WDTHOLD;
    ConfigureClockModule();

    // Initialize port pins associated with the PIR and LED.
    InitializePIRPortPin();
    InitializeLEDPortPin();

    // Configure timer A to generate the required interrupt.
    _enable_interrupts();

    // Set Output to P2.1
    P2DIR |= 0x02; // Set pin 2.1 to the output direction.
    P2SEL |= 0x02; // Select pin 2.1 as our PWM output.

    // PWM
    TA1CCR0 = 249; // Set the period in the Timer A0 Capture/Compare 0 register to 249us.
    TA1CCTL1 = OUTMOD_7;

    // Lux Values
    int luxVal = 0;

    // Infinite loop
    while (1) {

    	// If motion ... turn it "on"
    	if ((READ_PIR_VAL)){
    		if(luxVal < 100){
    			TA1CCR1 = 50; // The period in microseconds that the power is ON. It's half the time, which translates to a 20% duty cycle.
    		}
    		else if(luxVal >= 100 && luxVal <= 200){
    			TA1CCR1 = 25;
    		}
    		else if(luxVal >= 201 && luxVal <= 500){
    			TA1CCR1 = 10;
    		}
    		else{
    			TA1CCR1 = 0;
    		}
    		_delay_cycles(1000000 * 5); // holds the light for 5 seconds
    	}

    	// If no Motion ... turn it "off"
    	else {
		    TA1CCR1 = 0; // The period in microseconds that the power is ON. It's half the time, which translates to a 0% duty cycle.
    	}

    	TA1CTL = TASSEL_2 + MC_1; // TASSEL_2 selects SMCLK as the clock source, and MC_1 tells it to count up to the value in TA0CCR0.
    }
}

void ConfigureClockModule(void){
    // Configure Digitally Controlled Oscillator (DCO) using factory calibrations.
    DCOCTL  = CALDCO_1MHZ;
    BCSCTL1 = CALBC1_1MHZ;
    BCSCTL2 |= DIVS_3;
}
