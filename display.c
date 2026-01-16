#include "display.h"

const uint8_t SVN_SEG[10] = {
    0x3F, //0
    0x06, //1
    0x5B, //2
    0x4F, //3
    0x66, //4
    0x6D, //5
    0x7D, //6
    0x07, //7
    0x7F, //8
    0x6F  //9
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
