#include <LiquidCrystal.h>
#include <Keypad.h>

// Initialize the LCD library with the interface pins
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

// 4x4 Matrix Keypad setup
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns

// Define the Keymap
char keys[ROWS][COLS] = {
  {'7', '8', '9', '/'},
  {'4', '5', '6', 'X'},
  {'1', '2', '3', '-'},
  {'#', '0', '=', '+'}
};

// Connect keypad ROW0, ROW1, ROW2, and ROW3 to Arduino pins
byte rowPins[ROWS] = {5, 4, 3, 2};

// Connect keypad COL0, COL1, COL2, and COL3 to Arduino pins
byte colPins[COLS] = {6, 7, 8, 9};

// Create the Keypad object
Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String inputVar = ""; // Variable to store the input
int cal = 0;          // Variable to store the number of steps
int mmtoStep = 80;    // Conversion factor for mm to steps

// Stepper Motor Pins
const int stepPin = 10;  // Step signal
const int dirPin = 11;   // Direction signal
const int enPin = 12;    // Enable signal

int stepDelay = 500;     // Step delay in microseconds

// Flag to indicate whether the movement should be stopped
bool isStopped = false;

void setup() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enPin, OUTPUT);

  digitalWrite(enPin, LOW); // Enable the driver

  // Initialize Serial communication
  Serial.begin(9600);

  // Initialize the LCD's number of columns and rows
  lcd.begin(16, 2);
  lcd.print("Input Display");
  delay(2000); // Wait for 2 seconds
  lcd.clear();
}

void loop() {
  char key = kpd.getKey(); // Get the pressed key

  if (key) {
    if (key == '#') { // Stop movement and reset everything
      isStopped = true;
      inputVar = ""; // Clear the input
      cal = 0;       // Reset the step count
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Resetting...");
      delay(1000);  // Pause to show reset
      lcd.clear();
    } else if (key == '=') { // Confirm and display the final value
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("X axis :");
      lcd.setCursor(9, 0);
      lcd.print(inputVar);
      cal = atoi(inputVar.c_str()) * mmtoStep; // Convert input to steps
      lcd.setCursor(0, 1);
      lcd.print("Steps: ");
      lcd.print(cal);
      delay(2000);

      // Stop the motor if it's stopped
      if (!isStopped) {
        digitalWrite(stepPin, LOW); // Ensure motor is not moving
        return;
      }

      // Set the direction based on the sign of cal
      if (cal > 0) {
        digitalWrite(dirPin, LOW); // Clockwise
      } else if (cal < 0) {
        digitalWrite(dirPin, HIGH);
        delay(1000);// Counterclockwise
        cal = -cal;  // Convert negative cal value to positive for loop
      }

      // Execute the steps
      for (int i = 0; i < cal; i++) {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(stepDelay);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(stepDelay);
      }

      // Pause for 5 seconds after movement
      delay(2000);
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Moved");
      lcd.setCursor(1, 1);
      lcd.print(cal);
      lcd.setCursor(6, 1);
      lcd.print(" steps.");
      delay(2000); // Pause for 5 seconds before repeating
      lcd.clear();
    } else { // Append valid keys to the input variable
      inputVar += key;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Xaxis Coordinate:");
      lcd.setCursor(0, 1);
      lcd.print(inputVar);
    }
  }

  // If stopped, don't let the motor move
  if (!isStopped) {
    digitalWrite(stepPin, LOW); // Ensure motor is not moving
  }
} 
