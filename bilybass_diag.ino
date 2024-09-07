#include <MX1508.h>

// Motor setup
MX1508 headMotor(5, 3);   // Head (mouth) motor on PWM pins 5 and 3
MX1508 neckMotor(10, 11); // Neck motor on PWM pins 10 and 11
MX1508 tailMotor(6, 9);   // Tail motor on PWM pins 6 and 9

// Audio input
const int audioPin = A0;
const int silenceThreshold = 12;

// Variables for motor control
int headSpeed = 0;
int neckSpeed = 0;
int tailSpeed = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  
  // Initialize motors
  headMotor.setSpeed(0);
  neckMotor.setSpeed(0);
  tailMotor.setSpeed(0);

  // Set up audio input
  pinMode(audioPin, INPUT);

  printMenu();
}

void loop() {
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    processCommand(cmd);
  }

  // Continuously check for audio input
  int audioLevel = analogRead(audioPin);
  if (audioLevel > silenceThreshold) {
    Serial.println("Audio detected: " + String(audioLevel));
  }
}

void printMenu() {
  Serial.println("\n===== Billy Bass Diagnostic Menu =====");
  Serial.println("h: Move head (mouth)");
  Serial.println("n: Move neck");
  Serial.println("t: Move tail");
  Serial.println("a: Test audio detection");
  Serial.println("s: Stop all motors");
  Serial.println("?: Print this menu");
  Serial.println("=====================================");
}

void processCommand(char cmd) {
  switch (cmd) {
    case 'h':
      testHead();
      break;
    case 'n':
      testNeck();
      break;
    case 't':
      testTail();
      break;
    case 'a':
      testAudio();
      break;
    case 's':
      stopAllMotors();
      break;
    case '?':
      printMenu();
      break;
    default:
      Serial.println("Unknown command. Type '?' for menu.");
  }
}

void testHead() {
  Serial.println("Testing head (mouth) movement...");
  headMotor.setSpeed(200);
  headMotor.forward();
  delay(1000);
  headMotor.backward();
  delay(1000);
  headMotor.halt();
  Serial.println("Head test complete.");
}

void testNeck() {
  Serial.println("Testing neck movement...");
  neckMotor.setSpeed(200);
  neckMotor.forward();
  delay(1000);
  neckMotor.backward();
  delay(1000);
  neckMotor.halt();
  Serial.println("Neck test complete.");
}

void testTail() {
  Serial.println("Testing tail movement...");
  tailMotor.setSpeed(200);
  tailMotor.forward();
  delay(1000);
  tailMotor.backward();
  delay(1000);
  tailMotor.halt();
  Serial.println("Tail test complete.");
}

void testAudio() {
  Serial.println("Testing audio detection for 5 seconds...");
  unsigned long startTime = millis();
  while (millis() - startTime < 5000) {
    int audioLevel = analogRead(audioPin);
    if (audioLevel > silenceThreshold) {
      Serial.println("Audio detected: " + String(audioLevel));
    }
    delay(100);
  }
  Serial.println("Audio test complete.");
}

void stopAllMotors() {
  headMotor.halt();
  neckMotor.halt();
  tailMotor.halt();
  Serial.println("All motors stopped.");
}