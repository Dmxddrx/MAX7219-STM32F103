#include "patterns.h"
#include <stdlib.h> // Needed for rand()
#include <stdbool.h>
/* 1. Blink 1 to 128 (Row by Row) */
void Matrix_Pattern_Sequential(uint32_t delay_ms) {
    for (uint8_t y = 0; y < 8; y++) {
        for (uint8_t x = 0; x < 16; x++) {
            MAX7219_Clear();
            MAX7219_SetPixel(x, y, 1);
            MAX7219_Update();
            HAL_Delay(delay_ms);
        }
    }
}

/* 2. Blink Column by Column (Vertical) */
void Matrix_Pattern_Columns(uint32_t delay_ms) {
    for (uint8_t x = 0; x < 16; x++) {
        for (uint8_t y = 0; y < 8; y++) {
            MAX7219_Clear();
            MAX7219_SetPixel(x, y, 1);
            MAX7219_Update();
            HAL_Delay(delay_ms);
        }
    }
}

/* 3. Hourglass Sand Drop (Single Particle falling through pinch) */
void Matrix_Pattern_SandDrop(uint32_t delay_ms) {
    for (uint8_t step = 0; step < 16; step++) {
        MAX7219_Clear();

        if (step < 8) {
            uint8_t top_x = 8 + step;
            uint8_t top_y = step;
            MAX7219_SetPixel(top_x, top_y, 1);
        } else {
            uint8_t bottom_step = step - 8;
            uint8_t bottom_x = 7 - bottom_step;
            uint8_t bottom_y = bottom_step;
            MAX7219_SetPixel(bottom_x, bottom_y, 1);
        }

        MAX7219_Update();
        HAL_Delay(delay_ms);
    }
}

/* 4. Hourglass Horizontal Sweep (Straight Line Down 16 rows) */
void Matrix_Pattern_HorizontalSweep(uint32_t delay_ms) {
    for (uint8_t row = 0; row < 16; row++) {
        MAX7219_Clear();

        if (row < 8) {
            for (uint8_t x = 8; x < 16; x++) {
                MAX7219_SetPixel(x, row, 1);
            }
        } else {
            uint8_t local_y = row - 8;
            for (uint8_t x = 0; x < 8; x++) {
                MAX7219_SetPixel(x, local_y, 1);
            }
        }

        MAX7219_Update();
        HAL_Delay(delay_ms);
    }
}

/* 5. Hourglass Diagonal Sweep (30 Steps matching the glass shape) */
void Matrix_Pattern_HourglassSweep(uint32_t delay_ms) {
    for (uint8_t step = 0; step < 30; step++) {
        MAX7219_Clear();

        if (step < 15) {
            /* Top Half (Module 1) */
            uint8_t d = step;
            for (uint8_t local_x = 0; local_x < 8; local_x++) {
                int8_t local_y = d - local_x;
                if (local_y >= 0 && local_y < 8) {
                    MAX7219_SetPixel(local_x + 8, local_y, 1);
                }
            }
        } else {
            /* Bottom Half (Module 0) */
            uint8_t d = step - 15;
            for (uint8_t local_x = 0; local_x < 8; local_x++) {
                int8_t local_y = d - local_x;
                if (local_y >= 0 && local_y < 8) {
                    MAX7219_SetPixel(local_x, local_y, 1);
                }
            }
        }

        MAX7219_Update();
        HAL_Delay(delay_ms);
    }
}


/* Helper function extracted from WS2812B logic */
static bool IsHeart(uint8_t x, uint8_t y) {
    uint8_t local_x = x % 8; // Automatically duplicates the heart across both modules

    // Heart coordinates mapped from ws2812b_UI.c
    if (y == 1 && (local_x == 1 || local_x == 2 || local_x == 5 || local_x == 6)) return true;
    if (y == 2 || y == 3) return true;
    if (y == 4 && (local_x >= 1 && local_x <= 6)) return true;
    if (y == 5 && (local_x >= 2 && local_x <= 5)) return true;
    if (y == 6 && (local_x == 3 || local_x == 4)) return true;

    return false;
}

/* 6. Rain filling up two hearts */
void Matrix_Pattern_RainHearts(uint32_t delay_ms, uint16_t free_rain_frames) {
    bool is_frozen[16][8] = {false};
    int drop_y[16];
    int frozen_count = 0;
    uint16_t frame_count = 0; // Tracks how long the animation has been running

    // 32 pixels per heart * 2 modules = 64 total pixels to catch
    const int total_pixels = 64;

    // Initialize drop starting positions off-screen
    for (int i = 0; i < 16; i++) {
        drop_y[i] = -(rand() % 15);
    }

    // Keep raining until both hearts are 100% full
    while (frozen_count < total_pixels) {
        MAX7219_Clear();

        // Activate freeze mode only after the initial free-rain period has passed
        bool freeze_mode = (frame_count >= free_rain_frames);

        // 1. Draw already frozen pixels
        for (int x = 0; x < 16; x++) {
            for (int y = 0; y < 8; y++) {
                if (is_frozen[x][y]) {
                    MAX7219_SetPixel(x, y, 1);
                }
            }
        }

        // 2. Process falling rain and catching logic
        for (int col = 0; col < 16; col++) {

            int target_y = -1;

            // Only calculate a target to catch the drop IF freeze mode is active
            if (freeze_mode) {
                for (int y = 7; y >= 0; y--) {
                    if (IsHeart(col, y) && !is_frozen[col][y]) {
                        target_y = y;
                        break;
                    }
                }

                // If the column's heart shape is completely filled, stop raining in this column
                if (target_y == -1) {
                    continue;
                }
            }

            // Move drop down
            drop_y[col]++;

            // Draw the falling drop if it's currently on the screen
            if (drop_y[col] >= 0 && drop_y[col] < 8) {
                MAX7219_SetPixel(col, drop_y[col], 1);
            }

            // Catch logic: If in freeze mode and it hits the target
            if (freeze_mode && drop_y[col] >= target_y) {
                is_frozen[col][target_y] = true;
                frozen_count++;
                drop_y[col] = -(rand() % 8 + 1); // Reset drop back to the top
            }
            // Free rain logic: If it falls off the bottom of the screen
            else if (drop_y[col] >= 8) {
                drop_y[col] = -(rand() % 8 + 1); // Reset drop back to the top
            }
        }

        MAX7219_Update();
        HAL_Delay(delay_ms);
        frame_count++;
    }
}

/* 7. Slide the two hearts together to form one in the center */
void Matrix_Pattern_MergeHearts(uint32_t delay_ms) {
    // 4 shifting steps to push them directly into the center
    for (int step = 1; step <= 4; step++) {
        MAX7219_Clear();

        // Loop through the 8x8 template of a single heart
        for (int y = 0; y < 8; y++) {
            for (int local_x = 0; local_x < 8; local_x++) {

                // If this coordinate is part of the heart shape, draw it for both halves
                if (IsHeart(local_x, y)) {

                    // Left Heart (Module 1): Shift right by 'step'
                    int left_x = local_x + step;
                    MAX7219_SetPixel(left_x, y, 1);

                    // Right Heart (Module 0): Shift left by 'step'
                    int right_x = (local_x + 8) - step;
                    MAX7219_SetPixel(right_x, y, 1);
                }
            }
        }

        MAX7219_Update();
        HAL_Delay(delay_ms); // Controls how fast the slide happens
    }
}
