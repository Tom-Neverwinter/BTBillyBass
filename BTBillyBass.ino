/*This is my crack at a state-based approach to automating a Big Mouth Billy Bass.
 This code was built on work done by both Donald Bell and github user jswett77. 
 See links below for more information on their previous work.

 In this code you'll find reference to the MX1508 library, which is a simple 
 library I wrote to interface with the extremely cheap 2-channel H-bridges that
 use the MX1508 driver chip. It may also work with other H-bridges that use different
 chips (such as the L298N), so long as you can PWM the inputs.

 This code watches for a voltage increase on input A0, and when sound rises above a
 set threshold it opens the mouth of the fish. When the voltage falls below the threshold,
 the mouth closes.The result is the appearance of the mouth "riding the wave" of audio
 amplitude, and reacting to each voltage spike by opening again. There is also some code
 which adds body movements for a bit more personality while talking.

 Most of this work was based on the code written by jswett77, and can be found here:
 https://github.com/jswett77/big_mouth/blob/master/billy.ino

 Donald Bell wrote the initial code for getting a Billy Bass to react to audio input,
 and his project can be found on Instructables here:
 https://www.instructables.com/id/Animate-a-Billy-Bass-Mouth-With-Any-Audio-Source/

 Author: Jordan Bunker <jordan@hierotechnics.com> 2019
 License: MIT License (https://opensource.org/licenses/MIT)
*/

#include <MX1508.h>           // Include MX1508 motor driver library
#include <RunningAverage.h>   // Include RunningAverage library for smoother sound detection

// Initialize motors for the body, mouth, and tail with respective control pins
MX1508 bodyMotor(6, 9);       // Body rotation motor
MX1508 mouthMotor(5, 3);      // Mouth motor
MX1508 tailMotor(10, 11);     // New tail motor

// Sound input and control variables
int soundPin = A0;            // Analog pin for sound input
int silence = 12;             // Threshold value to determine "silence"
int bodySpeed = 0;            // Speed of body movement
int soundVolume = 0;          // Measured sound volume
int fishState = 0;            // Current state of the fish animation

bool talking = false;         // Flag to indicate if the fish is talking

// Timing variables to control actions
long currentTime;             // Current time in milliseconds
long mouthActionTime;         // Next time to perform mouth action
long bodyActionTime;          // Next time to perform body action
long tailActionTime;          // Next time to perform tail action
long lastActionTime;          // Last time any action was performed

// Variables for improved vocal detection
RunningAverage vocalsRA(10);  // Running average of the last 10 sound samples
float vocalThreshold = 1.5;   // Threshold multiplier for detecting vocals

void setup() {
  // Initialize motor speeds to 0 (stop)
  bodyMotor.setSpeed(0);
  mouthMotor.setSpeed(0);
  tailMotor.setSpeed(0);  // Initialize the tail motor
  
  pinMode(soundPin, INPUT);  // Set the sound pin as input
  
  Serial.begin(9600);  // Start serial communication for debugging
}

void loop() {
  currentTime = millis();  // Get the current time
  updateSoundInput();      // Update the sound input reading
  SMBillyBass();           // Run the state machine for Billy Bass animation
}

