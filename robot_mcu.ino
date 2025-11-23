#include <Arduino_RouterBridge.h>
#include <Servo.h>

// Configuration
const int MAX_SERVOS = 2; // Left and Right wheels
const int LEFT_SERVO_PIN = 5;
const int RIGHT_SERVO_PIN = 6;
const int SENSOR_PIN = A0; // Placeholder for sensor

Servo leftServo;
Servo rightServo;

// Bridge function to set servo angle (Legacy support, though we mainly use L/R commands now)
void set_servo(int index, int angle) {
  angle = constrain(angle, 0, 180);
  if (index == 0) leftServo.write(angle);
  else if (index == 1) rightServo.write(angle);
  Bridge.returnResult(true);
}

void setup() {
  // Initialize Bridge
  Bridge.begin();
  
  // Attach servos
  leftServo.attach(LEFT_SERVO_PIN);
  rightServo.attach(RIGHT_SERVO_PIN);
  
  // Stop motors initially (90 is usually stop for continuous rotation servos)
  leftServo.write(90);
  rightServo.write(90);
  
  // Register Bridge functions
  Bridge.provide("set_servo", set_servo);
}

void loop() {
  // 1. Ask Python for commands
  String command;
  if (Bridge.call("tick").result(command)) {
    if (command != "NOOP" && command.length() > 0) {
      parseAndExecute(command);
    }
  }
  
  // 2. Read Sensors and send to Python
  int sensorValue = analogRead(SENSOR_PIN);
  // We can send this every loop or periodically. Let's do it every loop for now.
  Bridge.call("update_sensors", sensorValue);
  
  // Small delay
  delay(50);
}

void parseAndExecute(String cmdString) {
  // Format: "L<angle>;R<angle>" e.g. "L100;R80"
  
  int separator = cmdString.indexOf(';');
  if (separator > 0) {
    String leftCmd = cmdString.substring(0, separator);
    String rightCmd = cmdString.substring(separator + 1);
    
    if (leftCmd.startsWith("L")) {
      int angle = leftCmd.substring(1).toInt();
      leftServo.write(constrain(angle, 0, 180));
    }
    
    if (rightCmd.startsWith("R")) {
      int angle = rightCmd.substring(1).toInt();
      rightServo.write(constrain(angle, 0, 180));
    }
  } else {
      // Fallback for single commands if any
  }
}
