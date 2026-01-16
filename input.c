#include "input.h"

void init_input_gpio(void)
{
    
    RCC->AHB1ENR |= (1<<0) | (1<<1); 
    
    
    GPIOA->MODER &= ~((3<<(2*2)) | (3<<(3*2))); 
    GPIOA->PUPDR &= ~((3<<(2*2)) | (3<<(3*2)));
    GPIOA->PUPDR |= (2<<(2*2)) | (2<<(3*2)); 
    
    GPIOB->MODER &= ~((3<<(0*2)) | (3<<(1*2)));
    GPIOB->PUPDR &= ~((3<<(0*2)) | (3<<(1*2))); 
    GPIOB->PUPDR |= (1<<(0*2)) | (1<<(1*2)); 

    GPIOA->MODER &= ~(3<<(10*2)); 
    GPIOA->MODER |= (1<<(10*2)); 
    GPIOA->ODR &= ~(1<<10); 
}

void beep(int duration_ms) {
    int volume = 300; 

    for(int i=0; i<duration_ms; i++) {
        GPIOA->ODR |= (1<<10);
        for(volatile int j=0; j<volume; j++); 
        GPIOA->ODR &= ~(1<<10);
        for(volatile int j=0; j<(3200 - volume); j++); 
    }
}

int get_pressed_button_index(void) {
    if((GPIOA->IDR & (1<<2))) return 0; 
    else if((GPIOA->IDR & (1<<3))) return 1;
    else if(!(GPIOB->IDR & (1<<0))) return 2; 
    else if(!(GPIOB->IDR & (1<<1))) return 3; 
    return -1;
}

int check_specific_button(int index) {
    if(index == 0) return (GPIOA->IDR & (1<<2));  
    else if(index == 1) return (GPIOA->IDR & (1<<3));  
    else if(index == 2) return !(GPIOB->IDR & (1<<0)); 
    else if(index == 3) return !(GPIOB->IDR & (1<<1)); 
    else return 0;
}
