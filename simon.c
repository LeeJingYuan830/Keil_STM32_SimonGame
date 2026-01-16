#include "stm32f446xx.h"
#include <stdlib.h>

#define pwm_low     6       
#define pwm_high    13      
#define status      2       
#define bright_max  30      
#define bright_min  5       
#define volume      300     
#define SEG_S       0x6D 

volatile uint32_t rgb_buffer[170]; 

const uint8_t SVN_SEG[10] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};
const int LED_MAP[4] = {0, 1, 3, 4}; 
const uint8_t GAME_COLORS[4][3] = {
    {bright_max, 0, 0},             
    {0, bright_max, 0},             
    {0, 0, bright_max},             
    {bright_max, bright_max, 0}     
};
const int SPEED_LEVELS[5] = {300, 250, 200, 120, 80};

void initialize_GPIO(void);
void initialize_RGB(void);
void startup(void);
int DA(int score);
void beep(int duration_ms);        
void clear_buffer(void);
void set_led(int index, uint8_t r, uint8_t g, uint8_t b);
void update_leds(void);
void display_score(int score);
void display_speed_menu(int speed_level); 
void delay_ms(int ms);
int get_pressed_button_index(void);
int check_specific_button(int index);
void wait_and_config_speed(int *current_speed_idx); 

int main(void)
{
    initialize_GPIO();
    initialize_RGB();
    startup();
    update_leds();
    
    SysTick->LOAD = 0xFFFFFF;
    SysTick->CTRL = 5; 
    
    int speed_idx = 2; 

    wait_and_config_speed(&speed_idx);
    
    srand(SysTick->VAL);
    
    beep(100);
    delay_ms(500);

    int score = 0;
    int sequence[50];
    int pattern_len = 2; 
    
    int current_delay = SPEED_LEVELS[speed_idx]; 
    
    while(1)
    {
        display_score(score);
        pattern_len = DA(score);
        current_delay = SPEED_LEVELS[speed_idx]; 
            
        set_led(status, bright_min, 0, bright_min); 
        update_leds();
        delay_ms(500); 
        set_led(status, 0, 0, 0); 
        update_leds();
        delay_ms(200);

        retry: 
        for(int i=0; i<pattern_len; i++) 
            sequence[i] = rand() % 4;

        set_led(status, 0, 0, 0);
        update_leds();
        delay_ms(500); 

        for(int i=0; i<pattern_len; i++) {
            int gamePos = sequence[i];
            int physLed = LED_MAP[gamePos];

            set_led(physLed, GAME_COLORS[gamePos][0], GAME_COLORS[gamePos][1], GAME_COLORS[gamePos][2]);
            update_leds();
            
            delay_ms(current_delay); 

            set_led(physLed, 0, 0, 0);
            update_leds();
            delay_ms(100); 
        }

        set_led(status, bright_min, 0, bright_min); 
        update_leds();
        
        for(int i=0; i<pattern_len; i++) {
            int expected = sequence[i];
            int input = -1;
            
            while(input == -1) {
                input = get_pressed_button_index();
            }
            
            int physLed = LED_MAP[input];
            set_led(physLed, GAME_COLORS[input][0], GAME_COLORS[input][1], GAME_COLORS[input][2]);
            update_leds();
            
            int elapsed = 0;
            int is_hold_action = 0;
            
            while(check_specific_button(input)) {
                delay_ms(10);
                elapsed += 10;
                
                if(elapsed > 1500) {
                    is_hold_action = 1;
                    break; 
                }
            }
            
            set_led(physLed, 0, 0, 0);
            update_leds();
            delay_ms(50);

            if (is_hold_action) {
                set_led(status, bright_max, bright_max, bright_max);
                update_leds();
                beep(200);
                while(check_specific_button(input));
                delay_ms(500);
                set_led(status, 0, 0, 0);
                update_leds();
                goto retry; 
            }
            else {
                if(input != expected) {
                    goto game_over;
                }
            }
        }
        
        score++;
        set_led(status, 0, bright_min, 0); 
        update_leds();
        beep(100); 
        delay_ms(200);
        set_led(status, 0, 0, 0); 
        update_leds();
        delay_ms(300);
        continue; 

        game_over:
        score = 0;
        display_score(score);
        set_led(status, bright_min, 0, 0); 
        
        for(int k=0; k<4; k++) 
            set_led(LED_MAP[k], 0, 0, 0);
        
        update_leds();
        beep(500);        
        delay_ms(1000); 
        set_led(status, 0, 0, 0); 
        update_leds();
        
        wait_and_config_speed(&speed_idx);
        
        beep(100);
        delay_ms(500); 
    }
}

