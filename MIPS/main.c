#include "rfid_interface.h"
#include "states.h"
#include "timers.h"
#include "general.h"


// Pin definitions.
//
sbit LD1 at ODR12_GPIOE_ODR_bit;
sbit LD2 at ODR15_GPIOE_ODR_bit;

// Checks if the button is pressed.
//
int buttonPressed()
{
     return Button(&GPIOA_IDR, 10, 2, 0);
}

// Initializes the system.
//
void initSystem()
{
    // Initialize input/output pins.
    //
    GPIO_Digital_Input(&GPIOA_BASE, _GPIO_PINMASK_10);
    GPIO_Digital_Output(&GPIOE_BASE, _GPIO_PINMASK_12 | _GPIO_PINMASK_15);
    GPIO_Digital_Input(&GPIOE_BASE, _GPIO_PINMASK_6);
    GPIO_Digital_Output(&GPIOE_BASE, _GPIO_PINMASK_1);

    // Initialize RFID module.
    //
    InitRFID();

    // Initialize timers.
    //
    InitTimer2();
    InitTimer3();
    InitTimer4();
    InitTimer7();
}

void updateSystem()
{
    // Iteration of the RFID module.
    //
    RFID_iteration();

    // Update signal LEDs.
    //
    LD1 = !isInSecureState() && !isInArmingState();
    LD2 = isInArmingState() || isInAgitatedState();

    // Detect motion and transition into agitated state.
    //
    if (isInArmedState() && GPIOE_IDR.B6)
    {
        transitionToAgitated();
    }

    // Detect system arming (BUTTON2 pressed) and transition into arming state.
    //
    if (isInSecureState() && buttonPressed())
    {
        transitionToArming();
    }
}


// main function
void main() {

    initSystem();

    while(1)
    {
        updateSystem();
    }
}