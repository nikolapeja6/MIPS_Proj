#ifndef _states_h_
#define _states_h_

// States of the system.
//
enum State
{
    // Secure state.
    // The alarm is deactivated. The system will not register any motion.
    // The alarm is turned on (by pressing BUTTON2 on the device).
    //
    Secure,

    // Armed state.
    // The alarm is activated and will register motion in the vicinity of the device.
    // The alarm can be turned off when the user authenticates.
    //
    Armed,

    // Agitated state.
    // The system was armed and movement was detected causing
    // the system to transition into the agitated state.
    // In this state there is a faint beeping sound that resembles a countdown.
    // If the user does not authenticate within 5s the system goes into the breached state.
    //
    Agitated,

    // Breached state.
    // Security has been breached. The system emits two interchanging tones
    // indicating the breached state.
    // The user can still make the system transition into the secure state by authenticating.
    //
    Breached,

    // Arming state.
    // When the user submits the request to transition from the secure into armed state,
    // the system goes into the arming state and will automatically transition
    // into the armed state after 10s. During this time no motion will be detected.
    // This state is used to give the user some time to leave the vicinity of the device.
    //
    Arming
};

// Transitions the system into the agitated state.
//
void transitionToAgitated();

// Transitions the system into the breached state.
//
void transitionToBreached();

// Transitions the system into the secure state.
//
void transitionToSecure();

// Transitions the system into the armed state.
//
void transitionToArmed();

// Transitions the system into the arming state.
//
void transitionToArming();


// Checks if the system is in the secured state.
//
int isInSecureState();

// Checks if the system is in the armed state.
//
int isInArmedState();

// Checks if the system is in the agitated state.
//
int isInAgitatedState();

// Checks if the system is in the breached state.
//
int isInBreachedState();

// Checks if the system is in the arming state.
//
int isInArmingState();

#endif
