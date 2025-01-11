#include <MX1508.h>

// --- Constants ---
const int SERIAL_BAUD_RATE = 9600;
const unsigned long COMMAND_TIMEOUT = 200;  // Time to wait for a new serial command (ms)

// --- Pin Definitions ---
// Audio input is not used for direct control in this version
// but we keep the definitions for potential future use.
const int SOUND_PIN_LEFT = A0;
const int SOUND_PIN_RIGHT = A1;

// --- Motor Declarations ---
MX1508 bodyMotor(6, 9);
MX1508 mouthMotor(5, 3);
MX1508 headMotor(10, 11);

// --- Global Variables ---
bool debugMode = false; // Set to true to enable manual motor control via serial
unsigned long lastCommandTime = 0; // Timestamp of the last received command

// --- Function Prototypes ---
void moveMouth(int direction); // 1: open, -1: close, 0: stop
void moveHead(int direction);  // 1: forward, -1: backward, 0: stop
void moveBody(int direction);  // 1: forward, -1: backward, 0: stop
void stopAllMotors();
void checkSerialInput();
void handleDebugCommand(String command);

void setup() {
    // Initialize motor speeds to zero
    bodyMotor.setSpeed(0);
    mouthMotor.setSpeed(0);
    headMotor.setSpeed(0);

    // Start Serial communication
    Serial.begin(SERIAL_BAUD_RATE);

    // Print welcome message
    Serial.println("Billy Bass Motor Control - Ready");
}

void loop() {
    checkSerialInput();

    // Basic motor control based on last command received
    if (!debugMode) {
        // If no command is received within the timeout, stop motors
        if (millis() - lastCommandTime >= COMMAND_TIMEOUT) {
            stopAllMotors();
        }
    }
}

// --- Motor Control Functions ---

void moveMouth(int direction) {
    if (direction == 1) {
        mouthMotor.setSpeed(220);
        mouthMotor.forward();
    } else if (direction == -1) {
        mouthMotor.setSpeed(180);
        mouthMotor.backward();
    } else {
        mouthMotor.halt();
    }
}

void moveHead(int direction) {
    if (direction == 1) {
        headMotor.setSpeed(200);
        headMotor.forward();
    } else if (direction == -1) {
        headMotor.setSpeed(200);
        headMotor.backward();
    } else {
        headMotor.halt();
    }
}

void moveBody(int direction) {
    if (direction == 1) {
        bodyMotor.setSpeed(200);
        bodyMotor.forward();
    } else if (direction == -1) {
        bodyMotor.setSpeed(200);
        bodyMotor.backward();
    } else {
        bodyMotor.halt();
    }
}

void stopAllMotors() {
    mouthMotor.halt();
    headMotor.halt();
    bodyMotor.halt();
}

// --- Serial Input and Debug Mode ---

void checkSerialInput() {
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();

        if (input.equalsIgnoreCase("debug")) {
            debugMode = true;
            Serial.println("Entered Debug Mode.");
            Serial.println("Commands: mouth [open, close, stop], head [forward, backward, stop], body [forward, backward, stop], all stop, exit");
        } else if (input.equalsIgnoreCase("exit") && debugMode) {
            debugMode = false;
            Serial.println("Exited Debug Mode.");
        } else if (debugMode) {
            handleDebugCommand(input);
        } else {
            // Interpret the input as a movement command
            if (input.equalsIgnoreCase("open")) {
                moveMouth(1);
            } else if (input.equalsIgnoreCase("close")) {
                moveMouth(-1);
            }
            lastCommandTime = millis();
        }
    }
}

void handleDebugCommand(String command) {
    if (command.startsWith("mouth")) {
        if (command.indexOf("open") != -1) {
            Serial.println("Mouth Open");
            moveMouth(1);
        } else if (command.indexOf("close") != -1) {
            Serial.println("Mouth Close");
            moveMouth(-1);
        } else if (command.indexOf("stop") != -1) {
            Serial.println("Mouth Stop");
            moveMouth(0);
        }
    } else if (command.startsWith("head")) {
        if (command.indexOf("forward") != -1) {
            Serial.println("Head Forward");
            moveHead(1);
        } else if (command.indexOf("backward") != -1) {
            Serial.println("Head Backward");
            moveHead(-1);
        } else if (command.indexOf("stop") != -1) {
            Serial.println("Head Stop");
            moveHead(0);
        }
    } else if (command.startsWith("body")) {
        if (command.indexOf("forward") != -1) {
            Serial.println("Body Forward");
            moveBody(1);
        } else if (command.indexOf("backward") != -1) {
            Serial.println("Body Backward");
            moveBody(-1);
        } else if (command.indexOf("stop") != -1) {
            Serial.println("Body Stop");
            moveBody(0);
        }
    } else if (command.equalsIgnoreCase("stop all")) {
        Serial.println("Stopping all motors.");
        stopAllMotors();
    } else {
        Serial.println("Unknown command.");
    }
}
