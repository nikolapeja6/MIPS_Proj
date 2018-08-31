// pin definitions
sbit LD1 at ODR12_GPIOE_ODR_bit;
sbit LD2 at ODR15_GPIOE_ODR_bit;

// main function
void main() {
  // Set GPIO_PORTE pin 0 as digital input
  GPIO_Digital_Input(&GPIOE_BASE, _GPIO_PINMASK_0);

  // Set GPIO_PORTA pin 10 as digital input
  GPIO_Digital_Input(&GPIOA_BASE, _GPIO_PINMASK_10);

  // Set GPIO_PORTE pins 12 and 15 as digital output
  GPIO_Digital_Output(&GPIOE_BASE, _GPIO_PINMASK_12 | _GPIO_PINMASK_15);
  
  LD1 = 1;
  LD2 = 1;
}