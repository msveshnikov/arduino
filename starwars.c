#include <Arduino_LED_Matrix.h>
#include <Modulino.h>

ModulinoBuzzer buzzer;
Arduino_LED_Matrix matrix;

// Star Wars Imperial March notes
const int NOTE_G4 = 392;
const int NOTE_G5 = 784;
const int NOTE_DS5 = 622;
const int NOTE_F5 = 698;
const int NOTE_D5 = 587;
const int NOTE_C5 = 523;
const int NOTE_AS4 = 466;
const int NOTE_GS4 = 415;
const int NOTE_FS5 = 740;
const int NOTE_AS5 = 932;

// Death Star rotation frames (8 frames, 8x13 matrix with 8 shades of gray)
// Frame 0 - Death Star with trench at top
uint8_t deathStar0[104] = {
    0,0,0,0,2,4,5,4,2,0,0,0,0,
    0,0,2,5,6,7,7,7,6,5,2,0,0,
    0,2,5,7,7,7,7,7,7,7,5,2,0,
    2,4,6,7,7,3,3,3,7,7,6,4,2,
    2,5,7,7,7,7,7,7,7,7,7,5,2,
    2,5,7,7,7,7,7,7,7,7,7,5,2,
    0,2,5,7,7,7,7,7,7,7,5,2,0,
    0,0,2,4,6,7,7,7,6,4,2,0,0
};

// Frame 1 - Trench rotated
uint8_t deathStar1[104] = {
    0,0,0,0,2,4,5,4,2,0,0,0,0,
    0,0,2,5,6,7,7,7,6,5,2,0,0,
    0,2,5,7,7,7,7,7,7,7,5,2,0,
    2,4,6,7,7,7,7,7,7,7,6,4,2,
    2,5,7,7,3,3,3,3,3,7,7,5,2,
    2,5,7,7,7,7,7,7,7,7,7,5,2,
    0,2,5,7,7,7,7,7,7,7,5,2,0,
    0,0,2,4,6,7,7,7,6,4,2,0,0
};

// Frame 2 - Trench rotated more
uint8_t deathStar2[104] = {
    0,0,0,0,2,4,5,4,2,0,0,0,0,
    0,0,2,5,6,7,7,7,6,5,2,0,0,
    0,2,5,7,7,7,7,7,7,7,5,2,0,
    2,4,6,7,7,7,7,7,7,7,6,4,2,
    2,5,7,7,7,7,7,7,7,7,7,5,2,
    2,5,7,3,3,3,3,3,3,3,7,5,2,
    0,2,5,7,7,7,7,7,7,7,5,2,0,
    0,0,2,4,6,7,7,7,6,4,2,0,0
};

// Frame 3 - Trench at bottom
uint8_t deathStar3[104] = {
    0,0,0,0,2,4,5,4,2,0,0,0,0,
    0,0,2,5,6,7,7,7,6,5,2,0,0,
    0,2,5,7,7,7,7,7,7,7,5,2,0,
    2,4,6,7,7,7,7,7,7,7,6,4,2,
    2,5,7,7,7,7,7,7,7,7,7,5,2,
    2,5,7,7,7,7,7,7,7,7,7,5,2,
    0,2,5,7,7,3,3,3,7,7,5,2,0,
    0,0,2,4,6,7,7,7,6,4,2,0,0
};

// Frame 4 - Trench rotated
uint8_t deathStar4[104] = {
    0,0,0,0,2,4,5,4,2,0,0,0,0,
    0,0,2,5,6,7,7,7,6,5,2,0,0,
    0,2,5,7,7,7,7,7,7,7,5,2,0,
    2,4,6,7,7,7,7,7,7,7,6,4,2,
    2,5,7,7,7,7,7,7,7,7,7,5,2,
    2,5,7,3,3,3,3,3,3,3,7,5,2,
    0,2,5,7,7,7,7,7,7,7,5,2,0,
    0,0,2,4,6,7,7,7,6,4,2,0,0
};

// Frame 5 - Trench on left side
uint8_t deathStar5[104] = {
    0,0,0,0,2,4,5,4,2,0,0,0,0,
    0,0,2,5,6,7,7,7,6,5,2,0,0,
    0,2,5,7,7,7,7,7,7,7,5,2,0,
    2,4,6,7,7,7,7,7,7,7,6,4,2,
    2,5,7,3,3,3,3,3,7,7,7,5,2,
    2,5,7,7,7,7,7,7,7,7,7,5,2,
    0,2,5,7,7,7,7,7,7,7,5,2,0,
    0,0,2,4,6,7,7,7,6,4,2,0,0
};

