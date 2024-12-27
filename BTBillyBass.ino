#include <MX1508.h>

// Declare motors for each part of the fish
MX1508 bodyMotor(6, 9);    // Body motor on PWM pins 6 and 9
MX1508 mouthMotor(5, 3);   // Mouth motor on PWM pins 5 and 3
MX1508 headMotor(10, 11);  // Head motor on PWM pins 10 and 11

// Sound input pins for left and right channels
const int soundPinLeft = A0;
const int soundPinRight = A1;

// Threshold for "silence". Anything below this level is ignored.
int silenceThreshold = 12; 

// Motor speed variables
int bodySpeed = 0;  
int headSpeed = 0;

// Sound input variables
int soundVolumeLeft = 0;  
int soundVolumeRight = 0; 
int soundVolume = 0;      

// Fish state variable
enum FishState {
  WAITING,
  TALKING,
  FLAPPING
};
FishState fishState = WAITING;

// Timing variables
unsigned long currentTime;
unsigned long mouthActionTime = 0;
unsigned long bodyActionTime = 0;
unsigned long headActionTime = 0;
unsigned long nextFlapTime = 0;

// Debug mode flag
bool debugMode = false; 

void setup() {
  // Initialize motors
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

  // Print welcome message
  Serial.println("Billy Bass Ready"); 
}

void loop() {
  currentTime = millis(); 

  // Check for serial input
  checkSerialInput();

  if (debugMode) {
    debugMenu(); 
  } else {
    updateSoundInput();    
    updateFishState();      
  }
}

void updateFishState() {
  switch (fishState) {
    case WAITING: 
      if (soundVolume > silenceThreshold && currentTime > mouthActionTime) {
        mouthActionTime = currentTime + 100; 
        fishState = TALKING; 
      } else if (currentTime > mouthActionTime + 100) {
        stopMotors(); 
      }

      if (currentTime >= nextFlapTime) { 
        fishState = FLAPPING; 
      }
      break;

    case TALKING: 
      if (currentTime < mouthActionTime) { 
        openMouth(); 
        articulateBody(true); 
        moveHead(true);    
      } else { 
        closeMouth();
        articulateBody(false);
        moveHead(false);
        fishState = WAITING; 
      }
      break;

    case FLAPPING: 
      flap();
      nextFlapTime = currentTime + random(30000, 60000); 
      fishState = WAITING; 
      break;
  }
}

void updateSoundInput() {
  soundVolumeLeft = analogRead(soundPinLeft);
  soundVolumeRight = analogRead(soundPinRight);

  // Combine the audio signals (consider other methods if needed)
  soundVolume = (soundVolumeLeft + soundVolumeRight) / 2; 
}

void openMouth() {
  mouthMotor.halt();     
  mouthMotor.setSpeed(220); 
  mouthMotor.forward();  
}

void closeMouth() {
  mouthMotor.halt();      
  mouthMotor.setSpeed(180);
  mouthMotor.backward();   
}

void articulateBody(bool talking) {
  if (talking && currentTime > bodyActionTime) {
    int r = random(0, 8); 
    switch (r) {
      case 0: 
        bodySpeed = 0;  
        bodyActionTime = currentTime + random(500, 1000); 
        bodyMotor.forward(); 
        break;
      case 1: 
      case 2: 
        bodySpeed = 150; 
        bodyActionTime = currentTime + random(500, 1000); 
        bodyMotor.forward(); 
        break;
      case 3:
        bodySpeed = 200; 
        bodyActionTime = currentTime + random(500, 1000); 
        bodyMotor.forward(); 
        break;
      case 4:
        bodySpeed = 255; 
        bodyActionTime = currentTime + random(900, 1200); 
        bodyMotor.backward(); 
        break;
      default: 
        bodySpeed = 255; 
        bodyActionTime = currentTime + random(1500, 3000); 
        bodyMotor.forward(); 
        break;
    }
    bodyMotor.setSpeed(bodySpeed); 
  } else if (currentTime > bodyActionTime) {
    bodyMotor.halt(); 
    bodyActionTime = currentTime + random(20, 50); 
  }
}

void moveHead(bool talking) {
  // Similar structure to articulateBody() for improved readability
  if (talking && currentTime > headActionTime) {
    int r = random(0, 8); 
    switch (r) {
      case 0: 
        headSpeed = 0;  
        headActionTime = currentTime + random(500, 1000); 
        headMotor.forward(); 
        break;
      case 1: 
      case 2: 
        headSpeed = 150; 
        headActionTime = currentTime + random(500, 1000); 
        headMotor.forward(); 
        break;
      case 3:
        headSpeed = 200; 
        headActionTime = currentTime + random(500, 1000); 
        headMotor.forward(); 
        break;
      case 4:
        headSpeed = 255; 
        headActionTime = currentTime + random(900, 1200); 
        headMotor.backward(); 
        break;
      default: 
        headSpeed = 255; 
        headActionTime = currentTime + random(1500, 3000); 
        headMotor.forward(); 
        break;
    }
    headMotor.setSpeed(headSpeed); 
  } else if (currentTime > headActionTime) {
    headMotor.halt(); 
    headActionTime = currentTime + random(20, 50); 
  }
}

void flap() {
  bodyMotor.setSpeed(180);  
  bodyMotor.backward();    
  delay(500);             
  bodyMotor.halt();        

  // Optional: Add head movement during flap 
  // headMotor.setSpeed(180);  
  // headMotor.backward();    
  // delay(500);             
  // headMotor.halt();        
}

void checkSerialInput() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim(); 

    if (input.equalsIgnoreCase("debug")) {
      enterDebugMode();
    } else if (input.equalsIgnoreCase("exit")) {
      exitDebugMode();
    } else if (debugMode) {
      handleDebugCommand(input);
    }
  }
}

void enterDebugMode() {
  debugMode = true;
  Serial.println("Entered Debug Mode.");
  // ... (print debug commands)
}

void exitDebugMode() {
  debugMode = false;
  stopMotors(); // Ensure motors are stopped when exiting debug mode
  Serial.println("Exited Debug Mode.");
}

void debugMenu() {
  // In debug mode, motors are controlled via serial commands
}

void handleDebugCommand(String command) {
  // ... (handle debug commands as before) 
}

void stopMotors() {
  bodyMotor.halt();
  mouthMotor.halt();
  headMotor.halt();
}
