#include "rfid.h"


enum State {Secure, Armed, Agitated, Breached, Arming};

// pin definitions
sbit LD1 at ODR12_GPIOE_ODR_bit;
sbit LD2 at ODR15_GPIOE_ODR_bit;

sbit SPEAKER at ODR1_GPIOE_ODR_bit;

/*********************************
***         Global Vars        ***
*********************************/
volatile int speakerTurnedOn = 0;
volatile int state = Secure;

const int TONE_DURATION = 2000;
int tone = 0;
int toneCnt = TONE_DURATION;


const int COOLDOWN_SECS = 10;
const int AGITATED_SECS = 5;
int cooldownCnt = AGITATED_SECS;

const int AGITATED_BEEP_CNT = 7;
int agitatedBeepCnt = AGITATED_BEEP_CNT;
const int AGITATED_BEEP_DURATION = 30;
int agitatedBeepDuration = AGITATED_BEEP_DURATION;


void transitionToAgitated(){
     if(state == Secure)
     return;
     
     cooldownCnt = AGITATED_SECS;
     state = Agitated;
     speakerTurnedOn = 1;
     
     agitatedBeepCnt = AGITATED_BEEP_CNT;
     agitatedBeepDuration = AGITATED_BEEP_DURATION;
}

void transitionToBreached(){
     if(state == Secure)
     return;

	cooldownCnt = COOLDOWN_SECS;
     speakerTurnedOn = 1;
     state = Breached;
}

void transitionToSecure(){
     cooldownCnt = COOLDOWN_SECS;
     state = Secure;
     speakerTurnedOn = 0;
}

void transitionToArmed(){
     if(state == Secure)
     return;
     cooldownCnt = AGITATED_SECS;
     state = Armed;
     speakerTurnedOn = 0;
}

void transitionToArming(){
     cooldownCnt = COOLDOWN_SECS;
     state = Arming;
     speakerTurnedOn = 0;
}

/*********************************
***         Timers             ***
*********************************/


// Cooldown

//Timer2 Prescaler :239; Preload = 62499; Actual Interrupt Time = 1 s

//Place/Copy this part in declaration section
void InitTimer2(){
  RCC_APB1ENR.TIM2EN = 1;
  TIM2_CR1.CEN = 0;
  TIM2_PSC = 239;
  TIM2_ARR = 62499;
  NVIC_IntEnable(IVT_INT_TIM2);
  TIM2_DIER.UIE = 1;
  TIM2_CR1.CEN = 1;
}

void Timer2_interrupt() iv IVT_INT_TIM2 {
  TIM2_SR.UIF = 0;
  if(state == Agitated || state == Arming){
                   cooldownCnt--;
                   //speakerTurnedOn = 0;
                   if(cooldownCnt <= 0){
                   
                   if(state == Agitated){
                                           transitionToBreached();
                                           }
                                           else{
                                                transitionToArmed();
                                           }
                   }
  }
}



//Timer3 Prescaler :0; Preload = 3072; Actual Interrupt Time = 204.866666667 us

//Place/Copy this part in declaration section
void InitTimer3(){
  RCC_APB1ENR.TIM3EN = 1;
  TIM3_CR1.CEN = 0;
  TIM3_PSC = 0;
  TIM3_ARR = 3072;
  NVIC_IntEnable(IVT_INT_TIM3);
  TIM3_DIER.UIE = 1;
  TIM3_CR1.CEN = 1;
}

void Timer3_interrupt() iv IVT_INT_TIM3 {
 const int toneId = 0;
  TIM3_SR.UIF = 0;
  
  if(!speakerTurnedOn)
  {
   toneCnt = TONE_DURATION;
   tone = 0;
  }

  if(speakerTurnedOn && tone == toneId && state == Breached)  {
     SPEAKER ^= 1;
     toneCnt--;
     
     if(toneCnt <= 0){
        toneCnt = TONE_DURATION;
        tone = (tone +1 )%2;
     }
  }
}

