#ifndef PATTERNS_H
#define PATTERNS_H

#include <stdint.h>
#include "max7219.h"
#include "stm32f1xx_hal.h"

/* --- Pattern Function Prototypes --- */

// Standard 16x8 matrix patterns
void Matrix_Pattern_Sequential(uint32_t delay_ms);
void Matrix_Pattern_Columns(uint32_t delay_ms);

// Hourglass specific patterns
void Matrix_Pattern_SandDrop(uint32_t delay_ms);
void Matrix_Pattern_HorizontalSweep(uint32_t delay_ms);
void Matrix_Pattern_HourglassSweep(uint32_t delay_ms);

// Heart Rain Animation
void Matrix_Pattern_RainHearts(uint32_t delay_ms, uint16_t free_rain_frames);
// Heart Merge Animation
void Matrix_Pattern_MergeHearts(uint32_t delay_ms);

#endif /* PATTERNS_H */
