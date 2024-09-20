#include <MX1508.h>

// Declare motors for each part of the fish
MX1508 bodyMotor(6, 9);   // Body motor on PWM pins 6 and 9
MX1508 mouthMotor(5, 3);  // Mouth motor on PWM pins 5 and 3
MX1508 headMotor(10, 11); // Head motor on PWM pins 10 and 11

// Sound input pins for left and right channels
const int soundPinLeft = A0;
const int soundPinRight = A1;

// Threshold for "silence". Anything below this level is ignored.
int silence = 12;
int bodySpeed = 0; // Body motor speed initialized to 0
int headSpeed = 0; // Head motor speed initialized to 0
int soundVolumeLeft = 0;  // Variable to hold the left channel audio value
int soundVolumeRight = 0; // Variable to hold the right channel audio value
int soundVolume = 0;      // Combined audio value
int fishState = 0;        // Variable to indicate the state Billy is in

bool talking = false;     // Indicates whether the fish should be talking or not
bool debugMode = false;   // Flag to indicate if debug mode is active

// Variables for timing
unsigned long currentTime;
unsigned long mouthActionTime = 0;
unsigned long bodyActionTime = 0;
unsigned long headActionTime = 0;
unsigned long nextFlapTime = 0;

void setup() {
  // Make sure all motor speeds are set to zero
  bodyMotor.setSpeed(0);
  mouthMotor.setSpeed(0);
  headMotor.setSpeed(0);

  // Input mode for sound pins
  pinMode(soundPinLeft, INPUT);
  pinMode(soundPinRight, INPUT);

  // Start Serial communication
  Serial.begin(9600);

  // Initialize nextFlapTime
  nextFlapTime = millis() + random(30000, 60000); // Next flap in 30 to 60 seconds

  // Print welcome message and instructions
  Serial.println("Billy Bass Debug Mode");
  Serial.println("Type 'debug' to enter debug mode.");
  Serial.println("Type 'exit' to exit debug mode.");
}

void loop() {
  currentTime = millis(); // Update the time each time the loop is run

  // Check for serial input
  checkSerialInput();

  if (debugMode) {
    // In debug mode, handle motor testing
    debugMenu();
  } else {
    updateSoundInput();     // Update the volume levels detected
    SMBillyBass();          // State machine to control the fish
  }
}

void SMBillyBass() {
  switch (fishState) {
    case 0: // START & WAITING
      if (soundVolume > silence) { // If we detect audio input above the threshold
        if (currentTime > mouthActionTime) { // And if we haven't yet scheduled a mouth movement
          talking = true; // Set talking to true and schedule the mouth movement action
          mouthActionTime = currentTime + 100;
          fishState = 1; // Jump to the talking state
        }
      } else if (currentTime > mouthActionTime + 100) { // If we're beyond the scheduled talking time, halt the motors
        bodyMotor.halt();
        mouthMotor.halt();
        headMotor.halt();
      }
      if (currentTime >= nextFlapTime) { // Time to flap
        fishState = 2; // Jump to the flapping state
      }
      break;

    case 1: // TALKING
      if (currentTime < mouthActionTime) { // If we have a scheduled mouthActionTime in the future
        if (talking) { // And if we think we should be talking
          openMouth(); // Open the mouth
          articulateBody(true); // Move the body
          moveHead(true);       // Move the head
        }
      } else { // Otherwise, close the mouth, don't articulate the body or head, and set talking to false
        closeMouth();
        articulateBody(false);
        moveHead(false);
        talking = false;
        fishState = 0; // Jump back to waiting state
      }
      break;

    case 2: // FLAPPING
      flap();
      nextFlapTime = currentTime + random(30000, 60000); // Schedule next flap in 30 to 60 seconds
      fishState = 0; // Return to waiting state
      break;
  }
}

void updateSoundInput() {
  // Read audio inputs from both left and right channels
  soundVolumeLeft = analogRead(soundPinLeft);
  soundVolumeRight = analogRead(soundPinRight);

  // Combine the audio signals (you can adjust this logic as needed)
  soundVolume = (soundVolumeLeft + soundVolumeRight) / 2;

  // Print audio values to Serial Monitor for debugging
  Serial.print("Left Channel: ");
  Serial.print(soundVolumeLeft);
  Serial.print(" | Right Channel: ");
  Serial.print(soundVolumeRight);
  Serial.print(" | Combined: ");
  Serial.println(soundVolume);
}

void openMouth() {
  mouthMotor.halt();      // Stop the mouth motor
  mouthMotor.setSpeed(220); // Set the mouth motor speed
  mouthMotor.forward();   // Open the mouth
}

void closeMouth() {
  mouthMotor.halt();        // Stop the mouth motor
  mouthMotor.setSpeed(180); // Set the mouth motor speed
  mouthMotor.backward();    // Close the mouth
}

