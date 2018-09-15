#include "states.h"

// State of the system.
//
volatile int state = Secure;

// Indicates if the speaker is turned on.
//
volatile int speakerTurnedOn = 0;

// Variables for the alarm tones.
//
const int TONE_DURATION = 2000;
int tone = 0;
int toneCnt = TONE_DURATION;

// Variables for the transitions with countdowns.
//
const int COOLDOWN_SECS = 10;
const int AGITATED_SECS = 5;
int cooldownCnt = AGITATED_SECS;

// Variables for the agitated (countdown) beeps.
//
const int AGITATED_BEEP_CNT = 7;
int agitatedBeepCnt = AGITATED_BEEP_CNT;
const int AGITATED_BEEP_DURATION = 30;
int agitatedBeepDuration = AGITATED_BEEP_DURATION;

// Transitions the system into the agitated state.
//
void transitionToAgitated(){
    if (state == Secure)
    {
        return;
    }

    cooldownCnt = AGITATED_SECS;
    state = Agitated;
    speakerTurnedOn = 1;

    agitatedBeepCnt = AGITATED_BEEP_CNT;
    agitatedBeepDuration = AGITATED_BEEP_DURATION;
}

// Transitions the system into the breached state.
//
void transitionToBreached()
{
    if (state == Secure)
    {
        return;
    }

    cooldownCnt = COOLDOWN_SECS;
    speakerTurnedOn = 1;
    state = Breached;
}

// Transitions the system into the secure state.
//
void transitionToSecure()
{
    cooldownCnt = COOLDOWN_SECS;
    state = Secure;
    speakerTurnedOn = 0;
}

// Transitions the system into the armed state.
//
void transitionToArmed()
{
    if (state == Secure)
    {
        return;
    }
    cooldownCnt = AGITATED_SECS;
    state = Armed;
    speakerTurnedOn = 0;
}

// Transitions the system into the arming state.
//
void transitionToArming()
{
    cooldownCnt = COOLDOWN_SECS;
    state = Arming;
    speakerTurnedOn = 0;
}

// Checks if the system is in the secured state.
//
int isInSecureState()
{
    return state == Secure;
}

// Checks if the system is in the armed state.
//
int isInArmedState()
{
    return state == Armed;
}

// Checks if the system is in the agitated state.
//
int isInAgitatedState()
{
    return state == Agitated;
}

// Checks if the system is in the breached state.
//
int isInBreachedState()
{
    return state == Breached;
}

// Checks if the system is in the arming state.
//
int isInArmingState()
{
    return state == Arming;
}
