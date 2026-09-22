#ifndef MAX7219_H
#define MAX7219_H

#include "main.h"
#include "fonts.h" // Include your fonts library

#define MAX7219_NUM_MODULES 2

/* MAX7219 Registers */
#define REG_NOOP         0x00
#define REG_DIGIT0       0x01
#define REG_DECODEMODE   0x09
#define REG_INTENSITY    0x0A
#define REG_SCANLIMIT    0x0B
#define REG_SHUTDOWN     0x0C
#define REG_DISPLAYTEST  0x0F

/* Hardware Functions */
void MAX7219_Init(void);
void MAX7219_SetBrightness(uint8_t brightness);
void MAX7219_Clear(void);
void MAX7219_Update(void);

/* Pixel mapping (Upgraded to int16_t for off-screen rendering) */
void MAX7219_SetPixel(int16_t x, int16_t y, uint8_t state);

/* Text Rendering Functions */
void MAX7219_DrawChar(char ch, int16_t x, int16_t y, FontDef_t* Font);
void MAX7219_DrawString(char* str, int16_t x, int16_t y, FontDef_t* Font);
void MAX7219_ScrollText(char* str, uint32_t speed_ms, FontDef_t* Font);

#endif /* MAX7219_H */
