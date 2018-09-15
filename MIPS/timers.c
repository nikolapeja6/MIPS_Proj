#include "timers.h"
#include "states.h"
#include "general.h"

// Global variables.
//
extern volatile int speakerTurnedOn;
extern int state;
extern const int TONE_DURATION;
extern int tone;
extern int toneCnt;
const int COOLDOWN_SECS;
extern const int AGITATED_SECS;
extern int cooldownCnt;
extern const int AGITATED_BEEP_CNT;
extern int agitatedBeepCnt;
extern const int AGITATED_BEEP_DURATION;
extern int agitatedBeepDuration;

// Pin definitions.
//
sbit SPEAKER at ODR1_GPIOE_ODR_bit;

//Timer2 Prescaler :239; Preload = 62499; Actual Interrupt Time = 1 s
void InitTimer2()
{
    RCC_APB1ENR.TIM2EN = 1;
    TIM2_CR1.CEN = 0;
    TIM2_PSC = 239;
    TIM2_ARR = 62499;
    NVIC_IntEnable(IVT_INT_TIM2);
    TIM2_DIER.UIE = 1;
    TIM2_CR1.CEN = 1;
}

void Timer2_interrupt() iv IVT_INT_TIM2
{
    TIM2_SR.UIF = 0;
    if (isInAgitatedState() || isInArmingState())
    {
        cooldownCnt--;
        if (cooldownCnt <= 0)
        {

            if (isInAgitatedState())
            {
                transitionToBreached();
            }
            else
            {
                transitionToArmed();
            }
        }
    }
}

//Timer3 Prescaler :0; Preload = 3072; Actual Interrupt Time = 204.866666667 us
void InitTimer3()
{
    RCC_APB1ENR.TIM3EN = 1;
    TIM3_CR1.CEN = 0;
    TIM3_PSC = 0;
    TIM3_ARR = 3072;
    NVIC_IntEnable(IVT_INT_TIM3);
    TIM3_DIER.UIE = 1;
    TIM3_CR1.CEN = 1;
}

void Timer3_interrupt() iv IVT_INT_TIM3
{
    const int toneId = 0;
    TIM3_SR.UIF = 0;

    if (!speakerTurnedOn)
    {
        toneCnt = TONE_DURATION;
        tone = 0;
    }

    if (speakerTurnedOn && tone == toneId && isInBreachedState())
    {
        SPEAKER ^= 1;
        toneCnt--;

        if (toneCnt <= 0)
        {
            toneCnt = TONE_DURATION;
            tone = (tone + 1) % 2;
        }
    }
}

void InitTimer4()
{
    RCC_APB1ENR.TIM4EN = 1;
    TIM4_CR1.CEN = 0;
    TIM4_PSC = 0;
    TIM4_ARR = 6144;
    NVIC_IntEnable(IVT_INT_TIM4);
    TIM4_DIER.UIE = 1;
    TIM4_CR1.CEN = 1;
}

void Timer4_interrupt() iv IVT_INT_TIM4
{
    const int toneId = 1;
    TIM4_SR.UIF = 0;
    if (speakerTurnedOn && tone == toneId && isInBreachedState())
    {
        SPEAKER ^= 1;
        toneCnt--;

        if (toneCnt <= 0)
        {
            toneCnt = TONE_DURATION;
            tone = (tone + 1) % 2;
        }
    }

}

//Timer7 Prescaler :0; Preload = 17044; Actual Interrupt Time = 1.136333333 ms
void InitTimer7()
{
    RCC_APB1ENR.TIM7EN = 1;
    TIM7_CR1.CEN = 0;
    TIM3_PSC = 0;
    TIM3_ARR = 3072;
    NVIC_IntEnable(IVT_INT_TIM7);
    TIM7_DIER.UIE = 1;
    TIM7_CR1.CEN = 1;
}

void Timer7_interrupt() iv IVT_INT_TIM7
{
    TIM7_SR.UIF = 0;
    if (speakerTurnedOn && isInAgitatedState()) {

        agitatedBeepDuration--;

        if (agitatedBeepDuration <= 0)
        {
            agitatedBeepDuration = AGITATED_BEEP_DURATION;
            agitatedBeepCnt--;

            if (agitatedBeepCnt < 0)
            {
                agitatedBeepCnt = AGITATED_BEEP_CNT;
            }
        }

        if (agitatedBeepCnt <= 0)
        {
            SPEAKER ^= 1;
        }
    }
}