void SMBillyBass() {
  // State machine to control Billy Bass behavior
  switch (fishState) {
    case 0:  // START & WAITING state
      if (detectVocals()) {  // Check if vocals are detected
        if (currentTime > mouthActionTime) {
          talking = true;                   // Set talking flag to true
          mouthActionTime = currentTime + 100; // Set the next mouth action time
          fishState = 1;                    // Switch to TALKING state
        }
      } else if (currentTime > mouthActionTime + 100) {
        haltAllMotors();                   // Stop all motors if no vocals detected
      }
      if (currentTime - lastActionTime > 1500) {  // Random idle animation trigger
        lastActionTime = currentTime + random(30000, 60000);  // Set next random idle time
        fishState = 2;  // Switch to GOTTA FLAP! state
      }
      break;

    case 1:  // TALKING state
      if (currentTime < mouthActionTime) {  // Check if it's time to keep the mouth open
        if (talking) {
          openMouth();               // Open the mouth
          lastActionTime = currentTime;  // Update last action time
          articulateBody(true);      // Move body if talking
          articulateTail(true);      // Move tail if talking
        }
      } else {
        closeMouth();                // Close the mouth
        articulateBody(false);       // Stop body movement
        articulateTail(false);       // Stop tail movement
        talking = false;             // Reset talking flag
        fishState = 0;               // Switch back to START & WAITING state
      }
      break;

    case 2:  // GOTTA FLAP! state
      flap();       // Perform flap action
      fishState = 0;  // Return to START & WAITING state
      break;
  }
}

void updateSoundInput() {
  soundVolume = analogRead(soundPin);  // Read sound volume from sensor
  vocalsRA.addValue(soundVolume);      // Add the reading to running average
}

bool detectVocals() {
  // Detect if the current sound volume indicates vocal input
  float average = vocalsRA.getAverage();   // Get average of recent sound samples
  float currentValue = soundVolume;        // Current sound volume
  return (currentValue > silence && currentValue > average * vocalThreshold);  // Compare with threshold
}

void openMouth() {
  mouthMotor.setSpeed(220);  // Set speed for mouth motor
  mouthMotor.forward();      // Move mouth motor forward (open mouth)
}

void closeMouth() {
  mouthMotor.setSpeed(180);  // Set speed for mouth motor
  mouthMotor.backward();     // Move mouth motor backward (close mouth)
}

void articulateBody(bool talking) {
  // Control body movement based on talking state
  if (talking) {
    if (currentTime > bodyActionTime) {  // Time to update body movement
      int r = random(8);  // Randomize body movement speed and direction
      if (r < 1) {
        bodySpeed = 0;    // Stop body
      } else if (r < 3) {
        bodySpeed = 150;  // Set moderate speed
      } else if (r == 4) {
        bodySpeed = 200;  // Set high speed
      } else if (r == 5) {
        bodySpeed = 255;  // Max speed, backward movement
        bodyMotor.backward();
      } else {
        bodySpeed = 255;  // Max speed, forward movement
      }
      bodyActionTime = currentTime + random(500, 3000);  // Set next body action time
      bodyMotor.setSpeed(bodySpeed);  // Update body motor speed
      bodyMotor.forward();  // Move body motor forward
    }
  } else {
    if (currentTime > bodyActionTime) {  // No talking, halt body
      bodyMotor.halt();
      bodyActionTime = currentTime + random(20, 50);  // Set short idle time
    }
  }
}

void articulateTail(bool talking) {
  // Control tail movement based on talking state
  if (talking) {
    if (currentTime > tailActionTime) {  // Time to update tail movement
      int r = random(5);                // Randomize tail speed
      int tailSpeed = r * 50 + 5;       // Set tail speed between 5 and 255
      tailMotor.setSpeed(tailSpeed);    // Update tail motor speed
      tailMotor.forward();              // Move tail motor forward
      tailActionTime = currentTime + random(300, 1000);  // Set next tail action time
    }
  } else {
    if (currentTime > tailActionTime) {  // No talking, halt tail
      tailMotor.halt();
      tailActionTime = currentTime + random(20, 50);  // Set short idle time
    }
  }
}

void flap() {
  // Function to perform a flapping motion
  bodyMotor.setSpeed(180);  // Set speed for body motor
  bodyMotor.backward();     // Move body backward (flap)
  tailMotor.setSpeed(255);  // Set speed for tail motor
  tailMotor.forward();      // Move tail forward (flap)
  delay(500);               // Short delay for flap effect
  haltAllMotors();          // Stop all motors after flapping
}

void haltAllMotors() {
  // Function to halt all motors
  bodyMotor.halt();
  mouthMotor.halt();
  tailMotor.halt();
}

