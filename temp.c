#include <Arduino_RouterBridge.h>
extern "C" void matrixWrite(const uint32_t *buf);
extern "C" void matrixBegin();

// Larger 6 row x 5 column digit patterns (using bits 0-4)
const uint32_t digit0[] = {
    0x0E, // 01110
    0x11, // 10001
    0x11, // 10001
    0x11, // 10001
    0x11, // 10001
    0x0E, // 01110
    0x00,
    0x00,
};

const uint32_t digit1[] = {
    0x04, // 00100
    0x0C, // 01100
    0x04, // 00100
    0x04, // 00100
    0x04, // 00100
    0x0E, // 01110
    0x00,
    0x00,
};

const uint32_t digit2[] = {
    0x0E, // 01110
    0x11, // 10001
    0x01, // 00001
    0x0E, // 01110
    0x10, // 10000
    0x1F, // 11111
    0x00,
    0x00,
};

const uint32_t digit3[] = {
    0x0E, // 01110
    0x11, // 10001
    0x01, // 00001
    0x0E, // 01110
    0x01, // 00001
    0x1E, // 11110
    0x00,
    0x00,
};

const uint32_t digit4[] = {
    0x11, // 10001
    0x11, // 10001
    0x11, // 10001
    0x1F, // 11111
    0x01, // 00001
    0x01, // 00001
    0x00,
    0x00,
};

const uint32_t digit5[] = {
    0x1F, // 11111
    0x10, // 10000
    0x10, // 10000
    0x1E, // 11110
    0x01, // 00001
    0x1E, // 11110
    0x00,
    0x00,
};

const uint32_t digit6[] = {
    0x0E, // 01110
    0x10, // 10000
    0x10, // 10000
    0x1E, // 11110
    0x11, // 10001
    0x0E, // 01110
    0x00,
    0x00,
};

const uint32_t digit7[] = {
    0x1F, // 11111
    0x01, // 00001
    0x02, // 00010
    0x04, // 00100
    0x08, // 01000
    0x08, // 01000
    0x00,
    0x00,
};

const uint32_t digit8[] = {
    0x0E, // 01110
    0x11, // 10001
    0x11, // 10001
    0x0E, // 01110
    0x11, // 10001
    0x0E, // 01110
    0x00,
    0x00,
};

const uint32_t digit9[] = {
    0x0E, // 01110
    0x11, // 10001
    0x11, // 10001
    0x0F, // 01111
    0x01, // 00001
    0x0E, // 01110
    0x00,
    0x00,
};

// Array of pointers to digit patterns
const uint32_t *digits[10] = {
    digit0, digit1, digit2, digit3, digit4,
    digit5, digit6, digit7, digit8, digit9};

uint32_t displayBuffer[4];

void displayNumber(int num)
{
    if (num < 0)
        num = 0;
    if (num > 99)
        num = 99;

    int tens = num / 10;
    int ones = num % 10;

    // Clear display buffer
    displayBuffer[0] = 0;
    displayBuffer[1] = 0;
    displayBuffer[2] = 0;
    displayBuffer[3] = 0;

    // Reverse bits for horizontal mirroring (bits 0-4 for 5-bit width)
    auto reverseBits5 = [](uint32_t val) -> uint32_t
    {
        return ((val & 0x01) << 4) | ((val & 0x02) << 2) | (val & 0x04) |
               ((val & 0x08) >> 2) | ((val & 0x10) >> 4);
    };

    // Build rows: tens digit in lower 5 bits, 1 space bit, ones digit in upper 5 bits
    // Total: 5 + 1 + 5 = 11 bits per row (fits well in 13-bit columns)
    uint32_t row0 = reverseBits5(digits[tens][0] & 0x1F) | (reverseBits5(digits[ones][0] & 0x1F) << 6);
    uint32_t row1 = reverseBits5(digits[tens][1] & 0x1F) | (reverseBits5(digits[ones][1] & 0x1F) << 6);
    uint32_t row2 = reverseBits5(digits[tens][2] & 0x1F) | (reverseBits5(digits[ones][2] & 0x1F) << 6);
    uint32_t row3 = reverseBits5(digits[tens][3] & 0x1F) | (reverseBits5(digits[ones][3] & 0x1F) << 6);
    uint32_t row4 = reverseBits5(digits[tens][4] & 0x1F) | (reverseBits5(digits[ones][4] & 0x1F) << 6);
    uint32_t row5 = reverseBits5(digits[tens][5] & 0x1F) | (reverseBits5(digits[ones][5] & 0x1F) << 6);
    uint32_t row6 = 0;
    uint32_t row7 = 0;

    // Pack into display buffer (8 rows x 13 columns)
    displayBuffer[0] = row0 | (row1 << 13) | (row2 << 26);
    displayBuffer[1] = (row2 >> 6) | (row3 << 7) | (row4 << 20);
    displayBuffer[2] = (row4 >> 12) | (row5 << 1) | (row6 << 14) | (row7 << 27);
    displayBuffer[3] = (row7 >> 5);

    matrixWrite(displayBuffer);
}

void setup()
{
    matrixBegin();
    Bridge.begin();
}

void loop()
{
    int temperature;
    displayNumber(42);
    bool ok = Bridge.call("get_temperature").result(temperature);

    if (ok)
    {
        if (temperature < 0)
            temperature = 0;
        displayNumber(temperature);
    }
    delay(10000);
}