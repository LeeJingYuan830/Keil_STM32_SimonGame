#include "rgb.h"

#define PWM_LOW   6   
#define PWM_HIGH  13  
#define LED_COUNT 5   


volatile uint32_t rgb_buffer[LED_COUNT * 24 + 50]; 

void init_rgb(void) {
    RCC->AHB1ENR |= (1<<1) | (1<<21); // GPIOB + DMA1
    RCC->APB1ENR |= (1<<0);           // TIM2

    
    GPIOB->MODER &= ~(3<<(10*2));   
    GPIOB->MODER |= (2<<(10*2));    
    GPIOB->AFR[1] |= (1<<8);        // AF1 (0001) at pin 10

    
    TIM2->PSC = 0;                  
    TIM2->ARR = 19;                 
    TIM2->CCMR2 |= (6<<4) | (1<<3); 
    TIM2->CCER |= (1<<8);           
    TIM2->DIER |= (1<<8);     
      
    
    DMA1_Stream1->PAR = (uint32_t)&TIM2->CCR3;   
    DMA1_Stream1->M0AR = (uint32_t)rgb_buffer;   
    
    DMA1_Stream1->CR = (3<<25) | (0<<13) | (2<<11) | (1<<10) | (1<<6) | (1<<4);
}

void set_led(int index, uint8_t r, uint8_t g, uint8_t b) {
    
    uint32_t color = (g << 16) | (r << 8) | b;
    int start = index * 24;
    
    for(int i=0; i<24; i++) {
        
        if (color & (1<<(23-i))) 
            rgb_buffer[start + i] = PWM_HIGH;
        else 
            rgb_buffer[start + i] = PWM_LOW;
    }
}

void update_leds(void) {
    
    DMA1->LIFCR |= 0xF80;           
    DMA1_Stream1->NDTR = LED_COUNT * 24 + 50; 
    DMA1_Stream1->CR |= 1;          
    TIM2->CR1 |= 1;                 
    
    
    while(DMA1_Stream1->NDTR > 0);
    
    
    TIM2->CR1 &= ~1;
    DMA1_Stream1->CR &= ~1;
    TIM2->CCR3 = 0; 
}

void clear_leds(void) {
    for(int i=0; i<LED_COUNT; i++) {
        set_led(i, 0, 0, 0);
    }
    update_leds();
}