// Frame 6 - Trench rotated
uint8_t deathStar6[104] = {
    0,0,0,0,2,4,5,4,2,0,0,0,0,
    0,0,2,5,6,7,7,7,6,5,2,0,0,
    0,2,5,7,7,3,3,3,7,7,5,2,0,
    2,4,6,7,7,7,7,7,7,7,6,4,2,
    2,5,7,7,7,7,7,7,7,7,7,5,2,
    2,5,7,7,7,7,7,7,7,7,7,5,2,
    0,2,5,7,7,7,7,7,7,7,5,2,0,
    0,0,2,4,6,7,7,7,6,4,2,0,0
};

// Frame 7 - Trench on right side
uint8_t deathStar7[104] = {
    0,0,0,0,2,4,5,4,2,0,0,0,0,
    0,0,2,5,6,7,7,7,6,5,2,0,0,
    0,2,5,7,7,7,7,7,7,7,5,2,0,
    2,4,6,7,7,7,7,7,7,7,6,4,2,
    2,5,7,7,7,3,3,3,3,3,7,5,2,
    2,5,7,7,7,7,7,7,7,7,7,5,2,
    0,2,5,7,7,7,7,7,7,7,5,2,0,
    0,0,2,4,6,7,7,7,6,4,2,0,0
};

uint8_t* deathStarFrames[] = {
    deathStar0, deathStar1, deathStar2, deathStar3,
    deathStar4, deathStar5, deathStar6, deathStar7
};

int currentFrame = 0;
unsigned long lastFrameChange = 0;
const unsigned long FRAME_INTERVAL = 200;

void setup() {
    
    matrix.begin();
    matrix.setGrayscaleBits(3); // Enable 8 shades of gray
    Modulino.begin(Wire1);
    buzzer.begin();
}

void playImperialMarch() {
    // Measure 1-2
    buzzer.tone(NOTE_G4, 500);
    updateDeathStar(500);
    buzzer.tone(NOTE_G4, 500);
    updateDeathStar(500);
    buzzer.tone(NOTE_G4, 500);
    updateDeathStar(500);
    
    // Measure 3
    buzzer.tone(NOTE_DS5, 350);
    updateDeathStar(350);
    buzzer.tone(NOTE_AS4, 150);
    updateDeathStar(150);
    
    // Measure 4
    buzzer.tone(NOTE_G4, 500);
    updateDeathStar(500);
    buzzer.tone(NOTE_DS5, 350);
    updateDeathStar(350);
    buzzer.tone(NOTE_AS4, 150);
    updateDeathStar(150);
    
    // Measure 5
    buzzer.tone(NOTE_G4, 1000);
    updateDeathStar(1000);
    
    // Measure 6-7
    buzzer.tone(NOTE_D5, 500);
    updateDeathStar(500);
    buzzer.tone(NOTE_D5, 500);
    updateDeathStar(500);
    buzzer.tone(NOTE_D5, 500);
    updateDeathStar(500);
    
    // Measure 8
    buzzer.tone(NOTE_DS5, 350);
    updateDeathStar(350);
    buzzer.tone(NOTE_AS4, 150);
    updateDeathStar(150);
    
    // Measure 9
    buzzer.tone(NOTE_GS4, 500);
    updateDeathStar(500);
    buzzer.tone(NOTE_DS5, 350);
    updateDeathStar(350);
    buzzer.tone(NOTE_AS4, 150);
    updateDeathStar(150);
    
    // Measure 10
    buzzer.tone(NOTE_G4, 1000);
    updateDeathStar(1000);
}

void updateDeathStar(int duration) {
    unsigned long startTime = millis();
    while (millis() - startTime < duration) {
        if (millis() - lastFrameChange >= FRAME_INTERVAL) {
            matrix.draw(deathStarFrames[currentFrame]);
            currentFrame = (currentFrame + 1) % 8;
            lastFrameChange = millis();
        }
    }
}

void loop() {
    playImperialMarch();
    delay(1000); // Pause before repeating
}