void wait_and_config_speed(int *current_speed_idx) {
    display_score(0); 
    
    while(1) {
        int btn = get_pressed_button_index();
        
        if (btn != -1) {
            int hold_time = 0;
            int entered_config = 0;
            
            while(check_specific_button(btn)) {
                delay_ms(10);
                hold_time += 10;
                
                if (hold_time > 1000) {
                    entered_config = 1;
                    beep(200); 
                    
                    while(check_specific_button(btn)); 
                    delay_ms(100); 
                    break; 
                }
            }

            if (!entered_config) {
                return; 
            }
            
            while(1) {
                display_speed_menu(*current_speed_idx + 1);
                
                int menu_btn = get_pressed_button_index();
                
                if (menu_btn != -1) {
                    int menu_hold_time = 0;
                    int exit_menu = 0;
                    
                    while(check_specific_button(menu_btn)) {
                        delay_ms(10);
                        menu_hold_time += 10;
                        
                        if (menu_hold_time > 1000) {
                            exit_menu = 1;
                            beep(200); 
                            while(check_specific_button(menu_btn));
                            break;
                        }
                    }
                    
                    if (exit_menu) {
                        display_score(0); 
                        delay_ms(200);
                        break; 
                    } else {
                        (*current_speed_idx)++;
                        if (*current_speed_idx > 4) *current_speed_idx = 0;
                        
                        beep(50); 
                        delay_ms(200); 
                    }
                }
            }
        }
    }
}

void display_speed_menu(int level) {
    GPIOC->ODR = (SEG_S << 8) | SVN_SEG[level];
}

void update_leds(void) {
    delay_ms(1); 
    DMA1->LIFCR |= 0xF80; 
    DMA1_Stream1->NDTR = 170; 
    DMA1_Stream1->CR |= 1;      
    TIM2->CR1 |= 1;             
    while(DMA1_Stream1->NDTR > 0); 
}

void beep(int duration_ms) {
    for(int i=0; i<duration_ms; i++) {
        GPIOA->ODR |= (1<<10);
        for(volatile int j=0; j<volume; j++); 
        GPIOA->ODR &= ~(1<<10);
        for(volatile int j=0; j<(3200 - volume); j++); 
    }
}

void clear_buffer(void) {
    for(int i=0; i<170; i++) rgb_buffer[i] = 0;
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

void display_score(int score) {
    if(score > 99) score = 99;
    int tens = score / 10;
    int units = score % 10;
    GPIOC->ODR = (SVN_SEG[tens] << 8) | SVN_SEG[units];
}

void delay_ms(int ms) {
    for(int i=0; i<ms; i++) {
        for(volatile int j=0; j<3200; j++);
    }
}

void set_led(int index, uint8_t r, uint8_t g, uint8_t b) {
    uint32_t color = (g << 16) | (r << 8) | b;
    int start = index * 24;
    for(int i=0; i<24; i++) {
        rgb_buffer[start + i] = (color & (1<<(23-i))) ? pwm_high : pwm_low;
    }
}

void initialize_GPIO(void)
{
    RCC->AHB1ENR |= (1<<2) | (1<<1) | (1<<0); 
    
    GPIOA->MODER &= ~((3<<(2*2)) | (3<<(3*2)) | (3<<(10*2))); 
    GPIOA->MODER |= (1<<(10*2)); 
    
    GPIOB->MODER &= ~((1<<(0*2)) | (1<<(1*2)));
    GPIOB->MODER |= 2<<(10*2);  
    
    GPIOC->MODER |= 0x55555555;
    
    GPIOA->PUPDR &= ~((3<<(2*2)) | (3<<(3*2)));
    GPIOB->PUPDR &= ~((3<<(0*2)) | (3<<(1*2)));
    GPIOA->PUPDR |= (2<<(2*2)) | (2<<(3*2)); 
    GPIOB->PUPDR |= (1<<(0*2)) | (1<<(1*2)); 
    
    GPIOA->ODR &= ~(1<<10); 
}

void initialize_RGB(void)
{
    RCC->APB1ENR |= (1<<0);  
    RCC->AHB1ENR |= (1<<21); 

    GPIOB->AFR[1] |= (1<<8); 

    TIM2->PSC = 0;
    TIM2->ARR = 19; 
    TIM2->CCMR2 |= (6<<4) | (1<<3);
    TIM2->CCER |= (1<<8);
    TIM2->DIER |= (1<<8);

    DMA1_Stream1->PAR = (uint32_t)&TIM2->CCR3;
    DMA1_Stream1->M0AR = (uint32_t)rgb_buffer;
    DMA1_Stream1->CR = (3<<25) | (2<<13) | (2<<11) | (1<<10) | (1<<6) | (1<<4);
    
    NVIC_EnableIRQ(DMA1_Stream1_IRQn);
}

void startup(void)
{
    GPIOB->MODER &= ~(3<<(10*2));   
    GPIOB->MODER |= (1<<(10*2));    
    GPIOB->ODR &= ~(1<<10);         
    delay_ms(10);                   
    GPIOB->MODER &= ~(3<<(10*2));   
    GPIOB->MODER |= (2<<(10*2));    
    
    clear_buffer();
    update_leds();
    delay_ms(100);
    beep(50); delay_ms(50); beep(50);
    for(int i=0; i<5; i++) {
        set_led(i, 5, 5, 5); 
        update_leds();
        delay_ms(100);
        set_led(i, 0, 0, 0); 
    }
}

int DA(int score)
{
    if (score < 3) return 2;
    else if (score < 10) return 3;
    else if (score < 20) return 4;
    else return 5;
}

void DMA1_Stream1_IRQHandler(void)
{
    if (DMA1->LISR & (1<<11)) {     
        DMA1->LIFCR |= (1<<11);     
        DMA1_Stream1->CR &= ~1;     
        TIM2->CR1 &= ~1;            
        TIM2->CCR3 = 0;             
    }

}
