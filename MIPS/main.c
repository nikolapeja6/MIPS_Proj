// pin definitions
sbit LD1 at ODR12_GPIOE_ODR_bit;
sbit LD2 at ODR15_GPIOE_ODR_bit;

sbit SPEAKER at ODR1_GPIOE_ODR_bit;


/*********************************
***         Global Vars        ***
*********************************/
int speakerTurnedOn = 0;


/*********************************
***         Timers             ***
*********************************/

//Timer2 Prescaler :959; Preload = 62499; Actual Interrupt Time = 500 ms
void InitTimer2(){
  RCC_APB1ENR.TIM2EN = 1;
  TIM2_CR1.CEN = 0;
  TIM2_PSC = 119;//959;
  TIM2_ARR = 62499;
  NVIC_IntEnable(IVT_INT_TIM2);
  TIM2_DIER.UIE = 1;
  TIM2_CR1.CEN = 1;
}

// ISR
void Timer2_interrupt() iv IVT_INT_TIM2 {
  TIM2_SR.UIF = 0;
  
     //LD1 ^= 1;
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
  TIM3_SR.UIF = 0;
  if(speakerTurnedOn)
     SPEAKER ^= 1;
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

void update(){
     // check T2 button
    if (Button(&GPIOA_IDR, 10, 2, 0)) {           // Detect logical zero
      buttonStateOld = 1;                              // Update flag
    }
    if (buttonStateOld && Button(&GPIOA_IDR, 10, 2, 1)) {   // Detect zero-to-one transition
      buttonStateOld = 0;
      buttonRisingEdge = 1;
    }
    else{
         buttonRisingEdge = 0;
    }
}

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
  
  SPEAKER = 0;
  
  InitTimer2();
  InitTimer3();
  
  while(1){

    update();
    
    speakerTurnedOn = (speakerTurnedOn + buttonRisingEdge)%2;


    if (buttonPressed()) {
        LD1 = 1;
    }
    else{
         LD1 = 0;
    }
                     
   LD2 =  GPIOE_IDR.B6;
    
    
  }
  
}