#ifndef RGB_H
#define RGB_H

#include "stm32f4xx.h"

#define RED     50, 0, 0
#define GREEN   0, 50, 0
#define BLUE    0, 0, 50
#define YELLOW  50, 50, 0
#define WHITE   20, 20, 20
#define OFF     0, 0, 0

void init_rgb(void);
void set_led(int index, uint8_t r, uint8_t g, uint8_t b);
void update_leds(void);
void clear_leds(void);

#endif