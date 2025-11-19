#include <Arduino_RouterBridge.h>
#include <Servo.h>

// Configuration
const int MAX_SERVOS = 4;
const int SERVO_PINS[MAX_SERVOS] = {3, 5, 6, 9}; 

Servo servos[MAX_SERVOS];

// Bridge function to set servo angle
// Usage from Python: Bridge.call("set_servo", index, angle)
void set_servo(int index, int angle) {
  if (index >= 0 && index < MAX_SERVOS) {
    angle = constrain(angle, 0, 180);
    servos[index].write(angle);
    // Return success
    Bridge.returnResult(true);
  } else {
    // Return failure
    Bridge.returnResult(false);
  }
}

void setup() {
  // Initialize Bridge
  Bridge.begin();
  
  // Attach servos
  for (int i = 0; i < MAX_SERVOS; i++) {
    servos[i].attach(SERVO_PINS[i]);
    servos[i].write(90); // Default to center
  }
  
  // Register Bridge functions
  Bridge.provide("set_servo", set_servo);
}

void loop() {
  // In the Bridge architecture (polling), the MCU usually asks the MPU for commands
  // via Bridge.call(). However, for simple servo control, it's often better
  // if the MPU pushes commands. 
  // BUT, the user asked to follow the `temp.c` example which uses `Bridge.call("get_temperature")`.
  // So we will implement a "tick" where the MCU asks "what should I do?".
  
  String command;
  // Call Python function "tick"
  // Expected return: "S0:90" or "NOOP" or multiple commands separated by ;
  if (Bridge.call("tick").result(command)) {
    if (command != "NOOP" && command.length() > 0) {
      parseAndExecute(command);
    }
  }
  
  // Small delay to prevent flooding
  delay(50);
}

void parseAndExecute(String cmdString) {
  // Simple parser for "S0:90"
  // If multiple commands, we'd need more complex parsing, but let's keep it simple for now
  // or assume Python sends one command per tick for simplicity, or we split by ';'
  
  int separatorIndex = cmdString.indexOf(':');
  if (cmdString.startsWith("S") && separatorIndex > 1) {
    int servoIndex = cmdString.substring(1, separatorIndex).toInt();
    int angle = cmdString.substring(separatorIndex + 1).toInt();
    set_servo(servoIndex, angle);
  }
}