void articulateBody(bool talking) {
  if (talking) { // If Billy is talking
    if (currentTime > bodyActionTime) { // And if we don't have a scheduled body movement
      int r = random(0, 8); // Create a random number between 0 and 7
      if (r < 1) {
        bodySpeed = 0; // Don't move the body
        bodyActionTime = currentTime + random(500, 1000); // Schedule body action
        bodyMotor.forward(); // Move the body motor to raise the head

      } else if (r < 3) {
        bodySpeed = 150; // Move the body slowly
        bodyActionTime = currentTime + random(500, 1000); // Schedule body action
        bodyMotor.forward(); // Move the body motor to raise the head

      } else if (r == 4) {
        bodySpeed = 200; // Move the body at medium speed
        bodyActionTime = currentTime + random(500, 1000); // Schedule body action
        bodyMotor.forward(); // Move the body motor to raise the head

      } else if (r == 5) {
        bodySpeed = 255; // Set the body motor to full speed
        bodyActionTime = currentTime + random(900, 1200); // Schedule body action
        bodyMotor.backward(); // Move the body motor to raise the tail

      } else {
        bodySpeed = 255; // Move the body at full speed
        bodyActionTime = currentTime + random(1500, 3000); // Schedule action time
        bodyMotor.forward(); // Move the body motor to raise the head
      }
    }

    bodyMotor.setSpeed(bodySpeed); // Set the body motor speed
  } else {
    if (currentTime > bodyActionTime) { // If we're beyond the scheduled body action time
      bodyMotor.halt(); // Stop the body motor
      bodyActionTime = currentTime + random(20, 50); // Set the next scheduled body action
    }
  }
}

void moveHead(bool talking) {
  if (talking) { // If Billy is talking
    if (currentTime > headActionTime) { // And if we don't have a scheduled head movement
      int r = random(0, 8); // Create a random number between 0 and 7
      if (r < 1) {
        headSpeed = 0; // Don't move the head
        headActionTime = currentTime + random(500, 1000); // Schedule head action
        headMotor.forward(); // Move the head motor forward

      } else if (r < 3) {
        headSpeed = 150; // Move the head slowly
        headActionTime = currentTime + random(500, 1000); // Schedule head action
        headMotor.forward(); // Move the head motor forward

      } else if (r == 4) {
        headSpeed = 200; // Move the head at medium speed
        headActionTime = currentTime + random(500, 1000); // Schedule head action
        headMotor.forward(); // Move the head motor forward

      } else if (r == 5) {
        headSpeed = 255; // Set the head motor to full speed
        headActionTime = currentTime + random(900, 1200); // Schedule head action
        headMotor.backward(); // Move the head motor backward

      } else {
        headSpeed = 255; // Move the head at full speed
        headActionTime = currentTime + random(1500, 3000); // Schedule action time
        headMotor.forward(); // Move the head motor forward
      }
    }

    headMotor.setSpeed(headSpeed); // Set the head motor speed
  } else {
    if (currentTime > headActionTime) { // If we're beyond the scheduled head action time
      headMotor.halt(); // Stop the head motor
      headActionTime = currentTime + random(20, 50); // Set the next scheduled head action
    }
  }
}

void flap() {
  // Body flap
  bodyMotor.setSpeed(180);  // Set the body motor speed
  bodyMotor.backward();     // Move the body motor to raise the tail
  delay(500);               // Wait for half a second
  bodyMotor.halt();         // Halt the body motor

  // Head movement during flap (optional)
  headMotor.setSpeed(180);  // Set the head motor speed
  headMotor.backward();     // Move the head motor backward
  delay(500);               // Wait for half a second
  headMotor.halt();         // Halt the head motor
}

void checkSerialInput() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim(); // Remove any whitespace

    if (input.equalsIgnoreCase("debug")) {
      debugMode = true;
      Serial.println("Entered Debug Mode.");
      Serial.println("Commands:");
      Serial.println("  test mouth_open");
      Serial.println("  test mouth_close");
      Serial.println("  test head_forward");
      Serial.println("  test head_backward");
      Serial.println("  test body_forward");
      Serial.println("  test body_backward");
      Serial.println("  stop all");
      Serial.println("  exit");
    } else if (input.equalsIgnoreCase("exit")) {
      debugMode = false;
      Serial.println("Exited Debug Mode.");
    } else if (debugMode) {
      handleDebugCommand(input);
    }
  }
}

void debugMenu() {
  // In debug mode, motors are controlled via serial commands
  // The main loop does nothing unless a command is received
}

void handleDebugCommand(String command) {
  if (command.startsWith("test ")) {
    String action = command.substring(5);

    if (action.equalsIgnoreCase("mouth_open")) {
      Serial.println("Testing: Mouth Open");
      mouthMotor.setSpeed(220);
      mouthMotor.forward();
    } else if (action.equalsIgnoreCase("mouth_close")) {
      Serial.println("Testing: Mouth Close");
      mouthMotor.setSpeed(180);
      mouthMotor.backward();
    } else if (action.equalsIgnoreCase("head_forward")) {
      Serial.println("Testing: Head Forward");
      headMotor.setSpeed(200);
      headMotor.forward();
    } else if (action.equalsIgnoreCase("head_backward")) {
      Serial.println("Testing: Head Backward");
      headMotor.setSpeed(200);
      headMotor.backward();
    } else if (action.equalsIgnoreCase("body_forward")) {
      Serial.println("Testing: Body Forward");
      bodyMotor.setSpeed(200);
      bodyMotor.forward();
    } else if (action.equalsIgnoreCase("body_backward")) {
      Serial.println("Testing: Body Backward");
      bodyMotor.setSpeed(200);
      bodyMotor.backward();
    } else {
      Serial.println("Unknown test command.");
    }
  } else if (command.equalsIgnoreCase("stop all")) {
    Serial.println("Stopping all motors.");
    mouthMotor.halt();
    headMotor.halt();
    bodyMotor.halt();
  } else if (command.equalsIgnoreCase("exit")) {
    debugMode = false;
    Serial.println("Exited Debug Mode.");
  } else {
    Serial.println("Unknown command.");
  }
}
