#include "stm32f4xx.h"
#include <stdlib.h>  
#include "input.h"   
#include "display.h" 
#include "rgb.h"     


void delay_ms(int ms) {
    for(int i=0; i<ms; i++) {
        for(volatile int j=0; j<3200; j++);
    }
}


const int SPEED_LEVELS[5] = {300, 250, 200, 120, 80}; 
const int LED_MAP[4] = {0, 1, 3, 4}; 

int main(void) {
    init_input_gpio();    // Member 1 Work
    init_display_gpio();  // Member 2 Work
    init_rgb();           // Member 3 Work
    
    SysTick->LOAD = 0xFFFFFF;
    SysTick->CTRL = 5;

    display_score(88); 
    for(int i=0; i<5; i++) {
        set_led(i, 20, 20, 20); 
        update_leds();
        beep(50);
        delay_ms(100);
        set_led(i, 0, 0, 0);
    }
    update_leds();
    display_score(0);

    int score = 0;
    int speed_idx = 1; 
    int sequence[50];  
    
    while(1) {
        display_speed_menu(speed_idx + 1);
        while(get_pressed_button_index() == -1); 
        delay_ms(500); // 防抖
        
        srand(SysTick->VAL); 
        score = 0;
        

        while(1) {
            display_score(score);
            int pattern_len = (score < 3) ? 2 : (score < 10 ? 3 : 5); 
            int current_delay = SPEED_LEVELS[speed_idx];

            for(int i=0; i<pattern_len; i++) {
                sequence[i] = rand() % 4; 
            }

            delay_ms(500);
            for(int i=0; i<pattern_len; i++) {
                int color_idx = sequence[i];
                int led_idx = LED_MAP[color_idx];
                
                if(color_idx==0) set_led(led_idx, RED);
                if(color_idx==1) set_led(led_idx, GREEN);
                if(color_idx==2) set_led(led_idx, BLUE);
                if(color_idx==3) set_led(led_idx, YELLOW);
                
                update_leds();
                beep(100);
                delay_ms(current_delay);
                
                set_led(led_idx, OFF);
                update_leds();
                delay_ms(100);
            }

            for(int i=0; i<pattern_len; i++) {
                int expected = sequence[i];
                int input = -1;
                
                while(input == -1) {
                    input = get_pressed_button_index();
                }
                
                int led_idx = LED_MAP[input];
                set_led(led_idx, 20, 20, 20); 
                update_leds();
                beep(50);
                
                while(check_specific_button(input)); 
                set_led(led_idx, OFF);
                update_leds();

                if(input != expected) {
                    goto GAME_OVER; 
                }
            }
            
            score++;
            delay_ms(500);
        }

        
        GAME_OVER:
        display_score(score); 
        for(int k=0; k<3; k++) {
            for(int j=0; j<5; j++) set_led(j, RED);
            update_leds();
            beep(300);
            clear_leds();
            delay_ms(200);
        }
        delay_ms(1000); 
    }
}