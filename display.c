#include "display.h"

const uint8_t SVN_SEG[10] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

#define SEG_S 0x6D 

void init_display_gpio(void) {
    RCC->AHB1ENR |= (1<<2); 
    
    GPIOC->MODER |= 0x55555555;
    
    GPIOC->ODR = 0;
}


void display_score(int score) {
    
    if(score > 99) score = 99;
    
    
    int tens = score / 10;
    int units = score % 10;
    
    GPIOC->ODR = (SVN_SEG[tens] << 8) | SVN_SEG[units];
}


void display_speed_menu(int level) {
    GPIOC->ODR = (SEG_S << 8) | SVN_SEG[level];
}