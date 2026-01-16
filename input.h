#ifndef INPUT_H
#define INPUT_H

#include "stm32f4xx.h" 

void init_input_gpio(void);       
void beep(int duration_ms);       
int get_pressed_button_index(void); 
int check_specific_button(int index);

#endif