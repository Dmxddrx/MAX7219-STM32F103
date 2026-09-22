#include "max7219.h"
#include <string.h>

extern SPI_HandleTypeDef hspi1;

/* Frame buffer: [Module][Row] */
static uint8_t frame_buffer[MAX7219_NUM_MODULES][8] = {0};

/* Internal function to send a command to all chained modules */
static void MAX7219_WriteBroadcast(uint8_t reg, uint8_t data) {
    HAL_GPIO_WritePin(MAX7219_CS_GPIO_Port, MAX7219_CS_Pin, GPIO_PIN_RESET);
    for (uint8_t i = 0; i < MAX7219_NUM_MODULES; i++) {
        uint8_t spiData[2] = {reg, data};
        HAL_SPI_Transmit(&hspi1, spiData, 2, HAL_MAX_DELAY);
    }
    HAL_GPIO_WritePin(MAX7219_CS_GPIO_Port, MAX7219_CS_Pin, GPIO_PIN_SET);
}

void MAX7219_Init(void) {
    MAX7219_WriteBroadcast(REG_SHUTDOWN, 0x01);
    MAX7219_WriteBroadcast(REG_DECODEMODE, 0x00);
    MAX7219_WriteBroadcast(REG_SCANLIMIT, 0x07);
    MAX7219_WriteBroadcast(REG_DISPLAYTEST, 0x00);
    MAX7219_WriteBroadcast(REG_INTENSITY, 0x04);

    MAX7219_Clear();
    MAX7219_Update();
}

void MAX7219_SetBrightness(uint8_t brightness) {
    if (brightness > 15) brightness = 15;
    MAX7219_WriteBroadcast(REG_INTENSITY, brightness);
}

void MAX7219_Clear(void) {
    for (uint8_t m = 0; m < MAX7219_NUM_MODULES; m++) {
        for (uint8_t row = 0; row < 8; row++) {
            frame_buffer[m][row] = 0x00;
        }
    }
}

void MAX7219_SetPixel(int16_t x, int16_t y, uint8_t state) {
    // Bounds check to ensure we only draw visible pixels
    if (x < 0 || x >= (MAX7219_NUM_MODULES * 8) || y < 0 || y >= 8) return;

    /* --- CONTIGUOUS DISPLAY PATCH ---
     * Reverses the module order so X=0 starts on the far-left module (Module 1)
     * and X=15 ends on the far-right module (Module 0).
     */
    uint8_t module = (MAX7219_NUM_MODULES - 1) - (x / 8);

    uint8_t bit_pos = 7 - (x % 8);

    if (state) {
        frame_buffer[module][y] |= (1 << bit_pos);
    } else {
        frame_buffer[module][y] &= ~(1 << bit_pos);
    }
}

void MAX7219_Update(void) {
    for (uint8_t row = 0; row < 8; row++) {
        HAL_GPIO_WritePin(MAX7219_CS_GPIO_Port, MAX7219_CS_Pin, GPIO_PIN_RESET);

        for (int8_t m = MAX7219_NUM_MODULES - 1; m >= 0; m--) {
            uint8_t reg = REG_DIGIT0 + row;
            uint8_t data = frame_buffer[m][row];
            uint8_t spiData[2] = {reg, data};
            HAL_SPI_Transmit(&hspi1, spiData, 2, HAL_MAX_DELAY);
        }

        HAL_GPIO_WritePin(MAX7219_CS_GPIO_Port, MAX7219_CS_Pin, GPIO_PIN_SET);
    }
}

/* --- Font & Text Rendering Functions --- */

void MAX7219_DrawChar(char ch, int16_t x, int16_t y, FontDef_t* Font) {
    // Check if character is printable
    if (ch < 32 || ch > 126) return;

    // Find the starting index for this character's data
    uint32_t char_offset = (ch - 32) * Font->FontHeight;

    // Iterate through the font array rows
    for (uint8_t row = 0; row < Font->FontHeight; row++) {
        uint16_t row_data = Font->data[char_offset + row];

        // Iterate through the font array columns
        for (uint8_t col = 0; col < Font->FontWidth; col++) {
            // The MSB (0x8000) represents the leftmost pixel
            if (row_data & (0x8000 >> col)) {
                MAX7219_SetPixel(x + col, y + row, 1);
            }
        }
    }
}

void MAX7219_DrawString(char* str, int16_t x, int16_t y, FontDef_t* Font) {
    while (*str) {
        MAX7219_DrawChar(*str, x, y, Font);
        x += Font->FontWidth; // Move cursor right by font width for next char
        str++;
    }
}

void MAX7219_ScrollText(char* str, uint32_t speed_ms, FontDef_t* Font) {
    // Calculate total pixel width of the entire string
    int16_t text_length = strlen(str) * Font->FontWidth;

    // Start drawing just off the right edge of the chained matrices
    int16_t start_x = MAX7219_NUM_MODULES * 8;

    // Stop drawing when the last pixel of the string slides off the left edge
    int16_t end_x = -text_length;

    for (int16_t x = start_x; x > end_x; x--) {
        MAX7219_Clear();
        MAX7219_DrawString(str, x, 0, Font);
        MAX7219_Update();
        HAL_Delay(speed_ms);
    }
}
