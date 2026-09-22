#include "general.h"


void general_init(void) {
    MAX7219_Init();
    MAX7219_SetBrightness(2); // Keep brightness low for testing over USB/breadboard power
}

void general_run(void) {
    // 1. Rain down at 70ms per step, and fall freely for 45 frames (~3 seconds) before catching
    Matrix_Pattern_RainHearts(90, 100);

    // 2. Hold the completed hearts on the screen for 2 seconds
    HAL_Delay(1000);

    // 3. Slide the two hearts into the center (100ms per shift for a smooth, deliberate slide)
	Matrix_Pattern_MergeHearts(250);

	// 4. Hold the newly merged single giant heart for 1.5 seconds
	HAL_Delay(1500);

    // 5. Clear the screen and smoothly scroll your text (40ms speed)
    MAX7219_Clear();
    MAX7219_ScrollText("HANSIMA", 100, &Font_6x8);

    // 6. Brief pause before the loop restarts
    HAL_Delay(1000);
}
