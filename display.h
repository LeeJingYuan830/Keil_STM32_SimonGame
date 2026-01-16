#ifndef DISPLAY_H
#define DISPLAY_H

#include "stm32f4xx.h"
void init_display_gpio(void);       
void display_score(int score);      
void display_speed_menu(int level); 

#endif