//Place/Copy this part in declaration section
void InitTimer4(){
  RCC_APB1ENR.TIM4EN = 1;
  TIM4_CR1.CEN = 0;
  TIM4_PSC = 0;
  TIM4_ARR = 6144;
  NVIC_IntEnable(IVT_INT_TIM4);
  TIM4_DIER.UIE = 1;
  TIM4_CR1.CEN = 1;
}

void Timer4_interrupt() iv IVT_INT_TIM4 {
 const int toneId = 1;
  TIM4_SR.UIF = 0;
    if(speakerTurnedOn && tone == toneId && state == Breached)  {
     SPEAKER ^= 1;
     toneCnt--;

     if(toneCnt <= 0){
        toneCnt = TONE_DURATION;
        tone = (tone +1 )%2;
     }
  }

}


//Timer7 Prescaler :0; Preload = 17044; Actual Interrupt Time = 1.136333333 ms

//Place/Copy this part in declaration section
void InitTimer7(){
  RCC_APB1ENR.TIM7EN = 1;
  TIM7_CR1.CEN = 0;
  TIM3_PSC = 0;
  TIM3_ARR = 3072;
  NVIC_IntEnable(IVT_INT_TIM7);
  TIM7_DIER.UIE = 1;
  TIM7_CR1.CEN = 1;
}


void Timer7_interrupt() iv IVT_INT_TIM7 {
  TIM7_SR.UIF = 0;
  if(speakerTurnedOn && state == Agitated)  {

     agitatedBeepDuration--;

     if(agitatedBeepDuration <= 0){
     agitatedBeepDuration = AGITATED_BEEP_DURATION;
         agitatedBeepCnt--;
         if(agitatedBeepCnt < 0){
              agitatedBeepCnt = AGITATED_BEEP_CNT;
         }
     }
     
      if(agitatedBeepCnt <=0 ){
      SPEAKER ^= 1;
      }
  }
}


/********************************
***         Buttons           ***
********************************/

int buttonRisingEdge = 0;
int buttonStateOld = 0;
int buttonStateNew = 0;
int buttonPressed()
{
     return Button(&GPIOA_IDR, 10, 2, 0);
}


/******************************
***         Helpers         ***
******************************/
 /*
void update(){
     // check T2 button
    if (Button(&GPIOA_IDR, 10, 2, 0)) {           // Detect logical zero
      buttonStateOld = 1;                              // Update flag
      
      transitionToArmed();
      
    }
    if (buttonStateOld && Button(&GPIOA_IDR, 10, 2, 1)) {   // Detect zero-to-one transition
      buttonStateOld = 0;
      buttonRisingEdge = 1;
    }
    else{
         buttonRisingEdge = 0;
    }
}
*/



/******************************
***         Main            ***
******************************/

// main function
void main() {

  // Set GPIO_PORTA pin 10 as digital input
  GPIO_Digital_Input(&GPIOA_BASE, _GPIO_PINMASK_10);

  // Set GPIO_PORTE pins 12 and 15 as digital output
  GPIO_Digital_Output(&GPIOE_BASE, _GPIO_PINMASK_12 | _GPIO_PINMASK_15);

  GPIO_Digital_Input(&GPIOE_BASE, _GPIO_PINMASK_6);


  GPIO_Digital_Output(&GPIOE_BASE, _GPIO_PINMASK_1 );
  
  LD1 = 1;
  LD2 = 0;
  
  InitRFID();
  
  SPEAKER = 0;
  
  InitTimer2();
  InitTimer3();
  InitTimer4();
  InitTimer7();
  
  while(1){
           iteration();
  
       LD1 = state != Secure && state != Arming;//iteration();

     LD2 =  state == Arming || state == Agitated; //GPIOE_IDR.B6;
  

    //update();
    
    if(state == Armed && GPIOE_IDR.B6){
                         transitionToAgitated();
    }
    
    if(state == Secure && buttonPressed())  {
      transitionToArming();
     }
  }
}