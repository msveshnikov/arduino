#include <Arduino_LED_Matrix.h>
#include <Arduino_RouterBridge.h>
#include <Modulino.h>

ModulinoBuzzer buzzer;
ModulinoKnob knob;
Arduino_LED_Matrix matrix;

// Matrix dimensions
const int COLS = 13;
const int ROWS = 8;

// Frame buffer for grayscale values (0-7)
uint8_t frame[ROWS * COLS];

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

// Death Star parameters
const float CENTER_X = 6.0;
const float CENTER_Y = 3.5;
const float RADIUS = 3.5;
const float TRENCH_WIDTH = 0.6;
const float SUPERLASER_RADIUS = 0.8;

bool music = false;
// Knob control
bool autoRotate = false;
int lastKnobValue = 0;

void set_led_state(bool state)
{
    // LOW state means LED is ON
    music = state ;
}

// Animation state
float rotationAngle = 0;
const float ROTATION_SPEED = 0.15; // Radians per frame
unsigned long lastFrameUpdate = 0;
const unsigned long FRAME_INTERVAL = 50; // 20 FPS



// Helper function to calculate distance from point to line segment
float distanceToLineSegment(float px, float py, float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    float lenSq = dx * dx + dy * dy;

    if (lenSq == 0)
        return sqrt((px - x1) * (px - x1) + (py - y1) * (py - y1));

    float t = max(0.0f, min(1.0f, ((px - x1) * dx + (py - y1) * dy) / lenSq));
    float projX = x1 + t * dx;
    float projY = y1 + t * dy;

    return sqrt((px - projX) * (px - projX) + (py - projY) * (py - projY));
}

void generateDeathStarFrame()
{
    memset(frame, 0, sizeof(frame));

    // Calculate trench position based on rotation
    float trenchX1 = CENTER_X + RADIUS * cos(rotationAngle);
    float trenchY1 = CENTER_Y + RADIUS * sin(rotationAngle);
    float trenchX2 = CENTER_X - RADIUS * cos(rotationAngle);
    float trenchY2 = CENTER_Y - RADIUS * sin(rotationAngle);

    // Calculate superlaser dish position (opposite side of trench)
    float dishAngle = rotationAngle + PI;
    float dishX = CENTER_X + (RADIUS * 0.7) * cos(dishAngle);
    float dishY = CENTER_Y + (RADIUS * 0.7) * sin(dishAngle);

    for (int row = 0; row < ROWS; row++)
    {
        for (int col = 0; col < COLS; col++)
        {
            float x = col;
            float y = row;

            // Distance from center
            float dx = x - CENTER_X;
            float dy = y - CENTER_Y;
            float distFromCenter = sqrt(dx * dx + dy * dy);

            int idx = row * COLS + col;

            // Check if pixel is within Death Star sphere
            if (distFromCenter <= RADIUS)
            {
                // Calculate 3D depth effect (simulated sphere shading)
                float normalizedDist = distFromCenter / RADIUS;

                // Lambertian shading - brighter at center, darker at edges
                float lighting = 1.0 - (normalizedDist * normalizedDist * 0.7);

                // Base brightness based on sphere geometry
                uint8_t baseShade = (uint8_t)(lighting * 7);

                // Distance to trench line
                float distToTrench = distanceToLineSegment(x, y, trenchX1, trenchY1, trenchX2, trenchY2);

                // Distance to superlaser dish
                float distToDish = sqrt((x - dishX) * (x - dishX) + (y - dishY) * (y - dishY));

                // Darken the trench
                if (distToTrench < TRENCH_WIDTH && distFromCenter < RADIUS - 0.3)
                {
                    baseShade = max(0, baseShade - 4);
                }

                // Darken the superlaser dish (concave depression)
                if (distToDish < SUPERLASER_RADIUS)
                {
                    float dishDepth = 1.0 - (distToDish / SUPERLASER_RADIUS);
                    baseShade = max(0, (int)(baseShade * (0.3 + dishDepth * 0.4)));
                }

                // Add subtle surface detail variation
                if ((col + row) % 3 == 0 && distFromCenter > RADIUS * 0.4)
                {
                    baseShade = max(0, baseShade - 1);
                }

                // Edge darkening for sphere effect
                if (distFromCenter > RADIUS - 0.5)
                {
                    float edgeFade = (RADIUS - distFromCenter) / 0.5;
                    baseShade = (uint8_t)(baseShade * edgeFade);
                }

                frame[idx] = baseShade;
            }
        }
    }
}

void updateDeathStar(int duration)
{
    unsigned long startTime = millis();
    while (millis() - startTime < duration)
    {
        if (millis() - lastFrameUpdate >= FRAME_INTERVAL)
        {
            generateDeathStarFrame();
            matrix.draw(frame);

            // Auto-rotate during music
            rotationAngle += ROTATION_SPEED;
            if (rotationAngle > TWO_PI)
                rotationAngle -= TWO_PI;

            lastFrameUpdate = millis();
        }
    }
}

void playImperialMarch()
{
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

void setup()
{
    Bridge.begin();
    Bridge.provide("set_led_state", set_led_state);

    matrix.begin();
    matrix.setGrayscaleBits(3); // Enable 8 shades of gray (0-7)
    Modulino.begin(Wire1);
    buzzer.begin();
    knob.begin();
    Monitor.begin(9600);

    // Clear frame buffer
    memset(frame, 0, sizeof(frame));

    Monitor.println("Death Star Control:");
    Monitor.println("- Turn knob to rotate manually");
    Monitor.println("- Press knob to toggle auto-rotation");
    Monitor.println("- Music plays with auto-rotation");
}

void loop()
{
    // Update knob state
    int knobValue = knob.get();

    // Check for button press to toggle auto-rotation
    if (knob.isPressed())
    {
        autoRotate = !autoRotate;
        Monitor.print("Auto-rotate: ");
        Monitor.println(autoRotate ? "ON" : "OFF");
        delay(300); // Debounce
    }

    // Manual rotation when not auto-rotating and not playing music
    if (!autoRotate && !music)
    {
        // Map knob value (0-100) to full rotation (0 to 2*PI)
        rotationAngle = (knobValue / 100.0) * TWO_PI;

        // Only update display if knob value changed
        if (abs(knobValue - lastKnobValue) > 1)
        {
            generateDeathStarFrame();
            matrix.draw(frame);
            lastKnobValue = knobValue;
        }
    }

    // Auto-rotation when enabled and not playing music
    if (autoRotate && !music)
    {
        if (millis() - lastFrameUpdate >= FRAME_INTERVAL)
        {
            generateDeathStarFrame();
            matrix.draw(frame);
            rotationAngle += ROTATION_SPEED;
            if (rotationAngle > TWO_PI)
                rotationAngle -= TWO_PI;
            lastFrameUpdate = millis();
        }
    }

    // Play music (overrides other rotation modes)
    if (music)
    {
        Monitor.println("Playing Imperial March with Death Star animation");
        playImperialMarch();
     //   music = false; // Reset after playing
    }
}