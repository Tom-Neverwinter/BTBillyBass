based on the works of: https://maker.pro/arduino/projects/how-to-animate-billy-bass-with-bluetooth-audio-source


This diagnostic code provides the following features:

Individual motor testing for head (mouth), neck, and tail.
Audio detection testing.
A menu system accessible via the serial monitor.
Continuous audio level monitoring.

Here's how to use the diagnostic code:

Upload the code to your Arduino.
Open the Serial Monitor (make sure it's set to 9600 baud).
You'll see a menu with available commands.
Type a single letter command and press enter to execute:

'h': Test head (mouth) movement
'n': Test neck movement
't': Test tail movement
'a': Run a 5-second audio detection test
's': Stop all motors
'?': Print the menu again



The code will continuously monitor for audio input and print a message when it detects sound above the silence threshold.
This diagnostic setup will allow you to:

Verify that each motor is working correctly and moving in both directions.
Check that the audio detection is functioning and adjust the silence threshold if needed.
Ensure that serial communication is working properly.

Once you've confirmed that everything is working correctly with this diagnostic code, we can move on to implementing the full behavior for Billy Bass. We can incorporate learnings from this diagnostic phase, such as adjusting motor speeds or the silence threshold, into the final code.
